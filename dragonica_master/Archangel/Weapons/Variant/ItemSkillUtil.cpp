#include "stdafx.h"
#include "ItemSkillUtil.h"
#include "DefAbilType.h"
#include "ItemSetDefMgr.h"

namespace ItemSkillUtil
{
	//지금 가진 아이템의 스킬시전 어빌값 리턴(스킬번호)
	int GetItemSkillNo(int const iItemNo)
	{
		GET_DEF( CItemDefMgr, kItemDefMgr);
		CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
		if(!pkItemDef)
		{
			return false;
		}
		GET_DEF(CItemSetDefMgr, kItemSetDefMgr);
		int const iSetNo = kItemSetDefMgr.GetItemSetNo(iItemNo);
		if(!iSetNo)
		{
			return false;
		}
		CItemSetDef const *pkItemSet = kItemSetDefMgr.GetDef(iSetNo);
		if(!pkItemSet)
		{
			return false;
		}

		return (pkItemSet->GetAbil(AT_ITEM_SKILL));
	}
	//이 스킬을 사용할 수 있냐
	bool CheckUseItemSkill(PgPlayer *pkPlayer, int const iSkillNo, EInvType const eInvType)
	{
		if(!pkPlayer)
		{
			return false;
		}
		if(!pkPlayer->GetInven())
		{
			return false;
		}
		if( IT_FIT != eInvType && IT_FIT_CASH != eInvType )
		{
			return false;
		}
		GET_DEF(CItemSetDefMgr, kItemSetDefMgr);
		//내가 장착한 아이템들 가져오고
		CONT_HAVE_ITEM_DATA kContEquipItem;
		if( E_FAIL == pkPlayer->GetInven()->GetItems(eInvType, kContEquipItem, true) )
		{
			return false;
		}
		CONT_HAVE_ITEM_DATA::const_iterator item_iter = kContEquipItem.begin();
		while( item_iter != kContEquipItem.end() )
		{
			PgBase_Item const &rkItem = (*item_iter).second;
			int const iItemSkillNo = GetItemSkillNo( rkItem.ItemNo() );
			if( iItemSkillNo == iSkillNo)
			{//찾는 스킬이 있다면
				//세트조건을 충족하는지 확인
				int const iSetNo = kItemSetDefMgr.GetItemSetNo(rkItem.ItemNo());
				CItemSetDef const *pkItemSet = kItemSetDefMgr.GetDef(iSetNo);
				if(!pkItemSet)
				{
					return false;
				}
				bool bCompleteSet = false;
				int const iEquipPiece =  pkItemSet->CheckNeedItem(kContEquipItem, dynamic_cast<CUnit*>(pkPlayer), bCompleteSet);
				CItemSetDef const *pkEquipItemSet = kItemSetDefMgr.GetEquipAbilDef(iSetNo, iEquipPiece);	//장비한세트의 세트정보를 가져오고
				if(!pkEquipItemSet)
				{
					return false;
				}
				//해당 정보에 스킬시전이 있고, 찾는 번호와 같은지 확인
				return (iSkillNo == pkEquipItemSet->GetAbil(AT_ITEM_SKILL));

			}
			++item_iter;
		}

		return false;
	}
	bool CheckUseItemSkillAll(PgPlayer *pkPlayer, int const iSkillNo)
	{
		return ( ItemSkillUtil::CheckUseItemSkill(pkPlayer, iSkillNo, IT_FIT)
				|| ItemSkillUtil::CheckUseItemSkill(pkPlayer, iSkillNo, IT_FIT_CASH) );
	}
	//현재 착용한 아이템에서 사용가능한 모든 스킬시전 번호 받아오기
	bool GetAllItemSkillFromEquipItem(PgPlayer *pkPlayer, std::set<int> &rkContSkillNo, EInvType const eInvType)
	{
		if(!pkPlayer)
		{
			return false;
		}
		if(!pkPlayer->GetInven())
		{
			return false;
		}
		if( IT_FIT != eInvType && IT_FIT_CASH != eInvType )
		{
			return false;
		}

		GET_DEF(CItemSetDefMgr, kItemSetDefMgr);
		//내가 장착한 아이템들 가져오고
		CONT_HAVE_ITEM_DATA kContEquipItem;
		if( E_FAIL == pkPlayer->GetInven()->GetItems(eInvType, kContEquipItem, true) )
		{
			return false;
		}
		CONT_HAVE_ITEM_DATA::const_iterator item_iter = kContEquipItem.begin();
		while( item_iter != kContEquipItem.end() )
		{
			PgBase_Item const &rkItem = (*item_iter).second;
			int const iItemSkillNo = GetItemSkillNo( rkItem.ItemNo() );
			if( 0 != iItemSkillNo )
			{//찾는 스킬이 있다면
				//세트조건을 충족하는지 확인
				int const iSetNo = kItemSetDefMgr.GetItemSetNo(rkItem.ItemNo());
				CItemSetDef const *pkItemSet = kItemSetDefMgr.GetDef(iSetNo);
				if(!pkItemSet)
				{
					return false;
				}
				bool bCompleteSet = false;
				int const iEquipPiece =  pkItemSet->CheckNeedItem(kContEquipItem, dynamic_cast<CUnit*>(pkPlayer), bCompleteSet);
				CItemSetDef const *pkEquipItemSet = kItemSetDefMgr.GetEquipAbilDef(iSetNo, iEquipPiece);	//장비한세트의 세트정보를 가져오고
				if(pkEquipItemSet)
				{
					//해당 정보에 스킬시전이 있고, 이미 들어간 번호인지 확인
					int const iSkillNo = pkEquipItemSet->GetAbil(AT_ITEM_SKILL);
					if( 0 != iSkillNo 
						&& rkContSkillNo.end() == rkContSkillNo.find(iSkillNo) )
					{
						rkContSkillNo.insert(iSkillNo);
					}
				}
			}
			++item_iter;
		}
		return true;
	}
}