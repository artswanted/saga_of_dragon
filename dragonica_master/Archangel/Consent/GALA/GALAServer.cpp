#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "AilePack/Constant.h"
#include "AilePack/GALA_Constant.h"
#include "Constant.h"
#include "GALAServer.h"

PgGalaServer::PgGalaServer()
: m_dwLastGTDXRequestKey(0)
{
	
}

PgGalaServer::~PgGalaServer()
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

void CALLBACK PgGalaServer::OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnConnectFromImmigration(pkSession);
}

void CALLBACK PgGalaServer::OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnDisconnectFromImmigration(pkSession);
}

void CALLBACK PgGalaServer::OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kGala.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgGalaServer::RecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
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
					RecvPT_A_CN_REQ_QUERY_CASH( kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM:
				{
					RecvPT_A_CN_REQ_BUYCASHITEM(eNationCode, kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_EXPANDTIEM:
				{
					RecvPT_A_CN_REQ_EXPANDTIEM(eNationCode, kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_SENDGIFT:
				{
					RecvPT_A_CN_REQ_SENDGIFT(eNationCode, kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM_ROLLBACK:
				{
					RecvPT_A_CN_REQ_BUYCASHITEM_ROLLBACK(eNationCode, kSiteSI, pkPacket);
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

void CALLBACK PgGalaServer::OnConnectFromGTXD( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnConnectFromGTXD(pkSession);
}

void CALLBACK PgGalaServer::OnDisconnectFromGTXD( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnDisconnectFromGTXD(pkSession);
}

void CALLBACK PgGalaServer::OnRecvFromGTXD(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kGala.Locked_OnRecvFromGTXD(pkSession, pkPacket);
}

void PgGalaServer::Locked_OnConnectFromGTXD( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnConnected(pkSession);
}

void PgGalaServer::Locked_OnDisconnectFromGTXD( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected(pkSession);
}

void PgGalaServer::Locked_OnRecvFromGTXD(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iRdPos = pkPacket->RdPos();
	GALA::GTDX_HEADER kHead(GALA::EUSER_REQ_NONE);
	pkPacket->Pop(kHead);
	pkPacket->RdPos(iRdPos);
	switch (kHead.ReqType)
	{
	case GALA::EUSER_REQ_CASH_QUERY:
		{
			OnGTDXReceive<GALA::BILL_PACK_BALANCE>(*pkPacket);
		}break;
	case GALA::EUSER_REQ_ITEM_BUY:
		{
			OnGTDXReceive<GALA::BILL_PACK_BUY>(*pkPacket);
		}break;
	case GALA::EUSER_REQ_GIFT:
		{
			OnGTDXReceive<GALA::BILL_PACK_GIFT>(*pkPacket);
		}break;
	case GALA::EUSER_REQ_CANCEL_ITEM_BUY:
		{
			OnGTDXReceive<GALA::BILL_PACK_BUY_CNL>(*pkPacket);
		}break;
	case GALA::EUSER_REQ_SERVER_STATE:
		{
			OnGTDXReceive_ServerState(*pkPacket);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown ReqType=") << kHead.ReqType);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown ReqType=") << kHead.ReqType);
		}break;
	}
}

bool PgGalaServer::Locked_OnRegistConnector(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_Gala_Config.ini"));
	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	case CEL::ST_GALA:
		{
			TCHAR chValue[100] = {0,};
			int iValue;
			GetPrivateProfileString(_T("GTXD"), _T("IP"), _T("0.0.0.0"), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = GetPrivateProfileInt(_T("GTXD"), _T("PORT"), 0, wstrFilename.c_str());
			AddAddress(GTDX_SERVER_SITE_NUM, rkArg.guidObj, chValue, static_cast<WORD>(iValue));
		}break;
	case CEL::ST_EXTERNAL1:
		{
			TCHAR chValue[100] = {0,};
			int iValue;
			GetPrivateProfileString(_T("AUTH_HTTP"), _T("ADDRESS"), _T(""), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = GetPrivateProfileInt(_T("AUTH_HTTP"), _T("PORT"), 0, wstrFilename.c_str());
			SetAuthServer(chValue, iValue);
			HttpConnector(rkArg.guidObj);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}
	return true;
}

void CALLBACK PgGalaServer::OnConnectFromHttp( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnConnectFromHttp(pkSession);
}

void CALLBACK PgGalaServer::OnDisconnectFromHttp( CEL::CSession_Base *pkSession )
{
	g_kGala.Locked_OnDisconnectFromHttp(pkSession);
}

void CALLBACK PgGalaServer::OnRecvFromHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	g_kGala.Locked_OnRecvFromHttp(pkSession, pkPacket);
}

void PgGalaServer::Locked_OnConnectFromHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	bool const bIsSucc = pkSession->IsAlive();
	if (bIsSucc)
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Http Connected"));
		if (false == SendLoginRequest(pkSession))
		{
			pkSession->VTerminate();
		}
	}
	else
	{
		INFO_LOG(BM::LOG_LV9, __FL__ << _T("Failed Http Connected"));
		// try again ....
		if (false == m_kRequestList.empty())
		{
			CEL::SESSION_KEY kSessionKey(HttpConnector(), BM::GUID::NullData());
			g_kCoreCenter.Connect(m_kHttpAddress, kSessionKey);
		}
	}
}

void PgGalaServer::Locked_OnDisconnectFromHttp( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("Http Disconnected"));
	BM::GUID const rkReq = pkSession->CustomKey();

	ParsingLoginAnswer(rkReq);
}

void PgGalaServer::Locked_OnRecvFromHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
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

void PgGalaServer::SetAuthServer(std::wstring const &strAddress, WORD wPort)
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
	m_kHttpAddress.ip = pkSockAddress->sin_addr;
	m_kHttpAddress.wPort = wPort;
	m_strHttpHostname = strAddress;
}

void PgGalaServer::Locked_TestHttp()
{
	BM::CAutoMutex kLock(m_kMutex);

	CEL::SESSION_KEY kSessionKey(HttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kHttpAddress, kSessionKey);
}

void PgGalaServer::RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SAuthRequest kRecv;
	kRecv.ReadFromPacket(*pkPacket);
	if (kRecv.strID.length() <= 0 || kRecv.strPW.length() <= 0)
	{
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_WRONG_ID);
		return;
	}

	if (false == IsServiceReady())
	{
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_BLOCKED_SERVER_LOGIN);
		return;
	}

	short sSite;
	bool bSiteFind = PgSessionMgr::GetSessionKey(pkSession, sSite);
	SessionInfo* const pkSessionInfo = PgSessionMgr::GetSessionInfo(pkSession);
	if ( bSiteFind == false || pkSessionInfo == NULL )
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_WRONG_ID);
		return;
	}

	// 이미 접속중인가 검사하기
	if (PgSiteMemberMgr::IsHaveUserID(kRecv.strID))
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		CAUTION_LOG(BM::LOG_LV7, __FL__ << _T("Alread requested user ID=") << kRecv.strID);
		SendLoginResult(pkSession, SUserInfo(0, kRecv, LOCAL_MGR::NC_NOT_SET, GALA::EUSER_REQ_LOGIN), E_TLR_ALREADY_TRY);
		return;
	}

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
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
	pkNew->kLoginLog.push_back(::timeGetTime());	// step 1
