# IR types, memo keys, hashing, output normalizers.
add_library(air_core STATIC
  version.cpp
)
target_include_directories(air_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(air_core PRIVATE AIR_VERSION="${PROJECT_VERSION}")
target_link_libraries(air_core
  PUBLIC nlohmann_json::nlohmann_json
  PRIVATE BLAKE3::blake3
)
air_configure_target(air_core)
