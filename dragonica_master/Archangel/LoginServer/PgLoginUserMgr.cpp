#include "stdafx.h"
#include "Variant/Global.h"
#include "PgLoginUserMgr.h"
#include "PgPatchVersionManager.h"

//
PgLoginUserMgr::PgLoginUserMgr()
{
}

PgLoginUserMgr::~PgLoginUserMgr()
{
}

bool PgLoginUserMgr::InsertAuth( CEL::CSession_Base const * pkSession, SAuthInfo &kAuthInfo )
{
	kAuthInfo.addrRemote = pkSession->Addr();
	kAuthInfo.kLoginServer = g_kProcessCfg.ServerIdentity();
	kAuthInfo.SessionKey( pkSession->SessionKey() );
	kAuthInfo.eLoginState = ELogin_ReqAuth;

	auto ret = m_kContTryLoginUser.insert( std::make_pair( pkSession->SessionKey(), kAuthInfo ) );
	return ret.second;
}

eTryLoginResult PgLoginUserMgr::ProcessAuth_Common( SClientTryLogin &kCTL, CEL::CSession_Base* pkSession, BM::Stream const *pkAddPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if( NULL == pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"User ID[" << kCTL.ID() << L"], Null Point Session");
		return E_TLR_FAILED;
	}

	INFO_LOG(BM::LOG_LV7, __FL__ << L"User ID[" << kCTL.ID() << L"], "<<C2L(pkSession->SessionKey()));

	SIpRange kOut;
	if( IsBlockAddr( pkSession->Addr().ip, kOut ) )
	{
		if(!IsAcceptAddr( pkSession->Addr().ip, kOut ))//예외 걸렸음.
		{
			INFO_LOG(BM::LOG_LV6, __FL__ << L"BlockIP[" << C2L(pkSession->Addr()) << L"] : User[" << kCTL.ID() << L"]" );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return E_TLR_BLOCKED_IP;
		}
	}

	kCTL.MakeCorrect();
	pkSession->m_kSessionData.Clear();
	kCTL.WriteToPacket(pkSession->m_kSessionData);
	
	if(!g_kPatchVersionMgr.CheckVersion(kCTL.PatchVersion()))
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << L"Wrong Patch Version : User[" << kCTL.ID() << L"-" << C2L(pkSession->Addr()) << L"], ReqVersion[" << kCTL.PatchVersion() << L"], ServerVersion[" << g_kPatchVersionMgr.GetPatchVersion() << L"]");
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_TLR_WRONG_PATCH_VERSION;
	}

	if( !CProcessConfig::IsCorrectVersion( kCTL.szVersion1, true ) )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << L"Wrong Version : User[" << kCTL.ID() << L"-" << C2L(pkSession->Addr()) << L"], ReqVersion[" << kCTL.szVersion1 << L"], ServerVersion[" << PACKET_VERSION_C << L"]");
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_TLR_WRONG_VERSION;
	}

	SAuthInfo kAuthInfo(kCTL);
	if ( true == InsertAuth( pkSession, kAuthInfo ) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << L"ID[" << kCTL.ID() << L"] -> Auth Succes IP[" << C2L(pkSession->Addr()) << L"]");

		BM::Stream kPacket(PT_L_IM_TRY_AUTH);
		static_cast<SServerTryLogin>(kAuthInfo).WriteToPacket(kPacket);
		kPacket.Push( *pkAddPacket );
		if( !SendToImmigration(kPacket) )
		{
			return E_TRL_CENTER_ERROR;//센터 없음
		}
	}
	else
	{//같은 세션에서 여러번 요청이 오게 되었다.
		INFO_LOG(BM::LOG_LV5, __FL__ << L"ID[" << kCTL.ID() << L"] -> Overlap Auth IP[" << C2L(pkSession->Addr()) << L"]");
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_TLR_ALREADY_TRY;
	}

	return E_TLR_SUCCESS;
}

