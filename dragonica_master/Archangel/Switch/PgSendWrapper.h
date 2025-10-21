#ifndef SWITCH_SWITCHSERVER_PGSENDWRAPPER_H
#define SWITCH_SWITCHSERVER_PGSENDWRAPPER_H

extern bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket);
extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);
extern bool SendToCenter(BM::Stream const &rkPacket, bool bPublic);
extern bool SendToGround(BM::GUID const &kCharacterGuid, SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket);
extern bool SendToUser(BM::GUID const &kMemberGuid, BM::Stream const &rkPacket);
extern bool SendToChannelChatMgr(BM::Stream const &rkPacket, bool bPublic);
extern bool SendToChannelContents( EContentsMessageType eType, BM::Stream const &rkPacket, bool bPublic, int const iSecondType = 0 );
extern bool SendToRealmChatMgr(BM::Stream const &rkPacket);
extern bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket );
extern bool SendToImmigration(BM::Stream const &rkPacket);
extern bool SendToContents(BM::Stream const &rkPacket);

extern SERVER_IDENTITY g_kPublicCenterSI;
extern SERVER_IDENTITY g_kCenterSI;

#endif // SWITCH_SWITCHSERVER_PGSENDWRAPPER_H