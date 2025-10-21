#ifndef SWITCH_SWITCHSERVER_PGRECVFROMMAP_H
#define SWITCH_SWITCHSERVER_PGRECVFROMMAP_H

extern void CALLBACK OnAcceptFromMap( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromMap( CEL::CSession_Base *pkSession );

extern void CALLBACK OnRecvFromMap(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // SWITCH_SWITCHSERVER_PGRECVFROMMAP_H