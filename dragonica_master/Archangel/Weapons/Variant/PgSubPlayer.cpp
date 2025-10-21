#include "stdafx.h"
#include "Variant/tabledatamanager.h"
//#include "Variant/PetSkillDataConverter.h"
#include "Global.h"
#include "PgControlDefMgr.h"
#include "PgPlayer.h"
#include "PgSubPlayer.h"

PgSubPlayer::PgSubPlayer()
:m_pkCaller(NULL)
{
}

PgSubPlayer::~PgSubPlayer()
{
}

void PgSubPlayer::Init()
{
	PgControlUnit::Init();
	m_pkCaller = NULL;
}

//HRESULT Create( CUnit * pkOwner, CONT_PET_MAPMOVE_DATA::mapped_type &kPetData )
HRESULT PgSubPlayer::Create( CUnit* pkOwner )
{
	if ( pkOwner )
	{
		SetCallerUnit(pkOwner);	// Caller설정
		SetSyncType( pkOwner->GetSyncType() );
		CUnit::SetAbil( AT_LEVEL, pkOwner->GetAbil(AT_LEVEL) );
		CUnit::SetAbil( AT_CLASS, 10000 );
		CUnit::SetAbil( AT_GENDER, pkOwner->GetAbil(AT_GENDER) );
		CUnit::SetAbil( AT_CALLER_TYPE, pkOwner->UnitType() );
		
		CUnit::SetAbil( AT_C_ATTACK_SPEED, pkOwner->GetAbil(AT_C_ATTACK_SPEED) );
		//CUnit::SetAbil( AT_C_STR, pkOwner->GetAbil(AT_C_STR) );
		//CUnit::SetAbil( AT_C_INT, pkOwner->GetAbil(AT_C_INT) );
		//CUnit::SetAbil( AT_C_CON, pkOwner->GetAbil(AT_C_CON) );
		//CUnit::SetAbil( AT_C_DEX, pkOwner->GetAbil(AT_C_DEX) );
		
		//CUnit::SetAbil( AT_C_PHY_ATTACK, pkOwner->GetAbil(AT_C_PHY_ATTACK) );
		//CUnit::SetAbil( AT_C_MAGIC_ATTACK, pkOwner->GetAbil(AT_C_MAGIC_ATTACK) );
		//CUnit::SetAbil( AT_C_HIT_SUCCESS_VALUE, pkOwner->GetAbil(AT_C_HIT_SUCCESS_VALUE) );
		//CUnit::SetAbil( AT_C_CRITICAL_SUCCESS_VALUE, pkOwner->GetAbil(AT_C_CRITICAL_SUCCESS_VALUE) );
		//CUnit::SetAbil( AT_C_CRITICAL_POWER, pkOwner->GetAbil(AT_C_CRITICAL_POWER) );
		//
		//CUnit::SetAbil( AT_C_PHY_ATTACK_MIN, pkOwner->GetAbil(AT_C_PHY_ATTACK_MIN) );
		//CUnit::SetAbil( AT_C_PHY_ATTACK_MAX, pkOwner->GetAbil(AT_C_PHY_ATTACK_MAX) );
		//CUnit::SetAbil( AT_PHY_DMG_PER2, pkOwner->GetAbil(AT_PHY_DMG_PER2) );

		//CUnit::SetAbil( AT_C_MAGIC_ATTACK_MIN, pkOwner->GetAbil(AT_C_MAGIC_ATTACK_MIN) );
		//CUnit::SetAbil( AT_C_MAGIC_ATTACK_MAX, pkOwner->GetAbil(AT_C_MAGIC_ATTACK_MAX) );
		//CUnit::SetAbil( AT_MAGIC_DMG_PER2, pkOwner->GetAbil(AT_MAGIC_DMG_PER2) );

		SetState(US_IDLE);
		SetPos( pkOwner->GetPos() );
		FrontDirection( pkOwner->FrontDirection() );
		Caller( pkOwner->GetID() );
		SetAbil( AT_OWNER_TYPE, pkOwner->GetAbil(AT_OWNER_TYPE) );

		return S_OK;
	}

	return E_FAIL;
}

void PgSubPlayer::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket( rkPacket, kWriteType );

	rkPacket.Push( m_kCommon.sLevel );// AT_LEVEL
	rkPacket.Push( m_kCommon.byGender);// AT_GENDER
	rkPacket.Push( m_kCommon.iClass);// AT_CLASS

	rkPacket.Push( m_kCaller );
