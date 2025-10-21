#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "PgEmporia.h"
#include "EmporiaTournament.h"
#include "DefAbilType.h"
#include "variant/TableDataManager.h"

// Emporia Mode 2 ----->
PgEmporia::PgEmporia(void)
:	m_byGate(EMPORIA_GATE_CLOSE)
{
}

PgEmporia::PgEmporia( BM::GUID const &kID, BYTE const byGrade )
:	m_byGate(EMPORIA_GATE_CLOSE)
,	m_kKey( kID, byGrade )
{
}

PgEmporia::PgEmporia( SEmporiaKey const &kKey, SEmporiaGuildInfo const &kOwner, BYTE const byGate, BM::DBTIMESTAMP_EX const &kHaveDate )
{
	Init( kKey, kOwner, byGate, kHaveDate );
}

PgEmporia::PgEmporia( PgEmporia const &rhs )
:	m_kKey(rhs.m_kKey)
,	m_kOwner(rhs.m_kOwner)
,	m_byGate(rhs.m_byGate)
,	m_kHaveDate(rhs.m_kHaveDate)
,	m_kEmporiaGroundKey(rhs.m_kEmporiaGroundKey)
,	m_kFunction(rhs.m_kFunction)
{
}

PgEmporia::~PgEmporia(void)
{
}

PgEmporia& PgEmporia::operator = ( PgEmporia const &rhs )
{
	Init( rhs.m_kKey, rhs.m_kOwner, rhs.m_byGate, rhs.m_kHaveDate );
	m_kEmporiaGroundKey = rhs.m_kEmporiaGroundKey;
	m_kFunction = rhs.m_kFunction;
	return *this;
}

void PgEmporia::Init( SEmporiaKey const &kKey, SEmporiaGuildInfo const &kOwner, BYTE const byGate, BM::DBTIMESTAMP_EX const &kHaveDate )
{
	m_kKey = kKey;
	m_kOwner = kOwner;
	m_byGate = byGate;
	m_kHaveDate = kHaveDate;
}

bool PgEmporia::Open( bool const bOpen )
{
	if ( bOpen == IsOpen() )
	{
		return false;
	}

	if ( true == bOpen )
	{
		m_byGate = EMPORIA_GATE_GUILD;
	}
	else
	{
		m_byGate = EMPORIA_GATE_CLOSE;
	}

	m_kOwner = SEmporiaGuildInfo();
	return true;
}

bool PgEmporia::SetGate( BYTE const byGate )
{
	if ( m_byGate != byGate )
	{
		if ( EMPORIA_GATE_MAX > byGate)
		{
			m_byGate = byGate;
			return true;
		}
	}
	return false;
}

bool PgEmporia::IsAccess( BM::GUID const &kGuildID )const
{
	switch ( m_byGate )
	{
	case EMPORIA_GATE_CLOSE:
		{
		}break;
	case EMPORIA_GATE_GUILD:
		{
			return m_kOwner.kGuildID == kGuildID;
		}break;
	case EMPORIA_GATE_PUBLIC:
		{
            if( m_kOwner.IsEmpty() )
            {
                CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Owner Is Empty EmporiaID<") << m_kKey.kID << _T(">, Grade<") << m_kKey.byGrade << _T(">, Gate<") << m_byGate << _T(">") );
                return false;
            }
			return true;
		}break;
	}
	return false;
}

void PgEmporia::WriteToPacket( BM::Stream &kPacket, bool const bSimple )const
{
	kPacket.Push( m_kKey );
	kPacket.Push( m_byGate );
	m_kOwner.WriteToPacket( kPacket );
	kPacket.Push( m_kHaveDate );
	kPacket.Push( bSimple );
	if ( !bSimple )
	{
		WriteToPacket_Function( kPacket );
	}
}

bool PgEmporia::ReadFromPacket( BM::Stream &kPacket )
{
	kPacket.Pop( m_kKey );
	kPacket.Pop( m_byGate );
	if ( true == m_kOwner.ReadFromPacket( kPacket ) )
	{
		kPacket.Pop( m_kHaveDate );
		bool bSimple = false;
		if ( true == kPacket.Pop( bSimple ) )
		{
			//m_kFunction.Clear();// 클리어 하면 안된다.
			if ( BM::IsCorrectType( bSimple ) )
			{
				if ( !bSimple )
				{
					ReadFromPacket_Function( kPacket );
				}
			}
		}
	}
	return false;
}

bool PgEmporia::SetOwner( SEmporiaGuildInfo const &kOwner )
{
	if ( m_kOwner.kGuildID != kOwner.kGuildID )
	{
		m_kOwner = kOwner;

		if ( !m_kOwner.IsEmpty() )
		{
			if ( EMPORIA_GATE_CLOSE == m_byGate )
			{
				m_byGate = EMPORIA_GATE_GUILD;
			}
		}

		m_kHaveDate.SetLocalTime();
		return true;
	}
	return false;
}

bool PgEmporia::GetOwner( SEmporiaGuildInfo &rkOwner )const
{
	if ( !m_kOwner.IsEmpty() )
	{
		rkOwner = m_kOwner;
		return true;
	}
	return false;
}

int PgEmporia::GetRewardAchievementNo(int const iIdx, bool bIsTournament)
{
	int iBattleIndex = 0;
	if( bIsTournament )
	{
		switch(iIdx)
		{
		case TBL_DEF_EMPORIA_REWARD::ECG_WINNER:
			{
				iBattleIndex = 1;
			}break;
		}
	}
	else
	{
		iBattleIndex = PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX + iIdx;
	}

	CONT_DEF_CONT_ACHIEVEMENTS const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);
	if( !pkContDef ){ return 0; }

	CONT_DEF_CONT_ACHIEVEMENTS::const_iterator type_citer = pkContDef->find(AT_ACHIEVEMENT_EMPORIA_VICTORY);
	if(type_citer == pkContDef->end()){ return 0; }

	CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::const_iterator value_citer = type_citer->second.find(iBattleIndex);
	if(value_citer == type_citer->second.end()){ return 0; }

	return value_citer->second.iSaveIdx;
}
