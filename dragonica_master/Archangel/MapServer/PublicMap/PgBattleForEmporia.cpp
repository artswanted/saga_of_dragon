#include "stdafx.h"
#include "DataPack/Packinfo.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/Global.h"
#include "Variant/PgEventView.h"
#include "Variant/PgEmporia.h"
#include "PgRequest.h"
#include "PgBattleForEmporia.h"
#include "PgWarGround.h"

PgBattleForEmporia::PgBattleForEmporia()
:	m_dwLastReBirthTime(0)
,	m_bIsRevive(false)
,	m_iUserCount_MinInTeam(0)
,	m_iUserCount_MaxInTeam(0)
,	m_iUserCount_AttackTeam(0)
,	m_iUserCount_DefenceTeam(0)
,	m_i64ReadyTime(0i64)
,   m_eResultType(ERNT_NONE)
,	m_iBattleIndex(0)
{

}

HRESULT PgBattleForEmporia::Init( BM::Stream &kPacket, PgWarGround *pkGnd )
{
	m_i64ReadyTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );

	if ( SUCCEEDED(PgWarMode::Init( kPacket, pkGnd ) ) )
	{
		m_kContOutUser.clear();
		m_kContUser.clear();
		BM::GUID kDefencdGuildID;
		std::wstring wstrDefenceGuildName;
		BM::GUID kAttackGuildID;
		std::wstring wstrAttackGuildName;
		__int64 i64StartBattleTime = 0i64;
		short nBattleTimeMin = 0;

 		kPacket.Pop( m_kEmporiaID );// 엠포리아키
		kPacket.Pop( m_kBattleID );// 엠포리아키
		kPacket.Pop( m_iBattleIndex );// 엠포리아키
		
		kPacket.Pop( kDefencdGuildID );
		kPacket.Pop( wstrDefenceGuildName );
		kPacket.Pop( kAttackGuildID );
		kPacket.Pop( wstrAttackGuildName );
		kPacket.Pop( m_i64NextEventTime );
		kPacket.Pop( nBattleTimeMin );
 
 		m_kDefenceGuildInfo.Set( kDefencdGuildID, wstrDefenceGuildName );
 		m_kAttackGuildInfo.Set( kAttackGuildID, wstrAttackGuildName ); 

		m_dwGamePlayTime = nBattleTimeMin * 60000;
		m_dwLastReBirthTime = 0;
		m_bIsRevive = false;
		
		{		
			if ( FAILED( g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_Battle_UserCount_MinInTeam, m_iUserCount_MinInTeam ) ) )
			{
				m_iUserCount_MinInTeam = 1;// 기본은 1명
			}

			if ( FAILED( g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_Battle_UserCount_MaxInTeam, m_iUserCount_MaxInTeam ) ) )
			{
				m_iUserCount_MaxInTeam = 25;	// 기본은 25명
			}

			CONT_DEF_EMPORIA const *pkDefGuildEmporia = NULL;
			g_kTblDataMgr.GetContDef( pkDefGuildEmporia );

			if ( pkDefGuildEmporia )
			{

				CONT_DEF_EMPORIA::const_iterator def_itr = pkDefGuildEmporia->find( GetEmporiaGUID() );
				if ( def_itr != pkDefGuildEmporia->end() )
				{
//					m_iUserCount_MaxInTeam = static_cast<size_t>(def_itr->second.nBattleUserCountInTeam);
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Not Found EmporiaKey<") << GetEmporiaGUID() << _T("> in CONT_DEF_GUILD_EMPORIA") );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_GUILD_EMPORIA is NULL") );
			}
		}
		
		return InitBattleArea(pkGnd);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

// bool PgBattleForEmporia::IsReadyUser( PgPlayer *pkPlayer )const
// {
// 	if ( pkPlayer )
// 	{
// 		return TEAM_NONE != GetTeam( pkPlayer );
// 	}
// 	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 	return false;
// }

