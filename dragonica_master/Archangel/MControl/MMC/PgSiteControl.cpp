#include "stdafx.h"
#include "PgSiteControl.h"
#include "PgMMCManager.h"

std::wstring const ADMIN_GATEWAY_ANS_PACKET_TYPE = L"11002";

PgSiteControl::PgSiteControl( int const iDBIndex )
:	m_iDBIndex(iDBIndex)
,	m_pkSession(NULL)
,	m_bReqConnect(false)
,	m_bRefresh(false)
{

}

PgSiteControl::~PgSiteControl(void)
{

}

bool PgSiteControl::InitRealmMgr()
{
	CheckServerInfo();
	CONT_SERVER_HASH kServerHash;
	GetServerHash( kServerHash );
	return m_kRealmMgr.Init( m_kRealmCandi, kServerHash );
}

void PgSiteControl::GetServerList( short const nServerType, ContServerSiteID &rkOut )const
{
	CONT_SERVER_STATE_EX2::const_iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		if ( nServerType == itr->first.nServerType )
		{
			ContServerSiteID::value_type kValue( itr->first, static_cast<short>(m_iDBIndex) );
			rkOut.push_back( kValue );
		}
	}
}

void PgSiteControl::GetServerHash( CONT_SERVER_HASH &rkOutServerHash )const
{
	CONT_SERVER_STATE_EX2::const_iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		rkOutServerHash.insert( std::make_pair( itr->first, itr->second.kTbl ) );
	}
}

bool PgSiteControl::AddServerInfo( TBL_SERVERLIST const &rkInfo )
{
	tagServerIdentity kServerIdentity = static_cast<tagServerIdentity>(rkInfo);
	CONT_SERVER_STATE_EX2::mapped_type kServerState;
	kServerState.kTbl = rkInfo;

	auto kPair = m_kTotalServerState.insert(std::make_pair(kServerIdentity, kServerState));
	return kPair.second;
}

void PgSiteControl::CheckServerInfo(void)
{
	std::set< std::wstring > kContCheckIP;
	SERVER_IDENTITY kConsentIdentity;

	CONT_SERVER_STATE_EX2::iterator itr_consent = m_kTotalServerState.end();
	CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
	while ( itr != m_kTotalServerState.end() )
	{
		switch ( itr->second.kTbl.nServerType )
		{
		case CEL::ST_IMMIGRATION:
			{
				if ( !m_kImmAddr.IsCorrect() )
				{
					m_kImmAddr = itr->second.kTbl.addrServerBind;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Immigration error" );
				}
			}break;
		case CEL::ST_CONSENT:
			{
				itr_consent = itr;
			}break;
		case CEL::ST_SUB_MACHINE_CONTROL:
			{
				kContCheckIP.insert( itr->second.kTbl.strMachinIP );
			}break;
		case CEL::ST_MACHINE_CONTROL:
			{
				itr = m_kTotalServerState.erase( itr );
				continue;
			}
		default:
			{

			}break;
		}

		++itr;
	}

	if ( itr_consent != m_kTotalServerState.end() )
	{
		if ( kContCheckIP.end() == kContCheckIP.find( itr_consent->second.kTbl.strMachinIP ) )
		{
			m_kTotalServerState.erase( itr_consent );
		}
	}
}

void PgSiteControl::Connect( BM::GUID const &kConnecterID )
{
	if ( !m_pkSession )
	{
		if ( !m_bReqConnect )
		{
			if ( true == m_kImmAddr.IsCorrect() )
			{
				m_bReqConnect = g_kCoreCenter.Connect( m_kImmAddr, CEL::SESSION_KEY(kConnecterID, BM::GUID::NullData()) );	
			}
		}	
	}
}

