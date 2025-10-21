#ifndef MAP_MAPSERVER_NETWORK_PGRECVFROMUSER_H
#define MAP_MAPSERVER_NETWORK_PGRECVFROMUSER_H

#include "PgPacketManager.h"

extern void CALLBACK OnRecvFromUser( SGroundKey const& rkGroundKey, BM::GUID const &rkCharacterGuid, BM::Stream * const pkPacket );

#endif // MAP_MAPSERVER_NETWORK_PGRECVFROMUSER_H