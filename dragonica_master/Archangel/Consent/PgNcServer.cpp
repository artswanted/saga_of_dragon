#include "stdafx.h"
#include "PgNcServer.h"
#include "PgNcGSM.h"

namespace NCMonUtil
{
	bool bUseMonitor = true;
}

using namespace NC;

PgNcServer::PgNcServer()
:m_lCashRequestKey(0)
{
	PointCategory(0);
	
	CashId(0);
	HappyCoinId(0);
	UnitCoinId(0);
	
	ItemCount(0);
}

PgNcServer::~PgNcServer()
{
	m_kContUserByUserId.clear();

	CONT_USER_BY_GUSID::iterator loop_iter = m_kContUserByGusId.begin();
	while( m_kContUserByGusId.end() != loop_iter )
	{
		if( NULL != (*loop_iter).second )
		{
			delete (*loop_iter).second;
		}
		++loop_iter;
	}

	m_kContUserByGusId.clear();
	m_kContServerStateStr.clear();
}

unsigned long PgNcServer::GetCashRequestKey()
{
	return ::InterlockedIncrement(&m_lCashRequestKey);
}

void CALLBACK PgNcServer::OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{	
	g_kNc.Locked_OnConnectFromImmigration(pkSession);
}

void CALLBACK PgNcServer::OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{	
	g_kNc.Locked_OnDisconnectFromImmigration(pkSession);
}

void CALLBACK PgNcServer::OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kNc.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void CALLBACK PgNcServer::OnConnectFromSA( CEL::CSession_Base *pkSession )
{	
	g_kNc.Locked_OnConnectFromSA(pkSession);
}

void CALLBACK PgNcServer::OnDisconnectFromSA( CEL::CSession_Base *pkSession )
{
	g_kNc.Locked_OnDisconnectFromSA(pkSession);
}

void CALLBACK PgNcServer::OnRecvFromSA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kNc.Locked_OnRecvFromSA( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgNcServer::Locked_OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);

	if( OnConnected(pkSession) )
	{// Imm 연결 된 후에 SA Connector 등록
		CEL::INIT_CORE_DESC kSAConnectorInit;
		kSAConnectorInit.OnSessionOpen	= PgNcServer::OnConnectFromSA;
		kSAConnectorInit.OnDisconnect	= PgNcServer::OnDisconnectFromSA;
		kSAConnectorInit.OnRecv			= PgNcServer::OnRecvFromSA;
		kSAConnectorInit.kOrderGuid.Generate();
		kSAConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
		kSAConnectorInit.ServiceHandlerType( CEL::SHT_NC );
		g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kSAConnectorInit );
	}
}

void PgNcServer::Locked_OnConnectFromSA( CEL::CSession_Base *pkSession )
{	
	BM::CAutoMutex kLock(m_kMutex);
	
	if( OnConnected(pkSession) )
	{// 초기 패킷 보내야 함.
		DWORD dwVersion = 0; // dummy값

		std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_NC_Config.ini"));
		TCHAR wszIP[MAX_PATH] = {0,};
		
		GameID(GetPrivateProfileInt(_T("ID_INFO"), _T("GAME_ID"), 0, wstrFilename.c_str()));
		ServerID(GetPrivateProfileInt(_T("ID_INFO"), _T("SERVER_ID"), 0, wstrFilename.c_str()));		
		PointCategory(GetPrivateProfileInt(_T("POINT"), _T("CATEGORY"), 0, wstrFilename.c_str()));		
		CashId(GetPrivateProfileInt(_T("POINT"), _T("CASH_ID"), 0, wstrFilename.c_str()));
		HappyCoinId(GetPrivateProfileInt(_T("POINT"), _T("HAPPYCOIN_ID"), 0, wstrFilename.c_str()));
		UnitCoinId(GetPrivateProfileInt(_T("POINT"), _T("UNITCOIN_ID"), 0, wstrFilename.c_str()));
		
		ItemCount(1); // 1로 고정
		
		BM::Stream kPacket(IBI_PROTOCOL_VER);
		kPacket.Push(dwVersion);
		kPacket.Push(GameID());
		kPacket.Push(ServerID());
		pkSession->VSend(kPacket);
	}
}

void PgNcServer::Locked_OnDisconnectFromSA( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected(pkSession);
}

