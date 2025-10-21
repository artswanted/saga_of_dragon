#include "stdafx.h"
#include "PgResultMgr.h"

PgResultMgr::PgResultMgr()
{
	m_kPool.Init( 20, 20 );
}

PgResultMgr::~PgResultMgr()
{
	Clear();
}

void PgResultMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	ConResult::iterator result_itr = m_kResult.begin();
	for ( ; result_itr!=m_kResult.end() ; ++result_itr )
	{
		m_kPool.Delete( result_itr->second );
	}

	ContExpeditionResult::iterator expedition_iter = m_ExpeditionResult.begin();
	for( ; expedition_iter != m_ExpeditionResult.end() ; ++expedition_iter)
	{
		m_ExpeditionPool.Delete( expedition_iter->second );
	}
	m_kResult.clear();
	m_ExpeditionResult.clear();
}

void PgResultMgr::ProcessMsg(SEventMessage *pkMsg)
{
	PACKET_ID_TYPE usType = 0;
	pkMsg->Pop( usType );

	switch ( usType )
	{
	case PT_M_T_ANS_CREATE_GROUND:
		{
			SGroundKey kGndKey;
			if ( pkMsg->Pop( kGndKey ) )
			{
				int Gattr = GKIND_DEFAULT;
				CONT_DEFMAP const * pDefMap;
				g_kTblDataMgr.GetContDef( pDefMap );
				CONT_DEFMAP::const_iterator iter = pDefMap->find(kGndKey.GroundNo());
				if( iter != pDefMap->end() )
				{
					Gattr = iter->second.iAttr;
				}

				BM::CAutoMutex kLock(m_kMutex);

				if( Gattr & GKIND_EXPEDITION )
				{
					PgExpeditionIndunPlayInfo * pInfo = m_ExpeditionPool.New();
					if( pInfo )
					{
						pInfo->Init(kGndKey);

						std::pair<ContExpeditionResult::iterator, bool> Ret = m_ExpeditionResult.insert( std::make_pair(kGndKey, pInfo) );
						if( !Ret.second )
						{
							m_ExpeditionPool.Delete(pInfo);
						}

						VEC_JOININDUN JoinIndunList;
						PU::TLoadArray_A( *pkMsg, JoinIndunList );
						AddExpeditionPlayer( kGndKey, JoinIndunList );
					}
				}
				else
				{
					PgInstanceDungeonPlayInfo *pkInfo = m_kPool.New();
					if ( pkInfo )
					{
						pkInfo->Init( kGndKey );

						std::pair<ConResult::iterator, bool> ibRet = m_kResult.insert( std::make_pair( kGndKey, pkInfo ) );
						if( !ibRet.second )
						{
							m_kPool.Delete(pkInfo);
						}

						VEC_JOININDUN JoinIndunList;
						PU::TLoadArray_A( *pkMsg, JoinIndunList );
						AddPlayer( kGndKey, JoinIndunList );
					}
				}
			}	
		}break;
	case PT_M_T_NFY_DELETE_GROUND:
		{
			SGroundKey kGndKey;
			pkMsg->Pop( kGndKey );
			EraseResult( kGndKey );
		}break;
	}
}

