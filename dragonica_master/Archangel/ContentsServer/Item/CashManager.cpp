#include "stdafx.h"
#include "CashManager.h"
#include "Transaction.h"
#include "PgServerSetMgr.h"
#include "Item/PgCashShopManager.h"
#include "jobworker.h"
#include "jobdispatcher.h"

PgCashManager::PgCashManager()
{
}

PgCashManager::~PgCashManager()
{
}

bool PgCashManager::RequestCurerntLimitSell(BM::Stream& rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}

	size_t const iRDPos = pkTran->AddedPacket().RdPos();
	int iArticleIdx = 0;

	switch( pkTran->EventType() )
	{
	case CIE_CS_Buy:
		{
			CONT_REQ_BUY_ARTICLE kContBuyArticle;
			pkTran->AddedPacket().Pop(kContBuyArticle);
			pkTran->AddedPacket().RdPos(iRDPos);

			CONT_REQ_BUY_ARTICLE::iterator articleiter = kContBuyArticle.begin();
			if( articleiter != kContBuyArticle.end() )
			{
				TABLE_LOCK(CONT_CASH_SHOP_ITEM_LIMITSELL) kObjLock;
				g_kTblDataMgr.GetContDef(kObjLock);
				CONT_CASH_SHOP_ITEM_LIMITSELL const * pCont = kObjLock.Get();

				if(!pCont)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
					return false;
				}

				CONT_CASH_SHOP_ITEM_LIMITSELL::const_iterator iter = pCont->find((*articleiter).iArticleIdx);
				if(iter != pCont->end())
				{
					// 한정 판매 아이템만 저장해준다.
					iArticleIdx = (*articleiter).iArticleIdx;
				}
			}
		}break;
	case CIE_CS_Gift:
		{
			std::wstring	kSenderName;
			int				iArticleIdxValue = 0;
			pkTran->AddedPacket().Pop(kSenderName);
			pkTran->AddedPacket().Pop(iArticleIdxValue);
			pkTran->AddedPacket().RdPos(iRDPos);

			TABLE_LOCK(CONT_CASH_SHOP_ITEM_LIMITSELL) kObjLock;
			g_kTblDataMgr.GetContDef(kObjLock);
			CONT_CASH_SHOP_ITEM_LIMITSELL const * pCont = kObjLock.Get();

			if(!pCont)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return CSR_INVALID_ARTICLE"));
				return false;
			}

			CONT_CASH_SHOP_ITEM_LIMITSELL::const_iterator iter = pCont->find(iArticleIdxValue);
			if(iter != pCont->end())
			{
				// 한정 판매 아이템만 저장해준다.
				iArticleIdx = iArticleIdxValue;
			}
		}break;
	default:
		{			
		}break;
	}

	pkTran->ItemIdx(iArticleIdx);

	CEL::DB_QUERY kQuery(DT_LOCAL,DQT_CS_CASH_LIMIT_ITEM_QUERY,L"EXEC [dbo].[UP_LoadDefCashItemLimitSell]");

	kQuery.InsertQueryTarget(pkTran->CharacterGuid());
	kQuery.PushStrParam(pkTran->MemberGuid());
	kQuery.PushStrParam(iArticleIdx);
	kQuery.QueryOwner(pkTran->CharacterGuid());


	pkTran->WriteToPacket(kQuery.contUserData);

	if (S_OK != g_kCoreCenter.PushQuery(kQuery))
	{
		return false;
	}
	return true;
}

bool PgCashManager::RecvCurerntLimitSell(BM::Stream& rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}

	ECashShopResult eResult = CSR_INVALID_ARTICLE;
	pkTran->ResultPacket().Pop(eResult);
	if (eResult == CSR_SUCCESS)
	{
		if( false == kTran.RequestCurerntCash() )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("RequestCurerntCash order failed TransactionType=") << pkTran->GetTransactionType());
			return false;
		}
	}
	else
	{
		BM::vstring vLogMsg;
		kTran.Log(vLogMsg);
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("GetCashBalance Failed Error=") << eResult << _T(", Tran=") << vLogMsg);
		return false;
	}
	return true;
}

bool PgCashManager::RequestCurerntCash(BM::Stream& rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}
	switch (g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				return GALA::RequestCurrentCash(*pkTran);
			}
		} // no break;
	default:
		{
			CEL::DB_QUERY kQuery(DT_MEMBER,DQT_CS_CASH_QUERY,L"EXEC [dbo].[UP_CS_SELECTMEMBERCASH]");
			kQuery.InsertQueryTarget(pkTran->CharacterGuid());
			kQuery.PushStrParam(pkTran->MemberGuid());

			kQuery.QueryOwner(pkTran->CharacterGuid());
			pkTran->WriteToPacket(kQuery.contUserData);
		
			if (S_OK != g_kCoreCenter.PushQuery(kQuery))
			{
				return false;
			}
			return true;
		}break;
	}
	return false;
}

