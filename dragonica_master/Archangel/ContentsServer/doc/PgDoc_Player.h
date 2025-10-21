#ifndef CONTENTS_CONTENTSSERVER_DOC_PGDOC_PLAYER_H
#define CONTENTS_CONTENTSSERVER_DOC_PGDOC_PLAYER_H

#include "Lohengrin/PacketStruct2.h"
#include "variant/unit.h"
#include "variant/PgPlayer.h"
#include "variant/PgParty.h"
#include "variant/PgUserMapInfo.h"
#include "Variant/ClientOption.h"
#include "PgDoc_PetInfo.h"

typedef enum eQueryCheckerType
{
	QCHK_EXPERANCE		= AT_EXPERIENCE,
	QCHK_EXPERANCE_PET	= AT_PET_INV,//....
	QCHK_MONEY			= AT_MONEY,
	QCHK_FRAN			= AT_FRAN,
	QCHK_SIDEJOB		= AT_HOME_SIDEJOB,
	QCHK_QUEST,
	QCHK_ACHIEVEMENT_MIN = AT_ACHIEVEMENT_DATA_MIN,
	QCHK_ACHIEVEMENT_MAX = AT_ACHIEVEMENT_DATA_MAX,
}EQueryCheckerType;

class PgContLogMgr;

namespace DocPlayerUtil
{
	struct SOrderInfo
	{
		SOrderInfo(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, PgContLogMgr& rkContLogMgr);
		SOrderInfo(SOrderInfo const& rhs);

		DB_ITEM_STATE_CHANGE_ARRAY& m_rkChangeArray;
		SPMO const& m_rkOrder;
		PgContLogMgr& m_rkContLogMgr;
	};
	void CreateQuestEndOrder(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, int const iQuestID, int const iClearQuest, PgContLogMgr& rkContLogMgr, PgMyQuest const* pkMyQuest = NULL);
	void CreateQuestEndOrder(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder, ContQuestID const& rkVec, int const iClearQuest, PgContLogMgr& rkContLogMgr, PgMyQuest const* pkMyQuest = NULL);
}

class PgQueryChecker
{
public:
	static unsigned short CHK_EXP;
	static unsigned short CHK_FRAN;
	static unsigned short CHK_MONEY;

	static bool InitConstantValue(void);

public:
	PgQueryChecker(void);
	virtual ~PgQueryChecker(void);

	void ClearAllChecker();
	
	void ClearChecker( WORD const Type );
	bool IsQuery( WORD const Type )const;
	void AddChecker( WORD const Type );
	unsigned short GetCheckerValue( WORD const Type )const;

protected:
	void SetCheckerMax( WORD const Type );

	void CopyTo(PgQueryChecker& rkChecker)const;

	unsigned short	m_nChk_Exp;
	unsigned short	m_nChk_Exp_Pet;
	unsigned short	m_nChk_Fran;
	unsigned short	m_nChk_Money;
};

//#define _USE_JOBCHECKER
#ifdef _USE_JOBCHECKER
class PgJobChecker;
#endif

struct SStrategySkill
{
	SStrategySkill():iSP(0){}

	int			iSP;
	PgMySkill	kMySkill;	//전략스킬 선택되지 않은것
	PgQuickInventory kQInv;
};

