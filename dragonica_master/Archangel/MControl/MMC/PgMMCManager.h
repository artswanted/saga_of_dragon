#ifndef MACHINE_MMC_PGMMCMANAGER_H
#define MACHINE_MMC_PGMMCMANAGER_H

#include "datapack/pgdatapackmanager.h"
#include "PgSiteControl.h"

typedef enum eDBQueryType_Immgration
{
	DQT_LOAD_SITE_CONFIG			= 1,
	DQT_LOAD_REALM_CONFIG			= 2,

	//	DQT_TRY_AUTH					= 5,

	DQT_LOAD_MAP_CONFIG_STATIC		= 6,
	DQT_LOAD_MAP_CONFIG_MISSION		= 7,
	DQT_LOAD_MAP_CONFIG_PUBLIC		= 8,
	DQT_PATCH_VERSION_EDIT			= 9,

}EDBQueryType_Immgration;

typedef enum
{
	MCL_NONE = 0,
	MCL_GM_LOGIN,
	MCL_GM_LOGOUT,
	MCL_SERVER_ON,
	MCL_SERVER_OFF,
	MCL_SERVER_SHUTDOWN,
	MCL_RE_SYNC_START,
	MCL_RESYNC_END,
	MCL_CHANGE_MAX_USER,

}EMMC_Cmd_Log;

typedef struct TagMMCLog
{
	TagMMCLog()
	{
		eLogType = MCL_NONE;
		iUserMaxCount = 0;
	}

	EMMC_Cmd_Log eLogType;
	std::wstring kID;
	std::wstring kLoginIP;
	SERVER_IDENTITY kSI;
	std::wstring kMachineIP;
	int iUserMaxCount;
}SMMCLog;

typedef struct tagMonToolInfo
:	public	STerminalAuthInfo
{
	tagMonToolInfo()
	{
	}

	tagMonToolInfo( STerminalAuthInfo const &kAuthInfo )
		:	STerminalAuthInfo(kAuthInfo)
	{}

	CEL::SESSION_KEY kSessionKey;
}SMonToolInfo;
typedef std::map< std::wstring, SMonToolInfo > CONT_MON_TOOL_SESSION;

typedef struct tagSyncSessionInfo
{
	typedef enum
	{
		SYNC_WAIT		= 0x00,
		SYNC_WAIT2		= 0x01,
		SYNC_STEP1		= 0x02,
		SYNC_STEP2		= 0x03,

		SYNC_ADD_STEP	= 0x02,
	}E_SYNC_STATE;

	tagSyncSessionInfo( BYTE const _byState = SYNC_WAIT )
		:	dwLastGetFileTime( 0 )
		,	byState( _byState )
	{}

	void operator = ( tagSyncSessionInfo const &rkRhs )
	{
		this->dwLastGetFileTime = rkRhs.dwLastGetFileTime;
		this->byState = rkRhs.byState;
		this->wstrFileName = rkRhs.wstrFileName;
	}

	DWORD					dwLastGetFileTime;
	BYTE						byState;
	std::wstring				wstrFileName;
}SSyncSessionInfo;

class PgSiteControl;

class PgMMCManager
{
	static DWORD const	ms_dwSyncTimeOut = 1000 * 60 * 3;	// 3분
	static DWORD const	ms_dwSyncEndTime = 1000 * 60 * 30;	// 30분

public:
	typedef std::map< int, PgSiteControl* >				CONT_SITE;
	typedef std::map< std::wstring, BM::FolderHash >	CONT_PATCH_FILELIST;

	typedef std::map< CEL::CSession_Base * const, SSyncSessionInfo > CONT_SYNC_SESSION;

public:
	PgMMCManager(void);
	~PgMMCManager(void);

	void Terminate();

	static HRESULT CALLBACK OnDB_EXECUTE( CEL::DB_RESULT &rkResult );
	static HRESULT CALLBACK OnDB_EXECUTE_TRAN( CEL::DB_RESULT_TRAN &rkResult );
	static bool RegistAcceptor(TBL_SERVERLIST const &rkServerInfo);
	static int GetSiteNo( CEL::CSession_Base * const pkSession );

	HRESULT Locked_DQT_LOAD_SITE_CONFIG( CEL::DB_RESULT &rkResult );
	HRESULT Locked_DQT_LOAD_REALM_CONFIG( CEL::DB_RESULT &rkResult );
	HRESULT Locked_DQT_PATCH_VERSION_EDIT( CEL::DB_RESULT &rkResult);

	bool Locked_ReadFromConfig( LPCWSTR lpFileName);
	bool Locked_ReadFromConfig( LPCWSTR lpFileName, bool const bOnlyAuth );
	bool Locked_LoadDB( int const iDBIndex );
	void Locked_LoadPatchFileList( bool const bInit = false );

	void Locked_ConnectImmigration(void);
	bool Locked_SetConnectImmigration( CEL::CSession_Base* pkSession, bool const bConnect );
	bool Locked_SetConnectSMC( CEL::CSession_Base* pkSession, SERVER_IDENTITY_SITE &kRecvSI, bool const bConnect );
	bool Locked_SetSycnEndSMC( CEL::CSession_Base* pkSession );

