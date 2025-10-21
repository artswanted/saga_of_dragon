#ifndef CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCGSM_H
#define CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCGSM_H

typedef enum eNC_GSM_NOTIFY
{
	GSM_NOTIFY_NONE					= 0,

	SQ_KICK_ACCOUNT					= 5,
	SQ_NOTIFY_COIN					= 12,
	SQ_NOTIFY_REMAIN_TIME			= 14,
	SQ_NOTIFY_NEW_TICKET_APPLIED	= 15,
}E_NC_GSM_NOTIFY;

struct AccountKick
{
	BYTE gusID[16];
	BYTE reason;
	int userID;

	AccountKick()
		:reason(0), userID(0)
	{
		::memset(gusID, 0, sizeof(gusID));		
	}
};

class PgNcGSM
{
private:
	HMODULE m_hGSMModule;

protected:
	mutable Loki::Mutex m_kNcSgm;	

public:
	PgNcGSM(void);
	~PgNcGSM(void);

	static void CALLBACK OnAcceptFromGsm(CEL::CSession_Base *pkSession);
	static void CALLBACK OnDisconnectFromGsm(CEL::CSession_Base *pkSession);
	static void CALLBACK OnRecvFromGsm(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	// GSMBridge function
	typedef bool	(*pFuncBridgeDLLStartup)(wchar_t *callerDirPath,int callerDirPathLength);
	typedef int		(*pFuncLoginWithWebSID2)(unsigned char* webSID,unsigned long userIP,unsigned char* gusID, wchar_t* account, int* userID, PortalFlag * pPortalFlag, GameFlag *pGameFlag, int *birthDay);
	typedef int		(*pFuncLoginWithIDPWD)(wchar_t *account,wchar_t* pwd ,__int64 macAddr, unsigned long userIP,unsigned char* gusID, int* userID, PortalFlag * pPortalFlag, GameFlag *pGameFlag, int *birthDay);
	typedef int		(*pFuncLogOut)(unsigned char* gusID);
	typedef bool	(*pFuncGetErrorMsg)(int errCode,int errMsgSize, wchar_t* errMessage, int &errMsgLength);
	typedef void	(*pFuncBridgeDLLTerminate)(void);
	typedef int		(*pFuncOnRecv)(unsigned char*  packet, int packetLength, int * castType,void * resultData,int * resultDataSize);
	typedef int		(*pFuncCheckPCCafeIP)( unsigned char *gusID, unsigned long userIP, int *pccafeCode , int *pccafeGrade );

	pFuncBridgeDLLStartup	m_pFuncBridgeDLLStartup;
	pFuncLoginWithWebSID2	m_pFuncLoginWithWebSID2;
	pFuncLoginWithIDPWD		m_pFuncLoginWithIDPWD;
	pFuncLogOut				m_pFuncLogOut;
	pFuncGetErrorMsg		m_pFuncGetErrorMsg;
	pFuncBridgeDLLTerminate	m_pFuncBridgeDLLTerminate;
	pFuncOnRecv				m_pFuncOnRecv;
	pFuncCheckPCCafeIP		m_pFuncCheckPCCafeIP;

	bool InitGsmBridge();
	void TryAuthNcLoginWithWeb(int& iRet, SServerTryLogin &rkLoginInfo, std::wstring const &rkSesskey, int const iUserIP, BYTE* pbyGusId, BYTE& byGMLevel);
	void TryAuthNcLoginWithID(int& iRet, SServerTryLogin &rkLoginInfo, std::wstring const &rkID, std::wstring const &rkPW, int const iUserIP, BYTE* pbyGusId, BYTE& byGMLevel);	
	
	void Locked_OnRecvFromGsm( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
};

#define g_kGSM SINGLETON_STATIC(PgNcGSM)

#endif // CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCGSM_H