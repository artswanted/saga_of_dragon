#include "stdafx.h"
#include "lohengrin/dbtables.h"
#include "lohengrin/dbtables2.h"
#include "PgEventItemSet.h"

////////////////////////////////////////////
// User Equip Info
//
PgEquipItemInfo::PgEquipItemInfo()
	: m_kContItemCount()
{
}
PgEquipItemInfo::PgEquipItemInfo(PgEquipItemInfo const& rhs)
	: m_kContItemCount(rhs.m_kContItemCount)
{
}
PgEquipItemInfo::~PgEquipItemInfo()
{
}
void PgEquipItemInfo::Equip(int const iItemNo)
{
	CONT_ITEM_COUNT::iterator find_iter = m_kContItemCount.find( iItemNo );
	if( m_kContItemCount.end() != find_iter )
	{
		++(*find_iter).second;
	}
	else
	{
		m_kContItemCount.insert( std::make_pair(iItemNo, 1) );
	}
}
void PgEquipItemInfo::Unequip(int const iItemNo)
{
	CONT_ITEM_COUNT::iterator find_iter = m_kContItemCount.find(iItemNo);
	if( m_kContItemCount.end() != find_iter )
	{
		--(*find_iter).second;
		if( 0 >= (*find_iter).second )
		{
			m_kContItemCount.erase( find_iter );
		}
	}
}
void PgEquipItemInfo::Unequip(CONT_ITEM_NO const& rkCont)
{
	CONT_ITEM_NO::const_iterator iter = rkCont.begin();
	while( rkCont.end() != iter )
	{
		Unequip((*iter));
		++iter;
	}
}
int PgEquipItemInfo::GetEquipCount(int const iItemNo) const
{
	CONT_ITEM_COUNT::const_iterator find_iter = m_kContItemCount.find(iItemNo);
	if( m_kContItemCount.end() != find_iter )
	{
		return (*find_iter).second;
	}
	return 0;
}

//
tagPlayerEventItemEquipInfo::tagPlayerEventItemEquipInfo()
	: m_kContItems()
{
}
tagPlayerEventItemEquipInfo::tagPlayerEventItemEquipInfo(tagPlayerEventItemEquipInfo const& rhs)
	: m_kContItems(rhs.m_kContItems)
{
}
tagPlayerEventItemEquipInfo::~tagPlayerEventItemEquipInfo()
{
}
void tagPlayerEventItemEquipInfo::Equip(int const iItemNo)
{
	m_kContItems.insert( iItemNo );
}
void tagPlayerEventItemEquipInfo::Unequip(int const iItemNo)
{
	m_kContItems.erase( iItemNo );
}

//
PgEquipItemMgr::PgEquipItemMgr()
	: m_kContEquipInfo(), m_kAllEquipInfo()
{
}
PgEquipItemMgr::PgEquipItemMgr(PgEquipItemMgr const& rhs)
	: m_kContEquipInfo(rhs.m_kContEquipInfo), m_kAllEquipInfo(rhs.m_kAllEquipInfo)
{
}
PgEquipItemMgr::~PgEquipItemMgr()
{
}
void PgEquipItemMgr::Add(BM::GUID const& rkGuid)
{
	m_kContEquipInfo.insert( std::make_pair(rkGuid, CONT_PLAYER_ITEM_EQUIP_INFO::mapped_type()) );
}
void PgEquipItemMgr::Del(BM::GUID const& rkGuid)
{
	CONT_PLAYER_ITEM_EQUIP_INFO::iterator find_iter = m_kContEquipInfo.find(rkGuid);
	if( m_kContEquipInfo.end() != find_iter )
	{
		m_kAllEquipInfo.Unequip((*find_iter).second.ContItems());
		m_kContEquipInfo.erase(find_iter);
	}
}
void PgEquipItemMgr::Equip(BM::GUID const& rkGuid, int const iItemNo)
{
	CONT_PLAYER_ITEM_EQUIP_INFO::iterator find_iter = m_kContEquipInfo.find(rkGuid);
	if( m_kContEquipInfo.end() != find_iter )
	{
		(*find_iter).second.Equip(iItemNo);
	}
	m_kAllEquipInfo.Equip(iItemNo);
}
void PgEquipItemMgr::Unequip(BM::GUID const& rkGuid, int const iItemNo)
{
	CONT_PLAYER_ITEM_EQUIP_INFO::iterator find_iter = m_kContEquipInfo.find(rkGuid);
	if( m_kContEquipInfo.end() != find_iter )
	{
		(*find_iter).second.Unequip(iItemNo);
	}
	m_kAllEquipInfo.Unequip(iItemNo);
}
void PgEquipItemMgr::GetGuids(VEC_GUID& rkContOut)
{
	CONT_PLAYER_ITEM_EQUIP_INFO::iterator find_iter = m_kContEquipInfo.begin();
	while( m_kContEquipInfo.end() != find_iter )
	{
		rkContOut.push_back( (*find_iter).first );
		++find_iter;
	}
}


