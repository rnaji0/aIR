# The `air` binary. First command: `air exec -- <cmd>`.
add_executable(air main.cpp)
target_link_libraries(air PRIVATE air_core air_backend)
if(TARGET air_trace)
  target_link_libraries(air PRIVATE air_trace)
endif()
air_configure_target(air)
