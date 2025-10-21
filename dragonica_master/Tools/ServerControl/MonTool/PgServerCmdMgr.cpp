#include "StdAfx.h"
#include "PgServerCmdMgr.h"
#include "PgNetwork.h"
#include "GeneralDlg.h"
#include "PgTimer.h"
#include "PgLogCopyMgr.h"
#include "PgMCTTask.h"
#include "ServerOnDlg.h"

PgServerCmdMgr::PgServerCmdMgr(void)
: m_iCmdIndex(0)
, m_iCmdMaxIndex(0)
, m_iSiteNo(1)
{
}

PgServerCmdMgr::~PgServerCmdMgr(void)
{
	m_kContReserveServer.clear();
}

void PgServerCmdMgr::ProcessInfoCmd(EMMC_CMD_TYPE eCmdType, CEL::CSession_Base *pkSession, BM::CPacket * const pkPacket)
{
	INFO_LOG( BM::LOG_LV2, L"RECV PT_MMC_TOOL_NFY_INFO [CMD TYPE : " << GetCmdTypeName(eCmdType) << L"]" );

	switch(eCmdType)
	{
	case MCT_SERVER_OFF:
		{
			ContServerID kContServerId;
			PU::TLoadArray_M(*pkPacket, kContServerId);
			ContServerID::const_iterator itor = kContServerId.begin();
			while( kContServerId.end() != itor )
			{
				if( UpdateReserveCommand(eCmdType, (*itor)) )
				{
					RunReserveCommand();
				}
				++itor;
			}
		}break;
	case MCT_SERVER_ON:
	case MCT_SERVER_SHUTDOWN:
		{
			bool bRet = false;
			SERVER_IDENTITY kSI;
			pkPacket->Pop( kSI );
			pkPacket->Pop( bRet );
			if( !bRet )
			{
				BM::vstring kErrorMessage = CmdLog(eCmdType, kSI);
				MessageBox( NULL, kErrorMessage.operator const wchar_t *(), _T("ERROR"), MB_OK );
			}
			else
			{
				if( UpdateReserveCommand(eCmdType, kSI) )
				{
					RunReserveCommand();
				}
			}
		}break;
	case MCT_REFRESH_STATE://상태 갱신
		{
			if( true == g_kGameServerMgr.RefreshServerState(*pkPacket) )
			{
				g_kMainDlg.InvalidateListView(true, DM_SERVER_STATE);
			}
		}break;
	case MCT_REFRESH_LIST://리스트 갱신
		{
			if( true == g_kGameServerMgr.RefreshServerState(*pkPacket) )
			{
				g_kMainDlg.InvalidateTreeView();
			}
		}break;
	case MCT_REFRESH_LOG://로그 정보 갱신
		{
			SLogCopyer::CONT_LOG_INFO kContLogInfo;
			int iPriKey = g_kLogCopyMgr.Locked_Recv_MCT_REFRESH_LOG(pkPacket, kContLogInfo);
			g_kMainDlg.InvalidateListView(true, DM_LOG_INFO);
		}break;
	case MCT_ERR_CMD://에러
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket(*pkPacket);

			BM::vstring kErrorMessage = CmdLog(eCmdType, kSI);
			MessageBox( NULL, kErrorMessage.operator const wchar_t *(), _T("ERROR"), MB_OK );
		}break;
	case MCT_MMC_ANS_LOG_FILE_INFO:
	case MCT_MMC_ANS_GET_FILE:
	case MCT_SMC_ANS_LOG_FILE_INFO:
	case MCT_SMC_ANS_GET_FILE:
		{
			SEventMessage kEventMsg;
			kEventMsg.PriType(static_cast<int>(eCmdType));
			kEventMsg.Push(pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	}
}

std::wstring PgServerCmdMgr::GetCmdTypeName(EMMC_CMD_TYPE eCmdType)
{
	std::wstring wstrName = L"";
	switch(eCmdType)
	{
	case MCT_REFRESH_CFG :{wstrName = L"MCT_REFRESH_CFG";}break;
	case MCT_REFRESH_LIST :{wstrName = L"MCT_REFRESH_LIST";}break;
	case MCT_REFRESH_STATE :{wstrName = L"MCT_REFRESH_STATE";}break;
	case MCT_SERVER_ON :{wstrName = L"MCT_SERVER_ON";}break;
	case MCT_SERVER_OFF :{wstrName = L"MCT_SERVER_OFF";}break;
	case MCT_SERVER_SHUTDOWN :{wstrName = L"MCT_SERVER_SHUTDOWN";}break;
	case MCT_RE_SYNC :{wstrName = L"MCT_RE_SYNC";}break;
	case MCT_CHANGE_MAXUSER :{wstrName = L"MCT_CHANGE_MAXUSER";}break;
	case MCT_SMC_SYNC :{wstrName = L"MCT_SMC_SYNC";}break;
	case MCT_ERR_CMD :{wstrName = L"MCT_ERR_CMD";}break;
	case MCT_GET_LOG_FILE_INFO :{wstrName = L"MCT_GET_LOG_FILE_INFO";}break;
	case MCT_MMC_ANS_LOG_FILE_INFO :{wstrName = L"MCT_MMC_ANS_LOG_FILE_INFO";}break;
	case MCT_SMC_ANS_LOG_FILE_INFO :{wstrName = L"MCT_SMC_ANS_LOG_FILE_INFO";}break;
	case MCT_REQ_GET_FILE :{wstrName = L"MCT_REQ_GET_FILE";}break;
	case MCT_MMC_ANS_GET_FILE :{wstrName = L"MCT_MMC_ANS_GET_FILE";}break;
	case MCT_SMC_ANS_GET_FILE :{wstrName = L"MCT_SMC_ANS_GET_FILE";}break;
	case MCT_REFRESH_LOG :{wstrName = L"MCT_REFRESH_LOG";}break;
	case MCT_REQ_NOTICE :{wstrName = L"MCT_REQ_NOTICE";}break;
	default:
		{
			wstrName = L"UNKNOWN";
		}break;
	}

	return wstrName;
}

BM::vstring PgServerCmdMgr::CmdLog(short nCmdType, SERVER_IDENTITY const & rkSI)
{
	BM::vstring kCmdMessage(L"[CmdType = ");
	kCmdMessage << nCmdType << L"] [ServerNo = " << rkSI.nServerNo << L"] [ServerType = " << rkSI.nServerType << L"] [RealmNo = " << rkSI.nRealm << L"] [ChannelNo = " << rkSI.nChannel << L"]";
	INFO_LOG( BM::LOG_LV2, kCmdMessage );
	return kCmdMessage;
}


bool PgServerCmdMgr::AddReserveServerList(EMMC_CMD_TYPE const eCmdType, short const nServerType, CONT_SERVER_STATE_FOR_VIEW const & rkServerList)
{
	SCmdServerInfo kCmdServerInfo(eCmdType, nServerType);
	if( g_kGameServerMgr.GetServerList(nServerType, rkServerList, kCmdServerInfo.m_kContServer) )
	{
		m_kContReserveServer.insert( std::make_pair( m_iCmdIndex, kCmdServerInfo ) );
		++m_iCmdIndex;
	}

	return true;
}

bool PgServerCmdMgr::AddReserveServerList(EMMC_CMD_TYPE const eCmdType)
{
	m_kContReserveServer.insert( std::make_pair( m_iCmdIndex, SCmdServerInfo(eCmdType, NULL) ) );

	return true;
}

bool PgServerCmdMgr::UpdateReserveCommand(EMMC_CMD_TYPE const eCmdType, SERVER_IDENTITY const & rkSI)
{
	CONT_MCT_COMMAND::iterator ReserveServer_Itor = m_kContReserveServer.find(m_iCmdIndex);
	if( m_kContReserveServer.end() == ReserveServer_Itor )
	{
		return false;
	}

	if( eCmdType != ReserveServer_Itor->second.m_eCmdType )
	{
		INFO_LOG( BM::LOG_LV2, L"Not Find Command, [Type : " << GetCmdTypeName(eCmdType) << L"]" );
		return false;
	}

	ContServerID::iterator Server_Itor = ReserveServer_Itor->second.m_kContServer.begin();
	while( ReserveServer_Itor->second.m_kContServer.end() != Server_Itor )
	{
		if( rkSI == (*Server_Itor) )
		{
			ReserveServer_Itor->second.m_kContServer.erase(Server_Itor);
			break;
		}
		++Server_Itor;
	}

	if( !ReserveServer_Itor->second.m_kContServer.size() )
	{
		m_kContReserveServer.erase(ReserveServer_Itor);
		++m_iCmdIndex;
		INFO_LOG( BM::LOG_LV2, L"Complete Command, [Type :  " << GetCmdTypeName(eCmdType) << L"] Run Next Command, [Index : " << L"]");
		return true;
	}

	return false;
}

bool PgServerCmdMgr::RunReserveCommand()
{
	CONT_MCT_COMMAND::iterator ReserveServer_Itor = m_kContReserveServer.find(m_iCmdIndex);
	if( m_kContReserveServer.end() == ReserveServer_Itor )
	{
		return false;
	}
	short nCmdType = static_cast<short>(ReserveServer_Itor->second.m_eCmdType);

	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD);
	kPacket.Push( nCmdType );

	switch( nCmdType )
	{
	case MCT_SERVER_ON:
	case MCT_SERVER_OFF:
	case MCT_SERVER_SHUTDOWN:
		{
			kPacket.Push( m_iSiteNo );
			PU::TWriteArray_M(kPacket, ReserveServer_Itor->second.m_kContServer);
		}break;
	default :
		{
		}break;
	}
	g_kNetwork.Send(kPacket);

	BM::vstring kMessage(L"Running [Cmd : ");
	kMessage += GetCmdTypeName(ReserveServer_Itor->second.m_eCmdType);
	kMessage += L"] [ServerType : ";
	kMessage += ServerConfigUtil::GetServerTypeName(ReserveServer_Itor->second.m_nServerType);
	kMessage += L"]";
	g_kMainDlg.UpdateProcessText(kMessage);
	g_kMainDlg.StepItProcess();
	return true;
}

