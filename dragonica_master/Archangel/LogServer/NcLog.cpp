#include "stdafx.h"
#include "constant.h"
#include "NcLog.h"
#include "RecvFromSP.h"
#include "LogUtil.h"
#include "NcLogUtil.h"
#include "PgLogMgr.h"
#include "variant/PgStringUtil.h"
#include "Variant/global.h"

namespace NCLogUtil
{
	std::wstring const kComma = L",";

	int ConvertIdx(int const iIndex)
	{
		return iIndex - 1;
	}

	DR2LogUtil::ELogValue_Type GetColumnType(ConvertLog2NC::ENCLogModelType const iLogTypeModel, int const iColumnIdx)
	{
		switch( iLogTypeModel )
		{
		case ConvertLog2NC::NCLMT_NORMAL:	{ return SNcLog::GetColumnType(iColumnIdx); }break;
		case ConvertLog2NC::NCLMT_CHAT:		{ return SNcChatLog::GetColumnType(iColumnIdx); }break;
		default:
			{
			}break;
		}
		return DR2LogUtil::ELogValue_None;
	}

	template< typename _T, size_t _T_ARRAY_SIZE >
	void AttachTo(BM::vstring& rkOut, _T const (&abyArray)[_T_ARRAY_SIZE], bool const bLastComma = true)
	{
		_T const* pBegin = abyArray;
		while( PgArrayUtil::IsInArray(pBegin, abyArray) )
		{
			rkOut << *pBegin;
			if( PgArrayUtil::IsInArray(++pBegin, abyArray) )
			{
				rkOut << NCLogUtil::kComma;
			}
		}
		if( bLastComma )
		{
			rkOut << NCLogUtil::kComma;
		}
	}

	template< typename _T, size_t _T_ARRAY_SIZE >
	void AttachTo(CEL::DB_QUERY& rkOut, _T const (&abyArray)[_T_ARRAY_SIZE])
	{
		_T const* pBegin = abyArray;
		while( PgArrayUtil::IsInArray(pBegin, abyArray) )
		{
			rkOut.PushStrParam( *pBegin );
			++pBegin;
		}
	}

	//
	// local structure function
	template< typename _T_NC_LOG_STRUCT >
	struct SSetNCCoulumn
	{
		SSetNCCoulumn(_T_NC_LOG_STRUCT& rkNcLog)
			: kNcLog(rkNcLog)
		{
		}
		SSetNCCoulumn(SSetNCCoulumn const& rhs)
			: kNcLog(rhs.kNcLog)
		{
		}
		void operator ()(int const iTargetField, int const& iValue)
		{
			kNcLog.SetColumn(iTargetField, iValue);
		}
		void operator ()(int const iTargetField, __int64 const& i64Value)
		{
			kNcLog.SetColumn(iTargetField, i64Value);
		}
		void operator ()(int const iTargetField, std::wstring const& rkString)
		{
			kNcLog.SetColumn(iTargetField, rkString);
		}
	private:
		_T_NC_LOG_STRUCT& kNcLog;
	};

	// local structure function
	struct SMergerCoulumn
	{
		SMergerCoulumn(BM::vstring& rkDest)
			: kDest(rkDest)
		{
		}
		SMergerCoulumn(SMergerCoulumn const& rhs)
			: kDest(rhs.kDest)
		{
		}
		void operator ()(int const iTargetField, int const& iValue)
		{
			AddSeperator();
			kDest << iValue;
		}
		void operator ()(int const iTargetField, __int64 const& i64Value)
		{
			AddSeperator();
			kDest << i64Value;
		}
		void operator ()(int const iTargetField, std::wstring const& rkString)
		{
			AddSeperator();
			kDest << rkString;
		}
	protected:
		inline void AddSeperator()
		{
			if( 0 != kDest.size() )
			{
				kDest << L"/";
			}
		}
	private:
		BM::vstring& kDest;
	};
};

