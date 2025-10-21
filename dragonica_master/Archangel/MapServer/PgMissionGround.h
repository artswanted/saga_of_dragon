#ifndef MAP_MAPSERVER_MAP_GROUND_PGMISSIONGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGMISSIONGROUND_H

#include "PgIndun.h"
#include "Variant/PgMission.h"
#include "Variant/PgMissionInfo.h"


bool IsDefenceMissionLevelCheck(PgPlayer *pkPlayer, PgGround const *pkGround, int const iMissionNo, int const iMissionLevel);
bool SortyByHP(PgMonster const * rhs, PgMonster const * lhs);	// 몬스터의 HP 순으로 정렬.

class PgMissionGround
	:	public PgIndun
	,	protected PgMission
{
public:
	typedef std::vector<const PgGroundResource*>			ConStageResource;
	typedef std::list<PgMonster*> LIST_MONSTER;
	typedef std::map<int, LIST_MONSTER, std::greater<int> > KillableMonsterList;

	PgMissionGround();
	virtual ~PgMissionGround();

	virtual EOpeningState Init( int const iMonsterControlID=0, bool const bMonsterGen=true );
	virtual void Clear();
	bool Clone( PgMissionGround* pkIndun );
	virtual bool Clone( PgGround* pkGround );

	virtual void OnTick1s();

	virtual EGroundKind GetKind()const{	return GKIND_MISSION;	}
	virtual T_GNDATTR GetAttr()const;
	virtual int GetGroundNo() const; //현재 진행중인 진짜 그라운드 번호
	int GetMissionNo()const	{	return PgMission_Base::GetMissionNo();	}

public:
	virtual void SetState( EIndunState const eState, bool bAutoChange=false, bool bChangeOnlyState=false);

protected:
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual bool RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket );
	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );
	virtual bool LogOut(BM::GUID const &rkCharGUid);
	virtual void GMCommand_RecvGamePoint( PgPlayer *pkPlayer, int const iPoint );
	
    virtual bool CheckApplyUnitOwner(CUnit* pkOwner, BM::GUID& rkOutApplyOwner);
    virtual int GetGiveLevel(CUnit * pkMonster, CUnit * pkOwner);
    virtual void GiveExp(CUnit* pkMonster, CUnit* pkOwner);
	virtual HRESULT SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont );
	virtual bool RecvRecentMapMove( PgPlayer *pkUser );	// 리센트맵 위치로 돌려 보내라!
	virtual void CheckMonsterDieQuest(CUnit* pkCaster, CUnit* pkMonster);
	
	//bool CheckMissionQuestCard(PgPlayer *pkPlayer);
//	bool ProcessMissionQuestCmd(EMissionQuestCommand eCmd, BM::Stream &rkPacket, CUnit *pkUnit);

	bool CheckPlayerDie(CUnit* pkUnit);
	bool CheckPlayerInArea(CUnit* pkUnit, float fRange=100.0f);
	bool CheckMonsterInMap();
    bool IsStrategicMonsterDie(CUnit * pkUnit)const;

	virtual int GetMaxSummonUnitCount(CUnit * pkUnit)const;
	virtual void SendMonsterCount();
    int GetUserAverageLevel();
    int GetTunningLevel();