#endif
	m_kRequestList.push_back(pkNew->kAuthInfo.kRequestGuid);
	CEL::SESSION_KEY kSessionKey(HttpConnector(), BM::GUID::NullData());
	g_kCoreCenter.Connect(m_kHttpAddress, kSessionKey);
}

void PgGalaServer::SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult)
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

std::wstring PgGalaServer::URLEncoding( std::wstring& kSource )
{
	std::wstring kOutput;

	for( std::wstring::const_iterator wstring_iter = kSource.begin(); wstring_iter != kSource.end(); ++wstring_iter )
	{
		const unsigned char ucSource = static_cast<unsigned char>(*wstring_iter);

		if( isalnum( ucSource ) || strchr( "-._~", ucSource ) )
		{
			kOutput += ucSource;
		}
		else
		{
			kOutput += L"%";
			kOutput += ConvertToHex( ucSource >> 4 );
			kOutput += ConvertToHex( ucSource );
		}
	}

	return kOutput;
}

bool PgGalaServer::SendLoginRequest(CEL::CSession_Base *pkSession)
{
	SUserInfo* pkUser = NULL;
	while (false == m_kRequestList.empty())
	{
		BM::GUID kReqGuid = m_kRequestList.back();
		m_kRequestList.pop_back();

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

#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
	pkUser->kLoginLog.push_back(::timeGetTime());	// step 2
#endif

	pkSession->CustomKey(pkUser->kAuthInfo.kRequestGuid);

	/* HTTP Request */
	//GET /Account/Authentication.Aspx?MODE=O&GAMECODE=G001&GPOTATOID=@NTEST4&PASSWORD=qordnjs&USERIP=211.232.162.178&BUFF1=825B49AC-44C0-48f4-89ED-49019905C74D HTTP/1.1\r\n
    //Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, */*\r\n
    //Accept-Language: ko\r\n
    //UA-CPU: x86\r\n
    //Accept-Encoding: gzip, deflate\r\n
    //User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2)\r\n
    //Host: spe.gpotato.eu\r\n
    //Connection: Keep-Alive\r\n
    //\r\n

	std::string strMsg = "POST /Account/Authentication.Aspx";
	BM::vstring vParam;
	std::string strNationCode;
	if ( false == GetGameCode(pkUser->eNationCode, strNationCode) )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown NationCode=") << static_cast<DWORD>(pkUser->eNationCode));
		return false;
	}
	std::string strReqCode;
	if ( false == GetRequestCode(pkUser->eReqType, strReqCode) )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown RequestCode=") << static_cast<DWORD>(pkUser->eReqType));
		return false;
	}

	std::wstring strID = URLEncoding( pkUser->kAuthInfo.strID );
	std::wstring strPW = URLEncoding( pkUser->kAuthInfo.strPW );	

	vParam << "MODE=" << strReqCode << "&GAMECODE=" << strNationCode << "&GPOTATOID=" << strID << "&PASSWORD=" << strPW << "&USERIP=" 
		<< pkUser->kAuthInfo.kAddress.IP() << "&BUFF1=" << pkUser->kAuthInfo.kRequestGuid;
	
	strMsg += " HTTP/1.1\r\nAccept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, */*\r\n";
	strMsg += "Accept-Language: ko\r\nUA-CPU: x86\r\nAccept-Encoding: gzip, deflate\r\n";
	strMsg += "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.2; Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2)\r\n";
	strMsg += "Host: ";
	strMsg += MB(m_strHttpHostname);
	strMsg += "\r\n";
	strMsg += "Content-type: application/x-www-form-urlencoded\r\n";
	strMsg += "Content-Length: ";
	strMsg += MB(BM::vstring(vParam.size()).operator std::wstring const&());
	strMsg += "\r\nConnection: close\r\n\r\n";
	strMsg += MB(vParam.operator std::wstring const&());

	BM::Stream kPacket;
	kPacket.Push(strMsg.c_str(), strMsg.length());
	pkSession->VSend(kPacket);
	return true;
}

