#include "stdafx.h"
#include "PgConstellationGround.h"
#include "PgConstellationMgr.h"
#include "PgGroundMgr.h"
#include "PgAction.h"

#include "Lohengrin/PacketStruct.h"

#include "Variant/PgSimpleTime.h"
#include "Variant/PgEventView.h"
#include "Variant/Inventory.h"
#include "Variant/PgquestInfo.h"
#include "Variant/Inventory.h"

using namespace Constellation;

// PgConstellationGround
PgConstellationGround::PgConstellationGround()
{
}

PgConstellationGround::~PgConstellationGround()
{
}

EOpeningState PgConstellationGround::Init(int const iMonsterControlID, bool const bMonsterGen)
{
	return PgIndun::Init(iMonsterControlID, bMonsterGen);
}

bool PgConstellationGround::Clone(PgConstellationGround * pGround)
{
	if( pGround )
	{
		return PgIndun::Clone(dynamic_cast<PgIndun*>(pGround));
	}

	return false;
}

bool PgConstellationGround::Clone(PgGround * pGround)
{
	if( pGround )
	{
		return Clone(dynamic_cast<PgConstellationGround*>(pGround));
	}

	return false;
}

void PgConstellationGround::Clear()
{
	PgIndun::Clear();
}

int PgConstellationGround::GetGroundNo() const
{
	return PgGround::GetGroundNo();
}

void PgConstellationGround::GetGenGroupKey(SGenGroupKey& rkGenGrpKey)const
{
	rkGenGrpKey.iMapNo = GetGroundNo();
	rkGenGrpKey.iBalance = GetConstellationKey().Key.kSecKey;
}

void PgConstellationGround::RecvUnitDie(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgIndun::RecvUnitDie(pUnit);

	HRESULT const hResult = g_ConstellationPartyMgr.RecvUnitDie(pUnit, this);
	if(hResult == E_INDUN_MISSION_FAIL)
	{
		m_FailTime = BM::GetTime32();
		SetState(INDUN_STATE_FAIL);
	}
}

bool PgConstellationGround::RecvRecentMapMove( PgPlayer *pkUser )
{
	if( NULL == pkUser )
	{
		return false;
	}
	
	if( this->GetAttr() & GATTR_CONSTELLATION_BOSS )
	{
	}

	return PgIndun::RecvRecentMapMove(pkUser);
}

void PgConstellationGround::SendMapLoadComplete( PgPlayer *pkUser )
{
	if(pkUser == NULL) { return; }

	PgIndun::SendMapLoadComplete(pkUser);
}

bool PgConstellationGround::IsMonsterTickOK() const
{
	if(m_eState!=INDUN_STATE_PLAY)
	{
		return false;
	}

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::const_iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while( (pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor) ) != NULL )
	{
		if( pkUnit->IsAlive() )
		{
			return true;
		}
		++kItor;
	}
	return false;
}

void PgConstellationGround::SetState(EIndunState const eState, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( eState == m_eState)
	{
		return;
	}

	if( bChangeOnlyState )
	{
		return;
	}

	switch( eState )
	{
	case INDUN_STATE_OPEN:			// 인던이 오픈되었다.
	case INDUN_STATE_WAIT:			// 인던이 유저를 기다리는 중이다.
	case INDUN_STATE_READY:			// 인던이 준비가 되었다.
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	case INDUN_STATE_PLAY:			// 인던의 유저들이 놀고 있는 중이다.
		{	
			IndunState_Play_Action();

			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	case INDUN_STATE_FAIL:
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	case INDUN_STATE_RESULT_WAIT:	// 인던의 플레이 결과를 보여주어야 한다.
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	case INDUN_STATE_RESULT:		//
		{
			m_dwAutoStateRemainTime = 0;
			m_eState = eState;
		}break;
	case INDUN_STATE_CLOSE:			// 인던이 닫혀야 한다.
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	default:
		{
		}break;
	}
}

void PgConstellationGround::IndunState_Play_Action()
{
}

void PgConstellationGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	switch( m_eState )
	{
	case INDUN_STATE_OPEN:			// 인던이 오픈되었다.
	case INDUN_STATE_WAIT:			// 인던이 유저를 기다리는 중이다.
	case INDUN_STATE_READY:			// 인던이 준비가 되었다.
		{
		}break;
	case INDUN_STATE_PLAY:			// 인던의 유저들이 놀고 있는 중이다.
		{
		}break;
	case INDUN_STATE_FAIL:
		{
			DWORD const dwNow = BM::GetTime32();
			if( dwNow - m_FailTime > 15000)
			{
				SetState(INDUN_STATE_CLOSE);
			}
		}break;
	case INDUN_STATE_RESULT_WAIT:	// 인던의 플레이 결과를 보여주어야 한다.
		{
		}break;
	case INDUN_STATE_RESULT:		//
		{
			m_kConstellationResult.OnTick1s(this, true);
		}break;
	case INDUN_STATE_CLOSE:			// 인던이 닫혀야 한다.
		{
		}break;
	default:
		{
		}break;
	}

	{
		PgIndun::OnTick1s();
	}
}

