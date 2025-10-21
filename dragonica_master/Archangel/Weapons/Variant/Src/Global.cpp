#include "stdafx.h"
#include "BM/Localmgr.h"
#include "Lohengrin/LogGroup.h"
#include "Global.h"
#include "DefAbilType.h"
#include "PgActionResult.h"
#include "PgTotalObjectMgr.h"
#include "PgControlDefMgr.h"
#include "tabledatamanager.h"

bool g_bDebugLogOff_20130114 = true;
#define USE_GALAXY_FORMUL
//#ifndef BATTLE_DEBUG
//#define BATTLE_DEBUG
//#endif
inline int GetGroggyPoint(CSkillDef const* pkSkillDef)
{
	if( pkSkillDef )
	{
		int const iCount = std::max(pkSkillDef->GetAbil(AT_COMBO_HIT_COUNT),1);
		return iCount * pkSkillDef->GetAbil(AT_ADD_GROGGYPOINT);
	}
	return 0;
}

inline void GetCasterLevelAndType(CUnit const* pkCaster, int & iCasterLevel, EUnitType & eCasterType)
{
	if( !pkCaster )
	{
		return;
	}

	iCasterLevel = pkCaster->GetBattleLevel();
	eCasterType = pkCaster->UnitType();

	//캐스터가 엔티티 일 경우
	switch(eCasterType)
	{
	case UT_SUMMONED:
	case UT_PET:
	case UT_SUB_PLAYER:
	case UT_ENTITY:
		{
			//캐스터를 호출 한 것이 플래이어일 경우
			if(UT_PLAYER == pkCaster->GetAbil(AT_CALLER_TYPE))
			{
				//캐스터 타입과 레벨을 Player의 것으로 세팅(가디언의 경우 예외)
				eCasterType = UT_PLAYER;
				iCasterLevel = (ENTITY_GUARDIAN!=pkCaster->GetAbil(AT_ENTITY_TYPE)) ? pkCaster->GetAbil(AT_CALLER_LEVEL) : iCasterLevel;
			}
		}break;
	default:
		{
		}break;
	}
}

bool CheckBalanceWStringLen( std::wstring const &wstr, size_t const iLen )
{
	int iRemainLen = static_cast<int>(iLen) * 2;
	std::wstring::const_iterator itr = wstr.begin();
	for ( ; itr != wstr.end() ; ++itr )
	{
		if ( 0xFF >= *itr )
		{
			iRemainLen -= sizeof(char);
		}
		else
		{
			iRemainLen -= sizeof(wchar_t);
		}

		if ( 0 > iRemainLen )
		{
			return false;
		}
	}
	return true;
}

WORD GetBasicAbil(WORD const wAbil)
{
	if (AT_CALCUATEABIL_MIN <= wAbil
		&& AT_CALCUATEABIL_MAX >= wAbil
		)
	{
		return (wAbil / 10 * 10 + 1);
	}
	
	//switch(wAbil)
	//{// 그외 범주를 벗어나는 어빌들은 개별적으로 세팅해주고
	//default:
	//	{// 없다면
	//	}break;
	//}
	return wAbil;
}

// 옵션 증폭 값 뺄 때만 사용 그 외엔 사용 금지.
WORD GetAmpliAbil(WORD const wAbil)
{
	if(wAbil < AT_CALCUATEABIL_MIN || wAbil > AT_CALCUATEABIL_MAX)
	{
		return wAbil + 12;
	}

	return (wAbil / 10 * 10 + 4);
}

bool IsCalculatedAbil(WORD const wAbil)
{
	if (AT_CALCUATEABIL_MIN <= wAbil
		&& AT_CALCUATEABIL_MAX >= wAbil
		)
	{
		return ((wAbil % 10) == 3) ? true : false;
	}

	//switch(wAbil)
	//{// 그외 범주를 벗어나는 어빌들은 개별적으로 세팅해주고
	//}
	return false;
}

bool IsMonsterCardAbil(WORD const wAbil)
{
	if (wAbil < AT_ATTACK_EFFECTNUM01 || (wAbil > AT_ATTACK_EFFECTNUM10_TARGET_TYPE_IS_ME && wAbil < AT_MONSTER_CARD_ABIL_MIN) || wAbil > AT_MONSTER_CARD_ABIL_MAX)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	return true;
}

bool IsHitAbilFromEquipItem(WORD const wAbil)
{// 장비에서 세팅해주는 '타격시 걸리는 이펙트'에 사용되는 어빌인가?
	if (wAbil < AT_ATTACK_EFFECTNUM01_FROM_EQUIP 
		|| (wAbil > AT_ATTACK_EFFECTNUM50_TARGET_TYPE_IS_ME_FROM_EQUIP)
		)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool IsItemActionAbil(WORD const wAbil)
{
	if((wAbil < AT_N_ATTACK_EFFECT_NO_MIN) || (AT_BLOCKED_EFFECT_TARGET_MAX < wAbil))
	{
		return false;
	}
	return true;
}

bool IsRateAbil(WORD const wAbil)
{
	if (wAbil < AT_CALCUATEABIL_MIN || wAbil > AT_CALCUATEABIL_MAX)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return ((wAbil % 10) == 2) ? true : false;
}

WORD GetRateAbil(WORD const wAbil)
{
	if (AT_CALCUATEABIL_MIN <= wAbil
		&& AT_CALCUATEABIL_MAX >= wAbil
		)
	{// 일반 계산 어빌이면, 뒷자리가 2이면 계산 어빌이므로 계산하고
		return (wAbil / 10 * 10 + 2);
	}
	//switch(wAbil)
	//{// 그외 범주를 벗어나는 어빌들은 개별적으로 세팅해주고
	//default:
	//	{// 없다면
	//	}break;
	return 0;	// 0 을 반환 한다
}

WORD GetCalculateAbil(WORD const wAbil)
{
	if (wAbil >= AT_CALCUATEABIL_MIN && wAbil <= AT_CALCUATEABIL_MAX)
	{
		return (wAbil / 10 * 10 + 3);
	}
	
	//switch(wAbil)
	//{// 그외 범주를 벗어나는 어빌들은 개별적으로 세팅해주고
	//}
	return 0;
}

bool IsCalculateAbilRange(WORD const wAbil)
{
	bool bRet = false;
	if( AT_CALCUATEABIL_MIN < wAbil && AT_CALCUATEABIL_MAX > wAbil)				{ bRet = true; }
	if(	AT_I_PHY_DEFENCE_ADD_RATE <= wAbil && AT_I_MAGIC_DEFENCE_ADD_RATE >= wAbil) { bRet = true; }

	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	 return bRet;
}

bool IsElementAbil(WORD const wAbil)
{
	bool bRet = false;

	if( AT_ELEMENT_ABIL_NONE < wAbil && AT_ELEMENT_ABIL_MAX > wAbil) { bRet = true; }

	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bRet;
}

bool IsBasicElementDmgAbilRange(WORD const wAbil)
{
	bool bRet = false;

	if( AT_ATTACK_ADD_FIRE <= wAbil && AT_ATTACK_ADD_DESTROY >= wAbil ) { bRet = true; }
	if( AT_RESIST_ADD_FIRE <= wAbil && AT_RESIST_ADD_DESTROY >= wAbil ) { bRet = true; }

	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bRet;
}

bool IsConvertElementalDmgRateAbilRange(WORD const wAbil)
{
	bool bRet = false;

	if( AT_R_ATTACK_ADD_FIRE_PHY <= wAbil && AT_R_ATTACK_ADD_DESTROY_MAGIC >= wAbil ) { bRet = true; }
	if( AT_R_RESIST_ADD_FIRE_PHY <= wAbil && AT_R_RESIST_ADD_DESTROY_MAGIC >= wAbil ) { bRet = true; }

	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bRet;
}

float CalcTotalElementalResistRate(CUnit const* pAttacker, CUnit const* pDefender, int const iAbil)
{
	if(NULL==pAttacker || NULL==pDefender)
	{
		return 0.0f;
	}

	if(!(AT_C_RESIST_ADD_FIRE <= iAbil && AT_C_RESIST_ADD_DESTROY >= iAbil))
	{
		return 0.0f;//잘못된 어빌
	}

	float result = CalcTotalElementalResistRateSimple(pAttacker->GetAbil(AT_LEVEL), pDefender->GetAbil(iAbil));

	float add = 0;
	add+=pDefender->GetAbil(iAbil+245);//추가어빌
	add+=pDefender->GetAbil(AT_RESIST_RATE_ADD_5ELEMENT);
	if(AT_C_RESIST_ADD_DESTROY!=iAbil)
	{
		add+=pDefender->GetAbil(AT_RESIST_RATE_ADD_4ELEMENT);
	}
	result+=(add*0.0001f);
	return result;
}

float CalcTotalElementalResistRateSimple(int const iAttackerLevel, int const iDefenderResiste)
{
	float fDefenderResiste = static_cast<float>(iDefenderResiste);
	float fAttackerLevel = static_cast<float>(iAttackerLevel);
	float result = fDefenderResiste/(fDefenderResiste*0.6f + fAttackerLevel * fAttackerLevel);

	return result;
}

bool IsCountAbil( WORD const wAbil )
{
	switch ( wAbil )
	{
	case AT_CANNOT_ATTACK:
	case AT_CANNOT_DAMAGE:
//	case AT_UNIT_HIDDEN:
		{
			return true;
		}break;
	default:
		{

		}break;
	}
	return false;
}

int GetCountAbilCheckFlag( int const iFlag )
{
	switch ( iFlag )
	{
	case AT_CF_EFFECT:{return AT_CF_EFFECT_CHK;}break;
	case AT_CF_QUEST:{return AT_CF_QUEST_CHK;}break;
	case AT_CF_EVENTSCRIPT:{return AT_CF_EVENTSCRIPT_CHK;}break;
	case AT_CF_HYPERMOVE:{return AT_CF_HYPERMOVE_CHK;}break;
	case AT_CF_FAKEREMOVE:{return AT_CF_FAKEREMOVE_CHK;}break;
	case AT_CF_ELITEPATTEN:{return AT_CF_ELITEPATTEN_CHK;}break;
	case AT_CF_GM:{return AT_CF_GM_CHK;}break;
	case AT_CF_BS:{return AT_CF_BS_CHK;}break;
	case AT_CF_ALL:{return AT_CF_ALL;}break;
	}

	return AT_CF_NONE;
}

int GetCountAbil( CAbilObject const *pkAbilObj, WORD const wType, int const iFlag )
{
	if ( pkAbilObj && (true == IsCountAbil( wType )) )
	{
		int const iChkType = GetCountAbilCheckFlag( iFlag );
		if ( AT_CF_NONE != iChkType )
		{
			int iValue = pkAbilObj->GetAbil( wType );
			iValue &= iChkType;
			return iValue;
		}
	}
	return 0;
}

bool AddCountAbil( CAbilObject *pkAbilObj, WORD const wType, int const iFlag, bool const bAdd )
{
	if ( pkAbilObj && (true == IsCountAbil( wType )) )
	{
		int const iChkType = GetCountAbilCheckFlag( iFlag );
		if ( AT_CF_NONE == iChkType )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error Flag<") << iFlag << _T("> Type<") << wType << _T(">") );
			return false;
		}

		int iValue = pkAbilObj->GetAbil( wType );
		int const iTypeValue = ( iChkType & iValue );

		if ( true == bAdd )
		{
			if ( iTypeValue && (iChkType == iTypeValue) )
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("ADD Over Type<") << wType << _T("> Flag <") << iFlag << _T(">") );
			}
			else
			{
				iValue += iFlag;
				pkAbilObj->SetAbil( wType, iValue );
			}
		}
		else
		{
			if ( iTypeValue )
			{
				iValue -= iFlag;
				pkAbilObj->SetAbil( wType, iValue );
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("MINUS Over ClassNo<") << pkAbilObj->GetAbil(AT_CLASS) << _T("> Type<") << wType << _T("> Flag <") << iFlag << _T(">") );
			}
		}
		return true;
	}

	return false;
}

bool RemoveCountAbil( CAbilObject *pkAbilObj, WORD const wType, int const iFlag )
{
	if ( pkAbilObj )
	{
		if ( true == IsCountAbil( wType ) )
		{
			int const iChkType = GetCountAbilCheckFlag( iFlag );
			if ( AT_CF_NONE != iChkType )
			{
				int iValue = pkAbilObj->GetAbil( wType );
				iValue &= (~iChkType);
				pkAbilObj->SetAbil( wType, iValue );
				return true;
			}
		}
	}
	return false;
}

void POINT3_2_POINT3BY(POINT3 const &rkPos, POINT3BY& rkOut)
{
	int const iNormalVectorMultiply = 120;
	// rkPos 값은 normalize 된 상태로 입력되어야 한다.
	assert(rkPos.x<=1 && rkPos.y<=1 && rkPos.z <=1);
	rkOut.x = (char)(rkPos.x * iNormalVectorMultiply);
	rkOut.y = (char)(rkPos.y * iNormalVectorMultiply);
	rkOut.z = (char)(rkPos.z * iNormalVectorMultiply);
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Pos[%6.2f, %6.2f, %6.2f]->[%d, %d, %d]"), __FUNCTIONW__, rkPos.x, rkPos.y,
	//	rkPos.z, (int)rkOut.x, (int)rkOut.y, (int)rkOut.z);
}


inline bool IsSubDamageAction(CUnit const * const pkUnit)
{
	if(!pkUnit)
	{
		return false;
	}

	int const iDamageActionType = pkUnit->GetAbil(AT_DAMAGEACTION_TYPE);
	int const iCheck = E_DMGACT_AI_FIRESKILL | E_DMGACT_AI_CHASE_ENEMY;
	return !(iDamageActionType&iCheck);
}

//확률 적용이 없는 부분 수퍼아머 타입 검사(참: 액션한다, 거짓: 액션안한다)
inline bool IsNotRatePartDamageAction(CUnit const * const pkUnit)
{
	if(!pkUnit)
	{
		return false;
	}

	int const iDamageActionType = pkUnit->GetAbil(AT_DAMAGEACTION_TYPE);
	if(iDamageActionType&E_DMGACT_TYPE_PART_SUPERARMOUR)
	{
		if(EAI_ACTION_NONE==pkUnit->GetAI()->eCurrentAction )
		{//클라용, 인공지능이 없는 상태
			if(pkUnit->GetAbil(AT_POSTURE_STATE) || EAI_ACTION_BLOWUP==pkUnit->GetAI()->eCurrentAction)
			{
				return false;
			}

			if(pkUnit->IsState(US_IDLE) && pkUnit->GetDelay())
			{//IDEL상태에서 Delay가 있으면 Fire 중임
				return false;
			}
		}

		if(pkUnit->IsState(US_SKILL_CAST) || pkUnit->IsState(US_SKILL_FIRE) || pkUnit->IsState(US_FIRE_WAITING))
		{
			return false;
		}
	}
	return true;
}
inline bool IsDamageAction(CUnit const * const pkUnit, CSkillDef const * const pkSkillDef)
{
	if(!pkUnit)
	{
		return false;
	}

	if(!pkSkillDef)
	{
		return false;
	}

	if(pkSkillDef->GetAbil(AT_DAMAGEACTION_FORCE))
	{
		// 스킬에서 강제로 Damage Action 이 들어갈수 있게 한 경우
		return true;
	}

	int const iDamageActionType = pkUnit->GetAbil(AT_DAMAGEACTION_TYPE);
	if((iDamageActionType&E_DMGACT_CHECK) || (pkSkillDef->GetAbil(AT_DAMAGEACTION_TYPE)!=0))
	{
		return false;
	}

	if(iDamageActionType&E_DMGACT_TYPE_PART_SUPERARMOUR)
	{
		if( !IsNotRatePartDamageAction(pkUnit) )
		{
			return false;
		}

		int const iRate = pkUnit->GetAbil(AT_DAMAGEACTION_RATE);
		if( iRate )
		{
			return false;
		}
	}

	return true;
}

void AddDamageEffectAndChangeBlockValue(CUnit* pkCaster, CUnit* pkTarget, CSkillDef const* pkSkillDef, PgActionResult* pkResult)
{
	if(!pkCaster || !pkTarget || !pkSkillDef || !pkResult)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("AddDamageEffectAndChangeBlockValue Failed!"));
		return;
	}

	int const iRate = pkTarget->GetAbil(AT_DAMAGEACTION_RATE);//임시
	//{ BM::vstring vStr("블럭리액션확률: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
	DWORD const dwRand = pkCaster->GetRandom();
	bool bDamageAction = false;

	if (IsDamageAction(pkTarget,pkSkillDef))
	{
		bDamageAction = true;
		int const iDmgEffect = pkSkillDef->GetAbil(AT_DAM_EFFECT_S + pkTarget->GetAbil(AT_UNIT_SIZE) -1);
		if (iDmgEffect > 0)
		{
			if(pkTarget->CheckSkillFilter(iDmgEffect, NULL, ESFilter_Ignore_Action_Effect))
			{
				bool bCanBlock = PgActionResult::EBT_REACTION == pkResult->GetBlocked();
				bCanBlock &= PgSkillHelpFunc::CanChangeToBlockActionEffect(iDmgEffect);
				if(bCanBlock)
				{
					bDamageAction = false;
				}
				else
				{
					pkResult->SetBlocked(PgActionResult::EBT_NONE);
					pkResult->AddEffect(iDmgEffect);
				}
			}
		}
	}
	else	//데미지액션 무시 타입이 있으면
	{
		if(!pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE) || 0 == pkSkillDef->GetAbil(AT_DAMAGEACTION_TYPE) )	//발사체일 경우 데미지 액션을 하지 않음
		{
			if( iRate
			 && IsSubDamageAction(pkTarget)
			 && IsNotRatePartDamageAction(pkTarget) )
			{
				if(iRate > int(dwRand % ABILITY_RATE_VALUE))	//데미지 액션을 한다
				{
					bDamageAction = true;
					int const iDmgEffect = pkSkillDef->GetAbil(AT_DAM_EFFECT_S + pkTarget->GetAbil(AT_UNIT_SIZE) -1);
					if (0 < iDmgEffect)
					{
						if(pkTarget->CheckSkillFilter(iDmgEffect, NULL, ESFilter_Ignore_Action_Effect))
						{
							bool bCanBlock = PgActionResult::EBT_REACTION == pkResult->GetBlocked();
							bCanBlock &= PgSkillHelpFunc::CanChangeToBlockActionEffect(iDmgEffect);
							if(bCanBlock)
							{
								bDamageAction = false;
							}
							else
							{
								pkResult->SetBlocked(PgActionResult::EBT_NONE);
								pkResult->AddEffect(iDmgEffect);
							}
						}
					}
					SetDamageDelay(pkSkillDef, pkCaster, pkTarget);	//여기에서 EAI_EVENT_DAMAGED 해주니까
				}
				else
				{
					int iRate = pkTarget->GetAbil(AT_AI_TARGETTING_RATE);
					if(0 == iRate)
					{
						iRate = 3000;//기본적으로 30%
					}

					if (iRate > BM::Rand_Index(iRate) && pkTarget->GetReserveTarget().IsNull())
					{
						pkTarget->SetReserveTarget(pkCaster->GetID());
					}	
					pkResult->SetEndure(true);
				}
			}
		}
	}

	pkTarget->SetDamageAction(bDamageAction);
	pkResult->SetDamageAction(bDamageAction);
}

