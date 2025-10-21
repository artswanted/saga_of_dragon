#include "StdAfx.h"
#include "Lohengrin/LogGroup.h"
#include "TableDataManager.h"
#include "Item.h"
#include "PgClassPetDefMgr.h"
#include "PgControlDefMgr.h"
#include "BM/LocalMgr.h"

PgClassPetDef::PgClassPetDef( TBL_DEF_CLASS_PET const &kDefPet, SClassPetDefLevel const *pkDefAbil )
:	m_kDefPet(kDefPet)
,	m_pkDefAbil(pkDefAbil)
{
}

PgClassPetDef::PgClassPetDef( PgClassPetDef const &rhs )
:	m_kDefPet(rhs.m_kDefPet)
,	m_pkDefAbil(rhs.m_pkDefAbil)
{

}

int PgClassPetDef::GetClassGrade( int const iClass )
{
	return iClass % 10;
}

int PgClassPetDef::GetBaseClass( int const iClass )
{
	return iClass / 10 * 10;
}

HRESULT PgClassPetDef::IsClassLimit( __int64 const i64ClassLimit, int const iClass )
{
	if ( 9i64 < i64ClassLimit )
	{
		int const iBaseClass = GetBaseClass( iClass );
		int const iBaseClass2 = GetBaseClass( static_cast<int>(i64ClassLimit) );
		if ( iBaseClass != iBaseClass2 )
		{
			return E_FAIL;
		}
	}

	int const iClassGrade = GetClassGrade( iClass );
	if ( iClassGrade >= GetClassGrade( static_cast<int>(i64ClassLimit) ) )
	{
		return S_OK;
	}
	return E_ACCESSDENIED;
}

__int64 PgClassPetDef::GetPetUpgradeCost(PgItem_PetInfo* pkPetItemInfo)
{
	if(NULL == pkPetItemInfo) { return 0; }

	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
	PgClassPetDef kPetDef;
	if ( false == kClassPetDefMgr.GetDef( pkPetItemInfo->ClassKey(), &kPetDef ) )
	{
		return 0;
	}

	BYTE byPetType = kPetDef.GetPetType();
	int iPetGrade = GetClassGrade(pkPetItemInfo->ClassKey().iClass);
	bool bCostFreeUpgrade = kPetDef.GetAbil(AT_PET_FREEUPGRADE) > 0;

	switch(g_kLocal.ServiceRegion())
	{
//	case LOCAL_MGR::NC_DEVELOP:
//		{
//		}break;
	default:
		{ //현재는 전국가 가격정책 동일
			if(true == bCostFreeUpgrade)
			{
				return 0;
			}
			else if(byPetType == EPET_TYPE_2) //전투펫
			{
				if(iPetGrade == 0) //1차->2차
				{
					return 10000;
				}
				else if(iPetGrade == 1) //2차->3차
				{
					return 100000;
				}
				else { return 0; } //전직 불가
			}
			else if(byPetType == EPET_TYPE_3) //라이딩펫
			{
				if(iPetGrade == 0) //1차->2차
				{
					return 30000;
				}
				else if(iPetGrade == 1) //2차->3차
				{
					return 200000;
				}
				else { return 0; } //전직 불가
			}
			else { return 0; }
		}break;
	}
	return 0;
}