bool PgSiteControl::SetConnect( CEL::CSession_Base* pkSession, bool const bConnect, SERVER_IDENTITY &rkSI  )
{
	if ( true == bConnect )
	{
		if ( !m_pkSession )
		{
			if ( m_kImmAddr == pkSession->Addr() )
			{
				m_pkSession = pkSession;
				pkSession->m_kSessionData.Push( m_iDBIndex );
				return true;
			}
		}
	}
	else
	{
		if ( pkSession == m_pkSession )
		{
			m_pkSession = NULL;
			m_bReqConnect = false;
			m_bRefresh = true;

			CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.begin();
			for ( ; server_itr!=m_kTotalServerState.end() ; ++server_itr )
			{
				CONT_SERVER_STATE_EX2::mapped_type &kElement = server_itr->second;
				if ( CEL::ST_SUB_MACHINE_CONTROL != server_itr->second.kTbl.nServerType )
				{
					if ( true == server_itr->second.bReadyToService )
					{
						rkSI = server_itr->second.kTbl;
						server_itr->second.SetDisconnected();
					}
				}
			}

			return true;
		}
		else if ( !m_pkSession )
		{
			if ( pkSession->Addr() == m_kImmAddr )
			{
				m_bReqConnect = false;
				return true;
			}
		}
	}
	return false;
}

bool PgSiteControl::SetSyncEndSMC( CEL::CSession_Base* pkSession )
{
	pkSession->m_kSessionData.PosAdjust();

	SERVER_IDENTITY_SITE kSI;
	kSI.ReadFromPacket( pkSession->m_kSessionData );

	pkSession->m_kSessionData.PosAdjust();

	bool bRet = false;
	CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.begin();
	for ( ; server_itr!=m_kTotalServerState.end() ; ++server_itr )
	{
		if ( pkSession->Addr().IP() == server_itr->second.kTbl.strMachinIP )
		{
			if( CEL::ST_SUB_MACHINE_CONTROL == server_itr->second.kTbl.nServerType )
			{
				if( bRet )
				{
					ASSERT_LOG( false, BM::LOG_LV0, L"Duplicate SMC. < IP : " << pkSession->Addr().IP() << L" >" 
						<< L" < ServerNo : " << server_itr->second.kTbl.nServerNo <<L">" );
				}
				server_itr->second.bReadyToService = true;
				m_bRefresh = true;
				bRet = true;
			}
			server_itr->second.pkSMCSession = pkSession;
		}
	}

	return bRet;
}

bool PgSiteControl::SetConnectSMC( CEL::CSession_Base* pkSession, SERVER_IDENTITY_SITE const &kRecvSI, bool const bConnect )
{
	SERVER_IDENTITY kServerIdentity = static_cast<SERVER_IDENTITY>(kRecvSI);

	if ( true == bConnect )
	{
		CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.find( kServerIdentity );
		if ( server_itr != m_kTotalServerState.end() )
		{
			CONT_SERVER_STATE_EX2::mapped_type &kElement = server_itr->second;
			if ( !kElement.pkSMCSession )
			{
				if ( pkSession->Addr().IP() == kElement.kTbl.strMachinIP )
				{
					return true;
				}
			}
		}
		return false;
	}
	else
	{
		CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.begin();
		for ( ; server_itr!=m_kTotalServerState.end() ; ++server_itr )
		{
			CONT_SERVER_STATE_EX2::mapped_type &kElement = server_itr->second;
			if ( pkSession == kElement.pkSMCSession )
			{
				server_itr->second.kProcessInfo = tagPROCESS_INFORMATION_EX();
				if ( CEL::ST_SUB_MACHINE_CONTROL == server_itr->second.kTbl.nServerType )
				{
					server_itr->second.SetDisconnected();
				}
				kElement.pkSMCSession = NULL;
				m_bRefresh = true;
			}
		}
	}

	return true;
}

void PgSiteControl::GetPatchServerList( CEL::CSession_Base *pkSMCSession, ContServerID &rkOutID )const
{
	std::wstring const kSMC_IP = pkSMCSession->Addr().IP();

	CONT_SERVER_STATE_EX2::const_iterator server_itr = m_kTotalServerState.begin();
	for ( ; server_itr != m_kTotalServerState.end() ; ++server_itr )
	{
		if(		kSMC_IP == server_itr->second.kTbl.strMachinIP
			&&	server_itr->second.kTbl.nServerType != CEL::ST_SUB_MACHINE_CONTROL
			&&	server_itr->second.kTbl.nServerType != CEL::ST_MACHINE_CONTROL)
		{
			rkOutID.push_back(server_itr->first);
		}
	}
}