void GetChildSkillPower(bool const bPhysicSkill, PgPlayer * pkPlayer, CSkillDef const* pkSkillDef, int & iSkillPower, int & iSkillPRate, int & iInvenAttackAddRate)
{
	if(!pkPlayer || !pkSkillDef)
	{
		return;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	int iSkillindex = 0;
	int iAT_Attack = AT_PHY_ATTACK;
	int iAT_Dmg_Per = AT_PHY_DMG_PER;
	//int iAT_Dmg_Per2 = AT_PHY_DMG_PER2;
	int iAT_Attack_Add_Rate = AT_I_PHY_ATTACK_ADD_RATE;

	if(false == bPhysicSkill)
	{
		iAT_Attack = AT_MAGIC_ATTACK;
		iAT_Dmg_Per = AT_MAGIC_DMG_PER;
		//iAT_Dmg_Per2 = AT_MAGIC_DMG_PER2;
		iAT_Attack_Add_Rate = AT_I_MAGIC_ATTACK_ADD_RATE;
	}

	while(CHILD_SKILL_MAX > iSkillindex)
	{
		//영향 받는 베이스 스킬
		int const iChildBaseSkillNo = pkSkillDef->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
		if(0 < iChildBaseSkillNo)
		{
			if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
			{
				//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
				int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
				if(0 < iLearnedChildSkillNo)
				{
					if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
					{
						iSkillPower += pkLearnedSkillDef->GetAbil(iAT_Attack);
						iSkillPRate += pkLearnedSkillDef->GetAbil(iAT_Dmg_Per);
						//iSkillPRate += pkLearnedSkillDef->GetAbil(iAT_Dmg_Per2);
						iInvenAttackAddRate += pkLearnedSkillDef->GetAbil(iAT_Attack_Add_Rate);
					}
				}
			}
		}
		else
		{
			break;
		}
		++iSkillindex;
	}
}

bool CS_GetSkillResultDefault(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector, CUnit * pkCaller)
{
	if(rkTargetArray.empty())
	{
		return true;
	}

#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"SkillNo["<<iSkillNo<<L"]");
#endif
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (NULL == pkSkillDef)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get SkillDef, SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	bool bPhysicSkill = pkSkillDef->IsSkillAtt(SAT_PHYSICS);
	int iMin = 0, iMax = 0, iSkillPRate = 0, iSkillPower = 0;
	int iInvenAttackAddRate = 0;
	int iAddPower = 0;
	if (bPhysicSkill)
	{
#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"\t\t** PHYSICAL ATTACK");
#endif
		iMin = pkCaster->GetAbil(AT_C_PHY_ATTACK_MIN);
		iMax = pkCaster->GetAbil(AT_C_PHY_ATTACK_MAX);
		iSkillPower = pkSkillDef->GetAbil(AT_PHY_ATTACK);
		iSkillPRate = pkSkillDef->GetAbil(AT_PHY_DMG_PER);
		//iSkillPRate += pkSkillDef->GetAbil(AT_PHY_DMG_PER2);
		iInvenAttackAddRate = pkSkillDef->GetAbil(AT_I_PHY_ATTACK_ADD_RATE);

		//캐스터가 PgPlayer라면
		if(UT_PLAYER == pkCaster->UnitType())
		{
			GetChildSkillPower(bPhysicSkill, dynamic_cast<PgPlayer*>(pkCaster), pkSkillDef, iSkillPower, iSkillPRate, iInvenAttackAddRate);
		}
		else if(UT_SUMMONED == pkCaster->UnitType())
		{
			GetChildSkillPower(bPhysicSkill, dynamic_cast<PgPlayer*>(pkCaller), pkSkillDef, iSkillPower, iSkillPRate, iInvenAttackAddRate);
		}
		else if(UT_ENTITY == pkCaster->UnitType())
		{
			int iSkillindex = 0;
			while(CHILD_SKILL_MAX > iSkillindex)
			{
				//Entity는 Entity에 미리 저장되어 있다.
				//영향 받는 베이스 스킬 실제 배운 스킬 번호가 저장되어 있다.
				int const iLearnedChildSkillNo = pkCaster->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
				if(0 < iLearnedChildSkillNo)
				{
					if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
					{
						iSkillPower += pkLearnedSkillDef->GetAbil(AT_PHY_ATTACK);
						iSkillPRate += pkLearnedSkillDef->GetAbil(AT_PHY_DMG_PER);
						//iSkillPRate += pkLearnedSkillDef->GetAbil(AT_PHY_DMG_PER2);
						iInvenAttackAddRate += pkLearnedSkillDef->GetAbil(AT_I_PHY_ATTACK_ADD_RATE);
					}
				}
				else
				{
					break;
				}
				++iSkillindex;
			}
		}
	}
	else
	{
#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"\t\t__ MAGIC ATTACK");
#endif
		iMin = pkCaster->GetAbil(AT_C_MAGIC_ATTACK_MIN);
		iMax = pkCaster->GetAbil(AT_C_MAGIC_ATTACK_MAX);
		iSkillPower = pkSkillDef->GetAbil(AT_MAGIC_ATTACK);
		iSkillPRate = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
		//iSkillPRate += pkSkillDef->GetAbil(AT_MAGIC_DMG_PER2);
		iInvenAttackAddRate = pkSkillDef->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE);
		
		if(UT_PLAYER == pkCaster->UnitType())
		{
			GetChildSkillPower(bPhysicSkill, dynamic_cast<PgPlayer*>(pkCaster), pkSkillDef, iSkillPower, iSkillPRate, iInvenAttackAddRate);
		}
		else if(UT_SUMMONED == pkCaster->UnitType())
		{
			GetChildSkillPower(bPhysicSkill, dynamic_cast<PgPlayer*>(pkCaller), pkSkillDef, iSkillPower, iSkillPRate, iInvenAttackAddRate);
		}
		else if(UT_ENTITY == pkCaster->UnitType())
		{
			int iSkillindex = 0;
			while(CHILD_SKILL_MAX > iSkillindex)
			{
				//Entity는 Entity에 미리 저장되어 있다.
				//영향 받는 베이스 스킬 실제 배운 스킬 번호가 저장되어 있다.
				int const iLearnedChildSkillNo = pkCaster->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
				if(0 < iLearnedChildSkillNo)
				{
					if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
					{
						iSkillPower += pkLearnedSkillDef->GetAbil(AT_MAGIC_ATTACK);
						iSkillPRate += pkLearnedSkillDef->GetAbil(AT_MAGIC_DMG_PER);
						//iSkillPRate += pkLearnedSkillDef->GetAbil(AT_MAGIC_DMG_PER2);
						iInvenAttackAddRate += pkLearnedSkillDef->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE);
					}
				}
				else
				{
					break;
				}
				++iSkillindex;
			}
		}

		

	}

	if (0 == iSkillPRate)
	{
		iSkillPRate = ABILITY_RATE_VALUE;
	}

	if (bPhysicSkill)
	{
		// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
		if(0 < pkCaster->GetAbil(AT_PHY_DMG_PER))
		{
			iSkillPRate = static_cast<int>(iSkillPRate * static_cast<float>(pkCaster->GetAbil(AT_PHY_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
		}
		// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
		else if(0 > pkCaster->GetAbil(AT_PHY_DMG_PER))
		{
			iSkillPRate += pkCaster->GetAbil(AT_PHY_DMG_PER);
		}
		// pkCaster AT_PHY_DMG_PER가 저렇게 동작해야 하는지 이해할수 없지만 사이드 이펙트 최소화를 위해 다시 계산해주는 부분을 만듬
		int DmgPer2 = pkCaster->GetAbil(AT_PHY_DMG_PER2);
		if( pkCaster->IsUnitType(UT_SUB_PLAYER) )
		{
			DmgPer2 += pkCaster->GetAbil(AT_PHY_DMG_PER2_SUB_PLAYER);
		}
		if(DmgPer2)
		{
			iSkillPRate += (iSkillPRate *  DmgPer2)/ABILITY_RATE_VALUE;
		}
	}
	else
	{
		// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
		if(0 < pkCaster->GetAbil(AT_MAGIC_DMG_PER))
		{
			iSkillPRate = static_cast<int>(iSkillPRate * static_cast<float>(pkCaster->GetAbil(AT_MAGIC_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
		}
		// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
		else if(0 > pkCaster->GetAbil(AT_MAGIC_DMG_PER))
		{
			iSkillPRate += pkCaster->GetAbil(AT_MAGIC_DMG_PER);
		}
		// pkCaster AT_MAGIC_DMG_PER가 저렇게 동작해야 하는지 이해할수 없지만 사이드 이펙트 최소화를 위해 다시 계산해주는 부분을 만듬
		int DmgPer2 = pkCaster->GetAbil(AT_MAGIC_DMG_PER2);
		if( pkCaster->IsUnitType(UT_SUB_PLAYER) )
		{
			DmgPer2 += pkCaster->GetAbil(AT_MAGIC_DMG_PER2_SUB_PLAYER);
		}
		if(DmgPer2)
		{
			iSkillPRate += ( iSkillPRate * DmgPer2 )/ABILITY_RATE_VALUE;
		}
	}
	//{ BM::vstring vStr("무기 공격값: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
	DWORD const iInvenAttackRandomValue = pkCaster->GetRandom();
	if( iInvenAttackAddRate )
	{
		int iInvenAttackMin = 0;
		int iInvenAttackMax = 0;
		if (bPhysicSkill)
		{
			iInvenAttackMin = pkCaster->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MIN);
			iInvenAttackMax = pkCaster->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MAX);			
		}
		else
		{
			iInvenAttackMin = pkCaster->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN);
			iInvenAttackMax = pkCaster->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX);			
		}

		int const iInvenAttack = abs(iInvenAttackMax - iInvenAttackMin);
		
		int iResult = 0;
		if(0 != iInvenAttack)
		{
			iResult = iInvenAttackRandomValue % iInvenAttack;
		}

		iAddPower = iResult + iInvenAttackMin;
		iAddPower = int(iAddPower * (float)iInvenAttackAddRate / ABILITY_RATE_VALUE_FLOAT);
	}
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"bPhysic["<<bPhysicSkill<<L"], Min["<<iMin<<L"], Max["<<iMax<<L"], SkillPower["<<iSkillPower<<L"], SkillRate["<<iSkillPRate<<L"]");
#endif
	int const iCriticalPowerRate = pkCaster->GetAbil(AT_C_CRITICAL_POWER);

	int const iDecDodgeAbs = pkCaster->GetAbil(AT_DEC_TARGETDODGE_ABS);
	int const iDecDodgeRate = pkCaster->GetAbil(AT_DEC_TARGETDODGE_RATE);

	__int64 i64Damage = 0;
	UNIT_PTR_ARRAY::const_iterator itor = rkTargetArray.begin();
	while (rkTargetArray.end()  != itor)
	{
		//참고해야하는 유닛 이면
		if(true == (*itor).bReference)
		{
			//나중에 처리
			++itor;
			continue;
		}
		CUnit* pkTarget = itor->pkUnit;

		PgActionResult* pkResult = pkResultVector->GetResult(pkTarget->GetID(), true);
		if( NULL == pkResult )
		{
			++itor;
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"ActionResult is NULL Unit Guid["<<itor->pkUnit->GetID()<<L"]");
			continue;
		}

		// Target이 실제로 Damage를 입든 안입든, Target이 다시 잡히지 않도록 해야 한다.
		pkCaster->OnTargetDamaged(pkTarget->GetID());
		pkResult->SetValue(0);
		if(	(NULL == pkTarget) 
		||	(pkTarget->IsDead()))
		{
			++itor;
			pkResult->SetInvalid(true);
			if(false==g_bDebugLogOff_20130114)
			{
				INFO_LOG(BM::LOG_LV9, __FL__<<L"Invalid Target");
			}
			continue;
		}

		// 절대 Block Rate 계산
		//{ BM::vstring vStr("절대블록: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
		if (iRandValue < pkTarget->GetAbil(AT_100PERECNT_BLOCK_RATE))
		{
			pkResult->SetBlocked(PgActionResult::EBT_NONE_REACTION);
			++itor;
			continue;
		}
#ifdef BATTLE_DEBUG
		//INFO_LOG(BM::LOG_LV9, __FL__<<L"\tHitRate["<<iHitRate<<L"] - Dodge["<<pkTarget->GetAbil(AT_C_DODGE_SUCCESS_VALUE)<<L"], Rand["<<iRandValue<<L"]");
#endif
		//{ BM::vstring vStr("명중굴림: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int const iHitSuccessRate = PgSkillHelpFunc::GetCalcHitSuccessRate(pkCaster, pkTarget, pkSkillDef, iDecDodgeAbs, iDecDodgeRate);
		iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
#ifdef BATTLE_DEBUG
		//INFO_LOG(BM::LOG_LV7, _T("HitRate=") << iDecHitRate);
#endif
		if( iHitSuccessRate <= iRandValue )
		{
			// 타격 실패 (Missed)
			pkResult->SetMissed(true);
#ifdef BATTLE_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"--Missed");
#endif
			++itor;
			continue;
		}

		bool bReActionBlockSuccess = false;
		if( PgSkillHelpFunc::IsAbleToBlock(pkTarget, pkSkillDef) )	// Block이 가능하면
		{// Block Rate 계산 
			//{ BM::vstring vStr("일반블록: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
			/*
				(클라와 서버 부분이 정확히 싱크가 되지 않는다. 따라서 이곳에서는 GetRandom()을 호출하지 않고, 이전의 값을 그대로 사용한다
				iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE; 
			*/
			int const iBlockRate = PgSkillHelpFunc::CalcSuccessValueToRate(PgSkillHelpFunc::EAT_BLOCK, pkCaster, pkTarget, NULL, 0);
			if (iRandValue < iBlockRate)
			{
				pkResult->SetBlocked(PgActionResult::EBT_REACTION);
				pkResult->AddEffect(ACTIONEFFECT_BLOCK);
				bReActionBlockSuccess = true;
				//++itor;
				//continue;
			}
		}
		if( false == bReActionBlockSuccess )
		{// 이 스킬에 피격되면
			if( ECBA_MAKE_CAN_NOT_BLOCK == pkSkillDef->GetAbil(AT_HIT_AFTER_TARGET_CHANGE_BLOCK_ABLE) )
			{// 블록 불가 상태가 되어야 하면 선택하고 
				pkTarget->SetAbil(AT_CAN_NOT_BLOCK_BY_ACTION, 1, true, true);
			}
		}

		int iCriticalRate = PgSkillHelpFunc::CalcSuccessValueToRate(PgSkillHelpFunc::EAT_CRITICAL, pkCaster, pkTarget, pkSkillDef, 0);
		int const iOneHit = pkCaster->GetAbil(AT_CRITICAL_ONEHIT);
		if (0 < iOneHit)
		{
			iCriticalRate = ABILITY_RATE_VALUE;
			pkCaster->SetAbil(AT_CRITICAL_ONEHIT, iOneHit-1);
		}

		//기본적인 공격자의 공격력 + 스킬 공격력
		//{ BM::vstring vStr("데미지랜덤: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;

		// 최종공격력(물리) = 기본공격력 + 아이템공격력 + 물리후방공격력
		// 최종공격력(마법) = (기본공격력 + 아이템공격력) * 1.5		
		int const iAddDmg = (bPhysicSkill ? (pkCaster->GetAbil(AT_C_ABS_ADDED_DMG_PHY)) : (pkCaster->GetAbil(AT_C_ABS_ADDED_DMG_MAGIC)));

		i64Damage = int((iMin + iRandValue % __max(1, iMax-iMin)) * ((bPhysicSkill) ? 1.0f : 1.5f)) + iAddDmg;
		i64Damage += iSkillPower;	// Skill 증가값은 공격력에 더하고, Skill증가률은 Damage 나온 값에 곱해준다.
		i64Damage += iAddPower;
		{// 스킬 데미지 조정
			i64Damage =  PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_DMG, iSkillNo, pkCaster, i64Damage);
			{
				__int64 const i64OriginDmamge = i64Damage;
				// 누적 데미지 값을 계산하고
				i64Damage = PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_INC_PHASE_DMG, iSkillNo, pkCaster, i64Damage);
				// 스킬 데미지를 계산한 시점의 값으로,최저 최대 값 얻어와
				__int64 const i64MinDmg = PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_RESULT_MIN_DMG_BY_PHASE, iSkillNo, pkCaster, i64OriginDmamge);
				__int64 const i64MaxDmg = PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_RESULT_MAX_DMG_BY_PHASE, iSkillNo, pkCaster, i64OriginDmamge);
				//보정해준다.
				if(0 != i64MinDmg
					&& i64OriginDmamge > i64MinDmg	// min값 적용하는데 원래 데미지보다 더 늘어나면 말이 안됨 
					&& i64MinDmg > i64Damage
					)
				{
					i64Damage = i64MinDmg;
				}
				if(0 != i64MaxDmg
					&& i64OriginDmamge < i64MaxDmg	// max값 적용하는데 원래 데미지가 줄어들면 말이 안됨
					&& i64MaxDmg < i64Damage
					)
				{ 
					i64Damage = i64MaxDmg; 
				}
			}

		}
		
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"AttackPower["<<i64Damage<<L"] = AttackPower + 2ndPower["<<i64Damage<<L"]");
#endif
#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"Final AttackPower : "<<i64Damage<<L"");
#endif
		int iExtraDamageRate = pkCaster->GetAbil(AT_1ST_ATTACK_ADDED_RATE);
		if (0 != iExtraDamageRate)
		{
#ifdef BATTLE_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"ExtraDamageRate["<<iExtraDamageRate<<L"]");
#endif
			i64Damage = i64Damage + i64Damage * iExtraDamageRate / ABILITY_RATE_VALUE;
			// 1방에만 적용하는 기능을 여러대 적용하는 어빌이
			int const iApplyNum = pkCaster->GetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM);
			if(1 < iApplyNum)
			{// 충분하게 존재하면 횟수만 감소 시키고
				pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, iApplyNum-1);
			}
			else
			{// 이번이 마지막 적용이면 0으로 횟수와, 증가값을 초기화 시킨다 
				pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, 0);
				pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE, 0);
			}
		}

		i64Damage = CS_CheckDamage(pkCaster, pkTarget, i64Damage, bPhysicSkill, pkResult, iSkillPRate, pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE), pkSkillDef);	

#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"iDamage["<<i64Damage<<L"]");
#endif
		//{ BM::vstring vStr("크리티컬성공랜덤: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str());}
		iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
		bool const bIsCritical = iRandValue < iCriticalRate;
		if (bIsCritical)
		{
			i64Damage = PgSkillHelpFunc::CalcCriticalDamage( i64Damage, pkCaster, pkTarget );
#ifdef BATTLE_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"--Ciritical");
#endif
			pkResult->SetCritical(true);
		}
		i64Damage = CS_CheckItemDamage( pkCaster, pkTarget, bIsCritical, i64Damage );

		// 반사 데미지 계산 및 ActionResult에 설정
		PgSkillHelpFunc::CalcReflectDamage(pkCaster, pkTarget, i64Damage, pkResult, pkSkillDef, bPhysicSkill, false, false);
		{// 속성 데미지 계산 부분
			int const iDestroyDmg = pkCaster->GetAbil(AT_C_ATTACK_ADD_DESTROY) 
				* (1.0f - CalcTotalElementalResistRate(pkCaster, pkTarget, AT_C_RESIST_ADD_DESTROY));
			int const iFireDmg = pkCaster->GetAbil(AT_C_ATTACK_ADD_FIRE)
				* (1.0f - CalcTotalElementalResistRate(pkCaster, pkTarget, AT_C_RESIST_ADD_FIRE));

			int const iIceDmg = pkCaster->GetAbil(AT_C_ATTACK_ADD_ICE)
				* (1.0f -CalcTotalElementalResistRate(pkCaster, pkTarget, AT_C_RESIST_ADD_ICE));

			int const iNatureDmg = pkCaster->GetAbil(AT_C_ATTACK_ADD_NATURE)
				* (1.0f -CalcTotalElementalResistRate(pkCaster, pkTarget, AT_C_RESIST_ADD_NATURE));

			int const iCurseDmg = pkCaster->GetAbil(AT_C_ATTACK_ADD_CURSE)
				* (1.0f -CalcTotalElementalResistRate(pkCaster, pkTarget, AT_C_RESIST_ADD_CURSE));

			int const iTotalElemDmg = iDestroyDmg + iFireDmg + iIceDmg + iNatureDmg + iCurseDmg;
			i64Damage += iTotalElemDmg;
		}

		AddDamageEffectAndChangeBlockValue(pkCaster, pkTarget, pkSkillDef, pkResult);
