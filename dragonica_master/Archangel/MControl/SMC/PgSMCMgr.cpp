#include "StdAfx.h"
#include "PgRecvFromMMC.h"
#include "PgSMCMgr.h"
#include "PgSendWrapper.h"  
#include "PgKeyEvent.h"
#include "PgLogCopyer.h"
#include <telegram/telegram.h>

const TCHAR* szProcInfoFile = _T("ProcInfo.dat");

//>>
void LogLastError()
{
#ifdef _DEBUG
	DWORD const dwErrorCode = GetLastError();
	TCHAR szMsg[2048] = {0, };
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, 0, szMsg, 2048, NULL);
	std::cout << MB(szMsg) << std::endl;
#endif
}
void InfoLogLastError(const TCHAR* szLog, const TCHAR* szFunc, DWORD const dwLine)
{
	DWORD const dwErrorCode = GetLastError();
	TCHAR szMsg[2048] = {0, };
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, 0, szMsg, 2048, NULL);
	INFO_LOG( BM::LOG_LV1, __FL__ << _T(" ") << szLog << _T(" - ErrorCode: ") << dwErrorCode << _T(" (") << szMsg << _T(")") );
}
//<<

//1 seconds
PgSMCMgr::PgSMCMgr(void)
:	m_bConnectMMC(false)
{
	SMC_Sync(false);
	AutoPatch(false);
	DataSync(false);
}

PgSMCMgr::~PgSMCMgr(void)
{
	SaveAllStatus();
}

bool PgSMCMgr::ReadFromConfig( LPCWSTR lpFileName )
{
	if ( !PathFileExists(lpFileName) )
	{//! 파일 존재 유무 검사
		return false;
	}

	wchar_t chValue[MAX_PATH] = {0,};
	m_kIsDeleteLog = static_cast<bool>( ::GetPrivateProfileIntW( L"LOG", L"IS_DELETE", 1, chValue) );
	m_kDeleteLogInterval = ::GetPrivateProfileIntW( L"LOG", L"DELETE_INTERVAL", 120, chValue);
	
	::GetPrivateProfileStringW( L"LOG", L"PATH_LOG", L"LogFiles/", chValue, MAX_PATH, lpFileName );
	m_kPathLog = chValue;

	::GetPrivateProfileStringW( L"LOG", L"PATH_BAK_LOG", L"LogFiles_Bak/", chValue, MAX_PATH, lpFileName );
	m_kPathLogBak = chValue;

	::GetPrivateProfileStringW( L"LOG", L"PATH_DUMP", L".\\Patch\\", chValue, MAX_PATH, lpFileName);
	m_kPathDump = chValue;


	::GetPrivateProfileStringW( L"TELEGRAM", L"TOKEN", L"", chValue, MAX_PATH, lpFileName);
	m_kTelegramToken = MB(chValue);
	m_kTelegramOpsChat = ::GetPrivateProfileIntW(L"TELEGRAM", L"OPS_CHAT", 0, lpFileName);

	return true;
}

void PgSMCMgr::OnDisconnectMMC()
{
	BM::CAutoMutex kLock( m_kMutex );
	m_kNeedDownload.clear();//클리어.
	m_bConnectMMC = false;
}

void PgSMCMgr::ConnectToMMC()
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( !m_bConnectMMC )
	{
		std::wcout << L"Connect To MMC" << std::endl;
		CEL::ADDR_INFO const& rkAddr = g_kProcessCfg.MMCServerAddr();
		m_bConnectMMC = g_kCoreCenter.Connect(rkAddr, m_kMMCSessionKey);
	}
}

void PgSMCMgr::Recv_PT_A_S_ANS_GREETING( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	m_kSyncPath.clear();
	m_kServerHash.clear();
	m_kRunServerInfo.clear();
	
	PU::TLoadTable_MM(*pkPacket, m_kSyncPath);
	PU::TLoadTable_MM(*pkPacket, m_kServerHash);
	pkPacket->Pop( m_kSMC_Sync );
	pkPacket->Pop( m_kDataSync );
	pkPacket->Pop( m_kForceDataPath );


	CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.begin();
	for( ; server_itor != m_kServerHash.end() ; ++server_itor )
	{
		CONT_SERVER_STATE::mapped_type kServerState;
		kServerState.kTbl = (*server_itor).second;

		if( IsMyMachine(kServerState.kTbl) )
		{	
			SERVER_IDENTITY const kServerIdentity = static_cast<SERVER_IDENTITY>(kServerState.kTbl);
			auto kRet = m_kRunServerInfo.insert(std::make_pair(kServerIdentity, PROCESS_INFORMATION_EX()));
			if ( g_kProcessCfg.ServerIdentity() == kServerIdentity )
			{
				kRet.first->second.dwProcessId = GetCurrentProcessId();
				kRet.first->second.hProcess = NULL;
				if ( UpdateStatus( kRet.first->second ) != EPS_OK )
				{
					kRet.first->second = PROCESS_INFORMATION_EX();
				}
			}
		}
	}

	LoadAllStatus();

	//먼저 SMC부터 Sync할게 있는지 확인한다.
	Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO( pkSession, m_kSMC_Sync, m_kDataSync);
}

