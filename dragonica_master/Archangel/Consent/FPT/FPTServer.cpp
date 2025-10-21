#include "stdafx.h"
#include "atlenc.h"
#include "tinyxml/tinyxml.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgStringUtil.h"
#include "Constant.h"
#include "FPTServer.h"
#include "md5.h"


const std::string PgFPTServer::FPT_GAMEPASSWORD = "c6279631-94b5-49f4-825a-9c59f6b93591";
const std::string PgFPTServer::FPT_DRAGONICA_GAMEID = "142";


PgFPTServer::PgFPTServer()
{
	LoadXMLFiles();
}

PgFPTServer::~PgFPTServer()
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
}

void CALLBACK PgFPTServer::OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kFPT.Locked_OnConnectFromImmigration(pkSession);
}

void CALLBACK PgFPTServer::OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kFPT.Locked_OnDisconnectFromImmigration(pkSession);
}

void CALLBACK PgFPTServer::OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kFPT.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgFPTServer::RecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
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
	default:
		{
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	}
}

bool PgFPTServer::Locked_OnRegistConnector(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_FPT_Config.ini"));
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
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}
	return true;
}

void CALLBACK PgFPTServer::OnConnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	g_kFPT.Locked_OnConnectFromAuthHttp(pkSession);
}

void CALLBACK PgFPTServer::OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	g_kFPT.Locked_OnDisconnectFromAuthHttp(pkSession);
}

void CALLBACK PgFPTServer::OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kFPT.Locked_OnRecvFromAuthHttp(pkSession, pkPacket);
}

void PgFPTServer::Locked_OnConnectFromAuthHttp( CEL::CSession_Base *pkSession )
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

void PgFPTServer::Locked_OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("Auth Http Disconnected"));
	BM::GUID const rkReq = pkSession->CustomKey();

	ParsingLoginAnswer(rkReq);
}

void PgFPTServer::Locked_OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
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
}

void PgFPTServer::SetAuthServer(std::wstring const &strAddress, WORD wPort)
{
	char chAddress[100] = {0,};
	char chPort[20] = {0,};
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



bool PgFPTServer::SendLoginRequest(CEL::CSession_Base *pkSession)
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
	// TODO : ±ёЗцЗШѕЯ ЗФ..
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
	strMsg += "POST /SrvCheckAccount_DR.asmx HTTP/1.1\r\n";
	//strMsg += "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2)\r\n";
	strMsg += "Host: auth.gate.vn";
	//strMsg += MB(m_strAuthHttpHostname);
	strMsg += "\r\nContent-Type: text/xml; charset=utf-8\r\n";
	strMsg += "Content-Length: ";
	strMsg += std::string(chLength);
	strMsg += "\r\nSOAPAction: \"https://psp.gate.vn/CheckAccount_DR\"";
	strMsg += "\r\nConnection: close\r\n\r\n";

	strMsg += strBody;

	BM::Stream kPacket;
	kPacket.Push(strMsg.c_str(), strMsg.length());
	pkSession->VSend(kPacket);

	INFO_LOG(BM::LOG_LV7, __FL__ << _T("LoginRequest =\r\n") << strMsg);
	return true;
}

