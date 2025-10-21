#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgGround.h"
#include "PgEffectFunction_Object.h"

////////////////////////////////////////////////////////////////////////////
// PgRecoveryPoolEffect 회복우물 : 일정틱마다 주위 플레이어의 HP/MP를 회복
////////////////////////////////////////////////////////////////////////////
void PgRecoveryPoolEffect::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	pkUnit->SetAbil( AT_INVINCIBLE2, 1, false, false );
}

void PgRecoveryPoolEffect::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	pkUnit->SetAbil( AT_INVINCIBLE2, 0, false, false );
}

int PgRecoveryPoolEffect::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	pkArg->Get( ACTARG_GROUND, pkGround );
	if( !pkGround )
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	BM::Stream kCustomActionPacket( PT_M_C_NFY_ACTION_CUSTOM_CONTROL, pkUnit->GetID() );
	kCustomActionPacket.Push( pkEffect->GetEffectNo() );
	kCustomActionPacket.Push( true );// 회복이펙트 함
	pkUnit->Send( kCustomActionPacket, E_SENDTYPE_BROADCAST );

	int const iTeam = pkUnit->GetAbil(AT_TEAM);
	int const iAddHP = pkEffect->GetAbil(AT_HP);
	int const iAddMP = pkEffect->GetAbil(AT_MP);
	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	int const iAddEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);

	int iMaxVal = 0;
	int iNowVal = 0;
	UNIT_PTR_ARRAY kTargetUnitArray;
	pkGround->GetUnitInRange( pkUnit->GetPos(), iRange, UT_PLAYER, kTargetUnitArray );

	UNIT_PTR_ARRAY::iterator unit_itr = kTargetUnitArray.begin();
	for ( ; unit_itr!=kTargetUnitArray.end() ; ++unit_itr )
	{
		CUnit *pkTargetUnit = unit_itr->pkUnit;
		if ( iTeam == pkTargetUnit->GetAbil(AT_TEAM) )
		{
			if( pkTargetUnit->IsAlive() )
			{
				if ( 0 < iAddHP )
				{
					iMaxVal = pkTargetUnit->GetAbil(AT_C_MAX_HP);
					iNowVal = pkTargetUnit->GetAbil(AT_HP);
					if ( iNowVal < iMaxVal )
					{
						OnSetAbil( pkTargetUnit, AT_HP, __min( iMaxVal, iNowVal+iAddHP) );
					}
				}

				if ( 0 < iAddMP )
				{
					iMaxVal = pkTargetUnit->GetAbil(AT_C_MAX_MP);
					iNowVal = pkTargetUnit->GetAbil(AT_MP);
					if ( iNowVal < iMaxVal )
					{
						OnSetAbil( pkTargetUnit, AT_MP, __min( iMaxVal, iNowVal+iAddMP) );
					}
				}

				if( 0 < iAddEffectNo )
				{
					EffectQueueData kData(EQT_ADD_EFFECT, iAddEffectNo, 0, pkArg, pkUnit->GetID());
					pkUnit->AddEffectQueue(kData);
				}
			}	
		}
	}

	return ECT_DOTICK;
}

int PgRecoveryPoolEffect::EffectReset(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	BM::Stream kCustomActionPacket( PT_M_C_NFY_ACTION_CUSTOM_CONTROL, pkUnit->GetID() );
	kCustomActionPacket.Push( pkEffect->GetEffectNo() );
	kCustomActionPacket.Push( false );// 회복 이펙트 못함.
	pkUnit->Send( kCustomActionPacket, E_SENDTYPE_BROADCAST );
	return ECT_RESET;
}
