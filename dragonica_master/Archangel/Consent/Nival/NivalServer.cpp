#include "stdafx.h"
#include "atlenc.h"
#include "tinyxml/tinyxml.h"
#include "Lohengrin/VariableContainer.h"
#include "AilePack/GALA_Constant.h"
#include "Variant/PgStringUtil.h"
#include "Constant.h"
#include "NivalServer.h"
#include "md5.h"

const std::string PgNivalServer::NIVAL_GAMEPASSWORD = "982092ca79d8e5e1e8a8e4c33c128d1e";

PgNivalServer::PgNivalServer()
{
	LoadXMLFiles();
}

PgNivalServer::~PgNivalServer()
{
	CONT_REQUEST::iterator itor_req = m_kUserByRequest.begin();
	while (itor_req != m_kUserByRequest.end())
	{
		if ((*itor_req).second != NULL)
		{
			delete (*itor_req).second;
		}
		++itor_req;
	}
	m_kUserByRequest.clear();
	m_kUserByID.clear();

	CONT_CASHREQUEST::iterator itor_cash = m_kCashRequest.begin();
	while (itor_cash != m_kCashRequest.end())
	{
		if ((*itor_cash).second != NULL)
		{
			delete (*itor_cash).second;
		}
		++itor_cash;
	}
	m_kCashRequest.clear();
}

void CALLBACK PgNivalServer::OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnConnectFromImmigration(pkSession);
}

void CALLBACK PgNivalServer::OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnDisconnectFromImmigration(pkSession);
}

