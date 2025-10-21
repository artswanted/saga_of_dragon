#include "stdafx.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgHackDualKeyboard.h"
#include "PgTask_MapServer.h"
#include "PgLocalPartyMgr.h"
#include "PgGroundMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"

namespace TaskUtil
{
	// 외부로 안내주는 CPP 한정 지역변수
	HackDualKeyboard::PgActionCheckMgr kHackDualKeyboardCheckMgr;

	size_t iDualKeyboardMatchRate = 2000; // 20%
	DWORD dwDualKeyboardLimitTime = 10 * 60 * 1000; // 10분

	bool Init()
	{
		int iValue = 0;
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_DualKeyboardHack, eVar_DualKeyboardHack_TIME_MSEC, iValue) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Can't find constant.ini EVar_Kind_DualKeyboardHack, eVar_DualKeyboardHack_TIME_MSEC");
			return false;
		}
		dwDualKeyboardLimitTime = iValue;
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_DualKeyboardHack, eVar_DualKeyboardHack_MATCH_RATE, iValue) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Can't find constant.ini EVar_Kind_DualKeyboardHack, eVar_DualKeyboardHack_MATCH_RATE");
			return false;
		}
		iDualKeyboardMatchRate = iValue;
		return true;
	}

	void HackLogDualKeyboard(SGroundKey const& rkGndKey, HackDualKeyboard::CONT_MATCH_RESULT_LIST const& rkContResult, bool const bPassCheck = false)
	{
		HackDualKeyboard::CONT_MATCH_RESULT_LIST::const_iterator iter = rkContResult.begin();
		while( rkContResult.end() != iter )
		{
			HackDualKeyboard::CONT_MATCH_RESULT_LIST::value_type const& rkMatchResult = (*iter);
			if( bPassCheck 
			||	(HackDualKeyboard::IsCanDetection(iDualKeyboardMatchRate, rkMatchResult) && HackDualKeyboard::IsCanReset(dwDualKeyboardLimitTime/2, rkMatchResult)) )
			{
				float const fOnePercent = 100.f;
				float const fLhsMatchResult = rkMatchResult.iMatchCount * fOnePercent / rkMatchResult.iLhsTotalCount;
				float const fRhsMatchResult = rkMatchResult.iMatchCount * fOnePercent / rkMatchResult.iRhsTotalCount;
				HACKING_LOG(BM::LOG_LV1, L"DUAL KEYBOARD HACK MATCH:" << fLhsMatchResult << L"," << fRhsMatchResult
					<< L","<<rkMatchResult.iMatchCount<<L","<<rkMatchResult.iLhsTotalCount<<L","<<rkMatchResult.iRhsTotalCount
					<< L"," << rkMatchResult.dwTotalEleapsedTime << L"," << rkGndKey.GroundNo() << L"," << rkGndKey.Guid()
					<< L"," << rkMatchResult.kLhsKey << L"," << rkMatchResult.kRhsKey );
			}
			++iter;
		}
	}
	void MessageHelper(HackDualKeyboard::EActionCheckMgrMsg const ePacketType, BM::Stream* pkPacket)
	{
		if( !pkPacket )
		{
			return;
		}

		switch( ePacketType )
		{
		case HackDualKeyboard::ACMM_REG_GROUND:
			{
				SGroundKey kGndKey;

				kGndKey.ReadFromPacket( *pkPacket );
				kHackDualKeyboardCheckMgr.Reg(kGndKey, g_kEventView.GetServerElapsedTime());
			}break;
		case HackDualKeyboard::ACMM_UNREG_GROUND:
			{
				SGroundKey kGndKey;

				kGndKey.ReadFromPacket( *pkPacket );
				kHackDualKeyboardCheckMgr.Unreg(kGndKey);
			}break;
		case HackDualKeyboard::ACMM_ADD_USER:
			{
				BM::GUID kCharGuid;
				SGroundKey kGndKey;

				kGndKey.ReadFromPacket( *pkPacket );
				pkPacket->Pop(kCharGuid);

				kHackDualKeyboardCheckMgr.Add(kGndKey, kCharGuid);
			}break;
		case HackDualKeyboard::ACMM_DEL_USER:
			{
				BM::GUID kCharGuid;
				SGroundKey kGndKey;

				kGndKey.ReadFromPacket( *pkPacket );
				pkPacket->Pop(kCharGuid);

				HackDualKeyboard::CONT_MATCH_RESULT_LIST kContResult;
				kHackDualKeyboardCheckMgr.Del(kGndKey, kCharGuid, kContResult);

				HackLogDualKeyboard(kGndKey, kContResult);
			}break;
		case HackDualKeyboard::ACMM_ACTION:
			{
				BM::GUID kCharGuid;
				HackDualKeyboard::EHackActionCheckType eActionType = HackDualKeyboard::HACT_NONE;
				DWORD dwTime = 0;
				SGroundKey kGndKey;

				kGndKey.ReadFromPacket( *pkPacket );
				pkPacket->Pop(kCharGuid);
				pkPacket->Pop(eActionType);
				pkPacket->Pop(dwTime);

				kHackDualKeyboardCheckMgr.Action(kGndKey, kCharGuid, eActionType, dwTime);
			}break;
		case HackDualKeyboard::ACCM_TICK:
			{
				HackDualKeyboard::CONT_MATCH_MAP_RESULT kContResult;
				kHackDualKeyboardCheckMgr.Tick(g_kEventView.GetServerElapsedTime(), kContResult, iDualKeyboardMatchRate, dwDualKeyboardLimitTime);
				HackDualKeyboard::CONT_MATCH_MAP_RESULT::const_iterator iter = kContResult.begin();
				while( kContResult.end() != iter )
				{
					HackLogDualKeyboard((*iter).first, (*iter).second, true); // 여기에서 얻는 결과는 탐지된 녀석들만
					++iter;
				}
			}break;
		default:
			{
			}break;
		}
	}
};

