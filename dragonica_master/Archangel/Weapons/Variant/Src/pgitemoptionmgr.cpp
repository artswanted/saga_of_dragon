#include "StdAfx.h"
#include "PgItemOptionMgr.h"
#include "PgControlDefMgr.h"
#include "TableDataManager.h"
#include "global.h"
#include "variant/inventoryutil.h"

PgItemOptionMgr::PgItemOptionMgr(void)
{
}

PgItemOptionMgr::~PgItemOptionMgr(void)
{
}

void PgItemOptionMgr::Clear()
{
	m_kContOptValue.clear();
	m_kContOptCandi.clear();
}

void PgItemOptionMgr::swap(PgItemOptionMgr& rkRight)
{
	m_kContOptValue.swap(rkRight.m_kContOptValue);
	m_kContOptCandi.swap(rkRight.m_kContOptCandi);
}

bool PgItemOptionMgr::Build(CONT_TBL_DEF_ITEM_OPTION const &kItemOption)
{
	bool bReturn = true;
	Clear();

	static eEquipLimit const aeLimits[]= //장착부위.
		{	EQUIP_LIMIT_HAIR,	
			EQUIP_LIMIT_FACE,	
			EQUIP_LIMIT_SHOULDER,
			EQUIP_LIMIT_CLOAK,
			EQUIP_LIMIT_GLASS,	
			EQUIP_LIMIT_WEAPON,	
			EQUIP_LIMIT_SHEILD,	
			EQUIP_LIMIT_NECKLACE,
			EQUIP_LIMIT_EARRING	,
			EQUIP_LIMIT_RING,	
			EQUIP_LIMIT_BELT,	
			EQUIP_LIMIT_ATTSTONE,
			EQUIP_LIMIT_MEDAL,
			EQUIP_LIMIT_HELMET	,
			EQUIP_LIMIT_SHIRTS	,
			EQUIP_LIMIT_PANTS	,
			EQUIP_LIMIT_BOOTS	,
			EQUIP_LIMIT_GLOVE	,
			EQUIP_LIMIT_ARM		,
			EQUIP_LIMIT_PET		,
			EQUIP_LIMIT_KICKBALL
		};
	size_t const pos_count = _countof(aeLimits);

	//능력치 찾기 쉽도록 셋팅.
	CONT_TBL_DEF_ITEM_OPTION::const_iterator opt_itr = kItemOption.begin();
	for( ; opt_itr != kItemOption.end() ; ++opt_itr )
	{//DB의 전체 내용으로.
		CONT_ITEM_OPTION_VALUE::mapped_type const &kOptElem = *opt_itr;
		
		size_t pos_index = 0;
		while(pos_count > pos_index)
		{
			if(aeLimits[pos_index] & kOptElem.iAbleEquipPos)
			{//맞는 포지션
				CONT_ITEM_OPTION_VALUE::key_type kOptValueKey(kOptElem.iOptionType, CItemDef::EquipLimitToPos(aeLimits[pos_index]), kOptElem.iOptionGroup);//타입, pos 그룹
				m_kContOptValue.insert( std::make_pair(kOptValueKey, kOptElem) );

				CONT_OPTION_CANDI::key_type kKey(CItemDef::EquipLimitToPos(aeLimits[pos_index]), kOptElem.iOptionGroup);//타입, pos 그룹
				auto ret = m_kContOptCandi.insert( std::make_pair(kKey, CONT_OPTION_CANDI::mapped_type()) );
				if(kOptElem.iOutRate && kOptElem.iOptionType)
				{
					auto sub_ret = ret.first->second.insert(std::make_pair(kOptElem.iOptionType, kOptElem.iOutRate));

					if(!sub_ret.second)
					{	//ret.first->second.find(kOptElem.iAbilType);
						ASSERT_LOG(false, BM::LOG_LV1, __FL__<<L"Build Candi, Already use (Pos & Type)");
						bReturn = false;
					}
				}
			}
			++pos_index;
		}
	}

	//해당 위치와 그룹이 만들어 낼 수 있는 타입과 확률 정리.

	



/*
	//Type Pos Group 으로 입력된 상태.
	CONT_TBL_DEF_ITEM_OPTION::const_iterator opt_itor = kItemOption.begin();
	while(opt_itor != kItemOption.end())
	{//OptionCandi Ready 시키는 부분. 실제 데이터는 안들어감 
		int const iAblePos = (*opt_itor).second.iAbleEquipPos;
		int const iOptType = (*opt_itor).second.iOptionType;
		int const iGroup = (*opt_itor).second.iOptionGroup;

		for(size_t idx = 0; pos_count > idx; ++idx)
		{
			PrepareOptCandi(aeLimits[idx], iGroup, iAblePos, iOptType);
		}
		++opt_itor;
	}
	// 장착 위치면 등록 대기줄 만듬.
	//////////////////////////////////////////////////////////////////////





	///////////////////////////////////////////////////////////////////
	// 실제 옵션 등록
	CONT_OPTION_CANDI::iterator candi_itor = m_kContOptCandi.begin();
	while(candi_itor != m_kContOptCandi.end())
	{
		CONT_OPTION_CANDI::mapped_type &candi_elem = (*candi_itor).second;//타입, OutRate
		
		int iAccRate = 0;
		CONT_OPTION_CANDI::mapped_type::iterator candi_key_itor = candi_elem.begin();
		while(candi_key_itor != candi_elem.end())
		{
//			TBL_TRIPLE_KEY_INT((*candi_key_itor).first, pos
//			(*candi_key_itor).first
			CONT_TBL_DEF_ITEM_OPTION::const_iterator opt_itor = kItemOption.find();//옵션 타입으로 서치 해서 확률 뽑음.
			if(opt_itor != kItemOption.end())
			{
				(*candi_key_itor).second = (*opt_itor).second.iOutRate;
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Already Used Option");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
				bReturn = false;
			}

			++candi_key_itor;
		}
		
		++candi_itor;
	}
*/
	return bReturn;
}

