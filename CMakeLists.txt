cmake_minimum_required(VERSION 3.25)

project(air VERSION 1.0.0 LANGUAGES C CXX) # x-release-please-version

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AIR_WERROR "Treat warnings as errors" OFF)
option(AIR_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(AIR_CLANG_TIDY "Run clang-tidy on aIR targets while compiling" OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CTest) # defines BUILD_TESTING (ON by default)
include(Sanitizers) # before dependencies, so everything is instrumented together
include(Warnings)
include(Dependencies)

add_subdirectory(src/core)
add_subdirectory(src/backend)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_subdirectory(src/trace) # ptrace/strace: Linux only
endif()
add_subdirectory(src/cli)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
