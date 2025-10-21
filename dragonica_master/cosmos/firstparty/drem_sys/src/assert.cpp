#include <drem/sys/assert.h>
#include <drem/sys/log.h>
#include <ace/Stack_Trace.h>

#ifdef DREM_DEV
    #pragma message("Drem assert build in debug mode")
#endif

void drem::assertion_failed(char const* expr, char const* function, char const* file, long line)
{
    char message[4096];
    sprintf_s(message, sizeof(message),
        "Assert failed!\n\n"
        "%s\n\n"
        "%s",
        ACE_Stack_Trace::ACE_Stack_Trace(1, 10).c_str(),
        expr
    );

#ifdef DREM_DEV
    MessageBoxA(NULL, message, "Assert failed", MB_OK | MB_ICONERROR);
#endif
    drem::logfatal(message);
}

void drem::assertion_failed_fmt(char const* expr, char const* function, char const* file, long line, const char* fmt, ...)
{
    char message[4096];
    char buffer[512];
    {
        va_list arglist;
        va_start( arglist, fmt );
        sprintf_s(buffer, sizeof(buffer), fmt, arglist );
        va_end( arglist );
    }

    sprintf_s(message, sizeof(message),
        "Assert failed!\n\n"
        "%s\n\n"
        "%s - %s",
        ACE_Stack_Trace::ACE_Stack_Trace(1, 10).c_str(),
        expr,
        buffer
    );

#ifdef DREM_DEV
    MessageBoxA(NULL, message, "Assert failed", MB_OK | MB_ICONERROR);
#endif
    drem::logfatal(message);
}