bool PgNcServer::Locked_OnRegistConnector(CEL::SRegistResult const &rkArg)
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename(g_kProcessCfg.ConfigDir() + _T("Consent_NC_Config.ini"));
	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	case CEL::ST_EXTERNAL1:
		{
			TCHAR wszIP[MAX_PATH] = {0,};
			int nPort;
			GetPrivateProfileString(_T("SA"), _T("IP"), _T("0.0.0.0"), wszIP, sizeof(wszIP), wstrFilename.c_str());
			nPort = GetPrivateProfileInt(_T("SA"), _T("PORT"), 0, wstrFilename.c_str());
			AddAddress(SA_SERVER_SITE_NUM, rkArg.guidObj, wszIP, static_cast<WORD>(nPort));
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}
	return true;
}

void PgNcServer::Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration(pkSession, usType, pkPacket);
}

void PgNcServer::RecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	switch( usType )
	{
	case PT_IM_CN_REQ_LOGIN_AUTH_NC:
		{
			RecvPT_IM_CN_REQ_LOGIN_AUTH_NC(pkSession, pkPacket);
		}break;
	case PT_IM_CN_NFY_LOGOUT_NC:
		{
			RecvPT_IM_CN_NFY_LOGOUT_NC(pkSession, pkPacket);
		}break;
	case PT_N_X_REQ_CASH:
		{
			SERVER_IDENTITY kFromSI;
			kFromSI.ReadFromPacket(*pkPacket);

			BM::Stream::DEF_STREAM_TYPE wSubType;
			pkPacket->Pop(wSubType);
			SERVER_IDENTITY_SITE kSiteSI(kFromSI, GetSessionKey(pkSession));

			INFO_LOG(BM::LOG_LV7, __FL__ << _T("Recv from Contents Server [PT_N_X_REQ_CASH] -> subtype: ") << wSubType);

			switch(wSubType)
			{
			case PT_A_CN_REQ_QUERY_CASH:
				{// 캐시 및 마일리지 조회
					RecvPT_A_CN_REQ_QUERY_CASH(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_BUYCASHITEM:
			case PT_A_CN_REQ_EXPANDTIEM: // 기간연장도 아이템 구매와 같음.
				{
					RecvPT_A_CN_REQ_BUYCASHITEM(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_SENDGIFT:
				{
					RecvPT_A_CN_REQ_SENDGIFT(kSiteSI, pkPacket);
				}break;
			case PT_A_CN_REQ_DELETE_ITEMS:
				{
					RecvPT_A_CN_REQ_DELETE_ITEM(kSiteSI, pkPacket);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown type [PT_N_X_REQ_CASH]->") << wSubType);
				}break;
			}
		}break;
	default:
		{
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	}
}

void PgNcServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);
	Timer5s();
}

void PgNcServer::Timer5s()
{
	PgSiteMemberMgr::Timer5s();
}

void PgNcServer::RecvPT_IM_CN_NFY_LOGOUT_NC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{// 로그아웃 처리
	BYTE byGusId[GUSID_LENGTH] = {0,};
	pkPacket->PopMemory(byGusId, sizeof(byGusId));

	CONT_USER_BY_GUSID::iterator find_iter_byGusId = m_kContUserByGusId.find(std::string(reinterpret_cast<char*>(byGusId)));
	if( m_kContUserByGusId.end() != find_iter_byGusId )
	{
		CONT_USER_BY_GUSID::mapped_type kElement = (*find_iter_byGusId).second;

		g_kGSM.m_pFuncLogOut(byGusId);
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Logout user's ID=") << kElement->kAuthInfo.strID);
		
		m_kContUserByUserId.erase(kElement->kAuthInfo.iUID);
		m_kContUserByGusId.erase(find_iter_byGusId);

		delete kElement;
	}
}

void PgNcServer::RecvPT_IM_CN_REQ_LOGIN_AUTH_NC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SAuthRequest kRecv;
	std::wstring kSesskey;

	kRecv.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kSesskey);

	bool bWebLinkAge = true;
	if( kSesskey.empty() )
	{// 웹세션키가 없다면 ID/PW 로그인 한것.
		bWebLinkAge = false;
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

	// GSM 서버에 인증 요청
	SServerTryLogin kRecvInfo;
	BYTE abyGusID[GUSID_LENGTH] = {0,};
	BYTE byGMLevel = 0;
	int iOutRet = 0;

	if( bWebLinkAge )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("TryAuthNcLoginWithWeb SessKey=[") << kSesskey << _T("]"));
		g_kGSM.TryAuthNcLoginWithWeb(iOutRet, kRecvInfo, kSesskey, ::inet_addr( MB( kRecv.kAddress.IP() ) ), abyGusID, byGMLevel );
	}
	else
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("TryAuthNcLoginWithID AccountID=[") << kRecv.strID << _T("]"));
		g_kGSM.TryAuthNcLoginWithID(iOutRet, kRecvInfo, kRecv.strID, kRecv.strPW, ::inet_addr( MB( kRecv.kAddress.IP() ) ), abyGusID, byGMLevel);
	}
	INFO_LOG(BM::LOG_LV1, __FL__ << L"GSM Complete");

	if(iOutRet > 0)
	{
		wchar_t szErrBuf[1024] = {0,};
		int MsgLen = 0;
		g_kGSM.m_pFuncGetErrorMsg(iOutRet, sizeof(szErrBuf), szErrBuf, MsgLen);
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("GSM Error: ") << szErrBuf << _T(", SessKey: ") << kSesskey );

		//gudID값이 0이 아닌데 에러난거면 로그아웃 시켜야한다.
		if( (abyGusID[0] != 0)
			&& (kRecvInfo.iUID != 0) )
		{
			g_kGSM.m_pFuncLogOut(abyGusID);
		}
		
		kRecv.strErrorMsg = szErrBuf;
		SendLoginResult(pkSession, SUserInfo(0,kRecv), static_cast<ETryLoginResult>(E_TLR_NC_ERROR_BASE+iOutRet));
		return;
	}

	// PCCafe Check
	int pPCCafeCode = 0;
	int pPCCafeGrade = 0;
	g_kGSM.m_pFuncCheckPCCafeIP( abyGusID, ::inet_addr( MB( kRecv.kAddress.IP() ) ), &pPCCafeCode, &pPCCafeGrade );
	if( pPCCafeCode > 0 )
	{// PCCafe에서 접속 한 것.
		kRecv.bIsPCCafe = true;
	}

	kRecv.iUID = kRecvInfo.iUID;
	kRecv.strID = kRecvInfo.ID();
	kRecv.strPW = kRecvInfo.PW();
	kRecv.byGMLevel = byGMLevel;

	SUserInfo* pkNew = new SUserInfo(sSite, kRecv);
	if(pkNew == NULL)
	{
		INFO_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		CAUTION_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_MEMORY_ERROR);
		delete pkNew;
		return;
	}
	
	auto pairGusIdResult = m_kContUserByGusId.insert(std::make_pair(std::string(reinterpret_cast<char*>(abyGusID)), pkNew));
	if( pairGusIdResult.second )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << L"m_kContUserByGusId.insert success");
		auto pairUserIdResult = m_kContUserByUserId.insert(std::make_pair(kRecv.iUID, pkNew));
		if( pairUserIdResult.second )
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"m_kContUserByUserId.insert success");
			SendLoginResult(pkSession, *pkNew, E_TLR_SUCCESS, abyGusID );			
		}
		else
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"m_kContUserByUserId.insert failed");
			m_kContUserByGusId.erase(pairGusIdResult.first);
			SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_MEMORY_ERROR);
			delete pkNew;
			return ;
		}
	}
	else
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << L"m_kContUserByGusId.insert failed");
		SendLoginResult(pkSession, SUserInfo(0, kRecv), E_TLR_MEMORY_ERROR);
		delete pkNew;
		return ;
	}
}

