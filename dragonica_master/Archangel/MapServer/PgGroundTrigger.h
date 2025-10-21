#ifndef MAP_MAPSERVER_MAP_PGGROUNDTRIGGER_H
#define MAP_MAPSERVER_MAP_PGGROUNDTRIGGER_H

#include "Variant/PgPortalAccess.h"
#include "Variant/PgEmporia.h"
#include "Variant/PgSimpleTime.h"

typedef std::string GTRIGGER_ID;

typedef enum
{
	GTRIGGER_TYPE_NONE,
	GTRIGGER_TYPE_NORMAL,
	GTRIGGER_TYPE_PORTAL,
	GTRIGGER_TYPE_MISSION,
	GTRIGGER_TYPE_INMISSION,
	GTRIGGER_TYPE_PORTAL_EMPORIA,
	GTRIGGER_TYPE_TELEMOVE,
	GTRIGGER_TYPE_BATTLEAREA,
	GTRIGGER_TYPE_MISSION_NPC,
	GTRIGGER_TYPE_MISSION_EVENT_NPC,
	GTRIGGER_TYPE_TRANSTOWER,
	GTRIGGER_TYPE_MISSION_EASY,
	GTRIGGER_TYPE_HIDDEN_PORTAL,
	GTRIGGER_TYPE_SUPER_GROUND,
	GTRIGGER_TYPE_INSUPER_GROUND,
	GTRIGGER_TYPE_PARTY_PORTAL,
	GTRIGGER_TYPE_PARTY_MEMBER_PORTAL,
	GTRIGGER_TYPE_JOB_SKILL,
	GTRIGGER_TYPE_DOUBLE_UP,
	GTRIGGER_TYPE_CHANNELPORTAL,
	GTRIGGER_TYPE_NPCMOVE,
	GTRIGGER_TYPE_KING_OF_HILL,
	GTRIGGER_TYPE_SCORE,
	GARIGGER_TYPE_LOVEFENCE,
	GTRIGGER_TYPE_MISSION_EVENT_NOT_HAVE_ARCADE,
	GTRIGGER_TYPE_RAG_PARTY_MEMBER_PORTAL,
	GTRIGGER_TYPE_CHECK_POINT,
	GTRIGGER_TYPE_MISSION_UNUSED_GADACOIN,
}EGTriggerType;

class PgGround;

class PgGroundTrigger
{
public:
	PgGroundTrigger(void);
	explicit PgGroundTrigger( PgGroundTrigger const & );
	virtual ~PgGroundTrigger(void);

	PgGroundTrigger& operator=( PgGroundTrigger const & );

	GTRIGGER_ID const &GetID(void)const{return m_kID;}
	virtual EGTriggerType const GetType(void)const = 0;
	virtual PgGroundTrigger* Clone()const = 0;

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement ) = 0;
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket ) = 0;
	virtual void AddTimeEvent(TiXmlElement const *pkElement);

	void SetID( GTRIGGER_ID const &kID ){m_kID=kID;}
	bool IsInPos( POINT3 const &pt3Pos )const;
	POINT3 const& Min()const {return m_ptMin;}
	POINT3 const& Max()const {return m_ptMax;}

	virtual void WriteToPacket(BM::Stream& rkPacket) const;

	bool CheckEffectFromPlayer( PgPlayer *pPlayer, PgGround *pGround, BYTE const MoveType );
protected:
	bool Init( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, float const fBuffer = 25.0f );
	virtual bool InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer);

protected:
	GTRIGGER_ID		m_kID;
	POINT3			m_ptMin;
	POINT3			m_ptMax;

	//
	CLASS_DECLARATION_S(bool, Enable);
	CLASS_DECLARATION_S(int, CheckEffectNo);
	CLASS_DECLARATION_S(int, ErrorEffectMsgNo);
	typedef std::vector<SSIMPLETIMELIMIT> CONT_TIME_EVENT;
	CONT_TIME_EVENT m_kContTimeEvent;
};

typedef std::map< GTRIGGER_ID, PgGroundTrigger* >	CONT_GTRIGGER;
typedef std::list<GTRIGGER_ID> CONT_ORDER_CHECKPOINT;


