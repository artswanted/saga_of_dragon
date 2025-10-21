#include "Lohengrin/GameTime.h"
#include "Global.h"
#include "PgEmporiaPack.h"
#include "Item/PgPostManager.h"

template< size_t iNum >
SEmporiaBattleGroundInfo const PgEmporiaPack< iNum >::ms_kBattleGroundInfo_Challenge( 9930100, WAR_TYPE_DESTROYCORE );

template< size_t iNum >
SEmporiaBattleGroundInfo const PgEmporiaPack< iNum >::ms_kBattleGroundInfo_Emporia( 9930200, WAR_TYPE_PROTECTDRAGON );
	
template< size_t iNum >
PgEmporiaPack< iNum >::PgEmporiaPack( BM::GUID const &kID, int const iState, char const nBattlePeriodForWeeks, BM::GUID const &kBattleID, BM::DBTIMESTAMP_EX const &kBattleDate, BM::GUID const &kPrevBattleID,  SGroundKey const &kBaseMapKey )
:	m_kID(kID)
,	m_kBaseMapKey(kBaseMapKey)
,	m_iState(EMPORIA_CLOSE)
,	m_i64NextStateChangeTime(0i64)
,	m_nBattlePeriodsForWeeks(nBattlePeriodForWeeks)
,	m_kBattleID(kBattleID)
,	m_i64BattleTime(CGameTime::S20000101)
,	m_i64TotalChallengeExp(0i64)
,	m_kPrevBattleID(kPrevBattleID)
{
	CGameTime::DBTimeEx2SecTime( kBattleDate, m_i64BattleTime, CGameTime::DEFAULT );
	PgEmporiaPack< iNum >::SetState( iState, false );
}

template< size_t iNum >
PgEmporiaPack< iNum >::~PgEmporiaPack(void)
{
}

template< size_t iNum >
void PgEmporiaPack< iNum >::Save()
{
	BM::DBTIMESTAMP_EX kBattleDate;
	CGameTime::SecTime2DBTimeEx( m_i64BattleTime, kBattleDate, CGameTime::DEFAULT );

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_PACK, _T("EXEC [dbo].[up_SaveEmporiaPack]") );
	kQuery.QueryOwner( GetID() );
	kQuery.InsertQueryTarget( GetID() );

	kQuery.PushStrParam( GetID() );
	kQuery.PushStrParam( m_iState );
	kQuery.PushStrParam( m_nBattlePeriodsForWeeks );
	kQuery.PushStrParam( m_kBattleID );
	kQuery.PushStrParam( kBattleDate );
	kQuery.PushStrParam( m_kPrevBattleID );
	g_kCoreCenter.PushQuery( kQuery );
}

template< size_t iNum >
void PgEmporiaPack< iNum >::SaveElement( size_t const iIndex, SEmporiaGuildInfo const *pOldGuildInfo )
{
	if ( iIndex < iNum )
	{
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA, _T("EXEC [dbo].[up_SaveEmporia]") );
		kQuery.QueryOwner( GetID() );
		kQuery.InsertQueryTarget( GetID() );

		kQuery.PushStrParam( GetID() );
		kQuery.PushStrParam( static_cast<BYTE>(iIndex+1) );
		kQuery.PushStrParam( m_kEmporia[iIndex].GetGate() );
		kQuery.PushStrParam( m_kEmporia[iIndex].GetOwnerGuildID() );
		kQuery.PushStrParam( m_kEmporia[iIndex].GetHaveDate() );

		if ( pOldGuildInfo )
		{
			kQuery.contUserData.Push( true );
			pOldGuildInfo->WriteToPacket( kQuery.contUserData );
		}
		else
		{
			kQuery.contUserData.Push( false );
		}

		g_kCoreCenter.PushQuery( kQuery );
	}
}

template< size_t iNum >
void PgEmporiaPack< iNum >::SaveBattleRet( BM::GUID const &kWinGuildID, BM::GUID const &kLoseGuildID, short const nIndex )
{
	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_TOURNAMENT, _T("EXEC [dbo].[up_SaveEmporia_Tournament]") );
	kQuery.InsertQueryTarget( GetID() );
	kQuery.QueryOwner( GetID() );

	kQuery.PushStrParam( GetID() );
	kQuery.PushStrParam( GetBattleID() );
	kQuery.PushStrParam( nIndex );
	kQuery.PushStrParam( kWinGuildID );
	kQuery.PushStrParam( kLoseGuildID );

	g_kCoreCenter.PushQuery( kQuery );
}

template< size_t iNum >
void PgEmporiaPack< iNum >::SaveSwapEmporia( size_t const iLeft, size_t const iRight )
{
	if (	(iLeft <= iNum)
		&&	(iRight <= iNum)
		)
	{
		SEmporiaGuildInfo kLeftOwner;
		m_kEmporia[iLeft].GetOwner( kLeftOwner );

		SEmporiaGuildInfo kRightOwner;
		m_kEmporia[iRight].GetOwner( kRightOwner );

		m_kEmporia[iLeft].SetOwner( kRightOwner );
		m_kEmporia[iRight].SetOwner( kLeftOwner );

		CEL::DB_QUERY_TRAN kTran;
		kTran.DBIndex(DT_PLAYER);
		kTran.QueryOwner(GetID());
		kTran.QueryType(DQT_SWAP_EMPORIA);

		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA, L"EXEC [dbo].[up_SaveEmporia]" );
			kQuery.QueryOwner( GetID() );
			kQuery.InsertQueryTarget( GetID() );

			kQuery.PushStrParam( GetID() );
			kQuery.PushStrParam( static_cast<BYTE>(iLeft+1) );
			kQuery.PushStrParam( m_kEmporia[iLeft].GetGate() );
			kQuery.PushStrParam( m_kEmporia[iLeft].GetOwnerGuildID() );
			kQuery.PushStrParam( m_kEmporia[iLeft].GetHaveDate() );

			kQuery.contUserData.Push(false);

			kTran.push_back(kQuery);
		}

		{
			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA, L"EXEC [dbo].[up_SaveEmporia]" );
			kQuery.QueryOwner( GetID() );
			kQuery.InsertQueryTarget( GetID() );

			kQuery.PushStrParam( GetID() );
			kQuery.PushStrParam( static_cast<BYTE>(iRight+1) );
			kQuery.PushStrParam( m_kEmporia[iRight].GetGate() );
			kQuery.PushStrParam( m_kEmporia[iRight].GetOwnerGuildID() );
			kQuery.PushStrParam( m_kEmporia[iRight].GetHaveDate() );

			kQuery.contUserData.Push(false);

			kTran.push_back(kQuery);
		}

		g_kCoreCenter.PushQuery(kTran);
	}
}

template< size_t iNum >
void PgEmporiaPack< iNum >::SaveMercenary( int const iBattleIndex, BM::GUID const & rkGuildID, BM::GUID const & rkCharID )const
{
	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_MERCENARY, _T("EXEC [dbo].[up_SaveEmporia_Mercenary]") );
	kQuery.QueryOwner( GetID() );
	kQuery.InsertQueryTarget( GetID() );

	kQuery.PushStrParam( GetID() );
	kQuery.PushStrParam( m_kBattleID );
	kQuery.PushStrParam( iBattleIndex );
	kQuery.PushStrParam( rkGuildID );
	kQuery.PushStrParam( rkCharID );
	g_kCoreCenter.PushQuery( kQuery );
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::InitTournament( BM::GUID const &kBattleID, CONT_TBL_EMPORIA_TOURNAMENT const &kContTournament, CONT_TBL_EMPORIA_CHALLENGE_BATTLE const &kBattleElement, CONT_EMPORIA_FROMGUILD &kContGuild )
{
	BM::CAutoMutex kLock( m_kMutex );

	bool bInsertGuild = true;

	switch ( m_iState )
	{
	case EMPORIA_CLOSE:
		{
			return S_OK;
		}break;
	case EMPORIA_PEACE:
		{
			// 이기간에는 PrevBattleID와 같아야 함
			if ( kBattleID != m_kPrevBattleID )
			{
				return S_OK;
			}

			bInsertGuild = false;
		}break;
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			if ( kBattleID == m_kBattleID )
			{
				CONT_TBL_EMPORIA_CHALLENGE_BATTLE::const_iterator battle_itr = kBattleElement.begin();
				for ( ; battle_itr != kBattleElement.end() ; ++battle_itr )
				{
					auto kPair = kContGuild.insert( std::make_pair( battle_itr->kGuildInfo.kGuildID, SEmporiaHaveInfo(battle_itr->kGuildInfo, GetID())) );
					if ( true == kPair.second )
					{
						AddChallenge( dynamic_cast<SEmporiaChallenge const*>(&(*battle_itr)), true );
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("This Guild<") << battle_itr->kGuildInfo.kGuildID << _T("> Overlap EmporiaID<") << GetID() << _T(">") );
					}
				}
			}
			else
			{
				return S_OK;
			}
		}break;
	default:
		{
			// 이기간에는 현재 BattleID와 같아야 함
			if ( kBattleID != m_kBattleID )
			{
				return S_OK;
			}
		}break;
	}

	short const iMaxTournamentGameCount = static_cast<short>(EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT - 1);

	short iCheckIndex = 0;
	__int64 i64Temp = 0i64;
	bool IsOwnerShip = false;

	SEmporiaGuildInfo kEmporiaTournamentAttackGuildInfo;
	SEmporiaGuildInfo kEmporiaTournamentMissingGuildInfo;

	PgEmporiaTournament::CONT_INIT_TIME	kContInitTime;
	PgEmporiaTournament::CONT_INIT_TIME	kContInitTime2;
	PgEmporiaTournament::CONT_INIT_GUILD kContInitGuild;
	kContInitTime.reserve( static_cast<size_t>(iMaxTournamentGameCount) );

	VEC_GUID kWinGuildList;
	VEC_GUID kWinGuildList2;
	kWinGuildList.reserve( static_cast<size_t>(iMaxTournamentGameCount) );

	SET_GUID kLoseGuildList;

	CONT_TBL_EMPORIA_TOURNAMENT::const_iterator tour_itr = kContTournament.begin();
	for ( ; tour_itr!= kContTournament.end() ; ++tour_itr )
	{
		if ( iCheckIndex == tour_itr->nIndex )
		{
			CGameTime::DBTimeEx2SecTime( tour_itr->kBattleDate, i64Temp, CGameTime::DEFAULT );

			if ( true == IsOwnerShip )
			{
				size_t const iGrade = static_cast<size_t>(iCheckIndex) - PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX;
				if ( iGrade < iNum )
				{
					SEmporiaGuildInfo kOwnerGuildInfo;
					m_kEmporia[iGrade].GetOwner( kOwnerGuildInfo );

					kContInitTime2.push_back( i64Temp );
					kWinGuildList2.push_back( tour_itr->kWinGuildID );

					if ( BM::GUID::IsNotNull(tour_itr->kWinGuildID) )
					{
						bool const bAttackerWin = ( tour_itr->kWinGuildID == kEmporiaTournamentAttackGuildInfo.kGuildID );
						if ( true == bAttackerWin )
						{
							CONT_EMPORIA_FROMGUILD::const_iterator guild_itr = kContGuild.find( tour_itr->kLoseGuildID );
							if ( guild_itr != kContGuild.end() )
							{
								kContInitGuild.insert( std::make_pair( static_cast<size_t>(tour_itr->nIndex), guild_itr->second.kGuildInfo) );
							}
							else
							{
								kContInitGuild.insert( std::make_pair( static_cast<size_t>(tour_itr->nIndex), kEmporiaTournamentMissingGuildInfo) );
							}				
						}
						else
						{
							kEmporiaTournamentAttackGuildInfo = kOwnerGuildInfo;
							kContInitGuild.insert( std::make_pair( static_cast<size_t>(tour_itr->nIndex), kOwnerGuildInfo) );
						}
//						kOwnerShip.SetWinGuild( bAttackerWin );

						if ( 0 == iGrade )
						{
							kLoseGuildList.insert( tour_itr->kLoseGuildID );
						}
					}
					else
					{
						kContInitGuild.insert( std::make_pair( static_cast<size_t>(tour_itr->nIndex), kOwnerGuildInfo) );
						kEmporiaTournamentAttackGuildInfo = SEmporiaGuildInfo();
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Emporia<") << GetID() << _T("> Battle Index<") << iCheckIndex << _T("> Error") );
				}
			}
			else
			{
				kContInitTime.push_back( i64Temp );
				kWinGuildList.push_back( tour_itr->kWinGuildID );
				kLoseGuildList.insert( tour_itr->kLoseGuildID );

				if ( 0 == iCheckIndex )
				{
					if ( BM::GUID::IsNotNull( tour_itr->kWinGuildID) )
					{
						CONT_TBL_EMPORIA_CHALLENGE_BATTLE::const_iterator battle_itr = kBattleElement.begin();
						for ( ; battle_itr != kBattleElement.end() ; ++battle_itr )
						{
							if ( battle_itr->kGuildInfo.kGuildID == tour_itr->kWinGuildID )
							{
								kEmporiaTournamentAttackGuildInfo = battle_itr->kGuildInfo;
							}
							else
							{
								if ( EMBATTLE_LOSTEMPORIA == battle_itr->iState )
								{
									kEmporiaTournamentMissingGuildInfo = battle_itr->kGuildInfo;
									kLoseGuildList.insert( kEmporiaTournamentMissingGuildInfo.kGuildID );
								}
							}
						}
					}
				}
			}
		}
		else
		{
			// 이러면 안되는데.
			// 전쟁을 만드는 중에 서버가 다운되서 DB에 빠졌나....
			kContInitTime.push_back( _I64_MAX );
			kWinGuildList.push_back( BM::GUID::NullData() );
		}

		++iCheckIndex;
		if ( !IsOwnerShip && (iCheckIndex >= iMaxTournamentGameCount) )
		{
			iCheckIndex = static_cast<short>(PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX);
			IsOwnerShip = true;
		}
	}
	
	if ( EMPORIA_TERM_OF_CHALLENGE != m_iState )
	{
		CONT_TBL_EMPORIA_CHALLENGE_BATTLE::const_iterator battle_itr = kBattleElement.begin();
		for ( ; battle_itr != kBattleElement.end() ; ++battle_itr )
		{
			if ( true == bInsertGuild )
			{
				SET_GUID::const_iterator guid_itr = kLoseGuildList.find( battle_itr->kGuildInfo.kGuildID );
				if ( guid_itr == kLoseGuildList.end() )
				{
					auto kPair = kContGuild.insert( std::make_pair( battle_itr->kGuildInfo.kGuildID, SEmporiaHaveInfo(battle_itr->kGuildInfo, GetID())) );
					if ( !kPair.second )
					{
						if ( EMPORIA_OWNERSHIP_OF_TOURNAMENT != m_iState )
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("This Guild<") << battle_itr->kGuildInfo.kGuildID << _T("> Overlap EmporiaID<") << GetID() << _T(">") );
							continue;
						}
					}
				}
			}

			if ( (0 < battle_itr->iState) && (battle_itr->iState <= EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT) )
			{
				kContInitGuild.insert( std::make_pair( EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMINDEX[battle_itr->iState-1], battle_itr->kGuildInfo) );
			}
		}
	}

	if ( iNum > kContInitTime2.size() )
	{
		if ( true == m_kEmporia[kContInitTime2.size()].IsOpen() )
		{
			kContInitTime2.push_back( 0i64 );
		}
	}

	if ( S_OK != m_kTournament.Init( kContInitTime, kContInitTime2 ) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error Emporia ID = ") << GetID() );
		return E_FAIL;
	}

	if ( kContInitGuild.size() && (S_OK != m_kTournament.InitGuild( kContInitGuild )) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error Emporia ID = ") << GetID() );
		return E_FAIL;
	}

	{
		size_t iIndex = kWinGuildList.size() - 1;
		VEC_GUID::const_reverse_iterator r_win_itr = kWinGuildList.rbegin();
		for ( ; r_win_itr != kWinGuildList.rend() ; ++r_win_itr )
		{
			if ( BM::GUID::IsNotNull(*r_win_itr) )
			{
				m_kTournament.SetWinner( iIndex, *r_win_itr );
			}

			--iIndex;
		}

		iIndex = PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX;
		VEC_GUID::const_iterator win_itr = kWinGuildList2.begin();
		for ( ; win_itr != kWinGuildList2.end() ; ++win_itr )
		{
			if ( BM::GUID::IsNotNull(*win_itr) )
			{
				m_kTournament.SetWinner( iIndex, *win_itr );
			}

			++iIndex;
		}
	} 

	int const iTempState = m_iState;
	m_iState = EMPORIA_CLOSE;
	this->SetState( iTempState, false );

	return S_OK;
}

