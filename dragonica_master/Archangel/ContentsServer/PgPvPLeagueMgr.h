#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUEMGR_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUEMGR_H

#include "bm/twrapper.h"
#include "PgPvPLeague.h"

namespace PgPvPLeagueUtil
{
	bool Q_DQT_LOAD_DEF_PVPLEAGUE_TIME(CEL::DB_RESULT& rkResult);
	bool Q_DQT_LOAD_DEF_PVPLEAGUE_SESSION(CEL::DB_RESULT& rkResult);
	bool Q_DQT_LOAD_DEF_PVPLEAGUE_REWARD(CEL::DB_RESULT& rkResult);
	bool Q_DQT_LOAD_PVPLEAGUE(CEL::DB_RESULT &rkResult);
	bool Q_DQT_LOAD_PVPLEAGUE_SUB(CEL::DB_RESULT &rkResult);
	bool Q_DQT_INSERT_PVPLEAGUE(CEL::DB_RESULT &rkResult);
	bool Q_DQT_INSERT_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult);
	bool Q_DQT_DELETE_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult);
	bool Q_DQT_INSERT_PVPLEAGUE_TOURNAMENT(CEL::DB_RESULT &rkResult);
	bool Q_DQT_INSERT_PVPLEAGUE_BATTLE(CEL::DB_RESULT &rkResult);
	bool Q_DQT_SET_PVPLEAGUE_BATTLE(CEL::DB_RESULT &rkResult);
	bool Q_DQT_SET_PVPLEAGUE_STATE(CEL::DB_RESULT &rkResult);
	bool Q_DQT_SET_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult);
};

class PgPvPLeagueImpl
{
	typedef std::map<BM::GUID, PgPvPLeague> CONT_LEAGUE;
	typedef std::queue<SPvPLeague_Event> CONT_EVENT;
public:
	PgPvPLeagueImpl(void);
	~PgPvPLeagueImpl(void);

	void OnTick();
	bool ProcessMsg(BM::Stream& rkPacket);

	//League
	bool CheckNewLeage(CONT_DEF_PVPLEAGUE const & rkContLeagueInfo, BM::GUID const & rkLeagueGuid) const;
	bool AddLeague(CONT_DEF_PVPLEAGUE& rkContLeague);
	bool AddLeague(BM::GUID const & rkLeagueID, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement);
	bool InitLeague(BM::GUID const & rkLeagueID);
	void WriteLeagueInfo(BM::Stream &rkPacket) const;
	bool GetCurLeagueID(BM::GUID &rkLeagueGuid) const;

	//DB
	bool LoadDB(BM::GUID const & rkLeagueGuid, BM::Stream& rkPacket);

	//Team
	bool GetTeamID(BM::GUID const & rkUserGuid, BM::GUID &rkOutTeamGuid) const;
	bool GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM &rkOutTeamInfo);
	HRESULT RegistTeam(BM::GUID const &rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo);
	bool ReleaseTeam(BM::GUID const &rkTeamGuid);
	bool SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid);

	//battle
	bool SetBattleResult(BM::GUID const rkBattleGuid, int const iWinTeam);
	void EndBattle();

	//Broadcast
	void SendToUser(CONT_LEAGUE_USER const &rkContUser, BM::Stream const &rkPacket) const;
	void SendToTeam(BM::GUID const & rkTeamGuid, BM::Stream const &rkPacket) const;

	bool NfyMessage(EPvPLeagueMsg const eMessageType) const;
	bool NfyLobby_ChangeState() const;


	//Event
	void ProcessEvent(ePvPLeagueEvent eEvent);

protected:
	void Clear();

	bool BuildTime();	

	PgPvPLeague* GetLeague(BM::GUID const & rkLeagueGuid);
	PgPvPLeague const* GetLeague(BM::GUID const & rkLeagueGuid) const;
	
	bool GetNextBattleTime(int const iLeagueLevel, TBL_DEF_PVPLEAGUE_TIME& rkOutInfo) const;
	bool IsJoinEndDay(WORD const wDayOfWeek) const;

private:
	CONT_LEAGUE m_kContLeague;

	CONT_EVENT m_kContEvent;
	BM::GUID m_kCurLeagueID;
	SYSTEMTIME m_kDateTime;
};


class PgPvPLeagueMgr
	:	public TWrapper< PgPvPLeagueImpl >
{
public:
	PgPvPLeagueMgr(void);
	virtual ~PgPvPLeagueMgr(void);

	void OnTick();
	bool ProcessMsg(BM::Stream& rkPacket);

private:
	bool CheckNewLeage(CONT_DEF_PVPLEAGUE const & rkContLeagueInfo, BM::GUID const & rkLeagueGuid) const;
	bool AddLeague(CONT_DEF_PVPLEAGUE& rkContLeague);
	bool AddLeague(BM::GUID const & rkLeagueID, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement);
	bool InitLeague(BM::GUID const & rkLeagueID);
	bool LoadDB(BM::GUID const & rkLeagueGuid, BM::Stream& rkPacket);
	void WriteLeagueInfo(BM::Stream &rkPacket) const;
	bool GetCurLeagueID(BM::GUID &rkLeagueGuid) const;
	
	bool GetTeamID(BM::GUID const & rkUserGuid, BM::GUID &rkOutTeamGuid) const;
	bool GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM &rkOutTeamInfo);
	HRESULT RegistTeam(BM::GUID const &rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo);
	bool ReleaseTeam(BM::GUID const &rkTeamGuid);
	bool SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid);

	bool SetBattleResult(BM::GUID const rkBattleGuid, int const iWinTeam);
	void EndBattle();

	void SendToUser(CONT_LEAGUE_USER const &rkContUser, BM::Stream const &rkPacket);
	void SendToTeam(BM::GUID const & rkTeamGuid, BM::Stream const &rkPacket);
	bool NfyMessage(EPvPLeagueMsg const eMessageType) const;
	void NfyLobby_ChangeState() const;

	void ProcessGMCommand(BM::Stream& rkPacket);
	void ProcessEvent(ePvPLeagueEvent eEvent);
private:
	mutable Loki::Mutex m_kMutex;
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUEMGR_H