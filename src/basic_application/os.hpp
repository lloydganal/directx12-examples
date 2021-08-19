#ifndef LAG_OS_HPP
#define LAG_OS_HPP

#if defined(_WIN32)
#define LAG_TARGET_OS_WINDOWS
#if defined(_WIN64)
#define LAG_TARGET_OS_WINDOWS_64
#else
#define LAG_TARGET_OS_WINDOWS_32
#endif
#include <windows.h>
#endif

#include <stdint.h>

#if defined(_DEBUG) || defined(DEBUG)
#define LAG_DEBUG
#endif

#define RV_SUCCESS 0
#define RV_MUST_EXIT 1
#define RV_ERROR -1

namespace lag {

struct Rectangle {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

struct Resolution {
    uint32_t width;
    uint32_t height;
};

}  // namespace lag

#endif  // LAG_OS_HPP
