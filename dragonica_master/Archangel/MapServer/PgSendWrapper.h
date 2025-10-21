#ifndef MAP_MAPSERVER_NETWORK_PGSENDWRAPPER_H
#define MAP_MAPSERVER_NETWORK_PGSENDWRAPPER_H

#include "Lohengrin/packetstruct2.h"

extern HRESULT SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);
extern bool SendToCenter(BM::Stream const &rkPacket);
extern bool SendToItem(SGroundKey const &kGndKey, BM::Stream const &rkPacket);
extern bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket );
extern bool SendToGround(SGroundKey const &kGndKey, BM::Stream const &rkPacket);
extern bool SendToClient( BM::GUID const &kMemberGuid, BM::Stream const &kPacket);
extern bool SendToContents( BM::Stream const &rkPacket );
extern bool SendToChannelContents( EContentsMessageType eType, BM::Stream const &rkPacket, int const iSecondType = 0 );

extern bool SendToGuildMgr( BM::Stream const &rkPacket );
extern bool SendToCoupleMgr(BM::Stream const &rkPacket );
extern bool SendToChannelChatMgr(BM::Stream const &rkPacket);
extern bool SendToRealmChatMgr(BM::Stream const &rkPacket);
extern bool SendToGlobalPartyMgr( BM::Stream const &rkPacket );
extern bool SendToMissionMgr( BM::Stream const &rkPacket );
extern bool SendToResultMgr( BM::Stream const &rkPacket );
extern bool SendToFriendMgr( BM::Stream const &rkPacket );
extern bool SendToCouponEventView_Map(BM::Stream const rkPacket);
extern bool SendToRealmContents(BM::Stream const &rkPacket);
extern bool SendToRankMgr(BM::Stream const &rkPacket);
extern bool SendToOXQuizEvent(BM::Stream const &rkPacket);
extern bool SendToLuckyStarEvent(BM::Stream const &rkPacket);
extern bool SendToPvPLobby(BM::Stream const &kPacket, int const iLobbyID);
extern bool SendToPvPRoom( int const iLobbyID, int const iRoomIndex, BM::Stream const &rkPacket );
extern bool SendToHardCoreDungeonMgr( BM::Stream const &rkPacket );
extern bool SendToMyhomeMgr(BM::Stream const &rkPacket);
extern bool SendToExpeditionListMgr( BM::Stream const & Packet);

#endif // MAP_MAPSERVER_NETWORK_PGSENDWRAPPER_H