void CALLBACK PgNivalServer::OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kNival.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgNivalServer::RecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	switch( usType )
	{
	case PT_IM_CN_REQ_LOGIN_AUTH_GALA:
		{
			RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(pkSession, pkPacket);
		}break;
	case PT_IM_CN_NFY_LOGOUT_LOGIN:
		{
			size_t iRdPos = pkPacket->RdPos();
			RecvPT_IM_CN_NFY_LOGOUT_LOGIN(pkSession, pkPacket);

			pkPacket->RdPos(iRdPos);
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	case PT_N_X_REQ_CASH:
		{
			SERVER_IDENTITY kFromSI;
			kFromSI.ReadFromPacket(*pkPacket);

			BM::Stream::DEF_STREAM_TYPE wSubType;
			pkPacket->Pop(wSubType);
			SERVER_IDENTITY_SITE kSiteSI(kFromSI, GetSessionKey(pkSession));
			switch (wSubType)
			{
			case PT_A_CN_REQ_QUERY_CASH:
				{
					RecvPT_A_CN_REQ_QUERY_CASH( kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM:
				{
					RecvPT_A_CN_REQ_BUYCASHITEM(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_EXPANDTIEM:
				{
					RecvPT_A_CN_REQ_EXPANDTIEM(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_SENDGIFT:
				{
					RecvPT_A_CN_REQ_SENDGIFT(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_CASH_ROLLBACK:
				{
					RecvPT_A_CN_REQ_CASH_ROLLBACK(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_OM_BUY_ARTICLE:
				{
					RecvPT_A_CN_REQ_OM_BUY_ARTICLE(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_ADDCASH:
				{
					RecvPT_A_CN_REQ_ADDCASH(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_OM_REG_ARTICLE:
				{
					RecvPT_A_CN_REQ_OM_REG_ARTICLE(kSiteSI, pkPacket);
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

bool PgNivalServer::Locked_OnRegistConnector(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_Nival_Config.ini"));
	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	case CEL::ST_EXTERNAL1:
		{
			TCHAR chValue[100] = {0,};
			int iValue;
			GetPrivateProfileString(_T("AUTH_HTTP"), _T("ADDRESS"), _T(""), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = GetPrivateProfileInt(_T("AUTH_HTTP"), _T("PORT"), 0, wstrFilename.c_str());
			SetAuthServer(chValue, iValue);
			AuthHttpConnector(rkArg.guidObj);
		}break;
	case CEL::ST_EXTERNAL2:
		{
			TCHAR chValue[100] = {0,};
			int iValue;
			GetPrivateProfileString(_T("CASH_HTTP"), _T("ADDRESS"), _T(""), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = GetPrivateProfileInt(_T("CASH_HTTP"), _T("PORT"), 0, wstrFilename.c_str());
			SetCashServer(chValue, iValue);
			CashHttpConnector(rkArg.guidObj);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}
	return true;
}

void CALLBACK PgNivalServer::OnConnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnConnectFromAuthHttp(pkSession);
}

void CALLBACK PgNivalServer::OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnDisconnectFromAuthHttp(pkSession);
}

void CALLBACK PgNivalServer::OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kNival.Locked_OnRecvFromAuthHttp(pkSession, pkPacket);
}

void PgNivalServer::Locked_OnConnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bIsSucc = pkSession->IsAlive();
	if (bIsSucc)
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Auth Http Connected"));
		if (false == SendLoginRequest(pkSession))
		{
			pkSession->VTerminate();
		}
	}
	else
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Failed Http Connected"));
		// try again ....
		if (false == m_kLoginRequestList.empty())
		{
			CEL::SESSION_KEY kSessionKey(AuthHttpConnector(), BM::GUID::NullData());
			g_kCoreCenter.Connect(m_kAuthHttpAddress, kSessionKey);
		}
	}
}

void PgNivalServer::Locked_OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("Auth Http Disconnected"));
	BM::GUID const rkReq = pkSession->CustomKey();

	ParsingLoginAnswer(rkReq);
}

void PgNivalServer::Locked_OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::GUID const rkReq = pkSession->CustomKey();

	CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(rkReq);
	if (m_kUserByRequest.end() == itor_req)
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find request=") << rkReq);
		return;
	}

	SUserInfo* const pkUser = (*itor_req).second;
	size_t iSize = pkPacket->Size();
	char* pkMsg = new char[pkPacket->Size() + 1];
	pkPacket->PopMemory(pkMsg, iSize);
	pkMsg[iSize] = '\0';
	pkUser->strAnswer += pkMsg;
	delete[] pkMsg;
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("RecvMsg =") << pkUser->strAnswer);
}

void PgNivalServer::SetAuthServer(std::wstring const &strAddress, WORD wPort)
{
	char chAddress[100];
	char chPort[20];
	sockaddr_in* pkSockAddress = NULL;
	sprintf_s(chAddress, sizeof(chAddress), "%s", MB(strAddress.c_str()));
	sprintf_s(chPort, sizeof(chAddress), "%hd", wPort);
	addrinfo kHint;
	addrinfo *pkAddressList = NULL;
	memset(&kHint, 0, sizeof(kHint));
	kHint.ai_family = AF_INET;
	kHint.ai_socktype = SOCK_STREAM;
	kHint.ai_protocol = IPPROTO_TCP;
	int iReturn = getaddrinfo(chAddress, chPort, &kHint, &pkAddressList);
	if (iReturn != 0 || pkAddressList == NULL)
	{
		int iError = WSAGetLastError();
		ASSERT_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot get ErrorCode=") << iError << _T(", Address=") << strAddress << _T(", Port=") << wPort);
		return;
	}
	pkSockAddress = (sockaddr_in*)pkAddressList->ai_addr;
	m_kAuthHttpAddress.ip = pkSockAddress->sin_addr;
	m_kAuthHttpAddress.wPort = wPort;
	m_strAuthHttpHostname = strAddress;
}



bool PgNivalServer::SendLoginRequest(CEL::CSession_Base *pkSession)
{
	SUserInfo* pkUser = NULL;
	while (false == m_kLoginRequestList.empty())
	{
		BM::GUID kReqGuid = m_kLoginRequestList.back();
		m_kLoginRequestList.pop_back();

		CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(kReqGuid);
		if (m_kUserByRequest.end() != itor_req)
		{
			pkUser = (*itor_req).second;
			break;
		}
	}

	if (pkUser == NULL)
	{
		return false;
	}


	pkSession->CustomKey(pkUser->kAuthInfo.kRequestGuid);
	// TODO : 구현해야 함..
	std::string strBody;
	ETryLoginResult eResult = MakeLoginRequestBody(*pkUser, strBody);
	if (E_TLR_SUCCESS != eResult)
	{
		SendLoginResult(pkSession, *pkUser, eResult);
		return false;
	}
	char chLength[10];
	sprintf_s(chLength, 10, "%d", strBody.length());

	std::string strMsg;
	strMsg += "POST / HTTP/1.1\r\n";
	strMsg += "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2)\r\n";
	strMsg += "Host: ";
	strMsg += MB(m_strAuthHttpHostname);
	strMsg += "\r\nContent-type: application/x-www-form-urlencoded\r\n";     
	strMsg += "Content-length: ";
	strMsg += std::string(chLength);
	strMsg += "\r\nConnection: close\r\n\r\n";
	strMsg += "xml=";
	strMsg += strBody;

	BM::Stream kPacket;
	kPacket.Push(strMsg.c_str(), strMsg.length());
	pkSession->VSend(kPacket);

	return true;
}

void PgNivalServer::ParsingLoginAnswer(BM::GUID const& rkRequest)
{
	CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(rkRequest);
	if (m_kUserByRequest.end() == itor_req)
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find request=") << rkRequest);
		return;
	}
	std::string strXML;
	TiXmlDocument kXML;
	std::wstring strBanned;

	ETryLoginResult eResult = E_TLR_NONE;
	SUserInfo* const pkUser = (*itor_req).second;

	if (false == GetXMLFromHttpMsg(pkUser->strAnswer, strXML))
	{
		eResult = E_TLR_PARSING_ERROR;
		goto NivalParsingLoginAnswer_Error;
	}

	kXML.Parse(strXML.c_str());
	if ( kXML.Error() )
	{
		eResult = E_TLR_PARSING_ERROR;
		goto NivalParsingLoginAnswer_Error;
	}

	TiXmlNode const* pkRoot = kXML.FirstChild("xml");
	int iErrorNo = 0;
	if (pkRoot == NULL)
	{
		eResult = E_TLR_PARSING_ERROR;
		goto NivalParsingLoginAnswer_Error;
	}
	HRESULT hReturn = GetXMLValue(pkRoot, "errno", iErrorNo);
	if (S_OK == hReturn)
	{
		// Login failed
		if (S_OK != GetXMLValue(pkRoot, "error", pkUser->kAuthInfo.strErrorMsg))
		{
			pkUser->kAuthInfo.strErrorMsg.clear();
		}
		goto NivalParsingLoginAnswer_Error;
	}
	// LOGIN 성공 했을 때
	TiXmlNode const* pkUserNode = pkRoot->FirstChild("user");
	if (pkUserNode == NULL)
	{
		eResult = E_TLR_PARSING_ERROR;
		goto NivalParsingLoginAnswer_Error;
	}
	GetXMLValue(pkUserNode, "banned", strBanned);
	if (strBanned == _T("t"))
	{
		eResult = E_TLR_BLOCKED_USER;
		goto NivalParsingLoginAnswer_Error;
	}
	hReturn = GetXMLValue(pkUserNode, "user_id", pkUser->kAuthInfo.iUID);	// UID 값은 있는데, 사용하는 곳이 없다 --> 에러나도 그냥 패스
	if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	{
		pkUser->kAuthInfo.byGMLevel = 0xFF;	// DEBUG 모드 일때는 무조건 GM 레벨 준다.
	}
	eResult = E_TLR_SUCCESS;

NivalParsingLoginAnswer_Error :
	SendLoginResult(NULL, *pkUser, eResult);
	/* Request information release */
	RemoveUserInfo(rkRequest);
}

void PgNivalServer::RemoveUserInfo(BM::GUID const& rkRequest)
{
	INFO_LOG(BM::LOG_LV8, __FL__ << _T("RequestGuid=") << rkRequest);
	CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(rkRequest);
	SUserInfo* pkUser = NULL;
	if (m_kUserByRequest.end() != itor_req)
	{
		pkUser = (*itor_req).second;
		m_kUserByRequest.erase(itor_req);
	}

	if (pkUser != NULL)
	{
		m_kUserByID.erase(pkUser->kAuthInfo.strID);
		delete pkUser;
	}
}

void PgNivalServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);
	Timer5s();
}

void PgNivalServer::Timer5s()
{
	ACE_UINT64 const ui64Now = BM::GetTime64();

	{
		VEC_GUID kDelVector;
		CONT_REQUEST::const_iterator itor_req = m_kUserByRequest.begin();
		while (itor_req != m_kUserByRequest.end())
		{
			if ((*itor_req).second->i64ExpireTime < ui64Now)
			{
				kDelVector.push_back((*itor_req).first);
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("LoginRequest timeout....ID=") << (*itor_req).second->kAuthInfo.strID );
			}
			++itor_req;
		}

		VEC_GUID::const_iterator itor_del = kDelVector.begin();
		while (itor_del != kDelVector.end())
		{
			RemoveUserInfo(*itor_del);
			++itor_del;
		}
	}

	{
		VEC_GUID kDelVector;
		CONT_CASHREQUEST::iterator itor_cash = m_kCashRequest.begin();
		while (m_kCashRequest.end() != itor_cash)
		{
			if ((*itor_cash).second->i64ExpireTime < ui64Now)
			{
				kDelVector.push_back((*itor_cash).first);
			}
			++itor_cash;
		}
		VEC_GUID::const_iterator itor_del = kDelVector.begin();
		while (itor_del != kDelVector.end())
		{
			RemoveCashRequest(*itor_del);
			++itor_del;
		}
	}

	PgSiteMemberMgr::Timer5s();
}

void PgNivalServer::Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration(pkSession, usType, pkPacket);
}


void PgNivalServer::RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SAuthRequest kRecv;
	kRecv.ReadFromPacket(*pkPacket);
	if (kRecv.strID.length() <= 0 || kRecv.strPW.length() <= 0)
	{
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_WRONG_ID);
		return;
	}

	if (false == IsServiceReady())
	{
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_BLOCKED_SERVER_LOGIN);
		return;
	}

	short sSite;
	bool bSiteFind = PgSessionMgr::GetSessionKey(pkSession, sSite);
	SessionInfo* const pkSessionInfo = PgSessionMgr::GetSessionInfo(pkSession);
	if ( bSiteFind == false || pkSessionInfo == NULL )
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_WRONG_ID);
		return;
	}

	// 이미 접속중인가 검사하기
	if (PgSiteMemberMgr::IsHaveUserID(kRecv.strID))
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		CAUTION_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_ALREADY_TRY);
		return;
	}

	CONT_ID::const_iterator itor_id = m_kUserByID.find(kRecv.strID);
	if (itor_id != m_kUserByID.end())
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		CAUTION_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_ALREADY_TRY);
		return;
	}

	SUserInfo* pkNew = new SUserInfo(sSite, kRecv);
	if (pkNew == NULL)
	{
		INFO_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		CAUTION_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_MEMORY_ERROR);
		delete pkNew;
		return;
	}

	INFO_LOG(BM::LOG_LV8, __FL__ << _T("Add Request=") << pkNew->kAuthInfo.kRequestGuid << _T(", ID=") << pkNew->kAuthInfo.strID);
	auto ibRet = m_kUserByRequest.insert(std::make_pair(pkNew->kAuthInfo.kRequestGuid, pkNew));
	if (false == ibRet.second)
	{
		SendLoginResult(pkSession, *pkNew, E_TLR_ALREADY_TRY);
		delete pkNew;
		return;
	}
	auto ibRet2 = m_kUserByID.insert(std::make_pair(pkNew->kAuthInfo.strID, pkNew));
	if (false == ibRet2.second)
	{
		m_kUserByRequest.erase(ibRet.first);
		SendLoginResult(pkSession, *pkNew, E_TLR_ALREADY_TRY);
		delete pkNew;
		return;
	}

	// Try Connecting~~~~~
	m_kLoginRequestList.push_back(pkNew->kAuthInfo.kRequestGuid);
	CEL::SESSION_KEY kSessionKey(AuthHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kAuthHttpAddress, kSessionKey);
}

