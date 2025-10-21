#ifndef WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENT_H
#define WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENT_H

class PgWorldEvent;
namespace PgWorldEventUtil
{
	extern std::string const kWorldEventElement;

	struct SErrorInfo
	{
		explicit SErrorInfo(PgWorldEvent const* pkWorldEvent, wchar_t const* szFunc, size_t const iLine, BM::vstring const& rkErrorMsg, TiXmlElement const* pkNode = NULL);
		explicit SErrorInfo(SErrorInfo const& rhs);

		BM::vstring m_kErrorMsg;
		wchar_t const* m_szFunc;
		size_t const m_iLine;
	};

	void AddError(SErrorInfo const& rkErrorInfo);
	bool DisplayResult();
}

namespace WorldEventTarget
{
	enum ETargetType // 어떤걸?
	{
		WETT_None			= 0,
		WETT_WorldEvent,			// 월드 이벤트
		WETT_WEClientObject,		// 클라이언트 오브젝트
		WETT_Trigger,				// 그라운드 트리거
		WETT_MonRegenPoint,			// 몬스터 젠 포인트
		WETT_MonRegenPointGroup,	// 몬스터 젠 포인트 그룹
		WETT_ObjectRegenPoint,		// 오브젝트 젠 포인트
		WETT_SummonNPC_RegenPoint,	// 소환 NPC 젠 포인트
		//WETT_RegenPointGroup,
		WETT_Item,					// 아이템
		//WETT_Buff,
		WETT_Quest,
		WETT_MonEffect,				//몬스터 이펙트
		WETT_ObjectRegenPointGroup,	// 오브젝트 젠 포인트 그룹
		WETT_MoveUnit,				//유닛 이동
		WETT_AliveUnit,				//유닛 생존유무
		WETT_UnitLevel,				// 유닛 레벨
		WETT_IndunState,			//그라운드 상태
		WETT_EventGroundState,		// 이벤트 그라운드 상태.
		WETT_UnitGoalTrigger,		// Unit move possition, used for card ground
	};
};

namespace WorldEventCondition
{
	enum EConditionType // 어떻게?
	{
		CT_None			= 0,
		CT_Status,
		CT_Enter,
		CT_Leave,
		CT_Action,
		CT_Equal,		// ==
		CT_Have,		// <=
		CT_Over,		// <
		CT_Less,		// >
		CT_Interval,	// 구간.
		CT_ActAfterTime,
		CT_Ing,
		CT_End,
	};
};

namespace WorldEventAction
{
	enum EActionType // 어떻게?
	{
		AT_None			= 0,
		AT_Status,
		AT_Enable,
		AT_Show,
		AT_Remove,
		AT_Script,
		//AT_Use,
		AT_Active,
	};
};

/// ======================================================
/// == in game class
//
template< typename _EnumType >
struct SWorldEventBase
{
protected:
	SWorldEventBase(_EnumType const Type, BM::vstring const& Value)
		: iType(Type), kValue(Value)
	{}

	SWorldEventBase(SWorldEventBase const& rhs)
		: iType(rhs.iType), kValue(rhs.kValue)
	{}

	void operator =(SWorldEventBase const& rhs)
	{
		iType = rhs.iType;
		kValue = rhs.kValue;
	}

	bool operator ==(SWorldEventBase const& rhs) const
	{
		return (iType == rhs.iType) && (kValue == rhs.kValue);
	}

	int ValueAsInt() const
	{
		return kValue.operator int const();
	}

	DWORD ValueAsDWORD() const
	{
		return kValue.operator DWORD const();
	}

	BM::GUID const ValueAsGuid() const
	{
		return BM::GUID( kValue.operator std::wstring const&() );
	}

	bool ValueAsBool() const
	{
		return 0 != ValueAsInt();
	}

	_EnumType iType;
	BM::vstring kValue;
};