int PgItemOptionMgr::DiceRarity(int const iControlType)
{
	CONT_DEF_ITEM_RARITY_CONTROL const *pkRarityControl = NULL;
	g_kTblDataMgr.GetContDef(pkRarityControl);

	if ( pkRarityControl )
	{
		CONT_DEF_ITEM_RARITY_CONTROL::const_iterator find_itor = pkRarityControl->find(iControlType);
		if(find_itor != pkRarityControl->end())
		{
			size_t iIndex = 0;
			if(RouletteRate((*find_itor).second.iSuccessRateControlNo, iIndex, IG_MAX))
			{
				short nRet = 0;
				if(S_OK == GenRarityValue((E_ITEM_GRADE const)iIndex, nRet,true))
				{
					return nRet;
				}
			}
		}
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgItemOptionMgr::DicePlusUp( int const iControlType, int const iMaxPlusUpLv )
{
	CONT_DEF_ITEM_PLUSUP_CONTROL const *pkPlusUpControl = NULL;
	g_kTblDataMgr.GetContDef(pkPlusUpControl );

	if ( pkPlusUpControl )
	{
		CONT_DEF_ITEM_PLUSUP_CONTROL::const_iterator ctl_itr = pkPlusUpControl->find( iControlType );
		if ( ctl_itr != pkPlusUpControl->end() )
		{
			std::list<int> kListSuccessControlRate;

			int iLowEnchantLv = 0;
			if ( ctl_itr->second.iSuccessRateControlNo_Low )
			{
				kListSuccessControlRate.push_back( ctl_itr->second.iSuccessRateControlNo_Low );
			}
			else
			{
				iLowEnchantLv = MAX_SUCCESS_RATE_ARRAY;
			}

			int iMaxIndex = iMaxPlusUpLv - iLowEnchantLv;
			if ( iMaxPlusUpLv >= MAX_SUCCESS_RATE_ARRAY )
			{
				if ( ctl_itr->second.iSuccessRateControlNo_High )
				{
					kListSuccessControlRate.push_back( ctl_itr->second.iSuccessRateControlNo_High );
				}
				else
				{
					iMaxIndex = MAX_SUCCESS_RATE_ARRAY;
				}
			}

			int iRet = 0;
			if ( true == ::RouletteRateEx( kListSuccessControlRate, iRet, iMaxIndex ) )
			{
				iRet += iLowEnchantLv;
			}
			return iRet;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgItemOptionMgr::DiceOptionLv(int const iItemLv)const
{
//	=아이템제한레벨/7+Rand(2, -4)
	int iLv = (iItemLv/5) + BM::Rand_Range(2, -2);
	iLv = std::min(iLv, MAX_DICE_ITEM_OPTION_LEVEL);//최대 15. 31
	iLv = std::max(iLv, 1);//최하 1. -> 비트 플래그가 4 bit.
	return iLv;
}

int PgItemOptionMgr::DiceOptionType(bool const bIsCashItem, int const iOptGroup, int const iEquipPos, std::list< int > const &kContIgnoreType)const
{
	CONT_OPTION_CANDI::const_iterator candi_itor = m_kContOptCandi.find(CONT_OPTION_CANDI::key_type(iEquipPos,iOptGroup));

	if(candi_itor == m_kContOptCandi.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_OPTION_CANDI::mapped_type kElement = (*candi_itor).second;//복사본을 뜸.

	std::list< int >::const_iterator ig_itor = kContIgnoreType.begin();
	while(ig_itor != kContIgnoreType.end())
	{
		kElement.erase((*ig_itor));//없엠.
		++ig_itor;
	}

	if(true == bIsCashItem)
	{//캐시아이템 이면.
		CONT_CASHITEMABILFILTER const * pkDef = NULL;
		g_kTblDataMgr.GetContDef(pkDef);
		if(NULL != pkDef)
		{
			CONT_OPTION_CANDI::mapped_type::iterator itor = kElement.begin();
			while(itor != kElement.end())
			{//내 타입들 돌면서.
				CONT_ITEM_OPTION_VALUE::const_iterator opt_itor = m_kContOptValue.find(CONT_ITEM_OPTION_VALUE::key_type((*itor).first, iEquipPos, iOptGroup) );//옵션 후보군을 돌면서??
				if(opt_itor == m_kContOptValue.end())//옵션 후보군에 없으면?? 이게 뭐냐..
				{
					itor = kElement.erase(itor);
				}
				else
				{
					CONT_CASHITEMABILFILTER::const_iterator filter = pkDef->find((*opt_itor).second.iAbilType);
					if(filter != pkDef->end())
					{
						itor = kElement.erase(itor);
					}
					else
					{
						++itor;
					}
				}
			}
		}
	}

	if(!kElement.size())
	{//뽑아낼것이 없다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int iTotalRate = 0;

	CONT_OPTION_CANDI::mapped_type::const_iterator element_itor = kElement.begin();
	while(element_itor != kElement.end())
	{
		iTotalRate += (*element_itor).second;
		++element_itor;
	}

	if( 0 < iTotalRate)
	{
		int const iRetRate = (BM::Rand_Index(iTotalRate));
		
		int iAccRate = 0;

		element_itor = kElement.begin();
		while(element_itor != kElement.end())
		{
			iAccRate += (*element_itor).second;
			if(iRetRate < iAccRate)
			{
				return (*element_itor).first;
			}
			++element_itor;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

bool PgItemOptionMgr::CanAddOption(PgBase_Item const & rkItem)const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());

	if(!pkItemDef 
	|| !pkItemDef->CanEquip() || (ICMET_Cant_SoulCraft & pkItemDef->GetAbil(AT_ATTRIBUTE)))//장착 안되면 옵션 못단다.
	{//소울 불가 옵션일 경우 영력도 붙으면 안됨
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgItemOptionMgr::GetItemOptionCount(E_ITEM_GRADE const kItemGrade, int & iGenOptionCount) const
{
	const CONT_DEF_ITEM_RARITY_UPGRADE* pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);
	CONT_DEF_ITEM_RARITY_UPGRADE::const_iterator itor = pCont->find(kItemGrade);
	if(itor == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	iGenOptionCount = (*itor).second.iItemOptionCount;

	return true;
}

bool PgItemOptionMgr::GenerateOption_Sub(PgBase_Item& rkItem,int const iCustomItemLimit) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool const bIsCashItem = pkItemDef->IsType(ITEM_TYPE_AIDS);

	int const iLimitLv = (iCustomItemLimit == 0 ? pkItemDef->GetAbil(AT_LEVELLIMIT) : iCustomItemLimit );// 레벨.

	E_ITEM_GRADE const kItemGrade = GetItemGrade(rkItem);
	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.

	int const iCurseRate = kItemGrade*10;
	if(iCurseRate > (int)BM::Rand_Index(100))
	{
		kEnchantInfo.IsCurse(true);
	}

	int iGenOptionCount = 0;

	if(!GetItemOptionCount(kItemGrade,iGenOptionCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iEquipPos = pkItemDef->EquipPos();

	kEnchantInfo.BasicType1(0);
	kEnchantInfo.BasicType2(0);
	kEnchantInfo.BasicType3(0);
	kEnchantInfo.BasicType4(0);

	kEnchantInfo.BasicLv1(0);
	kEnchantInfo.BasicLv2(0);
	kEnchantInfo.BasicLv3(0);
	kEnchantInfo.BasicLv4(0);

	while(0 < iGenOptionCount)
	{
		--iGenOptionCount;

		std::list< int > kContIgnore;
		kEnchantInfo.BasicOptionTypes(kContIgnore);

		if(!kEnchantInfo.BasicType1())
		{
			kEnchantInfo.BasicType1(DiceOptionType(bIsCashItem,0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv1(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType2())
		{
			kEnchantInfo.BasicType2(DiceOptionType(bIsCashItem,0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv2(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType3())
		{
			kEnchantInfo.BasicType3(DiceOptionType(bIsCashItem,0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv3(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType4())
		{
			kEnchantInfo.BasicType4(DiceOptionType(bIsCashItem,0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv4(DiceOptionLv(iLimitLv));
			continue;
		}
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.

	// <<<<<<<<<<<<<<<< Stat Track start
	if (pkItemDef->CanEquip() && 
		iEquipPos == EQUIP_POS_WEAPON)
	{
		SStatTrackInfo kStatTrackInfo;
		if (50 >= BM::Rand_Index(100))
		{
			kStatTrackInfo.HasStatTrack(true);
		}
		rkItem.StatTrackInfo(kStatTrackInfo);
	}
	// >>>>>>>>>>>>>>>> Stat Track end
	return true;	
}

bool PgItemOptionMgr::GenerateOption(PgBase_Item& rkItem, int const iRatiryControlNo) const
{
	GenerateOption_Rare(rkItem);// 레어 아이템 옵션 이건 무조건 생성된다. 단 레어 아이템 이라면..

	if(GIOT_NONE == iRatiryControlNo || !CanAddOption(rkItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iLimitLv = pkItemDef->GetAbil(AT_LEVELLIMIT);// 레벨.

	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.
	
	int const iRarity = DiceRarity(iRatiryControlNo);
	kEnchantInfo.Rarity(iRarity);

	PgBase_Item kTempItem;//아이템등급을 뽑기위한 임시 값.
	kTempItem.EnchantInfo(kEnchantInfo);
	E_ITEM_GRADE const kItemGrade = GetItemGrade(kTempItem);

	if(IG_NORMAL < kItemGrade)
	{//무조건 봉인.
		kEnchantInfo.IsSeal(true);
		rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.
		return true;
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.
	return GenerateOption_Sub(rkItem);
}

bool PgItemOptionMgr::ReplaceOption(int const iIdx, PgBase_Item& rkItem) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool const bIsCashItem = pkItemDef->IsType(ITEM_TYPE_AIDS);

	int const iLimitLv = pkItemDef->GetAbil(AT_LEVELLIMIT);// 레벨.

	E_ITEM_GRADE const kItemGrade = GetItemGrade(rkItem);

	int iGenOptionCount = 0;

	if(!GetItemOptionCount(kItemGrade,iGenOptionCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	if(iGenOptionCount <= iIdx)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iEquipPos = pkItemDef->EquipPos();

	//등급 뽑고. 추가 하나. 
	// 체크 해서 주자.
	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.

	std::list< int > kContIgnore;
	kEnchantInfo.BasicOptionTypes(kContIgnore);

	switch(iIdx)
	{
	case 0:
		{
			kEnchantInfo.BasicType1(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv1(DiceOptionLv(iLimitLv));
		}break;
	case 1:
		{
			kEnchantInfo.BasicType2(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv2(DiceOptionLv(iLimitLv));
		}break;
	case 2:
		{
			kEnchantInfo.BasicType3(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv3(DiceOptionLv(iLimitLv));
		}break;
	case 3:
		{
			kEnchantInfo.BasicType4(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv4(DiceOptionLv(iLimitLv));
		}break;
	default:
		{
			return false;
		}break;
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.
	return true;
}

bool PgItemOptionMgr::ReDiceOption(PgBase_Item& rkItem) const
{//옵션 하나 더 붙이는거임.
	if(!CanAddOption(rkItem))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	bool const bIsCashItem = pkItemDef->IsType(ITEM_TYPE_AIDS);

	int const iLimitLv = pkItemDef->GetAbil(AT_LEVELLIMIT);// 레벨.

	//등급 뽑고. 추가 하나. 
	// 체크 해서 주자.
	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.

	E_ITEM_GRADE const kItemGrade = GetItemGrade(rkItem);

	int iGenOptionCount = 0;

	if(!GetItemOptionCount(kItemGrade,iGenOptionCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	int const iEquipPos = pkItemDef->EquipPos();


/*  이전 옵션 초기화 하고 다시 할당 하는 동작을 삭제함 
	kEnchantInfo.BasicType1(0);
	kEnchantInfo.BasicType2(0);
	kEnchantInfo.BasicType3(0);
	kEnchantInfo.BasicType4(0);
*/

	// 기존 옵션 개수를 파악 하고 더 추가 할 수 있는 옵션의 개수를 계산한다.
	iGenOptionCount = iGenOptionCount - kEnchantInfo.OptionCount();
	iGenOptionCount = std::max(iGenOptionCount,0);

	while(iGenOptionCount)
	{
		--iGenOptionCount;

		std::list< int > kContIgnore;
		kEnchantInfo.BasicOptionTypes(kContIgnore);

		if(!kEnchantInfo.BasicType1())
		{
			kEnchantInfo.BasicType1(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv1(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType2())
		{
			kEnchantInfo.BasicType2(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv2(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType3())
		{
			kEnchantInfo.BasicType3(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv3(DiceOptionLv(iLimitLv));
			continue;
		}

		if(!kEnchantInfo.BasicType4())
		{
			kEnchantInfo.BasicType4(DiceOptionType(bIsCashItem, 0, iEquipPos, kContIgnore));
			kEnchantInfo.BasicLv4(DiceOptionLv(iLimitLv));
			continue;
		}
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.
	
	return true;
}

int PgItemOptionMgr::GetBasicAbil(SItemExtOptionKey const &kKey) const
{
	int const idx = kKey.kTrdKey - 1;
	if(	idx >= MAX_ITEM_OPTION_ABIL_NUM 
	||	idx < 0)
	{
		return 0;
	}

	CONT_ITEM_OPTION_VALUE::const_iterator opt_itor = m_kContOptValue.find(CONT_ITEM_OPTION_VALUE::key_type(kKey.kPriKey, kKey.kSecKey, kKey.kQudKey));//type, pos, grp
	if(opt_itor != m_kContOptValue.end())
	{
		return opt_itor->second.aiValue[idx];
	}

	return 0;
}

bool PgItemOptionMgr::GetBasicAbil(SItemExtOptionKey const &kKey, CAbilObject *pkAbilObj)const
{	
	if(!pkAbilObj 
	|| 0 >= kKey.kTrdKey)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const idx = kKey.kTrdKey - 1;
	if(	idx >= MAX_ITEM_OPTION_ABIL_NUM 
	||	idx < 0)
	{
		return false;
	}

	CONT_ITEM_OPTION_VALUE::const_iterator opt_itor = m_kContOptValue.find(CONT_ITEM_OPTION_VALUE::key_type(kKey.kPriKey, kKey.kSecKey, kKey.kQudKey));//type, pos, grp
	if(opt_itor != m_kContOptValue.end())
	{
		switch ( opt_itor->second.iAbilType )
		{
		case AT_PHY_ATTACK:
			{
				pkAbilObj->AddAbil( AT_PHY_ATTACK_MAX, (*opt_itor).second.aiValue[idx] );
				pkAbilObj->AddAbil( AT_PHY_ATTACK_MIN, (*opt_itor).second.aiValue[idx] );
			}break;
		case AT_R_PHY_ATTACK:
			{
				pkAbilObj->AddAbil( AT_R_PHY_ATTACK_MAX, (*opt_itor).second.aiValue[idx] );
				pkAbilObj->AddAbil( AT_R_PHY_ATTACK_MIN, (*opt_itor).second.aiValue[idx] );
			}break;
		case AT_MAGIC_ATTACK:
			{
				pkAbilObj->AddAbil( AT_MAGIC_ATTACK_MAX, (*opt_itor).second.aiValue[idx] );
				pkAbilObj->AddAbil( AT_MAGIC_ATTACK_MIN, (*opt_itor).second.aiValue[idx] );
			}break;
		case AT_R_MAGIC_ATTACK:
			{
				pkAbilObj->AddAbil( AT_R_MAGIC_ATTACK_MAX, (*opt_itor).second.aiValue[idx] );
				pkAbilObj->AddAbil( AT_R_MAGIC_ATTACK_MIN, (*opt_itor).second.aiValue[idx] );
			}break;
		default:
			{
				pkAbilObj->AddAbil((*opt_itor).second.iAbilType,(*opt_itor).second.aiValue[idx]);
			}break;
		}
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgItemOptionMgr::DownGrade(int const iIdx, PgBase_Item& rkItem) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	E_ITEM_GRADE const kItemGrade = GetItemGrade(rkItem);

	//등급 뽑고. 추가 하나. 
	// 체크 해서 주자.
	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.

	switch(iIdx)
	{
	case 0:
		{
			if(0 == kEnchantInfo.BasicType1() || 0 == kEnchantInfo.BasicLv1())
			{
				return false;
			}
			kEnchantInfo.BasicType1(0);
			kEnchantInfo.BasicLv1(0);
		}break;
	case 1:
		{
			if(0 == kEnchantInfo.BasicType2() || 0 == kEnchantInfo.BasicLv2())
			{
				return false;
			}
			kEnchantInfo.BasicType2(0);
			kEnchantInfo.BasicLv2(0);
		}break;
	case 2:
		{
			if(0 == kEnchantInfo.BasicType3() || 0 == kEnchantInfo.BasicLv3())
			{
				return false;
			}
			kEnchantInfo.BasicType3(0);
			kEnchantInfo.BasicLv3(0);
		}break;
	case 3:
		{
			if(0 == kEnchantInfo.BasicType4() || 0 == kEnchantInfo.BasicLv4())
			{
				return false;
			}
			kEnchantInfo.BasicType4(0);
			kEnchantInfo.BasicLv4(0);
		}break;
	default:
		{
			return false;
		}break;
	}

	short nRet = 0;
	E_ITEM_GRADE kNewGrade = std::max(IG_NORMAL,static_cast<E_ITEM_GRADE>(kItemGrade - 1));

	if(S_OK == GenRarityValue(kNewGrade, nRet))
	{
		kEnchantInfo.Rarity( nRet );//새로운 등급 적용
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.
	return true;
}

bool PgItemOptionMgr::GenerateOption_Rare(PgBase_Item& rkItem) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkItem.ItemNo());
	if(!pkItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkItemDef->CanEquip() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SEnchantInfo kEnchantInfo = rkItem.EnchantInfo();//참조형 안됨.

	// 메직 옵션 생성
	{
		CONT_RAREOPT_MAGIC const * pkDef = NULL;
		g_kTblDataMgr.GetContDef(pkDef);
		if(NULL == pkDef)
		{
			return false;
		}

		CONT_RAREOPT_MAGIC::const_iterator iter = pkDef->find(pkItemDef->GetAbil(AT_RARE_OPTION_MAGIC));
		if(iter != pkDef->end())
		{
			size_t kIdx = 0;
			if(true == RouletteRate((*iter).second.iSuccessControlNo, kIdx, MAX_RAREOPT_MAGIC_ARRAY_NUM))
			{
				kEnchantInfo.MagicOpt((*iter).second.iMagicNo[kIdx]);
			}
		}
	}

	// 스킬 옵션 생성
	{
		CONT_RAREOPT_SKILL const * pkDef = NULL;
		g_kTblDataMgr.GetContDef(pkDef);
		if(NULL == pkDef)
		{
			return false;
		}

		CONT_RAREOPT_SKILL::const_iterator iter = pkDef->find(pkItemDef->GetAbil(AT_RARE_OPTION_SKILL));
		if(iter != pkDef->end())
		{
			size_t kIdx = 0;
			if(true == RouletteRate((*iter).second.iSuccessControlNo, kIdx, MAX_RAREOPT_SKILL_ARRAY_NUM))
			{
				kEnchantInfo.SkillOpt((*iter).second.iSkillIdx[kIdx]);
			}

			size_t kSkillLv = 0;
			if(true == RouletteRate((*iter).second.iSkillLvSuccessControlNo, kSkillLv, MAX_RAREOPT_SKILL_LEVEL))
			{
				kEnchantInfo.SkillOptLv((kSkillLv+1));
			}
		}
	}

	// 일반 추가 옵션 생성
	{
		bool const bIsCashItem = pkItemDef->IsType(ITEM_TYPE_AIDS);

		int const iOptGroup = pkItemDef->OptionGroup();
		if(0 < iOptGroup)
		{
			int const iLimitLv = pkItemDef->GetAbil(AT_LEVELLIMIT);// 레벨.

			int iGenOptionCount = BM::Rand_Range(3,1);

			int const iEquipPos = pkItemDef->EquipPos();

			kEnchantInfo.RareOptType1(0);
			kEnchantInfo.RareOptType2(0);
			kEnchantInfo.RareOptType3(0);
			kEnchantInfo.RareOptType4(0);

			kEnchantInfo.RareOptLv1(0);
			kEnchantInfo.RareOptLv2(0);
			kEnchantInfo.RareOptLv3(0);
			kEnchantInfo.RareOptLv4(0);

			while(0 < iGenOptionCount)
			{
				--iGenOptionCount;

				std::list< int > kContIgnore;
				kEnchantInfo.RareOptionTypes(kContIgnore);

				if(!kEnchantInfo.RareOptType1())
				{
					kEnchantInfo.RareOptType1(DiceOptionType(bIsCashItem, iOptGroup, iEquipPos, kContIgnore));
					kEnchantInfo.RareOptLv1(DiceOptionLv(iLimitLv));
					continue;
				}

				if(!kEnchantInfo.RareOptType2())
				{
					kEnchantInfo.RareOptType2(DiceOptionType(bIsCashItem, iOptGroup, iEquipPos, kContIgnore));
					kEnchantInfo.RareOptLv2(DiceOptionLv(iLimitLv));
					continue;
				}

				if(!kEnchantInfo.RareOptType3())
				{
					kEnchantInfo.RareOptType3(DiceOptionType(bIsCashItem, iOptGroup, iEquipPos, kContIgnore));
					kEnchantInfo.RareOptLv3(DiceOptionLv(iLimitLv));
					continue;
				}

				if(!kEnchantInfo.RareOptType4())
				{
					kEnchantInfo.RareOptType4(DiceOptionType(bIsCashItem, iOptGroup, iEquipPos, kContIgnore));
					kEnchantInfo.RareOptLv4(DiceOptionLv(iLimitLv));
					continue;
				}
			}
		}
	}

	rkItem.EnchantInfo(kEnchantInfo);//덮어쓰기.

	return true;
}