void PgSiteControl::WriteToPacket_MonTool( BM::Stream &rkPacket )const
{
	PU::TWriteTable_MM( rkPacket, m_kTotalServerState );
	m_kRealmMgr.WriteToPacket( rkPacket, 0, ERealm_SendType_All );
}

void PgSiteControl::ServerControl( CEL::SESSION_KEY const &kCmdOwner, ContServerID const &kCommandVec, EMMC_CMD_TYPE const eCmdType )
{
	ContServerID::const_iterator command_itr = kCommandVec.begin();
	for ( ; command_itr!=kCommandVec.end() ; ++command_itr )
	{
		CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.find( *command_itr );
		if ( server_itr != m_kTotalServerState.end() )
		{
			CONT_SERVER_STATE_EX2::mapped_type &kElement = server_itr->second;

			switch( eCmdType )
			{
			case MCT_SERVER_OFF:
				{
					if ( m_pkSession )
					{
						BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_SERVER_COMMAND, MCC_Shutdown_Service);
						command_itr->WriteToPacket( kPacket );
						m_pkSession->VSend( kPacket );
					}
				}break;
			case MCT_SERVER_ON:
				{
					if ( true == kElement.bReadyToService )
					{
						break;
					}
				} // no break
			case MCT_SERVER_SHUTDOWN:
				{
					if ( kElement.pkSMCSession )
					{
						BM::Stream kPacket(PT_MMC_SMC_NFY_CMD, eCmdType);
						kPacket.Push(kCmdOwner);//오더내리는사람 KEY
						kPacket.Push(*command_itr);
						kElement.pkSMCSession->VSend( kPacket );
					}
				}break;
			default:
				{
				}break;
			}
		}
	}
}

bool PgSiteControl::ServerControlRet( SERVER_IDENTITY const kServerIdentity, EMMC_CMD_TYPE const eCmdType )
{
	CONT_SERVER_STATE_EX2::iterator server_itr = m_kTotalServerState.find( kServerIdentity );
	if ( server_itr != m_kTotalServerState.end() )
	{
		CONT_SERVER_STATE_EX2::mapped_type &kElement = server_itr->second;
		switch( eCmdType )
		{
		case MCT_SERVER_ON:
			{
			}break;
		case MCT_SERVER_OFF:
		case MCT_SERVER_SHUTDOWN:
			{

			}break;
		default:
			{

			}break;
		}
		return true;
	}
	return false;
}

bool PgSiteControl::Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND( CEL::CSession_Base *pkSession, E_IMM_MCC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket, CONT_SERVER_ID& rkContServerID )
{
	if ( m_pkSession == pkSession )
	{
		SERVER_IDENTITY kSI;
		kSI.ReadFromPacket(*pkPacket);

		CONT_SERVER_STATE_EX2::iterator state_itr = m_kTotalServerState.find(kSI);
		if( state_itr != m_kTotalServerState.end())
		{
			CONT_SERVER_STATE_EX2::mapped_type &kElement = state_itr->second;
			switch(eCmdType)
			{
			case IMCT_STATE_NFY:
				{
					bool bIsConnect = false;
					bool bIsExecption = false;
					int iMaxUsers = 0;
					int iConnectionUsers = 0;
					DWORD dwProcessID = 0;
				
					pkPacket->Pop(bIsConnect);
					pkPacket->Pop(bIsExecption);
					pkPacket->Pop(iConnectionUsers);
					pkPacket->Pop(iMaxUsers);
					pkPacket->Pop(dwProcessID);

					if ( !m_bRefresh )
					{
						if ( kElement.bReadyToService != bIsConnect )
						{
							rkContServerID.push_back(kElement.kTbl);
							m_bRefresh = true;
						}

						if ( kElement.bIsException != bIsExecption )
						{
							m_bRefresh = true;
						}
					}

					kElement.bReadyToService = bIsConnect;
					kElement.bIsException = bIsExecption;
					kElement.uiMaxUsers = static_cast<unsigned int>(iMaxUsers);
					kElement.uiConnectionUsers = static_cast<unsigned int>(iConnectionUsers);
					kElement.dwLastUpdateTime = BM::GetTime32();

					if ( dwProcessID && kElement.pkSMCSession )
					{
						BM::Stream kPacket( PT_MCTRL_MMC_SMC_NFY_PROCESSID );
						kSI.WriteToPacket( kPacket );
						kPacket.Push( dwProcessID );
						kElement.pkSMCSession->VSend( kPacket );
					}
				}break;
			default:
				{

				}break;
			}

			return true;
		}
		
	}
	return false;
}

