inline DWORD PgGround::GetTimeStamp()
{
	return m_dwTimeStamp++;
}

inline bool PgGround::CheckUnit( BM::GUID const &rkGuid )const
{
	BM::CAutoMutex Lock(m_kRscMutex);

	// CheckUnit을 const함수로 만들어야 하는데...
	// GetUnit을 const함수로 만들수가 없어서 어쩔수 없이 const_cast사용
	PgGround *pkGnd = const_cast<PgGround*>(this);
	return (NULL != pkGnd->GetUnit(rkGuid));
}

inline CUnit* PgGround::GetUnit( BM::GUID const &rkGuid ) const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit* pkUnit = PgObjectMgr2::GetUnit(rkGuid);
	if( pkUnit )
	{
		return pkUnit;
	}
	return PgGroundResource::GetNPC(rkGuid);
}

inline PgPlayer* PgGround::GetUser( BM::GUID const &rkCharGuid ) const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit* pkUnit = PgObjectMgr2::GetUnit( UT_PLAYER, rkCharGuid );
	return (dynamic_cast<PgPlayer*>(pkUnit));
}

inline PgPet* PgGround::GetPet( BM::GUID const &kPetID )const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit *pkUnit = PgObjectMgr2::GetUnit( UT_PET, kPetID );
	return dynamic_cast<PgPet*>(pkUnit);
}

inline PgPet* PgGround::GetPet( PgPlayer * pkPlayer )const
{
	BM::CAutoMutex kLock( m_kRscMutex );
	return GetPet( pkPlayer->CreatePetID() );
}
inline PgSubPlayer* PgGround::GetSubPlayer( PgPlayer * pkPlayer )const
{
	BM::CAutoMutex kLock( m_kRscMutex );
	return GetSubPlayer( pkPlayer->SubPlayerID() );
}

inline PgSubPlayer* PgGround::GetSubPlayer( BM::GUID const &rkSubPlayerGuid )const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	CUnit *pkUnit = PgObjectMgr2::GetUnit( UT_SUB_PLAYER, rkSubPlayerGuid );
	return dynamic_cast<PgSubPlayer*>(pkUnit);
}

inline CUnit* PgGround::GetTarget( CUnit *pkUnit )const
{
	return PgObjectMgr2::GetUnit( pkUnit->GetTarget() );
}

inline void PgGround::GetTargetList(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iInNum = 1)
{
	int iNum = iInNum;
	if(iNum == 0)
	{
		iNum = 1;
	}

	CUnit::DequeTarget& rkTarget = pkUnit->GetTargetList();
	CUnit::DequeTarget::const_iterator itor = rkTarget.begin();
	int iCount = 0;
	while (itor != rkTarget.end() && iCount < iNum)
	{
		CUnit* pkTarget = GetUnit(itor->kGuid);
		if (pkTarget != NULL)
		{
			rkUnitArray.Add(pkTarget);
		}
		++itor;
		++iCount;
	}
}
/*
inline void PgGround::GetFirstUnit(EUnitType const eUnitType, CONT_OBJECT_MGR_UNIT::iterator& rkItor)
{
	PgObjectMgr::GetFirstUnit(eUnitType, rkItor);
}

inline CUnit* PgGround::GetNextUnit(EUnitType const eUnitType, CONT_OBJECT_MGR_UNIT::iterator& rkItor)
{
	return PgObjectMgr::GetNextUnit(eUnitType, rkItor);
}
*/
inline void PgGround::SetUserLife( PgPlayer *pkUser, int const iHP, int const iMP, DWORD const dwFlag )
{
	pkUser->SetAbil(AT_HP, iHP );
	pkUser->SetAbil(AT_MP, iMP );
	WORD wType[2] = { AT_HP, AT_MP };
	pkUser->SendAbiles(wType, 2, dwFlag );
}

inline void PgGround::SendContentsToDirect( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy )
{
	BM::Stream kNPacket(wType, pkUnit->GetID());
	kNPacket.Push(*pkNfy);
	SendToContents(kNPacket);
}

inline bool PgGround::GroundWeight(int const iWeight)
{
	if(m_iGroundWeight==iWeight)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	m_iGroundWeight = iWeight;
	return true;
}

inline void PgGround::TunningLevel(int const iLevel)
{
	m_iTunningLevel = iLevel;
}

inline int PgGround::GroundTunningNo()const
{
	return m_iGroundTunningNo;
}

inline PgSmallArea* PgGround::GetArea( int const iAreaIndex )const
{
	if ( iAreaIndex >= 0 && (iAreaIndex < (int)m_kAreaCont.size()) )
	{
		return m_kAreaCont.at(iAreaIndex);
	}
	else
	{
		if ( PgSmallArea::BIG_AREA_INDEX == iAreaIndex )
		{
			return m_pkBigArea;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

inline void PgGround::SetMapLevel( int const iMapLevel )
{
	if ( (iMapLevel >= 0) && ( iMapLevel < MAX_MONSTERBAG_ELEMENT ) )
	{
		m_iMapLevel = iMapLevel;
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"Error!! MapLevel["<<iMapLevel<<L"] Ground["<<GetGroundNo()<<L"-"<<GroundKey().Guid()<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("SetMapLevel Failed!"));
	}
}

inline float PgGround::GetAutoHealMultiplier()const
{
	switch( this->GetAttr() )
	{
	case GATTR_VILLAGE:
	case GATTR_EMPORIA:
	case GATTR_MYHOME:
	case GATTR_HOMETOWN:
		{
		}break;
	default:
		{
			return ms_fAutoHealMultiplier_NotInViliage;
		}break;
	}
	return 1.0f;
}
