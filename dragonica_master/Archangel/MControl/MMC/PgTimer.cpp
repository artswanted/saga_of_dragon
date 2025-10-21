#include "stdafx.h"
#include "PgMMCManager.h"
#include "PgTimer.h"
#include "PgFileCopyMgr.h"

void CALLBACK Timer_Connector(DWORD dwUserData)
{
	g_kMMCMgr.Locked_ConnectImmigration();
	g_kMMCMgr.Locked_CheckSync();
}

void CALLBACK Timer_ServerState(DWORD dwUserData)
{
	g_kMMCMgr.Locked_CheckHeartBeat();
	g_kLogCopyMgr.Locked_Tick();
}