////////////////////////////////////////////
// Def
//
tagEventItem::tagEventItem()
	: iItemNo(0), iCount(0), bIsAbsoluteCount(false)
{
}
tagEventItem::tagEventItem(tagEventItem const& rhs)
	: iItemNo(rhs.iItemNo), iCount(rhs.iCount), bIsAbsoluteCount(rhs.bIsAbsoluteCount)
{
}
tagEventItem::tagEventItem(int const& riItemNo, int const& riCount, bool const& rbIsAbsoluteCount)
	: iItemNo(riItemNo), iCount(riCount), bIsAbsoluteCount(rbIsAbsoluteCount)
{
}
tagEventItem::~tagEventItem()
{
}

bool tagEventItem::operator ==(int const rhs) const
{
	return iItemNo == rhs;
}

//
PgEventItemSet::PgEventItemSet()
	: m_iEventID(0), m_bIsUse(true), m_kContItemNo(), m_kContRewardEffect()
{
}
PgEventItemSet::PgEventItemSet(PgEventItemSet const& rhs)
	: m_iEventID(rhs.m_iEventID), m_bIsUse(rhs.m_bIsUse), m_kContItemNo(rhs.m_kContItemNo), m_kContRewardEffect(rhs.m_kContRewardEffect)
{
}
PgEventItemSet::PgEventItemSet(TBL_DEF_EVENT_ITEM_SET const& rkDef)
	: m_iEventID(rkDef.iEventItemSetID), m_bIsUse(true), m_kContItemNo(), m_kContRewardEffect()
{
	Regist(rkDef);
}
PgEventItemSet::~PgEventItemSet()
{
}

void PgEventItemSet::Regist(TBL_DEF_EVENT_ITEM_SET const& rkDef)
{
	if( m_bIsUse != rkDef.bIsUse ) // 사용
	{
		m_bIsUse = m_bIsUse && rkDef.bIsUse; // And 연산
	}
	if( 0 < rkDef.iEquipItemNo
	&&	0 < rkDef.iItemCount ) // 아이템
	{
		CONT_DEF_BUILDED_EVENT_ITEM::iterator find_iter = std::find(m_kContItemNo.begin(), m_kContItemNo.end(), rkDef.iEquipItemNo);
		if( m_kContItemNo.end() == find_iter )
		{
			m_kContItemNo.push_back( CONT_DEF_BUILDED_EVENT_ITEM::value_type(rkDef.iEquipItemNo, rkDef.iItemCount, rkDef.bIsAbsoluteCount) );
		}
		else
		{
			(*find_iter).iCount += rkDef.iItemCount;
		}
	}
	for( int iCur = 0; MAX_EVENT_ITEM_SET_REWARD > iCur; ++iCur )
	{
		if( 0 < rkDef.aiRewardEffectID[iCur] ) // 보상 이펙트
		{
			m_kContRewardEffect.insert(rkDef.aiRewardEffectID[iCur]);
		}
	}
}

