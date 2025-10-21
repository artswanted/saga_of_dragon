#include "stdafx.h"
#include "ItemDropControl.h"

////////////////////////////////////////////////////////////////////////////
//	PgItemDropControlImpl

PgItemDropControlImpl::PgItemDropControlImpl()
{
	QueryCount(0);
	MgrGuid(BM::GUID::Create());
}

PgItemDropControlImpl::~PgItemDropControlImpl()
{
}

HRESULT PgItemDropControlImpl::OnDBProcess(CEL::DB_RESULT &rkResult)
{
	HRESULT hRet = E_FAIL;
	switch(rkResult.QueryType())
	{
	case DQT_LOAD_LOCAL_LIMITED_ITEM:
		{
			hRet = Q_DQT_LOAD_LOCAL_LIMITED_ITEM(rkResult);
		}break;
	case DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL:
		{
			hRet = Q_DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL(rkResult);
		}break;
	case DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD:
		{
			hRet = Q_DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD(rkResult);
		}break;
	case DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD:
		{
			hRet = Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD(rkResult);
		}break;
	case DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP:
		{
			hRet = Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP(rkResult);
		}break;
	case DQT_SYNC_LOCAL_LIMITED_ITEM:
		{
			hRet = Q_DQT_SYNC_LOCAL_LIMITED_ITEM(rkResult);
		}break;
	case DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD:
		{
			hRet = Q_DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD(rkResult);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Unknown DB QueryType=") << rkResult.QueryType());
		}break;
	}
	return hRet;
}

HRESULT PgItemDropControlImpl::Q_DQT_LOAD_LOCAL_LIMITED_ITEM(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet 
	&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return E_FAIL;
	}

	CONT_LIMITED_ITEM kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(kIter != rkResult.vecArray.end())
	{
		CONT_LIMITED_ITEM::key_type		kKey;
		CONT_LIMITED_ITEM::mapped_type	kValue;

		kIter->Pop(kKey);					++kIter;
		kValue.iEventNo = kKey;
			
		kIter->Pop(kValue.iBagNo);			++kIter;
		kIter->Pop(kValue.kStartDate);		++kIter;
		kIter->Pop(kValue.kEndDate);		++kIter;
		kIter->Pop(kValue.iLimit_ResetPeriod);			++kIter;
		kIter->Pop(kValue.iSafe_ResetPeriod);			++kIter;
		kIter->Pop(kValue.iRefreshCount);		++kIter;

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SYNC_LOCAL_LIMITED_ITEM, _T("EXEC [dbo].[UP_LimitedItem_SyncToRecord]"));
		kQuery.InsertQueryTarget(MgrGuid());
		kQuery.PushStrParam(kValue.iEventNo);
		kQuery.PushStrParam(kValue.kStartDate);
		kQuery.PushStrParam(kValue.kEndDate);
		kQuery.PushStrParam(kValue.iLimit_ResetPeriod);
		kQuery.PushStrParam(kValue.iSafe_ResetPeriod);
		kQuery.PushStrParam(kValue.iRefreshCount);
		if( S_OK != g_kCoreCenter.PushQuery(kQuery) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot PushQuery Query=") << kQuery.Command());
		}

		kCont.insert(std::make_pair(kKey,kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return S_OK;
}

HRESULT PgItemDropControlImpl::Q_DQT_LOAD_LOCAL_LIMITED_ITEM_CONTROL(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet 
	&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return E_FAIL;
	}

	CONT_LIMITED_ITEM_CONTROL kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(kIter != rkResult.vecArray.end())
	{
		CONT_LIMITED_ITEM_CONTROL::key_type		kKey;
		CONT_LIMITED_ITEM_CONTROL::mapped_type::value_type	kValue;

		kIter->Pop(kKey.iObjectType);		++kIter;
		kIter->Pop(kKey.iObjectNo);			++kIter;
		kIter->Pop(kValue.iEventNo);		++kIter;
		kIter->Pop(kValue.iRate);			++kIter;
		
		kValue.iObjectType = kKey.iObjectType;
		kValue.iObjectType = kKey.iObjectNo;

		CONT_LIMITED_ITEM_CONTROL::iterator itor_control = kCont.find(kKey);
		if (itor_control == kCont.end())
		{
			auto ibRet = kCont.insert(std::make_pair(kKey, CONT_LIMITED_ITEM_CONTROL::mapped_type()));
			itor_control = ibRet.first;
		}
		(*itor_control).second.push_back(kValue);
	}

	g_kCoreCenter.ClearQueryResult(rkResult);
	g_kTblDataMgr.SetContDef(kCont);

	return S_OK;
}


