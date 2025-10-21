#include "stdafx.h"
#include "PgSwitchAssignMgr.h"

PgSwitchAssignMgr::PgSwitchAssignMgr()
:	m_iMaxPlayerCount(0)
,	m_iNowPlayerCount(0)
{
}

PgSwitchAssignMgr::~PgSwitchAssignMgr()
{

}

bool PgSwitchAssignMgr::ConnectSwitch( SERVER_IDENTITY const &kSI )
{	
	// і» Г¤іОАЗ ЅєА§ДЎёё °ьё®ЗШѕЯ ЗСґЩ!
	if (	kSI.nChannel == g_kProcessCfg.ChannelNo()
		&&	kSI.nServerType == CEL::ST_SWITCH )
	{
		std::pair<CONT_SWITCH_ASSIGN::iterator, bool> kPair = m_kContSwitchServer.insert( std::make_pair( kSI, SET_GUID() ) );
		if ( kPair.second )
		{
			return true;
		}

		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error ServerNo[") << kSI.nServerNo << _T("]") );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSwitchAssignMgr::DisconnectSwitch( SERVER_IDENTITY const &kSI )
{
	if ( kSI.nServerType == CEL::ST_SWITCH )
	{
		if ( m_kContSwitchServer.erase( kSI ) )
		{
			_RefreshUserCount();
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSwitchAssignMgr::GetBalanceSwitch( SERVER_IDENTITY &kOutSI, bool const bIsIgnoreMax)const
{
	if ( m_kContSwitchServer.size() )
	{
//		if ( m_iNowPlayerCount )
//		{
			if( m_iNowPlayerCount < (MaxPlayerCount()) 
			||	bIsIgnoreMax )//АОїшБ¦ЗС №«ЅГ.
			{
				// ЗцАз БўјУ АЇАъ іЄґ©±в ЅєА§ДЎ ј­№ц јэАЪ +3А» ЗШБЦѕо є§·±ЅМА» ЗПёй µИґЩ.
				size_t const iMaxCount = ( m_iNowPlayerCount / m_kContSwitchServer.size() ) + 3;
				CONT_SWITCH_ASSIGN::const_iterator itr = m_kContSwitchServer.begin();
				for ( ; itr!=m_kContSwitchServer.end(); ++itr )
				{
					if ( itr->second.size() < iMaxCount )
					{
						kOutSI = itr->first;
						return true;
					}
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("MaxPlayerCount Limit! NowPlayerCount[") << m_iNowPlayerCount << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
//		}
//		else
//		{
//			kOutSI = m_kContSwitchServer.begin()->first;
//			return true;
//		}
	}

	INFO_LOG( BM::LOG_LV5, _T("[") << __FL__ << _T("]Error") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSwitchAssignMgr::InsertAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid )
{
	CONT_SWITCH_ASSIGN::iterator itr = m_kContSwitchServer.find( kSI );
	if ( itr!=m_kContSwitchServer.end() )
	{
		std::pair<SET_GUID::iterator, bool> kPair = itr->second.insert( kMemberGuid );
		if ( !kPair.second )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("ChannelNo[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("] Failed MemberGUID[") << kMemberGuid.str().c_str() << _T("]") );
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("ChannelNo[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("] Failed MemberGUID[") << kMemberGuid.str().c_str() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		++m_iNowPlayerCount;
		return true;
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T("ChannelNo[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("] Error MemberGUID[") << kMemberGuid.str().c_str() << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSwitchAssignMgr::RemoveAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid )
{
	CONT_SWITCH_ASSIGN::iterator itr = m_kContSwitchServer.find( kSI );
	if ( itr!=m_kContSwitchServer.end() )
	{
		if(!itr->second.empty())
		{
			if ( !itr->second.erase( kMemberGuid ) )
			{
				//VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[%s] ChannelNo[%d] ServerNo[%d Failed MemberGUID[%s]"), __FUNCTIONW__, kSI.nChannel, kSI.nServerNo, kMemberGuid.str().c_str() );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			--m_iNowPlayerCount;
			return true;
		}
		return true;
	}
	//ЖЫєнёЇ јѕЕНµйїЎ ЅєА§ДЎё¦ ЗТґзЗПБц ѕК±в ¶§№®їЎ, ѕЖ·Ў ·О±ЧґВ АЗ№М ѕшАЅ
	//INFO_LOG( BM::LOG_LV0, __FL__ << _T("ChannelNo[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("] Error MemberGUID[") << kMemberGuid.str().c_str() << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgSwitchAssignMgr::_RefreshUserCount()
{
	m_iNowPlayerCount = 0;

	CONT_SWITCH_ASSIGN::const_iterator itr = m_kContSwitchServer.begin();
	for ( ; itr!=m_kContSwitchServer.end(); ++itr )
	{
		m_iNowPlayerCount += itr->second.size();
	}
}

bool PgSwitchAssignMgr::SendToUser( BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const
{
	CONT_SWITCH_ASSIGN::const_iterator itr = m_kContSwitchServer.begin();
	for ( ; itr!=m_kContSwitchServer.end(); ++itr )
	{
		SET_GUID::const_iterator member_itr = itr->second.find( kMemberGuid );
		if ( member_itr != itr->second.end() )
		{
			return ::SendToUser( kMemberGuid, itr->first, kPacket );
		}
	}
	return false;
}

bool PgSwitchAssignMgr_Wrapper::ConnectSwitch( SERVER_IDENTITY const &kSI )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->ConnectSwitch( kSI );
}

bool PgSwitchAssignMgr_Wrapper::DisconnectSwitch( SERVER_IDENTITY const &kSI )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->DisconnectSwitch( kSI );
}

bool PgSwitchAssignMgr_Wrapper::GetBalanceSwitch( SERVER_IDENTITY &kOutSI, bool const bIsIgnoreMax )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetBalanceSwitch( kOutSI, bIsIgnoreMax );
}

bool PgSwitchAssignMgr_Wrapper::InsertAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->InsertAssignSwitch( kSI, kMemberGuid );
}

bool PgSwitchAssignMgr_Wrapper::RemoveAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->RemoveAssignSwitch( kSI, kMemberGuid );
}

void PgSwitchAssignMgr_Wrapper::MaxPlayerCount( size_t const iMaxPlayerCount )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->MaxPlayerCount( iMaxPlayerCount );
}

size_t PgSwitchAssignMgr_Wrapper::MaxPlayerCount()const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->MaxPlayerCount();
}

size_t PgSwitchAssignMgr_Wrapper::NowPlayerCount()const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->NowPlayerCount();
}

bool PgSwitchAssignMgr_Wrapper::SendToUser( BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->SendToUser( kMemberGuid, kPacket );
}