void PgNcServer::SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult, BYTE* pbyGusID)
{
	BYTE byGusId[16] = {0, };
	if( NULL != pbyGusID )
	{
		::memcpy( byGusId, pbyGusID, sizeof(byGusId));
	}

	SAuthRequest kAuth = kUser.kAuthInfo;

 	if(eResult != E_TLR_SUCCESS)
 	{
 		INFO_LOG(BM::LOG_LV5, __FL__ << _T("UserID=") << kUser.kAuthInfo.strID << _T(", ErrorCode=") << eResult);
 	}
	else
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("UserID=") << kUser.kAuthInfo.strID << _T(", GSM auth success"));
	}

	BM::Stream kXPacket(PT_CN_IM_ANS_LOGIN_AUTH_NC);
	kAuth.WriteToPacket(kXPacket);
	kXPacket.Push(eResult);
	kXPacket.Push(byGusId, sizeof(byGusId));
	
	if (pkSession != NULL)
	{
		pkSession->VSend(kXPacket);
	}
	else
	{
		PgSessionMgr::SendPacket(kUser.sSite, kXPacket);
	}
}

void PgNcServer::SendNotifyUserKick(BYTE* pbyGusId, int iUID, BYTE byReason)
{
	if(!pbyGusId)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("gusId NULL, UID = [") << iUID << "]");
		return ;
	}

	CONT_USER_BY_GUSID::iterator find_iter_byGusId = m_kContUserByGusId.find(std::string(reinterpret_cast<char*>(pbyGusId)));
	if( m_kContUserByGusId.end() != find_iter_byGusId )
	{
		CONT_USER_BY_GUSID::mapped_type kElement = (*find_iter_byGusId).second;

		const bool bDisConnected = true; // GSM에서 이미 끊어졌다.
		BM::Stream kDisConnectPacket( PT_CN_IM_NFY_USER_TERMINATE, static_cast<EClientDisconnectedCause>(CDC_GM_GSMKick) );
		kDisConnectPacket.Push(kElement->kAuthInfo.strID);
		kDisConnectPacket.Push( bDisConnected );
		kDisConnectPacket.Push( E_TLR_NC_ERROR_BASE+byReason );
		PgSessionMgr::SendPacket(kElement->sSite, kDisConnectPacket);

		CONT_USER_BY_USERID::iterator find_iter_byUserId = m_kContUserByUserId.find(kElement->kAuthInfo.iUID);
		if( m_kContUserByUserId.end() != find_iter_byUserId )
		{
			m_kContUserByUserId.erase(find_iter_byUserId);
		}

		delete kElement;
		m_kContUserByGusId.erase(find_iter_byGusId);
	}
}

