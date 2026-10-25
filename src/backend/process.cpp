#include "backend/process.hpp"

#include <array>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "backend/fd.hpp"

namespace air::backend {

namespace {
// asnyc safe for err handling
void write_stderr(const char* text) {
    [[maybe_unused]] auto written = write(STDERR_FILENO, text, std::strlen(text));
}

// create pipe. returns {read end, write end}, each owned by Fd
// both close-on-exec: never leaks into cmd
std::expected<std::pair<Fd, Fd>, std::string> make_pipe() {
    std::array<int, 2> fds{};
#ifdef __linux__
    if (pipe2(fds.data(), O_CLOEXEC) == -1) { // atomic: flag set at creation
        return std::unexpected("pipe failed: " + std::generic_category().message(errno));
    }
    return std::pair{Fd(fds[0]), Fd(fds[1])};
#else
    // macOS has no pipe2 so set flag after
    // TODO: if multi-threaded, this introduces race conditions
    if (pipe(fds.data()) == -1) {
        return std::unexpected("pipe failed: " + std::generic_category().message(errno));
    }
    std::pair ends{Fd(fds[0]), Fd(fds[1])};
    for (int fd : fds) {
        if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) { // NOLINT(cppcoreguidelines-pro-type-vararg)
            return std::unexpected("fcntl failed: " + std::generic_category().message(errno));
        }
    }
    return ends;
#endif
}

// blocks until child execs or execs fails
int read_exec_errno(const Fd& err_read) {
    int exec_errno = 0;
    while (true) {
        ssize_t got = read(err_read.get(), &exec_errno, sizeof(exec_errno));
        if (got == -1 && errno == EINTR) {
            continue; // interrupted, retry
        }
        if (got != static_cast<ssize_t>(sizeof(exec_errno))) {
            return 0; // eof or err
        }
        return exec_errno;
    }
}

// reads until EOF. stops early on a real read error
std::string read_all(const Fd& from) {
    std::string captured;
    std::array<char, 4096> buf{};
    while (true) {
        ssize_t n = read(from.get(), buf.data(), buf.size());
        if (n > 0) {
            captured.append(buf.data(), static_cast<std::size_t>(n));
        } else if (n == 0 || errno != EINTR) { // eof or err
            return captured;
        }
    }
}

} // namespace

std::expected<ProcessResult, std::string> run_process(std::span<char*> argv) {
    if (argv.empty()) {
        return std::unexpected("empty command");
    }

    // w/o we can't guarantee null-terminated array
    std::vector<char*> exec_argv(argv.begin(), argv.end());
    exec_argv.push_back(nullptr);

    auto out_pipe = make_pipe();
    if (!out_pipe) {
        return std::unexpected(out_pipe.error());
    }
    auto [out_read, out_write] = std::move(*out_pipe);

    // child -> parent: exec errno on failure, EOF on success (close-on-exec)
    auto err_pipe = make_pipe();
    if (!err_pipe) {
        return std::unexpected(err_pipe.error());
    }
    auto [err_read, err_write] = std::move(*err_pipe);

    pid_t pid = fork();
    if (pid == -1) { // failed to create child
        return std::unexpected("fork failed: " + std::generic_category().message(errno));
    }

    if (pid == 0) {
        //child of air made
        if (dup2(out_write.get(), STDOUT_FILENO) == -1) {
            write_stderr("air: cannot redirect stdout\n");
            _exit(126);
        }

        //(program ie 'echo', arg array)
        execvp(exec_argv[0], exec_argv.data());
        //exec failed
        int err = errno;
        //send actual rzn to parent
        [[maybe_unused]] auto sent = write(err_write.get(), &err, sizeof(err));
        _exit(err == ENOENT ? 127 : 126);
    }
    out_write.reset();
    err_write.reset(); // else our own copy blocks EOF on success

    int exec_errno = read_exec_errno(err_read);
    err_read.reset();

    std::string captured = read_all(out_read);
    out_read.reset(); // done reading; would also close on return

    // parent blocks until child ends, collects how it ended
    int status = 0;
    while (waitpid(pid, &status, 0) == -1) {
        int err = errno;
        if (err != EINTR) { // wait failed
            return std::unexpected("waitpid failed: " + std::generic_category().message(err));
        }
        // wait interrupted
    }

    if (WIFEXITED(status)) {
        return ProcessResult{.signaled = false,
                             .code = WEXITSTATUS(status),
                             .stdout_data = std::move(captured),
                             .exec_errno = exec_errno};
    }
    if (WIFSIGNALED(status)) {
        return ProcessResult{.signaled = true,
                             .code = WTERMSIG(status),
                             .stdout_data = std::move(captured),
                             .exec_errno = exec_errno};
    }
    return std::unexpected("child ended in an unknown state");
}

} // namespace air::backend
