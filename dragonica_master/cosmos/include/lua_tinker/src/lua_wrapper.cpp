#include "stdafx.h"
#include "lua_tinker.h"
#include "lua_wrapper.h"
#include "AutoLock.h"


lua_wrapper::lua_wrapper()
{
	m_pkState = NULL;
}

lua_wrapper::~lua_wrapper()
{
}

HRESULT lua_wrapper::open()
{
	CAutoMutex kLock(m_kMutex);

	lua_State *pkState = lua_open();
	if(pkState)
	{
		luaopen_base(pkState);
		luaopen_string(pkState);
		luaopen_table(pkState);
		luaopen_math(pkState);
		luaopen_bit(pkState);

		lua_settop(pkState, 0);

		lua_tinker::init(pkState);

		m_pkState = pkState;
		return S_OK;
	}
	return E_FAIL;
}

void lua_wrapper::close()
{
	CAutoMutex kLock(m_kMutex);
	if(m_pkState)
	{
		lua_close(m_pkState);
	}
}

void show_stack(lua_State* L, int n)
{
    lua_Debug ar;
    if(lua_getstack(L, n, &ar) == 1)
	{
		show_stack(L, n+1);
		lua_getinfo(L, "nSlu", &ar);

		lua_pushstring(L, "_ALERT");
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L, -1))
		{
			if(ar.name)
				lua_pushfstring(L, "	%s() : line %d [%s : line %d]\n", ar.name, ar.currentline, ar.source, ar.linedefined);
			else
				lua_pushfstring(L, "	unknown : line %d [%s : line %d]\n", ar.currentline, ar.source, ar.linedefined);
		}
		else
		{
			lua_pop(L, 1);
		}
	}
}

int lua_tinker::_exception(lua_State *L)
{
	lua_pushstring(L, "_ALERT");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(lua_isfunction(L, -1))
	{
		lua_pushfstring(L, "%s\n", lua_tostring(L, -2));
		lua_call(L, 1, 0);
	}
	else
	{
		lua_pop(L, 1);
	}
	show_stack(L, 0);

	return 0;
}