bool PgSMCMgr::CreateArgument(SERVER_IDENTITY const& rkServerID, std::wstring& rkOut)
{
//	Contents	/REALM=1 /SERVER_NO=9				/IMM_ADDR=127.0.0.1:1001
//	Subcenter /REALM=1 /CHANNEL=1 /SERVER_NO=1 /CONTENTS_ADDR=127.0.0.1:1500
//	IMM /SERVER_NO=1000
//	map			/REALM=1 /CHANNEL=1 /SERVER_NO=11 /CENTER_ADDR=127.0.0.1:1000 
//	switch		/REALM=1 /CHANNEL=1 /SERVER_NO=6 /CENTER_ADDR=127.0.0.1:1000 
//	login /SERVER_NO=5 /IMM_ADDR=127.0.0.1:4000
//	/REALM=1 /SERVER_NO=1 /IMM_ADDR=127.0.0.1:4000
	typedef enum eArgFlag
	{
		AF_NONE = 0,
		AF_REALM		= 0x0001  << 1,
		AF_CHANNEL		= 0x0001  << 2,
		AF_SERVER_NO	= 0x0001  << 3,
		AF_IMM_ADDR		= 0x0001  << 4,
		AF_CENTER_ADDR	= 0x0001  << 5,
		AF_CONTENTS_ADDR = 0x0001 << 6, 
	}E_ARG_FLAG;
	
	DWORD dwArgFlag = 0;

	switch(rkServerID.nServerType)
	{
	case CEL::ST_IMMIGRATION:
	case CEL::ST_CONSENT:
		{
			dwArgFlag = AF_SERVER_NO;
		}break;
	case CEL::ST_LOGIN:
	case CEL::ST_GMSERVER:
	case CEL::ST_AP:
		{	
			dwArgFlag = AF_SERVER_NO|AF_IMM_ADDR;
		}break;
	case CEL::ST_CONTENTS:{dwArgFlag = AF_REALM|AF_SERVER_NO|AF_IMM_ADDR;}break;
	case CEL::ST_CENTER:		{dwArgFlag = AF_REALM|AF_CHANNEL|AF_SERVER_NO|AF_CONTENTS_ADDR;}break;
	case CEL::ST_SWITCH:		{dwArgFlag = AF_REALM|AF_CHANNEL|AF_SERVER_NO|AF_CENTER_ADDR;}break;
	case CEL::ST_MAP:			{dwArgFlag = AF_REALM|AF_CHANNEL|AF_SERVER_NO|AF_CENTER_ADDR;}break;
	case CEL::ST_LOG://로그는 로그로~/ 
		{
			if(rkServerID.nRealm == 0)
			{
				dwArgFlag = AF_SERVER_NO|AF_IMM_ADDR;
			}
			else 
			{
				dwArgFlag = AF_REALM|AF_SERVER_NO|AF_IMM_ADDR;
			}
		}break;
	}
	
	std::wstring kArgument;

	if(dwArgFlag & AF_REALM)
	{
		BM::vstring kTemp(rkServerID.nRealm);
		kArgument += L" /REALM=";
		kArgument += (std::wstring)kTemp;
	}
	if(dwArgFlag & AF_CHANNEL)
	{
		BM::vstring kTemp(rkServerID.nChannel);
		kArgument += L" /CHANNEL=";
		kArgument += (std::wstring)kTemp;
	}
	if(dwArgFlag & AF_SERVER_NO)
	{
		BM::vstring kTemp(rkServerID.nServerNo);
		kArgument += L" /SERVER_NO=";
		kArgument += (std::wstring)kTemp;
	}

	if(dwArgFlag & AF_IMM_ADDR)
	{
		BM::CAutoMutex kLock ( m_kMutex );

		CONT_SERVER_HASH::const_iterator server_itr = m_kServerHash.begin();
		for( ; server_itr != m_kServerHash.end() ; ++server_itr )
		{
			if ( CEL::ST_IMMIGRATION == server_itr->second.nServerType )
			{
				kArgument += L" /IMM_ADDR=";
				kArgument += server_itr->second.addrServerNat.ToString();
				break;
			}
		}
		
		if ( server_itr == m_kServerHash.end() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" No ImmServer Info") );
		}
	}

	if(dwArgFlag & AF_CENTER_ADDR)
	{
		BM::CAutoMutex kLock ( m_kMutex );

		CONT_SERVER_HASH::const_iterator server_itr = m_kServerHash.begin();
		for( ; server_itr != m_kServerHash.end() ; ++server_itr )
		{
			CONT_SERVER_HASH::mapped_type const &kElement = server_itr->second;
			if(		kElement.nChannel == rkServerID.nChannel
				&&	kElement.nRealm == rkServerID.nRealm
				&&	kElement.nServerType == CEL::ST_CENTER )
			{
				kArgument += L" /CENTER_ADDR=";
				kArgument += kElement.addrServerNat.ToString();
				break;
			}
		}

		if ( server_itr == m_kServerHash.end() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" No CenterServer Info Realm[") << rkServerID.nRealm << _T("], Channel[") << rkServerID.nChannel << _T("]") );
		}
	}

	if(dwArgFlag & AF_CONTENTS_ADDR)
	{
		BM::CAutoMutex kLock ( m_kMutex );
		CONT_SERVER_HASH::const_iterator server_itr = m_kServerHash.begin();
		for( ; server_itr != m_kServerHash.end() ; ++server_itr )
		{
			CONT_SERVER_HASH::mapped_type const &kElement = server_itr->second;
			if(		kElement.nRealm == rkServerID.nRealm
				&&	kElement.nServerType == CEL::ST_CONTENTS )
			{
				kArgument += L" /CONTENTS_ADDR=";
				kArgument += server_itr->second.addrServerNat.ToString();
				break;
			}
		}

		if ( server_itr == m_kServerHash.end() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" No Contents Info") );
		}
	}

	//RUN_MODE 확인
	if(g_kProcessCfg.RunMode() == CProcessConfig::E_RunMode_ReadIni)
	{
		kArgument += L" /RUN_MODE=INI";
	}
	else if(g_kProcessCfg.RunMode() == CProcessConfig::E_RunMode_ReadInb)
	{
		kArgument += L" /RUN_MODE=INB";
	}
	
	if( kArgument.empty() )
	{
		return false;
	}
	
	rkOut = kArgument;
	return true;
}

