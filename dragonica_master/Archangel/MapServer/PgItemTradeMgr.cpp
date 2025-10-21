#include "stdafx.h"
#include "PgItemTradeMgr.h"
#include "BM/PgTask.h"
#include "Global.h"

CItemTradeMgr::CItemTradeMgr()
{
	Init();
}

CItemTradeMgr::~CItemTradeMgr()
{
	Clear();
}

void CItemTradeMgr::Clear()
{
	BM::CAutoMutex kLock(m_kLock);
	TradeItemCont::iterator trade_it = m_kTradeItemCont.begin();
	while (m_kTradeItemCont.end() != trade_it)
	{
		m_kItemTradePool.Delete((*trade_it).second);
		++trade_it;
	}
	m_kTradeItemCont.clear();
	m_kTradeItemContByChar.clear();
	m_kTradeTimeCont.clear();
}

void CItemTradeMgr::Init()
{
	Clear();
}

const BM::GUID CItemTradeMgr::NewItemTrade(BM::GUID const &kFrom, BM::GUID const &kTo)
{
	BM::CAutoMutex kLock(m_kLock);

	STradeInfo *pkNewInfo = m_kItemTradePool.New();
	if (!pkNewInfo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NullData"));
		return BM::GUID::NullData();
	}

	if(!pkNewInfo->Create(kFrom, kTo))
	{
		m_kItemTradePool.Delete(pkNewInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NullData"));
		return BM::GUID::NullData();
	}

	auto target_ib = m_kTradeItemContByChar.insert( std::make_pair(pkNewInfo->kTargetGuid, pkNewInfo) );
	auto caster_ib = m_kTradeItemContByChar.insert( std::make_pair(pkNewInfo->kCasterGuid, pkNewInfo) );
	if(!target_ib.second || !caster_ib.second) //둘중 하나라도 실패하면
	{
		m_kTradeItemContByChar.erase( pkNewInfo->kTargetGuid );
		m_kTradeItemContByChar.erase( pkNewInfo->kCasterGuid );
		m_kItemTradePool.Delete( pkNewInfo );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NullData"));
		return BM::GUID::NullData();
	}

	auto eRet = m_kTradeItemCont.insert( std::make_pair(pkNewInfo->kTradeGuid, pkNewInfo));
	if (!eRet.second)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"m_kTradeItemCont insert failed");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
		DWORD const dwTime = BM::GetTime32();
		TradeItemContConst::iterator cast_it = m_kTradeItemContByChar.find(pkNewInfo->kCasterGuid);
		if (m_kTradeItemContByChar.end() != cast_it)
		{
			m_kTradeTimeCont.insert(std::make_pair(pkNewInfo->kCasterGuid, dwTime));
			m_kTradeItemContByChar.erase(cast_it);
		}

		TradeItemContConst::iterator targ_it = m_kTradeItemContByChar.find(pkNewInfo->kTargetGuid);
		if (m_kTradeItemContByChar.end() != targ_it)
		{
			m_kTradeTimeCont.insert(std::make_pair(pkNewInfo->kTargetGuid, dwTime));
			m_kTradeItemContByChar.erase(targ_it);
		}
		pkNewInfo->Init();
		m_kItemTradePool.Delete(pkNewInfo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NullData"));
		return BM::GUID::NullData();
	}

	return pkNewInfo->kTradeGuid;
}