void PgNcServer::SendCashResult( BM::Stream& rkPacket )
{
	unsigned int uiTid = 0;
	rkPacket.Pop( uiTid );

	CONT_CASH_REQ_KEY::iterator find_iter = m_kContNcCashReqkey.find( static_cast<unsigned long>( uiTid ) );
	if( m_kContNcCashReqkey.end() != find_iter )
	{
		NC::SNcCashTranInfo kNcCashTranInfo = (*find_iter).second;

		BM::Stream kPacket(PT_X_N_ANS_CASH);
		dynamic_cast<tagServerIdentity const&>(kNcCashTranInfo.kSI).WriteToPacket(kPacket);
		kPacket.Push(kNcCashTranInfo.wSubType);
		kPacket.Push(kNcCashTranInfo.kCost);

		kPacket.Push(rkPacket);
		kPacket.Push(kNcCashTranInfo.OrgPacket);
		SendToServer(kNcCashTranInfo.kSI.nSiteNo, kPacket);

		m_kContNcCashReqkey.erase(find_iter);
	}
}

void PgNcServer::Locked_OnRecvFromSA( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(usType)
	{
	case IBO_PROTOCOL_VER://SA버전정보
		{
			DWORD dwSAVersion = 0;
			pkPacket->Pop(dwSAVersion);
			INFO_LOG(BM::LOG_LV6, __FL__ << L"SA Version:" << dwSAVersion);
			
			{// SA와 연결이 끊어져 있는 동안 반영된(웹 구매) 아이템 리스트 요청
				const unsigned long ulTid = GetCashRequestKey();
				SNcCashTranInfo kTemp;
				m_kContNcCashReqkey.insert(std::make_pair(ulTid, kTemp));

				BM::Stream kPacket(IBI_GET_ITEMS_FOR_SERVER_REQ);
				kPacket.Push(ulTid);
				pkSession->VSend(kPacket);
			}
		}break;
	case IBO_GET_ITEMS_FOR_SERVER_ACK:
		{
		}break;
	case IBO_GET_ITEMS_ACK:
		{
		}break;
	case IBO_POINT_CHANGED_NOTI:
		{
			unsigned int uiUserId = 0;
			unsigned short usPointCategory = 0;
			unsigned short usPointId = 0;
			unsigned __int64 uiPointAmount = 0;
			unsigned __int64 uiPointDifference = 0;

			pkPacket->Pop(uiUserId);
			pkPacket->Pop(usPointCategory);
			pkPacket->Pop(usPointId);
			pkPacket->Pop(uiPointAmount);
			pkPacket->Pop(uiPointDifference);

			INFO_LOG(BM::LOG_LV7, __FL__ << _T("Recv IBO_POINT_CHANGED_NOTI UID: ") << uiUserId << _T(", Point ID: ") << usPointId << _T(", Point Amount: ") << uiPointAmount);

			CONT_USER_BY_USERID::iterator find_iter = m_kContUserByUserId.find(uiUserId);
			if( m_kContUserByUserId.end() != find_iter )
			{
				CONT_USER_BY_USERID::mapped_type kElement = (*find_iter).second;

				BM::Stream kPacket(PT_CN_A_NFY_CASH_NC);
				kPacket.Push(kElement->kAuthInfo.strID);
				kPacket.Push(usPointId);// 포인트 구분 id
				kPacket.Push(uiPointAmount); // 캐시 또는 해피포인트 남은 금액
				kPacket.Push(uiPointDifference); // 변화한 금액
				SendToServer(kElement->sSite, kPacket);
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("can't find user UID: ") << uiUserId);
			}
		}break;
	case IBO_BUY_ITEM_ACK:	// 아이템 구매 결과
	case IBO_GIFT_ITEM_ACK:	// 아이템 선물 결과
	case IBO_GET_POINT_ACK:	// 조회한 게정의 잔여 캐시
		{
			SendCashResult(*pkPacket);
		}break;
	case IBO_NEW_ITEM_DELIVERED_NOTI:
		{// 웹 to 게임 아이템 지급( 현재 SA 프로토콜로 구현 불가 )
			/*BM::Stream kPacket( PT_CN_A_NFY_ITEM );
			kPacket.Push( *pkPacket );
			Locked_SendToImmigrationOne( kPacket );*/
		}break;
	case IBO_DELETE_ITEMS_ACK:
		{// SA에서 최종 처리가 완료 되었다.
			unsigned int uiTid = 0;
			unsigned int uiResult = 0;
			pkPacket->Pop( uiTid );
			pkPacket->Pop( uiResult );
			if( uiResult != NC::SARC_SUCCESS ) 
			{// 이미 아이템 지급까지 다 끝나서 커밋을 보냈는데, 여기서 에러나면.. 로그 찍어주는 거 말고 처리방법이 없다.
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("SA Error: ") << uiResult);
			}
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown packet = ") << usType);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unknown packet = ") << usType);
		}break;
	}
}

