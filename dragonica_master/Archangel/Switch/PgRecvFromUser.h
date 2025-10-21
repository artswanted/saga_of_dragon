#ifndef SWITCH_SWITCHSERVER_PGRECVFROMUSER_H
#define SWITCH_SWITCHSERVER_PGRECVFROMUSER_H

extern void CALLBACK OnAcceptFromUser( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromUser( CEL::CSession_Base *pkSession );

extern void CALLBACK OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // SWITCH_SWITCHSERVER_PGRECVFROMUSER_H