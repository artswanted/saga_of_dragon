#ifndef LOGIN_LOGINSERVER_PGRECVFROMUSER_H
#define LOGIN_LOGINSERVER_PGRECVFROMUSER_H

extern void CALLBACK OnAcceptFromUser(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToUser(CEL::CSession_Base *pkSession);

extern void CALLBACK OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // LOGIN_LOGINSERVER_PGRECVFROMUSER_H