template< size_t iNum >
void PgEmporiaPack< iNum >::CheckData()
{
	BM::CAutoMutex kLock( m_kMutex );

	switch ( m_iState )
	{
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			if ( m_kTournament.IsEmpty() )
			{
				m_iState = EMPORIA_PEACE;
				SetState( EMPORIA_TERM_OF_CHALLENGE, true );
			}
		}break;
	default:
		{
			
		}break;
	}
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::InitMercenary( CONT_TBL_EM_MERCENARY const & rkContMercenary )
{
	BM::CAutoMutex kLock( m_kMutex );

	CONT_TBL_EM_MERCENARY::key_type kKey;
	kKey.kEmporiaID = GetID();
	kKey.kBattleID = m_kBattleID;
	CONT_TBL_EM_MERCENARY::const_iterator it_mecenary = rkContMercenary.find(kKey);
	if(it_mecenary == rkContMercenary.end())
	{
		return E_FAIL;
	}

	m_kTournament.InitMercenary(it_mecenary->second);
	return S_OK;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::BattleStart( __int64 const i64Time )
{
	BM::CAutoMutex kLock( m_kMutex );

	switch ( m_iState )
	{
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			if ( FAILED(SetState( EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT, true )) )
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia<") << GetID() << _T("> Error!!") );
				break;
			}
		}// no break
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
		{
			if ( FAILED(SetState( EMPORIA_CHALLENGE_OF_TOURNAMENT, true )) )
			{	
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia<") << GetID() << _T("> Error!!") );;
			}
		}// no break
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			int iReadyMinTime = 10;
			g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EmporiaBattleReadyMinTime, iReadyMinTime );
			__int64 const EMPORIA_BATTLE_READY_TIME = ( CGameTime::MINUTE * static_cast<__int64>(iReadyMinTime) );

			m_kTournament.SetBattleTimeAsSoon( i64Time + EMPORIA_BATTLE_READY_TIME);
			m_i64NextStateChangeTime = i64Time;
			Update( i64Time );
			return S_OK;
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Emporia<") << GetID() << _T("> Error!! Cause : State<") << m_iState << _T(">") );
		}break;
	}
	return E_FAIL;
}

