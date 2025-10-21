#include "stdafx.h"
//#include "constant.h"
//#include "PgRecvFromUser.h"
#include "Variant/Item.h"
#include "Variant/ItemBagMgr.h"
#include "Variant/ItemMakingDefMgr.h"
#include "PgAction.h"
#include "PgActionAchievement.h"

void ReadPacketForItemMaking(CONT_REQ_ITEM_MAKING& rkItemMaking, BM::Stream &rkPacket)
{
	int iSize = 0;
	rkPacket.Pop(iSize);
	while ( iSize-- )
	{
		SReqItemMaking kReqItemMaking;
		kReqItemMaking.ReadFromPacket(rkPacket);
		rkItemMaking.push_back(kReqItemMaking);
	}
}

PgAction_ItemMaking::PgAction_ItemMaking(SGroundKey const &kGndKey, BM::Stream* pkPacket)
	:	m_kGndKey(kGndKey),m_kPacket(*pkPacket), m_kItemDefMgr(g_kControlDefMgr.GetCItemDefMgr())
{
	// Fill the Req Item Making container
}

bool PgAction_ItemMaking::CreateResultCookingItem(CUnit* pkCaster, TBL_DEF_COOKING const& kMakingData, bool const bUseOptionItem, CONT_RESULT_ITEM_DATA& kContResultItems, int& iWarnMessage)
{
	if( !pkCaster )
	{
		return false;
	}

	int iTotalRate = 0;
	for(int i = 0; i < TBL_DEF_COOKING::MAX_RESULT; ++i)
	{
		int const iRetItemNo = kMakingData.kResult[i].iItemNo;
		int const iRetItemCount = kMakingData.kResult[i].iCount;
		int const iRate = bUseOptionItem ? kMakingData.kResult[i].iOptionRate : kMakingData.kResult[i].iRate;
		if(iRetItemNo && iRetItemCount && iRate > 0)
		{
			iTotalRate += iRate;
		}
	}

	if(iTotalRate)
	{
		int const iResultRate = BM::Rand_Index(iTotalRate);

		int iAccRate = 0;
		for(int i = 0; i < TBL_DEF_COOKING::MAX_RESULT; ++i)
		{
			int const iRetItemNo = kMakingData.kResult[i].iItemNo;
			int const iRetItemCount = kMakingData.kResult[i].iCount;
			int const iRate = bUseOptionItem ? kMakingData.kResult[i].iOptionRate : kMakingData.kResult[i].iRate;
			if(iRetItemNo && iRetItemCount && iRate > 0)
			{
				iAccRate += iRate;//누적값이 넘어 서는 순간. 간다.
				if( iAccRate > iResultRate )
				{
					CItemDef const *pRetkItemDef = m_kItemDefMgr.GetDef(iRetItemNo);
					if(!pRetkItemDef)
					{
						iWarnMessage = 1502;
						return false;
					}

					PgBase_Item kRetItem;

					CONT_RESULT_ITEM_DATA::iterator ret_itor = kContResultItems.find(iRetItemNo);

					if(	ret_itor != kContResultItems.end()//이미 있었고
					&&	pRetkItemDef->IsAmountItem())//수량 아아팀이면
					{//있으니 있던거에 내구 추가를 하던지 하자.(성능 때문에 그러함)
						(*ret_itor).second.Count((*ret_itor).second.Count()+iRetItemCount);//3만개 넘으면 진짜 곤란.
					}
					else
					{//없으니 새로 만들어 넣고
						if(S_OK == ::CreateSItem(iRetItemNo, iRetItemCount, GIOT_NONE, kRetItem))
						{
							kContResultItems.insert(std::make_pair(kRetItem.ItemNo(), kRetItem));
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool PgAction_ItemMaking::CreateResultItem(CUnit* pkCaster, int const iMakingType, SDefItemMakingData const& kMakingData, int const iAddSuccessRate, int const iRareRecipeItemNo, CONT_RESULT_ITEM_DATA& kContResultItems, int& iWarnMessage)
{
	if( !pkCaster )
	{
		return false;
	}

	bool bCreateSuccess = false;
	size_t iRetArrayIndex = 0;
	if(::RouletteRate(kMakingData.iSuccesRateControlNo, iRetArrayIndex, MAX_ITEMMAKING_ARRAY,iAddSuccessRate))
	{
		int iRetElementIndex = 0;
		while(MAX_SUCCESS_RATE_ARRAY > iRetElementIndex)
		{//필요아이템 해싱.
			int const iRetItemNo = kMakingData.akResultItemElements[iRetArrayIndex].aElement[iRetElementIndex];//
			int const iRetItemCount = kMakingData.akResultItemCount[iRetArrayIndex].aCount[iRetElementIndex];//
		
			if(iRetItemNo)
			{
				bCreateSuccess = bCreateSuccess || true; // 아이템 번호가 있으면 생성 성공이다
				CItemDef const *pRetkItemDef = m_kItemDefMgr.GetDef(iRetItemNo);
				if(!pRetkItemDef)
				{
					iWarnMessage = 1502;
					return false;
				}

				PgBase_Item kRetItem;

				CONT_RESULT_ITEM_DATA::iterator ret_itor = kContResultItems.find(iRetItemNo);

				if(	ret_itor != kContResultItems.end()//이미 있었고
				&&	pRetkItemDef->IsAmountItem())//수량 아아팀이면
				{//있으니 있던거에 내구 추가를 하던지 하자.(성능 때문에 그러함)
					(*ret_itor).second.Count((*ret_itor).second.Count()+iRetItemCount);//3만개 넘으면 진짜 곤란.
				}
				else
				{//없으니 새로 만들어 넣고

					int const iRarityControlNo = kMakingData.iRarityControlNo;	//레어 컨트롤 번호가 없으면 그냥 영력 0이다.

					if(S_OK == ::CreateSItem(iRetItemNo, iRetItemCount, iRarityControlNo, kRetItem))
					{
						kContResultItems.insert(std::make_pair(kRetItem.ItemNo(), kRetItem));

						CONT_GAMBLE_SHOUT_ITEM const * pkDefShoutItem = NULL;
						g_kTblDataMgr.GetContDef(pkDefShoutItem);
						if((iMakingType & EMAKING_TYPE_MAZE_ITEM) == EMAKING_TYPE_MAZE_ITEM && pkDefShoutItem)
						{
							CONT_GAMBLE_SHOUT_ITEM::const_iterator kShoutItem = pkDefShoutItem->find(CONT_GAMBLE_SHOUT_ITEM::key_type(iRareRecipeItemNo,iRetItemNo));
							if(kShoutItem != pkDefShoutItem->end())
							{
								PgBase_Item kTmpItem = kRetItem;

								SEnchantInfo kEnchant = kTmpItem.EnchantInfo();
								kEnchant.IsCurse(0);
								kEnchant.IsSeal(0);
								kTmpItem.EnchantInfo(kEnchant);

								E_ITEM_GRADE kGrade = GetItemGrade(kTmpItem);

								if(kGrade >= (*kShoutItem).second)
								{
									BM::Stream kNoti(PT_M_C_NFY_ITEM_MAKING_SUCCESS);
									kNoti.Push(pkCaster->Name());
									kNoti.Push(kRetItem.ItemNo());
									SendToItem(m_kGndKey,kNoti);
								}
							}
						}
					}
				}
			}
			++iRetElementIndex;
		}
		++iRetArrayIndex;
	}
	return bCreateSuccess;
}

bool PgAction_ItemMaking::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	CONT_REQ_ITEM_MAKING m_kContItemMaking;
	ReadPacketForItemMaking(m_kContItemMaking,m_kPacket);

	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	SItemPos kInsurancePos;
	m_kPacket.Pop(kInsurancePos);

	int iWarnMessage = 0;

	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(true == pkCaster->IsDead())
	{
		pkCaster->SendWarnMessage(10410);
		return false;
	}

	PgInventory *pInv = pkCaster->GetInven();
	if(!pInv)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if(!m_kContItemMaking.size()// 없거나.
	||	m_kContItemMaking.size() > 10)//10개보다 많은 메이킹은 실패해라.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kResultPacket;	// Result 때 쓰이는 패킷.

	CONT_PLAYER_MODIFY_ORDER kOrder;

	int iAddSuccessRate = 0;

	PgBase_Item kAddRateItem;
	if((SItemPos::NullData() != kItemPos) && S_OK == pInv->GetItem(kItemPos,kAddRateItem))
	{
		CItemDef const* pItemDef = m_kItemDefMgr.GetDef(kAddRateItem.ItemNo());
		if(pItemDef && (UICT_MAKING_SUCCESS == pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)))
		{
			iAddSuccessRate = pItemDef->GetAbil(AT_SUCCESSRATE);
			kOrder.push_back(SPMO(IMET_MODIFY_COUNT, pkCaster->GetID(), SPMOD_Modify_Count(kAddRateItem,kItemPos,-1)));
		}
	}

	int iInsuranceType = 0;

	PgBase_Item kInsuranceItem;
	if((SItemPos::NullData() != kInsurancePos) && S_OK == pInv->GetItem(kInsurancePos,kInsuranceItem))
	{
		CItemDef const* pItemDef = m_kItemDefMgr.GetDef(kInsuranceItem.ItemNo());
		if(pItemDef)
		{
			iInsuranceType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
		}
	}

	CONT_RESULT_ITEM_DATA::const_iterator result_itor;
	CONT_NEED_ITEMS::const_iterator needs_itor;
	////
	// 전혀 다른 종류/내용의 제조를 여러개 할 수도 있다고 생각! (헌데 거의 안할듯..)

	CONT_NEED_ITEMS kContNeedItems;
	CONT_RESULT_ITEM_DATA kContResultItems;
	__int64 iTotalNeedMoney = 0;

	int iMakingType = EMAKING_TYPE_NONE;
	int iMakingSuccessCount = 0;

	bool bUseInsurance = false;

	CONT_REQ_ITEM_MAKING::const_iterator itr = m_kContItemMaking.begin();
	while(itr != m_kContItemMaking.end())
	{
		SReqItemMaking const& rkReqItemMaking = (*itr);
		// 메이킹 넘버 * 갯수 => 필요 아이템 수량을 총합산.
		// 합산된 아이템이 있는지 확인.
		// 적절한 처리.

		if(EMAKING_TYPE_NEW_COOKING == rkReqItemMaking.iMakingType)
		{
			iMakingType = rkReqItemMaking.iMakingType;
			if(S_OK != ProcessCooking(pkCaster, rkReqItemMaking, iTotalNeedMoney, kContNeedItems, kContResultItems, iMakingSuccessCount, iWarnMessage))
			{
				goto __SEND_RESULT_FAILED;
			}
		}
		else
		{
			if(S_OK != Process(pkCaster, rkReqItemMaking, iAddSuccessRate, iInsuranceType, bUseInsurance, iMakingType, iTotalNeedMoney, kContNeedItems, kContResultItems, iMakingSuccessCount, iWarnMessage))
			{
				goto __SEND_RESULT_FAILED;
			}
		}

		++itr;
	}

	if(true == bUseInsurance)
	{
		kOrder.push_back(SPMO(IMET_MODIFY_COUNT, pkCaster->GetID(), SPMOD_Modify_Count(kInsuranceItem,kInsurancePos,-1)));
	}

	if(iTotalNeedMoney)
	{
		__int64 const iMoney = pkCaster->GetAbil64(AT_MONEY);
		if(iTotalNeedMoney > iMoney )
		{//Not Enough Money
			iWarnMessage = 700036;
			goto __SEND_RESULT_FAILED;
		}
		else
		{
			SPMOD_Add_Money kDelData(-iTotalNeedMoney);//필요머니 빼기.
			SPMO kIMO(IMET_ADD_MONEY, pkCaster->GetID(), kDelData);
			kOrder.push_back(kIMO);
		}
	}
	
	needs_itor = kContNeedItems.begin();
	while(needs_itor != kContNeedItems.end())
	{//내가 다 갖고 있는지 확인.
		CONT_NEED_ITEMS::key_type const &kNeedItemNo = (*needs_itor).first;
		CONT_NEED_ITEMS::mapped_type const &kNeedItemCount = (*needs_itor).second;
		size_t const now_count = pInv->GetTotalCount(kNeedItemNo);

		if(int(now_count) < kNeedItemCount)
		{
			iWarnMessage = 1502;
			goto __SEND_RESULT_FAILED;
		}

		if( 60000960 == kNeedItemNo )
		{//악마의 영혼석 사용 개수에 따른 업적
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_SOULSTONE, kNeedItemCount, m_kGndKey );
			kMA.DoAction( pkCaster, NULL );
		}

		SPMOD_Add_Any kDelData(kNeedItemNo, -kNeedItemCount);
		SPMO kIMO(IMET_ADD_ANY, pkCaster->GetID(), kDelData);
		kOrder.push_back(kIMO);

		++needs_itor;
	}

	result_itor = kContResultItems.begin();
	while(result_itor != kContResultItems.end())
	{//결과물 지급.
		CONT_RESULT_ITEM_DATA::mapped_type const &kItem = (*result_itor).second;

		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
		if(pItemDef)
		{
			int iItemCount = kItem.Count();
			if( pItemDef->IsAmountItem())//소비형일 경우
			{
				while( 0 != iItemCount)
				{//수량이 1보다 많으면 쪼개서 하나씩 보내자.(겹치면 안되는 아이템이 겹쳐지는 문제 방지)
					CONT_RESULT_ITEM_DATA::mapped_type kCopyItem = kItem;
					kCopyItem.Count(1);
					SPMOD_Insert_Fixed kAddData(kCopyItem, SItemPos(), true);//
					SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddData);
					kOrder.push_back(kIMO);
					--iItemCount;
				}
			}
			else
			{//장비형이면 그대로 보내자.
				SPMOD_Insert_Fixed kAddData(kItem, SItemPos(), true);//
				SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddData);
				kOrder.push_back(kIMO);
			}
		}
		
		++result_itor;
	}
	
	PU::TWriteTable_MM(kResultPacket, kContResultItems);
	kResultPacket.Push(iMakingType);
	kResultPacket.Push(iMakingSuccessCount);
	kResultPacket.Push(bUseInsurance);

	goto __SEND_RESULT;
__SEND_RESULT:
	{
		if( 0 < iMakingSuccessCount )
		{
			SPMOD_AddRankPoint kAddRank( E_RANKPOINT_FOODMAKE, iMakingSuccessCount );// 랭킹 올려.
			kOrder.push_back(SPMO(IMET_ADD_RANK_POINT, pkCaster->GetID(), kAddRank));
		}
		
		PgAction_ReqModifyItem kAction(CIE_Make, m_kGndKey, kOrder, kResultPacket);
		kAction.DoAction(pkCaster, NULL);
		return true;
	}
__SEND_RESULT_FAILED:
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if (pkPlayer)
		{
			pkPlayer->SendWarnMessage(iWarnMessage);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

HRESULT PgAction_ItemMaking::Process(CUnit* pkCaster, SReqItemMaking const& rkReqItemMaking, int const& iAddSuccessRate, int const& iInsuranceType, bool& bUseInsurance, int& iMakingType, __int64& iTotalNeedMoney, CONT_NEED_ITEMS& kContNeedItems, CONT_RESULT_ITEM_DATA& kContResultItems, int& iMakingSuccessCount, int& iWarnMessage)
{
	SDefItemMakingData kMakingData;
	GET_DEF(CItemMakingDefMgr, kItemMakingDefMgr);
	bool const bRet = kItemMakingDefMgr.GetDefItemMakingInfo(kMakingData, rkReqItemMaking.iMakingNo);
	if(	!bRet 
	|| (rkReqItemMaking.iMakingCount <= 0))
	{
		iWarnMessage = 1502;
		return E_FAIL;
	}

	int iRareRecipeItemNo = 0;
	int iMakingCount = 0;

	CONT_NEED_ITEMS kContRecipe;

	while(rkReqItemMaking.iMakingCount > iMakingCount)
	{//결과물을 확률에 맞춰서.

		iTotalNeedMoney += kMakingData.iNeedMoney;//필요머니 누적.

		int iNeedIndex = 0;
		while(MAX_SUCCESS_RATE_ARRAY > iNeedIndex)
		{//필요아이템 해싱.
			int const iNeedItemNo = kMakingData.kNeedElements.aElement[iNeedIndex];//
			int const iNeedItemCount = kMakingData.kNeedCount.aCount[iNeedIndex];//

			if(iNeedItemNo)
			{
				CItemDef const *pkItemDef = m_kItemDefMgr.GetDef(iNeedItemNo);
				if(pkItemDef)
				{
					int const iType = pkItemDef->GetAbil(AT_MAKING_TYPE);
					iMakingType |= iType;

					int const iCustomType = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);

					if(((iType & EMAKING_TYPE_MAZE_ITEM) == EMAKING_TYPE_MAZE_ITEM) || (UICT_SKILL_EXTEND == pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)))
					{
						kContRecipe[iNeedItemNo] += iNeedItemCount;
						iRareRecipeItemNo = iNeedItemNo;

						switch(iInsuranceType)
						{
						case UICT_MAKING_INSURANCE:
							{
								if((iType & EMAKING_TYPE_MAZE_ITEM) == EMAKING_TYPE_MAZE_ITEM)
								{
									bUseInsurance = true;
								}
							}break;
						case UICT_SKILLEXTEND_INSURANCE:
							{
								if(UICT_SKILL_EXTEND == iCustomType)
								{
									bUseInsurance = true;
								}
							}break;
						default:
							{
								bUseInsurance = false;
							}break;
						}
					}
					else
					{
						kContNeedItems[iNeedItemNo] += iNeedItemCount;
					}
				}
			}
			++iNeedIndex;
		}

		if( CreateResultItem(pkCaster, iMakingType, kMakingData, iAddSuccessRate, iRareRecipeItemNo, kContResultItems, iWarnMessage) )
		{
			++iMakingSuccessCount;
		}
		else
		{
			if( 0 != iWarnMessage )
			{
				return E_FAIL;
			}

			if(true == bUseInsurance)
			{
				CONT_NEED_ITEMS().swap(kContRecipe);
			}
		}

		kContNeedItems.insert(kContRecipe.begin(),kContRecipe.end());
		++iMakingCount;
	}
	return S_OK;
}

HRESULT PgAction_ItemMaking::ProcessCooking(CUnit* pkCaster, SReqItemMaking const& rkReqItemMaking, __int64& iTotalNeedMoney, CONT_NEED_ITEMS& kContNeedItems, CONT_RESULT_ITEM_DATA& kContResultItems, int& iMakingSuccessCount, int& iWarnMessage)
{
	if(rkReqItemMaking.iMakingCount <= 0)
	{
		iWarnMessage = 1502;
		return E_FAIL;
	}

	CONT_DEFCOOKING const* pkDefCooking = NULL;
	g_kTblDataMgr.GetContDef(pkDefCooking);
	if(NULL == pkDefCooking)
	{
		iWarnMessage = 1502;
		return E_FAIL;
	}

	CONT_DEFCOOKING::const_iterator cooking_it = pkDefCooking->find(rkReqItemMaking.iMakingNo);
	if(cooking_it == pkDefCooking->end())
	{
		iWarnMessage = 1502;
		return E_FAIL;
	}

	PgInventory *pInv = pkCaster->GetInven();

	CONT_DEFCOOKING::mapped_type const& kMakingData = (*cooking_it).second;
	CONT_NEED_ITEMS kContHaveOptionItem;	//first:itemNo, second:인벤에 가지고 있는 아이템 수량

	int iMakingCount = 0;
	while(iMakingCount < rkReqItemMaking.iMakingCount)
	{
		iTotalNeedMoney += kMakingData.iNeedMoney;//필요머니 누적.

		for(int i = 0; i < TBL_DEF_COOKING::MAX_NEEDITEM; ++i)
		{
			int const iNeedItemNo = kMakingData.kNeedItem[i].iItemNo;
			int const iNeedItemCount = kMakingData.kNeedItem[i].iCount;

			if(iNeedItemNo > 0 && m_kItemDefMgr.GetDef(iNeedItemNo))
			{
				kContNeedItems[iNeedItemNo] += iNeedItemCount;
			}
		}

		//옵션이 있는지 체크
		bool bUseOptionRate = false;
		if(rkReqItemMaking.bUseOptionItem)
		{
			int const iOptItemNo = kMakingData.kOptionItem.iItemNo;
			int const iOptItemCount = kMakingData.kOptionItem.iCount;
			if(iOptItemNo && iOptItemCount)
			{
				auto ret = kContHaveOptionItem.insert(std::make_pair(iOptItemNo, pInv->GetTotalCount(iOptItemNo)));
				CONT_NEED_ITEMS::mapped_type & kCount = ret.first->second;

				kCount -= iOptItemCount;
				if(kCount >= 0)
				{
					kContNeedItems[iOptItemNo] += iOptItemCount;
					bUseOptionRate = true;
				}
			}
		}

		if( CreateResultCookingItem(pkCaster, kMakingData, bUseOptionRate, kContResultItems, iWarnMessage) )
		{
			++iMakingSuccessCount;
		}
		else
		{
			if( 0 != iWarnMessage )
			{
				return E_FAIL;
			}
		}
		++iMakingCount;
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////
// 미션 악마의 영혼석 거래

PgAction_SoulStoneTrade::PgAction_SoulStoneTrade(SGroundKey const &kGndKey, BM::Stream* pkPacket)
	:	m_kGndKey(kGndKey), m_kPacket(*pkPacket), m_kItemDefMgr(g_kControlDefMgr.GetCItemDefMgr())
{
}

bool PgAction_SoulStoneTrade::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	int iCount;
	m_kPacket.Pop(iCount);
	SReqSoulStoneTrade kReqSoulStoneTrade;
	kReqSoulStoneTrade.ReadFromPacket(m_kPacket);

	SItemPos kItemPos;
	m_kPacket.Pop(kItemPos);

	SItemPos kInsurancePos;
	m_kPacket.Pop(kInsurancePos);

	int iWarnMessage = 0;

	if(!pkCaster)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(true == pkCaster->IsDead())
	{
		pkCaster->SendWarnMessage(10410);
		return false;
	}

	PgInventory *pInv = pkCaster->GetInven();
	if(!pInv)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::Stream kResultPacket;	// Result 때 쓰이는 패킷.

	CONT_PLAYER_MODIFY_ORDER kOrder;

	int iAddSuccessRate = 0;

	PgBase_Item kAddRateItem;
	if((SItemPos::NullData() != kItemPos) && S_OK == pInv->GetItem(kItemPos,kAddRateItem))
	{
		CItemDef const* pItemDef = m_kItemDefMgr.GetDef(kAddRateItem.ItemNo());
		if(pItemDef && (UICT_MAKING_SUCCESS == pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE)))
		{
			iAddSuccessRate = pItemDef->GetAbil(AT_SUCCESSRATE);
			kOrder.push_back(SPMO(IMET_MODIFY_COUNT, pkCaster->GetID(), SPMOD_Modify_Count(kAddRateItem,kItemPos,-1)));
		}
	}

	int iInsuranceType = 0;

	PgBase_Item kInsuranceItem;
	if((SItemPos::NullData() != kInsurancePos) && S_OK == pInv->GetItem(kInsurancePos,kInsuranceItem))
	{
		CItemDef const * pItemDef = m_kItemDefMgr.GetDef(kInsuranceItem.ItemNo());
		if(pItemDef)
		{
			iInsuranceType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
		}
	}

	CONT_RESULT_ITEM_DATA kContResultItems;
	int iTotalMaterialNeedsCount = kReqSoulStoneTrade.iMaterialNeedsCount * kReqSoulStoneTrade.iTradeCount; // 한 개 제작시 필요한 재료 수 * 총 제작할 아이템 수
	{
		CItemDef const *pRetkItemDef = m_kItemDefMgr.GetDef(kReqSoulStoneTrade.iItemNo);
		if(!pRetkItemDef)
		{
			iWarnMessage = 1502;
			goto __SEND_RESULT_FAILED;
		}

		
		PgBase_Item kRetItem;
		if(S_OK != ::CreateSItem(kReqSoulStoneTrade.iItemNo, kReqSoulStoneTrade.iTradeCount, GIOT_MAKING, kRetItem))
		{
			goto __SEND_RESULT_FAILED;
		}

		size_t const now_count = pInv->GetTotalCount(kReqSoulStoneTrade.iMaterialNo);
		if(int(now_count) < iTotalMaterialNeedsCount)
		{
			iWarnMessage = 1502;
			goto __SEND_RESULT_FAILED;
		}

		kContResultItems.insert(std::make_pair(kRetItem.ItemNo(), kRetItem));

		//악마의 영혼석 사용 개수에 따른 업적
		PgAddAchievementValue kMA( AT_ACHIEVEMENT_USE_SOULSTONE, iTotalMaterialNeedsCount, m_kGndKey );
		kMA.DoAction( pkCaster, NULL );

		SPMOD_Add_Any kDelData(kReqSoulStoneTrade.iMaterialNo, -iTotalMaterialNeedsCount);
		SPMO kIMO(IMET_ADD_ANY, pkCaster->GetID(), kDelData);
		kOrder.push_back(kIMO);


		//결과물 지급.
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kRetItem.ItemNo());
		if(pItemDef)
		{
			int iItemCount = kRetItem.Count();
			if( pItemDef->IsAmountItem())//소비형일 경우
			{
				while( 0 != iItemCount)
				{//수량이 1보다 많으면 쪼개서 하나씩 보내자.(겹치면 안되는 아이템이 겹쳐지는 문제 방지)
					CONT_RESULT_ITEM_DATA::mapped_type kCopyItem = kRetItem;
					kCopyItem.Count(1);
					SPMOD_Insert_Fixed kAddData(kCopyItem, SItemPos(), true);//
					SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddData);
					kOrder.push_back(kIMO);
					--iItemCount;
				}
			}
		}

		PU::TWriteTable_MM(kResultPacket, kContResultItems);
		kResultPacket.Push(EMAKING_TYPE_SOULSTONE);
		kResultPacket.Push(1);
		kResultPacket.Push(false);
	}

	goto __SEND_RESULT;
__SEND_RESULT:
	{
		if( 0 < kReqSoulStoneTrade.iTradeCount )
		{
			SPMOD_AddRankPoint kAddRank( E_RANKPOINT_FOODMAKE, kReqSoulStoneTrade.iTradeCount );// 랭킹 올려.
			kOrder.push_back(SPMO(IMET_ADD_RANK_POINT, pkCaster->GetID(), kAddRank));
		}
		
		PgAction_ReqModifyItem kAction(CIE_Make, m_kGndKey, kOrder, kResultPacket);
		kAction.DoAction(pkCaster, NULL);
		return true;
	}

__SEND_RESULT_FAILED:
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if (pkPlayer)
		{
			pkPlayer->SendWarnMessage(iWarnMessage);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}