void PgBattleForEmporia::AddUserCount( int const iTeam, bool const bAdd )
{
	bool bSendFull = false;
	switch ( iTeam )
	{
	case TEAM_ATTACKER:
		{
			if ( bAdd )
			{
				bSendFull =- ( (++m_iUserCount_AttackTeam) >= m_iUserCount_MaxInTeam );
			}
			else
			{
				bSendFull = ( (m_iUserCount_AttackTeam--) >= m_iUserCount_MaxInTeam );
			}
		}break;
	case TEAM_DEFENCER:
		{
			if ( bAdd )
			{
				bSendFull =- ( (++m_iUserCount_DefenceTeam) >= m_iUserCount_MaxInTeam );
			}
			else
			{
				bSendFull = ( (m_iUserCount_DefenceTeam--) >= m_iUserCount_MaxInTeam );
			}
		}break;
	default:
		{
		}break;
	}

	if ( bSendFull )
	{
		BM::Stream kPacket( PT_M_N_NFY_EMPORIA_BATTLE_USERCOUNT_FULL, m_kEmporiaID );
		kPacket.Push( GndKey() );
		kPacket.Push( iTeam );
		kPacket.Push( bAdd );
		Send( kPacket );
	}
}

bool PgBattleForEmporia::IsJoin( PgPlayer *pkPlayer )
{
	if ( pkPlayer )
	{
		int iTeam = GetTeam( pkPlayer );
		if ( TEAM_NONE == iTeam )
		{
			iTeam = pkPlayer->GetAbil(AT_TEAM);
			pkPlayer->SetAbil( AT_TEAM, 0 );
		}

		switch ( iTeam )
		{
		case TEAM_ATTACKER:
			{
				if ( m_iUserCount_AttackTeam >= m_iUserCount_MaxInTeam )
				{
					return false;
				}
			}break;
		case TEAM_DEFENCER:
			{
				if ( m_iUserCount_DefenceTeam >= m_iUserCount_MaxInTeam )
				{
					return false;
				}
			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}break;
		}

		//용병으로 참여했다가 상대방 길드로 들어오려고 하면 막음
		bool const bMercenary = IsMercenary(iTeam, pkPlayer->GetGuildGuid());
		DWORD const dwNow = BM::GetTime32();
		if( bMercenary && IsStart(dwNow) )
		{
			CONT_EMPORIA_BATTLE_USER::iterator itr = m_kContOutUser.find( pkPlayer->GetID() );
			if ( (itr!=m_kContOutUser.end()) && (iTeam!=itr->second.iTeam) )
			{
				BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, 71071 );
				kPacket.Push(EL_Warning);
				pkPlayer->Send( kPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );
				return false;
			}
		}

		std::pair<CONT_EMPORIA_BATTLE_USER::iterator, bool> kPair;
		kPair.second = false;

		CONT_EMPORIA_BATTLE_USER::iterator itr = m_kContOutUser.find( pkPlayer->GetID() );
		if ( itr != m_kContOutUser.end() )
		{
			kPair = m_kContUser.insert( std::make_pair( pkPlayer->GetID(), itr->second ) );
			m_kContOutUser.erase( itr );
		}
		else
		{
			kPair = m_kContUser.insert( std::make_pair( pkPlayer->GetID(), CONT_EMPORIA_BATTLE_USER::mapped_type() ) );
		}

		if ( true == kPair.second )
		{
			CONT_EMPORIA_BATTLE_USER::mapped_type &kElement = kPair.first->second;
			kElement.kCharGuid = pkPlayer->GetID();
			kElement.iTeam = iTeam;
			kElement.iClass = pkPlayer->GetAbil(AT_CLASS);
			kElement.wstrName = pkPlayer->Name();
			kElement.bMercenary = bMercenary;

			AddUserCount( kElement.iTeam, true );

			BM::GUID const& kGuildGuid = TEAM_RED==iTeam ? m_kAttackGuildInfo.kGuildGuid : m_kDefenceGuildInfo.kGuildGuid;
			PgLogCont kContLog(ELogMain_Contents_Guild,ELogSub_EmBattle,m_kEmporiaID,m_kBattleID);
			kContLog.ID(pkPlayer->GetID().str());
			kContLog.Name(pkPlayer->Name());
			kContLog.ChannelNo(g_kProcessCfg.ChannelNo());
			kContLog.Class(pkPlayer->GetAbil(AT_CLASS));
			kContLog.Level(pkPlayer->GetAbil(AT_LEVEL));
			kContLog.RealmNo(g_kProcessCfg.RealmNo());
			kContLog.GroundNo(GndKey().GroundNo());
			PgLog kLog( ELOrderMain_Game, ELOrderSub_Join );
			kLog.Set( 0, kGuildGuid.str() );
			kLog.Set( 0, m_iBattleIndex ); //경기번호
			kLog.Set( 1, static_cast<int>(bMercenary) );	// 용병여부
			kLog.Set( 2, iTeam );
			kContLog.Add(kLog);
			kContLog.Commit();
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgBattleForEmporia::IsMercenary(int const iTeam, BM::GUID const rkGuildGuid)
{
	BM::GUID const & kTeamGuid = (iTeam==TEAM_ATTACKER ? m_kAttackGuildInfo.kGuildGuid : m_kDefenceGuildInfo.kGuildGuid);
	if( kTeamGuid==rkGuildGuid )
	{
		return false;
	}

	return true;
}

HRESULT PgBattleForEmporia::Join( PgPlayer *pkPlayer, bool const bInturde )
{
	CONT_EMPORIA_BATTLE_USER::iterator itr = m_kContUser.find( pkPlayer->GetID() );
	if ( itr != m_kContUser.end() )
	{
		BM::Stream kPacket( PT_M_C_NFY_GAME_WAIT, this->Type() );
		m_kDefenceGuildInfo.WriteToPacket( kPacket );
		m_kAttackGuildInfo.WriteToPacket( kPacket );
		kPacket.Push( m_i64ReadyTime );
		kPacket.Push( m_i64NextEventTime );
		pkPlayer->Send( kPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );

		if ( true == bInturde )
		{
			BM::Stream kBroadPacket( PT_M_C_NFY_GAME_INTRUDER, pkPlayer->GetID() );
			itr->second.WriteToPacket( kBroadPacket );
			VNotify( &kBroadPacket );
		}

		int iEffectNo = 0;
		switch ( itr->second.iTeam )
		{
		case TEAM_ATTACKER:
			{
				iEffectNo = EFFECTNO_DRESS_WAR_RED;
			}break;
		case TEAM_DEFENCER:
			{
				iEffectNo = EFFECTNO_DRESS_WAR_BLUE;
			}break;
		}

		if ( iEffectNo )
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = EFFECT_TYPE_PENALTY;
			kCreate.iEffectNum = iEffectNo;
			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
			pkPlayer->AddEffect( kCreate );
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgBattleForEmporia::Exit( BM::GUID const &kCharGuid )
{
	CONT_EMPORIA_BATTLE_USER::iterator itr = m_kContUser.find( kCharGuid );
	if ( itr != m_kContUser.end() )
	{
		CONT_EMPORIA_BATTLE_USER::mapped_type const& rkUser = itr->second;
		BM::GUID const& kGuildGuid = TEAM_RED==rkUser.iTeam ? m_kAttackGuildInfo.kGuildGuid : m_kDefenceGuildInfo.kGuildGuid;
		PgLogCont kContLog(ELogMain_Contents_Guild,ELogSub_EmBattle,m_kEmporiaID,m_kBattleID);
		kContLog.ID(rkUser.kCharGuid.str());
		kContLog.Name(rkUser.wstrName);
		kContLog.ChannelNo(g_kProcessCfg.ChannelNo());
		kContLog.Class(rkUser.iClass);
		//kContLog.Level(pkPlayer->GetAbil(AT_LEVEL));
		kContLog.RealmNo(g_kProcessCfg.RealmNo());
		kContLog.GroundNo(GndKey().GroundNo());
		PgLog kLog( ELOrderMain_Game, ELOrderSub_End );
		kLog.Set( 0, kGuildGuid.str() );
		kLog.Set( 0, m_iBattleIndex ); //경기번호
		kLog.Set( 1, static_cast<int>(rkUser.bMercenary) );	// 용병여부
		kLog.Set( 2, rkUser.iTeam );
		kContLog.Add(kLog);
		kContLog.Commit();

		m_kContOutUser.insert( std::make_pair( kCharGuid, itr->second ) );
		AddUserCount( itr->second.iTeam, false );
		m_kContUser.erase( itr );

		BM::Stream kBroadPacket( PT_M_C_NFY_GAME_EXIT, kCharGuid );
		VNotify( &kBroadPacket );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgBattleForEmporia::ReadyGame(PgWarGround *pkGnd)
{
	BM::Stream kPacket( PT_M_N_NFY_GAME_READY, m_kEmporiaID );
	kPacket.Push( GndKey() );
	Send(kPacket);

	m_i64NextEventTime = g_kEventView.GetLocalSecTime(CGameTime::DEFAULT);
}

bool PgBattleForEmporia::IsStart( DWORD const dwNowTime )
{
	m_dwLastReBirthTime = g_kEventView.GetServerElapsedTime();// 여기서 맞춰놓고 있어야 그나마 동기화가 맞는다
	__int64 const i64NowTime = g_kEventView.GetLocalSecTime(CGameTime::DEFAULT);
	if ( m_i64NextEventTime <= i64NowTime )
	{
		return true;
	}
	return false;
}

bool PgBattleForEmporia::Start( PgPlayer *pkPlayer, PgWarGround * pkGnd )
{
	pkPlayer->Alive( EALIVE_PVP, E_SENDTYPE_BROADALL );
	int const iTeam = GetTeam( pkPlayer );
	pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
	pkGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pkPlayer), 0, true, iTeam );

	pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
	return iTeam != TEAM_NONE;
}

void PgBattleForEmporia::StartGame( BM::Stream &kPacket, bool const bGame )
{
	if ( bGame )
	{
		BM::Stream kNoticePacket;
		kNoticePacket.Push( NOTICE_EMPORIABATTLE_START );
		kNoticePacket.Push( GetEmporiaGUID() );

		PgRequest_Notice kAction;
		kAction.DoAction( kNoticePacket );
		m_bIsRevive = false;
	}

	m_kDefenceGuildInfo.WriteToPacket( kPacket );
	m_kAttackGuildInfo.WriteToPacket( kPacket );

	PU::TWriteTable_AM(kPacket, m_kContObjectInfo);

	PU::TWriteTable_AM(kPacket, m_kContUser);
	kPacket.Push( m_dwLastReBirthTime );
	kPacket.Push( m_iUserCount_MaxInTeam );

	this->WriteToStartAddOnPacket( kPacket );
}

bool PgBattleForEmporia::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	if ( true == m_bEndGame )
	{
		if ( TEAM_NONE == m_iWinTeam )
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Who is Win???? Ground<") << pkGnd->GroundKey().GroundNo() << _T("/") << pkGnd->GroundKey().Guid() << _T(">") );
			m_iWinTeam = TEAM_DEFENCER; // 기본은 방어 길드 승리
		}
	}

	if ( !bAutoChange && !m_bEndGame )
	{
		SendResultNotice(m_iWinTeam, ERNT_TIMEOVER);
	}

	CONT_EM_RESULT_USER kContWinUser;
	CONT_EM_RESULT_USER kContLoseUser;
	GetMemberData(kContWinUser, kContLoseUser);
	
	// 유저한테 줄 패킷을 만든다.
	BM::Stream kEndPacket( PT_M_C_NFY_GAME_END, bAutoChange );
	kEndPacket.Push( m_iWinTeam );
	kEndPacket.Push( m_bEndGame );
	PU::TWriteTable_AM( kEndPacket, m_kContUser );// Player들의 결과를 보내준다.
	pkGnd->Broadcast( kEndPacket );

	// 서버로 결과를 보내야지
	BM::Stream kRetPacket( PT_M_N_NFY_EMPORIA_BATTLE_RESULT, m_kEmporiaID );
	kRetPacket.Push( m_iWinTeam );
	kRetPacket.Push( pkGnd->GroundKey() );
	kRetPacket.Push( kContWinUser );
	kRetPacket.Push( kContLoseUser );
    kRetPacket.Push( m_eResultType );
	Send( kRetPacket );
	return true;
}

EResultEndType PgBattleForEmporia::ResultEnd()
{
	// 이긴놈들은 엠포리아로 이동시켜 줘야 해
	BM::Stream kPacket( PT_M_C_NFY_GAME_RESULT_END );
	VNotify( &kPacket );
	return RESULT_END_CLOSE;
}

HRESULT PgBattleForEmporia::InitBattleArea(PgWarGround *pkGnd)
{
	if ( !pkGnd )
	{
		return E_FAIL;
	}

	CONT_GTRIGGER const& kContTrigger = pkGnd->GetContTrigger();
	if ( !kContTrigger.empty() )
	{
		CONT_GTRIGGER::const_iterator trigger_itr = kContTrigger.begin();
		for ( ; trigger_itr!=kContTrigger.end() ; ++trigger_itr )
		{
			if ( GTRIGGER_TYPE_BATTLEAREA == trigger_itr->second->GetType() )
			{
				PgGTrigger_BattleArea *pkGTri_BattleArea = dynamic_cast<PgGTrigger_BattleArea*>(trigger_itr->second);
				if ( pkGTri_BattleArea )
				{
					size_t const iSize = static_cast<size_t>(pkGTri_BattleArea->GetParam());
					if ( m_kContBattleArea.size() < iSize )
					{
						m_kContBattleArea.resize( iSize );
					}

					m_kContBattleArea.at( iSize-1 ) = PgBattleArea( *pkGTri_BattleArea );
				}
			}
		}
	}
	return S_OK;
}

int PgBattleForEmporia::GetTeam( PgPlayer *pkPlayer )const
{
	BM::GUID const &kGuildGuid = pkPlayer->GuildGuid();
	if ( kGuildGuid == m_kDefenceGuildInfo.kGuildGuid )
	{
		return TEAM_DEFENCER;
	}

	if ( kGuildGuid == m_kAttackGuildInfo.kGuildGuid )
	{
		return TEAM_ATTACKER;
	}

	CONT_EMPORIA_BATTLE_USER::const_iterator c_iter = m_kContUser.find(pkPlayer->GetID());
	if(c_iter != m_kContUser.end())
	{
		return c_iter->second.iTeam;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return TEAM_NONE"));
	return TEAM_NONE;
}

BYTE PgBattleForEmporia::AddTeamPoint( int const iTeam, unsigned short nPoint, bool const bCharacter, PgWarGround *pkGnd )
{	
	switch( iTeam )
	{
	case TEAM_DEFENCER:
		{
			m_kDefenceGuildInfo.AddPoint( nPoint, bCharacter );
		}break;
	case TEAM_ATTACKER:
		{
			m_kAttackGuildInfo.AddPoint( nPoint, bCharacter );
		}break;
	}
	return 0;
}

DWORD PgBattleForEmporia::GetTime( EPvPTime const kType )const
{
	switch ( kType )
	{
	case PVP_TIME_READY:
		{
			return 4000;
		}break;
	case PVP_TIME_ROUNDTERM:
		{
			return 30000;//30초
		}break;
	case PVP_TIME_RESULT:
		{
			return 10000;//10초
		}break;
	case PVP_TIME_REVIVE:
		{
			return 1000;// 최소 1초
		}break;
	}

	return PgWarMode::GetTime( kType );
}

HRESULT PgBattleForEmporia::Update( DWORD const dwCurTime )
{
	if ( true == m_bEndGame )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( m_iUserCount_AttackTeam < m_iUserCount_MinInTeam )
	{
		INFO_LOG( BM::LOG_LV5, _T("[Emporia Battle] Attack User Count<") << m_iUserCount_AttackTeam << _T("> Lost Guild<") << m_kAttackGuildInfo.kGuildGuid << _T("> Ground<") << GndKey().GroundNo() << _T("/") << GndKey().Guid() << _T(">") );
		m_bEndGame = true;
		m_iWinTeam = TEAM_DEFENCER;

		SendResultNotice(m_iWinTeam, ERNT_USEROUT);
		return E_FAIL;
	}

	if ( m_iUserCount_DefenceTeam < m_iUserCount_MinInTeam )
	{
		INFO_LOG( BM::LOG_LV5, _T("[Emporia Battle] Attack User Count<") << m_iUserCount_DefenceTeam << _T("> Lost Guild<") << m_kDefenceGuildInfo.kGuildGuid << _T("> Ground<") << GndKey().GroundNo() << _T("/") << GndKey().Guid() << _T(">") );
		m_bEndGame = true;
		m_iWinTeam = TEAM_ATTACKER;

		SendResultNotice(m_iWinTeam, ERNT_USEROUT);
		return E_FAIL;
	}

	if ( true == m_bIsRevive )
	{
		m_bIsRevive = false;
		m_dwLastReBirthTime += 10000;
	}
	else
	{
		DWORD const dwCurServerElapsedTime = g_kEventView.GetServerElapsedTime();
		m_bIsRevive = ( 9000 <= ::DifftimeGetTime( m_dwLastReBirthTime, dwCurServerElapsedTime ) );// 여기서 체크는 9초로해야지 동기화가 맞다(함수호출 순서상)
	}

	return S_OK;
}

void PgBattleForEmporia::SetEvent_AdjustArea( CUnit *pkUnit )
{
	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		if ( true == pkUnit->IsAlive() )
		{
			int iLastBattleAreaIndex = pkUnit->GetAbil( AT_BATTLEAREA_INDEX );
			if ( iLastBattleAreaIndex > 0 )
			{
				size_t const iIndex = iLastBattleAreaIndex - 1;
				if ( iIndex < m_kContBattleArea.size() )
				{
					if ( !m_kContBattleArea.at(iIndex).IsInPos( pkUnit->GetPos() ) )
					{
						int const iSpawnType = ( (TEAM_ATTACKER == pkUnit->GetAbil(AT_TEAM)) ? E_SPAWN_BATTLEAREA_ATK : E_SPAWN_BATTLEAREA_DEF );

						// 해킹인가?
						BM::Stream kSpawnPacket( PT_A_G_NFY_RESPAWN, pkUnit->GetID() );
						kSpawnPacket.Push( m_kContBattleArea.at(iIndex).GetParam() );
						kSpawnPacket.Push( iSpawnType );
						VNotify( &kSpawnPacket );
					}
				}
			}
			else
			{
				CONT_BATTLEAREA::iterator area_itr = m_kContBattleArea.begin();
				for ( ; area_itr != m_kContBattleArea.end() ; ++area_itr )
				{
					if ( true == area_itr->IsInPos( pkUnit->GetPos() ) )
					{
						int const iSpawnType = ( (TEAM_ATTACKER == pkUnit->GetAbil(AT_TEAM)) ? E_SPAWN_RED : E_SPAWN_BLUE );

						// 해킹인가?
						BM::Stream kSpawnPacket( PT_A_G_NFY_RESPAWN, pkUnit->GetID() );
						kSpawnPacket.Push( static_cast<int>(1) );
						kSpawnPacket.Push( iSpawnType );
						VNotify( &kSpawnPacket );
					}
				}
			}
		}
	}
}

void PgBattleForEmporia::WriteToStartAddOnPacket( BM::Stream &kPacket )const
{
	PU::TWriteArray_M( kPacket, m_kContBattleArea );
}

HRESULT PgBattleForEmporia::SetSelectArea( CUnit *pkUnit, int const iAreaIndex, PgGround *pkGnd )
{
	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		int const iTeam = pkUnit->GetAbil(AT_TEAM);
		int const iLastBattleAreaIndex = pkUnit->GetAbil( AT_BATTLEAREA_INDEX );
		if ( iAreaIndex != iLastBattleAreaIndex )
		{
			if ( iAreaIndex > 0 )
			{	
				if ( NULL == pkUnit->GetEffect( EFFECTNO_DRAGON_SELECT_DELAYTIME ) )
				{
					size_t const iIndex = static_cast<size_t>(iAreaIndex) - 1;
					if ( iIndex < m_kContBattleArea.size() )
					{
						int const iSpawnType = ( (TEAM_ATTACKER == iTeam) ? E_SPAWN_BATTLEAREA_ATK : E_SPAWN_BATTLEAREA_DEF );

						if ( true == pkGnd->SendToSpawnLoc( pkUnit, m_kContBattleArea.at(iIndex).GetParam(), true, iSpawnType ) )
						{
							if ( S_OK == m_kContBattleArea.at(iIndex).AddUser( pkUnit->GetID(), iTeam ) )
							{
								BM::Stream kBroadPacket( PT_M_C_NFY_BATTLE_AREA_USER, iLastBattleAreaIndex );
								kBroadPacket.Push( iAreaIndex );
								kBroadPacket.Push( pkUnit->GetID() );
								kBroadPacket.Push( iTeam );
								pkGnd->Broadcast( kBroadPacket );
							}

							pkUnit->SetAbil( AT_BATTLEAREA_INDEX, iAreaIndex, false, false );

                            SEffectCreateInfo kCreate;
			                kCreate.eType = EFFECT_TYPE_PENALTY;
			                kCreate.iEffectNum = EFFECTNO_DRAGON_SELECT_DELAYTIME;
                            kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
                            pkUnit->AddEffect(kCreate);
						}
						else
						{
							return E_FAIL;
						}
					}
				}
				else
				{
					// 아직 바꿀 수 없다.
					pkUnit->SendWarnMessage( 235 );
					return E_FAIL;
				}
			}
			else
			{
				int const iSpawnType = ( (TEAM_ATTACKER == iTeam) ? E_SPAWN_RED : E_SPAWN_BLUE );

				if ( true == pkGnd->SendToSpawnLoc( pkUnit->GetID(), 1, true, iSpawnType ) )
				{
					pkUnit->SetAbil( AT_BATTLEAREA_INDEX, 0, false, false );
				}
			}

			if ( iLastBattleAreaIndex > 0 )
			{
				size_t const iIndex = static_cast<size_t>(iLastBattleAreaIndex) - 1;
				if ( iIndex < m_kContBattleArea.size() )
				{
					m_kContBattleArea.at(iIndex).RemoveUser( pkUnit->GetID() );
				}
			}
			return S_OK;
		}
        else
        {
            pkUnit->SendWarnMessage( 245 );
			return E_FAIL;
        }
	}
	return E_FAIL;
}

void PgBattleForEmporia::GetMemberData(CONT_EM_RESULT_USER & kContWinUser, CONT_EM_RESULT_USER & kContLoseUser)
{
	CONT_EMPORIA_BATTLE_USER::const_iterator c_iter = m_kContUser.begin();
	while(c_iter != m_kContUser.end())
	{
		CONT_EMPORIA_BATTLE_USER::mapped_type const & user = c_iter->second;
		
		CONT_EM_RESULT_USER::value_type kMember;
		kMember.bMercenary = user.bMercenary;
		kMember.kCharGuid = user.kCharGuid;

		if(m_iWinTeam==user.iTeam)
		{			
			kContWinUser.push_back(kMember);
		}
		else
		{
			kContLoseUser.push_back(kMember);
		}
		++c_iter;
	}
}

HRESULT PgBattleForEmporia::SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd )
{
	if ( !pkUnit )
	{
		return E_FAIL;
	}

	CONT_EMPORIA_BATTLE_USER::iterator user_itr = m_kContUser.find( pkUnit->GetID() );
	if ( user_itr != m_kContUser.end() )
	{
		user_itr->second.kResult.AddPoint( static_cast<int>(nAddPoint) );

		BYTE byLeaderTeam = 0;

		// 먼저 보내고 AddTeamPoint
		if ( 0 < nAddPoint )
		{
			byLeaderTeam = this->AddTeamPoint( pkUnit->GetAbil(AT_TEAM), static_cast<unsigned short>(nAddPoint), true, pkGnd );
		}

		BM::Stream kNfyPacket( PT_M_C_NFY_ADD_POINT_USER, pkUnit->GetID() );
		kNfyPacket.Push( static_cast<WORD>(nAddPoint) );
		kNfyPacket.Push( byLeaderTeam );
		kNfyPacket.Push( m_kDefenceGuildInfo.nCharacterPoint );
		kNfyPacket.Push( m_kAttackGuildInfo.nCharacterPoint );
		pkGnd->Broadcast( kNfyPacket );
		return S_OK;
	}
	return E_FAIL;
}

