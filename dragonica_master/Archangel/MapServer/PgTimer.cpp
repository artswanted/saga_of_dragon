#include "stdafx.h"
#include "Lohengrin/PacketType.h"
#include "BM/PgTask.h"
#include "PgTask_MapServer.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGroundMgr.h"
#include "PgAction.h"
#include "PgHackDualKeyboard.h"

void CALLBACK Timer100ms(DWORD dwUserData)	// 100 ms
{
	g_kGndMgr.OnTimer(ETICK_INTERVAL_100MS);
}

void CALLBACK Timer5s(DWORD dwUserData)	// 5 sec
{
	g_kGndMgr.OnTimer(ETICK_INTERVAL_5S);
	PgAction_EventSystem::DoAction();

	static DWORD s_dwTryConnectToCenter = 0;
	if (BM::TimeCheck(s_dwTryConnectToCenter, 10000))
	{
		g_kProcessCfg.Locked_ConnectSwitch();
	}
}

void CALLBACK Timer30s(DWORD dwUserData)	// 30 sec
{
	g_kGndMgr.OnTimer(ETICK_INTERVAL_30S);


	{
		static Loki::Mutex s_UserCounterLogMutex;
		static DWORD s_dwUserCounterLogTimer = BM::GetTime32();
		{
			// Timer Thread 가 혹시 두개 이상 될까봐 Thread safe...
			BM::CAutoMutex kLock(s_UserCounterLogMutex);
			if ( BM::TimeCheck( s_dwUserCounterLogTimer, 900000 ) )	// 15분마다 한번씩..
			{
				//서버 접속자 정보 로그
				SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
				PgLogCont kLogCont(ELogMain_System_User_Count);
				PgLog kLog;
				kLog.Set(0, kSI.nServerType);
				kLog.Set(1, kSI.nServerNo);
				kLog.Set(2, g_kGndMgr.Locked_GetConnectionMapUserCount());
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
		}
	}

	{
		SEventMessage kMsg(PMET_HACK_CHECK_DUALKEYBOARD, HackDualKeyboard::ACCM_TICK);
		g_kTask.PutMsg(kMsg);
	}
}

void CALLBACK Timer1s(DWORD dwUserData)// 1 frame / sec
{
	g_kGndMgr.OnTimer(ETICK_INTERVAL_1S);
}

void CALLBACK Timer1m(DWORD dwUserData)
{
	g_kGndMgr.OnTimer(ETICK_INTERVAL_1M);
}