#include <gtest/gtest.h>
#include <unistd.h>

#include "backend/process.hpp"
#include "test_args.hpp"

using air::backend::run_process;
using air::test::Args;

namespace {

// lowest unused fd number. if run_process leaked an fd this would change
int lowest_free_fd() {
    int fd = dup(STDIN_FILENO); // dup picks lowest
    close(fd);
    return fd;
}

} // namespace

TEST(RunProcess, CapturesStdout) {
    Args args{"echo", "hi"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->signaled);
    EXPECT_EQ(result->code, 0);
    EXPECT_EQ(result->stdout_data, "hi\n");
}

TEST(RunProcess, CapturesOutputWithoutTrailingNewline) {
    Args args{"printf", "no newline"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->stdout_data, "no newline");
}

TEST(RunProcess, ReportsExitCode) {
    Args args{"sh", "-c", "exit 3"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value()); // the command failing is not run_process failing
    EXPECT_FALSE(result->signaled);
    EXPECT_EQ(result->code, 3);
}

TEST(RunProcess, ReportsSignal) {
    Args args{"sh", "-c", "kill -9 $$"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->signaled);
    EXPECT_EQ(result->code, 9);
}

TEST(RunProcess, CommandNotFoundExits127) {
    Args args{"air-test-no-such-command"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->signaled);
    EXPECT_EQ(result->code, 127);
}

TEST(RunProcess, NotExecutableExits126) {
    Args args{"/"}; // a directory: found, but can't be executed
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->code, 126);
}

TEST(RunProcess, DoesNotCaptureStderr) {
    Args args{"sh", "-c", "echo to-err >&2"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->stdout_data, "");
}

TEST(RunProcess, LargeOutputDoesNotDeadlock) {
    // 1 MB is ~16x a pipe's buffer: hangs if we waited before reading
    Args args{"head", "-c", "1000000", "/dev/zero"};
    auto result = run_process(args.span());
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->code, 0);
    EXPECT_EQ(result->stdout_data.size(), 1000000U);
}

TEST(RunProcess, EmptyCommandIsAnError) {
    Args args{};
    auto result = run_process(args.span());
    EXPECT_FALSE(result.has_value());
}

TEST(RunProcess, DoesNotLeakFds) {
    int before = lowest_free_fd();
    Args args{"echo", "hi"};
    ASSERT_TRUE(run_process(args.span()).has_value());
    EXPECT_EQ(lowest_free_fd(), before);
}
