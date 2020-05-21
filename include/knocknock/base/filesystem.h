#pragma once

#if (__cplusplus >= 201703L)
#include <filesystem>
namespace fs = std::filesystem;
#else
#warning \
    "This compiler does not support <filesystem>. Turn on C++17 support if available."
#warning "Trying <experimental/filesystem>..."

#if defined(__GNUC__)
#define GCC_VERSION \
    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if (GCC_VERSION >= 50300)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#else
#error "GCC must be >= 5.3.0"
#endif  // (GCC_VERSION >= 50300)
#endif  // defined(__GNUC__)

#if defined(_MSC_VER)
#pragma message( \
    "You're using Visual C++. This is not supported. Caveat emptor.")

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#endif  // defined(_MSC_VER)
#endif  // (__cplusplus >= 201703L)
