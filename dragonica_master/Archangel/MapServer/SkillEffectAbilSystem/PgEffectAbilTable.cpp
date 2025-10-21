#include "stdafx.h"
#include "PgEffectAbilTable.h"

///////////////////////////////////////////////////////////
//  PgAbilTypeTable
///////////////////////////////////////////////////////////
PgAbilTypeTable::PgAbilTypeTable()
{}

PgAbilTypeTable::~PgAbilTypeTable()
{}

void PgAbilTypeTable::Init()
{
	//따로 처리해야되는 어빌 타입의 경우 추가 해주면 됨.
	m_kConAbilTable.insert(std::make_pair(AT_HP,					EAT_TICK_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_MP,					EAT_TICK_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CRITICAL_ONEHIT,		EAT_SETABIL_AND_NULL));	
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_FILTER_TYPE,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+1,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+2,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+3,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+4,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+5,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+6,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+7,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+8,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_01+9,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01,			EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+1,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+2,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+3,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+4,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+5,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+6,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+7,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+8,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_NEED_SKILL_01+9,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_PARENT_CAST_SKILL_NO,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_PROJECTILE_SPEED_RATE,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_CALLER_TYPE,			EAT_IGNORE));	
	m_kConAbilTable.insert(std::make_pair(AT_DEF_RES_NO,			EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_EXCEPT_GRADE,			EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_EXCEPT_GROUND_TYPE,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECT_GROUP_NO,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_MAPMOVE_DELETE,		EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_R_MAX_HP,				EAT_MAX_HP_MP));
	m_kConAbilTable.insert(std::make_pair(AT_R_MAX_MP,				EAT_MAX_HP_MP));
	m_kConAbilTable.insert(std::make_pair(AT_MAX_HP,				EAT_MAX_HP_MP));
	m_kConAbilTable.insert(std::make_pair(AT_MAX_MP,				EAT_MAX_HP_MP));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_01,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_02,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_03,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_04,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_05,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_06,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_07,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_08,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_09,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CHILD_EFFECT_NUM_10,	EAT_ADD_EFFECT_ABIL));
	m_kConAbilTable.insert(std::make_pair(AT_AWAKE_STATE,			EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECT_BEGIN_SEND_TTW_MSG,	EAT_BEGIN_SEND_TTW_MSG));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECT_END_SEND_TTW_MSG,	EAT_END_SEND_TTW_MSG));
	m_kConAbilTable.insert(std::make_pair(AT_HP_DEC_MAX_HP_RATE,	EAT_HPMP_DEC_MAX_HPMP_RATE));
	m_kConAbilTable.insert(std::make_pair(AT_MP_DEC_MAX_MP_RATE,	EAT_HPMP_DEC_MAX_HPMP_RATE));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM1,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM2,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM3,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM4,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM5,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM6,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM7,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM8,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM9,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECTNUM10,	EAT_EFFECT_NUM));
	m_kConAbilTable.insert(std::make_pair(AT_LEVEL,	EAT_IGNORE));
	m_kConAbilTable.insert(std::make_pair(AT_EFFECT_DELETE_ALL_DEBUFF,	EAT_SETABIL_AND_NULL));

	//연계효과 스킬 관련 어빌----------------------------------------------------------------------------------
	m_kConAbilTable.insert(std::make_pair(AT_BEGINL_SKILL_LINKAGE,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET02,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET03,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET04,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET05,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET06,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET07,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET08,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET09,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET10,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT01,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT02,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT03,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT04,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT05,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT06,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT07,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT08,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT09,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT_EFFECT10,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_SKILL_LINKAGE_POINT,	EAT_ADDABIL_AND_NULL));

	m_kConAbilTable.insert(std::make_pair(AT_HIT_AFTER_TARGET_CHANGE_BLOCK_ABLE,	EAT_SETABIL));
	m_kConAbilTable.insert(std::make_pair(AT_ACTION_AFTER_CHANGE_BLOCK_ABLE,	EAT_SETABIL));
	m_kConAbilTable.insert(std::make_pair(AT_CAN_NOT_BLOCK_BY_ACTION,	EAT_SETABIL));

	//연계효과 스킬 관련 어빌 끝--------------------------------------------------------------------------------
	m_kConAbilTable.insert(std::make_pair(AT_COMBAT_REVIVE_DEL_EFFECT_NO,	EAT_SETABIL_AND_NULL));
	m_kConAbilTable.insert(std::make_pair(AT_COMBAT_REVIVE_ADD_EFFECT_NO,	EAT_SETABIL_AND_NULL));
	
	for(int i = 0 ; i < 100; ++i)
	{
		m_kConAbilTable.insert(std::make_pair(AT_FILTER_EXCEPT_EX_01+i,	EAT_IGNORE));

	}

	m_kConAbilTable.insert(std::make_pair(AT_ENABLE_CHECK_ATTACK,	EAT_ATTACK));

	m_kConAbilTable.insert(std::make_pair(AT_PROVOKE_EFFECT_NO,		EAT_SETABIL));

	for(int i=0; i<10; ++i)
	{
		m_kConAbilTable.insert(std::make_pair(AT_LOCK_SKILLNO_01+i,	EAT_SETABIL));
	}

	m_kConAbilTable.insert(std::make_pair(AT_AI_TARGETTING_TYPE,		EAT_SETABIL));
	m_kConAbilTable.insert(std::make_pair(AT_AI_TARGETTING_SECOND_TYPE,	EAT_SETABIL));
	m_kConAbilTable.insert(std::make_pair(AT_AI_TARGETTING_THIRD_TYPE,	EAT_SETABIL));
}

WORD PgAbilTypeTable::GetAbilTypeTable(WORD const wType) const
{
	CONT_ABIL_DEFINE::const_iterator abil_itor = m_kConAbilTable.find(wType);
	
	if(m_kConAbilTable.end() != abil_itor)
	{
		return (*abil_itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EAT_ADDABIL"));
	return EAT_ADDABIL;//없으면 Default
}