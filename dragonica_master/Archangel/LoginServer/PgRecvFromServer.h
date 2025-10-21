#ifndef LOGIN_LOGINSERVER_PGRECVFROMSERVER_H
#define LOGIN_LOGINSERVER_PGRECVFROMSERVER_H

extern void CALLBACK OnAcceptFromServer(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToServer(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // LOGIN_LOGINSERVER_PGRECVFROMSERVER_H