/////////////////////////////////////////////////
// party
//
PgPartyEquiptItemInfo::PgPartyEquiptItemInfo()
	: m_kContCompleteEventID(), m_kEquipItemMgr()
{
}
PgPartyEquiptItemInfo::PgPartyEquiptItemInfo(PgPartyEquiptItemInfo const& rhs)
	: m_kContCompleteEventID(rhs.m_kContCompleteEventID), m_kEquipItemMgr(rhs.m_kEquipItemMgr)
{
}
PgPartyEquiptItemInfo::~PgPartyEquiptItemInfo()
{
}
void PgPartyEquiptItemInfo::Regist(BM::GUID const& rkGuid, CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr)
{
	m_kEquipItemMgr.Add(rkGuid);
	// 이미 달성된 이펙트 추가
	CONT_COMPLETE_EVENT_ITEM_SET_ID::const_iterator iter = m_kContCompleteEventID.begin();
	while( m_kContCompleteEventID.end() != iter )
	{
		CONT_DEF_BUILDED_EVENT_ITEM_SET::const_iterator find_iter = rkContEventItemSet.find((*iter));
		if( rkContEventItemSet.end() != find_iter )
		{
			rkEffectQueueMgr.AddEffect(rkGuid, (*find_iter).second.ContRewardEffect());
		}
		++iter;
	}
}
void PgPartyEquiptItemInfo::Unregist(BM::GUID const& rkGuid, CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr)
{
	m_kEquipItemMgr.Del(rkGuid);
	// 나가는 유저에 붙은 이펙트 제거
	CONT_COMPLETE_EVENT_ITEM_SET_ID::const_iterator iter = m_kContCompleteEventID.begin();
	while( m_kContCompleteEventID.end() != iter )
	{
		CONT_DEF_BUILDED_EVENT_ITEM_SET::const_iterator find_iter = rkContEventItemSet.find((*iter));
		if( rkContEventItemSet.end() != find_iter )
		{
			rkEffectQueueMgr.DelEffect(rkGuid, (*find_iter).second.ContRewardEffect());
		}
		++iter;
	}
}
void PgPartyEquiptItemInfo::Equip(BM::GUID const& rkGuid, int const iItemNo)
{
	m_kEquipItemMgr.Equip(rkGuid, iItemNo);
}
void PgPartyEquiptItemInfo::Unequip(BM::GUID const& rkGuid, int const iItemNo)
{
	m_kEquipItemMgr.Unequip(rkGuid, iItemNo);
}
void PgPartyEquiptItemInfo::Check(CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr)
{
	CONT_PREPARATION_EFFECT kContAddEffects;
	CONT_PREPARATION_EFFECT kContDel;

	CONT_DEF_BUILDED_EVENT_ITEM_SET::const_iterator iter = rkContEventItemSet.begin();
	while( rkContEventItemSet.end() != iter )
	{
		CONT_DEF_BUILDED_EVENT_ITEM_SET::mapped_type const& rkEventItemSet = (*iter).second;
		if( rkEventItemSet.IsUse() )
		{
			CONT_DEF_BUILDED_EVENT_ITEM::const_iterator item_iter = rkEventItemSet.ContItemNo().begin();
			bool bCompleteEvent = true;
			while( rkEventItemSet.ContItemNo().end() != item_iter )
			{
				CONT_DEF_BUILDED_EVENT_ITEM::value_type const& rkEventItem = (*item_iter);
				int const iItemCount = m_kEquipItemMgr.GetEquipCount(rkEventItem.iItemNo);
				if( 0 < iItemCount )
				{
					if( rkEventItem.bIsAbsoluteCount )
					{
						bCompleteEvent = (bCompleteEvent && (rkEventItem.iCount == iItemCount));
					}
					else
					{
						// 1 == rkEventItem.iCount, 
						bCompleteEvent = (bCompleteEvent && (rkEventItem.iCount <= iItemCount));
					}
				}
				else
				{
					bCompleteEvent = false;
				}
				++item_iter;
			}

			VEC_GUID kContGuid;
			m_kEquipItemMgr.GetGuids(kContGuid);
			CONT_COMPLETE_EVENT_ITEM_SET_ID::iterator eventid_iter = m_kContCompleteEventID.find(rkEventItemSet.EventID());
			if( bCompleteEvent )
			{
				if( m_kContCompleteEventID.end() == eventid_iter )
				{
					m_kContCompleteEventID.insert(rkEventItemSet.EventID());
					kContAddEffects.push_back( SPreparationEffectData(kContGuid, rkEventItemSet.ContRewardEffect()) );
				}
			}
			else
			{
				if( m_kContCompleteEventID.end() != eventid_iter )
				{
					m_kContCompleteEventID.erase(eventid_iter);
					kContDel.push_back( SPreparationEffectData(kContGuid, rkEventItemSet.ContRewardEffect()) );
				}
			}
		}
		++iter;
	}

	{
		CONT_PREPARATION_EFFECT::const_iterator kItor = kContDel.begin();
		while(kContDel.end() != kItor)
		{// 지금삭제 해야하는 이펙트가, 다시 걸려야 하는 경우도 있으므로 Add하기전에 삭제하고, 
			rkEffectQueueMgr.DelEffect((*kItor).kContGuid, (*kItor).kContEffectID);
			++kItor;
		}

		kItor = kContAddEffects.begin();
		while(kContAddEffects.end() != kItor)
		{// 다 삭제된 이후에 이펙트를 Add한다
			rkEffectQueueMgr.AddEffect((*kItor).kContGuid, (*kItor).kContEffectID);
			++kItor;
		}
	}
}

