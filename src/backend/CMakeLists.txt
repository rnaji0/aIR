# Platform backends: Docker first, then Firecracker and gVisor.
# Header-only for now; switch to STATIC (and call air_configure_target) with the first .cpp.
add_library(air_backend INTERFACE)
target_include_directories(air_backend INTERFACE ${PROJECT_SOURCE_DIR}/src)
