#ifndef CENTER_CENTERSERVER_NETWORK_PGRECVFROMCONTENTS_H
#define CENTER_CENTERSERVER_NETWORK_PGRECVFROMCONTENTS_H

extern void CALLBACK OnConnectFromContents(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisconnectFromContents(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromContents(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern void OnPT_A_S_ANS_GREETING(CEL::CSession_Base *pkSession, SERVER_IDENTITY const &rkServerIdentity);

#endif // CENTER_CENTERSERVER_NETWORK_PGRECVFROMCONTENTS_H