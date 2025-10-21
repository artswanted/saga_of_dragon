#include "stdafx.h"
#include "Global.h"
#include "PgControlDefMgr.h"
#include "PgObjectUnit.h"

PgObjectUnit::PgObjectUnit()
:	m_kAttributes(EObj_Attr_None)
{
}

PgObjectUnit::~PgObjectUnit()
{
}

void PgObjectUnit::Init()
{
	CUnit::Init();
	m_kGroupIndex.Clear();
	Attributes(EObj_Attr_None);
}

HRESULT PgObjectUnit::Create(const void *pkInfo)
{	
	if( !pkInfo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T(" pkObjInfo is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	
	Init();

	SObjUnitCreateInfo const *pkObjInfo = (SObjUnitCreateInfo const *)pkInfo;
	SetID(pkObjInfo->kGuid);
	SetAbil(AT_CLASS, pkObjInfo->iID);
	SetPos(pkObjInfo->pt3Pos);
	m_kGroupIndex.Set(pkObjInfo->kGroup);
//	m_kName = pkObjInfo->wstrName;
	Attributes(pkObjInfo->dwAttribute);

	if ( !SetBasicAbil() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	SetAbil(AT_HP, GetAbil(AT_C_MAX_HP));
//	SetAbil(AT_DP, GetAbil(AT_C_MAX_DP));
	GoalPos(POINT3(0, 0, 0));

	SetState(US_IDLE);
	SetDelay(0);
	ActionID(0);
	SetSync(true);
	return S_OK;
}

bool PgObjectUnit::SetBasicAbil()
{
	GET_DEF(PgObjectUnitDefMgr, kObjectUnitDefMgr)
	PgObjectUnitDef const *pkDef = kObjectUnitDefMgr.GetDef(GetAbil(AT_CLASS));
	if ( !pkDef )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kCommon.sLevel = pkDef->GetAbil(AT_LEVEL);
	m_kCommon.byGender = 0;
	m_kCommon.iRace = pkDef->GetAbil(AT_RACE);
	m_kBasic.iMaxHP = pkDef->GetAbil(AT_MAX_HP);		// Basic Max HP (DB value)
	m_kBasic.iHPRecoveryInterval = pkDef->GetAbil(AT_HP_RECOVERY_INTERVAL);	// Basic HP Recovery Speed
	m_kBasic.sHPRecovery = pkDef->GetAbil(AT_HP_RECOVERY);
	m_kBasic.iMaxMP = pkDef->GetAbil(AT_MAX_MP);		// Basic Max MP
	m_kBasic.iMPRecoveryInterval = pkDef->GetAbil(AT_MP_RECOVERY_INTERVAL);	// Basic MP Recovery Speed
	m_kBasic.sMPRecovery = pkDef->GetAbil(AT_MP_RECOVERY);
// 	SetAbil(AT_MAX_DP,pkDef->GetAbil(AT_MAX_DP));
// 	SetAbil(AT_DP_RECOVERY_INTERVAL,pkDef->GetAbil(AT_DP_RECOVERY_INTERVAL));
// 	SetAbil(AT_DP_RECOVERY,pkDef->GetAbil(AT_DP_RECOVERY));
	m_kBasic.sInitStr = pkDef->GetAbil(AT_STR);			// Basic Strength
	m_kBasic.sInitInt = pkDef->GetAbil(AT_INT);			// Basic Intelligence
	m_kBasic.sInitCon = pkDef->GetAbil(AT_CON);			// Basic Constitution
	m_kBasic.sInitDex = pkDef->GetAbil(AT_DEX);			// Basic Dexterity
	m_kBasic.sMoveSpeed = pkDef->GetAbil(AT_MOVESPEED);		// Basic Move speed
	m_kBasic.sPhysicsDefence = pkDef->GetAbil(AT_PHY_DEFENCE);	// Basic Physics Defence
	m_kBasic.sMagicDefence = pkDef->GetAbil(AT_MAGIC_DEFENCE);	// Basic Magic Defence
	m_kBasic.sAttackSpeed = pkDef->GetAbil(AT_ATTACK_SPEED);		// Basic Attack speed
	m_kBasic.sBlockRate = pkDef->GetAbil(AT_BLOCK_SUCCESS_VALUE);		// Basic Block-attack rate
	m_kBasic.sDodgeRate = pkDef->GetAbil(AT_DODGE_SUCCESS_VALUE);		// Basic Dodge-attack rate
	m_kBasic.sCriticalRate = pkDef->GetAbil(AT_CRITICAL_SUCCESS_VALUE);	// AT_CRITICAL_SUCCESS_VALUE : Basic Critical Rate
	m_kBasic.sCriticalPower = pkDef->GetAbil(AT_CRITICAL_POWER);	// AT_CRITICAL_POWER : Basic Critical Power
	m_kBasic.sHitRate = pkDef->GetAbil(AT_HIT_SUCCESS_VALUE);
	m_kBasic.iAIType = pkDef->GetAbil(AT_AI_TYPE);	// AT_AI_TYPE : AI Type
	SetAbil( AT_HP_GAUGE_TYPE, pkDef->GetAbil(AT_HP_GAUGE_TYPE) );
//	m_kBasic.sInvenSize = pkDef->GetAbil(AT_INVEN_SIZE);		// AT_INVEN_SIZE
//	m_kBasic.sEquipsSize = pkDef->GetAbil(AT_EQUIPS_SIZE);		// AT_EQUIPS_SIZE
	m_fHeight = pkDef->Height();
	return true;
}

int PgObjectUnit::GetAbil(WORD const Type) const
{
	int iValue = 0;
	switch(Type)
	{
	case AT_MOVE_RANGE:		
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0; 
		}break;
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Plz Use GetAbil64") );
		}break;
	case AT_ATTRIBUTE:
		{
			iValue = Attributes();
		}break;
	default:
		{
			iValue = CUnit::GetAbil(Type);
			if (iValue == 0 && !CAbilObject::HasAbil(Type))	// 진짜로 Value==0일때와, 아직 값을 안 읽어서 0 일때를 구별해 주어야 한다.
			{
				int const iObjectNo = CUnit::GetAbil(AT_CLASS);
				GET_DEF(PgObjectUnitDefMgr, kObjectUnitDef)
				PgObjectUnitDef const *pDef = kObjectUnitDef.GetDef(iObjectNo);
				if(pDef)
				{
					WORD wTemp = Type;
					if (IsCalculatedAbil(Type))
					{
						wTemp = GetBasicAbil(Type);
					}
					iValue = pDef->GetAbil(wTemp);
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" pDef Is Null") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ObjectUnitDef Failed!"));
				}
			}
		}break;
	}
	return iValue;
}

