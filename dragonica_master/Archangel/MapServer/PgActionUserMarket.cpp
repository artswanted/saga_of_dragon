#include "stdafx.h"
#include "variant/Global.h"
#include "Lohengrin/VariableContainer.h"
#include "PgAction.h"
#include "PgActionUserMarket.h"
#include "Global.h"
#include "constant.h"

#ifdef _DEBUG
#pragma warning (push, 4)
#endif
/*
	CIE_UM_Article_Reg,		//경매 물품 등록
	CIE_UM_Article_Dereg,	//등록된 경매 취소
	CIE_UM_Article_Query,	//경매 물품 조회
	CIE_UM_Article_Buy,		//경매 물품 구입
	CIE_UM_Dealing_Query,	//경매 내역 조회
*/

PgUMReqArticleReg::PgUMReqArticleReg(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

	//판매자 GUID
	//아이템 수정 타입
	//판매자 이름
	//판매 물품 GUID
	//판매 물품 위치
	//판매 가격 타입
	//판매 가격
	//판매 아이템 번호 (TB_DefItem:ItemNo)
	//판매 아이템 이름 (TB_DefItem:Memo)
	//판매 아이템 등급 (GetItemGrade()) 사용
	//판매 아이템 레벨 (TB_DefItem:LevelLimit)
	//판매 아이템 직업군 (TB_DefItem:ClassLimit) bitflag 사용중
	//판매 물품 대분류 (무기,방어구 등등..TB_DefItem:Type)
	//판매 무룸 소분류 (한손검,양손검 등등...TB_DefItem:Attribute)

EUserMarketResult PgUMReqArticleReg::OnProcess(CUnit* pkCaster)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if(!pkPlayer)
	{
		return UMR_DB_ERROR;
	}

	SItemPos kItemPos;
	__int64 i64ArticleCost;
	bool	bCostType;
	short					siMoney = 0;
	eUserMarketArticleType	kArticleType = UMAT_ITEM;

	m_krPacket.Pop(kItemPos);
	m_krPacket.Pop(bCostType);
	m_krPacket.Pop(i64ArticleCost);
	m_krPacket.Pop(siMoney);
	m_krPacket.Pop(kArticleType);

	if(i64ArticleCost <= 0)		// 판매 금액을 0 이거나 그보다 작게 넣을수 없다.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_NEED_COST"));
		return UMR_NEED_COST;
	}

	std::wstring kItemName;
	CONT_PLAYER_MODIFY_ORDER kOrder;
	PgInventory * pkInv = pkCaster->GetInven();
	PgBase_Item kItem;

	switch(kArticleType)
	{
	case UMAT_ITEM:
		{
			if(S_OK != pkInv->GetItem(kItemPos, kItem))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_NOT_FOUND_ITEM"));
				return UMR_NOT_FOUND_ITEM;
			}
			kOrder.push_back(SPMO(IMET_MODIFY_POS, pkCaster->GetID(),SPMOD_Modify_Pos(SItemPos(IT_USER_MARKET,0),kItemPos,PgBase_Item::NullData(),kItem)));
		}break;
	case UMAT_CASH:
		{
			if((0 >= siMoney) || MAX_MARKET_ARITCLE_MONEY_COUNT < siMoney)
			{
				return UMR_TOO_MANY_COST_VALUE;
			}
			kItem.ItemNo(MARKET_ARTICLE_CASHNO);
			kItem.Count(siMoney);
			bCostType = false; // 캐시는 게임 머니로만 팔수 있다.
			kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkCaster->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(IT_USER_MARKET,0))));
		}break;
	case UMAT_MONEY:
		{
			__int64 const i64Money = (siMoney * iGoldToBronze);

			if((0 >= i64Money) || (pkInv->Money() < i64Money) || (MAX_MARKET_ARITCLE_MONEY_COUNT < siMoney))
			{
				return UMR_TOO_MANY_COST_VALUE;
			}

			kItem.ItemNo(MARKET_ARTICLE_MONEYNO);
			kItem.Count(siMoney);
			bCostType = true; // 게임 머니는 캐시로만 팔수 있다.

			kOrder.push_back(SPMO(IMET_ADD_MONEY,pkCaster->GetID(), SPMOD_Add_Money(-i64Money)));
			kOrder.push_back(SPMO(IMET_INSERT_FIXED, pkCaster->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(IT_USER_MARKET,0))));
		}break;
	default:
		{
			return UMR_INVALID_ITEM;
		}break;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(	!pkItemDef || false == CheckEnableTrade(kItem,ICMET_Cant_Auction))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
		return UMR_INVALID_ITEM;
	}

	::GetItemName(kItem.ItemNo(),kItemName);

	BM::Stream kPacket(PT_M_I_UM_REQ_ARTICLE_REG);

	kPacket.Push(CIE_UM_Article_Reg);
	kOrder.WriteToPacket(kPacket);
	kPacket.Push(pkCaster->GetID());

	SUserMarketArticleInfo kArticleInfo;
	kArticleInfo.kArticleCost = i64ArticleCost;
	kArticleInfo.kClassLimit = pkItemDef->GetAbil64(AT_CLASSLIMIT);
	kArticleInfo.kCostType = bCostType;
	kArticleInfo.kEquipPos = pkItemDef->EquipPos();
	kArticleInfo.kGrade = GetItemGrade(kItem);
	kArticleInfo.kDisplayGrade = pkItemDef->GetAbil(AT_ITEM_DISPLAY_GRADE);
	kArticleInfo.kInvType = pkItemDef->PrimaryInvType();
	kArticleInfo.kItemGuId = kItem.Guid();
	kArticleInfo.kItemName = kItemName;
	kArticleInfo.kLevelLimit = pkItemDef->GetAbil(AT_LEVELLIMIT);
	if(UMAT_ITEM == kArticleType)
	{
		kArticleInfo.kItem.Guid(kItem.Guid());
	}
	else
	{
		kArticleInfo.kItem = kItem;
	}

	if(kArticleInfo.kArticleCost > (kArticleInfo.kCostType == 0 ? MARKET_GAME_COST_MAX : MARKET_CASH_COST_MAX))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_TOO_MANY_COST_VALUE"));
		return UMR_TOO_MANY_COST_VALUE;
	}

	kArticleInfo.WriteToPacket(kPacket);

	kPacket.Push(pkPlayer->GetMemberGUID());
	kPacket.Push(pkPlayer->Name());
	
	if(!SendToItem(m_kGndKey,kPacket))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_NET_ERROR"));
		return UMR_NET_ERROR;
	}

	return UMR_SUCCESS;
}