//
template< typename _TargetType >
struct SWorldEventTargetObject : private SWorldEventBase< _TargetType >
{
	typedef typename SWorldEventBase< _TargetType > _MyBase;

	explicit SWorldEventTargetObject(_TargetType const TargetType, BM::vstring const& TargetID)
		: SWorldEventBase(TargetType, TargetID)
	{}
	explicit SWorldEventTargetObject(SWorldEventTargetObject const& rhs)
		: SWorldEventBase(rhs)
	{}

	void operator =(SWorldEventTargetObject const& rhs)		{ _MyBase::operator =(rhs); }
	bool operator ==(SWorldEventTargetObject const& rhs)	{ return _MyBase::operator ==(rhs); }

	_TargetType TargetType() const				{ return _MyBase::iType; }
	BM::vstring const& TargetID() const			{ return _MyBase::kValue; }
	int TargetIDAsInt() const					{ return _MyBase::ValueAsInt(); }
	DWORD TargetIDAsDWORD() const				{ return _MyBase::ValueAsDWORD(); }
	BM::GUID const TargetIDAsGuid() const		{ return _MyBase::ValueAsGuid(); }
};


//
template< typename _ConditionType >
struct SWorldEventConditionType : private SWorldEventBase< _ConditionType >
{
	typedef typename SWorldEventBase< _ConditionType > _MyBase;

	explicit SWorldEventConditionType(_ConditionType const ConditionType, BM::vstring const& ConditionValue)
		: SWorldEventBase(ConditionType, ConditionValue)
	{}
	explicit SWorldEventConditionType(SWorldEventConditionType const& rhs)
		: SWorldEventBase(rhs)
	{}

	void operator =(SWorldEventConditionType const& rhs)	{ _MyBase::operator =(rhs); }
	bool operator ==(SWorldEventConditionType const& rhs)	{ return _MyBase::operator ==(rhs); }

	_ConditionType ConditionType() const				{ return _MyBase::iType; }
	BM::vstring const& ConditionValue() const		{ return _MyBase::kValue; }
	int ConditionValueAsInt() const					{ return _MyBase::ValueAsInt(); }
	DWORD ConditionValueAsDWORD() const				{ return _MyBase::ValueAsDWORD(); }
	BM::GUID const ConditionValueAsGuid() const		{ return _MyBase::ValueAsGuid(); }
	bool ConditionValueAsBool() const				{ return _MyBase::ValueAsBool(); }
};


//
template< typename _ActionType >
struct SWorldEventActionType : private SWorldEventBase< _ActionType >
{
	typedef typename SWorldEventBase< _ActionType > _MyBase;

	explicit SWorldEventActionType(_ActionType const ConditionType, BM::vstring const& ConditionValue)
		: SWorldEventBase(ConditionType, ConditionValue)
	{}
	explicit SWorldEventActionType(SWorldEventActionType const& rhs)
		: SWorldEventBase(rhs)
	{}

	void operator =(SWorldEventActionType const& rhs)		{ _MyBase::operator =(rhs); }
	bool operator ==(SWorldEventActionType const& rhs)		{ return _MyBase::operator ==(rhs); }

	_ActionType ActionType() const					{ return _MyBase::iType; }
	BM::vstring const& ActionValue() const			{ return _MyBase::kValue; }
	int ActionValueAsInt() const					{ return _MyBase::ValueAsInt(); }
	BM::GUID const ActionValueAsGuid() const		{ return _MyBase::ValueAsGuid(); }
	bool ActionValueAsBool() const					{ return _MyBase::ValueAsBool(); }
};


//
//
struct SWorldEventCondition : public SWorldEventTargetObject< WorldEventTarget::ETargetType >, public SWorldEventConditionType< WorldEventCondition::EConditionType >
{
	typedef WorldEventTarget::ETargetType _TargetType;
	typedef SWorldEventTargetObject< _TargetType > _TargetBaseType;
	typedef SWorldEventConditionType< WorldEventCondition::EConditionType > _ConditionBaseType;

