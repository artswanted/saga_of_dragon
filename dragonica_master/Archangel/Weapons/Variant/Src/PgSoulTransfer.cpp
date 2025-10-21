#include "stdafx.h"
#include "PgSoulTransfer.h"
#include "PgPlayer.h"
#include "Item.h"
#include "BM/LocalMgr.h"


//영력 추출에 적합한 아이템이냐
bool SoulTransitionUtil::IsSoulExtractItem(PgBase_Item const &rkItem)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef( rkItem.ItemNo() );
	if(!pItemDef)
	{
		return false;
	}
	//소울크래프트 불가옵션이면 불가
	if(ICMET_Cant_SoulCraft & pItemDef->GetAbil(AT_ATTRIBUTE))
	{
		return false;
	}
	//장착아이템 아니면 불가
	if( !pItemDef->CanEquip() )
	{
		return false;
	}
	//펫아이템 불가
	if( pItemDef->IsPetItem() )
	{
		return false;
	}
	//만료 아이템 불가
	if( rkItem.IsUseTimeOut() )
	{
		return false;
	}
	//인챈트정보
	SEnchantInfo const &rkEnchantInfo = rkItem.EnchantInfo();
	//봉인 아이템 불가
	if( rkEnchantInfo.IsSeal() )
	{
		return false;
	}
	//저주 아이템 불가
	if( rkEnchantInfo.IsCurse() )
	{
		return false;
	}
	//영력없으면 불가
	if( 0 == rkEnchantInfo.Rarity() )
	{
		return false;
	}
	//영력으로 인한 추가 옵션 없으면 불가
	if( 0 == rkEnchantInfo.BasicType1()
		&&  0 == rkEnchantInfo.BasicType2()
		&&  0 == rkEnchantInfo.BasicType3()
		&&  0 == rkEnchantInfo.BasicType4() )
	{
		return false;
	}	
	if( LOCAL_MGR::NC_JAPAN == g_kLocal.ServiceRegion() 
		&& CheckIsCashItem(rkItem))
	{//일본의 경우 캐시 아이템은 소울 크래프트 관련 작업 불가
		return false;
	}
	return true;
}

//영력 삽입에 적합한 아이템이냐
bool SoulTransitionUtil::IsSoulInsertItem(PgBase_Item const &rkItem)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef( rkItem.ItemNo() );
	if(!pItemDef)
	{
		return false;
	}
	//장착아이템 아니면 불가
	if( !pItemDef->CanEquip() )
	{
		return false;
	}
	//펫아이템 불가
	if( pItemDef->IsPetItem() )
	{
		return false;
	}
	//소울크래프트 불가옵션이면 불가
	if(ICMET_Cant_SoulCraft & pItemDef->GetAbil(AT_ATTRIBUTE))
	{
		return false;
	}
	//만료 아이템 불가
	if( rkItem.IsUseTimeOut() )
	{
		return false;
	}
	//인챈트정보
	SEnchantInfo const &rkEnchantInfo = rkItem.EnchantInfo();
	//봉인 아이템 불가
	if( rkEnchantInfo.IsSeal() )
	{
		return false;
	}
	//저주 아이템 불가
	if( rkEnchantInfo.IsCurse() )
	{
		return false;
	}	
	if( LOCAL_MGR::NC_JAPAN == g_kLocal.ServiceRegion() 
		&& CheckIsCashItem(rkItem))
	{//일본의 경우 캐시 아이템은 소울 크래프트 관련 작업 불가
		return false;
	}
	return true;
}

//영력 보관 구슬이 맞냐
bool SoulTransitionUtil::IsSoulTransitionItem(int const iItemNo)
{
	CONT_DEF_JOBSKILL_SOUL_EXTRACT const* pkDefSoulExtract = NULL;
	g_kTblDataMgr.GetContDef(pkDefSoulExtract);
	if( pkDefSoulExtract )
	{
		CONT_DEF_JOBSKILL_SOUL_EXTRACT::const_iterator find_iter = pkDefSoulExtract->begin();
		while( pkDefSoulExtract->end() != find_iter)
		{
			CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type const kTemp = (*find_iter);
			if( iItemNo == kTemp.iResultItemNo)
			{
				return true;
			}
			++find_iter;
		}
	}
	return false;
}

