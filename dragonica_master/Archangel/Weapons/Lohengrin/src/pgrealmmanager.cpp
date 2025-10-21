#include "StdAfx.h"
//#include "Constant.h"
#include "PgRealmManager.h"

PgRealmManager::PgRealmManager(void)
{
}

PgRealmManager::~PgRealmManager(void)
{
}

PgRealmManager::PgRealmManager( PgRealmManager const &rhs )
{
	m_kContRealm = rhs.m_kContRealm;
	m_kDefRestrictions = rhs.m_kDefRestrictions;
}

PgRealmManager& PgRealmManager::operator = ( PgRealmManager const &rhs )
{
	m_kContRealm = rhs.m_kContRealm;
	m_kDefRestrictions = rhs.m_kDefRestrictions;
	return *this;
}

void PgRealmManager::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kContRealm.clear();
	m_kDefRestrictions.clear();
}

bool PgRealmManager::Init( CONT_REALM_CANDIDATE const &kRealmCandi, CONT_SERVER_HASH const &rkServerList )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_REALM_CANDIDATE::const_iterator candi_itor = kRealmCandi.begin();
	while( candi_itor != kRealmCandi.end())
	{
		SRealmElement const &element = (*candi_itor).second;

		auto kRet = m_kContRealm.insert(std::make_pair(element.RealmNo(), PgRealm(element.RealmNo(), element.RealmName())));
		if(!kRet.second)
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Realm insert failed, I think this situation is impossible RealmNo["<<element.RealmNo()<<L"]");
			m_kContRealm.clear();
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++candi_itor;
	}

//	렐름별로 서버 추가를 다하고
//	서버 구성이 올바른지 체크 하도록.!~

	CONT_SERVER_HASH::const_iterator server_itor = rkServerList.begin();
	while(server_itor != rkServerList.end() )
	{
		short const nRealmNo = server_itor->second.nRealm;
		CONT_REALM::iterator itor = m_kContRealm.find(nRealmNo);
		if(m_kContRealm.end() == itor)
		{//예약되지 않은 렐름은 건너뜀.(0 번만 그렇게 될꺼임)
			if(nRealmNo)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Realm insert failed, I think this situation is impossible RealmNo["<<nRealmNo<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			++server_itor;
			continue;
		}

		if(!itor->second.AddServer(server_itor->second))
		{
			INFO_LOG(BM::LOG_LV5, L"[PgRealmManager] Add Server Failed "<< C2L((*server_itor).first) );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Add Server Failed"));
		}
		else
		{
			INFO_LOG(BM::LOG_LV7, L"[PgRealmManager] Add Server "<<C2L(server_itor->first) );
		}
		++server_itor;
	}

	CONT_REALM::iterator realm_itor = m_kContRealm.begin();
	while(realm_itor != m_kContRealm.end())
	{
		(*realm_itor).second.Build();
		++realm_itor;
	}

	return true;
}

void PgRealmManager::SetRestrictions( CONT_RESTRICT_ON_CHANNEL_REALMS const &kCont )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_RESTRICT_ON_CHANNEL_REALMS::const_iterator itr = kCont.begin();
	for ( ; itr!=kCont.end(); ++itr )
	{
		CONT_REALM::iterator realm_itr = m_kContRealm.find( itr->first );
		if ( realm_itr != m_kContRealm.end() )
		{
			PgRealm &kRealm = realm_itr->second;

			CONT_RESTRICT_ON_CHANNEL::const_iterator chan_itr = itr->second.begin();
			for ( ; chan_itr!=itr->second.end(); ++chan_itr )
			{
				kRealm.SetRestrictions( m_kDefRestrictions, *chan_itr );
			}
		}
		else
		{
			INFO_LOG( BM::LOG_LV5, __FL__<<L"Not Found RealmNo["<<itr->first<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Realm"));
		}
	}
}

/*
bool PgRealmManager::ConnectAllCenter(BM::GUID const &rkConnector)
{
//xxx 로그인 서버의 채널 접속 관련 문제.

	BM::CAutoMutex kLock(m_kMutex);

	Connector((rkConnector == BM::GUID::NullData()) ? Connector() : rkConnector);
	CONT_REALM::iterator itor = m_kContRealm.begin();
	while (m_kContRealm.end() != itor)
	{
		itor->second.ConnectAllCenter(Connector());
		++itor;
	}
	return true;
}
*/
bool PgRealmManager::WriteToPacket(BM::Stream &rkPacket, short sRealm, ERealm_SendType const eSendType)const
{
	BM::CAutoMutex kLock(m_kMutex);

	rkPacket.Push(eSendType);
	if (sRealm == 0)
	{
		rkPacket.Push((short)m_kContRealm.size());
		CONT_REALM::const_iterator itor = m_kContRealm.begin();
		while(itor != m_kContRealm.end())
		{
			itor->second.WriteToPacket(rkPacket, eSendType);
			++itor;
		}
	}
	else
	{
		CONT_REALM::const_iterator itor = m_kContRealm.find(sRealm);
		if (itor != m_kContRealm.end())
		{
			rkPacket.Push((short)1);
			itor->second.WriteToPacket(rkPacket, eSendType);
		}
		else
		{
			rkPacket.Push((short)0);
		}
	}

	return true;
}

