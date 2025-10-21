#ifndef CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMIMMIGRATION_H
#define CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMIMMIGRATION_H

extern void CALLBACK OnConnectFromImmigration(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisconnectFromImmigration(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern void OnPT_A_S_ANS_GREETING(CEL::CSession_Base *pkSession, SERVER_IDENTITY const &rkServerIdentity);

#endif // CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMIMMIGRATION_H