void PgNcServer::RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	unsigned int uiUserId;
	CEL::ADDR_INFO kAddress;

	pkPacket->Pop(kAccountID);
	pkPacket->Pop(uiUserId);
	kAddress.ReadFromPacket(*pkPacket);

	// 결과를 보낼때 필요한 정보 세팅
	const unsigned long ulTid = GetCashRequestKey();
	SNcCashTranInfo kNcCashTranInfo(rkFrom, PT_CN_A_ANS_QUERY_CASH, CASH::SCashCost(0,0,0), pkPacket );	
	m_kContNcCashReqkey.insert(std::make_pair(ulTid, kNcCashTranInfo));

	BM::Stream kPacket(IBI_GET_POINT_REQ);
	kPacket.Push(ulTid);
	kPacket.Push(uiUserId);
	kPacket.Push(2);// 캐시, 해피코인
	kPacket.Push(PointCategory());
	kPacket.Push(CashId());
	kPacket.Push(PointCategory());
	kPacket.Push(HappyCoinId());

	if( !SendToServer(SA_SERVER_SITE_NUM, kPacket) )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Failed Send to SA."));

		BM::Stream kPacket;
		kPacket.Push( ulTid );
		kPacket.Push( NC::SARC_SA_ERROR );
		SendCashResult( kPacket );
	}
}