HRESULT PgConstellationGround::SetUnitDropItem(CUnit * pOwner, CUnit * pDroper, PgLogCont & LogCont )
{
	if ( INDUN_STATE_PLAY != m_eState )
	{// 그냥 S_OK로 리턴~
		return S_OK;
	}

	//Item & Gold
	switch( pDroper->GetAbil(AT_GRADE) )
	{
	//case EMGRADE_ELITE: //정예몬스터 또는 보스몬스터가 죽으면 던전 클리어
	case EMGRADE_BOSS:
		{
			SetState(INDUN_STATE_RESULT_WAIT);

			if( pOwner && pDroper && 0==pDroper->GetAbil(AT_VOLUNTARILY_DIE) && pOwner->GetAbil(AT_DUEL) <= 0)
			{
				// init drop pos
				POINT3 kDropPos = pDroper->GetPos();
				PgCreateSpreadPos kAction(kDropPos);
				POINT3BY OrientedBy = pDroper->GetOrientedVector();
				POINT3 Oriented(OrientedBy.x, OrientedBy.y, OrientedBy.z);
				Oriented.Normalize();

				int iSpreadRange = 10;
				int const iForceRange = pDroper->GetAbil(AT_SPREAD_RANGE);
				if( iForceRange )
				{
					iSpreadRange = iForceRange;
				}

				if( EMGRADE_ELITE == pDroper->GetAbil(AT_GRADE) )
				{
					iSpreadRange = 200;
				}
				kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iSpreadRange);

				PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pOwner);
				const int iDropRate = pkPC ? pkPC->GetDropRate() : 100;
				if(iDropRate > 0)
				{
					size_t const iAddDropCount = GetAddDropItemCount(pkPC);

					VEC_GUID kOwnerVec;
					GetItemOwner(pOwner, kOwnerVec);

					//AlwaysDropItem 아이템 드롭
					PgGroundUtil::ContAbilValue kAbilCont;
					PgGroundUtil::GetAlwaysDropItem(*pDroper, kAbilCont);
					PgGroundUtil::ContAbilValue::const_iterator iterItem = kAbilCont.begin();
					while( kAbilCont.end() != iterItem )
					{
						int const iItemNo = iterItem->first;
						int iItemCount = iterItem->second + iAddDropCount;

						while(iItemCount--)
						{
							kAction.PopPos(kDropPos);
							PgBase_Item kDropItem;
							if(S_OK == CreateSItem(iItemNo, 1, GIOT_FIELD, kDropItem))
							{
								InsertItemBox(kDropPos, kOwnerVec, pDroper, kDropItem, 0i64, LogCont );
							}
						}

						++iterItem;
					}
				}
			}

			return pDroper->GetAbil(AT_MON_RESULT_KILL_ALL) ? S_FALSE : S_OK;
		}break;
	default:
		{
			return PgGround::SetUnitDropItem(pOwner, pDroper, LogCont );
		}break;
	}
	return S_OK;
}

HRESULT PgConstellationGround::InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo, 
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller, 
		bool DropAllItem, int EnchantGradeNo, 
		ECREATE_HP_TYPE const Create_HP_Type)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	HRESULT const Ret = PgGround::InsertMonster(GenInfo, MonNo, OutGuid, Caller, DropAllItem, EnchantGradeNo, Create_HP_Type);
	if( SUCCEEDED(Ret) )
	{
		CUnit * pMonster = GetUnit(OutGuid);
		if( pMonster )
		{
			switch( pMonster->GetAbil(AT_GRADE) )
			{
			case EMGRADE_ELITE:
			case EMGRADE_BOSS:
			default:
				{
				}break;
			}
		}
	}

	return Ret;
}


void PgConstellationGround::ProcessGateWayUnLock(HRESULT const hRet, CUnit * pkCaster, BM::Stream & rkAddonPacket)
{
	int iErrorMessageNo = 0;
	rkAddonPacket.Pop( iErrorMessageNo );

	if( S_OK == hRet )
	{
		if(pkCaster)
		{
			pkCaster->VNotify( &rkAddonPacket );
		}
	}
	else
	{
		BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, (iErrorMessageNo > 0 ? iErrorMessageNo : 18998) );
		kPacket.Push( static_cast< BYTE >(EL_Warning) );
		Broadcast(kPacket);

		m_FailTime = BM::GetTime32();
	}
}