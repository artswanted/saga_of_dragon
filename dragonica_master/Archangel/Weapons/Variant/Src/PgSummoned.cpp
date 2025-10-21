#include "stdafx.h"
#include "PgControlDefMgr.h"
#include "PgSummoned.h"
#include "Variant/Global.h"

PgSummoned::PgSummoned() : m_kLifeTime(0), m_bEternalLife(false), m_kUniqueClass(false), m_kSupply(0), m_bAddSummonedToOwner(false)
{
}

PgSummoned::~PgSummoned()
{
}

EUnitType PgSummoned::UnitType() const
{
	return UT_SUMMONED;
}

void PgSummoned::Init()
{
	Caller(BM::GUID::NullData());
	LifeTime(0);
	m_bAddSummonedToOwner = false;
	CUnit::Init();
}

unsigned long PgSummoned::LifeTime() const
{
	return m_kLifeTime;
}
void PgSummoned::LifeTime(unsigned long const kTime)
{
	m_kLifeTime = kTime;
}

int PgSummoned::AutoHeal( unsigned long ulElapsedTime, float const fMultiplier )
{
	if ( !m_bEternalLife )
	{
		m_kLifeTime = (m_kLifeTime <= ulElapsedTime) ? 0 : m_kLifeTime - ulElapsedTime;
		if ( !m_kLifeTime )
		{
			SetAbil(AT_HP,0);
			m_bEternalLife = true;
		}
	}

	return CUnit::AutoHeal(ulElapsedTime, fMultiplier);;
}

void PgSummoned::CopyAbilFromClassDef( CLASS_DEF_BUILT const *pDef)
{
	if (pDef == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDef is NULL. Caller ")<< Caller());
		return;
	}

	
	CLASSDEF_ABIL_CONT::const_iterator itor = pDef->kAbil.begin();
	while (itor != pDef->kAbil.end())
	{
		SetAbil(itor->first, itor->second);

		++itor;
	}
	if(0 == GetAbil(AT_MAX_HP))
	{
		SetAbil(AT_MAX_HP, static_cast<int>(pDef->kDef.sMaxHP) );	//임시
	}
	if(0 == GetAbil(AT_HP))
	{
		SetAbil(AT_HP, GetAbil(AT_MAX_HP) );
	}
	if(0 == GetAbil(AT_AI_TYPE))
	{
		m_kBasic.iAIType = 1;//없으면 선공형 몹 AI
	}

	SetAbil(AT_MOVESPEED, 0==pDef->kDef.sMoveSpeed ? 50 : static_cast<int>(pDef->kDef.sMoveSpeed) );
}

bool PgSummoned::SetInfo(SummonedInfo_ const * pInfo)
{
	if( !pInfo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" pInfo is NULL. Caller is ") << Caller());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// ClassDef로 부터 모든 Abil 세팅하기
	GET_DEF(PgClassDefMgr, kClassDefMgr);
	CLASS_DEF_BUILT const* pDef = kClassDefMgr.GetDef(pInfo->kClassKey);

	if( !pDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot Get ClassDef(Class:") << pInfo->kClassKey.iClass << _T(", Level:") << pInfo->kClassKey.nLv << _T(") Caller is ")  << Caller() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CopyAbilFromClassDef(pDef);

	SetID( pInfo->kGuid );
	Caller(pInfo->kCaller);
	//m_kParty = pInfo->kParty;
	m_bEternalLife = pInfo->bEternalLife;

	SetAbil(AT_CLASS, pInfo->kClassKey.iClass);//PgControlUnit으로 들어가면 안됨.
	SetAbil(AT_LEVEL, pInfo->kClassKey.nLv);
	GoalPos(POINT3(0, 0, 0));

	int iOpeningDelay = GetAbil(AT_MON_OPENING_TIME);
	if( 0==iOpeningDelay )
	{
		iOpeningDelay = 3000;
	}
	else
	{
		iOpeningDelay += 3;
	}

	SetState(US_OPENING);
	SetDelay(iOpeningDelay);

	std::map<WORD, int>::const_iterator itor = pInfo->kAbil.begin();
	while (itor != pInfo->kAbil.end())
	{
		SetAbil(itor->first, itor->second);
		++itor;
	}

	CalcCreateSummonBasicAbil(AT_C_MAX_HP, GetAbil(AT_CREATE_SUMMONED_HP_RATE));
	CalcCreateSummonBasicAbil(AT_C_PHY_ATTACK_MIN, GetAbil(AT_CREATE_SUMMONED_ATTACK_RATE));
	CalcCreateSummonBasicAbil(AT_C_PHY_ATTACK_MAX, GetAbil(AT_CREATE_SUMMONED_ATTACK_RATE));
	CalcCreateSummonBasicAbil(AT_C_MAGIC_ATTACK_MIN, GetAbil(AT_CREATE_SUMMONED_ATTACK_RATE));
	CalcCreateSummonBasicAbil(AT_C_MAGIC_ATTACK_MAX, GetAbil(AT_CREATE_SUMMONED_ATTACK_RATE));
	CalcCreateSummonBasicAbil(AT_C_PHY_DEFENCE, 0);
	CalcCreateSummonBasicAbil(AT_C_PHY_DMG_DEC, 0);
	CalcCreateSummonBasicAbil(AT_C_MAGIC_DEFENCE, 0);
	CalcCreateSummonBasicAbil(AT_C_MAGIC_DMG_DEC, 0);
	
	SetAbil(AT_HP, GetAbil(AT_C_MAX_HP));

	int iValue = GetAbil(AT_AI_TYPE);
	m_kAI.SetPattern((iValue>0) ? iValue : 1);
	m_kAI.SetCurrentAction(EAI_ACTION_OPENING);
	for (int i=1; i<EAI_ACTION_MAX; i++)
	{
		iValue = GetAbil(AT_AI_ACTIONTYPE_MIN + i);
		if (iValue != 0)
		{
			m_kAI.AddActionType((EAIActionType)i, iValue);
		}
	}

	return true;
}