	explicit SWorldEventCondition(_TargetType const iTargetType, BM::vstring const& rkTargetID, WorldEventCondition::EConditionType const iConditionType, BM::vstring const& rkConditionValue)
		: _TargetBaseType(iTargetType, rkTargetID), _ConditionBaseType(iConditionType, rkConditionValue), m_kStatus(false)
	{}
	explicit SWorldEventCondition(SWorldEventCondition const& rhs)
		: _TargetBaseType(rhs), _ConditionBaseType(rhs), m_kStatus(rhs.m_kStatus)
	{}

	void operator =(SWorldEventCondition const& rhs)
	{
		_TargetBaseType::operator =(rhs);
		_ConditionBaseType::operator =(rhs);
		m_kStatus = rhs.m_kStatus;
	}
	bool operator ==(SWorldEventCondition const& rhs)
	{
		// not use (m_kStatus == rhs.m_kStatus)
		return _TargetBaseType::operator ==(rhs) && _ConditionBaseType::operator ==(rhs);
	}
	CLASS_DECLARATION_S(bool, Status);
};
typedef std::list< SWorldEventCondition > ContWorldEventCondition;


//
//
struct SWorldEventAction : public SWorldEventTargetObject< WorldEventTarget::ETargetType >, public SWorldEventActionType< WorldEventAction::EActionType >
{
	typedef WorldEventTarget::ETargetType _TargetType;
	typedef SWorldEventTargetObject< _TargetType > _TargetBaseType;
	typedef SWorldEventActionType< WorldEventAction::EActionType > _ActionBaseType;

	explicit SWorldEventAction(_TargetType const iTargetType, BM::vstring const& rkTargetID, WorldEventAction::EActionType const iActionType, BM::vstring const& rkActionValue)
		: _TargetBaseType(iTargetType, rkTargetID), _ActionBaseType(iActionType, rkActionValue)
	{}
	explicit SWorldEventAction(SWorldEventAction const& rhs)
		: _TargetBaseType(rhs), _ActionBaseType(rhs)
	{}
	void operator =(SWorldEventAction const& rhs)
	{
		_TargetBaseType::operator =(rhs);
		_ActionBaseType::operator =(rhs);
	}
	bool operator ==(SWorldEventAction const& rhs)
	{
		return _TargetBaseType::operator ==(rhs) && _ActionBaseType::operator ==(rhs);
	}
};
typedef std::list< SWorldEventAction > ContWorldEventAction;




//
//
//
class PgWorldEventState
{
public:
	explicit PgWorldEventState();
	explicit PgWorldEventState(WORD const& rkID, bool const bEnable);
	explicit PgWorldEventState(PgWorldEventState const& rhs);

	~PgWorldEventState();

	void operator =(PgWorldEventState const& rhs);

	void WriteToPacket(BM::Stream& rkPacket) const;
	bool ReadFromPacket(BM::Stream& rkPacket);

	CLASS_DECLARATION_S_NO_SET(WORD, ID);
	CLASS_DECLARATION_S(bool, Enable);					// 활성화/비활성화
	CLASS_DECLARATION_S_NO_SET(bool, Status);			// 동작/비동작
};


//
//
//
class PgWorldEvent : public PgWorldEventState
{
public:
	using PgWorldEventState::Status;
	explicit PgWorldEvent(WORD const& rkID, bool const bEnable, DWORD const dwResetTime);
	explicit PgWorldEvent(PgWorldEvent const& rhs);
	~PgWorldEvent();


	bool Parse(TiXmlElement const* pkRoot);
	bool Tick(DWORD const dwEleapsedTime, bool& bNextStatus); // true가 리턴이면 상태가 변함
	bool Status(bool const bNewStatus, ContWorldEventAction const*& pkOut, bool const bByAction);
	bool SetCondition(SWorldEventCondition const& rkWhatCondition, bool const bStatus);
	bool CheckCondition() const;

protected:
	bool SetCondition(ContWorldEventCondition& rkContCondition, SWorldEventCondition const& rkWhatCondition, bool const bStatus, bool& bUpdated);
	bool ParseCondition(TiXmlElement const* pkNode);
	bool ParseAction(ContWorldEventAction& rkAction, TiXmlElement const* pkNode);