bool PgServerCmdMgr::OnCommand(EMMC_CMD_TYPE const eCmdType, ContServerID const &rkContServerId, CONT_SERVER_STRING const & rkContServerString)
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(eCmdType) << L"]");
	ServerOnDlg	ServerOn;
	ServerOn.SetServerList(rkContServerString);
	ServerOn.SetCmdType(eCmdType);
	ServerOn.ShowTerminateBtn(false);
	if( IDOK == ServerOn.DoModal() )
	{
		// 패킷 전송.
		BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, eCmdType);
		kPacket.Push( m_iSiteNo );
		PU::TWriteArray_M(kPacket, rkContServerId);
		g_kNetwork.Send(kPacket);

		//INFO_LOG(BM::LOG_LV2, L"REQ MCT_SERVER_OFF");
		return true;
	}

	return false;
}

bool PgServerCmdMgr::OnCommand( EMMC_CMD_TYPE const eCmdType )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(eCmdType) << L"]");
	switch( eCmdType )
	{
	case MCT_SERVER_ON:
		{
			CmdServerOn();
		}break;
	case MCT_SERVER_OFF:
		{
			CmdServerOff();
		}break;
	case MCT_RE_SYNC:
		{
			CmdServerReSync();
		}break;
	case MCT_REFRESH_STATE:
	case MCT_REFRESH_LIST:
		{
			BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, eCmdType);
			g_kNetwork.Send(kPacket);
		}break;
	case MCT_SERVER_SHUTDOWN:
	case MCT_REFRESH_LOG:
	case MCT_REQ_GET_FILE:
	case MCT_CHANGE_MAXUSER:
	case MCT_GET_LOG_FILE_INFO:
	case MCT_REQ_NOTICE:
	default:
		{
			
		}break;
	}

	//::CmdLog(MCT_CHANGE_MAXUSER, m_kSI);
	return false;
}

