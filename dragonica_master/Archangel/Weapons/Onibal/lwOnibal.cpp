#include "stdafx.h"
#include <assert.h>
#include "lwOnibal.h"
#include "lwInt64.h"
#include "lwLogging.h"
#include "lwBase.h"

using namespace lua_tinker;

#define REGISTER_WRAPPER(x) \
	lw ##x::RegisterWrapper(pkState);

void lwOnibal::RegisterWrapper(lua_State *pkState)
{
	assert(pkState);
	REGISTER_WRAPPER(Base);
	REGISTER_WRAPPER(Int64)
}

static lwOnibal::AddLibAuto* g_kAddLibList = NULL;
static lwOnibal::AddLibAuto* g_kLast = NULL;

bool lwOnibal::InitOnibal()
{
	if(S_OK != g_kLuaTinker.open())
	{
		return false;
	}

	lua_wrapper_user kLua(g_kLuaTinker);
	RegisterWrapper(*kLua);

	AddLibAuto* next = g_kAddLibList;
	while (next != NULL)
	{
		next->m_kCallback(*kLua);
		next = next->m_kNext;
	}
	return true;
}

bool lwOnibal::InitOnibalServerSide()
{
	lua_wrapper_user kLua(g_kLuaTinker);
	lua_State *pkState = *kLua;
	REGISTER_WRAPPER(Logging);
	return true;
}

lwOnibal::AddLibAuto::AddLibAuto(REGISTER_LIB_CALLBACK callback)
{
	if (g_kLast)
		g_kLast->m_kNext = this;
	else
		g_kAddLibList = this;
	g_kLast = this;
	m_kNext = NULL;
	m_kCallback = callback;
}

#undef REGISTER_WRAPPER