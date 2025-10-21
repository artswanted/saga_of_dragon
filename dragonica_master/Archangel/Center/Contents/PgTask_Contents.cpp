#include "StdAfx.h"
#include <set>
#include "PgTask_Contents.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgExpedition.h"
#include "Variant/PgExpeditionMgr.h"
#include "PgGlobalPartyMgr.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromCenter.h"
#include "PgMissionMgr.h"
#include "ChannelChatMgr.h"
#include "PgPvPLobbyMgr.h"
#include "PgPortalMgr.h"
#include "PgResultMgr.h"

PgTask_ChannelContents::PgTask_ChannelContents(void)
{
}

PgTask_ChannelContents::~PgTask_ChannelContents(void)
{
}

void PgTask_ChannelContents::Close()
{
	this->VDeactivate();
}

void PgTask_ChannelContents::HandleMessage(SEventMessage *pkMsg)
{
	bool bRet = false;

	switch(pkMsg->PriType())
	{
	case PMET_SendToContents:
		{
			OnRecvFromCenter(pkMsg);
		}break;
	case PMET_SendToCenter:
		{
			OnRecvFromServer(NULL, pkMsg);
		}break;
	case PMET_PARTY:
		{
			bRet = g_kPartyMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_ChannelChat:
		{
			bRet = PgChannelChatMgr::ProcessMsg(pkMsg);
		}break;
	case PMET_MissionMgr:
		{
			g_kMissionMgr.RecvPacket(pkMsg);
		}break;
	case PMET_PvP:
	case PMET_PVP_LEAGUE:
		{
			g_kPvPLobbyMgr.ProcessMsg( pkMsg );
		}break;
	case PMET_Portal:
		{
			g_kPortalMgr.ProcessMsg( pkMsg );
		}break;
	case PMET_Boss:
		{
			g_kResultMgr.ProcessMsg( pkMsg );
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("invalid Task primary type[") << pkMsg->PriType() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}
	}
}
