#ifndef GM_GMSERVER_NETWORK_PGSENDWRAPPER_H
#define GM_GMSERVER_NETWORK_PGSENDWRAPPER_H

extern bool SetSendWrapper(const SERVER_IDENTITY &kRecvSI);

extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToImmigration(BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);

#endif // GM_GMSERVER_NETWORK_PGSENDWRAPPER_H