void PgGalaServer::ParsingLoginAnswer(BM::GUID const& rkRequest)
{
	CONT_REQUEST::iterator itor_req = m_kUserByRequest.find(rkRequest);
	if (m_kUserByRequest.end() == itor_req)
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find request=") << rkRequest);
		return;
	}
	ETryLoginResult eResult = E_TLR_NONE;
	SUserInfo* const pkUser = (*itor_req).second;

#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
	pkUser->kLoginLog.push_back(::timeGetTime());	// Step 3
#endif

	int iErrorCode = 0;
	BM::GUID kTempReqGuid;
	int iSerialNumber = 0;

	std::list< std::string > kLineString;
	BM::vstring::CutTextByKey<std::string>(pkUser->strAnswer, "\r\n", kLineString, true);
	std::list< std::string >::const_iterator itor_list = kLineString.end();

	// Finding HTML Body
	size_t iHeaderLine = 0;
	itor_list = kLineString.begin();
	while (itor_list != kLineString.end())
	{
		if ( (*itor_list).length() <= 0 )
		{
			++iHeaderLine;
			++itor_list;
			break;
		}
		++iHeaderLine;
		++itor_list;
	}
	if (itor_list == kLineString.end())
	{
		eResult = E_TLR_PARSING_ERROR;
		goto ParsingLoginAnswer_SEND_RESULT;
	}
	if (kLineString.size() < LOGIN_ANSWER_MIN_LINE+iHeaderLine)
	{
		eResult = E_TLR_PARSING_ERROR;
		goto ParsingLoginAnswer_SEND_RESULT;
	}

	// Line 1 : Error Code
	if (S_OK != BM::vstring(*itor_list).GetNumber(std::wstring(_T("%d")), iErrorCode))
	{
		eResult = E_TLR_PARSING_ERROR;
		goto ParsingLoginAnswer_SEND_RESULT;
	}
	switch (iErrorCode)
	{
	case GALA::EGALA_ERROR_SUCESS:
		{
			eResult = E_TLR_SUCCESS;
		}break;
	case GALA::EGALA_ERROR_SYSTEM_ERROR:
		{
			eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		}break;
	case GALA::EGALA_ERROR_OUTOFSERICE:
		{
			eResult = E_TLR_EXTERNAL_SERVER_DOWN;
		}break;
	case GALA::EGALA_ERROR_BLOCKED_USER:
		{
			eResult = E_TLR_BLOCKED_USER;
		}break;
	default:
		{
			eResult = E_TLR_EXTERNAL_SERVER_ERROR;
		}break;
	}

	++itor_list;	// Line 2 : Error message
	pkUser->kAuthInfo.strErrorMsg = UNI((*itor_list).c_str());
	if (eResult != E_TLR_SUCCESS)
	{
		goto ParsingLoginAnswer_SEND_RESULT;
	}
	++itor_list;	// Line 3 : GPotatoID
	++itor_list;	// Line 4 : GPotatoNo
	if (S_OK != BM::vstring(*itor_list).GetNumber(std::wstring(_T("%d")), iSerialNumber))
	{
		eResult = E_TLR_PARSING_ERROR;
		goto ParsingLoginAnswer_SEND_RESULT;
	}
	pkUser->kAuthInfo.iUID = iSerialNumber;
	++itor_list;	// Line 5 : Nick name
	++itor_list;	// Line 6 : GM 여부
	if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	{
		pkUser->kAuthInfo.byGMLevel = 0xFF;	// DEBUG 모드 일때는 무조건 GM 레벨 준다.
	}
	else
	{
		pkUser->kAuthInfo.byGMLevel = (_stricmp((*itor_list).c_str(), "Y") == 0) ? 0xFF : 0;	// 최대 GM 권한
	}
	++itor_list;	// Line 7 : Encrypted String
	++itor_list;	// Line 8 : Buff1 (Request GUID)
	kTempReqGuid.Set(*itor_list);
	if (kTempReqGuid != pkUser->kAuthInfo.kRequestGuid)
	{
		eResult = E_TLR_PARSING_ERROR;
		goto ParsingLoginAnswer_SEND_RESULT;
	}

	{
		GALA::BILL_PACK_BALANCE kCashReq;
		SAFE_STRNCPY( kCashReq.UserID, pkUser->kAuthInfo.strID.c_str() );
		HRESULT hReturn = E_FAIL;
		if ((hReturn = RequestGTDX(kCashReq, PT_CN_IM_ANS_LOGIN_GALA, SERVER_IDENTITY_SITE(SERVER_IDENTITY(), 0))) != S_OK)
		{
			switch(hReturn)
			{
			case E_ALREADY_CREATED: {eResult = E_TLR_ALREADY_TRY;}break;
			case E_SYSTEM_ERROR: {eResult = E_TLR_EXTERNAL_SERVER_DOWN;}break;
			default: {eResult = E_TLR_FAILED;}break;
			}
			goto ParsingLoginAnswer_SEND_RESULT;
		}
	}

	// SUCCESS....
	return;