bool PgSMCMgr::TerminateGameServer(SERVER_IDENTITY const& rkServerID)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.find(rkServerID);
	if( m_kRunServerInfo.end() != iter )
	{
		return _TerminateGameServer((*iter).second);
	}
	return false;
}

bool PgSMCMgr::_TerminateGameServer(CONT_SERVER_PROCESS::mapped_type &kServer)
{
	if(kServer.IsCorrect())
	{
		DWORD dwExitCode = 0;
		bool const bGetExitRet = ::GetExitCodeProcess(kServer.hProcess, &dwExitCode);
		if(	bGetExitRet
		&&	STILL_ACTIVE == dwExitCode )
		{
			bool const TerminateThreadRet = ::TerminateThread(kServer.hThread, 0);
			bool const TerminateProcRet = ::TerminateProcess(kServer.hProcess, 0);
		}
	
		kServer.hProcess = INVALID_HANDLE_VALUE;//여기서 안해주면 CloseHandle 하다 뻑난다
		kServer.hThread = INVALID_HANDLE_VALUE;//여기서 안해주면 CloseHandle 하다 뻑난다
		kServer.dwProcessId = 0; // Clear process id
		kServer.Clear();

		return true;
	}
	return false;
}

bool PgSMCMgr::TerminateGameServerAll()
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_SERVER_PROCESS::iterator itr = m_kRunServerInfo.begin();
	for( ; m_kRunServerInfo.end() != itr ; ++itr )
	{
		if ( g_kProcessCfg.ServerIdentity() != itr->first )
		{
			_TerminateGameServer( itr->second );
		}
	}
	return false;
}


