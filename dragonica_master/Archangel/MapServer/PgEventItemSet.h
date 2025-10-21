#ifndef MAP_MAPSERVER_EVENT_PGEVENTITEM_H
#define MAP_MAPSERVER_EVENT_PGEVENTITEM_H

typedef std::set< int > CONT_ITEM_NO;
typedef std::map< int, int > CONT_ITEM_COUNT;
typedef std::set< int > CONT_REWARD_EFFECT_ID;
typedef std::set< int > CONT_EVENT_EFFECT_QUEUE;

////////////////////////////////////////////
// User Equip Info
//
class PgEquipItemInfo
{
public:
	PgEquipItemInfo();
	PgEquipItemInfo(PgEquipItemInfo const& rhs);
	~PgEquipItemInfo();

	void Equip(int const iItemNo);
	void Unequip(int const iItemNo);
	void Unequip(CONT_ITEM_NO const& rkCont);
	int GetEquipCount(int const iItemNo) const;
private:
	CONT_ITEM_COUNT m_kContItemCount;
};

typedef struct tagPlayerEventItemEquipInfo
{
	tagPlayerEventItemEquipInfo();
	tagPlayerEventItemEquipInfo(tagPlayerEventItemEquipInfo const& rhs);
	~tagPlayerEventItemEquipInfo();

	void Equip(int const iItemNo);
	void Unequip(int const iItemNo);

private:
	CLASS_DECLARATION_S_NO_SET(CONT_ITEM_NO, ContItems);
} SPlayerEventItemEquipInfo;

class PgEquipItemMgr
{
	typedef std::map< BM::GUID, SPlayerEventItemEquipInfo > CONT_PLAYER_ITEM_EQUIP_INFO;
public:
	PgEquipItemMgr();
	PgEquipItemMgr(PgEquipItemMgr const& rhs);
	~PgEquipItemMgr();

	void Add(BM::GUID const& rkGuid);
	void Del(BM::GUID const& rkGuid);
	void Equip(BM::GUID const& rkGuid, int const iItemNo);
	void Unequip(BM::GUID const& rkGuid, int const iItemNo);
	int GetEquipCount(int const iItemNo) const		{ return m_kAllEquipInfo.GetEquipCount(iItemNo); }
	bool IsEmpty() const							{ return m_kContEquipInfo.empty(); }
	void GetGuids(VEC_GUID& rkContOut);

private:
	CONT_PLAYER_ITEM_EQUIP_INFO m_kContEquipInfo;
	PgEquipItemInfo m_kAllEquipInfo;
};

////////////////////////////////////////////
// Def
//
typedef struct tagEventItem
{
	tagEventItem();
	tagEventItem(tagEventItem const& rhs);
	explicit tagEventItem(int const& riItemNo, int const& riCount, bool const& rbIsAbsoluteCount);
	~tagEventItem();

	bool operator ==(int const rhs) const;

	int iItemNo;
	int iCount;
	bool bIsAbsoluteCount;
} SEventItem;
typedef std::list< SEventItem > CONT_DEF_BUILDED_EVENT_ITEM;

//
class PgEventItemSet
{
public:
	PgEventItemSet();
	PgEventItemSet(PgEventItemSet const& rhs);
	explicit PgEventItemSet(TBL_DEF_EVENT_ITEM_SET const& rkDef);
	~PgEventItemSet();

	void Regist(TBL_DEF_EVENT_ITEM_SET const& rkDef);

private:
	CLASS_DECLARATION_NO_SET(int, m_iEventID, EventID);
	CLASS_DECLARATION_NO_SET(bool, m_bIsUse, IsUse);
	CLASS_DECLARATION_S_NO_SET(CONT_DEF_BUILDED_EVENT_ITEM, ContItemNo);
	CLASS_DECLARATION_S_NO_SET(CONT_REWARD_EFFECT_ID, ContRewardEffect);
};
typedef std::map< int, PgEventItemSet > CONT_DEF_BUILDED_EVENT_ITEM_SET; // EventID -> Event