ParsingLoginAnswer_SEND_RESULT :
	SendLoginResult(NULL, *pkUser, eResult);
	/* Request information release */
	RemoveUserInfo(rkRequest);
}

void PgGalaServer::RemoveUserInfo(BM::GUID const& rkRequest)
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
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
		
#endif
		m_kUserByID.erase(pkUser->kAuthInfo.strID);
		delete pkUser;
	}
}

bool PgGalaServer::GetGameCode(LOCAL_MGR::NATION_CODE const eCode, std::string& rkOutCode)
{
	switch (eCode)
	{
	case LOCAL_MGR::NC_GERMANY:
		{
			rkOutCode = "G007";	// DE
		}break;
	case LOCAL_MGR::NC_FRANCE:
		{
			rkOutCode = "G008";	// FR
		}break;
	default:
		{
			// 나중에 English 서비스 지역 확정되면 default는 error 처리하도록 변경하자.
			rkOutCode = "G009";	// EN
		}break;
	}
	return true;
}

bool PgGalaServer::GetRequestCode(GALA::EUSER_REQ_TYPE const eReqCode, std::string& rkOutCode)
{
	switch (eReqCode)
	{
	case GALA::EUSER_REQ_LOGIN:
		{
			rkOutCode = "L";
		}break;
	case GALA::EUSER_REQ_LOGOUT:
		{
			rkOutCode = "O";
		}break;
	default:
		{
			rkOutCode = "";
			return false;
		}break;
	}
	return true;
}

void PgGalaServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);
	Timer5s();
}

void PgGalaServer::Timer5s()
{
	ACE_UINT64 const ui64Now = BM::GetTime64();

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

	{
		CONT_GTDX_REQUEST::iterator itor_gtdx = m_kGTDXRequest.begin();
		while (m_kGTDXRequest.end() != itor_gtdx)
		{
			if ((*itor_gtdx).second.ui64ExpireTime < ui64Now)
			{
				itor_gtdx = RemoveGTDXRequest<CONT_GTDX_REQUEST::iterator>(itor_gtdx);
			}
			else
			{
				++itor_gtdx;
			}
		}
	}

	PgSiteMemberMgr::Timer5s();
}

void PgGalaServer::RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	// 원래는 LOGOUT request 를 보내야 하는데
	// GALA와 협의해 보니, LOGOUT request 가 필요없다고 한다.
}

bool PgGalaServer::SendToGTDX( BM::Stream const &kPacket )const
{
	return PgSessionMgr::SendToServer(GTDX_SERVER_SITE_NUM, kPacket);
}

bool PgGalaServer::AddGTDXRequest(GALA::EUSER_REQ_TYPE const eType, std::wstring const& strID, DWORD const dwRequestKey, PACKET_ID_TYPE const wReqPacket, SERVER_IDENTITY_SITE const& kFromSI, 
	BM::Stream const * pkAddonPacket)
{
	auto ibRet = m_kGTDXRequest.insert(std::make_pair(strID, SGTDXRequest(strID, dwRequestKey, eType, wReqPacket, kFromSI, pkAddonPacket)));
	return ibRet.second;
}

void PgGalaServer::OnGTDXReceive_ServerState( BM::Stream &kPacket)
{
	GALA::BILL_PACK_ISALIVE kRecv;
	kPacket.Pop(kRecv);
}

