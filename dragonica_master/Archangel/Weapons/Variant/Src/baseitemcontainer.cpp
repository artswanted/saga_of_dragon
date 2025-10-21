#include "stdafx.h"
#include "BaseItemContainer.h"

#include "Variant/PgDbCache.h"
#include "Variant/PgControlDefMgr.h"
#include "Variant/pgitemrarityupgradeformula.h"
#include "Variant/PgParty.h"
#include "Variant/PgLogUtil.h"
#include "variant/PgPlayer.h"
#include "variant/Global.h"

PgBaseItemContainer::~PgBaseItemContainer()
{
}

int PgBaseItemContainer::GetItemEnchantLevel(SItemPos const& rkPos)
{
	CONT_ITEM_WRAPPER::const_iterator find_iter = find(rkPos);
	if( end() != find_iter )
	{
		return static_cast< int >((*find_iter).second.EnchantInfo().PlusLv());
	}
	return 0;
}

void PgBaseItemContainer::MakeSortOrderSub(CUnit const * const pkUnit,CONT_ITEM_WRAPPER const & kOrg,CONT_ITEM_WRAPPER const & kCopy, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	for(CONT_ITEM_WRAPPER::const_iterator iter = kOrg.begin();iter != kOrg.end();++iter)
	{
		CONT_ITEM_WRAPPER::const_iterator copy_iter = kCopy.find((*iter).first);
		
		if(copy_iter == kCopy.end() || (*copy_iter).second.PgBase_Item::operator != ((*iter).second))
		{
			rkContModifyOrder.push_back(SPMO(IMET_MODIFY_COUNT|IMC_POS_BY_SORT, pkUnit->GetID(), SPMOD_Modify_Count((*iter).second, (*iter).first, 0, true)));
		}
	}

	for(CONT_ITEM_WRAPPER::const_iterator iter = kCopy.begin();iter != kCopy.end();++iter)
	{
		CONT_ITEM_WRAPPER::const_iterator org_iter = kOrg.find((*iter).first);
		
		if(org_iter == kOrg.end() || (*org_iter).second.PgBase_Item::operator != ((*iter).second))
		{
			rkContModifyOrder.push_back(SPMO(IMET_INSERT_FIXED|IMC_POS_BY_SORT, pkUnit->GetID(), SPMOD_Insert_Fixed((*iter).second, (*iter).first, false)));
		}
	}
}

HRESULT PgBaseItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	return S_OK;
}

HRESULT PgBaseItemContainer::WriteToPacket(BM::Stream &kPacket)const// 이따가 가상함수로 변경
{	
	CONT_ITEM_WRAPPER::const_iterator item_itor = begin();

	kPacket.Push(size());
	while(item_itor != end())
	{
		kPacket.Push( (*item_itor).second.Pos() );
		(*item_itor).second.PgBase_Item::WriteToPacket(kPacket);
		++item_itor;
	}
	
	kPacket.Push(m_kReduction);
	kPacket.Push(m_kExtendMaxIdx);

	return S_OK;
}

HRESULT PgBaseItemContainer::ReadFromPacket(BM::Stream &kPacket)// 이따가 가상함수로 변경
{
	size_t const_size = 0;
	kPacket.Pop(const_size);

	while(const_size--)
	{
		SItemPos kPos;
		PgBase_Item kItem;

		kPacket.Pop(kPos);
		kItem.ReadFromPacket(kPacket);

		auto ret = insert(std::make_pair(kPos, PgItemWrapper(kItem, kPos)));

		if(!ret.second)
		{
			assert(NULL);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		}
	}

	kPacket.Pop(m_kReduction);
	kPacket.Pop(m_kExtendMaxIdx);

	return S_OK;
}

