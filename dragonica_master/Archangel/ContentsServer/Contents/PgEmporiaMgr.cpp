#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "PgGMProcessMgr.h"
#include "PgEmporiaMgr.h"

__int64 EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TIMETERM[EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT-1] = { CGameTime::OneDay*6, CGameTime::OneDay*4, CGameTime::OneDay*5, 0i64, CGameTime::OneDay, CGameTime::OneDay*2, CGameTime::OneDay*3 };
__int64 EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM[MAX_EMPORIA_GRADE] = { (CGameTime::OneDay*7) + (CGameTime::HOUR*2), (CGameTime::OneDay*8) + (CGameTime::HOUR*2), (CGameTime::OneDay*9) + (CGameTime::HOUR*2), (CGameTime::OneDay*10) + (CGameTime::HOUR*2), (CGameTime::OneDay*11) + (CGameTime::HOUR*2) };


SGuild_Inventory_Log GetGuildInvLog(BM::GUID const & kGuildID, EGuildInvTradeType const eType, __int64 const i64Money)
{
	int const iNameNo = (eType == EGIT_GM_ORDER) ? 12019 : 2000000108; //GM : 엠포리아 관리자

    std::wstring kName;
    ::GetDefString( iNameNo, kName );

    BM::DBTIMESTAMP_EX kNowTime;
    kNowTime.SetLocalTime();
    return SGuild_Inventory_Log(kGuildID, BM::GUID::NullData(), kNowTime, kName, eType, 0, i64Money);
}

PgEmporiaMgr::PgEmporiaMgr()
{

}

PgEmporiaMgr::~PgEmporiaMgr()
{

}

bool PgEmporiaMgr::LoadEmporiaBattleTime(void)
{
	/* 일단 이거는 사용 안한다.
	{
		int iStart = 0, iEnd = 0;
		if ( SUCCEEDED(g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_BeginChallenge_BeforeBasicMinTime, iStart )) )
		{
			if ( 0 < iStart )
			{
				EMPORIA_BATTLE_CHALLENGE_STARTTIME_BEFORE_BATTLETIME = static_cast<__int64>(iStart) * CGameTime::MINUTE;
			}
		}

		if ( SUCCEEDED(g_kVariableContainer.Get( EVar_Kind_Emporia, EVar_EndChallenge_BeforeBasicMinTime, iEnd )) )
		{
			if ( (0 <= iEnd) && (iStart > iEnd) )
			{
				EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME = static_cast<__int64>(iEnd) * CGameTime::MINUTE;
			}
			else
			{
				if ( EMPORIA_BATTLE_CHALLENGE_STARTTIME_BEFORE_BATTLETIME < EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME )
				{
					EMPORIA_BATTLE_CHALLENGE_ENDTIME_BEFORE_BATTLETIME = EMPORIA_BATTLE_CHALLENGE_STARTTIME_BEFORE_BATTLETIME;
				}
			}
		}
	}
	*/
	
	// 여기서 데이터 체크를 해보고, 데이터가 맞지 않으면 적용하지 않는다.
	// 서버 시작 초기에 호출하고 리로드 하지 말자.(쓰레드 세이프 되지 않는다)
	__int64 i64ChallengeTournamentTimeTerm[EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TEAMCOUNT - 1] = {0i64,};
	__int64 i64EmporiaTournamentTimeTerm[MAX_EMPORIA_GRADE] = {0i64,};

	int iMaxValue = 0;
	int i = 0;

	// 1. 8강
	for ( i=EVar_ChallengeTournament_1stOfQuarterFinal_AfterBasicMinTime; i<=EVar_ChallengeTournament_4stOfQuarterFinal_AfterBasicMinTime; ++i )
	{
		int iValue = 0;
		g_kVariableContainer.Get( EVar_Kind_Emporia, i, iValue );
		i64ChallengeTournamentTimeTerm[i-1] = static_cast<__int64>(iValue) * CGameTime::MINUTE;

		if ( iValue > iMaxValue )
		{
			iMaxValue = iValue;
		}
	}

	int iChkMinValue = iMaxValue;

	// 4강
	for ( i=EVar_ChallengeTournament_1stOfSemiFinal_AfterBasicMinTime; i<=EVar_ChallengeTournament_2ndOfSemiFinal_AfterBasicMinTime; ++i )
	{
		int iValue = 0;
		g_kVariableContainer.Get( EVar_Kind_Emporia, i, iValue );

		if ( iValue <= iChkMinValue )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, L"EVar_ChallengeTournament Time Error!!! Type<" << i << "> Value<" << iValue << L">" );
			return false;
		}

		i64ChallengeTournamentTimeTerm[i-1] = static_cast<__int64>(iValue) * CGameTime::MINUTE;
		if ( iValue > iMaxValue )
		{
			iMaxValue = iValue;
		}
	}

	iChkMinValue = iMaxValue;

	// 결승
	{
		i = EVar_ChallengeTournament_Final_AfterBasicMinTime;
		int iValue = 0;
		g_kVariableContainer.Get( EVar_Kind_Emporia, i, iValue );

		if ( iValue <= iChkMinValue )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, L"EVar_ChallengeTournament Time Error!!! Type<" << EVar_ChallengeTournament_Final_AfterBasicMinTime << "> Value<" << iValue << L">" );
			return false;
		}

		i64ChallengeTournamentTimeTerm[i-1] = static_cast<__int64>(iValue) * CGameTime::MINUTE;
		iChkMinValue = iMaxValue = iValue;
	}


	// 엠포리아 토너먼트
	for ( i = 0; i<MAX_EMPORIA_GRADE; ++i )
	{
		int iValue = 0;
		g_kVariableContainer.Get( EVar_Kind_Emporia, i+EVar_EmporiaTournament_1st_AfterBasicMinTime, iValue );

		if ( iValue <= iChkMinValue )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, L"EVar_EmporiaTournament Time Error!!! Type<" << (i+EVar_EmporiaTournament_1st_AfterBasicMinTime) << "> Value<" << iValue << L">" );
			return false;
		}

		i64EmporiaTournamentTimeTerm[i] = static_cast<__int64>(iValue) * CGameTime::MINUTE;
		iChkMinValue = iValue;
	}
	

	::memcpy( EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TIMETERM, i64ChallengeTournamentTimeTerm, sizeof(EMPORIA_BATTLE_CHALLENGE_TOURNAMENT_TIMETERM) );
	::memcpy( EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM, i64EmporiaTournamentTimeTerm, sizeof(EMPORIA_BATTLE_EMPORIA_TOURNAMENT_TIMETERM) );
	return true;
}

void PgEmporiaMgr::LoadDB(void)const
{
	{	
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LOAD_EMPORIA, _T( "EXEC [dbo].[UP_LoadEmporia]") );
		g_kCoreCenter.PushQuery(kQuery);
	}
}

