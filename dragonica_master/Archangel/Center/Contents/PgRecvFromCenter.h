#ifndef CENTER_CENTERSERVER_CONTENTS_PGRECVFROMCENTER_H
#define CENTER_CENTERSERVER_CONTENTS_PGRECVFROMCENTER_H

extern void CALLBACK OnConnectToCenter(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisconnectFromCenter(CEL::CSession_Base *pkSession);

extern void CALLBACK OnRecvFromCenter(BM::Stream * const pkPacket);

#endif // CENTER_CENTERSERVER_CONTENTS_PGRECVFROMCENTER_H