	void operator =(PgWorldEvent const& rhs);
	
	
	CLASS_DECLARATION_S_NO_SET(DWORD, ResetTime);
	CLASS_DECLARATION_S_NO_SET(DWORD, AccumActivatedTime);
	CLASS_DECLARATION_S_NO_SET(ContWorldEventCondition, AndCondition);
	CLASS_DECLARATION_S_NO_SET(ContWorldEventCondition, OrCondition);
	CLASS_DECLARATION_S_NO_SET(ContWorldEventAction, ActivateAction);
	CLASS_DECLARATION_S_NO_SET(ContWorldEventAction, DeactivateAction);

private:
	mutable Loki::Mutex m_kMutex;
};
typedef std::map< WORD, PgWorldEvent > ContWorldEvent;




//
namespace PgWorldEventMgrUtil
{
	struct SWorldEventLinkedCondition
	{
		SWorldEventLinkedCondition(PgWorldEvent const& rkWorldEvent, SWorldEventCondition const& rkCondition);
		SWorldEventLinkedCondition(SWorldEventLinkedCondition const& rhs);

		PgWorldEvent const& kWorldEvnet;
		SWorldEventCondition const& kCondition;
	};
	typedef std::list< SWorldEventLinkedCondition > ContLinkedCondition;
};



//
class PgWorldEventBaseMgr
{
protected:
	PgWorldEventBaseMgr();
	virtual ~PgWorldEventBaseMgr();

public:
	bool ParseWorldEvent(TiXmlElement const* pkRoot);
	bool GetWorldEvent(WORD const& rkWorldEventID, PgWorldEvent const* &pkOut) const;

protected:
	ContWorldEvent m_kEvent;
};


//
//
//
//
class PgWorldEventMgr : private PgWorldEventBaseMgr
{
	typedef std::map< BM::GUID, PgWorldEventMgrUtil::ContLinkedCondition > ContGenPointToConditionMap;
	typedef std::map< WORD, PgWorldEventMgrUtil::ContLinkedCondition > ContWorldEventToConditionMap;
	typedef std::map< WORD, PgWorldEventMgrUtil::ContLinkedCondition > ContWEClientObjectMap;
	typedef std::map< int,  PgWorldEventMgrUtil::ContLinkedCondition > ContGenPointGroupToContidionMap;
	typedef std::map< int,  PgWorldEventMgrUtil::ContLinkedCondition > ContIndunStateToConditionMap;
	typedef std::map< int,  PgWorldEventMgrUtil::ContLinkedCondition > ContEventGroundStateToConditionMap;
	typedef std::map< BM::GUID, size_t > ContGenPointChildCount;
	typedef std::map< int,  size_t > ContGenPointGroupChildCount;
	typedef std::map< std::string,  PgWorldEventMgrUtil::ContLinkedCondition > ContUnitGoalTarget;

protected:
	PgWorldEventMgr();
	virtual ~PgWorldEventMgr();

	void CloneWorldEvent(PgWorldEventMgr &rTo) const;

public:
	bool ParseWorldEvent(TiXmlElement const* pkRoot)	{ return PgWorldEventBaseMgr::ParseWorldEvent(pkRoot); }
	void BuildWorldEventLinkedData();

protected:
	bool GetWorldEvent(WORD const& rkWorldEventID, PgWorldEvent const* &pkOut) const	{ return PgWorldEventBaseMgr::GetWorldEvent(rkWorldEventID, pkOut); }
	bool SetWorldEventStatus(int const iGroundNo, WORD const& rkWorldEventID, bool const bNextStatus, bool bByAction);
	bool SetWorldEventEnable(WORD const& rkWorldEventID, bool const bEnable);
	bool SetWorldEventConditionStatus(int const iGroundNo, WORD const& rkWorldEventID, SWorldEventCondition const& rkWhatCondition, bool const bStatus);