void PgNcServer::RecvPT_A_CN_REQ_BUYCASHITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	std::wstring kAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring kCharacterName;
	CONT_BUYITEM kBuyItem;
	unsigned int uiUserId;
	CASH::SCashCost kCost;

	pkPacket->Pop(kAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(kCharacterName);
	pkPacket->Pop(uiUserId);
	kCost.ReadFromPacket(*pkPacket); // 아이템이 여러개일 경우 합산된 가격임.
	PU::TLoadArray_M(*pkPacket, kBuyItem); // 현재는 무조건 1개.
	
	// SA가 여러개의 아이템에 대해서 처리 하지 못하고 있다.
	// 관련 프로토콜 제공되기 전 까지 장바구니 사용 안한다.
	const long ulTid = GetCashRequestKey();
	SNcCashTranInfo kNcCashTranInfo( rkFrom, PT_CN_A_ANS_BUYCASHITEM, kCost, pkPacket );
	m_kContNcCashReqkey.insert( std::make_pair( ulTid, kNcCashTranInfo ) );

	CONT_BUYITEM::iterator item_iter = kBuyItem.begin();
	while( kBuyItem.end() != item_iter )
	{
		CONT_BUYITEM::value_type const kElement = (*item_iter);
		
		BM::Stream kPacket(IBI_BUY_ITEM_REQ);
		kPacket.Push(static_cast<unsigned int>(ulTid));
		kPacket.Push(uiUserId);
		kPacket.Push(0);// CharacterId 사용하지 않을경우 0 세팅
		kPacket.Push(static_cast<unsigned int>( kElement.iItemNo ) );
		kPacket.Push(ItemCount());// 개별 아이템 개수는 1개
		kPacket.Push(1); // 통합포인트 1개만 사용
		kPacket.Push(PointCategory());// 3: AP System
		kPacket.Push(UnitCoinId());// 15: 통합포인트
		kPacket.Push(kElement.i64Cash);// 개별 아이템 가격
		kPacket.Push(kCharacterName.c_str());

		if( !SendToServer( SA_SERVER_SITE_NUM, kPacket ) )
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << _T("Failed Send to SA."));

			BM::Stream kPacket;
			kPacket.Push( ulTid );
			kPacket.Push( NC::SARC_SA_ERROR );
			SendCashResult( kPacket );
			return ;
		}

		++item_iter;
	}
}

void PgNcServer::RecvPT_A_CN_REQ_DELETE_ITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{	
	unsigned __int64 i64WarehouseNo = 0;	
	pkPacket->Pop(i64WarehouseNo);

	// 결과를 보낼때 필요한 정보 세팅
	const unsigned long ulTid = GetCashRequestKey();
	SNcCashTranInfo kNcCashTranInfo(rkFrom, PT_A_CN_ANS_DELETE_ITEMS, CASH::SCashCost(0,0,0), pkPacket );
	m_kContNcCashReqkey.insert(std::make_pair(ulTid, kNcCashTranInfo));

	BM::Stream kPacket(IBI_DELETE_ITEMS_REQ);
	kPacket.Push(ulTid);
	kPacket.Push(ItemCount());
	kPacket.Push(i64WarehouseNo);

	if( !SendToServer(SA_SERVER_SITE_NUM, kPacket) )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Failed Send to SA."));

		BM::Stream kPacket;
		kPacket.Push( ulTid );
		kPacket.Push( NC::SARC_SA_ERROR );		
		SendCashResult( kPacket );
	}
}