void PgSiteControl::RefreshState( BM::Stream * const pkPacket )
{
	CONT_SERVER_PROCESS kRunServers;
	PU::TLoadTable_MM(*pkPacket, kRunServers);

	CONT_SERVER_PROCESS::const_iterator proc_itr = kRunServers.begin();
	for( ; proc_itr != kRunServers.end() ; ++proc_itr )
	{
		CONT_SERVER_STATE_EX2::iterator state_itr = m_kTotalServerState.find(proc_itr->first);
		if( state_itr != m_kTotalServerState.end())
		{
			if ( state_itr->second.kProcessInfo != proc_itr->second )
			{
				CONT_SERVER_STATE_EX2::iterator state_itr = m_kTotalServerState.find(proc_itr->first);
				if( state_itr != m_kTotalServerState.end())
				{
					if ( state_itr->second.kProcessInfo != proc_itr->second )
					{
						m_bRefresh = true;
					}
					state_itr->second.kProcessInfo = proc_itr->second;//프로세스 정보만 갱신.
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("Incorrect Process Info"));
				}
				m_bRefresh = true;
			}
		}
	}
}

bool PgSiteControl::CheckHeartBeat( DWORD const dwCurTime, DWORD const dwMaxTime, CONT_SERVER_ID &rkContServerId )
{
	if ( m_pkSession )
	{
		CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
		for ( ; itr != m_kTotalServerState.end() ; ++itr )
		{
			CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;

			if(		kElement.kTbl.nServerType != CEL::ST_MACHINE_CONTROL			
				&&	kElement.kTbl.nServerType != CEL::ST_SUB_MACHINE_CONTROL)
			{
				EMCtrlCommand const kCmdType = ( kElement.kProcessInfo.IsCorrect() ? MCC_Heart_Beat : MCC_GetProcessID );

				BM::Stream kPacket( PT_MCTRL_MMC_A_NFY_SERVER_COMMAND, kCmdType );
				itr->first.WriteToPacket( kPacket );
				m_pkSession->VSend( kPacket );

				if ( true == kElement.bReadyToService )
				{
					DWORD const dwTime = kElement.dwLastUpdateTime + dwMaxTime;
					if ( dwTime <= dwCurTime )
					{
						rkContServerId.push_back(itr->first);
						kElement.SetDisconnected();;
						m_bRefresh = true;
					}
				}
			}
		}
	}

	if ( true == m_bRefresh )
	{
		m_bRefresh = false;
		return true;
	}

	return false;
}

void PgSiteControl::BroadCast_SMC( BM::Stream const &rkPacket )
{
	CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;
		if ( kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL )
		{
			if ( kElement.pkSMCSession )
			{
				kElement.pkSMCSession->VSend( rkPacket );
			}
		}
	}
}

void PgSiteControl::Disconnect_SMC()
{
	CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;
		if ( kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL )
		{
			if ( kElement.pkSMCSession )
			{
				kElement.pkSMCSession->VTerminate();
				kElement.pkSMCSession = NULL;
				kElement.bReadyToService = false;
				m_bRefresh = true;
			}
		}
	}
}

void PgSiteControl::Clear_SMC()
{
	CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;
		kElement.pkSMCSession = NULL;
	}
}

