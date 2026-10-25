#pragma once

// Command-line parsing for the `air` binary

#include <expected>
#include <span>
#include <string>

namespace air::cli {

// Joins args with single spaces: {"echo", "hi"} -> "echo hi". Empty -> ""
std::string str_input(std::span<char*> args);

// returns the command after the first "--"
std::expected<std::span<char*>, std::string> parse_cmd(std::span<char*> args);

} // namespace air::cli
