#ifndef IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMMMC_H
#define IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMMMC_H

//extern bool InitCMCtrlConnector();
//extern bool ConnectMCtrlServer();
//extern void CALLBACK OnConnectToMCtrl( CEL::CSession_Base *pkSession );
//extern void CALLBACK OnDisConnectToMCtrl(CEL::CSession_Base *pkSession);
extern bool OnRecvFromMMC(CEL::CSession_Base * const pkSession, WORD const wkType, BM::Stream * const pkPacket);

#endif // IMMIGRATION_IMMIGRATIONSERVER_NETWORK_PGRECVFROMMMC_H