//
tagNcLog::tagNcLog(int const iType)
	: iLogType(iType)
{
	Init();
}
void tagNcLog::Init()
{
	memset(iData, 0, sizeof(iData));
	memset(i64Data, 0, sizeof(i64Data));
	//for (int i=0; i<MAX_STRING_COUNT; ++i)
	//{
	//	wstrData[i] = _T("");
	//}
}
void tagNcLog::SetColumn(int const iIndex, int const& kValue)
{
	NCLogUtil::Set(iData, NCLogUtil::ConvertIdx(iIndex), kValue);
}
void tagNcLog::SetColumn(int const iIndex, __int64 const& kValue)
{
	NCLogUtil::Set(i64Data, NCLogUtil::ConvertIdx(iIndex-MAX_INT_COUNT), kValue);
}
void tagNcLog::SetColumn(int const iIndex, std::wstring const& kValue)
{
	NCLogUtil::Set(wstrData, NCLogUtil::ConvertIdx(iIndex-MAX_INT_COUNT-MAX_I64INT_COUNT), kValue);
}
DR2LogUtil::ELogValue_Type const tagNcLog::GetColumnType(int iColumnIndex)
{
	--iColumnIndex;
	if( iColumnIndex < MAX_INT_COUNT )
	{
		return DR2LogUtil::ELogValue_INT;
	}
	else
	{
		iColumnIndex -= MAX_INT_COUNT;
		if( iColumnIndex < MAX_I64INT_COUNT )
		{
			return DR2LogUtil::ELogValue_I64;
		}
		else
		{
			iColumnIndex -= MAX_I64INT_COUNT;
			if( iColumnIndex < MAX_STRING_COUNT )
			{
				return DR2LogUtil::ELogValue_STRING;
			}
		}
	}
	return DR2LogUtil::ELogValue_None;
}

//
tagNcChatLog::tagNcChatLog(int const iType)
	: iLogType(iType)
{
	Init();
}
void tagNcChatLog::Init()
{
	memset(iData, 0, sizeof(iData));
	memset(i64Data, 0, sizeof(i64Data));
}
void tagNcChatLog::SetColumn(int const iIndex, int const& kValue)
{
	NCLogUtil::Set(iData, NCLogUtil::ConvertIdx(iIndex), kValue);
}
void tagNcChatLog::SetColumn(int const iIndex, std::wstring const& kValue)
{
	NCLogUtil::Set(wstrData, NCLogUtil::ConvertIdx(iIndex-MAX_INT_COUNT), kValue);
}
void tagNcChatLog::SetColumn(int const iIndex, __int64 const& kValue)
{
	NCLogUtil::Set(i64Data, NCLogUtil::ConvertIdx(iIndex-MAX_INT_COUNT-MAX_STRING_COUNT), kValue);
}
DR2LogUtil::ELogValue_Type const tagNcChatLog::GetColumnType(int iColumnIndex)
{
	--iColumnIndex;
	if( iColumnIndex < MAX_INT_COUNT )
	{
		return DR2LogUtil::ELogValue_INT;
	}
	else
	{
		iColumnIndex -= MAX_INT_COUNT;
		if( iColumnIndex < MAX_STRING_COUNT )
		{
			return DR2LogUtil::ELogValue_STRING;
		}
		else
		{
			iColumnIndex -= MAX_STRING_COUNT;
			if( iColumnIndex < MAX_I64INT_COUNT )
			{
				return DR2LogUtil::ELogValue_I64;
			}
		}
	}
	return DR2LogUtil::ELogValue_None;
}


//
namespace ConvertLog2NC
{
	//
	int const iSingleValue = 1;
	int const iAnyLogType = -1;


