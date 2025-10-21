#ifndef CENTER_CENTERSERVER_NETWORK_PGSENDWRAPPER_H
#define CENTER_CENTERSERVER_NETWORK_PGSENDWRAPPER_H

extern bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI);

extern bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket);
extern bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket);
extern bool SendToUser( BM::GUID const &kMemberGuid, SERVER_IDENTITY const &kSwitchSI, BM::Stream const &rkPacket );

extern bool SendToImmigration(BM::Stream const &rkPacket);
extern bool SendToManagementServer(BM::Stream const &rkPacket);
extern bool SendToLog(BM::Stream const &rkPacket);
extern bool SendToChannelContents( EContentsMessageType const eType, BM::Stream const &rkPacket, int const iSecondType = 0 );
extern bool SendToOtherChannelContents( short const nTargetChannel, EContentsMessageType const eType, BM::Stream const &rkPacket, int const iSecondType = 0 );
extern bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket );
extern bool SendToItem(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket);

extern bool SendToCenter(BM::Stream const &rkPacket);
extern bool SendToContents( BM::Stream const &rkPacket );
extern bool SendToChannelGround( short const nTargetChannel, SGroundKey const &kTargetGndKey, BM::Stream const &rkPacket, bool const bIsGndWrap = true );

extern bool SendToGlobalPartyMgr(BM::Stream const &rkPacket);
extern bool SendToPvPLobby(BM::Stream const &rkPacket, int const iLobbyID);
extern bool SendToMissionMgr(BM::Stream const &rkPacket);
extern bool SendToPortalMgr(BM::Stream const &rkPacket);
extern bool SendToFriendMgr(BM::Stream const &rkPacket);
extern bool SendToGuildMgr(BM::Stream const &rkPacket);
extern bool SendToChannelChatMgr(BM::Stream const &rkPacket);
extern bool SendToRealmChatMgr(BM::Stream const &rkPacket);
extern bool SendToCoupleMgr(BM::Stream const &rkPacket);
extern bool SendToRankMgr(BM::Stream const &rkPacket);
extern bool SendToHardCoreDungeonMgr( BM::Stream const &rkPacket );
extern bool SendToExpeditionListMgr( BM::Stream const & Packet);

#endif // CENTER_CENTERSERVER_NETWORK_PGSENDWRAPPER_H