bool PgSMCMgr::IsMyMachine(SERVER_IDENTITY const& kTargetSI)const
{
	CONT_SERVER_HASH::const_iterator itr = m_kServerHash.find( g_kProcessCfg.ServerIdentity() );
	if ( itr != m_kServerHash.end() )
	{
		CONT_SERVER_HASH::const_iterator itr2 = m_kServerHash.find( kTargetSI );
		if ( itr2 != m_kServerHash.end() )
		{
			return itr->second.strMachinIP == itr2->second.strMachinIP;
		}
		else
		{
			return false;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not Found My Server ") );
	return false;
}

bool PgSMCMgr::CreateGameServer(SERVER_IDENTITY const& rkServerID)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool const bFindRet = IsExistProcess(rkServerID);
	if( bFindRet )
	{
		return false;
	}

//	bool const bSyncRet = SyncGameSever((*path_itor).second.kSrcPath, (*path_itor).second.kDestPath);
//	if( !bSyncRet )//싱크 실패면
//	{
//		return false;
//	}

	bool const bRunRet = CreateGameServer_Sub(rkServerID);
	if( !bRunRet )
	{
		return false;
	}
	return true;
}

//
bool PgSMCMgr::CreateGameServer_Sub(SERVER_IDENTITY const& rkServerID)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.find(rkServerID);
	if( m_kRunServerInfo.end() == iter )
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Cannot Find ServerInfo [Channel:") << rkServerID.nChannel << _T(", Type:") << rkServerID.nServerType
			<< _T(", ServerNo:") << rkServerID.nServerNo << _T("]") );
		return false;
	}

	if((*iter).second.IsCorrect())
	{//이미 떠있어.
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Game Server Is Already On!") );
		return false;
	}

	PROCESS_INFORMATION &kPI = (*iter).second;

	CON_SYNC_PATH::const_iterator path_itor = m_kSyncPath.find((CEL::E_SESSION_TYPE)rkServerID.nServerType);
	if(path_itor == m_kSyncPath.end())
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Cannot Find SyncPath [Channel:") << rkServerID.nChannel << _T(", Type:") << rkServerID.nServerType
			<< _T(", ServerNo:") << rkServerID.nServerNo << _T("]") );
		return false;
	}
	SGameServerPath kServerPath = (*path_itor).second;
 
	std::wstring kArgument;
	bool const bArgumentRet = CreateArgument(rkServerID, kArgument);//아규먼트 만들기.
	if( !bArgumentRet )
	{
		INFO_LOG( BM::LOG_LV1, _T(" Can't Make argument server [Channel:") << rkServerID.nChannel << _T(", Type:") << rkServerID.nServerType
			<< _T(", ServerNo:") << rkServerID.nServerNo << _T("]") );
		return false;
	}

	std::wstring kPath;

	kPath += _T("./");
	if( false == DataSync() )
	{//싱크 OFF 일때만 사용됨.
		kPath += ForceDataPath();
	}
	kPath += kServerPath.kSrcPath;

	BM::AddFolderMark(kPath);
	
	kPath += kServerPath.kFilename;

	std::wstring kRunPath;
	std::wstring kFileName;
	BM::DivFolderAndFileName(kPath, kRunPath, kFileName);

	std::wstring kExecuteComamnd = L"\"";
	kExecuteComamnd += kPath;
	kExecuteComamnd += L"\" ";
	kExecuteComamnd += kArgument;
	INFO_LOG( BM::LOG_LV6, _T(" Create Process: ") << kFileName.c_str() << _T(" ") << kArgument.c_str() << _T(" ") << kRunPath.c_str() );

	std::wstring const kAppName = kFileName + kArgument;

	STARTUPINFO kSI;
	SECURITY_ATTRIBUTES kProcessSecuAttr;
	SECURITY_ATTRIBUTES kThreadSecuAttr;

	memset(&kSI, 0, sizeof(STARTUPINFO));
	memset(&kProcessSecuAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	memset(&kThreadSecuAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	kSI.cb = sizeof(STARTUPINFO);
	kProcessSecuAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	kThreadSecuAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

	const LPWSTR pExecuteCommand = (LPWSTR)kExecuteComamnd.c_str();
	//bool const bCreateProcRet = ::CreateProcess(NULL, pExecuteCommand, &kProcessSecuAttr, &kThreadSecuAttr, 0, CREATE_NEW_CONSOLE, 0, kRunPath.c_str(), &kSI, &kPI);
	bool const bCreateProcRet = ::CreateProcess(NULL, pExecuteCommand, &kProcessSecuAttr, &kThreadSecuAttr, 0, CREATE_NEW_CONSOLE, 0, kRunPath.c_str(), &kSI, &kPI);
	if( !bCreateProcRet )
	{
		InfoLogLastError(_T("CreateProcess Error "), __FUNCTIONW__, __LINE__);
		return false;
	}

	//
	HANDLE hEvnetHandle = kPI.hProcess;
	MsgWaitForMultipleObjects(1, &hEvnetHandle, true, 1000, QS_ALLEVENTS);

	if( bCreateProcRet )//핸들 체크
	{
		DWORD dwExitCode = 0;
		bool const bRet = GetExitCodeProcess(kPI.hProcess, &dwExitCode);
		if( !bRet )
		{
			return false;
		}

		if( !dwExitCode )//실패
		{
			return false;
		}
	}
	return true;
}

void PgSMCMgr::Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(CEL::CSession_Base* const pkSession, const bool bIsSMC_Sync, const bool bUseDataSync)
{
	if( !bUseDataSync )
	{
		SMC_Sync(false);
		INFO_LOG( BM::LOG_LV1, _T("[ Pass Data Sync ]") );
		BM::Stream kPacket(PT_SMC_MMC_ANS_DATA_SYNC_END);//싱크 끝.
		kPacket.Push(SMC_Sync());
		pkSession->VSend(kPacket);
		return;
	}

	std::wstring kLogMsg;
	if(bIsSMC_Sync)
	{
		kLogMsg = _T("SMC File Sync Process Start...");
	}
	else
	{
		kLogMsg = _T("Data Sync Process Start...");
	}

	INFO_LOG( BM::LOG_LV1, kLogMsg.c_str() );
	BM::Stream kReqSyncPacket(PT_SMC_MMC_REQ_DATA_SYNC_INFO);
	kReqSyncPacket.Push(bIsSMC_Sync);
	pkSession->VSend(kReqSyncPacket);
}

void PgSMCMgr::Recv_PT_MCTRL_MMC_SMC_NFY_PROCESSID( BM::Stream * const pkPacket )
{
	SERVER_IDENTITY kSI;
	pkPacket->Pop( kSI );
	
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SERVER_PROCESS::iterator itr = m_kRunServerInfo.find( kSI );
	if ( itr != m_kRunServerInfo.end() )
	{
		if ( !itr->second.IsCorrect() )
		{
			pkPacket->Pop( itr->second.dwProcessId );
			itr->second.hProcess = NULL;
			if ( UpdateStatus( itr->second ) != EPS_OK )
			{
				CAUTION_LOG( BM::LOG_LV2, __FL__ << L"Error Process ID<" << itr->second.dwProcessId << L"> " << C2L(kSI) );
				itr->second = PROCESS_INFORMATION_EX();
			}
		}
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV2, __FL__ << L"Not Found : " << C2L(kSI) );
	}
}