bool PgCashManager::RecvCurrentCash(BM::Stream& rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}

	ECashShopResult eResult = CSR_INVALID_ARTICLE;
	pkTran->ResultPacket().Pop(eResult);
	if (eResult == CSR_SUCCESS)
	{
		HRESULT hReturn = TRAN_HELPER::RecvCashBalance(kTran);
		if (hReturn != S_OK)
		{
			g_kRealmUserMgr.Locked_LeaveEvent(pkTran->CharacterGuid());
			return false;
		}
	}
	else
	{
		BM::vstring vLogMsg;
		kTran.Log(vLogMsg);
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("GetCashBalance Failed Error=") << eResult << _T(", Tran=") << vLogMsg);
	}
	return true;
}

bool PgCashManager::RequestCashModify(BM::Stream &rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}
	if (false == pkTran->PayCash())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("PayCash order failed TransactionType=") << pkTran->GetTransactionType());
		return false;
	}
	return true;
}

bool PgCashManager::RecvCashModify(BM::Stream &rkPacket)
{
	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}

	ECashShopResult eResult = CSR_INVALID_ARTICLE;
	__int64 i64CashCur;
	__int64 i64MileageCur;
	__int64 i64AddedMileageCur;
	pkTran->ResultPacket().Pop(eResult);
	pkTran->ResultPacket().Pop(i64CashCur);
	pkTran->ResultPacket().Pop(i64MileageCur);
	pkTran->ResultPacket().Pop(i64AddedMileageCur);

	if (eResult == CSR_SUCCESS)
	{
		pkTran->CurCash(i64CashCur);
		pkTran->CurMileage(i64MileageCur);
		pkTran->AddedMileage(i64AddedMileageCur);
		CONT_PLAYER_MODIFY_ORDER kOrder;
		if (false == pkTran->GetOrder(kOrder))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("GetOrder failed TransactionType=") << pkTran->GetTransactionType());
			return false;
		}
		if (kOrder.empty())
		{
			kTran.Finish(S_OK);
			return true;
		}
		if (false == kTran.RequestItemModify(kOrder))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("RequestItemModify order failed TransactionType=") << pkTran->GetTransactionType());
			return false;
		}
	}
	else
	{
		BM::vstring vLogMsg;
		kTran.Log(vLogMsg);
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("CashModify Failed Error=") << eResult << _T(", Tran=") << vLogMsg);
		// Client 에 에러 보내주면 더 좋겠음.
		g_kRealmUserMgr.Locked_LeaveEvent(pkTran->CharacterGuid());

		BM::Stream kPacket(PT_M_C_CS_ANS_BUY_ARTICLE);
		kPacket.Push(eResult);
		kPacket.Push((int)0);
		g_kRealmUserMgr.Locked_SendToUser(pkTran->CharacterGuid(), kPacket, false);
	}
	return true;
}