void PgBattleForEmporia::SendResultNotice(int const iWinTeam, EEmporiaResultNoticeType const eType, CUnit const * pkKiller)const
{
	SGuildBattleInfo const & rkWinGuild = (TEAM_ATTACKER==iWinTeam) ? m_kAttackGuildInfo : m_kDefenceGuildInfo;

    m_eResultType = eType;

	BM::Stream kPacket;
	kPacket.Push(eType);

	switch(eType)
	{
	case ERNT_USEROUT:
	case ERNT_TIMEOVER:
		{
			//#GUILDNAME# 길드가 길드전에서 승리했습니다.
			kPacket.Push(rkWinGuild.wstrName);
		}break;
	case ERNT_HAVEPOINT:
		{
			//#GUILDNAME# 길드가 포인트 '#POINT#'점을 획득하여 길드전에서 승리했습니다.
			kPacket.Push(rkWinGuild.wstrName);
			kPacket.Push(rkWinGuild.GetPoint());
		}break;
	case ERNT_DESTORYCORE:
	case ERNT_KILLDRAGON:
		{
			std::wstring kKillerName;
			if( pkKiller )
			{
				CONT_EMPORIA_BATTLE_USER::const_iterator c_iter = m_kContUser.find(pkKiller->GetID());
				if( c_iter!=m_kContUser.end() )
				{
					kKillerName = c_iter->second.wstrName;
				}
			}

			//'#USER#'님이 코어를 파괴하여 #WINGUILD# 길드가 길드전에서 승리했습니다.
			//'#USER#'님이 거대수호용을 섬멸하여 #WINGUILD# 길드가 길드전에서 승리했습니다.
			//#GUILDNAME# 길드가 길드전에서 승리했습니다.(empty())
			kPacket.Push(kKillerName);
			kPacket.Push(rkWinGuild.wstrName);
		}break;
	}

	if( kPacket.Size() > sizeof(EEmporiaResultNoticeType) )
	{
		BM::Stream kBroadPacket;
		kBroadPacket.Push(E_NoticeMsg_SendPacket);
		kBroadPacket.Push(NOTICE_ALL);
		kBroadPacket.Push(BM::GUID::NullData());
		kBroadPacket.Push(NOTICE_EMPORIABATTLE_RESULT);
		kBroadPacket.Push(kPacket);
		::SendToRealmContents( PMET_Notice, kBroadPacket );
	}
}

HRESULT PgBattleForEmporia::SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd )
{
	return S_OK;
}

HRESULT PgBattleForEmporia::SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd )
{
	return S_OK;
}