//	rkPacket.Push( m_i64PeriodSecTime );
//	rkPacket.Push( m_kName );
	
	//rkPacket.Push( GetAbil(AT_C_ATTACK_SPEED) );

	//rkPacket.Push( GetAbil(AT_C_STR) );
	//rkPacket.Push( GetAbil(AT_C_INT) );
	//rkPacket.Push( GetAbil(AT_C_CON) );
	//rkPacket.Push( GetAbil(AT_C_DEX) );

	//rkPacket.Push( GetAbil(AT_C_PHY_ATTACK) );
	//rkPacket.Push( GetAbil(AT_C_MAGIC_ATTACK) );
	//rkPacket.Push( GetAbil(AT_C_HIT_SUCCESS_VALUE) );
	//rkPacket.Push( GetAbil(AT_C_CRITICAL_SUCCESS_VALUE) );
	//rkPacket.Push( GetAbil(AT_C_CRITICAL_POWER) );
	//
	//rkPacket.Push( GetAbil(AT_C_PHY_ATTACK_MIN) );
	//rkPacket.Push( GetAbil(AT_C_PHY_ATTACK_MAX) );
	//rkPacket.Push( GetAbil(AT_PHY_DMG_PER2) );
	//
	//rkPacket.Push( GetAbil(AT_C_MAGIC_ATTACK_MIN) );
	//rkPacket.Push( GetAbil(AT_C_MAGIC_ATTACK_MAX) );
	//rkPacket.Push( GetAbil(AT_MAGIC_DMG_PER2) );

	rkPacket.Push( static_cast<BYTE>(GetAbil(AT_TEAM)) );

	rkPacket.Push( GetPos().x );
	rkPacket.Push( GetPos().y );
	rkPacket.Push( GetPos().z );

	//m_kMySkill.WriteToPacket( WT_DEFAULT, rkPacket );
	//m_Skill.WriteToPacket( rkPacket, kWriteType );

	//switch( WTCHK_TYPE & kWriteType )
	//{
	//case WT_SIMPLE:
	//	{
	//		m_kInv.WriteToPacket( IT_FIT, rkPacket );
	//	}break;
	//case WT_DEFAULT:
	//case WT_DEFAULT_WITH_EXCLUDE:
	//default:
	//	{
	//		m_kInv.WriteToPacket( rkPacket, kWriteType );
	//	}break;
	//}
}

EWRITETYPE PgSubPlayer::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE const kWriteType = CUnit::ReadFromPacket(rkPacket);
	
	rkPacket.Pop( m_kCommon.sLevel );// AT_LEVEL
	rkPacket.Pop( m_kCommon.byGender);// AT_GENDER
	rkPacket.Pop( m_kCommon.iClass);// AT_CLASS
	SetBasicAbil();

	rkPacket.Pop( m_kCaller );
//	rkPacket.Pop( m_i64PeriodSecTime );
//	rkPacket.Pop( m_kName );
	
	//int iValue = 0;
	//rkPacket.Pop( iValue );	SetAbil( AT_C_ATTACK_SPEED, iValue );
	//
	//rkPacket.Pop( iValue );	SetAbil( AT_C_STR, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_INT, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_CON, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_DEX, iValue );

	//rkPacket.Pop( iValue );	SetAbil( AT_C_PHY_ATTACK, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_MAGIC_ATTACK, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_HIT_SUCCESS_VALUE, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_CRITICAL_SUCCESS_VALUE, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_CRITICAL_POWER, iValue );
	//
	//rkPacket.Pop( iValue );	SetAbil( AT_C_PHY_ATTACK_MIN, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_PHY_ATTACK_MAX, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_PHY_DMG_PER2, iValue );
	//
	//rkPacket.Pop( iValue );	SetAbil( AT_C_MAGIC_ATTACK_MIN, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_C_MAGIC_ATTACK_MAX, iValue );
	//rkPacket.Pop( iValue );	SetAbil( AT_MAGIC_DMG_PER2, iValue );
	//int iAttackSpeed = 0;
	//rkPacket.Pop( iAttackSpeed );SetAbil( AT_C_ATTACK_SPEED, iAttackSpeed );

	BYTE byTeam = 0;
	rkPacket.Pop( byTeam );	SetAbil( AT_TEAM, static_cast<int>(byTeam) );

	rkPacket.Pop( m_Pos.x );
	rkPacket.Pop( m_Pos.y );
	rkPacket.Pop( m_Pos.z );

	//m_kMySkill.ReadFromPacket( rkPacket );
	//m_Skill.ReadFromPacket( rkPacket, kWriteType );
	//m_kInv.OwnerGuid( GetID() );
	//m_kInv.OwnerName( Name() );

	//switch( WTCHK_TYPE & kWriteType )
	//{
	//case WT_SIMPLE:
	//	{
	//		m_kInv.ReadFromPacket( IT_FIT, rkPacket );
	//	}break;
	//case WT_DEFAULT:
	//case WT_DEFAULT_WITH_EXCLUDE:
	//default:
	//	{
	//		m_kInv.ReadFromPacket( rkPacket, kWriteType );
	//	}break;
	//}

	//InvenRefreshAbil();
	return kWriteType;
}

