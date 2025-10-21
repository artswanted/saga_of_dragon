#include "stdafx.h"
#include "PgGround.h"
#include "PgEffectFunction_Pet.h"

//이펙트 사용의 주체는 펫이다
int PgAutoLootByPetFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = GetGroundPtr(pkArg);
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	PgPet* pkPet = dynamic_cast<PgPet*>(pkUnit);
	if(!pkPet)
	{
		return ECT_MUSTDELETE;
	}

	PgPlayer* pkPlayer = pkGround->GetUser(pkPet->Caller());
	if(!pkPlayer)
	{
		return ECT_MUSTDELETE;
	}

	int const iSkillNo = pkEffect->GetAbil(AT_MON_SKILL_01/* + pkPet->GetConditionGrade()*/);//0Base	상태에 따라 참고할 스킬 번호
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkill)
	{
		return ECT_MUSTDELETE;
	}

	POINT3 const& rkPos = pkPlayer->GetPos();	//아이템 줍는 위치는 플레이어 위치로
	UNIT_PTR_ARRAY kTargetArray;
	pkGround->GetUnitInRange(rkPos, pkSkill->GetAbil(AT_ATTACK_RANGE), UT_GROUNDBOX, kTargetArray, static_cast<int>(AI_Z_LIMIT));

	UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();

	int iMaxCount = pkSkill->GetAbil(AT_MAX_LOOTNUM);

	while(kTargetArray.end() != unit_itor && 0<iMaxCount)
	{
		PgGroundItemBox* pkBox = dynamic_cast<PgGroundItemBox*>((*unit_itor).pkUnit);
		if (pkBox && pkBox->IsOwner(pkPlayer->GetID()))	//주인이면
		{
			pkGround->PickUpGroundBox(pkPlayer, pkBox->GetID(), pkUnit);	//펫에게 아이템이 빨려 들어가야 됨
			--iMaxCount;
		}
		++unit_itor;
	}

	return ECT_DOTICK;
}

int PgNurseByPetFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = GetGroundPtr(pkArg);
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	PgPet* pkPet = dynamic_cast<PgPet*>(pkUnit);
	if(!pkPet)
	{
		return ECT_MUSTDELETE;
	}

	PgPlayer* pkPlayer = pkGround->GetUser(pkPet->Caller());
	if(!pkPlayer)
	{
		return ECT_MUSTDELETE;
	}

	if(pkPlayer->IsDead())
	{
		return ECT_MUSTDELETE;
	}

	bool bAddEffect = false;
	//HP
	{
		int const iAddValue = pkEffect->GetAbil(AT_HP);

		int const iNowValue = pkPlayer->GetAbil(AT_HP);
		int const iMaxValue = pkPlayer->GetAbil(AT_C_MAX_HP);

		int const iNewValue = __min(iAddValue + iNowValue, iMaxValue);

		if(iNowValue != iNewValue)
		{
			OnSetAbil(pkPlayer, AT_HP, iNewValue);
			bAddEffect = true;
		}
	}

	//MP
	{
		int const iAddValue = pkEffect->GetAbil(AT_MP);

		int const iNowValue = pkPlayer->GetAbil(AT_MP);
		int const iMaxValue = pkPlayer->GetAbil(AT_C_MAX_MP);

		int const iNewValue = __min(iAddValue + iNowValue, iMaxValue);

		if(iNowValue != iNewValue)
		{
			OnSetAbil(pkPlayer, AT_MP, iNewValue);
			bAddEffect = true;
		}
	}

	if(bAddEffect)
	{
		// 반짝이는 이펙트 효과
		int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
		if(0 < iAddEffect)
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkPlayer->GetID());
			pkPlayer->AddEffectQueue(kData);
		}
	}

	return ECT_DOTICK;
}

void PgMetaAblityToPetFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( UT_PET != pkUnit->UnitType() )
	{
		return;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if ( pkGround )
	{
		PgPlayer *pkPlayer = pkGround->GetUser( pkUnit->Caller() );
		if ( pkPlayer )
		{
			__int64 const i64PhyAttackRate = static_cast<__int64>(pkEffect->GetAbil( AT_R_PHY_ATTACK ));
			__int64 const i64MagicAttackRate = static_cast<__int64>(pkEffect->GetAbil( AT_R_MAGIC_ATTACK ));

			int const iPhyAttackMin = static_cast<int>(static_cast<__int64>(pkPlayer->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MIN)) * i64PhyAttackRate / ABILITY_RATE_VALUE64);
			int const iPhyAttackMax = static_cast<int>(static_cast<__int64>(pkPlayer->GetAbil(AT_EQUIP_WEAPON_PHY_ATTACK_MAX)) * i64PhyAttackRate / ABILITY_RATE_VALUE64);
			int const iMagAttackMin = static_cast<int>(static_cast<__int64>(pkPlayer->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN)) * i64MagicAttackRate / ABILITY_RATE_VALUE64);
			int const iMagAttackMax = static_cast<int>(static_cast<__int64>(pkPlayer->GetAbil(AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX)) * i64MagicAttackRate / ABILITY_RATE_VALUE64);
			
			OnAddAbil( pkUnit, AT_EQUIP_WEAPON_PHY_ATTACK_MIN, iPhyAttackMin );
			OnAddAbil( pkUnit, AT_EQUIP_WEAPON_PHY_ATTACK_MAX, iPhyAttackMax );
			OnAddAbil( pkUnit, AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN, iMagAttackMin );
			OnAddAbil( pkUnit, AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX, iMagAttackMax );

			pkEffect->SetActArg( ACTARG_EQUIP_WEAPON_PHY_ATTACK_MIN, iPhyAttackMin );
			pkEffect->SetActArg( ACTARG_EQUIP_WEAPON_PHY_ATTACK_MAX, iPhyAttackMax );
			pkEffect->SetActArg( ACTARG_EQUIP_WEAPON_MAGIC_ATTACK_MIN, iMagAttackMin );
			pkEffect->SetActArg( ACTARG_EQUIP_WEAPON_MAGIC_ATTACK_MAX, iMagAttackMax );
		}
	}
}

void PgMetaAblityToPetFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int iValue = 0;
	if ( S_OK == pkEffect->GetActArg( ACTARG_EQUIP_WEAPON_PHY_ATTACK_MIN, iValue ) )
	{
		OnAddAbil( pkUnit, AT_EQUIP_WEAPON_PHY_ATTACK_MIN, -iValue );
	}

	if ( S_OK == pkEffect->GetActArg( ACTARG_EQUIP_WEAPON_PHY_ATTACK_MAX, iValue ) )
	{
		OnAddAbil( pkUnit, AT_EQUIP_WEAPON_PHY_ATTACK_MAX, -iValue );
	}

	if ( S_OK == pkEffect->GetActArg( ACTARG_EQUIP_WEAPON_MAGIC_ATTACK_MIN, iValue ) )
	{
		OnAddAbil( pkUnit, AT_EQUIP_WEAPON_MAGIC_ATTACK_MIN, -iValue );
	}

	if ( S_OK == pkEffect->GetActArg( ACTARG_EQUIP_WEAPON_MAGIC_ATTACK_MAX, iValue ) )
	{
		OnAddAbil( pkUnit, AT_EQUIP_WEAPON_MAGIC_ATTACK_MAX, -iValue );
	}
}

int PgMetaAblityToPetFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

void PgActivateSkillPetFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( UT_PET != pkUnit->UnitType() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error UnitType<" << pkUnit->UnitType() << L"> Effect<" << pkEffect->GetEffectNo() << L">" );
		return;
	}

	PgPet *pkPet = dynamic_cast<PgPet*>(pkUnit);
	if ( !pkPet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Critical Error Unit<" << pkUnit->GetID() << L"> Effect<" << pkEffect->GetEffectNo() << L">" );
		return;
	}

	if ( !pkEffect->GetValue() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error Value Unit<" << pkUnit->GetID() << L"> Effect<" << pkEffect->GetEffectNo() << L">" );
		return;
	}
}

void PgActivateSkillPetFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( UT_PET != pkUnit->UnitType() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Error UnitType<" << pkUnit->UnitType() << L"> Effect<" << pkEffect->GetEffectNo() << L">" );
		return;
	}

	PgPet *pkPet = dynamic_cast<PgPet*>(pkUnit);
	if ( !pkPet )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Critical Error Unit<" << pkUnit->GetID() << L"> Effect<" << pkEffect->GetEffectNo() << L">" );
		return;
	}

	PgGround * pkGround = NULL;
	if ( pkArg )
	{
		pkArg->Get( ACTARG_GROUND, pkGround );
	}

	int const iSkillNo = pkEffect->GetValue();
	if ( true == pkPet->GetMySkill()->Delete( iSkillNo ) )
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, iSkillNo);
		pkPet->AddEffectQueue(kData);

		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if ( pkSkillDef )
		{
			if ( ESTARGET_CASTER & pkSkillDef->GetTargetType() )
			{
				PgPlayer *pkPlayer = pkGround->GetUser( pkPet->Caller() );
				if ( pkPlayer )
				{
					pkPlayer->AddEffectQueue(kData);
				}
			}
		}

		BM::Stream kPacket( PT_C_M_REQ_DEACTIVATE_SKILL, iSkillNo );
		pkPet->Send( kPacket, E_SENDTYPE_SELF );
	}
}

int PgActivateSkillPetFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}