HRESULT PgLoginUserMgr::ProcessAuth(SClientTryLogin const &kInCTL, CEL::CSession_Base* pkSession, std::wstring const &kAddonString)
{
	BM::CAutoMutex kLock(m_kMutex);

	SClientTryLogin kCTL = kInCTL;

	BM::Stream kPacket;
	kPacket.Push(kAddonString);
	
	eTryLoginResult eRet = ProcessAuth_Common( kCTL, pkSession, &kPacket);
	if( E_TLR_SUCCESS != eRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, eRet );
		kPacket.Push(std::wstring());
		kPacket.SetStopSignal(true);	
		pkSession->VSend(kPacket);
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT PgLoginUserMgr::ProcessAuth_NC(SClientTryLogin const &kInCTL, CEL::CSession_Base* pkSession, std::wstring const &kSessKey)
{
	BM::CAutoMutex kLock(m_kMutex);

	SClientTryLogin kCTL = kInCTL;

	if( g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_KOREA_NC_SERVICE) )
	{
		static int i = 0;//-> NC 대책이 없어서.
		if(i > 10000000)
		{
			i = 0;
		}

		if( kCTL.ID().empty() )
		{//웹세션키로 로그인 하는 경우에는 아이디가 날라오지 않는다.
		 //겹치지 않게 임시값을 넣어준다.
			kCTL.SetID(BM::vstring(i++));
		}
	}	
	
	BM::Stream kAddPacket;
	kAddPacket.Push( kSessKey );

	eTryLoginResult eRet = ProcessAuth_Common( kCTL, pkSession, &kAddPacket );
	if( E_TLR_SUCCESS != eRet )
	{
		BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, eRet );
		kPacket.Push(std::wstring());
		kPacket.SetStopSignal(true);	
		pkSession->VSend(kPacket);
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT PgLoginUserMgr::ProcessAutoAuth( BM::GUID const &kAuthKey, CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);

	eTryLoginResult eRet = E_TLR_FAILED;
	if( NULL == pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL, Auth:"<<kAuthKey);
		return E_TLR_FAILED;
	}

	CONT_WAIT_AUTOLOGIN_USER::iterator itr = m_kContWaitAutoLogin.find( kAuthKey );
	if ( itr != m_kContWaitAutoLogin.end() )
	{
		if ( pkSession->Addr().ip.S_un.S_addr == itr->second.kRSRM.addrRemote.ip.S_un.S_addr )
		{
			pkSession->m_kSessionData.Clear();
			static_cast<SClientTryLogin>(itr->second.kRSRM).WriteToPacket(pkSession->m_kSessionData);

			SAuthInfo kAuthInfo;
			itr->second.kRSRM.CopyTo( kAuthInfo );
			if ( true == InsertAuth( pkSession, kAuthInfo ) )
			{
				BM::Stream kPacket( PT_L_IM_TRY_AUTH_AUTO);
				kAuthInfo.WriteToPacket(kPacket);

				if( true == ::SendToImmigration(kPacket) )
				{
					eRet = E_TLR_SUCCESS;
				}
				else
				{
					eRet = E_TRL_CENTER_ERROR;//센터 없음
				}

				m_kContWaitAutoLogin.erase( itr );
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Error Different IP<") << C2L(pkSession->Addr()) << _T("> WaitIP<") << C2L(itr->second.kRSRM.addrRemote) << _T(">") );
		}
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Not Found SwitchAuthKey<") << kAuthKey << _T("> IP<") << C2L(pkSession->Addr()) << _T(">") );
	}

	if ( E_TLR_SUCCESS != eRet )
	{
		BM::Stream kPacket( PT_L_C_TRY_LOGIN_RESULT, eRet );
		kPacket.Push(std::wstring());
		kPacket.SetStopSignal(true);	
		pkSession->VSend(kPacket);
		return E_FAIL;
	}
	return S_OK;
}

bool PgLoginUserMgr::SetLoginState( CONT_LOGIN_USER::key_type const &kSessionKey, ELoginState const eLoginState, bool const bNewLogin )
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_LOGIN_USER::iterator user_itor = m_kContTryLoginUser.find(kSessionKey);
	if(user_itor != m_kContTryLoginUser.end())
	{
		user_itor->second.eLoginState = eLoginState;
		user_itor->second.bNewLogin = bNewLogin;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgLoginUserMgr::ProcessLogin( SClientTryLogin const& rkCTL, CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL, "<<C2L(rkCTL));
		return E_TLR_SUCCESS;
	}

	INFO_LOG(BM::LOG_LV7, __FL__ << L"Try User ID[" << rkCTL.ID() << L"], "<<C2L(pkSession->SessionKey()));

	ETryLoginResult eResult = E_TLR_SUCCESS;

	CONT_LOGIN_USER::iterator user_itor = m_kContTryLoginUser.find(pkSession->SessionKey());
	if(user_itor == m_kContTryLoginUser.end())
	{
		eResult = E_TLR_INCORRECT_LOGIN_SERVER;
		goto __FAILED;
	}
	else
	{
		if (user_itor->second.eLoginState >= ELogin_SendReqImm)
		{
			// 중복 REQ 검사 : Channel선택 Button 계속 클릭하면, 이쪽으로 두번이상 들어 올 수 있다.
			// 이미 요청 보낸 유저 -> 그냥 무시해야 한다. 접속 끊으면, 접속 성공 메시지 와도 못 받게 됨.
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;
		}

		if(!g_kPatchVersionMgr.CheckVersion(rkCTL.PatchVersion()))
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << L"Wrong Patch Version : User[" << rkCTL.ID() << L"-" << C2L(pkSession->Addr()) << L"], ReqVersion[" << rkCTL.PatchVersion() << L"], ServerVersion[" << g_kPatchVersionMgr.GetPatchVersion() << L"]");
			eResult = E_TLR_WRONG_PATCH_VERSION;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;
		}

		if(!CProcessConfig::IsCorrectVersion( rkCTL.szVersion1, true ) )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << L"Wrong Version : User[" << rkCTL.ID() << L"-" << C2L(pkSession->Addr()) << L"], ReqVersion[" << rkCTL.szVersion1 << L"], ServerVersion[" << PACKET_VERSION_C << L"]" );
			eResult = E_TLR_WRONG_VERSION;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;
		}

		if(PgRealm::EREALM_STATE_NONE == static_cast<PgRealm::EREALM_STATE>(g_kRealmMgr.GetRealmState(rkCTL.RealmNo())))
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << L"Not open Realm : User[" << rkCTL.ID() << L"-" << C2L(pkSession->Addr()) << L"], Realm[" << rkCTL.RealmNo() << L"]" );
			eResult = E_TRL_WRONG_CHANNEL;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;
		}

		INFO_LOG(BM::LOG_LV7, __FL__ << rkCTL.ID() << L" ID -> Try Succes IP[" << C2L(pkSession->Addr()) << L"]" );

		(*user_itor).second.RealmNo(rkCTL.RealmNo());
		(*user_itor).second.ChannelNo(rkCTL.ChannelNo());
		(*user_itor).second.eLoginState = ELogin_SendReqImm;
		
		SAuthInfo const &rkElement = (*user_itor).second;

		BM::Stream Packet(PT_L_IM_TRY_LOGIN);
		rkElement.WriteToPacket(Packet);
		bool const bCanSend = SendToImmigration(Packet);
		if(!bCanSend)
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << L"User Send IncorrectChannel[" << rkCTL.ChannelNo() << L"]" );
			eResult = E_TRL_WRONG_CHANNEL;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;
		}
		return true;
	}

