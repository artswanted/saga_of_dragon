#ifndef IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMSERVER_H
#define IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMSERVER_H

extern void CALLBACK OnAcceptFromServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnConnectToServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnRecvFromServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMSERVER_H