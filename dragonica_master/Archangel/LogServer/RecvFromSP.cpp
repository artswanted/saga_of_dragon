#include "stdafx.h"
#include "RecvFromSP.h"
#include "PgLogMgr.h"

void CALLBACK OnConnectToSP( CEL::CSession_Base *pkSession )
{
	g_kLogMgr.Locked_OnConnectSP(pkSession);
}

void CALLBACK OnDisConnectToSP(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG(BM::LOG_LV6, __FL__ << _T(".....ServiceProvider LogServer DISconnected"));
	g_kLogMgr.Locked_OnDisConnectSP(pkSession);
}

void CALLBACK OnReceiveFromSP(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kLogMgr.Locked_OnReceiveSP(pkSession, pkPacket);
}