__FAILED:
	{
		BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, eResult );
		kPacket.Push(std::wstring());
		kPacket.SetStopSignal(true);
		pkSession->VSend(kPacket);
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return false;
	}
}

bool PgLoginUserMgr::Clear( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_LOGIN_USER::iterator login_itor = m_kContTryLoginUser.find(pkSession->SessionKey());

	if(login_itor != m_kContTryLoginUser.end())
	{//지워진게 있음.
		BM::Stream kPacket(PT_L_IM_WAITING_USER_CLEAR);
		kPacket.Push(pkSession->SessionKey());
		kPacket.Push(login_itor->second.kMemberGuid);
		kPacket.Push(login_itor->second.eLoginState); // NC때문에 추가
		SendToImmigration(kPacket);

		m_kContTryLoginUser.erase(login_itor);//지워줌.
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

size_t PgLoginUserMgr::GetCounter()const
{
	BM::CAutoMutex kLock(m_kMutex);
	return m_kContTryLoginUser.size();	
}

HRESULT PgLoginUserMgr::UpdateAuthInfo(SAuthInfo const &kAuthInfo)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_LOGIN_USER::iterator user_itor = m_kContTryLoginUser.find(kAuthInfo.SessionKey());
	if(m_kContTryLoginUser.end() != user_itor)
	{
		(*user_itor).second = kAuthInfo;
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return E_FAIL;
}


HRESULT PgLoginUserMgr::GmWaitLogin(SAuthInfo  &kGM, CEL::CSession_Base* pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, BM::vstring(L"Session is NULL, ")<<C2L(kGM));
		return E_FAIL;
	}

	//패킷 버전에 상관없이 로그인 되도록
/*
	if( !CProcessConfig::IsCorrectVersion( kGM.szVersion ) )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << L"Wrong Version : User[" << kGM.szID << L"-" << pkSession->Addr() << L"], ReqVersion[" << kGM.szVersion << L"], ServerVersion[" << PACKET_VERSION << L"]" );
		return E_FAIL;
	}
*/
	SAuthInfo *pkGmInfo = new SAuthInfo;
	*pkGmInfo = kGM;

	auto kPair = m_kContGmWaitLogin.insert(std::make_pair(kGM.ID(), pkGmInfo));
	if(!kPair.second)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << L"Regist Yet ID[" << kGM.ID() << L"], Session[" << C2L(pkSession->SessionKey()) << L"]");
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		SAFE_DELETE( pkGmInfo );
		return E_FAIL;
	}

	SAuthInfo *pkAuthInfo = kPair.first->second;

	pkAuthInfo->eLoginState = ELogin_ReqAuth;
	pkAuthInfo->SessionKey(pkSession->SessionKey());
	pkAuthInfo->addrRemote	= pkSession->Addr();
	pkAuthInfo->kLoginServer	= g_kProcessCfg.ServerIdentity();
	pkAuthInfo->guidQuery.Generate();

	BM::Stream kPacket(PT_O_G_REQ_GMCOMMAND, EGMC_MATCH_GMID);
	pkAuthInfo->WriteToPacket(kPacket);
	if(!SendToGM(kPacket))
	{
		CONT_GM_AUTH_DATA_ID::iterator Itr = m_kContGmWaitLogin.find(pkAuthInfo->ID());
		if(Itr != m_kContGmWaitLogin.end())
		{
			m_kContGmWaitLogin.erase(Itr);
		}

		BM::Stream kErrPacket(PT_G_O_ANS_SERVER_ERROR);
		kErrPacket.Push(CEL::ST_GMSERVER);
		pkSession->VSend(kErrPacket);

		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_FAIL;
	}

	return S_OK;
}