void PgNcServer::RecvPT_A_CN_REQ_SENDGIFT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket)
{
	unsigned int uiUserId;
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring strCharacterName;
	std::wstring strReceiverName;
	unsigned int uiItemId;
	std::wstring strItemName;
	WORD wItemTime;
	CASH::SCashCost kCost;
	unsigned int uiReceiverUserId;

	pkPacket->Pop(uiUserId);
	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(strCharacterName);
	pkPacket->Pop(strReceiverName);
	pkPacket->Pop(uiItemId);
	pkPacket->Pop(strItemName);
	pkPacket->Pop(wItemTime);
	kCost.ReadFromPacket(*pkPacket);
	pkPacket->Pop(uiReceiverUserId);

	// 결과를 보낼때 필요한 정보 세팅
	const unsigned long ulTid = GetCashRequestKey();
	SNcCashTranInfo kNcCashTranInfo(rkFrom, PT_CN_A_ANS_SENDGIFT, kCost, pkPacket ); // 내부적으로 선물 처리
	m_kContNcCashReqkey.insert(std::make_pair(ulTid, kNcCashTranInfo));
	
	BM::Stream kPacket(IBI_GIFT_ITEM_REQ);
	kPacket.Push(ulTid);
	kPacket.Push(uiUserId);
	kPacket.Push(0); // 캐릭터번호 사용안함
	kPacket.Push(uiReceiverUserId); // 선물 받는 유저의 UID
	kPacket.Push(ServerID()); // 선물 받는 유저의 서버id
	kPacket.Push(0); // 캐릭터번호 사용안함
	kPacket.Push(uiItemId);
	kPacket.Push(ItemCount());
	kPacket.Push(1);
	kPacket.Push(PointCategory());
	kPacket.Push(CashId()); // 선물은 캐시로만 가능(해피포인트 사용 불가)
	kPacket.Push(-kCost.i64UsedCash);
	kPacket.Push(strCharacterName.c_str());
	kPacket.Push(strReceiverName.c_str());

	if( !SendToServer( SA_SERVER_SITE_NUM, kPacket ) )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << _T("Failed Send to SA."));

		BM::Stream kPacket;
		kPacket.Push( ulTid );
		kPacket.Push( NC::SARC_SA_ERROR );		
		SendCashResult( kPacket );
	}
}

void PgNcServer::Locked_SetServerState(BM::Stream * const pkPacket)
{
	int iRealmNo = 0;
	BM::vstring vstrTemp;

	if( pkPacket->Pop( iRealmNo )
	&&	pkPacket->Pop( vstrTemp ) )
	{
		BM::CAutoMutex kLock(m_kMutexMonitor);

		CONT_SERVERSTATE_STR::iterator serverStateStr_iter = m_kContServerStateStr.find( iRealmNo );
		if( serverStateStr_iter != m_kContServerStateStr.end() )
		{
			CONT_SERVERSTATE_STR::mapped_type& vStrServerState = serverStateStr_iter->second;
			vStrServerState = vstrTemp;
		}
		else
		{
			m_kContServerStateStr.insert( std::make_pair( iRealmNo, vstrTemp ) );
		}
	}
}

void PgNcServer::Locked_GetServerState( BM::vstring &rkOut, const int iRealmNo )
{
	BM::CAutoMutex kLock(m_kMutexMonitor);

	CONT_SERVERSTATE_STR::iterator serverStateStr_iter = m_kContServerStateStr.find( iRealmNo );
	if( serverStateStr_iter != m_kContServerStateStr.end() )
	{
		CONT_SERVERSTATE_STR::mapped_type vStrServerState = serverStateStr_iter->second;
		rkOut = vStrServerState;
	}
}

bool PgNcServer::InitAdminGateWay()
{
	if( !Init(PgNcServer::Excute) )
	{
		return false;
	}
	return true;
}

void PgNcServer::UnloadAdminGateWay()
{
	Unload();
}

void PgNcServer::Excute(const wchar_old* req, wchar_old* res)
{
	if( !NCMonUtil::bUseMonitor )
	{
		return;
	}

	wchar_t * pRes = reinterpret_cast<wchar_t*>(res);
	wchar_t const* pReq = reinterpret_cast<wchar_t const*>(req);

	std::wstring wstrReq(pReq);
	typedef	std::list<std::wstring>	CUT_STRING;
	CUT_STRING	kContResultStr;	
	BM::vstring::CutTextByKey<std::wstring>(wstrReq, L",", kContResultStr);

	if( 2 == kContResultStr.size() )
	{
		CUT_STRING::iterator string_iter = kContResultStr.begin();
		if( string_iter != kContResultStr.end() )
		{
			std::wstring wstrPacketType = (*string_iter);	++string_iter;
			std::wstring wstrRealmNo = (*string_iter);		++string_iter;

			if( wstrPacketType == L"11001" )
			{
				BM::vstring vStrAnswer;
				g_kNc.Locked_GetServerState( vStrAnswer, _wtof( wstrRealmNo.c_str() ) );
				if( vStrAnswer.size() )
				{
					wcscpy(pRes, UNI(vStrAnswer));
					return ;
				}
			}
		}
	}

	// 실패
	wcscpy(pRes,L"11003");
}
			