bool PgSMCMgr::ProcessCmdMMCOrder(CEL::CSession_Base *pkSession, CEL::SESSION_KEY const &kCmdOwner, EMMC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(eCmdType)
	{
	case MCT_REFRESH_CFG:
		{
			CON_SYNC_PATH kContPath;
			PU::TLoadTable_MM(*pkPacket, kContPath);
			SyncPath(kContPath);
		}break;
	case MCT_RE_SYNC:
		{
			TerminateGameServerAll();
			Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(pkSession);
		}break;
	case MCT_SMC_SYNC:
		{
			Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(pkSession, true);
		}break;
	case MCT_SERVER_ON:
		{
			SERVER_IDENTITY kServerIdentity;
			pkPacket->Pop( kServerIdentity );

			bool const bRet = CreateGameServer(kServerIdentity);

			BM::Stream kPacket(PT_SMC_MMC_ANS_CMD, eCmdType);//응답
			kPacket.Push(kCmdOwner);
			kPacket.Push(kServerIdentity);
			kPacket.Push(bRet);
			pkSession->VSend(kPacket);
		}break;
	case MCT_SERVER_SHUTDOWN:
		{
			SERVER_IDENTITY kServerIdentity;
			pkPacket->Pop( kServerIdentity );

			bool const bRet = TerminateGameServer(kServerIdentity);

			BM::Stream kPacket(PT_SMC_MMC_ANS_CMD, eCmdType);//응답
			kPacket.Push(kCmdOwner);
			kPacket.Push(kServerIdentity);
			kPacket.Push(bRet);
			pkSession->VSend(kPacket);
		}break;
	case MCT_REFRESH_LOG:
		{//! MCT -> MMC -> Refresh_Log 패킷 수신
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] REQ REFRESH_FILE"));
			int iKeyValue = 0;
			pkPacket->Pop(iKeyValue);

			BM::Stream kPacket( PT_SMC_MMC_ANS_CMD, eCmdType);
			kPacket.Push(iKeyValue);

			if(FT_DUMP == iKeyValue)
			{
				//! 컨테이너에 덤프 파일 정보를 담는다.
				PgLogCopyer::CONT_DUMP_INFO const & kContDumpInfo = g_kLogCopyer.GetDumpInfo(PathDump());
				PU::TWriteTable_AA( kPacket, kContDumpInfo);
			}
			else
			{
				//! 사용하기 편하도록 하기 위해, 폴더에 이름과 서버정보를 컨테이너에 담음 ( 처음 한번 만 수행함 )
				g_kLogCopyer.SetServerInfo(m_kRunServerInfo);

				//! 폴더에서 파일 검색, 정보 취합하여 컨테이너에 담는다
				PgLogCopyer::CONT_LOG_INFO kContLogInfo;
				g_kLogCopyer.GetLogInfo(kContLogInfo, iKeyValue);// iRealm과 같은 렐름의 정보만 취합함.

				PU::TWriteTable_AA( kPacket, kContLogInfo);
			}
			//! 응답을 날려줌. MMC를 거쳐 MCT로 날아감.
			pkSession->VSend( kPacket );
		}break;
	case MCT_GET_LOG_FILE_INFO:
		{//! 로그 파일 목록 요청
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] REQ LOG_FILE_LIST"));
			g_kLogCopyer.SetServerInfo(m_kRunServerInfo);

			PgLogCopyer::CONT_LOG_FILE_INFO kFileList;
			g_kLogCopyer.GetFileInfo(pkPacket, kFileList);

			BM::Stream kPacket( PT_SMC_MMC_ANS_CMD, MCT_SMC_ANS_LOG_FILE_INFO );
			kPacket.Push(kCmdOwner);
			PU::TWriteTable_AA( kPacket, kFileList);

			g_kCoreCenter.Send( m_kMMCSessionKey, kPacket );
			
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] Send, File List"));
		}break;
	case MCT_REQ_GET_FILE:
		{//! 파일 전송 요청
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] MCT_REQ_GET_FILE"));
			int iKeyValue = NULL;
			pkPacket->Pop(iKeyValue);

			std::wstring kFileName;
			kFileName.clear();
			pkPacket->Pop(kFileName);

			std::vector< char > kFileData;
			g_kLogCopyer.GetFile(kFileName, kFileData, iKeyValue);

			BM::Stream kPacket(PT_SMC_MMC_ANS_CMD, MCT_SMC_ANS_GET_FILE);
			kPacket.Push(kCmdOwner);
			kPacket.Push(kFileName);
			kPacket.Push(kFileData);
			g_kCoreCenter.Send( m_kMMCSessionKey, kPacket );
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] SEND FILE : ") << kFileName);
		}break;
	default:
		{
			
		}break;
	}
	return true;
}

// bool PgSMCMgr::SetStatus(SERVER_IDENTITY const& rkServerID, const PROCESS_INFORMATION& rkPI)
// {
// 	BM::CAutoMutex kLock(m_kMutex);
// 	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.find(rkServerID);
// 	if( m_kRunServerInfo.end() != iter )
// 	{
// 		(*iter).second = rkPI;
// 
// 		SaveAllStatus();//상태 저장
// 		return true;
// 	}
// 	return false;
// }

// bool PgSMCMgr::DelStatus(SERVER_IDENTITY const& rkServerID)
// {
// 	BM::CAutoMutex kLock(m_kMutex);
// 	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.find(rkServerID);
// 	if( m_kRunServerInfo.end() != iter )
// 	{
// 		(*iter).second.Clear();
// 
// 		SaveAllStatus();//상태 저장
// 		return true;
// 	}
// 	return false;
// }

// bool PgSMCMgr::GetStatus(SERVER_IDENTITY const& rkServerID, PROCESS_INFORMATION& rkOut)const
// {
// 	BM::CAutoMutex kLock(m_kMutex);
// 	CONT_SERVER_PROCESS::const_iterator iter = m_kRunServerInfo.find(rkServerID);
// 	if( m_kRunServerInfo.end() != iter )
// 	{
// 		CONT_SERVER_PROCESS::mapped_type const& rkElement = (*iter).second;
// 		rkOut = rkElement;
// 		return true;
// 	}
// 	return false;
// }

bool PgSMCMgr::IsExistProcess(SERVER_IDENTITY const& rkServerID)const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SERVER_PROCESS::const_iterator iter = m_kRunServerInfo.find(rkServerID);

	if(m_kRunServerInfo.end() != iter)
	{
		if(!(*iter).second.IsCorrect())
		{//안떠있다
			return false;
		}
	}

	return true;
}


