#include "stdafx.h"
#include "Variant/tabledatamanager.h"
#include "Variant/PetSkillDataConverter.h"
#include "Global.h"
#include "PgControlDefMgr.h"
#include "PgPlayer.h"
#include "PgPet.h"


PgPet::PgPet(void)
:	m_byPetType(EPET_TYPE_1)
,	m_i64PeriodSecTime(_I64_MAX)
,	m_i64HealthSecTime(0i64)
,	m_i64MentalSecTime(0i64)
,	m_i64NextExpUpSecTime(0i64)
,	m_bAddPetToOwner(false)
,	m_pkCaller(NULL)
{
}

PgPet::~PgPet(void)
{
}

void PgPet::Init()
{
	PgControlUnit::Init();
	m_byPetType = EPET_TYPE_1;
	m_i64PeriodSecTime = _I64_MAX;
	m_i64HealthSecTime = 0i64;
	m_i64MentalSecTime = 0i64;
	m_i64NextExpUpSecTime = 0i64;
	m_bAddPetToOwner = false;
	m_pkCaller = NULL;
}

HRESULT PgPet::Create( PgBase_Item const &kPetItem, CUnit * pkOwner, CONT_PET_MAPMOVE_DATA::mapped_type &kPetData )
{
	if ( pkOwner )
	{
		PgItem_PetInfo *pkPetInfo = NULL;
		if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
		{
			SetCallerUnit(pkOwner);	// 주인 참조

			CUnit::SetAbil( AT_LEVEL, static_cast<int>(pkPetInfo->ClassKey().nLv) );
			CUnit::SetAbil( AT_CLASS, pkPetInfo->ClassKey().iClass );
			CUnit::SetAbil( AT_CALLER_TYPE, pkOwner->UnitType() );

			GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
			PgClassPetDef kPetDef;
			SClassKey const kPetClassKey(GetClassKey());
			if ( true == kClassPetDefMgr.GetDef( kPetClassKey, &kPetDef ) )
			{
				SetAbil(AT_PET_CANNOT_RIDING_ATTACK, kPetDef.GetAbil(AT_PET_CANNOT_RIDING_ATTACK));

				Name( pkPetInfo->Name() );
				m_kInv.OwnerGuid( GetID() );
				m_kInv.OwnerName( Name() );

				SetBasicAbil(kPetDef);

				SetAbil64( AT_EXPERIENCE, pkPetInfo->Exp() );
				m_i64PeriodSecTime = ( (kPetItem.EnchantInfo().IsTimeLimit()) ? kPetItem.GetDelTime() : _I64_MAX );

				SetAbil( AT_MP, ( -1 < kPetData.iMP ? kPetData.iMP : pkPetInfo->GetAbil(AT_MP)) );
				SetAbil( AT_COLOR_INDEX, pkPetInfo->GetAbil(AT_COLOR_INDEX) );

				PgItem_PetInfo::SStateValue const kHealth( pkPetInfo->GetState_Health() );
				PgItem_PetInfo::SStateValue const kMental( pkPetInfo->GetState_Mental() );
				SetState_Health( kHealth );
				SetState_Mental( kMental );

				// Pet Skill
				CONT_DEFCLASS_PET_SKILL const *pkDefPetSkill = NULL;
				g_kTblDataMgr.GetContDef(pkDefPetSkill);

				m_kMySkill.Init();
				CONT_DEFCLASS_PET_SKILL::const_iterator skill_itr = pkDefPetSkill->find( kPetDef.GetSkillDefID() );
				if ( skill_itr != pkDefPetSkill->end() )
				{
					CONT_DEFCLASS_PET_SKILL::mapped_type const &kContElement = skill_itr->second;
					size_t const index = kContElement.find( PgDefClassPetSkillFinder(kPetClassKey.nLv) );
					if ( BM::PgApproximate::npos != index )
					{
						CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kElement = kContElement.at(index);
						PgPetSkillDataSet::InitSkill( kElement, this, pkPetInfo );
					}
				}
				m_Skill.InitCoolTime( kPetData.kSkillCoolTime );

				m_kInv.Swap( kPetData.kInventory );
				InvenRefreshAbil();
					
				if ( kPetData.kAddOnPacket.Size() )
				{
					kPetData.kAddOnPacket.Pop( m_i64NextExpUpSecTime );
					m_kEffect.ReadFromPacket( kPetData.kAddOnPacket, false );
				}
				else
				{
					switch( GetPetType() )
					{
					case EPET_TYPE_1:
						{
							m_i64NextExpUpSecTime = _I64_MAX;
						}break;
					case EPET_TYPE_2:
					case EPET_TYPE_3: //성장형펫과 라이딩펫은 동일하게 처리
						{
							__int64 const i64CurTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
							m_i64NextExpUpSecTime = i64CurTime + ms_i64ExpUpPeriodSecTime;
						}break;
					}
				}

				SetState(US_IDLE);
				SetPos( pkOwner->GetPos() );
				FrontDirection( pkOwner->FrontDirection() );
				Caller( pkOwner->GetID() );
				SetAbil( AT_OWNER_TYPE, pkOwner->GetAbil(AT_OWNER_TYPE) );
				return S_OK;
			}
		}	
	}

	return E_FAIL;
}