bool PgServerCmdMgr::CmdServerOff()
{
	CONT_SERVER_STATE_FOR_VIEW kServerList;
	if( !g_kGameServerMgr.GetServerList( m_iSiteNo, kServerList ) )
	{
		return false;
	}

	if( !m_kContReserveServer.empty() )
	{// 이미 뭔가 진행중이다.
		return false;
	}

	m_iCmdIndex = 0;
	CONT_SERVER_STRING kContServerString;
	kContServerString.push_back(BM::vstring(L"1. SHUTDOWN Login Server"));
	kContServerString.push_back(BM::vstring(L"2. SHUTDOWN Switch Server"));
	kContServerString.push_back(BM::vstring(L"3. SHUTDOWN Consent Server"));
	kContServerString.push_back(BM::vstring(L"4. SHUTDOWN GM Server"));
	kContServerString.push_back(BM::vstring(L"5. OFF Center Server"));
	kContServerString.push_back(BM::vstring(L"6. OFF Map Server"));
	kContServerString.push_back(BM::vstring(L"7. OFF Contents Server"));
	kContServerString.push_back(BM::vstring(L"8. OFF Log Server"));
	kContServerString.push_back(BM::vstring(L"9. OFF Immigration Server"));

	ServerOnDlg	ServerOn;
	ServerOn.SetServerList(kContServerString);
	ServerOn.SetCmdType(MCT_SERVER_OFF);
	ServerOn.ShowTerminateBtn(true);

	m_iCmdIndex = 0;
	if( IDOK == ServerOn.DoModal() )
	{
		EMMC_CMD_TYPE eCmdType = MCT_SERVER_OFF;
		if( ServerOn.IsTerminateMode() )
		{
			eCmdType = MCT_SERVER_SHUTDOWN;
		}
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_LOGIN, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_SWITCH, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_CONSENT, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_GMSERVER, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_CENTER, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_MAP, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_CONTENTS, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_LOG, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_IMMIGRATION, kServerList) ) return false;
	}
	
	m_iCmdIndex = 0;
	if( RunReserveCommand() )
	{
		m_iCmdMaxIndex = m_kContReserveServer.size();
		g_kMainDlg.StartProcess();
		return true;
	}

	m_kContReserveServer.clear();
	return false;
}

