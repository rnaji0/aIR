#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "cli/args.hpp"
#include "test_args.hpp"

using air::cli::parse_cmd;
using air::cli::str_input;
using air::test::Args;

namespace {

// The parsed command as plain strings, for easy comparison
std::vector<std::string_view> words(std::span<char*> cmd) {
    return {cmd.begin(), cmd.end()};
}

} // namespace

TEST(ParseCmd, ReturnsCommandAfterDashDash) {
    Args args{"air", "exec", "--", "echo", "hi"};
    auto cmd = parse_cmd(args.span());
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(words(*cmd), (std::vector<std::string_view>{"echo", "hi"}));
}

TEST(ParseCmd, SingleWordCommand) {
    Args args{"air", "exec", "--", "ls"};
    auto cmd = parse_cmd(args.span());
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(words(*cmd), (std::vector<std::string_view>{"ls"}));
}

TEST(ParseCmd, OnlyTheFirstDashDashSplits) {
    Args args{"air", "exec", "--", "ls", "--", "-la"};
    auto cmd = parse_cmd(args.span());
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(words(*cmd), (std::vector<std::string_view>{"ls", "--", "-la"}));
}

TEST(ParseCmd, ReturnsAViewIntoArgvNotACopy) {
    Args args{"air", "exec", "--", "echo", "hi"};
    auto cmd = parse_cmd(args.span());
    ASSERT_TRUE(cmd.has_value());
    EXPECT_EQ(cmd->data(), args.span().data() + 3); // NOLINT: pointer arithmetic is the point
}

TEST(ParseCmd, MissingDashDashIsAnError) {
    Args args{"air", "exec", "echo", "hi"};
    auto cmd = parse_cmd(args.span());
    ASSERT_FALSE(cmd.has_value());
    EXPECT_EQ(cmd.error(), "missing '--' before the command");
}

TEST(ParseCmd, GluedDashDashIsNotASeparator) {
    Args args{"air", "exec", "--echo", "hi"};
    EXPECT_FALSE(parse_cmd(args.span()).has_value());
}

TEST(ParseCmd, NothingAfterDashDashIsAnError) {
    Args args{"air", "exec", "--"};
    auto cmd = parse_cmd(args.span());
    ASSERT_FALSE(cmd.has_value());
    EXPECT_EQ(cmd.error(), "no command after '--'");
}

TEST(ParseCmd, TooFewArgsIsAnErrorNotACrash) {
    Args only_air{"air"};
    EXPECT_FALSE(parse_cmd(only_air.span()).has_value());
    Args none{};
    EXPECT_FALSE(parse_cmd(none.span()).has_value());
}

TEST(StrInput, JoinsWithSingleSpaces) {
    Args args{"echo", "hi", "there"};
    EXPECT_EQ(str_input(args.span()), "echo hi there");
}

TEST(StrInput, SingleWordHasNoSpaces) {
    Args args{"ls"};
    EXPECT_EQ(str_input(args.span()), "ls");
}

TEST(StrInput, EmptyIsEmptyString) {
    Args args{};
    EXPECT_EQ(str_input(args.span()), "");
}
