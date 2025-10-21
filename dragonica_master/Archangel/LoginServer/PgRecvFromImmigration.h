#ifndef LOGIN_LOGINSERVER_PGRECVFROMIMMIGRATION_H
#define LOGIN_LOGINSERVER_PGRECVFROMIMMIGRATION_H

extern void CALLBACK OnConnectToImmigration(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToImmigration(CEL::CSession_Base *pkSession);

extern void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif //LOGIN_LOGINSERVER_PGRECVFROMIMMIGRATION_H