bool PgServerCmdMgr::CmdServerOn()
{
	CONT_SERVER_STATE_FOR_VIEW kServerList;
	if( !g_kGameServerMgr.GetServerList( m_iSiteNo, kServerList ) )
	{
		return false;
	}

	if( !m_kContReserveServer.empty() )
	{// 이미 뭔가 진행중이다.
		return false;
	}

	m_iCmdIndex = 0;
	CONT_SERVER_STRING kContServerString;
	kContServerString.push_back(BM::vstring(L"1. ON Login Server"));
	kContServerString.push_back(BM::vstring(L"2. ON Switch Server"));
	kContServerString.push_back(BM::vstring(L"3. ON Consent Server"));
	kContServerString.push_back(BM::vstring(L"4. ON GM Server"));
	kContServerString.push_back(BM::vstring(L"5. ON Center Server"));
	kContServerString.push_back(BM::vstring(L"6. ON Map Server"));
	kContServerString.push_back(BM::vstring(L"7. ON Contents Server"));
	kContServerString.push_back(BM::vstring(L"8. ON Log Server"));
	kContServerString.push_back(BM::vstring(L"9. ON Immigration Server"));

	ServerOnDlg	ServerOn;
	ServerOn.SetServerList(kContServerString);
	ServerOn.SetCmdType(MCT_SERVER_ON);
	ServerOn.ShowTerminateBtn(false);

	m_iCmdIndex = 0;
	if( IDOK == ServerOn.DoModal() )
	{
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_IMMIGRATION, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_SWITCH, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_CONSENT, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_GMSERVER, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_CONTENTS, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_CENTER, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_MAP, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_LOG, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_ON, CEL::ST_LOGIN, kServerList) ) return false;
	}

	m_iCmdIndex = 0;
	if( RunReserveCommand() )
	{
		m_iCmdMaxIndex = m_kContReserveServer.size();
		g_kMainDlg.StartProcess();
		return true;
	}

	m_kContReserveServer.clear();
	return false;
}

