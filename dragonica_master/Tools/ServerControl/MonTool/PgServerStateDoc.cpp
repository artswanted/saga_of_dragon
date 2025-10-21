#include "StdAfx.h"
#include "PgServerStateDoc.h"

PgServerStateDoc::PgServerStateDoc(void)
:	m_iConsentSite(0)
,	m_wRecordCCUTime(0)
,	m_dwLastRefreshTime(0)
,	m_bUseBeep(false)
{
	bControlLevel(false);
}

PgServerStateDoc::~PgServerStateDoc(void)
{
}

void PgServerStateDoc::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kControlInfo.clear();
}

bool PgServerStateDoc::RefreshServerState( BM::CPacket& rkPacket )
{
	bool bRet = false;


	BM::CAutoMutex kLock(m_kMutex);

	m_dwLastRefreshTime = BM::GetTime32();
	m_kContLiveSMC.clear();

	size_t iSize = 0;
	rkPacket.Pop( iSize );

	while ( iSize-- )
	{
		int iSiteNo = 0;
		rkPacket.Pop( iSiteNo );

		CONT_CONTROL_INFO::iterator itr = m_kControlInfo.find( iSiteNo );
		if ( itr == m_kControlInfo.end() )
		{
			auto kPair = m_kControlInfo.insert( std::make_pair( iSiteNo, CONT_CONTROL_INFO::mapped_type()) );
			itr = kPair.first;
		}

		//		itr->second.kContServerOldStateView = itr->second.kContServerStateView;
		itr->second.kContServerStateView.clear();

		int iIndex = 1;
		SERVER_IDENTITY kServerIdentity;
		CONT_SERVER_STATE_FOR_VIEW::mapped_type kElement;
		size_t iSize2 = 0;
		rkPacket.Pop( iSize2 );
		while ( iSize2-- )
		{
			kServerIdentity.ReadFromPacket( rkPacket );
			kElement.ReadFromPacket( rkPacket );

			if ( CEL::ST_CONSENT == kServerIdentity.nServerType )
			{
				m_iConsentSite = iSiteNo;
			}
			else if ( CEL::ST_SUB_MACHINE_CONTROL == kServerIdentity.nServerType )
			{
				if (	true == kElement.kProcessInfo.IsCorrect()
					&&	true == kElement.bReadyToService )
				{
					m_kContLiveSMC.insert( kElement.kTbl.strMachinIP );
				}
			}

			itr->second.kContServerStateView.insert( std::make_pair( iIndex, kElement ) );
			++iIndex;
		}

		itr->second.kRealmMgr.ReadFromPacket( rkPacket );
	}

	// 	int i = 0;
	// 	CONT_SERVER_STATE_EX::const_iterator state_itor = kContServerState.begin();
	// 	while(state_itor != kContServerState.end())
	// 	{
	// 		CONT_SERVER_STATE_FOR_VIEW::iterator Old_Itr = m_kContServerOldStateView.begin();
	// 		while(Old_Itr != m_kContServerOldStateView.end())
	// 		{
	// 			CONT_SERVER_STATE_FOR_VIEW::mapped_type kElement = Old_Itr->second;
	// 			SERVER_IDENTITY kOld_SI = kElement.kTbl;
	// 			SERVER_IDENTITY kSI = state_itor->second.kTbl;
	// 			if(kOld_SI == kSI)
	// 			{
	// 				if(kElement.bIsException != state_itor->second.bIsException
	// 					|| kElement.bReadyToService != state_itor->second.bReadyToService
	// 					|| kElement.uiMaxUsers != state_itor->second.uiMaxUsers
	// 					|| kElement.uiConnectionUsers != state_itor->second.uiConnectionUsers)
	// 				{
	// 					if(kElement.kTbl.nServerType != CEL::ST_SUB_MACHINE_CONTROL)
	// 						bRet = true;
	// 				}
	// 			}
	// 
	// 			++Old_Itr;
	// 		}
	// 
	// 		m_kContServerStateView.insert(std::make_pair(i++, (*state_itor).second));
	// 
	// 		++state_itor;
	// 	}

	return true;
}