PgPartyEquiptItemInfoMgr::PgPartyEquiptItemInfoMgr(CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkBuildedDef, PgEventEffectQueueMgr& rkEffectQueueMgr)
	: m_kContPartyInfo(), m_rkContEventItemSet(rkBuildedDef), m_rkEffectQueueMgr(rkEffectQueueMgr)
{
}
PgPartyEquiptItemInfoMgr::PgPartyEquiptItemInfoMgr(PgPartyEquiptItemInfoMgr const& rhs)
	: m_kContPartyInfo(), m_rkContEventItemSet(rhs.m_rkContEventItemSet), m_rkEffectQueueMgr(rhs.m_rkEffectQueueMgr)
{
}
PgPartyEquiptItemInfoMgr::~PgPartyEquiptItemInfoMgr()
{
}
void PgPartyEquiptItemInfoMgr::Clear()
{
	m_kContPartyInfo.clear();
}
void PgPartyEquiptItemInfoMgr::Regist(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid)
{
	CONT_PARTY::iterator find_iter = m_kContPartyInfo.find(rkPartyGuid);
	if( m_kContPartyInfo.end() == find_iter )
	{
		auto kRet = m_kContPartyInfo.insert( std::make_pair(rkPartyGuid, CONT_PARTY::mapped_type()) );
		if( kRet.second )
		{
			find_iter = kRet.first;
		}
	}
	if( m_kContPartyInfo.end() != find_iter )
	{
		(*find_iter).second.Regist(rkGuid, m_rkContEventItemSet, m_rkEffectQueueMgr);
	}
}
void PgPartyEquiptItemInfoMgr::Unregist(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid)
{
	CONT_PARTY::iterator find_iter = m_kContPartyInfo.find(rkPartyGuid);
	if( m_kContPartyInfo.end() != find_iter )
	{
		(*find_iter).second.Unregist(rkGuid, m_rkContEventItemSet, m_rkEffectQueueMgr);
		if( (*find_iter).second.IsEmpty() )
		{
			m_kContPartyInfo.erase(find_iter);
		}
	}
}
void PgPartyEquiptItemInfoMgr::Equip(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid, int const iItemNo)
{
	CONT_PARTY::iterator find_iter = m_kContPartyInfo.find(rkPartyGuid);
	if( m_kContPartyInfo.end() != find_iter )
	{
		(*find_iter).second.Equip(rkGuid, iItemNo);
	}
}
void PgPartyEquiptItemInfoMgr::Unequip(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid, int const iItemNo)
{
	CONT_PARTY::iterator find_iter = m_kContPartyInfo.find(rkPartyGuid);
	if( m_kContPartyInfo.end() != find_iter )
	{
		(*find_iter).second.Unequip(rkGuid, iItemNo);
	}
}
void PgPartyEquiptItemInfoMgr::Check(BM::GUID const& rkPartyGuid)
{
	CONT_PARTY::iterator find_iter = m_kContPartyInfo.find(rkPartyGuid);
	if( m_kContPartyInfo.end() != find_iter )
	{
		(*find_iter).second.Check(m_rkContEventItemSet, m_rkEffectQueueMgr);
	}
}

