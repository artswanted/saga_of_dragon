#include "StdAfx.h"
#include "PgMissionMutator.h"

PgMissionMutator::PgMissionMutator()
{
}

PgMissionMutator::PgMissionMutator(PgMissionMutator const & rkData)
{
	this->m_kMutators = rkData.m_kMutators;
}

PgMissionMutator::~PgMissionMutator()
{
}

bool PgMissionMutator::MutatorVerify(const CONT_DEF_MISSION_MUTATOR *pkRhs)
{
	bool bRet = true;
	if( m_kMutators.size() == 0 )
	{
		return bRet; // not used mutator
	}

	if( !pkRhs ) // empty table
	{
		return false;
	}

	for(MUTATOR_SET::iterator iter = m_kMutators.begin(); iter != m_kMutators.end(); iter++)
	{
		if( pkRhs->find((*iter)) == pkRhs->end() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV2, __FL__ << _T("Not found Mutator No[") << *iter << "]" );
			return false; // server not found data
		}
	}
	return bRet;
}

void PgMissionMutator::WriteToPacket_Mutator(BM::Stream& rkPacket) const
{
	rkPacket.Push(m_kMutators);
}

void PgMissionMutator::ReadFromPacket_Mutator(BM::Stream &rkPacket)
{
	rkPacket.Pop(m_kMutators);
}