	void TickWorldEvent(int const iGroundNo, DWORD const dwEleapsedTime);

	virtual void NfyWorldEventToGroundUser(PgWorldEventState const& rkState);
	void WorldEventWriteToPacket(BM::Stream& kPacket) const;

	void DoMonsterEffect(int const iGroundNo, CUnit const * pkUnit);
	void DoMonsterRegenPoint(int const iGroundNo, BM::GUID const& rkMonRegenPointGuid, int const iGenPointGroup, CUnit const*pUnit = NULL);
	void DoObjectRegenPoint(int const iGroundNo, BM::GUID const& rkObjectRegenPointGuid, int const iGenPointGroup, CUnit const*pUnit = NULL);
	void DoWEClientObject(int const iGroundNo, WORD const kWEClientObjectID);
	void DoIndunState(int const iGroundNo, int const iState, CUnit const* pkUnit);
	void DoEventGroundState(int const iGroundNo, int const iState);
	void DoUnitGoalTarget(int const iGroundNo, CUnit const* pkUnit);

	void DoWorldEventCondition(int const iGroundNo, WORD const& rkWorldEventID, CUnit const* pkCaster = NULL);
	void DoWorldEventCondition(int const iGroundNo, ContWorldEventCondition const& rkConditionCont, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster = NULL); // PgGround에서 구현 해야 함
	virtual void DoWorldEventCondition(int const iGroundNo, SWorldEventCondition const& rkCondition, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster = NULL); // PgGround에서 구현 해야 함
	virtual void DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent); // PgGround에서 구현 해야 함

	//
	void IncMonsterGenPointCount(BM::GUID const& rkGenPointGuid);
	void DecMonsterGenPointCount(BM::GUID const& rkGenPointGuid);
	size_t GetMonsterGenPointCount(BM::GUID const& rkGenPointGuid);
	bool IsMonsterGenPointCount(BM::GUID const& rkGenPointGuid);
	//
	void IncMonsterGenPointGroupCount(int const& riGenPointGroupNo);
	void DecMonsterGenPointGroupCount(int const& riGenPointGroupNo);
	size_t GetMonsterGenPointGroupCount(int const& riGenPointGroupNo);
	bool IsMonsterGenPointGroupCount(int const& riGenPointGroupNo);
	//
	void IncObjectGenPointCount(BM::GUID const& rkGenPointGuid);
	void DecObjectGenPointCount(BM::GUID const& rkGenPointGuid);
	size_t GetObjectGenPointCount(BM::GUID const& rkGenPointGuid);
	bool IsObjectGenPointCount(BM::GUID const& rkGenPointGuid);
	//
	void IncObjectGenPointGroupCount(int const& riGenPointGroupNo);
	void DecObjectGenPointGroupCount(int const& riGenPointGroupNo);
	size_t GetObjectGenPointGroupCount(int const& riGenPointGroupNo);
	bool IsObjectGenPointGroupCount(int const& riGenPointGroupNo);

	void ClearGenPointCount();

	bool IsActivateAction(ContWorldEventAction const& rkActionCont, WorldEventTarget::ETargetType eTargetType, WorldEventAction::EActionType eActionType);
	int GetMonRegenPointGroupTotalCount();	
private:
	void DoWorldEventAction(int const iGroundNo, ContWorldEventAction const& rkActionList, PgWorldEvent const& rkWorldEvent);
	void BuildWorldEventLinkedData(PgWorldEvent const& rkWorldEvent, ContWorldEventCondition const& rkConditionList);
	
	template< typename _KeyType, typename _ValType >
	void DoLinkedCondition(std::map< _KeyType, std::list< _ValType > > const& rkContMap, int const iGroundNo, _KeyType const& rkKey, CUnit const*pUnit = NULL);

	template< typename _KeyType, typename _ValType >
	void IncGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey);
	template< typename _KeyType, typename _ValType >
	void DecGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey);
	template< typename _KeyType, typename _ValType >
	size_t GetGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey);