bool PgEmporiaMgr::Locked_Q_DQT_LOAD_EMPORIA( CEL::DB_RESULT &rkResult )
{
	CONT_DEF_EMPORIA const * pkContDefEmporia = NULL;
	g_kTblDataMgr.GetContDef( pkContDefEmporia );
	if ( !pkContDefEmporia )
	{
		ASSERT_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error CONT_DEF_EMPORIA is NULL" );
		return false;
	}
	
	CONT_EMPORIA_PACK		kContEmporiaPack;
	CONT_EMPORIA_FROMGUILD	kContEmporiaFromGuild;
	CONT_TBL_EMPORIA_CHALLENGE_BATTLE2	kContEmporiaChallengeBattle;
	CONT_TBL_EMPORIA_TOURNAMENT2		kContEmporiaTournament;
	CONT_TBL_EM_MERCENARY	kContEmporiaMercenary;

	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
			if ( itr!= rkResult.vecArray.end() )
			{
				if ( 6 != rkResult.vecResultCount.size() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("SELECT Table Count <") << rkResult.vecResultCount.size() << _T("> is ERROR!!") );
					return false;
				}

				__int64 const i64CurTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

				int iPackCount = rkResult.vecResultCount.at(0);
				int iElementCount = rkResult.vecResultCount.at(1);
				int iFunctionCount = rkResult.vecResultCount.at(2);
				int iChallengeBattleCount = rkResult.vecResultCount.at(3);
				int iTournamentCount = rkResult.vecResultCount.at(4);

				while ( itr != rkResult.vecArray.end() )
				{
					if ( iPackCount > 0 )
					{
						BM::GUID			kEmporiaID;
						int					iStatus = 0;
						char				cBattlePeriodsForWeeks = 0;
						BM::GUID			kBattleID;
						BM::DBTIMESTAMP_EX	kBattleDate;
						BM::GUID			kPrevBattleID;

						itr->Pop( kEmporiaID );					++itr;
						itr->Pop( iStatus );					++itr;
						itr->Pop( cBattlePeriodsForWeeks );		++itr;
						itr->Pop( kBattleID );					++itr;
						itr->Pop( kBattleDate );				++itr;
						itr->Pop( kPrevBattleID );				++itr;
						--iPackCount;

						CONT_DEF_EMPORIA::const_iterator def_itr = pkContDefEmporia->find( kEmporiaID );
						if ( def_itr != pkContDefEmporia->end() )
						{
							T_EMPORIA_PACK* pkPack = new T_EMPORIA_PACK( kEmporiaID, iStatus, cBattlePeriodsForWeeks, kBattleID, kBattleDate, kPrevBattleID, SGroundKey( def_itr->second.iBaseMapNo ) );
							if ( pkPack )
							{
								auto kPair = kContEmporiaPack.insert( std::make_pair( kEmporiaID, pkPack ) );
								if ( !kPair.second )
								{
									VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Overlapping EmporiaID<") << kEmporiaID << _T(">") );
									SAFE_DELETE( pkPack );
								}
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Memoria New Error!! EmporiaID<") << kEmporiaID << _T(">") );
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Not Found CONT_DEF_EMPORIA EmporiaID<") << kEmporiaID << _T(">") );
						}
						
					}
					else if ( iElementCount > 0 )
					{
						SEmporiaKey			kEmporiaKey;
						BYTE				byStatus = EMPORIA_CLOSE;
						SEmporiaGuildInfo	kOwner;
						BM::DBTIMESTAMP_EX	kHaveDate;

						itr->Pop( kEmporiaKey.kID );		++itr;
						itr->Pop( kEmporiaKey.byGrade );	++itr;
						itr->Pop( byStatus );				++itr;
						itr->Pop( kOwner.kGuildID );		++itr;
						itr->Pop( kHaveDate );				++itr;
						itr->Pop( kOwner.wstrName );		++itr;
						itr->Pop( kOwner.nEmblem );			++itr;
						--iElementCount;

						CONT_EMPORIA_PACK::iterator pack_itr = kContEmporiaPack.find( kEmporiaKey.kID );
						if ( pack_itr != kContEmporiaPack.end() )
						{
							PgEmporia kEmporia( kEmporiaKey, kOwner, byStatus, kHaveDate );
							if ( true == pack_itr->second->SetElement( static_cast<size_t>(kEmporiaKey.byGrade-1), kEmporia ) )
							{
								if ( !kOwner.IsEmpty() )
								{
									CONT_EMPORIA_FROMGUILD::mapped_type kElement( kEmporiaKey, kOwner );
									kElement.bOwner = true;

									auto kPair = kContEmporiaFromGuild.insert( std::make_pair( kOwner.kGuildID, kElement ) );
									if ( !kPair.second )
									{
										VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Error Emporia<") << kEmporiaKey.kID << _T("> Grade<") << kEmporiaKey.byGrade << _T("> OwnerGuild<") << kOwner.kGuildID << _T("/") << kOwner.wstrName << _T("> Have More Emporia") );
									}
								}
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Error Emporia<") << kEmporiaKey.kID << _T("> Grade<") << kEmporiaKey.byGrade << _T("> OwnerGuild<") << kOwner.kGuildID << _T("/") << kOwner.wstrName << _T(">") );
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Not Found EmporiaID<") << kEmporiaKey.kID << _T("> Emporia OwnerGuild<") << kOwner.kGuildID << _T("/") << kOwner.wstrName << _T(">") );
						}
					}
					else if ( iFunctionCount > 0 )
					{
						SEmporiaKey			kEmporiaKey;
						short				nFuncNo = 0;
						BM::DBTIMESTAMP_EX	kHaveDate;
						BM::DBTIMESTAMP_EX	kExpirationDate;
						__int64				i64ExtValue = 0i64;
						itr->Pop( kEmporiaKey.kID );		++itr;
						itr->Pop( kEmporiaKey.byGrade );	++itr;
						itr->Pop( nFuncNo );				++itr;
						itr->Pop( kHaveDate );				++itr;
						itr->Pop( kExpirationDate );		++itr;
						itr->Pop( i64ExtValue );			++itr;

						CONT_EMPORIA_PACK::iterator pack_itr = kContEmporiaPack.find( kEmporiaKey.kID );
						if ( pack_itr != kContEmporiaPack.end() )
						{
							SEmporiaFunction kFunction( kHaveDate, kExpirationDate, i64ExtValue );
							if ( kFunction.i64ExpirationDate > i64CurTime )
							{
								if ( FAILED( pack_itr->second->AddFunction( static_cast<size_t>(kEmporiaKey.byGrade-1), nFuncNo, kFunction ) ) )
								{
									VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Error Emporia<") << kEmporiaKey.kID << _T("> Grade<") << kEmporiaKey.byGrade << L"> FunctionNo<" << nFuncNo << L">" );
								}
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found EmporiaID<" << kEmporiaKey.kID << L">" );
						}
						--iFunctionCount;
					}
					else if ( iChallengeBattleCount > 0 )
					{
						CONT_TBL_EMPORIA_CHALLENGE_BATTLE2::key_type kKey;
						TBL_EMPORIA_CHALLENGE_BATTLE kElement;
						BM::DBTIMESTAMP_EX kChallengeDate;
						itr->Pop( kKey.kEmporiaID );				++itr;
						itr->Pop( kKey.kBattleID );					++itr;
						itr->Pop( kElement.kGuildInfo.kGuildID );	++itr;
						itr->Pop( kElement.i64Cost );				++itr;
						itr->Pop( kChallengeDate );					++itr;
						CGameTime::DBTimeEx2SecTime( kChallengeDate, kElement.i64ChallengeTime, CGameTime::DEFAULT );
						itr->Pop( kElement.kGuildInfo.wstrName );	++itr;
						itr->Pop( kElement.kGuildInfo.nEmblem );	++itr;
						itr->Pop( kElement.iState );				++itr;
						--iChallengeBattleCount;

						CONT_TBL_EMPORIA_CHALLENGE_BATTLE2::iterator itr = kContEmporiaChallengeBattle.find( kKey );
						if ( itr == kContEmporiaChallengeBattle.end() )
						{
							auto kPair = kContEmporiaChallengeBattle.insert( std::make_pair( kKey, CONT_TBL_EMPORIA_CHALLENGE_BATTLE2::mapped_type() ) );
							itr = kPair.first;
						}

						if ( itr != kContEmporiaChallengeBattle.end() )
						{
							itr->second.push_back( kElement );
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error EmporiaID<") << kKey.kEmporiaID << _T("> BattleID<") << kKey.kBattleID << _T(">") );
						}
					}
					else if ( iTournamentCount > 0 )
					{
						CONT_TBL_EMPORIA_TOURNAMENT2::key_type kKey;
						TBL_EMPORIA_TOURNAMENT kElement;
						itr->Pop( kKey.kEmporiaID );		++itr;
						itr->Pop( kKey.kBattleID );			++itr;
						itr->Pop( kElement.nIndex );		++itr;
						itr->Pop( kElement.kBattleDate );	++itr;
						itr->Pop( kElement.kWinGuildID );	++itr;
						itr->Pop( kElement.kLoseGuildID );	++itr;
						--iTournamentCount;

						CONT_TBL_EMPORIA_TOURNAMENT2::iterator itr = kContEmporiaTournament.find( kKey );
						if ( itr == kContEmporiaTournament.end() )
						{
							auto kPair = kContEmporiaTournament.insert( std::make_pair( kKey, CONT_TBL_EMPORIA_TOURNAMENT2::mapped_type() ) );
							itr = kPair.first;
						}

						if ( itr != kContEmporiaTournament.end() )
						{
							itr->second.insert( kElement );
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error EmporiaID<") << kKey.kEmporiaID << _T("> BattleID<") << kKey.kBattleID << _T(">") );
						}
					}
					else
					{
						CONT_TBL_EM_MERCENARY::key_type kKey;
						TBL_EM_MERCENARY_INDEX kIndex;
						TBL_EM_MERCENARY_JOINGUILD kJoinGuild;
						BM::GUID kJoinGuildID;
						BM::GUID kCharID;
						
						itr->Pop( kKey.kEmporiaID );	++itr;
						itr->Pop( kKey.kBattleID );		++itr;
						itr->Pop( kIndex.nIndex );		++itr;
						itr->Pop( kJoinGuildID );		++itr;
						itr->Pop( kCharID );			++itr;

						CONT_TBL_EM_MERCENARY::iterator it_1 = kContEmporiaMercenary.find( kKey );
						if ( it_1 == kContEmporiaMercenary.end() )
						{
							auto kPair = kContEmporiaMercenary.insert( std::make_pair( kKey, CONT_TBL_EM_MERCENARY::mapped_type() ) );
							it_1 = kPair.first;
						}

						if ( it_1 != kContEmporiaMercenary.end() )
						{
							CONT_TBL_EM_MERCENARY_INDEX::iterator it_2 = it_1->second.find( kIndex.nIndex );
							if ( it_2 == it_1->second.end() )
							{
								auto kPair = it_1->second.insert( CONT_TBL_EM_MERCENARY_INDEX::value_type(kIndex.nIndex) );
								it_2 = kPair.first;
							}

							if ( it_2 != it_1->second.end() )
							{
								CONT_TBL_EM_MERCENARY_JOINGUILD::iterator it_3 = it_2->kContJoinGuild.find( kJoinGuildID );

								// Const cast is okay in these cases
								// because this element doesn't do anything for generate indexes in hash map
								if ( it_3 == it_2->kContJoinGuild.end() )
								{
									auto kPair = const_cast<TBL_EM_MERCENARY_INDEX*>(&(*it_2))->kContJoinGuild.insert(CONT_TBL_EM_MERCENARY_JOINGUILD::value_type(kJoinGuildID));
									it_3 = kPair.first;
								}

								if ( it_3 != it_2->kContJoinGuild.end() )
								{
									const_cast<TBL_EM_MERCENARY_JOINGUILD*>(&(*it_3))->kContCharID.push_back(kCharID);
								}
								else
								{
									VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error Mercenary JoinGuildID EmporiaID<") << kKey.kEmporiaID << _T("> BattleID<") << kKey.kBattleID << _T(">") );
								}
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error Mercenary Index EmporiaID<") << kKey.kEmporiaID << _T("> BattleID<") << kKey.kBattleID << _T(">") );
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error Mercenary EmporiaID<") << kKey.kEmporiaID << _T("> BattleID<") << kKey.kBattleID << _T(">") );
						}
					}
				}
			}	
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Can't Load [TB_EMPORIA] table Result : ") << rkResult.eRet );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}

	{
		CONT_TBL_EMPORIA_TOURNAMENT kDummyTournament;
		CONT_TBL_EMPORIA_CHALLENGE_BATTLE2::const_iterator battle_itr = kContEmporiaChallengeBattle.begin();
		for ( ; battle_itr!=kContEmporiaChallengeBattle.end() ; ++battle_itr )
		{
			CONT_EMPORIA_PACK::iterator pack_itr = kContEmporiaPack.find( battle_itr->first.kEmporiaID );
			if ( pack_itr != kContEmporiaPack.end() )
			{
				CONT_TBL_EMPORIA_TOURNAMENT2::const_iterator tour_itr = kContEmporiaTournament.find( battle_itr->first );
				if ( tour_itr != kContEmporiaTournament.end() )
				{
					pack_itr->second->InitTournament( battle_itr->first.kBattleID, tour_itr->second, battle_itr->second, kContEmporiaFromGuild );
				}
				else
				{	
					pack_itr->second->InitTournament( battle_itr->first.kBattleID, kDummyTournament, battle_itr->second, kContEmporiaFromGuild );
				}

				pack_itr->second->InitMercenary( kContEmporiaMercenary );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Can't Find Emporia : ") << battle_itr->first.kEmporiaID );
			}
		}
	}
	
	{
		CONT_EMPORIA_PACK::iterator pack_itr = kContEmporiaPack.begin();
		for ( ; pack_itr != kContEmporiaPack.end() ; ++pack_itr )
		{
			pack_itr->second->CheckData();
		}
	}


	bool const bRet = Locked_Build( kContEmporiaPack, kContEmporiaFromGuild );
	Release( kContEmporiaPack );
	return bRet;
}

bool PgEmporiaMgr::Locked_Q_DQT_CREATE_EMPORIA( CEL::DB_RESULT &rkResult )
{
	if(		CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		BM::CAutoMutex kLock( m_kMutex, true );
		CloseEmporia( rkResult.QueryOwner(), true );

		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Create Emporia<") << rkResult.QueryOwner() << _T("> Failed __ Release Emporia") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Create Emporia<") << rkResult.QueryOwner() << _T("> Success") );
	return true;
}

bool PgEmporiaMgr::Locked_Build( CONT_EMPORIA_PACK &rkContEmporiaPack, CONT_EMPORIA_FROMGUILD &kContEmporiaFromGuild  ) 
{
	// Reload해도 잘 되게끔 여기서 잘 처리 한다.(아직 처리 안됨)
	CONT_DEF_EMPORIA const *pkDefEmporia = NULL;
	g_kTblDataMgr.GetContDef( pkDefEmporia );
	if ( !pkDefEmporia )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_EMPORIA_PACK is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		Release( rkContEmporiaPack );
		return false;
	}

	// Data Check!!
	CONT_DEF_EMPORIA kTempDef = *pkDefEmporia;

	// USER DB에 있는데 Def에 없는것 확인하기.
	CONT_EMPORIA_PACK::iterator pack_itr = rkContEmporiaPack.begin();
	while ( pack_itr!=rkContEmporiaPack.end() )
	{
		CONT_EMPORIA_PACK::mapped_type pkPack = pack_itr->second;
		CONT_DEF_EMPORIA::iterator def_itr = kTempDef.find( pack_itr->first );
		if ( def_itr != kTempDef.end() )
		{
//			pkEmporia->Init();

			// 찾았으면 TempDef에서 지워(중복안하게 하기 위해서)
			kTempDef.erase( def_itr );
			++pack_itr;
		}
		else
		{
			// 일단 없으니까 메모리에서만 지워야 한다.
//			VERIFY_INFO_LOG_NEW( false, BM::LOG_LV4, __FL__ << _T("Emporia<") << pack_itr->first << _T("> OwnerGuild<") << pkEmporia->GetOwnerGuild() << _T("> DELETE!!!") );
//			pkEmporia->ReleaseReserver();
			pack_itr = rkContEmporiaPack.erase( pack_itr );
		}
	}

	// Def에 있는데 USER DB에 없으면 생성하자!!
	CONT_DEF_EMPORIA::const_iterator def_itr = kTempDef.begin();
	for ( ; def_itr!=kTempDef.end() ; ++def_itr )
	{
		char const nDefaultBattlePeriodForWeeks = 3;

		CONT_DEF_EMPORIA::mapped_type const &kElement = def_itr->second;
		T_EMPORIA_PACK* pkPack = new T_EMPORIA_PACK( kElement.guidEmporiaID, EMPORIA_CLOSE, nDefaultBattlePeriodForWeeks, BM::GUID::NullData(), BM::DBTIMESTAMP_EX(), BM::GUID::NullData(), SGroundKey( kElement.iBaseMapNo ) );
		if ( pkPack )
		{
			auto kPair = rkContEmporiaPack.insert( std::make_pair( kElement.guidEmporiaID, pkPack ) );
			if ( true == kPair.second )
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CREATE_EMPORIA, _T("EXEC [dbo].[UP_CreateEmporia]"));
				kQuery.InsertQueryTarget(kElement.guidEmporiaID);
				kQuery.QueryOwner(kElement.guidEmporiaID);

				kQuery.PushStrParam( kElement.guidEmporiaID );
				kQuery.PushStrParam( nDefaultBattlePeriodForWeeks );

				if ( S_OK == g_kCoreCenter.PushQuery(kQuery) )
				{
					INFO_LOG( BM::LOG_LV6, __FL__ << _T("Request Create Emporia<") << kElement.guidEmporiaID << _T(">") );
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Request Create Emporia<") << kElement.guidEmporiaID << _T("> Failed") );
					SAFE_DELETE(pkPack);
					rkContEmporiaPack.erase( kPair.first );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Overlapping EmporiaID<") << kElement.guidEmporiaID << _T(">") );
				SAFE_DELETE( pkPack );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Memoria New Error!! EmporiaID<") << kElement.guidEmporiaID << _T(">") );
		}
	}

	{
		BM::CAutoMutex kLock( m_kMutex, true );
		m_kContEmporiaPack.swap( rkContEmporiaPack );
		m_kContEmporiaFromGuild.swap( kContEmporiaFromGuild );
	}

	return true;
}

HRESULT PgEmporiaMgr::Locked_Q_DQT_SWAP_EMPORIA( CEL::DB_RESULT_TRAN &rkContResult )
{
	CEL::DB_RESULT_TRAN::iterator itr = rkContResult.begin();
	for( ;itr != rkContResult.end();++itr)
	{
		Locked_Q_DQT_SAVE_EMPORIA( *itr );
	}
	return S_OK;
}

void PgEmporiaMgr::Locked_Q_DQT_SAVE_EMPORIA( CEL::DB_RESULT &rkResult )
{
	// Lock은 무조건 {}안에서만 잡아야 한다!!

	bool bRet = ((CEL::DR_SUCCESS == rkResult.eRet) || (CEL::DR_NO_RESULT == rkResult.eRet));
	switch( rkResult.QueryType() )
	{
	case DQT_SAVE_EMPORIA:
		{
			int iError = 0;
			BYTE byGrade = 0;
			
			CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
			if ( itr!= rkResult.vecArray.end() )
			{
				itr->Pop( iError );				++itr;
				itr->Pop( byGrade );			++itr;
			}

			bRet = ( !iError && (itr != rkResult.vecArray.end()) );

			if ( true == bRet )
			{
				//!! Lock 잡았음
				BM::CAutoMutex kWLock( m_kMutex, true );

				if ( itr != rkResult.vecArray.end() )
				{
					SEmporiaGuildInfo kGuildInfo;
					itr->Pop( kGuildInfo.kGuildID );	++itr;
					itr->Pop( kGuildInfo.wstrName );	++itr;
					itr->Pop( kGuildInfo.nEmblem );		++itr;

					if ( !kGuildInfo.IsEmpty() )
					{
						CONT_EMPORIA_FROMGUILD::iterator guild_itr = m_kContEmporiaFromGuild.find( kGuildInfo.kGuildID );
						if ( guild_itr != m_kContEmporiaFromGuild.end() )
						{
							if ( guild_itr->second.kEmporiaKey.GetKey() == rkResult.QueryOwner() )
							{
								T_EMPORIA_PACK *pkPack = GetEmporiaPack( rkResult.QueryOwner() );
								if ( pkPack )
								{
									guild_itr->second.bOwner = true;
									guild_itr->second.kEmporiaKey.byGrade = byGrade;

									SGuildEmporiaInfo kEmporiaInfo(EMPORIA_KEY_MINE);
									kEmporiaInfo.kID = rkResult.QueryOwner();
									kEmporiaInfo.byGrade = byGrade;

									BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kGuildInfo.kGuildID );
									kGPacket.Push( kEmporiaInfo );
									::SendToGuildMgr( kGPacket );

									// 엠포리아로 정보를 주어야 한다.
									size_t const iEmporiaIndex = static_cast<size_t>(byGrade-1);
									BM::Stream kMPacket( PT_N_M_NFY_EMPORIA_INFO );
									if ( S_OK == pkPack->WriteToPacket_Grade( iEmporiaIndex, kMPacket, true ) )
									{	
										pkPack->Send( iEmporiaIndex, kMPacket );
									}

									// Gate 정보를 업데이트 해주어야 한다.
									BM::Stream kGatePacket( PT_N_M_NFY_EMPORIA_PORTAL_INFO, static_cast<size_t>(1) );
									pkPack->WriteToPacket_Grade( iEmporiaIndex, kGatePacket, true );
									::SendToGround( ALL_CHANNEL_NUM, pkPack->GetBaseMapKey(), kGatePacket, true );
								}
								else
								{
									CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Not Found Emporia<") << rkResult.QueryOwner() << _T(">") );
								}
							}
							else
							{
								CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Error Guild<") << kGuildInfo.kGuildID << _T("> EmporiaInfo Different -> EmporiaID_DBRet<") << rkResult.QueryOwner() << _T("> EmporiaID_Memory<") << guild_itr->second.kEmporiaKey.GetKey() << _T(">") );
							}
						}
						else
						{
							CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Not Found Guild<") << kGuildInfo.kGuildID << _T("> Emporia<") << rkResult.QueryOwner() << _T(">") );
						}
					}
					else
					{
						T_EMPORIA_PACK *pkPack = GetEmporiaPack( rkResult.QueryOwner() );
						if ( pkPack )
						{
							size_t const iEmporiaIndex = static_cast<size_t>(byGrade-1);
							// Gate 정보를 업데이트 해주어야 한다.
							BM::Stream kGatePacket( PT_N_M_NFY_EMPORIA_PORTAL_INFO, static_cast<size_t>(1) );
							pkPack->WriteToPacket_Grade( iEmporiaIndex, kGatePacket, true );
							::SendToGround( ALL_CHANNEL_NUM, pkPack->GetBaseMapKey(), kGatePacket, true );
						}
					}
				}

				bool bOldGuildInfo = false;
				rkResult.contUserData.Pop( bOldGuildInfo );

				if ( true == bOldGuildInfo )
				{
					SEmporiaGuildInfo kOldGuildInfo;
					if ( true == kOldGuildInfo.ReadFromPacket( rkResult.contUserData ) )
					{
						SGuildEmporiaInfo kEmporiaInfo;

						BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, kOldGuildInfo.kGuildID );
						kGPacket.Push( kEmporiaInfo );
						::SendToGuildMgr( kGPacket );

						m_kContEmporiaFromGuild.erase( kOldGuildInfo.kGuildID );
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!") );
					}
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Save Failed Emporia<") << rkResult.QueryOwner() << _T(">") );
			}
		}break;
	case DQT_SAVE_EMPORIA_PACK:
		{
			int iError = 0;

			CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
			if ( itr!= rkResult.vecArray.end() )
			{
				itr->Pop( iError );				++itr;
			}
		}break;
	case DQT_SAVE_EMPORIA_RESERVE:
	case DQT_SAVE_EMPORIA_THROW:
        {
            int iError = 1;
            __int64 i64Money = 0i64;

            CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
            if ( itr!= rkResult.vecArray.end() )
            {
                itr->Pop( iError );				++itr;
                if ( itr!= rkResult.vecArray.end() )
                {
                    itr->Pop( i64Money );	++itr;
                }
            }

            BM::GUID kReqCharGuid;
            BM::GUID kGuildID;
            BM::GUID kEmporiaID;
            BYTE byType = GCR_Success;
            SGuildEmporiaInfo kEmporiaInfo;
            rkResult.contUserData.Pop( kReqCharGuid );
            rkResult.contUserData.Pop( kGuildID );
            rkResult.contUserData.Pop( kEmporiaID );
            rkResult.contUserData.Pop( byType );
            rkResult.contUserData.Pop( kEmporiaInfo.i64BattleTime );

            //!! Lock 잡았음
            BM::CAutoMutex kWLock( m_kMutex, true );

            if ( !iError )
            {
                // 에러가 아니다.
                if ( DQT_SAVE_EMPORIA_RESERVE == rkResult.QueryType() )
                {
                    kEmporiaInfo.byType = EMPORIA_KEY_BATTLERESERVE;
                    kEmporiaInfo.kID = kEmporiaID;
                }
                else
                {
                    m_kContEmporiaFromGuild.erase( kGuildID );
                    if( GCR_System!=byType )
                    {   //GCR_Success, GCR_Failed이 오면 수수료 부과
                        i64Money -= (i64Money * 0.1); //10%의 수수료 부과
                    }
                }

                BM::Stream kSuccessPacket( PT_N_N_ANS_EMPORIA_BATTLE_RESERVE, kGuildID );
                kSuccessPacket.Push( kReqCharGuid );
                kSuccessPacket.Push( kEmporiaInfo );
                kSuccessPacket.Push( i64Money );
                kSuccessPacket.Push( DQT_SAVE_EMPORIA_RESERVE!=rkResult.QueryType() );
                kSuccessPacket.Push( byType );
                ::SendToGuildMgr( kSuccessPacket );
            }
            else
            {
                // 에러네..
                if ( DQT_SAVE_EMPORIA_RESERVE == rkResult.QueryType() )
                {
                    byType = GCR_None;

                    T_EMPORIA_PACK* pkPack = GetEmporiaPack( kEmporiaID );
                    if ( pkPack )
                    {
                        SEmporiaChallenge kChallenge;
                        pkPack->RemoveChallenge( kGuildID, &kChallenge, true );
                        i64Money = kChallenge.i64Cost;
                    }

                    m_kContEmporiaFromGuild.erase( kGuildID );
                }
                else
                {
                    CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Error!!! GuildID<") << kGuildID << _T("> EmporiaID<") << kEmporiaID << _T(">") );
                }

                // 포기는 실패했어도 일단 포기 처리를 해야 한다...
                BM::Stream kFailedPacket( PT_N_N_ANS_EMPORIA_BATTLE_RESERVE, kGuildID );
                kFailedPacket.Push( kReqCharGuid );
                kFailedPacket.Push( kEmporiaInfo );
                kFailedPacket.Push( i64Money );
                kFailedPacket.Push( false );
                kFailedPacket.Push( byType );
                ::SendToGuildMgr( kFailedPacket );
            }
		}break;
	case DQT_SAVE_EMPORIA_BATTLE_STATE:
		{
			if ( true == bRet )
			{
				if ( !rkResult.contUserData.IsEmpty() )
				{
					// contUserData의 사이즈가 있으면 길드로 메일을 전송해야 한다.
					BM::Stream kMailPacket( PT_A_N_REQ_GUILD_MAIL, rkResult.QueryOwner() );
					kMailPacket.Push( static_cast<BYTE>(GMG_Master) );
					kMailPacket.Push( rkResult.contUserData );
					::SendToGuildMgr( kMailPacket );
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Save Failed GuildID<") << rkResult.QueryOwner() << _T(">") );
			}
		}break;
	case DQT_CREATE_EMPORIA_TOURNAMENT:
		{
			if ( !bRet )
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Create Failed Tournament Emporia<") << rkResult.QueryOwner() << _T(">") );
			}
		}break;
	case DQT_SAVE_EMPORIA_TOURNAMENT:
		{
			if ( !bRet )
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Save Failed Tournament Emporia<") << rkResult.QueryOwner() << _T(">") );
			}
		}break;
	case DQT_SAVE_EMPORIA_MERCENARY:
		{
			if ( !bRet )
			{
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Save Failed Mercenary Emporia<") << rkResult.QueryOwner() << _T(">") );
			}
		}break;
	case DQT_SAVE_EMPORIA_FUNCTION:
		{
			if ( 0 < rkResult.contUserData.Size() )
			{	
				BM::GUID kNpcID;
				SEmporiaKey kEmporiaKey;
				short nFuncNo = 0;
				short nWeekCount = 0;
				SEmporiaFunction kFunc;
				BM::GUID kGuildID;
				__int64 i64Exp = 0i64;
				rkResult.contUserData.Pop( kNpcID );
				rkResult.contUserData.Pop( kEmporiaKey );
				rkResult.contUserData.Pop( nFuncNo );
				rkResult.contUserData.Pop( nWeekCount );
				kFunc.ReadFromPacket( rkResult.contUserData );
				rkResult.contUserData.Pop( kGuildID );
				rkResult.contUserData.Pop( i64Exp );

				if ( true == bRet )
				{
					// 성공
					BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
					kAnsPacket.Push( EFUNC_SUCCESS );
					kAnsPacket.Push( true );
					kAnsPacket.Push( nFuncNo );
					kAnsPacket.Push( nWeekCount );
					g_kRealmUserMgr.Locked_SendToUser( rkResult.QueryOwner(), kAnsPacket, false );

					// 성공시에 EmporiaGround로 통보 해주어야 한다.
					bool bPrevHaveFunc = false;
					BM::CAutoMutex kLock( m_kMutex, false );// Lock
					T_EMPORIA_PACK * pkPack = GetEmporiaPack( kEmporiaKey.kID );
					if ( pkPack )
					{
						size_t const iEmporiaIndex = static_cast<size_t>(kEmporiaKey.byGrade-1);

						SEmporiaFunction kTmpFunc;
						bPrevHaveFunc = (S_OK==pkPack->GetFunction(iEmporiaIndex, nFuncNo, kTmpFunc));

						if ( S_OK == pkPack->AddFunction( iEmporiaIndex, nFuncNo, kFunc ) )
						{
							// EmporiaGround로 통보 해주어야 한다.
							// 엠포리아로 정보를 주어야 한다.
							BM::Stream kMPacket( PT_N_M_NFY_EMPORIA_INFO );
							if ( S_OK == pkPack->WriteToPacket_Grade( iEmporiaIndex, kMPacket, false ) )
							{	
								pkPack->Send( iEmporiaIndex, kMPacket );
							}
						}
					}

					BM::Stream kSuccessPacket( PT_N_N_NFY_GUILD_EXP_BROADCAST, kGuildID );
					::SendToGuildMgr( kSuccessPacket );


					BM::DBTIMESTAMP_EX kRentalDate;
					BM::DBTIMESTAMP_EX kExpirationDate;
					CGameTime::SecTime2DBTimeEx( kFunc.i64RentalDate, kRentalDate, CGameTime::DEFAULT );
					CGameTime::SecTime2DBTimeEx( kFunc.i64ExpirationDate, kExpirationDate, CGameTime::DEFAULT );

					PgLogCont kContLog(ELogMain_Contents_Guild,ELogSub_EmBattle,BM::GUID::NullData(),rkResult.QueryOwner());
					kContLog.RealmNo(g_kProcessCfg.RealmNo());
					PgLog kLog( ELOrderMain_Structure, ELOrderSub_Install );
					kLog.Set( 0, kEmporiaKey.kID.str() );
					kLog.Set( 1, kGuildID.str() );
					kLog.Set( 2, static_cast<std::wstring>(kRentalDate) );
					kLog.Set( 3, static_cast<std::wstring>(kExpirationDate) );
					kLog.Set( 0, static_cast<int>(kEmporiaKey.byGrade) );
					kLog.Set( 1, static_cast<int>(nFuncNo) );
					kLog.Set( 2, static_cast<int>(nWeekCount) );
					kLog.Set( 3, static_cast<int>(bPrevHaveFunc) );
					kLog.Set( 0, i64Exp );
					kContLog.Add(kLog);
					kContLog.Commit();
				}
				else
				{
					// 실패시에 길드 경험치를 복구 시켜주어야 한다.

					BM::Stream kFailedPacket( PT_N_N_NFY_REFUND_GUILD_EXP, kGuildID );
					kFailedPacket.Push( i64Exp );
					::SendToGuildMgr( kFailedPacket );

					// 실패
					BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
					kAnsPacket.Push( EFUNC_ERROR );
					kAnsPacket.Push( true );
					kAnsPacket.Push( nFuncNo );
					kAnsPacket.Push( nWeekCount );
					g_kRealmUserMgr.Locked_SendToUser( rkResult.QueryOwner(), kAnsPacket, false );
				}
			}
			else
			{
				// 이경우는 지웠거나

			}
		}break;
	case DQT_UPDATE_EMPORIA_FUNCTION:
		{
			if ( true == bRet )
			{
				SEmporiaKey kEmporiaKey;
				short nFuncNo = 0;
				__int64 i64ExtValue = 0i64;
				rkResult.contUserData.Pop( kEmporiaKey );
				rkResult.contUserData.Pop( nFuncNo );
				rkResult.contUserData.Pop( i64ExtValue );

				BM::CAutoMutex kLock( m_kMutex, false );

				T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaKey.kID );
				if ( pkPack )
				{
					pkPack->UpdateFunctionExtValue( static_cast<size_t>(kEmporiaKey.byGrade-1), nFuncNo, i64ExtValue );
				}
			}
		}break;
	default:
		{

		}break;
	}
}