HRESULT PgItemDropControlImpl::Q_DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult)
{
	QueryCount(QueryCount()-1);

	if( CEL::DR_SUCCESS != rkResult.eRet 
	&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
		return E_FAIL;
	}

	CONT_LIMITED_ITEM_RECORD kCont;

	CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
	while(kIter != rkResult.vecArray.end())
	{
		CONT_LIMITED_ITEM_RECORD::mapped_type	kValue;

		kIter->Pop(kValue.iEventNo);			++kIter;
		kIter->Pop(kValue.dtLimitRefreshDate);	++kIter;
		kIter->Pop(kValue.dtSafeRefreshDate);	++kIter;
		kIter->Pop(kValue.dtStartDate);			++kIter;
		kIter->Pop(kValue.dtEndDate);			++kIter;
		kIter->Pop(kValue.iLimit_ResetPeriod);	++kIter;
		kIter->Pop(kValue.iSafe_ResetPeriod);	++kIter;
		kIter->Pop(kValue.iRefreshCount);		++kIter;
		kIter->Pop(kValue.iLimitCount);			++kIter;
		kIter->Pop(kValue.iSafeCount);			++kIter;

		kCont.insert(std::make_pair(kValue.iEventNo, kValue));
	}

	g_kCoreCenter.ClearQueryResult(rkResult);

	m_kContRecord.swap(kCont);
	
	BM::DBTIMESTAMP_EX kNow;
	kNow.SetLocalTime();
	
	{
		CONT_LIMITED_ITEM_RECORD::const_iterator record_itor = m_kContRecord.begin();
		while(record_itor != m_kContRecord.end())
		{
			CONT_LIMITED_ITEM_RECORD::mapped_type const &kElement = (*record_itor).second;

			if(kElement.IsCorrectTime(kNow))
			{
				ACE_Time_Value kAceNow = kNow;
				ACE_Time_Value kAceSafeRefreshDate(kElement.dtSafeRefreshDate.operator const ACE_Time_Value());
				kAceSafeRefreshDate += ACE_Time_Value(static_cast<double>(kElement.iSafe_ResetPeriod*60));

	//			int iSec1 = kAceSafeRefreshDate.sec();
	//			int iSec2 = kAceNow.sec();
				if(	kElement.dtSafeRefreshDate.year == 0 || kAceSafeRefreshDate < kAceNow )
				{//창고 갱신 해야한다.
					//업데이트 날려야함. 창고 숫자 ++ 하는걸로.
	//				INFO_LOG( BM::LOG_LV1, __FL__ << _T("창고 갱신 시도 "));

					QueryCount(QueryCount()+1);
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD, _T("EXEC dbo.[UP_UpdateLimitedItemRecord_Safe]"));
					kQuery.InsertQueryTarget(MgrGuid());
					kQuery.PushStrParam(kElement.iEventNo);
					kQuery.PushStrParam(kNow);

					if( S_OK != g_kCoreCenter.PushQuery(kQuery) )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot PushQuery Query=") << kQuery.Command());
					}
				}
				else
				{
	//				INFO_LOG( BM::LOG_LV2, __FL__ << _T("창고 갱신 시도 실패(시간초과)"));
				}
			}

			++record_itor;
		}

	}

	{
		CONT_LIMITED_ITEM_RECORD::const_iterator record_itor = m_kContRecord.begin();
		while(record_itor != m_kContRecord.end())
		{
			CONT_LIMITED_ITEM_RECORD::mapped_type const &kElement = (*record_itor).second;

			if(kElement.IsCorrectTime(kNow))
			{
				ACE_Time_Value kAceNow = kNow;
				ACE_Time_Value kAceLimitRefreshDate = kElement.dtLimitRefreshDate;
				kAceLimitRefreshDate += ACE_Time_Value(static_cast<double>(kElement.iLimit_ResetPeriod*60));

				if(	(kElement.dtLimitRefreshDate.year == 0 || kAceLimitRefreshDate < kAceNow)
				&&	0 < kElement.iSafeCount 
				&&	0 >= kElement.iLimitCount )//실물이 비어야 함
				{//창고 갱신 해야한다.
	//				INFO_LOG( BM::LOG_LV3, __FL__ << _T("실물 갱신 시도"));
					//업데이트 날려야함. 창고 숫자 ++ 하는걸로.
					QueryCount(QueryCount()+1);
					int const iLimitAdd = std::max(1, static_cast<int>(static_cast<float>(kElement.iLimit_ResetPeriod) / kElement.iSafe_ResetPeriod * kElement.iRefreshCount));
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD, _T("EXEC dbo.[UP_UpdateLimitedItemRecord_Limit]"));
					kQuery.InsertQueryTarget(MgrGuid());
					kQuery.PushStrParam(kElement.iEventNo);
					kQuery.PushStrParam(iLimitAdd);
					kQuery.PushStrParam(kNow);

					if( S_OK != g_kCoreCenter.PushQuery(kQuery) )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot PushQuery Query=") << kQuery.Command());
					}
				}
				else
				{
	//				INFO_LOG( BM::LOG_LV2, __FL__ << _T("실물 갱신 시도 실패"));
				}
			}

			++record_itor;
		}
	}

	return S_OK;
}