#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"Final Damage["<<i64Damage<<L"]");
#endif
		{// F7 감소율에 의한 최종 데미지감소(모든 데미지 계산 후에 가장 마지막에 위치해야 함)
			int iAfterDecPer = 0;
			if ( bPhysicSkill )
			{
				iAfterDecPer = pkTarget->GetAbil(AT_C_ABS_DEC_DMG_PHY);
			}
			else
			{
				iAfterDecPer = pkTarget->GetAbil(AT_C_ABS_DEC_DMG_MAGIC);
			}
			if( iAfterDecPer > ABILITY_RATE_VALUE_FLOAT )
			{// 데미지 감소만을 위한 항목이므로 음수가 나와서는 안되고 항상 기본값인 10000 이상이어야 한다.
				int iAfterDecPerDamage = 
					static_cast<int>(i64Damage * ((static_cast<float>(iAfterDecPer) - ABILITY_RATE_VALUE_FLOAT ) / ABILITY_RATE_VALUE_FLOAT));
				//감소하는 데미지가 더 크면 데미지는 1
				if(i64Damage <= iAfterDecPerDamage)
				{
					i64Damage = 1;
				}
				else
				{
					i64Damage -= iAfterDecPerDamage;
				}
			}
		}

		if(0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)) //데미지를 입을 수 있는 상태일때만 세팅
		{
			int iResultDamage = (0!=pkTarget->GetAbil(AT_DAMAGE_IS_ONE)) ? 1+pkCaster->GetAbil(AT_DAMAGE_IS_ONE_ALPHA) : static_cast<int>( std::min(static_cast<__int64>(INT_MAX), i64Damage) );
			if( PgActionResult::EBT_REACTION == pkResult->GetBlocked() )
			{// 리액션 블록이면 데미지 감소 
				iResultDamage = PgSkillHelpFunc::CalcDamageAfterBlock( iResultDamage, pkTarget->GetAbil(AT_C_BLOCK_DAMAGE_RATE) );
			}
			pkResult->SetValue(iResultDamage);
		}

		++itor;
	}

	CheckActionResultVec(rkTargetArray, *pkResultVector);

	return true;
}


// bPhysicalAttack : 물리공격이냐?
// caster : 공격하는 unit
// target : 공격 받는 unit
// iDmgRate : 최종스킬의 Damage에서 몇%를 Damage로 계산할 것인가?
//		룰렛같은 스킬에서 최종공격력의 %로 Damage를 줘야 한다.
// iDmg : 최종스킬의 Damage에서 추가적인 Damage를 더해줘라-- aresult : 결과값
bool CS_GetDmgResult(int const iSkillNo, CUnit* pkCaster, CUnit* pkTarget, int const iDmgRate, int const iDmg, PgActionResult* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (NULL == pkSkillDef)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get SkillDef, SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	bool bPhysicSkill = pkSkillDef->IsSkillAtt(SAT_PHYSICS);
	int iMin = 0, iMax = 0, iSkillPRate = 0, iSkillPower = 0;
	int iInvenAttackAddRate = 0;
	int iAddPower = 0;
	if (bPhysicSkill)
	{
		iMin = pkCaster->GetAbil(AT_C_PHY_ATTACK_MIN);
		iMax = pkCaster->GetAbil(AT_C_PHY_ATTACK_MAX);
		iSkillPower = pkSkillDef->GetAbil(AT_PHY_ATTACK);
		iSkillPRate = pkSkillDef->GetAbil(AT_PHY_DMG_PER);
		//iSkillPRate += pkSkillDef->GetAbil(AT_PHY_DMG_PER2);
		iInvenAttackAddRate = pkSkillDef->GetAbil(AT_I_PHY_ATTACK_ADD_RATE);

		//캐스터가 PgPlayer라면
		if(UT_PLAYER == pkCaster->UnitType())
		{
			if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster))
			{
				int iSkillindex = 0;
				while(CHILD_SKILL_MAX > iSkillindex)
				{
					//영향 받는 베이스 스킬
					int const iChildBaseSkillNo = pkSkillDef->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
					if(0 < iChildBaseSkillNo)
					{
						if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
						{
							//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
							int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
							if(0 < iLearnedChildSkillNo)
							{
								if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
								{
									iSkillPower += pkLearnedSkillDef->GetAbil(AT_PHY_ATTACK);
									iSkillPRate += pkLearnedSkillDef->GetAbil(AT_PHY_DMG_PER);
									//iSkillPRate += pkLearnedSkillDef->GetAbil(AT_PHY_DMG_PER2);
									iInvenAttackAddRate += pkLearnedSkillDef->GetAbil(AT_I_PHY_ATTACK_ADD_RATE);
								}
							}
						}
					}
					else
					{
						break;
					}
					++iSkillindex;
				}			
			}
		}
	}
	else
	{
		iMin = pkCaster->GetAbil(AT_C_MAGIC_ATTACK_MIN);
		iMax = pkCaster->GetAbil(AT_C_MAGIC_ATTACK_MAX);
		iSkillPower = pkSkillDef->GetAbil(AT_MAGIC_ATTACK);
		iSkillPRate = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
		//iSkillPRate += pkSkillDef->GetAbil(AT_MAGIC_DMG_PER2);
		iInvenAttackAddRate = pkSkillDef->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE);

		//캐스터가 PgPlayer라면
		if(UT_PLAYER == pkCaster->UnitType())
		{
			if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster))
			{
				int iSkillindex = 0;
				while(CHILD_SKILL_MAX > iSkillindex)
				{
					int const iChildBaseSkillNo = pkSkillDef->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
					if(0 < iChildBaseSkillNo)
					{
						if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
						{
							//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
							int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
							if(0 < iLearnedChildSkillNo)
							{
								if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
								{
									iSkillPower += pkLearnedSkillDef->GetAbil(AT_MAGIC_ATTACK);
									iSkillPRate += pkLearnedSkillDef->GetAbil(AT_MAGIC_DMG_PER);
									//iSkillPRate += pkLearnedSkillDef->GetAbil(AT_MAGIC_DMG_PER2);
									iInvenAttackAddRate += pkLearnedSkillDef->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE);
								}
							}
						}
					}
					else
					{
						break;
					}
					++iSkillindex;
				}			
			}
		}
	}

	if (0 == iSkillPRate)
	{
		iSkillPRate = ABILITY_RATE_VALUE;
	}

	if (bPhysicSkill)
	{
		// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
		if(0 < pkCaster->GetAbil(AT_PHY_DMG_PER))
		{
			iSkillPRate = static_cast<int>(iSkillPRate * static_cast<float>(pkCaster->GetAbil(AT_PHY_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
		}
		// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
		else if(0 > pkCaster->GetAbil(AT_PHY_DMG_PER))
		{
			iSkillPRate += pkCaster->GetAbil(AT_PHY_DMG_PER);
		}
		// pkCaster AT_PHY_DMG_PER가 저렇게 동작해야 하는지 이해할수 없지만 사이드 이펙트 최소화를 위해 다시 계산해주는 부분을 만듬
		int DmgPer2 = pkCaster->GetAbil(AT_PHY_DMG_PER2);
		if( pkCaster->IsUnitType(UT_SUB_PLAYER) )
		{
			DmgPer2 += pkCaster->GetAbil(AT_PHY_DMG_PER2_SUB_PLAYER);
		}
		if(DmgPer2)
		{
			iSkillPRate += ( iSkillPRate * DmgPer2 )/ABILITY_RATE_VALUE;
		}
	}
	else
	{
		// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
		if(0 < pkCaster->GetAbil(AT_MAGIC_DMG_PER))
		{
			iSkillPRate = static_cast<int>(iSkillPRate * static_cast<float>(pkCaster->GetAbil(AT_MAGIC_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
		}
		// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
		else if(0 > pkCaster->GetAbil(AT_MAGIC_DMG_PER))
		{
			iSkillPRate += pkCaster->GetAbil(AT_MAGIC_DMG_PER);
		}
		// pkCaster AT_MAGIC_DMG_PER가 저렇게 동작해야 하는지 이해할수 없지만 사이드 이펙트 최소화를 위해 다시 계산해주는 부분을 만듬
		int DmgPer2 = pkCaster->GetAbil(AT_MAGIC_DMG_PER2);
		if( pkCaster->IsUnitType(UT_SUB_PLAYER) )
		{
			DmgPer2 += pkCaster->GetAbil(AT_MAGIC_DMG_PER2_SUB_PLAYER);
		}
		if(DmgPer2)
		{
			iSkillPRate += ( iSkillPRate * DmgPer2 )/ABILITY_RATE_VALUE;
		}
	}

	DWORD const iInvenAttackRandomValue = pkCaster->GetRandom();
	if( iInvenAttackAddRate )
	{
		int iInvenAttackMin = 0;
		int iInvenAttackMax = 0;
		if (bPhysicSkill)
		{
			iInvenAttackMin = pkCaster->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MIN);
			iInvenAttackMax = pkCaster->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MAX);			
		}
		else
		{
			iInvenAttackMin = pkCaster->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN);
			iInvenAttackMax = pkCaster->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX);			
		}

		int const iInvenAttack = abs(iInvenAttackMax - iInvenAttackMin);
		
		int iResult = 0;
		if(0 != iInvenAttack)
		{
			iResult = iInvenAttackRandomValue % iInvenAttack;
		}

		iAddPower = iResult + iInvenAttackMin;
		iAddPower = int(iAddPower * (float)iInvenAttackAddRate / ABILITY_RATE_VALUE_FLOAT);
	}

	int iCriticalRate = PgSkillHelpFunc::CalcSuccessValueToRate(PgSkillHelpFunc::EAT_CRITICAL, pkCaster, pkTarget, pkSkillDef, 0);
	int const iCriticalPowerRate = pkCaster->GetAbil(AT_C_CRITICAL_POWER);
	int iDamage = 0;

	// Target이 실제로 Damage를 입든 안입든, Target이 다시 잡히지 않도록 해야 한다.
	pkCaster->OnTargetDamaged(pkTarget->GetID());
	pkResult->SetValue(0);
	if(	(pkTarget == NULL) 
	||	(pkTarget->IsDead()))
	{
		pkResult->SetInvalid(true);
		return true;
	}

	int iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
	int const iDecDodgeAbs = pkCaster->GetAbil(AT_DEC_TARGETDODGE_ABS);
	int const iDecDodgeRate = pkCaster->GetAbil(AT_DEC_TARGETDODGE_RATE);

	// 2007-12-05 변경 후의 공식
	// Caster:HitRate - Target:Dodge => 타격성공 여부 결정
#ifdef BATTLE_DEBUG
	//INFO_LOG(BM::LOG_LV9, __FL__<<L"\tHitRate["<<iHitRate<<L"] - Dodge["<<pkTarget->GetAbil(AT_C_DODGE_SUCCESS_VALUE)<<L"], Rand["<<iRandValue<<L"]");
#endif
	
	int const iHitSuccessRate = PgSkillHelpFunc::GetCalcHitSuccessRate(pkCaster, pkTarget, pkSkillDef, iDecDodgeAbs, iDecDodgeRate);
#ifdef BATTLE_DEBUG
	//INFO_LOG(BM::LOG_LV9, _T("iDecHitRate =") << iDecHitRate);
#endif
	if( iHitSuccessRate <= iRandValue )
	{
		// 타격 실패 (Missed)
		pkResult->SetMissed(true);
#ifdef BATTLE_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"--Missed");
#endif
		return true;
	}

		
	//기본적인 공격자의 공격력 + 스킬 공격력
	iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
	// 최종공격력(물리) = 기본공격력 + 아이템공격력 + 물리후방공격력
	// 최종공격력(마법) = (기본공격력 + 아이템공격력) * 1.5
	int const iAddDmg = (bPhysicSkill ? (pkCaster->GetAbil(AT_C_ABS_ADDED_DMG_PHY)) : (pkCaster->GetAbil(AT_C_ABS_ADDED_DMG_MAGIC)));
	
	iDamage = int((iMin + iRandValue % __max(1, iMax-iMin)) * ((bPhysicSkill) ? 1.0 : 1.5)) + iAddDmg;
	iDamage += iSkillPower;
	iDamage += iAddPower;
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"AttackPower["<<iDamage<<L"] = AttackPower + 2ndPower["<<iAddDmg<<L"]");
#endif

	// 여기가 위에 함수와 다른 부분 /////////////////////////
	iDamage = iDamage * iDmgRate / ABILITY_RATE_VALUE + iDmg;
	/////////////////////////////////////////////////////////

	int iExtraDamageRate = pkCaster->GetAbil(AT_1ST_ATTACK_ADDED_RATE);
	if (iExtraDamageRate != 0)
	{
		iDamage = iDamage + iDamage * iExtraDamageRate / ABILITY_RATE_VALUE;

		// 1방에만 적용하는 기능을 여러대 적용하는 어빌이
		int const iApplyNum = pkCaster->GetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM);
		if(1 < iApplyNum)
		{// 충분하게 존재하면 횟수만 감소 시키고
			pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, iApplyNum-1);
		}
		else
		{// 이번이 마지막 적용이면 0으로 횟수와, 증가값을 초기화 시킨다 
			pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, 0);
			pkCaster->SetAbil(AT_1ST_ATTACK_ADDED_RATE, 0);
		}
	}
	iDamage = CS_CheckDamage(pkCaster, pkTarget, iDamage, bPhysicSkill, pkResult, iSkillPRate, pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE), pkSkillDef);

	iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
	if (iRandValue < iCriticalRate)
	{
		__int64 i64Damage = iDamage;
		i64Damage = i64Damage * static_cast<int>( PgSkillHelpFunc::AdjustMaxValue(AT_CRITICAL_POWER, static_cast<float>(iCriticalPowerRate), 0, 0) );
		i64Damage /= ABILITY_RATE_VALUE;
		iDamage = static_cast<int>(i64Damage);
		pkResult->SetCritical(true);
		iDamage = CS_CheckItemDamage( pkCaster, pkTarget, true, iDamage );
	}
	else
	{
		iDamage = CS_CheckItemDamage( pkCaster, pkTarget, false, iDamage );
	}
	
	// 반사 데미지 계산 및 ActionResult에 설정
	PgSkillHelpFunc::CalcReflectDamage(pkCaster, pkTarget, iDamage, pkResult, pkSkillDef, bPhysicSkill, false, false);

	AddDamageEffectAndChangeBlockValue(pkCaster, pkTarget, pkSkillDef, pkResult);

	if(0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)) //데미지를 입을 수 있는 상태일때만 세팅
	{
		pkResult->SetValue(iDamage);
	}
	return true;
}

//어떤 Abil에 따라 Damage를 조정해 주어야 할 필요가 있다.
__int64 CS_CheckDamage(CUnit* pkCaster, CUnit* pkTarget, __int64 i64Damage, bool bPhysical, PgActionResult* pkResult, int const iSkillRate, bool const bProjectileDmg, CSkillDef const* pkSkillDef)
{
	if(pkTarget && pkTarget->GetAbil(AT_DAMAGE_IS_ONE))
	{
		return pkCaster ? 1+pkCaster->GetAbil(AT_DAMAGE_IS_ONE_ALPHA) : 1;
	}

	int iDecPer = 0;
	int iDefence = 0;
	int iDefenceIgnoreRate = 0;
	
	int iCasterLevel = 0;
	EUnitType eCasterType = UT_NONETYPE;
	GetCasterLevelAndType(pkCaster, iCasterLevel, eCasterType);

	if (bPhysical)
	{
		// 최종물리방어력 = (기본 물리 방어력+방어구의 방어력 합)*0.4
		//iDefence = (int) pkTarget->GetAbil(AT_C_PHY_DEFENCE) * (1 + pkTarget->GetAbil(AT_PHY_DEFENCE)/1000.0);
		iDefence = static_cast<int>(pkTarget->GetAbil(AT_C_PHY_DEFENCE) * 0.4f);
		iDecPer = pkTarget->GetAbil(AT_C_PHY_DMG_DEC);
		iDefenceIgnoreRate = pkCaster->GetAbil(AT_PHY_DEFENCE_IGNORE_RATE);
	}
	else
	{
		iDefence = static_cast<int>(pkTarget->GetAbil(AT_C_MAGIC_DEFENCE) * 0.5f);
		iDecPer = pkTarget->GetAbil(AT_C_MAGIC_DMG_DEC);
		iDefenceIgnoreRate = pkCaster->GetAbil(AT_MAGIC_DEFENCE_IGNORE_RATE);
	}
	if(pkTarget 
		&& pkCaster
		)
	{// 방어 무시(물방, 마방)
		iDefenceIgnoreRate += PgSkillHelpFunc::AdjustMaxValue(AT_C_DEFENCE_IGNORE_RATE ,pkCaster->GetAbil(AT_C_DEFENCE_IGNORE_RATE), iCasterLevel, pkTarget->GetAbil(AT_LEVEL) );
	}

	//방어 무시 확률이 있을 경우
	if(0 < iDefenceIgnoreRate)
	{
		//{ BM::vstring vStr("방어무시확률: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
		if(iRandValue < iDefenceIgnoreRate)
		{
			iDecPer = 0;
			iDefence = 0;
			if(pkResult)
			{
				pkResult->SetDefenceIgnore(true);
			}
		}
	}

	// 방어력으로 인한 Damage감소
#ifdef BATTLE_FORMULA_081120_ICE
	__int64 i64DecPerDamage = static_cast<int>(i64Damage * (static_cast<float>(iDecPer) / ABILITY_RATE_VALUE_FLOAT));
	// 감소하는 데미지가 음수 이면 모든 데미지가 감소한 것이다.
	if(0 > i64DecPerDamage)
	{
		i64Damage = 1;
	}
	else
	{
		i64Damage -= i64DecPerDamage;
	}

	if (UT_PLAYER == pkTarget->UnitType())
	{
		int iCasterLevel = 0;
		EUnitType eCasterType = UT_NONETYPE;
		GetCasterLevelAndType(pkCaster, iCasterLevel, eCasterType);

		// Player 일때만 적용
		float const fRate1 = (eCasterType == UT_PLAYER) ? (0.5f) : (12.0f);
		float const fRate2 = (eCasterType == UT_PLAYER) ? (1.0f) : (1.2f);
		int const iBaseValue = (eCasterType == UT_PLAYER) ? (100) : (10);
		//int const iTargetLevel = pkTarget->GetAbil(AT_LEVEL);
		
		// Player 방어력이 0보다 작다면 0으로 두고 계산한다. (그렇지 않다면 아래 공식으로는 데미지 1이 나온다)
		int const iTempDefence = iDefence < 0 ? 0 : iDefence;
		float fTempReductionRate = (iTempDefence - iCasterLevel*fRate2) / (iTempDefence - iCasterLevel * fRate2 + iBaseValue + fRate1 * iCasterLevel);
		fTempReductionRate = __max(0.0f, fTempReductionRate); //데미지 감쇄율이 0% 미만이 될 수 없다.
		fTempReductionRate = __min(1.0f, fTempReductionRate); //데미지 감쇄율이 100%를 초과 할 수 없다.
		float const fReductionRate = fTempReductionRate;
		i64Damage = static_cast<__int64>(i64Damage * (1.0f - fReductionRate));
	}
	else
	{
		i64Damage -= iDefence;
	}

	i64Damage = __max(1, i64Damage);

	if(1 == i64Damage)
	{
		// 데미지가 최소치가 되었을 경우, 한번 데미지로 연타로 나누어 연출하는 스킬의 경우 데미지를 나누는만큼 증가시켜야 한다.
		if(pkSkillDef)
		{
			int const iComBoHit = pkSkillDef->GetAbil(AT_COMBO_HIT_COUNT);
			if(0 < iComBoHit)
			{
				i64Damage *= iComBoHit;
			}
		}
	}
#else
	i64Damage = i64Damage - i64Damage * iDecPer / ABILITY_RATE_VALUE - iDefence;
#endif
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"i64Damage["<<i64Damage<<L"] = BeforeDmg - BeforeDmg * ["<<iDecPer<<L"] - ["<<iDefence<<L"]");
#endif
	//GET_DEF(PgDefPropertyMgr, kPropertyMgr);
	//float const fPropertyRate = static_cast<float>(kPropertyMgr.GetRate(pkCaster->GetAbil(AT_OFFENSE_PROPERTY),pkCaster->GetAbil(AT_OFFENSE_PROPERTY_LEVEL), pkTarget->GetAbil(AT_DEFENCE_PROPERTY),pkTarget->GetAbil(AT_DEFENCE_PROPERTY_LEVEL)) / ABILITY_RATE_VALUE);
	//i64Damage = static_cast<__int64>(i64Damage * fPropertyRate);
