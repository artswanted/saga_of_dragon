#include "stdafx.h"
#include "PgObjectUnitDef.h"

int PgObjectUnitDef::GetAbil(WORD const wAbilType) const
{
	switch(wAbilType)
	{
		/*	case AT_ATTR_DEFENCE:		{ iValue = m_byDefenceAttr; }break;
		case AT_ATTR_ATTACK:		{ iValue = m_byAttackAttr; }break;
		//case AT_HP:
		case AT_MAX_HP:
		case AT_C_MAX_HP:			{ iValue = m_sHP; }break;
		//case AT_MP:
		case AT_MAX_MP:
		case AT_C_MAX_MP:			{ iValue = m_sMP; }break;
		case AT_LEVEL:				{ iValue = m_sLevel; }break;
		case AT_PHY_ATTACK_MAX:
		case AT_C_PHY_ATTACK_MAX:	{ iValue = m_sPhyAttackMax; }break;
		case AT_PHY_ATTACK_MIN:
		case AT_C_PHY_ATTACK_MIN:	{ iValue = m_sPhyAttackMin; }break;
		case AT_PHY_DEFENCE:
		case AT_C_PHY_DEFENCE:		{ iValue = m_sPhyDefence; }break;
		case AT_MAGIC_ATTACK:
		case AT_C_MAGIC_ATTACK:		{ iValue = m_sMagicAttack; }break;
		case AT_MAGIC_DEFENCE:
		case AT_C_MAGIC_DEFENCE:	{ iValue = m_sMagicDefence; }break;
		case AT_BLOCK_SUCCESS_VALUE:
		case AT_C_BLOCK_SUCCESS_VALUE:		{ iValue = GET_RATE_VALUE(m_sBlock); }break;
		case AT_DETECT_RANGE:		{ iValue = m_sDetectRange; }break;
		case AT_CHASE_RANGE:		{ iValue = m_sChaseRange; }break;
		case AT_MOVESPEED:
		case AT_C_MOVESPEED:		{ iValue = m_sSpeed; }break;
		case AT_CRITICAL_SUCCESS_VALUE:
		case AT_C_CRITICAL_SUCCESS_VALUE:	{ iValue = GET_RATE_VALUE(m_sCriticalRate); }break;
		case AT_CRITICAL_POWER:
		case AT_C_CRITICAL_POWER:	{ iValue = m_sCriticalPower; }break;
		case AT_MAX_SKILL_NUM:		{ iValue = m_bySkillNum; }break;
		case AT_NORMAL_SKILL_ID:	{ iValue = m_byDefaultSkill; }break;
		case AT_NORMAL_SKILL_RATE:	{ iValue = GET_RATE_VALUE(m_sDefaultSkillRate); }break;
		*/
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