public:
	bool SetMission(PgMission* pkMission);
	void ReStartMission(PgMission* pkMission,ConStageResource& rkContResource);
	void AddStage( ConStageResource::value_type pkGndResource );
	bool SwapStage_Before( size_t const iStage, CUnit *pkUnit );
	size_t GetStage()const	{	return m_iNowStage;	}
	void SetGMMissionScore(int iScore);
	int GetOwnerLevel();
	bool GetDefenceObjectHpCheck();
	bool IsDefenceMode()const	{	return (m_iModeType == MO_DEFENCE);	}
	bool IsDefenceMode7()const	{	return (m_iModeType == MO_DEFENCE7);	}
	bool IsDefenceMode8()const	{	return (m_iModeType == MO_DEFENCE8);	}
	bool IsDefenceGameMode()const { return (MO_DEFENCE <= m_iModeType); }
    virtual void SetTeam(BM::GUID const& kGuid, int const iTeam);
    virtual int GetTeam(BM::GUID const& kGuid)const;
	virtual bool IsDefenceItemList(int const iItemNo)const;
	virtual bool PushMissionDefenceInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem);
	int GetGuardianStrategicPoint(int const iSlot);
	void SubNeedGuardianStrategicPoint(int const iTeam, int const iGuardian, CUnit * pCaster = NULL);
    void SubNeedGuardianSkillPoint(int const iTeam, int const iGuardianNo, int const iSkillNo);
	bool IsNeedGuardianStrategicPoint(int const iTeam, int const iGuardian);
    bool IsNeedGuardianSkillPoint(int const iTeam, int const iGuardianNo, int const iSkillNo);
	void SetMissionInsertDropItem(POINT3 const& rkPos, int const iItemNo);
	int GetDefence7DropItemNo();
	int GetSkillStrategicPoint(int const iSkillNo);
	bool DelGuardian(std::wstring kName);
	bool IsNeedSkillStrategicPoint(int const iTeam, int const iSkillNo);
	void GM_DefenceNowStage( int const iNowStage );
	void Defence7RelayStage(CUnit * pkUnit, bool const bRemoveGuardian);
	void Defence7PointCopy(PgPlayer * pPlayer);
	bool IsMissionItemLevel(int const iUseLevel);
	void GetContDefenceMissionSkill(CONT_DEFENCE_SKILL & kOut)const;

	void OccupiedPointCopy();	// 포인트 복제기를 누군가 사용했다면, 매스테이지를 클리어 할 때마다 점수 증가.(최대 15회).

	bool IsBossStage() const { return (m_iStageCount == (m_iNowStage + 1)); }	// StageCount는 보스맵, nowstage는 0부터 시작해서 + 1

protected:
    void SendWarnMessageBroadcast(int const iTTNo);
	
	void SendMissionState(CUnit * pkUnit=NULL);

	bool SwapStage( BM::Stream * const pkPacket );
	bool IsLastStage()const	{	return (size_t)(m_iNowStage+1) == m_kGndResounrce.size();	}
	void ClearStage(){	m_kGndResounrce.clear();}//주의 아무데서나 호출하지 마라
	
	void EndMission_Before();
	void EndMission(BM::Stream * const pkPacket);

	EChapterType ChapterCheck(const ContMissionOptionChapter* rkOptionChapter);
	bool GetEffeckCheck(VEC_INT &rkEffect);
	bool GetMonsterCheck(VEC_INT &rkValue, int const iTotalCount);
	bool GetObjectCheck(VEC_INT &rkValue, int const iTotalCount);
	EMissionType MissionCheck(const ContMissionOptionMissionList* rkOptionMission);
	bool GetPortalMissionState();