void PgEmporiaMgr::Locked_ConnectPublicCenter( bool const bConnect )
{
	BM::CAutoMutex kWLock( m_kMutex, false );

	if ( true == bConnect )
	{
	}
	else
	{
		SGroundKey kEmptyGndkey;

		CONT_EMPORIA_PACK::iterator pack_itr = m_kContEmporiaPack.begin();
		for ( ; pack_itr != m_kContEmporiaPack.end() ; ++pack_itr )
		{
			pack_itr->second->RecvDeleteEmporiaGround( kEmptyGndkey );
		}
	}
}

void PgEmporiaMgr::Release( CONT_EMPORIA_PACK &rkContEmporiaPack )
{
	CONT_EMPORIA_PACK::iterator itr = rkContEmporiaPack.begin();
	for ( ; itr!=rkContEmporiaPack.end() ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}
	rkContEmporiaPack.clear();
}

bool PgEmporiaMgr::CloseEmporia( BM::GUID const &kEmporiaID, bool const bDelete )
{
	CONT_EMPORIA_PACK::iterator pack_itr = m_kContEmporiaPack.find( kEmporiaID );
	if ( pack_itr != m_kContEmporiaPack.end() )
	{
		CONT_EMPORIA_PACK::mapped_type pkElement = pack_itr->second;

		size_t iIndex = 0;
		PgEmporia const *pkEmporia = pkElement->GetElement( iIndex );
		while ( pkEmporia )
		{
			// 처리를 해주고...

			pkEmporia = pkElement->GetElement( ++iIndex );
		}

		pkElement->Close();

		if ( true == bDelete )
		{
			SAFE_DELETE( pkElement );
			m_kContEmporiaPack.erase( pack_itr );
		}

		return true;
	}
	return false;
}