void PgSMCMgr::SaveAllStatus()
{
	BM::CAutoMutex kLock(m_kMutex);

	HANDLE hFile = CreateFile(szProcInfoFile, FILE_ADD_FILE| GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == hFile )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't open MCtrl [") << szProcInfoFile << _T("] File") );
		return;
	}

	DWORD dwWorkSize = 0;
	BOOL bWriteRet = 0;
	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.begin();

	const size_t iCount = m_kRunServerInfo.size();
	bWriteRet = WriteFile(hFile, &iCount, sizeof(iCount), &dwWorkSize, NULL);
	if( !bWriteRet )
	{
		goto __ERROR_RET;
	}

	while(m_kRunServerInfo.end() != iter)
	{
		const CONT_SERVER_PROCESS::key_type& rkKey = (*iter).first;
		const CONT_SERVER_PROCESS::mapped_type& rkElement = (*iter).second;
		
		bWriteRet = WriteFile(hFile, &rkKey, sizeof(CONT_SERVER_PROCESS::key_type), &dwWorkSize, NULL);
		if( !bWriteRet )
		{
			goto __ERROR_RET;
		}

		bWriteRet = WriteFile(hFile, &rkElement, sizeof(CONT_SERVER_PROCESS::mapped_type), &dwWorkSize, NULL);
		if( !bWriteRet )
		{
			goto __ERROR_RET;
		}
		++iter;
	}

	CloseHandle(hFile);
	return;

__ERROR_RET:
	CloseHandle(hFile);
	DeleteFile(szProcInfoFile);
}

void PgSMCMgr::LoadAllStatus()
{
	HANDLE hFile = CreateFile(szProcInfoFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == hFile )
	{
		//DWORD const dwErrorCode = GetLastError();
		//TCHAR szMsg[1024] = {0, };
		//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, 0, szMsg, 1024, NULL);
		return;
	}

	DWORD dwWorkSize = 0;
	size_t iCount = 0;
	BOOL bWorkRet = 0;

	bWorkRet = ReadFile(hFile, &iCount, sizeof(iCount), &dwWorkSize, NULL);
	if( !bWorkRet )
	{
		return;
	}

	const size_t iKeySize = sizeof(CONT_SERVER_PROCESS::key_type);
	const size_t iElementSize = sizeof(CONT_SERVER_PROCESS::mapped_type);
	for(size_t iCur = 0; iCount > iCur; ++iCur)
	{
		CONT_SERVER_PROCESS::key_type kNewKey;
		CONT_SERVER_PROCESS::mapped_type kNewElement;
		memset(&kNewElement, 0, sizeof(iElementSize));

		if( ReadFile(hFile, &kNewKey, iKeySize, &dwWorkSize, NULL) )
		{
			if ( ReadFile(hFile, &kNewElement, iElementSize, &dwWorkSize, NULL) )
			{
				if ( g_kProcessCfg.ServerIdentity() != kNewKey )
				{
					//강제로 핸들을 재 설정 한다.
					kNewElement.hProcess = NULL;

					CONT_SERVER_PROCESS::iterator itr = m_kRunServerInfo.find( kNewKey );
					if ( itr != m_kRunServerInfo.end() )
					{
						if ( EPS_OK == UpdateStatus( kNewElement ) )
						{
							itr->second = kNewElement;
						}
					}
				}
			}
		}	
	}

	::CloseHandle(hFile);
}


void PgSMCMgr::RefreshProcessState()
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SERVER_PROCESS::iterator iter = m_kRunServerInfo.begin();
	while(m_kRunServerInfo.end() != iter)
	{
		CONT_SERVER_PROCESS::mapped_type& rkElement = (*iter).second;

		EProcessState const processState = UpdateStatus( rkElement);
		if (processState == EPS_DEAD)
			telegram::send_message(m_kTelegramOpsChat, MB(BM::vstring("[$OPS] Process ") << (*iter).first.ToString() << " with pid " << rkElement.dwProcessId << " dead!") );

		if( EPS_OK != processState )//성공
			rkElement.Clear();
		++iter;
	}
}

void PgSMCMgr::NfyProcessStateToMMC()
{
	BM::CAutoMutex kLock(m_kMutex);
	
	BM::Stream kPacket(PT_SMC_MMC_REFRESH_STATE);

	PU::TWriteTable_MM(kPacket, m_kRunServerInfo);

	::SendToMMC( kPacket );
}

