
inline void PgIndun::InitWaitUser( VEC_GUID& rkCharGuidList )
{
	m_kWaitUserList.clear();

	VEC_GUID::iterator guid_itr;
	for ( guid_itr=rkCharGuidList.begin(); guid_itr!=rkCharGuidList.end(); ++guid_itr )
	{
		auto kPair = m_kWaitUserList.insert(SWaitUser(*guid_itr,E_OPENING_NONE));
		if ( !kPair.second )
		{
			INFO_LOG(BM::LOG_LV3, __FL__<<L"Insert Failed GUID["<<(*guid_itr)<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		}
	}

	switch(GetState())
	{
	case INDUN_STATE_PLAY:
		{
		}break;
	default:
		{
			this->SetState( INDUN_STATE_WAIT );
		}break;
	}
}

inline bool PgIndun::AddWaitUser( BM::GUID const & rkCharGuid )
{
	std::pair<ConWaitUser::iterator, bool> kPair;
	switch( m_eState )
	{
	case INDUN_STATE_WAIT:
		{
			kPair = m_kWaitUserList.insert(SWaitUser(rkCharGuid,E_OPENING_NONE));
		}break;
	case INDUN_STATE_READY:
		{
			kPair = m_kWaitUserList.insert(SWaitUser(rkCharGuid,E_OPENING_READY));
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
			return true;
		}break;
	}

	if( !kPair.second )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return kPair.second;
}

inline HRESULT PgIndun::ReleaseWaitUser( PgPlayer *pkUser )
{
	if ( !pkUser )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ConWaitUser::iterator user_itr = m_kWaitUserList.find(pkUser->GetID());
	if ( user_itr != m_kWaitUserList.end() )
	{
		if ( user_itr->kStep != m_eOpening )
		{
			// kSteep is a data not a key, so we can do const_cast there
			const_cast<SWaitUser*>(&(*user_itr))->kStep = m_eOpening;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
			return S_FALSE;
		}
		else
		{
			m_kWaitUserList.erase( user_itr );
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

inline bool PgIndun::IsAllUserSameStep(const EOpeningState eState)const
{
	if ( (eState == E_OPENING_NONE)
		|| (eState == E_OPENING_PLAY) )
	{
		if( !m_kWaitUserList.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		}
		return m_kWaitUserList.empty();
	}

	ConWaitUser::const_iterator user_itr;
	for ( user_itr=m_kWaitUserList.begin(); user_itr!=m_kWaitUserList.end(); ++user_itr )
	{
		if ( user_itr->kStep != eState )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}