#include "stdafx.h"
#include "Lohengrin/PgPlayLimiter.h"
#include "Variant/PgHardCoreDungeonParty.h"
#include "PgHardCoreDungeonSystem.h"
#include "PgHardCoreDungeonSystemMgr.h"

PgHardCoreDungeonSystemMgr::PgHardCoreDungeonSystemMgr(void)
{

}

PgHardCoreDungeonSystemMgr::~PgHardCoreDungeonSystemMgr(void)
{
	CONT_SYSTEM::iterator itr = m_kContSystem.begin();
	for ( ; itr!=m_kContSystem.end() ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}
}

HRESULT PgHardCoreDungeonSystemMgr::Regist( CONT_SYSTEM::key_type const iID )
{
	CONT_SYSTEM::const_iterator itr = m_kContSystem.find( iID );
	if ( itr != m_kContSystem.end() )
	{
		return E_FAIL;
	}

	PgHardCoreDungeonSystem * pkSystem = new PgHardCoreDungeonSystem( iID );
	if ( !pkSystem )
	{
		return E_OUTOFMEMORY;
	}

	m_kContSystem.insert( std::make_pair( iID, pkSystem ) );
	return S_OK;
}

void PgHardCoreDungeonSystemMgr::OnTick()
{
	CONT_DEF_PLAYLIMIT_INFO const *pkPlayLimitInfo = NULL;
	g_kTblDataMgr.GetContDef( pkPlayLimitInfo );

	__int64 const i64NowTimeInDay = g_kEventView.GetLocalSecTimeInDay( CGameTime::DEFAULT ) + CGameTime::MINUTE;//1єРБ¤µµ ЅГ°ЈА» »Ўё® ГјЕ©ЗШј­ №Мё® ёёµйАЪ.

	CONT_SYSTEM::const_iterator itr = m_kContSystem.begin();
	for ( ; itr != m_kContSystem.end() ; ++itr )
	{
		PgHardCoreDungeonSystem * pkSystem = itr->second;
		if ( !pkSystem->IsOpen() )
		{
			CONT_DEF_PLAYLIMIT_INFO::const_iterator info_itr = pkPlayLimitInfo->find( itr->first );
			if ( info_itr != pkPlayLimitInfo->end() )
			{
				CONT_DEF_PLAYLIMIT_INFO::mapped_type::value_type kFindInfo;
				PgPlayLimit_Finder kFinder( info_itr->second );
				if ( S_OK == kFinder.Find( i64NowTimeInDay, kFindInfo ) )
				{
					__int64 const i64BeginTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
					SYSTEMTIME kTempTime;
					CGameTime::SecTime2SystemTime( i64BeginTime, kTempTime, CGameTime::DEFAULT );
					kTempTime.wHour = 0;
					kTempTime.wMinute = 0;
					kTempTime.wSecond = 0;
					kTempTime.wMilliseconds = 0;
					__int64 i64BaseTime = 0i64;
					CGameTime::SystemTime2SecTime( kTempTime, i64BaseTime, CGameTime::DEFAULT );

					pkSystem->ReqOpen( i64BeginTime, i64BaseTime + kFindInfo.i64EndTime, static_cast<int>(kFindInfo.nLevelLimit), kFindInfo.iValue01, kFindInfo.iValue02 );
				}
			}	
		}
	}
}

bool PgHardCoreDungeonSystemMgr::IsOpen( CONT_SYSTEM::key_type const iID )const
{
	CONT_SYSTEM::const_iterator itr = m_kContSystem.find( iID );
	if ( itr != m_kContSystem.end() )
	{
		return itr->second->IsOpen();
	}

	return false;
}

int PgHardCoreDungeonSystemMgr::IsCanJoin( CONT_SYSTEM::key_type const iID, int const iLevel, SGroundKey &rkOutGndKey )const
{
	CONT_SYSTEM::const_iterator itr = m_kContSystem.find( iID );
	if ( itr != m_kContSystem.end() )
	{
		return itr->second->IsCanJoin( iLevel, rkOutGndKey );
	}
	return 200153;
}

HRESULT PgHardCoreDungeonSystemMgr::RecvOpenResult( CONT_SYSTEM::key_type const iID, HRESULT const hRet, SGroundKey const &kGndKey )
{
	PgHardCoreDungeonSystem *pkSystem = GetSystem( iID );
	if ( pkSystem )
	{
		return pkSystem->RecvOpenResult( hRet, kGndKey );
	}

	return E_FAIL;
}

