#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUE_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUE_H

#include "Lohengrin/PacketStruct2.h"

class PgPvPGroup
{
public:
	explicit PgPvPGroup(BM::GUID const & rkTournamentGuid, int const iGroupIndex);
	~PgPvPGroup();

public:
	void Clear();
	
	bool CreateBattle(int const iTournamentIndex);
	bool AddBattle(int const iTournamentIndex, TBL_DEF_PVPLEAGUE_BATTLE &rkBattleInfo);
	bool AddBattle(BM::GUID const rkBattleGuid, TBL_DEF_PVPLEAGUE_BATTLE const & rkBattleInfo);
	bool GetNextBattle(TBL_DEF_PVPLEAGUE_BATTLE &rkOutBattleInfo) const;

	bool IsEmpty() const;

	bool IsAllAutoWin() const;
	bool IsAutoWin(BM::GUID const &rkTeamGuid) const;

	bool Insert(BM::GUID const &rkTeamGuid);
	bool Find(BM::GUID const rkBattleGuid);

	void SaveBattle(BM::GUID const & rkLeagueGuid, int const iLeagueLevel);

	bool SetBattleResult(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, BM::GUID const rkBattleGuid, int const iWinTeam, int const iNextLeagueLevel);
	void WriteToBattleInfo(CONT_DEF_PVPLEAGUE_BATTLE & rkContBattle) const;

	void AutoWin(int const iNextLeagueLevel, CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, bool bIsSaveDB);

private:
	int m_iGroupIndex;
	BM::GUID m_kTournamentGuid;
	CONT_DEF_PVPLEAGUE_BATTLE m_kContBattle;
};

class PgPvPTournament
{
	typedef std::map< int, PgPvPGroup > CONT_DEF_PVPLEAGUE_GROUP;
public:
	explicit PgPvPTournament(BM::GUID const & rkTournamentGuid);
	explicit PgPvPTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement);
	~PgPvPTournament();

public:
	void Clear();

	bool IsAllAutoWin() const;
	bool IsAutoWin(BM::GUID const &rkTeamGuid) const;
	bool IsAllEmptyBattle() const;

	bool CreateTournament(BM::GUID const & rkLeagueGuid, int const iMaxGroupCount, int const iMaxTeamCount, CONT_DEF_PVPLEAGUE_TEAM const &rkContTeam);
	bool CreateNextTournament(BM::GUID const & rkLeagueGuid, PgPvPTournament& rkTournament, TBL_DEF_PVPLEAGUE_TOURNAMENT const & rkBasicInfo);

	bool AddBattle(int const iGroupIndex, int const iTournamentIndex, TBL_DEF_PVPLEAGUE_BATTLE & rkBattleInfo);
	bool AddBattle(BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkElement);
	void SaveBattle(BM::GUID const & rkLeagueGuid);
	
	bool WriteToBattleInfo(CONT_DEF_PVPLEAGUE_BATTLE& rkContBattle) const;
	bool WriteToBattleInfo(BM::Stream& rkPacket) const;
	bool SetBattleResult(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, BM::GUID const & rkBattleGuidint, int const iWinTeam);

	TBL_DEF_PVPLEAGUE_TOURNAMENT const & GetBasicInfo() const { return m_kBasicInfo; }
	void SetBasicInfo(TBL_DEF_PVPLEAGUE_TOURNAMENT const & rkBasicInfo ) { m_kBasicInfo = rkBasicInfo; }

	BM::GUID const &GetID()const{return m_kGuid;}
	int GetLeagueLevel() const { return m_kBasicInfo.iLeagueLevel; }

	bool WriteTournamentInfo(BM::Stream& rkPacket) const;
	void AutoWin(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, bool bIsSaveDB = true);

private:
	bool CreateGroup(int const iMaxGroupCount);
	bool RegistTeam(CONT_DEF_PVPLEAGUE_TEAM const &rkContTeam);

private:
	CONT_DEF_PVPLEAGUE_GROUP m_kContGroup;
	TBL_DEF_PVPLEAGUE_TOURNAMENT m_kBasicInfo;
	BM::GUID m_kGuid;
};


