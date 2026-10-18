// The `air` binary. First command: `air exec -- <cmd>`.
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <vector>

int main(int argc, char* argv[]) {
    auto arguments = std::span(argv, static_cast<std::size_t>(argc));

    if (std::string_view(arguments[1]) == "exec") {
        std::println("{}", arguments.subspan(3));
        //handle_exec(arguments.subspan(3));
    }
    return 0;
}