#ifdef BATTLE_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("i64Damage[%d] = Damage * fPropertyRate[%.4f]"), i64Damage, fPropertyRate);
#endif
	i64Damage = static_cast<int>(i64Damage * static_cast<float>(iSkillRate) / ABILITY_RATE_VALUE_FLOAT);
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Damage["<<i64Damage<<L"] = Damage * SkillPRate["<<iSkillRate<<L"]");
#endif

	i64Damage += static_cast<__int64>(i64Damage * static_cast<float>(pkCaster->GetAbil(AT_SKILL_EFFICIENCY)) / ABILITY_RATE_VALUE_FLOAT);

	// 이펙트에 의해서 추가되는 절대 데미지
	int const iAddDmg2 = (bPhysical ? (pkCaster->GetAbil(AT_ABS_ADDED_DMG_PHY_2)) : (pkCaster->GetAbil(AT_ABS_ADDED_DMG_MAGIC_2)));
	i64Damage += iAddDmg2;

	// 절대 데미지 감소
	int const iSubtractDmg = (bPhysical ? (pkTarget->GetAbil(AT_PHY_DMG_SUBTRACT)) : (pkTarget->GetAbil(AT_MAGIC_DMG_SUBTRACT)));
	i64Damage -= iSubtractDmg;

	// 발사체 데미지 감소가 있을 경우
	if(bProjectileDmg)
	{
		int const iSubtractProjectileDmg =  pkTarget->GetAbil(AT_PROJECTILE_DMG_SUBTRACT);
		i64Damage -= iSubtractProjectileDmg;
	};

	//Plyer일 때는 그레이드에 따라 데미지 증폭은 적용되지 않아야 함.
	if (UT_PLAYER != pkTarget->UnitType())
	{
		// 몬스터 타격시 그레이드에 따라 데미지 증가 어빌이 있을 경우
		int iGradeAddDmgRate = 0;
		switch(pkTarget->GetAbil(AT_GRADE))
		{
		case EMGRADE_NORMAL:
			{
				iGradeAddDmgRate = pkCaster->GetAbil(AT_GRADE_NORMAL_DMG_ADD_RATE);
			}break;
		case EMGRADE_UPGRADED:
			{
				iGradeAddDmgRate = pkCaster->GetAbil(AT_GRADE_UPGRADED_DMG_ADD_RATE);
			}break;
		case EMGRADE_ELITE:
			{
				iGradeAddDmgRate = pkCaster->GetAbil(AT_GRADE_ELITE_DMG_ADD_RATE);
			}break;
		case EMGRADE_BOSS:
			{
				iGradeAddDmgRate = pkCaster->GetAbil(AT_GRADE_BOSS_DMG_ADD_RATE);
			}break;
		default:
			{
				iGradeAddDmgRate = 0;
			}break;
		}

		if(0 < iGradeAddDmgRate)
		{
			i64Damage += static_cast<int>(i64Damage * static_cast<float>(iGradeAddDmgRate) / ABILITY_RATE_VALUE_FLOAT);
		}
	}

	//Caster가 Monster일 때만 적용
	if (UT_MONSTER == pkCaster->UnitType())
	{
		// 몬스터 타격시 그레이드에 따라 데미지 감소 어빌이 있을 경우
		int iGradeAddDmgRate = 0;

		//물리 데미지 감소
		if(bPhysical)
		{
			switch(pkCaster->GetAbil(AT_GRADE))
			{
			case EMGRADE_NORMAL:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_NORMAL_PHY_DMG_DEC_RATE);
				}break;
			case EMGRADE_UPGRADED:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_UPGRADED_PHY_DMG_DEC_RATE);
				}break;
			case EMGRADE_ELITE:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_ELITE_PHY_DMG_DEC_RATE);
				}break;
			case EMGRADE_BOSS:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_BOSS_PHY_DMG_DEC_RATE);
				}break;
			default:
				{
					iGradeAddDmgRate = 0;
				}break;
			}

			if(0 < iGradeAddDmgRate)
			{
				i64Damage -= static_cast<int>(i64Damage * static_cast<float>(iGradeAddDmgRate) / ABILITY_RATE_VALUE_FLOAT);
			}
		}
		// 마법 데미지 감소
		else
		{
			switch(pkCaster->GetAbil(AT_GRADE))
			{
			case EMGRADE_NORMAL:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_NORMAL_MAGIC_DMG_DEC_RATE);
				}break;
			case EMGRADE_UPGRADED:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_UPGRADED_MAGIC_DMG_DEC_RATE);
				}break;
			case EMGRADE_ELITE:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_ELITE_MAGIC_DMG_DEC_RATE);
				}break;
			case EMGRADE_BOSS:
				{
					iGradeAddDmgRate = pkTarget->GetAbil(AT_GRADE_BOSS_MAGIC_DMG_DEC_RATE);
				}break;
			default:
				{
					iGradeAddDmgRate = 0;
				}break;
			}

			if(0 < iGradeAddDmgRate)
			{
				i64Damage -= static_cast<int>(i64Damage * static_cast<float>(iGradeAddDmgRate) / ABILITY_RATE_VALUE_FLOAT);
			}
		}
	}

	i64Damage = __max(1, i64Damage);

	if(pkTarget->GetAbil(AT_INVINCIBLE)) // 무적 어빌이 적용되어 있다면
	{
		i64Damage = 0;
	}

	if (i64Damage > 0)
	{
		int iBarrierCount = pkTarget->GetAbil(AT_BARRIER_100PERECNT_COUNT);
		int iBarrerAmount = pkTarget->GetAbil(AT_BARRIER_AMOUNT);

		// 방어 스킬 우선 순위 CANNOT_DAMAGE > AT_BARRIER_100PERECNT_COUNT > AT_BARRIER_AMOUNT > AT_DMG_CONSUME_MP
		//100% 데미지 흡수 배리어가 발동 순서가 우선순위 이다.

		if(iBarrierCount)
		{
			int iBarrier1 = pkTarget->GetAbil(AT_BARRIER_100PERECNT_COUNT);
			int iBarrier2 = pkTarget->GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT);
			int iValue = iBarrier1 - iBarrier2;

			pkTarget->GetEffectMgr().AddAbil(AT_BARRIER_100PERECNT_COUNT, -1);
			pkTarget->NftChangedAbil(AT_BARRIER_100PERECNT_COUNT, E_SENDTYPE_EFFECTABIL);

			pkTarget->GetEffectMgr().AddAbil(AT_PREV_BARRIER_100PERECNT_COUNT, iValue);
			pkTarget->NftChangedAbil(AT_PREV_BARRIER_100PERECNT_COUNT, E_SENDTYPE_EFFECTABIL);

			i64Damage = 0;
		}
		//데미지 흡수 배리어가 발동한다.
		else if(iBarrerAmount)
		{
			__int64 i64Amount = iBarrerAmount - i64Damage;

			if(i64Amount > 0)
			{
				int const iCutedDamage = static_cast<int>( std::max(static_cast<__int64>(INT_MIN), -i64Damage) ); // iCutedDamage가 음수임에 유의할 것

				// 데미지만큼 배리어를 감소 하고 데미지는 0으로 한다.
				pkTarget->GetEffectMgr().AddAbil( AT_BARRIER_AMOUNT, iCutedDamage );
				pkTarget->NftChangedAbil(AT_BARRIER_AMOUNT, E_SENDTYPE_EFFECTABIL);
				if(pkResult)
				{
					pkResult->SetAbsorbValue(-iCutedDamage);
				}
				i64Damage = 0;
			}
			else
			{
				// 배리어를 0으로 하고 남은 만큼을 데미지로 한다.
				pkTarget->GetEffectMgr().AddAbil(AT_BARRIER_AMOUNT, -iBarrerAmount);
				pkTarget->NftChangedAbil(AT_BARRIER_AMOUNT, E_SENDTYPE_EFFECTABIL);
				if(pkResult)
				{
					pkResult->SetAbsorbValue(iBarrerAmount);
				}
				i64Damage = -i64Amount;
			}
		}
		else
		{	
			// 마나쉴드 같이 Damage를 다시 조절해 주어야 할 스킬
			int iMPPer = pkTarget->GetAbil(AT_DMG_CONSUME_MP);
			__int64 i64DmgDec = 0;
			if (bPhysical)
			{
				i64DmgDec = i64Damage * pkTarget->GetAbil(AT_PDMG_DEC_RATE_2ND) / ABILITY_RATE_VALUE;
			}
			else
			{
				i64DmgDec = i64Damage * pkTarget->GetAbil(AT_MDMG_DEC_RATE_2ND) / ABILITY_RATE_VALUE;
			}
			if (iMPPer > 0)
			{
				int iCount = pkTarget->GetAbil(AT_DMG_DEC_RATE_COUNT);
				if (iCount > 0)
				{
					pkTarget->SetAbil(AT_DMG_DEC_RATE_COUNT, iCount-1);
				}
				if (iCount != 0)	// >0 이면 개수를 줄여주는 의미, -1 이면 회수는 unlimited
				{
					__int64 i64MPNeed = i64DmgDec / iMPPer;
					__int64 i64MP = pkTarget->GetAbil(AT_MP);
					if (i64MP < i64MPNeed)
					{
						pkTarget->SetAbil(AT_MP, 0, true);
						i64DmgDec = i64MP * iMPPer;
					}
					else
					{
						pkTarget->SetAbil(AT_MP, static_cast<int>(i64MP - i64MPNeed), true);
					}
				}
			}
			i64Damage = __max(1, i64Damage - i64DmgDec);
		}
	}
	if(0 < pkTarget->GetAbil(AT_AWAKE_CHARGE_STATE))
	{// 각성 챠지 스킬을 사용중이고
		if(0 < i64Damage)
		{// 데미지가 있다면
			pkTarget->SetAbil(AT_IS_DAMAGED_DURING_AWAKE_CHARGE, 1);
		}
	}

	//최종 데미지 확률을 적용
	if(int const iAddRate = pkCaster->GetAbil(AT_LAST_ATTACK_DAMAGE_RATE))
	{
		i64Damage += SRateControl::GetValueRate<int>( i64Damage, iAddRate );
	}

	// 암살자 : 베놈크래셔 스킬은 최종Damage에서 추가적인 Dmg를 더해 주어야 한다.
	//i64Damage = i64Damage + pkCaster->GetAbil(AT_ABS_ADDED_DMG);
	return i64Damage;
}

int CS_GetReflectDamage(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, bool bPhysicSkill)
{	
	int iResult = 0;
	{//스킬로 인한
		int const iHitRate = pkTarget->GetAbil(AT_C_DMG_REFLECT_HITRATE);
		if (0 < iHitRate)
		{// 반사가 일어날 확률이 존재하고
			int const iValue = int(pkCaster->GetRandom() % ABILITY_RATE_VALUE);
			if(iHitRate > iValue)
			{// 발동 한다면
				int const iReflectRate = pkTarget->GetAbil(AT_C_DMG_REFLECT_RATE);
				if (0 < iReflectRate)
				{// 받은 데미지의 %만큼 반사하는 값을 계산해 적용해주고
					iResult = iDamage * iReflectRate / ABILITY_RATE_VALUE;
				}
			}
		}
	}

	{// 아이템으로 인한
		iResult += CS_GetReflectDamage_FromItem(pkCaster, pkTarget, iDamage, bPhysicSkill);
	}	
	
	if(0 < iResult )
	{// 카오스맵이라면 반사 데미지도 1이 될수 있는 체크를 해야한다.
		iResult = CS_CheckDmgPer(pkTarget, iResult, bPhysicSkill);
	}

	if(	UT_PLAYER == pkCaster->UnitType() )
	{
		int const iRemainHP = pkCaster->GetAbil(AT_HP) - iResult;
		int const iMaxHP = pkCaster->GetAbil(AT_C_MAX_HP);
		int const iRemainHP_10000Per = iRemainHP*ABILITY_RATE_VALUE;
		static int _1000Per = ABILITY_RATE_VALUE / 10;
		if(_1000Per  >= iRemainHP_10000Per / iMaxHP )
		{// 이번 반사로 인해 10%(만분률이므로 1000) HP가 된다면 반사 데미지는 0으로 한다
			return 0;
		}
	}

	return iResult;
}

int CS_GetReflectDamage_FromItem(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, bool bPhysicSkill)
{	
	PgInventory* pkInv = pkTarget->GetInven();
	if(!pkInv)
	{
		return 0;
	}

	int iResult = 0;
	for(int i=AT_DMG_REFLECT_RATE_FROM_ITEM01; i <= AT_DMG_REFLECT_RATE_FROM_ITEM10; ++i)
	{
		int const iReflectRateType_FromItemAbilType = i;
		int const iReflectDmg_FromItemAbilType = i+10;
		
		int const iHitRate_FromItem = pkInv->GetAbil(iReflectRateType_FromItemAbilType);
		if(0 < iHitRate_FromItem)
		{
			int const iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
			if(iRandValue < iHitRate_FromItem)
			{// 반사 데미지 절대값이 있다면 추가적으로 더해주고
				iResult += pkInv->GetAbil(iReflectDmg_FromItemAbilType);
			}
		}
	}

	if(0 < iResult )
	{// 카오스맵이라면 반사 데미지도 1이 될수 있는 체크를 해야한다.
		iResult = CS_CheckDmgPer(pkTarget, iResult, bPhysicSkill);
	}

	return iResult;
}

// pkCaster : 최초 Caster (즉 Reflected 당해서 Damage 받을 unit)
// pkTarget : 최초 Target (즉 Reflect 효과를 가지고서 반사 데미지를 발생시킨 Unit)
// [RETURN]
//	Caster의 남은 HP값
int CS_DoReflectDamage(CUnit const* pkCaster, CUnit const* pkTarget, int const iSkillNo, int const iReflected)
{
	int const iHP = pkCaster->GetAbil(AT_HP);
	return __max(0, iHP-iReflected);
}

DWORD DifftimeGetTime(DWORD const& dwStart, DWORD const& dwEnd)
{
	return dwEnd - dwStart;//오버플로우를 고려 해도. 테스트 해본결과 이게 맞음.

/*	if( dwStart < dwEnd )
	{
		return dwEnd - dwStart;
	}
	else if( dwStart > dwEnd )//dwEnd Overflow
	{
		return (ULONG_MAX - dwStart) + dwEnd;
	}
	//else//dwStart == dwEnd
*/	return 0;
}

int SetDamageDelay(CSkillDef const *pkSkillDef, CUnit const* pkCaster, CUnit* pkTarget)
{
	if(!pkSkillDef || !pkCaster || !pkTarget)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get SkillDef");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	BM::GUID const& rkCasterGuid = (pkCaster != NULL) ? pkCaster->Caller() : BM::GUID::NullData();
	//if(0==(UT_MONSTER & pkTarget->UnitType()))
	{
		pkTarget->SetState(US_PUSHED_BY_DAMAGE);
	}
	int iDelayTime = 0;

	switch(pkSkillDef->GetAbil(AT_DMG_DELAY_TYPE))
	{
	case 1: {iDelayTime = pkSkillDef->GetAbil(AT_DAMAGED_DELAY); break;}	// Skill의 DamagedDley값
	case 2: {iDelayTime = pkSkillDef->GetAbil(AT_DAMAGED_DELAY) + pkTarget->GetAbil(AT_DAMAGED_DELAY); break;} // Skill+Unit
	case 0:
	default:
		{
			iDelayTime = pkTarget->GetAbil(AT_DAMAGED_DELAY);
			break;
		}
	}

	if(0 == iDelayTime)
	{
		// Dmg Ani 할 시간을 줘야 한다.
		iDelayTime = 450;
	}

	// Desc : 맞으면 처음 부터 캐스팅 하게 됨.
	pkTarget->SetDelay(iDelayTime);	// Monster가 피격 애니를 하는 시간(그동안 서버에서도 가만히 있어야 함)
	pkTarget->GetAI()->SetEvent(rkCasterGuid, EAI_EVENT_DAMAGED);

	return iDelayTime;
}

void CheckActionResultVec(UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector& kActionResultVec)
{
	//대신 맞을 유닛이 있는지 검사 하고 처리하는 루틴

	int iIndex = 0;

	//임시로 사용하는 구조체 이곳에서만 쓰인다.
	typedef struct _tagGuidValue
	{
		_tagGuidValue() : m_iValue(0)
		{}

		int			m_iValue;	// ActionResult.GetValue 
		BM::GUID	m_kGuid;	// DeliverDamageUnit Guid
	} sGuidValue;

	std::list<sGuidValue> kGuidList; 
	kGuidList.clear();

	for(UNIT_PTR_ARRAY::iterator itor = rkTargetArray.begin(); itor != rkTargetArray.end(); ++itor)
	{
		// 참고용 유닛은 리스트에서 지나친다.
		if(true == (*itor).bReference)
		{
			continue;
		}

		// 복구해야하는 유닛이면 복구 처리 한다.
		if(true == (*itor).bRestore)
		{
			CUnit* pkTarget = (*itor).pkUnit;
			if(NULL != pkTarget)
			{
				PgActionResult* pkActionResult = kActionResultVec.GetResult(pkTarget->GetID());
				if(NULL != pkActionResult)
				{
					//남은 HP는 서버에 있는 유닛의 현재 HP로 세팅한다.
					pkActionResult->Init();
					pkActionResult->SetRemainHP(pkTarget->GetAbil(AT_HP));
					pkActionResult->SetRestore(true);					
				}
			}

			continue;
		}

		BM::GUID kGuid = (*itor).pkUnit->GetDeliverDamageUnit();

		//데미지를 대신 맞는 유닛이 있다.
		if(!BM::GUID::IsNull(kGuid))
		{
			if((*itor).pkUnit->GetID() == kGuid) // 대신 맞는 유닛이 자신 일 경우
			{
				continue;
			}

			//원래의 데미지와 / 대신 맞을 유닛의 guid를 저장하고 / 원래 유닛의 Damage를 0으로 한다.
			PgActionResult* pkActionResult = kActionResultVec.GetResult((*itor).pkUnit->GetID()); //원래 유닛의 ActionResult를 얻어온다.
			int iValue = 0;
			if(pkActionResult)
			{
				iValue = pkActionResult->GetValue();
				pkActionResult->SetValue(0);
				pkActionResult->AddAbil(AT_NO_DAMAGE_USE_DAMAGEACTION,1); // 데미지는 없으나 모션만 취한다.
			}

			// 데미지가 있을 경우에만 처리
			if(0 < iValue)
			{
				sGuidValue kValue;
				// 10000분률 
				kValue.m_iValue = iValue - static_cast<int>(static_cast<float>(iValue) * (static_cast<float>((*itor).pkUnit->GetAbil(AT_DELIVER_DAMAGE_BLOCK_RATE)) / ABILITY_RATE_VALUE_FLOAT)); //대신 입는 블럭 데미지 감소율을 적용
				kValue.m_kGuid = kGuid;
				//대신 맞는 경우 데미지가 있을 시에만 저장한다.
				kGuidList.push_back(kValue);
			}
		}
	}

	if(!kGuidList.empty())
	{
		// 실제로 데미지를 입는 유닛은 따로 있다.
		for(std::list<sGuidValue>::iterator itor2 = kGuidList.begin(); itor2 != kGuidList.end(); ++itor2)
		{
			PgActionResult* pkActionResult = kActionResultVec.GetResult((*itor2).m_kGuid, true);

			if(pkActionResult)
			{
				pkActionResult->SetCollision(true);
				pkActionResult->SetValue((*itor2).m_iValue);
				pkActionResult->AddAbil(AT_DAMAGEACTION_TYPE, E_DMGACT_TYPE_NO); // 데미지 모션을 취하지 않으면서 데미지만 입는다.
			}
		}
	}
}

int CalcDecHitRate(CUnit const* pkCaster, CUnit const* pkTarget, int const iHitRate, int const iDecDodgeAbs, int const iDecDodgeRate)
{
	int iDodge = pkTarget->GetAbil(AT_C_DODGE_SUCCESS_VALUE);
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV7, _T("Before iDodge=") << iDodge << _T(", DecDodgeAbs=") << iDecDodgeAbs << _T(", DecDodgeRate=") << iDecDodgeRate);
#endif
	iDodge = iDodge - static_cast<int>(iDecDodgeAbs + iDodge / ABILITY_RATE_VALUE_FLOAT * iDecDodgeRate);
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV7, _T("After iDodge=") << iDodge);
#endif

	int iDecHitRate = std::max(iHitRate - iDodge,3000); // 아이템으로 증가시킨 회피율은 최소 30% 이하로 내려가지 않는다.

	//{ // 인챈트 차이에 의한 명중률 변화
	//	if( 0 != pkTarget->GetAbil(AT_MON_ENCHANT_LEVEL)
	//	||	0 != pkCaster->GetAbil(AT_MON_ENCHANT_LEVEL) )
	//	{
	//		int const iDiffEnchantLevel = pkCaster->GetAbil(AT_OFFENCE_ENCHANT_LEVEL) - pkTarget->GetAbil(AT_DEFENCE_ENCHANT_LEVEL);
	//		iDecHitRate += int(MAKE_ABIL_RATE(iDiffEnchantLevel / 2.0f));
	//	}
	//}

