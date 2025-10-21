#ifndef WEAPON_VARIANT_PGNPCEVENT_H
#define WEAPON_VARIANT_PGNPCEVENT_H

#include <boost/tr1/memory.hpp>

class CUnit;

typedef enum NpcEventConditionType
{
	NECT_NONE			= 0,
	NECT_QUEST			= 1,
	NECT_ITEM			= 2,
	NECT_EFFECT			= 3,
};

typedef enum ENpcEvent_Quest_State
{
	NEQS_NONE			= 0,
	NEQS_NOT_ACCEPT		= 0, //수락하지 않은 상태
	NEQS_ACCEPT			= 1, //수락한 상태
	NEQS_COMPLETE		= 2, //완료한 상태
};

typedef enum ENpcEvent_Item_Pos
{
	NEIE_NONE				= 0,
	NEIE_INVENTORY			= 0, //인벤토리
	NEIE_EQUIP_SLOT			= 1, //착용슬롯
	NEIE_SAFE				= 2, //개인금고
	NEIE_SHARE_RENTAL_SAFE	= 3, //계정금고
};

typedef enum ENpcEventOperatorType : BYTE
{
	NEO_NONE	= 0,
	NEO_AND		= 1,
	NEO_OR		= 2,
};

class PgNpcEventCondition
{
public:
	PgNpcEventCondition() : m_eConditionType(NECT_NONE), m_eOperator(NEO_NONE)
	{
	}

	virtual bool Parse(TiXmlElement const* pkRoot);
	bool CommonParse(std::string const& kName, std::string const& kValue);

	virtual bool Check(CUnit * pkUnit);

	ENpcEventOperatorType Operator()const { return m_eOperator; }

private:
	NpcEventConditionType m_eConditionType;
	ENpcEventOperatorType m_eOperator;
};

class PgNpcEventCondition_Quest : public PgNpcEventCondition
{
public:
	PgNpcEventCondition_Quest() : m_iQuestNo(0), m_eState(NEQS_NONE)
	{
	}

	virtual bool Parse(TiXmlElement const* pkRoot);
	
	virtual bool Check(CUnit * pkUnit);

private:
	int m_iQuestNo;
	ENpcEvent_Quest_State m_eState;
};

class PgNpcEventCondition_Item : public PgNpcEventCondition
{
public:
	PgNpcEventCondition_Item() : m_iItemNo(0), m_bIsHave(true), m_ePosType(NEIE_NONE)
	{
	}

	virtual bool Parse(TiXmlElement const* pkRoot);
	
	virtual bool Check(CUnit * pkUnit);

private:
	int m_iItemNo;
	bool m_bIsHave;
	ENpcEvent_Item_Pos m_ePosType;
};

class PgNpcEventCondition_Effect : public PgNpcEventCondition
{
public:
	PgNpcEventCondition_Effect() : m_iEffectNo(0), m_bIsHave(true)
	{
	}

	virtual bool Parse(TiXmlElement const* pkRoot);
	
	virtual bool Check(CUnit * pkUnit);

private:
	int m_iEffectNo;
	bool m_bIsHave;
};

class SNpcEventAction
{
public:
	SNpcEventAction() : m_bHidden(false)
	{}
	
	SNpcEventAction(BM::GUID const& rkNpcGuid, bool const bHidden)
		: m_kNpcGuid(rkNpcGuid), m_bHidden(bHidden)
	{}
	
	~SNpcEventAction()
	{}

	BM::GUID const& GetGuid()const { return m_kNpcGuid; }
	bool IsHidden()const { return m_bHidden; }

private:
	BM::GUID m_kNpcGuid;
	bool m_bHidden;
};

typedef std::tr1::shared_ptr<PgNpcEventCondition> SP_NpcEventCondition;
typedef std::vector< SP_NpcEventCondition > CONT_NPC_EVENT_AND_CONDITION;
typedef std::vector< CONT_NPC_EVENT_AND_CONDITION > CONT_NPC_EVENT_OR_CONDITION;
typedef std::list< SNpcEventAction > CONT_NPC_EVENT_ACTION;

class PgNpcEvent
{
public:
	explicit PgNpcEvent(WORD const& rkID);
	explicit PgNpcEvent(PgNpcEvent const& rhs);
	~PgNpcEvent();

	bool Parse(TiXmlElement const* pkRoot);
	bool IsHiddenNpc(CUnit const* pkNpc, CUnit const* pkUnit)const;
	bool CheckCondition(CUnit * pkUnit);

protected:
	void operator =(PgNpcEvent const& rhs);

	bool ParseCondition(TiXmlElement const* pkNode);
	bool ParseAction(TiXmlElement const* pkNode, CONT_NPC_EVENT_ACTION& rkAction);
	
	CLASS_DECLARATION_S(WORD, ID);
	CLASS_DECLARATION_S_NO_SET(CONT_NPC_EVENT_OR_CONDITION, OrCondition);
	CLASS_DECLARATION_S_NO_SET(CONT_NPC_EVENT_ACTION, ActivateAction);
	CLASS_DECLARATION_S_NO_SET(CONT_NPC_EVENT_ACTION, DeactivateAction);
};
typedef std::map< WORD, PgNpcEvent > CONT_NPC_EVENT;
typedef std::map< int, CONT_NPC_EVENT > CONT_MAP_NPC_EVENT;
typedef std::map< WORD, CONT_NPC_EVENT_ACTION const* > CONT_ACTIVATE_NPC_EVENT_ACTION;


class PgNpcEventMgr
{
public:
	PgNpcEventMgr();
	~PgNpcEventMgr();


	bool Parse(TiXmlElement const* pkRoot, int const iMapNo);
	bool Check(int const iMapNo, CUnit * pkUnit, CONT_ACTIVATE_NPC_EVENT_ACTION & kContAction);
	bool IsHiddenNpc(int const iMapNo, BM::GUID const& rkNpcGuid, CUnit * pkUnit);
	bool IsHiddenNpc(BM::GUID const& rkNpcGuid, CONT_ACTIVATE_NPC_EVENT_ACTION const& rkContAction);

private:
	CONT_MAP_NPC_EVENT m_kContMapNpcEvent;
};

#define g_kNpcEventMgr SINGLETON_STATIC(PgNpcEventMgr)


#endif // WEAPON_VARIANT_PGNPCEVENT_H