void PgPet::SetCallerUnit(CUnit* pkCaller)
{
	m_pkCaller = dynamic_cast<PgPlayer*>(pkCaller);
}

void PgPet::SetState_Health( PgItem_PetInfo::SStateValue const &kValue )
{
	__int64 const i64NowSecTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
	m_i64HealthSecTime = static_cast<__int64>(kValue.Time()) * (CGameTime::HOUR / CGameTime::SECOND);
	if ( i64NowSecTime < m_i64HealthSecTime )
	{
		SetAbil( AT_HEALTH, kValue.Value() );
	}
	else
	{
		SetAbil( AT_HEALTH, 0 );
		m_i64HealthSecTime = 0i64;
	}
}

void PgPet::SetState_Mental( PgItem_PetInfo::SStateValue const &kValue )
{
	__int64 const i64NowSecTime = g_kEventView.GetLocalSecTime( CGameTime::SECOND );
	m_i64MentalSecTime = static_cast<__int64>(kValue.Time()) * (CGameTime::HOUR / CGameTime::SECOND);
	if ( i64NowSecTime < m_i64MentalSecTime )
	{
		SetAbil( AT_MENTAL, kValue.Value() );
	}
	else
	{
		SetAbil( AT_MENTAL, 0 );
		m_i64MentalSecTime = 0i64;
	}
}

int PgPet::AutoHeal( unsigned long ulElapsedTime, float const fMultiplier )
{
	// 건강은 MP 회복율이다.
	float const fAddMultiplier = static_cast<float>(GetAbil(AT_HEALTH)) / 100.0f;
	return CUnit::AutoHeal( ulElapsedTime, fMultiplier + fAddMultiplier );
}

BYTE PgPet::UpdateNextConditionCheckSecTime( __int64 const i64CurSecTime )
{
	BYTE byRet = PET_STATE_CHK_NONE;

	if ( m_i64NextExpUpSecTime <= i64CurSecTime )
	{
		__int64 i64Gap = i64CurSecTime - m_i64NextExpUpSecTime;
		i64Gap /= ms_i64ExpUpPeriodSecTime;
		++i64Gap;
		i64Gap *= ms_i64ExpUpPeriodSecTime;
		m_i64NextExpUpSecTime += i64Gap;

		byRet |= PET_STATE_CHK_EXP;
	}

	if ( (0i64!=m_i64HealthSecTime) && (m_i64HealthSecTime <= i64CurSecTime) )
	{
		SetAbil( AT_HEALTH, 0 );
		m_i64HealthSecTime = 0i64;
	}

	if ( (0i64!=m_i64MentalSecTime) && (m_i64MentalSecTime <= i64CurSecTime) )
	{
		SetAbil( AT_MENTAL, 0 );
		m_i64MentalSecTime = 0i64;
	}

	return byRet;
}

bool PgPet::SetBasicAbil(void)
{
	GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
	PgClassPetDef kPetDef;
	if ( true == kClassPetDefMgr.GetDef( SClassKey( m_kCommon.iClass, m_kCommon.sLevel ), &kPetDef ) )
	{
		SetBasicAbil( kPetDef );
		return true;
	}
	return false;
}