bool PgEmporiaMgr::IsEmporiaHaveGuild( BM::GUID const &kGuildID )const
{
	CONT_EMPORIA_FROMGUILD::const_iterator itr = m_kContEmporiaFromGuild.find ( kGuildID );
	return itr != m_kContEmporiaFromGuild.end();
}

bool PgEmporiaMgr::GetEmporiaHaveGuild( BM::GUID const &kGuildID, SEmporiaHaveInfo &rkOutInfo )const
{
	CONT_EMPORIA_FROMGUILD::const_iterator itr = m_kContEmporiaFromGuild.find( kGuildID );
	if ( itr != m_kContEmporiaFromGuild.end() )
	{
		rkOutInfo = itr->second;
		return true;
	}
	return false;
}

PgEmporiaMgr::T_EMPORIA_PACK* PgEmporiaMgr::GetEmporiaPack( BM::GUID const &kID )const
{
	CONT_EMPORIA_PACK::const_iterator pack_itr = m_kContEmporiaPack.find( kID );
	if ( pack_itr != m_kContEmporiaPack.end() )
	{
		return pack_itr->second;
	}
	return NULL;
}

PgEmporiaMgr::T_EMPORIA_PACK* PgEmporiaMgr::GetEmporiaPackByGuild( BM::GUID const &kGuild, size_t &iOutIndex )const
{
	SEmporiaHaveInfo kHaveInfo;
	if ( true == GetEmporiaHaveGuild( kGuild, kHaveInfo ) )
	{
		if ( true == kHaveInfo.bOwner )
		{
			iOutIndex = static_cast<size_t>(kHaveInfo.kEmporiaKey.byGrade-1);
			return GetEmporiaPack( kHaveInfo.kEmporiaKey.kID );
		}
	}
	return NULL;
}

