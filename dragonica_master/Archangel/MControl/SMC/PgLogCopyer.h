#ifndef MACHINE_SMC_PGLOGCOPYER_H
#define MACHINE_SMC_PGLOGCOPYER_H

typedef struct tagLogInfo
{
	tagLogInfo()
	{ 
		Clear(); 
	}
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

typedef struct tagFileType
{
	int m_bType1;//cel log
	int m_bType2;//info log
}SFileType;

typedef enum eFileType
{
	FT_DUMP = -2,
	FT_CATION_HACK = -1,
	FT_ETC = 0
}EFileType;

class PgLogCopyer
{
public:
	typedef std::list< std::wstring > CONT_FOLDER_LIST;
	typedef std::map< SERVER_IDENTITY, SLogInfo > CONT_LOG_INFO;
	typedef std::map< std::wstring, __int64 >	CONT_LOG_FILE_INFO;
	typedef std::map< std::wstring, SERVER_IDENTITY > CONT_SERVER_INFO;
	typedef std::map< short, PgLogCopyer::CONT_LOG_FILE_INFO > CONT_DUMP_INFO;

public:
	PgLogCopyer(void);
	~PgLogCopyer(void);

	void Init( std::wstring const &wstrDir, std::wstring const &wstrBakDir, __int64 i64DeleteDay );

	void DeleteFile( std::wstring const &kFileName ) const;

	bool GetFileList( std::wstring wstrDir, CONT_LOG_FILE_INFO &rkContFile, const wchar_t *lpExtension = L"*.txt") const;

	void GetLogInfo( CONT_LOG_INFO &rkContLog, int const iRealm, int  const iChannel = 0);
	CONT_DUMP_INFO const & GetDumpInfo( std::wstring wstrDir );
	void GetFileInfo( BM::Stream *pkPacket, CONT_LOG_FILE_INFO &rkContFile );
	
	void GetFile( std::wstring const &rkFileName, std::vector< char > &rkFileData, int const iFileType = FT_ETC );

	void SetServerInfo( CONT_SERVER_PROCESS &rkContServerInfo );
	
protected:
	// 백업 드라이브를 넣지 않으면 File을 지운다.
	void FileOrder( std::wstring wstrDir, std::wstring wstrBakDir, FILETIME const &kOrderFileTime );

	bool GetFileListToType( std::wstring wstrDir, SFileType const &rkFileType, CONT_LOG_FILE_INFO &rkContFile ) const;

	// 폴더 리스트를 얻어온다.
	void GetFolderList( std::wstring wstrDirRoot, CONT_FOLDER_LIST &rkContFolder ) const;
	void ReserveFindList( std::wstring wstrFindKey, CONT_FOLDER_LIST &rkContFolder ) const;
	void GetLogInfoToFileList( std::wstring wstrDir, SLogInfo &rkLogInfo) const;
	bool IsFileExist(wchar_t const *lpFileName);

	std::wstring GetServerTypeName(short nServerType);

private:
	mutable Loki::Mutex		m_kMutex;

	std::wstring			m_wstrDir;
	std::wstring			m_wstrBakDir;

	CONT_SERVER_INFO	m_kContServerInfo;
	CONT_DUMP_INFO		m_kContDumpInfo;
};

#define g_kLogCopyer SINGLETON_STATIC(PgLogCopyer)

#endif // MACHINE_SMC_PGLOGCOPYER_H