void PgSMCMgr::ProcessDownload(CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bSmcPatchEnd = false;
	
	//wait 를 걸어야 되네... 으으으으음;;.

	//다운 받은거 감지는 어떻게 하지??..
	CONT_DOWNLOAD::iterator head_itor = m_kNeedDownload.begin();
	while(head_itor != m_kNeedDownload.end())
	{
		BM::FolderHash &kFolderHash = (*head_itor).second;
		BM::FolderHash::iterator folder_itor = kFolderHash.begin();

		while(folder_itor != kFolderHash.end())
		{
			BM::PgFolderInfoPtr kFolderInfo = (*folder_itor).second;
			BM::FileHash &kFileHash = kFolderInfo->kFileHash;
			
			BM::FileHash::iterator file_itor = kFileHash.begin();

			if(file_itor != kFileHash.end())
			{
				if( FindStr(folder_itor->first, (std::wstring)L"SMC") 
					&& FindStr(file_itor->first, (std::wstring)L".EXE") )
				{
					SMCFileName(file_itor->first);
					SMCPath(folder_itor->first);
				}

				//폴더와 파`일 컨터이너 비면 지워야함.
				std::wstring const strFileName = folder_itor->first + file_itor->first;
				kFileHash.erase(file_itor);
	
				INFO_LOG( BM::LOG_LV1, _T("Req File [") << strFileName.c_str() << _T("]") );
				BM::Stream kPacket( PT_SMC_MMC_REQ_GET_FILE);
				kPacket.Push(strFileName);
//				SendToMMC(kPacket);
				pkSession->VSend(kPacket);
				return;
			}

			if( FindStr(folder_itor->first, (std::wstring)L"SMC")
				&& file_itor == kFileHash.end()
				&& SMC_Sync() )
			{
				bSmcPatchEnd = true;
			}

			kFolderHash.erase(folder_itor++);
		}
		m_kNeedDownload.erase(head_itor++);
	}

	INFO_LOG( BM::LOG_LV1, _T("Sync Process End..") );

	BM::Stream kPacket(PT_SMC_MMC_ANS_DATA_SYNC_END);//싱크 끝.
	kPacket.Push(SMC_Sync());
	pkSession->VSend(kPacket);

	//만약 SMC관련 패치가 끝났으면 Temp를 만들고 종료한다.
	if(bSmcPatchEnd && SMC_Sync() )
	{
		StartSelfPatch();
	}
	else if(!bSmcPatchEnd && SMC_Sync())
	{
		Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(pkSession);
	}
}
//>>
#include <tlhelp32.h>
bool FindProcess(DWORD const dwFindProcessID)
{
	if( !dwFindProcessID )
	{
		return false;
	}

	bool bFindRet = false;

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		goto __RETURN;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hProcessSnap, &pe32) )
	{
		goto __RETURN;
	}

	do
	{
		bFindRet = dwFindProcessID == pe32.th32ProcessID;
		if( bFindRet )
		{
			break;
		}
	}while(Process32Next(hProcessSnap, &pe32));

__RETURN:
	CloseHandle(hProcessSnap);
	return bFindRet;
}

EProcessState PgSMCMgr::UpdateStatus( PROCESS_INFORMATION_EX& rkPI )
{
	bool const bFindProcess = FindProcess(rkPI.dwProcessId);
	if( !bFindProcess )
	{
		return rkPI.dwProcessId != 0 ? EPS_DEAD : EPS_NOT_EXIST;
	}

	if( !rkPI.hProcess )
	{
		rkPI.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, rkPI.dwProcessId);
	}

	return rkPI.IsCorrect() ? EPS_OK : EPS_NOT_EXIST;
}

void PgSMCMgr::StartSelfPatch()
{
	if(!AutoPatch())
	{
		std::vector<char> kFileData;
		std::wstring kSrcFileName = SMCPath() + SMCFileName();
		BM::FileToMem(kSrcFileName, kFileData);

		std::wstring kDstFolder;
		std::wstring kDstFileName;
		//카피할 곳으로 변경해준다.
		BM::DivFolderAndFileName(SMC_RunPath(), kDstFolder, kDstFileName);

		if(kDstFolder.size() == 0)
			kDstFileName = L"./SMC_Temp.exe";
		else 
			kDstFileName = L"SMC_Temp.exe";
		kDstFolder += kDstFileName;
		if(BM::MemToFile(kDstFolder, kFileData))
		{
			SMCSelfStart(kDstFolder);
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Sync Failed [") << kDstFolder.c_str() << _T("]") );//싱크 못헀으면 로그찍고 SMC 종료
		} 
	}
	else
	{
		std::wstring kDstFolder;
		std::wstring kDstFileName;
		//카피할 곳으로 변경해준다.
		BM::DivFolderAndFileName(SMC_RunPath(), kDstFolder, kDstFileName);
		//복사할 파일 목록을 뽑는다.
/*
		
		std::wstring kTemp = kDstFolder;//L"./";
		kTemp += SMCPath();
		BM::AddFolderMark(kTemp);
		SMCPath(kTemp);
*/
		BM::FolderHash kSMCFolders;
		BM::PgDataPackManager::GetFileList(SMCPath(), kSMCFolders);
		BM::FolderHash::iterator folder_itor = kSMCFolders.begin();
		while(folder_itor != kSMCFolders.end())
		{
			BM::PgFolderInfoPtr kFolderInfo = (*folder_itor).second;
			BM::FileHash &kFileHash = kFolderInfo->kFileHash;
			
			BM::FileHash::iterator file_itor = kFileHash.begin();

			while(file_itor != kFileHash.end())
			{
				if (0 == _tcsicmp(_T("SMC_TEMP.EXE"), file_itor->first.c_str()))
				{//! SMC_TEMP.EXE 는 현재 실행되고있음.( 이미 패치도 끝났음 ) 그러므로 패스한다.
					CAUTION_LOG( BM::LOG_LV0, _T("Sync PASS [") << file_itor->first << _T("]") );
					++file_itor;
					continue;
				}

				//원본 데이터를 메모리에
				std::wstring const strFileName = folder_itor->first + file_itor->first;
				std::vector<char> kSrcData;
				BM::FileToMem(strFileName, kSrcData);
				CAUTION_LOG( BM::LOG_LV0, _T("File To Memory [") << strFileName.c_str() << _T("]") );

				std::wstring const kFolder = kDstFolder + file_itor->first;// + _T("1");
				
				//SMC 폴더 안에 파일 생성
				if(!BM::MemToFile(kFolder, kSrcData))
				{
					CAUTION_LOG( BM::LOG_LV0, _T("Sync Failed [") << kDstFolder.c_str() << _T("]") );//싱크 못헀으면 로그찍고 SMC 종료
				}
				CAUTION_LOG( BM::LOG_LV0, _T("Memory To File [") << kFolder.c_str() << _T("]") );
				++file_itor;
			}
			++folder_itor;
		}
		SMC_RunPath(kDstFolder + _T("SMC_Release.exe"));
		CAUTION_LOG( BM::LOG_LV0, _T("Run Path [") << SMC_RunPath().c_str() << _T("]") );
		SMCSelfStart(SMC_RunPath());
	}       

	OnTerminateServer(E_Terminate_By_MControl);
//	SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
}

