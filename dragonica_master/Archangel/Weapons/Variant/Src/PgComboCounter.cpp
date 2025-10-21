#include "stdafx.h"

#include "PgEventView.h"
#include "PgComboCounter.h"
#include "Global.h"

PgComboCounter::PgComboCounter()
	: m_kOwnerGuid(), m_dwDelayTime(0), m_kComboCounter(), m_iTotalHitCount(0), m_iTotalDamageCount(0)
{
}

PgComboCounter::~PgComboCounter()
{
}

void PgComboCounter::Clear(bool const bOwner)
{
	if( bOwner )
	{
		m_kOwnerGuid.Clear();
	}
	m_dwDelayTime = 0;
	m_kComboCounter.Clear();
	m_iTotalHitCount = 0;
	m_iTotalDamageCount = 0;
}

COMBO_TYPE PgComboCounter::AddCombo(const COMBO_TYPE iAddedCombo, bool& bResetOut, DWORD const dwDelay)
{
	DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
	DWORD const& dwLastComboTime = m_kComboCounter.dwLastComboTime;
	DWORD const dwDiffTime = DifftimeGetTime(dwLastComboTime, dwCurTime);
	if( (g_dwComboContinueTime + m_dwDelayTime) < dwDiffTime )
	{
		m_kComboCounter.ResetCombo(iAddedCombo);
		bResetOut = true;//콤보 초기화

		m_dwDelayTime = 0;
	}
	else
	{
		m_kComboCounter.iTotalCombo += iAddedCombo;
		bResetOut = false;//콤보 초기화 안됬다

		if( 0 != m_dwDelayTime )
		{
			if( m_dwDelayTime > dwDiffTime )
			{
				m_dwDelayTime -= dwDiffTime;
			}
			else
			{
				m_dwDelayTime = 0;
			}
		}
	}

	m_kComboCounter.dwLastComboTime = dwCurTime;//시간
	if( dwDelay )
	{
		if( dwDelay > m_dwDelayTime )
		{
			m_dwDelayTime = dwDelay;
		}
	}
	++m_iTotalHitCount;

	return m_kComboCounter.iTotalCombo;
}
COMBO_TYPE PgComboCounter::GetComboCount(const ECOMBO_STYLE kStyle)
{
	switch( kStyle )
	{
	case ECOMBO_TOTAL:
		return m_kComboCounter.iTotalCombo;
		break;
	case ECOMBO_MAX:
		m_kComboCounter.Update();
		return m_kComboCounter.iMaxCombo;
		break;
	case ECOUNT_TOTAL_HIT:
		return m_iTotalHitCount;
	case ECOUNT_TOTAL_DAMAGE:
		return m_iTotalDamageCount;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}