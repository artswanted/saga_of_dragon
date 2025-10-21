#include "stdafx.h"
#include "PgDefencePartyMgr.h"

void PgDefencePartyMgrImpl::Clear()
{
	m_kDefencePartyInfo.clear();
}

int PgDefencePartyMgrImpl::IsJoinParty(BM::GUID const& kPartyGuid)
{
	CONT_DEFENCE_PARTYINFO::const_iterator iter = m_kDefencePartyInfo.find(kPartyGuid);
	
	if( m_kDefencePartyInfo.end() != iter )
	{
		CONT_DEFENCE_PARTYINFO::mapped_type kValue = iter->second;

		return kValue;
	}
	return static_cast<int>(PI_NONE);
}

void PgDefencePartyMgrImpl::FindWaitList(VEC_GUID& rkVecGuid)
{
	rkVecGuid.clear();

	CONT_DEFENCE_PARTYINFO::const_iterator iter = m_kDefencePartyInfo.begin();
	while( m_kDefencePartyInfo.end() != iter )
	{
		CONT_DEFENCE_PARTYINFO::key_type kKey = (*iter).first;
		CONT_DEFENCE_PARTYINFO::mapped_type kValue = (*iter).second;

		if( PI_WAIT == static_cast<EDefence7PartyState>(kValue) )
		{
			rkVecGuid.push_back( kKey );
		}
		++iter;
	}
}

bool PgDefencePartyMgrImpl::AddWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	auto bRet = m_kDefencePartyInfo.insert(std::make_pair(kPartyGuid, static_cast<int>(eType)));
	if( !bRet.second )
	{
		// 이미 대기중, 전쟁중인 파티
		return false;
	}
	return true;
}

bool PgDefencePartyMgrImpl::DelWaitParty(BM::GUID const& kPartyGuid)
{
	CONT_DEFENCE_PARTYINFO::iterator iter = m_kDefencePartyInfo.find(kPartyGuid);	
	if( m_kDefencePartyInfo.end() != iter )
	{
		m_kDefencePartyInfo.erase(iter);
		return true;
	}
	return false;
}

bool PgDefencePartyMgrImpl::ModifyWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	CONT_DEFENCE_PARTYINFO::iterator iter = m_kDefencePartyInfo.find(kPartyGuid);
	if( m_kDefencePartyInfo.end() != iter )
	{
		CONT_DEFENCE_PARTYINFO::mapped_type& kValue = iter->second;
		kValue = eType;

		return true;
	}
	return false;
}

void PgDefencePartyMgrImpl::GetDefencePartyTypeList(VEC_GUID& rkVecGuid, int eType)
{
	rkVecGuid.clear();

	CONT_DEFENCE_PARTYINFO::const_iterator iter = m_kDefencePartyInfo.begin();
	while( m_kDefencePartyInfo.end() != iter )
	{
		CONT_DEFENCE_PARTYINFO::key_type kKey = (*iter).first;
		CONT_DEFENCE_PARTYINFO::mapped_type kValue = (*iter).second;

		if( eType == kValue )
		{
			rkVecGuid.push_back( kKey );
		}
		++iter;
	}
}

//===============================================================================================================================================================================================

void PgDefencePartyMgr::Clear()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	Instance()->Clear();
}

int PgDefencePartyMgr::IsJoinParty(BM::GUID const& kPartyGuid)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->IsJoinParty(kPartyGuid);
}

void PgDefencePartyMgr::FindWaitList(VEC_GUID& rkVecGuid)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	return Instance()->FindWaitList(rkVecGuid);
}

bool PgDefencePartyMgr::AddWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->AddWaitParty(kPartyGuid, eType);
}

bool PgDefencePartyMgr::DelWaitParty(BM::GUID const& kPartyGuid)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->DelWaitParty(kPartyGuid);
}

bool PgDefencePartyMgr::ModifyWaitParty(BM::GUID const& kPartyGuid, int eType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->ModifyWaitParty(kPartyGuid, eType);
}

void PgDefencePartyMgr::GetDefencePartyTypeList(VEC_GUID& rkVecGuid, int eType)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);

	Instance()->GetDefencePartyTypeList(rkVecGuid, eType);
}