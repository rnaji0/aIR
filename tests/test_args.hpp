#pragma once

// Test helper: builds a main()-style argv (std::span<char*>) from string literals

#include <initializer_list>
#include <span>
#include <string>
#include <vector>

namespace air::test {

class Args {
  public:
    Args(std::initializer_list<std::string> words) : words_(words) {
        // Pointers into words_: valid only while this Args is alive and unchanged
        for (std::string& word : words_) {
            ptrs_.push_back(word.data());
        }
    }

    // Copying would copy ptrs_ still pointing into the *original* strings
    Args(const Args&) = delete;
    Args& operator=(const Args&) = delete;
    Args(Args&&) = delete;
    Args& operator=(Args&&) = delete;
    ~Args() = default;

    std::span<char*> span() { return ptrs_; }

  private:
    std::vector<std::string> words_; // owns the text
    std::vector<char*> ptrs_; // views into words_, like argv
};

} // namespace air::test
