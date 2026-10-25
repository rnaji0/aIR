#pragma once

// Run a command as a local child process: fork, exec, wait.

#include <expected>
#include <span>
#include <string>

namespace air::backend {

struct ProcessResult {
    bool signaled = false; //true if killed by a signal instead of exiting
    int code = 0;//(0-255)
    std::string stdout_data;
};

// Runs `argv` and waits for it to finish. error case means air failed not cmd
std::expected<ProcessResult, std::string> run_process(std::span<char*> argv);

} // namespace air::backend
