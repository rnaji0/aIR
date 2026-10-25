#include "backend/process.hpp"

#include <cerrno>
#include <print>
#include <system_error>

#include <sys/wait.h>
#include <unistd.h>

namespace air::backend {

std::expected<ProcessResult, std::string> run_process(std::span<char*> argv) {
    (void)argv;

    pid_t pid = fork();
    if (pid == -1) { //failed to create child
        return std::unexpected("fork failed: " + std::generic_category().message(errno));
    }

    if (pid == 0) {
        //child of air made
        // TODO: now pass in cmd/argv to child proc
        std::println(stderr, "child: pid {}, my parent {}", getpid(), getppid());
        _exit(0);
    }
    // parent since fork return pid of child
    std::println(stderr, "parent: pid {}, my child {}", getpid(), pid);
    waitpid(pid, nullptr, 0); //TODO: wait parent until child is terminal
    return std::unexpected("process placeholder");
}

} // namespace air::backend