bool PgServerCmdMgr::CmdServerReSync()
{
	CONT_SERVER_STATE_FOR_VIEW kServerList;
	if( !g_kGameServerMgr.GetServerList( m_iSiteNo, kServerList ) )
	{
		return false;
	}

	if( !m_kContReserveServer.empty() )
	{// 이미 뭔가 진행중이다.
		return false;
	}

	m_iCmdIndex = 0;

	CONT_SERVER_STRING kContServerString;
	kContServerString.push_back(BM::vstring(L"1. SHUTDOWN Login Server"));
	kContServerString.push_back(BM::vstring(L"2. SHUTDOWN Switch Server"));
	kContServerString.push_back(BM::vstring(L"3. SHUTDOWN Consent Server"));
	kContServerString.push_back(BM::vstring(L"4. SHUTDOWN GM Server"));
	kContServerString.push_back(BM::vstring(L"5. OFF Center Server"));
	kContServerString.push_back(BM::vstring(L"6. OFF Map Server"));
	kContServerString.push_back(BM::vstring(L"7. OFF Contents Server"));
	kContServerString.push_back(BM::vstring(L"8. OFF Log Server"));
	kContServerString.push_back(BM::vstring(L"9. OFF Immigration Server"));

	ServerOnDlg	ServerOn;
	ServerOn.SetServerList(kContServerString);
	ServerOn.SetCmdType(MCT_RE_SYNC);
	ServerOn.ShowTerminateBtn(true);

	m_iCmdIndex = 0;
	if( IDOK == ServerOn.DoModal() )
	{
		EMMC_CMD_TYPE eCmdType = MCT_SERVER_OFF;
		if( ServerOn.IsTerminateMode() )
		{
			eCmdType = MCT_SERVER_SHUTDOWN;
		}
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_LOGIN, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_SWITCH, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_CONSENT, kServerList) ) return false;
		if( !AddReserveServerList(MCT_SERVER_SHUTDOWN, CEL::ST_GMSERVER, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_CENTER, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_MAP, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_CONTENTS, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_LOG, kServerList) ) return false;
		if( !AddReserveServerList(eCmdType, CEL::ST_IMMIGRATION, kServerList) ) return false;
		if( !AddReserveServerList(MCT_RE_SYNC) ) return false;
	}

	m_iCmdIndex = 0;
	if( RunReserveCommand() )
	{
		m_iCmdMaxIndex = m_kContReserveServer.size();
		g_kMainDlg.StartProcess();
		return true;
	}

	m_kContReserveServer.clear();

	INFO_LOG( BM::LOG_LV2, L"REQ MCT_RE_SYNC" );
	return false;
}

bool PgServerCmdMgr::OnRefreshLog( int const iPriKey, int const iSecKey )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(MCT_REFRESH_LOG) << L"]");
	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, MCT_REFRESH_LOG);
	int iKeyValue = 0;
	switch(iPriKey)
	{
	case TT_SITE_ROOT:
		if( 1 == iSecKey )
		{//! 최상위 루트, 아직 안쓴다 걍 종료
			return false;
		}
	case TT_REALM_ROOT:
		iKeyValue = iSecKey;
		break;
	}

	kPacket.Push(iKeyValue);
	g_kNetwork.Send(kPacket);

	return true;
}

bool PgServerCmdMgr::OnReqGetFile( int const iFileType, std::wstring const & wstrFileName, std::wstring const & wstrOldFileName )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(MCT_REQ_GET_FILE) << L"]");
	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, MCT_REQ_GET_FILE);
	kPacket.Push(iFileType);//! 파일 타입 ( 목적지 파악 )
	kPacket.Push(wstrFileName);
	kPacket.Push(wstrOldFileName);
	g_kNetwork.Send(kPacket);

	return true;
}

bool PgServerCmdMgr::OnChangeMaxUser( SERVER_IDENTITY const &rkSI, int const iMaxUser )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(MCT_CHANGE_MAXUSER) << L"]");
	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, MCT_CHANGE_MAXUSER);
	kPacket.Push( m_iSiteNo );
	rkSI.WriteToPacket(kPacket);
	kPacket.Push(iMaxUser);
	g_kNetwork.Send(kPacket);

	return true;
}

bool PgServerCmdMgr::OnReqLogFileInfo( int const iKeyValue, int const iCheckType1, int const iCheckType2, SLogCopyer::CONT_LOG_INFO const rkContLogInfo )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(MCT_GET_LOG_FILE_INFO) << L"]");
	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, MCT_GET_LOG_FILE_INFO);
	kPacket.Push(iKeyValue);
	kPacket.Push(iCheckType1);
	kPacket.Push(iCheckType2);
	PU::TWriteTable_AA( kPacket, rkContLogInfo);
	g_kNetwork.Send(kPacket);

	return true;
}

bool PgServerCmdMgr::OnReqNotice( short const nRealm, short const nChannel, std::wstring const & wstrContents )
{
	INFO_LOG( BM::LOG_LV2, L"On Command, [Type :  " << GetCmdTypeName(MCT_REQ_NOTICE) << L"]");
	BM::CPacket kPacket(PT_REQ_TOOL_MON_CMD, MCT_REQ_NOTICE );
	kPacket.Push(m_iSiteNo);
	kPacket.Push(nRealm);
	kPacket.Push(nChannel);
	kPacket.Push(wstrContents);
	g_kNetwork.Send(kPacket);

	return true;
}