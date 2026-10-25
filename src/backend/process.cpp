#include "backend/process.hpp"

#include <array>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <utility>
#include <vector>

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
std::expected<std::pair<Fd, Fd>, std::string> make_pipe() {
    std::array<int, 2> fds{};
    if (pipe(fds.data()) == -1) {
        return std::unexpected("pipe failed: " + std::generic_category().message(errno));
    }
    return std::pair{Fd(fds[0]), Fd(fds[1])};
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
        // RAII won't help in child for execvp/exit can't
        out_read.reset();
        out_write.reset();

        //(program ie 'echo', arg array)
        execvp(exec_argv[0], exec_argv.data());

        //exec failed
        int err = errno;
        bool not_found = (err == ENOENT);
        write_stderr("air: cannot run '");
        write_stderr(exec_argv[0]); //TODO: find way to not lose exact reason (propogate up)
        write_stderr(not_found ? "': command not found\n" : "': cannot execute\n");
        _exit(not_found ? 127 : 126);
    }
    out_write.reset();

    // read all
    std::string captured;
    std::array<char, 4096> buf{};
    while (true) {
        ssize_t n = read(out_read.get(), buf.data(), buf.size());
        if (n > 0) {
            captured.append(buf.data(), static_cast<std::size_t>(n));
        } else if (n == 0 || errno != EINTR) { // eof or err
            break;
        }
    }
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
        return ProcessResult{
            .signaled = false, .code = WEXITSTATUS(status), .stdout_data = std::move(captured)};
    }
    if (WIFSIGNALED(status)) {
        return ProcessResult{
            .signaled = true, .code = WTERMSIG(status), .stdout_data = std::move(captured)};
    }
    return std::unexpected("child ended in an unknown state");
}

} // namespace air::backend
