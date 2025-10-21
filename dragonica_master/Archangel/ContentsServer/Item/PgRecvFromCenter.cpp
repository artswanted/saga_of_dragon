#include "stdafx.h"
#include "Variant/PgMctrl.h"
#include "item/PgDBProcess.h"
#include "item/PgPostManager.h"
#include "item/PgCashShopManager.h"
#include "JobDispatcher.h"
#include "PgOXQuizEvent.h"
#include "Transaction.h"
#include "Item/ItemDropControl.h"
#include "Variant/PgGambleMachine.h"


void CALLBACK OnConnectToCenter( CEL::CSession_Base *pkSession )
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T("Connect Success IP[") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Connect Failure") );
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectCenter();
	}
}

void CALLBACK OnDisConnectToCenter(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Disconnected SessionKey[") << pkSession->SessionKey().SessionGuid().str().c_str() << _T("]") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	g_kProcessCfg.Locked_ConnectCenter();
}

void CALLBACK OnRecvWrappedFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

//	INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] 패킷 받음 T[%d] SIze[%d]"), __FUNCTIONW__, __LINE__ , usType, pkPacket->Size());

	switch( usType )
	{
	case PT_A_I_WRAPPED_PACKET:
		{	//타입 뽑고 다른 함수로 넘겻.
			OnRecvWrappedFromCenter(pkSession, pkPacket);
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}break;
	}
	return;
}

