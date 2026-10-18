#pragma once

#include <string_view>

namespace air::core {

// Project version, set by CMake from project(VERSION ...).
[[nodiscard]] std::string_view version() noexcept;

} // namespace air::core