bool PgSiteControl::WriteToPacket_AdminGateway( BM::Stream &rkPacket, const int iRealmNo )
{
	if( !GetTotalServerState( iRealmNo ) ) // 해당 렐름의 정보만 셋팅
	{
		return false;
	}

	BM::vstring vstrInfo;
	vstrInfo = ADMIN_GATEWAY_ANS_PACKET_TYPE;
	vstrInfo += L"\n";
	
	{// 렐름정보
		PgRealm	Realm;
		if( S_OK != m_kRealmMgr.GetRealm( iRealmNo, Realm ) )
		{
			return false;
		}
		
		BM::vstring vstrTmp( L"CODE=0" );
		vstrTmp += L",";
		vstrTmp += L"WORLDID=";
		vstrTmp += iRealmNo;
		vstrTmp += L",";
		vstrTmp += L"WORLDNAME=";
		vstrTmp += Realm.Name();
		vstrTmp += L",";
		vstrInfo += vstrTmp;
	}

	// 서버 정보
	if( !AddTotalServerInfo(vstrInfo) )
	{
		return false;
	}

	// 채널 정보
	if( !AddChannelServerInfo(vstrInfo) )
	{
		return false;
	}

	// 물리서버 정보
	if( !AddMachineServerInfo(vstrInfo) )
	{
		return false;
	}

	rkPacket.Push( iRealmNo );
	rkPacket.Push(vstrInfo);
	return true;
}

bool PgSiteControl::GetTotalServerState( const int iRealmNo )
{
	if ( m_pkSession )
	{
		m_kContServerState.clear();
		m_kContChannelState.clear();

		CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
		for ( ; itr != m_kTotalServerState.end() ; ++itr )
		{
			CONT_SERVER_STATE_EX2::key_type const &kKey = itr->first;
			CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;
			
			if( iRealmNo != kKey.nRealm )
			{
				switch( kElement.kTbl.nServerType )
				{
				case CEL::ST_IMMIGRATION:
				case CEL::ST_LOGIN:
				case CEL::ST_LOG:
					{//렐름에 종속이 아니다.
					}break;
				default:
					{
						continue;
					}break;
				}
			}

			SChannelIdentity kChannelSI(kKey);
			CONT_CHANNELSTATE::iterator channel_itr = m_kContChannelState.find(kChannelSI);
			if( m_kContChannelState.end() != channel_itr )
			{
				if( CEL::ST_SWITCH == kElement.kTbl.nServerType )
				{
					channel_itr->second.uiConnectionUsers += kElement.uiConnectionUsers;
				}
			}
			else
			{
				if( CEL::ST_SWITCH != kElement.kTbl.nServerType )
				{
					kElement.uiConnectionUsers = 0;
				}
				else
				{
					m_kContChannelState.insert( std::make_pair( kChannelSI, kElement ) );
				}
			}

			switch( kElement.kTbl.nServerType )
			{
			case CEL::ST_SWITCH:
				{
					CONT_SERVERSTATE::iterator server_itr = m_kContServerState.find(ST_PLAY_USER_COUNT);
					if( m_kContServerState.end() != server_itr )
					{
						server_itr->second += kElement.uiConnectionUsers;
					}
					else
					{
						m_kContServerState.insert( std::make_pair(ST_PLAY_USER_COUNT, kElement.uiConnectionUsers) );
					}
				}break;
			case CEL::ST_LOGIN:
				{
					CONT_SERVERSTATE::iterator server_itr = m_kContServerState.find(ST_LOGIN_USER_COUNT);
					if( m_kContServerState.end() != server_itr )
					{
						server_itr->second += kElement.uiConnectionUsers;
					}
					else
					{
						m_kContServerState.insert( std::make_pair(ST_LOGIN_USER_COUNT, kElement.uiConnectionUsers) );
					}
				}break;
			case CEL::ST_CONTENTS:
				{
					CONT_SERVERSTATE::iterator server_itr = m_kContServerState.find(ST_CONN_USER_COUNT);
					if( m_kContServerState.end() != server_itr )
					{
						server_itr->second += kElement.uiConnectionUsers;
					}
					else
					{
						m_kContServerState.insert( std::make_pair(ST_CONN_USER_COUNT, kElement.uiConnectionUsers) );
					}
				}//no break!
			case CEL::ST_MAP:
			case CEL::ST_IMMIGRATION:
			case CEL::ST_LOG:
				{
					CONT_SERVERSTATE::iterator server_itr = m_kContServerState.find(kElement.kTbl.nServerType);
					if( m_kContServerState.end() != server_itr )
					{
						if( false == kElement.bReadyToService )
						{
							server_itr->second = kElement.bReadyToService;
						}
					}
					else
					{
						m_kContServerState.insert( std::make_pair(kElement.kTbl.nServerType, kElement.bReadyToService) );
					}
				}break;
			}
		}

		return !(m_kContServerState.empty());
	}

	return false;
}