class PgDoc_Player
	: public PgDoc
	, public PgPlayer
	, public PgQueryChecker
{

public:
	PgDoc_Player(void);
	virtual ~PgDoc_Player(void);

	//DOC 은 DB 연관 데이터만 관리 및 수정을 가함.
	//단. DOC 은 HP/MP 관리를 하지 않는다.
public:
	virtual EDocType DocType()const{ return TDOC_PLAYER; }
	virtual void GetQuery( DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray )const;

	virtual bool SetAbil(WORD const Type, int const iValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual bool AddAbil(WORD const Type, int const iValue);
	
	HRESULT AddExp(__int64 const &iAddValue, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);
	HRESULT SetExp(__int64 const iExp, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);
	HRESULT SetTacticsExp(__int64 const iVal, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);
	HRESULT AddTacticsExp(__int64 const iAddVal, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);
	HRESULT AddFran(__int64 const &iAddValue);
	HRESULT SetFran(__int64 const iFran);

	void SetSwitchServer(SERVER_IDENTITY const &kInSwitchServer){m_kNetModule.SetNet( kInSwitchServer );}
	
	//PLAYER_ACTION_INFO* GetPlayerActionInfo() { return &m_kActionInfo; }

	// Mission
	bool UpdateMission( SMissionKey const& kMissionKey, BYTE const kStageFlag ){return m_kMissionReport.Update(kMissionKey,kStageFlag);}
	void WriteToQuery( CEL::DB_QUERY &rkQuery, EDBSaveType const kWriteType=EDBSAVE_DEFAULT ) const;
	virtual void CopyTo(PgDoc_Player & rkPlayer, EWRITETYPE const eType) const;

	// int Quest
	bool AddIngQuest(int const iQuestID);
	bool DelIngQuest(int const iQuestID, bool const bComplete, ContQuestID &rkOut, int &iOut);
	bool SetIngQuest(int const iQuestID, int const iTargetState);
	bool AddIngQuestParam(int const iQuestID, int const iParamNo, int const iParamCount, int const iMaxCount, bool const bSet, int &iResultOut, int &iPreOut);

	// end Quest
	void AddEndQuest(int const iQuestID, bool const bClear);

	//etc quest
	bool BuildLoopQuest(PgContLogMgr &rkLogMgr);
	void RemoveNullQuest(PgContLogMgr &rkLogMgr); // 삭제 예정인 퀘스트를 자동 포기 시킨다.
	void BuildGroupQuest(PgContLogMgr &rkLogMgr);
	bool ProcessBuildLoopQuest(ContQuestID& rkScheduleOut, bool const bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo, int const iFlag);
	void RemoveCoupleQuest(DB_ITEM_STATE_CHANGE_ARRAY& rkChangeArray, SPMO const& rkOrder);

	int BuildGroupQuest(int const iGroupNo);

	void StartRandomQuest();
	void StartTactcisRandomQuest();
	void StartWantedQuest();

	void SetRecentPos(int const iPointType, POINT3 const &ptPos);
	void SetGMLevel( BYTE const byGMLevel ){m_byGMLevel=byGMLevel;}
	void SetPCCafe( const bool bIsPCCafe ){ m_bIsPCCafe = bIsPCCafe; }
	void SetPCCafeGrade( const BYTE byPCCafeGrade ){ m_byPCCafeGrade = byPCCafeGrade; }
	
	HRESULT ItemProcess( SPMO const &kOrder, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);

	HRESULT UpdatePetInfo( BM::GUID const &kPetID, SPetMapMoveData &rkPetMapMoveData, bool const bGetSkillCoolTimeMap );
	PgInventory* GetPetInven( BM::GUID const &kPetID );
	PgDoc_PetInfo* SetDocPetInfo( BM::GUID const &kPetID, PgInventory &kPetInv );
	PgDoc_PetInfo* LoadDocPetInfo( BM::GUID const &kPetID, bool const bIsLoadDB );
	void RemovePetInfo( BM::GUID const &kPetID );

	void UpdateModifyAchievementsAbil(WORD const wAbilType);
	void OnAchievementAbilSaveDB(DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray);

	void StrategySkillInit(SPlayerStrategySkillData const & kData);
	void StrategySkillSetInit(CONT_USER_SKILLSET const & kContSkillSet, CONT_USER_SKILLSET const & kContSkillSet_Sec);
	bool SwapMySkill(ESkillTabType const eTabType);
	void SetStrategyMySkill(PgMySkill const * pkMySkill);
	PgMySkill* GetStrategyMySkill() { return &m_kStrategySkill.kMySkill; }
	PgMySkill const* GetStrategyMySkill() const { return &m_kStrategySkill.kMySkill; }
	PgQuickInventory const* GetStrategyQInv() const { return &m_kStrategySkill.kQInv; }
	void SetStrategySP(int iValue) { m_kStrategySkill.iSP = iValue; }
	WORD GetStrategySP()const { return static_cast<WORD>(m_kStrategySkill.iSP); }

	void SendToAchievementMgr(short const Level);
public:
	void GetWorldMap( PgUserMapInfo<DWORD>& rkWorldMapFlag )const { rkWorldMapFlag = m_kWorldMapFlag; }
	void SetWorldMap( PgUserMapInfo<DWORD> const &kWorldMapFlag ){m_kWorldMapFlag=kWorldMapFlag;}
	void SetExternDB( SPvPReport const &kPvPReport, PgPlayer_MissionReport const &kMissionReport );

protected:
	void CopyAbilFromClassDef();

	bool BuildDayLoopQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo);
	bool BuildWeekLoopQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, DocPlayerUtil::SOrderInfo* pkOrderInfo);
	bool BuildRandomQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo);
	bool BuildTacticsRandomQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo);
	bool BuildWantedQuest(BM::PgPackedTime const& rkFailPrevTime, BM::PgPackedTime const& rkNextTime, bool bForce, DocPlayerUtil::SOrderInfo* pkOrderInfo);
	bool BuildScheduledQuest(BM::PgPackedTime const& rkNowTime, ContQuestID& rkOut, DocPlayerUtil::SOrderInfo* pkOrderInfo);

protected:
	CONT_DOC_PET_INFO		m_kContDocPetInfo;// 한번이라도 장착했던 Pet의 정보
	CONT_ACHIEVEMENT_ABIL	m_kContModifyAchievementAbil; // 수정된 업적 어빌
	mutable SStrategySkill			m_kStrategySkill;

#ifdef _USE_JOBCHECKER
	PgJobChecker* m_pkJobChecker;
public:
	void AddJobChecker( __int64 const iType );
	void DisplayJobChecker()const;
	void CreateJobChecker();
#endif
public:
	ClientOption			m_kClientOption;
};
typedef std::map< SModifyOrderOwner, PgDoc_Player* > CONT_DOC_PLAYER;

class PgDoc_MyHome : public PgDoc_Player
{
	CLASS_DECLARATION_S(BM::GUID,OwnerGuid);
	CLASS_DECLARATION_S(bool,IsInSide);
	CLASS_DECLARATION_S(std::wstring,OwnerName);

public:
	
	PgDoc_MyHome():m_kIsInSide(false){}
	virtual ~PgDoc_MyHome(){}

public:

	virtual void CopyTo(PgDoc_Player & rkPlayer, EWRITETYPE const eType) const;
	virtual EUnitType UnitType()const{return UT_MYHOME;}
	virtual EDocType DocType()const{return TDOC_MYHOME;}
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
};

#endif // CONTENTS_CONTENTSSERVER_DOC_PGDOC_PLAYER_H