HRESULT PgItemDropControlImpl::Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult)
{
	QueryCount(QueryCount()-1);
	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		if ( itr!= rkResult.vecArray.end() )
		{
			int iEventNo = 0;
			int iLimitCount = 0;
			(*itr).Pop(iEventNo);	++itr;
			(*itr).Pop(iLimitCount);	++itr;

			CONT_LIMITED_ITEM_RECORD::iterator itor_item = m_kContRecord.find(iEventNo);
			if (itor_item != m_kContRecord.end())
			{
				(*itor_item).second.iLimitCount = iLimitCount;
			}
		}
		return S_OK;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Limit Field Refresh Failed") );
	return E_FAIL;

}

HRESULT PgItemDropControlImpl::Q_DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		if ( itr!= rkResult.vecArray.end() )
		{
			int iEventNo = 0;
			int iLimitCount = 0;
			(*itr).Pop(iEventNo);	++itr;
			(*itr).Pop(iLimitCount);	++itr;

			CONT_LIMITED_ITEM_RECORD::iterator itor_item = m_kContRecord.find(iEventNo);
			if (itor_item != m_kContRecord.end())
			{
				(*itor_item).second.iLimitCount = std::max(0, iLimitCount-1);
			}

			if (iLimitCount > 0)
			{
				SERVER_IDENTITY kSI;
				SREQ_GIVE_LIMITED_ITEM kData;
				
				rkResult.contUserData.Pop(kSI);
				rkResult.contUserData.Pop(kData);

				BM::Stream kPacket(PT_M_N_ANS_GIVE_LIMITED_ITEM, kData);
				::SendToGround(kSI, kData.m_kGndKey, kPacket);
			}
		}
		return S_OK;
	}
	return E_FAIL;
}


HRESULT PgItemDropControlImpl::Q_DQT_SYNC_LOCAL_LIMITED_ITEM(CEL::DB_RESULT& rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet 
	&&	CEL::DR_NO_RESULT != rkResult.eRet)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Limit Field Refresh Failed") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! DB_RESULT=") << rkResult.eRet );
//		INFO_LOG( BM::LOG_LV6, __FL__ << _T("싱크실패!"));
		return E_FAIL;
	}

//	INFO_LOG( BM::LOG_LV6, __FL__ << _T("싱크 성공!"));
	return S_OK;
}

HRESULT PgItemDropControlImpl::Q_DQT_UPDATE_SAFE_LIMITED_ITEM_RECORD(CEL::DB_RESULT& rkResult)
{
	QueryCount(QueryCount()-1);
	return S_OK;
}


void PgItemDropControlImpl::Timer1m()
{
	if (0 < QueryCount())
	{
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Cannot Query, because DB is too busy QueryState=") << QueryCount());
		return;
	}
	QueryCount(QueryCount()+1);
	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CHECK_LIMIT_LIMITED_ITEM_RECORD, _T("EXEC [dbo].[UP_Load_LimitedItemRecord]"));
	kQuery.InsertQueryTarget(MgrGuid());
	if( S_OK != g_kCoreCenter.PushQuery(kQuery) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("PushQuery failed...Query=") << kQuery.Command());
	}
}

void PgItemDropControlImpl::RequestItemPop(SERVER_IDENTITY const& rkSI, SREQ_GIVE_LIMITED_ITEM const& kReq)
{
	CONT_LIMITED_ITEM_RECORD::const_iterator const_itor = m_kContRecord.find(kReq.m_kTryLimitEventNo);
	if (const_itor == m_kContRecord.end())
	{
		return;
	}
	BM::DBTIMESTAMP_EX kNow;
	kNow.SetLocalTime();
	if ((*const_itor).second.iLimitCount > 0 && (*const_itor).second.IsCorrectTime(kNow))
	{
		// iLimitCount > 0 일때만 요청 보내자.
		// 혹시 이 값이 정확하지 않을 지라도, DB에서 다시 검사하므로, 상관없다.
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_LIMIT_LIMITED_ITEM_RECORD_POP, _T("EXEC [dbo].[UP_UpdateLimitedItemRecord_Limit_Pop]"));
		kQuery.PushStrParam(kReq.m_kTryLimitEventNo);
		kQuery.contUserData.Push(rkSI);
		kQuery.contUserData.Push(kReq);
		if( S_OK != g_kCoreCenter.PushQuery(kQuery) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("PushQuery failed...Query=") << kQuery.Command());
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//	PgItemDropControl
PgItemDropControl::PgItemDropControl()
{
}

PgItemDropControl::~PgItemDropControl()
{
}

HRESULT PgItemDropControl::OnDBProcess(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_,true);
	return Instance()->OnDBProcess(rkResult);
}

void PgItemDropControl::Timer1m()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->Timer1m();
}

void PgItemDropControl::RequestItemPop(SERVER_IDENTITY const& rkSI, SREQ_GIVE_LIMITED_ITEM const& kReq)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	Instance()->RequestItemPop(rkSI, kReq);
}