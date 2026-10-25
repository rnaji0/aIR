#pragma once

// Fd: owns one file descriptor and closes it automatically (RAII).

#include <utility>

#include <unistd.h>

namespace air::backend {

class Fd {
  public:
    Fd() = default; // owns nothing
    explicit Fd(int fd) noexcept : fd_(fd) {}

    // Fd out of scope
    ~Fd() { reset(); }

    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;

    // transfer ownership
    Fd(Fd&& other) noexcept : fd_(std::exchange(other.fd_, -1)) {}
    Fd& operator=(Fd&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
    }

    // raw fd owned by THIS Fd
    [[nodiscard]] int get() const noexcept { return fd_; }
    [[nodiscard]] bool valid() const noexcept { return fd_ >= 0; }

    // close before deconstructor
    void reset() noexcept {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

  private:
    int fd_ = -1; // "owns nothing"
};

} // namespace air::backend
