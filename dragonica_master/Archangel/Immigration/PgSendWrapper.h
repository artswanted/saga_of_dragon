#ifndef IMMIGRATION_IMMIGRATIONSERVER_PGSENDWRAPPER_H
#define IMMIGRATION_IMMIGRATIONSERVER_PGSENDWRAPPER_H

extern bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);//연결이 싱글인 곳.
extern bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket);
extern bool SendToContentsServer(short const sRealm, BM::Stream const &rkPacket);

extern bool SendDisConnectUser( BYTE cCause, SERVER_IDENTITY const &kSwitchSI, BM::GUID const &kMemberGuid, BYTE byReason = 0 );
extern bool SendToConsentServer(BM::Stream const &rkPacket);

#endif // IMMIGRATION_IMMIGRATIONSERVER_PGSENDWRAPPER_H