void PgFPTServer::ParsingLoginAnswer(BM::GUID const& rkRequest)
{
	CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(rkRequest);
	if (m_kUserByRequest.end() == itor_req)
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find request=") << rkRequest);
		return;
	}
	SUserInfo* const pkUser = (*itor_req).second;
	ETryLoginResult eResult = E_TLR_NONE;
	{
		std::string strXML;
		TiXmlDocument kXML;
		std::string strHashTemp;
		int iValue = 0;

		int iErrorCode = FPT::EAUTH_ERROR_UNKNOWN;
		static int const iDigestSize = 16;
		std::wstring wstrHexaMd5;
		BM::GUID kReqGuid;

		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Answer =\r\n") << pkUser->strAnswer);
		if (false == GetXMLFromHttpMsg(pkUser->strAnswer, strXML))
		{
			eResult = E_TLR_PARSING_ERROR;
			goto FPTParsingLoginAnswer_Error;
		}

		kXML.Parse(strXML.c_str());
		if ( kXML.Error() )
		{
			eResult = E_TLR_PARSING_ERROR;
			goto FPTParsingLoginAnswer_Error;
		}

		TiXmlNode const* pkChild = kXML.FirstChild();
		if (pkChild == NULL)
		{
			eResult = E_TLR_PARSING_ERROR;
			goto FPTParsingLoginAnswer_Error;		
		}
		pkChild = pkChild->NextSibling("soap:Envelope");
		int iDepth = 0;
		while (pkChild != NULL && iDepth++ < 2)
		{
			pkChild = pkChild->FirstChild();
		}
		if (pkChild == NULL)
		{
			eResult = E_TLR_PARSING_ERROR;
			goto FPTParsingLoginAnswer_Error;		
		}
		std::wstring strResult;
		GetXMLValue(pkChild, "CheckAccount_DRResult", strResult);
		
		__int64 const iUID = _atoi64( MB(strResult) );
		if(0==iUID)
		{
			eResult = E_TLR_FAILED;
			goto FPTParsingLoginAnswer_Error;
		}
		// Success strResult = ERROR_CODE,ReqGuid,LoginID,UID,GMLevel,CheckSum
		// Failed strResult = ERROR_CODE,ReqGuid,LoginID,CheckSum
		//std::list< std::wstring > kLineString;
		//BM::vstring::CutTextByKey<std::wstring>(strResult, L",", kLineString, true);
		//if (kLineString.size() < 6)
		//{
		//	eResult = E_TLR_PARSING_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//std::list< std::wstring >::const_iterator itor_list = kLineString.begin();
		//// 1st Value = Error Code
		//if (S_OK != BM::vstring(*itor_list).GetNumber(std::wstring(_T("%d")), iErrorCode))
		//{
		//	eResult = E_TLR_PARSING_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//if (iErrorCode != FPT::EAUTH_ERROR_SUCCESS)
		//{
		//	// Error
		//	eResult = GetLoginResult(static_cast<FPT::EAUTH_ERROR>(iErrorCode));
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//strHashTemp += MB((*itor_list).c_str());
		//++itor_list;
		//// 2ndValue = ReqGuid
		//kReqGuid.Set(*itor_list);
		//if (kReqGuid != pkUser->kReqGuid)
		//{
		//	eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//strHashTemp += MB((*itor_list).c_str());
		//++itor_list;
		//// 3rd Value = Login ID
		//std::wstring strTempID = pkUser->kAuthInfo.strID;
		//BM::vstring::ConvToLWR(strTempID);
		//if ((*itor_list) != strTempID)
		//{
		//	eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//strHashTemp += MB((*itor_list).c_str());
		//++itor_list;
		//// 4th Value = UID
		//if (S_OK != BM::vstring(*itor_list).GetNumber(std::wstring(_T("%d")), iValue))
		//{
		//	eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//pkUser->kAuthInfo.iUID = iValue;
		//strHashTemp += MB((*itor_list).c_str());
		//++itor_list;
		//// 5th Value = GM Level
		//if (S_OK != BM::vstring(*itor_list).GetNumber(std::wstring(_T("%d")), iValue))
		//{
		//	eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		//pkUser->kAuthInfo.byGMLevel = static_cast<BYTE>(iValue);
		//strHashTemp += MB((*itor_list).c_str());
		//++itor_list;
		//// 6th Value = CheckSum
		//strHashTemp += FPT_GAMEPASSWORD;

		//md5_state_t md5state;
		//md5_byte_t digest[iDigestSize];
		//md5_init(&md5state);
		//md5_append(&md5state, (md5_byte_t const *)(strHashTemp.c_str()), static_cast<int>(strHashTemp.length()));
		//md5_finish(&md5state, digest);

		//wstrHexaMd5 = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
		//BM::vstring::ConvToLWR(wstrHexaMd5);
		//if (wstrHexaMd5 != *itor_list)
		//{
		//	INFO_LOG(BM::LOG_LV5, __FL__ << _T("CheckSum error ServerSide=") << wstrHexaMd5 << _T(", Received=") << (*itor_list));
		//	eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		//	goto FPTParsingLoginAnswer_Error;
		//}
		eResult = E_TLR_SUCCESS;
	}

FPTParsingLoginAnswer_Error :
	SendLoginResult(NULL, *pkUser, eResult);
	/* Request information release */
	RemoveUserInfo(rkRequest);
}

void PgFPTServer::RemoveUserInfo(BM::GUID const& rkRequest)
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

void PgFPTServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);
	Timer5s();
}

void PgFPTServer::Timer5s()
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

	PgSiteMemberMgr::Timer5s();
}

void PgFPTServer::Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration(pkSession, usType, pkPacket);
}


void PgFPTServer::RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SAuthRequest kRecv;
	kRecv.ReadFromPacket(*pkPacket);

	if( (kRecv.strID.length() <= 0 || kRecv.strID.length() > SClientTryLogin::GetIDMaxLength() )
	 || (kRecv.strPW.length() <= 0 || kRecv.strPW.length() > SClientTryLogin::GetPWMaxLength()) )
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

	short sSite = 0;
	bool bSiteFind = PgSessionMgr::GetSessionKey(pkSession, sSite);
	SessionInfo* const pkSessionInfo = PgSessionMgr::GetSessionInfo(pkSession);
	if ( bSiteFind == false || pkSessionInfo == NULL )
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_WRONG_ID);
		return;
	}

	// АМ№М БўјУБЯАО°Ў °Л»зЗП±в
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

void PgFPTServer::SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult)
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