#ifdef BATTLE_FORMULA_081120_ICE
	int iCasterLevel = 0;
	EUnitType eCasterType = UT_NONETYPE;
	GetCasterLevelAndType(pkCaster, iCasterLevel, eCasterType);
	
	EUnitType const eTargetType = pkTarget->UnitType();
	
	int const iLevelDiff = iCasterLevel - pkTarget->GetAbil(AT_LEVEL);
#ifdef BATTLE_DEBUG
	INFO_LOG(BM::LOG_LV7, _T("PLAYER->MONSTER : LevelDiff=") << iLevelDiff << _T(", HitRate=") << iHitRate);
#endif
	if (eCasterType == UT_PLAYER)
	{
		if (eTargetType == UT_MONSTER)
		{
			if (iLevelDiff < 0)
			{
				// Target_Level(Monster) > Caster_Level(Player)
				iDecHitRate -= int(MAKE_ABIL_RATE(pow(float(iLevelDiff), 2.0f) * 0.6f - 2.4f));
			}
			else
			{
				iDecHitRate += int(MAKE_ABIL_RATE(iLevelDiff / 2.0f));
			}
		}
		else
		{
			iDecHitRate += int(MAKE_ABIL_RATE(iLevelDiff / 2.0f));
		}
	}
#endif

	return iDecHitRate;
}

__int64 CS_CheckItemDamage( CUnit *pkCaster, CUnit *pkTarget, bool const bIsCritical, __int64 i64Damage )
{
	if ( 0 < i64Damage )
	{
		if ( true == bIsCritical )
		{
			i64Damage += SRateControl::Get<SRateControl::E_RATE_VALUE>( AT_CRITICAL_DAMAGE_ACTIVATE, pkCaster, i64Damage );
			i64Damage += SRateControl::Get<SRateControl::E_RATE_RATE>( AT_CRITICAL_DAMAGE_RATE_ACTIVATE, pkCaster, i64Damage );
		}
		else
		{
			int iHP = pkTarget->GetAbil( AT_HP );
			if ( iHP > i64Damage )
			{
				i64Damage += SRateControl::Get<SRateControl::E_RATE_RATE>( AT_HIT_DAMAGE_HP_RATE_ACTIVATE, pkCaster, iHP - i64Damage );
			}

			int iMP = pkTarget->GetAbil( AT_MP );
			if ( 0 < iMP )
			{
				int iRemoveMP = SRateControl::Get<SRateControl::E_RATE_RATE>( AT_HIT_DECREASE_MP_RATE_ACTIVATE, pkCaster, iMP );
				if ( 0 < iRemoveMP )
				{
					iMP = std::max( 0, iMP - iRemoveMP );
					pkTarget->SetAbil( AT_MP, iMP, true );
				}
			}
		}

		int const iCheckRate = pkTarget->GetAbil( AT_ATTACKED_DECREASE_DAMAGE_ACTIVATE_HP_RATE );
		if ( 0 < iCheckRate )
		{
			double const fMaxHP = static_cast<double>(pkTarget->GetAbil( AT_C_MAX_HP ));
			if ( 0.0 < fMaxHP )
			{
				if ( (static_cast<double>(iCheckRate) / ABILITY_RATE_VALUE_FLOAT) >= (pkTarget->GetAbil(AT_HP) / fMaxHP) )
				{
					i64Damage -= SRateControl::GetResult<SRateControl::E_RATE_RATE>( AT_ATTACKED_DECREASE_DAMAGE_RATE, pkTarget, static_cast<int>(i64Damage) );
				}
			}
		}
	}
	return i64Damage;
}

__int64 CS_CheckDmgPer(CUnit* pkTarget, __int64 const i64Damage, bool bPhysicSkill)
{
	if(0 >= i64Damage)
	{
		return 0;
	}

	__int64 i64NewDamage = i64Damage;

	bool bChosMap = false; // 데미지가 1이 되는 카오스 맵인가?
	int iDmgPer = 0;

	if(bPhysicSkill)
	{
		iDmgPer = pkTarget->GetAbil(AT_PHY_DMG_PER);
		if(0 > iDmgPer)
		{
			bChosMap = true;				
		}
	}
	else
	{
		iDmgPer = pkTarget->GetAbil(AT_MAGIC_DMG_PER);
		if(0 > iDmgPer)
		{
			bChosMap = true;
		}
	}

	if(bChosMap)
	{
		i64NewDamage = static_cast<int>(i64NewDamage * static_cast<float>(iDmgPer) / ABILITY_RATE_VALUE_FLOAT);
		i64NewDamage = __max(1, i64NewDamage);
	}

	return i64NewDamage;
}

bool IsHaveSkillDamage(CSkillDef const* pkSkillDef)
{
	if(pkSkillDef)
	{
		bool bSkillDamage = false;
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_PHY_ATTACK)) ? (true) : (false);
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_PHY_DMG_PER)) ? (true) : (false);
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_I_PHY_ATTACK_ADD_RATE)) ? (true) : (false);
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_MAGIC_ATTACK)) ? (true) : (false);
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_MAGIC_DMG_PER)) ? (true) : (false);
		bSkillDamage |= (0 < pkSkillDef->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE)) ? (true) : (false);

		return bSkillDamage;
	}

	return false;
}


// 도둑 기본 공격 2타 : a_thi_melee_02
bool CS_GetSkillResult103201201(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector)
{
	if(rkTargetArray.empty())
	{
		return true;
	}

	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkCaster, rkTargetArray, pkResultVector);
	// 도둑 2/4타 공격에 있어서 Double Attack이 될 가능성이 있다.
	int const iRate = pkCaster->GetAbil(AT_CLAW_DBL_ATTK_RATE);
	UNIT_PTR_ARRAY::const_iterator itor = rkTargetArray.begin();
	while (itor != rkTargetArray.end())
	{
		//참고용 일때
		if(true == (*itor).bReference)
		{
			//나중에 처리
			++itor;
			continue;
		}

		CUnit* pkTarget = itor->pkUnit;
		PgActionResult* pkAResult = pkResultVector->GetResult(pkTarget->GetID(), true);
		if( NULL == pkAResult )
		{
			++itor;
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"ActionResult is NULL Unit Guid["<<itor->pkUnit->GetID()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkAResult is NULL"));
			continue;
		}

		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed())
		{
			float fDamageRate = 1.0f;
			int iValue = int(pkCaster->GetRandom() % ABILITY_RATE_VALUE);
			
			if(iRate > iValue)
			{
				pkAResult->SetAbil(AT_HIT_COUNT, 2);
				int const iDmgRate = pkCaster->GetAbil(AT_CLAW_DBL_ATTK_DMG_RATE);
				fDamageRate = (iDmgRate * 2) / ABILITY_RATE_VALUE_FLOAT;

				//더블 어택이 동작할 경우에만 데미지를 계산해야한다.
				bReturn = CS_GetDmgResult(iSkillNo, pkCaster, pkTarget, (int)(ABILITY_RATE_VALUE_FLOAT * fDamageRate), 0, pkAResult);
			}			
		}
		++itor;
	}

	CheckActionResultVec(rkTargetArray, *pkResultVector);

	return bReturn;
}

//투사 룰렛
bool CS_GetSkillResult106000101(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector)
{
	if(rkTargetArray.empty())
	{
		return true;
	}

	//룰렛
	int iRand = pkCaster->GetRandom() % 12000;//	-- 1에서 6사이의 값 랜덤 얻기
	if(10000 < iRand)
	{
		iRand = 0;
	}
	else if(8000 < iRand)
	{
		iRand = 1;
	}
	else if(6000 < iRand)
	{
		iRand = 2;
	}
	else if(4000 < iRand)
	{
		iRand = 3;
	}
	else if(2000 < iRand)
	{
		iRand = 4;
	}
	else
	{
		iRand = 5;
	}
		
	UNIT_PTR_ARRAY::iterator unit_itor = rkTargetArray.begin();
	while(rkTargetArray.end() != unit_itor)
	{
		//참고용 일때
		if(true == (*unit_itor).bReference)
		{
			//나중에 처리
			++unit_itor;
			continue;
		}

		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResultVector->GetResult(pkTarget->GetID(), true);
		pkAResult->SetAbil(AT_DICE_VALUE, iRand);
		
		++unit_itor;
	}

	CheckActionResultVec(rkTargetArray, *pkResultVector);

	return true;
}

// 전사 조인트 브레이크
bool CS_GetSkillResult105300501(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector)
{
	if(rkTargetArray.empty())
	{
		return true;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (pkSkillDef == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get SkillDef, SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bSkillDamage = IsHaveSkillDamage(pkSkillDef);
	if(bSkillDamage)
	{
		return CS_GetSkillResultDefault(iSkillNo, pkCaster, rkTargetArray, pkResultVector);		
	}
	
	UNIT_PTR_ARRAY::const_iterator itor = rkTargetArray.begin();
	while (itor != rkTargetArray.end())
	{
		//참고용 일때
		if(true == (*itor).bReference)
		{
			//나중에 처리
			++itor;
			continue;
		}

		CUnit* pkTarget = itor->pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkResult = pkResultVector->GetResult(pkTarget->GetID(), true);
			if(!pkResult)
			{				
				++itor;
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"ActionResult is NULL Unit Guid["<<itor->pkUnit->GetID()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkResult is NULL"));
				continue;
			}
			
			int iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;
			int const iDecDodgeAbs = pkCaster->GetAbil(AT_DEC_TARGETDODGE_ABS);
			int const iDecDodgeRate = pkCaster->GetAbil(AT_DEC_TARGETDODGE_RATE);
			
			int const iHitSuccessRate = PgSkillHelpFunc::GetCalcHitSuccessRate(pkCaster, pkTarget, pkSkillDef, iDecDodgeAbs, iDecDodgeRate);
			if( iHitSuccessRate <= iRandValue )
			{
				// 타격 실패 (Missed)
				pkResult->SetMissed(true);
				++itor;
				continue;
			}

			AddDamageEffectAndChangeBlockValue(pkCaster, pkTarget, pkSkillDef, pkResult);
		}
		++itor;
	}

	CheckActionResultVec(rkTargetArray, *pkResultVector);

	return true;
}

//트랩퍼 MP제로 폭발
bool CS_GetSkillResult1100028011(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector)
{
	if(rkTargetArray.empty())
	{
		return true;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (pkSkillDef == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Cannot Get SkillDef, SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	UNIT_PTR_ARRAY::const_iterator itor = rkTargetArray.begin();
	while (itor != rkTargetArray.end())
	{
		CUnit* pkTarget = itor->pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkResult = pkResultVector->GetResult(pkTarget->GetID(), true);
			if(!pkResult)
			{
				++itor;
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"ActionResult is NULL Unit Guid["<<itor->pkUnit->GetID()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkResult is NULL"));
				continue;
			}

			int const iMP = pkTarget->GetAbil(AT_C_MAX_MP);
			int iValue = (int)((float)iMP * (float)pkSkillDef->GetAbil(AT_PERCENTAGE) / ABILITY_RATE_VALUE);
			iValue = __max(0, iValue);
			pkResult->SetValue(iValue / 2);			
		}
		++itor;
	}

	CheckActionResultVec(rkTargetArray, *pkResultVector);

	return true;
}

namespace PgNpcTalkUtil
{
	float const fNPC_TALK_ENABLE_RANGE = 200.f;
}

bool GetItemName(int const iItemNo, std::wstring & rkOut)
{
	CONT_DEFITEM const* pkDefItem = NULL;
	g_kTblDataMgr.GetContDef(pkDefItem);
	if( !pkDefItem )
	{
		return false;
	}

	CONT_DEFITEM::const_iterator def_item = pkDefItem->find( iItemNo );
	if( pkDefItem->end() == def_item )
	{
		return false;
	}

	CONT_DEFSTRINGS const* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);

	CONT_DEFSTRINGS::const_iterator str_itor = pContDefStrings->find( (def_item->second).NameNo );
	if(str_itor == pContDefStrings->end())
	{
		return false;
	}

	rkOut = (*str_itor).second.strText;
	return true;
}

namespace PgHometownPortalUtil
{
	__int64 UsePortalCost(int const iLevel)
	{
		CONT_HOMETOWNTOMAPCOST const * pkCont = NULL;
		g_kTblDataMgr.GetContDef(pkCont);
		if(NULL == pkCont)
		{
			return 0;
		}

		CONT_HOMETOWNTOMAPCOST::const_iterator iter = pkCont->find(iLevel);
		if(iter == pkCont->end())
		{
			return 0;
		}

		return static_cast<__int64>((*iter).second); // 고정 금액으로 수정
	}
}

namespace PgMyHomeFuncRate
{
	int GetHomeFuncItemValue(eMyHomeSideJob const kSideJob, eMyHomeSideJobRateType const kJobRateType, CUnit * pkUnit)
	{
		PgInventory * pkInv = pkUnit->GetInven();
		ContHaveItemNoCount kContHaveItem;
		GET_DEF(CItemDefMgr, kItemDefMgr);

		pkInv->GetItems(IT_HOME, UICT_HOME_SIDEJOB_NPC, kContHaveItem);

		for(ContHaveItemNoCount::const_iterator iter = kContHaveItem.begin();iter != kContHaveItem.end();++iter)
		{
			CItemDef const * pkItem = kItemDefMgr.GetDef((*iter).first);
			if(!pkItem || (pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1) != kSideJob) || (pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_2) != kJobRateType))
			{
				continue;
			}
			return pkItem->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_3);
		}

		return 0;
	}

	bool GetSideJobInfo(eMyHomeSideJob const kSideJob, TBL_DEFSIDEJOBRATE & kJobInfo)
	{
		CONT_DEFSIDEJOBRATE const *pkContDef = NULL;
		g_kTblDataMgr.GetContDef(pkContDef);

		if(!pkContDef)
		{
			return false;
		}

		CONT_DEFSIDEJOBRATE::const_iterator iter = pkContDef->find(kSideJob);
		if(iter == pkContDef->end())
		{
			return false;
		}

		kJobInfo = (*iter).second;

		return true;
	}

	double GetCostRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit)
	{
		double dValue = 1.0;

		if(!pkUnit)
		{
			return dValue;
		}

		int iFuncValue = GetHomeFuncItemValue(kSideJob, MSJRT_GOLD, pkUnit);

		if(0 >= iFuncValue)
		{
			TBL_DEFSIDEJOBRATE kJobInfo;
			if(false == GetSideJobInfo(kSideJob, kJobInfo))
			{
				return dValue;
			}
			iFuncValue = kJobInfo.iCostRate;
		}

		if(0 >= iFuncValue)
		{
			return dValue;
		}

		dValue = 1.0 - (static_cast<double>(iFuncValue)/static_cast<double>(ABILITY_RATE_VALUE)) * pkUnit->GetInven()->GetHomeEquipItemCount();

		dValue = std::max(0.0,dValue); // 혹시라도 100% 이상 넘으면 - 값이 되는것은 막아주자

		return dValue;
	}

	double GetSoulRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit)
	{
		double dValue = 1.0;

		if(!pkUnit)
		{
			return dValue;
		}

		int iFuncValue = GetHomeFuncItemValue(kSideJob, MSJRT_SOUL, pkUnit);

		if(0 >= iFuncValue)
		{
			TBL_DEFSIDEJOBRATE kJobInfo;
			if(false == GetSideJobInfo(kSideJob, kJobInfo))
			{
				return dValue;
			}
			iFuncValue = kJobInfo.iCostRate;
		}

		if(0 >= iFuncValue)
		{
			return dValue;
		}

		dValue = 1.0 - (static_cast<double>(iFuncValue)/static_cast<double>(ABILITY_RATE_VALUE)) * pkUnit->GetInven()->GetHomeEquipItemCount();

		dValue = std::max(0.0,dValue); // 혹시라도 100% 이상 넘으면 - 값이 되는것은 막아주자

		return dValue;
	}

	int GetSuccessRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit)
	{
		if(!pkUnit)
		{
			return 0;
		}

		int iFuncValue = GetHomeFuncItemValue(kSideJob, MSJRT_RATE, pkUnit);

		if(0 >= iFuncValue)
		{
			TBL_DEFSIDEJOBRATE kJobInfo;
			if(false == GetSideJobInfo(kSideJob, kJobInfo))
			{
				return 0;
			}
			iFuncValue = kJobInfo.iSuccessRate;
		}

		return iFuncValue * static_cast<int>(pkUnit->GetInven()->GetHomeEquipItemCount());
	}
}

