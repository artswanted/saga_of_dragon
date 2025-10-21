#include "stdafx.h"
#include "PgTimer.h"
#include "GFMobileLockPacketDef.h"
#include "PgGFServer.h"
#include "GALAServer.h"
#include "NivalServer.h"
#include "Nexon.h"
#include "FPTServer.h"
#include "PgNcServer.h"
#include "PgGravityServer.h"

void CALLBACK Timer_5s_JAPAN(DWORD dwUserData)
{
	g_kJapanDaemon.TryConnect();
	g_kJapanDaemon.Timer5s();
}

void CALLBACK Timer_5s_GF(DWORD dwUserData)
{
	g_kGFServer.Locked_TryConnect();
	g_kGFServer.Locked_Timer5s();
}

void CALLBACK Timer_30s_GF(DWORD dwUserData)
{
	g_kGFServer.Locked_CheckMobileLockWait();
}

void CALLBACK Timer_Ping_GF(DWORD dwUserData)
{
	BM::Stream kPing( PT_AP_GF_REQ_PING );
	g_kGFServer.Locked_SendToServer( PgGFServer::MOBILELOCK_SERVER_NUM, kPing );
}

void CALLBACK Timer_5s_GALA(DWORD dwUserData)
{
	g_kGala.Locked_TryConnect();
	g_kGala.Locked_Timer5s();
}

void CALLBACK Timer_5s_Nival(DWORD dwUserData)
{
	g_kNival.Locked_TryConnect();
	g_kNival.Locked_Timer5s();
}

void CALLBACK Timer_5s_FPT(DWORD dwUserData)
{
	g_kFPT.Locked_TryConnect();
	g_kFPT.Locked_Timer5s();
}

void CALLBACK Timer_5s_NC(DWORD dwUserData)
{
	g_kNc.Locked_TryConnect();
	g_kNc.Locked_Timer5s();
}

void CALLBACK Timer_5s_Gravity(DWORD dwUserData)
{
	g_kGravity.Locked_TryConnect();
	g_kGravity.Locked_Timer5s();
}