void CALLBACK OnRecvWrappedFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	//뽑아낼때는 SendToItem 함수와 맞출것
	
	SERVER_IDENTITY kSI;
	SGroundKey kGndKey;
	BM::Stream::DEF_STREAM_TYPE kPacketType;

	kSI.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kGndKey);
	pkPacket->Pop(kPacketType);

	if(g_kCashShopMgr.Locked_HandleRecvMessage(kPacketType,kSI,kGndKey,pkPacket))
	{
		return;
	}

	//if(g_kOXQuizEvent.Locked_HandleRecvMessage(kPacketType,kSI,kGndKey,pkPacket))
	//{
	//	return;
	//}

	switch(kPacketType)
	{
	case PT_M_I_REQ_GAMBLEMACHINEINFO:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			SContentsUser rkOut;
			SUserInfoExt kExtInfo;
			HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
			if(S_OK == hRet)
			{
				PgTranPointer kTran(ECASH_TRAN_CASHSHOP_ENTER, rkOut.kMemGuid, kOwnerGuid, kGndKey, kSI, CIE_CS_Select);
				PgEnterCashShopTran* pkTran = dynamic_cast<PgEnterCashShopTran*>(kTran.GetTran());
				if (pkTran != NULL)
				{
					pkTran->UID(rkOut.iUID);
					pkTran->AccountID(rkOut.kAccountID);
					pkTran->RemoteAddr(kExtInfo.kAddress);
				}
				kTran.RequestCurerntLimitSell();
			}
		}break;
	case PT_M_I_REQ_USE_GAMBLEMACHINE:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			PgDoc_Player rkCopyPlayer;
			if(false == g_kRealmUserMgr.Locked_GetDoc_Player(kOwnerGuid,false,rkCopyPlayer))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
				return;
			}

			BYTE bCoinType = 0;
			pkPacket->Pop(bCoinType);

			int iCost = 0;
			if(false == g_kGambleMachine.GetGambleMachineCost(static_cast<eGambleCoinGrade>(bCoinType),iCost))
			{
				BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE);
				kPacket.Push(E_GAMBLEMACHINE_INVALID_COIN);
				g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
				return;
			}

			bool bBroadcast = false;
			CONT_GAMBLEMACHINERESULT kContResult;
			HRESULT kErr = g_kGambleMachine.GetGambleResult(static_cast<eGambleCoinGrade>(bCoinType), kContResult, rkCopyPlayer.UnitRace(), bBroadcast);
			if(S_OK != kErr)
			{
				BM::Stream kPacket(PT_M_C_ANS_USE_GAMBLEMACHINE);
				kPacket.Push(kErr);
				g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid,kPacket,false);
				return;
			}

			PgTranPointer kTran(ECASH_TRAN_ADD_CASH, rkCopyPlayer.GetMemberGUID(), kOwnerGuid, rkCopyPlayer.GroundKey(), rkCopyPlayer.GetSwitchServer(), CIE_GambleMachine);
			PgAddCashTran* pkTran = dynamic_cast<PgAddCashTran*>(kTran.GetTran());
			if (pkTran != NULL)
			{
				pkTran->UID(rkCopyPlayer.UID());
				pkTran->AccountID(rkCopyPlayer.MemberID());
				pkTran->CharacterName(rkCopyPlayer.Name());
				pkTran->RemoteAddr(rkCopyPlayer.addrRemote());
				pkTran->Cash(static_cast<__int64>(-iCost));

				CONT_PLAYER_MODIFY_ORDER kOrder;
				kOrder.WriteToPacket(pkTran->AddedPacket());
				PU::TWriteArray_M(pkTran->AddedPacket(), kContResult);
				pkTran->AddedPacket().Push(bBroadcast);
			}
			kTran.RequestPayCash();
		}break;
	case PT_M_C_NFY_GAMBLEMACHINE_MIXUP_RESULT:
	case PT_M_C_NFY_GAMBLEMACHINE_RESULT:
	case PT_M_C_NFY_ITEM_MAKING_SUCCESS:
		{
			BM::Stream kPacket(kPacketType);
			kPacket.Push(*pkPacket);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
		}break;
	case PT_M_I_REQ_MYHOME_AUCTION_REG:
	case PT_M_I_REQ_MYHOME_VISITORS:
	case PT_M_I_REQ_MYHOME_INVITATION_CARD:
	case PT_M_I_REQ_MYHOME_POST_INVITATION_CARD:
	case PT_M_I_REQ_HOME_VISITLOG_ADD:
	case PT_M_I_REQ_HOME_VISITLOG_LIST:
	case PT_M_I_REQ_HOME_VISITLOG_DELETE:
	case PT_M_I_REQ_MYHOME_AUCTION_UNREG:
	case PT_M_I_REQ_MYHOME_PAY_TEX:
	case PT_M_I_REQ_MYHOME_INFO:
	case PT_M_I_REQ_MYHOME_ENTER:
	case PT_M_I_REQ_HOMETOWN_INFO:
	case PT_M_I_REQ_START_SIDE_JOB:
	case PT_M_I_REQ_CANCEL_SIDE_JOB:
	case PT_M_I_REQ_ENTER_SIDE_JOB:
	case PT_M_I_REQ_EXCHANGE_LOGCOUNTTOEXP:
	case PT_M_I_REQ_MYHOME_SELL:
		{
			g_kRealmUserMgr.RecvMyHomePacketHandler(kPacketType,kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_REQ_ACHIEVEMENT_RANK:
		{
			BM::GUID	kCharGuid;
			pkPacket->Pop(kCharGuid);

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SELECT_ACHIEVEMENT_RANK,L"EXEC [dbo].[up_SelectAchievementRank]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.QueryOwner(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_M_I_GEN_COUPON:
		{
			BM::GUID	kMemberGuid,
						kCharGuid;
			
			int iEventNo = 0;
			std::wstring kTitle;
			std::wstring kText;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(iEventNo);
			pkPacket->Pop(kTitle);
			pkPacket->Pop(kText);

			CEL::DB_QUERY kQuery( DT_MEMBER, DQT_GET_EVENT_COUPON,L"EXEC [dbo].[up_GetEventCoupon]");
			kQuery.InsertQueryTarget(kMemberGuid);
			kQuery.QueryOwner(kMemberGuid);
			kQuery.PushStrParam(kMemberGuid);
			kQuery.PushStrParam(iEventNo);
			kQuery.PushStrParam(g_kProcessCfg.SiteNo());
			kQuery.PushStrParam(g_kProcessCfg.RealmNo());

			kQuery.contUserData.Push(kCharGuid);
			kQuery.contUserData.Push(kTitle);
			kQuery.contUserData.Push(kText);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_M_I_REQ_SEARCH_MATCH_CARD:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_REQ_CHARACTER_CARD_INFO:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_CS_REQ_LAST_RECVED_GIFT:
		{
			BM::GUID kGuid;
			pkPacket->Pop(kGuid);

			CEL::DB_QUERY kQuery2(DT_PLAYER,DQT_GET_LAST_RECVED_GIFT,L"EXEC [dbo].[UP_CS_GET_LAST_RECVED_GIFT]");
			kQuery2.InsertQueryTarget(kGuid);

			kQuery2.PushStrParam(kGuid);
			kQuery2.QueryOwner(kGuid);

			g_kCoreCenter.PushQuery(kQuery2);
		}break;
	case PT_M_I_POST_SYSTEM_MAIL:
		{
			SSendMailInfo kMailInfo;
			kMailInfo.ReadFromPacket(*pkPacket);
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
			pkActionOrder->kPacket2nd.Push(kMailInfo.FromGuid());
			pkActionOrder->kPacket2nd.Push(kMailInfo.TargetName());
			pkActionOrder->kPacket2nd.Push(kMailInfo.MailTitle());
			pkActionOrder->kPacket2nd.Push(kMailInfo.MailText());
			pkActionOrder->kPacket2nd.Push(kMailInfo.ItemNo());
			pkActionOrder->kPacket2nd.Push(kMailInfo.ItemNum());
			pkActionOrder->kPacket2nd.Push(static_cast<int>(kMailInfo.Money()));
			pkActionOrder->kPacket2nd.Push(kMailInfo.FromName());

			g_kJobDispatcher.VPush(pkActionOrder);
		}break;
	case PT_M_I_UM_REQ_MARKET_ENTER:
		{
			BM::GUID	kOwnerGuid,
						kMemberGuid;
			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kMemberGuid);

			SContentsUser rkOut;
			SUserInfoExt kExtInfo;
			HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
			if(S_OK != hRet)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
				return;
			}

			PgTranPointer kTran(ECASH_TRAN_OPENMARKET_ENTER, rkOut.kMemGuid, kOwnerGuid, SGroundKey(), SERVER_IDENTITY(), CIE_CS_Select);
			PgEnterOpenMarketTran* pkTran = dynamic_cast<PgEnterOpenMarketTran*>(kTran.GetTran());
			if (pkTran != NULL)
			{
				pkTran->AccountID(rkOut.kAccountID);
				pkTran->UID(rkOut.iUID);
				pkTran->RemoteAddr(kExtInfo.kAddress);
			}
			kTran.RequestCurerntLimitSell();
			/*
			CEL::DB_QUERY kQuery2(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
			kQuery2.InsertQueryTarget(kMemberGuid);
			kQuery2.PushStrParam(kMemberGuid);
			kQuery2.QueryOwner(kOwnerGuid);
			kQuery2.contUserData.Push(CIE_CS_Select);
			g_kCoreCenter.PushQuery(kQuery2);
			*/
		}break;
	case PT_M_I_UM_REQ_MARKET_CLOSE:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_MARKET_CLOSE(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MARKET_MODIFY_STATE:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_MARKET_MODIFY_STATE(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_USE_MARKET_MODIFY_ITEM:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_USE_MARKET_MODIFY_ITEM(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MARKET_OPEN:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_MARKET_OPEN(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_ARTICLE_REG:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_ARTICLE_REG(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_ARTICLE_DEREG:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_ARTICLE_DEREG(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_ARTICLE_BUY:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_ARTICLE_BUY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_DEALINGS_READ:
		{
			g_kRealmUserMgr.RecvPT_M_I_UM_REQ_DEALINGS_READ(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MARKET_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MINIMUM_COST_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MY_MARKET_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MY_VENDOR_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_VENDOR_ENTER:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_VENDOR_REFRESH_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_MARKET_ARTICLE_QUERY:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_UM_REQ_BEST_MARKET_LIST:
		{
			g_kRealmUserMgr.Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_T_REQ_SMS:
		{
			BM::Stream kPacket(PT_M_T_REQ_SMS,*pkPacket);
			SendToServerType(CEL::ST_CENTER, kPacket);
		}break;
	case PT_M_I_POST_REQ_MAIL_MODIFY:
		{
			g_kPostMgr.OnRecvPT_M_I_POST_REQ_MAIL_MODIFY(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_POST_REQ_MAIL_RECV:
		{
			g_kPostMgr.OnRecvPT_M_I_POST_REQ_MAIL_RECV(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_POST_REQ_MAIL_SEND:
		{
			g_kPostMgr.OnRecvPT_M_I_POST_REQ_MAIL_SEND(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_POST_REQ_MAIL_MIN:
		{
			g_kPostMgr.OnRecvPT_M_I_POST_REQ_MAIL_MIN(kSI,kGndKey,pkPacket);
		}break;
	case PT_M_I_REQ_MODIFY_ITEM:
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();

			BM::GUID kOwnerGuid;
			bool bAddon;
			pkPacket->Pop(pkActionOrder->kCause);
			pkPacket->Pop(kOwnerGuid);
			pkActionOrder->InsertTarget(kOwnerGuid);
			pkActionOrder->kContOrder.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAddon);
			pkActionOrder->kAddonPacket.Push(*pkPacket);
			pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
			g_kJobDispatcher.VPush(pkActionOrder);
		}break;
	case PT_M_N_REQ_GIVE_LIMITED_ITEM:
		{
			SREQ_GIVE_LIMITED_ITEM kData;
			pkPacket->Pop(kData);
			g_kItemDropControl.RequestItemPop(kSI, kData);
		}break;
	case PT_M_I_REQ_CASH_MODIFY:
		{
			g_kRealmUserMgr.Locked_Recv_PT_M_I_REQ_CASH_MODIFY(pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << kPacketType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}