	//
	tagDRLogKey::tagDRLogKey()
		: iDRLogType(0), iDRSubLogType(0), iDROrderType(0), iDRActionType(0)
	{
	}
	tagDRLogKey::tagDRLogKey(int const& riDRLogType, int const& riDRSubLogType, int const& riDROrderType, int const& riDRActionType)
		: iDRLogType(riDRLogType), iDRSubLogType(riDRSubLogType), iDROrderType(riDROrderType), iDRActionType(riDRActionType)
	{
	}
	tagDRLogKey::~tagDRLogKey()
	{
	}
	bool tagDRLogKey::operator < (tagDRLogKey const& rhs) const
	{
		if( iDRLogType < rhs.iDRLogType )
		{
			return true;
		}
		else if( iDRLogType == rhs.iDRLogType )
		{
			if( iDRSubLogType < rhs.iDRSubLogType )
			{
				return true;
			}
			else if( iDRSubLogType == rhs.iDRSubLogType )
			{
				if( iDROrderType < rhs.iDROrderType )
				{
					return true;
				}
				else if( iDROrderType == rhs.iDROrderType )
				{
					if( iDRActionType < rhs.iDRActionType )
					{
						return true;
					}
				}
			}
		}
		return false;
	}
	bool tagDRLogKey::operator ==(tagDRLogKey const& rhs) const
	{
		return iDRLogType == rhs.iDRLogType
			&& iDRSubLogType == rhs.iDRSubLogType
			&& iDROrderType == rhs.iDROrderType
			&& iDRActionType == rhs.iDRActionType;
	}
	bool tagDRLogKey::IsHaveAny() const // -1
	{
		return	iAnyLogType == iDRLogType
			||	iAnyLogType == iDRSubLogType
			||	iAnyLogType == iDROrderType
			||	iAnyLogType == iDRActionType;
	}
	bool tagDRLogKey::IsEqualAny(tagDRLogKey const& rhs) const
	{
		if( IsHaveAny()
		&&	!rhs.IsHaveAny() )
		{
			bool bRet = ((iAnyLogType == iDRLogType) || (iAnyLogType != iDRLogType && iDRLogType == rhs.iDRLogType));
			bRet = bRet && ((iAnyLogType == iDRSubLogType) || (iAnyLogType != iDRSubLogType && iDRSubLogType == rhs.iDRSubLogType));
			bRet = bRet && ((iAnyLogType == iDROrderType) || (iAnyLogType != iDROrderType && iDROrderType == rhs.iDROrderType));
			bRet = bRet && ((iAnyLogType == iDRActionType) || (iAnyLogType != iDRActionType && iDRActionType == rhs.iDRActionType));
			return bRet;
		}
		return false;
	}
	BM::vstring tagDRLogKey::ToString() const
	{
		return BM::vstring() << iDRLogType << L"," << iDRSubLogType << L"," << iDROrderType << L"," << iDRActionType;
	}


	//
	bool MakeIndex(std::wstring const& rkStr, CONT_DR_LOG_INDEX& rkContOut)
	{
		if( rkStr.empty() )
		{
			return true;
		}

		VEC_WSTRING kVec;
		PgStringUtil::BreakSep(rkStr, kVec, L"/");
		VEC_WSTRING::const_iterator iter = kVec.begin();
		while( kVec.end() != iter )
		{
			VEC_WSTRING::value_type const& rkString = (*iter);
			std::wstring::const_iterator char_iter = rkString.begin();
			while( rkString.end() != char_iter ) // 모두 숫자인가?
			{
				if( 0 == ::iswdigit((*char_iter)) )
				{
					return false;
				}
				++char_iter;
			}

			int const iColumnIdx = PgStringUtil::SafeAtoi((*iter));
			if( 0 <= iColumnIdx )
			{
				rkContOut.push_back(iColumnIdx);
			}
			else
			{
				return false;
			}
			++iter;
		}
		return true;
	}

	template< typename _T_NC_LOG_STRUCT, typename _T_SET_FUNC >
	void ColumnValue(PgLogCont const& rkLogCont, PgLog const& rkLog, int const iSourceColumnIdx, int const iTargetFieldIdx, _T_SET_FUNC rkFunc)
	{
		DR2LogUtil::ELogValue_Type const eSourceType = DR2LogUtil::GetDR2ColumnType(iSourceColumnIdx);
		DR2LogUtil::ELogValue_Type const eTargetType = _T_NC_LOG_STRUCT::GetColumnType(iTargetFieldIdx);
		switch( eSourceType )
		{
		case DR2LogUtil::ELogValue_INT:
			{
				int iValue = 0;
				if( DR2LogUtil::GetColumnValue(rkLogCont, rkLog, iSourceColumnIdx, iValue) )
				{
					switch( eTargetType )
					{
					case DR2LogUtil::ELogValue_INT:
						{
							rkFunc(iTargetFieldIdx, iValue);
						}break;
					case DR2LogUtil::ELogValue_I64:
						{
							__int64 const i64Value = iValue;
							rkFunc(iTargetFieldIdx, i64Value);
						}break;
					case DR2LogUtil::ELogValue_STRING:
						{
							BM::vstring const kTemp(iValue);
							std::wstring const& rkString = kTemp;
							rkFunc(iTargetFieldIdx, rkString);
						}break;
					}
				}
			}break;
		case DR2LogUtil::ELogValue_I64:
			{
				__int64 i64Value = 0;
				if( DR2LogUtil::GetColumnValue(rkLogCont, rkLog, iSourceColumnIdx, i64Value) )
				{
					switch( eTargetType )
					{
					case DR2LogUtil::ELogValue_INT:	{ }break; // do not
					case DR2LogUtil::ELogValue_I64:
						{
							rkFunc(iTargetFieldIdx, i64Value);
						}break;
					case DR2LogUtil::ELogValue_STRING:
						{
							BM::vstring const kTemp(i64Value);
							std::wstring const& rkString = kTemp;
							rkFunc(iTargetFieldIdx, rkString);
						}break;
					}
				}
			}break;
		case DR2LogUtil::ELogValue_STRING:
			{
				std::wstring kString;
				if( DR2LogUtil::GetColumnValue(rkLogCont, rkLog, iSourceColumnIdx, kString) )
				{
					switch( eTargetType )
					{
					case DR2LogUtil::ELogValue_INT:	{ }break; // do not
					case DR2LogUtil::ELogValue_I64:	{ }break; // do not
					case DR2LogUtil::ELogValue_STRING:
						{
							rkFunc(iTargetFieldIdx, kString);
						}break;
					}
				}
			}break;
		default:
			{
			}break;
		}
	}