void PgEmporiaMgr::Locked_OnTick(void)
{
	BM::CAutoMutex kRLock( m_kMutex, false );

	__int64 const i64CurTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

	CONT_EMPORIA_PACK::iterator pack_itr = m_kContEmporiaPack.begin();
	for ( ; pack_itr != m_kContEmporiaPack.end() ; ++pack_itr )
	{
		pack_itr->second->Update( i64CurTime );
	}
}

void PgEmporiaMgr::Locked_GetEmporiaInfo( BM::GUID const &kGuildID, SGuildEmporiaInfo& rkEmporiaInfo )const
{
	BM::CAutoMutex kRLock( m_kMutex, false );

	CONT_EMPORIA_FROMGUILD::const_iterator guild_itr = m_kContEmporiaFromGuild.find( kGuildID );
	if ( guild_itr != m_kContEmporiaFromGuild.end() )
	{
		T_EMPORIA_PACK *pkPack = GetEmporiaPack( guild_itr->second.kEmporiaKey.kID );
		if ( pkPack )
		{
			if ( E_FAIL == pkPack->GetEmporiaInfo( kGuildID, rkEmporiaInfo ) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("Critical Error!!! GuildID<") << kGuildID << _T("> EmporiaID<") << guild_itr->second.kEmporiaKey.kID << _T(">") );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!! GuildID<") << kGuildID << _T("> EmporiaID<") << guild_itr->second.kEmporiaKey.kID << _T("> Not Found Emporia") );
		}
	}
}

BYTE PgEmporiaMgr::Locked_GetEmporiaKey( SEmporiaKey const &kEmporiaKey, BM::GUID const &kGuildGuid, SGroundKey &kOutGndKey )const
{
	BM::CAutoMutex kRLock( m_kMutex, false );

	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaKey.GetKey() );
	if ( pkPack )
	{
		return pkPack->GetEmporiaKey( static_cast<size_t>(kEmporiaKey.byGrade-1), kGuildGuid, kOutGndKey );
	}
	return MMET_Failed_Access;
}

BYTE PgEmporiaMgr::Locked_GetEmporiaKeyFromMercenary( BM::GUID const &kEmporiaID, BM::GUID const &kGuildGuid, BM::GUID const &kCharGuid, PgLimitClass const &kClassInfo, size_t const iJoinIndex, bool const bIsAttack, SGroundKey &kOutGndKey )const
{
	BM::CAutoMutex kRLock( m_kMutex, false );

	CONT_EMPORIA_FROMGUILD::const_iterator guild_itr = m_kContEmporiaFromGuild.find( kGuildGuid );
	if ( guild_itr != m_kContEmporiaFromGuild.end() )
	{
		if ( guild_itr->second.kEmporiaKey.GetKey() == kEmporiaID )
		{// 용병으로 참가 불가
			return MMET_Failed_Access;
		}
	}

	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		return pkPack->GetEmporiaKeyFromMercenary( kGuildGuid, kCharGuid, kClassInfo, iJoinIndex, bIsAttack, kOutGndKey );
	}
	return MMET_Failed_Access;
}

