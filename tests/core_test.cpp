#include <gtest/gtest.h>

#include "core/version.hpp"

TEST(Core, VersionIsSet) { EXPECT_FALSE(air::core::version().empty()); }