/////////////////////////////////////////////////
// effect queue
//
tagEventEffectQueue::tagEventEffectQueue()
	: kAddQueue(), kDelQueue()
{
}
tagEventEffectQueue::tagEventEffectQueue(tagEventEffectQueue const& rhs)
	: kAddQueue(rhs.kAddQueue), kDelQueue(rhs.kDelQueue)
{
}
tagEventEffectQueue::~tagEventEffectQueue()
{
}

void tagEventEffectQueue::AddQueue(int const iEffectID)
{
	kAddQueue.insert( iEffectID );
	kDelQueue.erase( iEffectID );
}
void tagEventEffectQueue::DelQueue(int const iEffectID)
{
	kAddQueue.erase( iEffectID );
	kDelQueue.insert( iEffectID );
}
bool tagEventEffectQueue::Empty() const
{
	return kAddQueue.empty() && kDelQueue.empty();
}
void tagEventEffectQueue::Swap(CONT_EVENT_EFFECT_QUEUE& rkAddQueue, CONT_EVENT_EFFECT_QUEUE& rkDelQueue)
{
	kAddQueue.swap( rkAddQueue );
	kDelQueue.swap( rkDelQueue );
}

//
PgEventEffectQueueMgr::PgEventEffectQueueMgr()
	: m_kContEffectQueue()
{
}
PgEventEffectQueueMgr::~PgEventEffectQueueMgr()
{
}
void PgEventEffectQueueMgr::Clear()
{
	m_kContEffectQueue.clear();
}
void PgEventEffectQueueMgr::AddEffect(BM::GUID const& rkGuid, int const iEffectID)
{
	CONT_PLAYER_EVENT_EFFECT_QUEUE::iterator find_iter = m_kContEffectQueue.find(rkGuid);
	if( m_kContEffectQueue.end() != find_iter )
	{
		(*find_iter).second.AddQueue(iEffectID);
	}
}
void PgEventEffectQueueMgr::DelEffect(BM::GUID const& rkGuid, int const iEffectID)
{
	CONT_PLAYER_EVENT_EFFECT_QUEUE::iterator find_iter = m_kContEffectQueue.find(rkGuid);
	if( m_kContEffectQueue.end() != find_iter )
	{
		(*find_iter).second.DelQueue(iEffectID);
	}
}
void PgEventEffectQueueMgr::AddEffect(BM::GUID const& rkGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID)
{
	CONT_REWARD_EFFECT_ID::const_iterator iter = rkContEffectID.begin();
	while( rkContEffectID.end() != iter )
	{
		AddEffect(rkGuid, (*iter));
		++iter;
	}
}
void PgEventEffectQueueMgr::DelEffect(BM::GUID const& rkGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID)
{
	CONT_REWARD_EFFECT_ID::const_iterator iter = rkContEffectID.begin();
	while( rkContEffectID.end() != iter )
	{
		DelEffect(rkGuid, (*iter));
		++iter;
	}
}
void PgEventEffectQueueMgr::AddEffect(VEC_GUID const& rkContGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID)
{
	VEC_GUID::const_iterator iter = rkContGuid.begin();
	while( rkContGuid.end() != iter )
	{
		AddEffect((*iter), rkContEffectID);
		++iter;
	}
}
void PgEventEffectQueueMgr::DelEffect(VEC_GUID const& rkContGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID)
{
	VEC_GUID::const_iterator iter = rkContGuid.begin();
	while( rkContGuid.end() != iter )
	{
		DelEffect((*iter), rkContEffectID);
		++iter;
	}
}
void PgEventEffectQueueMgr::Add(BM::GUID const& rkGuid)
{
	m_kContEffectQueue.insert( std::make_pair(rkGuid, CONT_PLAYER_EVENT_EFFECT_QUEUE::mapped_type()) );
}
void PgEventEffectQueueMgr::Del(BM::GUID const& rkGuid)
{
	m_kContEffectQueue.erase( rkGuid );
}
bool PgEventEffectQueueMgr::Check(BM::GUID const& rkGuid, CONT_EVENT_EFFECT_QUEUE& rkAddQueue, CONT_EVENT_EFFECT_QUEUE& rkDelQueue)
{
	CONT_PLAYER_EVENT_EFFECT_QUEUE::iterator find_iter = m_kContEffectQueue.find( rkGuid );
	if( m_kContEffectQueue.end() != find_iter )
	{
		bool const bRet = !(*find_iter).second.Empty();
		if( bRet )
		{
			(*find_iter).second.Swap(rkAddQueue, rkDelQueue);
		}
		return bRet;
	}
	return false;
}


