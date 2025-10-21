#include "stdafx.h"
#include <set>
#include <algorithm>
#include "variant/PgDbCache.h"
#include "Variant/PgControlDefMgr.h"
#include "PgServerSetMgr.h"
#include "PgSendWrapper.h"
#include "JobDispatcher.h"
#include "Transaction.h"

void PgRealmUserManager::Locked_InsertMarket(PgOpenMarket const & kUserMarket)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_kOpenMarketMgr.Locked_InsertMarket(kUserMarket);
}

void PgRealmUserManager::Locked_Tick()
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_kOpenMarketMgr.Locked_Tick();
	m_kMyHomeMgr.Tick();
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_MARKET_OPEN(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	EItemModifyParentEventType		kCause;
	CONT_PLAYER_MODIFY_ORDER		kOrder;
	BM::GUID						kOwnerGuid;
	
	pkPacket->Pop(kCause);
	kOrder.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kOwnerGuid);

	std::wstring kMarketName,
				 kCharName;

	int iMarketGrade,
		iMarketOnlineTime,
		iMarketOfflineTime;

	CONT_ARTICLEINFO_EX kContArticleInfo;

	pkPacket->Pop(kMarketName);
	pkPacket->Pop(kCharName);
	pkPacket->Pop(iMarketGrade);
	pkPacket->Pop(iMarketOnlineTime);
	pkPacket->Pop(iMarketOfflineTime);
	kContArticleInfo.ReadFromPacket(*pkPacket);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MARKET_OPEN);
	kAnsPacket.Push(iMarketOnlineTime);
	kAnsPacket.Push(iMarketOfflineTime);

	kOrder.push_back(SPMO(IMET_OPEN_MARKET,kOwnerGuid,tagPlayerModifyOrderData_ModifyOpenMarket(kMarketName,kCharName,iMarketGrade,iMarketOnlineTime,iMarketOfflineTime,kContArticleInfo)));

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = kCause;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_ARTICLE_DEREG(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);
	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.ReadFromPacket(*pkPacket);

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_DEREG);
	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = CIE_UM_Article_Dereg;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_ARTICLE_REG(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	EItemModifyParentEventType kCause;
	CONT_PLAYER_MODIFY_ORDER kOrder;
	BM::GUID kOwnerGuid;

	pkPacket->Pop(kCause);
	kOrder.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kOwnerGuid);

	SUserMarketArticleInfo  kArticleInfo;
	kArticleInfo.ReadFromPacket(*pkPacket);

	BM::GUID kMemberGuid;
	pkPacket->Pop(kMemberGuid);
	std::wstring kName;
	pkPacket->Pop(kName);

	if(kArticleInfo.kItem.IsEmpty())
	{
		BM::GUID const kItemGuid( kArticleInfo.kItem.Guid() );
		if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInven(kOwnerGuid,false, kItemGuid, kArticleInfo.kItem))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("GetPlayerInven isn't S_OK, CharGuid<") << kOwnerGuid << L"> ItemGuid<" << kItemGuid << L">");
			BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_REG);
			kPacket.Push(UMR_NOT_FOUND_ITEM);
			g_kRealmUserMgr.Locked_SendToUser(kOwnerGuid, kPacket, false);
			return;
		}
	}

	kOrder.push_back(SPMO(IMET_ADD_ARTICLE,kOwnerGuid,kArticleInfo));

	if(MARKET_ARTICLE_CASHNO == kArticleInfo.kItem.ItemNo())
	{
		SContentsUser rkOut;
		SUserInfoExt kExtInfo;
		HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);	// static 함수이므로 이렇게 호출하는 것이 맞음.
		if(S_OK != hRet)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
			return;
		}
		if( !g_kRealmUserMgr.Locked_EnterEvent(kOwnerGuid) )
		{
			return;
		}

		PgTranPointer kTran(ECASH_TRAN_OPENMARKET_SELLCASH, kMemberGuid, kOwnerGuid, kGndKey, kSI, CIE_UM_Article_CashReg);
		PgOpenMarketCashRegTran* pkTran = dynamic_cast<PgOpenMarketCashRegTran*>(kTran.GetTran());
		if (pkTran != NULL)
		{
			pkTran->UID(rkOut.iUID);
			pkTran->AccountID(rkOut.kAccountID);
			pkTran->RemoteAddr(kExtInfo.kAddress);
			BM::Stream& rkPacket = pkTran->AddedPacket();
			kOrder.WriteToPacket(rkPacket);
			rkPacket.Push(kOwnerGuid);
			kArticleInfo.WriteToPacket(rkPacket);
			kTran.RequestCurerntLimitSell();
		}
	}
	else
	{
		BM::Stream kAnsPacket(PT_M_C_UM_ANS_ARTICLE_REG);
		kArticleInfo.WriteToPacket(kAnsPacket);

		SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
		pkActionOrder->InsertTarget(kOwnerGuid);
		pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
		pkActionOrder->kCause = kCause;
		pkActionOrder->kContOrder = kOrder;
		pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
		g_kJobDispatcher.VPush(pkActionOrder);
	}
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_ARTICLE_BUY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID					kMemberGuid,
								kOwnerGuid,
								kArticleGuid;
	EItemModifyParentEventType	kCause;
	BM::GUID					kMarketGuId,
								kArticleGuId;
	std::wstring				kBuyerName;
	WORD						wBuyNum;
	DWORD						dwUID;
	std::wstring				kAccountID;

	pkPacket->Pop(kMemberGuid);
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kCause);
	pkPacket->Pop(kMarketGuId);
	pkPacket->Pop(kArticleGuId);
	pkPacket->Pop(wBuyNum);
	pkPacket->Pop(kBuyerName);
	pkPacket->Pop(dwUID);
	pkPacket->Pop(kAccountID);

	SContentsUser rkOut;
	SUserInfoExt kExtInfo;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,rkOut, &kExtInfo);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
		return;
	}

	PgTranPointer kTran(ECASH_TRAN_OPENMARKET_BUY, kMemberGuid, kOwnerGuid, kGndKey, kSI, kCause);
	PgOpenMarketBuyTran* pkTran = dynamic_cast<PgOpenMarketBuyTran*>(kTran.GetTran());
	if (pkTran != NULL)
	{
		pkTran->AccountID(kAccountID);
		pkTran->UID(dwUID);
		pkTran->MarketGuid(kMarketGuId);
		pkTran->ArticleGuid(kArticleGuId);
		pkTran->BuyNum(wBuyNum);
		pkTran->BuyerName(kBuyerName);
		pkTran->AccountID(kAccountID);
		pkTran->RemoteAddr(kExtInfo.kAddress);
	}
	kTran.RequestCurerntLimitSell();
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_DEALINGS_READ(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID kOwnerGuid,kDealingGuid;
	EItemModifyParentEventType kCause;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kCause);
	pkPacket->Pop(kDealingGuid);

	CONT_PLAYER_MODIFY_ORDER kContOrder;
	SPMOD_ReadDealing kReadDealing(kDealingGuid);
	kContOrder.push_back(SPMO(IMET_REMOVE_DEALING, kOwnerGuid,kReadDealing));

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_DEALINGS_READ);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = kCause;
	pkActionOrder->kContOrder = kContOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_USE_MARKET_MODIFY_ITEM(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	EItemModifyParentEventType kCause;
	eMarketModifyItemType eType = EMMIT_NONE;
	int iOnlineTime = 0;
	int iOfflineTime = 0;
	int iHistoryPoint = 0;
	int iGrade = 0;

	CONT_PLAYER_MODIFY_ORDER kOrder;

	std::wstring kMarketName;
	BM::GUID	kOwnerGuid;
	pkPacket->Pop(kCause);
	pkPacket->Pop(kOwnerGuid);
	kOrder.ReadFromPacket(*pkPacket);
	pkPacket->Pop(eType);
	pkPacket->Pop(iOnlineTime);
	pkPacket->Pop(iOfflineTime);
	pkPacket->Pop(iHistoryPoint);
	pkPacket->Pop(iGrade);
	pkPacket->Pop(kMarketName);

	// 프리미엄 서비스 중에는 시간연장이 불가능함
	PgPremiumMgr kPremiumMgr;
	HRESULT hRet = g_kRealmUserMgr.Locked_GetPlayerInfo(kOwnerGuid,false,kPremiumMgr);
	if(S_OK != hRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! GetPlayerInfo isn't S_OK"));
		return;
	}
	bool const bPremiumService = kPremiumMgr.IsUserService(true) && (NULL != kPremiumMgr.GetType<S_PST_OpenmarketState>());
	if(bPremiumService && g_kRealmUserMgr.Locked_IsOpenMarketSameGrade(kOwnerGuid, iGrade))
	{
		iOnlineTime = 0;
		iOfflineTime = 0;
	}

	kOrder.push_back(SPMO(IMET_MODIFY_MARKET_INFO,kOwnerGuid,tagPlayerModifyOrderData_ModifyMarket(kMarketName,iOnlineTime,iOfflineTime,iHistoryPoint,iGrade)));

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_USE_MARKET_MODIFY_ITEM);
	kAnsPacket.Push(eType);
	kAnsPacket.Push(static_cast<__int64>(iOnlineTime));
	kAnsPacket.Push(static_cast<__int64>(iOfflineTime));
	kAnsPacket.Push(iHistoryPoint);
	kAnsPacket.Push(kMarketName);
	kAnsPacket.Push(iGrade);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = kCause;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_MARKET_MODIFY_STATE(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID	kOwnerGuid;
	BYTE		kState;
	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kState);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_MODIFY_MARKET_STATE,kOwnerGuid,tagPlayerModifyOrderData_ModifyMarketState(kState)));

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MARKET_MODIFY_STATE);
	kAnsPacket.Push(kState);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = CIE_UM_Modify_Market_State;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::RecvPT_M_I_UM_REQ_MARKET_CLOSE(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::GUID	kOwnerGuid;
	pkPacket->Pop(kOwnerGuid);

	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_REMOVE_MARKET,kOwnerGuid,SPMOD_RemoveMarket(kOwnerGuid)));

	BM::Stream kAnsPacket(PT_M_C_UM_ANS_MARKET_CLOSE);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kOwnerGuid);
	pkActionOrder->kGndKey.Set(kSI.nChannel, kGndKey);
	pkActionOrder->kCause = CIE_UM_Market_Remove;
	pkActionOrder->kContOrder = kOrder;
	pkActionOrder->kAddonPacket.Push(kAnsPacket.Data());
	g_kJobDispatcher.VPush(pkActionOrder);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_MARKET_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_MY_MARKET_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_MY_VENDOR_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_VENDOR_ENTER(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_VENDOR_REFRESH_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_MINIMUM_COST_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_MARKET_ARTICLE_QUERY(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvPT_M_I_UM_REQ_BEST_MARKET_LIST(kSI, kGndKey, pkPacket);
}

void PgRealmUserManager::Locked_RecvORDER_MARKET_CLOSE(BM::GUID const & kGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kOpenMarketMgr.Locked_RecvORDER_MARKET_CLOSE(kGuid);
}

EUserMarketResult PgRealmUserManager::Locked_OpenMarketVerifyArticlePrice(BM::GUID const& kBuyerGuid, BM::GUID const& kMarketGuid, WORD const wBuyCount, BM::GUID const& kArticleGuid,
	  PgOpenMarketMgr::SCostInfo& rkCost) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kOpenMarketMgr.Locked_VerifyArticlePrice(kBuyerGuid, kMarketGuid, wBuyCount, kArticleGuid, rkCost);
}

bool PgRealmUserManager::Locked_IsOpenMarketSameGrade(BM::GUID const& kOwnerGuid, int const iGrade)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kOpenMarketMgr.Locked_IsSameGrade(kOwnerGuid, iGrade);
}

EUserMarketResult PgRealmUserManager::Locked_Cash_Article_Reg(PgTranPointer const& rkTran)
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kOpenMarketMgr.Locked_Cash_Article_Reg(rkTran);
}

