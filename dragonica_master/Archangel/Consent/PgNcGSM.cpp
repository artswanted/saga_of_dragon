#include "StdAfx.h"
#include "PgNcGSM.h"
#include "BM/Stream.h"
#include "PgNcServer.h"

PgNcGSM::PgNcGSM(void)
	:m_hGSMModule(NULL), m_pFuncBridgeDLLStartup(NULL), m_pFuncLoginWithWebSID2(NULL), m_pFuncLoginWithIDPWD(NULL), m_pFuncLogOut(NULL), m_pFuncGetErrorMsg(NULL), m_pFuncBridgeDLLTerminate(NULL),
	m_pFuncOnRecv(NULL), m_pFuncCheckPCCafeIP(NULL)

{
	InitGsmBridge();
}

PgNcGSM::~PgNcGSM(void)
{
	m_pFuncBridgeDLLTerminate();
	::FreeLibrary(m_hGSMModule);
}

void CALLBACK PgNcGSM::OnAcceptFromGsm(CEL::CSession_Base *pkSession)
{	
	INFO_LOG(BM::LOG_LV6, _T("[OnAcceptFromGsm] Succee Address=") << pkSession->Addr().ToString());	
}

void CALLBACK PgNcGSM::OnDisconnectFromGsm(CEL::CSession_Base *pkSession)
{
	INFO_LOG(BM::LOG_LV6, _T("[OnDisconnectFromGsm] Succee Address=") << pkSession->Addr().ToString());
}

void CALLBACK PgNcGSM::OnRecvFromGsm(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{// GSM으로 부터 OPEN_PORT(10008)를 통해받은 Notification Message
	unsigned short usLength = 0;
	if( pkPacket->Pop(usLength) ) // 처음 2바이트는 패킷의 길이
	{
		g_kGSM.Locked_OnRecvFromGsm( pkSession, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgNcGSM::Locked_OnRecvFromGsm( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kNcSgm);

	// GS <-> GSM 패킷타입은 1바이트
	BYTE byType = static_cast<BYTE>(GSM_NOTIFY_NONE); 
	pkPacket->Pop(byType);

	switch( byType )
	{
	case SQ_KICK_ACCOUNT:
	case SQ_NOTIFY_COIN:
	case SQ_NOTIFY_REMAIN_TIME:
	case SQ_NOTIFY_NEW_TICKET_APPLIED:	
		{
			int iCastType = 0;
			int iResultDataSize = 0;

			BM::Stream::STREAM_DATA kData = pkPacket->Data();
			char* resultData = new char[kData.size()+sizeof(short)];
			memset(resultData, 0, kData.size()+sizeof(short));

			int iResult = m_pFuncOnRecv( (unsigned char*)&kData[0], kData.size(),
				&iCastType, resultData, &iResultDataSize );

			AccountKick* pAccountKick = reinterpret_cast<AccountKick*>(resultData);
			if(!pAccountKick)
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("AccountKick NULL"));				
				return ;
			}

			switch(iCastType)
			{// 패킷을 파싱한 결과값에 대한 타입
			case 0:
				{//실패
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("OnRecv parsing error."));
				}break;
			case 1:
				{// 유저 킥
					g_kNc.SendNotifyUserKick(pAccountKick->gusID, pAccountKick->userID, pAccountKick->reason);
					INFO_LOG(BM::LOG_LV7, __FL__ << _T("recv SQ_KICK_ACCOUNT, UID = [") << pAccountKick->userID << _T("]") );
				}break;
			case 6:
				{// 코인 변동이 있음.

				}break;
			default:
				{
					
				}break;
			}

			delete[] resultData;

		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Packet Type = ") << byType );
		}break;
	}
}

BOOL HexaText2Binary( const  wchar_t *hexatext, size_t hexatextlen, unsigned char *binary, size_t *binarylen )
{
	size_t j= 0;
	int mych = 0;
	for(size_t i = 0; i < hexatextlen;i=i+2) 
	{
		wchar_t oneByte[3] = {0,};

		wcsncpy_s( oneByte, 3, (hexatext+i), 2 );
		swscanf_s( oneByte, L"%02x", &mych);
		binary[j++] = (BYTE)mych;
	}
	*binarylen = j;
	return TRUE;
}