bool PgResultMgr::EraseResult( SGroundKey const &rkKey )
{
	int Gattr = GKIND_DEFAULT;
	CONT_DEFMAP const * pDefMap;
	g_kTblDataMgr.GetContDef( pDefMap );
	CONT_DEFMAP::const_iterator iter = pDefMap->find(rkKey.GroundNo());
	if( iter != pDefMap->end() )
	{
		Gattr = iter->second.iAttr;
	}

	BM::CAutoMutex kLock(m_kMutex);
	if( Gattr & GKIND_EXPEDITION )
	{
		ContExpeditionResult::iterator iter = m_ExpeditionResult.find(rkKey);
		if( iter != m_ExpeditionResult.end() )
		{
			PgExpeditionIndunPlayInfo * pInfo = iter->second;
			pInfo->Clear();
			m_ExpeditionResult.erase(iter);
			m_ExpeditionPool.Delete(pInfo);

			return true;
		}
	}
	else
	{
		ConResult::iterator itr = m_kResult.find( rkKey );
		if ( itr != m_kResult.end() )
		{
			PgInstanceDungeonPlayInfo *pkInfo = itr->second;
			if ( !pkInfo->IsStart() )
			{		
				pkInfo->Clear();
				m_kResult.erase( itr );
				m_kPool.Delete( pkInfo );
			}
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgResultMgr::AddPlayer( SGroundKey const &rkKey, VEC_JOININDUN const& rkPlayerList )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( !pkPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_JOININDUN::const_iterator join_itr;
	for( join_itr=rkPlayerList.begin(); join_itr!=rkPlayerList.end(); ++join_itr )
	{
		pkPlayInfo->Join( *join_itr );
	}
	return true;
}

bool PgResultMgr::DeletePlayer( SGroundKey const &rkKey, BM::GUID const &rkCharGuid )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( !pkPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkPlayInfo->Leave( rkCharGuid );
	return true;
}

bool PgResultMgr::Start( SGroundKey const &rkKey, VEC_RESULTINDUN const& rkResultList )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( pkPlayInfo )
	{
		if ( pkPlayInfo->ResultStart( rkResultList ) )
		{
			m_kTick.push( pkPlayInfo );
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgResultMgr::SetItem( SGroundKey const &rkKey, const SNfyResultItemList& rkReusltItem )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( !pkPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return pkPlayInfo->SetResultItem( rkReusltItem );
}

// bool PgResultMgr::SetBonus( SGroundKey const &rkKey, SConResultBonus& rkBonus )
// {
// 	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
// 	if ( !pkPlayInfo )
// 	{
// 		return false;
// 	}
// 
// 	return pkPlayInfo->SetResultBonus( rkBonus );
// }

bool PgResultMgr::SelectBox( SGroundKey const &rkKey, BM::GUID const &rkCharGuid, int const iSelect )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( !pkPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return pkPlayInfo->SetSelectBox( (size_t)iSelect, rkCharGuid );
}

void PgResultMgr::Tick( DWORD const dwCurTime )
{
	BM::CAutoMutex kLock( m_kMutex );
	while ( !m_kTick.empty() )
	{
		PgInstanceDungeonPlayInfo *pkInfo = m_kTick.front();
		if ( pkInfo->GetTime() > dwCurTime )
		{
			break;
		}
		pkInfo->ResultEnd();
		m_kTick.pop();
		EraseResult( pkInfo->GetKey() );
	}
}

void PgResultMgr::BroadCast( SGroundKey const &rkKey, BM::Stream const& rkPacket )
{
	BM::CAutoMutex kLcok( m_kMutex );
	PgInstanceDungeonPlayInfo* pkPlayInfo = GetInfo( rkKey );
	if ( !pkPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find PlayInfo"));
		return;
	}

	pkPlayInfo->BroadCast( rkPacket );
}

bool PgResultMgr::ExpeditionStart( SGroundKey const & Key, VEC_RESULTINDUN const & ResultList)
{
	BM::CAutoMutex kLcok(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( pPlayInfo )
	{
		if( pPlayInfo->ResultStart(ResultList) )
		{
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find PlayInfo"));
	return false;
}

bool PgResultMgr::SetExpeditionResultItem( SGroundKey const & Key, BM::Stream & Pakcet )
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return pPlayInfo->SetExpeditionResultItem(Pakcet);
}

bool PgResultMgr::AddExpeditionPlayer( SGroundKey const & Key, VEC_JOININDUN const & PlayerList)
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_JOININDUN::const_iterator join_iter;
	for( join_iter = PlayerList.begin() ; join_iter != PlayerList.end() ; ++join_iter )
	{
		pPlayInfo->Join(*join_iter);
	}
	return true;
}

bool PgResultMgr::DeleteExpeditionPlayer(SGroundKey const & Key, BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pPlayInfo->Leave(CharGuid);
	return true;
}

void PgResultMgr::ExpeditionTick(DWORD const CurTime)
{
	BM::CAutoMutex kLcok(m_kMutex);
	// °б»к »уЕВ ѕчµҐАМЖ®.
	ContExpeditionResult::iterator iter = m_ExpeditionResult.begin();
	for( ; iter != m_ExpeditionResult.end() ; ++iter)
	{
		iter->second->OnTick(CurTime);
	}
}

bool PgResultMgr::TenderItem(SGroundKey const & Key, BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pPlayInfo->Tender(CharGuid);
	return true;
}

bool PgResultMgr::GiveupItem(SGroundKey const & Key, BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( Key );
	if ( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pPlayInfo->GiveupTender(CharGuid);
	return true;
}

bool PgResultMgr::ExpeditionClose(SGroundKey GndKey)
{
	BM::CAutoMutex Lock(m_kMutex);
	PgExpeditionIndunPlayInfo* pPlayInfo = GetExpeditionInfo( GndKey );
	if( !pPlayInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	EraseResult(GndKey);
	return true;
}