template<typename T_PACKET_STRUCT>
void PgGalaServer::OnGTDXReceive( BM::Stream &kPacket)
{
	T_PACKET_STRUCT kRecv;
	if (false == kPacket.Pop(kRecv))
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot pop RequestType=") << typeid(T_PACKET_STRUCT).name());
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Cannot pop RequestType=") << typeid(T_PACKET_STRUCT).name());
		return;
	}

	INFO_LOG(BM::LOG_LV6, __FL__ << kRecv.ToVString());

	// packet 검사하기
	CONT_GTDX_REQUEST::iterator itor_gtdx = m_kGTDXRequest.find(kRecv.UserID);
	if (m_kGTDXRequest.end() == itor_gtdx)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find Request UserID=") << kRecv.UserID << _T(", Type=") << typeid(T_PACKET_STRUCT).name());
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find Request UserID=") << kRecv.UserID << _T(", Type=") << typeid(T_PACKET_STRUCT).name());
		return;
	}

	if ((*itor_gtdx).second.dwRequestKey != kRecv.kHeader.ReqKey)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("RequestKey mismatch PacketKey=") << kRecv.kHeader.ReqKey << _T(", SavedKey=") << (*itor_gtdx).second.dwRequestKey);
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("RequestKey mismatch PacketKey=") << kRecv.kHeader.ReqKey << _T(", SavedKey=") << (*itor_gtdx).second.dwRequestKey);
		return;
	}

	if ((*itor_gtdx).second.eReqType != kRecv.kHeader.ReqType)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Request type mismatch PacketType=") << kRecv.kHeader.ReqType << _T(", SavedType=") << static_cast<int>((*itor_gtdx).second.eReqType));
		CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Request type mismatch PacketType=") << kRecv.kHeader.ReqType << _T(", SavedType=") << static_cast<int>((*itor_gtdx).second.eReqType));
		return;
	}

	SendGTDXResult((*itor_gtdx).second, kRecv);
	RemoveGTDXRequest<CONT_GTDX_REQUEST::iterator>(itor_gtdx);
}

template<typename T_RETURN, typename T_ERASE_KEY>
T_RETURN PgGalaServer::RemoveGTDXRequest(T_ERASE_KEY const& kKey)
{
	return m_kGTDXRequest.erase(kKey);
}

void PgGalaServer::SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BALANCE const &rkResult)
{
	switch (rkReq.wReqPacketType)
	{
	case PT_CN_IM_ANS_LOGIN_GALA:
		{
			CONT_ID::iterator itor_id = m_kUserByID.find(rkReq.strID);
			if (m_kUserByID.end() == itor_id)
			{
				INFO_LOG(BM::LOG_LV7, __FL__ << _T("Cannot find UseriD=") << rkReq.strID);
				return;
			}
			SUserInfo* pkUserInfo = (*itor_id).second;
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
			pkUserInfo->kLoginLog.push_back(::timeGetTime());
#endif
			ETryLoginResult eResult = E_TLR_EXTERNAL_SERVER_ERROR;
			switch (rkResult.RetCode)
			{
			case GALA::EGTDX_ERROR_SUCCESS:
				{
					eResult = E_TLR_SUCCESS;
				}break;
			case GALA::EGTDX_ERROR_NOT_EXIST:
				{
					eResult = E_TLR_WRONG_ID;
				}break;
			//case GALA::EGTDX_ERROR_CASH_SHORTAGE:
			//case GALA::EGTDX_ERROR_CANNOT_BUY:
			//case GALA::EGTDX_ERROR_NOT_EXIST:
			//case GALA::EGTDX_ERROR_BILLINGSERVER_ERROR:
			//case GALA::EGTDX_ERROR_ALREADY_CANCEL:
			default:
				{
					eResult = E_TLR_EXTERNAL_SERVER_ERROR;
				}break;
			};

			if (eResult == E_TLR_SUCCESS)
			{
				pkUserInfo->i64Cash = rkResult.CashBalance;
				if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
				{
					pkUserInfo->kAuthInfo.iUID = rkResult.UserNo;
				}
			}
			SendLoginResult(NULL, *pkUserInfo, eResult);
			RemoveUserInfo(pkUserInfo->kAuthInfo.kRequestGuid);
		}break;
	case PT_CN_A_ANS_QUERY_CASH:
		{
			BM::Stream kDPacket(PT_CN_A_ANS_QUERY_CASH);
			kDPacket.Push(rkResult.RetCode);
			kDPacket.Push(rkReq.strID);
			kDPacket.Push(rkResult.CashBalance);
			kDPacket.Push(0);	// Mileage
			//rkReq.kAddonPacket.PosAdjust();
			kDPacket.Push(rkReq.kAddonPacket);
			SendCashResult(rkReq.kFromSI, kDPacket);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown packet type = ") << rkReq.wReqPacketType);
		};break;
	}
}

