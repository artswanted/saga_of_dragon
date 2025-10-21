#include "stdafx.h"
#include "PgRestrict.h"
#include "Variant/PgEventView.h"

PgRestrict::PgRestrict()
:	m_kMaxUser(0)
,	m_kCurrentUser(0)
{

}

PgRestrict::~PgRestrict()
{

}

bool PgRestrict::Set( CONT_DEF_RESTRICTIONS const &kRestrictDef, SRestrictOnChannel const &kOptions )
{
	ContChecker kContChecker;
	kContChecker.reserve( kOptions.kContOptions.size() );

	CONT_RESTRICT_OPTION::const_iterator op_itr = kOptions.kContOptions.begin();
	for ( ; op_itr!=kOptions.kContOptions.end(); ++op_itr )
	{
		CONT_DEF_RESTRICTIONS::const_iterator def_itr = kRestrictDef.find( *op_itr );
		if ( def_itr != kRestrictDef.end() )
		{
			kContChecker.push_back( def_itr->second );
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Not Found Option ID["<<*op_itr<<L"] Realm["<<kOptions.nRealmNo<<L"] Channel["<<kOptions.nChannelNo<<L"]" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	MaxUser( kOptions.iMaxUserCount );
	m_kContCheck.swap( kContChecker );
	return true;
}

int PgRestrict::IsAccess( SAuthInfo const &kAuthInfo )const
{
	if ( MaxUser() <= CurrentUser() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
		return 1;
	}

	ContChecker::const_iterator itr = m_kContCheck.begin();
	for ( ; itr!=m_kContCheck.end(); ++itr )
	{
		if ( !IsAccess( *itr, kAuthInfo ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << itr->iErrorMsg);
			return itr->iErrorMsg;
		}
	}
	return 0;
}

bool PgRestrict::IsAccess( SRestrictChecker const &kChecker, SAuthInfo const &kAuthInfo )const
{
	switch( kChecker.kType )
	{
	case RESTRICT_GENDER:
		{
			if ( kChecker.iValue1 != (int)kAuthInfo.ucGender)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case RESTRICT_AGE:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime(&kNowTime);
			int const iAge = (int)( ((WORD)kAuthInfo.dtUserBirth.month > kNowTime.wMonth) ? (kNowTime.wYear - (WORD)kAuthInfo.dtUserBirth.year - 1) : (kNowTime.wYear - (WORD)kAuthInfo.dtUserBirth.year) );

			if ( (kChecker.iValue1 > iAge) || (kChecker.iValue2 < iAge) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case RESTRICT_AGEKOREAN:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime(&kNowTime);
			int const iAge = (int)kNowTime.wYear - (int)kAuthInfo.dtUserBirth.year + 1;

			if ( (kChecker.iValue1 > iAge) || (kChecker.iValue2 < iAge) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case RESTRICT_IP:
		{
			short nArea = (short)(kChecker.iValue1);
			if ( 0 == (nArea & kAuthInfo.m_kConnArea.nArea) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}
	return true;
}

void PgRestrict::WriteToPacket( BM::Stream &kPacket )const
{
	PU::TWriteArray_M( kPacket, m_kContCheck );
}

void PgRestrict::ReadFromPacket( BM::Stream &kPacket )
{
	m_kContCheck.clear();
	PU::TLoadArray_M( kPacket, m_kContCheck );
}
