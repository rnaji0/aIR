// The `air` binary. First command: `air exec -- <cmd>`
#include <cstddef>
#include <cstdio>
#include <print>
#include <span>
#include <string_view>

#include "backend/process.hpp"
#include "cli/args.hpp"

using air::cli::parse_cmd;
using air::cli::str_input;

int main(int argc, char* argv[]) {
    auto arguments = std::span(argv, static_cast<std::size_t>(argc));
    if (arguments.size() < 3) {
        std::println(stderr, "air: (error) invalid input, specify a mode and command '{}'",
                     str_input(arguments.subspan(1)));
        return 1;
    }
    if (std::string_view(arguments[1]) == "exec") {
        auto parsed_args = parse_cmd(arguments);
        if (!parsed_args) {
            std::println(stderr, "air: (error) {}", parsed_args.error());
            return 1;
        }
        //std::println("{}", str_input(*parsed_args)); use for debug TODO: add debug mode
        auto result = air::backend::run_process(*parsed_args);
        if (!result) {
            std::println(stderr, "air: (error) {}", result.error());
            return 1;
        }

        std::println(stderr, "air: captured {} bytes of stdout:", result->stdout_data.size());
        std::print("{}", result->stdout_data);
        if (result->signaled) {
            std::println(stderr, "air: killed by signal {}", result->code);
        } else {
            std::println(stderr, "air: exited with code {}", result->code);
        }
    } else {
        std::println(stderr, "air: (error) unknown command '{}'", std::string_view(arguments[1]));
        return 1;
    }
    return 0;
}
