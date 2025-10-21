#ifndef CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMMANAGEMENTSERVER_H
#define CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMMANAGEMENTSERVER_H

//extern bool InitCMCtrlConnector();
//extern bool ConnectMCtrlServer();
//extern void CALLBACK OnConnectFromMCtrl( CEL::CSession_Base *pkSession );
//extern void CALLBACK OnDisconnectFromMCtrl(CEL::CSession_Base *pkSession);
//extern void CALLBACK OnRecvFromMCtrl( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
bool OnRecvFromMCtrl( WORD wkType, BM::Stream * const pkPacket, CEL::CSession_Base *pkSession);
extern int GetMaxUser();
extern int GetConnectionUser();
extern void SetMaxUser(int const &ChangeCount);

#endif // CONTENTS_CONTENTSSERVER_NETWORK_PGRECVFROMMANAGEMENTSERVER_H