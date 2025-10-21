#ifndef MAP_MAPSERVER_NETWORK_PGRECVFROMCENTER_H
#define MAP_MAPSERVER_NETWORK_PGRECVFROMCENTER_H

extern void CALLBACK OnAcceptFromCenter( CEL::CSession_Base *pkSession );
extern void CALLBACK OnConnectToCenter( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToCenter( CEL::CSession_Base *pkSession );

extern void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

extern int GetConnectionUserCount();

#endif // MAP_MAPSERVER_NETWORK_PGRECVFROMCENTER_H