bool PgServerStateDoc::GetServerInfo( int const iSiteNo, int const iServerIdx, CONT_SERVER_STATE_EX::mapped_type& rkOutInfo )const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CONTROL_INFO::const_iterator control_itr = m_kControlInfo.find ( iSiteNo );
	if ( control_itr != m_kControlInfo.end() )
	{
		CONT_SERVER_STATE_FOR_VIEW::const_iterator view_itr = control_itr->second.kContServerStateView.find( iServerIdx );
		if ( view_itr != control_itr->second.kContServerStateView.end() )
		{
			rkOutInfo = view_itr->second;
			return true;
		}
	}

	return false;
}

bool PgServerStateDoc::GetServerList( int const iSiteNo, CONT_SERVER_STATE_FOR_VIEW &rkServerList )const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_CONTROL_INFO::const_iterator itr = m_kControlInfo.find( iSiteNo );
	if ( itr != m_kControlInfo.end() )
	{
		rkServerList = itr->second.kContServerStateView;
		return true;
	}
	return false;
}

bool PgServerStateDoc::IsAliveSMC(std::wstring const &kMachineIP) const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SMC_STATE::const_iterator itr = m_kContLiveSMC.find( kMachineIP );
	return itr != m_kContLiveSMC.end();
}

bool PgServerStateDoc::WriteCCU( SYSTEMTIME const &kNowTime )const
{
	BM::ReserveFolder( g_kGameServerMgr.RecordPath() );

	wchar_t wszFileName[MAX_PATH] = { 0, };
	::swprintf_s( wszFileName, MAX_PATH, L"%s/%04u%02u%02u_CCU.txt", m_wstrRecordPath.c_str(), kNowTime.wYear, kNowTime.wMonth, kNowTime.wDay );

	BM::CAutoMutex kLock( m_kMutex );

	std::wfstream out;
	out.open( wszFileName, std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary );
	if ( true == out.is_open() )
	{
		out.imbue( std::locale(g_kLocal.GetLocale().c_str()) );

		wchar_t wszTemp[MAX_PATH] = {0,};
		::swprintf_s( wszTemp, MAX_PATH, L"[%04u-%02u-%02u %02u:%02u:%02u]\n", kNowTime.wYear, kNowTime.wMonth, kNowTime.wDay, kNowTime.wHour, kNowTime.wMinute, kNowTime.wSecond );
		out.write( wszTemp, static_cast<std::streamsize>(wcslen(wszTemp)) );

		std::map< short, SRealmUserRecord >	kContRecord;
		SRealmUserRecord					kTempRecord;

		CONT_CONTROL_INFO::const_iterator itr = m_kControlInfo.begin();
		for ( ; itr != m_kControlInfo.end() ; ++itr )
		{
			unsigned int iSiteUser = 0;

			CONT_SERVER_STATE_FOR_VIEW const &kList = itr->second.kContServerStateView;

			CONT_SERVER_STATE_FOR_VIEW::const_iterator state_itr = kList.begin();
			for ( ; state_itr != kList.end() ; ++state_itr )
			{
				switch ( state_itr->second.kTbl.nServerType )
				{
				case CEL::ST_IMMIGRATION:
					{
						iSiteUser = state_itr->second.uiConnectionUsers;
					}break;
				case CEL::ST_CONTENTS:
					{
						std::map< short, SRealmUserRecord >auto kPair = kContRecord.insert( std::make_pair( state_itr->second.kTbl.nRealm, kTempRecord ) );
						kPair.first->second.iUserCount = state_itr->second.uiConnectionUsers;
					}break;
				case CEL::ST_CENTER:
					{
						std::map< short, SRealmUserRecord >auto kPair = kContRecord.insert( std::make_pair( state_itr->second.kTbl.nRealm, kTempRecord ) );
						kPair.first->second.kContChannel.insert( std::make_pair( state_itr->second.kTbl.nChannel, state_itr->second.uiConnectionUsers ) );
					}break;
				}
			}

			out << L"* Site_" << itr->first << L" : " << iSiteUser << std::endl;

			std::map< short, SRealmUserRecord >::const_iterator rec_itr = kContRecord.begin();
			for ( ; rec_itr != kContRecord.end() ; ++rec_itr )
			{
				PgRealm kRealm;
				itr->second.kRealmMgr.GetRealm( rec_itr->first, kRealm );

				size_t iChannelUser = 0;// 임시코드. 컨텐츠 서버에서 마이홈 갯수가 유저 카운트에 포함되는 버그로 인해, CCU가 제대로 안나옴. 
				std::map< short, unsigned int >::const_iterator chn_itr = rec_itr->second.kContChannel.begin();
				for ( ; chn_itr != rec_itr->second.kContChannel.end() ; ++chn_itr )
				{
					iChannelUser += chn_itr->second;
				}
				out << L" - Realm[" << kRealm.Name().c_str() << L"] : " << iChannelUser << L"  /  ";

				chn_itr = rec_itr->second.kContChannel.begin();
				for ( ; chn_itr != rec_itr->second.kContChannel.end() ; ++chn_itr )
				{
					out << L"C" << chn_itr->first << L"<" << chn_itr->second << L"> ";
				}

				out << std::endl;
			}
		}

		out << std::endl;
		out.flush();
		out.close();
		return true;
	}

	return false;
}

