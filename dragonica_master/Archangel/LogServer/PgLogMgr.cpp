#include "stdafx.h"
#include "BM/LocalMgr.h"
#include "Collins\Log.h"
#include "constant.h"
#include "PgLogMgr.h"
#include "NcLog.h"

PgLogManager::PgLogManager()
{
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_FILE_LOG, BM::OUTPUT_JUST_FILE | BM::OUTPUT_IGNORE_LOG_HEAD, BM::LFC_WHITE, L"./FileLog", L"LogFile.txt", g_kLocal.GetLocale()) );
	WriteMode(EW_Write_All);
	m_pkServiceProviderLog = NULL;
	Ready(false);
}

PgLogManager::~PgLogManager()
{
	SAFE_DELETE(m_pkServiceProviderLog);
}

void PgLogManager::Init()
{
	Ready(true);
	if ( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA)	)
	{
		if ( g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_KOREA_NC_SERVICE) )
		{
			SAFE_DELETE(m_pkServiceProviderLog);
			m_pkServiceProviderLog = new PgNcLog(false);
		}
		else if ( g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DR_DEVELOP) )
		{
			SAFE_DELETE(m_pkServiceProviderLog);
			m_pkServiceProviderLog = new PgNcLog(true);
			Ready( false );
		}
	}
}

bool PgLogManager::Locked_ReadConfigFile()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	std::wstring kPatch = g_kProcessCfg.ConfigDir() + L"Log_Config.ini";

	if(!PathFileExists(kPatch.c_str()) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Log_Config.ini isn't exist"));
		return false;
	}
	std::wstring kMain = L"Log";
	int iWriteMode = GetPrivateProfileInt(kMain.c_str(), _T("WriteMode"), 0, kPatch.c_str());
	if (iWriteMode < 0 || iWriteMode > EW_Max)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("WriteMode value is wrong in Log_Config.ini"));
	}
	else
	{
		WriteMode(static_cast<EWriteMode>(iWriteMode));
	}
	return true;
}


bool PgLogManager::WriteLogToFile(std::wstring const &kLogData) 
{
	if(!kLogData.size())
		return false;

	SDebugLogMessage kMsg(LT_FILE_LOG, BM::LOG_LV5, (BM::vstring() << kLogData).operator std::wstring const&()); 
	g_kLogWorker.PushLog(kMsg);

	return true;
}

void PgLogManager::CheckDailyLogTable() const
{
	//오늘 날짜 테이블 생성 확인한다.
	CEL::DB_QUERY kQuery( DT_LOG, DQT_CHECK_LOG_TABLE, _T("EXEC [dbo].[up_CheckTableByServer]"));
	g_kCoreCenter.PushQuery(kQuery);
}