bool PgRealmManager::ReadFromPacket(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	short nRealmCount = 0;
	ERealm_SendType eSendType;
	rkPacket.Pop(eSendType);
	rkPacket.Pop(nRealmCount);
	
	for(short usI=0; nRealmCount>usI; usI++)
	{
		PgRealm kRealm(0, _T(""));
		size_t const iRdPos = rkPacket.RdPos();
		kRealm.ReadFromPacket(rkPacket, eSendType);
		
		CONT_REALM::iterator itor = m_kContRealm.find(kRealm.RealmNo());
		if (itor != m_kContRealm.end())
		{
			// 기존것에 update
			rkPacket.RdPos(iRdPos);
			itor->second.ReadFromPacket(rkPacket, eSendType);
		}
		else
		{
			m_kContRealm.insert(std::make_pair(kRealm.RealmNo(), kRealm));
		}
	}

	return true;
}

void PgRealmManager::GetRealmCont(CONT_REALM &kCont)const
{
	BM::CAutoMutex kLock(m_kMutex);
	kCont = m_kContRealm;
}

HRESULT PgRealmManager::GetRealm(const CONT_REALM::key_type nRealmNo, PgRealm &rkOut)const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_REALM::const_iterator itor = m_kContRealm.find(nRealmNo);
	if(itor != m_kContRealm.end())
	{
		rkOut = (*itor).second;
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgRealmManager::GetChannel(const CONT_REALM::key_type nRealmNo, const CONT_CHANNEL::key_type nChannelNo, CONT_CHANNEL::mapped_type &rkOutChannel)const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_REALM::const_iterator itor = m_kContRealm.find(nRealmNo);
	if(itor != m_kContRealm.end())
	{
		const CONT_REALM::mapped_type &kRealm = (*itor).second;
		
		if(S_OK == kRealm.GetChannel(nChannelNo, rkOutChannel))
		{
			return S_OK;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

short PgRealmManager::AliveChannel(short const nRealm, short const nChannelNo, bool const bAlive)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	if (nRealm == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	CONT_REALM::iterator itor = m_kContRealm.find(nRealm);
	if (itor != m_kContRealm.end())
	{
		if (nChannelNo >= 0)
		{
			return itor->second.AliveChannel(nChannelNo, bAlive);
		}
		else
		{
			AliveRealm(nRealm, bAlive);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

/*
bool PgRealmManager::RefreshChannelInfo(BM::Stream &kPacket, const CEL::SESSION_KEY &rkSessionKey)
{//여기서 로지컬 채널 리저브 해야함.
	BM::CAutoMutex kLock(m_kMutex);

	SERVER_IDENTITY kSI;
	kSI.ReadFromPacket(kPacket);

	AliveChannel(kSI.nChannel, rkSessionKey);//채널 살림.
	return true;
}
*/

void PgRealmManager::UpdateUserCount(short const sRealm, short const sChannel, int const iMax, int const iCurrent)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_REALM::iterator itor = m_kContRealm.find(sRealm);
	if (itor != m_kContRealm.end())
	{
		itor->second.UpdateUserCount(sChannel, iMax, iCurrent);
	}
	else
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot find Realm["<<sRealm<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Realm"));
	}
}

void PgRealmManager::UpdatePrimeChannel(short int const sRealm, short int const sPrimeChannel)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_REALM::iterator itor = m_kContRealm.find(sRealm);
	if (itor != m_kContRealm.end())
	{
		itor->second.UpdatePrimeChannel(sPrimeChannel);
	}
	else
	{
		if (sRealm < 0)
		{
			// 모든 Realm의 PrimeChannel 값을 reset
			CONT_REALM::iterator itor = m_kContRealm.begin();
			while (itor != m_kContRealm.end())
			{
				itor->second.UpdatePrimeChannel(sPrimeChannel);
				++itor;
			}
		}
		else
		{
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot find Realm["<<sRealm<<L"]");
		}
	}
}

short PgRealmManager::GetPrimeChannel(short const sRealm)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_REALM::iterator itor = m_kContRealm.find(sRealm);
	if (itor != m_kContRealm.end())
	{
		return itor->second.GetPrimeChannel();
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

//DB
HRESULT PgRealmManager::Q_DQT_LOAD_DEF_RESTRICTIONS(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while ( rkResult.vecArray.end() != itr )
		{
			CONT_DEF_RESTRICTIONS::key_type kKey;
			CONT_DEF_RESTRICTIONS::mapped_type kElement;

			itr->Pop( kKey );				++itr;
			itr->Pop( kElement.kType );		++itr;
			itr->Pop( kElement.iErrorMsg );	++itr;

			itr->Pop( kElement.iValue1 );	++itr;
			itr->Pop( kElement.iValue2 );	++itr;

			if ( kElement.kType == RESTRICT_NONE )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"IDX["<<kKey<<L"] Error : Type is 0" );
			}
			else if ( !kElement.iErrorMsg )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"IDX["<<kKey<<L"] Error : ErrorMsg is 0" );
			}
			else
			{
				m_kDefRestrictions.insert( std::make_pair(kKey, kElement) );
			}	
		}
	}
	else if ( CEL::DR_NO_RESULT == rkResult.eRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
		return S_FALSE;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgRealmManager::Q_DQT_LOAD_RESTRICTIONS_ON_CHANNEL(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CONT_RESTRICT_ON_CHANNEL_REALMS	kCont;

		wchar_t wszTemp[201] = {0,};
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		wchar_t* pContextToken = NULL;
		wchar_t const *pDelimet = L",";
		while ( rkResult.vecArray.end() != itr )
		{
			CONT_RESTRICT_ON_CHANNEL::value_type kElement;
			std::wstring wstrOptions;

			itr->Pop( kElement.nRealmNo );			++itr;
			itr->Pop( kElement.nChannelNo );		++itr;
			itr->Pop( kElement.iMaxUserCount );		++itr;
			itr->Pop( wstrOptions );				++itr;

			if ( wstrOptions.size() < 201 )
			{
				::swprintf_s( wszTemp, 201, wstrOptions.c_str() );
				pContextToken = NULL;
				wchar_t *pToken = ::wcstok_s( wszTemp, pDelimet, &pContextToken);
				while ( pToken )
				{
					int iValue = ::_wtoi( pToken );
					if ( iValue )
					{
						kElement.kContOptions.push_back( iValue );
					}
					pToken = ::wcstok_s( NULL, pDelimet, &pContextToken );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Error Option["<<wstrOptions<<L"]" );
			}

			auto kRet = kCont.insert( std::make_pair( kElement.nRealmNo, CONT_RESTRICT_ON_CHANNEL() ) );
			kRet.first->second.push_back( kElement );
		}

		SetRestrictions( kCont );
		return S_OK;
	}
	else if ( CEL::DR_NO_RESULT == rkResult.eRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
		return S_FALSE;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgRealmManager::AliveRealm(short const nRealm, bool const bAlive)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_REALM::iterator itor_realm = m_kContRealm.find(nRealm);
	if (itor_realm != m_kContRealm.end())
	{
		itor_realm->second.IsAlive(bAlive);
	}
}

size_t PgRealmManager::GetDeadRealmCount(void)const
{
	BM::CAutoMutex kLock( m_kMutex );

	size_t iCount = 0;

	CONT_REALM::const_iterator itr_realm = m_kContRealm.begin();
	for ( ; itr_realm!=m_kContRealm.end() ; ++itr_realm )
	{
		if ( !itr_realm->second.IsAlive() )
		{
			++iCount;
		}
	}
	return iCount;
}

HRESULT PgRealmManager::SetRealmState(CONT_REALM::key_type const nRealmNo, short const sState)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_REALM::iterator itor_realm = m_kContRealm.find(nRealmNo);
	if (itor_realm != m_kContRealm.end())
	{
		itor_realm->second.State(sState);
		return S_OK;
	}
	return E_FAIL;
}

short PgRealmManager::GetRealmState(CONT_REALM::key_type const nRealmNo)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_REALM::const_iterator itor_realm = m_kContRealm.find(nRealmNo);
	if (itor_realm != m_kContRealm.end())
	{
		return itor_realm->second.State();
	}
	return PgRealm::EREALM_STATE_NONE;
}

