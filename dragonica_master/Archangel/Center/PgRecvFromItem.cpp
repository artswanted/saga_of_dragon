#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromItem.h"
#include "PgRecvFromManagementServer.h"

//ѕЖАМЕЫ ј­№ц·щ.
// Center ј­№ц іўё®АЗ ЕлЅЕ
bool CALLBACK OnRecvFromServer3( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket )
{
	//std::cout<< "ЖРЕ¶ №ЮАЅ T:[" << wkType <<"] Size["<< pkPacket->Size() << "]"<< std::endl;
	switch( usType )
	{
	case PT_I_M_CS_ANS_LAST_RECVED_GIFT:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_M_C_CS_ANS_LAST_RECVED_GIFT);
			kPacket.Push(*pkPacket);
			g_kServerSetMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}break;
	case PT_I_N_NOTY_GEN_SYSTEM_INVENTORY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_N_C_NOTY_GEN_SYSTEM_INVENTORY);
			kPacket.Push(*pkPacket);
			g_kServerSetMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}
		break;
	case PT_I_M_POST_NOTI_NEW_MAIL:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			BM::Stream kPacket(PT_M_C_POST_NOTI_NEW_MAIL);
			kPacket.Push(*pkPacket);
			g_kServerSetMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
		}break;
	case PT_C_T_REQ_CHANNLE_INFORMATION:
		{
			BM::GUID kMemberGuid;
			if ( pkPacket->Pop(kMemberGuid) )
			{
				BM::Stream kNPacket(PT_T_IM_REQ_CHANNEL_INFORMATION, g_kProcessCfg.ServerIdentity() );
				kNPacket.Push(kMemberGuid);
				return SendToContents(kNPacket);
			}
		}break;
	case PT_N_C_NFY_NOTICE_PACKET:
		{
			SendToServerType( CEL::ST_MAP, BM::Stream(PT_N_C_NFY_NOTICE_PACKET, *pkPacket) );
		}break;
	default:
		{
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}
