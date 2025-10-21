#ifndef CENTER_CENTERSERVER_NETWORK_PGRECVFROMLOG_H
#define CENTER_CENTERSERVER_NETWORK_PGRECVFROMLOG_H

extern void CALLBACK OnConnectToLog(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisconnectFromLog(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromLog(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // CENTER_CENTERSERVER_NETWORK_PGRECVFROMLOG_H