bool PgUMReqArticleReg::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EUserMarketResult kErr = OnProcess(pkCaster);
	if(UMR_SUCCESS == kErr)
		return true;
	
	BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_REG);
	kPacket.Push(kErr);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgUMReqArticleDereg::PgUMReqArticleDereg(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqArticleDereg::DoAction(CUnit* pkCaster,CUnit*)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kArticleGuid;
	m_krPacket.Pop(kArticleGuid);

	BM::Stream kPacket(PT_M_I_UM_REQ_ARTICLE_DEREG);
	CONT_PLAYER_MODIFY_ORDER kOrder;
	kOrder.push_back(SPMO(IMET_REMOVE_ARTICLE,pkPlayer->GetID(),SPlayerModifyOrderData_ModifyRemoveArticle(kArticleGuid,pkPlayer->GetMemberGUID(),pkPlayer->Name())));
	kPacket.Push(pkPlayer->GetID());
	kOrder.WriteToPacket(kPacket);
	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqArticleQuery::PgUMReqArticleQuery(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqArticleQuery::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_MARKET_QUERY);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(pkCaster->GetAbil(AT_LEVEL));
	kPacket.Push(m_krPacket);	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqMyMarketQuery::PgUMReqMyMarketQuery(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqMyMarketQuery::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_MY_MARKET_QUERY);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(m_krPacket);	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqMyVendorQuery::PgUMReqMyVendorQuery(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqMyVendorQuery::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_MY_VENDOR_QUERY);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(m_krPacket);	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqVendorEnter::PgUMReqVendorEnter(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqVendorEnter::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_VENDOR_ENTER);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(m_krPacket);	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqArticleBuy::PgUMReqArticleBuy(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqArticleBuy::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kMarketGuid,kArticleGuid;
	WORD wBuyNum = 0;
	m_krPacket.Pop(kMarketGuid);
	m_krPacket.Pop(kArticleGuid);
	m_krPacket.Pop(wBuyNum);

	BM::Stream kPacket(PT_M_I_UM_REQ_ARTICLE_BUY);

	kPacket.Push(pkPlayer->GetMemberGUID());
	kPacket.Push(pkPlayer->GetID());
	kPacket.Push(CIE_UM_Article_Buy);
	kPacket.Push(kMarketGuid);
	kPacket.Push(kArticleGuid);
	kPacket.Push(wBuyNum);
	kPacket.Push(pkPlayer->Name());
	kPacket.Push(pkPlayer->UID());
	kPacket.Push(pkPlayer->MemberID());
	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqDealingsRead::PgUMReqDealingsRead(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqDealingsRead::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::GUID kDealingGuid;
	m_krPacket.Pop(kDealingGuid);

	BM::Stream kPacket(PT_M_I_UM_REQ_DEALINGS_READ);

	kPacket.Push(pkCaster->GetID());
	kPacket.Push(CIE_UM_Dealing_Read);
	kPacket.Push(kDealingGuid);
	
	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqQueryMinimumCost::PgUMReqQueryMinimumCost(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

bool PgUMReqQueryMinimumCost::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_MINIMUM_COST_QUERY);
	DWORD dwItemNo;
	m_krPacket.Pop(dwItemNo);

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(dwItemNo);
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	kPacket.Push(pkCaster->GetID());
	kPacket.Push(dwItemNo);

	return SendToItem(m_kGndKey,kPacket);
}

PgUMReqMarketOpen::PgUMReqMarketOpen(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_krPacket(kPacket)
{
}

EUserMarketResult PgUMReqMarketOpen::Process(CUnit* pkCaster)
{
	PgInventory * pkInv = pkCaster->GetInven();

	SPT_C_M_UM_REQ_MARKET_OPEN kReadData;
	kReadData.ReadFromPacket(m_krPacket);

	GET_DEF(CItemDefMgr, kItemDefMgr);

	int	iMarketGrade = MG_NORMAL;
	int iMarketOnlineTime = 0;
	int iMarketOfflineTime = 0;

	CONT_PLAYER_MODIFY_ORDER kOrder;

	PgBase_Item kItem;
	if(SUCCEEDED(pkInv->GetItem(kReadData.MarketItemPos(), kItem)))
	{
		CItemDef const * pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if(!pkItemDef)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
			return UMR_INVALID_ITEM;
		}

		iMarketGrade = pkItemDef->GetAbil(AT_MARKET_OPEN_TYPE);

		if(MG_NORMAL == iMarketGrade)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
			return UMR_INVALID_ITEM;
		}

		iMarketOnlineTime = pkItemDef->GetAbil(AT_MARKET_ONLINE_TIME);
		iMarketOfflineTime = pkItemDef->GetAbil(AT_MARKET_OFFLINE_TIME);

		SPMOD_Modify_Count kModifyCount(kItem,kReadData.MarketItemPos(),-1);
		kOrder.push_back(SPMO(IMET_MODIFY_COUNT,pkCaster->GetID(),kModifyCount));
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_MARKET_OPEN);
	kPacket.Push(CIE_UM_Market_Open);
	kOrder.WriteToPacket(kPacket);
	kPacket.Push(pkCaster->GetID());

//-------------------------------------------------------------------------------------
	if(kReadData.MarketName().length() <= 0)
	{
		kPacket.Push(pkCaster->Name());
	}
	else
	{
		kPacket.Push(kReadData.MarketName());
	}
	kPacket.Push(pkCaster->Name());
	kPacket.Push(iMarketGrade);
	kPacket.Push(iMarketOnlineTime);
	kPacket.Push(iMarketOfflineTime);
//-------------------------------------------------------------------------------------
	
	if(!SendToItem(m_kGndKey,kPacket))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_DB_ERROR"));
		return UMR_DB_ERROR;
	}

	return UMR_SUCCESS;
}

