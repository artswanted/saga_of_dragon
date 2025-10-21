#ifndef MACHINE_MMC_PGFILECOPYMGR_H
#define MACHINE_MMC_PGFILECOPYMGR_H

typedef struct tagLogInfo
{
	tagLogInfo(){ Clear(); }
	~tagLogInfo(){}
	void Clear() 
	{ 
		m_dwDate = 0;
		m_dwSize1 = 0;
		m_dwCount1 = 0;
		m_dwSize2 = 0;
		m_dwCount2 = 0; 
	}

	DWORD		m_dwDate;
	DWORD		m_dwSize1;
	DWORD		m_dwCount1;
	DWORD		m_dwSize2;
	DWORD		m_dwCount2;
}SLogInfo;

typedef struct tagFileCopyer
{
	tagFileCopyer(){}
	~tagFileCopyer(){}

	typedef std::map< std::wstring, __int64 >					CONT_LOG_FILE_INFO;
	typedef std::map< std::wstring, CONT_LOG_FILE_INFO >	CONT_DOWNLOAD;

	typedef std::map< SERVER_IDENTITY, SLogInfo >			CONT_LOG_INFO;
	typedef std::map< DWORD, std::wstring > CONT_FOLDER_LIST;

	CONT_FOLDER_LIST m_kContFolderList;
	CONT_DOWNLOAD	m_kContDownLoad;
}SFileCopyer;

typedef struct tagFileType
{
	int m_iType1;//caution log
	int m_iType2;//hack log
}SFileType;

typedef enum eFileType
{
	FT_DUMP = -2,
	FT_CATION_HACK = -1,
}EFileType;

class PgFileCopyMgr
{
public:
	
	typedef std::map< CEL::SESSION_KEY, SFileCopyer >	CONT_SMC_DOWNLOAD;
	
public:
	PgFileCopyMgr(void);
	~PgFileCopyMgr(void);

	void Locked_ReadFromConfig( LPCWSTR lpFileName );
	void Locked_Init( ContServerSiteID const &kServerList );
	void Locked_Tick();

	void Locked_Connect( CEL::SESSION_KEY const &kSessionKey );
	void Locked_Disconnect( CEL::SESSION_KEY const &kSessionKey );

	//!
	void Locked_Recv_PT_SMC_MMC_ANS_LOG_FILE_INFO( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket );
	void Locked_Recv_PT_MMC_SMC_ANS_GET_FILE( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket );

	void Locked_PT_MCT_REQ_GET_FILE_INFO( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket );
	void Locked_Recv_MCT_REQ_GET_FILE( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket );
	void Locked_GetLogInfo( CEL::SESSION_KEY const &kSessionkey, BM::Stream * pkPacket );

private:
	void ProcessDownLoad( CONT_SMC_DOWNLOAD::iterator Download_itor, std::wstring const &wstrOldfile = std::wstring() );
	void GetPath( std::wstring const &wstrPath, std::wstring &rkOutPath )const;

	bool GetLogInfoToFileList( std::wstring &wstrPath, SLogInfo &kLogInfo, int iLogType ) const;

	void GetFileList( std::wstring wstrPath, SFileCopyer::CONT_LOG_FILE_INFO &rkContFile )const;
	void WriteFileTable(std::wstring wstrPath, BM::Stream *pkPacket, SFileCopyer::CONT_LOG_FILE_INFO &rkContFile)const;

	void GetFolderList( std::wstring wstrPath, SFileCopyer::CONT_FOLDER_LIST &rkContFolder )const;
	size_t GetFileListToType(BM::Stream *pkPacket, SFileCopyer::CONT_LOG_FILE_INFO &rkContFile, SFileType &rkFileType, LPCWSTR lpDate);

private:
	mutable Loki::Mutex			m_kMutex;

	CONT_SMC_DOWNLOAD		m_kContCopyer;
	SFileCopyer						m_kFileCopyer;
	CEL::SESSION_KEY				m_kTickCheckID;

	std::wstring						m_wstrPathLog;
	std::wstring						m_wstrPathDump;

	DWORD							m_dwLastUpdateTime;
	DWORD							m_dwHackLogIntervalTime;
	DWORD							m_dwUpdateTickTime;
};

inline void PgFileCopyMgr::GetPath( std::wstring const &wstrPath, std::wstring &rkOutPath )const
{
	BM::CAutoMutex kLock( m_kMutex );
	SYSTEMTIME kSystemTime;
	GetLocalTime( &kSystemTime );

	wchar_t wszPath[MAX_PATH] = {0,};
	::swprintf_s( wszPath, MAX_PATH, L"%s%04u%02u%02u\\", m_wstrPathLog.c_str(), kSystemTime.wYear, kSystemTime.wMonth, kSystemTime.wDay );

	rkOutPath = wszPath;
	rkOutPath += wstrPath;
	BM::ConvFolderMark(rkOutPath);
	BM::ReserveFolder( rkOutPath );
}

#define g_kLogCopyMgr SINGLETON_STATIC(PgFileCopyMgr)

#endif // MACHINE_MMC_PGFILECOPYMGR_H