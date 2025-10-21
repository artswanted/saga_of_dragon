#include "stdafx.h"
#include "Lohengrin/packettype.h"
#include "Onibal/lwOnibal.h"

void net_RegisterWrapper(lua_State *pkState)
{
    using namespace lua_tinker;
    set(pkState, "PT_C_M_REQ_GET_DAILY", PT_C_M_REQ_GET_DAILY);
}
