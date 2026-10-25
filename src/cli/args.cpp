#include "cli/args.hpp"

#include <algorithm>
#include <string_view>

namespace air::cli {

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
    if (args.size() < 2) { // subspan(2) below needs at least "air" and a mode
        return std::unexpected("missing mode and command");
    }
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

} // namespace air::cli