bool PgEmporiaMgr::ProcessMsg( SEventMessage *pkMsg )
{
	BM::Stream::DEF_STREAM_TYPE kType = 0;
	pkMsg->Pop( kType );

	switch ( kType )
	{
	case PT_A_N_REQ_EMPORIA_GM_COMMAND:
		{
			EGMCmdType eCommandType = GMCMD_NONE;
			BM::GUID kOrderGuid;
			pkMsg->Pop(eCommandType);
			pkMsg->Pop(kOrderGuid);

			bool const bRet = Locked_RecvGMCommand( eCommandType, pkMsg );
			if ( BM::GUID::IsNotNull(kOrderGuid) )
			{
				if(bRet)
				{
					g_kGMProcessMgr.SendOrderState( kOrderGuid, OS_DONE);
				}
				else
				{
					g_kGMProcessMgr.SendOrderFailed(kOrderGuid, GE_SYSTEM_ERR);
				}
			}
		}break;
	case PT_M_N_REQ_MAP_MOVE_CHECK:
		{
			BM::GUID kCharGuid;
			SReqMapMove_MT kRMM;
			bool bIsJoinMercenary = false;
			BM::GUID kGuildGuid;
			int iClass = 0;
			short nLevel = 0;
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kRMM);
			pkMsg->Pop(kGuildGuid);
			pkMsg->Pop(bIsJoinMercenary);
			
			if ( true == bIsJoinMercenary )
			{
				PgLimitClass kClassInfo;
				BM::GUID kEmporiaID;
				size_t iIndex = 0;
				bool bIsAttack;
				kClassInfo.ReadFromPacket( *pkMsg );
				pkMsg->Pop( kEmporiaID );
				pkMsg->Pop( iIndex );
				pkMsg->Pop( bIsAttack );
				kRMM.cType = Locked_GetEmporiaKeyFromMercenary( kEmporiaID, kGuildGuid, kCharGuid, kClassInfo, iIndex, bIsAttack, kRMM.kTargetKey );
				kRMM.iCustomValue = ( bIsAttack ? TEAM_RED : TEAM_BLUE );
			}
			else
			{
				SEmporiaKey kEmporiaKey;
				pkMsg->Pop(kEmporiaKey);
				kRMM.cType = Locked_GetEmporiaKey( kEmporiaKey, kGuildGuid, kRMM.kTargetKey );
			}

			if ( MMET_None == kRMM.cType )
			{
				kRMM.cType = MMET_GoToPublicGround;
				BM::Stream kSucceededPacket( PT_N_M_ANS_MAP_MOVE_CHECK, kCharGuid );
				kSucceededPacket.Push( kRMM );
				::SendToGround( kRMM.kCasterSI.nChannel, kRMM.kCasterKey, kSucceededPacket, true );
			}
			else
			{
				BM::Stream kFailedPacket( PT_N_C_ANS_MAP_MOVE_CHECK_FAILED, kRMM.cType );
				g_kRealmUserMgr.Locked_SendToUser( kCharGuid, kFailedPacket, false );
			}
		}break;
	case PT_C_N_REQ_EMPORIA_STATUS_LIST:
		{
			BM::GUID kCharGuid;
			BM::GUID kEmporiaID;
			EEmporiaStatusListType eType = ESLT_NONE;
			pkMsg->Pop( kCharGuid );
			pkMsg->Pop( kEmporiaID );
			pkMsg->Pop( eType );

			BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_STATUS_LIST );
			kAnsPacket.Push( eType );

			{
				BM::CAutoMutex kRLock( m_kMutex, false );

				T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
				if ( pkPack )
				{
					pkPack->WriteToPacket( kAnsPacket );
				}
			}

			g_kRealmUserMgr.Locked_SendToUser( kCharGuid, kAnsPacket, false );

		}break;
	case PT_N_N_REQ_EMPORIA_BATTLE_RESERVE:
		{
			Locked_Recv_PT_N_N_REQ_EMPORIA_BATTLE_RESERVE( pkMsg );
		}break;
	case PT_T_N_ANS_CREATE_PUBLICMAP:
		{
			Locked_Recv_PT_T_N_ANS_CREATE_PUBLICMAP( pkMsg );
		}break;
	case PT_T_N_ANS_DELETE_PUBLICMAP:
		{
			Locked_Recv_PT_T_N_ANS_DELETE_PUBLICMAP( pkMsg );
		}break;
	case PT_M_N_NFY_EMPORIA_BATTLE_RESULT:
		{
			Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_RESULT( pkMsg );
		}break;
	case PT_N_N_NFY_GUILD_REMOVE:
		{
			Locked_Recv_PT_N_N_NFY_GUILD_REMOVE( pkMsg );
		}break;
	case PT_M_N_NFY_EMPORIA_BATTLE_USERCOUNT_FULL:
		{
			Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_USERCOUNT_FULL( pkMsg );
		}break;
	case PT_M_N_REQ_EMPORIA_PORTAL_INFO:
		{
			Locked_Recv_PT_M_N_REQ_EMPORIA_PORTAL_INFO( pkMsg );
		}break;
	case PT_M_N_REQ_GET_EMPORIA_ADMINISTRATOR:
		{
			Locked_Recv_Administrator( pkMsg, false );
		}break;
	case PT_M_N_REQ_EMPORIA_ADMINISTRATION:
		{
			Locked_Recv_Administrator( pkMsg, true );
		}break;
	case PT_N_N_ANS_GET_SETTING_MERCENARY:
		{
			Locked_Recv_PT_N_N_ANS_GET_SETTING_MERCENARY( pkMsg );
		}break;
	case PT_M_N_NFY_GAME_READY:
		{
			Locked_Recv_PT_M_N_NFY_GAME_READY( pkMsg );
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Unknown Type : ") << kType );
			return false;
		}break;
	}
	
	return true;
}

bool PgEmporiaMgr::Locked_RecvGMCommand( EGMCmdType const eCommandType, BM::Stream * const pkPacket )
{
	BM::GUID kEmporiaID;
	pkPacket->Pop( kEmporiaID );

	BM::CAutoMutex kWLock( m_kMutex, true );

	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		INFO_LOG( BM::LOG_LV6, _T("[RecvEmporiaGMCommand] Type<") << (int)eCommandType << _T("> EmporiaID<") << kEmporiaID << _T(">") );
	}
	else
	{
		INFO_LOG( BM::LOG_LV5, _T("[RecvEmporiaGMCommand] Type<") << (int)eCommandType << _T("> Not Found EmporiaID<") << kEmporiaID << _T(">") ); 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bResult = false;
	__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

	switch( eCommandType )
	{
	case GMCMD_EMPORIA_OPEN:
		{
			BM::DBTIMESTAMP_EX kNextBattleTime;
			int iBattleWeek = 0;
			int iBattleTime = 0;
			int iOpenLevel = 0;
			pkPacket->Pop( kNextBattleTime );
			pkPacket->Pop( iBattleWeek );
			pkPacket->Pop( iBattleTime );
			pkPacket->Pop( iOpenLevel );
			bResult = SUCCEEDED( pkPack->Open( kNextBattleTime, static_cast<char const>(iBattleWeek), static_cast<size_t>(iOpenLevel) ) );
		}break;
	case GMCMD_EMPORIA_CLOSE:
		{
			if ( S_OK == pkPack->Close() )
			{
				SGuildEmporiaInfo kEmporiaInfo;

				CONT_EMPORIA_FROMGUILD::iterator guild_itr = m_kContEmporiaFromGuild.begin();
				while ( guild_itr!=m_kContEmporiaFromGuild.end() )
				{
					if ( true == guild_itr->second.bOwner )
					{
						if ( kEmporiaID == guild_itr->second.kEmporiaKey.GetKey() )
						{
							CAUTION_LOG( BM::LOG_LV6, _T("[RecvEmporiaGMCommand] Emporia<") << kEmporiaID << _T("> Close OwnerClear GuildID<") << guild_itr->second.kGuildInfo.kGuildID << _T("> EmporiaGrade<") << guild_itr->second.kEmporiaKey.byGrade << _T(">") );
							
							BM::Stream kGPacket( PT_N_N_NFY_EMPORIA_INFO, guild_itr->second.kGuildInfo.kGuildID );
							kGPacket.Push( kEmporiaInfo );
							::SendToGuildMgr( kGPacket );

							guild_itr = m_kContEmporiaFromGuild.erase( guild_itr );
							continue;
						}
					}
					++guild_itr;
				}

				bResult = true;
			}
		}break;
	case GMCMD_EMPORIA_OWNER_CHANGE:
		{
			SEmporiaGuildInfo kNewOwnerGuildInfo;
			int iEmblem = 0;
			int iGrade = 0;
			pkPacket->Pop( kNewOwnerGuildInfo.kGuildID );
			pkPacket->Pop( kNewOwnerGuildInfo.wstrName );
			pkPacket->Pop( iEmblem );
			pkPacket->Pop( iGrade );
			kNewOwnerGuildInfo.nEmblem = static_cast<BYTE>(iEmblem);

			CONT_EMPORIA_FROMGUILD::iterator guild_itr = m_kContEmporiaFromGuild.find( kNewOwnerGuildInfo.kGuildID );
			if ( guild_itr != m_kContEmporiaFromGuild.end() )
			{
				INFO_LOG( BM::LOG_LV5, _T("This Guild<") << kNewOwnerGuildInfo.kGuildID << _T("/") << kNewOwnerGuildInfo.wstrName << _T("> Have(or Reserve) Emporia<") << guild_itr->second.kEmporiaKey.GetKey() << _T(">") );
			}
			else
			{
				if ( S_OK == pkPack->SetChangeEmporiaOwner( static_cast<size_t>(iGrade-1), kNewOwnerGuildInfo, true, false ) )
				{
					if ( !kNewOwnerGuildInfo.IsEmpty() )
					{
						SEmporiaHaveInfo kEmporiaHaveInfo( SEmporiaKey( kEmporiaID, static_cast<BYTE>(iGrade) ), kNewOwnerGuildInfo );
						m_kContEmporiaFromGuild.insert( std::make_pair(kNewOwnerGuildInfo.kGuildID, kEmporiaHaveInfo) );
					}
					bResult = true;
				}
			}
		}break;
	case GMCMD_EMPORIA_BATTLE_START:
		{
			__int64 i64BattleTime = i64NowTime;
			if ( pkPacket->RemainSize() >= sizeof(BM::DBTIMESTAMP_EX) )
			{
				BM::DBTIMESTAMP_EX kBattleTime;
				pkPacket->Pop( kBattleTime );
				CGameTime::DBTimeEx2SecTime( kBattleTime, i64BattleTime, CGameTime::DEFAULT );
			}

			bResult = SUCCEEDED( pkPack->BattleStart( i64BattleTime ) );
		}break;
	case GMCMD_EMPORIA_BATTLE_TIMECHANGE:
		{
			__int64 i64BattleTime = i64NowTime;
			if ( pkPacket->RemainSize() >= sizeof(BM::DBTIMESTAMP_EX) )
			{
				BM::DBTIMESTAMP_EX kBattleTime;
				pkPacket->Pop( kBattleTime );
				CGameTime::DBTimeEx2SecTime( kBattleTime, i64BattleTime, CGameTime::DEFAULT );
			}
		}break;
	}

	if ( !bResult )
	{
		INFO_LOG( BM::LOG_LV5, _T("[RecvEmporiaGMCommand] Type<") << (int)eCommandType << _T("> EmporiaID<") << kEmporiaID << _T("> Command Failed") ); 
	}

	return bResult;
}

