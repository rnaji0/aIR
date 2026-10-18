# air_configure_target(<target>): warnings and optional clang-tidy for aIR's own targets.
# Dependencies don't get these, so their warnings never break the build.

if(AIR_CLANG_TIDY)
  find_program(AIR_CLANG_TIDY_EXE NAMES clang-tidy clang-tidy-18 REQUIRED)
endif()

function(air_configure_target target)
  target_compile_options(${target} PRIVATE
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
    -Wcast-align -Wnull-dereference -Wdouble-promotion
    -Wformat=2 -Wimplicit-fallthrough
    $<$<CXX_COMPILER_ID:GNU>:-Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast>
    $<$<BOOL:${AIR_WERROR}>:-Werror>
  )
  if(AIR_CLANG_TIDY)
    set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${AIR_CLANG_TIDY_EXE}")
  endif()
endfunction()