bool PgCashManager::FinishCashProcess(BM::Stream &rkPacket)
{
	HRESULT hResult = S_OK;
	rkPacket.Pop(hResult);

	PgTranPointer kTran(rkPacket);
	PgTransaction* pkTran = kTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PgTransaction is NULL"));
		return false;
	}

	// Cash 관련 마무리 처리
	switch(pkTran->EventType())
	{
	case CIE_CS_Buy:
		{
			PgCashShopBuyTran* pkTran = dynamic_cast<PgCashShopBuyTran*>(kTran.GetTran());
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("dynamic_cast<PgCashShopBuyTran*> is NULL"));
				break;
			}

			if (hResult == S_OK)
			{
				__int64 const i64UsedCash = pkTran->CurCash() - pkTran->InitCash();
				g_kCashShopMgr.UpdateUseCash(pkTran->CharacterGuid(), pkTran->ShopGuid(), i64UsedCash);
				g_kCashShopMgr.UpdateCashShopRank(pkTran->CharacterGuid(), CASH_LOG_FLAG, (i64UsedCash > 0 ? i64UsedCash : -i64UsedCash), 1);

				BM::Stream kPacket(PT_M_C_CS_ANS_BUY_ARTICLE);
				kPacket.Push(CSR_SUCCESS);
				kPacket.Push(pkTran->BuyType());
				g_kRealmUserMgr.Locked_SendToUser(pkTran->CharacterGuid(),kPacket,false);


				// 한정판매 갯수 업데이트
				int iItemIdx = pkTran->ItemIdx();
				if( 0 < iItemIdx )
				{
					CEL::DB_QUERY kQuery(DT_LOCAL,DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY,L"EXEC [dbo].[UP_UPDATE_CS_ITEM_LIMIT_SELL]");
					kQuery.InsertQueryTarget(pkTran->CharacterGuid());
					kQuery.PushStrParam(pkTran->MemberGuid());
					kQuery.PushStrParam(iItemIdx);
					kQuery.QueryOwner(pkTran->CharacterGuid());
					g_kCoreCenter.PushQuery(kQuery);
				}
			}
		}break;
	case CIE_CS_Gift:
		{
			PgSendGiftTran* pkTran = dynamic_cast<PgSendGiftTran*>(kTran.GetTran());
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("dynamic_cast<PgSendGiftTran*> is NULL"));
				break;
			}

			if (hResult == S_OK)
			{
				__int64 const i64UsedCash = pkTran->CurCash() - pkTran->InitCash();
				g_kCashShopMgr.UpdateUseCash(pkTran->CharacterGuid(), pkTran->ShopGuid(),i64UsedCash);
				g_kCashShopMgr.UpdateCashShopRank(pkTran->CharacterGuid(), CASH_LOG_FLAG, (i64UsedCash > 0 ? i64UsedCash : -i64UsedCash),1);

				BM::Stream kPacket(PT_M_C_CS_ANS_SEND_GIFT);
				kPacket.Push(CSR_SUCCESS);
				g_kRealmUserMgr.Locked_SendToUser(pkTran->CharacterGuid(), kPacket, false);

				{// 산타??
					SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
					pkActionOrder->InsertTarget(pkTran->CharacterGuid());
					pkActionOrder->kCause = CAE_Achievement;
					pkActionOrder->kContOrder.push_back(SPMO(IMET_ADD_ABIL,pkTran->CharacterGuid(),SPMOD_AddAbil(AT_ACHIEVEMENT_CASHSHOPGIFT,1)));
					g_kJobDispatcher.VPush(pkActionOrder);
				}

				// 한정판매 갯수 업데이트
				int iItemIdx = pkTran->ItemIdx();
				if( 0 < iItemIdx )
				{
					CEL::DB_QUERY kQuery(DT_LOCAL,DQT_CS_CASH_LIMIT_ITEM_UPDATE_QUERY,L"EXEC [dbo].[UP_UPDATE_CS_ITEM_LIMIT_SELL]");
					kQuery.InsertQueryTarget(pkTran->CharacterGuid());
					kQuery.PushStrParam(pkTran->MemberGuid());
					kQuery.PushStrParam(iItemIdx);
					kQuery.QueryOwner(pkTran->CharacterGuid());
					g_kCoreCenter.PushQuery(kQuery);
				}
			}
		}break;
	case CIE_CS_Add_TimeLimit:
		{
			PgExpandTimeLimitTran* pkTran = dynamic_cast<PgExpandTimeLimitTran*>(kTran.GetTran());
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("dynamic_cast<PgExpandTimeLimitTran*> is NULL"));
				break;
			}

			if (hResult == S_OK)
			{
				__int64 const i64UsedCash = pkTran->CurCash() - pkTran->InitCash();
				g_kCashShopMgr.UpdateUseCash(pkTran->CharacterGuid(), pkTran->ShopGuid(),i64UsedCash);
				BM::Stream kPacket(PT_M_C_CS_ANS_ADD_TIMELIMIT);
				kPacket.Push(CSR_SUCCESS);
				g_kRealmUserMgr.Locked_SendToUser(pkTran->CharacterGuid(),kPacket,false);
			}
		}break;
	case CIE_GambleMachine:
		{
			PgAddCashTran * pkGambleTran = dynamic_cast<PgAddCashTran*>(kTran.GetTran());
			if(NULL == pkGambleTran)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("dynamic_cast<PgAddCashTran*> is NULL"));
				break;
			}

			BM::Stream kAddonPacket = pkGambleTran->AddedPacket();

			BM::Stream kAnsPacket(PT_M_C_ANS_USE_GAMBLEMACHINE);

			if (S_OK == hResult)
			{
				bool bBroadcast = false;
				CONT_GAMBLEMACHINERESULT kContResult;
				PU::TLoadArray_M(kAddonPacket, kContResult);
				kAddonPacket.Pop(bBroadcast);

				if(true == bBroadcast)
				{
					BM::Stream kNotiPacket(PT_M_C_NFY_GAMBLEMACHINE_RESULT);
					kNotiPacket.Push(pkGambleTran->CharacterName());
					PU::TWriteArray_M(kNotiPacket, kContResult);
					SendToServerType(CEL::ST_CENTER,kNotiPacket);
				}

				kAnsPacket.Push(S_OK);
				PU::TWriteArray_M(kAnsPacket, kContResult);
				g_kRealmUserMgr.Locked_SendToUser(pkGambleTran->CharacterGuid(),kAnsPacket,false);

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(pkGambleTran->CharacterGuid());
				pkActionOrder->kCause = CIE_GambleMachine;

				CONT_PLAYER_MODIFY_ORDER kOrder;
				for(CONT_GAMBLEMACHINERESULT::const_iterator iter = kContResult.begin();iter != kContResult.end();++iter)
				{
					PgBase_Item const & kResultItem = (*iter);
					pkActionOrder->kContOrder.push_back(SPMO(IMET_INSERT_FIXED,pkGambleTran->CharacterGuid(),SPMOD_Insert_Fixed(kResultItem,SItemPos(), true)));
				}

				g_kJobDispatcher.VPush(pkActionOrder);
			}
			else
			{
				kAnsPacket.Push(E_GAMBLEMACHINE_NOT_ENOUGH_CASH);
				g_kRealmUserMgr.Locked_SendToUser(pkGambleTran->CharacterGuid(),kAnsPacket,false);
			}
		}break;
	}


	// Cash 공통 처리
	switch(pkTran->EventType())
	{
	case CIE_GambleMachine:
	case CIE_UM_Article_Buy:
	case CIE_CS_Buy:
	case CIE_CS_Gift:
	case CIE_CS_Add_TimeLimit:
	case CIE_Cash_Pack:
	case CIE_Coupon:
	case CIE_UM_Article_CashBack:
	case CIE_UM_Article_CashReg:
	case CIE_UM_Article_Dereg:
	case CIE_UM_Article_CashBuy:
		{
			if (S_OK == hResult)
			{
				if( !g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA) ) // 한국은 캐시 결과 오지 않음
				{
					BM::Stream kPacket(PT_I_M_CS_NOTI_CASH_MODIFY);
					kPacket.Push(pkTran->CharacterGuid());
					if (0 == (pkTran->TranState() & PgTransaction::ETranState_CashModified))
					{
						pkTran->CurCash(pkTran->InitCash());
						pkTran->CurMileage(pkTran->InitMileage());
					}
					kPacket.Push(pkTran->CurCash());
					kPacket.Push(pkTran->CurMileage());
					kPacket.Push(pkTran->CurCash() - pkTran->InitCash());
					kPacket.Push(pkTran->CurMileage() - pkTran->InitMileage());
					g_kRealmUserMgr.Locked_SendToUserGround(pkTran->CharacterGuid(), kPacket, false, true);
					// Log 남기기
					kTran.SendLog();
				}

				// Commit
				kTran.Commit();
			}
			else
			{
				// Rollback
				kTran.Rollback();
			}
			g_kRealmUserMgr.Locked_LeaveEvent(pkTran->CharacterGuid());
		}break;
	}
	return true;
}