bool PgNcGSM::InitGsmBridge()
{
	m_hGSMModule = ::LoadLibrary(_T("GSMBridge.dll"));
	if( m_hGSMModule )
	{	
		INFO_LOG( BM::LOG_LV6, _T("GSMBridge.dll loaded.") );
	}

	m_pFuncBridgeDLLStartup = (pFuncBridgeDLLStartup)::GetProcAddress(m_hGSMModule, "BridgeDLLStartup");
	m_pFuncLoginWithWebSID2 = (pFuncLoginWithWebSID2)::GetProcAddress(m_hGSMModule, "LoginWithWebSID2");
	m_pFuncLoginWithIDPWD = (pFuncLoginWithIDPWD)::GetProcAddress(m_hGSMModule, "LoginWithIDPWD");
	m_pFuncLogOut = (pFuncLogOut)::GetProcAddress(m_hGSMModule, "LogOut");
	m_pFuncGetErrorMsg = (pFuncGetErrorMsg)::GetProcAddress(m_hGSMModule, "GetErrorMsg");
	m_pFuncBridgeDLLTerminate = (pFuncBridgeDLLTerminate)::GetProcAddress(m_hGSMModule, "BridgeDLLTerminate");
	m_pFuncOnRecv = (pFuncOnRecv)::GetProcAddress(m_hGSMModule, "OnRecv");
	m_pFuncCheckPCCafeIP = (pFuncCheckPCCafeIP)::GetProcAddress(m_hGSMModule, "CheckPCCafeIP");

	if( !m_pFuncBridgeDLLStartup
	||	!m_pFuncLoginWithWebSID2
	||	!m_pFuncLoginWithIDPWD
	||	!m_pFuncLogOut
	||	!m_pFuncGetErrorMsg
	||	!m_pFuncBridgeDLLTerminate
	||	!m_pFuncOnRecv 
	||	!m_pFuncCheckPCCafeIP )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV2, __FL__ << _T("failed GetProcAddress") );
		return false;
	}

	wchar_t szPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szPath);
	bool const bRet = m_pFuncBridgeDLLStartup(szPath, MAX_PATH);
	if(!bRet)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV2, __FL__ << _T(" failed BridgeDLLStartup Path[") << szPath << _T("]") );
		return false;
	}

	return true;
}

void PgNcGSM::TryAuthNcLoginWithWeb(int& iRet, SServerTryLogin &rkLoginInfo, std::wstring const &rkSesskey, int const iUserIP, BYTE *pbyGusId, BYTE& byGMLevel)
{
	BM::CAutoMutex kLock(m_kNcSgm);

	unsigned char pUcKey[16] = {0,};
	size_t iSize = 0;
	if( false == HexaText2Binary(rkSesskey.c_str(), rkSesskey.size(), pUcKey, &iSize))
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("HeaxaText Converte to Binary Failed [Value:") << rkSesskey.c_str() << _T("]") );
		return ;
	}
	
	wchar_t szID[32] = {0,};
	int iUID = 0;
	PortalFlag kPotalFlag = {0,};
	GameFlag kGameFlag = {0,};
	int iBirth = 0;
	iRet = m_pFuncLoginWithWebSID2( pUcKey, iUserIP, pbyGusId, szID, &iUID, &kPotalFlag, &kGameFlag, &iBirth );
	
	// rkLoginInfo Output
	std::wstring kID = szID;
	rkLoginInfo.SetID(kID);
	UPR(kID);
	rkLoginInfo.SetPW(kID);
	rkLoginInfo.iUID = iUID;

	if( 2 == kGameFlag.roleFlag )
	{// 1: PC방 업주 계정, 2: GM 계정, 4: 사내 계정
	 // 일단 GM 계정에 한해서 GMLevel 3으로 만들어 주자.
		byGMLevel = 3;
	}
}

void PgNcGSM::TryAuthNcLoginWithID(int& iRet, SServerTryLogin &rkLoginInfo, std::wstring const &rkID, std::wstring const &rkPW, int const iUserIP, BYTE* pbyGusId, BYTE& byGMLevel)
{
	BM::CAutoMutex kLock(m_kNcSgm);

	wchar_t szID[16];
	::memcpy(szID, rkID.c_str(), sizeof(szID));
	wchar_t szPW[17];
	::memcpy(szPW, rkPW.c_str(), sizeof(szPW));
	
	int iUID = 0;
	PortalFlag kPotalFlag = {0,};
	GameFlag kGameFlag = {0,};
	int iBirth = 0;
	iRet = m_pFuncLoginWithIDPWD( szID, szPW, 0, iUserIP, pbyGusId, &iUID, &kPotalFlag, &kGameFlag, &iBirth );
	
	rkLoginInfo.SetID(rkID);	
	rkLoginInfo.SetPW(rkID); // PW는 ID로 저장
	rkLoginInfo.iUID = iUID;

	if( 2 == kGameFlag.roleFlag )
	{// 1: PC방 업주 계정, 2: GM 계정, 4: 사내 계정
		// 일단 GM 계정에 한해서 GMLevel 3으로 만들어 주자.
		byGMLevel = 3;
	}
}