inline wchar_t const* GetFormatText(wchar_t const* pkFormat, int const iValue)
{
	static wchar_t buf[MAX_PATH] = {0,};
	if(!pkFormat)
	{
		return L"";
	}

	swprintf_s(buf, _countof(buf), pkFormat, iValue);
	return buf;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SetState( int const iState, bool bCall )
{
	if ( iState == m_iState )
	{
		return E_FAIL;
	}

	switch ( iState )
	{
	case EMPORIA_CLOSE:
		{
            BM::GUID const kCloseBattleID(m_kBattleID);
			m_i64NextStateChangeTime = _I16_MAX;
			m_kBattleID.Clear();
			m_i64BattleTime = CGameTime::S20000101;

			switch( m_iState )
			{
			case EMPORIA_TERM_OF_CHALLENGE:
			case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
				{
					CONT_CHALLENGE_BATTLE::const_iterator chl_itr = m_kContChallenge.begin();
					for ( ; chl_itr!=m_kContChallenge.end() ; ++chl_itr )
					{
						// 도전한 놈들의 경험치는 모두 환불해 주어야 한다.
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_THROW, _T("EXEC [dbo].[up_SaveEmporia_BattleRefundCost]") );
						kQuery.InsertQueryTarget( chl_itr->second.kGuildInfo.kGuildID );
						kQuery.QueryOwner( chl_itr->second.kGuildInfo.kGuildID );

						kQuery.PushStrParam( kCloseBattleID );
						kQuery.PushStrParam( chl_itr->second.kGuildInfo.kGuildID );

                        kQuery.contUserData.Push( BM::GUID::NullData() );
						kQuery.contUserData.Push( chl_itr->second.kGuildInfo.kGuildID );
						kQuery.contUserData.Push( GetID() );
						kQuery.contUserData.Push( static_cast<BYTE>(GCR_System) );
                        kQuery.contUserData.Push( static_cast<__int64>(0i64) );

						g_kCoreCenter.PushQuery( kQuery );
					}
                    m_kContChallenge.clear();
				}break;
			case EMPORIA_CHALLENGE_OF_TOURNAMENT:
			case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
				{
					// 이때도 마찬가지로 도전한 놈들의 경험치는 모두 환불해 주어야 한다.
					PgEmporiaTournament::CONT_GUILDINFO kContGuildInfo;
 					m_kTournament.GetGuildInfo( kContGuildInfo );
 
 					PgEmporiaTournament::CONT_GUILDINFO::iterator guild_itr = kContGuildInfo.begin();
 					for ( ; guild_itr != kContGuildInfo.end() ; ++guild_itr )
 					{
 						// 도전한 놈들의 경험치는 모두 환불해 주어야 한다.
 						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_THROW, _T("EXEC [dbo].[up_SaveEmporia_BattleRefundCost]") );
 						kQuery.InsertQueryTarget( guild_itr->kGuildID );
						kQuery.QueryOwner( guild_itr->kGuildID );
 
 						kQuery.PushStrParam( kCloseBattleID );
 						kQuery.PushStrParam( guild_itr->kGuildID );

                        kQuery.contUserData.Push( BM::GUID::NullData() );
 						kQuery.contUserData.Push( guild_itr->kGuildID );
 						kQuery.contUserData.Push( GetID() );
 						kQuery.contUserData.Push( static_cast<BYTE>(GCR_System) );
                        kQuery.contUserData.Push( static_cast<__int64>(0i64) );
 
 						g_kCoreCenter.PushQuery( kQuery );
 					}
				}break;
			}
			
			for ( size_t i = 0; i<iNum ; ++i )
			{
				SEmporiaGuildInfo kOwnerGuildInfo;
				m_kEmporia[i].GetOwner( kOwnerGuildInfo );

				if ( true == m_kEmporia[i].Open( false ) )
				{
					SaveElement( i, kOwnerGuildInfo.IsEmpty() ? NULL : &kOwnerGuildInfo );
				}
			}
		}break;
	case EMPORIA_PEACE:
		{
			switch ( m_iState )
			{
			case EMPORIA_TERM_OF_CHALLENGE:
				{
					if ( 0 < m_kContChallenge.size() )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Call Error OldState<") << m_iState << _T("> NewState<") << iState << _T(">") );
						return E_FAIL;
					}
				}// break을 사용하지 않는다.
			case EMPORIA_CLOSE:
			case EMPORIA_PEACE:
			case EMPORIA_CHALLENGE_OF_TOURNAMENT:
			case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
				{
					if ( true == bCall )
					{
						__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
						__int64 const i64LimitTime = m_i64BattleTime - PgEmporiaTournament::EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME;
						if ( i64NowTime >= i64LimitTime )
						{
							// 시간이 교체 되어야 한다!
							__int64 const i64Gap = i64NowTime - m_i64BattleTime;
							__int64 const i64AddTime = static_cast<__int64>(m_nBattlePeriodsForWeeks) * CGameTime::OneWeek;
							__int64 const i64Weight = ((i64Gap > i64AddTime ) ? ((i64Gap / i64AddTime) + 1i64) : 1i64);

							m_i64BattleTime = m_i64BattleTime + (i64AddTime * i64Weight);	
						}

						m_kPrevBattleID = m_kBattleID;
						m_kBattleID.Generate();
					}
				}break;
			default:
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Call Error OldState<") << m_iState << _T("> NewState<") << iState << _T(">") );
					return E_FAIL;
				}break;
			}

			m_i64NextStateChangeTime = ( m_i64BattleTime - PgEmporiaTournament::EMPORIA_BATTLE_CHALLENGE_STARTTIME_BEFORE_BATTLETIME );
		}break;
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			m_kContBattle_Ing.clear();

			if ( true == bCall )
			{
				size_t const iMaxTournamentGameCount = EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT - 1;

				PgEmporiaTournament::CONT_INIT_TIME kContInitTime;
				kContInitTime.reserve(iMaxTournamentGameCount);

				// 도전자 토너먼트를 만든다.
				__int64 i64BattleTime = 0i64;
				for ( size_t i=0; i<iMaxTournamentGameCount; ++i )
				{
					i64BattleTime = m_i64BattleTime + EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TIMETERM[i];
					kContInitTime.push_back( i64BattleTime );

					// 저장1. Tournament를 DB에 저장해야 한다.
					BM::DBTIMESTAMP_EX kBattleDate;
					CGameTime::SecTime2DBTimeEx( i64BattleTime, kBattleDate, CGameTime::DEFAULT );

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CREATE_EMPORIA_TOURNAMENT, _T("EXEC [dbo].[up_CreateEmporia_Tournament]") );
					kQuery.InsertQueryTarget( GetID() );
					kQuery.QueryOwner( GetID() );

					kQuery.PushStrParam( GetID() );
					kQuery.PushStrParam( GetBattleID() );
					kQuery.PushStrParam( static_cast<short>(i) );
					kQuery.PushStrParam( kBattleDate );

					g_kCoreCenter.PushQuery( kQuery );
				}

				// 엠포리아 토너먼트를 만든다.
				PgEmporiaTournament::CONT_INIT_TIME kContInitTime2;
				PgEmporiaTournament::CONT_INIT_GUILD kContInitGuild;
				size_t index = 0;
				for ( ; index<iNum; ++index )
				{
					if ( !m_kEmporia[index].IsHaveOwner() )
					{
						if ( m_kEmporia[index].IsOpen() )
						{
							kContInitTime2.push_back( 0i64 );
						}
						break;
					}

					size_t const iBattleIndex = PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX + index;
					i64BattleTime = m_i64BattleTime + EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM[index];
					kContInitTime2.push_back( i64BattleTime );

					SEmporiaGuildInfo kOwnerGuildInfo;
					m_kEmporia[index].GetOwner( kOwnerGuildInfo );
					kContInitGuild.insert( std::make_pair( iBattleIndex, kOwnerGuildInfo ) );

					// Emporia Tournament를 DB에 저장해야 한다.
					BM::DBTIMESTAMP_EX kBattleDate;
					CGameTime::SecTime2DBTimeEx( i64BattleTime, kBattleDate, CGameTime::DEFAULT );

					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CREATE_EMPORIA_TOURNAMENT, _T("EXEC [dbo].[up_CreateEmporia_Tournament]") );
					kQuery.InsertQueryTarget( GetID() );
					kQuery.QueryOwner( GetID() );

					kQuery.PushStrParam( GetID() );
					kQuery.PushStrParam( GetBattleID() );
					kQuery.PushStrParam( iBattleIndex );
					kQuery.PushStrParam( kBattleDate );

					g_kCoreCenter.PushQuery( kQuery );
				}

				m_kTournament.Init( kContInitTime, kContInitTime2 );
				m_kTournament.InitGuild( kContInitGuild );
			}

			m_i64NextStateChangeTime = ( m_i64BattleTime - PgEmporiaTournament::EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME );
		}break;
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
		{
			if ( true == bCall )
			{
				if ( m_kContChallenge.empty() )
				{
					// 도전한 길드가 한개도 없으면.....
					// 이번 전쟁은 그냥 스킵 되어야 한다.
					CAUTION_LOG( BM::LOG_LV6, L"* Emporia<" << GetID() << "> Challenge Guild Count is 0 Skip Emporia Battle in this cycle." );
					return this->SetState( EMPORIA_PEACE, true );
				}

				// 토너먼트를 만들어야 해~~~
				// 토너먼트가 만들어 지면 자동으로 메일을 보내야 한다.

				// 1. 메일 내용 만들기(공통적인 사항만 여기서 만들어 둔다.)
				std::wstring wstrMail_Sender;
				std::wstring wstrMail_Title;
				std::wstring wstrMail_Contents_Top;
				std::wstring wstrMail_Gold;
				BM::vstring vstr;
				
				{
					::GetDefString( ms_iChallengeResultMail_Sender, wstrMail_Sender );
					::GetDefString( ms_iChallengeResultMail_Title, wstrMail_Title );
					::GetDefString( ms_iChallengeResultMail_Contents_Top, wstrMail_Contents_Top );
					::GetDefString( ms_iChallengeResultMail_Gold, wstrMail_Gold );
					wstrMail_Contents_Top += L'\n';

					// 도전 길드수
					if ( true == ::GetDefString( ms_iChallengeResultMail_Contents_1, vstr ) )
					{
						vstr += m_kContChallenge.size();
						vstr += L'\n';
						wstrMail_Contents_Top += vstr.operator std::wstring const&();
					}
					

					// 토너먼트 평균 길드골드
					if ( true == ::GetDefString( ms_iChallengeResultMail_Contents_2, vstr ) )
					{
						if ( m_kContChallenge.size() )
						{
							__int64 i64AverChallengeGold = m_i64TotalChallengeExp / static_cast<__int64>(m_kContChallenge.size());
							i64AverChallengeGold = !wstrMail_Gold.empty() ? i64AverChallengeGold/10000 : i64AverChallengeGold;
							vstr += i64AverChallengeGold;
						}
						else
						{
							vstr += 0;
						}
						
						vstr += wstrMail_Gold;
						vstr += L'\n';
						wstrMail_Contents_Top += vstr.operator std::wstring const&();
					}
				}

				// 2. 도전 길드골드 순서로 길드를 정렬
				std::multiset< SEmporiaChallenge, SEmporiaChallenge_Sorter > kSorter;
				CONT_CHALLENGE_BATTLE::const_iterator chl_itr = m_kContChallenge.begin();
				for ( ; chl_itr!=m_kContChallenge.end() ; ++chl_itr )
				{
					kSorter.insert( chl_itr->second );
				}

				PgEmporiaTournament::CONT_INIT_GUILD kContInitGuild;

				// 저장3. 이제 길드들을 저장(넣어준다) ( 2위 vs 7위, 4위 vs 5위, 1위 vs 8위, 3위 vs 6위 )
				std::multiset< SEmporiaChallenge, SEmporiaChallenge_Sorter >::iterator sort_itr = kSorter.begin();
				size_t iRank = 0;
				SYSTEMTIME kTempTime;
				for ( ; sort_itr!=kSorter.end() ; ++sort_itr )
				{
					bool bChallenge = false;
					if ( iRank < EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT )
					{
						bChallenge = true;
						kContInitGuild.insert( std::make_pair( EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMINDEX[iRank], sort_itr->kGuildInfo ) );

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_BATTLE_STATE, _T("EXEC [dbo].[up_SaveEmporia_BattleState]") );
						kQuery.InsertQueryTarget( sort_itr->kGuildInfo.kGuildID );
						kQuery.QueryOwner( sort_itr->kGuildInfo.kGuildID );

						kQuery.PushStrParam( GetBattleID() );
						kQuery.PushStrParam( sort_itr->kGuildInfo.kGuildID );
						kQuery.PushStrParam( static_cast<char>( EMBATTLE_REGIST_TOURNAMENT_1 + iRank ) );

						// 메일을 보내야 한다-------------------------------------
						std::wstring wstrMail_Contents = wstrMail_Contents_Top;

						// 도전 길드골드
						if ( true == ::GetDefString( ms_iChallengeResultMail_Contents_3, vstr ) )
						{
							vstr += !wstrMail_Gold.empty() ? sort_itr->i64Cost/10000 : sort_itr->i64Cost;
							vstr += wstrMail_Gold;
							vstr += L'\n';
							wstrMail_Contents += vstr.operator std::wstring const&();
						}

						// 전쟁 날짜
						if ( true == ::GetDefString( ms_iChallengeResultMail_Contents_Date, vstr ) )
						{
							size_t iParentIndex = 0;
							if ( S_OK == PgEmporiaTournamentElement::GetParentIndex(EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMINDEX[iRank], iParentIndex) )
							{
								__int64 i64TempTime = 0i64;
								m_kTournament.GetBattleTime( iParentIndex, i64TempTime );
								CGameTime::SecTime2SystemTime( i64TempTime, kTempTime, CGameTime::DEFAULT );

								vstr += kTempTime.wYear;
								vstr += L'-';
								vstr += kTempTime.wMonth;
								vstr += L'-';
								vstr += kTempTime.wDay;
								vstr += L' ';
								vstr += GetFormatText(L"%02d",kTempTime.wHour);
								vstr += L':';
								vstr += GetFormatText(L"%02d",kTempTime.wMinute);
								vstr += L':';
								vstr += GetFormatText(L"%02d",kTempTime.wSecond);
								vstr += L'\n';
								wstrMail_Contents += vstr.operator std::wstring const&();
							}
						}

						kQuery.contUserData.Push( wstrMail_Sender );
						kQuery.contUserData.Push( wstrMail_Title );
						kQuery.contUserData.Push( wstrMail_Contents );
						// ---------------------------------------------------------

						g_kCoreCenter.PushQuery( kQuery );
					}
					else
					{
						bChallenge = false;
						// 토너먼트에 참여하지 못하는 길드는 환불을 해주어야 한다.
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_THROW, _T("EXEC [dbo].[up_SaveEmporia_BattleRefundCost]") );
						kQuery.InsertQueryTarget( sort_itr->kGuildInfo.kGuildID );
						kQuery.QueryOwner( sort_itr->kGuildInfo.kGuildID );

						kQuery.PushStrParam( GetBattleID() );
						kQuery.PushStrParam( sort_itr->kGuildInfo.kGuildID );

                        kQuery.contUserData.Push( BM::GUID::NullData() );
						kQuery.contUserData.Push( sort_itr->kGuildInfo.kGuildID );
						kQuery.contUserData.Push( GetID() );
						kQuery.contUserData.Push( static_cast<BYTE>(GCR_Failed) );
                        kQuery.contUserData.Push( static_cast<__int64>(0i64) );

						g_kCoreCenter.PushQuery( kQuery );
					}

					//Result System Mesage Send
					BM::Stream kSendPacket;
					kSendPacket.Push( NOTICE_EM_CHALLENGE_RESULT );
					kSendPacket.Push( bChallenge );

					BM::Stream kNotiPacket;
					kNotiPacket.Push(E_NoticeMsg_SendPacket);
					kNotiPacket.Push(NOTICE_GUILD);
					kNotiPacket.Push(sort_itr->kGuildInfo.kGuildID);
					kNotiPacket.Push(kSendPacket);
					SendToNotice(kNotiPacket);

					++iRank;
				}

				for ( size_t i=0; i<iNum; ++i )
				{
					if ( !m_kEmporia[i].IsHaveOwner() )
					{
						break;
					}

					SEmporiaGuildInfo kOwnerGuildInfo;
					m_kEmporia[i].GetOwner( kOwnerGuildInfo );

					size_t const iBattleIndex = PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX + i;
					kContInitGuild.insert( std::make_pair( iBattleIndex, kOwnerGuildInfo ) );
				}

				m_kTournament.InitGuild( kContInitGuild );

				m_kContChallenge.clear();
				m_i64TotalChallengeExp = 0i64;
			}

			int iReadyMinTime = 10;
			g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EmporiaBattleReadyMinTime, iReadyMinTime );
			__int64 const EMPORIA_BATTLE_READY_TIME = ( CGameTime::MINUTE * static_cast<__int64>(iReadyMinTime+1i64) );

			m_i64NextStateChangeTime = (m_i64BattleTime - EMPORIA_BATTLE_READY_TIME);
		}break;
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
		{
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
			CheckChallengeTournament( i64NowTime );
		}break;
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{	
			__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
			if ( !CheckEmporiaTorunament( i64NowTime ) && bCall )
			{
				m_iState = iState;
				return this->SetState( EMPORIA_PEACE, true );
			}
		}break;
	}

	m_iState = iState;
	if ( true == bCall )
	{
		Save();
	}
	return S_OK;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::GetEmporiaInfo( BM::GUID const &kGuildID, SGuildEmporiaInfo& rkOutEmporiaInfo )const
{
	BM::CAutoMutex kLock( m_kMutex );

	HRESULT hRet = E_FAIL;

	switch ( m_iState )
	{
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			CONT_CHALLENGE_BATTLE::const_iterator challenge_itr = m_kContChallenge.find( kGuildID );
			if ( challenge_itr != m_kContChallenge.end() )
			{
				rkOutEmporiaInfo.kID = GetID();
				rkOutEmporiaInfo.byGrade = 0;
				rkOutEmporiaInfo.byType = EMPORIA_KEY_BATTLERESERVE;
				return S_OK;
			}
		}break;
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			CONT_ING_BATTLE::const_iterator ing_itr = m_kContBattle_Ing.begin();
			for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
			{
				PgEmporiaTournamentElement *pkElement = ing_itr->pkElement;
				if ( PgEmporiaTournamentElement::STATE_RECV_CREATE & pkElement->GetState() )
				{
					if ( true == pkElement->IsGuild( true, kGuildID ) )
					{
						rkOutEmporiaInfo.kID = GetID();
						rkOutEmporiaInfo.byGrade = 0;
						rkOutEmporiaInfo.byType = EMPORIA_KEY_BATTLE_ATTACK;
						return S_OK;
					}
					else if ( true == pkElement->IsGuild( false, kGuildID ) )
					{
						rkOutEmporiaInfo.kID = GetID();
						rkOutEmporiaInfo.byGrade = 0;
						rkOutEmporiaInfo.byType = EMPORIA_KEY_BATTLE_DEFENCE;
						return S_OK;
					}
				}
			}
		}// break 을 사용하지 않는다.
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
		{
			if ( true == m_kTournament.IsHaveGuild( kGuildID ) )
			{
				rkOutEmporiaInfo.kID = GetID();
				rkOutEmporiaInfo.byGrade = 0;
				rkOutEmporiaInfo.byType = EMPORIA_KEY_BATTLECONFIRM;
				return S_OK;
			}
		}break;
	case EMPORIA_PEACE:
		{
		}break;
	case EMPORIA_CLOSE:
	default:
		{
			return E_FAIL;
		}break;
	}

	for ( size_t i=0; i<iNum; ++i )
	{
		if ( true == m_kEmporia[i].IsOwner( kGuildID ) )
		{
			rkOutEmporiaInfo.Set( m_kEmporia[i].GetKey() );
			rkOutEmporiaInfo.byType = EMPORIA_KEY_MINE;
			hRet = S_OK;
			break;
		}
	}

	return hRet;
}

