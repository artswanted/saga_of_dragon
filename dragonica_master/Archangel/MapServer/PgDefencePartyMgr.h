#ifndef MAP_MAPSERVER_ACTION_MISSION_PGDEFENCEPARTYMGR_H
#define MAP_MAPSERVER_ACTION_MISSION_PGDEFENCEPARTYMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"

typedef enum eDefence7PartyState
{
	PI_NONE			= 0,	// 미등록 파티
	PI_WAIT			= 1,	// 대기중
	PI_PLAY			= 2,	// 전쟁중
	PI_TYPE_MAX
}EDefence7PartyState;

class PgDefencePartyMgrImpl
{
public:

	PgDefencePartyMgrImpl(){}
	~PgDefencePartyMgrImpl(){}

public:
	void Clear();

	int IsJoinParty(BM::GUID const& kPartyGuid);
	void FindWaitList(VEC_GUID& rkVecGuid);
	bool AddWaitParty(BM::GUID const& kPartyGuid, int eType);
	bool DelWaitParty(BM::GUID const& kPartyGuid);
	bool ModifyWaitParty(BM::GUID const& kPartyGuid, int eType);

	void GetDefencePartyTypeList(VEC_GUID& rkVecGuid, int eType);

private:
	typedef std::map<BM::GUID, int> CONT_DEFENCE_PARTYINFO;
	CONT_DEFENCE_PARTYINFO	m_kDefencePartyInfo;
};

class PgDefencePartyMgr : public TWrapper<PgDefencePartyMgrImpl>
{
public:
	PgDefencePartyMgr(){Clear();}
	~PgDefencePartyMgr(){}

	void Clear();

	int IsJoinParty(BM::GUID const& kPartyGuid);
	void FindWaitList(VEC_GUID& rkVecGuid);
	bool AddWaitParty(BM::GUID const& kPartyGuid, int eType);
	bool DelWaitParty(BM::GUID const& kPartyGuid);
	bool ModifyWaitParty(BM::GUID const& kPartyGuid, int eType);

	void GetDefencePartyTypeList(VEC_GUID& rkVecGuid, int eType);
};

#endif // MAP_MAPSERVER_ACTION_MISSION_PGDEFENCEPARTYMGR_H