void PgLoginUserMgr::ProcessGmLogin(BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kPacket(PT_L_O_ANS_TRY_GM_LOGIN);
	int iErrorCode;
	SAuthInfo kGmInfo;
	pkPacket->Pop(iErrorCode);
	kGmInfo.ReadFromPacket(*pkPacket);

	if(iErrorCode == GE_SUCCESS)
	{
		CONT_GM_AUTH_DATA::mapped_type pkData = new SAuthInfo;
		*pkData = kGmInfo;
		auto kPair =  m_kContGmLogined.insert(std::make_pair(kGmInfo.kMemberGuid, pkData));
		if(kPair.second)
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"[%s] Regist Success GmId: " << kGmInfo.ID());
		}
		else
		{
			SAFE_DELETE(pkData);
		}
	}
	g_kLoginUserMgr.DeleteGmWaitData(kGmInfo.ID());

	kPacket.Push(iErrorCode);
	kGmInfo.WriteToPacket(kPacket);
	kPacket.Push(*pkPacket);
	g_kCoreCenter.Send(kGmInfo.SessionKey(), kPacket);

}

void PgLoginUserMgr::ClearGmUserData()
{
	BM::CAutoMutex kLock(m_kMutex);


	BM::Stream kPacket(PT_G_O_ANS_SERVER_ERROR);
	kPacket.Push(CEL::ST_GMSERVER);
	//로그인 대기중인 애들꺼
	{
		CONT_GM_AUTH_DATA_ID::iterator Itr = m_kContGmWaitLogin.begin();
		while(Itr != m_kContGmWaitLogin.end())
		{
			//GM 유저 리스트를 지우라고 전한다.
			CONT_GM_AUTH_DATA_ID::mapped_type pkData = Itr->second;
			g_kCoreCenter.Send(pkData->SessionKey(), kPacket);						
			SAFE_DELETE(pkData);

			m_kContGmWaitLogin.erase(Itr++);
		}
//		m_kContGmWaitLogin.clear();
	}

	//로그인 완료된 애들꺼
	{
		CONT_GM_AUTH_DATA::iterator Itr = m_kContGmLogined.begin();
		while(Itr != m_kContGmLogined.end())
		{
			CONT_GM_AUTH_DATA::mapped_type pkData = Itr->second;
			g_kCoreCenter.Send(pkData->SessionKey(), kPacket);
			SAFE_DELETE(pkData);

			m_kContGmLogined.erase(Itr++);
		}
//		m_kContGmLogined.clear();
	}
}