//	bool GetOptionText(int iLevel, SMissionObject& pkOut);
	bool GetOptionTime(int& iMin, int& iSec);
	bool GetOptionItemTime(int iLevel, int& iMin, int& iSec);
	bool GetOptionScript(int& iScript);
	bool GetOptionErrorText(int& iErrorTTW);
	bool MissionOptionChapterCheck(eMissionOptionKind eType);
	int const CalcClass(int iClass);
	bool MissionStageClearCheck();
	bool MissionCompleteCheck();
	bool GetGadaCoinCount(int& iGadaCoinCount);
	void GetLowClassType(int iClass, VEC_INT & Vec_Class);

	int GetTypeScore(EMissionScoreType iType) const;
	int GetScoreCalc(int const iAdd);
	void AddMissionScore(BM::Stream * const pkPacket);
	void DisplayUpdateUI(EMissionUpdateType iType, int iTotal, BM::GUID const& rkCharGuid = BM::GUID::NullData());
	void Sense_Ability_Sum(int &iTotalScoreUpdate);
	void Sense_Ability_Sub(int &iTotalScoreUpdate);
	bool Sense_ValueUpdate(BM::GUID const& rkCharGuid);
	int GadaCoin_SendItemCheck(BM::GUID const &rkCharGuid);
	void GadaCoin_SendItem(CUnit* pkUnit, int iRetContNo);
	void GadaCoin_UserCheck(BM::GUID const &rkCharGuid);
	bool GetGadaCoinItemRankCheck(BM::GUID const &rkCharGuid);
	void UpdateScore();

	virtual bool IsMacroCheckGround() const;
	int GetTotalScoreUpdateResult();
	EMissionRank GetMissionRank(bool bErrorRank);
	void MissionCountAddExp(int iMissionCount, __int64 &iBonusExp);
	bool GetGadaCoinItemResult(int const iLevel, VEC_INT & Vec_Class, int const iCount, PgBase_Item & kItem);
	bool GetRankItemResult(int const iLevel, int const iRank, PgBase_Item & kItem);
	DWORD GetPartyTimeLimit();

	void DefenceWaveMonsterGenerate(CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type& kElement);

	bool GetDefenceStage(int const iStage, CONT_MISSION_DEFENCE_STAGE_BAG::mapped_type& kElement)const;
	/*template<typename T_Stage, typename T_Key, typename T_Element>
	bool GetDefenceStage(T_Stage const iStage, T_Key kKeyType, T_Element& kElement);*/

	bool GetDefenceWave(int const iWaveGroupNo, int const iWaveNo, CONT_MISSION_DEFENCE_WAVE_BAG::mapped_type& kElement)const;
	EDefenceMsg DefenceNextWaveMonsterGen(int const iNowStage, int const iNowWave);
	void DefenceNextStage();
	void DefenceNextStage(int const iNowStage, int const iNowWave);
	void SetWaveMonster();
	bool IsDefenceNextStage();

	bool GetDefenceAddMonsterStage(int const iStageNo, int const iSelect_SuccessCount, CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type& kElement);
	bool GetDefenceAddMonster(int const iMonsterGroupNo, int const iSelect_SuccessCount, CONT_DEF_DEFENCE_ADD_MONSTER::mapped_type& kElement);
	void ReqUseInfallibleSelection( CUnit *pkOwner, BM::Stream* pkNfy );
	size_t GetNeedSelectItemCount();
	void SetDirection(CUnit *pkOwner, BM::Stream* pkNfy);
	void SelectDirection(CUnit *pkOwner, BM::Stream* pkNfy);
	
	bool IsDefenceLastStage();
	int GetDefenceStageCount();
	bool IsDefenceLastWave();
	void EndDefenceMission();
	void NextStageSend();
	void SetDefenceGroundInfo();
	void AddMonsterEnchant();
	EDefenceMonsterGen GetSelectedDirection();
	void ObjectCreate();
	void ReqUseDefenceTimePlus(CUnit *pkOwner, BM::Stream* pkNfy);
	void SetDefenceTimePlus(CUnit *pkOwner, BM::Stream* pkNfy);
	void SetMonsterAttack(CUnit *pkMonster, int &iObjectAttackMonCount, int &iObjectAttackMonMaxCount);
	bool CloseSelectDirection();
	int GetStageTotalMonsterCount()const;
	void SetStageTimePlus(int const iTimePlus);
	void ReqUseDefenceHpPlus(CUnit *pkOwner, BM::Stream* pkNfy);
	void SetDefenceHpPlus(CUnit *pkOwner, BM::Stream* pkNfy);

	bool GetDefence7Stage(int const iStage, CONT_MISSION_DEFENCE7_STAGE_BAG::mapped_type& kElement)const;
	bool GetDefence7Wave(int const iStage, int const iWaveNo, CONT_MISSION_DEFENCE7_WAVE_BAG::key_type& kKey, CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type& kElement)const;
	void Defence7WaveMonsterGenerate(CONT_MISSION_DEFENCE7_WAVE_BAG::mapped_type& kElement);
	void EndDefence7Mission();
	int GetDefence7WaveStrategicPoint(int const iStage, int const iWaveNo);

	void SetDefenceItemList();
	void SendStrategicPoint(int const iTeam, int const OriValue, int const MulValue, EDefencePointType const eType=DP_NONE);
	void SendToUserItemList(CUnit* pkUnit, VEC_INT& kSendValue);

	virtual void Defence7ItemUse(CUnit* pkUnit, BM::Stream * const pkPacket);
	void GetTeamPartyMember(int const iTeam, VEC_GUID& kGuidVec);
	void Defence7AddEffect(CUnit* pkCaster, CUnit* pkTarget, int const iItemNo, int const iNewEffect);
	bool GetDefence7ItemPos(int const iTeam, int const iIndex, POINT3& rkOutPos)const;
	void SetDropItemView();
	bool GetGuardianGuid(std::wstring kName, BM::GUID& rkGuid);
	bool GetGuardianNo(std::wstring kName, int& iGuardianNo);
	bool IsGuardianPos(std::wstring kName);
	bool SetGuardian(std::wstring kName, int const iGuardianNo, BM::GUID const kGuid);
	bool UpGradeGuardian(CUnit* pkUnit, BM::Stream* pkNfy);
	void RemoveGuardian(CUnit* pkUnit, BM::Stream* pkNfy);
	bool InsertGuardian(CUnit* pkUnit, BM::Stream* pkNfy);
	void GetDefence7GuardianPos(int const iTeam, std::wstring const & kName, POINT3& rkOutPos);
	void Defence7SkillLearn(CUnit* pkUnit, BM::Stream * const pkPacket);
	void AddTeamStrategicPoint(int const iTeam, int& iStrategicPoint, bool const bNotProbability=false);
	void AddStageTeamStrategicPoint();
	void SetStrategicPoint(int const iTeam, int const iStrategicPoint);
	int GetStrategicPoint(int const iTeam)const;
	bool GuardianUpgradeCheck(CUnit* pkUnit, int iGuardianNo, std::wstring kName);
	void SendToFailStageCheck();
	void SendStageTeamPoint();
	void TeamChangeOwner(CUnit* pkUnit);
	void GuardianOwnerChange(CUnit* pkUnit, int const iTeam);
	void SendTeamMonsterGen(bool bLastWave);
	void SendTeamTowerAttackMsg(ETeam const eTeam);
	void CheckMonsterDistanceStrategicPoint();
    void ModifyInstallGuardian();
    HRESULT RealUpgradeGuardian(int const iGuardianNo, std::wstring const & rkTriggerName, CUnit * pkUnit);
	bool IsBonusStage();
	int GetBonusStageTime();

	void DecreaseStrategyFatigueDegree();	// 전략 디펜스 모드에서 스테이지를 시작할 때, 참여중인 플레어어들의 전략피로도를 감소시킨다.
	void CalcBonusExpDefence7();		// 전략 디펜스 모드에서 피로도에 따른 추가 경험치 비율 계산.

	int CaclAccumPoint();
	void AccumPointToStrategyPoint();	// 누적포인트를 전략포인트로 전환(포인트 복제기).
	void KillMonsterByAccumPoint(); // 포인트 복제기로 몬스터 죽임.