void PgPet::SetBasicAbil( PgClassPetDef const &kPetDef )
{
	m_kBasic.iMaxHP = std::max( kPetDef.GetAbil(AT_MAX_HP), 1 );		// Basic Max HP (DB value)
	m_kBasic.iHPRecoveryInterval = kPetDef.GetAbil(AT_HP_RECOVERY_INTERVAL);	// Basic HP Recovery Speed
	m_kBasic.sHPRecovery = kPetDef.GetAbil(AT_HP_RECOVERY);
	m_kBasic.iMaxMP = kPetDef.GetAbil(AT_MAX_MP);		// Basic Max MP
	m_kBasic.iMPRecoveryInterval = kPetDef.GetAbil(AT_MP_RECOVERY_INTERVAL);	// Basic MP Recovery Speed
	m_kBasic.sMPRecovery = kPetDef.GetAbil(AT_MP_RECOVERY);
	m_kBasic.sInitStr = kPetDef.GetAbil(AT_STR);			// Basic Strength
	m_kBasic.sInitInt = kPetDef.GetAbil(AT_INT);			// Basic Intelligence
	m_kBasic.sInitCon = kPetDef.GetAbil(AT_CON);			// Basic Constitution
	m_kBasic.sInitDex = kPetDef.GetAbil(AT_DEX);			// Basic Dexterity
	m_kBasic.sMoveSpeed = kPetDef.GetAbil(AT_MOVESPEED);		// Basic Move speed
	m_kBasic.sPhysicsDefence = kPetDef.GetAbil(AT_PHY_DEFENCE);	// Basic Physics Defence
	m_kBasic.sMagicDefence = kPetDef.GetAbil(AT_MAGIC_DEFENCE);	// Basic Magic Defence
	m_kBasic.sAttackSpeed = kPetDef.GetAbil(AT_ATTACK_SPEED);		// Basic Attack speed
	m_kBasic.sBlockRate = kPetDef.GetAbil(AT_BLOCK_SUCCESS_VALUE);		// Basic Block-attack rate
	m_kBasic.sDodgeRate = kPetDef.GetAbil(AT_DODGE_SUCCESS_VALUE);		// Basic Dodge-attack rate
	m_kBasic.sCriticalRate = kPetDef.GetAbil(AT_CRITICAL_SUCCESS_VALUE);	// AT_CRITICAL_SUCCESS_VALUE : Basic Critical Rate
	m_kBasic.sCriticalPower = kPetDef.GetAbil(AT_CRITICAL_POWER);	// AT_CRITICAL_POWER : Basic Critical Power
	m_kBasic.sHitRate = kPetDef.GetAbil(AT_HIT_SUCCESS_VALUE);
	m_kBasic.iAIType = kPetDef.GetAbil(AT_AI_TYPE);	// AT_AI_TYPE : AI Type
//	SetAbil(AT_HP_GAUGE_TYPE, kPetDef.GetAbil( AT_HP_GAUGE_TYPE));
	SetAbil(AT_PHY_ATTACK_MAX, kPetDef.GetAbil(AT_PHY_ATTACK_MAX));	//공격력
	SetAbil(AT_PHY_ATTACK_MIN, kPetDef.GetAbil(AT_PHY_ATTACK_MIN));
	SetAbil(AT_MAGIC_ATTACK_MAX, kPetDef.GetAbil(AT_MAGIC_ATTACK_MAX));
	SetAbil(AT_MAGIC_ATTACK_MIN, kPetDef.GetAbil(AT_MAGIC_ATTACK_MIN));
//	m_kBasic.sInvenSize = kDef.GetAbil(AT_INVEN_SIZE);		// AT_INVEN_SIZE
//	m_kBasic.sEquipsSize = kDef.GetAbil(AT_EQUIPS_SIZE);		// AT_EQUIPS_SIZE

	CUnit::SetAbil(AT_GENDER, GWL_PET_MALE );

	m_byPetType = kPetDef.GetPetType();
	SetAbil( AT_HP, GetAbil(AT_C_MAX_HP) );// 펫은 HP 유동없음
}

void PgPet::WriteToPacket_SPetMapMoveData( BM::Stream &rkPacket )
{//PetID + SPetMapMoveData::WriteToPacket() 과 동일하게
	rkPacket.Push( GetID() );
	m_kInv.WriteToPacket( rkPacket, WT_DEFAULT );
	m_Skill.WriteToPacket_CoolTimeMap( rkPacket );
	rkPacket.Push( GetAbil( AT_MP ) );

	size_t const iWRPos = rkPacket.WrPos();
	size_t iWRSize = 0;
	rkPacket.Push( iWRSize );
	rkPacket.Push( GetNextExpUpSecTime() );
	m_kEffect.WriteToPacket( rkPacket, false );
	iWRSize = rkPacket.WrPos() - iWRPos - sizeof(iWRSize);
	rkPacket.ModifyData( iWRPos, &iWRSize, sizeof(iWRSize) );
}

