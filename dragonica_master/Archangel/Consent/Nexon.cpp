#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "AilePack/Constant.h"
#include "AilePack/GALA_Constant.h"
#include "Constant.h"
#include "GALAServer.h"

#include "Nexon.h"
#include "Nexon/AuthCheck.h"
#pragma comment(lib, "Nexon/AuthCheck.lib")

bool InitNexonPassport()
{
	if(AUTHCHECK_ERROR_OK != AuthCheck_SetLocale(kLocaleID_JP))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Nexon Auth Init Failed") );
		return false;
	}
	else
	{
		INFO_LOG(BM::LOG_LV1, __FL__ << _T("Nexon Auth Init Success") );
	}

	g_kNxLoginDis.VInit(25);
	g_kNxLoginDis.VActivate();
	return true;
}

ETryLoginResult CheckNexonPassport(SNexonAuthInfo *pkWorkData)
{	
	WCHAR szId[32] = { 0, }; // out

	//ID, 넥슨시리얼, 로그인 IP, 성별, 나이, 패스워드해싱, 넥슨시리얼 해싱, 플래그1, 플래그2, 메타데이타, 국가코드);
	AuthCheckError eErr = AuthCheck_CheckPassport(pkWorkData->kStrPassport.c_str(), pkWorkData->kAuthReq.kAddress.IP().c_str(), true, szId );
	pkWorkData->kAuthReq.strID = szId;
	
	ETryLoginResult eRet = E_TLR_NONE;

	INFO_LOG(BM::LOG_LV1, __FL__ << _T("Nexon Check Value:")<< pkWorkData->kStrPassport << _T(" IP:")<<pkWorkData->kAuthReq.kAddress.IP());
	INFO_LOG(BM::LOG_LV1, __FL__ << _T("Nexon Check Ret = ")<< eErr );

	switch(eErr)
	{
	case AUTHCHECK_ERROR_OK:					{	eRet = E_TLR_SUCCESS;								}break;//	OK
	case AUTHCHECK_ERROR_INVALID_ARGUMENT:		{	eRet = E_TLR_AUTHCHECK_ERROR_INVALID_ARGUMENT;		}break;
	case AUTHCHECK_ERROR_INVALID_PASSPORT:		{	eRet = E_TLR_AUTHCHECK_ERROR_INVALID_PASSPORT;		}break;
	case AUTHCHECK_ERROR_TIMEOUT:				{	eRet = E_TLR_AUTHCHECK_ERROR_TIMEOUT;				}break;
	case AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN:	{	eRet = E_TLR_AUTHCHECK_ERROR_AUTH_SERVER_SHUTDOWN;	}break;
	case AUTHCHECK_ERROR_UNSUPPORTED_LOCALE:	{	eRet = E_TLR_AUTHCHECK_ERROR_UNSUPPORTED_LOCALE;	}break;
	case AUTHCHECK_ERROR_SET_LOCALE_NEEDED:		{	eRet = E_TLR_AUTHCHECK_ERROR_SET_LOCALE_NEEDED;		}break;
	case AUTHCHECK_ERROR_NO_SERVER_INFORMATION:	{	eRet = E_TLR_AUTHCHECK_ERROR_NO_SERVER_INFORMATION;	}break;
	case AUTHCHECK_ERROR_USER_IP_MISMATCHED:	{	eRet = E_TLR_AUTHCHECK_ERROR_USER_IP_MISMATCHED;	}break;
	case AUTHCHECK_ERROR_NO_DECRYPTION_KEY:		{	eRet = E_TLR_AUTHCHECK_ERROR_NO_DECRYPTION_KEY;		}break;
	default:									{	eRet = E_TLR_LOGIN_ERROR;							}break;
	}
//	kStrPassport
	return eRet;
}

void CALLBACK OnConnectFromImmigrationForJapan( CEL::CSession_Base *pkSession )
{
	g_kJapanDaemon.OnConnectFromImmigration(pkSession);
}

void CALLBACK OnDisconnectFromImmigrationForJapan( CEL::CSession_Base *pkSession )
{
	g_kJapanDaemon.OnDisconnectFromImmigration(pkSession);
}

