#include "backend/process.hpp"

#include <cerrno>
#include <cstring>
#include <system_error>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>

namespace air::backend {

namespace {
// asnyc safe for err handling
void write_stderr(const char* text) {
    [[maybe_unused]] auto written = write(STDERR_FILENO, text, std::strlen(text));
}

} // namespace

std::expected<ProcessResult, std::string> run_process(std::span<char*> argv) {
    if (argv.empty()) {
        return std::unexpected("empty command");
    }

    // w/o we can't guarantee null-terminated array
    std::vector<char*> exec_argv(argv.begin(), argv.end());
    exec_argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid == -1) { //failed to create child
        return std::unexpected("fork failed: " + std::generic_category().message(errno));
    }

    if (pid == 0) {
        //child of air made
        //(program ie 'echo', arg array)
        execvp(exec_argv[0], exec_argv.data());

        //exec failed
        int err = errno;
        bool not_found = (err == ENOENT);
        write_stderr("air: cannot run '");
        write_stderr(exec_argv[0]);
        write_stderr(not_found ? "': command not found\n" : "': cannot execute\n");
        _exit(not_found ? 127 : 126);
    }
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
        return ProcessResult{.signaled = false, .code = WEXITSTATUS(status)};
    }
    if (WIFSIGNALED(status)) {
        return ProcessResult{.signaled = true, .code = WTERMSIG(status)};
    }
    return std::unexpected("child ended in an unknown state");
}

} // namespace air::backend