void PgGalaServer::SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BUY const &rkResult)
{
	switch (rkReq.wReqPacketType)
	{
	case PT_CN_A_ANS_BUYCASHITEM:
	case PT_CN_A_ANS_EXPANDTIEM:
	case PT_CN_A_ANS_SENDGIFT:
		{
			BM::Stream kHPacket(rkReq.wReqPacketType);
			kHPacket.Push(rkResult.RetCode);
			kHPacket.Push(BM::GUID(rkResult.Character_id));
			kHPacket.Push(rkReq.strID);
			kHPacket.Push(rkResult.CashBalance);	// current cash
			kHPacket.Push(0);		// current mileage
			kHPacket.Push(0);		// Added Mileage
			std::wstring strChargeNo = UNI(rkResult.ChargeNo);
			kHPacket.Push(strChargeNo);
			kHPacket.Push(rkReq.kAddonPacket);
			SendCashResult(rkReq.kFromSI, kHPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unknown PacketType=") << rkReq.wReqPacketType);
		}break;
	}
}

void PgGalaServer::SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_GIFT const &rkResult)
{
	switch (rkReq.wReqPacketType)
	{
	case PT_CN_A_ANS_SENDGIFT:
		{
			BM::Stream kHPacket(rkReq.wReqPacketType);
			kHPacket.Push(rkResult.RetCode);
			kHPacket.Push(BM::GUID(rkResult.Character_id));
			kHPacket.Push(rkReq.strID);
			kHPacket.Push(rkResult.CashBalance);	// current cash
			kHPacket.Push(0);		// current mileage
			kHPacket.Push(0);		// Added Mileage
			std::wstring strChargeNo = UNI(rkResult.ChargeNo);
			kHPacket.Push(strChargeNo);
			kHPacket.Push(rkReq.kAddonPacket);
			SendCashResult(rkReq.kFromSI, kHPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("unknown PacketType=") << rkReq.wReqPacketType);
		}break;
	}
}

void PgGalaServer::SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BUY_CNL const &rkResult)
{
	BM::Stream kIPacket(rkReq.wReqPacketType);
	kIPacket.Push(rkResult.RetCode);
	kIPacket.Push(rkResult.CashBalance);
	kIPacket.Push(0);	// current mileage
	kIPacket.Push(0);	// added mileage
	kIPacket.Push(rkReq.kAddonPacket);
	SendCashResult(rkReq.kFromSI, kIPacket);
}

void PgGalaServer::Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration(pkSession, usType, pkPacket);
}


template<typename T_PACK_STRUCT>
HRESULT PgGalaServer::RequestGTDX(T_PACK_STRUCT& kInfo, PACKET_ID_TYPE const wReqPacket, SERVER_IDENTITY_SITE const& kFromSI,
	BM::Stream const* pkAddonPacket)
{
	INFO_LOG(BM::LOG_LV6, __FL__ << kInfo.ToVString());
	kInfo.kHeader.ReqKey = GetGTDXRequestKey();
	if (false == AddGTDXRequest(static_cast<GALA::EUSER_REQ_TYPE>(kInfo.kHeader.ReqType), kInfo.UserID, kInfo.kHeader.ReqKey, wReqPacket, kFromSI, pkAddonPacket))
	{
		return E_ALREADY_CREATED;
	}

	BM::Stream kAPacket;
	kAPacket.Push(kInfo);
	if (false == SendToGTDX(kAPacket))
	{
		return E_SYSTEM_ERROR;
	}
	return S_OK;
}

void PgGalaServer::RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	DWORD dwUID;
	CEL::ADDR_INFO kAddress;
	pkPacket->Pop(kAccountID);
	pkPacket->Pop(dwUID);
	kAddress.ReadFromPacket(*pkPacket);

	GALA::BILL_PACK_BALANCE kBill;
	SAFE_STRNCPY(kBill.UserID, kAccountID.c_str());
	kBill.UserNo = dwUID;
	HRESULT hReturn = S_OK;
	size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
	if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_QUERY_CASH, rkFrom, pkPacket)))
	{
		kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
		if (pkPacket != NULL)
		{
			pkPacket->RdPos(szRdPos);
		}
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_QUERY_CASH, rkFrom, pkPacket), kBill);
		return;
	}
}

GALA::EGTDX_ERROR_CODE PgGalaServer::GetErrorCode(HRESULT const hResult)
{
	GALA::EGTDX_ERROR_CODE eError = GALA::EGTDX_ERROR_UNKNOWN;
	switch(hResult)
	{
	case E_ALREADY_CREATED: { return GALA::EGTDX_ERROR_DUPLICATE_REQUEST; } break;
	case E_SYSTEM_ERROR: { return GALA::EGTDX_ERROR_SYSTEM_ERROR; } break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Error code =") << hResult);
		}break;
	}
	return eError;
}

bool PgGalaServer::SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket)
{
	BM::Stream kFPacket(PT_X_N_ANS_CASH);
	dynamic_cast<tagServerIdentity const&>(rkFromSI).WriteToPacket(kFPacket);
	kFPacket.Push(rkPacket);
	return SendToServer(rkFromSI.nSiteNo, kFPacket);
}

