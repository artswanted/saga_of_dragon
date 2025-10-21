#ifndef CONTENTS_CONTENTSSERVER_NETWORK_PGSENDWRAPPER_H
#define CONTENTS_CONTENTSSERVER_NETWORK_PGSENDWRAPPER_H

extern bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket);
extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);

extern bool SendToImmigration(BM::Stream const &rkPacket);
extern bool SendToCenter(short const nChannel, BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);
extern bool SendToRealmContents(EContentsMessageType eType, BM::Stream const &rkPacket);
extern bool SendToItem(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket);

extern bool SendToGround(short const sChannel, SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap=true);
extern bool SendToChannel( short const sChannelNo, BM::Stream const &rkPacket );
extern bool SendToPacketHandler(BM::Stream const &rkPacket);

extern bool SendToFriendMgr(BM::Stream const &rkPacket);
extern bool SendToRankMgr(BM::Stream const &rkPacket);
extern bool SendToGuildMgr(BM::Stream const &rkPacket);
extern bool SendToCoupleMgr(BM::Stream const &rkPacket);
extern bool SendToRealmChatMgr(BM::Stream const &rkPacket);
extern bool SendToNotice(BM::Stream const &rkPacket);
extern bool SendToOXGuizEvent(BM::Stream const &rkPacket);
extern bool SendToLuckyStarEvent(BM::Stream const &rkPacket);
extern bool SendToCouponEventDoc(BM::Stream const &rkPacket);
extern bool SendToCouponEventView(BM::Stream const &rkPacket);
extern bool SendToMissionMgr(short sChannel, BM::Stream const &rkPacket);
extern bool SendToGlobalPartyMgr(short sChannel, BM::Stream const &rkPacket);
extern bool SendToChannelContents(short sChannel, EContentsMessageType eType, BM::Stream const &rkPacket, int const iSecondType = 0 );
extern bool SendToGround(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket);
extern bool SendToMyhomeMgr(BM::Stream const &rkPacket);
extern bool SendToExpeditionListMgr( BM::Stream const & Packet);

#endif // CONTENTS_CONTENTSSERVER_NETWORK_PGSENDWRAPPER_H