template< size_t iNum >
BYTE PgEmporiaPack< iNum >::GetEmporiaKey( size_t iGrade, BM::GUID const &kGuildGuid, SGroundKey &kOutGndKey )const
{
	BM::CAutoMutex kLock(m_kMutex);

	switch ( m_iState )
	{
	case EMPORIA_CLOSE:
		{
		}break;
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			CONT_ING_BATTLE::const_iterator ing_itr = m_kContBattle_Ing.begin();
			for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
			{
				PgEmporiaTournamentElement *pkElement = ing_itr->pkElement;
				BYTE const byState = pkElement->GetState();

				if ( PgEmporiaTournamentElement::STATE_RECV_CREATE & byState )
				{
					if ( true == pkElement->IsGuild( true, kGuildGuid ) )
					{
						if ( PgEmporiaTournamentElement::FLAG_FULL_ATTACK & byState )
						{
							return MMET_Failed_Full;
						}

						kOutGndKey = ing_itr->kBattleGroundKey;
						return MMET_None;
					}
					else if ( true == pkElement->IsGuild( false, kGuildGuid ) )
					{
						if ( PgEmporiaTournamentElement::FLAG_FULL_DEFENCE & byState )
						{
							return MMET_Failed_Full;
						}

						kOutGndKey = ing_itr->kBattleGroundKey;
						return MMET_None;
					}
				}
			}
		} // break을 사용하지 않는다.
	case EMPORIA_PEACE:
	case EMPORIA_TERM_OF_CHALLENGE:
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
		{
			if ( iGrade < iNum )
			{
				if ( true == m_kEmporia[iGrade].IsAccess( kGuildGuid ) )
				{
					kOutGndKey = m_kEmporia[iGrade].GetEmporiaGroundKey();
					if ( true == kOutGndKey.IsEmpty() )
					{
						return MMET_Failed_Creating;
					}
					return MMET_None;
				}
			}
		}break;
	}

	return MMET_Failed_Access;
}

template< size_t iNum >
BYTE PgEmporiaPack< iNum >::GetEmporiaKeyFromMercenary( BM::GUID const &kGuildGuid, BM::GUID const &kCharGuid, PgLimitClass const &kClassInfo, size_t const iJoinIndex, bool const bIsAttack, SGroundKey &kOutGndKey )const
{
	BM::CAutoMutex kLock(m_kMutex);

	switch ( m_iState )
	{
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			CONT_ING_BATTLE::const_iterator ing_itr = m_kContBattle_Ing.begin();
			for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
			{
				PgEmporiaTournamentElement *pkElement = ing_itr->pkElement;
				if ( iJoinIndex == pkElement->GetIndex() )
				{
					BYTE const byState = pkElement->GetState();

					if ( PgEmporiaTournamentElement::STATE_RECV_START & byState 
					|| pkElement->IsBeforJoinMercenary(bIsAttack, kCharGuid) )
					{
						if ( true == bIsAttack )
						{
							if ( PgEmporiaTournamentElement::FLAG_FULL_ATTACK & byState )
							{
								return MMET_Failed_Full;
							}
						}
						else
						{
							if ( PgEmporiaTournamentElement::FLAG_FULL_DEFENCE & byState )
							{
								return MMET_Failed_Full;
							}
						}

						if ( !pkElement->IsAccess( bIsAttack, kClassInfo ) )
						{
							return MMET_Failed_Access;
						}

						kOutGndKey = ing_itr->kBattleGroundKey;
						return MMET_None;
					}
					else
					{
						return MMET_Failed_JoinTime;
					}
				}
			}
		}break;
	default:
		{

		}break;
	}

	return MMET_Failed_Access;
}

template< size_t iNum >
PgEmporia const * PgEmporiaPack< iNum >::GetElement( size_t iIndex )const
{
	if ( iIndex < iNum )
	{
		return &m_kEmporia[iIndex];
	}
	return NULL;
}

template< size_t iNum >
bool PgEmporiaPack< iNum >::SetElement( size_t iIndex, PgEmporia const &rkElement )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iIndex < iNum )
	{
		m_kEmporia[iIndex] = rkElement;
		return true;
	}
	return false;
}

