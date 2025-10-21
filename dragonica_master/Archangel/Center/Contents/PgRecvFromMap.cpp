#include "Stdafx.h"
#include <set>
#include "Lohengrin/PacketStruct4Map.h"
#include "PgRecvFromMap.h"
#include "PgMissionMgr.h"
#include "PgTask_Contents.h"
#include "Global.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/PacketStruct.h"
#include "PgTask_Contents.h"
#include "PgResultMgr.h"

void OnRecvWrappedFromCenter(unsigned short const usType, BM::Stream* const pkPacket)
{
	assert(pkPacket);

	BM::Stream &kPacket = *pkPacket;

	switch(usType)
	{
	case PT_C_N_REQ_MSN_FRIENDCOMMAND://ёЮЅЕБ®, ДЈ±ёёс·П ён·Йѕо
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("[PT_C_N_REQ_MSN_FRIENDCOMMAND] Cannot handle packet") );
		}break;
	case PT_C_N_REQ_CHAT_NOTICE://°шБц
		{
		}break;
	// Guild
	case PT_C_N_REQ_COUPLE_COMMAND:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("[PT_C_N_REQ_COUPLE_COMMAND] Cannot handle packet") );
		}break;
	case PT_C_M_REQ_MARRY_COMMAND:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("[PT_C_M_REQ_MARRY_COMMAND] Cannot handle packet") );
		}break;
	case PT_M_N_NFY_JOIN_INDUN_PLAYER:
		{
			SGroundKey kGndKey;
			VEC_JOININDUN kJoinIndunList;
			kPacket.Pop( kGndKey );
			kPacket.Pop( kJoinIndunList );
			g_kResultMgr.AddPlayer( kGndKey, kJoinIndunList );
//			g_kGndMgr.JoinIndunPlayer( kGndKey, kJoinIndunList );
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled packet [%d]"), __FUNCTIONW__, usType);
		}break;
	case PT_M_N_NFY_LEAVE_INDUN_PLAYER:
		{
			SGroundKey kGndKey;
			BM::GUID kCharGuid;
			kPacket.Pop( kGndKey );
			kPacket.Pop( kCharGuid );
			g_kResultMgr.DeletePlayer( kGndKey, kCharGuid );
//			g_kGndMgr.LeaveIndunPlayer( kGndKey, kCharGuid );
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled packet [%d]"), __FUNCTIONW__, usType);
		}break;
	case PT_M_N_NFY_RESULT_INDUN:
		{
			SGroundKey kGndKey;
			VEC_RESULTINDUN kResultList;
			kPacket.Pop( kGndKey );
			kPacket.Pop( kResultList );
			g_kResultMgr.Start( kGndKey, kResultList );
//			g_kGndMgr.StartIndunResult( kGndKey, kResultList );
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled packet [%d]"), __FUNCTIONW__, usType);
		}break;
	case PT_M_N_NFY_RESULTITEM_INDUN:
		{
			SGroundKey kGndKey;
			SNfyResultItemList kResultItemList;
			kPacket.Pop( kGndKey );
			kResultItemList.ReadFromPacket( kPacket );
			g_kResultMgr.SetItem( kGndKey, kResultItemList );
//			g_kGndMgr.SetIndunResultItem( kGndKey, kResultItemList );
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled packet [%d]"), __FUNCTIONW__, usType);
		}break;
	case PT_C_M_NFY_SELECTED_BOX:
		{
			SGroundKey kGndKey;
			BM::GUID kCharGuid;
			int iSelect;
			kPacket.Pop( kGndKey );
			kPacket.Pop( kCharGuid );
			kPacket.Pop( iSelect );
			g_kResultMgr.SelectBox( kGndKey, kCharGuid, iSelect );
//			g_kGndMgr.SelectResultBox( kGndKey, kCharGuid, iSelect );
			//INFO_LOG(BM::LOG_LV5, _T("[%s] unhandled packet [%d]"), __FUNCTIONW__, usType);
		}break;
	case PT_C_N_ANS_MISSION_RANK_INPUTMEMO:
		{
			BM::GUID kMemoID;
			std::wstring wstrMemo;
			pkPacket->Pop(kMemoID);
			pkPacket->Pop(wstrMemo);
//			g_kRankMgr.UpdateRegister(kMemoID,wstrMemo);
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled packet [") << usType << _T("]") );
		}break;
	case PT_C_M_REQ_RANK_PAGE:
		{
			BM::GUID kMemberGuid;
			SMissionKey kMissionKey;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kMissionKey);
//			g_kRankMgr.SendRankingList(kMissionKey,kMemberGuid);
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("unhandled packet [") << usType << _T("]") );
		}break;
	case PT_M_N_NFY_EXPEDITION_JOIN_PLAYER:
		{
			SGroundKey GndKey;
			VEC_JOININDUN JoinIndunList;
			kPacket.Pop(GndKey);
			kPacket.Pop(JoinIndunList);
			g_kResultMgr.AddExpeditionPlayer(GndKey, JoinIndunList);
		}break;
	case PT_M_N_NFY_EXPEDITION_LEAVE_PLAYER:
		{
			SGroundKey GndKey;
			BM::GUID CharGuid;
			kPacket.Pop(GndKey);
			kPacket.Pop(CharGuid);
			g_kResultMgr.DeleteExpeditionPlayer(GndKey, CharGuid);
		}break;
	case PT_M_N_NFY_EXPEDITION_RESULTITEM:
		{
			SGroundKey GndKey;
			kPacket.Pop(GndKey);
			g_kResultMgr.SetExpeditionResultItem(GndKey, kPacket);
		}break;
	case PT_M_N_NFY_EXPEDITION_RESULT:
		{
			SGroundKey GndKey;
			VEC_RESULTINDUN ResultList;
			kPacket.Pop(GndKey);
			kPacket.Pop(ResultList);
			g_kResultMgr.ExpeditionStart(GndKey, ResultList);
		}break;
	case PT_C_N_REQ_TENDER_ITEM:
		{
			SGroundKey GndKey;
			BM::GUID CharGuid;
			kPacket.Pop(GndKey);
			kPacket.Pop(CharGuid);
			g_kResultMgr.TenderItem(GndKey, CharGuid);
		}break;
	case PT_C_N_REQ_GIVEUP_TENDER:
		{
			SGroundKey GndKey;
			BM::GUID CharGuid;
			kPacket.Pop(GndKey);
			kPacket.Pop(CharGuid);
			g_kResultMgr.GiveupItem(GndKey, CharGuid);
		}break;
	case PT_M_N_NFY_EXPEDITION_INDUN_CLOSE:
		{
			SGroundKey GndKey;
			kPacket.Pop(GndKey);
			g_kResultMgr.ExpeditionClose(GndKey);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}
