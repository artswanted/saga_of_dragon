#include "stdafx.h"
#include "tabledatamanager.h"
#include "PgEventView.h"
#include "PgControlDefMgr.h"
#include "PgPetHatchMgr.h"

bool SPetBonusStatusSelector::PopBonusStatus( PgBase_Item &rkPetItem )const
{
	if ( kTypeList.size() && kLevelList.size() )
	{
		int iRandValue = BM::Rand_Index( 100 );
		if ( iSelectRate > iRandValue )
		{
			iRandValue = BM::Rand_Index( kTypeList.back().iRate );
			
			int iType = AT_NONE;
			int iValueID = 0;

			LIST_PET_BONUS_STATUS::const_iterator itr = kTypeList.begin();
			for ( ; itr != kTypeList.end() ; ++itr )
			{
				if ( itr->iRate > iRandValue )
				{
					iType = itr->iValue;
					break;
				}
			}

			if ( AT_NONE != iType )
			{// AT_NONE면 꽝이지롱~~ 꽝꽝꽝~~ ㅋ
				iRandValue = BM::Rand_Index( kLevelList.back().iRate );

				SEnchantInfo kEnchantInfo = rkPetItem.EnchantInfo();

				itr = kLevelList.begin();
				for ( ; itr != kLevelList.end() ; ++ itr )
				{
					if ( itr->iRate > iRandValue )
					{
						if ( 0 == kEnchantInfo.RareOptType1() )
						{
							kEnchantInfo.RareOptType1( iType );
							kEnchantInfo.RareOptLv1( itr->iValue );
						}
						else if ( 0 == kEnchantInfo.RareOptType2() )
						{
							kEnchantInfo.RareOptType2( iType );
							kEnchantInfo.RareOptLv2( itr->iValue );
						}
						else if ( 0 == kEnchantInfo.RareOptType3() )
						{
							kEnchantInfo.RareOptType3( iType );
							kEnchantInfo.RareOptLv3( itr->iValue );
						}
						else if ( 0 == kEnchantInfo.RareOptType4() )
						{
							kEnchantInfo.RareOptType4( iType );
							kEnchantInfo.RareOptLv4( itr->iValue );
						}
						else
						{
							return false;
						}

						rkPetItem.EnchantInfo( kEnchantInfo );
						return true;
					}
				}
			}
		}
	}
	return false;
}

PgPetHatchAction::PgPetHatchAction(void)
{

}

PgPetHatchAction::~PgPetHatchAction(void)
{

}

bool PgPetHatchAction::Init( PgClassPetDefMgr const &rkClassPetDefMgr, CONT_DEF_PET_HATCH::mapped_type const &kPetHatch, CONT_DEF_PET_BONUSSTATUS const &kContPetBonusStatus )
{
	bool bTotalRet = true;
	int iTotalRate = 0;
	for ( int i = 0 ; i < PET_HATCH_MAX_CLASS ; ++i )
	{
		if ( kPetHatch.iClassRate[i] > 0 )
		{
			iTotalRate += kPetHatch.iClassRate[i];

			SClassKey const kClassKey( kPetHatch.iClass[i], kPetHatch.sLevel[i] );
			SPetClassRate kData( kClassKey, iTotalRate );
			if ( 0 != kData.iClass )
			{// 0이면 꽝이다.
				PgClassPetDef kPetDef;
				if ( !rkClassPetDefMgr.GetDef( kClassKey, &kPetDef ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found PetClass<" << kClassKey.iClass << L"> Level<" << kClassKey.nLv << L">" );
					return false;
				}
			}
			
			bool bRet = true;
			for ( int j = 0; j < PET_HATCH_MAX_BONUSSTATUS ; ++ j )
			{
				if ( 0 != kPetHatch.iBonusStatusRateID[i][j] )
				{
					CONT_DEF_PET_BONUSSTATUS::const_iterator bonus_itr = kContPetBonusStatus.find( kPetHatch.iBonusStatusRateID[i][j] );
					if ( bonus_itr == kContPetBonusStatus.end() )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found BonusStatusID<" << kPetHatch.iBonusStatusRateID[i][j] << L"> In PetHatchID<" << kPetHatch.iID << L">" );
						return false;
					}

					SPetBonusStatusSelector kBonusSelector;
					kBonusSelector.iSelectRate = std::min( static_cast<int>(bonus_itr->second.bySelectRate), static_cast<int>(100) );

					int iTotalRate2 = 0;
					for ( int i = 0; i<MAX_PET_BONUSSTATUS_ITEMOPT2_NUM ; ++i )
					{
						if ( bonus_itr->second.iItemOpt2_Rate[i] > 0 )
						{
							iTotalRate2 += bonus_itr->second.iItemOpt2_Rate[i];
							kBonusSelector.kTypeList.push_back( SPetBonusStatus( bonus_itr->second.iItemOpt2_OptionType[i], iTotalRate2 ) );
						}
					}

					iTotalRate2 = 0;
					for ( int i = 0 ; i < MAX_ITEM_OPTION_ABIL_NUM ; ++i )
					{
						if ( bonus_itr->second.iItemOpt2_ValueLvRate[i] > 0 )
						{
							iTotalRate2 += bonus_itr->second.iItemOpt2_ValueLvRate[i];
							kBonusSelector.kLevelList.push_back( SPetBonusStatus( i+1, iTotalRate2 ) );
						}
					}

					if ( true == bRet )
					{
						kData.kBonusStatusSelectorList.push_back( kBonusSelector );
					}
				}
			}

			if ( true == bRet )
			{
				m_kListPetClassRate.push_back( kData );
			}
			else
			{
				bTotalRet = false;
			}
		}
	}

	iTotalRate = 0;
	for ( int i = 0; i < PET_HATCH_MAX_PERIOD ; ++i )
	{
		if ( kPetHatch.iPeriodRate[i] > 0 )
		{
			iTotalRate += kPetHatch.iPeriodRate[i];
			m_kListPetPeriodRate.push_back( SPetPeriodRate( kPetHatch.sPeriod[i], iTotalRate ) );
		}
	}

	return bTotalRet;
}

