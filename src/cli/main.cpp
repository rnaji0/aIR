// The `air` binary. First command: `air exec -- <cmd>`.
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <expected>
#include <print>
#include <span>
#include <string>
#include <string_view>

std::string str_input(std::span<char*> args) {
    std::string inp;
    if (args.empty()) { // just air
        return inp;
    }
    inp += args[0];
    for (char* elem : args.subspan(1)) {
        inp += " ";
        inp += elem;
    }
    return inp;
}

std::expected<std::span<char*>, std::string> parse_cmd(std::span<char*> args) {
    auto rest = args.subspan(2);
    auto it = std::ranges::find_if(rest, [](std::string_view c) { return c == "--"; });
    if (it == rest.end()) {
        return std::unexpected("missing '--' before the command");
    }
    std::span cmds_to_process(it + 1, rest.end());
    if (cmds_to_process.empty()) {
        return std::unexpected("no command after '--'");
    }
    return cmds_to_process;
}

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
        std::println("{}", str_input(*parsed_args));
    } else {
        std::println(stderr, "air: (error) unknown command '{}'", std::string_view(arguments[1]));
        return 1;
    }
    return 0;
}
