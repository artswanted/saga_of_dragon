#pragma once

namespace drem
{
    extern void logfatal(const char* fmt, ...);
    extern void logerr(const char* fmt, ...);
    extern void logwarn(const char* fmt, ...);
    extern void logdebug(const char* fmt, ...);
}