	//
	CONT_DEF_CONVERT_LOG2NC kDefMap; // Convert Def 기준
	CONT_DEF_CONVERT_LOG2NC kDefAnyMap; // -1 필터들 모음
};


//
PgNcLog::PgNcLog( bool const bInternalTest )
:	PgNullLog(bInternalTest)
{
	LogInit(BM::OUTPUT_JUST_FILE, _T("./NcLog"), _T("NcLog.txt"));
	m_kSessionKey.Clear();

	std::wstring const kConfigFile = g_kProcessCfg.ConfigDir() + _T("Log_Config.ini");

	int const iRealmNo = g_kProcessCfg.ServerIdentity().nRealm;
	wchar_t szLogHeaderName[MAX_PATH] = {0, };
	swprintf_s(szLogHeaderName, L"LogD_%d", iRealmNo);

	TCHAR chIP[100] = {0, };
	WORD wPort = 0;
	int iGameID = 0;
	if (GetPrivateProfileString(szLogHeaderName, _T("IP"), _T(""), chIP, sizeof(chIP), kConfigFile.c_str()) <= 0)
	{
		ASSERT_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot read [LogD].[IP] in ") << kConfigFile);
	}
	if ((wPort = GetPrivateProfileInt(szLogHeaderName, _T("PORT"), 0, kConfigFile.c_str())) <= 0)
	{
		ASSERT_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot read [LogD].[PORT] in ") << kConfigFile);
	}
	if ((iGameID = GetPrivateProfileInt(szLogHeaderName, _T("GAME_ID"), 0, kConfigFile.c_str())) <= 0)
	{
		ASSERT_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot read [LogD].[GAME_ID] in ") << kConfigFile);
	}
	SessionState(E_SPLOG_NONE);
	m_kAddress.Set(chIP, wPort);
	DR2LogUtil::iGameID = iGameID;

	if( m_bInternalTest )
	{
		this->CheckDailyLogTable();
	}

	{
		CEL::DB_QUERY kQuery(DT_LOG, DQT_LOAD_DEF_CONVERT2NC, L"EXEC [dbo].[up_LoadDefConvertLog2Nc]");
		g_kCoreCenter.PushQuery(kQuery);
	}
}

PgNcLog::~PgNcLog()
{
}


void PgNcLog::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);


	if (SessionState() == E_SPLOG_READY)
	{
		if( 0 == Address().wPort )
		{
			return;
		}
		if (!Address().IsCorrect())
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Wrong LogD Address = ") << Address().ToString());
			return;
		}
		// Try connect
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Trying connect to NC_LogD Address=") << Address().ToString());
		SessionState(E_SPLOG_CONNECT_TRY);
		g_kCoreCenter.Connect( Address(), m_kSessionKey );
	}
}

bool PgNcLog::InitConnector()
{
	CEL::INIT_CORE_DESC kNcLogConnector;
	kNcLogConnector.OnSessionOpen	= OnConnectToSP;
	kNcLogConnector.OnDisconnect	= OnDisConnectToSP;
	kNcLogConnector.OnRecv			= OnReceiveFromSP;
	kNcLogConnector.IdentityValue(CONNECTOR_TYPE);
	kNcLogConnector.ServiceHandlerType(CEL::SHT_NC);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kNcLogConnector);
	return true;
}

void PgNcLog::Locked_OnRegist(const CEL::SRegistResult &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kSessionKey.Set(rkArg.guidObj, BM::GUID::NullData());
	SessionState(E_SPLOG_READY);
}