bool PgLoginUserMgr::DeleteGmWaitData(std::wstring const &rkID)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA_ID::iterator Itr = m_kContGmWaitLogin.find(rkID);
	if(Itr != m_kContGmWaitLogin.end())
	{
		CONT_GM_AUTH_DATA_ID::mapped_type pkData = Itr->second;
		m_kContGmWaitLogin.erase(Itr->first);
		SAFE_DELETE(pkData);

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgLoginUserMgr::DeleteGmWaitData(CEL::CSession_Base const * pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA_ID::iterator Itr = m_kContGmWaitLogin.begin();
	while(Itr != m_kContGmWaitLogin.end())
	{
		CONT_GM_AUTH_DATA_ID::mapped_type pkData = Itr->second;
		
		if(pkData->SessionKey() == pkSession->SessionKey())
		{
			BM::GUID const &kMemberGuid = pkData->kMemberGuid;

			BM::Stream kPacket(PT_O_G_REQ_GMCOMMAND, EGMC_DELETE_GM);
			kPacket.Push(kMemberGuid);
			SendToGM(kPacket);
			
			SAFE_DELETE(pkData);
			m_kContGmWaitLogin.erase(Itr++);
			return true;
		}
		++Itr;
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgLoginUserMgr::DeleteGmLoinedData(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA::iterator Itr = m_kContGmLogined.find(rkGuid);
	if(Itr != m_kContGmLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type pkData = Itr->second;
		
		BM::Stream kPacket(PT_O_G_REQ_GMCOMMAND, EGMC_DELETE_GM);
		kPacket.Push(rkGuid);
		SendToGM(kPacket);

		SAFE_DELETE(pkData);
		m_kContGmLogined.erase(Itr);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgLoginUserMgr::DeleteGmLoinedData(CEL::CSession_Base const * pkSession)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA::iterator Itr = m_kContGmLogined.begin();
	while(Itr != m_kContGmLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type pkData = Itr->second;
		
		if(pkData->SessionKey() == pkSession->SessionKey())
		{
			BM::GUID const &kMemberGuid = pkData->kMemberGuid;
		
			BM::Stream kPacket(PT_O_G_REQ_GMCOMMAND, EGMC_DELETE_GM);
			kPacket.Push(kMemberGuid);
			SendToGM(kPacket);

			SAFE_DELETE(pkData);
			m_kContGmLogined.erase(Itr++);
			return true;
		}
		++Itr;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgLoginUserMgr::SendToGmUser(BM::GUID kMemberId, BM::Stream const&rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA::iterator Itr = m_kContGmLogined.find(kMemberId);
	if(Itr != m_kContGmLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type kElement = Itr->second;
		return g_kCoreCenter.Send(kElement->SessionKey(), rkPacket);
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

void PgLoginUserMgr::DisplayState()const
{
	BM::CAutoMutex kLock( m_kMutex );

	U_STATE_LOG(BM::LOG_LV0, _T("============= Wait User State ============"));
	U_STATE_LOG(BM::LOG_LV0, L"UserCount[" << m_kContTryLoginUser.size() << L"]");

	CONT_LOGIN_USER::const_iterator login_itor = m_kContTryLoginUser.begin();
	while(login_itor != m_kContTryLoginUser.end())
	{

		++login_itor;
	}
	U_STATE_LOG(BM::LOG_LV0, _T("============= Cut Line ============"));

}

BM::GUID PgLoginUserMgr::InsertAutoLoginWait( SWaitAutoLogin const &kWAL )
{
	BM::CAutoMutex kLock(m_kMutex);

	auto kPair = m_kContWaitAutoLogin.insert( std::make_pair( BM::GUID::Create(), kWAL ) );
	if ( true == kPair.second )
	{
		kPair.first->second.kRSRM.bNewLogin = false;
		kPair.first->second.dwRegistTime = BM::GetTime32();
		return kPair.first->first;
	}
	return BM::GUID::NullData();
}

void PgLoginUserMgr::Tick()
{
	BM::CAutoMutex kLock( m_kMutex );

	DWORD const dwCurTime = BM::GetTime32();

	CONT_WAIT_AUTOLOGIN_USER::iterator itr = m_kContWaitAutoLogin.begin();
	while ( itr != m_kContWaitAutoLogin.end() )
	{
		if ( ::DifftimeGetTime( itr->second.dwRegistTime, dwCurTime ) > 10000 )
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Clear WaitAutoLogin SwitchAuthKey<") << itr->first << _T("> IP<") << C2L(itr->second.kRSRM.addrRemote) << _T(">"));
			itr = m_kContWaitAutoLogin.erase( itr );
		}
		else
		{
			++itr;
		}
	}
}