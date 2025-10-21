#ifndef MAP_MAPSERVER_NETWORK_PGRECVFROMSWITCH_H
#define MAP_MAPSERVER_NETWORK_PGRECVFROMSWITCH_H

extern void CALLBACK OnAcceptFromSwitch( CEL::CSession_Base *pkSession );
extern void CALLBACK OnConnectToSwitch( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToSwitch( CEL::CSession_Base *pkSession );

extern void CALLBACK OnRecvFromSwitch(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // MAP_MAPSERVER_NETWORK_PGRECVFROMSWITCH_H