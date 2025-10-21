#pragma once
#include "lwUI.h"

class lwLockExp
{
public:
	static bool RegisterWrapper(lua_State *pkState);
};

void Req_PT_M_C_ANS_LOCKEXP_TOGGEL(BM::Stream &rkPacket);