int PgClassPetDef::GetAbil( WORD const wType )const
{
	if ( m_pkDefAbil )
	{
		switch(wType)
		{
		case AT_CLASS:					{ return m_kDefPet.iClass; }break;
		case AT_LEVEL:					{ return static_cast<int>(m_pkDefAbil->sLevel); }break;
		case AT_MAX_HP:					{ return 100; }break;
		case AT_HP_RECOVERY_INTERVAL:
		case AT_HP_RECOVERY:			{ return 0; }break;
		case AT_MAX_MP:					{ return static_cast<int>(m_pkDefAbil->sMaxMP); }break;
		case AT_MP_RECOVERY_INTERVAL:	{ return static_cast<int>(m_pkDefAbil->sMPRecoveryInterval); }break;
		case AT_MP_RECOVERY:			{ return static_cast<int>(m_pkDefAbil->sMPRecovery); }break;
		case AT_NAMENO:					{ return m_kDefPet.iClassName; }break;
		case AT_EXPERIENCE_TIME:		{ return m_pkDefAbil->iTimeExperience; }break;
		case AT_FRAN:
		case AT_EXPERIENCE:
		case AT_MONEY:
		case AT_DIE_EXP:
		case AT_CLASSLIMIT:
			{
	//			INFO_LOG(BM::LOG_LV0, _T("[%s] int64는 여기에서 뽑으면 안됨"), __FUNCTIONW__);
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Error !!") );
			}break;
		default:
			{
				CLASSDEF_ABIL_CONT::const_iterator itr = m_pkDefAbil->kAbil.find( wType );
				if (itr != m_pkDefAbil->kAbil.end())
				{
					return itr->second;
				}
			}break;
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" m_pkDefAbil == NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkDefAbil is NULL"));
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

__int64 PgClassPetDef::GetAbil64( WORD const wType )const
{
	if ( m_pkDefAbil )
	{
		switch( wType )
		{
		case AT_EXPERIENCE:			{return m_pkDefAbil->i64Experience;}break;
		case AT_EXPERIENCE_TIME:	{return static_cast<__int64>(m_pkDefAbil->iTimeExperience); }break;
		default:
			{ 
	//			INFO_LOG(BM::LOG_LV0, _T("[%s] GetAbil64함수를 사용해서 기입하시오"), __FUNCTIONW__);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" m_pkDefAbil == NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkDefAbil is NULL"));
	}

	return 0i64;
}

bool PgClassPetDef::GetPetItemOption( SEnchantInfo &rkOutEnchantInfo )const
{
	switch( GetPetType() )
	{
	case EPET_TYPE_2:
//	case EPET_TYPE_3: //주의: 이 라인은 절대 주석해제하면 안된다.
		{			  //(현재 라이딩펫은 펫옵션 초기화 이용권땜에 별도의 테이블 처리가 되므로 기존펫과 동일하게 처리되면 안된다)
			CONT_DEFCLASS_PET_ITEMOPTION const *pkDefPetItemOptionType = NULL;
			g_kTblDataMgr.GetContDef(pkDefPetItemOptionType);
			if ( pkDefPetItemOptionType )
			{
				CONT_DEFCLASS_PET_ITEMOPTION::const_iterator item_op_itr = pkDefPetItemOptionType->find( SClassKey(m_kDefPet.iItemOptionIndex, static_cast<short>(GetAbil(AT_LEVEL))) );
				if ( item_op_itr != pkDefPetItemOptionType->end() )
				{
					rkOutEnchantInfo.RareOptType1( item_op_itr->second.iOptionType[0] );
					rkOutEnchantInfo.RareOptLv1( item_op_itr->second.iOptionLevel[0] );
					rkOutEnchantInfo.RareOptType2( item_op_itr->second.iOptionType[1] );
					rkOutEnchantInfo.RareOptLv2( item_op_itr->second.iOptionLevel[1] );
					rkOutEnchantInfo.RareOptType3( item_op_itr->second.iOptionType[2] );
					rkOutEnchantInfo.RareOptLv3( item_op_itr->second.iOptionLevel[2] );
					rkOutEnchantInfo.RareOptType4( item_op_itr->second.iOptionType[3] );
					rkOutEnchantInfo.RareOptLv4( item_op_itr->second.iOptionLevel[3] );
					return true;
				}
			}
		}break;
	}
	return false;
}

PgClassPetDefMgr::PgClassPetDefMgr(void)
{
}

PgClassPetDefMgr::~PgClassPetDefMgr(void)
{
	Clear();
}

bool PgClassPetDefMgr::IsChangeClass( SClassKey const &kClassKey ) const
{
	int const iGrade = PgClassPetDef::GetClassGrade( kClassKey.iClass );
	short const nMinLv = 20 * iGrade + 20;
	if(nMinLv <= kClassKey.nLv)
	{
		SClassKey kNewKey(PgClassPetDef::GetBaseClass(kClassKey.iClass) + iGrade + 1, nMinLv);
		
		PgClassPetDef *pkOutDef = NULL;
		return GetDef(kNewKey, pkOutDef);
	}
	return false;
}

void PgClassPetDefMgr::CraeteLastLv( short const sMaximumLevel )
{
	m_kDefLastLv.clear();

	if ( m_kDef.empty() )
	{
		return;
	}

	int iCheckClass = 0;
	int iLastLevel = 0;

	ContClassPetDef::const_iterator def_itr2 = m_kDef.begin();
	ContClassPetDef::const_iterator def_itr = m_kDef.begin();
	for( ; def_itr != m_kDef.end() ; ++def_itr )
	{
		if ( iCheckClass != def_itr->first.iClass )
		{
			if ( 0 < iCheckClass )
			{
				m_kDefLastLv.insert( std::make_pair( iCheckClass, def_itr2->second ) );
			}

			iCheckClass = def_itr->first.iClass;
			iLastLevel = PgClassPetDef::GetClassGrade( iCheckClass ) * 20 + 20;
		}

		if ( def_itr->second.GetAbil( AT_LEVEL ) <= iLastLevel )
		{
			def_itr2 = def_itr;
		}
	}

	m_kDefLastLv.insert( std::make_pair( iCheckClass, def_itr2->second ) );
}

short PgClassPetDefMgr::GetLastLv( int const iClass )const
{
	ContClassPetLastLv::const_iterator last_lv_itr = m_kDefLastLv.find( iClass );
	if ( last_lv_itr != m_kDefLastLv.end() )
	{
		return last_lv_itr->second.GetAbil(AT_LEVEL);
	}
	return 0;
}

__int64 PgClassPetDefMgr::GetMaxExp( int const iClass )const
{
	ContClassPetLastLv::const_iterator last_lv_itr = m_kDefLastLv.find( iClass );
	if ( last_lv_itr != m_kDefLastLv.end() )
	{
		return last_lv_itr->second.GetAbil64(AT_EXPERIENCE);
	}
	return 0i64;
}

bool PgClassPetDefMgr::Build( CONT_DEFCLASS_PET const &rkDef, CONT_DEFCLASS_PET_LEVEL const &rkDefLv, CONT_DEFCLASS_PET_SKILL const &rkDefSkill, CONT_DEFCLASS_PET_ABIL const &rkAbil, short const sMaximumLevel )
{
	bool bReturn = true;

	Clear();

	CONT_DEFCLASS_PET_LEVEL::const_iterator lv_itr = rkDefLv.begin();
	for ( ; lv_itr != rkDefLv.end() ; ++lv_itr )
	{
		TBL_DEF_CLASS_PET_LEVEL const &kElement = lv_itr->second;
		SClassPetDefLevel * pkElement = new SClassPetDefLevel( static_cast<TBL_DEF_CLASS_PET_LEVEL_BASE>(kElement) );
		if ( pkElement )
		{
			auto kPair = m_kContPool.insert( std::make_pair( lv_itr->first, pkElement ) );
			if ( true == kPair.second )
			{
				for( int i=0; i<MAX_CLASS_ABIL_LIST; ++i )
				{
					int const iNeedAbilNo = kElement.aiAbil[i];
					CONT_DEFCLASS_PET_ABIL::const_iterator abil_itr = rkAbil.find(iNeedAbilNo);
					if ( abil_itr != rkAbil.end() )
					{
						for (  int j=0; j < MAX_CLASS_ABIL_ARRAY; ++j )
						{
							if ( abil_itr->second.iType[j] != 0 && abil_itr->second.iValue[j] != 0 )
							{
								pkElement->kAbil.insert(std::make_pair(abil_itr->second.iType[j], abil_itr->second.iValue[j]));
							}
						}
					}
					else
					{
						if ( iNeedAbilNo )
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV5, _T("Cannot Get ClassPetDef Ability Index[") << lv_itr->first.iClass << _T("], Level[") << lv_itr->first.nLv << _T("], Abil[") << iNeedAbilNo << _T("]") );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							bReturn = false;
						}
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Cannot Insert Index[") << lv_itr->first.iClass << _T("], Level[") << lv_itr->first.nLv << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				bReturn = false;
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Memory New Error!!" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if ( true == bReturn )
	{
		CONT_DEFCLASS_PET::const_iterator def_pet_itr = rkDef.begin();
		for ( ; def_pet_itr != rkDef.end(); ++def_pet_itr )
		{
			ContPoolDefLevel::iterator pool_itr = m_kContPool.end();

			SClassKey kIndexKey( def_pet_itr->second.iLevelIndex, 0 );
			while (		pool_itr == m_kContPool.end()
					&&	100 >= ++kIndexKey.nLv )
			{
				pool_itr = m_kContPool.find( kIndexKey );
			}

			if ( pool_itr != m_kContPool.end() )
			{
				while (		pool_itr != m_kContPool.end()
						&&	pool_itr->first.iClass == kIndexKey.iClass )
				{
					PgClassPetDef kData( def_pet_itr->second, pool_itr->second );
					m_kDef.insert( std::make_pair( SClassKey( def_pet_itr->second.iClass, pool_itr->first.nLv ), kData ) );
					++pool_itr;
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, _T("Cannot find CONT_DEFCLASS_PET_LEVEL Class[") << def_pet_itr->second.iClass << _T("]'s LevelIndex[") << def_pet_itr->second.iLevelIndex << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				bReturn = false;
			}
		}
	}

	if ( true == bReturn )
	{
		CraeteLastLv( sMaximumLevel );
	}

	return bReturn;
}

bool PgClassPetDefMgr::GetDef( SClassKey const &rkKey, PgClassPetDef *pkOutDef )const
{
	ContClassPetDef::const_iterator itr = m_kDef.find(rkKey);
	if( itr != m_kDef.end())
	{
		if ( pkOutDef )
		{
			*pkOutDef = itr->second;
		}
		return true;
	}

	INFO_LOG( BM::LOG_LV4, _T(" Cannot find ClassPetDef Class[") << rkKey.iClass << _T("], Level[") << rkKey.nLv << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return false;
}


// Getting Abil list
// [PARAMETER]
//	iClass : class id
//	sLevel : level
//	pkBasic : [OUT] Basic information to get
// HRESULT PgClassPetDefMgr::GetAbil( SClassKey const &rkKey, SPlayerBasicInfo &rkBasic)const
// {
// 	CLASS_PET_DEF_BUILT const * pkDef = GetDef(rkKey);
// 	if(pkDef == NULL)
// 	{
// 		//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s] GetAbil failed... Class[%d], Level[%hd]"), __FUNCTIONW__, rkKey.iClass, rkKey.nLv);
// 		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" GetAbil failed... Class[") << rkKey.iClass << _T("], Level[") << rkKey.nLv << _T("]") );
// 		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
// 		return E_FAIL;
// 	}
// 
// 	::memset(&rkBasic, 0, sizeof(rkBasic));
// 	rkBasic.iMaxHP = GetAbil(pkDef, AT_MAX_HP);
// 	rkBasic.iHPRecoveryInterval = GetAbil(pkDef, AT_HP_RECOVERY_INTERVAL);
// 	rkBasic.sHPRecovery = GetAbil(pkDef, AT_HP_RECOVERY);
// 	rkBasic.iMaxMP = GetAbil(pkDef, AT_MAX_MP);
// 	rkBasic.iMPRecoveryInterval = GetAbil(pkDef, AT_MP_RECOVERY_INTERVAL);
// 	rkBasic.sMPRecovery = GetAbil(pkDef, AT_MP_RECOVERY_INTERVAL);
// 	rkBasic.sInitStr = GetAbil(pkDef, AT_STR);
// 	rkBasic.sInitInt = GetAbil(pkDef, AT_INT);
// 	rkBasic.sInitCon = GetAbil(pkDef, AT_CON);
// 	rkBasic.sInitDex = GetAbil(pkDef, AT_DEX);
// 	rkBasic.sMoveSpeed = GetAbil(pkDef, AT_MOVESPEED);
// 	rkBasic.sPhysicsDefence = GetAbil(pkDef, AT_PHY_DEFENCE);
// 	rkBasic.sMagicDefence = GetAbil(pkDef, AT_MAGIC_DEFENCE);
// 	rkBasic.sAttackSpeed = GetAbil(pkDef, AT_ATTACK_SPEED);
// 	rkBasic.sBlockRate = GetAbil(pkDef, AT_BLOCK_SUCCESS_VALUE);
// 	rkBasic.sDodgeRate = GetAbil(pkDef, AT_DODGE_SUCCESS_VALUE);
// 	rkBasic.sCriticalRate = GetAbil(pkDef, AT_CRITICAL_SUCCESS_VALUE);
// 	rkBasic.sCriticalPower = GetAbil(pkDef, AT_CRITICAL_POWER);
// 	rkBasic.sHitRate = GetAbil(pkDef, AT_HIT_SUCCESS_VALUE);
// 	rkBasic.iAIType = GetAbil(pkDef, AT_AI_TYPE);
// //	rkBasic.sInvenSize = GetAbil(pkDef, AT_INVEN_SIZE);
// //	rkBasic.sEquipsSize = GetAbil(pkDef, AT_EQUIPS_SIZE);
// 
// 	return S_OK;
// }

void PgClassPetDefMgr::Clear()
{
	m_kDefLastLv.clear();
	m_kDef.clear();

	ContPoolDefLevel::iterator itr = m_kContPool.begin();
	for ( ; itr != m_kContPool.end() ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}
	m_kContPool.clear();
}

void PgClassPetDefMgr::swap( PgClassPetDefMgr &rRight )
{
	m_kContPool.swap(rRight.m_kContPool);
	m_kDef.swap(rRight.m_kDef);
	m_kDefLastLv.swap(rRight.m_kDefLastLv);
}

bool PgClassPetDefMgr::IsLvUp( SClassKey const &kNowKey, __int64 const &i64Exp, SClassKey &kRetKey )const
{
	//Next 레벨의 경험치가 되면. 레벨업이 되는거야.
	short const nLastLv = GetLastLv( kNowKey.iClass );
	if ( nLastLv > kNowKey.nLv )
	{
		SClassKey kNextKey( kNowKey.iClass, kNowKey.nLv + 1 );
		
		ContClassPetDef::const_iterator next_itr = m_kDef.find(kNextKey);
		if( next_itr != m_kDef.end())
		{
			if ( i64Exp >= next_itr->second.GetAbil64( AT_EXPERIENCE ) )
			{
				kRetKey = kNextKey;
				IsLvUp( kNextKey, i64Exp, kRetKey );
				return true;
			}
		}
	}
	return false;
}

HRESULT PgClassPetDefMgr::RevisionClassKey(SClassKey &rkKey, __int64 &i64Exp)const
{//변화 없으면 S_OK;

	ContClassPetLastLv::const_iterator last_lv_itor = m_kDefLastLv.find(rkKey.iClass);

	if(last_lv_itor != m_kDefLastLv.end())
	{
		i64Exp = std::min( last_lv_itor->second.GetAbil64(AT_EXPERIENCE), i64Exp );
		rkKey.nLv = std::min( static_cast<short>(last_lv_itor->second.GetAbil(AT_LEVEL)), rkKey.nLv );
		return S_OK;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find LastLevel information Class=") << rkKey.iClass);
	return E_FAIL;
}

bool PgClassPetDefMgr::CheckData( CONT_DEF_PET_HATCH const &rkDefPetHatch )const
{
	bool bRet = true;

	CONT_DEF_PET_HATCH::const_iterator def_hatch_itr = rkDefPetHatch.begin();
	for ( ; def_hatch_itr != rkDefPetHatch.end() ; ++def_hatch_itr )
	{
		// 존재하는 Class 여야만 한다.
		for ( int i = 0; i < PET_HATCH_MAX_CLASS ; ++i )
		{
			if ( def_hatch_itr->second.iClass[i] )
			{
				SClassKey const kClassKey( def_hatch_itr->second.iClass[i], def_hatch_itr->second.sLevel[i] );
				if ( !GetDef( kClassKey, NULL ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found CONT_DEF_PET_HATCH class<" << kClassKey.iClass << L"> Level <" << kClassKey.nLv << L">" );
					bRet = false;
				}
			}
		}
	}

	return bRet;
}
