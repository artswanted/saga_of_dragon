#include "stdafx.h"
#include "PgNetwork.h"
#include "PgServerStateDoc.h"
#include "PgTimer.h"
#include "PgServerCmdMgr.h"

WORD g_wNextRecordMinTime = 0;

void CALLBACK Timer(DWORD dwUserData)
{
	if ( !g_kNetwork.IsConnect() )
	{
		return;
	}

	WORD const wLogTime = g_kGameServerMgr.RecordCCUTimeMin();
	if ( wLogTime )
	{
		SYSTEMTIME kNowTime;
		::GetLocalTime( &kNowTime );

		if ( g_wNextRecordMinTime == kNowTime.wMinute )
		{// 레코딩 시작
			g_kGameServerMgr.WriteCCU( kNowTime );
			g_wNextRecordMinTime += wLogTime;
			if ( g_wNextRecordMinTime > 59 )
			{
				g_wNextRecordMinTime = 0;
			}
		}
		else
		{
			if ( kNowTime.wSecond > 50 )
			{
				bool bRefresh = false;

				if ( 0 == g_wNextRecordMinTime )
				{
					if ( 59 == kNowTime.wMinute )
					{
						bRefresh = true;
					}
				}
				else
				{
					bRefresh = (( kNowTime.wMinute + 1 ) == g_wNextRecordMinTime); 
				}

				if ( true == bRefresh )
				{
					DWORD const dwRefreshTime = BM::GetTime32() - 10000;
					if ( dwRefreshTime > g_kGameServerMgr.GetLastRefreshTime() )
					{
						g_kServerCmdMgr.OnCommand(MCT_REFRESH_STATE);
					}
				}
			}
		}
	}
}