private:
	//mutable Loki::Mutex m_kMutex;
	ContGenPointChildCount m_kMonGenCount;
	ContGenPointGroupChildCount m_kMonGenGroupCount;
	ContGenPointChildCount m_kObjectGenCount;
	ContGenPointGroupChildCount m_kObjectGenGroupCount;

	ContGenPointToConditionMap m_kMonGenToEvent;
	ContGenPointGroupToContidionMap m_kMonGenGroupToEvent;
	ContGenPointToConditionMap m_kObjectGenToEvent;
	ContGenPointGroupToContidionMap m_kObjectGenGroupToEvent;
	ContWorldEventToConditionMap m_kWorldEventToEvent;
	ContWEClientObjectMap m_kWEClientObjectToEvent;
	ContGenPointGroupToContidionMap m_kMonEffectToEvent;
	ContIndunStateToConditionMap m_kIndunStateToEvent;
	ContEventGroundStateToConditionMap m_EventGroundStateToEventMap;
	ContUnitGoalTarget m_UnitGoalTarget;
};

template< typename _KeyType, typename _ValType >
void PgWorldEventMgr::DoLinkedCondition(std::map< _KeyType, std::list< _ValType > > const& rkContMap, int const iGroundNo, _KeyType const& rkKey, CUnit const *pUnit)
{
	typedef std::map< _KeyType, std::list< _ValType > > _MapType;
	typedef _MapType::mapped_type _ListType;
	_MapType::const_iterator find_iter = rkContMap.find( rkKey );
	if( rkContMap.end() == find_iter )
	{
		return;
	}

	_ListType const& rkList = (*find_iter).second;

	
	_ListType::const_iterator loop_iter = rkList.begin();
	while( rkList.end() != loop_iter )
	{
		DoWorldEventCondition(iGroundNo, (*loop_iter).kWorldEvnet.AndCondition(), (*loop_iter).kWorldEvnet, pUnit);
		DoWorldEventCondition(iGroundNo, (*loop_iter).kWorldEvnet.OrCondition(), (*loop_iter).kWorldEvnet, pUnit);
		++loop_iter;
	}
}
template< typename _KeyType, typename _ValType >
void PgWorldEventMgr::IncGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey)
{
	typedef std::map< _KeyType, _ValType > _ContType;
	_ContType::iterator find_iter = rkCont.find(rkKey);
	if( rkCont.end() == find_iter )
	{
		auto kRet = rkCont.insert( std::make_pair(rkKey, 0) );
		if( kRet.second )
		{
			find_iter = kRet.first;
		}
	}
	if( rkCont.end() != find_iter )
	{
		++(*find_iter).second;
	}
}
template< typename _KeyType, typename _ValType >
void PgWorldEventMgr::DecGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey)
{
	typedef std::map< _KeyType, _ValType > _ContType;
	_ContType::iterator find_iter = rkCont.find(rkKey);
	if( rkCont.end() != find_iter )
	{
		if( 0 != (*find_iter).second )
		{
			--(*find_iter).second;
		}
	}
}
template< typename _KeyType, typename _ValType >
size_t PgWorldEventMgr::GetGenPointCount(std::map< _KeyType, _ValType >& rkCont, _KeyType const& rkKey)
{
	typedef std::map< _KeyType, _ValType > _ContType;
	_ContType::iterator find_iter = rkCont.find(rkKey);
	if( rkCont.end() == find_iter )
	{
		return 0;
	}
	return (*find_iter).second;
}

#endif // WEAPON_VARIANT_WORLDEVENT_PGWORLDEVENT_H