bool PgSiteControl::AddTotalServerInfo(BM::vstring &rkOut)
{
	BM::vstring vstrTmp, vstrTypeName;
	if( !GetTypeName(ST_TYPE_CODE, vstrTypeName) )
	{
		return false;
	}
	vstrTmp = vstrTypeName;
	vstrTmp += L"=1";
	AddTypeInfo(ST_AUTH_SERVER, vstrTmp);
	AddTypeInfo(ST_MAIN_SERVER, vstrTmp);
	AddTypeInfo(ST_GAME_SERVER, vstrTmp);
	vstrTmp += L",CACH=-";
	AddTypeInfo(ST_LOG_SERVER, vstrTmp);
	vstrTmp += L",MSGS=-";
	vstrTmp += L",ITEM=-";
	vstrTmp += L",NPCS=-";
	AddTypeInfo(ST_CONN_USER_COUNT, vstrTmp);
	AddTypeInfo(ST_PLAY_USER_COUNT, vstrTmp);
	AddTypeInfo(ST_LOGIN_USER_COUNT, vstrTmp);
	rkOut += vstrTmp;
	return true;
}

bool PgSiteControl::AddChannelServerInfo(BM::vstring &rkOut)
{
	BM::vstring vstrTmp;
	CONT_CHANNELSTATE::iterator channel_itr = m_kContChannelState.begin();
	while( m_kContChannelState.end() != channel_itr )
	{
		if( !AddChannelInfo(channel_itr->first, channel_itr->second, vstrTmp))
		{
			return false;
		}
		++channel_itr;
	}

	rkOut += vstrTmp;
	return true;
}

bool PgSiteControl::AddMachineServerInfo(BM::vstring &rkOut)
{
	BM::vstring vstrTmp, vstrTypeName;
	if( !GetTypeName(ST_TYPE_CODE, vstrTypeName) )
	{
		return false;
	}
	
	BM::vstring vstrMachineName, vstrMachineState;
	if( !GetTypeName(ST_MACHINE_NAME, vstrMachineName) )
	{
		return false;
	}
	if( !GetTypeName(ST_MACHINE_SERVER, vstrMachineState) )
	{
		return false;
	}

	CONT_SERVER_STATE_EX2::iterator itr = m_kTotalServerState.begin();
	for ( ; itr != m_kTotalServerState.end() ; ++itr )
	{
		CONT_SERVER_STATE_EX2::mapped_type &kElement = itr->second;
		if ( kElement.kTbl.nServerType == CEL::ST_SUB_MACHINE_CONTROL )
		{
			vstrTmp += L"\n";
			vstrTmp += vstrTypeName;
			vstrTmp += L"=3";

			vstrTmp += L",";
			vstrTmp += vstrMachineName;
			vstrTmp += L"=";
			vstrTmp += kElement.kTbl.strName.c_str();
			
			vstrTmp += L",";
			vstrTmp += vstrMachineState;
			vstrTmp += L"=";
			vstrTmp += static_cast<short>(kElement.bReadyToService);
			
		}
	}
	rkOut += vstrTmp;
	return true;
}