CONT_REALM::key_type PgRealmManager::RealmFirstNo()const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_REALM::const_iterator itor_realm = m_kContRealm.begin();
	while (itor_realm != m_kContRealm.end())
	{
		if(PgRealm::EREALM_STATE_NONE != itor_realm->second.State())
		{
			return itor_realm->first;
		}
		++itor_realm;
	}
	return 0;
}

HRESULT PgRealmManager::Q_DQT_LOAD_CHANNEL_NOTICE(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( CEL::DR_SUCCESS == rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while ( rkResult.vecArray.end() != itr )
		{
			short sRealm = 0;
			short sChannel = 0;
			std::wstring strNotice;
			std::wstring strNoticeInGame;
			itr->Pop( sRealm );			++itr;
			itr->Pop( sChannel );		++itr;
			itr->Pop( strNotice );		++itr;
			itr->Pop( strNoticeInGame );		++itr;

			SetNotice(sRealm, sChannel, strNotice, strNoticeInGame);
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgRealmManager::SetNotice(short const sRealm, short const sChannel, std::wstring const& kNotice, std::wstring const& kNoticeInGame)
{
	if (sRealm == 0)
	{
		CONT_REALM::iterator itor_realm = m_kContRealm.begin();
		while (itor_realm != m_kContRealm.end())
		{
			(*itor_realm).second.SetNotice(sChannel, kNotice, kNoticeInGame);
			++itor_realm;
		}
	}
	else
	{
		CONT_REALM::iterator itor_realm = m_kContRealm.find(sRealm);
		if (itor_realm == m_kContRealm.end())
		{
			return E_FAIL;
		}
		(*itor_realm).second.SetNotice(sChannel, kNotice, kNoticeInGame);
	}
	return S_OK;
}