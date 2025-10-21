#ifndef GM_GMSERVER_NETWORK_RECVFROMLOGIN_H
#define GM_GMSERVER_NETWORK_RECVFROMLOGIN_H

extern void CALLBACK OnConnectToLogin(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToLogin(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromLogin(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // GM_GMSERVER_NETWORK_RECVFROMLOGIN_H