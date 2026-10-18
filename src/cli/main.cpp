// The `air` binary. First command: `air exec -- <cmd>`.
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <print>
#include <span>
#include <string_view>

bool validate_input(std::span<char*> args) {
    if (args.size() < 3) {
        std::println(stderr, "usage: air <mode> -- <command> [args...]");
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    auto arguments = std::span(argv, static_cast<std::size_t>(argc));

    if (!validate_input(arguments)) {
        return 1;
    }

    if (std::string_view(arguments[1]) == "exec") {
        auto it = std::ranges::find_if(arguments.subspan(2), [](std::string_view c) {return c == "--";});
        if (it == arguments.end()) {
            std::println(stderr, "usage: air exec -- <command> [args...]");
            return 1;
        }
        std::span cmds_to_process(it + 1, arguments.end());
        if (cmds_to_process.empty()) {
            std::println(stderr, "usage: air exec -- <command> [args...]");
            return 1;
        }
        std::println("{}", cmds_to_process);
    } else {
        std::println(stderr, "air: unknown command '{}'", std::string_view(arguments[1]));
        return 1;
    }
    return 0;
}