bool CItemTradeMgr::DelItemTrade(BM::GUID const &kTradeGuid)
{
	BM::CAutoMutex kLock(m_kLock);
	if (BM::GUID::NullData() == kTradeGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TradeItemCont::iterator trade_it = m_kTradeItemCont.find(kTradeGuid);
	if (m_kTradeItemCont.end() == trade_it)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	STradeInfo *pkInfo = (*trade_it).second;
	if (pkInfo)
	{
		DWORD const dwTime = BM::GetTime32();
		TradeItemContConst::iterator cast_it = m_kTradeItemContByChar.find(pkInfo->kCasterGuid);
		if (m_kTradeItemContByChar.end() != cast_it)
		{
			m_kTradeTimeCont.insert(std::make_pair(pkInfo->kCasterGuid, dwTime));
			m_kTradeItemContByChar.erase(cast_it);
		}

		TradeItemContConst::iterator targ_it = m_kTradeItemContByChar.find(pkInfo->kTargetGuid);
		if (m_kTradeItemContByChar.end() != targ_it)
		{
			m_kTradeTimeCont.insert(std::make_pair(pkInfo->kTargetGuid, dwTime));
			m_kTradeItemContByChar.erase(targ_it);
		}

		pkInfo->Init();
		m_kItemTradePool.Delete(pkInfo);
		m_kTradeItemCont.erase(trade_it);
	}

	return true;
}

bool CItemTradeMgr::DelItemTradeByChar(BM::GUID const &kCharGuid)
{
	if (BM::GUID::NullData() == kCharGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kLock);
	TradeItemContConst::const_iterator trade_it = m_kTradeItemContByChar.find(kCharGuid);
	if (m_kTradeItemContByChar.end() == trade_it)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return DelItemTrade((*trade_it).second->kTradeGuid);
}

bool CItemTradeMgr::FindTrade(BM::GUID const &kGuid, STradeInfo &rkDest) const
{
	if (BM::GUID::NullData() == kGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kLock);

	TradeItemCont::const_iterator trade_it = m_kTradeItemCont.find(kGuid);
	if (m_kTradeItemCont.end() == trade_it)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkDest = *((*trade_it).second);
	return true;
}

bool CItemTradeMgr::FindTradeByChar(BM::GUID const &kGuid, STradeInfo &rkDest) const
{
	if (BM::GUID::NullData() == kGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::CAutoMutex kLock(m_kLock);

	TradeItemContConst::const_iterator char_it = m_kTradeItemContByChar.find(kGuid);
	if (m_kTradeItemContByChar.end() == char_it)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const STradeInfo *pkInfo = (*char_it).second;

	if (pkInfo)	//콘스트니까 절대로 참조만 해야된다
	{
		return FindTrade(pkInfo->kTradeGuid, rkDest);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CItemTradeMgr::ModifyTrade(const STradeInfo &rkDest)
{
	BM::CAutoMutex kLock(m_kLock);
	TradeItemCont::iterator trade_it = m_kTradeItemCont.find(rkDest.kTradeGuid);
	if (m_kTradeItemCont.end() != trade_it)
	{
		*(*trade_it).second = rkDest;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

DWORD const CItemTradeMgr::FineTradeEndTime(const BM::GUID kCharacterGuid) const
{
	if (BM::GUID::NullData() == kCharacterGuid)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	BM::CAutoMutex kLock(m_kLock);
	TradeTimeCont::const_iterator time_it = m_kTradeTimeCont.find(kCharacterGuid);
	if (m_kTradeTimeCont.end() == time_it)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	return (*time_it).second;
}

void CItemTradeMgr::Tick(DWORD const dwTime)
{
	BM::CAutoMutex kLock(m_kLock);
	TradeTimeCont::iterator time_it = m_kTradeTimeCont.begin();	
	while (m_kTradeTimeCont.end() != time_it)
	{
		DWORD const kTradeTime = (*time_it).second;
		if (3000 < dwTime - kTradeTime)
		{
			time_it = m_kTradeTimeCont.erase(time_it);
		}
		else
		{
			++time_it;
		}
	}
}

ETRADERESULT CItemTradeMgr::CheckTradeItem(CONT_TRADE_ITEM const &rkList, CUnit* pkUnit) const
{
	if(NULL==pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return TR_FAIL"));
		return TR_FAIL;
	}

	CONT_TRADE_ITEM::const_iterator item_it = rkList.begin();
	PgBase_Item kItem;
	while(rkList.end() != item_it)
	{
		kItem.Clear();
		if((S_OK != pkUnit->GetInven()->GetItem((*item_it).kPos,kItem)) ||	//아이템이 없거나
			(kItem.ItemNo()!=(*item_it).kItem.ItemNo()) ||						//번호가 다르거나
			(kItem.Guid()!=(*item_it).kItem.Guid()) ||						//GUID가 다르거나
			(kItem.Count()<(*item_it).kItem.Count()) )						//갯수가 서버가 알고있는것보다 많으면
		{
			INFO_LOG(BM::LOG_LV0, (BM::vstring)__FL__<<" Not Find Trade Item. MemberName : "<<pkUnit->Name()<<" MemberGuid["<<pkUnit->GetID().str().c_str()<<"] ItemNo["<<(*item_it).kItem.ItemNo()<<"]"); 
			return TR_NOT_FIND_ITEM;
		}

		if(false == CheckEnableTrade(kItem,ICMET_Cant_PlayerTrade))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return TR_FAIL"));
			return TR_FAIL;
		}
		++item_it;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return TR_NONE"));
	return TR_NONE;
}