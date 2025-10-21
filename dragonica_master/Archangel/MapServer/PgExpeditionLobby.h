#ifndef MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONLOBBYGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONLOBBYGROUND_H

#include "PgGround.h"
#include "Variant/PgConstellation.h"

class PgExpeditionLobby
	: public PgGround
{
public:

	typedef std::map< BM::GUID, BM::GUID > JoinExpeditionUserList; // first : CharGuid, second : ExpeditoinGuid

	PgExpeditionLobby();
	virtual ~PgExpeditionLobby();

	virtual EOpeningState Init(int const iMonsterControlID = 0, bool const bMonsterGen = true);
	bool Clone(PgExpeditionLobby * pGround);
	virtual bool Clone(PgGround * pGround);
	virtual void Clear();
	virtual void OnTick1s();

	virtual EGroundKind GetKind() const { return GKIND_EXPEDITION_LOBBY; }
	virtual T_GNDATTR GetAttr() const;
	virtual int GetGroundNo() const;

	bool DelJoinExpeditionWaitList(BM::GUID const & CharGuid);
	
	void CheckHaveKeyItem(BM::GUID ExpeditionGuid, VEC_GUID & CharList, VEC_GUID & OutList);

protected:

	virtual bool VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy);
	virtual bool RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket );

	bool AddJoinExpeditionWaitList(BM::Stream & Packet);
	void CheckJoinExpedition(CUnit * pUnit);

	HRESULT IsStartableExpedition(BM::GUID const & ExpeditionGuid, BM::GUID const & NpcGuid, VEC_GUID & Vec_Fail);
	HRESULT Recv_PT_C_M_REQ_NPC_ENTER_EXPEDITION(CUnit * pUnit, BM::Stream & Packet, VEC_GUID & Vec_Fail);
	void Recv_PT_C_M_REQ_LIST_USER_EXPEDITION(CUnit * pUnit, BM::Stream & Packet);

private:

	JoinExpeditionUserList	m_JoinExpeditionWaitList;

};


#endif //MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONLOBBYGROUND_H