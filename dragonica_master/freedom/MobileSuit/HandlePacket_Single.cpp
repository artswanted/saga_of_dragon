#include "stdafx.h"

#ifndef EXTERNAL_RELEASE

#include "HandlePacket_Single.h"
#include "BM/Stream.h"
#include "PgMobileSuit.h"
#include "lwPacket.h"
#include "lua_tinker/lua_tinker.h"
#include "PgTrigger.h"
#include "PgWorld.h"

bool Recv_PT_Single_Mode(BM::Stream *pkPacket)
{
	if (!pkPacket || !g_pkApp || !g_pkApp->IsSingleMode())
	{
		__asm int 3;
		return false;
	}

	WORD packetType = 0;
	pkPacket->Pop(packetType);
	switch (packetType)
	{
		default : { lua_tinker::call<void, int, lwPacket>("SinglePacketHandle", packetType, lwPacket(pkPacket)); }
	}
	return true;
}

#endif