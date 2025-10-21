#include "stdafx.h"

#include "BM/ThreadObject.h"
#include "../IPFilterWrapper.h"

PgIPBandFilter_Wrapper::PgIPBandFilter_Wrapper()
{
}

PgIPBandFilter_Wrapper::~PgIPBandFilter_Wrapper()
{
}

void PgIPBandFilter_Wrapper::Clear(bool const bIsBlockIP)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Clear(bIsBlockIP);
}


bool PgIPBandFilter_Wrapper::Build(bool const bIsBlockIP)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Build(bIsBlockIP);
}

bool PgIPBandFilter_Wrapper::RegIPRange(SIpRange const &kIPRange, bool const IsBlockIP, SIpRange &kUsedRange)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->RegIPRange(kIPRange, IsBlockIP, kUsedRange);
}

bool PgIPBandFilter_Wrapper::IsBlockAddr(in_addr const &in_addr_, SIpRange &out_range)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsBlockAddr(in_addr_, out_range);
}

bool PgIPBandFilter_Wrapper::IsAcceptAddr(in_addr const &in_addr_, SIpRange &out_range)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->IsAcceptAddr(in_addr_, out_range);
}

void PgIPBandFilter_Wrapper::WriteToPacket(BM::Stream &kPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->WriteToPacket(kPacket);
}

void PgIPBandFilter_Wrapper::WriteToPacket(BM::Stream &kPacket, bool const bIsBlockIP)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->WriteToPacket(kPacket, bIsBlockIP);
}

void PgIPBandFilter_Wrapper::ReadFromPacket(BM::Stream &kPacket, bool const bIsAll)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->ReadFromPacket(kPacket, bIsAll);
}

void PgIPBandFilter_Wrapper::GetCont(bool const bIsBlockIP, PgIPBandFilter::CONT_IP &kOutCont)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetCont(bIsBlockIP, kOutCont);
}

void PgIPBandFilter_Wrapper::SetCont(bool const bIsBlockIP, PgIPBandFilter::CONT_IP const &kInCont)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->SetCont(bIsBlockIP, kInCont);
}