/////////////////////////////////////////////////
// Event mgr
//
PgEventItemSetMgr::PgEventItemSetMgr()
	: m_kContEvent(), m_kContEventItem(), m_kEffectQueueMgr(), m_kPartyEventItemMgr(m_kContEvent, m_kEffectQueueMgr)/*, m_kCharToParty()*/
{
}
PgEventItemSetMgr::~PgEventItemSetMgr()
{
}

bool PgEventItemSetMgr::Build()
{
	CONT_DEF_EVENT_ITEM_SET const* pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}
	
	CONT_DEF_EVENT_ITEM_SET::const_iterator def_iter = pkDef->begin();
	while( pkDef->end() != def_iter )
	{
		CONT_DEF_EVENT_ITEM_SET::value_type const& rkDef = (*def_iter);
		CONT_DEF_BUILDED_EVENT_ITEM_SET::iterator find_iter = m_kContEvent.find( rkDef.iEventItemSetID );
		if( m_kContEvent.end() == find_iter )
		{
			auto kRet = m_kContEvent.insert( std::make_pair(rkDef.iEventItemSetID, CONT_DEF_BUILDED_EVENT_ITEM_SET::mapped_type(rkDef)) );
			if( kRet.second )
			{
				find_iter = kRet.first;
			}
		}
		else
		{
			(*find_iter).second.Regist( rkDef );
		}

		if( 0 < rkDef.iEquipItemNo )
		{
			m_kContEventItem.insert( rkDef.iEquipItemNo ); // Cache 생성
		}

		++def_iter;
	}	
	return Verify();
}
void PgEventItemSetMgr::Clear()
{
	m_kContEvent.clear();
	m_kContEventItem.clear();
	m_kEffectQueueMgr.Clear();
	m_kPartyEventItemMgr.Clear();
}

bool PgEventItemSetMgr::IsEventItem(int const iItemNo) const
{
	return m_kContEventItem.end() != m_kContEventItem.find(iItemNo);
}

