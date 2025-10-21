#include "StdAfx.h"
#include "PgChannel.h"

PgChannel::PgChannel()
{
	m_kServers.clear();
	IsAlive(false);
	MaxUser(0);
	CurrentUser(0);
	ChannelName(_T(""));
	ChannelNo(0);
	PrimeChannel(false);
}

PgChannel::PgChannel(const PgChannel &rhs)
{
	*this = rhs;
}

void PgChannel::operator = (const PgChannel &rhs)
{
	m_kServers = rhs.m_kServers;
	m_bIsAlive = rhs.m_bIsAlive;
	m_kMaxUser = rhs.m_kMaxUser;
	m_kCurrentUser = rhs.m_kCurrentUser;
	m_kChannelName = rhs.m_kChannelName;
	m_kChannelNo = rhs.m_kChannelNo;
	m_kPrimeChannel = rhs.PrimeChannel();
	m_kNotice_ChnList = rhs.Notice_ChnList();
	m_kNotice_InGame = rhs.Notice_InGame();
}


PgChannel::~PgChannel(void)
{

}
bool PgChannel::Build()
{
	CONT_SERVER_HASH::const_iterator itor = m_kServers.begin();
	while(itor != m_kServers.end())
	{
		CONT_SERVER_HASH::mapped_type const &element =  (*itor).second;
		if(element.nServerType == CEL::ST_CENTER)
		{
			ChannelName(element.strName);
			ChannelNo(element.nChannel);
		}
		++itor;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgChannel::AddServer(TBL_SERVERLIST const & slServer)
{
	auto ret = m_kServers.insert(std::make_pair( *(SERVER_IDENTITY*)&slServer, slServer));
	if(ret.second)
	{
		return true;
	}
	
	VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Server insert failed, Already Use RealmNo["<<slServer.nRealm<<L"] ServerNo["<<slServer.nServerNo<<L"] ChannelNo["<<slServer.nChannel<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgChannel::WriteToPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType)const
{
	rkPacket.Push(m_kChannelNo);
	rkPacket.Push(m_kChannelName);
	rkPacket.Push(m_bIsAlive);
	if (eSendType & ERealm_SendType_UserCountFake)
	{
		// Client에 UserCount 를 속여서 보내주자.
		int const iMaxUser = 100;
		int iCurrUser = 0;
		if ( (m_kMaxUser > 0) && (m_kCurrentUser > 0) )
		{
			if ( m_kCurrentUser == m_kMaxUser )
			{
				iCurrUser = iMaxUser;
			}
			else
			{
				iCurrUser = static_cast<int>(m_kCurrentUser * iMaxUser / m_kMaxUser) + 1;
			}
		}

		rkPacket.Push(iMaxUser);
		rkPacket.Push(iCurrUser);
	}
	else
	{
		rkPacket.Push(m_kMaxUser);
		rkPacket.Push(m_kCurrentUser);
	}
	rkPacket.Push(m_kPrimeChannel);

	if( eSendType & ERealm_SendType_ServerInfo )
	{
		rkPacket.Push(m_kServers.size());

		CONT_SERVER_HASH::const_iterator server_itor = m_kServers.begin();
		while(server_itor != m_kServers.end())
		{
			(*server_itor).first.WriteToPacket(rkPacket);
			(*server_itor).second.WriteToPacket(rkPacket);
			++server_itor;
		}
	}

	if ( eSendType & ERealm_SendType_Notice )
	{
		rkPacket.Push(Notice_ChnList());
		rkPacket.Push(Notice_InGame());
	}
}

void PgChannel::ReadFromPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType)
{
	rkPacket.Pop(m_kChannelNo);
	
	rkPacket.Pop(m_kChannelName);
	rkPacket.Pop(m_bIsAlive);
	rkPacket.Pop(m_kMaxUser);
	rkPacket.Pop(m_kCurrentUser);
	rkPacket.Pop(m_kPrimeChannel);

	if((eSendType & ERealm_SendType_ServerInfo) != 0)
	{
		m_kServers.clear();
		
		size_t element_size = 0;
		rkPacket.Pop(element_size);

		for(size_t i = 0;  element_size > i; i++)
		{
			SERVER_IDENTITY kSI;
			TBL_SERVERLIST kTbl;

			kSI.ReadFromPacket(rkPacket);
			kTbl.ReadFromPacket(rkPacket);
			
			auto ret = m_kServers.insert(std::make_pair(kSI, kTbl));
			if(!ret.second)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Server insert failed, Already Use RealmNo["<<kSI.nRealm<<L"] ServerNo["<<kSI.nServerNo<<L"] ChannelNo["<<kSI.nChannel<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			}
		}
	}

	if ((eSendType & ERealm_SendType_Notice) != 0)
	{
		rkPacket.Pop(m_kNotice_ChnList);
		rkPacket.Pop(m_kNotice_InGame);
	}
}

void PgChannel::IsAlive(bool const bIsAlive)
{
	if (!bIsAlive)
	{
		CurrentUser(0);
		PrimeChannel(false);
	}
	m_bIsAlive = bIsAlive;
}

HRESULT PgChannel::GetServerIdentity(CEL::E_SESSION_TYPE const eServerType, SERVER_IDENTITY &kOutSI)
{
	CONT_SERVER_HASH::const_iterator server_itor = m_kServers.begin();
	while(server_itor != m_kServers.end())
	{
		if(eServerType == (*server_itor).first.nServerType)
		{
			kOutSI= (*server_itor).first;
			return S_OK;
		}
		++server_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgChannel::UpdateUserCount(int const iMax, int const iCurrent)
{
	MaxUser(iMax);
	CurrentUser(iCurrent);
	IsAlive(true);
}