static void ProcessFailEmporiaReserve(BM::GUID const & kReqCharGuid, BM::GUID const & kGuildID, __int64 const i64Cost)
{
    SGuild_Inventory_Log kLog( GetGuildInvLog(kGuildID, EGIT_EMPORIA_THROW, i64Cost) );
    
    BM::Stream kPacket;
    kLog.WriteToPacket(kPacket);

    SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
    pkActionOrder->InsertTarget(kReqCharGuid);
    pkActionOrder->kCause = CIE_EmporiaReserve_Fail;
    pkActionOrder->kContOrder.push_back( SPMO(IMET_ADD_MONEY|IMC_GUILD_INV,SModifyOrderOwner(kGuildID,OOT_Guild),SPMOD_Add_Money(i64Cost)) );
    pkActionOrder->kAddonPacket.Push(kPacket.Data());
    g_kJobDispatcher.VPush(pkActionOrder);
}

void PgEmporiaMgr::Locked_Recv_PT_N_N_REQ_EMPORIA_BATTLE_RESERVE( BM::Stream * const pkPacket )
{
	bool bThrow = false;
    BM::GUID kReqCharGuid;
	BM::GUID kEmporiaID;
	SEmporiaChallenge kChallenge;
	pkPacket->Pop( bThrow );
    pkPacket->Pop( kReqCharGuid );
	if ( !bThrow )
	{
		pkPacket->Pop( kEmporiaID );
	}
	kChallenge.ReadFromPacket( *pkPacket );

	if ( BM::GUID::IsNull(kChallenge.kGuildInfo.kGuildID) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Cirtical Error!!") );
		return;
	}

	// 중복방지를 하기 위해서 다음과 같이 해야 한다.

	// 1. 엠포리아 전쟁을 신청한 경우는(WriteLock)
	// AddChallenge -> m_kContEmporiaFromGuild.insert -> DB저장 -> 실패시 m_kContEmporiaByGuild.erase

	// 2. 엠포리아 전쟁을 포기한 경우는(ReadLock)
	// RemoveChallenge -> DB저장 -> 성공시 m_kContEmporiaFromGuild.erase

	{//
		BM::CAutoMutex kLock( m_kMutex, !bThrow );

		CONT_EMPORIA_FROMGUILD::iterator itr = m_kContEmporiaFromGuild.find( kChallenge.kGuildInfo.kGuildID );
		if ( itr != m_kContEmporiaFromGuild.end() )
		{
			CONT_EMPORIA_FROMGUILD::mapped_type &kElement = itr->second;
			if ( true == bThrow )
			{
				if ( !kElement.bOwner )
				{
					kEmporiaID = kElement.kEmporiaKey.kID;
					T_EMPORIA_PACK* pkPack = GetEmporiaPack( kEmporiaID );
					if ( pkPack )
					{
						if ( S_OK == pkPack->RemoveChallenge( kChallenge.kGuildInfo.kGuildID, &kChallenge ) )
						{
							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_THROW, _T("EXEC [dbo].[up_SaveEmporia_BattleRefundCost]") );
							kQuery.InsertQueryTarget( kChallenge.kGuildInfo.kGuildID );
							kQuery.QueryOwner( kChallenge.kGuildInfo.kGuildID );

							kQuery.PushStrParam( pkPack->GetBattleID() );
							kQuery.PushStrParam( kChallenge.kGuildInfo.kGuildID );

                            kQuery.contUserData.Push( kReqCharGuid );
							kQuery.contUserData.Push( kChallenge.kGuildInfo.kGuildID );
							kQuery.contUserData.Push( kEmporiaID );
							kQuery.contUserData.Push( static_cast<BYTE>(GCR_Success) );
                            kQuery.contUserData.Push( static_cast<__int64>(0i64) );

							g_kCoreCenter.PushQuery( kQuery );
						}
                        else
                        {
                            CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("RemoveChallenge Fail = GuildGuid<") << kChallenge.kGuildInfo.kGuildID << _T(">") );
                        }
					}
				}
				else
				{
				}
			}
			else
			{
				SGuildEmporiaInfo kEmporiaInfo( EMPORIA_KEY_BATTLERESERVE );
				kEmporiaInfo.kID = kEmporiaID;

				BM::Stream kFailedPacket( PT_N_N_ANS_EMPORIA_BATTLE_RESERVE, kChallenge.kGuildInfo.kGuildID );
                kFailedPacket.Push( kReqCharGuid );
				kFailedPacket.Push( kEmporiaInfo );
				kFailedPacket.Push( kChallenge.i64Cost );
				kFailedPacket.Push( false );
				kFailedPacket.Push( static_cast<BYTE>(GCR_None) );
				::SendToGuildMgr( kFailedPacket );

				// 중복 도전은 불가능!!!
				g_kRealmUserMgr.Locked_SendWarnMessage( kReqCharGuid, 71031, EL_Warning, false );
			}
		}
		else
		{
			if ( true == bThrow )
			{
				// 도전한게 없는데 어떻게 포기를 하냐.
			}
			else
			{
				T_EMPORIA_PACK* pkPack = GetEmporiaPack( kEmporiaID );
                HRESULT const hRt = pkPack ? pkPack->AddChallenge( &kChallenge ) : E_FAIL;
                
                if(S_OK==hRt && pkPack)
                {
                    m_kContEmporiaFromGuild.insert( std::make_pair( kChallenge.kGuildInfo.kGuildID, SEmporiaHaveInfo(kChallenge.kGuildInfo, kEmporiaID)) );

					BM::DBTIMESTAMP_EX kRegistDate;
					CGameTime::SecTime2DBTimeEx( kChallenge.i64ChallengeTime, kRegistDate, CGameTime::DEFAULT );

					// 세이브를 해야지
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_RESERVE, _T("EXEC [dbo].[up_SaveEmporia_ChallengeBattle]") );
					kQuery.InsertQueryTarget( kChallenge.kGuildInfo.kGuildID );
					kQuery.QueryOwner( kChallenge.kGuildInfo.kGuildID );

					kQuery.PushStrParam( kEmporiaID );
					kQuery.PushStrParam( pkPack->GetBattleID() );
					kQuery.PushStrParam( kChallenge.kGuildInfo.kGuildID );
					kQuery.PushStrParam( kChallenge.i64Cost );
					kQuery.PushStrParam( kRegistDate );	// 5
					kQuery.PushStrParam( static_cast<BYTE>(EMBATTLE_NONE) );

                    kQuery.contUserData.Push( kReqCharGuid );
					kQuery.contUserData.Push( kChallenge.kGuildInfo.kGuildID );
					kQuery.contUserData.Push( kEmporiaID );
					kQuery.contUserData.Push( static_cast<BYTE>(GCR_Success) );
					kQuery.contUserData.Push( pkPack->GetBattleTime() );

					g_kCoreCenter.PushQuery( kQuery );
                }
                else
                {
                    ProcessFailEmporiaReserve(kReqCharGuid, kChallenge.kGuildInfo.kGuildID, kChallenge.i64Cost);
                }
			}
		}
	}//
}

bool PgEmporiaMgr::Locked_Recv_PT_T_N_ANS_CREATE_PUBLICMAP( BM::Stream * const pkPacket )
{
	SGroundMakeOrder kOrder;
	HRESULT hRet = E_FAIL;
	T_GNDATTR kGndAttr = GATTR_DEFAULT;
	BM::GUID kEmporiaID;
	size_t iIndex = 0;
	pkPacket->Pop( hRet );
	kOrder.ReadFromPacket(*pkPacket);
	pkPacket->Pop( kGndAttr );
	pkPacket->Pop( kEmporiaID );
	pkPacket->Pop( iIndex );

	BM::CAutoMutex kLock( m_kMutex, false );

	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		switch ( kGndAttr )
		{
		case GATTR_EMPORIABATTLE:
			{
				pkPack->RecvCreateBattle( kOrder.kKey, SUCCEEDED(hRet) );
				pkPack->CheckCreateBattleCompleted();
			}break;
		case GATTR_EMPORIA:
			{
				pkPack->RecvCreateEmporiaGround( iIndex, kOrder.kKey, SUCCEEDED(hRet) );
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("Unknown Attr<") << kGndAttr << _T("> EmporiaID<") << kEmporiaID << _T(">") );
			}break;
		}
		
		return true;
	}

	CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Not Found EmporiaID = ") << kEmporiaID );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgEmporiaMgr::Locked_Recv_PT_T_N_ANS_DELETE_PUBLICMAP( BM::Stream * const pkPacket )
{
	SGroundKey kGndkey;
	pkPacket->Pop( kGndkey );

	BM::CAutoMutex kWLock( m_kMutex, true );
	// WriteLock으로 해주자... 순서꼬임 방지?

	CONT_EMPORIA_PACK::iterator pack_itr = m_kContEmporiaPack.begin();
	for ( ; pack_itr != m_kContEmporiaPack.end() ; ++pack_itr )
	{
		if ( S_OK == pack_itr->second->RecvDeleteEmporiaGround( kGndkey ) )
		{
			break;
		}
	}
}

bool PgEmporiaMgr::Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_RESULT( BM::Stream * const pkPacket )
{
	BM::GUID kEmporiaID;
	int iWinTeam = TEAM_NONE;
	SGroundKey kBattleGndKey;
	CONT_EM_RESULT_USER kWinMember;
	CONT_EM_RESULT_USER kLoseMember;
    EEmporiaResultNoticeType eResultType = ERNT_NONE;
	pkPacket->Pop(kEmporiaID);
	pkPacket->Pop(iWinTeam);
	pkPacket->Pop(kBattleGndKey);
	pkPacket->Pop(kWinMember);
	pkPacket->Pop(kLoseMember);
    pkPacket->Pop(eResultType);

	BM::CAutoMutex kLock( m_kMutex, true );

	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		BM::GUID kEraseGuildID;
		pkPack->RecvBattleResult( kBattleGndKey, TEAM_ATTACKER == iWinTeam, kEraseGuildID, kWinMember, kLoseMember, eResultType );

		m_kContEmporiaFromGuild.erase( kEraseGuildID );
		return true;
	}
	
	CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found kEmporiaID<" << kEmporiaID << L">" );
	return false;
}

