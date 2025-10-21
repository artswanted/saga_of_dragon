#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Lohengrin/GMCommand.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromItem.h"
#include "PgRecvFromManagementServer.h"
#include "PgServerSetMgr.h"

//아이템 서버류.
// Center 서버 끼리의 통신
bool CALLBACK OnRecvFromServer3( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket )
{
	//std::cout<< "패킷 받음 T:[" << wkType <<"] Size["<< pkPacket->Size() << "]"<< std::endl;
	switch( usType )
	{
	case PT_I_M_CS_ANS_LAST_RECVED_GIFT:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_M_C_CS_ANS_LAST_RECVED_GIFT);
			kPacket.Push(*pkPacket);
			g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}break;
	case PT_I_N_NOTY_GEN_SYSTEM_INVENTORY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_N_C_NOTY_GEN_SYSTEM_INVENTORY);
			kPacket.Push(*pkPacket);
			g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}
		break;
	case PT_I_M_POST_NOTI_NEW_MAIL:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_M_C_POST_NOTI_NEW_MAIL);
			kPacket.Push(*pkPacket);
			g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}break;
//	case PT_I_T_NFY_INV_DATA:
//		{// 아이템 서버 -> 
//			g_kRealmUserMgr.Locked_Recv_PT_I_T_NFY_INV_DATA(pkPacket);
//		}break;
	case PT_C_T_REQ_CHANNLE_INFORMATION:
		{
			BM::GUID kMemberGuid;
			if ( pkPacket->Pop(kMemberGuid) )
			{
				g_kRealmUserMgr.Recv_PT_C_T_REQ_CHANNLE_INFORMATION(kMemberGuid);
			}
		}break;
	case PT_T_N_REQ_DELETE_GROUND:
		{
			short sChannel;
			GND_KEYLIST kGndKeyList;
			pkPacket->Pop(sChannel);
			pkPacket->Pop(kGndKeyList);
			g_kRealmUserMgr.Locked_Recv_PT_T_N_NFY_DELETE_GROUND(sChannel, kGndKeyList);
		}break;
	case PT_T_N_REQ_LOAD_MYHOME:
		{
			BM::GUID const kGuid( BM::GUID::Create() );

			SGroundKey kGndKey;
			short int sHomeTownNo = 0;
			kGndKey.ReadFromPacket( *pkPacket );
			pkPacket->Pop( sHomeTownNo );

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_MYHOME_INFO,L"EXEC [dbo].[up_LoadMyHomeInfo]");
			kQuery.InsertQueryTarget(kGuid);
			kQuery.QueryOwner(kGuid);
			kQuery.PushStrParam(sHomeTownNo);
			kQuery.contUserData.Push(sHomeTownNo);
			kQuery.contUserData.Push(static_cast<int>(0));
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	default:
		{	//여기서 없으면 3을 콜 하게 되어있다. 
//			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}
