#include "stdafx.h"
#include "constant.h"
#include "PgLogMgr.h"


HRESULT Q_DQT_WRITE_LOG( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet )
	{//실패로그는 파일로 남겨.
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Log Failed Log=") << rkResult.Command());
		//g_kLog.WriteLogToFile(rkResult.wstrQuery);
		if ( CEL::DR_NO_RESULT != rkResult.eRet )
		{
			// 로그 테이블이 없을 수도 있자나, 테이블을 만들어라~
			g_kLogMgr.CheckDailyLogTable();
		}
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_WRITE_LOG:		
	case DQT_LINKTEST_NCLOGD_WRITE_LOG:
		{
			return Q_DQT_WRITE_LOG( rkResult);
		}break;
	default:
		{
			return g_kLogMgr.Locked_OnDB_Execute(rkResult);
		}break;
	}
	return E_FAIL;
}

HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &rkResult)
{
	return S_OK;
}
