#ifndef MAP_MAPSERVER_ACTION_ACTION_PGREQMAPMOVE_H
#define MAP_MAPSERVER_ACTION_ACTION_PGREQMAPMOVE_H

#include "PgAction.h"
#include "Variant/PgConstellation.h"

class PgReqMapMove
{
	typedef std::list<PgPlayer*>		CONT_PLAYER;

public:
	explicit PgReqMapMove( PgGround * const pkGround, SReqMapMove_MT const &kRMM, PgPortalAccess const * const pkAccess );
	~PgReqMapMove(void);

	bool Add( PgPlayer *pkPlayer );
	bool DoAction(void);
	void AddModifyOrder(SPMO const & kOrder);

	void SetConstellationKey(Constellation::SConstellationKey const& ConstellationKey) { m_ConstellationKey = ConstellationKey; }
	int GetLastAccessError()const { return m_pkAccess ? m_pkAccess->GetLastAccessError() : 0; }
private:
	PgGround * const				m_pkGround;
	SReqMapMove_MT					m_kRMM;
	Constellation::SConstellationKey m_ConstellationKey;
	PgPortalAccess const * const	m_pkAccess;

	CONT_PLAYER						m_kContPlayer;
	CONT_PLAYER_MODIFY_ORDER		m_kStandByItemOrder;

private:
	PgReqMapMove();
	PgReqMapMove( PgReqMapMove const & );
	PgReqMapMove& operator=( PgReqMapMove const & );

};

class PgAction_MapLoaded
	:	public PgUtilAction
{
public:
	explicit PgAction_MapLoaded(PgGround* const pkGround);
	virtual ~PgAction_MapLoaded(){}

	static void RefreshPassiveSkill( PgControlUnit *pkControlUnit, PgGround* const pkGround, PgControlUnit *pkCallerUnit );

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit*);

private:
	PgGround * const				m_pkGround;

private:
	PgAction_MapLoaded();
	PgAction_MapLoaded( PgAction_MapLoaded const & );
	PgAction_MapLoaded& operator=( PgAction_MapLoaded const & );

};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGREQMAPMOVE_H