void PgNcLog::Locked_OnConnect(CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);

	if (pkSession->IsAlive())
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << _T(".....NcLogServer connected, ") << pkSession->Addr().ToString().c_str());
		SessionKey(pkSession->SessionKey());
		SessionState(E_SPLOG_CONNECTED);

		// Send Init Packet
		BM::Stream kPacket;
		kPacket.Push(RQ_SERVER_STARTED);
		kPacket.Push(DR2LogUtil::iGameID);
		kPacket.Push(NC_LOGD_SERVER_TYPE);
		Send(kPacket);
	}
	else
	{
		SessionState(E_SPLOG_READY);
	}
}

void PgNcLog::Locked_OnDisConnect(CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kSessionKey.Set(m_kSessionKey.WorkerGuid(), BM::GUID::NullData());
	SessionState(E_SPLOG_READY);
}

void PgNcLog::Locked_OnReceive(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV7, __FL__ << _T("Packet Received Size=") << pkPacket->Size());
}

bool PgNcLog::Send(BM::Stream& rkPacket)
{
	if (rkPacket.Size() > NCLOGD_MAX_PACKET_SIZE)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Packet size too big =") << rkPacket.Size());
		return false;
	}

	return g_kCoreCenter.Send(SessionKey(), rkPacket);
}

void PgNcLog::Locked_WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, PgLogCont const &kLogCont)
{
	//BM::CAutoMutex kLock(m_kMutex);
	size_t const iLogCount = kLogCont.LogCount();

	for( size_t iCur = 0; iLogCount > iCur; ++iCur)
	{
		PgLog const* const pkLog = kLogCont.GetLog(iCur);
		if( NULL == pkLog )
		{
			return;
		}

		ConvertLog2NC::SDRLogKey const kKey(kLogCont.LogMainType(), kLogCont.LogSubType(), pkLog->OrderMainType(), pkLog->OrderSubType());
		{
			using namespace ConvertLog2NC;
			CONT_DEF_CONVERT_LOG2NC::const_iterator find_iter = kDefMap.find(kKey);
			if( kDefMap.end() != find_iter )
			{
				switch( (*find_iter).second.iNCLogModelType )
				{
				case ConvertLog2NC::NCLMT_NORMAL:	{ WriteLog< SNcLog >(kNowTime, kLogCont, pkLog, (*find_iter).second ); }break;
				case ConvertLog2NC::NCLMT_CHAT:		{ WriteLog< SNcChatLog >(kNowTime, kLogCont, pkLog, (*find_iter).second ); }break;
				default:
					{
					}break;
				}
			}
		}
		{
			using namespace ConvertLog2NC;
			CONT_DEF_CONVERT_LOG2NC::const_iterator any_iter = kDefAnyMap.begin();
			while( kDefAnyMap.end() != any_iter )
			{
				if( (*any_iter).first.IsEqualAny(kKey) )
				{
					switch( (*any_iter).second.iNCLogModelType )
					{
					case ConvertLog2NC::NCLMT_NORMAL:	{ WriteLog< SNcLog >(kNowTime, kLogCont, pkLog, (*any_iter).second); }break;
					case ConvertLog2NC::NCLMT_CHAT:		{ WriteLog< SNcChatLog >(kNowTime, kLogCont, pkLog, (*any_iter).second ); }break;
					default:
						{
						}break;
					}
				}
				++any_iter;
			}
		}
	}
}

HRESULT PgNcLog::Locked_OnDB_Execute(CEL::DB_RESULT &rkResult)
{
	switch(rkResult.QueryType())
	{
	case DQT_LINKTEST_NCLOGD_CHECK_LOG_TABLE:
		{
			return Q_DQT_CHECK_LOG_TABLE(rkResult);
		}break;
	case DQT_LOAD_DEF_CONVERT2NC:
		{
			return Q_DQT_LOAD_DEF_CONVERT2NC(rkResult);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown QueryType = ") << rkResult.QueryType());
		}break;
	}
	return E_FAIL;
}

void PgNcLog::CheckDailyLogTable()const
{
	CEL::DB_QUERY kQuery(DT_LOG, DQT_LINKTEST_NCLOGD_CHECK_LOG_TABLE, L"EXEC [dbo].[up_LinkTest_NCLogD_CheckTableByServer]");
	g_kCoreCenter.PushQuery(kQuery);
}

HRESULT PgNcLog::Q_DQT_CHECK_LOG_TABLE(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet )
	{
		ASSERT_LOG(false, BM::LOG_LV0, __FL__ << L"Failed Check Daily Log Table!!");
		return E_FAIL;
	}

	g_kLogMgr.Ready( true );
	return S_OK;
}