bool CheckExistAdjustSkillNoInEffect(int const iSkillNo, CEffect* pkEffect)
{// IsApplySkill에서 사용하는 함수(해당 스킬 번호가 이펙트에 포함되는지 확인한다)
	int iTargetSkillNo=0;
	int i = AT_ADJUST_SKILL_BEGIN;
	while(i <= AT_ADJUST_SKILL_END)
	{
		iTargetSkillNo = pkEffect->GetAbil(i);
		if(iTargetSkillNo) 
		{// 현재 값이 있으면
			if(iSkillNo == iTargetSkillNo)
			{
				return true;	// 존재함
			}
			++i;
		}
		else
		{//현재 값이 없으면 10단위로 체크 한다.(1이 없으면 11, 21, 31 순으로 검색하게 함)
			i = ((i/10)+1)*10+1;
		}
	}
	return false;	// 존재 하지 않음
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class PgSkillHelpFunc ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float const PgSkillHelpFunc::MAX_DODGE_SUCCESS_RATE	= 7000.0f;
float const PgSkillHelpFunc::MAX_BLOCK_SUCCESS_RATE	= 8000.0f;
float const PgSkillHelpFunc::MAX_CRITICAL_RATE		= 4000.0f;
float const PgSkillHelpFunc::MAX_CRITICAL_POWER		= 40000.0f;
float const PgSkillHelpFunc::MAX_BLOCK_DAMAGE_RATE	= 9900.0f;
float const PgSkillHelpFunc::MAX_CRITICAL_POWER_DEC					= 7000.0f;
float const PgSkillHelpFunc::MAX_FINAL_CRITICAL_SUCCESS_RATE_DEC	= 10000.0f;
float const PgSkillHelpFunc::MAX_DEFENCE_IGNORE_RATE			= 5000.0f;
float const PgSkillHelpFunc::MAX_BLOCK_SUCCESS_RATE_DEC				= 5000.0f;
float const PgSkillHelpFunc::MAX_RESIST_DEBUFF						= 8000.0f;
float const PgSkillHelpFunc::MAX_SUCCESS_DEBUFF						= 5000.0f;
float const PgSkillHelpFunc::MAX_DEBUFF_DURATION_DEC_RATE	= 10000.0f;
float const PgSkillHelpFunc::MAX_DEBUFF_DURATION_ADD_RATE	= 5000.0f;

int PgSkillHelpFunc::iDexStatic = 0;
int PgSkillHelpFunc::iEvadeValueValueStatic = 0;
int PgSkillHelpFunc::iHitValueStatic = 0;
float PgSkillHelpFunc::iCriticalStatic = 0;
int PgSkillHelpFunc::CalcReflectDamage(CUnit* pkCaster, CUnit* pkTarget,__int64 const i64Damage, PgActionResult* pkActionResult, CSkillDef const* pkSkillDef, bool const bPhysicDmg, bool const bOffRandomSeed, bool const bCompelCalc)
{// 반사 데미지를 계산한다. (이펙트에서 주는 데미지에서도 반사 데미지를 계산 하게 됨에 따라 함수로 분리시킴)
	if(!pkCaster
		|| 0 == i64Damage
		)
	{
		return 0;
	}
	switch( pkCaster->UnitType() )
	{
	case UT_SUB_PLAYER:
		{// 보조 캐릭터 타입은 반사 데미지에 영향을 받지 않음
			return 0;
		}break;
	}
	if(pkCaster->GetAbil(AT_CANNOT_DAMAGE))
	{
		return 0;
	}

	// 데미지 절삭(int 64로 아직 맞추지 못했음)
	int const iDamage = INT_MAX < i64Damage ? INT_MAX : static_cast<int>(i64Damage);

	int iResultReflectDamage = 0;
	if(bCompelCalc 
		|| (pkSkillDef && 0 == pkSkillDef->GetAbil(AT_IGNORE_REFLECTED_DAMAGE_SKILL))
		)
	{// 강제로 계산이거나, 반사 스킬 무시 어빌이 없다면
		bool const bCurSeed = pkCaster->UseRandomSeedType();
		if(bCurSeed && bOffRandomSeed)
		{// 기존의 설정이 시드를 사용하게 되어있으면 시드를 끄고
			pkCaster->UseRandomSeedType(false);
		}

		//물리 / 마법 100% 반사 확률이 적용이 될 경우
		//{ BM::vstring vStr("100%반사확률: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int const i100PerReflectedRate = (bPhysicDmg ? (pkTarget->GetAbil(AT_PHY_DMG_100PERECNT_REFLECT_RATE)) : (pkTarget->GetAbil(AT_MAGIC_DMG_100PERECNT_REFLECT_RATE)));
		int	const iRandValue = pkCaster->GetRandom() % ABILITY_RATE_VALUE;

		// 물리 / 마법 데미지 반사할 확률
		//{ BM::vstring vStr("일반반사확률: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int const iDmgReflectRate = (bPhysicDmg ? (pkTarget->GetAbil(AT_C_PHY_REFLECT_RATE)) : (pkTarget->GetAbil(AT_C_MAGIC_REFLECT_RATE)));
		int const iRandValue2 = pkCaster->GetRandom() % ABILITY_RATE_VALUE;

		// 발사체 반사할 확률
		//{ BM::vstring vStr("발사체반사확률: ");	OutputDebugStringA(static_cast<std::string>(vStr).c_str()); }
		int const iProjectileReflectRate = pkTarget->GetAbil(AT_C_PROJECTILE_REFLECT_RATE);
		int const iRandValue3 = pkCaster->GetRandom() % ABILITY_RATE_VALUE;

		if(iRandValue < i100PerReflectedRate)
		{
			iResultReflectDamage = CS_CheckDmgPer(pkTarget, iDamage, bPhysicDmg);
			if(pkActionResult && pkSkillDef)
			{
				pkActionResult->AddAbil(AT_REFLECTED_DAMAGE, iResultReflectDamage);
				//처음이면 세팅, 이미 값이 세팅되어있으면 누적되어야 한다.
				int const iDmgHp = (pkActionResult->GetAbil(AT_REFLECT_DMG_HP) == 0) ? (CS_DoReflectDamage(pkCaster, pkTarget, pkSkillDef->No(), iResultReflectDamage)) : (__max(0, pkActionResult->GetAbil(AT_REFLECT_DMG_HP) - iResultReflectDamage));
				pkActionResult->SetAbil(AT_REFLECT_DMG_HP, iDmgHp);
			}
		}
		else if(iRandValue2 < iDmgReflectRate)
		{//물리 / 마법 반사할 확률
			// 반사할 데미지 량
			int const iDmgReflectDmg = (bPhysicDmg ? (pkTarget->GetAbil(AT_C_PHY_REFLECT_DMG_RATE)) : (pkTarget->GetAbil(AT_C_MAGIC_REFLECT_DMG_RATE)));

			iResultReflectDamage = static_cast<int>(iDamage * static_cast<float>(iDmgReflectDmg) / ABILITY_RATE_VALUE_FLOAT);

			iResultReflectDamage = CS_CheckDmgPer(pkTarget, iResultReflectDamage, bPhysicDmg);
			if(pkActionResult && pkSkillDef)
			{
				pkActionResult->AddAbil(AT_REFLECTED_DAMAGE, iResultReflectDamage);
				//처음이면 세팅, 이미 값이 세팅되어있으면 누적되어야 한다.
				int const iDmgHp = (pkActionResult->GetAbil(AT_REFLECT_DMG_HP) == 0) ? (CS_DoReflectDamage(pkCaster, pkTarget, pkSkillDef->No(), iResultReflectDamage)) : (__max(0, pkActionResult->GetAbil(AT_REFLECT_DMG_HP) - iResultReflectDamage));
				pkActionResult->SetAbil(AT_REFLECT_DMG_HP, iDmgHp);
			}
		}
		else if(pkSkillDef
			&& pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE)
			)
		{// 발사체 반사할 확률
			if(iRandValue3 < iProjectileReflectRate )
			{// 스킬로 인한 발사체 반사 확률
				int const iProjectileReflectDmg = pkTarget->GetAbil(AT_C_PROJECTILE_REFLECT_DMG_RATE);
				iResultReflectDamage = static_cast<int>(iDamage * static_cast<float>(iProjectileReflectDmg) / ABILITY_RATE_VALUE_FLOAT);
			}

			{// 아이템으로 인한 발사체의 반사확률(근접 공격도 같은 어빌로 CS_GetReflectDamage에서 계산함)
				iResultReflectDamage += CS_GetReflectDamage_FromItem(pkCaster, pkTarget, iDamage, bPhysicDmg);
			}

			iResultReflectDamage = CS_CheckDmgPer(pkTarget, iResultReflectDamage, bPhysicDmg);
			if(0 < iResultReflectDamage
				&& pkActionResult
				&& pkSkillDef
				)
			{
				pkActionResult->AddAbil(AT_REFLECTED_DAMAGE, iResultReflectDamage);
				//처음이면 세팅, 이미 값이 세팅되어있으면 누적되어야 한다.
				int const iDmgHp = (pkActionResult->GetAbil(AT_REFLECT_DMG_HP) == 0) ? (CS_DoReflectDamage(pkCaster, pkTarget, pkSkillDef->No(), iResultReflectDamage)) : (__max(0, pkActionResult->GetAbil(AT_REFLECT_DMG_HP) - iResultReflectDamage));
				pkActionResult->SetAbil(AT_REFLECT_DMG_HP, iDmgHp);
			}
		}
		else
		{//기본 반사가 적용될 경우
			iResultReflectDamage = CS_GetReflectDamage(pkCaster, pkTarget, iDamage, bPhysicDmg);
			if(pkActionResult && pkSkillDef)
			{
				if(!pkSkillDef->IsSkillAtt(SAT_CLIENT_CTRL_PROJECTILE)		//발사체 스킬이 아니고
					&& 0 < iResultReflectDamage								//반사 데미지가 있고
					)
				{
					pkActionResult->AddAbil(AT_REFLECTED_DAMAGE, iResultReflectDamage);

					//	Damage Reflect 값이 있다면 여기서 Damage를 주자.
					//	local iHP = DoReflectDamage(caster, target, iReflect, iSkill, actarg)
					//	aresult:SetAbil(AT_REFLECT_DMG_HP, iHP)
					//	InfoLog(9, "AT_REFLECTED_DAMAGE Reflected Damage:" .. iReflect .. ", iHP:" .. aresult:GetAbil(AT_REFLECT_DMG_HP))
					//end
					//처음이면 세팅, 이미 값이 세팅되어있으면 누적되어야 한다.
					int const iDmgHp = (pkActionResult->GetAbil(AT_REFLECT_DMG_HP) == 0) ? (CS_DoReflectDamage(pkCaster, pkTarget, pkSkillDef->No(), iResultReflectDamage)) : (__max(0, pkActionResult->GetAbil(AT_REFLECT_DMG_HP) - iResultReflectDamage));
					pkActionResult->SetAbil(AT_REFLECT_DMG_HP, iDmgHp);
				}
			}
		}

		if(bCurSeed && bOffRandomSeed)
		{// 기존의 설정이 시드를 사용하게 되어있었다면 끄고 사용했으니 다시 켜준다
			pkCaster->UseRandomSeedType(true);
		}
	}
	return iResultReflectDamage;
}

int PgSkillHelpFunc::GetCalcHitSuccessRate(CUnit* pkCaster,  CUnit* pkTarget, CSkillDef const* pkCasterSkillDef, int const iDecDodgeAbs, int const iDecDodgeRate)
{
#ifndef USE_GALAXY_FORMUL
	if (!pkCaster)
	{
		return 0;
	}
	int const iHitRate = pkCaster->GetAbil(AT_C_HIT_SUCCESS_VALUE);
	return CalcDecHitRate(pkCaster, pkTarget, iHitRate, iDecDodgeAbs, iDecDodgeRate);
#else
	int const iHitRate = PgSkillHelpFunc::CalcSuccessValueToRate(PgSkillHelpFunc::EAT_HIT, pkCaster, pkTarget, pkCasterSkillDef, 0); 
	int const iDodgeRate = PgSkillHelpFunc::CalcSuccessValueToRate( PgSkillHelpFunc::EAT_DODGE, pkCaster, pkTarget, NULL, -(iDecDodgeAbs+iDecDodgeRate) );
	int iHitSuccessRate = (iHitRate - iDodgeRate + ABILITY_RATE_VALUE);
	
	if(pkCaster && pkTarget)
	{// 레벨 보정 부분
		int iCasterLevel = 0;
		EUnitType eCasterType;
		GetCasterLevelAndType(pkCaster, iCasterLevel, eCasterType);
		int const iLevelDiff = iCasterLevel - pkTarget->GetAbil(AT_LEVEL);
		if (pkCaster->UnitType() == UT_PLAYER
			|| pkCaster->UnitType() == UT_SUB_PLAYER
			|| (pkCaster->UnitType() == UT_ENTITY && pkCaster->GetAbil(AT_CALLER_TYPE) == UT_PLAYER))
		{
			if( pkTarget->UnitType() == UT_MONSTER )
			{
				if (iLevelDiff < 0)
				{// Target_Level(Monster) > Caster_Level(Player)
					iHitSuccessRate -= int(MAKE_ABIL_RATE(pow(float(iLevelDiff), 2.0f) * 0.6f - 2.4f));
				}
				else
				{
					iHitSuccessRate += int(MAKE_ABIL_RATE(iLevelDiff / 2.0f));
				}
			}
			else
			{
				iHitSuccessRate += int(MAKE_ABIL_RATE(iLevelDiff / 2.0f));
			}
		}
	}

	int iResult = std::min(iHitSuccessRate, ABILITY_RATE_VALUE);
	iResult = std::max(iResult, 0);
	return iResult;
#endif
}

bool PgSkillHelpFunc::IsAbleToBlock(CUnit* pkBlocker, CSkillDef const* pkCasterSkillDef)
{
	if(!pkBlocker)
	{
		return false;
	}
	int const iBaseClass = pkBlocker->GetAbil(AT_BASE_CLASS);
	
	switch(iBaseClass)
	{// 블록은 
	case UCLASS_FIGHTER:	// 전사
	case UCLASS_THIEF:		// 도적
		{//만 가능 하며
		}break;
	default:
		{// 그외는 불가능하고
			return false;
		}break;
	}
	if( 0 < pkBlocker->GetAbil(AT_CAN_NOT_BLOCK_BY_ACTION)			// 방어자가 액션에 의한 블록 불가
		|| 0 < pkBlocker->GetAbil(AT_CAN_NOT_BLOCK_BY_EFFECT)		// 방어자가 이펙트에 의한 블록 불가
		|| pkCasterSkillDef->GetAbil(AT_CAN_NOT_BLOCK_BY_ACTION)	// 공격자의 공격 스킬에 블록 할수 없는 어빌이 있거나
		//|| 0 < pkBlocker->GetAbil(AT_POSTURE_STATE)		// 누워 있다면(blowup)
		)
	{
		return false; // 불가능 하고
	}
	// 모든 불가능한 조건을 체크 했다면 
	return true;	// 블록 할수 있는 상태
}
int PgSkillHelpFunc::CalcDamageAfterBlock(__int64 iOriginalDamage, int iBlockDamageRate)
{
	if(0 == iBlockDamageRate)
	{// 감소량이 없으면 그대로 반환하고
		return iOriginalDamage;
	}
	// 블록 데미지의 최대량을 체크하고
	iBlockDamageRate = PgSkillHelpFunc::AdjustMaxValue(AT_BLOCK_DAMAGE_RATE, iBlockDamageRate, 0, 0);
	// 블록된 후의 데미지를 계산해보고
	__int64 i64Result = iOriginalDamage;
	i64Result *=iBlockDamageRate;
	i64Result /=ABILITY_RATE_VALUE;
	i64Result = iOriginalDamage - i64Result;
	//int iResult = static_cast<int>( iOriginalDamage - ( (iOriginalDamage*iBlockDamageRate)/ABILITY_RATE_VALUE ) );
	// 최소 1데미지는 들어 갈수있게끔 설정한다
	i64Result = std::max(1i64, i64Result);
	return static_cast<int>(i64Result);
}

bool PgSkillHelpFunc::CanChangeToBlockActionEffect(int const iActionEffect)
{// 블록가능데미지액션(액션 이펙트) 인가?
	switch(iActionEffect)
	{
	case ACTIONEFFECT_LIGHTING_DEFAULT:
	case ACTIONEFFECT_LIGHTING_LOW:
	case ACTIONEFFECT_LIGHTING_THROW_DOWN:
	case ACTIONEFFECT_LIGHTING_FLOAT_DMG:
		{// 감전
			return false;
		}break;
	//case ACTIONEFFECT_PUSH_BACK_PUBLIC_1:
	//case ACTIONEFFECT_PUSH_BACK_PUBLIC_2:
	//case ACTIONEFFECT_PUSH_BACK:
	//case ACTIONEFFECT_PUSH_BACK1:
	//case ACTIONEFFECT_PUSH_BACK2:
	//case ACTIONEFFECT_PUSH_BACK_SHORT_1:
	//case ACTIONEFFECT_PUSH_BACK_SHORT_2:
	//case ACTIONEFFECT_PUSH_BACK_LONG_1:
	//case ACTIONEFFECT_PUSH_BACK_LONG_2:
	//case ACTIONEFFECT_PUSH_BACK_LONG1:
	//case ACTIONEFFECT_PUSH_BACK_LONG2:
	//case ACTIONEFFECT_PUSH_BACK_SHORT1:
	//case ACTIONEFFECT_PUSH_BACK_SHORT2:
		//{// 밀리기
		//}break;
	//case ACTIONEFFECT_BLOW_DOWN:
		//{// 제자리 다운
		//}break;
	//case ACTIONEFFECT_BLOW_UP_S:
	//case ACTIONEFFECT_BLOW_UP_M:
	//case ACTIONEFFECT_BLOW_UP_L:
		//{// 공중에 띄워지기
		//}break;
	}
	return true;
}

int PgSkillHelpFunc::CalcSuccessValueToRate(eAbilityType const eType, CUnit* pkCaster, CUnit* pkTarget, CSkillDef const* pkSkillDef, int const iAddExceptionalFinalSuccessRate)
{
	if(!pkCaster)
	{
		return 0;
	}

	int iSuccessValueType = 0;											// 수치값 abil타입과
	int iFinalSuccessRateType = 0;										// 률값 abil타입을 얻어오기 위해
	
	int iRegistValueType = 0;
	int iRegistRateType = 0;
	CUnit* pkFormulaTarget = pkCaster;
	switch(eType)
	{// 계산할 타입을 확인해서 
	case EAT_HIT:
		{// 명중률
#ifdef USE_GALAXY_FORMUL
			iSuccessValueType = AT_HIT_SUCCESS_VALUE;
			iFinalSuccessRateType = AT_C_FINAL_HIT_SUCCESS_RATE;
#else
			return pkCaster->GetAbil(AT_C_HIT_SUCCESS_VALUE);
#endif
			//iRegistValueType = 0;
			//iRegistRateType = 0;
		}break;
	case EAT_DODGE:
		{// 회피율
#ifdef USE_GALAXY_FORMUL
			iSuccessValueType = AT_DODGE_SUCCESS_VALUE;
			iFinalSuccessRateType = AT_C_FINAL_DODGE_SUCCESS_RATE;
#else
			return pkCaster->GetAbil(AT_C_DODGE_SUCCESS_VALUE);
#endif
			//iRegistValueType = 0;
			//iRegistRateType = 0;
		}break;
	case EAT_BLOCK:
		{// 블록율
			iSuccessValueType = AT_BLOCK_SUCCESS_VALUE;
			iFinalSuccessRateType = AT_C_FINAL_BLOCK_SUCCESS_RATE;
			
			//iRegistValueType = 0;
			iRegistRateType = AT_C_BLOCK_SUCCESS_RATE_DEC;
		}break;
	case EAT_CRITICAL:
		{// 크리티컬율
#ifdef USE_GALAXY_FORMUL
			iSuccessValueType = AT_CRITICAL_SUCCESS_VALUE;
			iFinalSuccessRateType = AT_C_FINAL_CRITICAL_SUCCESS_RATE;

			//iRegistValueType = 0;
			iRegistRateType = AT_C_FINAL_CRITICAL_SUCCESS_RATE_DEC;
#else
			return pkCaster->GetAbil(AT_C_CRITICAL_SUCCESS_VALUE);
#endif
		}break;
	default:
		{// 수치, 율 타입을 지정하고
			return 0;
		}break;
	}

	int const C_ABIL_TYPE = iSuccessValueType+2;
	int iTargetLv = 0;
	if(pkTarget)
	{
		iTargetLv = pkTarget->GetAbil(AT_LEVEL);
	}	

	// 이펙트, 장비, 캐릭터 자체에 세팅되어있는
	int iAbilValue = 0;				// 각 수치와  //ex)  AT_BLOCK_SUCCESS_VALUE -> AT_C_BLOCK_SUCCESS_VALUE
	int iAddFinalSuccessRate = 0;	// 율을 얻어와
	switch(eType)
	{// 계산할 타입을 확인해서 
	case EAT_HIT:
		{// 명중률
			iAbilValue = pkCaster->GetAbil(C_ABIL_TYPE);
			iAddFinalSuccessRate = pkCaster->GetAbil(iFinalSuccessRateType);
		}break;
	case EAT_DODGE:
		{// 회피율
			if(!pkTarget)
			{
				return 0;
			}
			iAbilValue = pkTarget->GetAbil(C_ABIL_TYPE);
			iAddFinalSuccessRate = pkTarget->GetAbil(iFinalSuccessRateType);
			pkFormulaTarget = pkTarget;
		}break;
	case EAT_BLOCK:
		{// 블록율
			if(!pkTarget)
			{
				return 0;
			}
			iAbilValue = pkTarget->GetAbil(C_ABIL_TYPE);
			iAddFinalSuccessRate = pkTarget->GetAbil(iFinalSuccessRateType);
			pkFormulaTarget = pkTarget;
		}break;
	case EAT_CRITICAL:
		{// 크리티컬율
			iAbilValue = pkCaster->GetAbil(C_ABIL_TYPE);
			iAddFinalSuccessRate = pkCaster->GetAbil(iFinalSuccessRateType);
		}break;
	}

	if(pkSkillDef)
	{// 스킬이 존재한다면 스킬에 포함되어 있는 능력치를 합산하고
		switch(eType)
		{
		case EAT_HIT:		// 명중률
		case EAT_CRITICAL:	// 크리티컬율
			{
				iAbilValue += pkSkillDef->GetAbil(iSuccessValueType);
				iAddFinalSuccessRate += pkSkillDef->GetAbil(iFinalSuccessRateType);
			}break;
		case EAT_DODGE: // 회피율
		case EAT_BLOCK: // 블록율
			{
			}break;
		}
	}

	int iCasterLv = 0;
	EUnitType eCasterType;
	GetCasterLevelAndType(pkCaster, iCasterLv, eCasterType);
	if(pkTarget
		&& 0 < iRegistRateType
		)
	{
		int iAdjustedResistRate = 0;
		switch(eType)
		{
		case EAT_BLOCK:
			{// 블록율 감소는 공격자(Caster)가 피격자(Target)에게 적용하는것이므로
				iAdjustedResistRate = PgSkillHelpFunc::AdjustMaxValue(iRegistRateType, static_cast<float>(pkCaster->GetAbil(AT_C_BLOCK_SUCCESS_RATE_DEC)), iCasterLv, iTargetLv);
			}break;
		case EAT_CRITICAL:
			{// 크리티컬율 감소는 피격자(Target)가 공격자(Caster)에게 적용하는 것이므로
				iAdjustedResistRate = PgSkillHelpFunc::AdjustMaxValue(iRegistRateType, static_cast<float>(pkTarget->GetAbil(AT_C_FINAL_CRITICAL_SUCCESS_RATE_DEC)), iCasterLv, iTargetLv);
			}break;
		}
		iAddFinalSuccessRate -= iAdjustedResistRate;							// 저항력을 적용해주고
	}

	iAddFinalSuccessRate += iAddExceptionalFinalSuccessRate;			// 예외적인 외부 값이 있으면 더해주고
	
	// 최종적으로 적용할 율값을 얻어낸다
	return CalcSuccessValueToRate( iSuccessValueType, iAbilValue, iFinalSuccessRateType, iAddFinalSuccessRate, iCasterLv, iTargetLv, pkFormulaTarget);
}

