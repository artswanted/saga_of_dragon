#include "StdAfx.h"
#include "PgClassDefMgr.h"
#include "TableDataManager.h"
#include "Lohengrin/LogGroup.h"

short PgClassDefMgr::s_sMaximumLevel = 0;

PgClassDefMgr::PgClassDefMgr(void)
	: m_kClassDefPool(100, 50)
{
}

PgClassDefMgr::~PgClassDefMgr(void)
{
	Clear();
}

bool PgClassDefMgr::Build( CONT_DEFCLASS const *pkDef, CONT_DEFCLASS_ABIL const *pkAbil)
{
	bool bReturn = true;
	if (pkDef == NULL || pkAbil == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	//BM::CAutoMutex kLock(m_kMutex);

	Clear();
	
	CONT_DEFCLASS::const_iterator def_tbl_itor = pkDef->begin();
	
	while(def_tbl_itor != pkDef->end())
	{//데프 돌면서.
		CLASS_DEF_BUILT* pkNew = m_kClassDefPool.New();
		pkNew->kDef = (*def_tbl_itor).second;
		pkNew->kAbil.clear();

		SClassKey const kKey(pkNew->kDef.iClass, pkNew->kDef.sLevel);//클래스 키 만들고													

		auto ret = m_kDef.insert(std::make_pair(kKey, pkNew));
		if(!ret.second)
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" Cannot Insert Class[") << kKey.iClass << _T("], Level[") << kKey.nLv << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		for(int j=0; j<MAX_CLASS_ABIL_LIST; j++)
		{
			int const iNeedAbilNo = (*def_tbl_itor).second.aiAbil[j];
			CONT_DEFCLASS_ABIL::iterator itor = ((CONT_DEFCLASS_ABIL*)pkAbil)->find(iNeedAbilNo);
			if (itor != pkAbil->end())
			{
				for (int k=0; k<MAX_CLASS_ABIL_ARRAY; k++)
				{
					if (itor->second.iType != 0 && itor->second.iValue != 0)
					{
						pkNew->kAbil.insert(std::make_pair(itor->second.iType[k], itor->second.iValue[k]));
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, _T("Cannot Get ClassDef Ability Class[") << kKey.iClass << _T("], Level[") << kKey.nLv << _T("], Abil[") << iNeedAbilNo << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
		++def_tbl_itor;
	}

	BuildLastLevel(s_sMaximumLevel);
	return bReturn;
}

bool PgClassDefMgr::BuildLastLevel(int const iMaxLevel)
{
	if (iMaxLevel <= 0)
	{
		return false;
	}
	s_sMaximumLevel = iMaxLevel;

	m_kDefLastLv.clear();

	ContClassDef::iterator def_itor = m_kDef.begin();
	while(def_itor != m_kDef.end())
	{
		const SClassKey &kKey = (*def_itor).first;
		CLASS_DEF_BUILT* pkElement = (*def_itor).second;

		if (pkElement->kDef.i64Experience > 0 && (pkElement->kDef.sLevel <= PgClassDefMgr::s_sMaximumLevel+1))
		{
			// Exp == 0 이면 player class 정보가 아니다.

			ContClassLastLv::iterator last_lv_itor = m_kDefLastLv.find(kKey.iClass);

			if(last_lv_itor != m_kDefLastLv.end())
			{
				if( (*last_lv_itor).second->kDef.sLevel <  pkElement->kDef.sLevel )
				{//새거가 더 쎄다 or s_sMaximumLevel값 검사
					m_kDefLastLv.erase(last_lv_itor);
				}
			}
			m_kDefLastLv.insert(std::make_pair(kKey.iClass, pkElement));
		}

		++def_itor;
	}
	return true;
}

int PgClassDefMgr::GetAbil(const SClassKey &rkKey, WORD const Type)const
{
	//BM::CAutoMutex kLock(m_kMutex);
	const CLASS_DEF_BUILT* pkDef = GetDef(rkKey);
	if (pkDef == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	int iValue = 0;
	switch(Type)
	{
	case AT_HP_RECOVERY_INTERVAL:	{ iValue = pkDef->kDef.sHPRecoveryInterval; }break;
	case AT_HP_RECOVERY:			{ iValue = pkDef->kDef.sHPRecovery; }break;
// 	case AT_MAX_DP:					{ iValue = pkDef->kDef.sMaxDP; }break;
// 	case AT_DP_RECOVERY_INTERVAL:	{ iValue = pkDef->kDef.sDPRecoveryInterval; }break;
// 	case AT_DP_RECOVERY:			{ iValue = pkDef->kDef.sDPRecovery; }break;
	case AT_MAX_MP:					{ iValue = pkDef->kDef.sMaxMP; }break;
	case AT_MP_RECOVERY_INTERVAL:	{ iValue = pkDef->kDef.sMPRecoveryInterval; }break;
	case AT_MP_RECOVERY:			{ iValue = pkDef->kDef.sMPRecovery; }break;
	case AT_STR:					{ iValue = pkDef->kDef.iStr; }break;
	case AT_INT:					{ iValue = pkDef->kDef.iInt; }break;
	case AT_CON:					{ iValue = pkDef->kDef.iCon; }break;
	case AT_DEX:					{ iValue = pkDef->kDef.iDex; }break;
	case AT_MOVESPEED:				{ iValue = pkDef->kDef.sMoveSpeed; }break;
	case AT_STATUS_BONUS:			{ iValue = pkDef->kDef.sBonusStatus; }break;
	case AT_STR_ADD_LIMIT:			{ iValue = pkDef->kDef.sLimitStr; } break;
	case AT_INT_ADD_LIMIT:			{ iValue = pkDef->kDef.sLimitInt; } break;
	case AT_CON_ADD_LIMIT:			{ iValue = pkDef->kDef.sLimitCon; } break;
	case AT_DEX_ADD_LIMIT:			{ iValue = pkDef->kDef.sLimitDex; } break;
	case AT_FRAN:
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
//			INFO_LOG(BM::LOG_LV0, _T("[%s] int64는 여기에서 뽑으면 안됨"), __FUNCTIONW__);
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Error !!") );
		}break;
	case AT_MAX_HP:
		{
			iValue = GetAbil(pkDef, Type);
			if(0 == iValue)
			{
				iValue = pkDef->kDef.sMaxHP;
			}
		}break;
	default:
		iValue = GetAbil(pkDef, Type);
		break;
	}
	
	return iValue;
}

__int64 PgClassDefMgr::GetAbil64(const SClassKey &rkKey, WORD const Type)const
{
	//BM::CAutoMutex kLock(m_kMutex);
	const CLASS_DEF_BUILT* pkDef = GetDef(rkKey);
	if(!pkDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0; 
	}

	__int64 iValue = 0;
	switch(Type)
	{
	case AT_EXPERIENCE:
		{
			iValue = pkDef->kDef.i64Experience;
		}break;
	default:
		{ 
//			INFO_LOG(BM::LOG_LV0, _T("[%s] GetAbil64함수를 사용해서 기입하시오"), __FUNCTIONW__);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			assert(NULL);
		}break;
	}
	return iValue;
}

const CLASS_DEF_BUILT* PgClassDefMgr::GetDef(const SClassKey &rkKey)const
{
	ContClassDef::const_iterator itor = m_kDef.find(rkKey);
	if(itor != m_kDef.end())
	{
		return itor->second;
	}

	INFO_LOG( BM::LOG_LV4, _T(" Cannot find ClassDef Class[") << rkKey.iClass << _T("], Level[") << rkKey.nLv << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

int PgClassDefMgr::GetAbil(const CLASS_DEF_BUILT* pkDef, WORD const wType) const
{
	if(NULL == pkDef)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" pkDef == NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDef is NULL"));
	}
	CLASSDEF_ABIL_CONT::const_iterator itor = pkDef->kAbil.find((int)wType);
	if (itor != pkDef->kAbil.end())
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

// Getting Abil list
// [PARAMETER]
//	iClass : class id
//	sLevel : level
//	pkBasic : [OUT] Basic information to get
HRESULT PgClassDefMgr::GetAbil(const SClassKey &rkKey, SPlayerBasicInfo &rkBasic)const
{
	CLASS_DEF_BUILT const * pkDef = GetDef(rkKey);
	if(pkDef == NULL)
	{
		//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s] GetAbil failed... Class[%d], Level[%hd]"), __FUNCTIONW__, rkKey.iClass, rkKey.nLv);
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" GetAbil failed... Class[") << rkKey.iClass << _T("], Level[") << rkKey.nLv << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	::memset(&rkBasic, 0, sizeof(rkBasic));

	rkBasic.iMaxHP = static_cast<int>(pkDef->kDef.sMaxHP);
	rkBasic.iHPRecoveryInterval = static_cast<int>(pkDef->kDef.sHPRecoveryInterval);
	rkBasic.sHPRecovery = static_cast<int>(pkDef->kDef.sHPRecovery);
	rkBasic.iMaxMP = static_cast<int>(pkDef->kDef.sMaxMP);
	rkBasic.iMPRecoveryInterval = static_cast<int>(pkDef->kDef.sMPRecoveryInterval);
	rkBasic.sMPRecovery = static_cast<int>(pkDef->kDef.sMPRecovery);
	rkBasic.sInitStr = pkDef->kDef.iStr;
	rkBasic.sInitInt = pkDef->kDef.iInt;
	rkBasic.sInitCon = pkDef->kDef.iCon;
	rkBasic.sInitDex = pkDef->kDef.iDex;
	rkBasic.sMoveSpeed = static_cast<int>(pkDef->kDef.sMoveSpeed);
	rkBasic.sPhysicsDefence = GetAbil(pkDef, AT_PHY_DEFENCE);
	rkBasic.sMagicDefence = GetAbil(pkDef, AT_MAGIC_DEFENCE);
	rkBasic.sAttackSpeed = GetAbil(pkDef, AT_ATTACK_SPEED);
	rkBasic.sBlockRate = GetAbil(pkDef, AT_BLOCK_SUCCESS_VALUE);
	rkBasic.sDodgeRate = GetAbil(pkDef, AT_DODGE_SUCCESS_VALUE);
	rkBasic.sCriticalRate = GetAbil(pkDef, AT_CRITICAL_SUCCESS_VALUE);
	rkBasic.sCriticalPower = GetAbil(pkDef, AT_CRITICAL_POWER);
	rkBasic.sHitRate = GetAbil(pkDef, AT_HIT_SUCCESS_VALUE);
	rkBasic.iAIType = GetAbil(pkDef, AT_AI_TYPE);
//	rkBasic.sInvenSize = GetAbil(pkDef, AT_INVEN_SIZE);
//	rkBasic.sEquipsSize = GetAbil(pkDef, AT_EQUIPS_SIZE);

	return S_OK;
}

void PgClassDefMgr::Clear()
{
	m_kDefLastLv.clear();

	ContClassDef::iterator itor = m_kDef.begin();
	while (itor != m_kDef.end())
	{
		if (itor->second != NULL) 
		{
			m_kClassDefPool.Delete(itor->second);
		}
		itor->second = NULL;

		itor++;
	}

	m_kDef.clear();
}

void PgClassDefMgr::swap(PgClassDefMgr &rRight)
{
	m_kDef.swap(rRight.m_kDef);
	m_kDefLastLv.swap(rRight.m_kDefLastLv);
//	memcpy(&rRight.m_kClassDefPool, &m_kClassDefPool, sizeof(m_kClassDefPool));
	m_kClassDefPool.swap(rRight.m_kClassDefPool);
}

bool PgClassDefMgr::IsLvUp(const SClassKey &kNowKey, __int64 const &i64Exp, SClassKey &kRetKey, SLvUpAddValue& kAdded)const
{//Next 레벨의 경험치가 되면. 레벨업이 되는거야.
	if (kNowKey.nLv >= PgClassDefMgr::s_sMaximumLevel)
	{
		return false;
	}
	SClassKey kNextKey = kNowKey;
	kNextKey.nLv += 1;

	__int64 const i64BaseExp = GetAbil64(kNowKey, AT_EXPERIENCE);//지금 레벨 기준.
	__int64 const i64NextExp = GetAbil64(kNextKey, AT_EXPERIENCE);//다음 레벨 기준.

	//if(i64BaseExp && i64BaseExp <= i64Exp	// i64BaseExp == 0(Lv1)일때 잘못 표기가 됨..
	if (i64BaseExp <= i64Exp
	&&	i64NextExp <= i64Exp//새로 들어온 경험치가. 지금레벨 요구조건 보다 높거나 같네.
	&&  i64NextExp > 0 )// 만랩 Levelup 금지
	{//다음레벨 보다 높다.
		kRetKey = kNextKey;
		kAdded.sSP += GetAbil(kNextKey, AT_SP);
		kAdded.sBonusStatus += GetAbil(kNextKey, AT_STATUS_BONUS);
		IsLvUp(kNextKey, i64Exp, kRetKey, kAdded);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgClassDefMgr::RevisionClassKey(SClassKey &rkKey, __int64 &i64Exp)const
{//변화 없으면 S_OK;
	ContClassLastLv::const_iterator last_lv_itor = m_kDefLastLv.find(rkKey.iClass);

	if(last_lv_itor != m_kDefLastLv.end())
	{
		i64Exp = __min(last_lv_itor->second->kDef.i64Experience, i64Exp);
		rkKey.nLv = __min(last_lv_itor->second->kDef.sLevel, rkKey.nLv);

		return S_OK;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find LastLevel information Class=") << rkKey.iClass);
	return E_FAIL;
}


HRESULT PgClassDefMgr::AccSkillPoint(const SClassKey &rkKey, int &iRet)const
{
	short nLv = rkKey.nLv;
	if(0 >= nLv)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	int iTotalSP = 0;
	while(nLv)
	{
		const SClassKey kKey(rkKey.iClass, nLv);
		iTotalSP += GetAbil(kKey, AT_SP);
		--nLv;
	}
	iRet = iTotalSP;
	return S_OK;
}

// 다음 레벨로 올라가기 위해 배워야 하는 경험치
__int64 PgClassDefMgr::GetExperience4Levelup(SClassKey& rkKey) const
{
	ContClassDef::const_iterator itor_lv = m_kDef.find(rkKey);
	if (m_kDef.end() == itor_lv)
	{
		return 0;
	}
	SClassKey kNext(rkKey.iClass, rkKey.nLv+1);
	ContClassDef::const_iterator itor_next = m_kDef.find(kNext);
	if (m_kDef.end() == itor_next)
	{
		return 0;	// 마지막 레벨일 때는 0
	}
	return (*itor_next).second->kDef.i64Experience - (*itor_lv).second->kDef.i64Experience;
}

__int64 PgClassDefMgr::GetMaxExperience(int const iClass) const
{
	ContClassLastLv::const_iterator last_lv_itor = m_kDefLastLv.find(iClass);

	if(last_lv_itor != m_kDefLastLv.end())
	{
		return (*last_lv_itor).second->kDef.i64Experience;
	}
	return 0i64;
}