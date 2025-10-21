#ifndef LOG_LOGSERVER_PGRECVFROMOTHERSERVER_H
#define LOG_LOGSERVER_PGRECVFROMOTHERSERVER_H

extern void CALLBACK OnAcceptFromOtherServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToOtherServer(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromOtherServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // LOG_LOGSERVER_PGRECVFROMOTHERSERVER_H