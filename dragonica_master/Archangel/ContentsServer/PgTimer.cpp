#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgEventview.h"
#include "Variant/PgMctrl.h"
#include "Variant/PgDBCache.h"
#include "Collins/Log.h"
#include "PgKeyEvent.h"
#include "PgDBProcess.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromItem.h"
#include "PgGMProcessMgr.h"
#include "PgServerSetMgr.h"
#include "PgTimer.h"
#include "PgOXQuizEvent.h"
#include "PgTask_Contents.h"
#include "Item/ItemDropControl.h"
#include "PgBSGame.h"
#include "PgEmporiaMgr.h"
#include "variant/PgLogUtil.h"
#include "PgIndunPartyMgr.h"

namespace ItemCountLogHelper
{
	DWORD const dwOneSec = 1000;
	DWORD const dwOneMinSec = 60;
	DWORD dwCountLogMin = 60 * dwOneMinSec * dwOneSec; // 기본 60분 마다 한번씩

	void Init()
	{
		DWORD dwValue = 0;
		if( S_OK == g_kVariableContainer.Get(EVar_Kind_Item, EVar_Item_CountLogMin, dwValue) )
		{
			dwCountLogMin = dwValue * dwOneMinSec * dwOneSec;
		}
	}
	HRESULT Q_DQT_ADMIN_LOAD_DEFLOGITEMCOUNT(CEL::DB_RESULT &rkResult)
	{
		if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
			return false;
		}

		GET_DEF(CItemDefMgr, kItemDefMgr)

		PgLogCont kLogCont(ELogMain_Statistics);
		kLogCont.RealmNo( g_kProcessCfg.ServerIdentity().nRealm );
		PgLog kLog(ELOrderMain_Item, ELOrderSub_Calculate);
		int const iMin = dwCountLogMin / (dwOneMinSec * dwOneSec);

		CEL::DB_DATA_ARRAY::iterator iter = rkResult.vecArray.begin();
		while(iter != rkResult.vecArray.end())
		{
			int iItemNo = 0;
			__int64 iCount = 0;
			__int64 iRowCount = 0;
			(*iter).Pop(iItemNo);		++iter;
			(*iter).Pop(iCount);		++iter;
			(*iter).Pop(iRowCount);		++iter;

			kLog.Set(PgLogUtil::AtIndex(1), iMin);
			kLog.Set(PgLogUtil::AtIndex(2), iItemNo);

			CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo);
			bool const bIsCountAble = (pkItemDef)? pkItemDef->IsAmountItem(): true;
			if( bIsCountAble )
			{
				kLog.Set(PgLogUtil::AtIndex(1), iCount);	// int64 전체 수량
				kLog.Set(PgLogUtil::AtIndex(2), iRowCount);	// int64 인벤 칸 수
			}
			else
			{
				kLog.Set(PgLogUtil::AtIndex(1), iRowCount);	// int64 인벤 칸 수 == 전체 수량
				kLog.Set(PgLogUtil::AtIndex(2), iRowCount);	// int64 인벤 칸 수 == 전체 수량
			}
			kLogCont.Add(kLog);
		}

		kLogCont.Commit();

		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}
};

//	100 millisecond
void CALLBACK Timer100ms(DWORD dwUserData)
{
	g_kIndunPartyMgr.Update();
}

//	1 seconds
void CALLBACK Timer1s(DWORD dwUserData)
{
	g_kTotalObjMgr.ReleaseFlush();

	g_kEventView.ProcessEvent();
	g_kContentsTask.Timer1s();
}

//	10 seconds
void CALLBACK Timer10s(DWORD dwUserData)
{
	g_kContentsTask.Timer10s();
	g_kRealmUserMgr.Locked_OnTick10s();
	g_kRealmUserMgr.Locked_OnTick_SpecificReward_Event();
}

//	1 minute
void CALLBACK Timer1m(DWORD dwUserData)
{
	g_kContentsTask.Timer1m();

	g_kRealmUserMgr.Locked_Tick();
	g_kRealmUserMgr.Locked_ClearPlayerWaiter(ACE_OS::gettimeofday());
	g_kRealmUserMgr.Locked_OnTick_Premium();
}

void CALLBACK Timer5m(DWORD dwUserData)
{
	//g_kRealmUserMgr.Locked_OnTick_SpecificReward_Event();
}