void PgGalaServer::RecvPT_A_CN_REQ_BUYCASHITEM(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
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

	GALA::BILL_PACK_BUY kBill;
	kBill.UserNo = dwUID;
	SAFE_STRNCPY(kBill.UserID, kAccountID.c_str());
	GetGTDXNationCode(eNation, kBill.LangCode, sizeof(kBill.LangCode));
	SAFE_STRNCPY(kBill.ClientIP, kAddress.IP());
	kBill.Server_index = rkFrom.nSiteNo;
	kBill.Character_id = kCharacterGuid;
	SAFE_STRNCPY(kBill.Character_name, kCharacterName.c_str());

	// GTDX가 여러개의 아이템에 대해서 처리 하지 못하고 있다..
	// 임시로 첫번째 아이템에 대한 정보만 입력
	if (kBuyItem.empty())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Empty BuyItem Container"));
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_BUYCASHITEM, rkFrom, pkPacket), kBill);
		return;
	}

	SBuyItemInfo const& rkItem = kBuyItem.at(0);
	kBill.Item_code = rkItem.iItemNo;
	SAFE_STRNCPY(kBill.Item_name, rkItem.strItemName.c_str());
	kBill.Item_term = static_cast<WORD>(rkItem.iItemHour);
	kBill.Item_price = static_cast<DWORD>(-kCost.i64UsedCash);	// 소모캐시량을 양수로 보내야 한다.
	kBill.CashBalance = 0;

	if (0 != kCost.i64UsedMileage || 0 != kCost.i64AddedMileage)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not support MileageUsage or AddMileage Cost=") << kCost.ToString());
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_BUYCASHITEM, rkFrom, pkPacket), kBill);
		return;
	}

	HRESULT hReturn = S_OK;
	size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
	if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_BUYCASHITEM, rkFrom, pkPacket)))
	{
		kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
		if (pkPacket != NULL)
		{
			pkPacket->RdPos(szRdPos);
		}
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_BUYCASHITEM, rkFrom, pkPacket), kBill);
		return;
	}

}

bool PgGalaServer::GetGTDXNationCode(LOCAL_MGR::NATION_CODE const eCode, LPTSTR lpszCode, size_t iCodeSize)
{
	if (lpszCode == NULL || iCodeSize < 3)
	{
		return false;
	}

	switch (eCode)
	{
	case LOCAL_MGR::NC_GERMANY:
		{
			_tcscpy_s(lpszCode, iCodeSize, _T("de"));	// DE
		}break;
	case LOCAL_MGR::NC_FRANCE:
		{
			_tcscpy_s(lpszCode, iCodeSize, _T("fr"));
		}break;
	case LOCAL_MGR::NC_ENGLAND:
	default:
		{
			// 나중에 English 서비스 지역 확정되면 default는 error 처리하도록 변경하자.
			_tcscpy_s(lpszCode, iCodeSize, _T("uk"));
		}break;
	}
	return true;
}

void PgGalaServer::RecvPT_A_CN_REQ_EXPANDTIEM(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
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

	GALA::BILL_PACK_BUY kBill;
	kBill.UserNo = dwUID;
	SAFE_STRNCPY(kBill.UserID, kAccountID.c_str());
	GetGTDXNationCode(eNation, kBill.LangCode, sizeof(kBill.LangCode));
	SAFE_STRNCPY(kBill.ClientIP, kAddress.IP());
	kBill.Server_index = rkFrom.nSiteNo;
	kBill.Character_id = kCharacterGuid;
	SAFE_STRNCPY(kBill.Character_name, kCharacterName.c_str());

	// GTDX가 여러개의 아이템에 대해서 처리 하지 못하고 있다..
	// 임시로 첫번째 아이템에 대한 정보만 입력
	if (kBuyItem.empty())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Empty BuyItem Container"));
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_EXPANDTIEM, rkFrom, pkPacket), kBill);
		return;
	}

	SBuyItemInfo const& rkItem = kBuyItem.at(0);
	kBill.Item_code = rkItem.iItemNo;
	SAFE_STRNCPY(kBill.Item_name, rkItem.strItemName.c_str());
	kBill.Item_term = static_cast<WORD>(rkItem.iItemHour);
	kBill.Item_price = static_cast<DWORD>(-kCost.i64UsedCash);
	kBill.CashBalance = 0;

	if (0 != kCost.i64UsedMileage || 0 != kCost.i64AddedMileage)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not support MileageUsage or AddMileage Cost=") << kCost.ToString());
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_EXPANDTIEM, rkFrom, pkPacket), kBill);
		return;
	}

	HRESULT hReturn = S_OK;
	size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
	if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_EXPANDTIEM, rkFrom, pkPacket)))
	{
		kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
		if (pkPacket != NULL)
		{
			pkPacket->RdPos(szRdPos);
		}
		SendGTDXResult(SGTDXRequest(kAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_EXPANDTIEM, rkFrom, pkPacket), kBill);
		return;
	}

}