void PgLogManager::Locked_WriteLog(BM::GUID const &kQueryIndex, PgLogCont const &kLogCont)
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iSize = kLogCont.LogCount();
	if (iSize <= 0)
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("LogCont size is =") << kLogCont.LogCount());
		return;
	}

	//INFO_LOG(BM::LOG_LV5, __FL__ << _T("QueryTarget=") << kQueryIndex << _T(", Type=") << static_cast<int>(kLogCont.LogMainType())
	//	<< _T(", Subtype=") << static_cast<int>(kLogCont.LogSubType()));

	BM::DBTIMESTAMP_EX kNowTime;
	kNowTime.SetLocalTime();
	TCHAR chProcMain[100];
	_stprintf_s(chProcMain, 100, _T("EXEC [dbo].[up_Log_%04d%02d%02d]"), kNowTime.year, kNowTime.month, kNowTime.day);
	TCHAR chProcSub[100];
	_stprintf_s(chProcSub, 100, _T("EXEC [dbo].[up_Log_%04d%02d%02d_Sub]"), kNowTime.year, kNowTime.month, kNowTime.day);

	CEL::DB_QUERY kQueryMain(DT_LOG, DQT_WRITE_LOG, chProcMain);
	kQueryMain.InsertQueryTarget(kQueryIndex);
	kQueryMain.PushStrParam(kNowTime);
	if ( !kLogCont.BuildQuery(kQueryMain) )
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Cannot buildQuery Main"));
		return;
	}
	EWriteMode eModeMain = EW_DB_Write;
	CONT_LOG_WRITE_MODE::const_iterator mode_itor = m_kContWriteMode.find(SLogTypeKey(kLogCont.LogMainType(), kLogCont.LogSubType(), ELOrderMain_None, ELOrderSub_None));
	if (mode_itor != m_kContWriteMode.end())
	{
		eModeMain = mode_itor->second;
	}
	if ((WriteMode() & EW_File_Write) && (eModeMain & EW_File_Write))
	{
		WriteLogToFile(kQueryMain.Command());
	}

	size_t iDBWriteCount = 0;
	for (size_t i=0; i<iSize; i++)
	{
		CEL::DB_QUERY kQuery( DT_LOG, DQT_WRITE_LOG, chProcSub);
		kQuery.InsertQueryTarget(kQueryIndex);
		if ( !kLogCont.BuildQuery(i, kQuery) )
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Cannot buildQuery Index=") << i);
			return;
		}

		PgLog const * const pkLog = kLogCont.GetLog(i);
		EWriteMode eMode = EW_Write_All;
		CONT_LOG_WRITE_MODE::const_iterator mode_itor = m_kContWriteMode.find(SLogTypeKey(kLogCont.LogMainType(), kLogCont.LogSubType(), pkLog->OrderMainType(), pkLog->OrderSubType()));
		if (mode_itor != m_kContWriteMode.end())
		{
			eMode = mode_itor->second;
		}
		if ( Ready() && (WriteMode() & EW_DB_Write) && (eMode & EW_DB_Write))
		{
			//g_kCoreCenter.PushQuery(kQuery);
			kQueryMain.Add(kQuery);
			++iDBWriteCount;
		}
		if ((WriteMode() & EW_File_Write) && (eMode & EW_File_Write))
		{
			WriteLogToFile(kQuery.Command());
		}
	}

	if ( Ready() && (iDBWriteCount>0) && (WriteMode() & EW_DB_Write) && (eModeMain & EW_DB_Write))
	{
		g_kCoreCenter.PushQuery(kQueryMain);
	}

	if (m_pkServiceProviderLog != NULL)
	{
		m_pkServiceProviderLog->Locked_WriteLog(kNowTime, kLogCont);
	}
}

void PgLogManager::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if (m_pkServiceProviderLog != NULL)
	{
		m_pkServiceProviderLog->Locked_Timer5s();
	}

	{
		// 내일 날짜 Log Table/Procedure 미리 생성 하기
		BM::DBTIMESTAMP_EX kNowTime;
		kNowTime.SetLocalTime();
		static BYTE s_byCheckDate = 0;
		if (kNowTime.day != s_byCheckDate)
		{
			CEL::DB_QUERY kQuery(DT_LOG, DQT_CHECK_LOG_TABLE_NEXTDAY, L"EXEC [dbo].[up_CreateDailyLogTable]");
			g_kCoreCenter.PushQuery(kQuery);
			s_byCheckDate = static_cast<BYTE>(kNowTime.day);
		}
	}

}

void PgLogManager::Locked_OnRegist(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if (m_pkServiceProviderLog == NULL)
	{
		return;
	}
	switch (m_pkServiceProviderLog->GetLoggerType())
	{
	case E_Logger_Type_NC:
		{
			PgNcLog* pkNcLog = dynamic_cast<PgNcLog*>(m_pkServiceProviderLog);
			pkNcLog->Locked_OnRegist(rkArg);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("undefined logger type =") << m_pkServiceProviderLog->GetLoggerType());
		}break;
	}
}