__int64 PgObjectUnit::GetAbil64(WORD const eAT)const
{
	__int64 iValue = 0;
	iValue = CUnit::GetAbil64(eAT);
	if (iValue == 0)
	{
		int const iObjectNo = GetAbil(AT_CLASS);
		GET_DEF(PgObjectUnitDefMgr, kObjectUnitDef)
		PgObjectUnitDef const *pDef = kObjectUnitDef.GetDef(iObjectNo);
		if(pDef)
		{
			iValue = pDef->GetAbil64(eAT);
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" pDef Is Null") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ObjectUnitDef Failed!"));
		}
	}
	return iValue;
}


bool PgObjectUnit::SetAbil(WORD const Type, int const iValue, bool const bIsSend, bool const bBroadcast)
{
	bool bIntercept = true;
	switch(Type)
	{
	case AT_MOVE_RANGE:{ }break;
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
	case AT_EXPERIENCE:
	case AT_MONEY:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" int64 can't put on SetAbil(use SetAbil64)") );
		}break;
	case AT_ATTRIBUTE:
		{
			Attributes(iValue);
		}break;
	default:
		{
			bIntercept = false;
			CUnit::SetAbil(Type, iValue, bIsSend, bBroadcast);
		}break;
	}

	if(bIntercept)
	{
		DWORD dwSendFlag = 0;

		if(bIsSend){dwSendFlag |= E_SENDTYPE_SELF;}
		if(bBroadcast){dwSendFlag |= E_SENDTYPE_BROADCAST;}

		if(dwSendFlag)
		{
			SendAbil((EAbilType)Type, dwSendFlag);
		}

		OnSetAbil(Type, iValue);
	}
	return true;
}

bool PgObjectUnit::OnDamage( int iPower, int& iHP )
{
	if( this->GetAbil(AT_INVINCIBLE2) > 0 )
	{
		iHP = this->GetAbil(AT_HP);
		m_kEffect.Damaged( true );
		return true;
	}
	return CUnit::OnDamage( iPower, iHP );
}

void PgObjectUnit::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket, kWriteType);

	rkPacket.Push(GetPos());
	rkPacket.Push(GetAbil(AT_C_MAX_HP));
	rkPacket.Push(GetAbil(AT_HP));
	rkPacket.Push(m_kGroupIndex);
	rkPacket.Push(Attributes());

	rkPacket.Push(static_cast<BYTE>(GetAbil(AT_TEAM)));//팀
	rkPacket.Push(static_cast<BYTE>(GetAbil(AT_MON_DEFAULT_ANGLE)));

	rkPacket.Push(GetAbil(AT_CANNOT_DAMAGE));
}

EWRITETYPE PgObjectUnit::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);

	POINT3 ptPos;
	int iHP = 0, iMaxHP = 0;
	int iValue = 0;

	rkPacket.Pop(ptPos);
	rkPacket.Pop(iMaxHP);
	rkPacket.Pop(iHP);
	rkPacket.Pop(m_kGroupIndex);
	rkPacket.Pop(m_kAttributes);

	SetPos(ptPos);
	SetAbil(AT_C_MAX_HP, iMaxHP);
	SetAbil(AT_HP, iHP);
	SetBasicAbil();

	BYTE byTeam = 0;
	rkPacket.Pop(byTeam);//팀
	SetAbil(AT_TEAM,static_cast<int>(byTeam));
	
	BYTE byDefaultAngle = 255;
	rkPacket.Pop(byDefaultAngle);
	SetAbil(AT_MON_DEFAULT_ANGLE, static_cast<int>(byDefaultAngle));

	rkPacket.Pop(iValue);		SetAbil(AT_CANNOT_DAMAGE, iValue);

	return kWriteType;
}

bool PgObjectUnit::IsAttribute(DWORD const dwAttr)
{
	return ((dwAttr & Attributes()) != 0) ? true : false;
}