template< size_t iNum >
size_t PgEmporiaPack< iNum >::GetOpenLevel(void)const
{
	BM::CAutoMutex kLock(m_kMutex);

	size_t iOpenLevel = 0;
	for ( ; iOpenLevel < iNum ; ++iOpenLevel )
	{
		if ( !m_kEmporia[iOpenLevel].IsOpen() )
		{
			break;
		}
	}
	return iOpenLevel;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::Open( BM::DBTIMESTAMP_EX const &kNextBattleDate, char nBattlePeriodsForWeeks, size_t iOpenLevel )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( iOpenLevel > iNum )
	{
		iOpenLevel = iNum;
	}
	
	switch ( m_iState )
	{
	case EMPORIA_CLOSE:
	case EMPORIA_PEACE:
		{
			CGameTime::DBTimeEx2SecTime( kNextBattleDate, m_i64BattleTime, CGameTime::DEFAULT );

			if ( 0 == nBattlePeriodsForWeeks )
			{
				// 0이면 현재설정된 값을 유지
				nBattlePeriodsForWeeks = m_nBattlePeriodsForWeeks;
			}

			{
				size_t const iMaxLevel = ( iOpenLevel ? iOpenLevel : GetOpenLevel() );
				char const nMinimumPeriodsForWeeks = static_cast<char>( EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM[ iMaxLevel - 1 ] / CGameTime::OneWeek ) + 1;
				if ( nBattlePeriodsForWeeks < nMinimumPeriodsForWeeks )
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << "Change Periods For Weeks : " << nBattlePeriodsForWeeks << L" -> " << nMinimumPeriodsForWeeks );
					nBattlePeriodsForWeeks = nMinimumPeriodsForWeeks;
				}
			}
			
			m_nBattlePeriodsForWeeks = nBattlePeriodsForWeeks;

			m_iState = EMPORIA_CLOSE;//CLOSE로 바꿔주고 해야지 업데이트가 먹는다.
			this->SetState( EMPORIA_PEACE, true );

			if ( iOpenLevel )
			{// 0이면 현재 설정된 값을 유지
				for ( size_t i = 0; i<iNum ; ++i )
				{
					OpenElement( i, i < iOpenLevel );
				}
			}
			else
			{
				// Level은 무조건 열려 있어야 한다.
				OpenElement( 0, true );
			}
		}break;
	case EMPORIA_TERM_OF_CHALLENGE:
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
	default:
		{
			return E_FAIL;
		}break;
	}
	return S_OK;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::Close()
{
	BM::CAutoMutex kLock(m_kMutex);
	return this->SetState( EMPORIA_CLOSE, true );
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::OpenElement( size_t const iIndex, bool const bOpen )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( EMPORIA_CLOSE != m_iState )
	{
		CONT_DEF_EMPORIA const *pkDefEmporia = NULL;
		g_kTblDataMgr.GetContDef( pkDefEmporia );

		if ( pkDefEmporia )
		{
			CONT_DEF_EMPORIA::const_iterator itr = pkDefEmporia->find( GetID() );
			if ( itr != pkDefEmporia->end() )
			{
				if ( iIndex < iNum )
				{
					if ( 0 != itr->second.iEmporiaGroundNo[iIndex] )
					{
						if ( true == m_kEmporia[iIndex].Open( bOpen ) )
						{
							SaveElement( iIndex, NULL );
							return S_OK;
						}
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not found CONT_DEF_EMPORIA EmporiaID<") << GetID() << _T(">") );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_EMPORIA is NULL") );
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::AddChallenge( SEmporiaChallenge const *pkChallenge, bool const bSystem )
{
	BM::CAutoMutex kLock( m_kMutex );
	if (	(true == bSystem) 
		||	(EMPORIA_TERM_OF_CHALLENGE == m_iState) )
	{
		if ( pkChallenge )
		{
			auto kPair = m_kContChallenge.insert( std::make_pair( pkChallenge->kGuildInfo.kGuildID, *pkChallenge ) );
			if ( kPair.second )
			{
				m_i64TotalChallengeExp += pkChallenge->i64Cost;
				return S_OK;
			}
		}
		return E_FAIL;
	}
	return E_ACCESSDENIED;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RemoveChallenge( BM::GUID const &kGuildID, SEmporiaChallenge *pkOutChallenge, bool const bSystem )
{
	BM::CAutoMutex kLock( m_kMutex );
	if (	(true == bSystem) 
		||	(EMPORIA_TERM_OF_CHALLENGE == m_iState) )
	{
		CONT_CHALLENGE_BATTLE::iterator itr = m_kContChallenge.find( kGuildID );
		if ( itr != m_kContChallenge.end() )
		{
			if ( pkOutChallenge )
			{
				*pkOutChallenge = itr->second;
			}
			m_i64TotalChallengeExp -= itr->second.i64Cost;
			m_kContChallenge.erase( itr );
			return S_OK;
		}
		return E_FAIL;
	}
	return E_ACCESSDENIED;
}

template< size_t iNum >
void PgEmporiaPack< iNum >::Update( __int64 const i64NowTime )
{
	BM::CAutoMutex kLock( m_kMutex );

	bool bCheckEmporiaFunc = false;

	switch ( m_iState )
	{
	case EMPORIA_CLOSE:
		{// 닫혀있네.
			return;
		}break;
	case EMPORIA_PEACE:
	case EMPORIA_TERM_OF_CHALLENGE:
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
		{
			if ( i64NowTime >= m_i64NextStateChangeTime )
			{
				this->SetState( m_iState + EMPORIA_STATE_ADD, true );
			}
			else
			{
				bCheckEmporiaFunc = true;
			}
		}break;
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
		{
			if (i64NowTime >= m_i64NextStateChangeTime)
			{
				if ( !CheckChallengeTournament( i64NowTime ) )
				{
					this->SetState( m_iState + EMPORIA_STATE_ADD, true );
				}
			}
		}break;
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			if (i64NowTime >= m_i64NextStateChangeTime)
			{
				if ( !CheckEmporiaTorunament( i64NowTime ) )
				{
					this->SetState( EMPORIA_PEACE, true );
				}
			}
		}break;
	}

	size_t iIndex = 0;
	for ( ; iIndex<iNum; ++iIndex )
	{
		if ( true == m_kEmporia[iIndex].IsHaveOwner() )
		{
			if ( true == m_kEmporia[iIndex].GetEmporiaGroundKey().IsEmpty() )
			{
				CONT_DEF_EMPORIA const *pkDefEmporia = NULL;
				g_kTblDataMgr.GetContDef( pkDefEmporia );

				if ( pkDefEmporia )
				{
					CONT_DEF_EMPORIA::const_iterator itr = pkDefEmporia->find( GetID() );
					if ( itr != pkDefEmporia->end() )
					{
						SGroundMakeOrder kOrder;
						kOrder.kKey.GroundNo( itr->second.iEmporiaGroundNo[iIndex] );
						kOrder.kKey.Guid( m_kEmporia[iIndex].GetOwnerGuildID() );

						BM::Stream kCreatePacket( PT_N_T_REQ_CREATE_PUBLICMAP, PMET_EMPORIA );
						kOrder.WriteToPacket( kCreatePacket );
						kCreatePacket.Push( static_cast<T_GNDATTR>(GATTR_EMPORIA) );
						kCreatePacket.Push( GetID() );
						kCreatePacket.Push( iIndex );
						m_kEmporia[iIndex].WriteToPacket( kCreatePacket, false );// Add
						::SendToCenter( CProcessConfig::GetPublicChannel(), kCreatePacket );
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << __FL__ << _T("Not Found CONT_DEF_EMPORIA EmporiaID<") << GetID() << _T(">") );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_EMPORIA is NULL") );
				}
			}
			else
			{
				if ( true == bCheckEmporiaFunc )
				{
					PgEmporiaFunction &kFunc = m_kEmporia[iIndex].GetFunc();

					CONT_EMPORIA_FUNCKEY kContDeleteKey;
					kFunc.CheckDelete( i64NowTime, kContDeleteKey );

					// 지우는 쿼리 하지 말자
					// 지우면 문제가 될 수 있다.
// 					CONT_EMPORIA_FUNCKEY::iterator delete_itr = kContDeleteKey.begin();
// 					for ( ; delete_itr != kContDeleteKey.end() ; ++delete_itr )
// 					{
// 						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_FUNCTION, _T("EXEC [dbo].[up_DeleteEmporiaFunction]"));
// 						kQuery.InsertQueryTarget( GetID() );
// 
// 						kQuery.PushStrParam( GetID() );
// 						kQuery.PushStrParam( static_cast<BYTE>(iIndex+1) );
// 						kQuery.PushStrParam( *delete_itr );
// 
// 						g_kCoreCenter.PushQuery( kQuery );
// 					}

					// 엠포리아 그라운드에게 통보 해주어야 한다.
					BM::Stream kDelPacket( PT_N_M_NFY_EMPORIA_FUNCTION_REMOVE, kContDeleteKey );
					Send( iIndex, kDelPacket );
				}
			}
		}
	}
}

template< size_t iNum >
void PgEmporiaPack< iNum >::WriteToPacket( BM::Stream &kPacket )const
{
	BM::CAutoMutex kLock( m_kMutex );

	kPacket.Push( m_kID );
	kPacket.Push( m_iState );
	switch ( m_iState )
	{
	case EMPORIA_TERM_OF_CHALLENGE:
		{
			kPacket.Push( m_kContChallenge.size() );
			kPacket.Push( m_i64TotalChallengeExp );	
		}// break 사용 금지
	case EMPORIA_PEACE:
	case EMPORIA_BEFORE_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_CHALLENGE_OF_TOURNAMENT:
	case EMPORIA_OWNERSHIP_OF_TOURNAMENT:
		{
			kPacket.Push( m_i64BattleTime );
			m_kTournament.WriteToPacket( kPacket );
			
			for ( size_t i = 0; i<iNum ; ++i )
			{
				m_kEmporia[i].WriteToPacket( kPacket );
			}
		}break;
	default:
		{

		}break;
	}
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::WriteToPacket_Grade( size_t const iIndex, BM::Stream &rkPacket, bool const bSimple )const
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		m_kEmporia[iIndex].WriteToPacket( rkPacket, bSimple );
		return S_OK;
	}
	else if ( iNum == iIndex )
	{
		for ( size_t i = 0 ; i < iNum ; ++i )
		{
			m_kEmporia[i].WriteToPacket( rkPacket, bSimple );
		}
		return S_OK;
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::GetFunction( size_t const iIndex, short nFuncNo, SEmporiaFunction &rkFuncInfo )const
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		PgEmporiaFunction const &rkFunc = m_kEmporia[iIndex].GetFunc();
		if ( true ==rkFunc.GetFunction( nFuncNo, rkFuncInfo ) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::AddFunction( size_t const iIndex, short nFuncNo, SEmporiaFunction const &kFuncInfo )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		PgEmporiaFunction &kFunc = m_kEmporia[iIndex].GetFunc();
		if ( true == kFunc.AddFunction( nFuncNo, kFuncInfo ) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RemoveFunction( size_t const iIndex, short nFuncNo )
{
	BM::CAutoMutex kLock( m_kMutex );
	
	if ( iNum > iIndex )
	{
		m_kEmporia[iIndex].RemoveFunction( nFuncNo );
		return S_OK;
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::UpdateFunctionExtValue( size_t const iIndex, short nFuncNo, __int64 i64ExtValue )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		SEmporiaFunction kFuncInfo;
		PgEmporiaFunction &rkFunc = m_kEmporia[iIndex].GetFunc();
		if ( true ==rkFunc.GetFunction( nFuncNo, kFuncInfo ) )
		{
			kFuncInfo.i64ExtValue = i64ExtValue;
			rkFunc.AddFunction( nFuncNo, kFuncInfo );
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SetGate( size_t const iIndex, BYTE const byGate, bool const bSave )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		if ( true == m_kEmporia[iIndex].SetGate( byGate ) )
		{
			if ( true == bSave )
			{
				SaveElement( iIndex, NULL );
			}
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::Send( size_t const iIndex, BM::Stream const &kPacket )const
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		if ( !m_kEmporia[iIndex].GetEmporiaGroundKey().IsEmpty() )
		{
			if ( true == ::SendToGround( CProcessConfig::GetPublicChannel(), m_kEmporia[iIndex].GetEmporiaGroundKey(), kPacket, true ) )
			{
				return S_OK;
			}
		}
		return S_FALSE;
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SetEmporiaBattleLock( SGroundKey const &kBattleGndkey, bool const bAttack, bool const bLock )
{
	BM::CAutoMutex kLock( m_kMutex );

	CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
	for( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
	{
		if ( kBattleGndkey == ing_itr->kBattleGroundKey )
		{
			BYTE byFlag = ( bAttack ? PgEmporiaTournamentElement::FLAG_FULL_ATTACK : PgEmporiaTournamentElement::FLAG_FULL_DEFENCE );
			if ( true == bLock )
			{
				byFlag |= ing_itr->pkElement->GetState();
			}
			else
			{
				byFlag = ~byFlag;
				byFlag &= ing_itr->pkElement->GetState();
			}

			ing_itr->pkElement->SetState( byFlag );
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SetChangeEmporiaOwner( size_t const iIndex, SEmporiaGuildInfo const &kGuildInfo, bool const bSave, bool const bCallSystem )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		if ( !bCallSystem && (EMPORIA_OWNERSHIP_OF_TOURNAMENT == m_iState) )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Emporia<") << GetID() << _T("> State<") << m_iState << _T("> Error!!") );
			return E_ACCESSDENIED;
		}

		SEmporiaGuildInfo kOldOwner;
		m_kEmporia[iIndex].GetOwner( kOldOwner );

		if ( true == m_kEmporia[iIndex].SetOwner( kGuildInfo ) )
		{
			if ( true == bSave )
			{
				SaveElement( iIndex, kOldOwner.IsEmpty() ? NULL : &kOldOwner );
			}
			return S_OK;
		}
		return S_FALSE;
	}
	return E_FAIL;
}

template< size_t iNum >
bool PgEmporiaPack< iNum >::CheckChallengeTournament( __int64 i64NowTime )
{
	if ( E_FAIL == m_kTournament.IsBattle(0) )
	{// 더이상 게임 할꺼 없으~
		return false;
	}
		
	int iReadyMinTime = 10;
	g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EmporiaBattleReadyMinTime, iReadyMinTime );
	__int64 const EMPORIA_BATTLE_READY_TIME = ( CGameTime::MINUTE * static_cast<__int64>(iReadyMinTime) );

	i64NowTime += EMPORIA_BATTLE_READY_TIME;//30분전에 시작되게
	if ( 0i64 >= i64NowTime )
	{
		i64NowTime = _I64_MAX;
	}

	__int64 i64Temp = _I64_MAX;// 이변수에는 i64NowTime보다는 크고 제일 가까운 값을 찾아야 한다.
	__int64 i64CheckTime;

	size_t iIndex = 0;
	while ( S_OK == m_kTournament.GetBattleTime( iIndex, i64CheckTime ) )
	{
		if ( i64NowTime >= i64CheckTime )
		{
			if ( S_OK == m_kTournament.IsBattle( iIndex ) )
			{
				SEmporiaGuildInfo kAttackGuildInfo;
				SEmporiaGuildInfo kDefenceGuildInfo;
				m_kTournament.GetGuildInfo( iIndex, true, kAttackGuildInfo );
				m_kTournament.GetGuildInfo( iIndex, false, kDefenceGuildInfo );

				if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID == kAttackGuildInfo.kGuildID )
				{
					m_kTournament.SetWinner( iIndex, kDefenceGuildInfo.kGuildID );
					SaveBattleRet( kDefenceGuildInfo.kGuildID, kAttackGuildInfo.kGuildID, static_cast<short>(iIndex) );

					if ( 0 == iIndex )
					{
						return false;
					}
				}
				else if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID == kDefenceGuildInfo.kGuildID )
				{
					m_kTournament.SetWinner( iIndex, kAttackGuildInfo.kGuildID );
					SaveBattleRet( kAttackGuildInfo.kGuildID, kDefenceGuildInfo.kGuildID, static_cast<short>(iIndex) );

					if ( 0 == iIndex )
					{
						return false;
					}
				}
				else
				{
					SEmporiaBattleGroundInfo kBattleGroundInfo = ms_kBattleGroundInfo_Challenge;

					CONT_DEF_EMPORIA const * pkDefEmporia = NULL;
					g_kTblDataMgr.GetContDef(pkDefEmporia);

					CONT_DEF_EMPORIA::const_iterator def_itr = pkDefEmporia->find( GetID() );
					if ( def_itr != pkDefEmporia->end() )
					{
						if ( def_itr->second.iChallengeBattleGroundNo )
						{
							kBattleGroundInfo.iGroundNo = def_itr->second.iChallengeBattleGroundNo;
						}
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found CONT_DEF_EMPORIA<" << GetID() << L">" );
					}

					// 전쟁을 만들어야 한다.
					SBattleInfo kBattleInfo( kBattleGroundInfo );
					kBattleInfo.pkElement = m_kTournament.GetElement(iIndex);
					if ( S_FALSE == ReqCreateBattle( kBattleInfo ) )
					{
						i64Temp = i64CheckTime;
					}
				}
			}
			else if ( i64Temp > i64CheckTime )
			{
				i64Temp = i64NowTime + CGameTime::MINUTE;
			}
		}
		else if ( i64Temp > i64CheckTime )
		{
			i64Temp = i64CheckTime;
		}

		++iIndex;
	}

	m_i64NextStateChangeTime = i64Temp - EMPORIA_BATTLE_READY_TIME;// 30분전
	return true;
}

template< size_t iNum >
bool PgEmporiaPack< iNum >::CheckEmporiaTorunament( __int64 i64NowTime )
{
	int iReadyMinTime = 10;
	g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EmporiaBattleReadyMinTime, iReadyMinTime );
	__int64 const EMPORIA_BATTLE_READY_TIME = ( CGameTime::MINUTE * static_cast<__int64>(iReadyMinTime) );

	i64NowTime += EMPORIA_BATTLE_READY_TIME;//30분전에 시작되게
	if ( 0i64 >= i64NowTime )
	{
		i64NowTime = _I64_MAX;
	}

	size_t iIndex = 0;

	PgEmporiaTournamentElement * pkElement = m_kTournament.GetElement( iIndex + PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX );
	while ( pkElement )
	{
		if ( S_OK == pkElement->IsBattle() )
		{
			if ( i64NowTime >= pkElement->GetBattleTime() )
			{
				if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID == pkElement->GetGuildGuid( false ) )
				{
					pkElement->SetWinner( pkElement->GetGuildGuid(true) );

					if ( 0 == iIndex )
					{
						SEmporiaGuildInfo kWinGuildInfo;
						if ( SUCCEEDED(pkElement->GetWinnerInfo(kWinGuildInfo)) )
						{
							SetChangeEmporiaOwner( 0, kWinGuildInfo, true, true );
						}	
					}
					else
					{
						// 더이상 전쟁을 만들 수 없다면 상위 엠포리아를 이긴놈이 차지 해야 한다.
						if ( iNum > iIndex )
						{
							if ( true == m_kEmporia[iIndex].IsOpen() )
							{
								SaveSwapEmporia( iIndex-1, iIndex );
							}
						}
					}

					m_i64NextStateChangeTime = _I64_MAX;
					return false;
				}
				else if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID == pkElement->GetGuildGuid( true ) )
				{
					pkElement->SetWinner( pkElement->GetGuildGuid(false) );
					m_i64NextStateChangeTime = _I64_MAX;
					return false;
				}

				// 전쟁을 만들어
				SEmporiaBattleGroundInfo kBattleGroundInfo = ms_kBattleGroundInfo_Emporia;

				CONT_DEF_EMPORIA const * pkDefEmporia = NULL;
				g_kTblDataMgr.GetContDef(pkDefEmporia);

				CONT_DEF_EMPORIA::const_iterator def_itr = pkDefEmporia->find( GetID() );
				if ( def_itr != pkDefEmporia->end() )
				{
					if ( def_itr->second.iBattleGroundNo[iIndex] )
					{
						kBattleGroundInfo.iGroundNo = def_itr->second.iBattleGroundNo[iIndex];
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found CONT_DEF_EMPORIA<" << GetID() << L">" );
				}

				SBattleInfo kBattleInfo( kBattleGroundInfo );
				kBattleInfo.pkElement = pkElement;

				if ( S_OK == ReqCreateBattle( kBattleInfo ) )
				{
					pkElement = m_kTournament.GetElement( ++iIndex + PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX );
					if ( !pkElement )
					{
						m_i64NextStateChangeTime = _I64_MAX;
						return true;
					}
				}
			}

			m_i64NextStateChangeTime = pkElement->GetBattleTime() - EMPORIA_BATTLE_READY_TIME;//30분전
			return true;
		}

		pkElement = m_kTournament.GetElement( ++iIndex + PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX );
	}

	return false;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::ReqCreateBattle( SBattleInfo const &kBattleInfo )
{
	if ( !kBattleInfo.pkElement )
	{
		return E_FAIL;
	}

	CONT_ING_BATTLE::iterator ing_itr = std::find( m_kContBattle_Ing.begin(), m_kContBattle_Ing.end(), kBattleInfo );
	if ( ing_itr == m_kContBattle_Ing.end() )
	{
		// Center한테 통보해주어야 한다.
		SGroundMakeOrder kOrder( kBattleInfo.kBattleGroundKey );

		BM::Stream kCreatePacket( PT_N_T_REQ_CREATE_PUBLICMAP, PMET_EMPORIA );
		kOrder.WriteToPacket( kCreatePacket );
		kCreatePacket.Push( static_cast<T_GNDATTR>(GATTR_EMPORIABATTLE) );
		kCreatePacket.Push( GetID() );
		kCreatePacket.Push( kBattleInfo.pkElement->GetIndex() );

		if ( !::SendToCenter( CProcessConfig::GetPublicChannel(), kCreatePacket ) )
		{
			CAUTION_LOG( BM::LOG_LV2, _T("[Emporia<") << GetID() << _T(">] BattleGround Create Failed : Not Connect Public Center") );
			return S_FALSE;
		}

		m_kContBattle_Ing.push_back( kBattleInfo );

		// 용병정보를 불러와야 하고..
		BM::Stream kGPacket( PT_N_N_REQ_GET_SETTING_MERCENARY, GetID() );
		kGPacket.Push( static_cast<size_t>(2) );
		kGPacket.Push( kBattleInfo.pkElement->GetGuildGuid(true) );
		kGPacket.Push( kBattleInfo.pkElement->GetGuildGuid(false) );
		::SendToGuildMgr( kGPacket );

		return S_OK;
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RecvCreateEmporiaGround( size_t iIndex, SGroundKey const &kEmporiaGndKey, bool const bSuccess )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( iNum > iIndex )
	{
		if ( true == bSuccess )
		{
			SGroundKey kOldEmpGndKey = m_kEmporia[iIndex].GetEmporiaGroundKey();
			if ( !kOldEmpGndKey.IsEmpty() )
			{
				// 이전 엠포리아그라운드는 삭제 해야 한다.
				BM::Stream kDeletePacket( PT_N_T_REQ_DELETE_PUBLICMAP );
				::SendToGround( CProcessConfig::GetPublicChannel(), kOldEmpGndKey, kDeletePacket, true );
			}
			m_kEmporia[iIndex].SetEmporiaGroundKey( kEmporiaGndKey );
		}
		return S_OK;
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RecvDeleteEmporiaGround( SGroundKey const &kGndKey )
{
	BM::CAutoMutex kLock( m_kMutex );

	bool const bAllDelete = kGndKey.IsEmpty();

	for ( size_t i = 0 ; i < iNum ; ++i )
	{
		if ( true == bAllDelete )
		{
			m_kEmporia[i].SetEmporiaGroundKey( SGroundKey() );
		}
		else
		{
			if ( kGndKey == m_kEmporia[i].GetEmporiaGroundKey() )
			{
				m_kEmporia[i].SetEmporiaGroundKey( SGroundKey() );
				return S_OK;
			}
		}
	}

	if ( true == bAllDelete )
	{
		m_kContBattle_Ing.clear();
	}
	else
	{
		CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
		for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
		{
			if ( kGndKey == ing_itr->kBattleGroundKey )
			{
				m_kContBattle_Ing.erase( ing_itr );
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RecvCreateBattle( SGroundKey const &kBattleGndKey, bool const bStart )
{
	BM::CAutoMutex kLock( m_kMutex );

	CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
	for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
	{
		if ( ing_itr->kBattleGroundKey == kBattleGndKey )
		{
			__int64 const i64CurTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

			PgEmporiaTournamentElement * pkElement = ing_itr->pkElement;
			if ( true == bStart )
			{
				int iReadyMinTime = 10;
				g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EmporiaBattleReadyMinTime, iReadyMinTime );
				__int64 const EMPORIA_BATTLE_READY_TIME = ( CGameTime::MINUTE * static_cast<__int64>(iReadyMinTime) );

				SEmporiaGuildInfo kAttackGuildInfo;
				SEmporiaGuildInfo kDefenceGuildInfo;
				pkElement->GetGuildInfo( true, kAttackGuildInfo );
				pkElement->GetGuildInfo( false, kDefenceGuildInfo );

				short nGameTimeMin = 30;
				g_kVariableContainer.Get( EVar_Kind_Emporia, ing_itr->kGameType, nGameTimeMin );

				// 레디 정보를 보내주어야 한다.
				BM::Stream kReadyPacket( PT_N_M_REQ_READY_EMPORIA_BATTLE, ing_itr->kGameType );
				kReadyPacket.Push( GetID() );
				kReadyPacket.Push( GetBattleID() );
				kReadyPacket.Push( static_cast<int>(pkElement->GetIndex()) );
				
				kReadyPacket.Push( kDefenceGuildInfo.kGuildID );
				kReadyPacket.Push( kDefenceGuildInfo.wstrName );

				kReadyPacket.Push( kAttackGuildInfo.kGuildID );
				kReadyPacket.Push( kAttackGuildInfo.wstrName );
				
				ing_itr->i64RealBattleTime = std::min<__int64>( ing_itr->pkElement->GetBattleTime(), i64CurTime + EMPORIA_BATTLE_READY_TIME );
				kReadyPacket.Push( ing_itr->i64RealBattleTime );
				kReadyPacket.Push( nGameTimeMin );

				::SendToGround( CProcessConfig::GetPublicChannel(), kBattleGndKey, kReadyPacket, true );

				pkElement->SetState( pkElement->GetState() | PgEmporiaTournamentElement::STATE_RECV_CREATE );

				// 각각의 길드에게 보내준다.
				SGuildEmporiaInfo kEmporiaInfo(EMPORIA_KEY_BATTLE_DEFENCE);
				kEmporiaInfo.kID = GetID();
				kEmporiaInfo.i64BattleTime = ing_itr->i64RealBattleTime;

				{
					BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kDefenceGuildInfo.kGuildID );
					kGPacket.Push( kEmporiaInfo );
					::SendToGuildMgr( kGPacket );
				}

				{
					kEmporiaInfo.byType = EMPORIA_KEY_BATTLE_ATTACK;

					BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kAttackGuildInfo.kGuildID );
					kGPacket.Push( kEmporiaInfo );
					::SendToGuildMgr( kGPacket );
				}

                //실제 엠포리아 게임 시작로그
                PgLogCont kContLog(ELogMain_Contents_Guild,ELogSub_EmBattle,GetID(),GetBattleID());
				kContLog.RealmNo(g_kProcessCfg.RealmNo());
				kContLog.RealmNo(kBattleGndKey.GroundNo());
                PgLog kLog( ELOrderMain_Game, ELOrderSub_Start );
				kLog.Set( 0, kDefenceGuildInfo.wstrName );
				kLog.Set( 1, kDefenceGuildInfo.kGuildID.str() );
				kLog.Set( 2, kAttackGuildInfo.wstrName );
				kLog.Set( 3, kAttackGuildInfo.kGuildID.str() );
			    //kLog.Set(0,iBattleIndex);
			    kContLog.Add(kLog);
			    kContLog.Commit();
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, _T("Emporia<") << GetID() << _T("> Ground<") << kBattleGndKey.GroundNo() << _T("/") << kBattleGndKey.Guid() << _T("> Create Failed") );

				// 잠시후에 다시 만들어야 한다.
				pkElement->SetState( PgEmporiaTournamentElement::STATE_NONE );
				m_i64NextStateChangeTime = i64CurTime + (2i64 * CGameTime::MINUTE);//2분뒤에 재시도.
				m_kContBattle_Ing.erase( ing_itr );
			}
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::CheckCreateBattleCompleted()
{
	BM::CAutoMutex kLock( m_kMutex );

	__int64 i64LimitTime = 0i64;
	PgLimitClass kLimit(0i64,SHRT_MAX);
	int iGameTimeSec = 0;
	CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
	for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
	{
		PgEmporiaTournamentElement * pkElement = ing_itr->pkElement;
		if(!pkElement){ continue; }

		if( !(pkElement->GetState() & PgEmporiaTournamentElement::STATE_RECV_CREATE) )
		{
			return E_FAIL;//아직 준비되지 않음
		}

		if(ing_itr->i64RealBattleTime > i64LimitTime)
		{
			i64LimitTime = ing_itr->i64RealBattleTime;
		}

		kLimit += pkElement->GetMercenaryLimit();

		short nGameTimeMin = 30;
		g_kVariableContainer.Get( EVar_Kind_Emporia, ing_itr->kGameType, nGameTimeMin );
	}

	__int64 const i64SendLimitTime = i64LimitTime + std::max<__int64>(0,iGameTimeSec*CGameTime::SECOND);

	BM::Stream kSendPacket;//생성자에 넣으면 타입이 맞지 않아 안됨
	kSendPacket.Push( NOTICE_EM_AD_MERCENARY );
	kSendPacket.Push( GetID() );
	kSendPacket.Push( kLimit );

	//용병모집 정보 전송
	BM::Stream kADMercenary;
	kADMercenary.Push(E_NoticeMsg_Add);
	kADMercenary.Push(0i64);//바로 보냄
	kADMercenary.Push(i64SendLimitTime);
	kADMercenary.Push(NOTICE_ALL);
	kADMercenary.Push(kSendPacket);
	SendToNotice(kADMercenary);

	return S_OK;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::RecvBattleResult( SGroundKey const &kBattleGndKey, bool const bAttackerWin, BM::GUID &rkOutEraseGuildID, CONT_EM_RESULT_USER const & rkWinMember, CONT_EM_RESULT_USER const & rkLoseMember, EEmporiaResultNoticeType const eResultType )
{
	BM::CAutoMutex kLock( m_kMutex );

	PgEmporiaTournamentElement * pkElement = NULL;

	{
		CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
		for ( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
		{
			if ( ing_itr->kBattleGroundKey == kBattleGndKey )
			{
				pkElement = ing_itr->pkElement;
				m_kContBattle_Ing.erase( ing_itr );
				break;
			}
		}
	}

	if ( pkElement )
	{
		SResultRewardInfo kWinRewardInfo;
		SResultRewardInfo kLoseRewardInfo;

		size_t const iBattleIndex = pkElement->GetIndex();

		SEmporiaGuildInfo kWinGuildInfo;
		SEmporiaGuildInfo kLoseGuildInfo;
		pkElement->GetGuildInfo( bAttackerWin, kWinGuildInfo );
		pkElement->GetGuildInfo( !bAttackerWin, kLoseGuildInfo );

		if ( SUCCEEDED(m_kTournament.SetWinner( iBattleIndex, kWinGuildInfo.kGuildID )) )
		{
			SaveWinMercenary( bAttackerWin, pkElement, kWinGuildInfo.kGuildID, rkWinMember, iBattleIndex );
			if ( iBattleIndex < PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX )
			{
				// 도전자 토너먼트
				SaveBattleRet( kWinGuildInfo.kGuildID, kLoseGuildInfo.kGuildID, static_cast<short>(iBattleIndex) );
				rkOutEraseGuildID = kLoseGuildInfo.kGuildID;

				SGuildEmporiaInfo kEmporiaInfo;

				BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kLoseGuildInfo.kGuildID );
				kGPacket.Push( kEmporiaInfo );
				::SendToGuildMgr( kGPacket );

				if ( 0 == iBattleIndex )
				{
					this->SetState( EMPORIA_OWNERSHIP_OF_TOURNAMENT, true );
					if ( EMPORIA_OWNERSHIP_OF_TOURNAMENT == m_iState )
					{
						SGuildEmporiaInfo kWinGuildEmporiaInfo(EMPORIA_KEY_BATTLECONFIRM);
						kWinGuildEmporiaInfo.kID = GetID();

						BM::Stream kGPacket2( PT_N_N_NFY_EMPORIA_INFO, kWinGuildInfo.kGuildID );
						kGPacket2.Push( kWinGuildEmporiaInfo );
						::SendToGuildMgr( kGPacket2 );
					}
					else
					{
						SendReward( true, rkWinMember, PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX, kWinGuildInfo.kGuildID, kWinRewardInfo );
					}
				}
				else
				{
					SGuildEmporiaInfo kWinGuildEmporiaInfo(EMPORIA_KEY_BATTLECONFIRM);
					kWinGuildEmporiaInfo.kID = GetID();

					BM::Stream kGPacket2( PT_N_N_NFY_EMPORIA_INFO, kWinGuildInfo.kGuildID );
					kGPacket2.Push( kWinGuildEmporiaInfo );
					::SendToGuildMgr( kGPacket2 );
				}
			}
			else
			{
				// 엠포리아 토너먼트
				size_t const iEmporiaIndex = iBattleIndex - PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX;
				if ( iEmporiaIndex < iNum )
				{
					SaveBattleRet( kWinGuildInfo.kGuildID, kLoseGuildInfo.kGuildID, static_cast<short>(iBattleIndex) );

					if ( true == bAttackerWin )
					{
						// 공격자가 이겼으니 엠포리아를 빼앗겨야 한다.
						if ( 0 == iEmporiaIndex )
						{
							SEmporiaGuildInfo kOldGuildInfo;
							m_kEmporia[0].GetOwner( kOldGuildInfo );

							if ( S_OK == SetChangeEmporiaOwner( 0, kWinGuildInfo, true, true ) )
							{
								if ( !kOldGuildInfo.IsEmpty() )
								{// 엠포리아 소유권을 빼앗긴 길드정보는 ChallengeBattle에 입력해 두어야 혹시 서버를 내렸다 올려도 정보를 가지고 올 수 있다.
									BM::DBTIMESTAMP_EX kRegistDate;
									kRegistDate.SetLocalTime();

									CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_BATTLE_STATE, _T("EXEC [DBO].[up_SaveEmporia_ChallengeBattle]") );
									kQuery.InsertQueryTarget( kOldGuildInfo.kGuildID );
									kQuery.QueryOwner( kOldGuildInfo.kGuildID );

									kQuery.PushStrParam( GetID() );
									kQuery.PushStrParam( GetBattleID() );
									kQuery.PushStrParam( kOldGuildInfo.kGuildID );
									kQuery.PushStrParam( 0i64 );
									kQuery.PushStrParam( kRegistDate );	// 5
									kQuery.PushStrParam( static_cast<BYTE>(EMBATTLE_LOSTEMPORIA) );

									g_kCoreCenter.PushQuery( kQuery );
								}	
							}
						}
						else
						{
							// 엠포리아 주인을 맞바꾼다.
							// 바꾸는건 DB도 트랜젝션 처리를 해야 한다.
							SaveSwapEmporia( iEmporiaIndex-1, iEmporiaIndex );
						}
					}
					else
					{
						SGuildEmporiaInfo kEmporiaInfo;

						// 수비자가 이겼으니 엠포리아를 유지한다.
						if ( 0 == iEmporiaIndex )
						{
							rkOutEraseGuildID = kLoseGuildInfo.kGuildID;
	
							{
								BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kLoseGuildInfo.kGuildID );
								kGPacket.Push( kEmporiaInfo );
								::SendToGuildMgr( kGPacket );
							}
						}
						else
						{
							kEmporiaInfo.kID = GetID();
							kEmporiaInfo.byGrade = static_cast<BYTE>(iEmporiaIndex);
							kEmporiaInfo.byType = EMPORIA_KEY_MINE;

							BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kLoseGuildInfo.kGuildID );
							kGPacket.Push( kEmporiaInfo );
							::SendToGuildMgr( kGPacket );
						}

						{
							kEmporiaInfo.kID = GetID();
							kEmporiaInfo.byGrade = static_cast<BYTE>(iEmporiaIndex+1);
							kEmporiaInfo.byType = EMPORIA_KEY_MINE;

							BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kWinGuildInfo.kGuildID );
							kGPacket.Push( kEmporiaInfo );
							::SendToGuildMgr( kGPacket );
						}
					}
				}

				if ( !CheckEmporiaTorunament( 0i64 ) )// 0으로 넣어주어야 전쟁이 시작하지는 않고 체크만 할 수 있다.
				{
					this->SetState( EMPORIA_PEACE, true );
					SendReward( true, rkWinMember, iBattleIndex, kWinGuildInfo.kGuildID, kWinRewardInfo );
				}
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Critical Error!! EmporiaID") << GetID() << _T(">") );
		}

		SendReward( false, rkLoseMember, iBattleIndex, kWinGuildInfo.kGuildID, kLoseRewardInfo );


		//실제 엠포리아 게임 종료로그
		PgLogCont kContLog(ELogMain_Contents_Guild,ELogSub_EmBattle,GetID(),GetBattleID());
		kContLog.RealmNo(g_kProcessCfg.RealmNo());
		kContLog.GroundNo(kBattleGndKey.GroundNo());
		PgLog kLog( ELOrderMain_Game, ELOrderSub_End );
		kLog.Set( 0, kWinGuildInfo.wstrName );
		kLog.Set( 1, kWinGuildInfo.kGuildID.str() );
		kLog.Set( 2, kLoseGuildInfo.wstrName );
		kLog.Set( 3, kLoseGuildInfo.kGuildID.str() );
		kLog.Set( 0, static_cast< int >(eResultType) );
		kLog.Set( 0, kWinRewardInfo.i64GuildGold );
		kLog.Set( 1, kLoseRewardInfo.i64GuildGold );
		kContLog.Add(kLog);

		INFO_LOG( BM::LOG_LV5, __FL__ << _T("=========    Emporia Battle Result : ") << iBattleIndex << _T("    =========") );
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("BattleGuid: ") << GetBattleID() );
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("WinerGuild: ") << kWinGuildInfo.kGuildID << _T(", MemberCount: ") << rkWinMember.size() );
		CONT_EM_RESULT_USER::const_iterator winm_it = rkWinMember.begin();
		while( winm_it != rkWinMember.end() )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("CharGuid: ") << (*winm_it).kCharGuid );

			PgLog kLog( ELOrderMain_Game, ELOrderSub_Reward );
			kLog.Set( 0, kWinGuildInfo.kGuildID.str() );				//길드GUID
			kLog.Set( 1, (*winm_it).kCharGuid.str() );					//캐릭GUID
			kLog.Set( 2, GetBattleID().str() );							//경기GUID
			kLog.Set( 0, static_cast<int>(iBattleIndex) );				//경기번호
			kLog.Set( 1, static_cast<int>((*winm_it).bMercenary) );		//용병여부
			kLog.Set( 2, static_cast<int>(true) );						//승리여부
			kLog.Set( 0, static_cast<__int64>(kWinRewardInfo.iItemNo) );			//보상아이템No
			kLog.Set( 1, static_cast<__int64>(kWinRewardInfo.iAchieveSaveIdx) );	//보상업적 SaveIdx
			kContLog.Add(kLog);

			++winm_it;
		}
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("LoseGuild: ") << kLoseGuildInfo.kGuildID << _T(", MemberCount: ") << rkLoseMember.size() );
		CONT_EM_RESULT_USER::const_iterator losem_it = rkLoseMember.begin();
		while( losem_it != rkLoseMember.end() )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("CharGuid: ") << (*losem_it).kCharGuid );

			PgLog kLog( ELOrderMain_Game, ELOrderSub_Reward );
			kLog.Set( 0, kLoseGuildInfo.kGuildID.str() );				//길드GUID
			kLog.Set( 1, (*losem_it).kCharGuid.str() );					//캐릭GUID
			kLog.Set( 2, GetBattleID().str() );							//경기GUID
			kLog.Set( 0, static_cast<int>(iBattleIndex) );				//경기번호
			kLog.Set( 1, static_cast<int>((*losem_it).bMercenary) );	//용병여부
			kLog.Set( 2, static_cast<int>(false) );						//승리여부
			kLog.Set( 0, static_cast<__int64>(kLoseRewardInfo.iItemNo) );			//보상아이템No
			kLog.Set( 1, static_cast<__int64>(kLoseRewardInfo.iAchieveSaveIdx) );	//보상업적 SaveIdx
			kContLog.Add(kLog);

			++losem_it;
		}
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("=======================================================") );

		kContLog.Commit();
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Not Found BattleGround<") << kBattleGndKey.GroundNo() << _T("/") << kBattleGndKey.Guid() << _T("> EmporiaID<") << GetID() << _T(">") );
	return E_FAIL;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SaveWinMercenary(bool const bAttacker, PgEmporiaTournamentElement * pkElement, BM::GUID const & rkGuildID, CONT_EM_RESULT_USER const & rkMember, int const iBattleIndex )
{
	VEC_GUID kContUser;
	for(CONT_EM_RESULT_USER::const_iterator c_iter=rkMember.begin();c_iter!=rkMember.end();++c_iter)
	{
		if(c_iter->bMercenary)
		{
			SaveMercenary(iBattleIndex, rkGuildID, c_iter->kCharGuid);
			kContUser.push_back(c_iter->kCharGuid);
		}
	}

	if(pkElement)
	{
		pkElement->SaveMercenary(bAttacker, kContUser);
	}

	return S_OK;
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SendReward(bool const bWinner, CONT_EM_RESULT_USER const & rkMember, int const iBattleIndex, const BM::GUID& rkWinGuildGuid, SResultRewardInfo & kRewardInfo )const
{
	if( rkMember.empty() ){ return E_FAIL; }

	CONT_DEF_EMPORIA const* pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef ){ return E_FAIL; }

	CONT_DEF_EMPORIA::const_iterator c_itDef = pkDef->find( GetID() );
	if(c_itDef == pkDef->end()){ return E_FAIL; }
	TBL_DEF_EMPORIA_REWARD const & rkReward = c_itDef->second.kReward;

	int iItemNo = 0;
	__int64 i64Money = 0;
	int iAchieveSaveIdx = 0;
	std::wstring kGradeText;
	if(iBattleIndex < PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX)
	{
		int idx = 0;
		if(3==iBattleIndex || 4==iBattleIndex || 5==iBattleIndex || 6==iBattleIndex)
		{
			idx = bWinner ? TBL_DEF_EMPORIA_REWARD::ECG_SEMI : TBL_DEF_EMPORIA_REWARD::ECG_QUARTER;
		}
		else if(1==iBattleIndex || 2==iBattleIndex)
		{
			idx = bWinner ? TBL_DEF_EMPORIA_REWARD::ECG_SECOND : TBL_DEF_EMPORIA_REWARD::ECG_SEMI;
		}
		else if(0==iBattleIndex)
		{
			idx = bWinner ? TBL_DEF_EMPORIA_REWARD::ECG_WINNER : TBL_DEF_EMPORIA_REWARD::ECG_SECOND;
		}

		iItemNo = rkReward.kChallenge[idx].iItem;
		i64Money = rkReward.kChallenge[idx].iMoney;
		iAchieveSaveIdx = PgEmporia::GetRewardAchievementNo(idx,true);
		GetDefString(rkReward.kChallenge[idx].iGradeTextNo, kGradeText);
	}
	else
	{
		int const iGradeNo = iBattleIndex-PgEmporiaTournament::EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX;
		if(iGradeNo < MAX_EMPORIA_GRADE)
		{
			if(0==iGradeNo && !bWinner)
			{
				iItemNo = rkReward.kChallenge[TBL_DEF_EMPORIA_REWARD::ECG_WINNER].iItem;
				i64Money = rkReward.kChallenge[TBL_DEF_EMPORIA_REWARD::ECG_WINNER].iMoney;
				iAchieveSaveIdx = PgEmporia::GetRewardAchievementNo(TBL_DEF_EMPORIA_REWARD::ECG_WINNER,true);
				GetDefString(rkReward.kChallenge[TBL_DEF_EMPORIA_REWARD::ECG_WINNER].iGradeTextNo, kGradeText);
			}
			else
			{
				int const idx = bWinner ? iGradeNo : iGradeNo-1;
				if(idx < MAX_EMPORIA_GRADE)
				{
					iItemNo = rkReward.kGrade[idx].iItem;
					i64Money = rkReward.kGrade[idx].iMoney;
					iAchieveSaveIdx = PgEmporia::GetRewardAchievementNo(idx,false);
					GetDefString(rkReward.kGrade[idx].iGradeTextNo, kGradeText);
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia SendReward Error EmporiaID<") << GetID() << _T(">") );
				}
			}
		}
	}

	INFO_LOG( BM::LOG_LV5, __FL__ << _T("Emporia SendReward ItemNo<") << iItemNo << _T(">, AchieveSaveIdx<") << iAchieveSaveIdx << _T(">, Money<") << i64Money << _T(">"));

	std::wstring kItemName;
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( pkItemDef )
	{
		GetDefString(pkItemDef->NameNo(), kItemName);
	}

	std::wstring kFrom;
	std::wstring kTitle;
	BM::vstring vBody;
	GetDefString(rkReward.iFromNo, kFrom);
	GetDefString(rkReward.iTitleNo, kTitle);
	GetDefString(rkReward.iBodyNo, vBody);
	vBody.Replace(L"#GRADE#",kGradeText);
	vBody.Replace(L"#ITEMNAME#",kItemName);
	vBody.Replace(L"#LIMITDAY#",rkReward.sLimitDay);

	if(iItemNo > 0)
	{
		CONT_EM_RESULT_USER::const_iterator c_iter = rkMember.begin();
		while(c_iter != rkMember.end())
		{//참여유저에게 제공
			PgBase_Item kItem;
			if(S_OK == CreateSItem(iItemNo, 1, GIOT_NONE, kItem))
			{
				kItem.SetUseTime(2, rkReward.sLimitDay);

				bool const bRt = g_kPostMgr.PostSystemMailByGuid(c_iter->kCharGuid, kFrom, kTitle, vBody, kItem, 0);
				if(false==bRt)
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia SendReward Error<SendMail Fail> ") << c_iter->kCharGuid );
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia SendReward Item Create Fail") );
			}

			++c_iter;
		}
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Emporia SendReward ItemNo = 0") );
	}

	if( 0!=iAchieveSaveIdx )
	{
		VEC_GUID kContRewardMember;
		CONT_EM_RESULT_USER::const_iterator c_iter = rkMember.begin();
		while(c_iter != rkMember.end())
		{
			if(!c_iter->bMercenary)
			{//보상업적은 길드원에게만 제공
				kContRewardMember.push_back(c_iter->kCharGuid);
			}

			++c_iter;
		}

		if( !kContRewardMember.empty() )
		{
			BM::Stream kAchievePacket(PT_N_M_REQ_COMPLETEACHIEVEMENT);
			kAchievePacket.Push(kContRewardMember);
			kAchievePacket.Push(iAchieveSaveIdx);
			::SendToRealmContents( PMET_SendToPacketHandler, kAchievePacket );
		}
	}

	{// 길드장 추가 업적
		BM::Stream kGPacket( PT_N_M_REQ_COMPLETEACHIEVEMENT );
		kGPacket.Push( rkWinGuildGuid );
		::SendToGuildMgr( kGPacket );
	}

	if( 0!=i64Money )
	{
		SGuild_Inventory_Log kLog( GetGuildInvLog(rkWinGuildGuid, EGIT_EMPORIA_REWARD, i64Money) );

        BM::Stream kPacket;
        kLog.WriteToPacket(kPacket);

        SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
        pkActionOrder->InsertTarget(rkWinGuildGuid);
        //pkActionOrder->kCause = CIE_EmporiaThrow;
        pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_MONEY|IMC_GUILD_INV,SModifyOrderOwner(rkWinGuildGuid,OOT_Guild),SPMOD_Add_Money(i64Money)) );
        pkActionOrder->kAddonPacket.Push(kPacket.Data());
        g_kJobDispatcher.VPush(pkActionOrder);
	}

	kRewardInfo.iItemNo = iItemNo;
	kRewardInfo.iAchieveSaveIdx = iAchieveSaveIdx;
	kRewardInfo.i64GuildGold = i64Money;

	return S_OK;
}

template< size_t iNum >
void PgEmporiaPack< iNum >::RecvSettingMercenary( BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	size_t iSize = 0;
	pkPacket->Pop( iSize );

	while ( iSize-- )
	{
		BM::GUID kGuildID;
		PgLimitClass kLimitClass;
		pkPacket->Pop( kGuildID );
		kLimitClass.ReadFromPacket( *pkPacket );

		m_kTournament.SetGuildMercenary( kGuildID, kLimitClass );
	}
}

template< size_t iNum >
HRESULT PgEmporiaPack< iNum >::SetEmporiaBattleReady( SGroundKey const &kBattleGndkey )
{
	BM::CAutoMutex kLock( m_kMutex );

	CONT_ING_BATTLE::iterator ing_itr = m_kContBattle_Ing.begin();
	for( ; ing_itr != m_kContBattle_Ing.end() ; ++ing_itr )
	{
		if ( kBattleGndkey==ing_itr->kBattleGroundKey && ing_itr->pkElement )
		{
			PgEmporiaTournamentElement * pkElement = ing_itr->pkElement;
			pkElement->SetState( pkElement->GetState() | PgEmporiaTournamentElement::STATE_RECV_START );

			SEmporiaGuildInfo kAttackGuildInfo;
			SEmporiaGuildInfo kDefenceGuildInfo;
			pkElement->GetGuildInfo( true, kAttackGuildInfo );
			pkElement->GetGuildInfo( false, kDefenceGuildInfo );

			BM::Stream kSendPacket;//생성자에 넣으면 타입이 맞지 않아 안됨
			kSendPacket.Push( NOTICE_EMPORIABATTLE_READY );
			kSendPacket.Push( g_kEventView.GetLocalSecTime(CGameTime::DEFAULT) );
			kSendPacket.Push( ing_itr->pkElement->GetIndex() );
			kSendPacket.Push( kAttackGuildInfo.wstrName );
			kSendPacket.Push( kDefenceGuildInfo.wstrName );
			
			//길드전 시작 알림
			BM::Stream kNotiPacket;
			kNotiPacket.Push(E_NoticeMsg_SendPacket);
			kNotiPacket.Push(NOTICE_ALL);
			kNotiPacket.Push(BM::GUID::NullData());
			kNotiPacket.Push(kSendPacket);
			SendToNotice(kNotiPacket);
			return S_OK;
		}
	}
	return E_FAIL;
}

template< size_t iNum >
__int64 PgEmporiaPack< iNum >::GetBattleTime()const
{
	return m_i64BattleTime;
}