protected:
	void ConstellationResultInit();

protected:
	static int const iResultItem = 90000000; // 꽝 처리(실제로 아이템 습득하면 안되기 때문에 보여지기만 한다.)
	static int const GADA_COIN_NO = 20200188;
	static int const DEFENCE_ITEM_USE_TIME = 30000;
	static int const TOWER_NOTI_DELAY = 10000;
	static int const PET_EXP_EFFECT_NO = 80003401;
	static int const DEFENCE_ITEM_INV_COUNT = 4;
	
	virtual void SendMapLoadComplete( PgPlayer *pkUser );

	// Death Delay Time
	DWORD GetChaosDeathDelayTime()const{	return 600000;	}
	virtual DWORD GetDeathDelayTime()const{	return 30000;	}
	virtual DWORD GetResultWaitTime();

	// Stage
	ConStageResource		m_kGndResounrce;
	size_t					m_iNowStage;	

	// Mission Clear
	size_t					m_iEleite;

	// Portal
	bool					m_bPortal;

	// MissionTime
	bool					m_bTimeAttack;
	DWORD					m_dwTotalTime;
	DWORD					m_dwTotalTimeLimit;
	DWORD					m_dwStartTimeLimit;

	// MissionItemTime
	DWORD					m_dwTotalItemTime;

	// GM MissionScore
	CLASS_DECLARATION_S(bool, GMState);
	CLASS_DECLARATION_S(int,  GMScore);

	// Monster
	typedef std::map<int, int> CONT_MON_NUM;
	typedef std::map<int, int> CONT_OBJECT_NUM;
	CONT_MON_NUM			m_kContMonsterNum;
	CONT_OBJECT_NUM			m_kContObjectNum;

	CLASS_DECLARATION_S(int, MonsterTotalCount);
	CLASS_DECLARATION_S(int, MonsterTotalKillCount);
	CLASS_DECLARATION_S(bool, StageClear);
	CLASS_DECLARATION_S(bool, ClassDemage);
	CLASS_DECLARATION_S(bool, PointCopying);	// 포인트 복제기 사용중?
	CLASS_DECLARATION_S(int, PointAccumCount);	// 포인트 누적 회수.
	CLASS_DECLARATION_S(int, AccumPoint);		// 누적 포인트
	CLASS_DECLARATION_S(int, AccumMonsterKillableCount);	// 포인트 복제기로 제거 가능한 몬스터 수.

	// MissionTimeScore Start Time
	DWORD					m_dwTimeScoreStart;
	int						m_iTimeScore;
	int						iTotalScoreUpdate;
	int						iTotalScoreChange;
	int						iTotalOverScore;

	// Defence Mode
	EDefenceMonsterGen		eMonsterGen;

	int						m_iNowWave;
	DWORD					m_dwNextWave_StartDelay;
	DWORD					m_dwNextWave_Delay;
    DWORD                   m_dwCheckReReqWavePacket;

	DWORD					m_dwStage_StartTime;
	DWORD					m_dwStage_Time;

	//DWORD					m_dwStage_StartDelay;
	//DWORD					m_dwStage_Delay;

	bool					m_bDefenceEndStage;

	int						m_iSelect_SuccessCount;
	int						m_iCountSelectItem;
	int						m_iPrevStage_CountSelectItem;
	bool					m_bPrevStage_UseSelectItem;//Clone at Stage StartTime
	bool					m_bSelectSuccess;
	DWORD					m_dwStageExp;

	DWORD					m_dwTowerHpMessageDelay;
	DWORD					m_dwTowerHpMessageDelay1;
	int						m_iStageTimePlus;
	bool					m_bDefenceStageStart;
    bool                    m_bGuardianTunningLevelChange;

	typedef std::map<BM::GUID, BYTE> CONT_USER_DIRECTION;
	CONT_USER_DIRECTION m_kContUserDirection;

	typedef std::map<int, int> CONT_STRATEGIC_POINT;
	CONT_STRATEGIC_POINT	m_kContStrategicPoint;

	CONT_SET_DATA			m_kContDefenceItemList;
	
	typedef std::map<BM::GUID, VEC_INT> CONT_DEFENCE_ITEM;
	CONT_DEFENCE_ITEM		m_kContDefenceItem;

	typedef std::map< std::wstring, SGuardian_Install > CONT_GUARDIAN_INSTALL;
	CONT_GUARDIAN_INSTALL	m_kGuardianInstall;	

	PgDefenceMissionSkill	m_kDefenceMissionSkill;

	typedef std::map<int, BM::GUID> CONT_OBJECT_TEAM;
	CONT_OBJECT_TEAM		m_kObjectTeam;

	typedef std::map<int, int> CONT_OBJECT_HP;
	CONT_OBJECT_HP			m_kObjectHp;
	
	CONT_TEAM_POINT			m_kTeamPoint;
	int						m_iTeamPointCheck;

	bool					m_bDefence_SetWaveMonster;	//MissionOwner가 나갔을때 처리될 수 있도록 하기 위해서 추가
	bool					m_bDefence_NextStage;
	bool					m_bFailMission;
	DWORD					m_dwFailMissionWaitTime;
	bool					m_bGameOwnerStart;
	bool					m_bGameOwnerStartWave;
};

#endif // MAP_MAPSERVER_MAP_GROUND_PGMISSIONGROUND_H