HRESULT PgBaseItemContainer::MergeItem(PgItemWrapper &kCasterWrapper, PgItemWrapper &kTargetWrapper, __int64& iCause)
{//실제 머징이 일어났을때만 S_OK 리턴
	if(	kCasterWrapper.IsEmpty()
	&&	kTargetWrapper.IsEmpty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	int const iCasterItemNo = kCasterWrapper.ItemNo();
	int const iTargetItemNo = kTargetWrapper.ItemNo();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(iCasterItemNo);

	HRESULT hReturn = E_INCORRECT_ITEM;
	if(	pItemDef
	&&	iCasterItemNo == iTargetItemNo
	&&	pItemDef->IsAmountItem()
	&&	kCasterWrapper.EnchantInfo() == kTargetWrapper.EnchantInfo() )//속성이 같아야 함.
	{
		int const iMaxAmount = (int)pItemDef->MaxAmount();

		PgBase_Item kCasterItem = kCasterWrapper;
		PgBase_Item kTargetItem = kTargetWrapper;

		int const iRemain = iMaxAmount - kCasterItem.Count();//

		if(iRemain == 0)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
			return E_INCORRECT_ITEM;
		}
		else if(iRemain > 0)
		{//남은 갯수.
			int const iTargetCount = kTargetItem.Count();
			int const iCasterCount = kCasterItem.Count();
			if(iTargetCount > iRemain)
			{//캐스터가 많으면
				kTargetItem.Count(iTargetCount-iRemain);
				kCasterItem.Count(iCasterCount+iRemain);
			}
			else
			{//캐스터가 지워져야지.
				kTargetItem.Count(0);
				kCasterItem.Count(iCasterCount+iTargetCount);//캐스터 카운트 만큼 추가.
			}

			kTargetWrapper.SetItem(kTargetItem);
			kCasterWrapper.SetItem(kCasterItem);

			// Merge되었으면, iCause에 표시해 주면
			// OverWriteItem 내부에서 없어진 아이템을 실제로 없애 준다.
			iCause |= IMD_MERGED_SUCCESS;
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
	return hReturn;
}

HRESULT PgBaseItemContainer::InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	HRESULT hReturn = E_FAIL;
	SItemPos kItemPos = kItemWrapper.Pos();

	if(bToEmptyPos)
	{//빈공간 찾아 넣으라고 하면.
		size_t last_idx = 0;
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(pItemDef)
		{
			if(pItemDef->IsAmountItem())
			{//겹치기 아이템은 머지를 해서 처리.
				while(last_idx != GetMaxIDX())
				{
					SItemPos const kPos((BYTE)InvType(), (BYTE)last_idx);
					CONT_ITEM_WRAPPER::iterator found_itor = find(kPos);

					if(found_itor != end())
					{
						PgItemWrapper kCastgerWrapper = (*found_itor).second;
						__int64 iCause2 = iCause;
						if(S_OK == (hReturn = MergeItem(kCastgerWrapper, kItemWrapper, iCause2)))
						{//머징 되면 되나부다 하면 됨.  kItemWrapper-> 갯수가 깎이거나 할테니까.
							if(	S_OK != (hReturn = OverWriteItem(iCause2, kCastgerWrapper, kChangeArray,kContLogMgr)))
							{
								TCHAR chMessage[500];
								_stprintf_s(chMessage, 500, _T("0x%08x"), hReturn);
								CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Can't Merge insert Failed ErrorCode[") << chMessage << _T("]") );
								assert(NULL);
							}
						}
					}

					if(PgBase_Item::IsEmpty(&kItemWrapper))
					{
						return S_OK;
					}

					++last_idx;
				}
			}
		}

		if(S_OK != (hReturn = EmptyPos(kItemPos)))//빈자리 받고 거기다 밀자.
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_ENOUGH_INV_SPACE"));
			return E_NOT_ENOUGH_INV_SPACE;
		}
	}

	kItemWrapper.Pos(kItemPos);
	return OverWriteItem(iCause, kItemWrapper, kChangeArray, kContLogMgr);
}