//영력 추출기 정보 받아오기
bool SoulTransitionUtil::GetSoulExtractInfo(int const iEquipPos, int const iLevel, CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type &rkOut)
{
	CONT_DEF_JOBSKILL_SOUL_EXTRACT const* pkDefSoulExtract = NULL;
	g_kTblDataMgr.GetContDef(pkDefSoulExtract);
	if( pkDefSoulExtract )
	{
		CONT_DEF_JOBSKILL_SOUL_EXTRACT::const_iterator find_iter = pkDefSoulExtract->begin();
		while( pkDefSoulExtract->end() != find_iter)
		{
			CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type const kTemp = (*find_iter);
			if( iEquipPos == kTemp.iEquipPos && iLevel == kTemp.iLevelLimit)
			{
				rkOut = kTemp;
				break;
			}	
			++find_iter;
		}
	}
	return !rkOut.IsNull();
}

//아이템에 맞는 추출정보 받아오기(아이템)
bool SoulTransitionUtil::GetSoulExtractInfo(int const iItemNo, CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type &rkOut)
{
	if( 0 == iItemNo )
	{
		return false;
	}
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef( iItemNo );
	if(!pItemDef)
	{
		return false;
	}
	int const iLevelLimit = pItemDef->GetAbil(AT_LEVELLIMIT);
	int const iEquipPos = pItemDef->EquipPos();
	bool const bRet = SoulTransitionUtil::GetSoulExtractInfo(iEquipPos, iLevelLimit, rkOut);

	return bRet;
}

//영력 보관 구슬 생성
bool SoulTransitionUtil::CreateSoulTransition(PgBase_Item const &rkItem, PgBase_Item &rkItemOut)
{
	if(rkItem.IsEmpty())
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SOUL_EXTRACT::value_type rkExtract;
	if( !SoulTransitionUtil::GetSoulExtractInfo(rkItem.ItemNo(), rkExtract) )
	{
		return false;
	}
	SEnchantInfo const &rkEnchantInfo = rkItem.EnchantInfo();
	CreateSItem(rkExtract.iResultItemNo, 1, 0, rkItemOut);

	SEnchantInfo kInputEnchantInfo;
	kInputEnchantInfo.Rarity( rkEnchantInfo.Rarity() );
	kInputEnchantInfo.BasicType1( rkEnchantInfo.BasicType1() );
	kInputEnchantInfo.BasicLv1( rkEnchantInfo.BasicLv1() );
	kInputEnchantInfo.BasicType2( rkEnchantInfo.BasicType2() );
	kInputEnchantInfo.BasicLv2( rkEnchantInfo.BasicLv2() );
	kInputEnchantInfo.BasicType3( rkEnchantInfo.BasicType3() );
	kInputEnchantInfo.BasicLv3( rkEnchantInfo.BasicLv3() );
	kInputEnchantInfo.BasicType4( rkEnchantInfo.BasicType4() );
	kInputEnchantInfo.BasicLv4( rkEnchantInfo.BasicLv4() );

	rkItemOut.EnchantInfo(kInputEnchantInfo);

	return true;
}
// 영력 삽입 정보 받아오기
bool SoulTransitionUtil::GetSoulTransitionInfo(int const iItemNo, int const iRairity, CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type &rkOut)
{
	if( 0 == iItemNo )
	{
		return false;
	}
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef( iItemNo );
	if(!pItemDef)
	{
		return false;
	}

	int const iLevelLimit = pItemDef->GetAbil(AT_LEVELLIMIT);
	int iEquipType = GetEquipType(iItemNo);

	if( 0 == iEquipType )
	{
		//메달이면 악세로 강제 적용
		if( EQUIP_POS_MEDAL == pItemDef->EquipPos() )
		{
			iEquipType = 3;
		}
	}

	CONT_DEF_JOBSKILL_SOUL_TRANSITION const* pkDefSoulTransition = NULL;
	g_kTblDataMgr.GetContDef(pkDefSoulTransition);
	if( pkDefSoulTransition )
	{
		CONT_DEF_JOBSKILL_SOUL_TRANSITION::const_iterator find_iter = pkDefSoulTransition->begin();
		while( pkDefSoulTransition->end() != find_iter)
		{
			CONT_DEF_JOBSKILL_SOUL_TRANSITION::value_type const kTemp = (*find_iter);
			if( iEquipType == kTemp.iEquipType 
				&& iLevelLimit == kTemp.iLevelLimit
				&& iRairity == kTemp.iRairity_Grade)
			{
				rkOut = kTemp;
				break;
			}	
			++find_iter;
		}
	}
	return !rkOut.IsNull();
}
//영력 전이 (원본/목표) - 목표가 없으면 추출, 있으면 삽입
bool SoulTransitionUtil::TransitionItem(PgBase_Item &rkOrgItem, PgBase_Item &rkDestItem)
{
	if( rkOrgItem.IsEmpty())
	{
		return false;
	}
	bool const bExtract = rkDestItem.IsEmpty();
	if( bExtract )
	{//추출
		if( false == CreateSoulTransition(rkOrgItem, rkDestItem) )
		{
			return false;
		}
		SEnchantInfo kInputEnchantInfo = rkOrgItem.EnchantInfo();
		kInputEnchantInfo.Rarity( 0 );
		kInputEnchantInfo.BasicType1( 0 );
		kInputEnchantInfo.BasicLv1( 0 );
		kInputEnchantInfo.BasicType2( 0 );
		kInputEnchantInfo.BasicLv2( 0 );
		kInputEnchantInfo.BasicType3( 0 );
		kInputEnchantInfo.BasicLv3( 0 );
		kInputEnchantInfo.BasicType4( 0 );
		kInputEnchantInfo.BasicLv4( 0 );
		rkOrgItem.EnchantInfo(kInputEnchantInfo);
	}
	else
	{//삽입
		SEnchantInfo kOrgEnchantInfo = rkOrgItem.EnchantInfo();
		SEnchantInfo kDestEnchantInfo = rkDestItem.EnchantInfo();
		kDestEnchantInfo.Rarity( kOrgEnchantInfo.Rarity() );
		kDestEnchantInfo.BasicType1( kOrgEnchantInfo.BasicType1() );
		kDestEnchantInfo.BasicLv1( kOrgEnchantInfo.BasicLv1() );
		kDestEnchantInfo.BasicType2( kOrgEnchantInfo.BasicType2() );
		kDestEnchantInfo.BasicLv2( kOrgEnchantInfo.BasicLv2() );
		kDestEnchantInfo.BasicType3( kOrgEnchantInfo.BasicType3() );
		kDestEnchantInfo.BasicLv3( kOrgEnchantInfo.BasicLv3() );
		kDestEnchantInfo.BasicType4( kOrgEnchantInfo.BasicType4() );
		kDestEnchantInfo.BasicLv4( kOrgEnchantInfo.BasicLv4() );
		rkDestItem.EnchantInfo(kDestEnchantInfo);
	}
	return true;
}