void PgLogManager::Locked_OnConnectSP(CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if (m_pkServiceProviderLog == NULL)
	{
		return;
	}

	switch (m_pkServiceProviderLog->GetLoggerType())
	{
	case E_Logger_Type_NC:
		{
			PgNcLog* pkNcLog = dynamic_cast<PgNcLog*>(m_pkServiceProviderLog);
			pkNcLog->Locked_OnConnect(pkSession);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("undefined logger type =") << m_pkServiceProviderLog->GetLoggerType());
		}break;
	}
}

void PgLogManager::Locked_OnDisConnectSP(CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if (m_pkServiceProviderLog == NULL)
	{
		return;
	}
	switch (m_pkServiceProviderLog->GetLoggerType())
	{
	case E_Logger_Type_NC:
		{
			PgNcLog* pkNcLog = dynamic_cast<PgNcLog*>(m_pkServiceProviderLog);
			pkNcLog->Locked_OnDisConnect(pkSession);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("undefined logger type =") << m_pkServiceProviderLog->GetLoggerType());
		}break;
	}
}

void PgLogManager::Locked_OnReceiveSP(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if (m_pkServiceProviderLog == NULL)
	{
		return;
	}
	switch (m_pkServiceProviderLog->GetLoggerType())
	{
	case E_Logger_Type_NC:
		{
			PgNcLog* pkNcLog = dynamic_cast<PgNcLog*>(m_pkServiceProviderLog);
			pkNcLog->Locked_OnReceive(pkSession, pkPacket);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("undefined logger type =") << m_pkServiceProviderLog->GetLoggerType());
		}break;
	}
}

HRESULT PgLogManager::Locked_OnDB_Execute(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	switch(rkResult.QueryType())
	{
	case DQT_DEF_WRITEMODE:
		{
			Q_DQT_DEF_WRITEMODE( rkResult );
		}break;
	case DQT_CHECK_LOG_TABLE:
		{
			Q_DQT_CHECK_LOG_TABLE( rkResult );
		}break;
	case DQT_CHECK_LOG_TABLE_NEXTDAY:
		{
			Q_DQT_CHECK_LOG_TABLE_NEXTDAY( rkResult );
		}break;
	default:
		{
			if (m_pkServiceProviderLog != NULL)
			{
				return m_pkServiceProviderLog->Locked_OnDB_Execute(rkResult);
			}
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType = ") << rkResult.QueryType());
			return E_FAIL;
		}break;
	}
	return S_OK;
}

bool PgLogManager::Q_DQT_DEF_WRITEMODE( CEL::DB_RESULT &rkResult )
{
	CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
	
	while ( itr != rkResult.vecArray.end() )
	{
		CONT_LOG_WRITE_MODE::key_type kKey;
		CONT_LOG_WRITE_MODE::mapped_type eMode;
	
		itr->Pop( kKey.eLogMain );		++itr;
		itr->Pop( kKey.eLogSub );		++itr;
		itr->Pop( kKey.eOrderMain );	++itr;
		itr->Pop( kKey.eOrderSub );		++itr;
		itr->Pop( eMode );				++itr;
		
		auto ret = m_kContWriteMode.insert(std::make_pair(kKey, eMode));
		if( !ret.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV7, __FL__ << _T("LogWrite Mode duplicated Key=") << static_cast<int>(kKey.eLogMain) << _COMMA_ 
				<< static_cast<int>(kKey.eLogSub) << _COMMA_ << static_cast<int>(kKey.eOrderMain) << _COMMA_ << static_cast<int>(kKey.eOrderSub));
		}
	}

	CheckDailyLogTable();
	return true;
}


bool PgLogManager::Q_DQT_CHECK_LOG_TABLE( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << L"Failed Check Daily Log Table!!");
		return false;
	}
	Init();
	return true;
}

bool PgLogManager::Q_DQT_CHECK_LOG_TABLE_NEXTDAY( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet )
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"NEXTDAY_Check_Table, Query = " << rkResult.Command());
		return false;
	}
	return true;
}