void PgNivalServer::SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult)
{
	if (eResult != E_TLR_SUCCESS)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("UserID=") << kUser.kAuthInfo.strID << _T(", ErrorCode=") << eResult);
	}

	BM::Stream kXPacket(PT_CN_IM_ANS_LOGIN_GALA);
	SAuthRequest kAuth(kUser.kAuthInfo.strID, std::wstring(), kUser.kAuthInfo.kRequestGuid, kUser.kAuthInfo.kAddress);
	kAuth = kUser.kAuthInfo;
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

ETryLoginResult PgNivalServer::MakeLoginRequestBody(SUserInfo const& rkUser, std::string& rkOutBody)
{
	GetRequestXml(EREQ_TYPE_LOGIN, rkOutBody);
	//rkOutBody.replace(pos, num, string)
	std::string strID = "%ID%";
	std::string strPW = "%PW%";
	std::string strIP = "%CLIENT_IP%";
	std::string::size_type iFind = rkOutBody.find(strID);
	if (iFind != std::string::npos)
	{
		std::string strUserID = MB(rkUser.kAuthInfo.strID);
		BM::vstring::ConvToLWR(strUserID);
		int iBase64Size = Base64EncodeGetRequiredLength(static_cast<int>(strUserID.length()));
		char* pszBase64 = new char[iBase64Size+1];
		if (false == Base64Encode((BYTE *)strUserID.c_str(), static_cast<int>(strUserID.length()), pszBase64, &iBase64Size))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Base64Encode error ID=") << rkUser.kAuthInfo.strID);
			delete[] pszBase64;
			return E_TLR_EXTERNAL_SERVER_ERROR;
		}
		pszBase64[iBase64Size] = '\0';
		rkOutBody.replace(iFind, strID.length(), pszBase64);
		delete[] pszBase64;
	}

	iFind = rkOutBody.find(strPW);
	if (iFind != std::string::npos)
	{
		std::string strUserPW = MB(rkUser.kAuthInfo.strPW);

		static int const iDigestSize = 16;
		md5_state_t md5state;
		md5_byte_t digest[iDigestSize];
		md5_init(&md5state);
		md5_append(&md5state, (md5_byte_t const *)(strUserPW.c_str()), static_cast<int>(strUserPW.length()));
		md5_finish(&md5state, digest);

		std::wstring wstrHexaPW = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
		BM::vstring::ConvToLWR(wstrHexaPW);
		rkOutBody.replace(iFind, strPW.length(), MB(wstrHexaPW));
	}

	iFind = rkOutBody.find(strIP);
	if (iFind != std::string::npos)
	{
		rkOutBody.replace(iFind, strIP.length(), MB(rkUser.kAuthInfo.kAddress.IP()));
	}

	return E_TLR_SUCCESS;

}

