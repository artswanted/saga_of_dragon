#include "stdafx.h"
#include "PacketType.h"
#include "ProcessConfig.h"


/* Send 계열 함수들은 랩핑하여 Center 로만 보내라.
	이 주석 지우지 말것.
	bool SendToCenter(BM::Stream const &rkPacket);//프로세스들은 CenterServer 밖에 인식이 안된다.
	bool SendToContents(BM::Stream const &rkPacket);// 맵서버가 콘텐츠서버로 보낼 때, 래핑하여 맵->센터->콘텐츠			
	bool SendToItem(BM::Stream const &rkPacket);// 맵서버가 콘텐츠서버로 보낼 때, 래핑하여 맵->센터->콘텐츠			

	bool SendContentsToGnd(BM::Stream const &rkPacket, SGroundKey const &rkKey);// 콘텐츠 -> 센터 -> 그라운드
	bool SendContentsToUser(BM::Stream const &rkPacket, BM::GUID const &rkMemberGuid);// 콘텐츠 -> 센터 -> 유저
	bool SendContentsToServer(BM::Stream const &rkPacket, short const nServerNo);// 콘텐츠 -> 센터 -> 서버
*/

bool CProcessConfig::Locked_SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)const// 콘텐츠 -> 센터 -> 서버
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToServer(kSI, rkPacket);
}

bool CProcessConfig::Locked_SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, bool const bRandOne )const//연결된 세션으로 바로 보냄.
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToServerType(eServerType, rkPacket, bRandOne );
}

bool CProcessConfig::Locked_SendToChannelServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, short const nChannel, bool bPublic )const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToChannelServerType(eServerType, rkPacket, nChannel, bPublic);
}

bool CProcessConfig::Locked_SendToContentsServer(short const nRealm, BM::Stream const &rkPacket) const
{
	// ImmigrationServer or CenterServer 에서만 호출해야 한다.
	BM::CAutoMutex kLock(m_kMutex);
	return SendToContentsServer(nRealm, rkPacket);
}

