#pragma once

namespace lwLogging
{
    void RegisterWrapper(lua_State *pkState);

    void lwMainLog(int level, const char *message);
    void lwCoreLog(int level, const char *message);
    void lwCautionLog(int level, const char *message);
    void lwInfoLog(int level, const char *message);
}