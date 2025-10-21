#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Action.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Global.h"
#include "Global.h"
#include "PgEffectFunction.h"
#include "PgGround.h"
#include "PgStaticEventGround.h"
#include "PublicMap/PgWarGround.h"

///////////////////////////////////////////////////////////
//  PgResurrection01SkillFunction
///////////////////////////////////////////////////////////
int PgResurrection01SkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	int iAlive = pkUnit->GetAbil(AT_REVIVED_BY_OTHER);
	if(0 < iAlive)
	{
		pkUnit->SetAbil(AT_REVIVED_BY_OTHER, 0);
		int const iHP = pkUnit->GetAbil(AT_HP_RESERVED);
		int const iMP = pkUnit->GetAbil(AT_MP_RESERVED);
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if (pkPlayer != NULL)
		{
			pkPlayer->Alive(EALIVE_ITEM, E_SENDTYPE_BROADALL, iHP, iMP);
			
			if(pkGround)
			{// SubPlayer를 소유하고 있는 클래스일경우
				//int const iClass = pkPlayer->GetAbil(AT_CLASS);
				//if( IsClass_OwnSubPlayer(iClass) )
				//{
				//	BM::GUID kGuid = pkPlayer->SubPlayerID();
				//	if( kGuid.IsNull() )
				//	{// SubPlayer를 생성함
				//		kGuid.Generate();
				//	}
				//	pkGround->CreateSubPlayer(pkPlayer, kGuid);
				//}

				if( pkGround->GetAttr() & GATTR_EVENT_GROUND )
				{
					PgStaticEventGround * pStaticGround = dynamic_cast<PgStaticEventGround *>(pkGround);
					if( pStaticGround && (pStaticGround->GetEventGroundState() & EGS_PLAY) )
					{
						pStaticGround->RevivePlayer();
					}
				}
			}
			if( pkPlayer->HaveExpedition() )	// 원정대에 가입되어 있다면, 자신의 상태 변화를 알려야 함.
			{
				BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
				Packet.Push(pkPlayer->ExpeditionGuid());
				Packet.Push(pkPlayer->GetID());
				Packet.Push(pkPlayer->IsAlive());
				::SendToGlobalPartyMgr(Packet);
			}

			BM::Stream kNfyPacket(PT_U_G_RUN_ACTION);
			kNfyPacket.Push( static_cast< short >(GAN_SumitLog) );
			kNfyPacket.Push( static_cast< int >(LOG_USER_ALIVE) );
			pkPlayer->VNotify(&kNfyPacket);
		}
		pkUnit->SetAbil(AT_HP_RESERVED, 0);
		pkUnit->SetAbil(AT_MP_RESERVED, 0);
	}	

	return 0; //EActionR_Success
}

///////////////////////////////////////////////////////////
//  PgRunSkillFunction
///////////////////////////////////////////////////////////
int PgRunSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg)
{
	if(0 < pkUnit->GetAbil(AT_UNLOCK_HIDDEN_MOVE))
	{
		//움직이면 Hidden 상태 해제
		pkUnit->SetAbil(AT_UNIT_HIDDEN, 0);
	}

	return 0; //EActionR_Success
}

///////////////////////////////////////////////////////////
//  PgKOH_area_bomb
///////////////////////////////////////////////////////////
int PgKOH_area_bomb::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if(!pArg || !pkUnitArray)
	{
		return 0;
	}

	PgGround* pkGround = NULL;
	pArg->Get(ACTARG_GROUND, pkGround);
	
	PgWarGround* pkWarGnd = dynamic_cast<PgWarGround*>(pkGround);
	if(!pkWarGnd)
	{
		return 0;
	}

	PgGroundTrigger * pkTrigger = NULL;
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		if( CUnit* pkTarget = (*unit_itor).pkUnit )
		{
			if( PgGTrigger_KingOfHill const* pkTrigger = pkWarGnd->Get_KOH_Trigger(pkTarget->GetID()) )
			{
				pkWarGnd->KOH_SetTriggerUnitPoint(pkTrigger->GetID(), 0);

				UNIT_PTR_ARRAY kUnitList;
				if( CUnit * pkTriggerUnit = pkGround->GetUnit(pkTrigger->GetUnitGuid()) )
				{
					pkWarGnd->GetUnitInDistance( pkTriggerUnit->GetPos(), pkTriggerUnit->GetAbil(AT_DISTANCE), UT_PLAYER, kUnitList, 50.f );
				}

				/*UNIT_PTR_ARRAY kUnitList;
				pkWarGnd->GetTriggerInRange(pkTrigger->GetID(), UT_PLAYER, kUnitList);*/
				UNIT_PTR_ARRAY::iterator itor = kUnitList.begin();
				while( kUnitList.end() != itor )
				{
					if((*itor).pkUnit && pkUnit->GetAbil(AT_TEAM)!=(*itor).pkUnit->GetAbil(AT_TEAM))
					{
						::OnDamaged(pkUnit, (*itor).pkUnit, 0, (*itor).pkUnit->GetAbil(AT_HP), pkGround, g_kEventView.GetServerElapsedTime());
					}
					++itor;
				}
			}
		}
		++unit_itor;
	}

	return 0;
}