void CALLBACK OnRecvFromImmigrationForJapan(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kJapanDaemon.OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void CALLBACK OnConnectFromNexonCash( CEL::CSession_Base *pkSession )
{
	g_kJapanDaemon.OnConnectFromNexonCash(pkSession);
}

void CALLBACK OnDisconnectFromNexonCash( CEL::CSession_Base *pkSession )
{
	g_kJapanDaemon.OnDisconnectFromNexonCash(pkSession);
}

void CALLBACK OnRecvFromNexonCash(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kJapanDaemon.OnRecvFromNexonCash(pkSession, pkPacket);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PgJapanServerImpl::PgJapanServerImpl()
	: m_dwLastNexonCashRequestKey(0)
{
}

PgJapanServerImpl::~PgJapanServerImpl()
{
}

void PgJapanServerImpl::OnRecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
//	INFO_LOG(BM::LOG_LV1, __FL__ << _T("Recv From IMM = ")<< usType );

	switch( usType )
	{
	case PT_IM_CN_REQ_LOGIN_AUTH_JAPAN:
		{
			RecvPT_IM_CN_REQ_LOGIN_AUTH_JAPAN(pkSession, pkPacket);
		}break;
	case PT_IM_CN_NFY_LOGOUT_LOGIN:
		{
//			size_t iRdPos = pkPacket->RdPos();
//			RecvPT_IM_CN_NFY_LOGOUT_LOGIN(pkSession, pkPacket);
//			pkPacket->RdPos(iRdPos);
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
		// Cash linkage packet
	case PT_N_X_REQ_CASH:
		{
			SERVER_IDENTITY kFromSI;
			kFromSI.ReadFromPacket(*pkPacket);

			SessionInfo* pkSessionInfo = GetSessionInfo(pkSession->Addr());
			LOCAL_MGR::NATION_CODE eNationCode = LOCAL_MGR::NC_NOT_SET;
			if (pkSessionInfo != NULL)
			{
				eNationCode = pkSessionInfo->eNationCode;
			}
			BM::Stream::DEF_STREAM_TYPE wSubType;
			pkPacket->Pop(wSubType);
			SERVER_IDENTITY_SITE kSiteSI(kFromSI, GetSessionKey(pkSession));
			switch (wSubType)
			{
			case PT_A_CN_REQ_QUERY_CASH:
				{
					RecvPT_A_CN_REQ_QUERY_CASH(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM:
			case PT_A_CN_REQ_EXPANDTIEM:
				{
					RecvUseCash(wSubType, eNationCode, kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_SENDGIFT:
				{
					RecvPT_A_CN_REQ_SENDGIFT(eNationCode, kSiteSI, pkPacket);
				}break;
			case PT_A_CN_NFY_TRAN_COMMIT:
				{
					RecvPT_A_CN_NFY_TRAN_COMMIT(kSiteSI, pkPacket, true);
				}break;
			case PT_A_CN_NFY_TRAN_ROLLBACK:
				{
					RecvPT_A_CN_NFY_TRAN_COMMIT(kSiteSI, pkPacket, false);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM_ROLLBACK://PT_A_CN_NFY_TRAN_ROLLBACK에 흡수됨.
				{
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown type [PT_N_X_REQ_CASH]->") << wSubType);
				}break;
			};
		}break;
	default:
		{
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	}
}

void PgJapanServerImpl::RecvPT_IM_CN_REQ_LOGIN_AUTH_JAPAN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SAuthRequest kRecv;
	kRecv.ReadFromPacket(*pkPacket);
	std::wstring kStrAddonMsg;
	pkPacket->Pop(kStrAddonMsg);

	/*if (kRecv.strID.length() <= 0 || kRecv.strPW.length() <= 0)
	{
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_WRONG_ID);
		return;
	}*/

	if (false == IsServiceReady())
	{
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_BLOCKED_SERVER_LOGIN);
		return;
	}

	short sSite = 0;
	bool bSiteFind = PgSessionMgr::GetSessionKey(pkSession, sSite);
	SessionInfo* const pkSessionInfo = PgSessionMgr::GetSessionInfo(pkSession);
	if ( bSiteFind == false || pkSessionInfo == NULL )
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_AUTHCHECK_ERROR_USER_IP_MISMATCHED);
		return;
	}

	//// 이미 접속중인가 검사하기
	//if (PgSiteMemberMgr::IsHaveUserID(kRecv.strID))
	//{
	//	INFO_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
	//	CAUTION_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
	//	SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_ALREADY_TRY);
	//	return;
	//}

	SNexonAuthInfo *pkNxMsg = new SNexonAuthInfo;

	if(pkNxMsg)
	{
		pkNxMsg->iSiteNo = sSite;
		pkNxMsg->kAuthReq = kRecv;
		pkNxMsg->kStrPassport = kStrAddonMsg;

		//INFO_LOG(BM::LOG_LV0, _T("Push AuthMessage ID:")<< kRecv.strID << _T(" IP:")<<kRecv.kAddress.IP());

		g_kNxLoginDis.VPush(pkNxMsg);
	}

//	ETryLoginResult eResult = CheckNexonPassport(kStrAddonMsg, kRecv.kAddress.IP());

//	INFO_LOG(BM::LOG_LV0, __FL__ << _T("Nexon Check Value==>")<< kStrAddonMsg << _T(" IP:")<<kRecv.kAddress.IP());
//	SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), eResult);

/*
	CONT_ID::const_iterator itor_id = m_kUserByID.find(kRecv.strID);
	if (itor_id != m_kUserByID.end())
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		CAUTION_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_ALREADY_TRY);
		return;
	}

	SUserInfo* pkNew = new SUserInfo(sSite, kRecv, pkSessionInfo->eNationCode, GALA::EUSER_REQ_LOGIN);
	if (pkNew == NULL)
	{
		INFO_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		CAUTION_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		SendLoginResult(pkSession, SUserInfo(sSite, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_MEMORY_ERROR);
		delete pkNew;
		return;
	}

	INFO_LOG(BM::LOG_LV8, __FL__ << _T("Add Request=") << pkNew->kAuthInfo.kRequestGuid << _T(", ID=") << pkNew->kAuthInfo.strID);
	auto ibRet = m_kUserByRequestJapan.insert(std::make_pair(pkNew->kAuthInfo.kRequestGuid, pkNew));
	if (false == ibRet.second)
	{
		SendLoginResult(pkSession, *pkNew, E_TLR_ALREADY_TRY);
		delete pkNew;
		return;
	}
	auto ibRet2 = m_kUserByID.insert(std::make_pair(pkNew->kAuthInfo.strID, pkNew));
	if (false == ibRet2.second)
	{
		m_kUserByRequestJapan.erase(ibRet.first);
		SendLoginResult(pkSession, *pkNew, E_TLR_ALREADY_TRY);
		delete pkNew;
		return;
	}

	ETryLoginResult eResult = CheckNexonPassport(kStrAddonMsg, kRecv.kAddress.IP());
	SendLoginResult(NULL, *pkNew, eResult);
//응답

//	m_kRequestList.push_back(pkNew->kAuthInfo.kRequestGuid);
//	CEL::SESSION_KEY kSessionKey(HttpConnector(), BM::GUID::NullData());
//	g_kCoreCenter.Connect(m_kHttpAddress, kSessionKey);
	*/
}

void PgJapanServerImpl::SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult)
{
	if (eResult != E_TLR_SUCCESS)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("UserID=") << kUser.kAuthInfo.strID << _T(", ErrorCode=") << eResult);
	}

	BM::Stream kXPacket(PT_CN_IM_ANS_LOGIN_GALA);
	SAuthRequest kAuth(kUser.kAuthInfo.strID, std::wstring(), kUser.kAuthInfo.kRequestGuid, kUser.kAuthInfo.kAddress);
	kAuth = kUser.kAuthInfo;
	kAuth.strPW.clear(); // PW는 저장하지 않는다
	kAuth.WriteToPacket(kXPacket);
	kXPacket.Push(eResult);
	if (pkSession != NULL)
	{
		pkSession->VSend(kXPacket);
	}
	else
	{
		PgSessionMgr::SendPacket(kUser.sSite, kXPacket);
	}
}

HRESULT NP_Push(BM::Stream &kPacket, BYTE byValue)
{
	kPacket.Push(byValue);
	return S_OK;
}

HRESULT NP_Push(BM::Stream &kPacket, short nValue)
{
	char* pkTemp = (char*)&nValue;
	short kX  = MAKEWORD(pkTemp[1], pkTemp[0]);

	kPacket.Push(kX);

	return S_OK;
}

HRESULT NP_Push(BM::Stream &kPacket, int iValue)
{
	int iTemp = 0;
	char* pkTemp = (char*)&iTemp;

	char* pkinBuf = (char*)&iValue;
	
	pkTemp[3] = pkinBuf[0];
	pkTemp[2] = pkinBuf[1];
	pkTemp[1] = pkinBuf[2];
	pkTemp[0] = pkinBuf[3];

	kPacket.Push(iTemp);
	return S_OK;
}

HRESULT NP_Push(BM::Stream &kPacket, __int64 iValue)
{
	__int64 iTemp = 0;
	char* pkTemp = (char*)&iTemp;

	char* pkinBuf = (char*)&iValue;
	
	pkTemp[7] = pkinBuf[0];
	pkTemp[6] = pkinBuf[1];
	pkTemp[5] = pkinBuf[2];
	pkTemp[4] = pkinBuf[3];
	pkTemp[3] = pkinBuf[4];
	pkTemp[2] = pkinBuf[5];
	pkTemp[1] = pkinBuf[6];
	pkTemp[0] = pkinBuf[7];

	kPacket.Push(iTemp);
	return S_OK;
}

HRESULT NP_Push(BM::Stream &kPacket, std::string &strValue)
{
	short n_len = strValue.length();
	NP_Push(kPacket, n_len);
	if(n_len)
	{
		kPacket.Push(&strValue.at(0), n_len);
	}
	return S_OK;
}

HRESULT NP_Push(BM::Stream &kPacket, std::wstring &strValue)
{
	return NP_Push(kPacket, (std::string)MB(strValue));
}


HRESULT NP_Pop(BM::Stream &kPacket, BYTE &byValue)
{
	kPacket.Pop(byValue);
	return S_OK;
}

HRESULT NP_Pop(BM::Stream &kPacket, short &nValue)
{
	short kTemp = 0; 
	kPacket.Pop(kTemp);

	char* pkTemp = (char*)&kTemp;
	char* pkValue = (char*)&nValue;

	pkValue[0] = pkTemp[1];
	pkValue[1] = pkTemp[0];
	return S_OK;
}

HRESULT NP_Pop(BM::Stream &kPacket, int &iValue)
{
	int iTemp = 0;
	kPacket.Pop(iTemp);

	char* pkTemp = (char*)&iTemp;
	char* pkinBuf = (char*)&iValue;
	
	pkinBuf[0] = pkTemp[3];
	pkinBuf[1] = pkTemp[2];
	pkinBuf[2] = pkTemp[1];
	pkinBuf[3] = pkTemp[0];
	return S_OK;
}

HRESULT NP_Pop(BM::Stream &kPacket, __int64 &iValue)
{
	__int64 iTemp = 0;
	char* pkTemp = (char*)&iTemp;

	kPacket.Pop(iTemp);

	char* pkinBuf = (char*)&iValue;
	
	pkinBuf[7] = pkTemp[0];
	pkinBuf[6] = pkTemp[1];
	pkinBuf[5] = pkTemp[2];
	pkinBuf[4] = pkTemp[3];
	pkinBuf[3] = pkTemp[4];
	pkinBuf[2] = pkTemp[5];
	pkinBuf[1] = pkTemp[6];
	pkinBuf[0] = pkTemp[7];
	return S_OK;
}

HRESULT NP_Pop(BM::Stream &kPacket, std::string &strValue)
{
	short n_len;
	NP_Pop(kPacket, n_len);

	if(n_len)
	{
		char *pkBuf = new char[n_len+1];
		kPacket.PopMemory(pkBuf, n_len);
		pkBuf[n_len] = 0;

		strValue = pkBuf;
	}
	return S_OK;
}

HRESULT NP_Pop(BM::Stream &kPacket, std::wstring &strValue)
{
	std::string kStr;
	HRESULT const hRet = NP_Pop(kPacket, kStr);

	strValue = UNI(kStr);
	return hRet;
}