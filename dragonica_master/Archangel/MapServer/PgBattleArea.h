#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEAREA_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEAREA_H

#include "PgGroundTrigger.h"

class PgBattleArea
	:	public PgGTrigger_BattleArea
{
public:
	typedef std::map< BM::GUID, int >		CONT_USER_LIST;	// second : team

public:
	PgBattleArea(void);
	explicit PgBattleArea( PgGTrigger_BattleArea const & );
	PgBattleArea( PgBattleArea const & );
	virtual ~PgBattleArea(void);

	PgBattleArea& operator=( PgBattleArea const & );

	HRESULT AddUser( BM::GUID const &kID, int const iTeam );
	HRESULT RemoveUser( BM::GUID const &kID );

	size_t GetUserCount( int const iTeam )const;

	void WriteToPacket( BM::Stream &kPacket )const;

protected:
	CONT_USER_LIST	m_kContUserList;

	size_t		m_iAttackerCount;
	size_t		m_iDefencerCount;
};

typedef std::vector< PgBattleArea >	CONT_BATTLEAREA;

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEAREA_H