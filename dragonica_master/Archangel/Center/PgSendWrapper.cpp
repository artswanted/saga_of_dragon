#include "stdafx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kImmigrationSI;
SERVER_IDENTITY g_kLogSI;
SERVER_IDENTITY g_kItemSI;
SERVER_IDENTITY g_kManagementServerSI;
SERVER_IDENTITY g_kContentsServerSI;

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
	case CEL::ST_ITEM:
		{
			g_kItemSI = kRecvSI;
		}break;
	case CEL::ST_MACHINE_CONTROL:
		{
			g_kManagementServerSI = kRecvSI;
		}break;
	case CEL::ST_SWITCH:
	case CEL::ST_MAP:
		{//ґЩБЯАё·О єЩґВ ј­№ц
		}break;
	case CEL::ST_LOGIN:
		{
		}break;
	case CEL::ST_CONTENTS:
		{
			g_kContentsServerSI = kRecvSI;
		}break;
	default:
		{//БўјУµЗёй ѕИµЗґВ ј­№ц
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

bool SendToUser( BM::GUID const &kMemberGuid, SERVER_IDENTITY const &kSwitchSI, BM::Stream const &rkPacket )
{
	BM::Stream kWrappedPacket(PT_S_S_SEND_TO_SWITCH_USER);
	kWrappedPacket.Push(kMemberGuid);
	kWrappedPacket.Push(rkPacket);
	return g_kProcessCfg.Locked_SendToServer( kSwitchSI, kWrappedPacket );
}

bool SendToImmigration(BM::Stream const &rkPacket)
{	
	BM::Stream kWrapPacket(PT_A_IM_WRAPPED_PACKET);
	kWrapPacket.Push(rkPacket);
	return SendToContents(kWrapPacket);
}

bool SendToManagementServer(BM::Stream const &rkPacket)
{
	return SendToServer(g_kManagementServerSI, rkPacket);
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}

bool SendToChannelContents( EContentsMessageType const eType, BM::Stream const &rkPacket, int const iSecondType )
{
	//ї©±вј­ ДБЕЩГч ЗФјц·О АМАь.
	SEventMessage kEventMsg(eType, iSecondType);
	kEventMsg.Push(rkPacket);
	g_kCenterTask.PutMsg(kEventMsg);
	return true;
}

bool SendToOtherChannelContents( short const nTargetChannel, EContentsMessageType const eType, BM::Stream const &rkPacket, int const iSecondType )
{
	if ( g_kProcessCfg.ChannelNo() != nTargetChannel )
	{
		BM::Stream kWrapPacket(PT_CN_CN_WRAPPED_PACKET, nTargetChannel);
		kWrapPacket.Push(eType);
		kWrapPacket.Push(iSecondType);
		kWrapPacket.Push(rkPacket);
		return ::SendToContents(kWrapPacket);
	}
	return false;
}

bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket )
{
	BM::Stream kWrapPacket(PT_A_RN_WRAPPED_PACKET);
	kWrapPacket.Push(eType);
	kWrapPacket.Push(rkPacket);
	return ::SendToContents(kWrapPacket);
}

bool SendToItem(SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket)
{	
	BM::Stream kItemPacket(PT_A_I_WRAPPED_PACKET);
	
//	BM::Stream kItemPacket;//ЖРЕ¶ ЕёАФ »МѕТґЩ ДЎ°н.
	kSI.WriteToPacket(kItemPacket);
	kItemPacket.Push(kGndKey);
	kItemPacket.Push(rkPacket);

	return g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kItemPacket);

//	return SendToServer(g_kItemSI, kItemPacket);
//	OnRecvWrappedFromCenter( NULL, &kItemPacket);
	return true;
}

bool SendToCenter(BM::Stream const &rkPacket)
{
	//јѕЕНј­№ц°Ў јТИ­ ЗПµµ·П јцБ¤.
	SEventMessage kEventMsg(PMET_SendToCenter, 0);
	kEventMsg.Push(rkPacket);
	g_kCenterTask.PutMsg(kEventMsg);
	return true;
}

bool SendToContents( BM::Stream const &rkPacket )
{
	return SendToServer(g_kContentsServerSI, rkPacket);
}

bool SendToChannelGround( short const nTargetChannel, SGroundKey const &kTargetGndKey, BM::Stream const &rkPacket, bool const bIsGndWrap )
{
	if ( g_kProcessCfg.ChannelNo() != nTargetChannel )
	{
		BM::Stream kWrapPacket(PT_A_CNGND_WRAPPED_PACKET, nTargetChannel);
		kTargetGndKey.WriteToPacket( kWrapPacket );
		kWrapPacket.Push(bIsGndWrap);
		kWrapPacket.Push(rkPacket);
		return ::SendToContents(kWrapPacket);
	}

	return g_kServerSetMgr.Locked_SendToGround( kTargetGndKey, rkPacket, bIsGndWrap );
}

bool SendToMissionMgr(BM::Stream const &rkPacket)
{
	return SendToChannelContents(PMET_MissionMgr, rkPacket);
}

bool SendToGlobalPartyMgr(BM::Stream const &rkPacket)
{
	return SendToChannelContents(PMET_PARTY, rkPacket);
}

bool SendToFriendMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_FRIEND, rkPacket);
}

bool SendToGuildMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_GUILD, rkPacket);
}

bool SendToChannelChatMgr(BM::Stream const &rkPacket)
{
	return SendToChannelContents(PMET_ChannelChat, rkPacket);
}

bool SendToPvPLobby( BM::Stream const &rkPacket, int const iLobbyID )
{
	if ( g_kProcessCfg.IsPublicChannel() )
	{
		return SendToChannelContents(PMET_PvP, rkPacket, iLobbyID);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool SendToPortalMgr(BM::Stream const &rkPacket)
{
	return SendToChannelContents(PMET_Portal, rkPacket);
}

bool SendToRealmChatMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_RealmChat, rkPacket);
}

bool SendToCoupleMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_COUPLE, rkPacket);
}

bool SendToRankMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_MissionRank, rkPacket);
}

bool SendToHardCoreDungeonMgr( BM::Stream const &rkPacket )
{
	return SendToRealmContents( PMET_HARDCORE_DUNGEON, rkPacket );
}

bool SendToExpeditionListMgr( BM::Stream const & Packet)
{
	return SendToRealmContents( PMET_EXPEDITION, Packet);
}