void PgGalaServer::RecvPT_A_CN_REQ_SENDGIFT(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
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

	int iValue = 0;
	bool bUseBuyItem;
	if( S_OK != g_kVariableContainer.Get(EVAR_KIND_GALA_CASH, EVAR_CN_GALACASH_GIFT2BUYITEM, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot read EVAR_CN_GALACASH_GIFT2BUYITEM value"));
	}
	else
	{
		bUseBuyItem = (iValue==1) ? true : false;
	}

	if (bUseBuyItem)
	{
		GALA::BILL_PACK_BUY kBill;
		kBill.UserNo = dwUID;
		SAFE_STRNCPY(kBill.UserID, strAccountID.c_str());
		GetGTDXNationCode(eNation, kBill.LangCode, sizeof(kBill.LangCode));
		SAFE_STRNCPY(kBill.ClientIP, kAddress.IP());
		kBill.Server_index = rkFrom.nSiteNo;
		kBill.Character_id = kCharacterGuid;
		SAFE_STRNCPY(kBill.Character_name, strCharacterName.c_str());

		kBill.Item_code = dwItemNo;
		SAFE_STRNCPY(kBill.Item_name, strItemName.c_str());
		kBill.Item_term = wItemTime;
		kBill.Item_price = static_cast<DWORD>(-kCost.i64UsedCash);	// 비용이 양수로 입력되어야 한다.
		kBill.CashBalance = 0;

		if (0 != kCost.i64UsedMileage || 0 != kCost.i64AddedMileage)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not support MileageUsage or AddMileage Cost=") << kCost.ToString());
			SendGTDXResult(SGTDXRequest(strAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket), kBill);
			return;
		}

		HRESULT hReturn = S_OK;
		size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
		if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket)))
		{
			kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
			if (pkPacket != NULL)
			{
				pkPacket->RdPos(szRdPos);
			}
			SendGTDXResult(SGTDXRequest(strAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket), kBill);
			return;
		}
	}
	else
	{
		GALA::BILL_PACK_GIFT kBill;
		kBill.UserNo = dwUID;
		SAFE_STRNCPY(kBill.UserID, strAccountID.c_str());
		GetGTDXNationCode(eNation, kBill.LangCode, sizeof(kBill.LangCode));
		SAFE_STRNCPY(kBill.ClientIP, kAddress.IP());
		kBill.Server_index = rkFrom.nSiteNo;
		kBill.Character_id = kCharacterGuid;
		SAFE_STRNCPY(kBill.Character_name, strCharacterName.c_str());
		kBill.Friend_id = BM::GUID::NullData();
		SAFE_STRNCPY(kBill.Friend_name, strReceiverName.c_str());
		kBill.Item_code = dwItemNo;
		SAFE_STRNCPY(kBill.Item_name, strItemName);
		kBill.Item_term = static_cast<WORD>(wItemTime);
		kBill.Item_price = static_cast<DWORD>(-kCost.i64UsedCash); // 비용이 양수로 입력되어야 한다.
		kBill.CashBalance = 0;

		if (0 != kCost.i64UsedMileage || 0 != kCost.i64AddedMileage)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Not support MileageUsage or AddMileage Cost=") << kCost.ToString());
			SendGTDXResult(SGTDXRequest(strAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket), kBill);
			return;
		}

		HRESULT hReturn = S_OK;
		size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
		if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket)))
		{
			kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
			if (pkPacket != NULL)
			{
				pkPacket->RdPos(szRdPos);
			}
			SendGTDXResult(SGTDXRequest(strAccountID, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_SENDGIFT, rkFrom, pkPacket), kBill);
			return;
		}
	}
}

void PgGalaServer::RecvPT_A_CN_REQ_BUYCASHITEM_ROLLBACK(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	DWORD dwUID;
	std::wstring strName;
	std::wstring strChargeNo;

	pkPacket->Pop(dwUID);
	pkPacket->Pop(strName);
	pkPacket->Pop(strChargeNo);

	GALA::BILL_PACK_BUY_CNL kBill;
	kBill.UserNo = dwUID;
	SAFE_STRNCPY(kBill.UserID, strName.c_str());
	SAFE_STRNCPY(kBill.ChargeNo, MB(strChargeNo.c_str()));

	HRESULT hReturn = S_OK;
	size_t const szRdPos = (pkPacket != NULL) ? pkPacket->RdPos() : 0;
	if (S_OK != (hReturn = RequestGTDX(kBill, PT_CN_A_ANS_BUYCASHITEM_ROLLBACK, rkFrom, pkPacket)))
	{
		kBill.RetCode = static_cast<GALA::EGTDX_ERROR_CODE>(GetErrorCode(hReturn));
		if (pkPacket != NULL)
		{
			pkPacket->RdPos(szRdPos);
		}
		SendGTDXResult(SGTDXRequest(strName, 0, static_cast<GALA::EUSER_REQ_TYPE>(kBill.kHeader.ReqType), PT_CN_A_ANS_BUYCASHITEM_ROLLBACK, rkFrom, pkPacket), kBill);
		return;
	}
}

