#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgMctrl.h"
#include "PgWaitingLobby.h"
#include "ImmTask.h"
#include "PgSiteMgr.h"

#include "Helldart/PgIPChecker.h"
extern PgIPChecker g_kIPChecker;

void UpdatePlayerPlayTime()
{
	if( !g_kDefPlayTime.IsUse() )
	{
		return;
	}

	bool bReset = false;
	switch( g_kDefPlayTime.ResetType() )
	{
	case PgDefPlayerPlayTimeImpl::EDPTRT_MIN:
	case PgDefPlayerPlayTimeImpl::EDPTRT_DAY:
		{
			SYSTEMTIME kSysTime;
			g_kEventView.GetLocalTime( &kSysTime );	//현재 시간 구하기
			BM::PgPackedTime kNowTime(kSysTime);

			if(g_kDefPlayTime.GetNextResetTime() < kNowTime)
			{
				g_kDefPlayTime.UpdateNextResetTime(kNowTime);
				bReset = true;
			}
		}break;
	case PgDefPlayerPlayTimeImpl::EDPTRT_ACCDISCON:
	default:
		{
		}break;
	}

	if( bReset )
	{
		g_kWaitingLobby.ProcessResetPlayTime();	//피로도 리셋

		BM::Stream kPacket(PT_SYNC_DEF_PLAYERPLAYTIME);
		g_kDefPlayTime.WriteToPacket(kPacket);
		g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kPacket);
	}
}

void CALLBACK Timer20s(DWORD dwUserData)
{
	g_kWaitingLobby.CheckTimeOverUser();

	SYSTEMTIME kSystemTime;
	::GetLocalTime(&kSystemTime);

	static SYSTEMTIME kPreSystemTimeDay = kSystemTime;
	static SYSTEMTIME kPreSystemTimeHour = kSystemTime;

	if(kSystemTime.wDay != kPreSystemTimeDay.wDay)
//	if(kSystemTime.wSecond != kPreSystemTimeDay.wSecond)
	{//block ip 는 1일에 한번 갱신.
		kPreSystemTimeDay = kSystemTime;
		bool bIsBlockIP = true;
		PgIPBandFilter::CONT_IP kPrevCont, kNextCont;
		g_kIPChecker.GetCont(bIsBlockIP, kPrevCont);
		{	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_IP_FOR_BLOCK, _T("EXEC [dbo].[up_LoadDefIPForBlock]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
		g_kIPChecker.GetCont(bIsBlockIP, kNextCont);

		if(kPrevCont != kNextCont)
		{
			BM::Stream kPacket(PT_IM_L_RELOAD_ACCEPT_IP);
			g_kIPChecker.WriteToPacket(kPacket, bIsBlockIP);
			::SendToServerType(CEL::ST_LOGIN, kPacket);
		}
	}

	if(kSystemTime.wHour != kPreSystemTimeHour.wHour)
//	if(kSystemTime.wSecond != kPreSystemTimeHour.wSecond)
	{//Accept ip 는 1시간에 한번 갱신.
		kPreSystemTimeHour = kSystemTime;
		bool bIsBlockIP = false;
		PgIPBandFilter::CONT_IP kPrevCont, kNextCont;
		g_kIPChecker.GetCont(bIsBlockIP, kPrevCont);
		{	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_IP_FOR_ACCEPT, _T("EXEC [dbo].[up_LoadDefIPForAccept]"));	g_kCoreCenter.PushQuery(kQuery, true);	}
		g_kIPChecker.GetCont(bIsBlockIP, kNextCont);

		if(kPrevCont != kNextCont)
		{
			BM::Stream kPacket(PT_IM_L_RELOAD_ACCEPT_IP);
			g_kIPChecker.WriteToPacket(kPacket, bIsBlockIP);
			::SendToServerType(CEL::ST_LOGIN, kPacket);
		}
	}

	UpdatePlayerPlayTime();	//피로도 리셋
}

void CALLBACK Timer_10s(DWORD dwUserData)
{
	g_kImmTask.Timer10s(dwUserData);
}