/////////////////////////////////////////////////
// party
//
class PgEventEffectQueueMgr;
class PgPartyEquiptItemInfo
{
	typedef std::set< int > CONT_COMPLETE_EVENT_ITEM_SET_ID;

	struct SPreparationEffectData
	{
		SPreparationEffectData(VEC_GUID const& rkContGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID)
			:kContGuid(rkContGuid), kContEffectID(rkContEffectID)
		{
		};
		VEC_GUID kContGuid;
		CONT_REWARD_EFFECT_ID kContEffectID;
	};
	typedef std::vector<SPreparationEffectData> CONT_PREPARATION_EFFECT;

public:
	PgPartyEquiptItemInfo();
	PgPartyEquiptItemInfo(PgPartyEquiptItemInfo const& rhs);
	~PgPartyEquiptItemInfo();

	void Regist(BM::GUID const& rkGuid, CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr);
	void Unregist(BM::GUID const& rkGuid, CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr);
	void Equip(BM::GUID const& rkGuid, int const iItemNo);
	void Unequip(BM::GUID const& rkGuid, int const iItemNo);
	void Check(CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkContEventItemSet, PgEventEffectQueueMgr& rkEffectQueueMgr);
	bool IsEmpty() const			{ return m_kEquipItemMgr.IsEmpty(); }

private:
	CONT_COMPLETE_EVENT_ITEM_SET_ID m_kContCompleteEventID;
	PgEquipItemMgr m_kEquipItemMgr;
};

class PgPartyEquiptItemInfoMgr
{
	typedef std::map< BM::GUID, PgPartyEquiptItemInfo > CONT_PARTY;
public:
	PgPartyEquiptItemInfoMgr(CONT_DEF_BUILDED_EVENT_ITEM_SET const& rkBuildedDef, PgEventEffectQueueMgr& rkEffectQueueMgr);
	PgPartyEquiptItemInfoMgr(PgPartyEquiptItemInfoMgr const& rhs);
	~PgPartyEquiptItemInfoMgr();
	void Clear();
	void Regist(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid);
	void Unregist(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid);
	void Equip(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid, int const iItemNo);
	void Unequip(BM::GUID const& rkPartyGuid, BM::GUID const& rkGuid, int const iItemNo);
	void Check(BM::GUID const& rkPartyGuid);

private:
	CONT_PARTY m_kContPartyInfo;
	CONT_DEF_BUILDED_EVENT_ITEM_SET const& m_rkContEventItemSet;
	PgEventEffectQueueMgr& m_rkEffectQueueMgr;
};

/////////////////////////////////////////////////
// effect queue
//
typedef struct tagEventEffectQueue
{
	tagEventEffectQueue();
	tagEventEffectQueue(tagEventEffectQueue const& rhs);
	~tagEventEffectQueue();

	void AddQueue(int const iEffectID);
	void DelQueue(int const iEffectID);
	bool Empty() const;
	void Swap(CONT_EVENT_EFFECT_QUEUE& rkAddQueue, CONT_EVENT_EFFECT_QUEUE& rkDelQueue);

private:
	CONT_EVENT_EFFECT_QUEUE kAddQueue;
	CONT_EVENT_EFFECT_QUEUE kDelQueue;
} SEventEffectQueue;
typedef std::map< BM::GUID, SEventEffectQueue > CONT_PLAYER_EVENT_EFFECT_QUEUE;

class PgEventEffectQueueMgr
{
public:
	PgEventEffectQueueMgr();
	~PgEventEffectQueueMgr();

	void Clear();
	void AddEffect(BM::GUID const& rkGuid, int const iEffectID);
	void DelEffect(BM::GUID const& rkGuid, int const iEffectID);
	void AddEffect(BM::GUID const& rkGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID);
	void DelEffect(BM::GUID const& rkGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID);
	void AddEffect(VEC_GUID const& rkContGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID);
	void DelEffect(VEC_GUID const& rkContGuid, CONT_REWARD_EFFECT_ID const& rkContEffectID);
	void Add(BM::GUID const& rkGuid);
	void Del(BM::GUID const& rkGuid);
	bool Check(BM::GUID const& rkGuid, CONT_EVENT_EFFECT_QUEUE& rkAddQueue, CONT_EVENT_EFFECT_QUEUE& rkDelQueue);

private:
	CONT_PLAYER_EVENT_EFFECT_QUEUE m_kContEffectQueue;
};

