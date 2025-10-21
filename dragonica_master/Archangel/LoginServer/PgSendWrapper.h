#ifndef LOGIN_LOGINSERVER_PGSENDWRAPPER_H
#define LOGIN_LOGINSERVER_PGSENDWRAPPER_H

extern bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToImmigration(BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);
extern bool SendToGM(BM::Stream const &rkPacket);

#endif // LOGIN_LOGINSERVER_PGSENDWRAPPER_H