HRESULT PgBaseItemContainer::OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//아이템이 지워질때는 절대 OverWriteItem을 사용 하면 안됨. PopItem을 사용할 것
	HRESULT hReturn = E_INVALID_ITEMPOS;
	if(	kItemWrapper.Pos().x == InvType()
	&&	kItemWrapper.Pos().y >= 0 
	&&	kItemWrapper.Pos().y < GetMaxIDX() )
	{
		if(PgBase_Item::IsEmpty( &kItemWrapper))//빈 아이템을 덮을 경우
		{
			PgItemWrapper kTempItemWrapper;
			
			HRESULT const hRet = PopItem(iCause, kItemWrapper.Pos(), kTempItemWrapper, true, kChangeArray, kContLogMgr);
			if(S_OK == hRet
			&& !PgBase_Item::IsEmpty( &kTempItemWrapper)
			&& (iCause & IMD_MERGED_SUCCESS) != 0)	// 실제로 Merge된 아이템이면, 어느 한곳의 아이템을 없애 줘야 할 경우가 발생한다.
			{// 아이템을 뽑아 냈는데 실제 뽑은놈이 있을때만.
//				PgItemWrapper kBlankItemWrapper;
//				DB_ITEM_STATE_CHANGE kItemChange(DISCT_REMOVE, iCause, OwnerGuid(), kTempItemWrapper, kBlankItemWrapper); 
//				kChangeArray.push_back(kItemChange);
			}
			return hRet;
		}

		if( S_OK == (hReturn = CanModify(kItemWrapper)) )
		{
			CONT_ITEM_WRAPPER::iterator found_itor = find(kItemWrapper.Pos());
			
			if(found_itor!= end())
			{//자리에 아이템이 있던경우면.

				if(!(	iCause & IMET_MODIFY_POS	
			//	||	iCause & IMET_MODIFY_COUNT
			//	||	iCause & IMET_MODIFY_ENCHANT
				||	iCause & IMC_POS_BY_EXCHANGE))
				{
					if((*found_itor).second.Guid() != kItemWrapper.Guid())
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
						return E_INVALID_ITEMPOS;
					}
				}

				DB_ITEM_STATE_CHANGE kItemChange(DISCT_MODIFY, iCause, OwnerGuid(), CharacterGuid(), (*found_itor).second, kItemWrapper); 
				kChangeArray.push_back(kItemChange);

				(*found_itor).second = kItemWrapper;

				{
					PgItemWrapper const & kOldItem = kItemChange.PrevItemWrapper();
					PgItemWrapper const & kCurItem = kItemChange.ItemWrapper();
					PgItemLogUtil::ModifyLog(OwnerGuid(),kContLogMgr,kCurItem,kOldItem);
				}

				return S_OK;
			}
			else
			{//빈자리.
				EDBItemStateChangeType eStateChangeType = DISCT_CREATE;

				if ( UIT_STATE_PET == kItemWrapper.State() )
				{
					PgItem_PetInfo *pkPetInfo = NULL;
					if ( true == kItemWrapper.GetExtInfo( pkPetInfo ) )
					{
						BYTE byFlag = pkPetInfo->SaveFlag();
						if ( PgItem_PetInfo::SVFL_HATCHING & byFlag )
						{
							byFlag &= ~PgItem_PetInfo::SVFL_HATCHING;
							pkPetInfo->SaveFlag( byFlag );
							eStateChangeType = DISCT_HATCH_PET;
						}
					}
				}

				auto ret = insert(std::make_pair(kItemWrapper.Pos(), kItemWrapper));
				if(ret.second)
				{
					PgItemWrapper kBlankWrapper;
					kBlankWrapper.Pos(kItemWrapper.Pos());
				//	빈자리 넣는건 무조건 새로 들어가는것.
				//	if(	iCause & IMET_MODIFY_POS
				//	||	iCause & IMET_MODIFY_COUNT
				//	||	iCause & IMET_MODIFY_ENCHANT
				//	||	iCause & IMC_POS_BY_EXCHANGE 교환도 Create.
				//	)
				//	{//위의 경우 Modify 로 처리됨
				//		DB_ITEM_STATE_CHANGE const kItemChange(DISCT_MODIFY, iCause, OwnerGuid(), kBlankWrapper, kItemWrapper);//
				//		kChangeArray.push_back(kItemChange);
				//	}
				//	else
					{
						DB_ITEM_STATE_CHANGE const kItemChange( eStateChangeType, iCause, OwnerGuid(), CharacterGuid(), kBlankWrapper, kItemWrapper);//
						kChangeArray.push_back(kItemChange);

						{
							PgItemWrapper const & kCurItem = kItemChange.ItemWrapper();
							PgItemLogUtil::CreateLog(OwnerGuid(),kContLogMgr,kCurItem);
						}

					}

					return S_OK;
				}
				hReturn = E_INVALID_ITEMPOS;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
	return hReturn;
}

HRESULT PgBaseItemContainer::CanModify(PgItemWrapper const &kItemWrapper)const
{
	CONT_ITEM_WRAPPER::const_iterator found_itor = find(kItemWrapper.Pos());
	
	if(kItemWrapper.Pos().x == InvType())//인벤타입 맞나 확인.
	{
		return S_OK;
/*		if(found_itor == end())
		{
			if(!bIsTest)
			{
				kItemWrapper.SetItemPos(kInsertPos);//위치 갱신
				auto ret = insert(std::make_pair(kInsertPos, kItemWrapper));//insert 는 여기서만 호출 하도록 하라.
				if(ret.second)
				{//있던거면 어뜨케??
					
					bool const bIsCreated = kItemWrapper.IsCreated();
					DB_ITEM_STATE_CHANGE const kItemChange( ((bIsCreated)?DISCT_CREATE:DISCT_MODIFY), OwnerGuid(), kItemWrapper);
					kChangeArray.push_back(kItemChange);
					return S_OK;
				}
			}
			else
			{
				return S_OK;
			}
		}
*/
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
	return E_INVALID_ITEMPOS;
}

HRESULT PgBaseItemContainer::EmptyPos(SItemPos &rkOutPos)const
{
{//빈칸 찾기
	size_t last_idx = 0;
	// -> find 횟수가 너무 많긴 한데. 코드는 이게 가장 깔끔
	while(last_idx != GetMaxIDX())
	{
		SItemPos const kPos((BYTE)InvType(), (BYTE)last_idx);
		CONT_ITEM_WRAPPER::const_iterator found_itor = find(kPos);
		if(found_itor == end())
		{
			rkOutPos = kPos;
			return S_OK;
		}
		++last_idx;
	}
}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
	return E_INVALID_ITEMPOS;
}

HRESULT PgBaseItemContainer::PopItem(__int64 const iCause, SItemPos const &rkPos, PgItemWrapper& rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//자리만 맞으면 Null 인데 Pop이 성공했다고 하기도 한다.
	if(	rkPos.x == InvType()
	&&	rkPos.y >= 0 
	&&	rkPos.y < GetMaxIDX() )
	{
		CONT_ITEM_WRAPPER::const_iterator found_itor = find(rkPos);
		
		if(found_itor != end())
		{
			rkOutItem = (*found_itor).second;
			if(bIsPop)
			{
				PgItemWrapper kBlankWrapper;
				kBlankWrapper.Pos(rkPos);

				if(	!(iCause & IMC_POS_BY_EXCHANGE))//교환 떄문에 하는 Pop은 체인지 로그를 안남김.
				{
//					if(iCause & IMET_MODIFY_POS)//자리 이동 때문에 Pop 하는것은 DB에 Pop 기록을 하지 않음
//					{//무조건 REMOVE
//						DB_ITEM_STATE_CHANGE const kItemChange(DISCT_REMOVE_IMAGE, iCause, OwnerGuid(), (*found_itor).second, kBlankWrapper); 
//						kChangeArray.push_back(kItemChange);
//					}
//					else
//					{
						DB_ITEM_STATE_CHANGE const kItemChange(DISCT_REMOVE, iCause, OwnerGuid(), CharacterGuid(), (*found_itor).second, kBlankWrapper); 
						kChangeArray.push_back(kItemChange);

						{// 로그 내용 추가
							PgItemWrapper const & kOldItem = kItemChange.PrevItemWrapper();
							PgItemLogUtil::DeleteLog(OwnerGuid(),kContLogMgr,kOldItem);
						}
//					}
				}
				else
				{// 교환 할때 일단 빼. -> 무조건 제거.
					DB_ITEM_STATE_CHANGE const kItemChange(DISCT_REMOVE, iCause, OwnerGuid(), CharacterGuid(), (*found_itor).second, kBlankWrapper); 
					kChangeArray.push_back(kItemChange);

					{// 로그 내용 추가
						PgItemWrapper const & kOldItem = kItemChange.PrevItemWrapper();
						PgItemLogUtil::DeleteLog(OwnerGuid(),kContLogMgr,kOldItem);
					}

				}

				erase(rkPos);//빼주고 지워.
			}
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
	return E_INVALID_ITEMPOS;
}

HRESULT PgBaseItemContainer::Release()
{
	CONT_ITEM_WRAPPER::iterator item_itor = begin();

	while(item_itor != end())
	{
//		PgWorldItemManager::ReleaseItem( (*item_itor).second );
		++item_itor;
	}

	clear();

	return S_OK;
}

bool PgBaseItemContainer::EmulateModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount,CONT_MODIFY_COUNT & kContModifyCount, int & riModifyCount,bool const bIsDuration)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(iItemNo);
	if(!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool const bIsAmountItem = pItemDef->IsAmountItem();

	int iRemainCount = iAddCount;

	CONT_ITEM_WRAPPER::iterator item_itor = begin();

	while(item_itor != end())
	{
		if(!iRemainCount)
		{
			break;//다 지웠다
		}

		PgBase_Item kItem = (*item_itor).second;

		if(kItem.ItemNo() == iItemNo)
		{
			int iRetCount = 0;
			bool bRemoveItem = false;

			if(0 > iAddCount)//삭제류
			{
				iRetCount = __max( 0-kItem.Count(), iRemainCount);//

				if(!bIsAmountItem)		// 겹쳐지는 아이템이 아니면
				{
					if(!bIsDuration)	// 내구력 삭제가 아니면 아이템은 삭제 되어야 한다.
					{
						bRemoveItem = true;
					}
				}
			}
			else//추가류
			{
				if(pItemDef->IsAmountItem())
				{
					iRetCount = __min(int(pItemDef->MaxAmount() - kItem.Count()), iRemainCount);// 최대내구- 현재내구, 추가할 내구
				}
				else
				{
					iRetCount = 0; // 겹져지는 아이템이 아니다.
				}
			}

			if(iRetCount)
			{
				iRemainCount -= iRetCount;

				SPMOD_Modify_Count kData;
				
				kData.Pos((*item_itor).second.Pos());
				kData.Item((*item_itor).second);
				kData.AddCount(iRetCount);
				kData.DoRemove(bRemoveItem);

				kContModifyCount.push_back(kData);
			}
		}

		++item_itor;
	}
	
	riModifyCount = iAddCount - iRemainCount;

	return true;
}

HRESULT PgBaseItemContainer::ModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//겹쳐지는 아이템이면 내부에서 넣어주는것.(기존 아이템 없으면 안들어감.)

	if(!iAddCount)
	{
		return S_OK;//성공했다고 보는거야 그냥
	}

	int iModifyCount = 0;
	CONT_MODIFY_COUNT kContModifyCount;

	if(!EmulateModifyCount(iCause,iItemNo,iAddCount,kContModifyCount,iModifyCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if(iAddCount == iModifyCount)
	{
		for(CONT_MODIFY_COUNT::iterator modify_iter = kContModifyCount.begin();modify_iter != kContModifyCount.end();++modify_iter)
		{
			ModifyCount(iCause, (*modify_iter), kChangeArray,kContLogMgr);
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgBaseItemContainer::ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//개별 개체에 대한 수정.
	HRESULT hReturn = E_FAIL;
	PgItemWrapper kItemWrapper;
	if(S_OK == (hReturn = PopItem(iCause, kData.Pos(), kItemWrapper, false, kChangeArray, kContLogMgr)))
	{
		PgBase_Item const &rkMyItem = kItemWrapper;

		if(rkMyItem != kData.Item())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
			return E_INCORRECT_ITEM;
		}

		PgBase_Item kTempItem = rkMyItem;

		if(	kData.DoRemove() //지우기 들어오거나.
		||	0 > kData.AddCount())//내구도 내림
		{//수량 수정 코드
			if(kTempItem.Count() < abs(kData.AddCount()))
			{//깔 내구도가 모자라다
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MODIFYITEM"));
				return E_CANNOT_MODIFYITEM;
			}

			if(kData.DoRemove())
			{
				kTempItem.Count(0);//삭제됨.
			}
			else
			{
				int const iRetCount = kTempItem.Count() + kData.AddCount();

				if( 0 > iRetCount)
				{
					INFO_LOG( BM::LOG_LV0, __FL__ << _T(" MODIFY COUNT") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MODIFYITEM"));
					return E_CANNOT_MODIFYITEM;
				}

				kTempItem.Count(iRetCount);//아이템 삭제 되면.. 음..
			}

			if(!kTempItem.Count())
			{//아이템이 지워졌음.
				if(S_OK == (hReturn = PopItem(iCause, kItemWrapper.Pos(), kItemWrapper, true, kChangeArray, kContLogMgr)))
				{
					return S_OK;
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
				return hReturn;
			}
			else
			{//아이템이 살아있음
				kItemWrapper.SetItem(kTempItem);
				return OverWriteItem(iCause, kItemWrapper, kChangeArray, kContLogMgr);
			}
		}//지우기나 내구도 내림.
		else if( !kData.DoRemove()
		&&	(0 < kData.AddCount()))//수량 조건 맞다면.
		{//추가 지급 혹은 내구 상승.
			int const iRetCount = kTempItem.Count() + kData.AddCount();

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pItemDef = kItemDefMgr.GetDef(kTempItem.ItemNo());
			if(pItemDef)
			{
				if (int(pItemDef->MaxAmount()) < iRetCount)
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_CANNOT_MODIFYITEM"));
					return E_CANNOT_MODIFYITEM;
				}

				SEnchantInfo kEnchant = kTempItem.EnchantInfo();

				if(((IMC_INC_DUR_BY_REPAIR & iCause) == IMC_INC_DUR_BY_REPAIR) && (true == kEnchant.IsNeedRepair()))
				{
					kEnchant.IsNeedRepair(false);
					kTempItem.EnchantInfo(kEnchant);
				}
				
				kTempItem.Count(iRetCount);
				kItemWrapper.SetItem(kTempItem);
				return OverWriteItem(iCause, kItemWrapper, kChangeArray, kContLogMgr);
			}
		}//추가 지급, 내구도 상승.
	}
	return S_OK;
}

HRESULT PgFitItemContainer::CanModify(PgItemWrapper const &kItemWrapper)const
{
	if(PgBase_Item::IsEmpty(&kItemWrapper))
	{//아이템이 없으니까.
		return S_OK;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());

	if(!pItemDef)
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Cannot Get ItemDef ItemNo[") << kItemWrapper.ItemNo() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	if( !pItemDef->IsType(ITEM_TYPE_EQUIP) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	int const iEquipType = pItemDef->GetAbil(AT_EQUIP_LIMIT);

	bool bIsRaiseError = false;

	/*
	if(iEquipType != (1 << kItemWrapper.Pos().y))
	{
		bIsRaiseError = true;
	}

	switch(kItemWrapper.Pos().y)
	{//넣는위치가.
	case EQUIP_POS_EARRING_R:
		{
			if(iEquipType == EQUIP_POS_EARRING)//이어링이면 허용
			{
				bIsRaiseError = false;
			}break;
		}break;
	case EQUIP_POS_RING_R:
		{
			if(iEquipType == EQUIP_POS_RING)//링이면 허용
			{
				bIsRaiseError = false;
			}break;
		}break;
	case EQUIP_POS_HAIR:
	case EQUIP_POS_FACE:
	case EQUIP_POS_SHOULDER:
	case EQUIP_POS_CLOAK:
	case EQUIP_POS_GLASS:
	case EQUIP_POS_WEAPON:
	case EQUIP_POS_SHEILD:
	case EQUIP_POS_NECKLACE:
	case EQUIP_POS_EARRING:
	case EQUIP_POS_RING:
	case EQUIP_POS_BELT:
	case EQUIP_POS_ATTSTONE:
	case EQUIP_POS_HELMET:
	case EQUIP_POS_SHIRTS:
	case EQUIP_POS_PANTS:
	case EQUIP_POS_BOOTS:
	case EQUIP_POS_GLOVE:
		{
		}break;
	default:
		{
			return E_FAIL;
		}break;
	}
	*/

	//다른 포지션 매쉬 가리는거 때문이면 이거 비트플래그가 나쁜데?.
	EEquipPos eEquipPos = (EEquipPos)pItemDef->EquipPos();
	switch(eEquipPos)
	{
	//case EQUIP_POS_EARRING:	// 귀고리는 왼쪽/오른쪽 두곳에 장착 가능
	//	{
	//		EEquipPos eTgPos = (EEquipPos) kItemWrapper.Pos().y;
	//		if (eTgPos != EQUIP_POS_EARRING_L && eTgPos != EQUIP_POS_EARRING_R)
	//		{
	//			bIsRaiseError = true;
	//		}
	//	}break;
	case EQUIP_POS_RING: // 반지는 왼쪽/오른쪽 두곳에 장착 가능
		{
			EEquipPos eTgPos = (EEquipPos) kItemWrapper.Pos().y;
			if (eTgPos != EQUIP_POS_RING_L && eTgPos != EQUIP_POS_RING_R)
			{
				bIsRaiseError = true;
			}
		}break;
	default:
		{
			if (eEquipPos != kItemWrapper.Pos().y)
			{
				bIsRaiseError = true;
			}
		}break;
	}

	if(!bIsRaiseError)//에러 없음
	{
		return PgBaseItemContainer::CanModify(kItemWrapper);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
	return E_INCORRECT_ITEM;
}

void PgBaseItemContainer::operator = (const PgBaseItemContainer &rhs)
{
	CONT_ITEM_WRAPPER::operator =(rhs);
	Reduction(rhs.Reduction());
	ExtendMaxIdx(rhs.ExtendMaxIdx());
//	m_kOwnerGuid = rhs.OwnerGuid();//주 : 레퍼런스
}

PgBaseItemContainer::PgBaseItemContainer(const PgBaseItemContainer &rhs)
	:	m_kOwnerGuid(rhs.m_kOwnerGuid), m_kCharacterGuid(rhs.m_kCharacterGuid)
{
	*this = rhs;
}

void PgBaseItemContainer::Swap( PgBaseItemContainer &rhs )
{
	CONT_ITEM_WRAPPER::swap( rhs );
	std::swap( m_kReduction, rhs.m_kReduction );
	std::swap( m_kExtendMaxIdx, rhs.m_kExtendMaxIdx );
//	std::swap( m_kOwnerGuid, rhs.m_kOwnerGuid );//주 : 레퍼런스
}

HRESULT PgBaseItemContainer::SetItem(PgBase_Item const & kItem, SItemPos const & kPos)
{
	CONT_ITEM_WRAPPER::iterator found_itor = find(kPos);
	if(found_itor == end())
	{
		if(kItem.IsEmpty())
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		return InsertItem(kItem,kPos);
	}
	else if(kItem.IsEmpty())
	{
		erase(kPos);
	}
	else
	{
		(*found_itor).second.SetItem(kItem);
	}

	return S_OK;
}

HRESULT PgBaseItemContainer::InsertItem(PgBase_Item const & kItem, SItemPos const & kPos)
{
	PgItemWrapper kItemWrapper(kItem, kPos);

	auto ret = insert(std::make_pair(kItemWrapper.Pos(),kItemWrapper));

	if(!ret.second)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	return S_OK;
}

bool PgBaseItemContainer::EnumEmptyInvPos(CONT_EMPTY_ITEM_POS & kContEmptyItemPos, int const iLimitCount)
{
	for(int i = 0;i < GetMaxIDX();i++)
	{
		if(iLimitCount <= (int)kContEmptyItemPos.size())
		{
			return true;
		}

		SItemPos kItemPos(InvType(),i);
		CONT_ITEM_WRAPPER::iterator item_itor = find(kItemPos);
		if(item_itor == end())
		{
			kContEmptyItemPos.push_back(kItemPos);
		}
	}

	return (iLimitCount > 0 ? (iLimitCount <= (int)kContEmptyItemPos.size()) : true);
}

void PgBaseItemContainer::Clear()
{
	clear();
}

HRESULT PgBaseItemContainer::SwapItem( SItemPos const & kPos, PgBase_Item &kItem, bool const bSameID )
{
	CONT_ITEM_WRAPPER::iterator itr = find(kPos);
	if( itr != end() )
	{
		if (	!bSameID
			||	kItem.Guid() == itr->second.Guid() )
		{
			itr->second.Swap( kItem );
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgBaseItemContainer::GetItem(SItemPos const & kPos, PgBase_Item &kOutItem) const
{
	CONT_ITEM_WRAPPER::const_iterator itor = find(kPos);
	if(itor != end())
	{
		kOutItem = (*itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgBaseItemContainer::GetItemID( SItemPos const & kPos, BM::GUID& kOutItemID )const
{
	CONT_ITEM_WRAPPER::const_iterator itr = find(kPos);
	if( itr != end())
	{
		kOutItemID = itr->second.Guid();
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

size_t PgBaseItemContainer::GetItemCount( BYTE const byChkState )const
{
	size_t iCount = 0;
	CONT_ITEM_WRAPPER::const_iterator itr = begin();
	for ( ; itr != end() ; ++itr )
	{
		if ( byChkState == itr->second.State() )
		{
			++iCount;
		}
	}
	return iCount;
}

bool PgBaseItemContainer::OnTimeOutedItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;

		/*============================================================================
						이미 기간 만료된 녀석은 다시 검사 하지 말자
		============================================================================*/

		if(true == kItemWrapper.IsTimeOuted() || false == kItemWrapper.IsUseTimeOut())
		{
			continue;
		}

		SEnchantInfo kEnchant = kItemWrapper.EnchantInfo();
		kEnchant.IsTimeOuted(1);
		rkContModifyOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, OwnerGuid(), SPMOD_Enchant( kItemWrapper, (*iter).first, kEnchant)));
	}
	return false;
}

bool PgBaseItemContainer::OnTimeOutedMonsterCard(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;

		/*============================================================================
						이미 기간 만료된 녀석은 다시 검사 하지 말자
		============================================================================*/

		if(true == kItemWrapper.IsTimeOuted())
		{
			continue;
		}

		/*============================================================================
									몬스터 카드 기간 만료 검사
		============================================================================*/

		if( (0 == kItemWrapper.EnchantInfo().MonsterCard()) || 
			(1 == kItemWrapper.EnchantInfo().IsMCTimeOuted()))
		{
			continue;
		}

		SMonsterCardTimeLimit kCard;
		if(false == kItemWrapper.Get(kCard))	// 기간제 몬스터 카드가 아니면 이 정보는 없다.
		{
			continue;
		}

		if(false == kCard.IsUseTimeOut())
		{
			continue;
		}

		SEnchantInfo kEnchant = kItemWrapper.EnchantInfo();
		kEnchant.IsMCTimeOuted(1);
		rkContModifyOrder.push_back(SPMO(IMET_MODIFY_ENCHANT, OwnerGuid(), SPMOD_Enchant( kItemWrapper, (*iter).first, kEnchant)));
	}
	return false;
}

HRESULT PgPostItemContainer::OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	PgItemWrapper kBlankWrapper;
	kBlankWrapper.Pos(kItemWrapper.Pos());
	const DB_ITEM_STATE_CHANGE kItemChange(DISCT_CREATE, iCause, OwnerGuid(), CharacterGuid(), kBlankWrapper, kItemWrapper);//
	kChangeArray.push_back(kItemChange);

	{
		PgItemWrapper const & kCurItem = kItemChange.ItemWrapper();
		PgItemLogUtil::CreateLog(OwnerGuid(),kContLogMgr,kCurItem);
	}

	return S_OK;
}

HRESULT PgUserMarketItemContainer::OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	PgItemWrapper kBlankWrapper;
	kBlankWrapper.Pos(kItemWrapper.Pos());
	const DB_ITEM_STATE_CHANGE kItemChange(DISCT_CREATE, iCause, OwnerGuid(), CharacterGuid(), kBlankWrapper, kItemWrapper);//
	kChangeArray.push_back(kItemChange);

	{
		PgItemWrapper const & kCurItem = kItemChange.ItemWrapper();
		PgItemLogUtil::CreateLog(OwnerGuid(),kContLogMgr,kCurItem);
	}

	return S_OK;
}

typedef struct tagSortKey
{
	tagSortKey():i64Value(0),bIsAscendingOrder(true){}
	explicit tagSortKey(__int64 const & __keyvalue,bool const __asc = true):i64Value(__keyvalue),bIsAscendingOrder(__asc){}

	bool bIsAscendingOrder;
	__int64 i64Value;

	bool operator < (tagSortKey const & rhs) const
	{
		if(false == bIsAscendingOrder)
		{
			return (i64Value > rhs.i64Value);
		}

		return (i64Value < rhs.i64Value);
	}

	bool operator == (tagSortKey const & rhs) const
	{
		return (i64Value == rhs.i64Value);
	}
}SSortKey;

typedef std::vector<SSortKey> CONT_KEY;

typedef struct tagContSortKey
{
	CONT_KEY kContKey;
	tagContSortKey(){}
	~tagContSortKey(){}

	bool operator < (tagContSortKey const & rhs) const
	{
		CONT_KEY::const_iterator iter_lhs = kContKey.begin();
		for(CONT_KEY::const_iterator iter_rhs = rhs.kContKey.begin();iter_lhs != kContKey.end(), iter_rhs != rhs.kContKey.end(); ++iter_lhs, ++iter_rhs)
		{
			if((*iter_lhs) < (*iter_rhs))
			{
				return true;	// 이 등급의 키가 작으면 작은것이다.
			}
			else if((*iter_lhs) == (*iter_rhs))
			{
				continue;		// 이 등급의 키가 같으면 다음것을 비교
			}

			return false;		// 여기까지 오면 큰것
		}

		return false;			// 모두 동일하다. 같은것 이다.
	}
}SContSortKey;

typedef std::map<SContSortKey,PgItemWrapper> CONT_SORT_INVENTORY;

void CastContSort2ContInv(EInvType const kInvType, CONT_SORT_INVENTORY const & kContSort,CONT_ITEM_WRAPPER & kContInv)
{
	BYTE bPos = 0;
	for(CONT_SORT_INVENTORY::const_iterator iter = kContSort.begin(); iter != kContSort.end(); ++iter, ++bPos)
	{
		kContInv.insert(std::make_pair(SItemPos(kInvType,bPos),(*iter).second));
	}
}

HRESULT PgEquipItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		// 클레스 리밋

		__int64 const i64ClassLimit = pItemDef->GetAbil64(AT_CLASSLIMIT);
		if (true == IS_CLASS_LIMIT(i64ClassLimit, pkUnit->GetAbil(AT_CLASS)))
		{
			kKey.kContKey.push_back(SSortKey(0));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(i64ClassLimit));
		}

		// order1
		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));
		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));
		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER3)));		
		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

HRESULT PgConsumeItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		kKey.kContKey.push_back(SSortKey(pItemDef->BuyPrice(), false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

HRESULT PgEtcItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	PgPlayer const * pkPlayer = dynamic_cast<PgPlayer const *>(pkUnit);
	if(NULL == pkPlayer)
	{
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		CONT_QUEST_ITEM::const_iterator find_iter = kContQuestItem.find(kItemWrapper.ItemNo());
		if(find_iter != kContQuestItem.end())
		{
			CONT_QUEST_SET const & kContQuestSet = (*find_iter).second;

			int iCount = 0;
			for(CONT_QUEST_SET::const_iterator q_iter = kContQuestSet.begin(); q_iter != kContQuestSet.end(); ++q_iter)
			{
				iCount += (true == pkPlayer->GetMyQuest()->IsIngQuest((*q_iter)) ? 1 : 0);
			}

			if(0 < iCount)
			{
				kKey.kContKey.push_back(SSortKey(1));
			}
			else
			{
				kKey.kContKey.push_back(SSortKey(2));
			}
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(3));
		}

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		kKey.kContKey.push_back(SSortKey(pItemDef->BuyPrice() ,false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

HRESULT PgCashItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER3)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

HRESULT PgCashSafeItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER3)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

HRESULT PgSafeItemContainer::MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_SORT_INVENTORY kContSort;

	int iIdx = 0;
	for(CONT_ITEM_WRAPPER::const_iterator iter = this->begin();iter != this->end();++iter,++iIdx)
	{
		PgItemWrapper const & kItemWrapper = (*iter).second;
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		if(NULL == pItemDef)
		{
			continue;
		}

		SContSortKey kKey;

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER1)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER2)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_ITEM_SORT_ORDER3)));

		kKey.kContKey.push_back(SSortKey(pItemDef->GetAbil(AT_LEVELLIMIT), false));

		kKey.kContKey.push_back(SSortKey(pItemDef->BuyPrice(), false));

		std::wstring kOut;
		if(true == GetItemName(kItemWrapper.ItemNo(), kOut))
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(kOut[0])));
		}
		else
		{
			kKey.kContKey.push_back(SSortKey(static_cast<__int64>(0xffff)));
		}

		kKey.kContKey.push_back(SSortKey(kItemWrapper.ItemNo()));

		kKey.kContKey.push_back(SSortKey(iIdx));

		kContSort.insert(std::make_pair(kKey,kItemWrapper));
	}

	CONT_ITEM_WRAPPER kCopyInven;
	CastContSort2ContInv(InvType(), kContSort, kCopyInven);
	MakeSortOrderSub(pkUnit, (*this), kCopyInven, rkContModifyOrder);
	return S_OK;
}

