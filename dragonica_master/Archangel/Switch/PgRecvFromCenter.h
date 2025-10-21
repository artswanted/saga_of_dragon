#ifndef SWITCH_SWITCHSERVER_PGRECVFROMCENTER_H
#define SWITCH_SWITCHSERVER_PGRECVFROMCENTER_H

extern void CALLBACK OnAcceptFromCenter( CEL::CSession_Base *pkSession );
extern void CALLBACK OnConnectFromCenter( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromCenter( CEL::CSession_Base *pkSession );

extern void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern int GetConnectionUserCount();

#endif // SWITCH_SWITCHSERVER_PGRECVFROMCENTER_H