#ifndef CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMSERVER_H
#define CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMSERVER_H

extern void CALLBACK OnAcceptFromServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnConnectFromServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnRecvFromServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

extern void CALLBACK OnAcceptFromServer_NC( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromServer_NC( CEL::CSession_Base *pkSession );
extern void CALLBACK OnRecvFromServer_NC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMSERVER_H