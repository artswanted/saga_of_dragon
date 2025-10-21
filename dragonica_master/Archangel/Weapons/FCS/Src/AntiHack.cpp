#include "stdafx.h"
#include "AntiHack.h"



PgAntiHackMgr::PgAntiHackMgr()
{
	m_kPolicy.resize(EAHP_POLICY_MAX+1);
}

PgAntiHackMgr::~PgAntiHackMgr()
{
}

bool PgAntiHackMgr::Locked_SetPolicy(EAntiHack_Policy const& ePolicy, SHackPolicy const & kPolicyInfo)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	m_kPolicy[ePolicy] = kPolicyInfo;
	return true;
}

bool PgAntiHackMgr::Locked_GetPolicy_Enabled(EAntiHack_Policy const & ePolicy) const
{
	BM::CAutoMutex kLock(m_kMutex);
	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	return m_kPolicy[ePolicy].bUse;
}

void PgAntiHackMgr::Locked_ReadFromPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_kPolicy.clear();
	rkPacket.Pop(m_sMaxIndex);
	PU::TLoadArray_M(rkPacket, m_kPolicy);
	INFO_LOG(BM::LOG_LV6, __FL__);
}

void PgAntiHackMgr::Locked_WriteToPacket(BM::Stream& rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	rkPacket.Push(m_sMaxIndex);
	PU::TWriteArray_M(rkPacket, m_kPolicy);
}

float PgAntiHackMgr::Locked_GetPolicy_IndexMutiflier(EAntiHack_Policy const & ePolicy) const
{
	BM::CAutoMutex kLock(m_kMutex);
	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	return m_kPolicy[ePolicy].fHackIndexMultiflier;
}

short PgAntiHackMgr::Locked_GetPolicy_IndexAdd(EAntiHack_Policy const & ePolicy) const
{
	BM::CAutoMutex kLock(m_kMutex);
	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	return m_kPolicy[ePolicy].sHackIndexAdd;
}

int PgAntiHackMgr::Locked_GetPolicy_BlockTime(EAntiHack_Policy const & ePolicy) const
{
	BM::CAutoMutex kLock(m_kMutex);
	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	return m_kPolicy[ePolicy].iBlockTimeSec;
}

void PgAntiHackMgr::Locked_SetMaxHackIndex(short const sMax)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_sMaxIndex = sMax;
}

short PgAntiHackMgr::Locked_GetMaxHackIndex() const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_sMaxIndex;
}

short PgAntiHackMgr::Locked_GetPolicy_MaxHackIndex(EAntiHack_Policy const & ePolicy) const
{
	BM::CAutoMutex kLock(m_kMutex);
	if (ePolicy > EAHP_POLICY_MAX && ePolicy <= 0)
	{
		return false;
	}
	return m_kPolicy[ePolicy].sMaxHackIndex;
}