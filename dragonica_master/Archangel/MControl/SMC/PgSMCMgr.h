#ifndef MACHINE_SMC_PGSMCMGR_H
#define MACHINE_SMC_PGSMCMGR_H

#include "DataPack/Common.h"

const int g_iSyncAddedPort = 369;

enum EProcessState
{
	EPS_OK,
	EPS_NOT_EXIST,
	EPS_DEAD
};

class PgSMCMgr
{
public:
	PgSMCMgr(void);
	virtual ~PgSMCMgr(void);

	bool ReadFromConfig( LPCWSTR lpFileName );

	void OnDisconnectMMC();
	void ConnectToMMC();

	void Recv_PT_A_S_ANS_GREETING( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );

	void ProcessDownload(CEL::CSession_Base *pkSession);

	//패킷 처리
	bool ProcessCmdMMCOrder(CEL::CSession_Base *pkSession, CEL::SESSION_KEY const &kCmdOwner, EMMC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket);

	//떠있는 Process 정보용
	bool IsExistProcess(SERVER_IDENTITY const& rkServerID)const;
	void RefreshProcessState();//떠있는 프로세스 상태 갱신
	void NfyProcessStateToMMC();

	bool IsMyMachine(SERVER_IDENTITY const &kTargetSI)const;
	void StartSelfPatch();
	bool CheckRunParam(int &argc, _TCHAR* argv[]);

	//독립 함수
	bool CreateGameServer(SERVER_IDENTITY const& rkServerID);
	bool OffGameServer(SERVER_IDENTITY const& rkServerID);
	bool TerminateGameServer(SERVER_IDENTITY const& rkServerID);
	bool CheckProcessShutDown(std::wstring const & kProcessName);

	void Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO(CEL::CSession_Base* const pkSession, const bool bIsSMC_Sync = false, const bool bUseDataSync = true);
	void Recv_PT_MCTRL_MMC_SMC_NFY_PROCESSID( BM::Stream * const pkPacket );
protected:
	bool SMCSelfStart(std::wstring const &kPath);
	bool TerminateGameServerAll();
	bool _TerminateGameServer(CONT_SERVER_PROCESS::mapped_type &kServer);

	bool CreateGameServer_Sub(SERVER_IDENTITY const& rkServerID);
	bool GetSubSessionKey(std::wstring const& rkIP, CEL::SESSION_KEY& rkOut) const;

	//서브/중앙 겸용
	bool CreateArgument(SERVER_IDENTITY const& rkServerID, std::wstring& rkOut);

//	bool DelStatus(SERVER_IDENTITY const& rkServerID);
//	bool GetStatus(SERVER_IDENTITY const& rkServerID, PROCESS_INFORMATION& rkOut)const;
//	bool SetStatus(SERVER_IDENTITY const& rkServerID, const PROCESS_INFORMATION& rkPI);

	void SaveAllStatus();
	void LoadAllStatus();
	EProcessState UpdateStatus(PROCESS_INFORMATION_EX& rkPI);

	bool FindStr(std::wstring const &kSrcWord, std::wstring const &kFindWord);

	CLASS_DECLARATION_S_NO_SET(CEL::ADDR_INFO, MMCAddr);

	CEL::SESSION_KEY m_kMMCSessionKey;
	CONT_SERVER_PROCESS m_kRunServerInfo;//이 SMC에 할당된 서버.

	CLASS_DECLARATION_S(CON_SYNC_PATH, SyncPath);
	CLASS_DECLARATION_S(std::wstring, SMC_Argv);
	CLASS_DECLARATION_S(bool, SMC_Sync);
	CLASS_DECLARATION_S(std::wstring, SMCFileName);
	CLASS_DECLARATION_S(std::wstring, SMCPath);
	CLASS_DECLARATION_S(std::wstring, SMC_RunPath);
	CLASS_DECLARATION_S(bool, AutoPatch);
	CLASS_DECLARATION_S(bool, DataSync);
	CLASS_DECLARATION_S(std::wstring, ForceDataPath);

	CLASS_DECLARATION_S(std::wstring, PathLog);
	CLASS_DECLARATION_S(std::wstring, PathLogBak);
	CLASS_DECLARATION_S(std::wstring, PathDump);
	CLASS_DECLARATION_S(bool, IsDeleteLog);
	CLASS_DECLARATION_S(int, DeleteLogInterval);

	CLASS_DECLARATION_S(std::string, TelegramToken);
	CLASS_DECLARATION_S(int, TelegramOpsChat);

	typedef std::list< BM::Stream > CONT_CMD;
	CONT_CMD m_kContCmdQueue;
////////////// 다운로드 기능.
	typedef std::map< std::wstring, BM::FolderHash > CONT_DOWNLOAD;
	CLASS_DECLARATION_S(CONT_DOWNLOAD, NeedDownload);

	void Recv_PT_MMC_SMC_ANS_DATA_SYNC_INFO(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
/////////////////////

protected:
	mutable Loki::Mutex m_kMutex;
	CONT_SERVER_HASH m_kServerHash;
	bool m_bConnectMMC;
};

#define g_kSMCMgr Loki::SingletonHolder< PgSMCMgr >::Instance()




/////////////////////
inline void WriteSuccessFailVec(const ContServerID& kSuccessVec, const ContServerID& kFailVec, BM::Stream& kPacket)
{
	PU::TWriteArray_M(kPacket, kSuccessVec);//성공한 목록
	PU::TWriteArray_M(kPacket, kFailVec);//실패 목록
}
inline void ReadSuccessFailVec(ContServerID& kSuccessVec, ContServerID& kFailVec, BM::Stream& kPacket)
{
	PU::TLoadArray_M(kPacket, kSuccessVec);//성공한 목록
	PU::TLoadArray_M(kPacket, kFailVec);//실패 목록
}

#endif // MACHINE_SMC_PGSMCMGR_H