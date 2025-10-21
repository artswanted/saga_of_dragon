#ifndef CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBYMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBYMGR_H

#include "BM/twrapper.h"
#include "PgPvPLobby_Base.h"

class PgPvPLobbyMgr
	:	public TWrapper< PgPvPLobby_AnterRoom >
{
public:
	PgPvPLobbyMgr(void);
	~PgPvPLobbyMgr(void);

	bool ProcessMsg( SEventMessage *pkMsg );

	bool AddLobby( int const iLobbyID, CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kTimeInfo );

	void OnTick(void);
	void OnTick_Event(void);
	void OnTick_Log(void);
};

#define g_kPvPLobbyMgr SINGLETON_STATIC(PgPvPLobbyMgr)

#endif // CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBYMGR_H