ETryLoginResult PgFPTServer::MakeLoginRequestBody(SUserInfo const& rkUser, std::string& rkOutBody)
{
	GetRequestXml(EREQ_TYPE_LOGIN, rkOutBody);
	//rkOutBody.replace(pos, num, string)
	std::string const strID = "%ID%";
	std::string const strPW = "%PW%";
	std::string const strIP = "%CLIENT_IP%";
	std::string const strGameID = "%GAME_ID%";

	{
		std::string strUserID = MB(rkUser.kAuthInfo.strID);
		BM::vstring::ConvToLWR(strUserID);
		/*
		int iBase64Size = Base64EncodeGetRequiredLength(static_cast<int>(strUserID.length()));
		char* pszBase64 = new char[iBase64Size+1];
		if (false == Base64Encode((BYTE *)strUserID.c_str(), static_cast<int>(strUserID.length()), pszBase64, &iBase64Size))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Base64Encode error ID=") << rkUser.kAuthInfo.strID);
			delete[] pszBase64;
			return E_TLR_EXTERNAL_SERVER_ERROR;
		}
		pszBase64[iBase64Size] = '\0';
		PgStringUtil::ReplaceStr(rkOutBody, strID, std::string(pszBase64), rkOutBody);
		delete[] pszBase64;
		*/
		PgStringUtil::ReplaceStr(rkOutBody, strID, strUserID, rkOutBody);
	}

	static int const iDigestSize = 16;
	{
		/*
		std::string strUserID = MB(rkUser.kAuthInfo.strID);
		std::reverse(strUserID.begin(), strUserID.end());
		BM::vstring::ConvToUPR(strUserID);
		std::string strUserPW = MB(rkUser.kAuthInfo.strPW);
		std::string strPWKey = strUserID + strUserPW;
		*/
		std::string strPWKey = MB(rkUser.kAuthInfo.strPW);

		md5_state_t md5state;
		md5_byte_t digest[iDigestSize];
		md5_init(&md5state);
		md5_append(&md5state, (md5_byte_t const *)(strPWKey.c_str()), static_cast<int>(strPWKey.length()));
		md5_finish(&md5state, digest);

		std::wstring wstrHexaPW = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
		BM::vstring::ConvToLWR(wstrHexaPW);
		PgStringUtil::ReplaceStr(rkOutBody, strPW, std::string(MB(wstrHexaPW)), rkOutBody);
	}

	//{
	//	// CRC
	//	std::string strTemp = MB(rkUser.kAuthInfo.strID);
	//	BM::vstring::ConvToLWR(strTemp);
	//	std::string strKey = strTemp;
	//	strKey = strKey + std::string(MB(rkUser.kAuthInfo.strPW));
	//	strKey = strKey + strReqGuid;
	//	strKey = strKey + std::string(MB(rkUser.kAuthInfo.kAddress.IP()));
	//	strKey = strKey + FPT_DRAGONICA_GAMEID;
	//	strKey = strKey + FPT_GAMEPASSWORD;

	//	md5_state_t md5state;
	//	md5_byte_t digest[iDigestSize];
	//	md5_init(&md5state);
	//	md5_append(&md5state, (md5_byte_t const *)(strKey.c_str()), static_cast<int>(strKey.length()));
	//	md5_finish(&md5state, digest);

	//	std::wstring wstrHexaPW = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
	//	BM::vstring::ConvToLWR(wstrHexaPW);
	//	PgStringUtil::ReplaceStr(rkOutBody, strCRC, std::string(MB(wstrHexaPW)), rkOutBody);
	//}

	PgStringUtil::ReplaceStr(rkOutBody, strGameID, FPT_DRAGONICA_GAMEID, rkOutBody);
	PgStringUtil::ReplaceStr(rkOutBody, strIP, std::string(MB(rkUser.kAuthInfo.kAddress.IP())), rkOutBody);

	return E_TLR_SUCCESS;

}

ETryLoginResult PgFPTServer::GetLoginResult(FPT::EAUTH_ERROR const eError) const
{
	using namespace FPT;
	ETryLoginResult eResult = E_TLR_NONE;
	switch (eError)
	{
		case EAUTH_ERROR_SUCCESS :			{ eResult = E_TLR_SUCCESS; } break;
		default:
			{
				INFO_LOG(BM::LOG_LV7, __FL__ << _T("unknown Error Code = ") << eError);
				eResult = E_TLR_WRONG_ID;
			}break;
	}
	return eResult;
}

void PgFPTServer::RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	// ЗТ°НАМ ѕшґЩ.
}

HRESULT PgFPTServer::LoadXMLFiles()
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

	if (S_OK != LoadXMLFile(EREQ_TYPE_LOGIN, szPath + std::wstring(_T("\\XML\\FPT\\req_login.xml"))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT PgFPTServer::LoadXMLFile(EREQ_TYPE const eType, std::wstring const& strFilename)
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

bool PgFPTServer::GetRequestXml(EREQ_TYPE const eType, std::string& rkOut) const
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

bool PgFPTServer::GetXMLFromHttpMsg(std::string const& rkInput, std::string& rkXML)
{
	std::string::size_type const iXMLBeginPos = rkInput.find("<?xml ");
	if (iXMLBeginPos == std::string::npos)
	{
		return false;
	}

	rkXML = rkInput.substr(iXMLBeginPos);
	return true;
}

