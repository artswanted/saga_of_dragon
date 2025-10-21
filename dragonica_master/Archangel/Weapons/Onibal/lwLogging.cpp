#include <stdafx.h>
#include "lwLogging.h"
#include <Lohengrin\LogGroup.h>

void lwLogging::lwMainLog(int level, const char *message)
{
    MAIN_LOG((const BM::E_LOG_LEVEL)(level), message);
}

void lwLogging::lwCoreLog(int level, const char *message)
{
    CORE_LOG((const BM::E_LOG_LEVEL)(level), message);
}

void lwLogging::lwCautionLog(int level, const char *message)
{
    CAUTION_LOG((const BM::E_LOG_LEVEL)(level), message);
}

void lwLogging::lwInfoLog(int level, const char *message)
{
    INFO_LOG((const BM::E_LOG_LEVEL)(level), message);
}

void lwLogging::RegisterWrapper(lua_State *pkState)
{
    #define REGISTER_FUNC(name) \
        def(pkState, #name, &lwLogging :: lw ##name);

    using namespace lua_tinker;
	REGISTER_FUNC(MainLog);
	REGISTER_FUNC(CoreLog);
	REGISTER_FUNC(CautionLog);
	REGISTER_FUNC(InfoLog);

    #undef REGISTER_FUNC
}