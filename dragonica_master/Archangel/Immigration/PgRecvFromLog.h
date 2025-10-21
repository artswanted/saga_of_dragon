#ifndef IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMLOG_H
#define IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMLOG_H

extern void CALLBACK OnConnectToLog(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToLog(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromLog(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMLOG_H