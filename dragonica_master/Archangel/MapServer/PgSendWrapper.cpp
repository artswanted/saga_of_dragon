#include "stdafx.h"
#include "BM/PgTask.h"
#include "PgTask_MapServer.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kLogSI;
SERVER_IDENTITY g_kCenterSI;

HRESULT SetSendWrapper(SERVER_IDENTITY const &kRecvSI)
{
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_LOG:
		{
			g_kLogSI = kRecvSI;
		}break;
	case CEL::ST_CENTER:
		{
			g_kCenterSI = kRecvSI;
		}break;
	case CEL::ST_IMMIGRATION:
		{

		}break;
	case CEL::ST_SWITCH:
	case CEL::ST_MAP:
	case CEL::ST_ITEM:
	case CEL::ST_LOGIN:
	default:
		{//접속되면 안되는 서버
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Incorrect ServerIdentity["<<kRecvSI.nServerType<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	}
	return S_OK;
}

bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServer(kSI, rkPacket);
}

bool SendToCenter( BM::Stream const &rkPacket )
{	
	return SendToServer(g_kCenterSI, rkPacket);
}

bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket )
{	
	// Send to RealmContents of ContentsServer
	BM::Stream kWrappedPacket(PT_A_RN_WRAPPED_PACKET);
	kWrappedPacket.Push(eType);
	kWrappedPacket.Push(rkPacket);

	return SendToCenter(kWrappedPacket);
}

bool SendToCouponEventView_Map(BM::Stream const rkPacket)
{
	SEventMessage kEvent(PMET_COUPON_EVENT_VIEW_MAP);
	kEvent.Push(rkPacket);
	return g_kTask.PutMsg(kEvent);
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}

bool SendToItem( SGroundKey const &kGndKey, BM::Stream const &rkPacket )
{	
	if (rkPacket.Size() > 10000)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Packet size is too long size=") << rkPacket.Size());
	}
	BM::Stream kWrappedPacket(PT_A_I_WRAPPED_PACKET);

	g_kProcessCfg.ServerIdentity().WriteToPacket(kWrappedPacket);
	kWrappedPacket.Push(kGndKey);
	kWrappedPacket.Push(rkPacket);

	return SendToCenter( kWrappedPacket );
}

bool SendToGround(SGroundKey const &kGndKey, BM::Stream const &rkPacket )
{	
	BM::Stream kWrappedPacket(PT_A_GND_WRAPPED_PACKET, kGndKey);
	kWrappedPacket.Push(rkPacket);
	return SendToCenter( kWrappedPacket );
}

bool SendToClient( BM::GUID const &kMemberGuid, BM::Stream const &kPacket )
{
	BM::Stream kCPacket( PT_A_C_WRAPPED_PACKET, kMemberGuid );
	kCPacket.Push(kPacket);
	return SendToContents( kCPacket );
}

bool SendToGuildMgr( BM::Stream const &rkPacket )
{
	return SendToRealmContents(PMET_GUILD, rkPacket);
}

bool SendToCoupleMgr( BM::Stream const &rkPacket )
{
	return SendToRealmContents(PMET_COUPLE, rkPacket);
}

bool SendToChannelChatMgr(BM::Stream const &rkPacket)
{
	return SendToChannelContents(PMET_ChannelChat, rkPacket);
}

bool SendToRealmChatMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_RealmChat, rkPacket);
}

bool SendToGlobalPartyMgr( BM::Stream const &rkPacket )
{
	return SendToChannelContents(PMET_PARTY, rkPacket);
}

bool SendToMissionMgr( BM::Stream const &rkPacket )
{
	return SendToChannelContents(PMET_MissionMgr, rkPacket);
}

bool SendToResultMgr( BM::Stream const &rkPacket )
{
	return SendToChannelContents(PMET_SendToContents, rkPacket);
}

bool SendToFriendMgr( BM::Stream const &rkPacket )
{
	return SendToRealmContents(PMET_FRIEND, rkPacket);
}

bool SendToContents( BM::Stream const &rkPacket )
{
	// Send to ContentsServer
	BM::Stream kWrappedPacket(PT_A_N_WRAPPED_PACKET);
	kWrappedPacket.Push(rkPacket);

	return SendToCenter(kWrappedPacket);
}

bool SendToRankMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_MissionRank, rkPacket);
}

bool SendToOXQuizEvent(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_OXQuizEvent, rkPacket);
}

bool SendToLuckyStarEvent(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_LuckyStarEvent, rkPacket);
}

bool SendToPvPLobby( BM::Stream const &kPacket, int const iLobbyID )
{
	if ( g_kProcessCfg.IsPublicChannel() )
	{
		return SendToChannelContents(PMET_PvP, kPacket, iLobbyID);
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"This Channel["<<g_kProcessCfg.ChannelNo()<<L"] Not Call Plz" );
	return false;
}

bool SendToPvPRoom( int const iLobbyID, int const iRoomIndex, BM::Stream const &rkPacket )
{
	BM::Stream kPacket( PT_A_TO_ROOM, iRoomIndex );
	kPacket.Push(rkPacket);
	return SendToPvPLobby( kPacket, iLobbyID );
}

bool SendToHardCoreDungeonMgr( BM::Stream const &rkPacket )
{
	return SendToRealmContents( PMET_HARDCORE_DUNGEON, rkPacket );
}

bool SendToChannelContents( EContentsMessageType eType, BM::Stream const &rkPacket, int const iSecondType )
{
	BM::Stream kWrappedPacket( PT_A_CN_WRAPPED_PACKET, eType);
	kWrappedPacket.Push(iSecondType);
	kWrappedPacket.Push(rkPacket);
	return SendToCenter(kWrappedPacket);
}
bool SendToMyhomeMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_MYHOME, rkPacket);
}

bool SendToExpeditionListMgr( BM::Stream const & Packet)
{
	return SendToRealmContents( PMET_EXPEDITION, Packet);
}