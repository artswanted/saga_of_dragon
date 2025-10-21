
#include "stdafx.h"
#include "BM/PgTask.h"
#include "PgLocalVendorMgr.h"

PgLocalVendorMgrImpl::PgLocalVendorMgrImpl()
{
}

PgLocalVendorMgrImpl::~PgLocalVendorMgrImpl()
{
}

void PgLocalVendorMgrImpl::Clear()
{
	CONT_VENDOR_LIST kTemp;
	m_kContVendorList.swap(kTemp);
}

bool PgLocalVendorMgrImpl::ReadToPacketVendorName(BM::GUID const& rkCharGuid, BM::Stream& rkPacket)
{
	CONT_VENDOR_LIST::iterator iterVendor = m_kContVendorList.find(rkCharGuid);
	if( m_kContVendorList.end() != iterVendor)
	{
		rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_ANS_VENDOR_STATE);
		rkPacket.Push( true );
		rkPacket.Push( (*iterVendor).first );
		rkPacket.Push( (*iterVendor).second );
		return true;
	}

	rkPacket.Clear();

	rkPacket.Push((BM::Stream::DEF_STREAM_TYPE)PT_M_C_ANS_VENDOR_STATE);
	rkPacket.Push( false );
	rkPacket.Push( BM::GUID::NullData() );
	std::wstring kTemp;
	rkPacket.Push( kTemp );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgLocalVendorMgrImpl::Create(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle)
{
	auto kPair= m_kContVendorList.insert( std::make_pair(rkCharGuid, rkNewVendorTitle) );
	if( !kPair.second )
	{
		std::wstring const& kValue = rkNewVendorTitle;
		kPair.first->second = kValue;

		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"PgLocalVendorMgrImpl::Create False ["<<rkNewVendorTitle<<L"]");
	}
	return true;
}

bool PgLocalVendorMgrImpl::Delete(BM::GUID const& rkCharGuid)
{
	return m_kContVendorList.erase( rkCharGuid );
}

bool PgLocalVendorMgrImpl::Rename(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle)
{
	CONT_VENDOR_LIST::iterator iterVendor = m_kContVendorList.find(rkCharGuid);
	if( m_kContVendorList.end() != iterVendor)
	{
		std::wstring kTemp = rkNewVendorTitle;
		(*iterVendor).second.swap(kTemp);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"PgLocalVendorMgrImpl::Rename False ["<<rkNewVendorTitle<<L"]");
	return false;
}

//
//
PgLocalVendorMgr::PgLocalVendorMgr()
{
}
PgLocalVendorMgr::~PgLocalVendorMgr()
{
}

void PgLocalVendorMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	Instance()->Clear();
}
bool PgLocalVendorMgr::ReadToPacketVendorName(BM::GUID const& rkCharGuid, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ReadToPacketVendorName(rkCharGuid, rkPacket);
}

bool PgLocalVendorMgr::Create(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Create(rkCharGuid, rkNewVendorTitle);
}

bool PgLocalVendorMgr::Rename(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Rename(rkCharGuid, rkNewVendorTitle);
}

bool PgLocalVendorMgr::Delete(BM::GUID const& rkCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->Delete(rkCharGuid);
}