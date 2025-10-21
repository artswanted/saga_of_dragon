#include "stdafx.h"
#include "DataPack/Packinfo.h"
#include "PgEmporiaFunction.h"

PgEmporiaFunction::PgEmporiaFunction()
{

}

PgEmporiaFunction::PgEmporiaFunction( PgEmporiaFunction const &rhs )
:	m_kContFunc(rhs.m_kContFunc)
//,	m_kReqAddingFuncID(rhs.m_kReqAddingFuncID)
{

}

PgEmporiaFunction::~PgEmporiaFunction()
{

}

PgEmporiaFunction& PgEmporiaFunction::operator=( PgEmporiaFunction const &rhs )
{
	m_kContFunc = rhs.m_kContFunc;
//	m_kReqAddingFuncID = rhs.m_kReqAddingFuncID;
	return *this;
}

void PgEmporiaFunction::Clear()
{
	m_kContFunc.clear();
//	m_kReqAddingFuncID.Clear();
}

void PgEmporiaFunction::Swap( PgEmporiaFunction &rhs )
{
	m_kContFunc.swap( rhs.m_kContFunc );
//	std::swap( m_kReqAddingFuncID, rhs.m_kReqAddingFuncID );
}

void PgEmporiaFunction::WriteToPacket( BM::Stream &kPacket )const
{
	PU::TWriteTable_AM( kPacket, m_kContFunc );
}

void PgEmporiaFunction::ReadFromPacket( BM::Stream &kPacket )
{
	m_kContFunc.clear();
	PU::TLoadTable_AM( kPacket, m_kContFunc );
}

bool PgEmporiaFunction::AddFunction( CONT_EMPORIA_FUNC::key_type const iType, CONT_EMPORIA_FUNC::mapped_type const &kFunction )
{
	auto kPair = m_kContFunc.insert( std::make_pair( iType, kFunction ) );
	if ( !kPair.second )
	{
		kPair.first->second = kFunction;
	}
	return true;
}

bool PgEmporiaFunction::RemoveFunction( CONT_EMPORIA_FUNC::key_type const iType )
{
	return m_kContFunc.erase( iType ) > 0;
}

bool PgEmporiaFunction::GetFunction( CONT_EMPORIA_FUNC::key_type const iType, CONT_EMPORIA_FUNC::mapped_type &kFunction )const
{
	CONT_EMPORIA_FUNC::const_iterator func_itr = m_kContFunc.find( iType );
	if ( func_itr != m_kContFunc.end() )
	{
		kFunction = func_itr->second;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return true;
	}
	return false;
}

// bool PgEmporiaFunction::NewAddingFunction( BM::GUID &kReqAddingFuncID )
// {
// 	if ( !IsAddingFunction() )
// 	{
// 		m_kReqAddingFuncID.Generate();
// 		kReqAddingFuncID = m_kReqAddingFuncID;
// 		return true;
// 	}
// 	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 	return false;
// }

// bool PgEmporiaFunction::ClearAddingFunction( BM::GUID const &kRecvGuid )
// {
// 	if ( m_kReqAddingFuncID == kRecvGuid )
// 	{
// 		m_kReqAddingFuncID.Clear();
// 		return true;
// 	}
// 	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 	return false;
// }

size_t PgEmporiaFunction::CheckDelete( __int64 const i64NowTime, CONT_EMPORIA_FUNCKEY &kContDeleteKey )
{
	kContDeleteKey.clear();

	CONT_EMPORIA_FUNC::iterator func_itr = m_kContFunc.begin();
	while ( func_itr!=m_kContFunc.end() )
	{
		if ( func_itr->second.i64ExpirationDate < i64NowTime )
		{
			// 지워져야 한다.
			kContDeleteKey.insert( func_itr->first );
			func_itr = m_kContFunc.erase( func_itr );
		}
		else
		{
			++func_itr;
		}
	}

	return kContDeleteKey.size();
}