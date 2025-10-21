#include "stdafx.h"
#include "Variant/PgPlayer.h"
#include "Global.h"
#include "AlramMission.h"

using namespace ALRAM_MISSION;

PgAlramMission::PgAlramMission()
:	m_pkDef(NULL)
,	m_dwBeginTime(0)
,	m_byState(STATE_NONE)
,	m_iExtValue(0)
{
}

PgAlramMission::~PgAlramMission()
{
}

HRESULT PgAlramMission::Begin( TBL_DEF_ALRAM_MISSION const * pkDef, DWORD const dwBeginTime, bool const bIsRebegin )
{
	if ( !pkDef )
	{
		return RET_FAIL;
	}

	{// 깨끗이 날려버리고
		VEC_PARAMS kTemp;
		m_kParamList.swap(kTemp);
	}

	m_byState = STATE_IDLE;
	m_pkDef = pkDef;
	m_dwBeginTime = dwBeginTime;
	m_iExtValue = 0;
	m_kParamList.resize(m_pkDef->kActionList.size());
	return RET_SUCCESS;
}

HRESULT PgAlramMission::OnTick( DWORD const dwCurTime )
{
	switch ( m_byState )
	{
	case STATE_IDLE:
	case STATE_CHECKTIME:
		{
			DWORD const dwElapsedTime = ::DifftimeGetTime( m_dwBeginTime, dwCurTime );
			DWORD const dwDefTime = static_cast<DWORD>(m_pkDef->iTime) * 1000 + PgAlramMission::ms_dwDelayTime;
			if ( dwElapsedTime > dwDefTime )
			{
				VEC_ALRAM_MISSION_ACTION const &kOrgActionList =  m_pkDef->kActionList;
				if ( kOrgActionList.size() != m_kParamList.size() )
				{
					m_byState = PgAlramMission::STATE_FAILED;
					return RET_TIMEOVER;
				}

				size_t index = 0;
				VEC_ALRAM_MISSION_ACTION::const_iterator action_itr = kOrgActionList.begin();
				for ( ; action_itr != kOrgActionList.end() ; ++action_itr, ++index )
				{
					if ( action_itr->iCount <= m_kParamList.at(index) )
					{
						if ( action_itr->byFailType )
						{
							m_byState = STATE_FAILED;
							return RET_TIMEOVER;
						}
					}
					else
					{
						if ( TYPE_SUCCESS == action_itr->byFailType )
						{
							m_byState = STATE_FAILED;
							return RET_TIMEOVER;
						}
					}
				}

				m_byState = STATE_SUCCEED;
				return RET_SUCCESS;
			}

			if ( (dwElapsedTime - dwDefTime) < 5000 )
			{
				m_byState = STATE_CHECKTIME;
			}
		}break;
	case STATE_PACKET:
		{
		}break;
	default:
		{
			return RET_NEED_NEWACTION;
		}break;
	}

	return RET_NONE;
}

bool PgAlramMission::GetParam( size_t const iIndex, int &iOutCount )const
{
	if ( iIndex < m_kParamList.size() )
	{
		iOutCount = m_kParamList.at(iIndex);
		return true;
	}
	return false;
}

bool PgAlramMission::SetParam( size_t const iIndex, int const iCount )
{
	if ( iIndex < m_kParamList.size() )
	{
		m_kParamList.at(iIndex) = iCount;
		return true;
	}
	return false;
}

size_t PgAlramMission::GetSuccessCount()const
{
	if ( !m_pkDef )
	{
		return 0;
	}

	size_t iRet = 0;
	size_t index = 0;
	VEC_ALRAM_MISSION_ACTION::const_iterator action_itr = m_pkDef->kActionList.begin();
	for ( ; action_itr != m_pkDef->kActionList.end() ; ++action_itr, ++index )
	{
		if ( index < m_kParamList.size() )
		{
			if ( action_itr->iCount <= m_kParamList.at(index) )
			{
				if ( TYPE_FAIL != action_itr->byFailType )
				{
					++iRet;
				}
			}
		}
	}

	return iRet;
}

void PgAlramMission::WriteToPacket( BM::Stream &kPacket )const
{
	switch ( m_byState )
	{
	case STATE_CHECKTIME:
	case STATE_IDLE:
		{
			DWORD const dwElapsedTime = ::DifftimeGetTime( m_dwBeginTime, g_kEventView.GetServerElapsedTime() );
			DWORD const dwDefTime = GetDefTime();
			if ( dwElapsedTime > dwDefTime )
			{
				kPacket.Push( static_cast<BYTE>(STATE_NONE) );
			}
			else
			{
				kPacket.Push( static_cast<BYTE>(STATE_PACKET) );
				kPacket.Push( GetID() );
				kPacket.Push( dwElapsedTime );// 사용한 시간을 보내주어 MAP이동에 소모되는 시간은 포함되지 않도록 잘 만들어 준다.
				PU::TWriteArray_A( kPacket, m_kParamList );
			}
		}break;
	case STATE_PACKET:
		{
			kPacket.Push( m_byState );
			kPacket.Push( m_iExtValue );
			kPacket.Push( m_dwBeginTime );
			PU::TWriteArray_A( kPacket, m_kParamList );
		}break;
	default:
		{
			kPacket.Push( static_cast<BYTE>(STATE_NONE) );
		}break;
	}
}

void PgAlramMission::ReadFromPacket( BM::Stream &kPacket )
{
	m_byState = STATE_NONE;
	kPacket.Pop( m_byState );

	if ( STATE_PACKET == m_byState )
	{
		kPacket.Pop( m_iExtValue );// PACKET상태에서는 여기에 ExtValue가
		kPacket.Pop( m_dwBeginTime );
		m_kParamList.clear();
		PU::TLoadArray_A( kPacket, m_kParamList );
	}
}

HRESULT PgAlramMission::WriteToPacket_Client( BM::Stream &kPacket )const
{
	if ( STATE_IDLE == m_byState )
	{
		if ( m_pkDef )
		{
			kPacket.Push( m_pkDef->iAlramID );
			kPacket.Push( m_dwBeginTime );
			PU::TWriteArray_A( kPacket, m_kParamList );
			return RET_SUCCESS;
		}
	}

	return RET_FAIL;
}