bool PgPetHatchAction::PopPet( PgBase_Item &rkOutPetItem )const
{
	// 1. Pet Type
	int iRandValue = BM::Rand_Index( m_kListPetClassRate.back().iRate );

	LIST_PET_CLASS_RATE::const_iterator class_itr = m_kListPetClassRate.begin();
	for ( ; class_itr != m_kListPetClassRate.end() ; ++class_itr )
	{
		if ( iRandValue < class_itr->iRate )
		{
			break;
		}
	}

	if ( class_itr == m_kListPetClassRate.end() )
	{
		// 로직상 이렇게 나올 수가 없는데..
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!");
		return false;
	}

	SClassKey const kClassKey( class_itr->iClass, class_itr->nLv );

	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);

	PgClassPetDef kPetDef;
	if ( !kClassPetDefMgr.GetDef( kClassKey, &kPetDef ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found PetClass<" << kClassKey.iClass << L"> Level<" << kClassKey.nLv << L">" );
		return false;
	}

	// Pet Item을 만들고!!
	int const iItemNo = kPetDef.GetAbil( AT_PARENT_ITEM_NO );
	if ( 0 < iItemNo )
	{
		HRESULT const hRet = ::CreateSItem( iItemNo, 1, 0, rkOutPetItem );

		PgItem_PetInfo *pkPetInfo = NULL;
		if ( true == rkOutPetItem.GetExtInfo( pkPetInfo ) )
		{
			if ( E_FAIL == hRet || ( kClassKey != pkPetInfo->ClassKey()) )
			{
				// 리턴값이 E_FAIL이지만, Pet정보가 있다면, 펫정보를 셋팅해주어야 한다.
				// Pet정보가 없는경우에는 만들기 실패
				CreatePetInfo( kClassKey, pkPetInfo );
			}
			
			// Pet의 능력치를 결정
			LIST_BONUS_STATUS_SELECTOR::const_iterator st_sel_itr = class_itr->kBonusStatusSelectorList.begin();
			for ( ; st_sel_itr != class_itr->kBonusStatusSelectorList.end() ; ++st_sel_itr )
			{
				st_sel_itr->PopBonusStatus( rkOutPetItem );
			}

			// Pet의 이용기간을 결정
			if ( m_kListPetPeriodRate.size() )
			{
				iRandValue = BM::Rand_Index( m_kListPetPeriodRate.back().iRate );
				LIST_PET_PERIOD_RATE::const_iterator period_itr = m_kListPetPeriodRate.begin();
				for ( ; period_itr != m_kListPetPeriodRate.end() ; ++period_itr )
				{
					if ( iRandValue < period_itr->iRate )
					{
						if ( period_itr->sPeriod > 0 )
						{// 이값이 0이면 영구펫
							rkOutPetItem.SetUseTime( UIT_DAY, static_cast<__int64>(period_itr->sPeriod) );
						}
						break;
					}
				}
			}
			return true;
		}
	}
	 
	VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Craete Failed PetItem<" << iItemNo << L"> PetClass<" << kClassKey.iClass << L"> Level<" << kClassKey.nLv << L">" );
	return false;
}

