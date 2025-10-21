#ifndef MAP_MAPSERVER_MAP_GROUND_PGSTATICEVENTGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGSTATICEVENTGROUND_H

#include "BM/GUID.h"
#include <map>
#include "PgGround.h"

enum eBossRegenType
{
	EBRT_RANDOM_REGEN_IN_LIMITTIME			= 0x01,
	EBRT_RANDOM_REGEN_ONCE					= 0x02,
	EBRT_SEQUENTIAL_REGEN_IN_LIMITTIME		= 0x04,
	EBRT_SEQUENTIAL_REGEN_ONCE				= 0x08,
};

class CUnit;
class PgGroundPVPTeamManager
{
public:
	PgGroundPVPTeamManager();
	~PgGroundPVPTeamManager();

	void SetTeam(PgGround* pkGround, CUnit* pkUnit);
	void Cleanup(void);

protected:

private:
	mutable Loki::Mutex m_kMutex;
	typedef std::map<BM::GUID, int> CONT_PARTY_TEAM;
	CONT_PARTY_TEAM m_kContPartyTeam;
	int m_iTeamCount;
};

typedef std::map<BM::GUID, int>	CONT_PARTY_SPAWN_LOC_LIST;

typedef struct tagEventMonsterInfo
{
	POINT3		GenPos;
	DWORD		DelayTime;
	int			MonsterNo;
	int			AdjustLevel;
	int			AdjustHP_Rate;
	int			AdjustDamage_Rate;
	int			AdjustDefence_Rate;
	int			AdjustExp_Rate;
	int			AdjustAttackSpeed;
	int			AdjustEvade;
	int			AdjustHitRate;
	int			RewardItemGroupNo;
	int			RewardItemCount;

	tagEventMonsterInfo()
		: GenPos(0.0f, 0.0f, 0.0f),
		DelayTime(0),
		MonsterNo(0),
		AdjustLevel(0),
		AdjustHP_Rate(0),
		AdjustDamage_Rate(0),
		AdjustDefence_Rate(0),
		AdjustExp_Rate(0),
		AdjustAttackSpeed(0),
		AdjustEvade(0),
		AdjustHitRate(0),
		RewardItemGroupNo(0),
		RewardItemCount(0)
	{

	}
} SEventMonsterInfo;
typedef std::vector<SEventMonsterInfo> VEC_REGEN_EVENT_MONSTER_INFO;
typedef std::vector<VEC_REGEN_EVENT_MONSTER_INFO> CONT_REGEN_EVENT_MONSTER_INFO;

typedef std::list<SEventScheduleData> CONT_EVENT_SCHEDULE;

class PgStaticEventGround : public PgGround
{

public:

	PgStaticEventGround();
	virtual ~PgStaticEventGround();

public:

	virtual EOpeningState Init(int const iMonsterControlID = 0, bool const bMonsterGen = true);
	virtual bool Clone(PgGround * pGround);
	virtual void Clear();
	virtual void OnTick1s();
	virtual bool ReleaseUnit(CUnit * pUnit, bool bRecursiveCall = false, bool const bSendArea = true);
	virtual bool IsUseItem() const { return !UnusableConsumeItem(); }// 소비 아이템을 사용 여부

public:
	
	bool Clone(PgStaticEventGround * pGround);
	EEventGroundState GetEventGroundState() const { return m_eState; }
	void RevivePlayer(void);

	void GMCommandStartEvent(int EventNo);
	void GMCommandEndEvent();

protected:

	virtual bool VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy);
	virtual bool RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket );

	virtual void RecvUnitDie(CUnit * pUnit);
	virtual void SendMapLoadComplete(PgPlayer * pUser);

	virtual void AddJoinPartyMemberList(BM::Stream & Packet);

protected:

	void KickUserFromGround(void);

	void SetState(EEventGroundState const State, bool bAutoChange = false, bool bChangeOnlyState = false);
	void UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime);

	void CheckEventStartable(void);
	bool GenerateGenInfo(int EventNo);
	void GenerateNextMonster(void);

	void SetAutoNextState(DWORD const dwAutoNextStateTime) { m_dwAutoStateRemainTime = dwAutoNextStateTime; }

	void SetPVPTeam(void); //현재 그라운드 내에 전체 유닛의 PVP 팀을 설정
	void RestorePVPTeam(void); //현재 그라운드 내에 전체 유닛의 PVP 팀을 해제

private:

	// 파티별 생성 스폰 위치 리스트.
	CONT_PARTY_SPAWN_LOC_LIST m_PartySpawnLocList;

	// PVP 팀 그룹 매니저. 이벤트(PVP활성상태) 시작시 각 유닛별로 SetTeam을 해주고 이벤트 종료시 Cleanup 해줘야 한다.
	PgGroundPVPTeamManager m_kTeamMgr;

	// 이벤트 그라운드 상태.
	EEventGroundState m_eState;

	// 자동으로 상태가 바뀔때 까지 남은 시간.
	DWORD m_dwAutoStateRemainTime;

	// 생성될 몬스터 리젠 정보를 가지고 있는 리스트.
	CONT_REGEN_EVENT_MONSTER_INFO m_RegenEventMonsterInfo;

	// 이 그라운드에서 진행할 수 있는 이벤트의 스케쥴.
	CONT_EVENT_SCHEDULE m_EventSchedule;

	// 이벤트가 시작될 때 유저들에게 걸어줄 이펙트 번호.
	int m_EventStartEffectNo;

	// 이벤트 제한 시간.
	DWORD m_EventLimitTime;

	// 보스 몬스터는 한번씩만 나오고 마지막 몬스터가 죽으면 이벤트 끝남.
	bool m_bBossRegenOnce;

	// 몬스터 리젠 컨테이너의 최대 인덱스(m_RegenEventMonsterInfo.size()).
	int m_nRegenMaxIdx;

	// 다음에 참조할 몬스터 리젠 컨테이너의 인덱스.
	int m_nRegenCurrentIdx;

	// 다음 몬스터 생성까지 딜레이 타임.
	DWORD m_dwNextMonsterGenDelayTime;

	// 생성한 몬스터 수.
	int m_nRegenMonsterCount;

	int m_nCurrentSpawnLocNum;

	// 이벤트 맵에 접속 중인 유저의 수.
	int m_nCurrentUserCount;

	// 살아 있는 유저의 수. 0이면 이벤트 실패.
	int m_nAlivePlayerCount;

	// 이벤트의 성공, 실패 여부.
	bool m_bSuccessEvent;

	bool m_bSendMonsterRegenWarnning;

	CLASS_DECLARATION_S(int, CurrentEventNo);
	CLASS_DECLARATION_S(bool, IsPK);	// pk option
	CLASS_DECLARATION_S(bool, UnusableReviveItem);	// 부활아이템 사용 가능?
	CLASS_DECLARATION_S(bool, UnusableConsumeItem);	// 소비아이템 사용 가능?

};

#endif // MAP_MAPSERVER_MAP_GROUND_PGSTATICEVENTGROUND_H