bool PgSMCMgr::SMCSelfStart(std::wstring const &kPath)
{
	std::wstring kRunPath;
	std::wstring kFileName;
	BM::DivFolderAndFileName(kPath, kRunPath, kFileName);

	// 실행 해도 된다

	std::wstring kExecuteComamnd = kPath;
	kExecuteComamnd += L" ";

	if(!AutoPatch())
	{
		kExecuteComamnd += SMC_Argv() + L"SELF_PATCH" + L" " + SMCPath() +  L" " + SMCFileName() + L" ";
	}
	else
	{
		kExecuteComamnd += SMC_Argv();
	}
	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Create Process: ") << kFileName.c_str() << _T(" ") << L" /RUN_MODE=AUTO_PATCH" << _T(" ") << kRunPath.c_str() );

	STARTUPINFO kSI;
	SECURITY_ATTRIBUTES kProcessSecuAttr;
	SECURITY_ATTRIBUTES kThreadSecuAttr;
	PROCESS_INFORMATION_EX kEx_PI;
	PROCESS_INFORMATION &kPI = kEx_PI;
	
	memset(&kSI, 0, sizeof(STARTUPINFO));
	memset(&kProcessSecuAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	memset(&kThreadSecuAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	kSI.cb = sizeof(STARTUPINFO);
	kProcessSecuAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	kThreadSecuAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

	const LPWSTR pExecuteCommand = (LPWSTR)kExecuteComamnd.c_str();
	bool const bCreateProcRet = ::CreateProcess(NULL, pExecuteCommand, &kProcessSecuAttr, &kThreadSecuAttr, 0, CREATE_NEW_CONSOLE, 0, kRunPath.c_str(), &kSI, &kPI);
	if( !bCreateProcRet )
	{
		InfoLogLastError(_T("Temp SMC Run Process Error "), __FUNCTIONW__, __LINE__);
		return false;
	}
	return true;
}

bool PgSMCMgr::FindStr(std::wstring const &kSrcWord, std::wstring const &kFindWord)
{
	if( kSrcWord.size()
		|| kFindWord.size() )
	{
		std::wstring kText = kSrcWord;
		int iStart = 0, iEnd = 0;
		std::basic_string<wchar_t> kTemp;
		for(int i = 0; i < (int)kSrcWord.size(); i++)
		{
			kTemp = kText.substr(i, kFindWord.size());
			iEnd = i;
			if( kTemp == kFindWord )
			{
				return true;
			}
		}
	}

	return false;
}

bool PgSMCMgr::CheckRunParam(int &argc, _TCHAR *argv[])
{
	if(argv == NULL)
	{
		return false;
	}

	//실행 경로 저장
	g_kSMCMgr.SMC_RunPath(argv[0]);


	//SelfPatch를 위해 어규먼트를 저장해야한다.
	int i = 1;
	std::wstring kArg;
	while(i < argc)
	{
		std::wstring kTemp = argv[i];
		if( kTemp == L"SELF_PATCH")
		{
			AutoPatch(true);
			//파일 경로 셋팅(무조건 /SELF_PATH 다음에 원본 파일들이 있는 폴더 위치, 실행 파일명이 있어야 한다.)
//			std::wstring kRunPath = argv[0];
			if( (i + 2) == (argc - 1) )
			{
				g_kSMCMgr.SMCPath(argv[i+1]);
				g_kSMCMgr.SMCFileName(argv[i+2]);

				BM::CPackInfo kPackInfo;
				kPackInfo.Init();
				if(!BM::PgDataPackManager::Init(kPackInfo))
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" BM::PgDataPackManager Init Failed") );
				}

				break;
			} 
			else 
			{
				return false;
			}
		}
		else 
		{
			kArg += argv[i];
			kArg += L" ";
		}
		++i;
	}
	argc = i;
	g_kSMCMgr.SMC_Argv(kArg);

	return true;
}

bool PgSMCMgr::CheckProcessShutDown(const std::wstring &kProcessName)
{
	bool bRet = false;
	HANDLE hProcessSnap = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hProcessSnap != NULL)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);

		if(Process32First(hProcessSnap, &pe))
		{
			do
			{
				std::wstring kRunFileName = pe.szExeFile;
				if( kRunFileName == kProcessName)
				{
					bRet = true;
					break;
				}

			} while( Process32Next(hProcessSnap, &pe));
		}
	}

	CloseHandle(hProcessSnap);
	return bRet;
}
