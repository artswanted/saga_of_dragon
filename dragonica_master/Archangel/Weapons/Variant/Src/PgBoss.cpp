#include "StdAfx.h"
#include "PgBoss.h"
//#include "MonsterDefMgr.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"

PgBoss::PgBoss(void)
{
}

PgBoss::~PgBoss(void)
{
}

int PgBoss::GetAbil(WORD const Type) const
{
	int iValue = 0;
	switch(Type)
	{
	case AT_AI_DELAY:
		iValue = m_kInfo.iAiDelay;
		break;
	case AT_C_HP_GAGE:
		iValue = m_kInfo.iCurrentHPGage;
		break;
	case AT_CURRENT_ACTION:
		iValue = m_kInfo.iCurrentAction;
		break;
	case AT_MONSTER_APPEAR:
		iValue = m_kInfo.iAppear;
		break;
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{	
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Plz Use GetAbil64") );
		}break;
	case AT_OWNER_TYPE:
	case AT_CALLER_TYPE:
		{
			return UT_BOSSMONSTER;
		}break;
	default:
		{
			iValue = PgMonster::GetAbil(Type);
			if (iValue == 0 && !CAbilObject::HasAbil(Type))	// 진짜로 Value==0일때와, 아직 값을 안 읽어서 0 일때를 구별해 주어야 한다.
			{
				int const iMonNo = GetAbil(AT_CLASS);
				GET_DEF(CMonsterDefMgr, kMonsterDefMgr);
				const CMonsterDef *pDef = kMonsterDefMgr.GetDef(iMonNo);
				if(NULL != pDef)
				{
					iValue = pDef->GetAbil(Type);
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" PgBoss::GetAbil pDef Is Null") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pMonsterDefMgr is NULL"));
				}
			}
			break;
		}
	}
	return iValue;
}
	
bool PgBoss::SetAbil(WORD const Type, int const iValue, bool const bIsSend, bool const bBroadcast)
{
	bool bIntercept = true;
	switch(Type)
	{
	case AT_AI_DELAY:		{ m_kInfo.iAiDelay = iValue; }break;
	case AT_C_HP_GAGE:		{ m_kInfo.iCurrentHPGage = iValue; }break;
	case AT_CURRENT_ACTION:	{ m_kInfo.iCurrentAction = iValue; }break;
	case AT_MONSTER_APPEAR:	{ m_kInfo.iAppear = iValue; }break;
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" use SetAbil64)") );
		}break;
	default:
		{
			bIntercept = false;
			PgMonster::SetAbil(Type, iValue, bIsSend, bBroadcast);
		}break;
	}

	if(bIntercept)
	{
		DWORD dwSendFlag = 0;
	
		if(bIsSend){dwSendFlag |= E_SENDTYPE_SELF;}
		if(bBroadcast){dwSendFlag |= E_SENDTYPE_BROADCAST;}

		if(dwSendFlag)
		{
			SendAbil((EAbilType)Type, dwSendFlag);
		}

		OnSetAbil(Type, iValue);
	}
	return true;
}

//bool PgBoss::IsAttackable(EAbilType eType, CUnit *pkTarget,int& iSkillNo)
//{
//	if(pkTarget == NULL)
//	{
//		INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] IsAttackable:Target is NULL "), __FUNCTIONW__, __LINE__);
//		return false;
//	}
//	
//	if (pkTarget->GetState() == US_DEAD || pkTarget->GetState() == US_DEADREADY )
//	{
//		INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] IsAttackable:Target is Dead "), __FUNCTIONW__, __LINE__);
//		return false;
//	}
//
//	// 보스는 일단 무조건 맞춘다!
//	///*
//	int iBossDist = GetAbil(eType);
//	if ( iBossDist < 1000 ) 
//	{
//		POINT3 const &ptM = GetPos();
//		POINT3 const &ptC = pkTarget->GetPos();
//
//		float fDist = sqrt(pow(ptM.x - ptC.x, 2) + pow(ptM.z - ptC.z, 2) + pow(ptM.y - ptC.y, 2));
//
//		if(fDist > iBossDist)
//		{
//			return false;
//		}
//	}//*/

//	return true;
//}


bool PgBoss::SetSPData(int iType , int iValue)
{
	if ( iType < 0 || iType >= MAX_SP_ATTACK_NUM )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	m_iSPData[iType] = iValue ;
	return true ;
}

int PgBoss::GetSPData(int iType)
{
	if ( iType < 0 || iType >= MAX_SP_ATTACK_NUM )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0 ;
	}

	return m_iSPData[iType] ;
}

void PgBoss::Init()
{
	memset(&m_kInfo, 0, sizeof(m_kInfo));
	
	PgMonster::Init();
}