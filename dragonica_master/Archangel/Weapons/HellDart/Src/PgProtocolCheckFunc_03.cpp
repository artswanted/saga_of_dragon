#include "stdafx.h"
#include "BM/Stream.h"
#include "BM/vstring.h"
#include "BM/Point.h"
#include "Lohengrin/dbtables3.h"
#include "Lohengrin/Packetstruct.h"
#include "variant/PgPlayer.h"
#include "Variant/PgEmporia.h"

HRESULT CALLBACK Check_PT_C_L_TRY_LOGIN(BM::Stream &kPacket)
{//OK. Lim 090408
	size_t const iCheckRemainSize = ( sizeof(short) * 2 ) + sizeof(bool);
	if( iCheckRemainSize == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_L_TRY_LOGIN_CHANNELMAPMOVE(BM::Stream & Packet)
{
	short iRealmNo, iChannelNo;
	int iGroundNo;
	bool bChannel;

	if( Packet.Pop(bChannel) )
	{
		if( Packet.Pop(iRealmNo) )
		{
			if( !Packet.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_C_SELF_DISCONNECT_SERVER(BM::Stream &kPacket)
{//OK. Lim 090408
	// 이 패킷은 서버로 보내지 않는다.
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_MAPLOADED(BM::Stream &kPacket)
{//OK. Lim 090408
	BYTE cCause = 0;
	if ( true == kPacket.Pop( cCause ) )
	{	// 나중에 cCause값 검사 하는거 추가
		if( sizeof(SGroundKey) == kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_SELECT_CHARACTER_OTHERMAP(BM::Stream &kPacket)
{//OK. Lim 090408
	if ( sizeof(int) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DEFAULT_MAP_MOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_CHANNLE_INFORMATION(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_POSITION(BM::Stream &kPacket)
{//OK. Lim 090408
	POINT3 pt3Pos;
	if ( true == kPacket.Pop(pt3Pos) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_RECENT_MAP_MOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_CREATE_ENTITY(BM::Stream &kPacket)
{//OK. Lim 090408
	return E_FAIL;//오면 안되는 패킷.
}

HRESULT CALLBACK Check_PT_C_M_REQ_DELETE_ENTITY(BM::Stream &kPacket)
{//OK. Lim 090408
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EFFECT_CONTROL(BM::Stream &kPacket)
{//OK. Lim 090408
	int iEffectNo = 0;
	if ( true == kPacket.Pop( iEffectNo ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RANK_PAGE(BM::Stream &kPacket)
{//OK. Lim 090408
	SMissionKey kMissionKey;
	if ( true == kPacket.Pop(kMissionKey) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_RANK_TOP(BM::Stream &kPacket)
{
	SMissionKey kMissionKey;
	if ( true == kPacket.Pop(kMissionKey) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}
HRESULT CALLBACK Check_PT_C_M_REQ_SPEND_MONEY(BM::Stream &kPacket)
{//OK. Lim 090408
	ESpendMoneyType kType = ESMT_NONE;
	if ( true == kPacket.Pop( kType ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRIGGER(BM::Stream &kPacket)
{//OK. Lim 090408
	int iType = QOT_None;
	
	if ( true == kPacket.Pop( iType ) )
	{	
		switch(iType)
		{
		case QOT_NPC:
		case QOT_Monster:
			{
				BM::GUID kGuid;
				if ( !kPacket.Pop( kGuid ) )
				{
					return E_FAIL;
				}

				int iActionType = 0;
				if ( !kPacket.Pop( iActionType ) )
				{
					return E_FAIL;
				}
			}break;
		case QOT_Trigger:
		case QOT_Item:
		case QOT_Time:
		case QOT_ShineStone:
		case QOT_Location:
		case QOT_Trap:
			{
				BM::GUID kGuid;
				int iActionType = 0;
				kPacket.Pop( kGuid );
				kPacket.Pop( iActionType );
			}break;
		case QOT_Dialog:
			{
				BM::GUID kGuid;
				int iValue = 0;
				POINT3 kPos;
				if ( !kPacket.Pop( kGuid ) ){return E_FAIL;}
				if ( !kPacket.Pop( iValue ) ){return E_FAIL;}
				if ( !kPacket.Pop( iValue ) ){return E_FAIL;}
				if ( !kPacket.Pop( kPos ) ){return E_FAIL;}
			}break;
		default:
			{
				return E_FAIL;
			}break;
		}

		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INDUN_START(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_QUESTSCROLL(BM::Stream &kPacket)
{
	int iQuestID = 0;
	BM::GUID kGuid;
	DWORD dwClientTime;

	if( true == kPacket.Pop( iQuestID ) )
	{
		if( true == kPacket.Pop( kGuid ) )
		{
			if( true == kPacket.Pop( dwClientTime) )
			{
				if( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SYNCTIME_CHECK(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRIGGER_ACTION(BM::Stream &kPacket)
{//OK. Lim 090408
	std::string strTriggerID;
	if ( true == kPacket.Pop( strTriggerID, MAX_PATH ) )
	{
		int iData = 0;
		if ( true == kPacket.Pop( iData ) )
		{
			if (	0 == kPacket.RemainSize() 
				||	sizeof(TBL_DEF_TRANSTOWER_TARGET_KEY) == kPacket.RemainSize()
			)
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_START_HYPER_MOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	int iType = 0;
	if ( true == kPacket.Pop( iType ) )
	{
		std::string strTriggerID;
		if ( true == kPacket.Pop( strTriggerID, MAX_PATH ) )
		{
			POINT3 pt3Pos;
			if ( true == kPacket.Pop( pt3Pos ) )
			{
				if( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_END_HYPER_MOVE(BM::Stream &kPacket)
{//OK. Lim 090408
	POINT3 pt3EndPos;
	if ( true == kPacket.Pop(pt3EndPos) )
	{
		BM::GUID kGuid;
		if ( true == kPacket.Pop( kGuid ) )
		{
			bool bRealyJump = false;
			if ( true == kPacket.Pop( bRealyJump ) )
			{
				if( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRANSTOWER_BY_PET(BM::Stream &kPacket)
{
	size_t const iCheckSize =
		sizeof(BM::GUID) + sizeof(BM::GUID) + sizeof(int) +
		sizeof(TBL_DEF_TRANSTOWER_TARGET_KEY) + sizeof(bool);
	if(kPacket.RemainSize() != iCheckSize)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CALLBACK Check_PT_C_M_REQ_TRANSTOWER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop( kGuid ) )
	{
		int iData = 0;
		if ( true == kPacket.Pop( iData ) )
		{
			switch( iData )
			{
			case E_TRANSTOWER_SAVEPOS:
				{
					if ( 0 == kPacket.RemainSize() )
					{
						return S_OK;
					}
				}break;
			case E_TRANSTOWER_MAPMOVE:
				{
					TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;
					if ( true == kPacket.Pop( kTargetKey ) )
					{
						bool bUseDiscount = false;
						if (	true == kPacket.Pop( bUseDiscount ) 
							&&	BM::IsCorrectType(bUseDiscount)
							)
						{
							if ( true == bUseDiscount )
							{
								if ( sizeof(SItemPos) == kPacket.RemainSize() )
								{
									return S_OK;
								}
							}
							else
							{
								if ( !kPacket.RemainSize() )
								{
									return S_OK;
								}
							}
						}				
					}
				}break;
/*
			case E_TRANSTOWER_OPENMAP:
				{
					TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;
					if ( true == kPacket.Pop( kTargetKey ) )
					{
						if ( sizeof(SItemPos) == kPacket.RemainSize() )
						{
							return S_OK;
						}
					}
				}break;
*/
			default:
				{
				}break;
			}
		}
	}
	return E_FAIL;
}

// PvP
HRESULT CALLBACK Check_PT_C_M_REQ_OBMODE_END(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_OBMODE_TARGET_CHANGE(BM::Stream &kPacket)
{//OK. Lim 090408
	if ( kPacket.RemainSize() != sizeof(BM::GUID) )
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CALLBACK Check_PT_C_M_REQ_DUEL_PVP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop( kGuid ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_ANS_DUEL_PVP(BM::Stream &kPacket)
{//OK. Lim 090408
	size_t const iCheckSize = ( sizeof(BM::GUID) + sizeof(bool) );
	if ( iCheckSize == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_DUEL_PVP_CANCEL(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop( kGuid ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_ERROR_DUEL_PVP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop( kGuid ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_JOIN_LOBBY(BM::Stream &kPacket)
{//OK. Lim 090408
	int iLobbyID = 0;
	kPacket.Pop( iLobbyID );

	switch( iLobbyID )
	{
	case PvP_Lobby_GroundNo_AnterRoom:
	case PvP_Lobby_GroundNo_Exercise:
	case PvP_Lobby_GroundNo_Ranking:
	case PvP_Lobby_GroundNo_League:
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}break;
	case HardCoreDungeon:
		{
			if ( sizeof(SItemPos) == kPacket.RemainSize() )
			{
				return S_OK;
			}
		}break;
	default:
		{

		}break;
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_EXIT_LOBBY(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_CREATE_ROOM(BM::Stream &kPacket)
{//OK. Lim 090408
	SPvPRoomBasicInfo kBasicInfo;
	if ( true == kBasicInfo.ReadFromPacket( kPacket ) )
	{
		SPvPRoomExtInfo kExtInfo;
		if ( true == kExtInfo.ReadFromPacket( kPacket ) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_JOIN_ROOM(BM::Stream &kPacket)
{//OK. Lim 090408
	int iRoomIndex = 0;
	if ( true == kPacket.Pop( iRoomIndex ) )
	{
		std::string strPassword;
		if ( true == kPacket.Pop( strPassword, PVP_ROOM_PASSWORD_LENS ) )
		{
			bool bJoin = false;

			if ( true == kPacket.Pop(bJoin) )
			{
				if( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_EXIT_ROOM(BM::Stream &kPacket)
{//OK. Lim 090408
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_TEAM_CHANGE(BM::Stream &kPacket)
{//OK. Lim 090408
	int iTeam = 0;
	if ( true == kPacket.Pop(iTeam) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_GAME_READY(BM::Stream &kPacket)
{//OK. Lim 090408
	SPvPRoomBasicInfo kBasicInfo;
	if ( true == kBasicInfo.ReadFromPacket( kPacket ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_MODIFY_ROOM(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bBasinInfo = false;
	if ( true == kPacket.Pop(bBasinInfo) )
	{
		if ( true == BM::IsCorrectType(bBasinInfo) )
		{
			if ( true == bBasinInfo )
			{
				SPvPRoomBasicInfo kBasicInfo;
				if ( !kBasicInfo.ReadFromPacket(kPacket) )
				{
					return E_FAIL;
				}
			}
			else
			{
				SPvPRoomExtInfo kExtInfo;
				if ( !kExtInfo.ReadFromPacket(kPacket) )
				{
					return E_FAIL;
				}
			}

			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_KICK_ROOM_USER(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop(kGuid) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_SLOTSTATUS_CHANGE(BM::Stream &kPacket)
{//OK. Lim 090408
	bool bOpen = false;
	if ( true == kPacket.Pop( bOpen ) )
	{
		if ( true == BM::IsCorrectType(bOpen) )
		{
			SPvPTeamSlot kTeamSlot;
			if ( true == kTeamSlot.ReadFromPacket(kPacket) )
			{
				if( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_ENTRUST_MASTER(BM::Stream &kPacket)
{
	BM::GUID kGuid;
	if ( true == kPacket.Pop(kGuid) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_WITH_PVP(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kGuid;
	if ( true == kPacket.Pop(kGuid) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_INVITE_PVP(BM::Stream &kPacket)
{//OK. Lim 090408
	size_t iSize = 0;
	if ( true == kPacket.Pop( iSize ) )
	{
		if ( PVP_INVITE_MAX_USER_COUNT >= iSize )
		{
			size_t const iRemainSize = ( sizeof(BM::GUID) * iSize );
			if ( iRemainSize == kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_CHANGE_LOBBY(BM::Stream &kPacket)
{
	int iTargetLobbyID = 0;
	if ( true == kPacket.Pop( iTargetLobbyID ) )
	{
		if ( !kPacket.RemainSize() )
		{
			if ( 0 < iTargetLobbyID && PvP_Lobby_GroundNo_Max > iTargetLobbyID )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_GET_PVPRANKING(BM::Stream &kPacket)
{
	if ( sizeof(__int64) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PVP_SELECTOR(BM::Stream &kPacket)
{
	if ( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_PVP_SELECTOR_CANCEL(BM::Stream &kPacket)
{
	if ( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_INVATE_FAIL(BM::Stream &kPacket)
{
	BM::GUID kCharGuid;
	if ( true == kPacket.Pop( kCharGuid ) )
	{
		if ( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_ENTRY_CHANGE(BM::Stream &kPacket)
{
	SPvPTeamEntry kTeamEntry;
	if ( true == kTeamEntry.ReadFromPacket(kPacket) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

// HardCore Dungeon
HRESULT CALLBACK Check_PT_C_M_REQ_RET_HARDCORE_VOTE(BM::Stream &kPacket)
{
	if ( sizeof(bool) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_BS_CHANNEL_INFO(BM::Stream &kPacket)
{
	if (kPacket.RemainSize() == 0)
	{
		return S_OK;
	}
	return E_FAIL;
}

// Emporia
HRESULT CALLBACK Check_PT_C_N_REQ_EMPORIA_STATUS_LIST(BM::Stream &kPacket)
{//OK. Lim 090408
	int const iSize = sizeof(BM::GUID)+sizeof(EEmporiaStatusListType);
	if ( kPacket.RemainSize() == iSize )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_JOIN_EMPORIA(BM::Stream &kPacket)
{//OK. Lim 090408
	if ( kPacket.RemainSize() == sizeof(SEmporiaKey) )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REG_JOIN_EMPORIA_MERCENARY(BM::Stream &kPacket)
{
	BM::GUID kEmporiaID;
	if ( true == kPacket.Pop( kEmporiaID ) )
	{
		size_t iIndex;
		if ( true == kPacket.Pop(iIndex) )
		{
			bool bIsAttack;
			if (	true == kPacket.Pop(bIsAttack)
				&&	true == BM::IsCorrectType(bIsAttack))
			{
				if ( !kPacket.RemainSize() )
				{
					return S_OK;
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_EMPORIA_ADMINISTRATION(BM::Stream &kPacket)
{//OK. Lim 090408
	BM::GUID kNpcGuid;
	if ( true == kPacket.Pop( kNpcGuid ) )
	{
		bool bControlFunc = false;
		if (	true == kPacket.Pop( bControlFunc ) 
			&&	true == BM::IsCorrectType(bControlFunc) )
		{
			if ( true == bControlFunc )
			{
				short nFuncNo = 0;
				if ( true == kPacket.Pop( nFuncNo ) )
				{
					short nWeekCount = 0;
					if ( true == kPacket.Pop( nWeekCount ) )
					{
						if( !kPacket.RemainSize() )
						{
							return S_OK;
						}
					}
				}
			}
			else
			{
				BYTE byGate = EMPORIA_GATE_CLOSE;
				if ( true == kPacket.Pop( byGate ) )
				{
					if (	(EMPORIA_GATE_CLOSE < byGate)
						&&	(EMPORIA_GATE_MAX > byGate)
						)
					{
						if( !kPacket.RemainSize() )
						{
							return S_OK;
						}
					}
				}
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_USE_EMPORIA_FUNCTION(BM::Stream &kPacket)
{//OK. Lim 090408
	if ( sizeof(short) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_GET_EMPORIA_ADMINISTRATOR(BM::Stream &kPacket)
{
	if ( sizeof(BM::GUID) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_SELECT_BATTLE_AREA(BM::Stream &kPacket)
{
	if ( sizeof(int) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

// PvP League
HRESULT CALLBACK Check_PT_C_N_REQ_REGIST_PVPLEAGUE_TEAM(BM::Stream &kPacket)
{
	BM::GUID kReqUserGuid;
	if ( true == kPacket.Pop(kReqUserGuid) )
	{
		TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
		if ( true == kTeamInfo.ReadFromPacket(kPacket) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_GIVEUP_PVPLEAGUE_TEAM(BM::Stream &kPacket)
{
	BM::GUID kReqUserGuid;
	if ( true == kPacket.Pop(kReqUserGuid) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_T_REQ_JOIN_LEAGUE_ROOM(BM::Stream &kPacket)
{
	if ( sizeof(BM::GUID) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_N_REQ_QUERY_PVPLEAGUE_TEAM(BM::Stream &kPacket)
{
	if ( sizeof(BM::GUID) == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

// GMCommand
HRESULT CALLBACK Check_PT_C_M_GODCMD(BM::Stream &kPacket)
{
	// 검사 방법이 매우매우 어렵다..
	// 맵서버에서 GMLevel을 체크해서 걸러내니까 일단 무조건 통과
	return S_OK;
}

HRESULT CALLBACK Check_PT_C_NT_GODCMD(BM::Stream &kPacket)
{
	// 검사 방법이 매우매우 어렵다..
	// 맵서버에서 GMLevel을 체크해서 걸러내니까 일단 무조건 통과
	return S_OK;
}

HRESULT CALLBACK Check_PT_C_M_REQ_INDUN_START2(BM::Stream &kPacket)
{
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_SAVE_CHARACTOR_SLOT(BM::Stream &kPacket)
{
	BM::GUID kReqUserGuid;
	int iSlot;
	if ( true == kPacket.Pop(kReqUserGuid) )
	{
		if( true == kPacket.Pop(iSlot) )
		{
			if( !kPacket.RemainSize() )
			{
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_REQ_FIND_CHARACTOR_EXTEND_SLOT(BM::Stream &kPacket)
{
	if( !kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_PROGRESS_POS(BM::Stream &kPacket)
{
	int iProgressPos = 0;
	if( true == kPacket.Pop( iProgressPos ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_NFY_RACE_CHECK_POINT(BM::Stream &kPacket)
{
	std::string strTriggerID;
	if ( true == kPacket.Pop( strTriggerID, MAX_PATH ) )
	{
		if( !kPacket.RemainSize() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_M_REQ_LOCKEXP_TOGGEL(BM::Stream &kPacket)
{
	bool bIsLocked = false;
	if(!kPacket.Pop(bIsLocked) || kPacket.RemainSize())
	{
		return E_FAIL;
	}

	return S_OK;
}