HRESULT PgHardCoreDungeonSystemMgr::RecvClose( SGroundKey const &kGndKey )
{
	CONT_SYSTEM::iterator itr = m_kContSystem.begin();
	for ( ; itr != m_kContSystem.end() ; ++itr )
	{
		if ( SUCCEEDED(itr->second->RecvClose( kGndKey)) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

void PgHardCoreDungeonSystemMgr::RecvAllClose()
{
	CONT_SYSTEM::iterator itr = m_kContSystem.begin();
	for ( ; itr != m_kContSystem.end() ; ++itr )
	{
		itr->second->SetClose();
	}
}

PgHardCoreDungeonSystem* PgHardCoreDungeonSystemMgr::GetSystem( int const IID )
{
	CONT_SYSTEM::iterator itr = m_kContSystem.find( IID );
	if ( itr != m_kContSystem.end() )
	{
		return itr->second;
	}
	return NULL;
}


//--------------------------------------------------------------
//-- PgHardCoreDungeonSystemMgr_Wrapper
//--------------------------------------------------------------
PgHardCoreDungeonSystemMgr_Wrapper::PgHardCoreDungeonSystemMgr_Wrapper(void)
{}

PgHardCoreDungeonSystemMgr_Wrapper::~PgHardCoreDungeonSystemMgr_Wrapper(void)
{}

HRESULT PgHardCoreDungeonSystemMgr_Wrapper::Regist( PgHardCoreDungeonSystemMgr::CONT_SYSTEM::key_type const iID )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->Regist( iID );
}

void PgHardCoreDungeonSystemMgr_Wrapper::OnTick()
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	Instance()->OnTick();
}

void PgHardCoreDungeonSystemMgr_Wrapper::RecvAllClose()
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	Instance()->RecvAllClose();
}

HRESULT PgHardCoreDungeonSystemMgr_Wrapper::ProcessPacket( BM::Stream * const pkPacket )
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop( usType );

	switch( usType )
	{
	case PT_T_N_ANS_DELETE_PUBLICMAP:
		{
			SGroundKey kGndKey;
			if ( true == pkPacket->Pop( kGndKey ) )
			{
				BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
				if ( FAILED(Instance()->RecvClose( kGndKey )) )
				{
					INFO_LOG( BM::LOG_LV5, L"[HardCoreDungeonSystem] Close Dungeon Failed(Not Found) : " << kGndKey.ToString() );
				}
			}
		}break;
	case PT_M_T_REQ_REGIST_HARDCORE_VOTE:
		{
			short nChannelNo = 0;
			SGroundKey kGndKey;
			int iMode = 0;
			BM::GUID kReqCharID;
			int iLevel = 0;
			bool bHadParty = false;
			pkPacket->Pop( nChannelNo );
			kGndKey.ReadFromPacket( *pkPacket );
			pkPacket->Pop( iMode );
			pkPacket->Pop( kReqCharID );
			pkPacket->Pop( iLevel );
			pkPacket->Pop( bHadParty );

			int iError = 0;
			SGroundKey kDungeonGndKey;

			{
				BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
				iError = Instance()->IsCanJoin( iMode, iLevel, kDungeonGndKey );
			}

			if ( !iError )
			{
				if ( true == bHadParty )
				{
					BM::Stream kPartyPacket( PT_M_T_REQ_REGIST_HARDCORE_VOTE, iMode );
					kDungeonGndKey.WriteToPacket( kPartyPacket );
					kPartyPacket.Push( kReqCharID );
					kPartyPacket.Push( *pkPacket );
					::SendToChannelContents( nChannelNo, PMET_PARTY, kPartyPacket );
				}
				else
				{
					PgPartyContents_HardCoreDungeon kContents;
					kContents.Init( kReqCharID );
					kContents.SetMode( iMode );
					kContents.SetDungeonGndKey( kDungeonGndKey );

					BM::Stream kMPacket( PT_T_M_ANS_REGIST_HARDCORE_VOTE, kReqCharID );
					kMPacket.Push( iError );
					kContents.WriteToPacket( kMPacket );
					::SendToGround( nChannelNo, kGndKey, kMPacket, true );
				}
			}
			else
			{
				BM::Stream kFailedPacket( PT_T_M_ANS_REGIST_HARDCORE_VOTE, kReqCharID );
				kFailedPacket.Push( iError );
				::SendToGround( nChannelNo, kGndKey, kFailedPacket, true );
			}
		}break;
	case PT_T_N_ANS_CREATE_PUBLICMAP:
		{
			SGroundMakeOrder kOrder;
			HRESULT hRet = E_FAIL;
			int iMode = 0;
			pkPacket->Pop( hRet );
			kOrder.ReadFromPacket(*pkPacket);
			pkPacket->Pop( iMode );

			BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
			Instance()->RecvOpenResult( iMode, hRet, kOrder.kKey );
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << L"Unknown Packet Type<" << usType << L">" );
			return E_FAIL;
		}break;
	}
	return S_OK;
}