HRESULT PgNcLog::Q_DQT_LOAD_DEF_CONVERT2NC(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet )
	{
		ASSERT_LOG(false, BM::LOG_LV5, __FL__ << _T("Query failed, Return=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return E_FAIL;
	}

	typedef std::list< BM::vstring > CONT_ERROR_MSG;
	CONT_ERROR_MSG kErrorMsg;

	ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC kMap, kAnyMap;

	CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != itor )
	{
		ConvertLog2NC::SDRLogKey kKey;
		ConvertLog2NC::SDefConvertLog2NC kValue;
		std::wstring kTemp;

		(*itor).Pop(kValue.iNCLogModelType);	++itor;
		(*itor).Pop(kKey.iDRLogType);			++itor;
		(*itor).Pop(kKey.iDRSubLogType);		++itor;
		(*itor).Pop(kKey.iDROrderType);			++itor;
		(*itor).Pop(kKey.iDRActionType);		++itor;
		(*itor).Pop(kValue.iNcLogID);			++itor;

		int const iMaxSizeColumn = 28;
		int const iDBSizeColumn = iMaxSizeColumn;

		for( int iCur = 0; iMaxSizeColumn > iCur; ++iCur )
		{
			int const iTargetIdx = iCur + 1;

			(*itor).Pop( kTemp );				++itor;

			switch( kValue.iNCLogModelType )
			{
			case ConvertLog2NC::NCLMT_NORMAL:
				{
					// 28 Columns (I 7, I64 6, S 15 )
				}break;
			case ConvertLog2NC::NCLMT_CHAT:
			default:
				{
					// 17 columns (I 9, S 5, I64 3)
					int const iTypeMaxColumn = 17;
					if( iTypeMaxColumn > iCur )
					{
					}
					else
					{
						if( false == kTemp.empty() )
						{
							kErrorMsg.push_back( BM::vstring() << __FL__ << L"wrong field, Dragonica LogType["<<kKey.ToString()<<L"] NC LogID["<<kValue.iNcLogID<<L"] Target Field IDX["<<iTargetIdx<<L", Value="<<kTemp<<L"]" );
							kTemp.clear();
						}
					}
				}break;
			}

			if( !kTemp.empty() )
			{
				ConvertLog2NC::CONT_DR_LOG_INDEX kContField;
				if( ConvertLog2NC::MakeIndex(kTemp, kContField) )
				{
					kValue.kContLogField.insert( std::make_pair(iTargetIdx, kContField) );
				}
				else
				{
					// error
					kErrorMsg.push_back( BM::vstring() << __FL__ << L"wrong field, Dragonica LogType["<<kKey.ToString()<<L"] NC LogID["<<kValue.iNcLogID<<L"] Target Field IDX["<<iTargetIdx<<L", Value="<<kTemp<<L"]" );
				}
			}
		}

		auto kRet = kMap.insert( std::make_pair(kKey, kValue) );
		if( !kRet.second )
		{
			ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC::key_type const& rkKey = (*kRet.first).first;
			ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC::mapped_type const& rkValue = (*kRet.first).second;
			kErrorMsg.push_back( BM::vstring() << __FL__ << L" duplicate Dragonica LogType["<<kKey.ToString()<<L", NC:"<<kValue.iNcLogID<<L"] and ["<<rkKey.ToString()<<L", NC:"<<rkValue.iNcLogID<<L"]" );
		}
	}

	{
		typedef ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC CONT_DEF_CONVERT_LOG2NC;
		typedef ConvertLog2NC::CONT_NC_LOG_FIELD CONT_NC_LOG_FIELD;

		CONT_DEF_CONVERT_LOG2NC::const_iterator iter = kMap.begin();
		while( kMap.end() != iter )
		{
			CONT_DEF_CONVERT_LOG2NC::key_type const& rkKey = (*iter).first;
			CONT_DEF_CONVERT_LOG2NC::mapped_type const& rkValue = (*iter).second;

			BM::vstring kHeaderString = BM::vstring() << L" Dragonica LogType["<<rkKey.ToString()<<L"] NC LogID["<<rkValue.iNcLogID<<L"] ";

			CONT_NC_LOG_FIELD::const_iterator field_iter = rkValue.kContLogField.begin();
			while( rkValue.kContLogField.end() != field_iter )
			{
				CONT_NC_LOG_FIELD::key_type const& iNCFieldIdx = (*field_iter).first;
				CONT_NC_LOG_FIELD::mapped_type const& rkDRFields = (*field_iter).second;
				
				DR2LogUtil::ELogValue_Type const eTargetType = NCLogUtil::GetColumnType(rkValue.iNCLogModelType, iNCFieldIdx);
				if( ConvertLog2NC::iSingleValue > rkDRFields.size() ) // 0 or less
				{
					// error
					kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"empty DRAGONICA Log field IDX, NC LogD IDX["<<iNCFieldIdx<<L"]" );
				}
				else if( ConvertLog2NC::iSingleValue == rkDRFields.size() ) // single value
				{
					int const iDRIndex = rkDRFields.front();
					DR2LogUtil::ELogValue_Type const eSourceType = DR2LogUtil::GetDR2ColumnType(iDRIndex);
					switch( eTargetType )
					{
					case DR2LogUtil::ELogValue_INT:
						{
							switch( eSourceType )
							{
							case DR2LogUtil::ELogValue_INT:
								{
								}break;
							case DR2LogUtil::ELogValue_I64: // error
								{
									kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"wrong destnation NC LogD IDX["<<iNCFieldIdx<<L", Type:INTEGER] from DRAGONICA IDX["<<iDRIndex<<L", Type:INETEGER64]" );
								}break;
							case DR2LogUtil::ELogValue_STRING: // error
								{
									kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"wrong destnation NC LogD IDX["<<iNCFieldIdx<<L", Type:INTEGER] from DRAGONICA IDX["<<iDRIndex<<L", Type:STRING]" );
								}break;
							}
						}break;
					case DR2LogUtil::ELogValue_I64:
						{
							switch( eSourceType )
							{
							case DR2LogUtil::ELogValue_INT:
							case DR2LogUtil::ELogValue_I64:
								{
								}break;
							case DR2LogUtil::ELogValue_STRING: // error
								{
									kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"wrong destnation NC LogD IDX["<<iNCFieldIdx<<L", Type:INTEGER64] from DRAGONICA IDX["<<iDRIndex<<L", Type:STRING]" );
								}break;
							}
						}break;
					case DR2LogUtil::ELogValue_STRING:
						{
							// anyway ok
						}break;
					}
				}
				else // multiple values
				{
					switch( eTargetType )
					{
					case DR2LogUtil::ELogValue_INT: // error
						{
							kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"wrong destnation NC LogD IDX["<<iNCFieldIdx<<L", Type:INTEGER]" );
						}break;
					case DR2LogUtil::ELogValue_I64: // error
						{
							kErrorMsg.push_back( BM::vstring() << __FL__ << kHeaderString << L"wrong destnation NC LogD IDX["<<iNCFieldIdx<<L", Type:INTEGER64]" );
						}break;
					case DR2LogUtil::ELogValue_STRING:
						{
							// anyway ok
						}break;
					}
				}

				++field_iter;
			}
			
			++iter;
		}

		if( kMap.empty() )
		{
			kErrorMsg.push_back( BM::vstring() << __FL__ << L" empty log convert table [TB_DefConvertLog2NC]" );
		}

		CONT_ERROR_MSG::const_iterator log_iter = kErrorMsg.begin();
		while( kErrorMsg.end() != log_iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*log_iter));
			++log_iter;
		}
	}
	{
		ConvertLog2NC::CONT_DEF_CONVERT_LOG2NC::iterator any_iter = kMap.begin();
		while( kMap.end() != any_iter )
		{
			if( (*any_iter).first.IsHaveAny() )
			{
				kAnyMap.insert( std::make_pair((*any_iter).first, (*any_iter).second) );
				any_iter = kMap.erase(any_iter);
			}
			else
			{
				++any_iter;
			}
		}
	}
	if( !kErrorMsg.empty() )
	{
		ASSERT_LOG(false, BM::LOG_LV5, __FL__ << L"NC Log linkage error" );
		return E_FAIL;
	}

	ConvertLog2NC::kDefMap.swap( kMap );
	ConvertLog2NC::kDefAnyMap.swap( kAnyMap );

	if( !m_bInternalTest )
	{
		InitConnector();
	}
	return S_OK;
}

