#ifndef CONTENTS_CONTENTSSERVER_ITEM_PGRECVFROMCENTER_H
#define CONTENTS_CONTENTSSERVER_ITEM_PGRECVFROMCENTER_H

extern void CALLBACK OnConnectToCenter( CEL::CSession_Base *pkSession );
//extern void CALLBACK OnAcceptFromCenter(CEL::CSession_Base *pkSession);
extern void CALLBACK OnDisConnectToCenter(CEL::CSession_Base *pkSession);
//extern void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern void CALLBACK OnRecvWrappedFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

#endif // CONTENTS_CONTENTSSERVER_ITEM_PGRECVFROMCENTER_H