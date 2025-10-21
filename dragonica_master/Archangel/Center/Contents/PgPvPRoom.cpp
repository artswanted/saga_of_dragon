#include "stdafx.h"
#include "Collins/Log.h"
#include "PgPvPRoom.h"
#include "PgPortalMgr.h"
#include "PgSendWrapper.h"

int const KTH_MIN_VICTORY_POINT = 1000;

PgPvPRoom::PgPvPRoom( int const iIndex, int const &iLobbyID )
:	m_iIndex(iIndex)
,	m_iLobbyID(iLobbyID)
,	m_kRoomGuid(BM::GUID::Create())
,	m_kBasicInfo(ROOM_STATUS_CLOSE)
,	m_bOrderClose(false)
{
}

PgPvPRoom::~PgPvPRoom()
{
}

void PgPvPRoom::ProcessMsg( SEventMessage* const pkMsg )
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( pkMsg->Pop( usType ) )
	{
		switch ( usType )
		{
		case PT_M_T_NFY_GAME_RESULT:
			{
				this->End( pkMsg );
			}break;
		case PT_M_T_NFY_GAME_RESULT_END:
			{
				bool bNoEnemy = false;
				pkMsg->Pop(bNoEnemy);
				ReloadRoom(bNoEnemy);
			}break;
		case PT_C_T_REQ_TEAM_CHANGE:
		case PT_C_T_REQ_GAME_READY:
		case PT_C_T_REQ_KICK_ROOM_USER:
		case PT_C_T_REQ_MODIFY_ROOM:
		case PT_C_T_REQ_SLOTSTATUS_CHANGE:
		case PT_C_T_REQ_ENTRUST_MASTER:
		case PT_C_T_REQ_ENTRY_CHANGE:
			{
				BM::GUID kCharGuid;
				pkMsg->Pop(  kCharGuid );
				RecvPacket( kCharGuid, usType, pkMsg );
			}break;
		case PT_M_T_ANS_READY_GROUND:
			{
				// АМ°Ф їАёй ёКј­№ц АЪГј ·№µрїЎ ЅЗЖРЗС°ЕѕЯ
				SGroundKey kGndKey;
				pkMsg->Pop( kGndKey );
				ReadyFailed( kGndKey );
			}break;
		default:
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T("invalid PacketType=") << usType );
				CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("invalid PacketType=") << usType );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("invalid PacketType=") << usType );
			}break;
		}
	}
}

void PgPvPRoom::RecvPacket( BM::GUID const &kCharGuid, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket )
{
	switch( usType )
	{
	case PT_C_T_REQ_MODIFY_ROOM:
		{
			Modify( kCharGuid, pkPacket );
		}break;
	case PT_C_T_REQ_EXIT_ROOM:
		{
			if ( SUCCEEDED( Exit( kCharGuid ) ) )
			{
				BM::Stream kPacket( PT_ROOM_TO_LOBBY_USER, static_cast<size_t>(1) );
				kPacket.Push( kCharGuid );
				kPacket.Push( false );	// Is Kick
				SendToPvPLobby( kPacket, m_iLobbyID );
			}
		}break;
	case PT_C_T_REQ_KICK_ROOM_USER:
		{
			BM::GUID kKickCharGuid;
			pkPacket->Pop( kKickCharGuid );
			KickUser( kCharGuid, kKickCharGuid );
		}break;
	case PT_C_T_REQ_SLOTSTATUS_CHANGE:
		{
			SetSlotStatus( kCharGuid, pkPacket );
		}break;
	case PT_C_T_REQ_ENTRY_CHANGE:
		{
			EntryChange( kCharGuid, pkPacket );
		}break;
	case PT_C_T_REQ_TEAM_CHANGE:
		{
			int iTeam = 0;
			pkPacket->Pop(iTeam);
			TeamChange( kCharGuid, iTeam );
		}break;
	case PT_C_T_REQ_ENTRUST_MASTER:
		{
			BM::GUID kTargetCharGuid;
			pkPacket->Pop( kTargetCharGuid );
			EntrustMaster( kCharGuid, kTargetCharGuid );
		}break;
	case PT_C_T_REQ_GAME_READY:
		{
			Ready( kCharGuid, pkPacket );
		}break;
	}
}

void PgPvPRoom::SendWarnMessage( PgPlayer const * pkPlayer, int const iMessageNo, BYTE const byLevel )
{
	if ( pkPlayer )
	{
		BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, iMessageNo);
		kPacket.Push(byLevel);
		::SendToUser( pkPlayer->GetMemberGUID(), pkPlayer->GetSwitchServer(), kPacket );
	}
}

void PgPvPRoom::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kBasicInfo.Clear();
	m_kExtInfo.Clear();
	m_kMasterGuid.Clear();
	m_kContSlot_User.clear();
	m_kContSlot_Empty.clear();
	m_kGameGround.Clear();
	m_kUserEvent.Clear();
	m_bOrderClose = false;
	m_kBattleGuid.Clear();
}