bool PgSiteControl::GetTypeName(EServerStateType const eType, BM::vstring &rkOut)
{
	//CONT_TYPENAME::const_iterator type_itr = m_kContTypeName.find(eType);
	//if( m_kContTypeName.end() != type_itr )
	//{
	//	rkOut = type_itr->second;
	//	return true;
	//}

	switch( eType )
	{
	case ST_TYPE_CODE:				{ rkOut = L"CODE"; }break;
	case ST_AUTH_SERVER:			{ rkOut = L"AUTH"; }break;
	case ST_MAIN_SERVER:			{ rkOut = L"MAIN"; }break;
	case ST_GAME_SERVER:			{ rkOut = L"GAME"; }break;
	case ST_LOG_SERVER:				{ rkOut = L"LOGS"; }break;
	case ST_CONN_USER_COUNT:		{ rkOut = L"CONN"; }break;
	case ST_PLAY_USER_COUNT:		{ rkOut = L"PLAY"; }break;
	case ST_LOGIN_USER_COUNT:		{ rkOut = L"LOGI"; }break;
	case ST_CHANNEL_CODE:			{ rkOut = L"CHCD"; }break;
	case ST_CHANNEL_NAME:			{ rkOut = L"CHNA"; }break;
	case ST_CHANNEL_STATE:			{ rkOut = L"CHST"; }break;
	case ST_CHANNEL_USER_COUNT:		{ rkOut = L"CHP1"; }break;
	case ST_MACHINE_NAME:			{ rkOut = L"SVNA"; }break;
	case ST_MACHINE_SERVER:			{ rkOut = L"SVST"; }break;
	case ST_WORLD_ID:				{ rkOut = L"WORLDID"; }break;
	case ST_WORLD_NAME:				{ rkOut = L"WORLDNAME"; }break;
	default:{ return false; }break;
	}

	return true;
}

bool PgSiteControl::GetTypeValue(EServerStateType const eType, int &riValue)
{
	CONT_SERVERSTATE::const_iterator type_itr = m_kContServerState.find(eType);
	if( m_kContServerState.end() != type_itr )
	{
		riValue = type_itr->second;
		return true;
	}

	return false;
}

bool PgSiteControl::AddTypeInfo(EServerStateType const eType, BM::vstring &rkOut)
{
	BM::vstring vstrTypeName;
	int iTypeValue = 0;
	if( !GetTypeName(eType, vstrTypeName) 
		|| !GetTypeValue(eType, iTypeValue) )
	{
		return false;
	}

	rkOut += L",";
	rkOut += vstrTypeName;
	rkOut += L"=";
	rkOut += iTypeValue;
	return true;
}
bool PgSiteControl::AddChannelInfo(SChannelIdentity const &rkChannelSI, SEx2SingleServerStatus const &rkServerInfo, BM::vstring &rkOut)
{
	BM::vstring vstrTypeName;

	//Add Type Code
	if( !GetTypeName(ST_TYPE_CODE, vstrTypeName) )
	{
		return false;
	}
	rkOut += L"\n";
	rkOut += vstrTypeName;
	rkOut += L"=2";

	//Add Channe Code
	if( !GetTypeName(ST_CHANNEL_CODE, vstrTypeName) )
	{
		return false;
	}
	rkOut += L",";
	rkOut += vstrTypeName;
	rkOut += L"=";
	rkOut += L"R";
	rkOut += rkChannelSI.nRealm;
	rkOut += L"C";
	rkOut += rkChannelSI.nChannel;

	//Add Channel Name
	if( !GetTypeName(ST_CHANNEL_NAME, vstrTypeName) )
	{
		return false;
	}
	rkOut += L",";
	rkOut += vstrTypeName;
	rkOut += L"=";
	rkOut += rkServerInfo.kTbl.strName.c_str();
	
	//Add Channel State
	if( !GetTypeName(ST_CHANNEL_STATE, vstrTypeName) )
	{
		return false;
	}
	rkOut += L",";
	rkOut += vstrTypeName;
	rkOut += L"=";
	rkOut += static_cast<short>(rkServerInfo.bReadyToService);
	

	//Add Channel UserCount
	if( !GetTypeName(ST_CHANNEL_USER_COUNT, vstrTypeName) )
	{
		return false;
	}
	rkOut += L",";
	rkOut += vstrTypeName;
	rkOut += L"=";
	rkOut += rkServerInfo.uiConnectionUsers;

	rkOut += L",CHP2=-";
	rkOut += L",CHRC=-";
	rkOut += L",CHGS=-";
	return true;
}