bool CProcessConfig::SendToServer(SERVER_IDENTITY const &kSI, BM::Stream const &rkPacket)const// 콘텐츠 -> 센터 -> 서버
{
	CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.find(kSI);
	if(m_kServerHash.end() != server_itor)
	{
		CEL::CSession_Base *pkSession = (*server_itor).second.pkSession;
		if(pkSession)
		{
			pkSession->VSend(rkPacket);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::SendToServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, bool const bRandOne )const//연결된 세션으로 바로 보냄.
{
	CONT_BASE_SESSION const * pkOut = NULL;
	if(S_OK == GetSessionCont(eServerType, pkOut))
	{
		CONT_BASE_SESSION::const_iterator session_itor = pkOut->begin();

		if ( true == bRandOne )
		{
			int const iSize = static_cast<int>(pkOut->size());
			if ( iSize > 0 )
			{
				int iValue = BM::Rand_Index( iSize );

				while ( iValue )
				{
					++session_itor;
					--iValue;
				}

				return session_itor->pkSession->VSend( rkPacket );
			}
			return false;
		}
		else
		{
			while(session_itor != pkOut->end())
			{
				(*session_itor).pkSession->VSend(rkPacket);
				++session_itor;
			}
		}
	}
	else
	{
		CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.begin();
		while(m_kServerHash.end() != server_itor)
		{
			CEL::CSession_Base *pkSession = (*server_itor).second.pkSession;
			if(pkSession)
			{
				if(	(eServerType == CEL::ST_NONE ||	eServerType == (*server_itor).second.nServerType))
				{
					pkSession->VSend(rkPacket);
				}
			}
			++server_itor;
		}
	}
	return true;
}

bool CProcessConfig::SendToChannelServerType(CEL::E_SESSION_TYPE const eServerType, BM::Stream const &rkPacket, short const nChannel, bool const bPublic )const
{//고속화 필요.
	if(nChannel < 0)
	{
		return SendToServerType(eServerType, rkPacket);
	}

	CONT_BASE_SESSION const * pkOut = NULL;
	if(S_OK == GetSessionCont(eServerType, pkOut))
	{
		CONT_BASE_SESSION::const_iterator session_itor = pkOut->begin();
		while(session_itor != pkOut->end())
		{
			if (session_itor->pkSession)
			{
				if ( bPublic && CProcessConfig::IsPublicChannel(session_itor->kSI.nChannel) )
				{
					session_itor->pkSession->VSend(rkPacket);
				}
				else if(	( nChannel == session_itor->kSI.nChannel )
				&& (eServerType == CEL::ST_NONE || eServerType == session_itor->kSI.nServerType))
				{
					session_itor->pkSession->VSend(rkPacket);
				}
			}
			++session_itor;
		}
	}
	else
	{
		CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.begin();
		while(m_kServerHash.end() != server_itor)
		{
			CEL::CSession_Base *pkSession = (*server_itor).second.pkSession;
			
			if(pkSession)
			{
				if(	( nChannel == server_itor->second.nChannel )
				&& (eServerType == CEL::ST_NONE || eServerType == (*server_itor).second.nServerType))
				{
					pkSession->VSend(rkPacket);
				}
			}
			++server_itor;
		}
	}
	return true;
}

HRESULT CProcessConfig::GetSessionCont(CEL::E_SESSION_TYPE const eServerType, CONT_BASE_SESSION const *&pkOut)const
{
	switch(eServerType)
	{
	case CEL::ST_CENTER:{ pkOut = &m_kSession_Center; return S_OK;}break;
	case CEL::ST_SWITCH:{ pkOut = &m_kSession_Switch; return S_OK;}break;
	case CEL::ST_MAP:	{ pkOut = &m_kSession_Map; return S_OK;}break;
	case CEL::ST_CONSENT: { pkOut = &m_kSession_Consent; return S_OK;}break;
	case CEL::ST_IMMIGRATION: { pkOut = &m_kSession_Immigration; return S_OK;}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::GetSessionCont(CEL::E_SESSION_TYPE const eServerType, CONT_BASE_SESSION *&pkOut)
{//GetSessionCont와 똑같게.
	switch(eServerType)
	{
	case CEL::ST_CENTER:{ pkOut = &m_kSession_Center; return S_OK;}break;
	case CEL::ST_SWITCH:{ pkOut = &m_kSession_Switch; return S_OK;}break;
	case CEL::ST_MAP:	{ pkOut = &m_kSession_Map; return S_OK;}break;
	case CEL::ST_CONSENT: { pkOut = &m_kSession_Consent; return S_OK;}break;
	case CEL::ST_IMMIGRATION: { pkOut = &m_kSession_Immigration; return S_OK;}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool CProcessConfig::SendToContentsServer(short const nRealm, BM::Stream const &rkPacket) const
{
	// (Immigration or Center) --> Contents
	// ImmigrationServer or CenterServer 에서만 호출해야 한다.
	CONT_BASE_SESSION const *pkSessionCont = NULL;
	if (GetSessionCont(CEL::ST_CONTENTS, pkSessionCont) == S_OK)
	{
		CONT_BASE_SESSION::const_iterator session_itor = pkSessionCont->begin();
		while(session_itor != pkSessionCont->end())
		{
			if (nRealm == 0)
			{
				(*session_itor).pkSession->VSend(rkPacket);
			}
			else if (((*session_itor).kSI.nRealm == nRealm) 
				&& ((*session_itor).kSI.nServerType == CEL::ST_CONTENTS))
			{
				(*session_itor).pkSession->VSend(rkPacket);
				return true;
			}
			++session_itor;
		}
	}
	else
	{
		CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.begin();
		while(m_kServerHash.end() != server_itor)
		{
			CEL::CSession_Base *pkSession = (*server_itor).second.pkSession;
			if(pkSession)
			{
				if(CEL::ST_CONTENTS == (*server_itor).second.nServerType)
				{
					if (nRealm == 0)
					{
						pkSession->VSend(rkPacket);
					}
					else if (((*server_itor).first.nRealm == nRealm) 
						&& ((*server_itor).first.nServerType == CEL::ST_CONTENTS))
					{
						pkSession->VSend(rkPacket);
						return true;
					}
				}
			}
			++server_itor;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::Locked_SendToConsentServer(BM::Stream const &rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_BASE_SESSION::const_iterator itor_consent = m_kSession_Consent.begin();
	bool bSend = false;
	while (itor_consent != m_kSession_Consent.end())
	{
		if ((*itor_consent).pkSession != NULL)
		{
			(*itor_consent).pkSession->VSend(rkPacket);
			bSend = true;
		}
		++itor_consent;
	}
	
	if (false == bSend)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("cannot find ConsentServer Session..."));
	}
	return bSend;


}