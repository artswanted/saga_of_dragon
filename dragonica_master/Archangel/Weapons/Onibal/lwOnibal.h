#pragma once
#include <lua_tinker/lua_tinker.h>

namespace lwOnibal
{
void RegisterWrapper(lua_State *pkState);
bool InitOnibal();
bool InitOnibalServerSide();

typedef void(*REGISTER_LIB_CALLBACK)(lua_State *);
struct AddLibAuto
{
    REGISTER_LIB_CALLBACK m_kCallback;
    AddLibAuto* m_kNext;
    explicit AddLibAuto(REGISTER_LIB_CALLBACK);
};

};