void PgPet::WriteToPacket( BM::Stream &rkPacket, EWRITETYPE const kWriteType )const
{
	CUnit::WriteToPacket( rkPacket, kWriteType );

	rkPacket.Push( m_kCommon.sLevel );// AT_LEVEL

	rkPacket.Push( m_kCaller );
	rkPacket.Push( m_i64PeriodSecTime );
	rkPacket.Push( Name() );

	rkPacket.Push( GetAbil64(AT_EXPERIENCE) );

//	rkPacket.Push( static_cast<BYTE>(GetAbil(AT_TEAM)) );

	rkPacket.Push( GetPos().x );
	rkPacket.Push( GetPos().y );
	rkPacket.Push( GetPos().z );

//	rkPacket.Push( GetAbil(AT_HP) );
	rkPacket.Push( GetAbil(AT_MP) );

	rkPacket.Push( static_cast<char>(GetAbil(AT_HEALTH)) );
	rkPacket.Push( static_cast<char>(GetAbil(AT_MENTAL)) );
	rkPacket.Push( static_cast<char>(GetAbil( AT_COLOR_INDEX )) );
	rkPacket.Push( static_cast<char>(GetAbil( AT_MOUNTED_PET_SKILL )) );
	rkPacket.Push( static_cast<char>(GetAbil( AT_PET_CANNOT_RIDING_ATTACK )) );

	rkPacket.Push( m_i64PeriodSecTime );
	rkPacket.Push( m_i64HealthSecTime );
	rkPacket.Push( m_i64MentalSecTime );

	m_kMySkill.WriteToPacket( WT_DEFAULT, rkPacket );
	m_Skill.WriteToPacket( rkPacket, kWriteType );

	switch( WTCHK_TYPE & kWriteType )
	{
	case WT_SIMPLE:
		{
			m_kInv.WriteToPacket( IT_FIT, rkPacket );
		}break;
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
	default:
		{
			m_kInv.WriteToPacket( rkPacket, kWriteType );
		}break;
	}
}

EWRITETYPE PgPet::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE const kWriteType = CUnit::ReadFromPacket(rkPacket);
	
	rkPacket.Pop( m_kCommon.sLevel );// AT_LEVEL

	SetBasicAbil();

	rkPacket.Pop( m_kCaller );
	rkPacket.Pop( m_i64PeriodSecTime );
	rkPacket.Pop( m_kName );

	__int64 i64Exp = 0i64;
	rkPacket.Pop( i64Exp );	SetAbil64( AT_EXPERIENCE, i64Exp );

//	BYTE byTeam = 0;
//	rkPacket.Pop( byTeam );	SetAbil( AT_TEAM, static_cast<int>(byTeam) );

	rkPacket.Pop( m_Pos.x );
	rkPacket.Pop( m_Pos.y );
	rkPacket.Pop( m_Pos.z );

	int iValue = 0;
//	rkPacket.Pop( iValue );	SetAbil( AT_HP, iValue );
	rkPacket.Pop( iValue );	SetAbil( AT_MP, iValue );

	char cValue = 0;
	rkPacket.Pop( cValue );SetAbil( AT_HEALTH, static_cast<int>(cValue) );
	rkPacket.Pop( cValue );SetAbil( AT_MENTAL, static_cast<int>(cValue) );
	rkPacket.Pop( cValue );SetAbil( AT_COLOR_INDEX, static_cast<int>(cValue) );
	rkPacket.Pop( cValue );SetAbil( AT_MOUNTED_PET_SKILL, static_cast<int>(cValue) );
	rkPacket.Pop( cValue );SetAbil( AT_PET_CANNOT_RIDING_ATTACK, static_cast<int>(cValue) );

	rkPacket.Pop( m_i64PeriodSecTime );
	rkPacket.Pop( m_i64HealthSecTime );
	rkPacket.Pop( m_i64MentalSecTime );

	m_kMySkill.ReadFromPacket( rkPacket );
	m_Skill.ReadFromPacket( rkPacket, kWriteType );
	m_kInv.OwnerGuid( GetID() );
	m_kInv.OwnerName( Name() );

	switch( WTCHK_TYPE & kWriteType )
	{
	case WT_SIMPLE:
		{
			m_kInv.ReadFromPacket( IT_FIT, rkPacket );
		}break;
	case WT_DEFAULT:
	case WT_DEFAULT_WITH_EXCLUDE:
	default:
		{
			m_kInv.ReadFromPacket( rkPacket, kWriteType );
		}break;
	}

	InvenRefreshAbil();
	return kWriteType;
}

