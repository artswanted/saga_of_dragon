#include "stdafx.h"
#include <drem/sys/log.h>
#include <Lohengrin/LogGroup.h>

void drem::logfatal(const char* fmt, ...)
{
    char buffer[4096];
    {
        va_list arglist;
        va_start( arglist, fmt );
        sprintf_s(buffer, sizeof(buffer), fmt, arglist );
        va_end( arglist );
    }
    CAUTION_LOG(BM::LOG_LV1, UNI(buffer));
}

void drem::logerr(const char* fmt, ...)
{
    char buffer[4096];
    {
        va_list arglist;
        va_start( arglist, fmt );
        sprintf_s(buffer, sizeof(buffer), fmt, arglist );
        va_end( arglist );
    }
    CAUTION_LOG(BM::LOG_LV3, UNI(buffer));
}