void PgTask_MapServer::Close()
{
	this->VDeactivate();
}

void PgTask_MapServer::HandleMessage(SEventMessage *pkMsg)
{
#ifdef _DEBUG
	//INFO_LOG(BM::LOG_LV8, _T("From Message [%s]-[%d], Message Type[%d][%d]"), pkMsg->kFunc.c_str(), pkMsg->iLine, pkMsg->PriType(), pkMsg->SecType());
#endif

	switch(pkMsg->PriType())
	{
	/*case PMET_PARTY_MAP:
		{
#ifdef _CHECK_PUBLIC_MAP// 올지 몰라서.
			if ( g_kProcessCfg.IsPublicChannel() )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[%s] PMET_PARTY-%d, Programer!!! No Call Public Channel Plz"), __FUNCTIONW__, pkMsg->SecType() );
			}
#endif
			bRet = g_kLocalPartyMgr.ProcessMsg(pkMsg);
		}break;*/
	case PMET_GROUND_MGR:
		{
			g_kGndMgr.ProcessMsg(pkMsg);
		}break;
	case PMET_COUPON_EVENT_VIEW_MAP:
		{
			m_kCouponEventView.ProcessMsg(pkMsg);
		}break;
	case PMET_HACK_CHECK_DUALKEYBOARD:
		{
			TaskUtil::MessageHelper(static_cast< HackDualKeyboard::EActionCheckMgrMsg >(pkMsg->SecType()), pkMsg);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"invalid Task primary type["<<pkMsg->PriType()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

bool PgTask_MapServer::WriteToPacket(EPrimaryEventMessageType eType, BM::Stream & rkPacket)
{
	switch (eType)
	{
	case PMET_COUPON_EVENT_VIEW_MAP:
		{
			m_kCouponEventView.WriteToPacket(rkPacket);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV3, __FL__ << _T("unknown type=") << static_cast<int>(eType));
			CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("unknown type=") << static_cast<int>(eType));
			return false;
		}
	}
	return true;
}

