#include "stdafx.h"
#include "PgLChannelMgr.h"
/*
HRESULT PgLChannelMgr::RegistChannel(int const iLChannelNo)
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( iLChannelNo < MaxChannelCount() )
	{
		ContChannelState::_Pairib kPair = m_kTotalState.insert(std::make_pair( iLChannelNo,SChannelState(MaxUserCountForChannel()) ) );
		if ( kPair.second )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT PgLChannelMgr::UnRegistChannel(int const iLChannelNo)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	ContChannelState::iterator ch_itr = m_kTotalState.find(iLChannelNo);
	if ( ch_itr != m_kTotalState.end() )
	{
		HRESULT eRet = S_OK;
		if ( ch_itr->second.now_user > 0 )
		{
			AddTotalUserCount(ch_itr->second.now_user);
			INFO_LOG(BM::LOG_LV0,_T("[%s] Now User Count[%d]"),__FUNCTIONW__,ch_itr->second.now_user);
			eRet = S_FALSE;
		}
		m_kTotalState.erase(ch_itr);
		return eRet;
	}
	return E_FAIL;
}

HRESULT PgLChannelMgr::GetMovableChannel(int const iLChannelNo,int const iCount)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContChannelState::iterator ch_itr = m_kTotalState.find(iLChannelNo);
	if ( ch_itr != m_kTotalState.end() )
	{
		if ( ch_itr->second.CanAdd(iCount) )
		{
			return S_OK;
		}
	}
	else
	{// ёшГЈѕТґЩёй
		if ( iLChannelNo < MaxChannelCount() )
		{
			// »х·О ёёµй јц АЦґЩґВ АЗ№М
			return S_FALSE;
		}
	}
	return E_FAIL;
}

HRESULT PgLChannelMgr::AddUserCount(int const iLChannelNo,int const iCount)
{
	BM::CAutoMutex kLock(m_kMutex);

	ContChannelState::iterator ch_itr = m_kTotalState.find(iLChannelNo);
	if ( ch_itr != m_kTotalState.end() )
	{
		if ( ch_itr->second.Add(iCount) )
		{
			return S_OK;
		}
		else
		{
			return S_FALSE;
		}
	}
	else
	{
		if ( iCount > 0 )
		{
			if ( SUCCEEDED(RegistChannel(iLChannelNo)) )
			{// Аз±Н ИЈГв
				return AddUserCount(iLChannelNo,iCount);
			}
		}
	}
	return E_FAIL;
}

HRESULT PgLChannelMgr::GetAutoLChannel(SGroundKey& rkGndKey,int const iCount)
{
	BM::CAutoMutex kLock(m_kMutex);
	int iLChannelNo = 0;
	ContChannelState::iterator ch_itr;
	for( ch_itr=m_kTotalState.begin(); ch_itr!=m_kTotalState.end(); ++ch_itr )
	{
		iLChannelNo = ch_itr->first;
		if ( ch_itr->second.CanAdd(iCount) )
		{
			return S_OK;
		}
	}

	return RegistChannel(iLChannelNo);
}
*/