void PgEmporiaMgr::Locked_Recv_PT_N_N_NFY_GUILD_REMOVE( BM::Stream * const pkPacket )
{
	BM::GUID kGuildID;
	pkPacket->Pop( kGuildID );

	BM::CAutoMutex kLock( m_kMutex, true );

	CONT_EMPORIA_FROMGUILD::iterator guild_itr = m_kContEmporiaFromGuild.find( kGuildID );
	if ( guild_itr != m_kContEmporiaFromGuild.end() )
	{
		T_EMPORIA_PACK *pkPack = GetEmporiaPack( guild_itr->second.kEmporiaKey.kID );
		if ( pkPack )
		{

		}
		m_kContEmporiaFromGuild.erase( guild_itr );
	}
}

void PgEmporiaMgr::Locked_Recv_PT_M_N_NFY_EMPORIA_BATTLE_USERCOUNT_FULL( BM::Stream * const pkPacket )
{
	BM::GUID kEmporiaID;
	SGroundKey kBattleGroundKey;
	int iTargetTeam = TEAM_NONE;
	bool bLock = false;
	pkPacket->Pop( kEmporiaID );
	pkPacket->Pop( kBattleGroundKey );
	pkPacket->Pop( iTargetTeam );
	pkPacket->Pop( bLock );

	BM::CAutoMutex kLock( m_kMutex, false );
	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		if ( E_FAIL == pkPack->SetEmporiaBattleLock( kBattleGroundKey, TEAM_ATTACKER == iTargetTeam, bLock ) )
		{
			CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found BattleGround<" << kBattleGroundKey.GroundNo() << L"/" << kBattleGroundKey.Guid() << L"> EmporiaID<" << kEmporiaID << L">" );
		}
	}
}

void PgEmporiaMgr::Locked_Recv_PT_M_N_REQ_EMPORIA_PORTAL_INFO( BM::Stream * const pkPacket )
{
	short nChannelNo = 0;
	SGroundKey kGndKey;
	SET_GUID kEmporiaIDList;
	pkPacket->Pop( nChannelNo );
	kGndKey.ReadFromPacket( *pkPacket );
	PU::TLoadArray_A( *pkPacket, kEmporiaIDList );

	BM::CAutoMutex kLock( m_kMutex, false );

	BM::Stream kAnsPacket( PT_N_M_NFY_EMPORIA_PORTAL_INFO );
	size_t const iWrPos = kAnsPacket.WrPos();
	size_t const iWriteSize = kEmporiaIDList.size() * MAX_EMPORIA_GRADE;
	kAnsPacket.Push( iWriteSize );
	
	size_t iSuccessSize = 0;
	SET_GUID::const_iterator itr = kEmporiaIDList.begin();
	for ( ; itr != kEmporiaIDList.end() ; ++itr )
	{
		T_EMPORIA_PACK *pkPack = GetEmporiaPack( *itr );
		if ( pkPack )
		{
			if ( S_OK == pkPack->WriteToPacket_Grade( MAX_EMPORIA_GRADE, kAnsPacket, true ) )
			{
				iSuccessSize += MAX_EMPORIA_GRADE;
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found Emporia<" << *itr << L">" );
		}
	}

	if ( iWriteSize != iSuccessSize )
	{
		kAnsPacket.ModifyData( iWrPos, &iSuccessSize, sizeof(iSuccessSize) );
	}

	::SendToGround( nChannelNo, kGndKey, kAnsPacket, true );
}

void PgEmporiaMgr::Locked_Recv_Administrator( BM::Stream * const pkPacket, bool const bAdmin )
{
	BM::GUID kGuildID;
	BM::GUID kCharID;
	BM::GUID kNpcID;
	pkPacket->Pop( kGuildID );
	pkPacket->Pop( kCharID );
	pkPacket->Pop( kNpcID );
	
	if ( true == bAdmin )
	{
		bool bControlFunc = false;
		pkPacket->Pop( bControlFunc );

		{
			BM::CAutoMutex kLock( m_kMutex, false );
			size_t iIndex = 0;
			T_EMPORIA_PACK *pkPack = GetEmporiaPackByGuild( kGuildID, iIndex );
			if ( pkPack )
			{
				if ( true == bControlFunc )
				{
					short nFuncNo = 0;
					short nWeekCount = 0;
					SEmporiaKey kEmporiaKey;
					__int64 i64Exp = 0i64;
					pkPacket->Pop( nFuncNo );
					pkPacket->Pop( nWeekCount );
					pkPacket->Pop( kEmporiaKey );
					pkPacket->Pop( i64Exp );

					if (	kEmporiaKey.kID == pkPack->GetID() 
						&&	(kEmporiaKey.byGrade-1) == static_cast<BYTE>(iIndex) )
					{
						SEmporiaFunction kFunc;
						pkPack->GetFunction( iIndex, nFuncNo, kFunc );
						kFunc.AddTime( static_cast<__int64>(nWeekCount) );

						// 경험치를 납부하고 기능을 설치
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_EMPORIA_FUNCTION, _T("EXEC [dbo].[up_SaveEmporiaFunction]") );
						kQuery.InsertQueryTarget( kEmporiaKey.kID );
						kQuery.QueryOwner( kCharID );

						kQuery.PushStrParam( kEmporiaKey.kID );
						kQuery.PushStrParam( kEmporiaKey.byGrade );
						kQuery.PushStrParam( nFuncNo );

						BM::DBTIMESTAMP_EX kDBTime;
						CGameTime::SecTime2DBTimeEx( kFunc.i64RentalDate, kDBTime, CGameTime::DEFAULT );
						kQuery.PushStrParam( kDBTime );
						CGameTime::SecTime2DBTimeEx( kFunc.i64ExpirationDate, kDBTime, CGameTime::DEFAULT );
						kQuery.PushStrParam( kDBTime );
						kQuery.PushStrParam( kFunc.i64ExtValue );

						kQuery.PushStrParam( kGuildID );
						kQuery.PushStrParam( i64Exp );

						kQuery.contUserData.Push( kNpcID );
						kQuery.contUserData.Push( kEmporiaKey );
						kQuery.contUserData.Push( nFuncNo );
						kQuery.contUserData.Push( nWeekCount );
						kFunc.WriteToPacket( kQuery.contUserData );
						kQuery.contUserData.Push( kGuildID );
						kQuery.contUserData.Push( i64Exp );

						g_kCoreCenter.PushQuery( kQuery );
					}
				}
				else
				{
					BYTE byGateState = EMPORIA_GATE_CLOSE;
					pkPacket->Pop( byGateState );

					if ( EMPORIA_GATE_CLOSE < byGateState )
					{
						pkPack->SetGate( iIndex, byGateState, true );

						BM::Stream kAnsPacket( PT_N_C_ANS_EMPORIA_ADMINISTRATION, kNpcID );
						kAnsPacket.Push( EFUNC_SUCCESS );
						kAnsPacket.Push( bControlFunc );// false
						kAnsPacket.Push( byGateState );
						g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
					}
				}
			}
		}
	}
	else
	{
		bool bIsOwner = false;
		pkPacket->Pop( bIsOwner );

		BM::Stream kAnsPacket( PT_N_C_ANS_GET_EMPORIA_ADMINISTRATOR, kNpcID );
		bool bRet = false;

		{
			BM::CAutoMutex kLock( m_kMutex, false );

			size_t iIndex = 0;
			T_EMPORIA_PACK *pkPack = GetEmporiaPackByGuild( kGuildID, iIndex );
			if ( pkPack )
			{
				PgEmporia const *pkEmporia = pkPack->GetElement( iIndex );
				if ( pkEmporia )
				{
					bRet = true;
					kAnsPacket.Push( bRet );
					kAnsPacket.Push( pkEmporia->GetKey() );
					kAnsPacket.Push( pkEmporia->GetGate() );
					kAnsPacket.Push( bIsOwner );
					if ( true == bIsOwner )
					{
						pkEmporia->WriteToPacket_Function( kAnsPacket );
					}
				}
			}
		}

		if ( !bRet )
		{
			kAnsPacket.Push( bIsOwner );
		}

		g_kRealmUserMgr.Locked_SendToUser( kCharID, kAnsPacket, false );
	}
}

void PgEmporiaMgr::Locked_Recv_PT_N_N_ANS_GET_SETTING_MERCENARY( BM::Stream * const pkPacket )
{
	BM::GUID kEmporiaID;
	pkPacket->Pop( kEmporiaID );

	BM::CAutoMutex kLock( m_kMutex, false );
	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		pkPack->RecvSettingMercenary( pkPacket );
	}
}

void PgEmporiaMgr::Locked_Recv_PT_M_N_NFY_GAME_READY( BM::Stream * const pkPacket )
{
	BM::GUID kEmporiaID;
	SGroundKey kBattleGroundKey;
	pkPacket->Pop( kEmporiaID );
	pkPacket->Pop( kBattleGroundKey );

	BM::CAutoMutex kLock( m_kMutex, false );
	T_EMPORIA_PACK *pkPack = GetEmporiaPack( kEmporiaID );
	if ( pkPack )
	{
		if ( E_FAIL == pkPack->SetEmporiaBattleReady( kBattleGroundKey ) )
		{
			CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found BattleGround<" << kBattleGroundKey.GroundNo() << L"/" << kBattleGroundKey.Guid() << L"> EmporiaID<" << kEmporiaID << L">" );
		}
	}
}