bool PgUMReqMarketOpen::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EUserMarketResult kError = Process(pkCaster);

	if(UMR_SUCCESS == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_UM_ANS_MARKET_OPEN);
	kPacket.Push(kError);
	pkCaster->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgUMReqUseMarketModifyItem::PgUMReqUseMarketModifyItem(SGroundKey const & kGndKey, BM::Stream & kPacket)
: m_kGndKey(kGndKey),m_rkPacket(kPacket)
{
}

EUserMarketResult PgUMReqUseMarketModifyItem::Process(CUnit* pkCaster)
{
	eMarketModifyItemType eType = EMMIT_NONE;
	SItemPos kItemPos;
	std::wstring kMarketName;
	BYTE kMarketGrade = 0;
	m_rkPacket.Pop(eType);
	m_rkPacket.Pop(kItemPos);
	m_rkPacket.Pop(kMarketName);
	m_rkPacket.Pop(kMarketGrade);

	if(kMarketGrade > MG_HISTORY)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
		return UMR_INVALID_ITEM;
	}

	PgInventory * pkInv = pkCaster->GetInven();

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_PLAYER_MODIFY_ORDER kOrder;

	int iMarketOnlineTime = 0;
	int iMarketOfflineTime = 0;
	int iMarketHistoryPoint = 0;
	int iMarketGrade = kMarketGrade;
	int iItemGrade = 0;

	if(MG_NORMAL==kMarketGrade)
	{
		g_kVariableContainer.Get(EVar_Kind_OpenMarket, EVar_OpenMarket_NormalMarketOfflineTime, iMarketOfflineTime);
	}
	else
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if(pkPlayer)
		{
			if(pkPlayer->GetPremium().GetType<S_PST_OpenmarketState>())
			{
				__int64 const kEndSec = pkPlayer->GetPremium().GetEndSecTime();
				iMarketOnlineTime = kEndSec;
				iMarketOfflineTime = kEndSec;
			}
		}
	}

	if(kItemPos != SItemPos::NullData())
	{
		PgBase_Item kItem;
		if(S_OK != pkInv->GetItem(kItemPos, kItem))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
			return UMR_INVALID_ITEM;
		}

		CItemDef const * pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if(!pkItemDef)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
			return UMR_INVALID_ITEM;
		}

		iMarketOnlineTime = pkItemDef->GetAbil(AT_MARKET_ONLINE_TIME);
		iMarketOfflineTime = pkItemDef->GetAbil(AT_MARKET_OFFLINE_TIME);
		iMarketHistoryPoint = pkItemDef->GetAbil(AT_MARKET_HISTORY_POINT);
		
		iItemGrade = pkItemDef->GetAbil(AT_MARKET_OPEN_TYPE);

		if(iMarketOnlineTime + iMarketOfflineTime)
		{
			if(iItemGrade != iMarketGrade)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
				return UMR_INVALID_ITEM;
			}
		}
		else
		{
			if(iMarketHistoryPoint <= 0 || MG_NORMAL == iMarketGrade)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_INVALID_ITEM"));
				return UMR_INVALID_ITEM;
			}
		}

		SPMOD_Modify_Count kModifyCount(kItem,kItemPos,-1);
		kOrder.push_back(SPMO(IMET_MODIFY_COUNT,pkCaster->GetID(),kModifyCount));
	}

	BM::Stream kPacket(PT_M_I_UM_REQ_USE_MARKET_MODIFY_ITEM);
	kPacket.Push(CIE_UM_Modify_Market);
	kPacket.Push(pkCaster->GetID());
	kOrder.WriteToPacket(kPacket);
	kPacket.Push(eType);
	kPacket.Push(iMarketOnlineTime);
	kPacket.Push(iMarketOfflineTime);
	kPacket.Push(iMarketHistoryPoint);
	kPacket.Push(iMarketGrade);
	kPacket.Push(kMarketName);

	if(!SendToItem(m_kGndKey,kPacket))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return UMR_DB_ERROR"));
		return UMR_DB_ERROR;
	}

	return UMR_SUCCESS;
}

bool PgUMReqUseMarketModifyItem::DoAction(CUnit* pkCaster,CUnit*)
{
	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EUserMarketResult kError = Process(pkCaster);

	if(UMR_SUCCESS == kError)
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