int PgSubPlayer::GetAbil(WORD const Type) const
{
	int iValue = 0;
	switch ( Type )
	{
	case AT_CLASS:
	case AT_GENDER:
	case AT_PREV_ACTION_ID:
		{
			iValue = CUnit::GetAbil( Type );
		}break;
	case AT_CALLER_TYPE:
		{
			iValue = CAbilObject::GetAbil(Type);
		}break;
	case AT_LEVEL:
	case AT_CALLER_LEVEL:
		{
			if(m_pkCaller)
			{
				iValue = m_pkCaller->GetAbil( AT_LEVEL );
			}
			else
			{
				iValue = CUnit::GetAbil( AT_LEVEL );
			}
		}break;
	default:
		{
			if(m_pkCaller)
			{
				iValue = m_pkCaller->GetAbil( Type );
			}
			else
			{
				iValue = CUnit::GetAbil( Type );
			}
		}break;
	}
	return iValue;
}

bool PgSubPlayer::SetAbil(WORD const Type, int const iInValue, bool const bIsSend, bool const bBroadcast)
{
	switch ( Type )
	{
	case AT_CLASS:
	case AT_LEVEL:
	case AT_BATTLE_LEVEL:
	case AT_PREV_ACTION_ID:
		{
			return PgControlUnit::SetAbil( Type, iInValue, bIsSend, bBroadcast );
		}break;
	case AT_HP:
		{// 죽어선 안됨
			return false;
		}break;
	default:
		{// 기본적으로 어빌을 세팅할수 없음
			if(!m_pkCaller)
			{
				return false;
			}
			m_pkCaller->SetAbil( Type, iInValue, bIsSend, bBroadcast );
			//return CUnit::SetAbil( Type, iInValue, bIsSend, bBroadcast );
		}break;
	}
	return true;
}

bool PgSubPlayer::AddAbil(WORD const Type, int const iValue)
{
	if ( 0 == iValue )
	{
		return false;
	}

	switch ( Type )
	{
	default:
		{
			return CUnit::AddAbil( Type, iValue );
		}break;
	}

	return true;
}

// SubPlayer가 Effect를 거는 경우가 있다면, PgPet의 m_bAddPetToOwner을 참고하고 작업 할껄
//protected:
bool PgSubPlayer::SetBasicAbil()
{
	return true;
}

void PgSubPlayer::SetBasicAbil(PgClassPetDef const &kPetDef)
{
}

bool PgSubPlayer::DoLevelup( SClassKey const& rkNewLevelKey )
{
	return true;
}

bool PgSubPlayer::DoBattleLevelUp( short const nBattleLv )
{
	return true;
}

void PgSubPlayer::VOnRefreshAbil()
{
//	WORD const wType[] = {	AT_LEVEL, AT_CLASS, AT_BATTLE_LEVEL };
//	SendAbiles(wType, 3, E_SENDTYPE_BROADALL);//레벨업/전직시 스탯을 모두에게 보내주어라
}

PgMySkill const* PgSubPlayer::GetCallerMySkill()
{
	if(m_pkCaller)
	{
		return m_pkCaller->GetMySkill();
	}
	return GetMySkill();
}
void PgSubPlayer::SetCallerUnit(CUnit* pkCaller) 
{ 
	m_pkCaller = dynamic_cast<PgPlayer*>(pkCaller);
}

CUnit* PgSubPlayer::GetCallerUnit()
{
	return m_pkCaller;
}