void PgCashManager::OnCashRollback_Failed(ECashShopResult const eResult, PgTranPointer& rkTran)
{
	PgTransaction* pkTran = rkTran.GetTran();
	if (pkTran == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Transaction is NULL"));
		return;
	}

	if (pkTran->InitCash() != pkTran->CurCash())
	{
		PgLog kLog(ELOrderMain_Cash,ELOrderSub_Rollback);
		kLog.Set(0, eResult);
		kLog.Set(0, pkTran->InitCash() - pkTran->CurCash());
		kLog.Set(1, pkTran->InitCash());
		kLog.Set(2, pkTran->CurCash());
		pkTran->LogCont().Add(kLog);
	}
	if (pkTran->InitMileage() != pkTran->CurMileage())
	{
		PgLog kLog(ELOrderMain_Mileage, ELOrderSub_Rollback);
		kLog.Set(0, eResult);
		kLog.Set(0, pkTran->InitMileage() - pkTran->CurMileage());
		kLog.Set(1, pkTran->InitMileage());
		kLog.Set(2, pkTran->CurMileage());
		pkTran->LogCont().Add(kLog);
	}
	if (pkTran->AddedMileage() != 0)
	{
		PgLog kLog(ELOrderMain_Mileage,ELOrderSub_Rollback);
		kLog.Set(0, eResult);
		kLog.Set(0, pkTran->InitMileage() - pkTran->CurMileage());
		kLog.Set(1, pkTran->InitMileage());
		kLog.Set(2, pkTran->CurMileage());
		pkTran->LogCont().Add(kLog);
	}
	
	rkTran.SendLog();
}