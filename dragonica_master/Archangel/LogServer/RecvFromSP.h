#ifndef LOG_LOGSERVER_RECVFROMSP_H
#define LOG_LOGSERVER_RECVFROMSP_H

extern void CALLBACK OnConnectToSP( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToSP(CEL::CSession_Base *pkSession);
extern void CALLBACK OnReceiveFromSP(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // LOG_LOGSERVER_RECVFROMSP_H