void CALLBACK Timer1m_2(DWORD dwUserData)
{
	//static DWORD dwItemCountLogTime = 0;
	//if( BM::TimeCheck(dwItemCountLogTime, ItemCountLogHelper::dwCountLogMin) )
	//{
	//	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_ADMIN_LOAD_DEFLOGITEMCOUNT, L"dbo.UP_admin_Load_DefLogItemCount" );
	//	g_kCoreCenter.PushQuery(kQuery, false);
	//}

	static UINT64 s_ui64CleanDeleteCharacter = BM::GetTime64();
	if ( (BM::GetTime64() - s_ui64CleanDeleteCharacter) > 600000i64 )
	{
		// 10분마다 한번씩..
		s_ui64CleanDeleteCharacter = BM::GetTime64();

		CleanDeleteCharacter();
	}

	g_kItemDropControl.Timer1m();
}

void CALLBACK Timer30s(DWORD dwUserData)
{
	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_USER_EVENT, _T("EXEC [dbo].[UP_LoadEvent]"));
	g_kCoreCenter.PushQuery(kQuery);

	CEL::DB_QUERY kQueryCouponEvent( DT_MEMBER, DQT_DEF_LOAD_COUPONEVENT, _T("EXEC [dbo].[up_LoadCouponEvent4]"));
	g_kCoreCenter.PushQuery(kQueryCouponEvent);

	CEL::DB_QUERY kQueryMacroTable( DT_LOCAL, DQT_LOAD_MACROCHECKTABLE, _T("EXEC [dbo].[up_LoadMacroCheckTable]"));
	g_kCoreCenter.PushQuery(kQueryMacroTable);

	static Loki::Mutex s_UserCounterLogMutex;
	static DWORD s_dwUserCounterLogTimer = BM::GetTime32();
	{
		DWORD dwCheckTime = 15;// 설정이 안되어 있으면 기본 15분
		g_kVariableContainer.Get( EVar_Kind_DBManager, EVar_DB_LogCCU_CycleMin ,dwCheckTime );
		dwCheckTime = std::max< DWORD >( dwCheckTime * 60000, 60000 );

		// Timer Thread 가 혹시 두개 이상 될까봐 Thread safe...
		BM::CAutoMutex kLock(s_UserCounterLogMutex);
		if ( BM::TimeCheck( s_dwUserCounterLogTimer, dwCheckTime ) )
		{
			//서버 접속자 정보 로그
			PgRealmUserManager::CONT_CHANNEL_USERCOUNT kContUserCount;
			size_t const iTotalCount = g_kRealmUserMgr.Locked_CurrentPlayerCount( kContUserCount );

			BM::vstring vstrMsg(L"T:");//Total
			vstrMsg += iTotalCount;
			vstrMsg += L"/";

			PgRealmUserManager::CONT_CHANNEL_USERCOUNT::const_iterator itr = kContUserCount.begin();
			for ( ; itr != kContUserCount.end() ; ++itr )
			{
				if ( 0 == itr->first )
				{
					vstrMsg += L"A:";//Assign
					vstrMsg += itr->second;
				}
				else
				{
					vstrMsg += L"C";//Channel
					vstrMsg += itr->first;
					vstrMsg += L":";
					vstrMsg += itr->second;
				}

				vstrMsg += L"/";
			}

			SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
			PgLogCont kLogCont(ELogMain_System_User_Count);
			PgLog kLog;
			kLog.Set(0, kSI.nServerType);
			kLog.Set(1, kSI.nServerNo);
			kLog.Set(2, static_cast<int>(iTotalCount) );
			kLog.Set(0, static_cast<std::wstring>(vstrMsg) );
			kLogCont.Add(kLog);
			kLogCont.Commit();
		}
	}
}

void CALLBACK TimerDay(DWORD dwUserData)
{
	static __int64 i64NewDayTime = 0i64;

	__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
	if ( i64NowTime >= i64NewDayTime )
	{
		BM::Stream kSTPacket(PT_N_A_NFY_GAMETIME);
		g_kEventView.WriteToPacket( true, kSTPacket );
		::SendToServerType( CEL::ST_CENTER, kSTPacket );

		// New Day
		SYSTEMTIME kNowDate;
		g_kEventView.GetLocalTime( &kNowDate );

		TCHAR szTime[100] = {0,};
		CGameTime::SystemTime2String( kNowDate, szTime, 100 );
		INFO_LOG( BM::LOG_LV6, __FL__ << _T("=== Start New Day [ ") << szTime << _T(" ] ===") );

		// 현재시간을 구했음.
		// 여기서 NewDay Event Function을 호출하도록 하자.
		// ----------------------------------------------------------
		// ----------------------------------------------------------

		kNowDate.wHour = 0;
		kNowDate.wMinute = 0;
		kNowDate.wSecond = 0;
		kNowDate.wMilliseconds = 0;
		CGameTime::GetNextTime( i64NewDayTime, kNowDate, CGameTime::EVERY_DAY, CGameTime::DEFAULT );
	}

	g_kEmporiaMgr.Locked_OnTick();
}

