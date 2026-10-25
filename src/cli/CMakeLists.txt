# The `air` binary. First command: `air exec -- <cmd>`

add_library(air_cli_args STATIC args.cpp)
target_include_directories(air_cli_args PUBLIC ${PROJECT_SOURCE_DIR}/src)
air_configure_target(air_cli_args)

add_executable(air main.cpp)
target_link_libraries(air PRIVATE air_cli_args air_core air_backend)
if(TARGET air_trace)
  target_link_libraries(air PRIVATE air_trace)
endif()
air_configure_target(air)