int PgPet::GetAbil(WORD const Type) const
{
	int iValue = 0;
	switch ( Type )
	{
	case AT_BASE_CLASS:
		{
			iValue = GetAbil( AT_CLASS );
			if ( EPET_TYPE_2 == GetPetType() || EPET_TYPE_3 == GetPetType() )
			{
				iValue = PgClassPetDef::GetBaseClass( iValue );
			}
		}break;
	case AT_LEVEL_RANK:
		{
			iValue = static_cast<int>(m_kLevelRank);
		}break;
	case AT_ENABLE_AUTOHEAL:
		{
			iValue = AUTOHEAL_PET;
		}break;
	case AT_INVEN_PET_ADD_TIME_EXP_RATE:
	case AT_INVEN_PET_ADD_HUNT_EXP_RATE:
		{
			iValue = m_kInv.GetAbil( Type );
		}break;
	case AT_GIVE_TO_CALLER_ADD_EXP_RATE:
		{
			iValue = CUnit::GetAbil( Type ) + m_kInv.GetAbil( Type );
		}break;
	default:
		{
			if(m_pkCaller && GetPetType() == EPET_TYPE_3)
			{ //라이딩펫 경우 기본공격력에 관련된 어빌은 모두 주인에게서 가져온다
				//스킬이 공격 속성이든 마법 속성이든 둘중의 높은 것으로 가져오기로 약속 되었다.
				if(	(Type >= AT_PHY_ATTACK_MAX && Type <= AT_C_PHY_ATTACK_MIN) ||
					(Type >= AT_MAGIC_ATTACK_MAX && Type <= AT_C_MAGIC_ATTACK_MIN)
					//Type == AT_C_ABS_ADDED_DMG_PHY ||
					//Type == AT_C_ABS_ADDED_DMG_MAGIC ||
					)
				{ //라이딩펫 공격력=주인의 공격력*0.6
					int iCompareType = Type;
					if(Type >= AT_PHY_ATTACK_MAX && Type <= AT_C_PHY_ATTACK_MIN)
					{
						iCompareType += 110;
					}
					else
					{
						iCompareType -= 110;
					}
					WORD iMaxValue = std::max(m_pkCaller->GetAbil(Type), m_pkCaller->GetAbil(iCompareType));
					iValue = static_cast<int>(iMaxValue *  0.6f);

					//float fAmplifyDmg = static_cast<float>(CUnit::GetAbil(AT_R_PHY_ATTACK));
					float fAmplifyDmg = static_cast<float>(m_kInv.GetAbil(AT_R_PHY_ATTACK) + GetEffectMgr().GetAbil(AT_R_PHY_ATTACK));
					if(fAmplifyDmg > 0) //스킬이나 아이템으로 부터 대미지 증폭을 계산
					{
						fAmplifyDmg /= 10000.f;
						iValue += static_cast<int>(iValue * fAmplifyDmg);
					}
				}
				else if(Type >= AT_MAGIC_ATTACK && Type <= AT_C_PHY_ATTACK)
				{
					int iCompareType = Type;
					if(Type >= AT_MAGIC_ATTACK && Type <= AT_C_MAGIC_ATTACK)
					{
						iCompareType += 10;
					}
					else
					{
						iCompareType -= 10;
					}
					WORD iMaxValue = std::max(m_pkCaller->GetAbil(Type), m_pkCaller->GetAbil(iCompareType));
					iValue = static_cast<int>(iMaxValue *  0.6f);

					//float fAmplifyDmg = static_cast<float>(CUnit::GetAbil(AT_R_MAGIC_ATTACK));
					float fAmplifyDmg = static_cast<float>(m_kInv.GetAbil(AT_R_MAGIC_ATTACK) + GetEffectMgr().GetAbil(AT_R_MAGIC_ATTACK));
					if(fAmplifyDmg > 0)
					{
						fAmplifyDmg /= 10000.f;
						iValue += static_cast<int>(iValue * fAmplifyDmg);
					}
				}
				else if(Type >= AT_HIT_SUCCESS_VALUE && Type <= AT_C_HIT_SUCCESS_VALUE)
				{
					iValue = m_pkCaller->GetAbil( Type );
				}
				else
				{
					iValue = CUnit::GetAbil( Type );
				}
			}
			else if(m_pkCaller != NULL && m_pkCaller->IsCompleteInit() == true && (Type == AT_DUEL || Type == AT_TEAM) )
			{ //PVP 관련 어빌값들은 주인에게 가져옴
				iValue = m_pkCaller->GetAbil(Type);
			}
			else
			{
				iValue = CUnit::GetAbil( Type );
			}
		}break;
	}
	return iValue;
}

