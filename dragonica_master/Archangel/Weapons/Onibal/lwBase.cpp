#include <stdafx.h>
#include "lwBase.h"

void lwBase::RegisterWrapper(lua_State *pkState)
{
	#define REGISTER_FUNC(name) \
        def(pkState, #name, &lwBase :: lw ##name);

    using namespace lua_tinker;
	REGISTER_FUNC(DoFile);

    #undef REGISTER_FUNC
}

void lwBase::lwDoFile(char const *pcFileName)
{
	if (pcFileName == NULL)
		return;
	lua_tinker::dofile(*lua_wrapper_user(g_kLuaTinker), pcFileName);
}