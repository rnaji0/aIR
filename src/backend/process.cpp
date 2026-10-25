#include "backend/process.hpp"

namespace air::backend {

std::expected<ProcessResult, std::string> run_process(std::span<char*> argv) {
    (void)argv;
    return std::unexpected("process placeholder");
}

} // namespace air::backend
