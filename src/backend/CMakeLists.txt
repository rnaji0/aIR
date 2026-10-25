# Platform backends
add_library(air_backend STATIC
  process.cpp
)
target_include_directories(air_backend PUBLIC ${PROJECT_SOURCE_DIR}/src)
air_configure_target(air_backend)