HRESULT PgSummoned::Create(void const *pInfo)
{
	if(NULL==pInfo)
	{
		return E_FAIL;
	}
	Init();

	SummonedInfo_ const *pSummonInfo = static_cast<SummonedInfo_ const *>(pInfo);

	if(false==SetInfo( pSummonInfo ))
	{
		return E_FAIL;
	}

	int iSkillNo = 0;
	for(int i=0; i<10; ++i)
	{
		iSkillNo = GetAbil(AT_MON_SKILL_01+i);
		if(0==iSkillNo)
		{
			break;
		}
		GetMySkill()->LearnNewSkill(iSkillNo);
	}
	return S_OK;
}

void PgSummoned::WriteToPacket_MapMoveData( BM::Stream &rkPacket )
{//SummonedID + SSummonedMapMoveData::WriteToPacket() 과 동일하게
	rkPacket.Push( GetID() );

	m_Skill.WriteToPacket_CoolTimeMap( rkPacket );
	rkPacket.Push(GetAbil(AT_CLASS));
	rkPacket.Push(GetAbil(AT_LEVEL));
	rkPacket.Push(GetAbil(AT_HP));
	rkPacket.Push(m_kUniqueClass);
	rkPacket.Push(m_kSupply);
	rkPacket.Push(m_kLifeTime);
	rkPacket.Push(GetAbil(AT_LIFETIME));

	size_t const iWRPos = rkPacket.WrPos();
	size_t iWRSize = 0;
	rkPacket.Push( iWRSize );
	m_kEffect.WriteToPacket( rkPacket, false );
	iWRSize = rkPacket.WrPos() - iWRPos - sizeof(iWRSize);
	rkPacket.ModifyData( iWRPos, &iWRSize, sizeof(iWRSize) );
}

