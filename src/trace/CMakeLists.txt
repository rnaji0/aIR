# Tracer: strace parser first, then ptrace, then eBPF. Linux only.
# Header-only for now; switch to STATIC (and call air_configure_target) with the first .cpp.
add_library(air_trace INTERFACE)
target_include_directories(air_trace INTERFACE ${PROJECT_SOURCE_DIR}/src)