ETryLoginResult PgNivalServer::GetLoginResult(NIVAL::EAUTH_ERROR const eError) const
{
	using namespace NIVAL;
	ETryLoginResult eResult = E_TLR_NONE;
	switch (eError)
	{
		case EAUTH_ERROR_SUCCESS :			{ eResult = E_TLR_SUCCESS; } break;
		case EAUTH_ERROR_EMAIL_NOTCREATED :
		case EAUTH_ERROR_EMAIL_NOTCREATED2 : 
		case EAUTH_ERROR_LOGIN_NOTCREATED3 :
		case EAUTH_ERROR_LOGIN_INVALID:
		case EAUTH_ERROR_LOGIN_NOTCREATED2 :
		case EAUTH_ERROR_LOGIN_NOTCREATED:
		case EAUTH_ERROR_EMAIL_NOTEXIST:
		case EAUTH_ERROR_EMAIL_INVALID:
		case EAUTH_ERROR_ACCOUNT_NOTCREATED:
		case EAUTH_ERROR_NEED_BETA_TESTACCOUNT: { eResult = E_TLR_WRONG_ID; } break;
		case EAUTH_ERROR_WRONG_PASSWORD: { eResult = E_TLR_WRONG_PASS; }break;
		case EAUTH_ERROR_NO_XMLDATA: { eResult = E_TLR_PARSING_ERROR; } break;
		case EAUTH_ERROR_METHOD_ERROR: { eResult = E_TLR_EXTERNAL_SERVER_ERROR; } break;
		default:
			{
				INFO_LOG(BM::LOG_LV7, __FL__ << _T("unknown Error Code = ") << eError);
				eResult = E_TLR_EXTERNAL_SERVER_ERROR;
			}break;
	}
	return eResult;
}

void PgNivalServer::RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	// 할것이 없다.
}

