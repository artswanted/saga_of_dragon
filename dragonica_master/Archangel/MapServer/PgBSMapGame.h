#ifndef MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSMAPGAME_H
#define MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSMAPGAME_H

#include "BM/TWrapper.h"


//
class PgBSMapGameImpl
	: public PgBSGame
{
public:
	PgBSMapGameImpl();
	explicit PgBSMapGameImpl(BM::Stream& rkPacket);
	~PgBSMapGameImpl();

	__int64 CalcPvpExp(int const iKillerLv, int const iDeadManLv);

	void Set(PgBSMapGameImpl const& rhs);
	void Clear();
	bool CalcReward(bool const bWin, int const iPlayerLevel, int const iPoint, int const iTeamPoint, CONT_BS_REWARD& rkOut) const;
	void MakeBonusReward(int const iWinTeamPlayer, CONT_BS_REWARD& rkContWinTeamBonusReward);
	bool IsBSItem(int const iItemNo) const;
	void WriteToMapUserPacket(BM::Stream& rkPacket) const;

protected:
	__int64 GetNowTime() const;
	static void FindReward(int const iValue, std::list< SBSRewardItem > const& rkRewardTable, CONT_BS_REWARD& rkOut);

private:
	DWORD m_dwPlayerTick; // 포인트 틱
	__int64 m_kStartTime;
	__int64 m_kPreOpenTime;
};

//
class PgBSObserverHelper
{
public:
	typedef std::set< BM::GUID > CONT_BS_OBSERVER_WAITER;
	typedef std::vector< BM::GUID > CONT_BS_OBSERVER_TARGET;
	typedef std::map< BM::GUID, BM::GUID > CONT_BS_OBSERVER_RESULT;

	PgBSObserverHelper();
	~PgBSObserverHelper();

	void Clear();
	void AddTarget(BM::GUID const& rkGuid);
	bool GetTarget(BM::GUID& rkOut) const;
	void DelTarget(BM::GUID const& rkGuid);
	void AddWaiter(BM::GUID const& rkGuid);
	void DelWaiter(BM::GUID const& rkGuid);
	void DelWaiter(ContGuidSet const& rkContGuid);

private:
	CLASS_DECLARATION_S_NO_SET(CONT_BS_OBSERVER_WAITER, ContWaiter); // 대기자
	CLASS_DECLARATION_S_NO_SET(CONT_BS_OBSERVER_TARGET, ContTarget); // 옵저버 가능 대상자
};

//
class PgBSMapGame
{
public:
	PgBSMapGame();
	~PgBSMapGame();

	static const int POINT_PER_ICON = 50;

	// PgBSMapGame
	__int64 CalcPvpExp(int const iKillerLv, int const iDeadManLv);
	void Set(PgBSMapGameImpl const& rhs);
	void Clear();
	void WriteToMapUserPacket(BM::Stream& rkPacket) const;
	void WriteScore(BM::GUID const& kCharGuid, BM::Stream& rkPacket) const;
	void WriteScore(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll = false) const;

	bool CalcReward(BM::GUID const& rkCharGuid, CONT_BS_REWARD& rkOut) const;
	void MakeWinTeamBonusReward(CONT_BS_USER_REWARD& rkContWinTeamBonusReward);
	bool IsBSItem(int const iItemNo) const;
	int GameIDX() const;

	// PgVolatileInvenMng
	void ClearInven();
	size_t PushItem(BM::GUID const& rkOwnerGuid, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff, BM::Stream& rkPacket);
	bool SwapInven(BM::GUID const& rkOwnerGuid, CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkDelBuff, BM::Stream& rkPacket);

	// PgBSTeamMng
	EBS_JOIN_RETURN JoinTeam(CUnit* pkUnit, EBattleSquareTeam const eTeam);
	int GetTeam(BM::GUID const& rkGuid) const;
	int GetWinTeam() const;
	void UpdateBSPoint(ContGuidSet& rkOutGuid);
	void UpdateTeamIconCount();

	// 통합
	void Leave(CUnit* pkUnit);
	void NfyPlayerKill(CUnit* pkKiller, CUnit* pkDeadMan, BM::Stream& rkScorePacket);
	void AddDeadMan(BM::GUID const& rkCharGuid);
	bool Tick1s(DWORD const dwElapsedTime, CONT_BS_HAVE_MINIMAP_ICON& rkOut, ContGuidSet& rkContRevive/*, CONT_BS_MEMBER_POINT& rkMemberPoint*/);
	void SyncPlayerList(BM::Stream& rkPacket) const; // 캐릭터 정보 동기화 (순위 리스트)
	bool ProcessWaiter(ContGuidSet& rkOut); // 대기자 처리
	void SetTeam(CUnit* pkUnit, bool const bIsGame) const;
	bool GetObserverTarget(BM::GUID& rkOut) const;
	void AddObserverWaiter(BM::GUID const& rkOut);
	bool ProcessObserverWaiter(PgBSObserverHelper::CONT_BS_OBSERVER_RESULT& rkContOut);
	void GetTeamPoint(int const iTeam, int& rPoint, int& rIconCount) const;

	//
	int GetMapItemBagNo() const;
	int GetGenGroupGroundNo() const;
	int GetBagControlNo() const;

	// GM 커맨드용
	void AddScore(BM::GUID const& rkCharGuid, int const iPoint, int const iTeamPoint, BM::Stream& rkScorePacket);

	static void AddTeamInfo(BM::GUID const& kGuid, EBattleSquareTeam const eTeam, CONT_BS_HAVE_MINIMAP_ICON& rkOut);

private:
	PgBSMapGame(PgBSMapGame const& rhs);		// 사용 금지
	void operator =(PgBSMapGame const& rhs);	// 사용 금지

private:
	mutable Loki::Mutex m_kMutex;
	PgBSMapGameImpl m_kBSGame;
	PgVolatileInvenMng m_kVIMng;
	PgBSTeamMng m_kTeamMng;
	PgBSTimeEventMgr m_kTimeEvent;
	PgBSObserverHelper m_kBSObserverMode;
};

#endif // MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSMAPGAME_H