	void Locked_GetServerList( short const nServerType, ContServerSiteID &rkOut )const;
	void Locked_GetSyncPath( CON_SYNC_PATH &kOut )const;
	bool Locked_GetServerHash( int const iDBIndex, CONT_SERVER_HASH &rkServerHash )const;

	void Locked_Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND( CEL::CSession_Base *pkSession, E_IMM_MCC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket );
	void Locked_Recv_PT_SMC_MMC_REQ_DATA_SYNC_INFO( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void Locked_Recv_PT_SMC_MMC_ANS_CMD( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void Locked_Recv_PT_SMC_MMC_REFRESH_STATE( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );

	BYTE Locked_LoginMonTool( CEL::CSession_Base *pkSession, std::wstring const &kID, std::wstring const &kPW );
	bool Locked_LogOutMonTool( CEL::CSession_Base *pkSession );
	bool Locked_RecvMonToolCmd( CEL::CSession_Base *pkSession, EMMC_CMD_TYPE const eCmdType, BM::Stream* const pkPacket );

	void Locked_CheckHeartBeat(void);
	void Locked_CheckSync(void);
	void Locked_GetFile( CEL::SESSION_KEY const &kSessionKey, BM::Stream* const pkPacket );

	bool Locked_IsLoadPatchFileList(void)const;
	void Locked_SetLoadPatchFileList(bool bLoadPatchFileList) { m_bLoadPatchFileList = bLoadPatchFileList; }
	void Locked_GetPatchVersion(std::wstring &wstrVersion)const;

	void Locked_SetConfigDirectory();

protected:
	void LoadPatchFileList( CON_SYNC_PATH const &kSyncPath, CONT_PATCH_FILELIST &rkOutPatchFileList );

	bool ReadIni_Path( CEL::E_SESSION_TYPE const &eST, std::wstring const &strCategory, std::wstring const &strFileName );
	PgSiteControl* GetSiteControl( int const iDBIndex )const;
	PgSiteControl* GetSiteControl( CEL::CSession_Base* const pkSession )const;

	void BroadCast_MonTool( BM::Stream const &kPacket )const;
	void BroadCast_SMC( BM::Stream const &kPacket )const;
	void Disconnect_SMC()const;

	void Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO( CEL::CSession_Base *pkSession, bool const bIsSMC_SyncStart );
	void SendFile( CEL::SESSION_KEY const &kSessionKey, std::wstring const &wstrAddr, std::wstring const &wstrFileName );
	void SetPatchVersion(int const iMajor,int const iMinor,int const iPatch)
	{
		m_kVersion.Version.i16Major = iMajor;
		m_kVersion.Version.i16Minor = iMinor;
		m_kVersion.Version.i32Tiny = iPatch;
	}

	bool ChangeMaxUser(SERVER_IDENTITY const & rkSI, int const iMaxUser);
	void ChangeMaxUser(CONT_SERVER_ID const & rkContServerId, int const iMaxUser);
	

	bool CheckMemory(DWORD dwSize);
	bool CheckMemory(std::wstring wstrFileName);
protected:

	mutable Loki::Mutex		m_kMutex;

	CONT_SITE				m_kContSite;

	CONT_MON_TOOL_SESSION	m_kUserAuthInfo;
	CON_SYNC_PATH			m_kSyncPath;

	CLASS_DECLARATION_S( BM::GUID, ImmigrationConnector );
	CLASS_DECLARATION(DWORD, m_dwIntervalTime, IntervalTime);
	CLASS_DECLARATION(bool,	m_bUseSMCAutoPatch, UseSMCAutoPatch);
	CLASS_DECLARATION(bool,	m_bUseSMCDataSync, UseSMCDataSync);
	CLASS_DECLARATION_S(std::wstring, ConfigDir);	//Config 디렉토리 값
	CLASS_DECLARATION_S(std::wstring, ForceDataPath);	//SMC 강제 데이터 폴더 지정

	CONT_PATCH_FILELIST		m_kContPatchFileList;
	bool					m_bLoadPatchFileList;

	CONT_SYNC_SESSION	m_kContSyncSession;
	size_t					m_iMaxSyncCount;

	int						m_iKeyValue;
	CEL::CSession_Base*	m_pkCmdReserveSession;//커맨드 예약 세션. 특정 커맨드의 응답을 특정 커맨드 에게만 보내줄 때 쓴다.
	BM::VersionInfo m_kVersion;//! 패치 버전
protected:
};

inline void PgMMCManager::Locked_GetSyncPath( CON_SYNC_PATH &kOut )const
{
	BM::CAutoMutex kLock(m_kMutex);
	kOut = m_kSyncPath;
}

inline bool PgMMCManager::Locked_IsLoadPatchFileList(void)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_bLoadPatchFileList;
}

#define g_kMMCMgr SINGLETON_STATIC(PgMMCManager)

#endif // MACHINE_MMC_PGMMCMANAGER_H