/////////////////////////////////////////////////
// Event mgr
//
//template< typename _T_KEY, typename _T_VALUE >
//class PgKeyToValue
//{
//	typedef std::map< _T_KEY, _T_VALUE > CONT_KEY_TO_VALUE;
//public:
//	PgKeyToValue()
//		: m_kContKeyToValue()
//	{
//	}
//	PgKeyToValue(PgKeyToValue const& rhs)
//		: m_kContKeyToValue(rhs.m_kContKeyToValue)
//	{
//	}
//	~PgKeyToValue()
//	{
//	}
//
//	void Add(_T_KEY const& rkKey, BM::GUID const& rkTarget)
//	{
//		m_kContKeyToValue[rkKey] = rkTarget;
//	}
//	void Del(_T_KEY const& rkKey)
//	{
//		m_kContKeyToValue.erase(rkKey);
//	}
//	bool Get(_T_KEY const& rkKey, _T_VALUE& rkOutValue)
//	{
//		CONT_KEY_TO_VALUE::iterator find_iter = m_kContKeyToValue.find(rkKey);
//		if( m_kContKeyToValue.end() != find_iter )
//		{
//			rkOutValue = (*find_iter).second;
//			return true;
//		}
//		return false;
//	}
//
//private:
//	CONT_KEY_TO_VALUE m_kContKeyToValue;
//};

class PgEventItemSetMgr
{
	//typedef PgKeyToValue< BM::GUID, BM::GUID > PgCharToPartyGuid;
public:
	PgEventItemSetMgr();
	~PgEventItemSetMgr();

	bool Build();
	void Clear();
	bool IsEventItem(int const iItemNo) const;

	void RegistUnit(PgPlayer* pkPlayer);
	void UnregistUnit(PgPlayer* pkPlayer);
	void Equip(PgPlayer* pkPlayer, int const iItemNo);
	void Unequip(PgPlayer* pkPlayer, int const iItemNo);
	void Tick(PgPlayer* pkPlayer);

	//컨텐츠 연결 용
	void JoinPartyUnit(PgPlayer* pkPlayer, BM::GUID const& rkPartyGuid);
	void LeavePartyUnit(PgPlayer* pkPlayer);

private:
	template< typename _T_Mgr >
	void ProcessEquipItem(_T_Mgr& rkMgr, PgInventory* pkInven, EInvType const eType, BM::GUID const& rkKeyGuid, BM::GUID const& rkOwnerGuid)
	{
		if( pkInven )
		{
			CONT_HAVE_ITEM_DATA kContItem;
			pkInven->GetItems(eType, kContItem, true);
			CONT_HAVE_ITEM_DATA::const_iterator iter = kContItem.begin();
			while( kContItem.end() != iter )
			{
				CONT_HAVE_ITEM_DATA::mapped_type const& rkItem = (*iter).second;
				if( false == rkItem.EnchantInfo().IsNeedRepair()
				&&	IsEventItem(rkItem.ItemNo()) )
				{
					rkMgr.Equip(rkKeyGuid, rkOwnerGuid, rkItem.ItemNo());
				}
				++iter;
			}
		}
	}

private:
	bool Verify();

private:
	CONT_DEF_BUILDED_EVENT_ITEM_SET m_kContEvent;
	CONT_ITEM_NO m_kContEventItem;
	PgEventEffectQueueMgr m_kEffectQueueMgr;
	PgPartyEquiptItemInfoMgr m_kPartyEventItemMgr;
	//PgCharToPartyGuid m_kCharToParty;
};

#endif // MAP_MAPSERVER_EVENT_PGEVENTITEM_H