//
class PgGTrigger_Normal : public PgGroundTrigger
{
public:
	PgGTrigger_Normal(void);
	explicit PgGTrigger_Normal(PgGTrigger_Normal const& rhs);
	virtual ~PgGTrigger_Normal(void);

	virtual EGTriggerType const GetType()const	{ return GTRIGGER_TYPE_NORMAL; }
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Normal(*this); }

	virtual bool Build(GTRIGGER_ID const& kID, NiNode* pkTriggerRoot, TiXmlElement const*);
	virtual bool Event(CUnit* pkUnit, PgGround* const pkGround, BM::Stream* const pkPacket);
};


//
class PgGTrigger_Portal
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_Portal(void);
	explicit PgGTrigger_Portal(PgGTrigger_Portal const& rhs);
	virtual ~PgGTrigger_Portal(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_PORTAL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Portal(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
	CONT_PORTAL_ACCESS	m_kContPortal;
};

class PgGTrigger_PartyPortal
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_PartyPortal(void);
	explicit PgGTrigger_PartyPortal(PgGTrigger_PartyPortal const& rhs);
	virtual ~PgGTrigger_PartyPortal(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_PARTY_PORTAL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_PartyPortal(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
	CONT_PORTAL_ACCESS	m_kContPortal;
};

class PgGTrigger_PartyMemberPortal
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_PartyMemberPortal(void);
	explicit PgGTrigger_PartyMemberPortal(PgGTrigger_PartyMemberPortal const& rhs);
	virtual ~PgGTrigger_PartyMemberPortal(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_PARTY_MEMBER_PORTAL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_PartyMemberPortal(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
	CONT_PORTAL_ACCESS	m_kContPortal;
};

class PgGTrigger_RagPartyMemberPortal
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_RagPartyMemberPortal(void);
	explicit PgGTrigger_RagPartyMemberPortal(PgGTrigger_RagPartyMemberPortal const& rhs);
	virtual ~PgGTrigger_RagPartyMemberPortal(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_RAG_PARTY_MEMBER_PORTAL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_RagPartyMemberPortal(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
	CONT_PORTAL_ACCESS	m_kContPortal;
};

//
class PgGTrigger_Mission
	:	public PgGroundTrigger
{
public:
	PgGTrigger_Mission(void);
	explicit PgGTrigger_Mission(PgGTrigger_Mission const& rhs);
	virtual ~PgGTrigger_Mission(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

	bool MissionEvent( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );
	void Set( int const iMissionKey, int const iMissionNo ){m_iMissionKey = iMissionKey, m_iMissionNo = iMissionNo;}

protected:
	int m_iMissionKey;
	int m_iMissionNo;

};


//
class PgGTrigger_Mission_Npc
	:	public PgGTrigger_Mission
{
public:
	PgGTrigger_Mission_Npc(void);
	explicit PgGTrigger_Mission_Npc( PgGTrigger_Mission_Npc const & );
	virtual ~PgGTrigger_Mission_Npc(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION_NPC;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission_Npc(*this); }

};

class PgGTrigger_Mission_Event_Npc
	:	public PgGTrigger_Mission
{
public:
	PgGTrigger_Mission_Event_Npc(void);
	explicit PgGTrigger_Mission_Event_Npc( PgGTrigger_Mission_Event_Npc const & );
	virtual ~PgGTrigger_Mission_Event_Npc(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION_EVENT_NPC;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission_Event_Npc(*this); }

};

class PgGTrigger_Mission_Event_Not_Have_Arcade
	:	public PgGTrigger_Mission
{
	public:
	PgGTrigger_Mission_Event_Not_Have_Arcade(void);
	explicit PgGTrigger_Mission_Event_Not_Have_Arcade( PgGTrigger_Mission_Event_Not_Have_Arcade const & );
	virtual ~PgGTrigger_Mission_Event_Not_Have_Arcade(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION_EVENT_NOT_HAVE_ARCADE;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission_Event_Not_Have_Arcade(*this); }
};

class PgGTrigger_Mission_Easy
	:	public PgGTrigger_Mission
{
public:
	PgGTrigger_Mission_Easy(void);
	explicit PgGTrigger_Mission_Easy( PgGTrigger_Mission_Easy const & );
	virtual ~PgGTrigger_Mission_Easy(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION_EASY;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission_Easy(*this); }


};

//
class PgGTrigger_Mission_Unused_GadaCoin
	:	public	PgGTrigger_Mission
{
public:
	PgGTrigger_Mission_Unused_GadaCoin(void);
	explicit PgGTrigger_Mission_Unused_GadaCoin( PgGTrigger_Mission_Unused_GadaCoin const & );
	virtual ~PgGTrigger_Mission_Unused_GadaCoin(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_MISSION_UNUSED_GADACOIN;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Mission_Unused_GadaCoin(*this); }
};

//
class PgGTrigger_InMission
	:	public PgGroundTrigger
{
public:
	PgGTrigger_InMission(void);
	explicit PgGTrigger_InMission(PgGTrigger_InMission const& rhs);
	virtual ~PgGTrigger_InMission(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_INMISSION;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_InMission(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const /*pkPacket*/ );

protected:
	int m_iType;// 1(next), 2(prev)

};


//
class PgGTrigger_PortalEmporia
	:	public PgGroundTrigger
{
public:
	PgGTrigger_PortalEmporia(void);
	explicit PgGTrigger_PortalEmporia( PgGTrigger_PortalEmporia const & );
	virtual ~PgGTrigger_PortalEmporia(void);

	PgGTrigger_PortalEmporia& operator=( PgGTrigger_PortalEmporia const & );

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_PORTAL_EMPORIA;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_PortalEmporia(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const /*pkPacket*/ );

	SEmporiaKey const &GetEmporiaKey(void)const{return m_kEmporiaKey;}

protected:
	SEmporiaKey			m_kEmporiaKey;

};

//
class PgGTrigger_Hidden_Portal
	:	public PgGroundTrigger
{
public:
	PgGTrigger_Hidden_Portal(void);
	explicit PgGTrigger_Hidden_Portal( PgGTrigger_Hidden_Portal const & );
	virtual ~PgGTrigger_Hidden_Portal(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_HIDDEN_PORTAL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Hidden_Portal(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const /*pkPacket*/ );

protected:
	int	m_iParam;
};
//
class PgGTrigger_TeleMove
	:	public PgGroundTrigger
{
public:
	PgGTrigger_TeleMove(void);
	explicit PgGTrigger_TeleMove(PgGTrigger_TeleMove const& rhs);
	virtual ~PgGTrigger_TeleMove(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_TELEMOVE;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_TeleMove(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );
protected:
	GTRIGGER_ID m_kTargetID;
	POINT3 m_kTargetPos;	//매번 찾을 수가 없잖아. 하나 저장해 두자
	bool m_bIsBossTrigger;

};


//
class PgGTrigger_BattleArea
	:	public PgGTrigger_Normal
{
public:
	PgGTrigger_BattleArea(void);
	explicit PgGTrigger_BattleArea( PgGTrigger_BattleArea const & );
	virtual ~PgGTrigger_BattleArea(void);

	PgGTrigger_BattleArea& operator=( PgGTrigger_BattleArea const & );

	virtual EGTriggerType const GetType()const{return GTRIGGER_TYPE_BATTLEAREA;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_BattleArea(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );

	int GetParam(void)const{return m_iParam;}

protected:
	virtual bool InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer);

protected:
	int	m_iParam;

};

// 전송타워
class PgGTrigger_TransTower
	:	public PgGTrigger_Normal
{
public:
	PgGTrigger_TransTower(void);
	explicit PgGTrigger_TransTower(PgGTrigger_TransTower const& rhs);
	virtual ~PgGTrigger_TransTower(void);

	virtual EGTriggerType const GetType()const	{ return GTRIGGER_TYPE_TRANSTOWER; }
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_TransTower(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

	static bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket, BM::GUID const &kTowerID, __int64 const i64SaveMoney, bool const bByPetSkill = false );

private:
	BM::GUID	m_kTowerID;
	__int64		m_i64SaveMoney;
};

//
class PgGTrigger_SuperGround
	:	public PgGroundTrigger
{
public:
	PgGTrigger_SuperGround(void);
	explicit PgGTrigger_SuperGround(PgGTrigger_SuperGround const& rhs);
	virtual ~PgGTrigger_SuperGround(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_SUPER_GROUND;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_SuperGround(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
	int m_iSuperGroundNo;
	int m_iSpawnNo;
};

//
class PgGTrigger_InSuperGround
	:	public PgGroundTrigger
{
public:	
	typedef std::tr1::shared_ptr<PgPortalAccess> SP_PortalAccess;

public:
	PgGTrigger_InSuperGround(void);
	explicit PgGTrigger_InSuperGround(PgGTrigger_InSuperGround const& rhs);
	virtual ~PgGTrigger_InSuperGround(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_INSUPER_GROUND;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_InSuperGround(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

private:
	int m_iSpawnNo;
	bool m_bMoveAbsolute;
	int m_iNextFloor;

	CONT_PORTAL_ACCESS	m_ContPortal;
	SP_PortalAccess m_spAccess;
};

class PgGTrigger_JobSkill
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_JobSkill(void);
	explicit PgGTrigger_JobSkill(PgGTrigger_JobSkill const& rhs);
	virtual ~PgGTrigger_JobSkill(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_JOB_SKILL;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_JobSkill(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

	int GetJobGrade()const{return iJobGrade;}
protected:
	int			iJobGrade;
};

class PgGTrigger_Double_Up
	:	public	PgGroundTrigger
{
public:
	PgGTrigger_Double_Up(void);
	explicit PgGTrigger_Double_Up(PgGTrigger_Double_Up const& rhs);
	virtual ~PgGTrigger_Double_Up(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_DOUBLE_UP;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_Double_Up(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

protected:
};

// 채널을 바꾸면서 맵이동을 해야할때 사용하는 트리거.
class PgGTrigger_ChannelPortal
	:	public PgGroundTrigger
{
	
public:

	PgGTrigger_ChannelPortal(void);
	explicit PgGTrigger_ChannelPortal(PgGTrigger_ChannelPortal const & rhs);
	virtual ~PgGTrigger_ChannelPortal(void);

	virtual EGTriggerType const GetType() const { return GTRIGGER_TYPE_CHANNELPORTAL; }
	virtual PgGroundTrigger * Clone() const		{ return new_tr PgGTrigger_ChannelPortal(*this); }

	virtual bool Build( GTRIGGER_ID const & kID, NiNode * pTriggerRoot, TiXmlElement const * pElement );
	virtual bool Event( CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket);

protected:

	CONT_PORTAL_ACCESS	m_ContPortal;
};

// 전송 타워
class PgGTrigger_NpcMove
	: public PgGTrigger_Normal
{

public:

	PgGTrigger_NpcMove(void);
	explicit PgGTrigger_NpcMove(PgGTrigger_NpcMove const & rhs);
	virtual ~PgGTrigger_NpcMove(void);

	virtual EGTriggerType const GetType() const { return GTRIGGER_TYPE_NPCMOVE; }
	virtual PgGTrigger_NpcMove * Clone() const { return new_tr PgGTrigger_NpcMove(*this); }

	virtual bool Build(GTRIGGER_ID const &ID, NiNode * pTriggerRoot, TiXmlElement const * pElement);
	virtual bool Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket );

	static bool Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket, BM::GUID const & NpcGuid);

private:

	BM::GUID m_NpcGuid;

};

// PvP 점령전 고지
typedef std::set<std::string> CONT_HILL_LINK;
typedef std::pair<int,int> KOH_ENTITY_KEY;
typedef std::map<int, KOH_ENTITY_KEY > CONT_KOH_DEF_ENTITY_NO;	//first: ..-1,0,1.. 레벨, second: EntityNo, Level
int const MAX_PVP_KOH_ENTITY = 3;
class PgGTrigger_KingOfHill
	:	public PgGroundTrigger
{
public:
	PgGTrigger_KingOfHill(void);
	explicit PgGTrigger_KingOfHill(PgGTrigger_KingOfHill const & rhs);
	virtual ~PgGTrigger_KingOfHill(void);

	virtual EGTriggerType const GetType() const { return GTRIGGER_TYPE_KING_OF_HILL; }
	virtual PgGroundTrigger * Clone() const		{ return new_tr PgGTrigger_KingOfHill(*this); }

	virtual bool Build( GTRIGGER_ID const & kID, NiNode * pkTriggerRoot, TiXmlElement const * pkElement );
	virtual bool Event( CUnit * pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket);

	float GetRadius() const { return m_fRadius; }

	CONT_HILL_LINK GetContLink()const {return m_kContLink;}
	ETeam GetInitEntityTeam()const {return m_eInitEntityTeam;}
	int GetIngEffectNo(ETeam const eTeam)const;
	KOH_ENTITY_KEY GetEntityNo(ETeam const eTeam, int const iLevel)const;

	void SetUnit(CUnit * const pkUnit);
	CUnit * GetUnit()const {return m_pkTriggerUnit;}
	BM::GUID const& GetUnitGuid()const {return m_kTriggerUnitGuid;}

protected:
	virtual bool InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer);

protected:
	float m_fRadius;
	CONT_HILL_LINK m_kContLink;
	ETeam m_eInitEntityTeam;
	int m_iIngRedEffect;
	int m_iIngBlueEffect;
	CONT_KOH_DEF_ENTITY_NO m_kContDefEntityNo;
	CUnit * m_pkTriggerUnit;
	BM::GUID m_kTriggerUnitGuid;
};

//PvP 러브러브 모드 스코어 트리거
class PgGTrigger_Score
	: public PgGroundTrigger
{

public:

	PgGTrigger_Score();
	virtual ~PgGTrigger_Score();

	explicit PgGTrigger_Score(PgGTrigger_Score const & rhs);
	
	virtual EGTriggerType const GetType() const { return GTRIGGER_TYPE_SCORE; }
	virtual PgGroundTrigger * Clone() const		{ return new_tr PgGTrigger_Score(*this); }

	virtual bool Build(GTRIGGER_ID const & ID, NiNode * pTriggerRoot, TiXmlElement const * pElement);
	virtual bool Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket);

public:

	inline int GetTriggerTeamNo() { return TriggerTeamNo; }
	void CheckBearOnTrigger(PgGround * pGnd, VEC_UNIT const & UnitArray);

protected:

	int TriggerTeamNo;
		
};

//PvP 러브러브 모드 울타리 트리거
class PgGTrigger_LoveFence
	: public PgGroundTrigger
{

public:

	PgGTrigger_LoveFence();
	virtual ~PgGTrigger_LoveFence();

	explicit PgGTrigger_LoveFence(PgGTrigger_LoveFence const & rhs);
	
	virtual EGTriggerType const GetType() const { return GARIGGER_TYPE_LOVEFENCE; }
	virtual PgGroundTrigger * Clone() const		{ return new_tr PgGTrigger_LoveFence(*this); }

	virtual bool Build(GTRIGGER_ID const & ID, NiNode * pTriggerRoot, TiXmlElement const * pElement);
	virtual bool Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket);

public:

	int GetFenceObjectNo(void);

protected:
	int FenceObjectNo;
	int FenceObjectTeamNo;
	bool FenceObjectAlive;
};

//달리기 이벤트 체크 포인트
class PgGTrigger_CheckPoint
	:	public PgGroundTrigger
{
public:
	PgGTrigger_CheckPoint(void);
	explicit PgGTrigger_CheckPoint(PgGTrigger_InSuperGround const& rhs);
	virtual ~PgGTrigger_CheckPoint(void);

	virtual EGTriggerType const GetType()const	{return GTRIGGER_TYPE_CHECK_POINT;}
	virtual PgGroundTrigger* Clone()const		{ return new_tr PgGTrigger_CheckPoint(*this); }

	virtual bool Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement );
	virtual bool Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket );

	void SetProgressNo(int iProgressNo) { m_iProgressNo = iProgressNo; }
	int GetProgressNo(void) { return m_iProgressNo; }

private:
	int m_iProgressNo;
};


#endif // MAP_MAPSERVER_MAP_PGGROUNDTRIGGER_H