#include "stdafx.h"
#include "PgTimer.h"
#include "PgLogMgr.h"

void CALLBACK Timer5s(DWORD dwUserData)
{
	g_kLogMgr.Locked_Timer5s();
}
