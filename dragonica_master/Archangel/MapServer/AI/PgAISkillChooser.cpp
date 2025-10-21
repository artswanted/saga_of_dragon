#include "stdafx.h"
#include "Variant\constant.h"
#include "Variant\PgControlDefMgr.h"
#include "PgAISkillChooser.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "SkillEffectAbilSystem\PgSkillAbilHandleManager.h"

PgAISkillChooser::PgAISkillChooser()
{
}

PgAISkillChooser::~PgAISkillChooser()
{
	Clear();
}

bool PgAISkillChooser::Init()
{
	BM::CAutoMutex kLock(m_kMutex);
	Clear();
	return true;
}

void PgAISkillChooser::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
{
	ContAISkillWeightInfo::iterator itor = m_kUnitSkillInfo.begin();
	while (itor != m_kUnitSkillInfo.end())
	{
		Clear(itor->second);

		++itor;
	}
	m_kUnitSkillInfo.clear();
}
{
	ContAISkillWeightInfo::iterator itor2 = m_kSkillGroupInfo.begin();
	while (itor2 != m_kSkillGroupInfo.end())
	{
		Clear(itor2->second);
		++itor2;
	}
	m_kSkillGroupInfo.clear();
}
}

void PgAISkillChooser::Clear(PgAISkillChooser::SkillWeightInfo* pkSkillWeightInfo)
{
	List_SkillWeight::iterator itor = pkSkillWeightInfo->kList.begin();
	while (itor != pkSkillWeightInfo->kList.end())
	{
		m_kSkillWeightPool.Delete(*itor);

		++itor;
	}
	pkSkillWeightInfo->kList.clear();

	m_kSkillWeightInfoPool.Delete(pkSkillWeightInfo);
}

