add_executable(air_tests
  core_test.cpp
)
target_link_libraries(air_tests PRIVATE air_core GTest::gtest_main)
air_configure_target(air_tests)

include(GoogleTest)
gtest_discover_tests(air_tests)