size_t PgServerStateDoc::GetCCU(CEL::E_SESSION_TYPE const eServerType, int const iSite, int const iRealm, int const iChannel)
{
	CONT_CONTROL_INFO::const_iterator Control_Itor = m_kControlInfo.find(iSite);
	if( m_kControlInfo.end() == Control_Itor )
	{
		return 0;
	}

	size_t iCCU = 0;
	CONT_SERVER_STATE_FOR_VIEW const &kList = Control_Itor->second.kContServerStateView;
	CONT_SERVER_STATE_FOR_VIEW::const_iterator state_itr = kList.begin();
	for ( ; state_itr != kList.end() ; ++state_itr )
	{
		switch ( eServerType )
		{
		case CEL::ST_IMMIGRATION:
			{
				if( eServerType == state_itr->second.kTbl.nServerType )
				{
					return state_itr->second.uiConnectionUsers;
				}
			}break;
		case CEL::ST_CONTENTS:
			{
				if( CEL::ST_CENTER == state_itr->second.kTbl.nServerType
					&& iRealm == state_itr->second.kTbl.nRealm )
				{
					iCCU += state_itr->second.uiConnectionUsers;
				}
			}break;
		case CEL::ST_CENTER:
			{// 임시코드. 컨텐츠 서버에서 마이홈 갯수가 유저 카운트에 포함되는 버그로 인해, CCU가 제대로 안나옴. 
				if( eServerType == state_itr->second.kTbl.nServerType 
					&& iRealm == state_itr->second.kTbl.nRealm 
					&& iChannel == state_itr->second.kTbl.nChannel )
				{
					return state_itr->second.uiConnectionUsers;
				}
			}break;
		}
	}

	if( CEL::ST_CONTENTS == eServerType )
	{
		return iCCU;
	}

	return 0;
}

bool PgServerStateDoc::GetServerList(short nServerType, CONT_SERVER_STATE_FOR_VIEW const & rkServerList, ContServerID& rkOutServerIDList)
{
	CONT_SERVER_STATE_FOR_VIEW::const_iterator itor = rkServerList.begin();
	while ( rkServerList.end() != itor)
	{
		CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = itor->second;
		if(	nServerType ==	kElement.kTbl.nServerType 
			&& true == kElement.bReadyToService )
		{
			rkOutServerIDList.push_back(kElement.kTbl);
		}
		++itor;
	}

	if( rkOutServerIDList.empty() )
	{
		return false;
	}

	return true;
}