int PgSkillHelpFunc::CalcSuccessValueToRate(int const iSuccessValueType, int iAbilValue, int iFinalSuccessRateType,int iAddFinalSuccessRate, int const iCasterLv, int const iTargetLv , CUnit* pkCaster)
{
	//Max 수치 제한
	float fValue = AdjustMaxValue(iSuccessValueType , static_cast<float>(iAbilValue), iCasterLv, iTargetLv, pkCaster);
	// 수치 -> 확률
	float fRate = ConvertAbilValueToRate(iSuccessValueType, fValue, iCasterLv,iTargetLv,pkCaster);
	// 성공률 합산
	fRate += iAddFinalSuccessRate;
	// Max성공률 제한
	return static_cast<int>( AdjustMaxValue(iFinalSuccessRateType, fRate, iCasterLv, iTargetLv,pkCaster) );
}

float PgSkillHelpFunc::GetRateValue (float &fMaxElement,int const &iAbilType, CUnit* pkCaster)
{
	if( !pkCaster )
	{
		return fMaxElement;
	}

	GET_DEF(PgClassDefMgr, kClassDefMgr);
	if(UT_PLAYER == pkCaster->UnitType() || UT_ENTITY == pkCaster->UnitType() ||UT_SUB_PLAYER == pkCaster->UnitType())
	{
		int iClass = pkCaster->GetAbil(AT_CLASS);
		int iCasterLv = pkCaster->GetAbil(AT_LEVEL);

		switch(iAbilType)
		{
		case AT_MAX_EVADERATE_VALUE:
		case AT_C_DODGE_SUCCESS_VALUE:
			{
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_MAX_EVADERATE_VALUE);
			}break;
		case AT_MAX_BLOCKRATE_VALUE:
		case AT_C_BLOCK_SUCCESS_VALUE:
			{
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_MAX_BLOCKRATE_VALUE);
			}break;
		case AT_CRITICALRATE_VALUE:
		case AT_C_CRITICAL_SUCCESS_VALUE:
			{
				if( pkCaster->IsUnitType(UT_ENTITY))
				{
					if(UT_PLAYER == pkCaster->GetAbil(AT_CALLER_TYPE))
					{
						iClass = pkCaster->GetMyClass();
						iCasterLv = pkCaster->GetAbil(AT_CALLER_LEVEL);
					}
				}
				else if(pkCaster->IsUnitType(UT_SUB_PLAYER))
				{
					PgSubPlayer* pkSubPlayer = static_cast<PgSubPlayer*>( pkCaster );
					if(NULL == pkSubPlayer)
					{
						return fMaxElement;
					}
					CUnit* pkUnit = NULL;
					pkUnit = pkSubPlayer->GetCallerUnit();
					if(NULL!= pkUnit && pkUnit->IsUnitType(UT_PLAYER) )//소유자가 Entity를 소환한 사람
					{
						iClass = pkUnit->GetAbil(AT_CLASS);
						iCasterLv = pkUnit->GetAbil(AT_LEVEL);
					}	
				}
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_CRITICALRATE_VALUE);
			}break;
		case AT_CRITICAL_DAMAGE_VALUE:
			{
				if( pkCaster->IsUnitType(UT_ENTITY))
				{
					if(UT_PLAYER == pkCaster->GetAbil(AT_CALLER_TYPE))
					{
						iClass = pkCaster->GetMyClass();
						iCasterLv = pkCaster->GetAbil(AT_CALLER_LEVEL);
					}
				}
				else if(pkCaster->IsUnitType(UT_SUB_PLAYER))
				{
					PgSubPlayer* pkSubPlayer = static_cast<PgSubPlayer*>( pkCaster );
					if(NULL == pkSubPlayer)
					{
						return fMaxElement;
					}
					CUnit* pkUnit = NULL;
					pkUnit = pkSubPlayer->GetCallerUnit();
					if(NULL!= pkUnit && pkUnit->IsUnitType(UT_PLAYER) )//소유자가 Entity를 소환한 사람
					{
						iClass = pkUnit->GetAbil(AT_CLASS);
						iCasterLv = pkUnit->GetAbil(AT_LEVEL);
					}	
				}
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_CRITICAL_DAMAGE_VALUE);
				
			}break;
		case AT_C_HIT_SUCCESS_VALUE:
		case AT_HIT_VALUE_CONSTANT:
			{
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_HIT_VALUE_CONSTANT);
				fMaxElement = fMaxElement * 0.0001f;
			}break;
		case AT_EVADE_VALUE_CONSTANT:
			{
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_EVADE_VALUE_CONSTANT);
				fMaxElement = fMaxElement * 0.0001f;
			}break;
		case AT_CRITICALRATE_VALUE_CONSTANT:
			{
				fMaxElement = kClassDefMgr.GetAbil(SClassKey(iClass, iCasterLv), AT_CRITICALRATE_VALUE_CONSTANT);
				fMaxElement = fMaxElement * 0.0001f;
			}break;
		}

	}
	
	return fMaxElement;
}

float PgSkillHelpFunc::AdjustMaxValue(int const iAbilType, float fValue, int const iCasterLv, int const iTargetLv, CUnit* pkCaster)
{// 최대수치, 최대율 제한 하는 함수
	float fMaxElement = 0.0f;
	switch(iAbilType)
	{// 공식에 사용할 상수값 설정
	case AT_HIT_SUCCESS_VALUE:	// 제한 없음
	case AT_C_HIT_SUCCESS_VALUE:
	case AT_C_FINAL_HIT_SUCCESS_RATE:
	case AT_FINAL_HIT_SUCCESS_RATE:
		{// 명중수치, 명중률
			return fValue;
		}break;
	case AT_DODGE_SUCCESS_VALUE:	// 50%
	case AT_C_DODGE_SUCCESS_VALUE:
		{// 회피수치
			fMaxElement = 0.5f;
		}break;
	case AT_BLOCK_SUCCESS_VALUE:	// 40%
	case AT_C_BLOCK_SUCCESS_VALUE:
		{// 블록수치
			fMaxElement = 0.4f;
		}break;
	case AT_CRITICAL_SUCCESS_VALUE:	// 20%
	case AT_C_CRITICAL_SUCCESS_VALUE:
		{// 크리티컬 수치
			fMaxElement = 0.2f;
		}break;	
	}

	float fMax = 0.0f;
	switch(iAbilType)
	{
	case AT_DODGE_SUCCESS_VALUE:
	case AT_C_DODGE_SUCCESS_VALUE:
		{
			fMax = GetRateValue(fMax,AT_MAX_EVADERATE_VALUE,pkCaster);
		}break;
	case AT_BLOCK_SUCCESS_VALUE:
	case AT_C_BLOCK_SUCCESS_VALUE:
		{
			fMax = GetRateValue(fMax,AT_MAX_BLOCKRATE_VALUE,pkCaster);
		}break;
	case AT_CRITICAL_SUCCESS_VALUE:
	case AT_C_CRITICAL_SUCCESS_VALUE:
		{
			fMax = GetRateValue(fMax,AT_CRITICALRATE_VALUE,pkCaster);
		}break;
	case AT_C_FINAL_DODGE_SUCCESS_RATE:
	case AT_FINAL_DODGE_SUCCESS_RATE:	// 회피율
		{
			fMax = MAX_DODGE_SUCCESS_RATE;
		}break;
	case AT_C_FINAL_BLOCK_SUCCESS_RATE:
	case AT_FINAL_BLOCK_SUCCESS_RATE:	// 블록율
		{
			fMax = MAX_BLOCK_SUCCESS_RATE;
		}break;
	case AT_C_FINAL_CRITICAL_SUCCESS_RATE:
	case AT_FINAL_CRITICAL_SUCCESS_RATE:// 크리티컬율
		{
			fMax = MAX_CRITICAL_RATE;
		}break;
	case AT_BLOCK_DAMAGE_RATE:			// 블록 데미지
	case AT_C_BLOCK_DAMAGE_RATE:
		{// 블록을 한후에 받을 데미지를 감소시킬 비율
			fMax = MAX_BLOCK_DAMAGE_RATE;
		}break;
	case AT_CRITICAL_POWER:
	case AT_C_CRITICAL_POWER:
		{
			//if(20000.0f > fValue)
			//{// 공격시 이경우에 걸린다면, 캐릭터 기본 크리티컬 데미지%가 DB에서 설정이 안된것(툴팁을 위해 max체크를 넣은것임)
			//	fValue += 20000.0f; // 기본이 2배이므로 더해준다
			//}
			fMax = GetRateValue(fMax,AT_CRITICAL_DAMAGE_VALUE,pkCaster);
		}break;
	case AT_CRITICAL_POWER_DEC:
	case AT_C_CRITICAL_POWER_DEC:
		{// 치명타 방어력
			fMax = MAX_CRITICAL_POWER_DEC;
		}break;
	case AT_FINAL_CRITICAL_SUCCESS_RATE_DEC:
	case AT_C_FINAL_CRITICAL_SUCCESS_RATE_DEC:
		{// 치명타저항율
			fMax = MAX_FINAL_CRITICAL_SUCCESS_RATE_DEC;
		}break;
	case AT_DEFENCE_IGNORE_RATE:
	case AT_C_DEFENCE_IGNORE_RATE: // 상대방의 방어력(물방,마방)을 무시하고 공격 할 확률 ( 만분률 )
		{
			fMax = MAX_DEFENCE_IGNORE_RATE;
		}break;
	case AT_BLOCK_SUCCESS_RATE_DEC:
	case AT_C_BLOCK_SUCCESS_RATE_DEC:
		{// 블록관통률
			fMax = MAX_BLOCK_SUCCESS_RATE_DEC;
		}break;
	case AT_RESIST_DEBUFF:
	case AT_C_RESIST_DEBUFF:
		{// 저주저항율	
			fMax = MAX_RESIST_DEBUFF;
		}break;
	case AT_SUCCESS_DEBUFF:
	case AT_C_SUCCESS_DEBUFF:
		{// 저주성공률
			fMax = MAX_SUCCESS_DEBUFF;
		}break;
	case AT_DEBUFF_DURATION_DEC_RATE:
	case AT_C_DEBUFF_DURATION_DEC_RATE:
		{//디버프 지속시간 감소
			fMax = MAX_DEBUFF_DURATION_DEC_RATE;
		}break;
	case AT_DEBUFF_DURATION_ADD_RATE:
	case AT_C_DEBUFF_DURATION_ADD_RATE:
		{//디버프 지속 시간 증가
			fMax = MAX_DEBUFF_DURATION_ADD_RATE;
		}break;
	}
	fValue = std::min(fValue, fMax);
	return fValue;
}
int PgSkillHelpFunc::GetFinalRateAbil(int const iAbilType)
{// 어떤 어빌의 최종율 증가(수치 -> 확률로 가는 류의 어빌) 어빌을 확인하는 함수
	switch(iAbilType)
	{
	case AT_HIT_SUCCESS_VALUE:
	case AT_C_HIT_SUCCESS_VALUE:
		{// 명중의 최종확률 증가 어빌
			return AT_FINAL_HIT_SUCCESS_RATE;
		}break;
	case AT_DODGE_SUCCESS_VALUE:
	case AT_C_DODGE_SUCCESS_VALUE:
		{// 회피의 최종확률 증가 어빌
			return AT_FINAL_DODGE_SUCCESS_RATE;
		}break;
	case AT_BLOCK_SUCCESS_VALUE:
	case AT_C_BLOCK_SUCCESS_VALUE:
		{// 블럭의 최종확률 증가 어빌
			return AT_FINAL_BLOCK_SUCCESS_RATE;
		}break;
	case AT_CRITICAL_SUCCESS_VALUE:
	case AT_C_CRITICAL_SUCCESS_VALUE:
		{// 크리티컬의 최종확률 증가 어빌
			return AT_FINAL_CRITICAL_SUCCESS_RATE;
		}break;
	default:
		{
		}break;
	}
	return 0;
}

__int64 PgSkillHelpFunc::CalcCriticalDamage(__int64 const i64OrigDamage, CUnit* pkCaster, CUnit* pkTarget)
{// 크리티컬로 인한 데미지 증가 값 계산
	if( !i64OrigDamage
		|| !pkCaster 
		|| !pkTarget
		)
	{
		return 0i64;
	}
	
	return CalcCriticalDamage( i64OrigDamage
								, pkCaster->GetAbil(AT_C_CRITICAL_POWER)
								, pkTarget->GetAbil(AT_C_CRITICAL_POWER_DEC)
								, pkCaster->GetAbil(AT_LEVEL)
								,pkTarget->GetAbil(AT_LEVEL)
								,pkCaster
							);
}

__int64 PgSkillHelpFunc::CalcCriticalDamage(__int64 i64OrigDamage, int const iCriticalPowerRate, int const iDecCriticalPowerRate, int const iCasterLv, int const iTargetLv,CUnit* pkCaster)
{
#ifdef USE_GALAXY_FORMUL
	// 크리티컬 데미지 상승 최대값을 먼저 조율하고
	float fValue = 0;
	int iAdjustedCriticalPowerRate = static_cast<int>( AdjustMaxValue(AT_CRITICAL_POWER, static_cast<float>(iCriticalPowerRate), iCasterLv, iTargetLv, pkCaster) );
	// 크리티컬 데미지 감소 최대값을 조율하고
	int const iAdjustedDec = static_cast<int>( AdjustMaxValue(AT_CRITICAL_POWER_DEC, static_cast<float>(iDecCriticalPowerRate), iCasterLv, iTargetLv) );
	// Target의 크리티컬 데미지 감소율 적용 하지만
	iAdjustedCriticalPowerRate -= iAdjustedDec;
	// 원래 데미지 보다는 작아질수 없게 제한하여, 증가율을 조정한다음
	iAdjustedCriticalPowerRate = std::max(iAdjustedCriticalPowerRate, ABILITY_RATE_VALUE);
	// 크리티컬 데미지 값을 계산한다
	i64OrigDamage = i64OrigDamage * iAdjustedCriticalPowerRate;
	i64OrigDamage /= ABILITY_RATE_VALUE;
#else
	// by reOiL
	i64OrigDamage = i64OrigDamage * ((iCriticalPowerRate == 0 ? ABILITY_RATE_VALUE * 2 : iCriticalPowerRate) / ABILITY_RATE_VALUE);
#endif
	return i64OrigDamage;
}
// Stat을 각 실 값으로 계산
void PgSkillHelpFunc::CalcStatToPhyAttack(int const iStr, int const iCon, int& riOutPhyMin, int& riOutPhyMax)
{// 물리공격력 (MIN/MAX) = INT((STR*1.5) + (CON/6))
	riOutPhyMax = static_cast<int>(iStr * 1.5f + iCon / 6.0f);
	riOutPhyMin = riOutPhyMax;
}
void PgSkillHelpFunc::CalcStatToAddPhyAttack(int const iLevel, int const iStr, int& riOutAddPhy)
{// 물리 후 공격력 = (STR*2) + LEVEL * 6
	riOutAddPhy = static_cast<int>(iStr * 2 + iLevel * 6);
}
void PgSkillHelpFunc::CalcStatToMagicAttack(int const iInt, int& riOutMin, int& riOutMax)
{// 2008.10.02: 마법공격력 (MIN/MAX) = TRUNC((LOG10(INT))*INT,0)
	riOutMin = static_cast<int>( __max(log10((float)iInt) * iInt , 0.0f) );
	riOutMax = riOutMin;
}
void PgSkillHelpFunc::CalcStatToPhyDefence(int const iStr, int const iCon, int const iDex, int& riOutPhyDefence)
{// 물리 방어력
	riOutPhyDefence = static_cast<int>(iStr * 0.8f + iCon * 0.4f + iDex * 0.2f);
}
void PgSkillHelpFunc::CalcStatToMagicDefence(int const iLevel, int const iInt, int& riOutPhyDefence)
{// 마법방어력
	riOutPhyDefence = static_cast<int>( (iInt * 1.2f + iInt * iLevel / 100.0f) * 2.0f );
}
void PgSkillHelpFunc::CalcStatToHitSuccess(PgPlayer * pkPlayer, int const iDex, int& riOutHitSuccessValue)
{// 명중수치. 기존 공식에서 셋팅해 놓은 idex * 12 에서 12를 default 값으로 해놓는다.
	if(!pkPlayer)
	{
		return;
	}
	float fValue = 0;
	fValue = GetRateValue(fValue, AT_HIT_VALUE_CONSTANT, pkPlayer);
#ifdef USE_GALAXY_FORMUL
	iHitValueStatic = fValue;
	riOutHitSuccessValue = iDex * fValue;
#else
	riOutHitSuccessValue = int(MAKE_ABIL_RATE(100) + iDex * fValue); // 12
#endif
}
void PgSkillHelpFunc::CalcStatToDodgeSuccess(PgPlayer * pkPlayer, int const iDex, int& riOutDodgeSuccessValue)
{// 회피수치 . 기존 공식에서 셋팅해 놓은 idex * 14 에서 14를 default 값으로 해놓는다.
	if(!pkPlayer)
	{
		return;
	}
	float fValue = 0;
	fValue = GetRateValue(fValue, AT_EVADE_VALUE_CONSTANT, pkPlayer);
#ifdef USE_GALAXY_FORMUL
	iEvadeValueValueStatic = fValue;
	riOutDodgeSuccessValue = iDex * fValue;
#else
	riOutDodgeSuccessValue = iDex * fValue;
#endif
}
void PgSkillHelpFunc::CalcStatToCriticalSuccess(PgPlayer * pkPlayer, int const iDex, int& riOutCriticalSuccess)
{// 크리티컬 성공 수치 . 기존 공식에서 셋팅해 놓은 idex * 2.4 + 15 에서 12를 default 값으로 해놓는다.
	if(!pkPlayer)
	{
		return;
	}
	float fValue = 0;
	fValue = GetRateValue(fValue, AT_CRITICALRATE_VALUE_CONSTANT, pkPlayer);
#ifdef USE_GALAXY_FORMUL
	iDexStatic = iDex;
	iCriticalStatic = fValue;
	riOutCriticalSuccess = iDex * fValue +15;
#else
	riOutCriticalSuccess = int(MAKE_ABIL_RATE(2) + iDex * fValue /*1.2*/);
#endif
}
void PgSkillHelpFunc::CalcStatToMaxHP(int const iLevel, int const iCon, int& riOutMaxHP)
{// Max HP
	riOutMaxHP = static_cast<int>( iCon * 25 * ( 1 + iLevel/100.0f ) );
}
void PgSkillHelpFunc::CalcStatToMaxMP(int const iLevel, int const iInt, int& riOutMaxMP)
{// Max MP
	riOutMaxMP = static_cast<int>( iInt * 25 * ( 1 + iLevel/100.0f ) );
}

float PgSkillHelpFunc::ConvertAbilValueToRate(int const iAbilType, float fValue, int const iCasterLv, int const iTargetLv, CUnit* pkCaster)
{// 수치 To 성공률
	if(!pkCaster)
	{
		return fValue;
	}

	switch(iAbilType)
	{
	case AT_HIT_SUCCESS_VALUE:		// 명중수치 -> 명중률
		{
			fValue = (fValue * 0.5f * ABILITY_RATE_VALUE_FLOAT) / (fValue * 0.5f + iTargetLv * (25+ iTargetLv * 0.1f) + 300);
		}break;
	case AT_DODGE_SUCCESS_VALUE:	// 회피수치 -> 회피율
	case AT_BLOCK_SUCCESS_VALUE:	// 블럭수치 -> 블럭율
		{
			fValue = (fValue * 0.5f * ABILITY_RATE_VALUE_FLOAT) / (fValue * 0.5f + iCasterLv * (25+ iCasterLv * 0.1f) + 300);
		}break;
	case AT_CRITICAL_SUCCESS_VALUE:	// 치명수치 -> 치명성공률
		{
			fValue = (fValue * 0.2f * ABILITY_RATE_VALUE_FLOAT) / (fValue * 0.5f + iTargetLv * (18+ iTargetLv * 0.1f) + 300);
		}break;
	}
	return fValue;
}