bool PgPet::SetAbil(WORD const Type, int const iInValue, bool const bIsSend, bool const bBroadcast )
{
	switch ( Type )
	{
	case AT_CLASS:
	case AT_LEVEL:
	case AT_BATTLE_LEVEL:
		{
			return PgControlUnit::SetAbil( Type, iInValue, bIsSend, bBroadcast );
		}break;
	case AT_HEALTH:
		{// 건강은 MP 회복율이다....
			int iValue = iInValue;
			PgItem_PetInfo::CheckStateAbil( iValue, true );
			CAbilObject::SetAbil( Type, iValue );
		}break;
	case AT_MENTAL:
		{// 지력은 스킬의 효율 증가이다.
			int iValue = iInValue;
			PgItem_PetInfo::CheckStateAbil( iValue, true );

			int const iOldValue = CAbilObject::GetAbil( Type );
			int const iChangeValue = iValue - iOldValue;
			if ( 0 != iChangeValue )
			{
				CAbilObject::AddAbil( AT_SKILL_EFFICIENCY, iChangeValue * 100 );
				CAbilObject::SetAbil( Type, iValue );
			}
		}break;
	default:
		{
			return CUnit::SetAbil( Type, iInValue, bIsSend, bBroadcast );
		}break;
	}

	return true;
}

bool PgPet::AddAbil(WORD const Type, int const iValue)
{
	if ( 0 == iValue )
	{
		return false;
	}

	switch ( Type )
	{
	case AT_HEALTH:
	case AT_MENTAL:
		{
			return SetAbil( Type, GetAbil( Type ) + iValue );
		}break;
	default:
		{
			return CUnit::AddAbil( Type, iValue );
		}break;
	}

	return true;
}

bool PgPet::DoLevelup( SClassKey const& rkNewLevelKey )
{
	SClassKey const kOldClassKey = GetClassKey();

	if ( kOldClassKey != rkNewLevelKey )
	{
		CUnit::SetAbil( AT_CLASS, rkNewLevelKey.iClass );
		CUnit::SetAbil( AT_LEVEL, rkNewLevelKey.nLv );

		GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
		PgClassPetDef kPetDef_New;
		if ( true == kClassPetDefMgr.GetDef( rkNewLevelKey, &kPetDef_New ) )
		{
			SetBasicAbil( kPetDef_New );
			CalculateInitAbil();

			SetAbil( AT_HP, GetAbil(AT_C_MAX_HP), false, false );
			SetAbil( AT_MP, GetAbil(AT_C_MAX_MP), false, false );

			{// Pet Skill Update
				CONT_DEFCLASS_PET_SKILL const *pkDefPetSkill = NULL;
				g_kTblDataMgr.GetContDef(pkDefPetSkill);

				m_kMySkill.Init();
				CONT_DEFCLASS_PET_SKILL::const_iterator skill_itr = pkDefPetSkill->find( kPetDef_New.GetSkillDefID() );
				if ( skill_itr != pkDefPetSkill->end() )
				{
					CONT_DEFCLASS_PET_SKILL::mapped_type const &kContElement = skill_itr->second;
					size_t const index = kContElement.find( PgDefClassPetSkillFinder(rkNewLevelKey.nLv) );
					if ( BM::PgApproximate::npos != index )
					{
						CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kElement = kContElement.at(index);
						PgPetSkillDataSet::InitSkill( kElement, this, NULL );
					}
				}
			}
				
			if( kOldClassKey.nLv != rkNewLevelKey.nLv )//변하면
			{
				VOnLvUp();
			}

			if( kOldClassKey.iClass != rkNewLevelKey.iClass )//변하면
			{
				VOnChangeClass();
			}

			VOnRefreshAbil();//캐릭터 어빌을 Refresh 한다.

			return true;
		}
		// rollback
		CUnit::SetAbil( AT_CLASS, kOldClassKey.iClass );
		CUnit::SetAbil( AT_LEVEL, kOldClassKey.nLv );
	}

	return false;
}

bool PgPet::DoBattleLevelUp( short const nBattleLv )
{
	return false;
}

void PgPet::VOnRefreshAbil()
{
	WORD const wType[] = {	AT_LEVEL, AT_CLASS, AT_BATTLE_LEVEL };
	SendAbiles(wType, 3, E_SENDTYPE_BROADALL);//레벨업/전직시 스탯을 모두에게 보내주어라
}