int PgAISkillChooser::GetAvailableSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SActArg* pkActArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	//if (!pkCaster->GetSkill()->Reserve(0))
	//{
	//	return 0;
	//}
	int const iCurSkillNo = pkCaster->GetSkill()->GetSkillNo();
	EAIChooseSkillType eChooseType = EAI_CHOOSE_SKILL_RANDOM;
	SkillWeightInfo* pkSkillWeightInfo = NULL;
	if (iCurSkillNo == 0)
	{
		int iPopSkill = pkCaster->GetSkill()->PopSkillStack();
		if (iPopSkill > 0)
		{
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"Skill Stack Pop["<<iPopSkill<<L"]");
#endif
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iPopSkill);
			if (pkSkillDef == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot get SkillDef SkillNo["<<iPopSkill<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
				return 0;
			}
			if (pkSkillDef->GetType() != EST_GROUP)
			{
				// Stack에서 Pop 한 것이 바로 사용할 수 있는 스킬이다.
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				if (pkGround != NULL)
				{
					if (pkGround->AI_GetSkillTargetList(pkCaster, iPopSkill, kTargetArray, false, pkActArg))
					{
						if(g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iPopSkill, pkActArg, &kTargetArray))
						{
							pkCaster->GetSkill()->Reserve(iPopSkill);
							return iPopSkill;
						}
					}
				}	
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
				return 0;
			}
			eChooseType = (EAIChooseSkillType) pkSkillDef->GetAbil(AT_SKILL_CHOOSE_TYPE);
			pkSkillWeightInfo = (SkillWeightInfo*) GetSkillGroup(iPopSkill);
		}
	}
	if (eChooseType == EAI_CHOOSE_SKILL_RANDOM)
	{
		eChooseType = (EAIChooseSkillType) pkCaster->GetAbil(AT_SKILL_CHOOSE_TYPE);
		pkSkillWeightInfo = (SkillWeightInfo*) Get(pkCaster);
	}

	if (pkSkillWeightInfo == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot Get SkillWeight information ClassID["<<pkCaster->GetAbil(AT_CLASS)<<L"], Unit["<<pkCaster->GetID()<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int iResultSkill = 0;
	int iLoopCount = 0;
	while (iLoopCount++ < 10)
	{
		switch(eChooseType)
		{
		case EAI_CHOOSE_SKILL_RANDOM:
			{
				iResultSkill = GetAvailableSkill_Random(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		case EAI_CHOOSE_SKILL_SEQUENSE:
			{
				iResultSkill = GetAvailableSkill_Sequence(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		case EAI_CHOOSE_SKILL_WEIGHT:
		default:
			{
				iResultSkill = GetAvailableSkill_Weight(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		}
		if (iResultSkill > 0)
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);
			const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iResultSkill);
			if (pkSkillDef == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot get SkillDef SkillNo["<<iResultSkill<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
				return 0;
			}
			if(CheckSkillHPLimit(pkCaster, pkSkillDef))	//HP체크
			{
				if (pkSkillDef->GetAbil(AT_TYPE) == EST_GROUP)
				{
					// GroupType 이므로 한바퀴 더 돌아 Group내에서 스킬을 찾아야 한다.
					pkSkillWeightInfo = (SkillWeightInfo*) GetSkillGroup(iResultSkill);
					eChooseType = (EAIChooseSkillType) pkSkillDef->GetAbil(AT_SKILL_CHOOSE_TYPE);
				}
				else
				{
					break;
				}
			}
			else
			{
				iResultSkill = 0;
			}
		}
		else
		{
			break;
		}
	}
	if (iResultSkill > 0 && !pkCaster->GetSkill()->Reserve(iResultSkill))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int const iDieSkill = pkCaster->GetAbil(AT_SKILL_ON_DIE);
	if (ON_DIE_SKILL_HP >= pkCaster->GetAbil(AT_HP) && iDieSkill)
	{
		return iDieSkill;
	}
	return iResultSkill;
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::Get(CUnit* pkUnit)
{
	// 나중에 문제 될수 있는 부분
	// ClassID 만으로 검색하기 때문에 ClassID가 같고 Level 다른 것에 대해서는 다른 정보를 얻을 수 없다.
	int iClass = pkUnit->GetAbil(AT_CLASS);
	int iLevel = 0;
	if(pkUnit->IsUnitType(UT_SUMMONED))
	{
		iLevel = pkUnit->GetAbil(AT_LEVEL);
	}

	int const iMetaMorphCount = pkUnit->GetAbil(AT_MON_TRANSFORM_COUNT);
	if( 0 < iMetaMorphCount )	//변신 중이면
	{
		iClass = pkUnit->GetAbil(AT_MON_TRANSFORM_CLASS_01+(iMetaMorphCount-1));	//변신 한 클래스 번호로 찾아보자
	}
	ContAISkillWeightInfo::key_type const kKey(iClass,iLevel);
	ContAISkillWeightInfo::const_iterator itor = m_kUnitSkillInfo.find(kKey);
	if (itor != m_kUnitSkillInfo.end())
	{
		return itor->second;
	}

	return ReadUnitSkillWeight(pkUnit, kKey);
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::ReadUnitSkillWeight(CUnit* pkUnit, ContAISkillWeightInfo::key_type const& kInfoKey)
{
	int const iClassID = kInfoKey.kPriKey;
	SkillWeightInfo* pkInfo = NULL;
	EUnitType const eUnitType = pkUnit->UnitType();
	switch(eUnitType)
	{
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
			GET_DEF(CMonsterDefMgr, kMonsterDefMgr);
			const CMonsterDef* pkMonsterDef = kMonsterDefMgr.GetDef(iClassID);
			if( !pkMonsterDef ) 
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get MonsterDef MonsterClass["<<iClassID<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
				return NULL;
			}
			return ReadNewInfo(iClassID, m_kUnitSkillInfo, pkMonsterDef);
		}break;
	case UT_ENTITY:
		{
			return ReadNewInfo(pkUnit->GetAbil(AT_CLASS), m_kUnitSkillInfo, pkUnit);
		}break;
	case UT_SUMMONED:
		{
			ContAISkillWeightInfo::key_type const kKey(pkUnit->GetAbil(AT_CLASS),pkUnit->GetAbil(AT_LEVEL));
			return ReadNewInfo(kKey, m_kUnitSkillInfo, pkUnit);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Not implemented unit type UnitType["<<eUnitType<<L"], Unit["<<pkUnit->GetID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return pkInfo;
}

/*
const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::ReadNewMonsterInfo(CUnit* pkUnit)
{
// 	if (!pkUnit->IsUnitType(UT_MONSTER))
// 	{
// 		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Unit is not MonsterType UnitType[%d]"), __FUNCTIONW__, pkUnit->UnitType());
// 		return NULL;
// 	}
	int iClass = pkUnit->GetAbil(AT_CLASS);
	const CMonsterDef* pkDef = g_MonsterDef.GetDef(iClass);
	VERIFY_INFO_LOG_RETURN(NULL, VPARAM(pkDef != NULL, BM::LOG_LV4, _T("[%s] Cannot Get MonsterDef MonsterClass[%d]"), __FUNCTIONW__, iClass));

	SkillWeightInfo* pkNew = m_kSkillWeightInfoPool.New();
	auto kInsert = m_kInfo.insert(std::make_pair(iClass, pkNew));
	if (!kInsert.second)
	{
		m_kSkillWeightInfoPool.Delete(pkNew);
		VERIFY_INFO_LOG_RETURN(NULL, VPARAM(false, BM::LOG_LV4, _T("[%s] Cannot insert ContAISkillWeightInfo ClassID[%d]"), __FUNCTIONW__, iClass));
	}

	int iSkill = 0;
	int iRate = 0;
	int iWeight = 0;
	for (int i=0; i<MAX_AI_SKILL_NUM; i++)
	{
		iSkill = pkDef->GetAbil(AT_MON_SKILL_01 + i);
		if (iSkill == 0)
		{
			break;
		}
		iRate = pkDef->GetAbil(AT_MON_SKILL_RATE_01 + i);
		const CSkillDef* pkSkillDef = g_SkillDefMgr.GetDef(iSkill);
		VERIFY_INFO_LOG_RUN(continue;, VPARAM(pkSkillDef != NULL, BM::LOG_LV4, _T("[%s] Cannot Get SkillDef[%d]"), __FUNCTIONW__, iSkill));

		int iAttackRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		int iMinDistance = GetMinDistanceFromTarget(pkUnit);//__max(pkUnit->GetAbil(AT_MON_MIN_RANGE), AI_MONSTER_MIN_DISTANCE_FROM_TARGET);

		if (iAttackRange < iMinDistance)
		{
			INFO_LOG(BM::LOG_LV5, _T("[%d] skill can't attack because of %d monster has %d min range and skill has %d range"), 
				iSkill, iClass, iMinDistance, iAttackRange);
		}

		SSkillWeight* pkSkill = m_kSkillWeightPool.New();
		pkSkill->iSkill = iSkill;
		pkSkill->iWeight = iRate;

		kInsert.first->second->kList.push_back(pkSkill);
		iWeight += iRate;
	}
	kInsert.first->second->iWeightTotal = iWeight;
	sort(kInsert.first->second->kList.begin(), kInsert.first->second->kList.end(), PgAISkillChooser::List_SkillWeight_Sort);
	return kInsert.first->second;
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::ReadNewClassInfo(CUnit* pkUnit)
{
// 	if (!pkUnit->IsUnitType(UT_ENTITY))
// 	{
// 		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Unit is not UT_ENTITY Type UnitType[%d]"), __FUNCTIONW__, pkUnit->UnitType());
// 		return NULL;
// 	}
	int iClass = pkUnit->GetAbil(AT_CLASS);
	SkillWeightInfo* pkNew = m_kSkillWeightInfoPool.New();
	auto kInsert = m_kInfo.insert(std::make_pair(iClass, pkNew));
	if (!kInsert.second)
	{
		m_kSkillWeightInfoPool.Delete(pkNew);
		VERIFY_INFO_LOG_RETURN(NULL, VPARAM(false, BM::LOG_LV4, _T("[%s] Cannot insert ContAISkillWeightInfo ClassID[%d]"), __FUNCTIONW__, iClass));
	}

	int iSkill = 0;
	int iRate = 0;
	int iWeight = 0;
	for (int i=0; i<MAX_AI_SKILL_NUM; i++)
	{
		iSkill = pkUnit->GetAbil(AT_MON_SKILL_01 + i);
		if (iSkill == 0)
		{
			break;
		}
		iRate = pkUnit->GetAbil(AT_MON_SKILL_RATE_01 + i);

		SSkillWeight* pkSkill = m_kSkillWeightPool.New();
		pkSkill->iSkill = iSkill;
		pkSkill->iWeight = iRate;

		kInsert.first->second->kList.push_back(pkSkill);
		iWeight += iRate;
	}
	kInsert.first->second->iWeightTotal = iWeight;
	sort(kInsert.first->second->kList.begin(), kInsert.first->second->kList.end(), PgAISkillChooser::List_SkillWeight_Sort);
	return kInsert.first->second;
}
*/

int PgAISkillChooser::GetAvailableSkill_Weight(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if( NULL == pkGround )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"NULL Ground");
		return 0;
	}
	
	// Weight에 따라 Sorting 되었으니..앞에것 부터 조사하다가,
	// 조건에 맞는 첫번째 넘을 선택하면 된다.
	List_SkillWeight::const_iterator itor = pkSkillWeightInfo->kList.begin();
	while (itor != pkSkillWeightInfo->kList.end())
	{
		int const iSkillNo = (*itor)->iSkill;

		if(Check(pkCaster, iSkillNo, pkGround))
		{
			if (pkGround->AI_GetSkillTargetList(pkCaster, iSkillNo, kTargetArray, false, pkActArg))
			{
				if(g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iSkillNo, pkActArg, &kTargetArray))
				{
					return iSkillNo;
				}
			}
		}
		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgAISkillChooser::GetAvailableSkill_Random(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}
	
	// Random방식은 GetReservableSkill_Random에서 선택한 스킬만을 사용하려고 시도한다.
	// 즉, 이미 사용할 스킬이 정해져 있다는 의미
	int iSkillNo = pkCaster->GetSkill()->GetSkillNo();
	//VERIFY_INFO_LOG_RUN(return 0;, VPARAM(iSkillNo > 0, BM::LOG_LV5, _T("[%s] AI SkillNo cannot be <0, SkillNo[%d], Caster[ClassID:%d]"), __FUNCTIONW__, iSkillNo,
	//	pkCaster->GetAbil(AT_CLASS)));
	if (iSkillNo <= 0)
	{
		// Skill이 Reserve 안되고 바로 이 함수로 들어 올 수 도 있다.
		if ((iSkillNo = PgAISkillChooser::GetReservableSkill_Random(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg)) <= 0)
		{
			//INFO_LOG(BM::LOG_LV0, _T("[%s] Cannot Select Skill...Caster Class[%d]"), __FUNCTIONW__, pkCaster->GetAbil(AT_CLASS));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
	}
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if (pkGround != NULL)
	{
		if (pkGround->AI_GetSkillTargetList(pkCaster, iSkillNo, kTargetArray, false, pkActArg))
		{
			if(g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iSkillNo, pkActArg, &kTargetArray))
			{
				return iSkillNo;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;

}

bool PgAISkillChooser::List_SkillWeight_Sort(SSkillWeight* pkFirst, SSkillWeight* pkSecond)
{
	return (*pkFirst) < (*pkSecond);
}

int PgAISkillChooser::GetReservableSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SActArg* pkActArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	int const iPopSkill = pkCaster->GetSkill()->PopSkillStack();
	EAIChooseSkillType eChooseType = EAI_CHOOSE_SKILL_RANDOM;
	SkillWeightInfo* pkSkillWeightInfo = NULL;
	
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	if (iPopSkill > 0)
	{
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Skill Stack Pop["<<iPopSkill<<L"]");
#endif
		const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iPopSkill);
		if (pkSkillDef == NULL)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot get SkillDef SkillNo["<<iPopSkill<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
		if (pkSkillDef->GetType() != EST_GROUP)
		{
			// Stack에서 Pop 한 것이 바로 사용할 수 있는 스킬이다.
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			if (pkGround != NULL)
			{
				if (pkGround->AI_GetSkillTargetList(pkCaster, iPopSkill, kTargetArray, false, pkActArg))
				{
					if(g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iPopSkill, pkActArg, &kTargetArray))
					{
						return iPopSkill;
					}
				}
			}	
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
		eChooseType = (EAIChooseSkillType) pkSkillDef->GetAbil(AT_SKILL_CHOOSE_TYPE);
		pkSkillWeightInfo = (SkillWeightInfo*) GetSkillGroup(iPopSkill);
	}
	else
	{
		eChooseType = (EAIChooseSkillType) pkCaster->GetAbil(AT_SKILL_CHOOSE_TYPE);
		pkSkillWeightInfo = (SkillWeightInfo*) Get(pkCaster);
	}

	if (pkSkillWeightInfo == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot Get SkillWeight information ClassID["<<pkCaster->GetAbil(AT_CLASS)<<L"], Unit["<<pkCaster->GetID()<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int iResultSkill = 0;
	int iLoopCount = 0;
	
	while (iLoopCount++ < 10)
	{
		switch(eChooseType)
		{
		case EAI_CHOOSE_SKILL_RANDOM:
			{
				iResultSkill = GetReservableSkill_Random(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		case EAI_CHOOSE_SKILL_SEQUENSE:
			{
				iResultSkill = GetReservableSkill_Sequence(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		case EAI_CHOOSE_SKILL_WEIGHT:
		default:
			{
				iResultSkill = GetReservableSkill_Weight(pkCaster, kTargetArray, bAttackSkill, pkSkillWeightInfo, pkActArg);
			}break;
		}
		if (iResultSkill > 0)
		{
			const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iResultSkill);
			if (pkSkillDef == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot get SkillDef SkillNo["<<iResultSkill<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
				return 0;
			}
			if(CheckSkillHPLimit(pkCaster, pkSkillDef))
			{
				if (pkSkillDef->GetAbil(AT_TYPE) == EST_GROUP)
				{
					// GroupType 이므로 한바퀴 더 돌아 Group내에서 스킬을 찾아야 한다.
					pkSkillWeightInfo = (SkillWeightInfo*) GetSkillGroup(iResultSkill);
					eChooseType = (EAIChooseSkillType) pkSkillDef->GetAbil(AT_SKILL_CHOOSE_TYPE);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	return iResultSkill;
}

int PgAISkillChooser::GetReservableSkill_Weight(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if( NULL == pkGround )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"NULL Ground");
		return 0;
	}

	// Weight에 따라 Sorting 되었으니..앞에것 부터 조사하다가,
	// 조건에 맞는 첫번째 넘을 선택하면 된다.
	List_SkillWeight::const_iterator itor = pkSkillWeightInfo->kList.begin();
	while (itor != pkSkillWeightInfo->kList.end())
	{
		if(false==Check(pkCaster, (*itor)->iSkill, pkGround))
		{
			++itor;
			continue;
		}

		if (1 < pkSkillWeightInfo->kList.size())	//갖고있는 스킬 갯수가 1개 이상일 때만
		{
			PgMonster const* pkMonster = dynamic_cast<PgMonster*>(pkCaster);
			if (pkMonster && pkMonster->IsFailedSkill((*itor)->iSkill) )	//실패한 적이 있으면
			{
				++itor;
				continue;
			}
		}
		if (g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, (*itor)->iSkill, pkActArg, &kTargetArray))
		{
			return (*itor)->iSkill;
		}			

		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgAISkillChooser::GetReservableSkill_Random(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if( NULL == pkGround )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"NULL Ground");
		return 0;
	}

	// RANDOM : 가중치 만큼 Random하게 스킬을 선택하고
	//	선택된 스킬을 Reserve시킬 수 있는지 검사하여
	//	Reserve스킬이 나올때 까지 랜덤하게 돌린다.
	int const iTotal = pkSkillWeightInfo->iWeightTotal;
	if (0>=iTotal)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	int iCount = 0;
	while (iCount++ < MAX_AI_SKILL_NUM)
	{
		// Random하게 한개의 Skill을 선택하고~
		int const iChoose = BM::Rand_Index(iTotal);
		int iCurr = 0;
		List_SkillWeight::const_iterator itor = pkSkillWeightInfo->kList.begin();
		while (itor != pkSkillWeightInfo->kList.end())
		{
			iCurr += (*itor)->iWeight;
			if (iCurr > iChoose)
			{
				break;
			}
			++itor;
		}
		// 선택한 Skill의 Reserve 가능한지 검사하자.
		if (itor != pkSkillWeightInfo->kList.end())
		{
			int const iSkill = (*itor)->iSkill;
			if(false==Check(pkCaster, iSkill, pkGround))
			{
				continue;
			}

			if (1 < pkSkillWeightInfo->kList.size())	//한개 이상일 때만
			{
				PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkCaster);
				if (pkMonster && pkMonster->IsFailedSkill(iSkill) )	//한번 실패한 놈이면
				{
					continue;
				}
			}
			
			if (g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iSkill, pkActArg, &kTargetArray))
			{
#ifdef AI_DEBUG
				INFO_LOG(BM::LOG_LV9, __FL__<<L"Skill Reserved ["<<iSkill<<L"]");
#endif
				return iSkill;
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::GetSkillGroup(int const iSkillGroupNo)
{
	ContAISkillWeightInfo::key_type const kKey(iSkillGroupNo,0);
	ContAISkillWeightInfo::const_iterator itor = m_kSkillGroupInfo.find(kKey);
	if (itor != m_kSkillGroupInfo.end())
	{
		return itor->second;
	}
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iSkillGroupNo);
	return ReadNewInfo(iSkillGroupNo, m_kSkillGroupInfo, pkSkillDef);
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::ReadNewInfo(const int iKey, ContAISkillWeightInfo& rkContainer,
																	   const CAbilObject* pkAbilObj)
{
	ContAISkillWeightInfo::key_type const kKey(iKey,0);
	return ReadNewInfo(kKey,rkContainer,pkAbilObj);
}

const PgAISkillChooser::SkillWeightInfo* PgAISkillChooser::ReadNewInfo(const ContAISkillWeightInfo::key_type& kKey, ContAISkillWeightInfo& rkContainer,
																	   const CAbilObject* pkAbilObj)
{
	int const iClassID = kKey.kPriKey;
	if( !pkAbilObj )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo CAbilObject is NULL, iClassID["<<iClassID<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	SkillWeightInfo* pkNew = m_kSkillWeightInfoPool.New();
	auto kInsert = rkContainer.insert(std::make_pair(kKey, pkNew));
	if (!kInsert.second)
	{
		m_kSkillWeightInfoPool.Delete(pkNew);
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot insert ContAISkillWeightInfo iClassID["<<iClassID<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	int iSkill = 0;
	int iRate = 0;
	int iWeight = 0;
	for (int i=0; i<MAX_AI_SKILL_NUM; ++i)
	{
		iSkill = pkAbilObj->GetAbil(AT_MON_SKILL_01 + i);
		if (iSkill == 0)
		{
			continue;
		}
		iRate = pkAbilObj->GetAbil(AT_MON_SKILL_RATE_01 + i);
		/*
		const CSkillDef* pkSkillDef = g_SkillDefMgr.GetDef(iSkill);
		VERIFY_INFO_LOG_RUN(continue;, VPARAM(pkSkillDef != NULL, BM::LOG_LV4, _T("[%s] Cannot Get SkillDef[%d]"), __FUNCTIONW__, iSkill));

		int iAttackRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		int iMinDistance = GetMinDistanceFromTarget(pkUnit);//__max(pkUnit->GetAbil(AT_MON_MIN_RANGE), AI_MONSTER_MIN_DISTANCE_FROM_TARGET);

		if (iAttackRange < iMinDistance)
		{
			INFO_LOG(BM::LOG_LV5, _T("[%d] skill can't attack because of %d monster has %d min range and skill has %d range"), 
				iSkill, iClass, iMinDistance, iAttackRange);
		}
		*/

		SSkillWeight* pkSkill = m_kSkillWeightPool.New();
		pkSkill->iSkill = iSkill;
		pkSkill->iWeight = iRate;

		kInsert.first->second->kList.push_back(pkSkill);
		iWeight += iRate;
	}
	kInsert.first->second->iWeightTotal = iWeight;
	sort(kInsert.first->second->kList.begin(), kInsert.first->second->kList.end(), PgAISkillChooser::List_SkillWeight_Sort);
	if( kInsert.first->second->kList.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"SkillWeightInfo size is ZERO Class["<<iClassID<<L"]");
	}
	//INFO_LOG(BM::LOG_LV5, _T("[%s] SkillWeightInfo size is ZERO Class[%d]"),__FUNCTIONW__, kKey);
	return kInsert.first->second;
}


int PgAISkillChooser::GetAvailableSkill_Sequence(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}
	
	// Stack에 넣는 것이므로, 거꾸로 넣어주어야 Pop 했을 때 순서대로 SkillNo가 나온다.
	List_SkillWeight::const_reverse_iterator rev_itor = pkSkillWeightInfo->kList.rbegin();
	int iSkillNo = 0;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	while (rev_itor != pkSkillWeightInfo->kList.rend())
	{
		iSkillNo = (*rev_itor)->iSkill;
		/*if(false==CheckLockSkill(pkCaster, iSkillNo)) //시퀀스스킬에 락을 걸 필요가 있을까?
		{
			++rev_itor;
			continue;
		}*/

		const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);	
		if(pkSkillDef && CheckSkillHPLimit(pkCaster, pkSkillDef ) )
		{
			pkCaster->GetSkill()->PushSkillStack(iSkillNo);
		}

		++rev_itor;
	}
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if (pkGround != NULL)
	{
		iSkillNo = pkCaster->GetSkill()->PopSkillStack();
		if (pkGround->AI_GetSkillTargetList(pkCaster, iSkillNo, kTargetArray, false, pkActArg))
		{
			return g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iSkillNo, pkActArg, &kTargetArray) ? iSkillNo : 0;
		}
	}	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgAISkillChooser::GetReservableSkill_Sequence(CUnit* pkCaster, UNIT_PTR_ARRAY& kTargetArray, bool bAttackSkill, SkillWeightInfo const* pkSkillWeightInfo,
											   SActArg* pkActArg)
{
	if( NULL == pkSkillWeightInfo )
	{
		ASSERT_LOG(false, BM::LOG_LV4, __FL__<<L"SkillWeightInfo is NULL Caster[ClassID:"<<pkCaster->GetAbil(AT_CLASS)<<L"]");
		return 0;
	}
	
	// Stack에 넣는 것이므로, 거꾸로 넣어주어야 Pop 했을 때 순서대로 SkillNo가 나온다.
	List_SkillWeight::const_reverse_iterator rev_itor = pkSkillWeightInfo->kList.rbegin();
	int iSkillNo = 0;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	while (rev_itor != pkSkillWeightInfo->kList.rend())
	{
		iSkillNo = (*rev_itor)->iSkill;
		/*if(false==CheckLockSkill(pkCaster, iSkillNo)) //시퀀스 스킬에 락을 걸 필요가 있을까?
		{
			++rev_itor;
			continue;
		}*/
		
		const CSkillDef* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if(pkSkillDef && CheckSkillHPLimit(pkCaster, pkSkillDef ) )
		{
			pkCaster->GetSkill()->PushSkillStack(iSkillNo);
		}

		++rev_itor;
	}

	return g_kSkillAbilHandleMgr.SkillCanReserve(pkCaster, iSkillNo, pkActArg, &kTargetArray) ? iSkillNo : 0;
}

bool PgAISkillChooser::Check(CUnit const* pkUnit, int const iSkillNo, PgGround * pkGround)const
{
	if(false==CheckLockSkill(pkUnit,iSkillNo))
	{
		return false;
	}
	if(false==CheckUseProjectile(pkUnit,iSkillNo))
	{
		return false;
	}
	if(false==CheckIsAllySkill(pkUnit,pkGround))
	{
		return false;
	}
	if(false==CheckIsInClass(pkUnit, iSkillNo, pkGround))
	{
		return false;
	}
	return true;
}

bool PgAISkillChooser::CheckLockSkill(CUnit const* pkUnit, int const iSkillNo)const
{
	if(!pkUnit)
	{
		return false;
	}

	if(pkUnit->GetAbil(AT_USE_LOCK_SKILL))
	{
		for(int i=0;i<MAX_AI_SKILL_NUM; ++i)
		{
			if(pkUnit->GetAbil(AT_LOCK_SKILLNO_01+i) == iSkillNo)
			{
				return false;
			}
		}
	}

	return true;
}

bool PgAISkillChooser::CheckUseProjectile(CUnit const* pkUnit, int const iSkillNo)const
{
	if(!pkUnit)
	{
		return false;
	}

	if(pkUnit->GetAbil(AT_USE_NOT_PROJECTILE_SKILL))
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if(pkSkillDef)
		{
			if(pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE))
			{
				return false;
			}
		}
	}
	return true;
}

bool PgAISkillChooser::CheckIsAllySkill(CUnit const* pkUnit, PgGround * pkGround)const
{
	if(!pkUnit || !pkGround)
	{
		return false;
	}

	if(pkUnit->GetAbil(AT_USE_IS_ALLY_SKILL))
	{
		return pkGround->Locked_IsUnitInRange(static_cast<ETeam>(pkUnit->GetAbil(AT_TEAM)), pkUnit->GetPos(), pkUnit->GetDetectRange(), UT_PLAYER);
	}
	return true;
}

bool PgAISkillChooser::CheckIsInClass(CUnit const* pkUnit, int const iSkillNo, PgGround * pkGround)const
{
	if(!pkUnit || !pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(pkSkillDef)
	{
		if(int const iClass = pkSkillDef->GetAbil(AT_SKILL_CHOOSE_IS_IN_CLASS_NO))
		{
			EUnitType eUnitType = static_cast<EUnitType>(pkSkillDef->GetAbil(AT_SKILL_CHOOSE_IS_IN_CLASS_TYPE));
			if(UT_NONETYPE == eUnitType)
			{
				eUnitType = UT_MONSTER;
			}
			if( NULL == pkGround->GetUnitByClassNo(iClass, eUnitType) )
			{
				return false;
			}
		}
	}
	return true;
}

bool PgAISkillChooser::CheckSkillHPLimit(CUnit const* pkUnit, CSkillDef const* pkDef)
{
	float const fHPRate = ((float)pkUnit->GetAbil(AT_HP)/(float)pkUnit->GetAbil(AT_C_MAX_HP))*10000.0f;
	bool bRet = true;
	if(0 < pkDef->GetAbil(AT_SKILL_HP_CHECK_HIGH))
	{
		 bRet = fHPRate > pkDef->GetAbil(AT_SKILL_HP_CHECK_HIGH);//AT_SKILL_HP_CHECK_HIGH어빌이 있고 남은 HP가 AT_SKILL_HP_CHECK_HIGH어빌 이상일 때
	}
	if(bRet && 0 < pkDef->GetAbil(AT_SKILL_HP_CHECK_LOW))	//false일 땐 무조건 실패기 때문에 체크할 필요 없음
	{
		bRet = fHPRate <= pkDef->GetAbil(AT_SKILL_HP_CHECK_LOW);
	}
	
	return 	bRet;
}