bool PgSkillHelpFunc::IsSystemEffect(CEffect const* pkEffect)
{// 시스템 디버프가 정의될때까지 사용 불가
	if(!pkEffect)
	{
		return false;
	}
	return IsSystemEffect(pkEffect->GetType(), pkEffect->GetAbil(AT_CURE_NOT_DELETE) | pkEffect->GetAbil(AT_IGNORE_REQ_USER_DEL_EFFECT) );		
}
bool PgSkillHelpFunc::IsSystemEffect(CEffectDef const* pkEffectDef)
{// 시스템 디버프가 정의될때까지 사용 불가
	if(!pkEffectDef)
	{
		return false;
	}
	return IsSystemEffect(pkEffectDef->GetType(), pkEffectDef->GetAbil(AT_CURE_NOT_DELETE) | pkEffectDef->GetAbil(AT_IGNORE_REQ_USER_DEL_EFFECT) );		
}

bool PgSkillHelpFunc::IsSystemEffect(short int const sEffectType, bool const bDeleteAble)
{// 시스템 이펙트를 확인하는 조건은 현재, 매우 모호하므로 추가적인 정보가 있다면 이곳에 추가해준다
	//switch(sEffectType)
	//{
	//case EFFECT_TYPE_BLESSED:
	//	{// 버프가
	//		if(bDeleteAble)
	//		{// 유저 요청으로 지울수 없는것이라면
	//			return true;
	//		}
	//	}break;
	//case EFFECT_TYPE_CURSED:
	//	{// 디버프가
	//		if(bDeleteAble)
	//		{// 지울수 없는것이라면
	//			return true;
	//		}
	//	}break;
	//case EFFECT_TYPE_NORMAL:
	//	{
	//		return true;
	//	}break;
	//}
	return false;
}

void PgSkillHelpFunc::SkillOnDie(CUnit * pkUnit, int const iSkillNo, bool const bVoluntarily)
{
	assert(pkUnit);

	int iHP = 0;
	if(iSkillNo > 0 && (pkUnit->UnitType() & UT_UNIT_AI))
	{
		iHP = ON_DIE_SKILL_HP;
		pkUnit->ClearAllEffect(true);
		pkUnit->SetAbil(AT_SKILL_ON_DIE, iSkillNo);
		pkUnit->SetAbil(AT_DAMAGEACTION_TYPE, E_DMGACT_TYPE_NO);
		pkUnit->AddCountAbil(AT_CANNOT_DAMAGE, AT_CF_EFFECT, true, E_SENDTYPE_BROADALL);
		pkUnit->m_kAI.SetEvent(BM::GUID::NullData(), EAI_EVENT_DIE_SKILL);
		pkUnit->SetDelay(0);
		pkUnit->SetAttackDelay(0);
	}
	pkUnit->SetAbil(AT_HP, iHP);

	if(bVoluntarily)
	{
		pkUnit->SetAbil(AT_VOLUNTARILY_DIE, 1, true, true);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class PgAdjustSkillFunc //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PgAdjustSkillFunc::IsApplySkill(int const iSkillNo, CEffect* pkEffect)
{// CalcAdjustSkillValue에서 해당 스킬을 조정하는 이펙트가 걸려있는지 확인하기 위한 함수
	if(!pkEffect)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkillDef)
	{
		return false;
	}

	int iParentCastSkillNo = 0;
	int const iParentSkill = pkSkillDef->GetParentSkill() ? pkSkillDef->GetParentSkill() : iSkillNo;
	{
		CSkillDef const *pkParentSkillDef = kSkillDefMgr.GetDef(iParentSkill);
		if(pkParentSkillDef)
		{// 이 스킬이 SkillActor등으로 사용되는 스킬이라면, 플레이어가 캐스팅 하는 스킬의 레벨로 얻어온다
			iParentCastSkillNo = pkParentSkillDef->GetAbil(AT_PARENT_CAST_SKILL_NO);
		}
	}
	// 이 스킬을 발동 시키기 위한 스킬이 있었다면 그 스킬로 DB에 입력되기 때문에 그 스킬의 부모 스킬로 비교해야 한다.
	int const iApplySkillNo = iParentCastSkillNo ? iParentCastSkillNo : iParentSkill;

	int const iCheckType = pkEffect->GetAbil(AT_ADJUST_SKILL_CHECK_TYPE);
	if(0 == iCheckType)
	{// 지금 사용한 스킬이 적용되는지 확인하고
		return false;
	}
	// 스킬 체크 타입, EAdjustSkillCheckType 1:모든 스킬, 2:일부 스킬만, 3:일부 스킬 제외하고 모두
	switch(iCheckType)
	{// 해당 스킬 번호
	case EASC_ALL:
		{// 모두가 대상
			return true;
		}break;
	case EASC_CHECK_SOME_SECTION:
		{// 존재하면 이 스킬은 영향을 받음
			if(CheckExistAdjustSkillNoInEffect(iApplySkillNo, pkEffect))
			{
				return true;
			}
			return false;
		}break;
	case EASC_CHECK_EXCLUSIVE_SECTION:
		{// 존재하지 않아야 스킬은 영향을 받음
			if(!CheckExistAdjustSkillNoInEffect(iApplySkillNo, pkEffect))
			{
				return true;
			}
			return false;
		}break;
	default:
		{
			return false;
		}break;
	}

	return false;
}

__int64 PgAdjustSkillFunc::GetAddtionalAdjustSkillValue(CUnit* pkUnit, int const iAbilType, CEffect* pkEffect, __int64 i64Value)
{// CalcAdjustSkillValue에서 사용하기 위한 함수
	__int64 i64Result = 0;
	switch(iAbilType)
	{
	case AT_E_ADJUST_SKILL_RANGE:
	case AT_E_ADJUST_SKILL_RANGE2:
	case AT_E_ADJUST_SKILL_DMG:
	case AT_E_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE:
	case AT_E_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE:
	case AT_E_ADJUST_SKILL_INC_PHASE_DMG:
		//case AT_ADJUST_SKILL_E_COOLTIME:
		{// 더해야 되는값
			i64Result = pkEffect->GetAbil(iAbilType);
		}break;
	case AT_R_ADJUST_SKILL_RANGE:
	case AT_R_ADJUST_SKILL_RANGE2:
	case AT_R_ADJUST_SKILL_DMG:
	case AT_R_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE:
	case AT_R_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE:
	case AT_R_ADJUST_SKILL_INC_PHASE_DMG:
		//case AT_ADJUST_SKILL_R_COOLTIME:
		{// 곱해야 되는값
			i64Result = i64Value * pkEffect->GetAbil(iAbilType)/ABILITY_RATE_VALUE;
		}break;
	default:
		{
			return i64Result;
		}break;
	}

	return i64Result;
}

__int64 PgAdjustSkillFunc::CalcAdjustSkillValue(EAdjustSkillCalcType const eCalcType, int const iSkillNo, CUnit* pkUnit, __int64 const i64OriginValue)
{// 곱하기와 더하기 계산이 같이 이루어지기 때문에, 한 계통에서 이함수가 여러번 호출할 경우가 생긴다면 다시 생각해봐야함
	if(!pkUnit)
	{// 대상을 얻어와서 
		return i64OriginValue;
	}

	PgUnitEffectMgr const& rkEffectMgr = pkUnit->GetEffectMgr();
	if(0 == rkEffectMgr.Size())
	{// 걸려있는 이펙트(버프)가 있다면
		return i64OriginValue;
	}

	__int64 i64ResultValue = i64OriginValue;

	ContEffectItor kItor;	
	rkEffectMgr.GetFirstEffect(kItor);
	CEffect* pkEffect = NULL;

	typedef std::vector<int>  CONT_INT;
	CONT_INT kCont;

	switch(eCalcType)
	{// 계산을 위해 계산하기 위해 체크해야할 어빌들을 넣어놓고
		//	(_R_ 먼저 넣어 곱하고 난후, _E_의 더하기 계산이 되게 해야한다)
	case EASCT_RANGE:
		{// 범위
			kCont.push_back(AT_R_ADJUST_SKILL_RANGE);
			kCont.push_back(AT_E_ADJUST_SKILL_RANGE);
		}break;
	case EASCT_RANGE2:
		{// 범위2
			kCont.push_back(AT_R_ADJUST_SKILL_RANGE2);
			kCont.push_back(AT_E_ADJUST_SKILL_RANGE2);
		}break;
	case EASCT_DMG:
		{// 데미지
			kCont.push_back(AT_R_ADJUST_SKILL_DMG);
			kCont.push_back(AT_E_ADJUST_SKILL_DMG);
		}break;
		//case EASCT_ADJUST_SKILLCOOLTIME:
		//	{// 쿨타임
		//		kCont.push_back(AT_ADJUST_SKILL_R_COOLTIME);
		//		kCont.push_back(AT_ADJUST_SKILL_E_COOLTIME);
		//	}break;
		/// 이하는 단계별 스킬 데미지 조절 값 계산 부분
	case EASCT_RESULT_MIN_DMG_BY_PHASE:
		{// 최소값
			kCont.push_back(AT_R_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE);
			kCont.push_back(AT_E_ADJUST_SKILL_RESULT_MIN_DMG_BY_PHASE);
		}break;
	case EASCT_RESULT_MAX_DMG_BY_PHASE:
		{// 최대값
			kCont.push_back(AT_R_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE);
			kCont.push_back(AT_E_ADJUST_SKILL_RESULT_MAX_DMG_BY_PHASE);
		}break;
	case EASCT_INC_PHASE_DMG:
		{// 증가값
			kCont.push_back(AT_R_ADJUST_SKILL_INC_PHASE_DMG);
			kCont.push_back(AT_E_ADJUST_SKILL_INC_PHASE_DMG);
		}break;
	default:
		{
			return i64OriginValue;
		}break;
	}


	std::set<CEffect*> ContEff;//이펙트 번호.
	__int64 i64AccCalcedValue = 0;
	while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
	{// Unit에 걸려있는 이펙트중에서
		if(pkEffect)
		{
			if(IsApplySkill(iSkillNo, pkEffect))
			{// 적용 되는 스킬이고
				if(EASCT_INC_PHASE_DMG == eCalcType)
				{// 점진적 데미지 감소 이펙트 이면 따로 저장해두고
					ContEff.insert(pkEffect);
				}

				CONT_INT::iterator itor = kCont.begin();
				while(kCont.end() != itor)
				{// kCont안의 어빌들로 값을 계산을 하여
					__int64 const i64CalcedVal = GetAddtionalAdjustSkillValue(pkUnit, (*itor), pkEffect, i64ResultValue);
					i64AccCalcedValue+=i64CalcedVal; // 최종적으로 적용할 값을 구한후
					++itor;
				}
			}
		}
	}


	{/// eCalcType 타입 별로, 최종 적용값인 iAccCalcedValue를 사용한다
		if(EASCT_INC_PHASE_DMG == eCalcType)
		{// 점진적 데미지 가감값 계산이라면
			__int64 i64ReduceDamage = 0;
			std::set<CEffect*>::iterator kEffItor = ContEff.begin();
			while(ContEff.end() != kEffItor)
			{// 점진적 감소이펙트들에 감소된 스킬 데미지 값을 저장하고
				pkEffect = *kEffItor;
				if(pkEffect)
				{
					SActArg* pkEffectActArg = pkEffect->GetActArg();
					if(pkEffectActArg)
					{
						DWORD const dwTime = BM::GetTime32();
						{// 때린 시간 갱신
							pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_BEGIN_TIME, dwTime);
							pkEffectActArg->Set(ACT_ARG_CUSTOMDATA1, true);
						}
						{// 데미지 감소량 증가
							//BM::vstring vStr; vStr+="계산전 원데미지 :"; vStr+=iOriginValue; vStr+="\n"; 
							pkEffectActArg->Get(ACT_ARG_DMG_BY_PASE_ACC_DMG, i64ReduceDamage);
							//if(0 == iReduceDamage) { vStr+="처음임\n"; }
							//vStr+="저장된 가감 값 :";vStr+=iReduceDamage; vStr+="\n"; vStr+="1차 계산된 가감값 :";	vStr+=iAccCalcedValue;	vStr+="\n";
							i64AccCalcedValue+=i64ReduceDamage;
							//vStr+="합계 :"; vStr+=iReduceDamage;vStr+="\n";

							if(i64AccCalcedValue < -i64OriginValue)
							{// 언더플로 방지 (적용되는 최소 값이 되므로)
								i64AccCalcedValue = -i64OriginValue;
							}
							{// 오버플로는?
							}
							pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_ACC_DMG, i64AccCalcedValue);
							//vStr+="\n";	OutputDebugStringA(static_cast<std::string>(vStr).c_str());
						}
					}
				}
				++kEffItor;
			}
			//이펙트에 저장된 데미지만 적용해준다.(따라서 두번째 타격 부터 데미지 감소가 적용됨)
			i64ResultValue+=i64ReduceDamage;
		}
		else
		{// 누적된 계산값을 합해주고
			i64ResultValue+=i64AccCalcedValue;
		}
	}

	if(0 > i64ResultValue)
	{/// 최소값을 벗어났을 경우
		switch(eCalcType)
		{// 타입에 맞는 최소값들을 설정한다
		case EASCT_DMG:	// 데미지 계열이라면 최소 값은 1
		case EASCT_INC_PHASE_DMG:
			{
				i64ResultValue= 1;
			}break;
		default:
			{
				i64ResultValue= 0;
			}break;
		}
	}
	return i64ResultValue;
}

int PgAdjustSkillFunc::GetAttackRange(CUnit * pkUnit, CSkillDef const* pkSkillDef)
{
	if(!pkUnit || !pkSkillDef)
	{
		return 0;
	}

	EAttackRangeSource const eRangeSource = static_cast<EAttackRangeSource>(pkSkillDef->GetAbil(AT_RANGE_TYPE));
	int iSkillRange = 0;
	switch(eRangeSource)
	{
	case EAttackRange_ItemRange:
	case EAttackRange_Skill_Item:
		{
			if(eRangeSource == EAttackRange_Skill_Item)
			{
				iSkillRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
			}

			PgBase_Item kSItem;
			if( pkUnit->GetInven()
				&& (S_OK == pkUnit->GetInven()->GetItem(IT_FIT, EQUIP_POS_WEAPON, kSItem)) )
			{
				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const *pDef = kItemDefMgr.GetDef(kSItem.ItemNo());
				if(pDef)
				{
					iSkillRange += pDef->GetAbil(AT_ATTACK_RANGE);
				}
			}
		}break;
	case EAttackRange_UnitRange:
		{
			iSkillRange = pkUnit->GetAbil(AT_ATTACK_RANGE);
		}break;
	case EAttackRange_SkillRange:
	case EAttackRange_SkillRange2:
	default:
		{
			iSkillRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		}break;
	}
	if(iSkillRange <= 0)
	{
		//INFO_LOG(BM::LOG_LV0, __FL__<<L"AttackRange is <0 SkillNo["<<pkSkillDef->No()<<L"], RangeType["<<eRangeSource<<L"], Range["<<iSkillRange<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return 0;
	}

	return CalcAttackRange(pkUnit, iSkillRange);
}

int PgAdjustSkillFunc::CalcAttackRange(CUnit * pkUnit, int const iSkillRange)
{
	if(pkUnit)
	{
		int iRate = pkUnit->GetAbil(AT_ATTACK_RANGE_RATE);
		if(pkUnit->GetInven())
		{
			//사정거리 증가 옵션이 있을 경우에만 계산
			iRate += pkUnit->GetInven()->GetAbil(AT_R_ATTACK_RANGE);
		}

		if(0 != iRate)
		{
			//기본값이 0 이므로 일단 100%로 맞춘다음 거기에 추가 해야 한다.
			iRate += ABILITY_RATE_VALUE;			
			return static_cast<int>(iSkillRange * (static_cast<float>(iRate) / ABILITY_RATE_VALUE_FLOAT));
		}
	}
	return iSkillRange;
}


//
namespace PgXmlLocalUtil
{
	char const* LOCAL_ELEMENT_NAME = "LOCAL";
	bool IsContainLocal(LOCAL_MGR::CLocal& rkLocalMgr, std::wstring const & rServiceName, bool const bNationCode = false, char const* szContents = "NULL")
	{
		typedef std::list< std::wstring > Contstr;
		Contstr kList;
		BM::vstring::CutTextByKey(rServiceName, std::wstring(_T("/")), kList);
#ifdef _MD_
		if( 1 < kList.size()
			&&	kList.end() != std::find( kList.begin(), kList.end(), std::wstring(L"DEFAULT")) )
		{
			std::wstring const& kTempStr = std::wstring(L"'DEFAULT' is can't same use, other local tag in ") + UNI(szContents);
			::MessageBox(NULL, kTempStr.c_str(), L"Error", MB_OK);
		}
#endif

		Contstr::const_iterator it = kList.begin();
		while(kList.end()!=it)
		{
			if( bNationCode )
			{
				if(true==rkLocalMgr.IsAbleNationCode((*it).c_str()))
				{
					return true;
				}
			}
			else
			{
				if(true==rkLocalMgr.IsAbleServiceName((*it).c_str()))
				{
					return true;
				}
			}
			++it;
		}

		return false;
	}
	TiXmlNode const* FindInLocal(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlNode const* pkLocalNode, char const* szContents, bool const bNationCode)
	{
		RESULT_NODE kResultNode = FindInLocalNode(rkLocalMgr, pkLocalNode, szContents, bNationCode);
		return kResultNode.first ? kResultNode.first : kResultNode.second;
	}

	RESULT_NODE FindInLocalNode(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlNode const* pkLocalNode, char const* szContents, bool const bNationCode)
	{
		RESULT_NODE kResultNode;
		if( !pkLocalNode )
		{
			return kResultNode;
		}

		TiXmlNode const* pkFindLocalNode = NULL;
		TiXmlNode const* pkDefaultLocalNode = NULL;
		TiXmlNode const* pkSubChild = pkLocalNode->FirstChild();
		while( pkSubChild )
		{
			TiXmlElement const* pkSubElement = dynamic_cast<TiXmlElement const*>(pkSubChild);
			if( pkSubElement )
			{
				TiXmlAttribute const* pkAttr = pkSubElement->FirstAttribute();
				if( pkAttr )
				{
					char const *pcAttrValue = pkAttr->Value();
					if( pcAttrValue )
					{
						std::string const kCurLocal(pcAttrValue);
						std::wstring const wstrLocalName(BM::vstring::ConvToUnicode(kCurLocal));

						if(true==IsContainLocal(rkLocalMgr, wstrLocalName, bNationCode, szContents))
						{
#ifdef _MD_
							if(pkFindLocalNode)
							{
								std::wstring const& kTempStr = std::wstring(wstrLocalName) + L"  local is duplicated. " + UNI(szContents);
								::MessageBox(NULL, kTempStr.c_str(), L"Error", MB_OK);
							}
#endif
							pkFindLocalNode = pkSubChild;
						}
						else if("DEFAULT" == kCurLocal)
						{
							pkDefaultLocalNode = pkSubChild;
						}
					}
				}
			}
			pkSubChild = pkSubChild->NextSibling();
		}

		kResultNode.first = pkFindLocalNode;
		kResultNode.second = pkDefaultLocalNode;
		return kResultNode;
	}

	TiXmlElement const* FindInLocalResult(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlElement const* pkLocalNode, bool const bNationCode)
	{
		TiXmlNode const* pkFindLocalNode = FindInLocal(rkLocalMgr, pkLocalNode, "NULL", bNationCode);
		if( pkFindLocalNode )
		{
			TiXmlElement const* pkResultNode = pkFindLocalNode->FirstChildElement();
			if( pkResultNode )
			{
				return pkResultNode;
			}
		}
		return NULL;
	}
}

bool IsClass_OwnSubPlayer(int const iClass)
{
	switch(iClass)
	{
	case UCLASS_DOUBLE_FIGHTER:
	case UCLASS_TWINS:
	case UCLASS_MIRAGE:
	case UCLASS_DRAGON_FIGHTER:
	case UCLASS_DRAGON_SHADOW:
		{
			return true;
		}break;
	}
	return false;
}