void PgNcLog::TestLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcLog const &rkLog)
{
	CEL::DB_QUERY kQuery(DT_LOG, DQT_LINKTEST_NCLOGD_WRITE_LOG, L"EXEC [dbo].[UP_LinkTest_NCLogD_Log_Write]");
	kQuery.PushStrParam( kNowTime );				// Time
	kQuery.PushStrParam( rkLog.iLogType );			// Log_ID
	NCLogUtil::AttachTo(kQuery, rkLog.iData);		// int type
	NCLogUtil::AttachTo(kQuery, rkLog.i64Data);		// __int64 type
	NCLogUtil::AttachTo(kQuery, rkLog.wstrData);	// string type
	g_kCoreCenter.PushQuery(kQuery);
}
void PgNcLog::TestLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcChatLog const &rkLog)
{
	CEL::DB_QUERY kQuery(DT_LOG, DQT_LINKTEST_NCLOGD_WRITE_LOG, L"EXEC [dbo].[UP_LinkTest_NCLogD_ChatLog_Write]");
	kQuery.PushStrParam( kNowTime );				// Time
	kQuery.PushStrParam( rkLog.iLogType );			// Log_ID
	NCLogUtil::AttachTo(kQuery, rkLog.iData);		// int type
	NCLogUtil::AttachTo(kQuery, rkLog.wstrData);	// string type
	NCLogUtil::AttachTo(kQuery, rkLog.i64Data);		// __int64 type
	g_kCoreCenter.PushQuery(kQuery);
}

