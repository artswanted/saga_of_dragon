#include "stdafx.h"
#include "PgRecvFromMMC.h"
#include "Variant/PgMCtrl.h"
#include "PgSendWrapper.h"
#include "Variant/PgNoticeAction.h"
#include "ImmTask.h"

BM::GUID g_kMCtrlRegistGuid;
CEL::SESSION_KEY g_kMCtrlSessionKey; //Connector Guid

bool OnRecvFromMMC(CEL::CSession_Base * const pkSession, WORD const wkType, BM::Stream * const pkPacket)
{
	switch( wkType )
	{
	case PT_MMC_CONSENT_NFY_INFO:
		{
			BM::Stream kPacket(PT_MMC_CONSENT_NFY_INFO);
			kPacket.Push(*pkPacket);
			return g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONSENT, kPacket);
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_MCTRL_MMC_A_NFY_NOTICE:
		{
			SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_PACKET);
			kMsg.Push(PT_GM_A_REQ_GMCOMMAND);
			kMsg.Push(EGMC_NOTICE);
			kMsg.Push(*pkPacket);
			g_kImmTask.PutMsg(kMsg);
		}break;
	default:
		{
			//VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] Incorrect Packet Type[%u]"), __FUNCTIONW__, __LINE__, wkType);
			return false;
		}break;
	}

	return true;
}
