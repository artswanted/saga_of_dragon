#pragma once

namespace drem
{
    void assertion_failed(char const* expr, char const* function, char const* file, long line);
    void assertion_failed_fmt(char const* expr, char const* function, char const* file, long line, const char* message, ...);
};

#define DREM_ASSERT(expr) if (!(expr)) drem::assertion_failed(#expr, __FUNCTION__, __FILE__, __LINE__);
#define DREM_ASSERT_FMT(expr, ...) if (!(expr)) drem::assertion_failed_fmt(#expr, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);

