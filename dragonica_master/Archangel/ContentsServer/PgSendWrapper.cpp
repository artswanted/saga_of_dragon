#include "stdafx.h"
#include "PgSendWrapper.h"
#include "PgServerSetMgr.h"
#include "Item/PgRecvFromCenter.h"
#include "PgTask_Contents.h"

SERVER_IDENTITY g_kImmigrationSI;
SERVER_IDENTITY g_kLogSI;
//SERVER_IDENTITY g_kItemSI;

bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI)
{
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_IMMIGRATION:
		{
			g_kImmigrationSI = kRecvSI;
		}break;
	case CEL::ST_LOG:
		{
			g_kLogSI = kRecvSI;
		}break;
	case CEL::ST_CENTER:
		{//쎈터는 다중 접속됨
		}break;
	default:
		{//접속되면 안되는 서버
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect ServerIdentity[") << C2L(kRecvSI) << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

bool SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServerType(eServerType, rkPacket);
}

bool SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)
{
	return g_kProcessCfg.Locked_SendToServer(kSI, rkPacket);
}

bool SendToImmigration(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kImmigrationSI, rkPacket);
}

bool SendToCenter(short const nChannel, BM::Stream const &rkPacket)
{
	CONT_SERVER_HASH kCont;
	g_kProcessCfg.Locked_GetServerInfo( CEL::ST_CENTER, kCont );

	CONT_SERVER_HASH::const_iterator server_itr = kCont.begin();
	for( ; server_itr!=kCont.end() ; ++server_itr )
	{
		SERVER_IDENTITY const &kSI = server_itr->first;
		if(nChannel == kSI.nChannel)
		{
			return SendToServer( kSI, rkPacket );
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}

bool SendToRealmContents(EContentsMessageType eType, BM::Stream const &rkPacket)
{
	//여기서 컨텐츠 함수로 이전.
	SEventMessage kEventMsg(eType, 0);
	kEventMsg.Push(rkPacket);
	g_kContentsTask.PutMsg(kEventMsg);
	return true;
}

bool SendToItem(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket)
{	
//	BM::Stream kItemPacket(PT_A_I_WRAPPED_PACKET);
	
	BM::Stream kItemPacket;//패킷 타입 뽑았다 치고.
	kSI.WriteToPacket(kItemPacket);
	kItemPacket.Push(kGndKey);
	kItemPacket.Push(rkPacket);

//	return SendToServer(g_kItemSI, kItemPacket);

	OnRecvWrappedFromCenter( NULL, &kItemPacket);
	return true;
}


bool SendToPacketHandler(BM::Stream const &rkPacket)
{
	//센터서버가 소화 하도록 수정.
	SEventMessage kEventMsg(PMET_SendToPacketHandler, 0);
	kEventMsg.Push(rkPacket);
	g_kContentsTask.PutMsg(kEventMsg);
	return true;
}

bool SendToFriendMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_FRIEND, rkPacket);
}

bool SendToRankMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_MissionRank, rkPacket);
}

bool SendToGuildMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_GUILD, rkPacket);
}

bool SendToCoupleMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_COUPLE, rkPacket);
}

bool SendToRealmChatMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_RealmChat, rkPacket);
}

bool SendToNotice(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_Notice, rkPacket);
}

bool SendToOXGuizEvent(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_OXQuizEvent, rkPacket);
}

bool SendToMyhomeMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_MYHOME, rkPacket);
}

bool SendToLuckyStarEvent(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_LuckyStarEvent, rkPacket);
}

bool SendToCouponEventDoc(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_CouponEventDoc, rkPacket);
}

bool SendToCouponEventView(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_CouponEventView, rkPacket);
}

bool SendToMissionMgr(short sChannel, BM::Stream const &rkPacket)
{
	return SendToChannelContents(sChannel, PMET_MissionMgr, rkPacket);
}

bool SendToGlobalPartyMgr(short sChannel, BM::Stream const &rkPacket)
{
	return SendToChannelContents(sChannel, PMET_PARTY, rkPacket);
}

bool SendToChannelContents(short sChannel, EContentsMessageType eType, BM::Stream const &rkPacket, int const iSecondType )
{
	BM::Stream kWrapPacket( PT_A_CN_WRAPPED_PACKET, eType );
	kWrapPacket.Push(iSecondType);
	kWrapPacket.Push(rkPacket);
	return SendToCenter(sChannel, kWrapPacket);
}

bool SendToGround(short const sChannel, SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap)
{
	BM::Stream kPacket(PT_A_GND_WRAPPED_PACKET, kKey);
	kPacket.Push(bIsGndWrap);
	kPacket.Push(rkPacket);
	return SendToChannel(sChannel, kPacket);
}

// 다른 Channel(=Center) 로 패킷을 보낸다.
//	sChannelNo = -1 이면 자기자신을 제외한 모든 채널로 보낸다. 
bool SendToChannel( short const sChannelNo, BM::Stream const &rkPacket )
{
	ContServerID kContSI;	
	if ( S_OK == g_kProcessCfg.Locked_GetServerIdentity( g_kProcessCfg.RealmNo(), sChannelNo, CEL::ST_CENTER, kContSI ) )
	{
		ContServerID::const_iterator itr = kContSI.begin();
		for ( ; itr != kContSI.end() ; ++itr )
		{
			SendToServer( *itr, rkPacket );
		}
		return true;
	}

	INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot Get ServerIdentity R[") << g_kProcessCfg.RealmNo() << _T("], C[") << sChannelNo << _T("], CENTER") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;

}

bool SendToGround(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket)
{	
	BM::Stream kWrappedPacket(PT_A_GND_WRAPPED_PACKET_DIRECT);
	kSI.WriteToPacket(kWrappedPacket);
	kWrappedPacket.Push(kGndKey);
	kWrappedPacket.Push(rkPacket);

	CONT_SERVER_HASH kContServer;
	g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kContServer);

	CONT_SERVER_HASH::const_iterator server_itor = kContServer.begin();
	while(server_itor != kContServer.end())
	{
		if((*server_itor).first.nChannel == kSI.nChannel)
		{
			g_kProcessCfg.Locked_SendToServer((*server_itor).first, kWrappedPacket);
			return true;
			break;
		}
		++server_itor;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool SendToExpeditionListMgr( BM::Stream const & Packet)
{
	return SendToRealmContents( PMET_EXPEDITION, Packet);
}