void PgSummoned::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	CUnit::WriteToPacket(rkPacket, kWriteType);
	rkPacket.Push(GetAbil(AT_CLASS));
	rkPacket.Push(static_cast<short>(GetAbil(AT_LEVEL)));
	rkPacket.Push( m_kCaller );
	rkPacket.Push( m_kLifeTime );
	rkPacket.Push( m_kSupply );
	rkPacket.Push( m_bNPC );

	DefaultSummonedInfo_ kInfo;
	kInfo.kGuid = GetID();
	kInfo.iClassNo = GetAbil(AT_CLASS);
	kInfo.ptPos = GetPos();
	kInfo.iHP = GetAbil(AT_HP);
	kInfo.iMaxHP = GetAbil(AT_C_MAX_HP);
	kInfo.sMoveSpeed = GetAbil(AT_MOVESPEED);
	kInfo.WriteToPacket(rkPacket);

	rkPacket.Push( GetAbil(AT_PHY_ATTACK_MIN) );
	rkPacket.Push( GetAbil(AT_PHY_ATTACK_MAX) );
	rkPacket.Push( GetAbil(AT_MAGIC_ATTACK_MIN) );
	rkPacket.Push( GetAbil(AT_MAGIC_ATTACK_MAX) );
	rkPacket.Push( GetAbil(AT_PHY_DEFENCE) );
	rkPacket.Push( GetAbil(AT_PHY_DMG_DEC) );
	rkPacket.Push( GetAbil(AT_MAGIC_DEFENCE) );
	rkPacket.Push( GetAbil(AT_MAGIC_DMG_DEC) );
	rkPacket.Push( GetAbil(AT_TEAM) );
	rkPacket.Push( GetAbil(AT_LIFETIME) );
	rkPacket.Push( GetAbil(AT_DUEL) );
	//어빌 종류가 일정 이상을 넘어가면 CAbilObject::WriteToPacket(rkPacket);를 사용하자
}
EWRITETYPE PgSummoned::ReadFromPacket(BM::Stream &rkPacket)
{
	EWRITETYPE kWriteType = CUnit::ReadFromPacket(rkPacket);

	SClassKey kClassKey;
	rkPacket.Pop(kClassKey.iClass);
	rkPacket.Pop(kClassKey.nLv);
	SetAbil(AT_LEVEL, kClassKey.nLv);

	GET_DEF(PgClassDefMgr, kClassDefMgr);
	CLASS_DEF_BUILT const* pDef = kClassDefMgr.GetDef(kClassKey);
	CopyAbilFromClassDef(pDef);

	rkPacket.Pop( m_kCaller );
	rkPacket.Pop( m_kLifeTime );
	rkPacket.Pop( m_kSupply );
	rkPacket.Pop( m_bNPC );

	DefaultSummonedInfo_ kInfo;
	kInfo.ReadFromPacket(rkPacket);
	SetInfoFromDefaultSummonedInfo(kInfo);

	int iValue = 0;
	rkPacket.Pop(iValue);	SetAbil(AT_PHY_ATTACK_MIN, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_PHY_ATTACK_MAX, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_MAGIC_ATTACK_MIN, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_MAGIC_ATTACK_MAX, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_PHY_DEFENCE, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_PHY_DMG_DEC, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_MAGIC_DEFENCE, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_MAGIC_DMG_DEC, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_TEAM, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_LIFETIME, iValue);
	rkPacket.Pop(iValue);	SetAbil(AT_DUEL, iValue);
	//어빌 종류가 일정 이상을 넘어가면 CAbilObject::WriteToPacket(rkPacket);를 사용하자

	return kWriteType;
}

bool PgSummoned::SetAbil( WORD const Type, int const iValue, bool const bIsSend, bool const bBroadcast)
{//펫과 달리 레벨, 클래스가 변경되지 않는다.
	bool bIntercept = true;
	switch(Type)
	{
	case AT_PHY_DEFENCE:
	case AT_MAGIC_DEFENCE:
		{
			CAbilObject::SetAbil(Type, iValue);
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

int PgSummoned::GetAbil(WORD const Type) const
{
	switch(Type)
	{
	case AT_OWNER_TYPE:
	case AT_CALLER_TYPE:
	case AT_PHY_DEFENCE:
	case AT_MAGIC_DEFENCE:
		{
			return CAbilObject::GetAbil(Type);
		}break;
	}
	return CUnit::GetAbil(Type);
}

void PgSummoned::SetInfoFromDefaultSummonedInfo(DefaultSummonedInfo_ const &rkInfo)
{
	SetID(rkInfo.kGuid);
	SetAbil(AT_CLASS, rkInfo.iClassNo);
	SetAbil(AT_MAX_HP, rkInfo.iMaxHP);
	SetAbil(AT_HP, rkInfo.iHP);
	SetPos(rkInfo.ptPos);
	SetAbil(AT_MOVESPEED, rkInfo.sMoveSpeed);
}

void PgSummoned::CalcCreateSummonBasicAbil(WORD const wType, int const iRate)
{
	WORD const wBasicType = GetBasicAbil(wType);
	int iValue = GetAbil(wType);
	if(iRate)
	{
		iValue = iValue * (iRate/ABILITY_RATE_VALUE_FLOAT);
	}
	SetAbil(wBasicType, iValue);
	CalculateAbil(wType);
}

/*void PgSummoned::SetCommonAbil()
{
	int const iClassNo = GetAbil(AT_CLASS);

}*/