bool SoulTransitionUtil::IsTransition(PgBase_Item const &rkTransitionItem, PgBase_Item const &rkDestItem)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pTransitionDef = kItemDefMgr.GetDef(rkTransitionItem.ItemNo());
	CItemDef const *pDestDef = kItemDefMgr.GetDef(rkDestItem.ItemNo());

	int const iTransitionLevelLimit = pTransitionDef->GetAbil(AT_LEVELLIMIT);
	int const iDestLevelLimit = pDestDef->GetAbil(AT_LEVELLIMIT);

	int const iTransitionEquipPos = pTransitionDef->GetAbil(AT_EQUIP_LIMIT);
	int const iDestEquipPos = pDestDef->GetAbil(AT_EQUIP_LIMIT);

	if(ICMET_Cant_SoulCraft & pDestDef->GetAbil(AT_ATTRIBUTE))
	{//소울 크래프트 금지 아이템이면 불가.
		return false;
	}

	//기본 허용 레벨 범위는 [아이템 레벨 ~ 아이템 레벨 +10]
	bool bResultEnable = ( (iTransitionLevelLimit+10) >= iDestLevelLimit
			&& iTransitionLevelLimit <= iDestLevelLimit
			&& iTransitionEquipPos == iDestEquipPos );

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_TAIWAN:
		{//일본, 대만은 허용 레벨 범위가 [1Lv ~ 아이템레벨 + 10Lv]
			bResultEnable = ( (iTransitionLevelLimit+10) >= iDestLevelLimit
				&& iTransitionEquipPos == iDestEquipPos );
		}break;
	}
	return bResultEnable;
}