#include "stdafx.h"
#include "PgSkillOnFindTargetManager.h"
#include "PgSkillOnFindTarget.h"
#include "variant/Global.h"

extern int CalcRangeCheckMutiflier(int const iRange);

PgSkillOnFindTargetManager::PgSkillOnFindTargetManager()
{
}

PgSkillOnFindTargetManager::~PgSkillOnFindTargetManager()
{
	Release();
}

void PgSkillOnFindTargetManager::Init()
{
}

void PgSkillOnFindTargetManager::Release()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SKILL_FIND_TARGET::iterator itor = m_kContFindTarget.begin();
	while(itor != m_kContFindTarget.end())
	{
		SAFE_DELETE((*itor).second);
		itor = m_kContFindTarget.erase(itor);
	}

	m_kContFindTarget.clear();
}

bool PgSkillOnFindTargetManager::Build()
{
	int const kTargetType[] = {0, 103100501, 101000801, 109001501, 2000302001, 300101101
, -1};//기본, 포인트 버스트, 스톰블레이드, 체인 라이트닝, 힐링 웨이브
	//기본 타겟팅 추가
	BM::CAutoMutex kLock(m_kMutex, true);

	if(m_kContFindTarget.empty())
	{
		PgISkillOnFindTarget* pkFindTarget = NULL;

		int iIdx = 0;
		while(-1 != kTargetType[iIdx])
		{
			pkFindTarget = CreateOnTargetFunc(kTargetType[iIdx]);
			if(pkFindTarget)
			{
				m_kContFindTarget.insert(std::make_pair(kTargetType[iIdx], pkFindTarget));
			}
			++iIdx;
		}
		return !m_kContFindTarget.empty();
	}

	return false;
}

PgISkillOnFindTarget* PgSkillOnFindTargetManager::CreateOnTargetFunc(int const iFuncType) const
{
	PgISkillOnFindTarget *pkTempFunc = NULL;
	switch(iFuncType)
	{
	case 0://기본
		{
			pkTempFunc = new_tr PgSkillOnFindTarget;
		}break;
	case 103100501://포인트 버스터류
		{
			pkTempFunc = new_tr PgSkillOnFindTargetPointBurst;
		}break;
	case 101000801://스톰 블레이드
		{
			pkTempFunc = new_tr PgSkillOnFindTargetStormBlade;
		}break;
	case 109001501://체인라이트닝 류
	case 2000302001: //힐링 웨이브
		{
			pkTempFunc = new_tr PgSkillOnFindTargetChainLight;
		}break;
	case 300101101:
		{
			pkTempFunc = new_tr PgSkillOnFindTargetSummoned;
		}break;
	default :
		{
			pkTempFunc = new_tr PgSkillOnFindTarget;
		}break;
	}

	if( !pkTempFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return pkTempFunc;
}

bool PgSkillOnFindTargetManager::OnFindTarget(BM::Stream *pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec)
{
	if(1>byTargetNum || NULL==pkPacket || NULL==pkDef || NULL==pkUnit || NULL==pkGround)
	{
		return false;
	}

	if(SAT_CLIENT_CTRL_PROJECTILE&(ESkillAttribute) pkDef->GetAbil(AT_SKILL_ATT))//프로젝틸이면
	{
		if(0<byTargetNum)
		{
			INFO_LOG(BM::LOG_LV5, __FL__<<"Projectile Type Skill has Target! It must have no target. "<<"SkillID : "<<pkDef->No()<<" TargetNo : "<<byTargetNum);
		}
		return 0==byTargetNum;//타겟된 놈이 있으면 문제.
	}

	// AT_ATTACK_UNIT_POS, AT_ATTACK_UNIT_POS
	// 위의 어빌로, 띄워진 Target의 Z 좌표값 이상을 보정하자.
	int iSkillRange = PgAdjustSkillFunc::GetAttackRange(pkUnit, pkDef);
	iSkillRange = CalcRangeCheckMutiflier(iSkillRange);

	BM::CAutoMutex kLock(m_kMutex);

	//CONT_SKILL_FIND_TARGET::const_iterator itor = m_kContFindTarget.find(pkDef->GetParentSkill()==103100501 ? 103100501 : pkDef->GetAbil(AT_SKILL_RANGE_TYPE2));//일단 찾자
	CONT_SKILL_FIND_TARGET::const_iterator itor = m_kContFindTarget.find(pkDef->GetAbil(AT_SKILL_RANGE_TYPE2));//일단 찾자
	if( itor != m_kContFindTarget.end() )
	{
		return (*itor).second->OnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, iSkillRange, pkGround, rkAction, rkArray, rkActionResultVec);
	}
	else//못찾으면 기본으로
	{
		itor = m_kContFindTarget.find(0);
		if( itor != m_kContFindTarget.end() )
		{
			return (*itor).second->OnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, iSkillRange, pkGround, rkAction, rkArray, rkActionResultVec);
		}
		else
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<"OnFindTargetCont is empty!");

		}
	}

	return false;
}