int PgServerStateDoc::GetServerCount(int const iSiteNo, short const nServerType, short const nRealmNo)
{
	BM::CAutoMutex kLock(m_kMutex);

	int iRetCount = 0;
	CONT_CONTROL_INFO::const_iterator Control_Itor = m_kControlInfo.find( iSiteNo );
	if ( m_kControlInfo.end() == Control_Itor )
	{
		return iRetCount;
	}
	CONT_SERVER_STATE_FOR_VIEW const & kServerList = Control_Itor->second.kContServerStateView;
	CONT_SERVER_STATE_FOR_VIEW::const_iterator Server_Itor = kServerList.begin();
	while ( kServerList.end() != Server_Itor)
	{
		CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = Server_Itor->second;
		if(	nServerType ==	kElement.kTbl.nServerType )
		{
			if( nRealmNo == kElement.kTbl.nRealm
				|| nRealmNo == 0 )
			{
				++iRetCount;
			}
		}
		++Server_Itor;
	}

	return iRetCount;
}

bool PgServerStateDoc::GetServerInfo( int const iSiteNo, SERVER_IDENTITY& rkSI, CONT_SERVER_STATE_EX::mapped_type& rkOutInfo )const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CONTROL_INFO::const_iterator Control_Itor = m_kControlInfo.find( iSiteNo );
	if ( m_kControlInfo.end() == Control_Itor )
	{
		return false;
	}
	CONT_SERVER_STATE_FOR_VIEW const & kServerList = Control_Itor->second.kContServerStateView;
	CONT_SERVER_STATE_FOR_VIEW::const_iterator Server_Itor = kServerList.begin();
	while ( kServerList.end() != Server_Itor)
	{
		CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement = Server_Itor->second;
		if(	rkSI.nServerType == kElement.kTbl.nServerType 
			&& rkSI.nRealm == kElement.kTbl.nRealm
			&& rkSI.nChannel == kElement.kTbl.nChannel)
		{
			rkSI = kElement.kTbl;
			rkOutInfo = kElement;
			return true;
		}
		++Server_Itor;
	}

	return false;
}

namespace ServerConfigUtil
{
std::wstring GetServerTypeName(short nServerType)
{
	std::wstring wstrName = L"";
	switch(nServerType)
	{
	case CEL::ST_NONE :{wstrName = L"NONE";}break;
	case CEL::ST_CENTER :{wstrName = L"Center";}break;
	case CEL::ST_MAP : wstrName = L"Map"; break;
	case CEL::ST_LOGIN : wstrName = L"Login"; break;
	case CEL::ST_SWITCH : wstrName = L"Switch"; break;
	case CEL::ST_MACHINE_CONTROL : wstrName = L"MMC"; break;
	case CEL::ST_SUB_MACHINE_CONTROL:	wstrName = L"SMC"; break;
		//	case CEL::ST_RELAY : wstrName = L"RELAY"; break;
		//	case CEL::ST_USER : wstrName = L"USER"; break;
	case CEL::ST_CONTENTS : wstrName = L"Contents"; break;
		//	case CEL::ST_CONTENTS : wstrName = L"CONTENTS"; break;
	case CEL::ST_LOG : wstrName = L"Log"; break;
		//	case CEL::ST_MMAP : wstrName = L"MMAP"; break;
	case CEL::ST_GMSERVER : wstrName = L"GM"; break;
	case CEL::ST_IMMIGRATION : wstrName = L"Immigration"; break;
	case CEL::ST_CONSENT:{wstrName = L"Consent";}break;
	default:
		{
			wstrName = L"UNKNOWN";
		}break;
	}

	return wstrName;
}

}
