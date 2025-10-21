#pragma once

typedef struct tagLogInfo
{
	tagLogInfo()
	:	m_dwSize1(0)
	,	m_dwCount1(0)
	,	m_dwDate(0)
	,	m_dwSize2(0)
	,	m_dwCount2(0)
	{ }

	DWORD		m_dwDate;
	DWORD		m_dwSize1;
	DWORD		m_dwCount1;
	DWORD		m_dwSize2;
	DWORD		m_dwCount2;
}SLogInfo;

typedef struct tagLogCopyer
{
	tagLogCopyer(){}
	~tagLogCopyer(){}

	typedef std::map< int, std::wstring >						CONT_FOLDER_LIST;
	typedef std::map< std::wstring, __int64 >					CONT_LOG_FILE_INFO;
	typedef std::map< std::wstring, CONT_LOG_FILE_INFO >	CONT_DOWNLOAD;

	typedef std::map< SERVER_IDENTITY, SLogInfo >			CONT_LOG_INFO;
	typedef std::map< int, CONT_LOG_INFO >					CONT_LIST_LOG_INFO;

	typedef std::map< short, CONT_LOG_FILE_INFO >		CONT_DUMP_INFO;
	typedef std::map< std::wstring, CONT_DUMP_INFO >	 CONT_LIST_DUMP_INFO;

	CONT_DOWNLOAD			m_kContDownLoad;
	CONT_LIST_LOG_INFO		m_kContLogInfo;
	CONT_LIST_DUMP_INFO	m_kContDumpInfo;
}SLogCopyer;

class PgLogCopyMgr
{
public:
	PgLogCopyMgr(void);
	~PgLogCopyMgr(void);

	void Locked_Connect( CEL::SESSION_KEY const &kSessionKey );

	void Locked_Recv_PT_SMC_MCT_LOG_FILE_INFO( BM::CPacket * pkPacket );
	void Locked_Recv_PT_MMC_MCT_LOG_FILE_INFO( BM::CPacket * pkPacket );
	void Locked_Recv_PT_MMC_MCT_ANS_GET_FILE( BM::CPacket * pkPacket );
	int Locked_Recv_MCT_REFRESH_LOG( BM::CPacket * pkPacket, SLogCopyer::CONT_LOG_INFO& rkContLogInfo );

	bool Locked_ReMoveAll();//모든 파일 삭제 

	bool Locked_GetTotalInfo(int const iKeyValue, SLogInfo& rkOutLogInfo);//모든 로그 파일 정보 계산해서 값을 구조체에 넣어줌.
	bool Locked_GetLogInfo(int const iKeyValue, short sServerNo, SLogCopyer::CONT_LOG_INFO& rkContOutLogInfo);
	bool Locked_GetLogInfo(int const iKeyValue, SLogCopyer::CONT_LOG_INFO& rkContOutLogInfo);

	bool Locked_SetDumpInfo(SLogCopyer::CONT_LIST_DUMP_INFO const & rkContDumpInfo);
	bool Locked_GetDumpInfo(std::wstring const & wstrMechineAddr, short const & iServerType, SLogCopyer::CONT_DUMP_INFO& rkContOutDumpInfo);
	bool Locked_GetDumpInfo(SLogCopyer::CONT_LIST_DUMP_INFO& rkContOutDumpInfo) const;

private:
	void ProcessDownLoad( SLogCopyer &kCopyer, std::wstring const &wstrOldfile = std::wstring() );
	void CancelDownload();//다운로드 취소
	__int64 GetDownloadList( BM::CPacket * pkPacket, SLogCopyer::CONT_LOG_FILE_INFO& rkContOutFileInfo );
	bool GetFolderList( std::wstring wstrDirTarget, SLogCopyer::CONT_FOLDER_LIST& rkContOutFolderList )const;

public:
	CLASS_DECLARATION_S( bool, IsStopDownload );
	CLASS_DECLARATION_S( int, FileType );

private:
	mutable Loki::Mutex		m_kMutex;
	std::wstring					m_wstrRootPath;
	SLogCopyer					m_kLogCopyer;
	CEL::SESSION_KEY			m_kMMC_ID;
};

#define g_kLogCopyMgr SINGLETON_STATIC(PgLogCopyMgr)

std::wstring TansformFileSize(double& dOutSize);//파일 크기 단위 변환 ( KB / MB / GB )
