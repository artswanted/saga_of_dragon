#ifndef MAP_MAPSERVER_MAP_GROUND_PGSTATICRACEGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGSTATICRACEGROUND_H

#include <list>
#include "PgGround.h"
#include "PgGroundTrigger.h"
#include "PgStaticEventGround.h"

typedef unsigned short EVENTMAP_POS;
typedef unsigned short RACE_TRACK_COUNT;

typedef std::map<int, PgPlayer const*> CONT_RANK_LIST;


template <typename T>
struct map_data_compare : public std::binary_function<typename T::value_type, typename T::mapped_type, bool>
{
public:
	bool operator() (typename T::value_type& pair, typename T::mapped_type i) const
	{
		return pair.second == i;
	}
};


class PgRankingSystem
{
protected:
	typedef std::multimap<DWORD, PgPlayer const*, std::less<DWORD> > CONT_SORTED_RECORD;
	typedef std::map<EVENTMAP_POS, CONT_SORTED_RECORD, std::greater<EVENTMAP_POS> > CONT_RANK_TREE;

	typedef std::map<PgPlayer const*, EVENTMAP_POS> CONT_PLAYERPOS;
	typedef std::map<EVENTMAP_POS, DWORD> CONT_LAPTIME;
	typedef std::map<PgPlayer const*, CONT_LAPTIME> CONT_PLAYER_LAPTIME;

	CONT_RANK_TREE m_kRankTree;
	CONT_PLAYERPOS m_kContPlayerPos;
	CONT_PLAYERPOS m_kContPlayerProgressPos;
	CONT_PLAYER_LAPTIME m_kContPlayerLapTime;

	int m_iTopPosition;

protected:
	bool SetPlayerPos(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos);
	bool SetRankRecord(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos, DWORD dwRec, EVENTMAP_POS kCurPos);

public:
	PgRankingSystem();
	virtual ~PgRankingSystem() {}

	void Init(void);
	EVENTMAP_POS GetPlayerPos(PgPlayer const* pkPlayer);
	EVENTMAP_POS GetPlayerProgressPos(PgPlayer const* pkPlayer);
	bool SetPlayerProgressPos(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos);
	bool SetRecord(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos, DWORD dwRec);
	bool GetRankList(CONT_RANK_LIST& kOutList);
	int GetTopPosition(void) { return m_iTopPosition; }
	DWORD GetCurrentPlayerRecord(PgPlayer const* pkPlayer) const;
	void RemovePlayer(PgPlayer const* pkPlayer);

};


enum ERACE_PROGRESS_STATE : BYTE
{
	ERPS_INCORRECT	= 0,
	ERPS_CORRECT	= 1,
	ERPS_FINISH		= 2,
};

class PgRaceProgress
{
public:
	PgRaceProgress() : m_kTrackCount(0), m_kMaxPos(0) {}
	PgRaceProgress(RACE_TRACK_COUNT kTrackCount, EVENTMAP_POS kMaxPos) : m_kTrackCount(kTrackCount), m_kMaxPos(kMaxPos)
	{}
	virtual ~PgRaceProgress() {}
	virtual BYTE IsCorrectNextPos(PgPlayer const*, EVENTMAP_POS, EVENTMAP_POS) = 0;
	void SetTrack(RACE_TRACK_COUNT kTrackCount, EVENTMAP_POS kMaxPos);
	void InitProgress(void);
	bool IsCompleteRace(PgPlayer const*);
	void RemovePlayer(PgPlayer const*);

protected:
	typedef std::map<PgPlayer const*, RACE_TRACK_COUNT> CONT_RACE_TRACK_POS;

	RACE_TRACK_COUNT m_kTrackCount;
	EVENTMAP_POS m_kMaxPos;
	CONT_RACE_TRACK_POS m_kContTrackPos;
};

class PgRaceBilateralWayProgress : public PgRaceProgress
{
public:
	PgRaceBilateralWayProgress(RACE_TRACK_COUNT kTrackCount, EVENTMAP_POS kMaxPos) : PgRaceProgress(kTrackCount, kMaxPos)
	{}
	virtual ~PgRaceBilateralWayProgress() {}
	virtual BYTE IsCorrectNextPos(PgPlayer const*, EVENTMAP_POS, EVENTMAP_POS);
};



class PgStaticRaceGround : public PgGround
{
public:
	PgStaticRaceGround();
	virtual ~PgStaticRaceGround();

	virtual EOpeningState Init(int const iMonsterControlID = 0, bool const bMonsterGen = true);
	virtual bool Clone(PgGround * pGround);
	virtual void Clear();
	virtual void OnTick1s();
	virtual bool ReleaseUnit(CUnit * pUnit, bool bRecursiveCall = false, bool const bSendArea = true);
	virtual bool IsUseItem() const { return !UnusableConsumeItem(); }// 소비 아이템을 사용 여부
	EEventGroundState GetEventGroundState() const { return m_eState; }
	void GMCommandStartEvent(int EventNo);
	void GMCommandEndEvent();

protected:
	enum ERACERULETYPE
	{
		ERRT_NONE = 0,
		ERRT_BYRANK,
		ERRT_BYTIME,
	};

