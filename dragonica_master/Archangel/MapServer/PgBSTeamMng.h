#ifndef MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTEAMMNG_H
#define MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTEAMMNG_H

//
class PgBSTeamMng
{
	typedef std::map< BM::GUID, EBattleSquareTeam > CONT_BS_CHAR_TEAM;
public:
	PgBSTeamMng();
	~PgBSTeamMng();

	bool IsReseve(BM::GUID const& rkCharGuid) const;
	EBS_JOIN_RETURN Reserve(BM::GUID const& rkCharGuid, int const iMaxMemberCount);
	EBS_JOIN_RETURN Join(CUnit* pkUnit, int const iMaxMemberCount, EBattleSquareTeam const eTeam);
	void Leave(CUnit* pkUnit);
	void ProcessWaiter(int const iMaxMemberCount, ContGuidSet& rkOut);
	void SetTeam(CUnit* pkUnit, bool const bIsGame) const;
	void Clear();

	void WriteToPacket(BM::Stream& rkPacket) const;
	void WriteToScorePacket(ContGuidSet const& rkContGuid, BM::Stream& rkPacket, bool const bSyncAll = false) const;

	bool GetMember(BM::GUID const& rkGuid, SBSTeamMember& rkOut) const;
	int GetTeam(BM::GUID const& rkGuid) const;
	int GetTeamPoint(BM::GUID const& rkGuid) const;
	void AddScore(BM::GUID const& rkGuid, int const iPoint, int const iKill, int const iDead);
	void AddTeamPoint(BM::GUID const& rkGuid, int const iPoint);
	int GetWinTeam() const;
	int GetTeamMemberCount(int const iTeamNo) const;
	void GetTeamMemberGuid(int const iTeamNo, VEC_GUID &rkOutCont) const;
	void SetTeamIconCount(int const iTeamNo, int const iIconCount);
	void GetTeamPoint(int const iTeam, int& rPoint, int& rIconCount) const;
	void IncreaseIcon(BM::GUID const& rkGuid);
	void DropAllIcon(BM::GUID const& rkGuid);

private:
	EBattleSquareTeam GetTeamType(BM::GUID const& rkCharGuid) const;
	void ProcessWaiter(PgBSTeam& rkTeam, int iProcessManCount, ContGuidSet& rkOut);

private:
	PgBSTeam m_kTeamBlue;
	PgBSTeam m_kTeamRed;
	CONT_BS_CHAR_TEAM m_kContCharTeam;
};

#endif // MAP_MAPSERVER_OBJECT_BATTLESQUARE_PGBSTEAMMNG_H