class PgPvPLeague
{
	typedef enum tagEPvpLeagueLevel
	{	
		EPLL_128			= 1, // 128강 진출
		EPLL_64				= 2,
		EPLL_32				= 3,
		EPLL_16				= 4,
		EPLL_8				= 5,
		EPLL_4				= 6,
		EPLL_2				= 7, // 결승 진출
		EPLL_VICTORY		= 8, // 우승
	}EPVP_LEAGUE_LEVEL;

	typedef enum tagEPvpLeagueString
	{
		EPLS_MAIL_TITLE		= 2000000200, // PVP리그전 상품
		EPLS_MAIL_SENDER	= 2000000201, // PVP리그전 관리자
		EPLS_MAIL_CONTENTS	= 2000000202, // 업적 메일 내용
		EPLS_MAIL_CONTENTS2= 2000000208, // 일반 메일 내용

		EPLS_16			= 2000000203, // 16강 진출
		EPLS_8			= 2000000204,
		EPLS_4			= 2000000205,
		EPLS_2			= 2000000206, // 결승 진출
		EPLS_VICTORY	= 2000000207, // 우승
	}EPVP_LEAGUE_STRING;

	typedef std::map< BM::GUID, PgPvPTournament > CONT_PVPLEAGUE_TOURNAMENT;
public:
	explicit PgPvPLeague(BM::GUID const & rkLeagueGuid, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement);
	~PgPvPLeague();

public:
	void Init();
	bool LoadDB(BM::Stream& rkPacket);
	
	HRESULT Check_RegistTeam(TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo) const;
	bool Check_ReleaseTeam(BM::GUID const & rkTeamGuid);

	HRESULT RegistTeam(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo);
	bool ReleaseTeam(BM::GUID const & rkTeamGuid);
	void InitTeamInfo();

	bool SetJoinStart();
	bool SetJoinEnd();
	bool SetGameReady();
	bool SetGameStart();
	bool SetGameEnd();

	bool NfyMessage(EPvPLeagueMsg const eMessageType) const;

	bool CreateTournament(TBL_DEF_PVPLEAGUE_TIME const & rkBattleTimeInfo, SYSTEMTIME const & rkNowDate);
	bool CheckTournament(SPvPLeague_Event eEvent);
	bool IsAllAutoWin() const;
	bool IsAllEmptyBattle() const;

	bool NfyLobby_ChangeState() const;
	bool ReqLobby_CreateRoom();

	bool WriteLeagueInfo(BM::Stream& rkPacket) const;
	bool SetBattleResult(BM::GUID const & rkBattleGuid, int const iWinTeam);
	bool SendBattleReward();
	
	int GetLeagueLevel() const;
	bool GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM& rkOutInfo) const;
	bool GetTeamID(BM::GUID const & rkCharGuid, BM::GUID& rkOutTeamGuid) const;
	bool IsRegistTeam(BM::GUID const & rkCharGuid) const;
	BM::GUID const &GetID()const{return m_kGuid;}
	int GetState() { return m_kBasicInfo.iLeagueState; }
	void SetState(int iState) { m_kBasicInfo.iLeagueState = iState; }

	void SaveWinTeam();
	void SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid);

private:
	void Clear();
	bool InitTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement);
	bool AddTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement);
	bool SetTournamentInfo(BM::GUID const & rkTournamentGuid, CONT_DEF_PVPLEAGUE_TOURNAMENT::mapped_type const & rkTournamentElement);
	TBL_DEF_PVPLEAGUE GetBasicInfo() { return m_kBasicInfo; }
	bool SendBattleReward(CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & rkElement);
	void GetGroupLevel(int const iTeamCount, int& iGroupCount, int& iLeagueLevel);
	void PostSystemMail(CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & rkElement, int const iLeagueLevel, int const iItemNo, int const iItemCount = 1, EPVP_LEAGUE_STRING const eMailContentsStringNo = EPLS_MAIL_CONTENTS);
private:
	TBL_DEF_PVPLEAGUE m_kBasicInfo;

	CONT_DEF_PVPLEAGUE_TEAM m_kContTeam;
	CONT_PVPLEAGUE_CHAR_TO_TEAM m_kContCharToTeam;
	CONT_PVPLEAGUE_NAME_TO_TEAM m_kContNameToTeam;

	CONT_PVPLEAGUE_TOURNAMENT m_kContTournament;

	BM::GUID m_kGuid;
	int m_iMaxTeamCount;
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGPVPLEAGUE_H