PgPetHatchMgr::PgPetHatchMgr(void)
{

}

PgPetHatchMgr::~PgPetHatchMgr(void)
{

}

bool PgPetHatchMgr::Build( PgClassPetDefMgr const &rkClassPetDefMgr, CONT_DEF_PET_HATCH const &rkDefPetHatch, CONT_DEF_PET_BONUSSTATUS const &rkDefPetBonusStatus )
{
	bool bRet = true;

	CONT_DEF_PET_HATCH::const_iterator hatch_itr = rkDefPetHatch.begin();
	for ( ; hatch_itr != rkDefPetHatch.end() ; ++hatch_itr )
	{
		PgPetHatchAction kPetHatchAction;
		if ( true == kPetHatchAction.Init( rkClassPetDefMgr, hatch_itr->second, rkDefPetBonusStatus ) )
		{
			m_kContPetHatchAction.insert( std::make_pair( hatch_itr->first, kPetHatchAction ) );

			LIST_PET_CLASS_RATE const & kCont = kPetHatchAction.PetClassRate();

			for(LIST_PET_CLASS_RATE::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
			{
				m_kContPetClassRate.insert(std::make_pair((*iter),(*iter)));
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"PetHatch<" << hatch_itr->first << L"> Init Failed" );
			bRet = false;
		}
	}

	return bRet;
}

bool PgPetHatchMgr::PopPet( int const iPetHatchNo, PgBase_Item &rkOutPetItem )const
{
	CONT_PET_HATCH_ACTION::const_iterator itr = m_kContPetHatchAction.find( iPetHatchNo );
	if ( itr != m_kContPetHatchAction.end() )
	{
		return itr->second.PopPet( rkOutPetItem );
	}
	return false;
}

bool PgPetHatchMgr::RedicePetOption(PgBase_Item const & kItem,SEnchantInfo & kNewEnchantInfo) const
{
	PgItem_PetInfo *pkPetInfo = NULL;
	if ( false == kItem.GetExtInfo( pkPetInfo ) )
	{
		return false;
	}

	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
	PgClassPetDef kPetDef;
	if ( !kClassPetDefMgr.GetDef( pkPetInfo->ClassKey(), &kPetDef ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found PetClass<" << pkPetInfo->ClassKey().iClass << L"> Level<" << pkPetInfo->ClassKey().nLv << L">" );
		return false;
	}

	CONT_PET_CLASS_RATE::const_iterator iter;
	if(kPetDef.GetPetType() == EPET_TYPE_3) //라이딩펫은 등급으로만 찾고 레벨은 무시(1로 설정)..
	{
		SClassKey const kClassKey( pkPetInfo->ClassKey().iClass, 1 );
		iter = m_kContPetClassRate.find(kClassKey);
		if(iter == m_kContPetClassRate.end())
		{
			return false;
		}
	}
	else
	{
		iter = m_kContPetClassRate.find(pkPetInfo->ClassKey());
		if(iter == m_kContPetClassRate.end())
		{
			return false;
		}
	}

//	CONT_PET_CLASS_RATE::const_iterator iter = m_kContPetClassRate.find(pkPetInfo->ClassKey());
//	if(iter == m_kContPetClassRate.end())
//	{
//		return false;
//	}

	PgBase_Item kCopyItem = kItem;

	SEnchantInfo kEnchantInfo = kCopyItem.EnchantInfo();

	kEnchantInfo.RareOptType1(0);
	kEnchantInfo.RareOptType2(0);
	kEnchantInfo.RareOptType3(0);
	kEnchantInfo.RareOptType4(0);

	kEnchantInfo.RareOptLv1(0);
	kEnchantInfo.RareOptLv2(0);
	kEnchantInfo.RareOptLv3(0);
	kEnchantInfo.RareOptLv4(0);

	kCopyItem.EnchantInfo(kEnchantInfo);

	LIST_BONUS_STATUS_SELECTOR const & kCont = (*iter).second.kBonusStatusSelectorList;

	for(LIST_BONUS_STATUS_SELECTOR::const_iterator st_sel_itr = kCont.begin();st_sel_itr != kCont.end();++st_sel_itr)
	{
		(*st_sel_itr).PopBonusStatus( kCopyItem );
	}

	kNewEnchantInfo = kCopyItem.EnchantInfo();

	return true;
}