#include "StdAfx.h"
#include "PgRealm.h"
#include "Lohengrin/processconfig.h"

PgRealm::PgRealm(short const nRealmNo, std::wstring const &wstrRealmName)
:	m_bIsAlive(false)
{
	m_nRealmNo=nRealmNo;
	Name(wstrRealmName);
	State(EREALM_STATE_NONE);
}

PgRealm::PgRealm(PgRealm const &rhs)
{
	*this = rhs;
}

PgRealm& PgRealm::operator = ( PgRealm const &rhs)
{
	m_nRealmNo=rhs.m_nRealmNo;
	m_kName = rhs.m_kName;
	m_kContChannel = rhs.m_kContChannel;
	m_bIsAlive = rhs.m_bIsAlive;
	State(rhs.State());
	return *this;
}

PgRealm::~PgRealm(void)
{
}

bool PgRealm::Build()
{
	CONT_CHANNEL::iterator itor = m_kContChannel.begin();

	while(itor != m_kContChannel.end())
	{
		(*itor).second.Build();
		++itor;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

bool PgRealm::AddServer(TBL_SERVERLIST const &rkServerList)
{//랠름은 같다고 보는거다.
	if ( !CProcessConfig::IsPublicChannel(rkServerList.nChannel) )
	{
		CONT_CHANNEL::iterator itor = m_kContChannel.find(rkServerList.nChannel);
		if(itor == m_kContChannel.end())
		{
			auto ret = m_kContChannel.insert(std::make_pair(rkServerList.nChannel, PgChannel()));
			if(!ret.second)
			{
				assert(NULL);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			itor = ret.first;
		}

		return itor->second.AddServer(rkServerList);
	}
	return true;
}

bool PgRealm::SetRestrictions( CONT_DEF_RESTRICTIONS const &kRestrictDef, SRestrictOnChannel const &kOptions )
{
	CONT_CHANNEL::iterator itor = m_kContChannel.find(kOptions.nChannelNo);
	if( itor != m_kContChannel.end() )
	{
		return itor->second.Set( kRestrictDef, kOptions );
	}

	INFO_LOG( BM::LOG_LV0, __FL__<<L"Not Found Realm["<<m_nRealmNo<<L"] Channel["<<kOptions.nChannelNo<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgRealm::WriteToPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType) const
{
	rkPacket.Push(m_nRealmNo);
	rkPacket.Push(Name());
	rkPacket.Push(IsAlive());
	rkPacket.Push(State());

	if((eSendType &  ERealm_SendType_AllChannel) == 0 
	||	!m_nRealmNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("WriteToPacket Failed"));
		return;
	}

	{
		rkPacket.Push((short)m_kContChannel.size());
		CONT_CHANNEL::const_iterator itor = m_kContChannel.begin();
		while (itor != m_kContChannel.end())
		{
			itor->second.WriteToPacket(rkPacket, eSendType);
			++itor;
		}
	}
}

void PgRealm::ReadFromPacket(BM::Stream &rkPacket, ERealm_SendType const eSendType)
{
	rkPacket.Pop(m_nRealmNo);
	rkPacket.Pop(m_kName);
	rkPacket.Pop(m_bIsAlive);
	rkPacket.Pop(m_kState);
	
	if(	(eSendType &  ERealm_SendType_AllChannel) == 0 
	||	!m_nRealmNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ReadFromPacket Failed"));
		return;
	}

	{
		short nSize = 0;
		rkPacket.Pop(nSize);
		for(short usI=0; usI<nSize; usI++)
		{
			PgChannel kChannel;
			size_t const iRdPos = rkPacket.RdPos();
			kChannel.ReadFromPacket(rkPacket, eSendType);
			CONT_CHANNEL::iterator itor = m_kContChannel.find(kChannel.ChannelNo());
			if (itor != m_kContChannel.end())
			{
				// 기존것에 Update 한다.
				rkPacket.RdPos(iRdPos);
				itor->second.ReadFromPacket(rkPacket, eSendType);
			}
			else
			{
				auto ret = m_kContChannel.insert(std::make_pair(kChannel.ChannelNo(), kChannel));
				if(!ret.second)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Channel insert failed, Already Use ChannelNo["<<kChannel.ChannelNo()<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
				}
			}
		}
	}
}

void PgRealm::GetChannelCont(CONT_CHANNEL &rkOutCont)const
{
	rkOutCont = m_kContChannel;
}

HRESULT PgRealm::GetChannel(const CONT_CHANNEL::key_type nChannelNo, CONT_CHANNEL::mapped_type &rkOutCont)const
{
	CONT_CHANNEL::const_iterator itor = m_kContChannel.find(nChannelNo);
	if(itor != m_kContChannel.end())
	{
		rkOutCont = (*itor).second;
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgRealm::GetChannel(CONT_CHANNEL::key_type const nChannelNo, CONT_CHANNEL::mapped_type const* &pkChannel )const
{
	CONT_CHANNEL::const_iterator itor = m_kContChannel.find(nChannelNo);
	if(itor != m_kContChannel.end())
	{
		pkChannel = &(itor->second);
		return S_OK;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

short PgRealm::AliveChannel(short const nChannel, bool const bAlive)
{
	CONT_CHANNEL::iterator itor = m_kContChannel.find(nChannel);
	if (itor != m_kContChannel.end())
	{
		itor->second.IsAlive(bAlive);
		short const sPrimeChannel = GetPrimeChannel();
		if ( sPrimeChannel <= 0 )
		{
			return UpdatePrimeChannel();
		}
		return sPrimeChannel;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

short PgRealm::UpdatePrimeChannel()
{
	int iMinUserCount = 999999;
	short sChannelNo = 0;

	{
		CONT_CHANNEL::const_iterator itor = m_kContChannel.begin();
		// 가장 작은 CCU의 Channel을 PrimeChannel 로 뽑는다.
		while (itor != m_kContChannel.end())
		{
			if (itor->second.IsAlive() && itor->second.CurrentUser() < iMinUserCount)
			{
				iMinUserCount = itor->second.CurrentUser();
				sChannelNo = itor->second.ChannelNo();
			}
			++itor;
		}
	}
	if (sChannelNo > 0)
	{
		CONT_CHANNEL::iterator itor = m_kContChannel.find(sChannelNo);
		if (itor == m_kContChannel.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Cannot find Channel["<<sChannelNo<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
			return 0;
		}
		itor->second.PrimeChannel(true);
	}
	return sChannelNo;
}

void PgRealm::UpdateUserCount(short const sChannel, int const iMax, int const iCurrent)
{
	CONT_CHANNEL::iterator itor = m_kContChannel.find(sChannel);
	if (itor != m_kContChannel.end())
	{
		itor->second.UpdateUserCount(iMax, iCurrent);
	}
	else
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot find Channel["<<sChannel<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Channel"));
	}
}

void PgRealm::UpdatePrimeChannel(short const sChannel)
{
	CONT_CHANNEL::iterator itor = m_kContChannel.begin();
	while (itor != m_kContChannel.end())
	{
		if (itor->second.ChannelNo() == sChannel)
		{
			itor->second.PrimeChannel(true);
		}
		else
		{
			itor->second.PrimeChannel(false);
		}

		++itor;
	}
}

short PgRealm::GetPrimeChannel(void)const
{
	CONT_CHANNEL::const_iterator itor = m_kContChannel.begin();
	while (itor != m_kContChannel.end())
	{
		if (itor->second.PrimeChannel() )
		{
			return itor->second.ChannelNo();
		}

		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void PgRealm::IsAlive(bool const bAlive)
{
	m_bIsAlive = bAlive;
	if (!bAlive)
	{
		CONT_CHANNEL::iterator itor_channel = m_kContChannel.begin();
		while (itor_channel != m_kContChannel.end())
		{
			itor_channel->second.IsAlive(bAlive);
			++itor_channel;
		}
	}
}

HRESULT PgRealm::SetNotice(short const sChannel, std::wstring const& kNotice, std::wstring const& kNoticeInGame)
{
	if (sChannel == 0)
	{
		CONT_CHANNEL::iterator itor_channel = m_kContChannel.begin();
		while (itor_channel != m_kContChannel.end())
		{
			(*itor_channel).second.Notice_ChnList(kNotice);
			(*itor_channel).second.Notice_InGame(kNoticeInGame);
			++itor_channel;
		}
	}
	else
	{
		CONT_CHANNEL::iterator itor_channel = m_kContChannel.find(sChannel);
		if (itor_channel == m_kContChannel.end())
		{
			return E_FAIL;
		}
		(*itor_channel).second.Notice_ChnList(kNotice);
		(*itor_channel).second.Notice_InGame(kNoticeInGame);
	}
	return S_OK;
}