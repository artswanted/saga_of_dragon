#include "stdafx.h"

#include "PgKeyEvent.h"
#include "Variant/PgDBCache.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromItem.h"
//#include "PgGMProcessMgr.h"
#include "Variant/PgEventview.h"
#include "Lohengrin/PgRealmManager.h"
#include "PgLChannelMgr.h"
//#include "PgIBMgr.h"
#include "Variant/PgEventView.h"
#include "Contents/PgResultMgr.h"
#include "PgTimer.h"
#include "PgPvPLobbyMgr.h"
#include "PgPortalMgr.h"
#include "PgGlobalMarryMgr.h"
#include "PgGlobalPartyMgr.h"

//	1 seconds
void CALLBACK Timer1s(DWORD dwUserData)
{
	//std::cout << __FUNCTION__ << std::endl;
	g_kTotalObjMgr.ReleaseFlush();

	g_kEventView.ProcessEvent();
	DWORD const dwCurTime = BM::GetTime32();
	g_kPortalMgr.OnTick( dwCurTime );
	g_kResultMgr.Tick(dwCurTime);
	g_kResultMgr.ExpeditionTick(dwCurTime);
	g_kMarryMgr.Tick();

	static DWORD s_dwPartyMgrTimer = BM::GetTime32();
	if( BM::TimeCheck( s_dwPartyMgrTimer, 10000 ) )
	{
		g_kPartyMgr.OnTick(dwCurTime);
	}
}

void CALLBACK TimerCheckShutDown(DWORD dwUserData)
{
	g_kServerSetMgr.Locked_CheckShutDown();
}

void CALLBACK Timer30s(DWORD dwUserData)
{
	static Loki::Mutex s_UserCounterLogMutex;
	static DWORD s_dwUserCounterLogTimer = BM::GetTime32();
	{
		// Timer Thread °Ў И¤ЅГ µО°і АМ»у µЙ±оєБ Thread safe...
		BM::CAutoMutex kLock(s_UserCounterLogMutex);
		if ( BM::TimeCheck( s_dwUserCounterLogTimer, 900000 ) )	// 15єРё¶ґЩ ЗС№шѕї..
		{
			//ј­№ц БўјУАЪ Б¤єё ·О±Ч
			SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
			PgLogCont kLogCont(ELogMain_System_User_Count);
			PgLog kLog;
			kLog.Set(0, kSI.nServerType);
			kLog.Set(1, kSI.nServerNo);
			kLog.Set(2, static_cast<int>(g_kServerSetMgr.Locked_CurrentPlayerCount()));
			kLogCont.Add(kLog);
			kLogCont.Commit();

#ifdef _MEMORY_TRACKING
			S_STATE_LOG(BM::LOG_LV0, _T("---Memory Observe---"));
			g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
			if (g_pkMemoryTrack)
			{
				g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE);
			}
#endif
		}
		if( !g_kProcessCfg.IsPublicChannel() )
		{
			BM::Stream DummyPacket(PT_T_T_REQ_CLEAR_JOIN_EXPEDITION_AWAITER);
			SEventMessage DummyMsg;
			DummyMsg.Push(DummyPacket);
			g_kPartyMgr.ProcessMsg(&DummyMsg);
		}
	}
	// ЗцАз Channel БўјУАЪјцё¦ єёіЅґЩ. (Center/Login)
	if ( !g_kProcessCfg.IsPublicChannel() )
	{
		UpdateCurrentUserCount();
	}
}

//	1 minute
void CALLBACK Timer1m(DWORD dwUserData)
{
	//DWORD const dwCurTime = BM::GetTime32();

	//static DWORD dwEventCheckTime = 0;
	//if(BM::TimeCheck(dwEventCheckTime, 30000))
	//{
	//	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_USER_EVENT, _T("EXEC [dbo].[UP_LoadEvent]"));
	//	g_kCoreCenter.PushQuery(kQuery);
	//}
}

void CALLBACK TimerPvP(DWORD dwUserData)
{
	static DWORD dwEventCheckTime = BM::GetTime32();
	if( BM::TimeCheck(dwEventCheckTime, 10000) )
	{
		g_kPvPLobbyMgr.OnTick();
	}
	else
	{
		g_kPvPLobbyMgr.OnTick_Event();
	}

	static DWORD dwLogCheckTime = BM::GetTime32();
	if ( BM::TimeCheck( dwLogCheckTime, 30000 ) )
	{
		g_kPvPLobbyMgr.OnTick_Log();
	}
}


void UpdateCurrentUserCount()
{
	BM::Stream kUPacket(PT_T_A_NFY_USERCOUNT);

	int const iMax = static_cast<int>( g_kSwitchAssignMgr.MaxPlayerCount() );
//	int const iCurrent = static_cast<int>( g_kSwitchAssignMgr.Locked_CurrentPlayerCount() );
	int const iCurrent = static_cast<int>( g_kSwitchAssignMgr.NowPlayerCount() );// Switch Assign MgrїЎј­ єёі»ѕЯ ЗСґЩ.(°шїлГ¤іОАЗ АЇАъјц°Ў ї©±в·О Д«їоЖГАМ µЗѕоѕЯЗФ)

	g_kRealmMgr.UpdateUserCount(g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), iMax, iCurrent);
	kUPacket.Push(g_kProcessCfg.RealmNo());
	kUPacket.Push(g_kProcessCfg.ChannelNo());
	kUPacket.Push(iMax);
	kUPacket.Push(iCurrent);

	SERVER_IDENTITY kSI;
	kSI.nServerType = CEL::ST_LOGIN;
	BM::Stream kLPacket(PT_A_A_WRAPPER, kSI);
	kLPacket.Push(kUPacket);
	::SendToImmigration(kLPacket);

	kSI.nServerType = CEL::ST_CONTENTS;
	BM::Stream kTPacket(PT_A_A_WRAPPER, kSI);
	kTPacket.PosAdjust();
	kTPacket.Push(kUPacket);
	::SendToImmigration(kTPacket);
}
