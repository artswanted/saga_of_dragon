#include "StdAfx.h"
#include "PgGMUserMgr.h"
#include "Lohengrin/PgRealmManager.h"


// - GM User Mgr ->
PgGMUserMgr::PgGMUserMgr(void)
{
}

PgGMUserMgr::~PgGMUserMgr(void)
{
	Clear();
}

void PgGMUserMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA_ID::iterator Itr = m_kGmUserWaitLogin.begin();
	while(Itr != m_kGmUserWaitLogin.end())
	{
		CONT_GM_AUTH_DATA_ID::mapped_type pData = Itr->second;
		SAFE_DELETE(pData);
		m_kGmUserWaitLogin.erase(Itr++);
	}

	CONT_GM_AUTH_DATA::iterator Auth_Itr = m_kGMUserLogined.begin();
	while(Auth_Itr != m_kGMUserLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type pData = Auth_Itr->second;
		SAFE_DELETE(pData);
		m_kGMUserLogined.erase(Auth_Itr++);
	}	
}

void PgGMUserMgr::DeleteLoginedGM(BM::Stream *pkPacket)
{

	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kMemberId;
	pkPacket->Pop(kMemberId);

	CONT_GM_AUTH_DATA::iterator Auth_Itr = m_kGMUserLogined.find(kMemberId);
	if(Auth_Itr != m_kGMUserLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type pData = Auth_Itr->second;
		SAFE_DELETE(pData);
		m_kGMUserLogined.erase(Auth_Itr);
	}	
}

bool PgGMUserMgr::WaitLogin(BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);

	SAuthInfo kGmAuth;
	kGmAuth.ReadFromPacket(*pkPacket);

	CONT_GM_AUTH_DATA_ID::mapped_type pData= new SAuthInfo;
	*pData = kGmAuth;

	auto kPair = m_kGmUserWaitLogin.insert(std::make_pair(kGmAuth.ID(), pData));
	if(!kPair.second)	
	{
		return false;
	}


	CEL::DB_QUERY kQuery(DT_GM, DQT_TRY_GMLOGIN, L"EXEC [dbo].[up_Common_Admin_GetLogin_Select]" );	
	kQuery.QueryGuid(kGmAuth.guidQuery);
	kQuery.PushStrParam(kGmAuth.ID());
	kQuery.PushStrParam(kGmAuth.PW());
	kQuery.PushStrParam(kGmAuth.addrRemote.IP());
	kQuery.PushStrParam(std::wstring(L""));

	g_kCoreCenter.PushQuery(kQuery, true);

	return true;
}

bool PgGMUserMgr::TryAuth(int iErr, SAuthInfo const &kLogin)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kPacket(PT_L_O_ANS_TRY_GM_LOGIN);
	CONT_GM_AUTH_DATA_ID::iterator Itr = m_kGmUserWaitLogin.find(kLogin.ID());
	if(Itr == m_kGmUserWaitLogin.end())
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Regist GmAuthData [") << kLogin.ID() << _T("]") );
		return false;
	}
	CONT_GM_AUTH_DATA_ID::mapped_type kElement = Itr->second;
	SAuthInfo kGmAuth = *kElement;
	SAFE_DELETE(kElement);
	m_kGmUserWaitLogin.erase(Itr);

	switch(iErr)
	{
	case 0:
		{
			kGmAuth.kMemberGuid = kLogin.kMemberGuid;
//			kGmAuth.SetName(kLogin.szName);
			kGmAuth.byGMLevel = kLogin.byGMLevel;

			CONT_GM_AUTH_DATA::mapped_type pData = new SAuthInfo;
			*pData = kGmAuth;

			auto kPair = m_kGMUserLogined.insert(std::make_pair(kGmAuth.kMemberGuid, pData));
			if(kPair.second)
			{
				iErr = GE_SUCCESS;
			} 
			else
			{
				iErr = GE_LOGINED_ID;
			}
		}break;
	case 1:
		{
			iErr = GE_WORNG_ID_OR_PW;
		}break;
	case 2:
		{
			iErr = GE_NOT_ALLOW_IP;
		}break;
	case 4:
		{
			iErr = GE_CLOSE_ACCOUNT;
		}break;
	}
	kPacket.Push(iErr);
	kGmAuth.WriteToPacket(kPacket);
	g_kRealmMgr.WriteToPacket(kPacket, 0, ERealm_SendType_All);//로그인서버는 렐름정보 붙여줌.
	SendToServer(kGmAuth.kLoginServer, kPacket);

	return true;
}

void PgGMUserMgr::WriteToPacketGmUser(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_GM_AUTH_DATA::iterator Itr = m_kGMUserLogined.begin();
	rkPacket.Push(m_kGMUserLogined.size());
	while(Itr != m_kGMUserLogined.end())
	{
		CONT_GM_AUTH_DATA::mapped_type kElement = Itr->second;
		rkPacket.Push(*kElement);
		++Itr;
	}
}


void PgGMUserMgr::BroadCast(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_GM_AUTH_DATA::iterator Itr = m_kGMUserLogined.begin();
	unsigned short usType = 0;
	rkPacket.Pop(usType);
	while(Itr != m_kGMUserLogined.end())
	{
		CONT_GM_AUTH_DATA::key_type kKey = Itr->first;
		BM::Stream kPacket(usType);
		kPacket.Push(kKey);
		kPacket.Push(rkPacket);
		SendToLogin(kKey, kPacket);
		++Itr;
	}
}

bool PgGMUserMgr::SendToLogin(BM::GUID const &rkMemberId, BM::Stream const &rkPacket)
{
	CONT_GM_AUTH_DATA::iterator Itr = m_kGMUserLogined.find(rkMemberId);
	if(Itr == m_kGMUserLogined.end())
	{
		return false;
	}
	CONT_GM_AUTH_DATA::mapped_type kElement = Itr->second;
	return SendToServer(kElement->kLoginServer, rkPacket);
}

// - GM User Mgr <-End