HRESULT PgPvPRoom::Create( BM::GUID const &kBattleGuid, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo /*unsigned int const iCreateNo*/ )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( IsOpen() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critacal Error Open Room No=") << m_iIndex );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
		return E_ACCESSDENIED;
	}

	if ( SUCCEEDED( Check( kBasicInfo, kExtInfo ) ) )
	{
		m_bOrderClose = false;
		kBasicInfo.m_kStatus = ROOM_STATUS_LOBBY;
		kBasicInfo.m_ucNowUser = 0;
		kBasicInfo.m_iRoomIndex = m_iIndex;
		m_kExtInfo = kExtInfo;
		kBasicInfo.m_bPwd = !m_kExtInfo.strPassWord.empty();
		m_kBasicInfo = kBasicInfo;

		// SlotА» ёёµйѕо µРґЩ.
		RefreshSlot();
		// їЈЖ®ё® ГК±вИ­
		RefreshEntry();

		m_kBattleGuid = kBattleGuid;
		SendModifyed();
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::Create( PgPlayer const * pkPlayer, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo /*unsigned int const iCreateNo*/ )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( IsOpen() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critacal Error Open Room No=") << m_iIndex );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
		return E_ACCESSDENIED;
	}

	if ( SUCCEEDED( Check( pkPlayer, kBasicInfo, kExtInfo ) ) )
	{
		m_bOrderClose = false;
		kBasicInfo.m_kStatus = ROOM_STATUS_LOBBY;
		kBasicInfo.m_ucNowUser = 1;
		kBasicInfo.m_iRoomIndex = m_iIndex;
		m_kExtInfo = kExtInfo;
		kBasicInfo.m_bPwd = !m_kExtInfo.strPassWord.empty();
		m_kBasicInfo = kBasicInfo;

		// SlotА» ёёµйѕо µРґЩ.
		RefreshSlot();
		// їЈЖ®ё® ГК±вИ­
		RefreshEntry();
		if ( SUCCEEDED( Join( pkPlayer, kExtInfo.strPassWord ) ) )
		{
			return S_OK;
		}

		CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Create Join Failed RoomIndex<") << m_iIndex << _T("> ") << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
		Clear();
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPRoom::Close()
{
	BM::CAutoMutex kLock(m_kMutex);

	switch( m_kBasicInfo.m_kStatus )
	{
	case ROOM_STATUS_CLOSE:
		{
		}break;
	case ROOM_STATUS_LOBBY:
	case ROOM_STATUS_READY:
	case ROOM_STATUS_READY_EXITUSER:
		{
			// ґЩ ¦iѕЖіЅґЩ.
			VEC_GUID kUserGuidList;
			kUserGuidList.reserve( m_kContSlot_User.size() );

			ContUserSlot::iterator user_itr = m_kContSlot_User.begin();
			for ( ; user_itr != m_kContSlot_User.end() ; ++user_itr )
			{
				kUserGuidList.push_back( user_itr->first );
			}

			BM::Stream kPacket1( PT_ROOM_TO_LOBBY_USER, kUserGuidList );
			SendToPvPLobby( kPacket1, m_iLobbyID );

			BM::Stream kPacket2( PT_ROOM_TO_LOBBY_REMOVED, GetIndex() );
			SendToPvPLobby( kPacket2, m_iLobbyID );

			Clear();
		}break;
	case ROOM_STATUS_PLAY:
	case ROOM_STATUS_RESULT:
		{
			// Бѕ·бµЗёй ¦iѕЖі»ѕЯ ЗСґЩ.(¦iѕЖі»±в ї№ѕа)
			m_bOrderClose = true;
		}break;
	}
}

HRESULT PgPvPRoom::Check( EPVPTYPE const kType, EPVPMODE& kMode, BYTE& ucRound )const
{
	HRESULT hRet = S_OK;
	if ( !ucRound )
	{
		ucRound = 1;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
		hRet = S_FALSE;
	}
	else if ( ucRound > 19 )
	{
		ucRound = 19;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
		hRet = S_FALSE;
	}

	switch ( kType )
	{
	case PVP_TYPE_DM:
		{
			if ( ucRound > 1 )
			{
				ucRound = 1;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
		}break;
	case PVP_TYPE_ANNIHILATION:
		{
			kMode = PVP_MODE_TEAM;
			if( ucRound > 9 )
			{	// ¶уїоµе°Ў 9єёґЩ Е©ёй 9·О јјЖГ.
				ucRound = 9;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
			else if( ucRound < 3 )
			{	// ¶уїоµе°Ў 3єёґЩ АЫАёёй 3Аё·О јјЖГ.
				ucRound = 3;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
			else if( !(ucRound % 2) )
			{	// ¶уїоµе°Ў В¦јцёй -1ЗШј­ И¦јц ¶уїоµе·О јјЖГ.
				ucRound -= 1;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
		}break;
	case PVP_TYPE_KTH:
		{
			kMode = PVP_MODE_TEAM;
			if ( ucRound != 1 )
			{
				ucRound = 1;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
		}break;
	case PVP_TYPE_DESTROY:
		{
			kMode = PVP_MODE_TEAM;
			if ( ucRound < 9 )
			{
				ucRound = 9;// ¶уїоµе°Ў 9єёґЩ АЫАє °жїмґВ 9°і·О °­Б¦
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
		}break;
	case PVP_TYPE_WINNERS:
		{
			int const iUserCount = m_kContSlot_User.size();
			if( 2 < iUserCount )
			{//ГЦґл ¶уїоµеґВ (АЇАъјц / 2) + 1
				ucRound = (iUserCount / 2) + 1;
				if( iUserCount % 2 )
				{//АЇАъ јц°Ў И¦јц( 3 / 5 / 7 ) АО °жїм +1ЗШБЬ.
					++ucRound;
				}
			}
			else
			{
				ucRound = 1;
			}
		}break;
	case PVP_TYPE_WINNERS_TEAM:
		{
			int iUserCount = 0, iRedTeamCount = 0, iBlueTeamCount = 0;
			iUserCount = GetTeamUserCount(iRedTeamCount, iBlueTeamCount);
			if( 1 < iUserCount )
			{
				if( 2 < iRedTeamCount )
				{
					iRedTeamCount = 2;
				}
				else
				{
					iRedTeamCount = 1;
				}
				if( 2 < iBlueTeamCount )
				{
					iRedTeamCount = 2;
				}
				else
				{
					iBlueTeamCount = 1;
				}
				ucRound = (iRedTeamCount + iBlueTeamCount) - 1;
			}
		}break;
	case PVP_TYPE_ACE:
		{
			if( 1 < m_kContSlot_User.size() )
			{//ГЦґл ¶уїоµеґВ АЇАъјц-1 АМґЩ
				ucRound = m_kContSlot_User.size() - 1;
			}
		}break;
	case PVP_TYPE_LOVE:
		{
			kMode = PVP_MODE_TEAM;
			if( ucRound > 9 )
			{	// ¶уїоµе°Ў 9єёґЩ Е©ёй 9·О јјЖГ.
				ucRound = 9;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
			else if( ucRound < 5 )
			{	// ¶уїоµе°Ў 5єёґЩ АЫАёёй 5Аё·О јјЖГ.
				ucRound = 5;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
			else if( !(ucRound % 2) )
			{	// ¶уїоµе°Ў В¦јцёй -1ЗШј­ И¦јц ¶уїоµе·О јјЖГ.
				ucRound -= 1;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FAILS"));
				return S_FALSE;
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}break;
	}
	return hRet;
}

HRESULT PgPvPRoom::Check( SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo )
{
	if ( kBasicInfo.m_kMode >= PVP_MODE_MAX )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Hacking? Mode=") << kBasicInfo.m_kMode );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( (0 == kBasicInfo.m_ucMaxUser) || (kBasicInfo.m_ucMaxUser > ms_ucMaxRoomUserCount) )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Hacking? MaxUserCount<") << kBasicInfo.m_ucMaxUser << _T("> is Wrong ") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CONT_DEF_PVP_GROUNDMODE const *pkDefPvPMode = NULL;
	g_kTblDataMgr.GetContDef(pkDefPvPMode);

	if ( !pkDefPvPMode )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FUNCTIONW__ << _T("Not Found DefPvPMode"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CONT_DEF_PVP_GROUNDMODE::const_iterator gnd_itr = pkDefPvPMode->find(kBasicInfo.m_iGndNo);
	if ( gnd_itr == pkDefPvPMode->end() )
	{// ГЈА» јц°Ў ѕшАёёй ѕИµИґЩ.
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Hacking? GroundNo=") << kBasicInfo.m_iGndNo );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	EPVPTYPE const _kType = (EPVPTYPE)(gnd_itr->second.iType);
	if ( !(_kType & kBasicInfo.m_kType) )
	{
		// ёрµеґВ °­Б¦·О єЇ°ж
		kBasicInfo.m_kType = PVP_TYPE_DM;
		while ( !(_kType & kBasicInfo.m_kType) )
		{
			kBasicInfo.m_kType <<= 1;
			if ( !(kBasicInfo.m_kType&PVP_TYPE_ALL) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Bad GroundNo=") << kBasicInfo.m_iGndNo << _T(", DefPvPMode=")\
					<< gnd_itr->second.iType );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
	}

	if ( kBasicInfo.m_ucMaxUser < kBasicInfo.m_ucNowUser )
	{// ЗцАз АЇАъєёґЩ АЫАёёй ѕИµК!
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FUNCTIONW__ << _T("MaxUseCount=") << kBasicInfo.m_ucMaxUser << _T(" is to low"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	switch ( m_iLobbyID )
	{
	case PvP_Lobby_GroundNo_Exercise:
		{
			BYTE byAndFlag = E_PVP_OPT_USEBATTLELEVEL;
			kExtInfo.kOption &= (~byAndFlag);
		}break;
	case PvP_Lobby_GroundNo_League:
	case PvP_Lobby_GroundNo_Ranking:
		{
			kExtInfo.kOption |= E_PVP_OPT_USEBATTLELEVEL;
		}break;
	}

	return Check( kBasicInfo.m_kType, kBasicInfo.m_kMode, kExtInfo.ucRound );
}

HRESULT PgPvPRoom::Check( PgPlayer const * pkPlayer, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo )
{
	if ( kBasicInfo.m_kMode >= PVP_MODE_MAX )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Hacking? Mode=") << kBasicInfo.m_kMode << _COMMA_ << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( (0 == kBasicInfo.m_ucMaxUser) || (kBasicInfo.m_ucMaxUser > ms_ucMaxRoomUserCount) )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Hacking? MaxUserCount<") << kBasicInfo.m_ucMaxUser << _T("> is Wrong ") << _COMMA_ << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CONT_DEF_PVP_GROUNDMODE const *pkDefPvPMode = NULL;
	g_kTblDataMgr.GetContDef(pkDefPvPMode);

	if ( !pkDefPvPMode )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FUNCTIONW__ << _T("Not Found DefPvPMode"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CONT_DEF_PVP_GROUNDMODE::const_iterator gnd_itr = pkDefPvPMode->find(kBasicInfo.m_iGndNo);
	if ( gnd_itr == pkDefPvPMode->end() )
	{// ГЈА» јц°Ў ѕшАёёй ѕИµИґЩ.
		SendWarnMessage( pkPlayer, 200115, EL_PvPMsg );
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Hacking? GroundNo=") << kBasicInfo.m_iGndNo << _COMMA_ << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	EPVPTYPE const _kType = (EPVPTYPE)(gnd_itr->second.iType);
	if ( !(_kType & kBasicInfo.m_kType) )
	{
		// ёрµеґВ °­Б¦·О єЇ°ж
		kBasicInfo.m_kType = PVP_TYPE_DM;
		while ( !(_kType & kBasicInfo.m_kType) )
		{
			kBasicInfo.m_kType <<= 1;
			if ( !(kBasicInfo.m_kType&PVP_TYPE_ALL) )
			{
				SendWarnMessage( pkPlayer, 200112, EL_PvPMsg );
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Bad GroundNo=") << kBasicInfo.m_iGndNo << _T(", DefPvPMode=")\
					<< gnd_itr->second.iType );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
	}

	if( PVP_TYPE_KTH & kBasicInfo.m_kType )
	{
		kBasicInfo.m_iGamePoint = std::max(kBasicInfo.m_iGamePoint, KTH_MIN_VICTORY_POINT);	//БЎ·ЙАь ГЦјТ ЅВё®ЖчАОЖ®°Ў јіБ¤µЗµµ·П ЗФ
	}

	if ( kBasicInfo.m_ucMaxUser < kBasicInfo.m_ucNowUser )
	{// ЗцАз АЇАъєёґЩ АЫАёёй ѕИµК!
		SendWarnMessage( pkPlayer, 200111, EL_PvPMsg );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// min, max µСґЩ 0АП °жїм Б¦ЗСАМ ѕшґВ °НАё·О ЗСґЩ.
	bool bRet = kBasicInfo.IsHaveLimitLevel();

	if( bRet )
	{ // min, max µС БЯїЎ ЗПіЄ¶уµµ °ЄАМ АЦАёёй °Л»зЗСґЩ.
		if ( m_kContSlot_User.empty() )
		{
			short const sLevel = static_cast<short>(pkPlayer->GetAbil(AT_LEVEL));
			if (	(sLevel < kBasicInfo.m_sLevelLimit_Min) 
				||	(sLevel > kBasicInfo.m_sLevelLimit_Max)
				)
			{// ·№є§АМ Б¦ґл·О ‰зґВБц ГјЕ©
				SendWarnMessage( pkPlayer, 200113, EL_PvPMsg );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
		else
		{
			ContUserSlot::const_iterator slot_itr = m_kContSlot_User.begin();
			for ( ; slot_itr!=m_kContSlot_User.end(); ++slot_itr )
			{
	//			ContUserSlot::mapped_type const &kElement = slot_itr->second;
				short const sLevel = static_cast<short>(slot_itr->second.pkPlayer->GetAbil(AT_LEVEL));
				if (	(sLevel < kBasicInfo.m_sLevelLimit_Min) 
					||	(sLevel > kBasicInfo.m_sLevelLimit_Max)
					)
				{// ·№є§АМ Б¦ґл·О ‰зґВБц ГјЕ©
					SendWarnMessage( pkPlayer, 200113, EL_PvPMsg );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}
		}
	}

	switch ( m_iLobbyID )
	{
	case PvP_Lobby_GroundNo_Exercise:
		{
			BYTE byAndFlag = E_PVP_OPT_USEBATTLELEVEL;
			kExtInfo.kOption &= (~byAndFlag);
		}break;
	case PvP_Lobby_GroundNo_League:
	case PvP_Lobby_GroundNo_Ranking:
		{
			kExtInfo.kOption |= E_PVP_OPT_USEBATTLELEVEL;
		}break;
	}

	return Check( kBasicInfo.m_kType, kBasicInfo.m_kMode, kExtInfo.ucRound );
}

//RefreshSlot АМірАє ModifyїЎј­ ИЈГвЗПґВµҐ °Е±вј­ Lock ЗШБЬ.
HRESULT PgPvPRoom::RefreshSlot()
{
	// ѕЖБч їПАьЗПБц ѕКґЩ ГіАЅёёµй¶§ёё »зїлЗП°н... іЄБЯїЎ ґЩЅГ АзБ¤єс ЗШѕЯ ЗСґЩ.
	size_t const iSlotSize = (size_t)(m_kBasicInfo.m_ucMaxUser);
	if ( iSlotSize < m_kContSlot_User.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( iSlotSize > 10 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_kContSlot_Empty.clear();

	switch ( m_kBasicInfo.m_kMode )
	{
	case PVP_MODE_TEAM:
		{
			BYTE kMaxSlot = (BYTE)(iSlotSize / 2);
			BYTE kNowSlot=0;
			for( ; kNowSlot<kMaxSlot; ++kNowSlot )
			{
				for ( int iTeam=TEAM_RED; iTeam<=TEAM_BLUE; ++iTeam )
				{
					m_kContSlot_Empty.insert( SPvPTeamSlot(iTeam,kNowSlot) );
				}
			}

			if ( iSlotSize % 2 )
			{
				m_kContSlot_Empty.insert( SPvPTeamSlot(TEAM_RED,kNowSlot) );
			}
		}break;
	case PVP_MODE_PERSONAL:
		{
			for ( BYTE i=0; i<(BYTE)iSlotSize; ++i )
			{
				m_kContSlot_Empty.insert( SPvPTeamSlot(i+TEAM_PERSONAL_BEGIN,i) );
			}
		}break;
	}

	ContUserSlot::iterator slot_itr;
	CONT_PVP_SLOT::iterator empty_itr;
	for ( slot_itr=m_kContSlot_User.begin(); slot_itr!=m_kContSlot_User.end(); ++slot_itr )
	{
		empty_itr = m_kContSlot_Empty.find( slot_itr->second.kTeamSlot );
		if ( empty_itr == m_kContSlot_Empty.end() )
		{
			empty_itr = m_kContSlot_Empty.begin();
		}

		slot_itr->second.kTeamSlot = *empty_itr;
		slot_itr->second.byStatus &= (~PS_READY);// ·№µрґВ З®·БѕЯ ЗСґЩ.
		m_kContSlot_Empty.erase(empty_itr);
	}
	return S_OK;
}

HRESULT PgPvPRoom::RefreshEntry()
{
	size_t const iEntrySize = (size_t)(m_kBasicInfo.m_ucMaxUser);
	if ( iEntrySize < m_kContSlot_User.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( iEntrySize > 10 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_kContEntry_Empty.clear();

	BYTE kMaxEntry = (BYTE)(iEntrySize / 2);
	BYTE kNowEntry=1;// 0АМёй ѕЖБч ГК±вИ­ µЗБц ѕКАє °ЕАУ
	for( ; kNowEntry<=kMaxEntry; ++kNowEntry )
	{
		for ( int iTeam=TEAM_RED; iTeam<=TEAM_BLUE; ++iTeam )
		{
			m_kContEntry_Empty.insert( SPvPTeamEntry(iTeam,kNowEntry) );
		}
	}

	ContUserSlot::iterator slot_itr;
	CONT_PVP_ENTRY::iterator entry_itr;
	for ( slot_itr=m_kContSlot_User.begin(); slot_itr!=m_kContSlot_User.end(); ++slot_itr )
	{
		entry_itr = m_kContEntry_Empty.find( SPvPTeamEntry(slot_itr->second.kTeamSlot.GetTeam(), slot_itr->second.byEntryNo) );
		if ( m_kContEntry_Empty.end() != entry_itr )
		{
			m_kContEntry_Empty.erase(entry_itr);
		}
	}
	return S_OK;
}

EPvPRoomJoinRet PgPvPRoom::SetJoin( PgPlayer const *pkPlayer, std::string const &strPassWord )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( !IsOpen() )
	{
		return PVP_JOIN_ERROR;
	}

	if ( IsPlaying() )
	{
		// °ФАУБЯїЎ µйѕо їГ јц ѕшґЩ.
		return PVP_JOIN_PLAYING;
	}

	if ( m_kExtInfo.strPassWord != strPassWord )
	{
		// ЖРЅєїцµе°Ў ЖІ·Бј­ µйѕо їГ јц ѕшґЩ.
		if ( strPassWord.empty() )
		{
			return PVP_JOIN_PASSWORD_EMPTY;
		}
		return PVP_JOIN_PASSWORD_FAILED;
	}

	// min, max µСґЩ 0АП °жїм Б¦ЗСАМ ѕшґВ °НАё·О ЗСґЩ.
	bool bRet = m_kBasicInfo.IsHaveLimitLevel();

	if( bRet )
	{ // min, max µС БЯїЎ ЗПіЄ¶уµµ °ЄАМ АЦАёёй °Л»зЗСґЩ.
		short const sLevel = static_cast<short>(pkPlayer->GetAbil(AT_LEVEL));
		if (	(sLevel < m_kBasicInfo.m_sLevelLimit_Min) 
			||	(sLevel > m_kBasicInfo.m_sLevelLimit_Max)
			)
		{	// ·№є§АМ ёВБц ѕКѕЖј­ АФАе ЗТ јц ѕшЅАґПґЩ.
			return PVP_JOIN_LEVELLIMIT;
		}
	}

	return JoinEmptySlot( pkPlayer );
}

HRESULT PgPvPRoom::Join( PgPlayer const * pkPlayer, int const iTeam)
{
	BM::CAutoMutex kLock(m_kMutex);

	EPvPRoomJoinRet kRet = PVP_JOIN_SUCCEED;
	if ( !IsOpen() )
	{
		kRet = PVP_JOIN_ERROR;
	}
	else if( IsPlaying() )
	{
		kRet = PVP_JOIN_PLAYING;
	}
	else
	{
		kRet = JoinEmptySlot(pkPlayer, iTeam);
	}

	BM::Stream kAnsPacket( PT_T_C_ANS_JOIN_ROOM, GetRoomNo() );
	kAnsPacket.Push( kRet );
	kAnsPacket.Push( true );
	if ( PVP_JOIN_SUCCEED == kRet )
	{
		m_kBasicInfo.WriteToPacket(kAnsPacket);
		m_kExtInfo.WriteToPacket(kAnsPacket);
		PU::TWriteTable_AM(kAnsPacket, m_kContSlot_User);
		PU::TWriteKey_A(kAnsPacket, m_kContSlot_Empty);
	}
	::SendToUser( pkPlayer->GetMemberGUID(), pkPlayer->GetSwitchServer(), kAnsPacket );

	if ( PVP_JOIN_SUCCEED == kRet )
	{
		if ( 0 < m_kContSlot_User.size() )
		{
			SendModifyed();
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::Join( PgPlayer const * pkPlayer, std::string const &strPassWord, bool const bJoin )
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kAnsPacket( PT_T_C_ANS_JOIN_ROOM, GetRoomNo() );

	EPvPRoomJoinRet const kRet = SetJoin( pkPlayer, strPassWord );
	
	kAnsPacket.Push( kRet );
	kAnsPacket.Push( bJoin );

	if ( PVP_JOIN_SUCCEED == kRet )
	{
		m_kBasicInfo.WriteToPacket(kAnsPacket);
		m_kExtInfo.WriteToPacket(kAnsPacket);
		PU::TWriteTable_AM(kAnsPacket, m_kContSlot_User);
		PU::TWriteKey_A(kAnsPacket, m_kContSlot_Empty);
	}
	
	::SendToUser( pkPlayer->GetMemberGUID(), pkPlayer->GetSwitchServer(), kAnsPacket );

	if ( PVP_JOIN_SUCCEED == kRet )
	{
		if ( m_kContSlot_User.size() > 1 )
		{
			SendModifyed();
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::TeamChange( BM::GUID const &kCharGuid, int const iTeam )
{
	BM::CAutoMutex kLock(m_kMutex);

	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
	if ( slot_itr != m_kContSlot_User.end() )
	{
		EPvPRoomJoinRet const kRet = JoinEmptySlot( slot_itr->second.pkPlayer, iTeam, true );
		switch( kRet )
		{
		case PVP_JOIN_SUCCEED:
			{
				return S_OK;
			}break;
		case PVP_JOIN_BALANCECLASS:
			{
				SendWarnMessage( slot_itr->second.pkPlayer, PVP_JOIN_BALANCECLASS, EL_PvPMsg );
			}break;
		default:
			{
			}break;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

inline bool CheckClassLimit(__int64 const iValue, __int64 const iClassLimit, __int64 const iClassLimit2)
{
	return (iValue&iClassLimit) && (iValue&iClassLimit2);
}

bool PgPvPRoom::IsCheckBalanceJoin( int const iClass, int const iTeam )const
{
	__int64 const iClassLimit = GET_CLASS_LIMIT(iClass);
	ContUserSlot::const_iterator user_slot_itr = m_kContSlot_User.begin();
	for ( ; user_slot_itr!=m_kContSlot_User.end() ; ++user_slot_itr )
	{
		if ( iTeam == user_slot_itr->second.kTeamSlot.GetTeam() )
		{
			int const iUserClass = user_slot_itr->second.pkPlayer->GetAbil( AT_CLASS );
			__int64 const iUserClassLimit = GET_CLASS_LIMIT(iUserClass);
			if(iClassLimit==iUserClassLimit){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_KNIGHT	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_WARRIOR	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_MAGE		, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_BATTLEMAGE, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_HUNTER	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_RANGER	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_CLOWN		, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_ASSASSIN	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_SHAMAN	, iClassLimit, iUserClassLimit)){ return false; }
			if(CheckClassLimit(UCLIMIT_COMMON_DOUBLE_FIGHTER	, iClassLimit, iUserClassLimit)){ return false; }
		}
	}

	return true;
}

bool PgPvPRoom::SetJoinEmptySlot( PgPlayer const *pkPlayer, int const iTeam, bool const bSendUnit )
{
	CONT_PVP_SLOT::iterator slot_itr = m_kContSlot_Empty.begin();
	for ( ; slot_itr!=m_kContSlot_Empty.end(); ++slot_itr )
	{
		if ( !iTeam || (slot_itr->GetTeam() == iTeam) )
		{
			std::pair<ContUserSlot::iterator, bool> kPair = m_kContSlot_User.insert(std::make_pair( pkPlayer->GetID(), ContUserSlot::mapped_type( pkPlayer, m_kContSlot_User.empty() ? PS_MASTER : PS_PNONE ) ) );
			if ( !kPair.second )
			{
				if ( kPair.first->second.byStatus & PS_NOTCHANGE_TEAM_FALG )
				{// ·№µр»уЕВАМ°ЕіЄ ЗГ·№АМ »уЕВ¶уёй єЇ°жЗПБц ёшЗСґЩ.
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if ( kPair.first->second.kTeamSlot.GetTeam() == iTeam )
				{// °°ґЩёй ±іИЇЗТ ЗКїд°Ў ѕшґЩ.
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				// єу ЅЅ·ФАё·О №ЭИЇЗШБЦ°н.
				m_kContSlot_Empty.insert( kPair.first->second.kTeamSlot );
			}

			// Б¤єёё¦ ±іГјЗСґЩ.
			kPair.first->second.kTeamSlot = *slot_itr;

			m_kBasicInfo.m_ucNowUser = (BYTE)m_kContSlot_User.size();
			m_kContSlot_Empty.erase(slot_itr);

			if ( ROOM_STATUS_LOBBY == m_kBasicInfo.m_kStatus )
			{
				// °ФАУБЯАП ¶§ґВ.... і­АФАМ±в ¶§№®їЎ ѕЖБч ЅЅ·ФїЎ µйѕоїВ °Е ±оБцґВ ЕлєёЗПёй ѕИµИґЩ.
				// °ФАУБЯАМ ѕЖґП°н ·л »уЕВїЎј­ёё Елєёё¦ ЗШБЦАЪ.
				// ё¶Вщ°ЎБц·О °ФАУБЯАМ ѕЖґТ ¶§ґВ Б¤єё ±іГјµµ ѕИµИґЩ.	
				BM::Stream kPacket( PT_T_C_ANS_TEAM_CHANGE, pkPlayer->GetID() );
				kPair.first->second.WriteToPacket(kPacket);
				BroadCast( kPacket, TEAM_NONE, PS_EMPTY, (bSendUnit ? BM::GUID::NullData() : pkPlayer->GetID() ) );

				if ( m_kContSlot_User.size() == 1 )
				{
					SetMaster( pkPlayer->GetID() );
				}
			}
			else
			{
				if ( true == bSendUnit )
				{
					BM::Stream kPacket( PT_T_C_ANS_TEAM_CHANGE, pkPlayer->GetID() );
					kPair.first->second.WriteToPacket(kPacket);
					::SendToUser( pkPlayer->GetMemberGUID(), pkPlayer->GetSwitchServer(), kPacket );
				}
			}

			return true;
		}
	}

	return false;
}

EPvPRoomJoinRet PgPvPRoom::JoinEmptySlot( PgPlayer const *pkPlayer, int iTeam, bool bSendUnit )
{
	if ( iTeam && m_kBasicInfo.m_kMode == PVP_MODE_PERSONAL && !IsLeague() )
	{
		// °іАОАьАП °жїмїЎ ЖА±іГј єТ°Ў
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PVP_JOIN_FULL_SLOT"));
		return PVP_JOIN_FULL_SLOT;
	}

	if ( true == IsBalance() && LOCAL_MGR::NC_KOREA != g_kLocal.ServiceRegion() )
	{//ЗС±№АП °жїмґВ є§·±Ѕє ГјЕ© ЗПБц ё»АЪ
		// ·©Е·ёрµеїЎј­ґВ є§·±Ѕєё¦ ГјЕ©ЗШѕЯ ЗСґЩ.
		// °Л»зё¦ ЗПАЪ
		int const iClass = pkPlayer->GetAbil( AT_CLASS );
		if ( !iTeam )
		{
			while ( ++iTeam < TEAM_MAX )
			{
				if ( true == IsCheckBalanceJoin( iClass, iTeam ) )
				{
					if ( true == SetJoinEmptySlot( pkPlayer, iTeam, bSendUnit ) )
					{
						return PVP_JOIN_SUCCEED;
					}
				}
			}

			if ( TEAM_MAX == iTeam )
			{
				return PVP_JOIN_BALANCECLASS;
			}
		}
		else
		{
			if ( !IsCheckBalanceJoin( iClass, iTeam ) )
			{
				return PVP_JOIN_BALANCECLASS;
			}
		}
	}

	if ( true == SetJoinEmptySlot( pkPlayer, iTeam, bSendUnit ) )
	{
		return PVP_JOIN_SUCCEED;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PVP_JOIN_FULL_SLOT"));
	return PVP_JOIN_FULL_SLOT;
}

HRESULT PgPvPRoom::ClearEntry( BM::GUID const &kCharGuid )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kBasicInfo.m_kStatus != ROOM_STATUS_LOBBY )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
	if( m_kContSlot_User.end() == slot_itr)
	{//АЇАъё¦ ГЈА»јц°Ў ѕшјТ.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if( !slot_itr->second.byEntryNo )
	{//їЈЖ®ё® µо·ПµыА§ ѕИЗЯАЅї©
		return S_OK;
	}

	SPvPTeamEntry kPrevEntry(slot_itr->second.kTeamSlot.GetTeam(), slot_itr->second.byEntryNo);
	SPvPTeamEntry kNewEntry(slot_itr->second.kTeamSlot.GetTeam(), 0);
	CONT_PVP_ENTRY::iterator entry_itr = m_kContEntry_Empty.find(kPrevEntry);
	if( m_kContEntry_Empty.end() == entry_itr )
	{//єу їЈЖ®ё®·О ГЯ°ЎЗП°н ГК±вИ­
		m_kContEntry_Empty.insert( kPrevEntry );
		slot_itr->second.byEntryNo = kNewEntry.GetEntry();

		RefreshEntry();

		BM::Stream kAnsPacket( PT_T_C_ANS_ENTRY_CHANGE );
		kAnsPacket.Push( slot_itr->second.kTeamSlot.GetTeam() );
		kAnsPacket.Push( slot_itr->second.kTeamSlot.GetSlot() );
		kPrevEntry.WriteToPacket(kAnsPacket);
		kNewEntry.WriteToPacket(kAnsPacket);
		BroadCast( kAnsPacket, kNewEntry.GetTeam() );

		return S_OK;
	}
	else
	{//АМ№М µо·ПµИ їЈЖ®ё®АУ. ±ЩµҐ ·ОБч»у АП·О їАёй №є°Ў ЗСВь АЯёшµИ°ЗµҐ..
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	return false;
}

HRESULT PgPvPRoom::Exit( BM::GUID const &kCharGuid, bool const bDisConnected )
{
	BM::CAutoMutex kLock(m_kMutex);

	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
	if ( slot_itr != m_kContSlot_User.end() )
	{
		PgPlayer const * pkPlayer = slot_itr->second.pkPlayer;

		// °ФАУБЯїЎ іЄ°Ўёй ЖР№и·О ±в·П
		if ( slot_itr->second.byStatus & PS_PLAYING )
		{
//			++(slot_itr->second.kNowRoomReport.m_iLose);
/*
			PgLogWrapper kLog(LOG_CONTENTS_PVP_USER_RESULT);
			kLog.MemberKey( rkPvPUser.kMemberGuid );
			kLog.CharacterKey( rkPvPUser.kCharacterGuid );
			kLog.Name( rkPvPUser.wstrName );
			kLog.Push( m_kGameGround.Guid() );
			kLog.Push( (int)m_kGameGround.GroundNo() );
			kLog.Push( (int)GetNo().kKey );
			kLog.Push( (m_kBasicInfo.m_kMode == PVP_MODE_PERSONAL) ? (__int64)LOG_CONTENTS_PVP_USER_RESULT__OUT_PERSONAL : (__int64)LOG_CONTENTS_PVP_USER_RESULT__OUT_TEAM );
			kLog.Push( slot_itr->second.GetWin(false) );
			kLog.Push( slot_itr->second.GetLost(false) );
			kLog.Push( slot_itr->second.GetDraw(false) );
			kLog.Send();
*/
		}
		else
		{//ґл±вБЯїЎ іЄ°¬Аёёй їЈЖ®ё® ГК±вИ­ µИ°Е ѕЛ·ББЬ.
			BM::Stream kAnsPacket( PT_T_C_ANS_ENTRY_CHANGE );
			kAnsPacket.Push( slot_itr->second.kTeamSlot.GetTeam() );
			kAnsPacket.Push( slot_itr->second.kTeamSlot.GetSlot() );
			SPvPTeamEntry kEntry(slot_itr->second.kTeamSlot.GetTeam(), slot_itr->second.byEntryNo);
			kEntry.WriteToPacket(kAnsPacket);
			kEntry.SetEntry(0);
			kEntry.WriteToPacket(kAnsPacket);
			BroadCast( kAnsPacket, kEntry.GetTeam() );
		}

		bool const bChangeMaster = IsMaster(kCharGuid);

		// і­АФЗШј­ ±Ч¶уїоµеїЎ АФАеЗПБц ѕК°н №жїЎёё АЦґЩ°Ў іЄ°Ј АЇАъґВ ЕлєёЗПБц ѕКґВґЩ.
		bool const bNfy = ( !IsPlaying() || (slot_itr->second.byStatus & PS_PLAYING) );

		if( slot_itr->second.byEntryNo )
		{
			m_kContEntry_Empty.insert( SPvPTeamEntry(slot_itr->second.kTeamSlot.GetTeam(), slot_itr->second.byEntryNo) );
		}
		m_kContSlot_Empty.insert(slot_itr->second.kTeamSlot);
		m_kContSlot_User.erase( slot_itr );// slot_itr БцїьґЩ...

		if ( m_kContSlot_User.empty() && PvP_Lobby_GroundNo_League != m_iLobbyID )
		{
			BM::Stream kPacket( PT_ROOM_TO_LOBBY_REMOVED, GetIndex() );
			SendToPvPLobby( kPacket, m_iLobbyID );

			Clear();
		}
		else
		{
			m_kBasicInfo.m_ucNowUser = (BYTE)m_kContSlot_User.size();

			switch ( m_kBasicInfo.m_kStatus )
			{
			case ROOM_STATUS_READY:
				{
					SetStatus( ROOM_STATUS_READY_EXITUSER );
				}break;
			case ROOM_STATUS_READY_EXITUSER:
				{
					// ¶ЗіЄ°¬АёґП±о ѕЖ№«°Нµµ ЗТЗКїдґВ ±»АМ ѕшіЧ.
				}break;
			case ROOM_STATUS_PLAY:
				{// ЗГ·№АМБЯїЎ іЄ°¬Аёёй MapServer·О ЕлєёЗШ БЦѕоѕЯ ЗСґЩ.
					if ( bDisConnected )
					{
						BM::Stream kDisPacket( PT_T_M_NFY_EXIT_ROOM, kCharGuid );
						g_kServerSetMgr.Locked_SendToGround( m_kGameGround, kDisPacket, true );
					}
				}// No Break
			default:
				{
					if ( bNfy && !m_kContSlot_User.empty() )
					{
						BM::Stream kPacket( PT_T_C_NFY_EXIT_ROOM_USER, kCharGuid );
						kPacket.Push(bChangeMaster);
						if ( bChangeMaster )
						{
							// №жАеАМёй №жАеА» ±іГјЗШѕЯ ЗСґЩ.
							BM::GUID const &kNewMasterCharGuid = m_kContSlot_User.begin()->first;
							SetMaster( kNewMasterCharGuid );
							kPacket.Push( kNewMasterCharGuid );
						}
						BroadCast( kPacket, TEAM_NONE, PS_EMPTY, kCharGuid );
					}

					SendModifyed();
				}break;
			}
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::SetSlotStatus( BM::GUID const &kCharGuid, BM::Stream *pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kBasicInfo.m_kStatus != ROOM_STATUS_LOBBY )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if( IsLeague() )
	{//ё®±ЧАьїЎј­ґВ ЅЅ·Ф ї­°н ґЭ°н ёшЗСґЩ.
		return E_FAIL;
	}

	if ( IsMaster(kCharGuid) )
	{
		SPvPTeamSlot kSlot;
		bool bOpen = false;
		pkPacket->Pop( bOpen );
		kSlot.ReadFromPacket( *pkPacket );

		if ( true == bOpen )
		{
			if ( !kSlot.IsCorrect( IsExercise() ) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			ContUserSlot::iterator slot_itr = m_kContSlot_User.begin();
			for ( ; slot_itr!=m_kContSlot_User.end(); ++slot_itr )
			{
				if ( slot_itr->second.kTeamSlot == kSlot )
				{
					// »зїлБЯАО ЅЅ·ФА» ї­ јцґВ ѕшґЩ.
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}

			// »зїлБЯАО SlotАМ ѕЖґП№З·О SlotА» ёёµйѕо БЦАЪ.
			std::pair<CONT_PVP_SLOT::iterator, bool> kPair = m_kContSlot_Empty.insert(kSlot);
			if ( true == kPair.second )
			{
				BM::Stream kAnsPacket( PT_T_C_ANS_SLOTSTATUS_CHANGE, bOpen );
				kSlot.WriteToPacket( kAnsPacket );

				if ( true == IsBalance() )
				{// є§·±Ѕє ёрµеїЎј­ґВ µО°іё¦ µїЅГїЎ ёёµйѕоѕЯ ЗСґЩ.
					SPvPTeamSlot kSlot2 = kSlot;
					kSlot2.SetEnemySlot();

					kPair = m_kContSlot_Empty.insert(kSlot2);
					if ( true == kPair.second )
					{
						++m_kBasicInfo.m_ucMaxUser;
					}

					kSlot2.WriteToPacket( kAnsPacket );
				}

				kAnsPacket.Push(++m_kBasicInfo.m_ucMaxUser);
				BroadCast( kAnsPacket, 0, PS_EMPTY );

				SendModifyed();
				return S_OK;
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		else
		{
			if ( 0 < m_kContSlot_Empty.erase(kSlot) )
			{
				BM::Stream kAnsPacket( PT_T_C_ANS_SLOTSTATUS_CHANGE, bOpen );
				kSlot.WriteToPacket( kAnsPacket );

				if ( true == IsBalance() )
				{// є§·±Ѕє ёрµеїЎј­ґВ µО°іё¦ µїЅГїЎ БцїцѕЯ ЗСґЩ.
					SPvPTeamSlot kSlot2 = kSlot;
					kSlot2.SetEnemySlot();

					if ( !m_kContSlot_Empty.erase(kSlot2) )
					{
						m_kContSlot_Empty.insert( kSlot );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
						return E_FAIL;
					}

					--m_kBasicInfo.m_ucMaxUser;
					kSlot2.WriteToPacket( kAnsPacket );
				}

				kAnsPacket.Push(--m_kBasicInfo.m_ucMaxUser);
				BroadCast( kAnsPacket, 0, PS_EMPTY );

				SendModifyed();
				return S_OK;
			}

			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	INFO_LOG(BM::LOG_LV5, __FL__ << _T("No Room Master CharcterGuid=") << kCharGuid);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
	return E_ACCESSDENIED;
}

HRESULT PgPvPRoom::EntryChange( BM::GUID const &kCharGuid, BM::Stream *pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kBasicInfo.m_kStatus != ROOM_STATUS_LOBBY )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	SPvPTeamEntry kNewEntry;
	kNewEntry.ReadFromPacket( *pkPacket );

	CONT_PVP_ENTRY::iterator entry_itr = m_kContEntry_Empty.find(kNewEntry);
	if( m_kContEntry_Empty.end() != entry_itr )
	{//єу їЈЖ®ё®ґЩ
		ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
		if( m_kContSlot_User.end() == slot_itr)
		{//АЇАъё¦ ГЈА»јц°Ў ѕшјТ.
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		if( slot_itr->second.byEntryNo == kNewEntry.GetEntry() )
		{//°°АєµҐ ёЦ №ЩІЮ??
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		SPvPTeamEntry kPrevEntry(slot_itr->second.kTeamSlot.GetTeam(), slot_itr->second.byEntryNo);
		if( slot_itr->second.byEntryNo )
		{//їЈЖ®ё®°Ў АЦґшірАє АЪЅЕАЗ їЈЖ®ё®ґВ єу їЈЖ®ё®·О..
			m_kContEntry_Empty.insert( kPrevEntry );
		}

		slot_itr->second.byEntryNo = entry_itr->GetEntry();
		m_kContEntry_Empty.erase(entry_itr);

		RefreshEntry();

		BM::Stream kAnsPacket( PT_T_C_ANS_ENTRY_CHANGE );
		kAnsPacket.Push( slot_itr->second.kTeamSlot.GetTeam() );
		kAnsPacket.Push( slot_itr->second.kTeamSlot.GetSlot() );
		kPrevEntry.WriteToPacket(kAnsPacket);
		kNewEntry.WriteToPacket(kAnsPacket);
		BroadCast( kAnsPacket, kNewEntry.GetTeam() );

		return S_OK;
	}
	else
	{//АМ№М µо·ПµИ їЈЖ®ё®АУ ¤·¤·
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	return E_FAIL;
}

HRESULT PgPvPRoom::EntrustMaster( BM::GUID const &kMasterCharGuid, BM::GUID const &kTargetCharGuid )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kBasicInfo.m_kStatus != ROOM_STATUS_LOBBY )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( IsMaster( kMasterCharGuid ) )
	{
		if ( SUCCEEDED( SetMaster( kTargetCharGuid ) ) )
		{
			BM::Stream kAnsPacket( PT_T_C_ANS_ENTRUST_MASTER, kTargetCharGuid );
			kAnsPacket.Push( kMasterCharGuid );
			BroadCast( kAnsPacket );
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::Modify( BM::GUID const &kCharGuid, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( m_kBasicInfo.m_kStatus != ROOM_STATUS_LOBBY )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if ( IsMaster(kCharGuid) )
	{
		ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
		if ( slot_itr != m_kContSlot_User.end() )
		{
			bool bBasic = true;
			if ( pkPacket->Pop(bBasic) )
			{
				BM::Stream kAnsPacket( PT_T_C_ANS_MODIFY_ROOM, bBasic );

				if ( bBasic )
				{
					SPvPRoomBasicInfo kBasicInfo;
					kBasicInfo.ReadFromPacket(*pkPacket);

					HRESULT hRet = Check( slot_itr->second.pkPlayer, kBasicInfo, m_kExtInfo );
					if ( SUCCEEDED(hRet) )
					{
						bool const bRefreshSlot = m_kBasicInfo.m_kMode != kBasicInfo.m_kMode;

						m_kBasicInfo.SetRoomInfo(kBasicInfo);
						m_kBasicInfo.WriteToPacket(kAnsPacket);

						if ( S_FALSE == hRet )
						{
							kAnsPacket.Push(true);
							m_kExtInfo.WriteToPacket(kAnsPacket);
						}
						else
						{
							kAnsPacket.Push(false);
						}

						kAnsPacket.Push(bRefreshSlot);
						if ( bRefreshSlot )
						{
							RefreshSlot();
							PU::TWriteTable_AM( kAnsPacket, m_kContSlot_User );
							PU::TWriteKey_A( kAnsPacket, m_kContSlot_Empty );
						}

						BroadCast( kAnsPacket );

						SendModifyed();
						return S_OK;
					}
					else
					{
						m_kBasicInfo.WriteToPacket(kAnsPacket);
					}
				}
				else
				{
					SPvPRoomExtInfo kExtInfo;
					kExtInfo.ReadFromPacket(*pkPacket);

					if ( SUCCEEDED( Check( slot_itr->second.pkPlayer, m_kBasicInfo, kExtInfo ) ) )
					{
						m_kExtInfo = kExtInfo;
						m_kBasicInfo.m_bPwd = !m_kExtInfo.strPassWord.empty();
						m_kExtInfo.WriteToPacket(kAnsPacket);
						BroadCast( kAnsPacket );

						/*
						// ЗЪµрДёАМ №ЩІ»ґЩёй
						if ( bUseHandyCap != m_kExtInfo.bUseHandyCap )
						{
							if ( !m_kExtInfo.bUseHandyCap )
							{	// ЗЪµрДёА» »зїлЗПБц ѕКґВґЩёй ёрµз ЗЪµрДёА» ГК±вИ­ ЅГДС БЦѕоѕЯ ЗСґЩ.
								// Е¬¶уАМѕрЖ®ґВ Е¬¶у°Ў ЅєЅє·О ЗПАЪ.
								ContUserSlot::iterator slot_itr;
								for ( slot_itr=m_kContSlot_User.begin(); slot_itr!=m_kContSlot_User.end(); ++slot_itr )
								{
									slot_itr->second.iHandyCap = 100;
								}
							}
						}
						*/
						return S_OK;
					}
					else
					{
						m_kExtInfo.WriteToPacket(kAnsPacket);
					}
				}
				::SendToUser( slot_itr->second.pkPlayer->GetMemberGUID(), slot_itr->second.pkPlayer->GetSwitchServer(), kAnsPacket );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!! MasterGuid[") << m_kMasterGuid << _T("] Not Found Slot") );
	}

	INFO_LOG(BM::LOG_LV5, __FL__ << _T(" No Room Master CharcterGuid=") << kCharGuid);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
	return E_ACCESSDENIED;
}

HRESULT PgPvPRoom::KickUser( BM::GUID const &kReqCharGuid, BM::GUID const &kKickCharGuid )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( IsMaster(kReqCharGuid) && (kReqCharGuid != kKickCharGuid) )
	{
		ContUserSlot::iterator master_itr = m_kContSlot_User.find( kReqCharGuid );
		if ( master_itr != m_kContSlot_User.end() )
		{
			ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kKickCharGuid );
			if ( slot_itr != m_kContSlot_User.end() )
			{
				switch( g_kLocal.ServiceRegion() )
				{
				case LOCAL_MGR::NC_SINGAPORE:
				case LOCAL_MGR::NC_THAILAND:
				case LOCAL_MGR::NC_INDONESIA:
				case LOCAL_MGR::NC_VIETNAM:
				case LOCAL_MGR::NC_RUSSIA:
				case LOCAL_MGR::NC_PHILIPPINES:
					{
						// ЅМ°ЎЖъАє №«Б¶°З Kick °ЎґЙ
					}break;
				default:
					{
						if ( PS_READY & slot_itr->second.byStatus )
						{
							// БШєсїП·бЗС Вь°ЎАЪґВ ГЯ№ж ЗПЅЗ јц ѕшЅАґПґЩ.
							SendWarnMessage( master_itr->second.pkPlayer, 400404, EL_PvPMsg );
							return E_FAIL;
						}
					}break;
				}
				
				// Бцїц
				m_kContSlot_Empty.insert(slot_itr->second.kTeamSlot);
				m_kContSlot_User.erase( slot_itr );// slot_itr БцїьґЩ...
				m_kBasicInfo.m_ucNowUser = static_cast<BYTE>(m_kContSlot_User.size());
				SendModifyed();

				BM::Stream kNfyPacket( PT_T_T_REQ_KICK_ROOM_USER, kKickCharGuid );
				SendToCenter( kNfyPacket );

				BM::Stream kClientPacket( PT_T_C_NFY_EXIT_ROOM_USER, kKickCharGuid );
				kClientPacket.Push(false);
				BroadCast( kClientPacket, TEAM_NONE, PS_EMPTY, kKickCharGuid );
				return S_OK;
			}

			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!! MasterGuid[") << m_kMasterGuid << _T("] Not Found Slot"));
		}
	}

	INFO_LOG(BM::LOG_LV5, __FL__ << _T("No Room Master CharcterGuid=") << kReqCharGuid );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
	return E_ACCESSDENIED;
}

HRESULT PgPvPRoom::SetMaster( BM::GUID const &kCharGuid )
{
	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
	if ( slot_itr != m_kContSlot_User.end() )
	{
		if ( BM::GUID::IsNotNull(m_kMasterGuid) )
		{
			ContUserSlot::iterator slot_itr2 = m_kContSlot_User.find( m_kMasterGuid );
			if ( slot_itr2 != m_kContSlot_User.end() )
			{
				slot_itr2->second.byStatus &= (~PS_MASTER);
			}
		}

		slot_itr->second.byStatus |= PS_MASTER;
		slot_itr->second.byStatus &= (~PS_READY);
		m_kMasterGuid = kCharGuid;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPRoom::BroadCast( BM::Stream const &kPacket, int const iTeam, BYTE const kFilterStatus, BM::GUID const &kRemoveCharGuid )
{
	BM::CAutoMutex kLock(m_kMutex);

	ContUserSlot::iterator slot_itr;
	for ( slot_itr=m_kContSlot_User.begin(); slot_itr!=m_kContSlot_User.end(); ++slot_itr )
	{
		ContUserSlot::mapped_type &kElement = slot_itr->second;
		if ( !iTeam || ( kElement.kTeamSlot.GetTeam() == iTeam ) )
		{
			if ( 0 == (kFilterStatus & kElement.byStatus) )
			{
				if ( kRemoveCharGuid != slot_itr->first )
				{
					::SendToUser( kElement.pkPlayer->GetMemberGUID(), kElement.pkPlayer->GetSwitchServer(), kPacket );
				}
			}
		}
	}
}

/*
HRESULT PgPvPRoom::ChangeHandyCap( BM::GUID const &kReqCharGuid, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);

	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kReqCharGuid );
	if ( slot_itr != m_kContSlot_User.end() )
	{
		BM::Stream kPacket( PT_T_C_NFY_CHANGE_HANDYCAP, kReqCharGuid );
		if ( m_kExtInfo.bUseHandyCap && !(slot_itr->second.byStatus & PS_READY) )
		{
			int iHandyCap = 0;
			pkPacket->Pop( iHandyCap );

			// Е¬¶уАМѕрЖ®ЗСЕЧґВ №йєРАІ·О №ЮґВґЩ.
			iHandyCap = __max( 1, iHandyCap );
			iHandyCap = __min( 200, iHandyCap );
			slot_itr->second.iHandyCap = iHandyCap;

			kPacket.Push(iHandyCap);
			BroadCast( kPacket, TEAM_NONE, PS_NONE, kReqCharGuid );
			return S_OK;
		}

		// №ЩІЬ јц ѕшґЩ..(№ЩІЬјц ѕшАёґП±о їш·Ў°НА» ґЩЅГ ѕЛ·ББЦѕоѕЯ ЗШ.
		kPacket.Push( slot_itr->second.iHandyCap );
		::SendToUser( slot_itr->second.rkLobbyUser.kMemberGuid, slot_itr->second.rkLobbyUser.kSwitchServer, kPacket );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_ACCESSDENIED"));
		return E_ACCESSDENIED;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}
*/

bool PgPvPRoom::SetStatus( EPvPRoomStatus const kStatus )
{
	switch( m_kBasicInfo.m_kStatus )
	{
	case ROOM_STATUS_CLOSE:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Old Status[CLOSE] New Status[") << (BYTE)kStatus << _T("] Error!!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	case ROOM_STATUS_RESULT:
	case ROOM_STATUS_READY_EXITUSER:
		{
			if ( ROOM_STATUS_LOBBY == kStatus )
			{
				m_kBasicInfo.m_kStatus = kStatus;
				return true;
			}
		}break;
	case ROOM_STATUS_READY:
		{
			if ( ROOM_STATUS_READY_EXITUSER == kStatus )
			{
				m_kBasicInfo.m_kStatus = kStatus;
				return true;
			}
		}// No Break
	default:
		{
			if ( (kStatus >> 1) == m_kBasicInfo.m_kStatus )
			{
				m_kBasicInfo.m_kStatus <<= 1;
				return true;
			}
		}break;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Old Status[") << m_kBasicInfo.m_kStatus << _T("] New Status[") << (int)kStatus << _T("] Error!!!"));
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPRoom::GetBattleGuid(BM::GUID& rkOut)
{
	BM::CAutoMutex kLock(m_kMutex);

	if( m_kBattleGuid.IsNotNull() )
	{
		rkOut = m_kBattleGuid;
		return true;
	}

	return false;
}

int	PgPvPRoom::GetTeamUserCount(int& iRedTeamCount, int& iBlueTeamCount)const
{
	iRedTeamCount = iBlueTeamCount = 0;
	ContUserSlot::const_iterator slot_itr = m_kContSlot_User.begin();
	while( m_kContSlot_User.end() != slot_itr )
	{
		ContUserSlot::mapped_type const &kElement = slot_itr->second;
		if ( kElement.kTeamSlot.GetTeam() == TEAM_RED )
		{
			++iRedTeamCount;
		}
		else
		{
			++iBlueTeamCount;
		}
		++slot_itr;
	}

	return m_kContSlot_User.size();
}

HRESULT PgPvPRoom::AutoReady()
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( IsPlaying() || m_kBattleGuid.IsNull() ||  m_kContSlot_User.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	int iUserCount = 0, iRedTeamCount = 0, iBlueTeamCount = 0;
	iUserCount = GetTeamUserCount(iRedTeamCount, iBlueTeamCount);
	
	if( 1 == m_kContSlot_User.size() )
	{//ЗСёнАМ¶уёй ±ЧЖААМ АМ±д°ЕАУ
		return S_FALSE;
	}

	if( 0 == iRedTeamCount
		|| 0 == iRedTeamCount )
	{//ЗСЖААМ ѕИїФіЧ
		return S_FALSE;
	}

	DWORD dwGameTime = (DWORD)m_kBasicInfo.m_iGameTime * 60000;
	HRESULT hRet = Check( m_kBasicInfo, m_kExtInfo );
	if ( FAILED(hRet) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if( !m_kContEntry_Empty.empty() )
	{// їЈЖ®ё®ё¦ Б¤ЗПБц ёшЗС АЇАъ°Ў АЦАёёй АЪµїАё·О °с¶у

	}

	m_kBasicInfo.m_kStatus = ROOM_STATUS_READY;

	// АЇАъµйЗСЕЧ ѕЛ·ББШґЩ.
	BM::Stream kClientPacket( PT_M_C_NFY_GAME_WAIT, m_kBasicInfo.m_kType );
	BroadCast( kClientPacket );

	// ёёµйѕо БЦЅГїНїл~~
	// АМ№М »зїл ЗП°н АЦґВ ±Ч¶уїоµе°Ў АЦАёёй ѕЛѕЖј­ ·№µр ЅГЕІґЩ.
	m_kGameGround.GroundNo(m_kBasicInfo.m_iGndNo);
	m_kGameGround.Guid(m_kRoomGuid);

	ReqCreateGround();

	return S_OK;
}

int PgPvPRoom::GetAutoWinTeam()
{
	int iRedTeamCount = 0;
	int iBlueTeamCount = 0;

	ContUserSlot::const_iterator slot_itr = m_kContSlot_User.begin();
	while( m_kContSlot_User.end() != slot_itr )
	{
		ContUserSlot::mapped_type const &kElement = slot_itr->second;
		if ( kElement.kTeamSlot.GetTeam() == TEAM_RED )
		{
			++iRedTeamCount;
		}
		else
		{
			++iBlueTeamCount;
		}
		++slot_itr;
	}

	if( iRedTeamCount > iBlueTeamCount )
	{
		return TEAM_RED;
	}
	else
	{
		return TEAM_BLUE;
	}
}

HRESULT PgPvPRoom::Ready( BM::GUID const &kCharGuid, BM::Stream * const pkPacket  )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( IsPlaying() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ContUserSlot::iterator slot_itr = m_kContSlot_User.find( kCharGuid );
	if ( slot_itr != m_kContSlot_User.end() )
	{
		PgPlayer const * pkPlayer = slot_itr->second.pkPlayer;

		if ( IsMaster(kCharGuid) )
		{// №жАеАМёй °ФАУА» ЅГАЫЗСґЩ.

			DWORD dwGameTime = (DWORD)m_kBasicInfo.m_iGameTime * 60000;

			SPvPRoomBasicInfo kBasicInfo;
			kBasicInfo.ReadFromPacket(*pkPacket);

			HRESULT hRet = Check( slot_itr->second.pkPlayer, kBasicInfo, m_kExtInfo );
			if ( FAILED(hRet) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
			m_kBasicInfo.SetRoomInfo(kBasicInfo);

			// ёрµО ·№µр »уЕВАОБц °Л»зЗПАЪ.
			int iRedTeamCount = 0;
			int iBlueTeamCount = 0;
			for( slot_itr=m_kContSlot_User.begin(); slot_itr!=m_kContSlot_User.end(); ++slot_itr )
			{
				ContUserSlot::mapped_type const &kElement = slot_itr->second;
				if ( !(kElement.byStatus & PS_POSSIBLESTART_AND_FLAG ) )
				{
					// ёрµО ·№µр »уЕВАМї©ѕЯБцёё °ФАУ ЅГАЫАМ °ЎґЙЗХґПґЩ.
					SendWarnMessage( pkPlayer, 400401, EL_PvPMsg );

					// ·№µрё¦ ЗШѕЯ ЗСґЩ.
					BM::Stream kPlzRdyPacket(PT_M_C_NFY_WARN_MESSAGE, (int)400402 );
					kPlzRdyPacket.Push(std::wstring());
					BroadCast( kPlzRdyPacket, TEAM_NONE, PS_READY, kCharGuid );
					return S_FALSE;
				}

				if ( kElement.kTeamSlot.GetTeam() == TEAM_RED )
				{
					++iRedTeamCount;
				}
				else
				{
					++iBlueTeamCount;
				}
			}

			switch ( m_kBasicInfo.m_kMode )
			{
			case PVP_MODE_TEAM:
				{
					if ( !iRedTeamCount || !iBlueTeamCount )
					{
						// АыАМ ѕшѕој­ °ФАУЅГАЫА» ёшЗСґЩ.
						SendWarnMessage( pkPlayer, 200100, EL_PvPMsg );
						return S_FALSE;
					}

					if (	iRedTeamCount != iBlueTeamCount
						&&	IsBalance()
						)
					{
						// є§·±Ѕє°Ў ёВБц ѕКѕЖ °ФАУЅГАЫА» ёшЗСґЩ.
						SendWarnMessage( pkPlayer, PVP_JOIN_MIN_TEAM, EL_PvPMsg );
						return S_FALSE;
					}
					
					if( IsExercise() && PVP_TYPE_LOVE == Mode() )
					{// ·ЇєкґлАЫАьАє ї¬ЅАёрµеїЎј­µµ 2:2 АМ»уАМѕоѕЯ ЗСґЩ.
						if((iRedTeamCount < 2) || (iBlueTeamCount < 2))
						{
							SendWarnMessage( pkPlayer, PVP_JOIN_MINIMUM_TEAM, EL_PvPMsg );
							return S_FALSE;
						}
					}

					if( ((iRedTeamCount < 2) || (iBlueTeamCount < 2)) && IsBalance() )
					{
						// АОїшјц°Ў єОБ·ЗХґПґЩ.(ГЦјТ 2ЖАѕї °ж±вё¦ ЗШѕЯµИґЩ.)
						SendWarnMessage( pkPlayer, PVP_JOIN_MINIMUM_TEAM, EL_PvPMsg );
						return S_FALSE;
					}
				}break;
			case PVP_MODE_PERSONAL:
				{
					if ( (iRedTeamCount + iBlueTeamCount) < 2 )
					{
						// АыАМ ѕшѕој­ °ФАУЅГАЫА» ёшЗСґЩ.
						SendWarnMessage( pkPlayer, 200100, EL_PvPMsg );
						return S_FALSE;
					}
				}break;
			}	

			switch ( m_kBasicInfo.m_kType )
			{
			case PVP_TYPE_WINNERS:
			case PVP_TYPE_WINNERS_TEAM:
			case PVP_TYPE_ACE:
				{
					if( !m_kContEntry_Empty.empty() )
					{// їЈЖ®ё®ё¦ Б¤ЗПБц ёшЗС АЇАъ°Ў АЦѕој­ °ФАУЅГАЫА» ёшЗСґЩ.
						SendWarnMessage( pkPlayer, 200100, EL_PvPMsg );
						return S_FALSE;
					}
				}break;
			default: { }break;
			}

			m_kBasicInfo.m_kStatus = ROOM_STATUS_READY;

			// АЇАъµйЗСЕЧ ѕЛ·ББШґЩ.
			BM::Stream kClientPacket( PT_M_C_NFY_GAME_WAIT, m_kBasicInfo.m_kType );
			BroadCast( kClientPacket );

			// ёёµйѕо БЦЅГїНїл~~
			// АМ№М »зїл ЗП°н АЦґВ ±Ч¶уїоµе°Ў АЦАёёй ѕЛѕЖј­ ·№µр ЅГЕІґЩ.
			m_kGameGround.GroundNo(m_kBasicInfo.m_iGndNo);
			m_kGameGround.Guid(m_kRoomGuid);

			ReqCreateGround();
		}
		else
		{
			// №жАеАМ ѕЖґПёй
			switch ( m_kBasicInfo.m_kType )
			{
			case PVP_TYPE_WINNERS:
			case PVP_TYPE_WINNERS_TEAM:
			case PVP_TYPE_ACE:
				{
					if( !(slot_itr->second.byEntryNo) )
					{//·№µрё¦ ЗТ¶уёй їЈЖ®ё®єОЕН Б¤ЗШѕЯ ЗСґЩ
						SendWarnMessage( pkPlayer, 200100, EL_PvPMsg );
						return S_FALSE;
					}
				}break;
			default: { }break;
			}
			if ( ROOM_STATUS_LOBBY == m_kBasicInfo.m_kStatus )
			{
				if ( slot_itr->second.byStatus & PS_READY )
				{
					slot_itr->second.byStatus &= (~PS_READY);
				}
				else
				{
					slot_itr->second.byStatus |= PS_READY;
				}

				BM::Stream kPacket(PT_T_C_NFY_USER_STATUS_CHANGE, kCharGuid );
				kPacket.Push( slot_itr->second.byStatus );
				BroadCast( kPacket );
			}
			else
			{
				if ( slot_itr->second.byStatus & PS_STARTGAME_AND_FLAG )
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Player Status is Bad, Character=") << pkPlayer->Name() << _COMMA_ << pkPlayer->GetID() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
				else
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
			}
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::Start( SGroundKey const &kGndKey, bool bSuccess )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( m_kGameGround == kGndKey )
	{
		switch ( m_kBasicInfo.m_kStatus )
		{
		case ROOM_STATUS_READY:
			{
				if ( bSuccess )
				{
					__int64 i64StartGameUser = 0i64;//·О±Ч±в·ПїлАМґЩ.
					VEC_GUID kUnitGuidList;
					kUnitGuidList.reserve( m_kContSlot_User.size() );

					ContUserSlot::iterator slot_itr = m_kContSlot_User.begin();
					for ( ; slot_itr!=m_kContSlot_User.end(); ++slot_itr )
					{
						ContUserSlot::mapped_type &kElement = slot_itr->second;
						if ( (kElement.byStatus & PS_POSSIBLESTART_AND_FLAG) )
						{	
							kUnitGuidList.push_back( slot_itr->first );
							kElement.byStatus |= PS_STARTGAME_AND_FLAG;
							++i64StartGameUser;
						}
					}

					DWORD dwGameTime = (DWORD)m_kBasicInfo.m_iGameTime * 60000;
					if(PVP_TYPE_KTH==m_kBasicInfo.m_kType || PVP_TYPE_LOVE == m_kBasicInfo.m_kType )
					{
						dwGameTime = 0;
					}

					BM::Stream kReadyPacket( PT_T_M_REQ_READY_GROUND, m_kBasicInfo.m_kType );
					kReadyPacket.Push( m_kExtInfo.kOption );

					kReadyPacket.Push( m_iLobbyID );
					kReadyPacket.Push( GetIndex() );
					kReadyPacket.Push( dwGameTime );
					kReadyPacket.Push( m_kBasicInfo.m_iGamePoint );
					kReadyPacket.Push( m_kBasicInfo.m_kMode );
					kReadyPacket.Push( m_kExtInfo.ucRound );
					PU::TWriteTable_AM( kReadyPacket, m_kContSlot_User );
					g_kServerSetMgr.Locked_SendToGround( m_kGameGround, kReadyPacket, true );

					m_kBasicInfo.m_kStatus = ROOM_STATUS_PLAY;
					SendModifyed();

					// ·О±Чё¦ іІ°Ь
					PgLogCont kLogCont(ELogMain_Contents_PVP, ELogSub_PVP_Game);
					kLogCont.MemberKey( m_kGameGround.Guid() );// GroundGuid
					kLogCont.GroundNo( m_kGameGround.GroundNo() );
					kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

					__int64 const i64UseHandyCap = ( (true == m_kExtInfo.IsUseHandyCap()) ? 1i64 : 0i64 );
					__int64 const i64UseItem = ( (true == m_kExtInfo.IsUseItem()) ? 1i64 : 0i64 );

					PgLog kLog( ELOrderMain_Game, ELOrderSub_Start );
					kLog.Set( 0, static_cast<int>(GetRoomNo()) );							// iValue1 방번호
					kLog.Set( 2, static_cast<int>(m_kBasicInfo.m_kMode) );					// iValue3 게임 모드
					kLog.Set( 3, static_cast<int>(m_kBasicInfo.m_kType) );					// iValue4 게임 타입
					kLog.Set( 0, i64StartGameUser );										// i64Value1 플레이어숫자
					kLog.Set( 1, static_cast<__int64>(m_kExtInfo.ucRound) );				// i64Value2 게임라운드숫자
					kLog.Set( 2, static_cast<__int64>(m_kBasicInfo.m_iGameTime) * 60i64 );	// i64Value3 게임시간(초)
					kLog.Set( 3, i64UseHandyCap );											// i64Value4 핸디캡 사용여부
					kLog.Set( 4, i64UseItem );												// i64Value5 아이템 사용여부
					kLog.Set( 0, m_kBasicInfo.m_wstrName );									// Message1 방이름

					kLogCont.Add( kLog );
					kLogCont.Commit();
				}
				else
				{
					m_kBasicInfo.m_kStatus = ROOM_STATUS_LOBBY;
					m_kGameGround.Clear();

					BM::Stream kWarnMessage(PT_M_C_NFY_WARN_MESSAGE, (int)1);
					kWarnMessage.Push(std::wstring());
					BroadCast( kWarnMessage );
				}
				return S_OK;
			}break;
		case ROOM_STATUS_READY_EXITUSER:
			{
				// °ФАУБЯїЎ АЇАъ°Ў іЄ°Ўј­ °ФАУ ЅГАЫА» ЗТ јц ѕшАЅ
				BM::Stream kPacket( PT_T_M_NFY_CANCEL_GROUND );
				g_kServerSetMgr.Locked_SendToGround( m_kGameGround, kPacket, true );

				BM::Stream kWarnMessage( PT_M_C_NFY_WARN_MESSAGE, (int)200139 );
				kWarnMessage.Push(std::wstring());
				BroadCast( kWarnMessage );

				ReloadRoom( true );
			}break;
		}
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPRoom::ReadyFailed( SGroundKey const &kGroundKey )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( m_kBasicInfo.m_kStatus == ROOM_STATUS_READY )
	{
		if ( m_kGameGround == kGroundKey )
		{
			// Groundёёµй±в ґЩЅГ ЅГµµ.
			ReqCreateGround( true );
		}
	}
}

void PgPvPRoom::ReqCreateGround( bool const bReCall )
{
	SERVER_IDENTITY kSI;
	HRESULT const hRet = g_kServerSetMgr.Locked_GroundLoadBalance( m_kGameGround, kSI );
	switch ( hRet )
	{
	case S_OK:
		{
			// АОґшАё·О ёёµйАЪ.
			SPortalWaiter kPortalWaiter(PORTAL_PVP);
			kPortalWaiter.Set( kSI, m_kGameGround );

			kPortalWaiter.kRMM.kCasterKey.GroundNo( m_iLobbyID );// ї©±вїЎ Lobby IDё¦ Бэѕо іЦґВґЩ.

			::GUID kGuid;
			::memset( &kGuid, 0, sizeof(::GUID) );
			kGuid.Data1 = static_cast<unsigned long>(GetIndex());
			kPortalWaiter.kRMM.kCasterKey.Guid( BM::GUID(kGuid) );// ї©±вїЎ №жАЗ АОµ¦Ѕєё¦ Бэѕо іЦґВґЩ.

			BM::Stream kPortalPacket( PT_A_T_REQ_INDUN_CREATE );
			kPortalWaiter.WriteToPacket( kPortalPacket );
			SendToPortalMgr( kPortalPacket );
		}break;
	case S_FALSE:
		{
			// №«ЗС Call №жБц
			if ( bReCall )
			{
				// ё№АМ Critical ЗСµҐ...
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Ground[") << m_kGameGround.GroundNo() << _COMMA_ << m_kGameGround.Guid()\
					<< _T(", Sync Have Problem"));
				Start( m_kGameGround, false );
			}
			else
			{
				Start( m_kGameGround, true );
			}
		}break;
	default:
		{
			Start( m_kGameGround, false );
		}break;
	}
}

HRESULT PgPvPRoom::End( SEventMessage* const pkMsg )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( SetStatus(ROOM_STATUS_RESULT) )
	{
		ContUserSlot::iterator slot_itr = m_kContSlot_User.begin();
		for( ; slot_itr!=m_kContSlot_User.end(); ++slot_itr )
		{// °ФАУАМ іЎіЄёй ёрµО ЗЗё¦ Г¤їц БШґЩ
			ContUserSlot::mapped_type &kElement = slot_itr->second;
			PgPlayer* pkPlayer = const_cast<PgPlayer*>(kElement.pkPlayer);
			if(pkPlayer)
			{
				int const iHP = pkPlayer->GetAbil(AT_MEMORIZED_HP);
				int const iMP = pkPlayer->GetAbil(AT_MEMORIZED_MP);
				if(0 < iHP 
					&& 0 < iMP
					)
				{
					pkPlayer->SetAbil(AT_HP		, iHP);
					pkPlayer->SetAbil(AT_MP		, iMP);
				}
			}
		}

		switch ( m_iLobbyID )
		{
		case PvP_Lobby_GroundNo_League:
			{
				int iWinTeam = TEAM_NONE;
				pkMsg->Pop(iWinTeam);

				BM::Stream kAnsPacket(PT_T_N_NFY_RESULT_PVPLEAGUE_GAME);
				kAnsPacket.Push(m_kBattleGuid);
				kAnsPacket.Push(iWinTeam);
				kAnsPacket.Push(m_iIndex);
				::SendToRealmContents( PMET_PVP_LEAGUE, kAnsPacket );
			}break;
		default: {}break;
		}

//		CONT_PVP_GAME_REWARD kReward;
//		PU::TLoadArray_M( *pkMsg, kReward );

//		CONT_PLAYER_MODIFY_ORDER kOrder;

//		CONT_PVP_GAME_REWARD::iterator reward_itr;
//		for ( reward_itr=kReward.begin(); reward_itr!=kReward.end(); ++reward_itr )
//		{
//			ContUserSlot::iterator slot_itr = m_kContSlot_User.find( reward_itr->kCharGuid );
//			if ( slot_itr != m_kContSlot_User.end() )
//			{
//				SPvPLobbyUser const &kLobbyUser = slot_itr->second.rkLobbyUser;

				

//				if( m_kBasicInfo.m_kMode == PVP_MODE_TEAM )
//				{
//					switch( reward_itr->kWinLose )
//					{
//					case GAME_DRAW:
//						{	// єс°еА» °жїм
//							++(slot_itr->second.kNowRoomReport.m_iDraw);
//							kLog.Push((__int64)LOG_CONTENTS_PVP_USER_RESULT__DRAW_TEAM);
// 						}break;
//					case GAME_WIN:
//						{
//							++(slot_itr->second.kNowRoomReport.m_iWin);
//							kLog.Push((__int64)LOG_CONTENTS_PVP_USER_RESULT__WIN_TEAM);
//						}break;
//					case GAME_LOSE:
//						{
//							++(slot_itr->second.kNowRoomReport.m_iLose);
//							kLog.Push((__int64)LOG_CONTENTS_PVP_USER_RESULT__LOSE_TEAM);
//						}break;
//					}
					
//					DWORD const dwAddPoint = reward_itr->GetTotalPoint();
//					slot_itr->second.kNowRoomReport.Add(reward_itr->kResult);
//					slot_itr->second.kNowRoomReport.m_iPoint += dwAddPoint;

//					kLog.Push( slot_itr->second.GetWin() );
//					kLog.Push( slot_itr->second.GetLost() );
//					kLog.Push( slot_itr->second.GetDraw() );
//					kLog.Push( (__int64)reward_itr->kRank );
//					kLog.Push( (__int64)dwAddPoint );
//					kLog.Push( (__int64)reward_itr->iCP );
//					kLog.Send();


//					/////////////////////////////////////////////////////////////////////
//					// ЅЗБ¦ ±в·П.
//					SPMOD_AddPvpRecord kAddRecord( reward_itr->kWinLose, 1 );
//					kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, kLobbyUser.kCharacterGuid, kAddRecord));
//					//їАґх єёі»ѕЯБц. AddCP ё¦ єёіѕ¶§ °°АМ.
//
//					SPMOD_AddPvpRecord kAddPoint( GAME_POINT, dwAddPoint );
//					kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, kLobbyUser.kCharacterGuid, kAddPoint));
//
//					SPMOD_AddPvpRecord kAddKill( GAME_KILL, reward_itr->kResult.usKillCount );
//					kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, kLobbyUser.kCharacterGuid, kAddKill));
//
//					SPMOD_AddPvpRecord kAddDeath( GAME_DEATH, reward_itr->kResult.usDieCount );
//					kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, kLobbyUser.kCharacterGuid, kAddDeath));
//
//					SPMOD_Add_CP kAddCP(reward_itr->iCP);
//					kOrder.push_back(SPMO(IMET_ADD_CP, kLobbyUser.kCharacterGuid, kAddCP));
//
//					PgAction_ReqModifyItem2 kItemModifyAction(IMEPT_NONE, m_kGameGround, kOrder);
//					kItemModifyAction.DoAction(&slot_itr->second, NULL);
//				}
//			}
//		}
//
//		// ClientЗСЕЧ єёі»БЩ ЖРЕ¶
//		BM::Stream kCPacket( PT_T_C_NFY_GAME_RESULT, iWinTeam );
//		PU::TWriteArray_M( kCPacket, kReward );
//		BroadCast( kCPacket );
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPRoom::ReloadRoom( bool const bNoEnemy )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( SetStatus(ROOM_STATUS_LOBBY) )
	{
		// ·О±Чё¦ іІ°Ь
		PgLogCont kLogCont(ELogMain_Contents_PVP, ELogSub_PVP_Game);
		kLogCont.MemberKey( m_kGameGround.Guid() );// GroundGuid
		kLogCont.GroundNo( m_kGameGround.GroundNo() );
		kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

		PgLog kLog( ELOrderMain_Game, ELOrderSub_End );
		kLog.Set( 0, static_cast<int>(GetRoomNo()) );							// iValue1 №ж№шИЈ
		kLog.Set( 2, static_cast<int>(m_kBasicInfo.m_kMode) );					// iValue3 °ФАУ ёрµе
		kLog.Set( 3, static_cast<int>(m_kBasicInfo.m_kType) );					// iValue4 °ФАУ ЕёАФ
		kLog.Set( 0, static_cast<__int64>(m_kContSlot_User.size()) );			// i64Value1 ЗГ·№АМѕојэАЪ
		kLog.Set( 1, static_cast<__int64>(bNoEnemy) );							// i64Value2 Б¤»у/єсБ¤»уБѕ·бї©єО
		kLog.Set( 0, m_kBasicInfo.m_wstrName );									// Message1 №жАМё§

		kLogCont.Add( kLog );
		kLogCont.Commit();

		if ( true == m_bOrderClose )
		{
			Close();
		}
		else
		{
			ContUserSlot::iterator slot_itr = m_kContSlot_User.begin();
			for( ; slot_itr!=m_kContSlot_User.end(); ++slot_itr )
			{
				ContUserSlot::mapped_type &kElement = slot_itr->second;
				kElement.byStatus &= PS_RELOADROOM_AND_FLAG;
			}

			BM::Stream kCPacket(PT_T_C_NFY_RELOAD_ROOM);
			PU::TWriteTable_AM( kCPacket, m_kContSlot_User);
			PU::TWriteKey_A( kCPacket, m_kContSlot_Empty );
			m_kUserEvent.WriteToPacket( kCPacket );
			BroadCast( kCPacket );

			m_kUserEvent.Clear();
			SendModifyed();
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPRoom::SetUserEvent( BM::Stream const &kPacket, PVPUTIL::PgUserEvent const &kMergeEvent )
{
	BM::CAutoMutex kLock( m_kMutex );

	switch ( m_kBasicInfo.m_kStatus )
	{
	case ROOM_STATUS_CLOSE:
		{
		}break;
	case ROOM_STATUS_LOBBY:
		{
			BroadCast( kPacket );
		}break;
	default:
		{// °ФАУБЯАМёй єёі»Бц ѕКґВґЩ.
			m_kUserEvent.Merge( kMergeEvent );
		}break;
	}
}
