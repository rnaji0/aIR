#include <array>
#include <utility>

#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>

#include "backend/fd.hpp"

using air::backend::Fd;

namespace {

// True if `fd` is currently open in this process.
bool is_open(int fd) {
    struct stat info {};
    return fstat(fd, &info) == 0; // fails with EBADF once fd is closed
}

// Makes a pipe and returns its two raw fds.
std::array<int, 2> make_pipe() {
    std::array<int, 2> fds{};
    EXPECT_EQ(pipe(fds.data()), 0);
    return fds;
}

} // namespace

TEST(Fd, ClosesWhenItGoesOutOfScope) {
    auto fds = make_pipe();
    {
        Fd read_end(fds[0]);
        Fd write_end(fds[1]);
        EXPECT_TRUE(is_open(fds[0]));
    } // both destructors run here
    EXPECT_FALSE(is_open(fds[0]));
    EXPECT_FALSE(is_open(fds[1]));
}

TEST(Fd, MoveTransfersOwnership) {
    auto fds = make_pipe();
    Fd write_end(fds[1]);
    Fd a(fds[0]);
    Fd b(std::move(a));

    EXPECT_FALSE(a.valid()); // NOLINT(bugprone-use-after-move): checking the moved-from state
    EXPECT_EQ(b.get(), fds[0]);
    EXPECT_TRUE(is_open(fds[0]));

    b.reset();
    EXPECT_FALSE(is_open(fds[0]));
}
