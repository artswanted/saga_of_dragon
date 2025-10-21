#include "stdafx.h"
#include "PgDoc_PetInfo.h"

PgDoc_PetInfo::PgDoc_PetInfo()
:	m_bLoadedInven(false)
{}

PgDoc_PetInfo::~PgDoc_PetInfo()
{}

PgDoc_PetInfo::PgDoc_PetInfo( PgDoc_PetInfo const &rhs )
:	PgDoc(rhs)
,	m_bLoadedInven(rhs.m_bLoadedInven)
,	m_kInv(rhs.m_kInv)
,	m_kSkillCoolTimeMap(rhs.m_kSkillCoolTimeMap)
{}

PgDoc_PetInfo& PgDoc_PetInfo::operator =( PgDoc_PetInfo const &rhs )
{
	PgDoc::operator = ( rhs );
	m_bLoadedInven = rhs.m_bLoadedInven;
	m_kInv = rhs.m_kInv;
	m_kSkillCoolTimeMap = rhs.m_kSkillCoolTimeMap;
	return *this;
}

PgInventory const * PgDoc_PetInfo::GetInven()const
{
	if ( true == m_bLoadedInven )
	{
		return &m_kInv;
	}
	return NULL;
}

PgInventory * PgDoc_PetInfo::GetInven()
{
	if ( true == m_bLoadedInven )
	{
		return &m_kInv;
	}
	return NULL;
}

void PgDoc_PetInfo::SetInven( PgInventory const &rkInv )
{
	m_kInv = rkInv;
	m_bLoadedInven = true;
}

void PgDoc_PetInfo::RefreshSkillCoolTimeMap()
{
	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();
	CSkill::MAP_COOLTIME::iterator itr = m_kSkillCoolTimeMap.begin();
	while ( itr != m_kSkillCoolTimeMap.end() )
	{
		if ( itr->second > dwServerElapsedTime )
		{
			++itr;
		}
		else
		{
			itr = m_kSkillCoolTimeMap.erase( itr );
		}
	}
}

void PgDoc_PetInfo::WriteToPacket( BM::Stream &kPacket )
{
	m_kInv.WriteToPacket( kPacket, WT_DEFAULT );
	PU::TWriteTable_AA( kPacket, m_kSkillCoolTimeMap );
}

HRESULT PgDoc_PetInfo::ItemProcess( SPMO const &kOrder, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	HRESULT hReturn = E_FAIL;
	__int64 const iCause = kOrder.Cause();
	switch( IMET_CMP_BASE & iCause )
	{
	case IMET_MODIFY_COUNT://타겟 대상으로 작업, 까기 올리기 둘다 지원.
		{
			SPMOD_Modify_Count kData;
			kOrder.Read(kData);

			hReturn = m_kInv.ModifyCount(iCause, kData, kChangeArray, kContLogMgr);
		}break;
	default:
		{
		}break;
	}

	return hReturn;
}