void PgNivalServer::RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	DWORD dwUID;
	CEL::ADDR_INFO kAddress;
	pkPacket->Pop(kAccountID);
	pkPacket->Pop(dwUID);
	kAddress.ReadFromPacket(*pkPacket);

	AddCashRequest(SCashRequest(rkFrom, kAccountID, EREQ_TYPE_GET_USERMONEY, CASH::SCashCost(0, 0, 0), 0, kAddress, PT_A_CN_REQ_QUERY_CASH, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_BUYCASHITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring kCharacterName;
	CONT_BUYITEM kBuyItem;
	DWORD dwUID;
	CASH::SCashCost kCost;

	pkPacket->Pop(kAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(kCharacterName);
	pkPacket->Pop(dwUID);
	kCost.ReadFromPacket(*pkPacket);
	PU::TLoadArray_M(*pkPacket, kBuyItem);

	if (kCost.i64UsedCash > 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(kAccountID, rkFrom, PT_A_CN_REQ_BUYCASHITEM, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	if (kBuyItem.size() < 1)
	{
		SendCashResult(NIVAL::SCashResult(kAccountID, rkFrom, PT_A_CN_REQ_BUYCASHITEM, NIVAL::EBILL_ERROR_INVALID_ITEM, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	SBuyItemInfo const& rkBuyItem = kBuyItem.at(0);
	AddCashRequest(SCashRequest(rkFrom, kAccountID, EREQ_TYPE_REMOVE_USERMONEY, kCost, rkBuyItem.iItemNo, kAddress, PT_A_CN_REQ_BUYCASHITEM, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_EXPANDTIEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring kCharacterName;
	CONT_BUYITEM kBuyItem;
	DWORD dwUID;
	CASH::SCashCost kCost;

	pkPacket->Pop(kAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(kCharacterName);
	pkPacket->Pop(dwUID);
	kCost.ReadFromPacket(*pkPacket);
	PU::TLoadArray_M(*pkPacket, kBuyItem);

	if (kCost.i64UsedCash > 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(kAccountID, rkFrom, PT_A_CN_REQ_EXPANDTIEM, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	if (kBuyItem.size() < 1)
	{
		SendCashResult(NIVAL::SCashResult(kAccountID, rkFrom, PT_A_CN_REQ_EXPANDTIEM, NIVAL::EBILL_ERROR_INVALID_ITEM, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	SBuyItemInfo const& rkBuyItem = kBuyItem.at(0);
	AddCashRequest(SCashRequest(rkFrom, kAccountID, EREQ_TYPE_REMOVE_USERMONEY, kCost, rkBuyItem.iItemNo, kAddress, PT_A_CN_REQ_EXPANDTIEM, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_SENDGIFT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	DWORD dwUID;
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring strCharacterName;
	std::wstring strReceiverName;
	DWORD dwItemNo;
	std::wstring strItemName;
	WORD wItemTime;
	CASH::SCashCost kCost;

	pkPacket->Pop(dwUID);
	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(strCharacterName);
	pkPacket->Pop(strReceiverName);
	pkPacket->Pop(dwItemNo);
	pkPacket->Pop(strItemName);
	pkPacket->Pop(wItemTime);
	kCost.ReadFromPacket(*pkPacket);

	if (kCost.i64UsedCash > 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(strAccountID, rkFrom, PT_A_CN_REQ_SENDGIFT, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	AddCashRequest(SCashRequest(rkFrom, strAccountID, EREQ_TYPE_REMOVE_USERMONEY, kCost, dwItemNo, kAddress, PT_A_CN_REQ_SENDGIFT, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_CASH_ROLLBACK(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring strAccountID;
	CASH::SCashCost kCost;
	CEL::ADDR_INFO kAddress;

	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	kCost.ReadFromPacket(*pkPacket);

	if (kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(strAccountID, rkFrom, PT_A_CN_REQ_CASH_ROLLBACK, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	AddCashRequest(SCashRequest(rkFrom, strAccountID, EREQ_TYPE_ADD_USERMONEY, kCost, NIVAL_ROLLBACK_ITEMID, kAddress, PT_A_CN_REQ_CASH_ROLLBACK, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_OM_BUY_ARTICLE(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	std::wstring strBuyer;
	CASH::SCashCost kCost;

	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(strBuyer);
	kCost.ReadFromPacket(*pkPacket);

	if (kCost.i64UsedCash > 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(strAccountID, rkFrom, PT_A_CN_REQ_OM_BUY_ARTICLE, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	AddCashRequest(SCashRequest(rkFrom, strAccountID, EREQ_TYPE_REMOVE_USERMONEY, kCost, 0, kAddress, PT_A_CN_REQ_OM_BUY_ARTICLE, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_ADDCASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	CASH::SCashCost kCost;

	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	kCost.ReadFromPacket(*pkPacket);

	if (kCost.i64UsedCash < 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(strAccountID, rkFrom, PT_A_CN_REQ_ADDCASH, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	AddCashRequest(SCashRequest(rkFrom, strAccountID, EREQ_TYPE_ADD_USERMONEY, kCost, 0, kAddress, PT_A_CN_REQ_ADDCASH, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

void PgNivalServer::RecvPT_A_CN_REQ_OM_REG_ARTICLE(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	CASH::SCashCost kCost;

	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	kCost.ReadFromPacket(*pkPacket);

	if (kCost.i64UsedCash > 0 || kCost.i64UsedMileage != 0 || kCost.i64AddedMileage != 0)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot support this Cost = ") << kCost.ToString());
		SendCashResult(NIVAL::SCashResult(strAccountID, rkFrom, PT_A_CN_REQ_OM_REG_ARTICLE, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, *pkPacket));
		return;
	}
	AddCashRequest(SCashRequest(rkFrom, strAccountID, EREQ_TYPE_REMOVE_USERMONEY, kCost, 0, kAddress, PT_A_CN_REQ_OM_REG_ARTICLE, pkPacket));

	// try connection....
	CEL::SESSION_KEY kSessionKey(CashHttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kCashHttpAddress, kSessionKey);
}

HRESULT PgNivalServer::LoadXMLFiles()
{
	m_kReqXml.clear();
	m_kReqXml.resize(EREQ_TYPE_MAX);

	TCHAR szPath[MAX_PATH];
	if (0 == GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH))
	{
		DWORD dwError = GetLastError();
		ASSERT_LOG(false, BM::LOG_LV0, __FL__ << _T("GetModuleFileName failed GetLastError=") << dwError);
		return E_FAIL;
	}
	PathRemoveFileSpec(szPath);

	if (S_OK != LoadXMLFile(EREQ_TYPE_LOGIN, szPath + std::wstring(_T("\\XML\\Nival\\req_login.xml"))))
	{
		return E_FAIL;
	}
	if (S_OK != LoadXMLFile(EREQ_TYPE_GET_USERMONEY, szPath + std::wstring(_T("\\XML\\Nival\\req_getusermoney.xml"))))
	{
		return E_FAIL;
	}
	if (S_OK != LoadXMLFile(EREQ_TYPE_REMOVE_USERMONEY, szPath + std::wstring(_T("\\XML\\Nival\\req_removeusermoney.xml"))))
	{
		return E_FAIL;
	}
	if (S_OK != LoadXMLFile(EREQ_TYPE_TRANSFER_USERMONEY, szPath + std::wstring(_T("\\XML\\Nival\\req_transferusermoney.xml"))))
	{
		return E_FAIL;
	}
	if (S_OK != LoadXMLFile(EREQ_TYPE_ADD_USERMONEY, szPath + std::wstring(_T("\\XML\\Nival\\req_addusermoney.xml"))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT PgNivalServer::LoadXMLFile(EREQ_TYPE const eType, std::wstring const& strFilename)
{
	std::ifstream xmlFile( MB(strFilename.c_str()) );
	if (false == xmlFile.is_open())
	{
		ASSERT_LOG(false, BM::LOG_LV0, __FL__ << _T("Cannot open file =") << strFilename);
	}

	size_t const iBufferSize = 1024;
	char chBuffer[iBufferSize+1];
	size_t iReadSize = 0;
	std::string strXML;
	while (true)
	{
		memset(chBuffer, 0, iBufferSize+1);
		xmlFile.read(chBuffer, iBufferSize);
		if (strlen(chBuffer) == 0)
		{
			break;
		}
		strXML = strXML + std::string(chBuffer);
	}
	m_kReqXml[eType] = strXML;
	return S_OK;
}

bool PgNivalServer::GetRequestXml(EREQ_TYPE const eType, std::string& rkOut) const
{
	rkOut.clear();
	if (eType <= EREQ_TYPE_NONE || eType >= EREQ_TYPE_MAX)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unknown request type=") << static_cast<int>(eType));
		return false;
	}
	rkOut = m_kReqXml[eType];
	return true;
}

bool PgNivalServer::AddCashRequest(SCashRequest const& rkReq)
{
	SCashRequest* pkNew = new SCashRequest(rkReq);
	auto ibRet = m_kCashRequest.insert(std::make_pair(rkReq.kReqGuid, pkNew));
	m_kCashRequestList.push_back(rkReq.kReqGuid);
	return ibRet.second;
}

void PgNivalServer::SetCashServer(std::wstring const &strAddress, WORD wPort)
{
	char chAddress[100];
	char chPort[20];
	sockaddr_in* pkSockAddress = NULL;
	sprintf_s(chAddress, sizeof(chAddress), "%s", MB(strAddress.c_str()));
	sprintf_s(chPort, sizeof(chAddress), "%hd", wPort);
	addrinfo kHint;
	addrinfo *pkAddressList = NULL;
	memset(&kHint, 0, sizeof(kHint));
	kHint.ai_family = AF_INET;
	kHint.ai_socktype = SOCK_STREAM;
	kHint.ai_protocol = IPPROTO_TCP;
	int iReturn = getaddrinfo(chAddress, chPort, &kHint, &pkAddressList);
	if (iReturn != 0 || pkAddressList == NULL)
	{
		int iError = WSAGetLastError();
		ASSERT_LOG(false, BM::LOG_LV4, __FL__ << _T("Cannot get ErrorCode=") << iError << _T(", Address=") << strAddress << _T(", Port=") << wPort);
		return;
	}
	pkSockAddress = (sockaddr_in*)pkAddressList->ai_addr;
	m_kCashHttpAddress.ip = pkSockAddress->sin_addr;
	m_kCashHttpAddress.wPort = wPort;
	m_strCashHttpHostname = strAddress;
}

void CALLBACK PgNivalServer::OnConnectFromCashHttp( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnConnectFromCashHttp(pkSession);
}

void CALLBACK PgNivalServer::OnDisconnectFromCashHttp( CEL::CSession_Base *pkSession )
{
	g_kNival.Locked_OnDisconnectFromCashHttp(pkSession);
}

void CALLBACK PgNivalServer::OnRecvFromCashHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kNival.Locked_OnRecvFromCashHttp(pkSession, pkPacket);
}

void PgNivalServer::Locked_OnConnectFromCashHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bIsSucc = pkSession->IsAlive();
	if (bIsSucc)
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Auth Http Connected"));
		if (false == SendCashRequest(pkSession))
		{
			pkSession->VTerminate();
		}
	}
	else
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Failed Http Connected"));
		// try again ....
		if (false == m_kCashRequest.empty())
		{
			CEL::SESSION_KEY kSessionKey(AuthHttpConnector(), BM::GUID::NullData());
			g_kCoreCenter.Connect(m_kAuthHttpAddress, kSessionKey);
		}
	}
}

void PgNivalServer::Locked_OnDisconnectFromCashHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("Auth Http Disconnected"));
	BM::GUID const rkReq = pkSession->CustomKey();

	ParsingCashAnswer(rkReq);
}

void PgNivalServer::Locked_OnRecvFromCashHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID const rkReq = pkSession->CustomKey();
	CONT_CASHREQUEST::iterator itor_req = m_kCashRequest.find(rkReq);
	if (m_kCashRequest.end() == itor_req)
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find request=") << rkReq);
		return;
	}

	SCashRequest* pkRequest = (*itor_req).second;
	if (pkRequest == NULL)
	{
		return;
	}
	size_t iSize = pkPacket->Size();
	char* pkMsg = new char[pkPacket->Size() + 1];
	pkPacket->PopMemory(pkMsg, iSize);
	pkMsg[iSize] = '\0';
	pkRequest->strAnswer += pkMsg;
	delete[] pkMsg;
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("RecvMsg =") << pkRequest->strAnswer);
}

bool PgNivalServer::SendCashRequest(CEL::CSession_Base *pkSession)
{
	NIVAL::EBILL_ERROR eErrorCode = NIVAL::EBILL_ERROR_SUCCESS;
	SCashRequest* pkRequest = NULL;
	while (false == m_kCashRequestList.empty())
	{
		BM::GUID kReqGuid = m_kCashRequestList.back();
		m_kCashRequestList.pop_back();

		CONT_CASHREQUEST::iterator itor_req = m_kCashRequest.find(kReqGuid);
		if (m_kCashRequest.end() != itor_req)
		{
			pkRequest = (*itor_req).second;
			break;
		}
	}

	if (NULL == pkRequest)
	{
		return false;
	}
	SSiteUserInfo kUserInfo;
	BM::Stream kPacket;
	std::string strMsg;
	std::string strXml;

	std::wstring strAccountID = pkRequest->strAccountID;
	BM::vstring::ConvToUPR(strAccountID);
	//if (false == GetUserInfo(strAccountID, kUserInfo))
	//{
	//	INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find user=") << pkRequest->strAccountID);
	//	eErrorCode = NIVAL::EBILL_ERROR_CANNOT_FIND_USER;
	//	goto NivalSendCashRequest_End;
	//}

	pkSession->CustomKey(pkRequest->kReqGuid);

	if (false == GetRequestXml(pkRequest->eReqType, strXml))
	{
		eErrorCode = NIVAL::EBILL_ERROR_SYSTEMERROR;
		goto NivalSendCashRequest_End;
	}
	if (S_OK != ConvertXMLVariable(strXml, *pkRequest, kUserInfo))
	{
		eErrorCode = NIVAL::EBILL_ERROR_SYSTEMERROR;
		goto NivalSendCashRequest_End;
	}

	char chLength[10];
	sprintf_s(chLength, 10, "%d", strXml.length());

	strMsg += "POST / HTTP/1.1\r\n";
	strMsg += "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2)\r\n";
	strMsg += "Host: ";
	strMsg += MB(m_strCashHttpHostname);
	strMsg += "\r\nContent-type: application/x-www-form-urlencoded\r\n";     
	strMsg += "Content-length: ";
	strMsg += std::string(chLength);
	strMsg += "\r\nConnection: close\r\n\r\n";
	strMsg += "xml=";
	strMsg += strXml;

	INFO_LOG(BM::LOG_LV9, __FL__ << _T("XML = ") << strXml);

	kPacket.Push(strMsg.c_str(), strMsg.length());
	if (false == pkSession->VSend(kPacket))
	{
		eErrorCode = NIVAL::EBILL_ERROR_SYSTEMERROR;
		goto NivalSendCashRequest_End;
	}
	return true;

NivalSendCashRequest_End:
	if (eErrorCode != NIVAL::EBILL_ERROR_SUCCESS && pkRequest != NULL)
	{
		SendCashResult(NIVAL::SCashResult(pkRequest->strAccountID, pkRequest->kSite, pkRequest->wReqPacket, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, pkRequest->kAddonPacket));
		RemoveCashRequest(pkRequest->kReqGuid);
	}
	return false;
}

void PgNivalServer::ParsingCashAnswer(BM::GUID const& rkRequest)
{
	CONT_CASHREQUEST::const_iterator itor_cash = m_kCashRequest.find(rkRequest);
	if (itor_cash == m_kCashRequest.end())
	{
		return;
	}
	SCashRequest const* pkRequest = (*itor_cash).second;
	std::string strXML;
	if (pkRequest == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("CashRequest info is NULL ReqGuid=") << (*itor_cash).first);
		goto ParsingCashAnswer_Error;
	}
	if (false == GetXMLFromHttpMsg(pkRequest->strAnswer, strXML))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Cannot get XML body MSG=") << pkRequest->strAnswer);
		goto ParsingCashAnswer_Error;
	}

	switch(pkRequest->eReqType)
	{
	case EREQ_TYPE_GET_USERMONEY:
		{
			OnCashRecvGetUserMoney(pkRequest, strXML);
		}break;
	case EREQ_TYPE_REMOVE_USERMONEY:
		{
			OnCashRecvRemoveUserMoney(pkRequest, strXML);
		}break;
	case EREQ_TYPE_TRANSFER_USERMONEY:
		{
			OnCashRecvTransferUserMoney(pkRequest, strXML);
		}break;
	case EREQ_TYPE_ADD_USERMONEY:
		{
			OnCashRecvAddUserMoney(pkRequest, strXML);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("unknown RequestType=") << pkRequest->eReqType);
			goto ParsingCashAnswer_Error;
		}break;
	}
	// success
	goto ParsingCashAnswer_End;

ParsingCashAnswer_Error:
	if (pkRequest != NULL)
	{
		SendCashResult(NIVAL::SCashResult(pkRequest->strAccountID, pkRequest->kSite, pkRequest->wReqPacket, NIVAL::EBILL_ERROR_SYSTEMERROR, std::wstring(), 0, 0, 0, pkRequest->kAddonPacket));
	}
ParsingCashAnswer_End:
	RemoveCashRequest(rkRequest);
}

bool PgNivalServer::RemoveCashRequest(CONT_CASHREQUEST::key_type const& rkKey)
{
	CONT_CASHREQUEST::iterator itor_cash = m_kCashRequest.find(rkKey);
	if (itor_cash != m_kCashRequest.end())
	{
		if ((*itor_cash).second != NULL)
		{
			delete (*itor_cash).second;
		}
		m_kCashRequest.erase(itor_cash);
		return true;
	}
	return false;
}

HRESULT PgNivalServer::ConvertXMLVariable(std::string& kXML, SCashRequest const& rkReq, SSiteUserInfo const& rkUserInfo) const
{
	std::string const strID = "%ID%";
	std::string const strIP = "%CLIENT_IP%";
	std::string const strReqCash = "%REQ_CASH%";
	std::string const strGameID = "%GAME_ID%";
	std::string const strItemID = "%ITEM_ID%";
	std::string const strReqGuid = "%REQ_KEY%";
	std::string const strCRC = "%CRC%";
	std::string const strColon = ":";

	__int64 i64Cash = 0;
	switch (rkReq.eReqType)
	{
	case EREQ_TYPE_REMOVE_USERMONEY:
		{
			i64Cash = -rkReq.kCost.i64UsedCash;	// 줄어드는 캐시량을 (+)로 보내주어야 한다.
		}break;
	case EREQ_TYPE_ADD_USERMONEY:
		{
			i64Cash = rkReq.kCost.i64UsedCash;
		}break;
	case EREQ_TYPE_GET_USERMONEY:
	case EREQ_TYPE_TRANSFER_USERMONEY:
	default:
		{
			//  cash변동없는 명령
			i64Cash = 0;
		}break;
	}

	std::string const kAccountID = MB(rkReq.strAccountID);
	std::string const kReqCash = MB(BM::vstring(i64Cash).operator std::wstring const&());
	std::string const kGameID = MB(BM::vstring(NIVAL_DRAGONICA_GAMEID).operator std::wstring const&());
	std::string const kItemID = MB(BM::vstring(rkReq.iItemID).operator std::wstring const&());
	std::string const kIP = MB(rkReq.kAddress.IP());

	// Making CRC
	std::string kCRC;
	switch (rkReq.eReqType)
	{
	case EREQ_TYPE_REMOVE_USERMONEY:
		{
			kCRC = kAccountID + strColon + kReqCash + strColon + kIP + strColon + kGameID + strColon + kItemID + strColon + NIVAL_GAMEPASSWORD;
		}break;
	case EREQ_TYPE_ADD_USERMONEY:
		{
			kCRC = kAccountID + strColon + kReqCash + strColon + kIP + strColon + kGameID + strColon + kItemID + strColon + NIVAL_GAMEPASSWORD;
		}break;
	case EREQ_TYPE_GET_USERMONEY:
		{
			kCRC.clear();
		}break;
	case EREQ_TYPE_TRANSFER_USERMONEY:
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown ReqType=") << rkReq.eReqType);
			return E_FAIL;
		}break;
	}

	PgStringUtil::ReplaceStr(kXML, strID, kAccountID, kXML);
	PgStringUtil::ReplaceStr(kXML, strIP, kIP, kXML);
	PgStringUtil::ReplaceStr(kXML, strReqCash, kReqCash, kXML);
	PgStringUtil::ReplaceStr(kXML, strGameID, kGameID, kXML);
	PgStringUtil::ReplaceStr(kXML, strItemID, kItemID, kXML);
	PgStringUtil::ReplaceStr(kXML, strReqGuid, std::string(MB(rkReq.kReqGuid.str())), kXML);

	if (kCRC.length() > 0)
	{
		static int const iDigestSize = 16;
		md5_state_t md5state;
		md5_byte_t digest[iDigestSize];
		md5_init(&md5state);
		md5_append(&md5state, (md5_byte_t const *)(kCRC.c_str()), static_cast<int>(kCRC.length()));
		md5_finish(&md5state, digest);

		std::wstring wstrHexaCRC = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
		BM::vstring::ConvToLWR(wstrHexaCRC);
		PgStringUtil::ReplaceStr(kXML, strCRC, std::string(MB(wstrHexaCRC)), kXML);
	}
	return S_OK;
}

void PgNivalServer::SendCashResult(NIVAL::SCashResult const& kResult)
{
	PACKET_ID_TYPE wResType;
	switch (kResult.wReqType)
	{
	case PT_A_CN_REQ_QUERY_CASH:		{ wResType = PT_CN_A_ANS_QUERY_CASH; } break;
	case PT_A_CN_REQ_SENDGIFT:			{ wResType = PT_CN_A_ANS_SENDGIFT; } break;
	case PT_A_CN_REQ_EXPANDTIEM:		{ wResType = PT_CN_A_ANS_EXPANDTIEM; } break;
	case PT_A_CN_REQ_BUYCASHITEM:		{ wResType = PT_CN_A_ANS_BUYCASHITEM; } break;
	case PT_A_CN_REQ_OM_BUY_ARTICLE:	{ wResType = PT_CN_A_ANS_OM_BUY_ARTICLE; } break;
	case PT_A_CN_REQ_ADDCASH:			{ wResType = PT_CN_A_ANS_ADDCASH; } break;
	case PT_A_CN_REQ_OM_REG_ARTICLE:	{ wResType = PT_CN_A_ANS_OM_REG_ARTICLE; } break;
	case PT_A_CN_REQ_CASH_ROLLBACK:		{ wResType = PT_CN_A_ANS_CASH_ROLLBACK; } break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unknown packet type=") << kResult.wReqType);
			return;
		}break;
	}

	BM::Stream kPacket(wResType);
	kResult.WriteToPacket(kPacket);
	SendCashResult(kResult.kSI, kPacket);
}

bool PgNivalServer::SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket)
{
	BM::Stream kFPacket(PT_X_N_ANS_CASH);
	dynamic_cast<SERVER_IDENTITY const&>(rkFromSI).WriteToPacket(kFPacket);
	kFPacket.Push(rkPacket);
	return SendToServer(rkFromSI.nSiteNo, kFPacket);
}


bool PgNivalServer::GetXMLFromHttpMsg(std::string const& rkInput, std::string& rkXML)
{
	std::string::size_type const iXMLBeginPos = rkInput.find("<?xml ");
	if (iXMLBeginPos == std::string::npos)
	{
		return false;
	}

	rkXML = rkInput.substr(iXMLBeginPos);
	return true;
}

void PgNivalServer::OnCashRecvGetUserMoney(SCashRequest const* pkReq, std::string const& strXML)
{
	NIVAL::EBILL_ERROR eErrorCode = NIVAL::EBILL_ERROR_SUCCESS;
	TiXmlDocument kXML;
	kXML.Parse(strXML.c_str());
	std::wstring strErrorMsg;
	CASH::SCashCost kCost;
	BM::GUID kReqGuid;
	if ( kXML.Error() )
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	TiXmlNode const* pkRoot = kXML.FirstChild("xml");
	int iErrorNo = 0;
	if (pkRoot == NULL)
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	/*
	if (S_OK != GetXMLValue(pkRoot, "req_id", kReqGuid))
	{
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	if (kReqGuid != pkReq->kReqGuid)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Mismatched GUID between ReturnedGuid and ReqGuid"));
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	*/
	TiXmlNode const* pkError = pkRoot->FirstChild("error");
	if (pkError != NULL)
	{
		// ERROR
		int iError = 0;
		if (S_OK != GetXMLValue<int>(pkError, "code", iError))
		{
			eErrorCode = NIVAL::EBILL_ERROR_SYSTEMERROR;
		}
		eErrorCode = static_cast<NIVAL::EBILL_ERROR>(iError);
		GetXMLValue(pkError, "description", strErrorMsg);
		goto Nival_OnCashRecvGetUserMoney_Return;
	}

	// SUCCESS
	TiXmlNode const* pkReturn = pkRoot->FirstChild("return");
	if (pkReturn == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <return> XML=") << strXML);
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;		
	}
	if (S_OK != GetXMLValue(pkReturn, "money", kCost.i64UsedCash))	// current cash
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <money> XML=") << strXML);
		kCost.i64UsedCash = 0;
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;		
	}

Nival_OnCashRecvGetUserMoney_Return:
	SendCashResult(NIVAL::SCashResult(pkReq->strAccountID, pkReq->kSite, pkReq->wReqPacket, eErrorCode, strErrorMsg, kCost.i64UsedCash, kCost.i64UsedMileage, kCost.i64AddedMileage, pkReq->kAddonPacket));
}

void PgNivalServer::OnCashRecvRemoveUserMoney(SCashRequest const* pkReq, std::string const& strXML)
{
	NIVAL::EBILL_ERROR eErrorCode = NIVAL::EBILL_ERROR_SUCCESS;
	TiXmlDocument kXML;
	kXML.Parse(strXML.c_str());
	std::wstring strErrorMsg;
	CASH::SCashCost kCost;
	BM::GUID kReqGuid;
	if ( kXML.Error() )
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;
		goto Nival_OnCashRecvRemoveUserMoney_Return;
	}
	TiXmlNode const* pkRoot = kXML.FirstChild("xml");
	int iErrorNo = 0;
	if (pkRoot == NULL)
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;
	}
	/*
	if (S_OK != GetXMLValue(pkRoot, "req_id", kReqGuid))
	{
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	if (kReqGuid != pkReq->kReqGuid)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Mismatched GUID between ReturnedGuid and ReqGuid"));
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	*/
	TiXmlNode const* pkReturn = pkRoot->FirstChild("return");
	if (pkReturn == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <return> XML=") << strXML);
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (S_OK != GetXMLValue(pkReturn, "money", kCost.i64UsedCash))	// current cash
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <money> XML=") << strXML);
		kCost.i64UsedCash = 0;
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (S_OK != GetXMLValue(pkReturn, "transfer", iErrorNo))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <transfer> XML=") << strXML);
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (iErrorNo != 1)	// 1이면 성공, 0이면 실패
	{
		eErrorCode = NIVAL::EBILL_ERROR_NOT_ENOUGH_CASH;
	}

Nival_OnCashRecvRemoveUserMoney_Return:
	SendCashResult(NIVAL::SCashResult(pkReq->strAccountID, pkReq->kSite, pkReq->wReqPacket, eErrorCode, strErrorMsg, kCost.i64UsedCash, kCost.i64UsedMileage, kCost.i64AddedMileage, pkReq->kAddonPacket));
}

void PgNivalServer::OnCashRecvTransferUserMoney(SCashRequest const* pkReq, std::string const& strXML)
{
	INFO_LOG(BM::LOG_LV5, __FL__ << _T("NOT IMPLEMENTED"));
}

void PgNivalServer::OnCashRecvAddUserMoney(SCashRequest const* pkReq, std::string const& strXML)
{
	NIVAL::EBILL_ERROR eErrorCode = NIVAL::EBILL_ERROR_SUCCESS;
	TiXmlDocument kXML;
	kXML.Parse(strXML.c_str());
	std::wstring strErrorMsg;
	CASH::SCashCost kCost;
	BM::GUID kReqGuid;
	if ( kXML.Error() )
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;
		goto Nival_OnCashRecvRemoveUserMoney_Return;
	}
	TiXmlNode const* pkRoot = kXML.FirstChild("xml");
	int iErrorNo = 0;
	if (pkRoot == NULL)
	{
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;
	}
	/*
	if (S_OK != GetXMLValue(pkRoot, "req_id", kReqGuid))
	{
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	if (kReqGuid != pkReq->kReqGuid)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Mismatched GUID between ReturnedGuid and ReqGuid"));
		eResult = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvGetUserMoney_Return;
	}
	*/
	TiXmlNode const* pkReturn = pkRoot->FirstChild("return");
	if (pkReturn == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <return> XML=") << strXML);
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (S_OK != GetXMLValue(pkReturn, "money", kCost.i64UsedCash))	// current cash
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <money> XML=") << strXML);
		kCost.i64UsedCash = 0;
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (S_OK != GetXMLValue(pkReturn, "transfer", iErrorNo))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot find <transfer> XML=") << strXML);
		eErrorCode = NIVAL::EBILL_ERROR_CASHSERVER_MALFUNC;;
		goto Nival_OnCashRecvRemoveUserMoney_Return;		
	}
	if (iErrorNo != 1)	// 1이면 성공, 0이면 실패
	{
		eErrorCode = NIVAL::EBILL_ERROR_NOT_ENOUGH_CASH;
	}

Nival_OnCashRecvRemoveUserMoney_Return:
	SendCashResult(NIVAL::SCashResult(pkReq->strAccountID, pkReq->kSite, pkReq->wReqPacket, eErrorCode, strErrorMsg, kCost.i64UsedCash, kCost.i64UsedMileage, kCost.i64AddedMileage, pkReq->kAddonPacket));
}
