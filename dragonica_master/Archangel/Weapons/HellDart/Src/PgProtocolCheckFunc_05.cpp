#include "stdafx.h"
#include "BM/Stream.h"
#include "variant/PgPlayer.h"
#include "Lohengrin/Packetstruct.h"

HRESULT CALLBACK Check_PT_C_M_CS_REQ_LAST_RECVED_GIFT(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_EXCHANGE_ITEM_ITEM(BM::Stream &kPacket)
{//OK. Lim 090408
	STradeInfoPacket kInfoPacket;
	bool bConfirmOrModify = false;
	bool bIsConfirm = false;

	if( true == kInfoPacket.ReadFromPacket(kPacket) 
	&&	true == kPacket.Pop(bConfirmOrModify) 
	&&	true == kPacket.Pop(bIsConfirm))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_EXCHANGE_ITEM_READY(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTradeGuid;
	if(true == kPacket.Pop(kTradeGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NOTI_DETECTION_HACKING(BM::Stream &kPacket)
{//OK. Lim 090408
	EDETECTION_HACK_TYPE kHackType;
	if(true == kPacket.Pop(kHackType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_ACTION(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kCasterPos;
	DWORD dwClientTime;

	if( true == kPacket.Pop(kCasterPos)
	&&	true == kPacket.Pop(dwClientTime))
	{
		if(		!kPacket.RemainSize() 
			||	(sizeof(SItemPos) == kPacket.RemainSize()) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_CHANGE(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kSourcePos;
	SItemPos kTargetPos;
	DWORD dwClientTime;
	BM::GUID kNpcGuid;
	
	if( true == kPacket.Pop( kSourcePos )
	&&	true == kPacket.Pop( kTargetPos ) 
	&&	true == kPacket.Pop( dwClientTime )
	&&	true == kPacket.Pop( kNpcGuid ))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_DISCHARGE(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos	kItemPos, kDischargeItemPos;
	if( true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kDischargeItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_DIVIDE(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	int iItemNo = 0;
	BM::GUID kItemGuid;
	int iCount = 0;
	
	if( true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(iItemNo) 
	&&	true == kPacket.Pop(kItemGuid) 
	&&	true == kPacket.Pop(iCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_MAKING(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	std::vector<SReqItemMaking> kCont;

	if( true == PU::TLoadArray_M(kPacket, kCont, MAX_ITEM_MAKING_NUM) &&
		true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SOULSTONE_TRADE(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	std::vector<SReqSoulStoneTrade> kCont;
	
	if( true == PU::TLoadArray_M(kPacket, kCont, MAX_ITEM_MAKING_NUM) &&
		true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kItemPos) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_PLUS_UPGRADE(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	SItemPos kItemPos;
	short siUseCount = 0;
	if( true == kPacket.Pop(kNpcGuid)
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kItemPos)
	&&	true == kPacket.Pop(siUseCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_REPAIR(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	CONT_REQ_ITEM_REPAIR kContRepair;

	if( true == kPacket.Pop(kNpcGuid) 
	&&	true == kPacket.Pop(kContRepair, EQUIP_POS_MAX))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_SMS(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bGenericSMS;
	std::wstring kString;
	std::wstring kChatText;

	BM::GUID kItemGuid;
	int iItemNo;
	SItemPos kItemPos;

	int const iOneItemLinkPerAddedTextCount = 27 + 9;
	if( true == kPacket.Pop(bGenericSMS) 
	&&	true == kPacket.Pop(iItemNo) 
	&&	true == kPacket.Pop(kItemGuid) 
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kChatText,(MAX_SMS_LEN + (MAX_SMS_LEN * iOneItemLinkPerAddedTextCount))) 
	&&	true == kPacket.Pop(kString,MAX_CHARACTER_NAME_LEN) 
	&&	true == kPacket.Pop(kString,MAX_MAIL_TITLE_LEN))
	{
		if( bGenericSMS )
		{
			unsigned int iCount = 0;
			if (	!kPacket.Pop( iCount )
				||	MAX_SMS_LEN < iCount )
			{
				return E_FAIL;
			}

			unsigned int iCur = 0;
			unsigned int iCount2 = 0;
			for( ; iCount > iCur; ++iCur )
			{
				if (	!kPacket.Pop( iCount2 )
					||	MAX_SMS_LEN < iCount2
					)
				{
					return E_FAIL;
				}

				unsigned int iCur2 = 0;
				for( ; iCount2 > iCur2; ++iCur2 )
				{
					std::wstring	m_kType;
					std::wstring	m_kValue;
					std::vector<BYTE>	m_kBinaryValue;

					int const iOnlySizeUse = 2;
					int const iNotUseNow = 0;
					if (	!kPacket.Pop( m_kType, iOnlySizeUse )
						||	!kPacket.Pop( m_kValue,  iNotUseNow )
						||	!kPacket.Pop( m_kBinaryValue, iOnlySizeUse )
						)
					{
						return E_FAIL;
					}
				}
			}

			int iAddValue = 0;
			if( iCount > 10 )
			{
				iAddValue = iCount - 10;
			}
		}

		if(!kPacket.RemainSize())	// 맞지 않으면 -_-;; 실패
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MACRO_INPUT_PASSWORD(BM::Stream &kPacket)
{//OK. Lim 090408
	int iCount = 0;
	if(true == kPacket.Pop(iCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MOVETOSUMMONER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID	kMemberGuid;
	SItemPos	kItemPos;
	SGroundKey	kGndKey;
	bool		bYesNo;

	if( true == kPacket.Pop(bYesNo) 
	&&	true == kPacket.Pop(kMemberGuid) 
	&&	true == kPacket.Pop(kGndKey) 
	&&	true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OXQUIZ_ANS_QUIZ(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kEventGuid;
	int kResult;
	if( true == kPacket.Pop(kEventGuid) 
	&&	true == kPacket.Pop(kResult))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OXQUIZ_ENTER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kEventGuid;
	if(true == kPacket.Pop(kEventGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PICKUPGBOX(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kBoxGuid;
	BM::GUID kLooterGuid;
	if(true == kPacket.Pop(kBoxGuid)
	&& true == kPacket.Pop(kLooterGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REGQUICKSLOT(BM::Stream &kPacket)
{//OK. Lim 090408
	size_t slot_idx;
	SQuickInvInfo kQuickInvInfo;

	if( true == kPacket.Pop(slot_idx) 
	&&	true == kPacket.Pop(kQuickInvInfo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REGQUICKSLOT_VIEWPAGE(BM::Stream &kPacket)
{//OK. Lim 090408
	char cViewPage;
	if(true == kPacket.Pop(cViewPage))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_STORE_ITEM_BUY(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kStoreGuid;
    BYTE byType;
	BYTE bySecondType;
	int iItemNo;
	int iCount;
	if( true == kPacket.Pop(kStoreGuid) 
    &&	true == kPacket.Pop(byType)
    &&	true == kPacket.Pop(bySecondType)
	&&	true == kPacket.Pop(iItemNo) 
	&&	true == kPacket.Pop(iCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_STORE_ITEM_SELL(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
    BYTE byType;
	SItemPos kItemPos;
	int iItemNo;
	int iCount;
	bool bIsStockShop;
	if( true == kPacket.Pop(kNpcGuid) 
    &&	true == kPacket.Pop(byType)
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(iItemNo) 
	&&	true == kPacket.Pop(iCount)
	&&	true == kPacket.Pop(bIsStockShop))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_TRY_TAKE_COUPON(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kStr;
	if(true == kPacket.Pop(kStr, MAX_COUPON_KEY_LEN))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_ENTER(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_EXIT(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

//============================ 추가 프로토콜 =======================================

HRESULT CALLBACK Check_PT_C_M_REQ_COUPON_EVENT_SYNC(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SYSTEM_INVENTORY_RECV(BM::Stream &kPacket)
{//OK. Lim 090408
	CONT_SYS2INV_INFO kSys2InvInfo;
	if( true == kPacket.Pop(kSys2InvInfo, MAX_SYSTEMINVEN_NUM))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SYSTEM_INVENTORY_REMOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	CONT_ITEMGUID kContItemGuid;
	if(true == kPacket.Pop(kContItemGuid, MAX_SYSTEMINVEN_NUM))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXCHANGE_ITEM_REQ(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTargetGuid;
	if(true == kPacket.Pop(kTargetGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_EXCHANGE_ITEM_ANS(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTradeGuid;
	bool bRet = false;
	if( true == kPacket.Pop(kTradeGuid)
	&&	true == kPacket.Pop(bRet))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_EXCHANGE_ITEM_QUIT(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTargetGuid;
	if(true == kPacket.Pop(kTargetGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_VIEW_OTHER_EQUIP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTargetGUID;
	if(true == kPacket.Pop(kTargetGUID))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_POST_REQ_MAIL_SEND(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	std::wstring sToName,sTitle,sText;
	BM::GUID kItemGuid;
	SItemPos kPos;
	__int64 i64Money;
	bool	bPaymentType;

	if( true == kPacket.Pop( kNpcGuid ) 
	&&	true == kPacket.Pop(sToName, MAX_CHARACTER_NAME_LEN) 
	&&	true == kPacket.Pop(sTitle, MAX_MAIL_TITLE_LEN) 
	&&	true == kPacket.Pop(sText, MAX_MAIL_TEXT_LEN) 
	&&	true == kPacket.Pop(kPos) 
	&&	true == kPacket.Pop(i64Money) 
	&&	true == kPacket.Pop(bPaymentType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_POST_REQ_MAIL_RECV(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kOwnerGuid;
	__int64 i64StartIndex;
	char	cRequestCount;

	if( true == kPacket.Pop(i64StartIndex)
	&&	true == kPacket.Pop(cRequestCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_POST_REQ_MAIL_MODIFY(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid,kMailGuid;
	EPostMailModifyType kMailModifyType;

	if( true == kPacket.Pop(kNpcGuid) 
	&&	true == kPacket.Pop(kMailGuid) 
	&&	true == kPacket.Pop(kMailModifyType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_POST_REQ_MAIL_MIN(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_ARTICLE_REG(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	__int64 i64ArticleCost;
	bool	bCostType;
	short	siMoney = 0;
	eUserMarketArticleType kCause;

	if(	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(bCostType) 
	&&	true == kPacket.Pop(i64ArticleCost)
	&&	true == kPacket.Pop(siMoney)
	&&	true == kPacket.Pop(kCause))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_ARTICLE_DEREG(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kArticleGuid;
	if(true == kPacket.Pop(kArticleGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_QUERY(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kSearchName;
	bool bSearchNameType;
	int iSearchType;
	bool bSearchTypeType;
	int iLevelMin,
		iLevelMax;
	E_ITEM_GRADE kItemGrade;
	E_ITEM_DISPLAY_GRADE kItemDisplayGrade;
	__int64 i64ClassLimit;
	bool bCostType;
	__int64 i64ArticleIndex;

	if( true == kPacket.Pop(kSearchName, MAX_OPENMARKET_SEARCH_STRING_LEN) 
	&&	true == kPacket.Pop(bSearchNameType) 
	&&	true == kPacket.Pop(iSearchType) 
	&&	true == kPacket.Pop(bSearchTypeType) 
	&&	true == kPacket.Pop(iLevelMin) 
	&&	true == kPacket.Pop(iLevelMax) 
	&&	true == kPacket.Pop(kItemGrade) 
	&&	true == kPacket.Pop(kItemDisplayGrade)
	&&	true == kPacket.Pop(i64ClassLimit) 
	&&	true == kPacket.Pop(bCostType) 
	&&	true == kPacket.Pop(i64ArticleIndex))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_ARTICLE_BUY(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kMarketGuid,kArticleGuid;
	WORD wBuyNum = 0;
	if( true == kPacket.Pop(kMarketGuid)
	&&	true == kPacket.Pop(kArticleGuid) 
	&&	true == kPacket.Pop(wBuyNum))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_DEALINGS_READ(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kDealingGuid;
	if(true == kPacket.Pop(kDealingGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MINIMUM_COST_QUERY(BM::Stream &kPacket)
{//OK. Lim 090408
	DWORD dwItemNo;
	if(true == kPacket.Pop(dwItemNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MY_MARKET_QUERY(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_OPEN(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kMarketName;			// 상점 이름 입력 하지 않으면 캐릭터 이름으로 대체된다.
	SItemPos kMarketItemPos;				// 상점 오픈 아이템 없으면? (0,0)
	CONT_ARTICLEINFO kArticles;			// 등록 요청 물품 리스트

	if( true == kPacket.Pop(kMarketName,MAX_MARKET_NAME_LEN)
	&&	true == kPacket.Pop(kMarketItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_USE_MARKET_MODIFY_ITEM(BM::Stream &kPacket)
{//OK. Lim 090408
	eMarketModifyItemType eType = EMMIT_NONE;
	SItemPos kItemPos;
	BYTE kMarketGrade = 0;
	std::wstring kMarketName;

	if( true == kPacket.Pop(eType)
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kMarketName, MAX_MARKET_NAME_LEN) 
	&&	true == kPacket.Pop(kMarketGrade))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_ARTICLE_QUERY(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kMarketGuid;
	if(true == kPacket.Pop(kMarketGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_MODIFY_STATE(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE		kState;
	if(true == kPacket.Pop(kState))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_MARKET_CLOSE(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_BUY_ARTICLE(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE bBuyType = 0;
	CONT_REQ_BUY_ARTICLE kContData;
	__int64 i64BonusUse = 0;
	BM::GUID kStoreValueKey;

	if( true == kPacket.Pop(bBuyType)
	&&	true == kPacket.Pop(kContData, MAX_BASKET_SLOT)
	&&	true == kPacket.Pop(i64BonusUse)
	&&	true == kPacket.Pop(kStoreValueKey)	)
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SIMPLE_BUY_ARTICLE(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE bBuyType = 0;
	CONT_REQ_BUY_ARTICLE kContData;
	__int64 i64BonusUse = 0;
	BM::GUID kStoreValueKey;

	if( true == kPacket.Pop(bBuyType)
	&&	true == kPacket.Pop(kContData, EQUIP_POS_MAX)
	&&	true == kPacket.Pop(i64BonusUse)
	&&	true == kPacket.Pop(kStoreValueKey)	)
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SEL_ARTICLE(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kPos;
	if(true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SEND_GIFT(BM::Stream &kPacket)
{//OK. Lim 090408
	int iArticleIdx;
	BYTE bTimeType;
	int iUseTime;
	std::wstring kRecverName,
				 kComment;
	__int64 i64BonusUse = 0;
	BM::GUID kStoreValueKey;

	if(	true == kPacket.Pop(iArticleIdx) 
	&&	true == kPacket.Pop(bTimeType) 
	&&	true == kPacket.Pop(iUseTime) 
	&&	true == kPacket.Pop(kRecverName,MAX_CHARACTER_NAME_LEN) 
	&&	true == kPacket.Pop(kComment,MAX_CASHSHOP_GIFT_COMMENT) 
	&&	true == kPacket.Pop(i64BonusUse)
	&&	true == kPacket.Pop(kStoreValueKey))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_ENTER_CASHSHOP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kStoreValueKey;

	if(	true == kPacket.Pop(kStoreValueKey) )
	{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SIMPLE_ENTER_CASHSHOP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kStoreValueKey;

	if(	true == kPacket.Pop(kStoreValueKey) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_RECV_GIFT(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGiftGuId;
	if(true == kPacket.Pop(kGiftGuId))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_MODIFY_VISABLE_RANK(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE bState;
	if(true == kPacket.Pop(bState))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_EXIT_CASHSHOP(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SIMPLE_EXIT_CASHSHOP(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_ADD_TIMELIMIT(BM::Stream &kPacket)
{//OK. Lim 090408
	int iArticleIdx = 0,
		iUseTime = 0;
	BYTE bTimeType = 0;
	SItemPos kItemPos;
	__int64 i64BonusUse = 0;
	BM::GUID kCashShopTableKey;

	if( true == kPacket.Pop(iArticleIdx) 
	&&	true == kPacket.Pop(bTimeType) 
	&&	true == kPacket.Pop(iUseTime) 
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(i64BonusUse) 
	&&	true == kPacket.Pop(kCashShopTableKey))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_CS_REQ_SIMPLE_ADD_TIMELIMIT(BM::Stream &kPacket)
{//OK. Lim 090408
	int iArticleIdx = 0,
		iUseTime = 0;
	BYTE bTimeType = 0;
	SItemPos kItemPos;
	__int64 i64BonusUse = 0;
	BM::GUID kCashShopTableKey;

	if( true == kPacket.Pop(iArticleIdx) 
	&&	true == kPacket.Pop(bTimeType) 
	&&	true == kPacket.Pop(iUseTime) 
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(i64BonusUse) 
	&&	true == kPacket.Pop(kCashShopTableKey))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ACHIEVEMENT_TO_ITEM(BM::Stream &kPacket)
{//OK. Lim 090408
	int iAchievementIdx;
	if(true == kPacket.Pop(iAchievementIdx))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USER_MAP_MOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	int iMapNo;
	SItemPos kItemPos;

	if( true == kPacket.Pop(iMapNo) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MOVETOPARTYMEMBER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kMemberGuid;
	SItemPos kItemPos;

	if( true == kPacket.Pop(kMemberGuid) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT	CALLBACK Check_PT_C_M_REQ_MOVETOPARTYMASTERGROUND(BM::Stream& kPacket)
{//OK. Lim 090408
	BM::GUID kMemberGuid;
	BM::GUID kTowerGuid;
	TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;

	if( true == kPacket.Pop(kMemberGuid) &&
		true == kPacket.Pop(kTowerGuid) &&
		true == kPacket.Pop(kTargetKey) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RENTALSAFE_EXTEND(BM::Stream &kPacket)
{//OK. Lim 090408
	int		 kInvType;
	SItemPos kItemPos;

	if( true == kPacket.Pop(kInvType) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SUMMONPARTYMEMBER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kCharGuid;
	SItemPos kItemPos;

	if( true == kPacket.Pop(kCharGuid) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OXQUIZ_EXIT(BM::Stream &kPacket)
{//OK. Lim 090408
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REMOVEITEM(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kPos;
	int iDeleteType = 0;
	if( true == kPacket.Pop(kPos) 
	&&	true == kPacket.Pop(iDeleteType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_RARITY_UPGRADE(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	EPropertyType	kPropertyType;
	bool			bUseIsuranceItem;
	bool			bUseSuccessRateItem;
	SItemPos		kItemPos;
	SItemPos		kSuccessRateItemPos;
	SItemPos		kIsuranceItemPos;

	if( true == kPacket.Pop(kNpcGuid) 
	&&	true == kPacket.Pop(kPropertyType) 
	&&	true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(bUseIsuranceItem) 
	&&	true == kPacket.Pop(kIsuranceItemPos) 
	&&	true == kPacket.Pop(bUseSuccessRateItem) 
	&&	true == kPacket.Pop(kSuccessRateItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_FIT2PRIMARYINV(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	if(true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_STORE_ITEM_LIST(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kStoreGuid;
    BYTE byType;
	if( true == kPacket.Pop(kStoreGuid)
    &&  true == kPacket.Pop(byType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_ACTION_BIND(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	if( true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ITEM_ACTION_UNBIND(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	if( true == kPacket.Pop(kItemPos) 
	&&	true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INVENTORY_EXTEND(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	EInvType kInvType;
	if( true == kPacket.Pop(kItemPos)
	&&	true == kPacket.Pop(kInvType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INVENTORY_EXTENDIDX(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kItemPos;
	EInvType kInvType;
	if( true == kPacket.Pop(kItemPos)
	&&	true == kPacket.Pop(kInvType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CREATE_CHARACTER_CARD(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos		kItemPos;
	BYTE			bYear,
					bSex;
	int				iLocal;
	BYTE			bConstellation,
					bHobby,
					bBlood,
					bStyle;
	std::wstring	kComment;

	if( true == kPacket.Pop(kItemPos)
	&&	true == kPacket.Pop(bYear)			
	&&	true == kPacket.Pop(bSex)			
	&&	true == kPacket.Pop(iLocal)			
	&&	true == kPacket.Pop(kComment, MAX_CARD_COMMENT_LEN)		
	&&	true == kPacket.Pop(bConstellation) 
	&&	true == kPacket.Pop(bHobby)			
	&&	true == kPacket.Pop(bBlood)			
	&&	true == kPacket.Pop(bStyle))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MODIFY_CHARACTER_CARD(BM::Stream &kPacket)
{
	SItemPos		kItemPos;
	BYTE			bYear,
					bSex;
	int				iLocal;
	BYTE			bConstellation,
					bHobby,
					bBlood,
					bStyle;
	std::wstring	kComment;

	if( true == kPacket.Pop(kItemPos)
	&&	true == kPacket.Pop(bYear)			
	&&	true == kPacket.Pop(bSex)							
	&&	true == kPacket.Pop(iLocal)							
	&&	true == kPacket.Pop(kComment, MAX_CARD_COMMENT_LEN)	
	&&	true == kPacket.Pop(bConstellation)					
	&&	true == kPacket.Pop(bHobby)							
	&&	true == kPacket.Pop(bBlood)							
	&&	true == kPacket.Pop(bStyle))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MODIFY_CHARACTER_CARD_COMMENT(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kString;
	if(true == kPacket.Pop(kString, MAX_CARD_COMMENT_LEN))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RECOMMEND_CHARACTER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kTargetGuid;
	if(true == kPacket.Pop(kTargetGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MODIFY_CHARACTER_CARD_STATE(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bEnable = false;
	if(true == kPacket.Pop(bEnable))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SEARCH_MATCH_CARD(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE kSex;
	BYTE kAge;
	int iLocal;

	if(	true == kPacket.Pop(kSex) 
	&&	true == kPacket.Pop(kAge) 
	&&	true ==	kPacket.Pop(iLocal))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CHARACTER_CARD_INFO(BM::Stream &kPacket)
{
	BM::GUID kGuid;

	if(	true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REG_PORTAL(BM::Stream &kPacket)
{//OK. Lim 090408
	std::wstring kComment;
	if(true == kPacket.Pop(kComment, MAX_PORTAL_COMMENT_LEN))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_PORTAL(BM::Stream &kPacket)
{//OK. Lim 090408
	SItemPos kPos;
	BM::GUID kGuid;
	DWORD dwClientTime;
	if(	true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(dwClientTime) &&
		true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GEMSTOREINFO(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	if(true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GEMSTORE_BUY(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	int iItemNo = 0;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(iItemNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SEAL_ITEM(BM::Stream &kPacket)
{
	SItemPos kPos;

	if( true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GEN_SOCKET(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	SItemPos kPos;
	int iOrder = 0;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos) &&
		kPacket.Pop(iOrder) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SET_MONSTERCARD(BM::Stream &kPacket)
{
	SItemPos kPos;
	int iItemNo = 0;
	int iItemCount = 0;
	if( true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(iItemNo) &&
		true == kPacket.Pop(iItemCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REMOVE_MONSTERCARD(BM::Stream &kPacket)
{	
	BM::GUID kNpcGuid;
	SItemPos kPos;
	int iOrder = 0;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos) &&
		kPacket.Pop(iOrder))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RESET_MONSTERCARD(BM::Stream &kPacket)
{	
	BM::GUID kNpcGuid;
	SItemPos kPos;
	int iOrder = 0;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos) &&
		kPacket.Pop(iOrder))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXTRACTION_MONSTERCARD(BM::Stream &kPacket)
{	
	BM::GUID kNpcGuid;
	SItemPos kPos;
	int iOrder = 0;
	int iCashItemCount = 0;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(iOrder) &&
		true == kPacket.Pop(iCashItemCount) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_UM_REQ_BEST_MARKET_LIST(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ROLLBACK_ENCHANT(BM::Stream &kPacket)
{
	SItemPos kPos;
	int iIdx;

	if( true == kPacket.Pop(iIdx) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OPEN_LOCKED_CHEST(BM::Stream &kPacket)
{
	SItemPos kPos;

	if( true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OPEN_GAMBLE(BM::Stream &kPacket)
{
	SItemPos kPos;

	if( true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_M_REQ_JOIN_EVENT(BM::Stream &kPacket)
{
	int iEventNo;

	if( true == kPacket.Pop(iEventNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EVENT_LIST(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CONVERTITEM(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	int iItemNo = 0,
		iConvertNum = 0;

	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(iItemNo) &&
		true == kPacket.Pop(iConvertNum))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_U_G_ABIL64_CHANGE(BM::Stream &kPacket)
{
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_U_G_ABIL_CHANGE(BM::Stream &kPacket)
{
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXIT_SAFE(BM::Stream &kPacket)
{
	bool bIsShareSafe = false;
	if(true == kPacket.Pop(bIsShareSafe))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXPCARD_ACTIVATE(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if(true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXPCARD_DEACTIVATE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXPCARD_USE(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if( true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_BUY(BM::Stream &kPacket)
{
	bool bTemp = false;
	short sBuildingNo = 0;

	if( true == kPacket.Pop(bTemp)
	&&	true == kPacket.Pop(sBuildingNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_ENTER(BM::Stream &kPacket)
{
	short siValue;
	int iValue;
	if( true == kPacket.Pop(siValue) &&
		true == kPacket.Pop(iValue))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_EXIT(BM::Stream &kPacket)
{
	BM::GUID kHomeGuid;
	bool bIsReturnRecentPos;

	if( true == kPacket.Pop(kHomeGuid) &&
		true == kPacket.Pop(bIsReturnRecentPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_INFO(BM::Stream &kPacket)
{
	SHOMEADDR kAddr;
	BYTE bType = 0;
	if( true == kPacket.Pop(kAddr) &&
		true == kPacket.Pop(bType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_REALTYDEALER(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	if( true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MIXUPITEM(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	SItemPos kItemPos;
	if( true == kPacket.Pop(kGuid)		&&
		true == kPacket.Pop(kItemPos)	&&
		true == kPacket.Pop(kItemPos)	&&
		true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOMETOWN_ENTER(BM::Stream &kPacket)
{
	int iValue = 0;

	if( true == kPacket.Pop(iValue) &&
		true == kPacket.Pop(iValue))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOMETOWN_EXIT(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_EQUIP(BM::Stream &kPacket)
{
	BM::GUID	kGuid;
	SItemPos	kItemPos;
	POINT3		kLocalPos;
	short		siDir;
	BM::GUID kParentGuid;
	BYTE bLinkIdx = 0;

	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kLocalPos) &&
		true == kPacket.Pop(siDir) &&
		true == kPacket.Pop(kParentGuid) &&
		true == kPacket.Pop(bLinkIdx) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_UNEQUIP(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	SItemPos kPos;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXCHANGE_LOGCOUNTTOEXP(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_VISITLOG_ADD(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	std::wstring kString;
	CONT_ITEMPOS kCont;
	bool bPrivate;

	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kString,MAX_HOME_VISITLOG_LEN) &&
		true == kPacket.Pop(bPrivate) &&
		true == PU::TLoadArray_A(kPacket,kCont,MAX_MYHOME_BUFF_NUM))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_VISITLOG_LIST(BM::Stream &kPacket)
{
	BM::GUID kHomeGuid;
	if( true == kPacket.Pop(kHomeGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_VISITLOG_DELETE(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_USE_ITEM_EFFECT(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	SItemPos kPos;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_VISITFLAG_MODIFY(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	BYTE kValue;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(kValue))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_AUCTION_REG(BM::Stream &kPacket)
{
	__int64 i64Cost;
	int		iAuctionHour;

	if( true == kPacket.Pop(i64Cost) &&
		true == kPacket.Pop(iAuctionHour))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_BIDDING(BM::Stream &kPacket)
{
	short siValue;
	int iHouseNo;
	__int64 i64Cost;

	if( true == kPacket.Pop(siValue) &&
		true == kPacket.Pop(iHouseNo) &&
		true == kPacket.Pop(i64Cost))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_AUCTION_UNREG(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOMETOWN_INFO(BM::Stream &kPacket)
{
	BYTE bType = 0;
	if(true == kPacket.Pop(bType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_PAY_TEX(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_POST_INVITATION_CARD(BM::Stream &kPacket)
{
	std::wstring kName;
	if(true == kPacket.Pop(kName,MAX_CHARACTER_NAME_LEN))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_INVITATION_CARD(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_VISITORS(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_ITEM_MODIFY(BM::Stream &kPacket)
{
	BM::GUID kHomeGuid;
	SItemPos kItemPos;
	POINT3 kPos;
	short kDir;
	BM::GUID kParentGuid;
	BYTE bLinkIdx = 0;

	if( true == kPacket.Pop(kHomeGuid) &&
		true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kPos) &&
		true == kPacket.Pop(kDir) &&
		true == kPacket.Pop(kParentGuid) &&
		true == kPacket.Pop(bLinkIdx))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HOME_USE_HOME_STYLEITEM(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	DWORD dwClientTime;
	BM::GUID kHomeGuid;

	if( true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(dwClientTime) &&
		true == kPacket.Pop(kHomeGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_HOME_REQ_MAPMOVE(BM::Stream &kPacket)
{
	int iGroundNo = 0;
	if( true == kPacket.Pop(iGroundNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_GAMBLEMACHINE(BM::Stream &kPacket)
{
	SItemPos kPos;
	if( true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_GAMBLEMACHINE_READY(BM::Stream &kPacket)
{
	SItemPos kPos;
	if( true == kPacket.Pop(kPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RELOAD_ROULETTE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ROULETTE_RESULT(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_GAMBLEMACHINE_MIXUP_READY(BM::Stream &kPacket)
{
	CONT_ITEMPOS kContPos;
	if( true == PU::TLoadArray_A(kPacket, kContPos,MAX_GAMBLEMACHINE_MIXUP_NUM))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RELOAD_ROULETTE_MIXUP(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ROULETTE_MIXUP_RESULT(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GAMBLEMACHINEINFO(BM::Stream &kPacket)
{
	bool kIsCashShop = false;
	if(true == kPacket.Pop(kIsCashShop))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_GAMBLEMACHINE_CASH(BM::Stream &kPacket)
{
	BYTE bCoinType = 0;
	if(true == kPacket.Pop(bCoinType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_LUCKYSTAR_ENTER(BM::Stream &kPacket)
{
	BM::GUID kEventGuid;
	int iSelectStar = 0;
	if( true == kPacket.Pop(kEventGuid) &&
		true == kPacket.Pop(iSelectStar) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_LUCKYSTAR_CHANGE_STAR(BM::Stream &kPacket)
{
	BM::GUID kEventGuid;
	int iSelectStar = 0;
	if( true == kPacket.Pop(kEventGuid) &&
		true == kPacket.Pop(iSelectStar) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_STORE_CLOSE(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_REPAIR_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SORT_ITEM(BM::Stream &kPacket)
{
	EInvType kInvType;
	if( true == kPacket.Pop(kInvType))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_ENCHANT_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_TELEPORT_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	DWORD dwClientTime;
	POINT3 kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(dwClientTime) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_STATUS_CHANGE(BM::Stream& kPacket)
{
	VEC_ABILINFO kAbil;
	if (true == kPacket.Pop(kAbil, 4))
	{
		if (!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_REPAIR_MAX_DURATION_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_REDICE_OPTION_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_UPGRADE_OPTION_ITEM(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	int iIdx = 0;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos) &&
		true == kPacket.Pop(iIdx))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_REDICE_OPTION_PET(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kCasterPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_BASIC_OPTION_AMP(BM::Stream &kPacket)
{
	SItemPos kTargetPos;
	SItemPos kInsurancePos;
	if( true == kPacket.Pop(kTargetPos) &&
		true == kPacket.Pop(kInsurancePos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RESET_ATTATCHED(BM::Stream &kPacket)
{
	SItemPos kItemPos,kTargetPos;

	if( true == kPacket.Pop(kItemPos) &&
		true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SKILL_EXTEND(BM::Stream &kPacket)
{
	SItemPos kTargetPos;
	int iSkillNo;
	if( true == kPacket.Pop(kTargetPos) &&
		true == kPacket.Pop(iSkillNo))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ACHIEVEMENT_RANK(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_HIDDEN_MOVE_CHECK(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	DWORD dwClientTime;

	if( true == kPacket.Pop(kCasterPos)
	&&	true == kPacket.Pop(dwClientTime))
	{
		if(		!kPacket.RemainSize() 
			||	(sizeof(SItemPos) == kPacket.RemainSize()) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_NPC_TALK(BM::Stream &kPacket)
{
	BM::GUID kGuid;

	if( true == kPacket.Pop(kGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_START_SIDE_JOB(BM::Stream &kPacket)
{
	eMyHomeSideJob kSideJob;
	int iTime = 0;

	if( true == kPacket.Pop(kSideJob) &&
		true == kPacket.Pop(iTime))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CANCEL_SIDE_JOB(BM::Stream &kPacket)
{
	eMyHomeSideJob kSideJob;

	if( true == kPacket.Pop(kSideJob))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENTER_SIDE_JOB(BM::Stream &kPacket)
{
	short siValue;
	if( true == kPacket.Pop(siValue) &&
		true == kPacket.Pop(siValue))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_SELL(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_ENTER(BM::Stream &kPacket)
{
	BM::GUID kRoomGuid;
	BM::GUID kNpcGuid;
	std::wstring kPassword;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kRoomGuid) &&
		true == kPacket.Pop(kPassword,MAX_MACRO_PASSWORD_LEN))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_EXIT(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	if( true == kPacket.Pop(kNpcGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_MODIFY_ROOM(BM::Stream &kPacket)
{
	std::wstring	kRoomTitle,
					kRoomNoti,
					kRoomPassword;
	size_t kMaxUserCount = 0;

	BM::GUID kNpcGuid;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kRoomTitle) &&
		true == kPacket.Pop(kRoomNoti) &&
		true == kPacket.Pop(kRoomPassword) &&
		true == kPacket.Pop(kMaxUserCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_MODIFY_GUEST(BM::Stream &kPacket)
{
	bool bEnablePrivateMsg;
	bool bNotStanding;
	BM::GUID kNpcGuid;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(bEnablePrivateMsg) &&
		true == kPacket.Pop(bNotStanding))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_ROOM_LIST(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	if( true == kPacket.Pop(kNpcGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_ROOM_CREATE(BM::Stream &kPacket)
{
	std::wstring	kRoomTitle,
					kRoomNoti,
					kRoomPassword;
	size_t kMaxUserCount = 0;

	BM::GUID kNpcGuid;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kRoomTitle) &&
		true == kPacket.Pop(kRoomNoti) &&
		true == kPacket.Pop(kRoomPassword) &&
		true == kPacket.Pop(kMaxUserCount))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_SET_ROOMMASTER(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	BM::GUID kTargetGuid;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kTargetGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MYHOME_CHAT_KICK_GEUST(BM::Stream &kPacket)
{
	BM::GUID kNpcGuid;
	BM::GUID kTargetGuid;
	if( true == kPacket.Pop(kNpcGuid) &&
		true == kPacket.Pop(kTargetGuid))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_ENCHANT_SHIFT(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	for(int i = 0;i < ESIP_NUM;++i)
	{
		if(false == kPacket.Pop(kItemPos))
		{
			return E_FAIL;
		}
	}

	bool bOption = false;

	if( true == kPacket.Pop(bOption) &&
		true == kPacket.Pop(bOption) &&
		true == kPacket.Pop(bOption))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFGEMSTOREINFO(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	int iMenu = 0;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(iMenu) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFGEMSTORE_BUY(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	int iItemNo = 0;
	int iMenu = 0;
	int iOrderIndex = 0;
	if( true == kPacket.Pop(kGuid) &&
		true == kPacket.Pop(iItemNo) &&
		true == kPacket.Pop(iMenu) &&
		true == kPacket.Pop(iOrderIndex))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_COLLECT_ANTIQUE(BM::Stream &kPacket)
{
	BM::GUID kOwner;
	int iMenu = 0;
	SItemPos ItemPos;
	int iItemNo = 0;
	int iIndex = 0;

	if( true == kPacket.Pop(kOwner)
		&& true == kPacket.Pop(iMenu)
		&& true == kPacket.Pop(ItemPos)
		&& true == kPacket.Pop(iItemNo)
		&& true == kPacket.Pop(iIndex) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EXCHANGE_GEMSTORE(BM::Stream &kPacket)
{
	int MenuType = 0;
	SItemPos kItemPos;
	int iTargetItemNo = 0;
	if( true == kPacket.Pop(MenuType)
		&& true == kPacket.Pop(kItemPos)
		&& true == kPacket.Pop(iTargetItemNo) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SOULTRANSFER_EXTRACT(BM::Stream &kPacket)
{
	SItemPos kTargetPos;
	if( true == kPacket.Pop(kTargetPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SOULTRANSFER_TRANSITION(BM::Stream &kPacket)
{
	SItemPos kCasterPos;
	SItemPos kTargetPos;
	bool bInsurance;
	int iRateUpItemNum;
	if( true == kPacket.Pop(kCasterPos) 
	 && true == kPacket.Pop(kTargetPos)
	 && true == kPacket.Pop(bInsurance)
	 && true == kPacket.Pop(iRateUpItemNum) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OPEN_TREASURE_CHEST(BM::Stream &kPacket)
{
	SItemPos kChestPos, kKeyPos;
	DWORD dwClientTime;
	if( true == kPacket.Pop(kChestPos) &&
		true == kPacket.Pop(kKeyPos) &&
		true == kPacket.Pop(dwClientTime)
		)
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TREASURE_CHEST_INFO(BM::Stream &kPacket)
{
	DWORD dwClientTime;
	if(true == kPacket.Pop(dwClientTime) && !kPacket.RemainSize())
	{
		return S_OK;
	}

	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_M_REQ_ELEMENT_EXTRACT(BM::Stream &kPacket)
{
	SItemPos kItemPos;
	if( true == kPacket.Pop(kItemPos))
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT CALLBACK Check_PT_C_M_REQ_ELEMENT_GROUND_MOVE(BM::Stream &kPacket)
{
	int iSuperGroundNo = 0;
	int iSuperGroundMode = 0;
	if( true == kPacket.Pop(iSuperGroundNo))
	{
		if( true == kPacket.Pop(iSuperGroundMode))
		{
			if(!kPacket.RemainSize())
			{
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_USERINFO(BM::Stream &kPacket)
{
	if(!kPacket.RemainSize())
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_MANUFACTURE(BM::Stream &kPacket)
{
	SItemPos kSrcItemPos;
	BYTE byScore;
	if( true == kPacket.Pop(kSrcItemPos) && true == kPacket.Pop(byScore) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_BUNDLE_MANUFACTURE(BM::Stream &kPacket)
{
	int iItemNo = 0;
	int iCount = 0;
	if( true == kPacket.Pop(iItemNo) && true == kPacket.Pop(iCount) )
	{
		if(!kPacket.RemainSize())
		{
			return S_OK;
		}
	}

	return E_FAIL;
}