HRESULT PgNcLog::WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcChatLog const &rkLog)
{
	BM::Stream kLogPacket;
	BM::vstring vLogString;

	kLogPacket.Push(RQ_LOG_SEND_MSG);
	// SERVER_TYPE
	kLogPacket.Push(NC_LOGD_SERVER_TYPE);
	// Log Group
	kLogPacket.Push(E_NC_LOG_GROUP_CHAT);

	// Time
	wchar_t chTime[100];
	_stprintf_s(chTime, 100, _T("%04d-%02d-%02d %02d:%02d:%02d"), kNowTime.year, kNowTime.month, kNowTime.day, kNowTime.hour, kNowTime.minute, kNowTime.second);
	vLogString << chTime << NCLogUtil::kComma;

	// Log_ID
	vLogString << rkLog.iLogType << NCLogUtil::kComma;
	// int type
	NCLogUtil::AttachTo(vLogString, rkLog.iData);
	// string type
	NCLogUtil::AttachTo(vLogString, rkLog.wstrData);
	// __int64 type
	NCLogUtil::AttachTo(vLogString, rkLog.i64Data, false);

	SDebugLogMessage kMsg(LT_FILE_LOG, BM::LOG_LV6, (BM::vstring() << vLogString).operator std::wstring const&()); 
	g_kLogWorker.PushLog(kMsg);

	vLogString << L"\r\n";

	std::wstring const & rkOutputString = vLogString;
	size_t iOutputStringSize = rkOutputString.length() * sizeof(std::wstring::value_type);
	kLogPacket.Push(rkOutputString.c_str(), iOutputStringSize);
	{
		BM::CAutoMutex kLock(m_kMutex);
		Send(kLogPacket);
	}
	return S_OK;
}
HRESULT PgNcLog::WriteLog(BM::DBTIMESTAMP_EX const &kNowTime, SNcLog const &rkLog)
{
	BM::Stream kLogPacket;
	BM::vstring vLogString;

	kLogPacket.Push(RQ_LOG_SEND_MSG);
	// SERVER_TYPE
	kLogPacket.Push(NC_LOGD_SERVER_TYPE);
	// Log Group
	kLogPacket.Push(E_NC_LOG_GROUP_GAMEDATA);

	// Time
	wchar_t chTime[100];
	_stprintf_s(chTime, 100, _T("%04d-%02d-%02d %02d:%02d:%02d"), kNowTime.year, kNowTime.month, kNowTime.day, kNowTime.hour, kNowTime.minute, kNowTime.second);
	vLogString << chTime << NCLogUtil::kComma;

	// Log_ID
	vLogString << rkLog.iLogType << NCLogUtil::kComma;
	// int type
	NCLogUtil::AttachTo(vLogString, rkLog.iData);
	// __int64 type
	NCLogUtil::AttachTo(vLogString, rkLog.i64Data);
	// string type
	NCLogUtil::AttachTo(vLogString, rkLog.wstrData, false);

	SDebugLogMessage kMsg(LT_FILE_LOG, BM::LOG_LV6, (BM::vstring() << vLogString).operator std::wstring const&()); 
	g_kLogWorker.PushLog(kMsg);

	vLogString << L"\r\n";

	std::wstring const & rkOutputString = vLogString;
	size_t iOutputStringSize = rkOutputString.length() * sizeof(std::wstring::value_type);
	kLogPacket.Push(rkOutputString.c_str(), iOutputStringSize);
	{
		BM::CAutoMutex kLock(m_kMutex);
		Send(kLogPacket);
	}
	return S_OK;
}