bool PgEventItemSetMgr::Verify()
{
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	GET_DEF(CItemDefMgr, kItemDefMgr);

	typedef std::list< BM::vstring > CONT_ERROR_LOG;
	CONT_ERROR_LOG kContErrorMsg;
	CONT_DEF_BUILDED_EVENT_ITEM_SET::const_iterator iter = m_kContEvent.begin();
	while( m_kContEvent.end() != iter )
	{
		CONT_DEF_BUILDED_EVENT_ITEM_SET::mapped_type const& rkEventItemSet = (*iter).second;
		CONT_DEF_BUILDED_EVENT_ITEM::const_iterator item_iter = rkEventItemSet.ContItemNo().begin();
		while( rkEventItemSet.ContItemNo().end() != item_iter )
		{
			int const iItemNo = (*item_iter).iItemNo;
			CItemDef const* pkDefItem = kItemDefMgr.GetDef(iItemNo);
			if( NULL == pkDefItem )
			{
				kContErrorMsg.push_back( CONT_ERROR_LOG::value_type(L"Error: can't find ItemNo[") << iItemNo << L"] from [TB_DefItem], EventItemSetID[" << rkEventItemSet.EventID() << L"]" );
			}
			else
			{
				if( false == pkDefItem->CanEquip() )
				{
					kContErrorMsg.push_back( CONT_ERROR_LOG::value_type(L"Error: wrong ItemNo[") << iItemNo << L"] is can't equipment item, EventItemSetID[" << rkEventItemSet.EventID() << L"]" );
				}
			}
			++item_iter;
		}
		CONT_REWARD_EFFECT_ID::const_iterator effect_iter = rkEventItemSet.ContRewardEffect().begin();
		while( rkEventItemSet.ContRewardEffect().end() != effect_iter )
		{
			int const iEffectID = (*effect_iter);
			CEffectDef const* pkDefEffect = kEffectDefMgr.GetDef(iEffectID);
			if( NULL == pkDefEffect )
			{
				kContErrorMsg.push_back( CONT_ERROR_LOG::value_type(L"Error: can't find EffectID[") << iEffectID << L"] from [TB_DefEffect], EventItemSetID[" << rkEventItemSet.EventID() << L"]" );
			}
			else
			{
				if( 0 != pkDefEffect->GetAbil(AT_SAVE_DB)
				||	0 == pkDefEffect->GetAbil(AT_NOT_DELETE_EFFECT)
				||	0 == pkDefEffect->GetAbil(AT_MAPMOVE_DELETE) )
				{
					kContErrorMsg.push_back( CONT_ERROR_LOG::value_type(L"Error: can't find abil [T=592 V=0][T=3121 V=1][T=5060 V=1] in EffectID[") << iEffectID << L"], EventItemSetID[" << rkEventItemSet.EventID() << L"]" );
				}
			}
			++effect_iter;
		}
		++iter;
	}

	{
		CONT_ERROR_LOG::const_iterator msg_iter = kContErrorMsg.begin();
		while( kContErrorMsg.end() != msg_iter )
		{
			CAUTION_LOG(BM::LOG_LV1, (*msg_iter));
			++msg_iter;
		}
	}
	return kContErrorMsg.empty();
}
void PgEventItemSetMgr::RegistUnit(PgPlayer* pkPlayer)
{
	if( pkPlayer )
	{
		BM::GUID const& rkCharGuid = pkPlayer->GetID();
		m_kEffectQueueMgr.Add(rkCharGuid);
		JoinPartyUnit(pkPlayer, pkPlayer->GetPartyGuid());
	}
}
void PgEventItemSetMgr::UnregistUnit(PgPlayer* pkPlayer)
{
	if( pkPlayer )
	{
		LeavePartyUnit(pkPlayer);
		m_kEffectQueueMgr.Del(pkPlayer->GetID());
	}
}
void PgEventItemSetMgr::JoinPartyUnit(PgPlayer* pkPlayer, BM::GUID const& rkPartyGuid)
{
	if( pkPlayer )
	{
		if( BM::GUID::IsNotNull(rkPartyGuid) )
		{
			//m_kCharToParty.Add(pkPlayer->GetID(), rkPartyGuid);
			m_kPartyEventItemMgr.Regist(rkPartyGuid, pkPlayer->GetID());
			ProcessEquipItem(m_kPartyEventItemMgr, pkPlayer->GetInven(), IT_FIT, rkPartyGuid, pkPlayer->GetID());
			ProcessEquipItem(m_kPartyEventItemMgr, pkPlayer->GetInven(), IT_FIT_CASH, rkPartyGuid, pkPlayer->GetID());
			ProcessEquipItem(m_kPartyEventItemMgr, pkPlayer->GetInven(), IT_FIT_COSTUME, rkPartyGuid, pkPlayer->GetID());
			m_kPartyEventItemMgr.Check(rkPartyGuid);
		}
	}
}
void PgEventItemSetMgr::LeavePartyUnit(PgPlayer* pkPlayer)
{
	if( pkPlayer )
	{
		if( BM::GUID::IsNotNull(pkPlayer->GetPartyGuid()) )
		{
			m_kPartyEventItemMgr.Unregist(pkPlayer->GetPartyGuid(), pkPlayer->GetID());
			m_kPartyEventItemMgr.Check(pkPlayer->GetPartyGuid());
			//m_kCharToParty.Del(pkPlayer->GetID());
		}
	}
}
void PgEventItemSetMgr::Equip(PgPlayer* pkPlayer, int const iItemNo)
{
	if( pkPlayer )
	{
		if( BM::GUID::IsNotNull(pkPlayer->GetPartyGuid())
		&&	IsEventItem(iItemNo) )
		{
			m_kPartyEventItemMgr.Equip(pkPlayer->GetPartyGuid(), pkPlayer->GetID(), iItemNo);
			m_kPartyEventItemMgr.Check(pkPlayer->GetPartyGuid());
		}
	}
}
void PgEventItemSetMgr::Unequip(PgPlayer* pkPlayer, int const iItemNo)
{
	if( pkPlayer )
	{
		if( BM::GUID::IsNotNull(pkPlayer->GetPartyGuid())
		&&	IsEventItem(iItemNo) )
		{
			m_kPartyEventItemMgr.Unequip(pkPlayer->GetPartyGuid(), pkPlayer->GetID(), iItemNo);
			m_kPartyEventItemMgr.Check(pkPlayer->GetPartyGuid());
		}
	}
}
void PgEventItemSetMgr::Tick(PgPlayer* pkPlayer)
{
	if( pkPlayer )
	{
		//
		CONT_EVENT_EFFECT_QUEUE kAddQueue, kDelQueue;
		if( m_kEffectQueueMgr.Check(pkPlayer->GetID(), kAddQueue, kDelQueue) )
		{
			CONT_EVENT_EFFECT_QUEUE::const_iterator add_iter = kAddQueue.begin();
			while( kAddQueue.end() != add_iter )
			{
				BM::Stream kPacket(PT_U_G_RUN_ACTION, static_cast< short >(GAN_AddEffect));
				kPacket.Push( (*add_iter) );
				pkPlayer->VNotify( &kPacket );
				++add_iter;
			}
			CONT_EVENT_EFFECT_QUEUE::const_iterator del_iter = kDelQueue.begin();
			while( kDelQueue.end() != del_iter )
			{
				BM::Stream kPacket(PT_U_G_RUN_ACTION, static_cast< short >(GAN_DelEffect));
				kPacket.Push( (*del_iter) );
				pkPlayer->VNotify( &kPacket );
				++del_iter;
			}
		}
	}
}