	typedef struct _tagRANKER_INFO
	{
		std::wstring kName;
		DWORD dwRecord;
		EVENTMAP_POS kCurPos, kCurProgressPos;
		bool bGoalIn;
		PgPlayer const* pkPlayer;
		RESULT_ITEM_BAG_LIST_CONT kContRewardItemList;

		_tagRANKER_INFO() : kName(), dwRecord(0), kCurPos(0), kCurProgressPos(0), bGoalIn(false), pkPlayer(NULL) {}
	}RANKER_INFO;
	typedef std::map<int, RANKER_INFO> CONT_RACE_RESULT; //레이스 최종 순위 결과

protected:

	// 이벤트가 시작될 때 유저들에게 걸어줄 이펙트 번호.
	int m_iEventStartEffectNo;

	// 자동으로 상태가 바뀔때 까지 남은 시간
	DWORD m_dwAutoStateRemainTime;

	// 이벤트 그라운드 상태.
	EEventGroundState m_eState;

	DWORD m_dwLimitTimeValue; //1등이 통화한 뒤 카운트다운 될 시간 초기화 값

	CLASS_DECLARATION_S(int, CurrentEventNo); //현재 이벤트 번호
	CLASS_DECLARATION_S(DWORD, LimitTime); //1등이 통화한 뒤 카운트 감소시킬 시간
	CLASS_DECLARATION_S(DWORD, MaxLimitPlayTime); //내부 체크 시간 (다음 이벤트까지 남은 시간)
	CLASS_DECLARATION_S(bool, UsableSkill); //스킬 사용 여부
	CLASS_DECLARATION_S(ERACERULETYPE, RaceType); //게임 룰. 등수별 보상 or 도착 시간대별 보상
	CLASS_DECLARATION_S(int, TrackAround); //왕복 횟수
	CLASS_DECLARATION_S(int, MpMax); //최대 MP 패널티
	CLASS_DECLARATION_S(bool, FirstGoalIn); //최초 골인 여부
	CLASS_DECLARATION_S(int, CurrentUserCount); //현재 참여중인 인원 수
	CLASS_DECLARATION_S(DWORD, RunningSpeed); // 플레이어에게 강제로 적용될 이동 속도
	CLASS_DECLARATION_S(bool, UnusableConsumeItem);	// 소비아이템 사용 가능?
	CLASS_DECLARATION_S(DWORD, EndWaitTime); //종료 대기시간. 결과창 나올 때 대기 시간
	CLASS_DECLARATION_S(DWORD, EndWaitMomentTime); //플레이-종료 대기시간. PLAY-END로 상태 넘어갈 때 잠시동안의 대기 시간
	int m_iTopPosition;
	DWORD m_dwStartTime;

	DWORD m_ArrRewardItemGroup[5]; //등수 or 시간 구간
	int m_ArrRewardItemBagGroup[5][4]; //그룹별 보상 아이템백

	bool SetRewardItemGroup(int Index, DWORD Value)
	{
		if(Index < 0 || Index > 4) { return false; }
		m_ArrRewardItemGroup[Index] = Value;
		return true;
	}
	DWORD GetRewardItemGroup(int Index)
	{
		if(Index < 0 || Index > 4) { return 0; }
		return m_ArrRewardItemGroup[Index];
	}
	bool SetRewardItemBagGroup(int Low, int Column, int Value)
	{
		if(Low < 0 || Low > 4 || Column < 0 || Column > 3) { return false; }
		m_ArrRewardItemBagGroup[Low][Column] = Value;
		return true;
	}
	int GetRewardItemBagGroup(int Low, int Column)
	{
		if(Low < 0 || Low > 4 || Column < 0 || Column > 3) { return 0; }
		return m_ArrRewardItemBagGroup[Low][Column];
	}


private:
	// 이 그라운드에서 진행할 수 있는 이벤트의 스케쥴.
	CONT_EVENT_SCHEDULE m_EventSchedule;
	PgRankingSystem m_kRankingSystem;
	PgRaceProgress* m_pkRaceProgress;
	CONT_RACE_RESULT m_kContRaceResult;

protected:
	void SetState(EEventGroundState const State, bool bChangeOnlyState = false);
	void SetAutoNextState(DWORD const dwAutoNextStateTime) { m_dwAutoStateRemainTime = dwAutoNextStateTime; }
	void UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime);
	void CheckEventStartable(void);
	void SetGame(int iEventNo, DWORD dwEventDuration, bool bUnusableItem = true);

	bool RewardItem(int iRank, RANKER_INFO& rkRankerInfo);
	void MergeFinalRankList(void);
	void BroadcastFinalResult(bool bSendToCompletePlayer);
	void KickUserFromGround(void);


	DWORD GetCurrentRecordTime(void);
	int GetTriggerIndex(GTRIGGER_ID const& kTrigID);
	bool EnterCheckPoint(GTRIGGER_ID const& kTrigID, PgPlayer* pkPlayer);

	virtual bool VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy);
	virtual void SendMapLoadComplete(PgPlayer * pUser);
	virtual bool VCheckUsableSkill(CUnit* pkUnit, const SActionInfo& rkAction); //그라운드 마다 스킬 사용 유무를 체크하기 위한 함수

	int GetRewardGroupNo(int iRank);

	void BroadcastRanking(bool bFirstGoalIn = false);
	bool GetPlayerInfo(PgPlayer const* pkPlayer, RANKER_INFO& rkOutInfo);
};


#endif //MAP_MAPSERVER_MAP_GROUND_PGSTATICRACEGROUND_H

