#include "stdafx.h"
#include "PgControlUnit.h"

PgControlUnit::PgControlUnit(void)
:	m_kLevelRank(0)
{

}

PgControlUnit::~PgControlUnit(void)
{

}

void PgControlUnit::Init()
{
	CUnit::Init();
	m_kMySkill.Init();
	m_kLevelRank = 0;
}

HRESULT PgControlUnit::Create(void const* pkInfo)
{
	__asm int 3;
	return E_FAIL;
}

bool PgControlUnit::IsCheckZoneTime(DWORD dwElapsed)
{
	// Player는 유저로 부터 패킷을 받았을때 CheckZone 하게 된다.
	//	m_dwElapsedTimeCheckZone 값이 최근 CheckZone 한 시간이 되도록...
	DWORD const dwNow = BM::GetTime32();
	if (dwNow - m_dwElapsedTimeCheckZone > 600)
	{
		m_dwElapsedTimeCheckZone = dwNow;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgControlUnit::SetAbil( WORD const Type, int const iValue, bool const bIsSend, bool const bBroadcast)
{
	switch(Type)
	{
	case AT_LEVEL:
		{ 
			SClassKey const kNewKey( GetAbil(AT_CLASS), iValue );
			return this->DoLevelup(kNewKey);
		}break;
	case AT_CLASS:
		{ 
			SClassKey const kNewKey(iValue, GetAbil(AT_LEVEL));
			return this->DoLevelup(kNewKey);
		}break;
	case AT_BATTLE_LEVEL:
		{
			return this->DoBattleLevelUp( static_cast<short>(iValue) );
		}break;
	case AT_LEVEL_RANK:
		{
			m_kLevelRank = static_cast<BYTE>(iValue);
			return true;
		}break;
	default:
		{
			return CUnit::SetAbil( Type, iValue, bIsSend, bBroadcast );
		}break;
	}
	return false;
}

int PgControlUnit::GetBattleLevel(void)const
{
	int const iBattleLv = GetAbil( AT_BATTLE_LEVEL );
	if ( 0 < iBattleLv )
	{
		return iBattleLv;
	}
	return this->GetAbil( AT_LEVEL );
}
