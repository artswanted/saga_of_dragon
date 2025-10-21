#include "stdafx.h"
#include "Global.h"
#include "MonsterDef.h"
#include "variant/TableDataManager.h"

int CMonsterDef::GetAbil(WORD const wAbilType) const
{
	switch(wAbilType)
	{
	case AT_NAMENO:				{ return m_iNo; }break;
	case AT_EXPERIENCE:
	case AT_DIE_EXP:
	case AT_MONEY:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Use GetAbil64(), AbilType[") << wAbilType << _T("]") );
		}break;
	default:
		return CAbilObject::GetAbil(wAbilType);
		break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

__int64 CMonsterDef::GetAbil64(WORD const wAbilType) const
{
	__int64 iValue = 0;
	switch(wAbilType)
	{
	case AT_EXPERIENCE:
	case AT_CLASSLIMIT:	{ iValue = CAbilObject::GetAbil64(wAbilType); }break;
	default:
		{
			//INFO_LOG(BM::LOG_LV0, _T("[%s] 해당타입의 벨류가 int64가 아닐수 있음 AbilType[%hd]"), __FUNCTIONW__, wAbilType);
			//assert(NULL);
			iValue = CAbilObject::GetAbil64(wAbilType);
		}break;
	}
	return iValue;
}

CMonsterTunningDefMgr::CMonsterTunningDefMgr(TBL_DEF_MONSTERTUNNING const * pkDef)
	: m_pkDef(pkDef)
	, m_pkContAbil(NULL)
{
	g_kTblDataMgr.GetContDef(m_pkContAbil);
}

CMonsterTunningDefMgr::~CMonsterTunningDefMgr()
{
}

bool CMonsterTunningDefMgr::FindAbil(int const iAbil, int & iValue) const
{
	if(!m_pkDef || !m_pkContAbil)
	{
		return false;
	}

	for(int i=0; i<MAX_MONSTER_ABIL_ARRAY; ++i)
	{
		CONT_DEFMONSTERABIL::const_iterator c_abil = (*m_pkContAbil).find((*m_pkDef).iAbil[i]);
		if((*m_pkContAbil).end()==c_abil)
		{
			continue;
		}

		for(int j=0; j<MAX_MONSTER_ABIL_ARRAY; ++j)
		{
			CONT_DEFMONSTERABIL::mapped_type const &kAbil = (*c_abil).second;
			if(kAbil.aType[j] == iAbil)
			{
				iValue = kAbil.aValue[j];
				return true;
			}
		}
	}

	return false;
}