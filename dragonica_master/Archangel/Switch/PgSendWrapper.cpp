#include "stdafx.h"
#include "PgSendWrapper.h"

SERVER_IDENTITY g_kLogSI;
SERVER_IDENTITY g_kCenterSI;
SERVER_IDENTITY g_kPublicCenterSI;

bool SetSendWrapper(SERVER_IDENTITY const &kRecvSI)
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
	case CEL::ST_SWITCH:
	case CEL::ST_MAP:
	case CEL::ST_ITEM:
	case CEL::ST_IMMIGRATION:
	case CEL::ST_LOGIN:
	default:
		{//접속되면 안되는 서버
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect ServerIdentity[") << C2L(kRecvSI) << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
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

bool SendToCenter( BM::Stream const &rkPacket, bool bPublic )
{	
	if ( bPublic )
	{
		return SendToServer( g_kPublicCenterSI, rkPacket );
	}
	return SendToServer( g_kCenterSI, rkPacket );
}

bool SendToLog(BM::Stream const &rkPacket)
{	
	return SendToServer(g_kLogSI, rkPacket);
}

bool SendToGround( BM::GUID const &kCharacterGuid, SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::Stream const &rkPacket)
{	
	BM::Stream kWrappedPacket(PT_S_M_WRAPPED_USER_PACKET);
	kGndKey.WriteToPacket(kWrappedPacket);
	kWrappedPacket.Push(kCharacterGuid);
	kWrappedPacket.Push(rkPacket);
	return SendToServer(kSI, kWrappedPacket);//이건 진짜 맵서버 세션에.
}

bool SendToUser(BM::GUID const &kMemberGuid, BM::Stream const &rkPacket)
{	
//	return SendToCenter(kWrappedPacket);
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool SendToChannelChatMgr(BM::Stream const &rkPacket, bool bPublic)
{
	return SendToChannelContents(PMET_ChannelChat, rkPacket, bPublic);
}

bool SendToChannelContents( EContentsMessageType eType, BM::Stream const &rkPacket, bool bPublic, int const iSecondType )
{
	BM::Stream kWrappedPacket( PT_A_CN_WRAPPED_PACKET, eType );
	kWrappedPacket.Push(iSecondType);
	kWrappedPacket.Push(rkPacket);

	return SendToCenter(kWrappedPacket, bPublic);
}

bool SendToRealmChatMgr(BM::Stream const &rkPacket)
{
	return SendToRealmContents(PMET_RealmChat, rkPacket);
}

bool SendToRealmContents( EContentsMessageType eType, BM::Stream const &rkPacket )
{	
	// Send to RealmContents of ContentsServer
	BM::Stream kWrappedPacket(PT_A_RN_WRAPPED_PACKET);
	kWrappedPacket.Push(eType);
	kWrappedPacket.Push(rkPacket);

	return SendToCenter(kWrappedPacket, false);
}

bool SendToImmigration(BM::Stream const &rkPacket)
{
	BM::Stream kWrappedPacket(PT_A_IM_WRAPPED_PACKET);
	kWrappedPacket.Push(rkPacket);
	return SendToCenter(kWrappedPacket, false);
}

bool SendToContents(BM::Stream const &rkPacket)
{
	BM::Stream kWrappedPacket(PT_A_N_WRAPPED_PACKET);
	kWrappedPacket.Push(rkPacket);
	return SendToCenter(kWrappedPacket, false);
}

