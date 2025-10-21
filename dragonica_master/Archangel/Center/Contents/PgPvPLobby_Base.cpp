#include "stdafx.h"
#include "Variant/PgPvPRanking.h"
#include "PgPvPLobby_Base.h"

PgPvPLobby_AnterRoom::PgPvPLobby_AnterRoom(void)
{
}

PgPvPLobby_AnterRoom::~PgPvPLobby_AnterRoom(void)
{
}

bool PgPvPLobby_AnterRoom::Exit( BM::GUID const &kCharacterGuid )
{
	BM::CAutoMutex kLock( m_kUserMutex );
	PgPvPLobby_Base::CONT_USER::iterator user_itr = m_kContUser.find( kCharacterGuid );
	if ( user_itr != m_kContUser.end() )
	{
		g_kTotalObjMgr.ReleaseUnit( user_itr->second );
		m_kContUser.erase( user_itr );
		return true;
	}
	return false;
}

bool PgPvPLobby_AnterRoom::ProcessMsg( SEventMessage *pkMsg )
{
	int const iSecType = pkMsg->SecType();
	switch( iSecType )
	{
	case 0:
		{
			SMsgBroadCast kMsgBroadCast( pkMsg );

			if ( PT_A_NFY_USER_DISCONNECT == kMsgBroadCast.m_kType )
			{
				BM::GUID kMemberGuid;
				BM::GUID kCharacterGuid;
				pkMsg->Pop(kMemberGuid);
				pkMsg->Pop(kCharacterGuid);
				Exit( kCharacterGuid );
			}

			m_kLobbys.ForEach( kMsgBroadCast );
		}break;
	case PvP_Lobby_GroundNo_AnterRoom:
		{
			BM::Stream::DEF_STREAM_TYPE kType = 0;
			pkMsg->Pop( kType );

			switch( kType )
			{
			case PT_T_T_REQ_JOIN_LOBBY:
				{
					UNIT_PTR_ARRAY kUnitArray;
					kUnitArray.ReadFromPacket( *pkMsg );

					UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
					for ( ; unit_itr != kUnitArray.end() ; ++unit_itr )
					{
						PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
						if ( pkPlayer )
						{
							BM::CAutoMutex kLock( m_kUserMutex );
							std::pair<PgPvPLobby_Base::CONT_USER::iterator, bool> kPair = m_kContUser.insert( std::make_pair( pkPlayer->GetID(), pkPlayer ) );
                            if ( true == kPair.second )
							{
								unit_itr->bAutoRemove = false;

								SAnsMapMove_MT kAMM( g_kProcessCfg.ServerIdentity(), MMET_PvP, MMR_PVP_LOBBY );
								kAMM.kAttr = GATTR_PVP;
								kAMM.kGroundKey.GroundNo(PvP_Lobby_GroundNo_AnterRoom);

								BM::Stream kTPacket( PT_T_T_ANS_MAP_MOVE_RESULT, kAMM );
								kTPacket.Push( pkPlayer->GetID() );
								::SendToCenter( kTPacket );

								BM::Stream kCPacket( PT_M_C_ANS_PVP_SELECTOR );
								::SendToUser( pkPlayer->GetMemberGUID(), pkPlayer->GetSwitchServer(), kCPacket );
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"CharacterGuid<" << pkPlayer->GetID() << L"> is Error!!" );
							}
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Only Player Join PvP!!!" );
						}
					}
				}break;
			case PT_A_NFY_USER_DISCONNECT:
				{
					BM::GUID kMemberGuid;
					BM::GUID kCharacterGuid;
					pkMsg->Pop(kMemberGuid);
					pkMsg->Pop(kCharacterGuid);
					Exit( kCharacterGuid );
				}break;
			case PT_S_M_WRAPPED_USER_PACKET:
				{
					BM::GUID kCharacterGuid;
					BM::Stream::DEF_STREAM_TYPE usPacketType = 0;
					pkMsg->Pop( kCharacterGuid );
					pkMsg->Pop( usPacketType );
					return RecvFromUser( kCharacterGuid, usPacketType, pkMsg );
				}break;
			}
		}break;
	default:
		{
			PgPvPLobby_Base * pkLobby = m_kLobbys.Get( iSecType );
			if ( pkLobby )
			{
				BM::Stream::DEF_STREAM_TYPE kType = 0;
				pkMsg->Pop( kType );

				return pkLobby->ProcessMsg( kType, pkMsg );
			}
			else
			{
				return false;
			}
		}break;
	}

	return true;
}

bool PgPvPLobby_AnterRoom::RecvFromUser( BM::GUID const &kCharGuid, BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock( m_kUserMutex );// Lock БЦАЗ

	PgPvPLobby_Base::CONT_USER::iterator user_itr = m_kContUser.find( kCharGuid );
	if ( user_itr != m_kContUser.end() )
	{
		switch( kType )
		{
		case PT_C_M_REQ_JOIN_LOBBY:
			{
				int iTargetLobbyID = 0;
				pkPacket->Pop( iTargetLobbyID );

				PgPvPLobby_Base * pkLobby = m_kLobbys.Get( iTargetLobbyID );
				if ( pkLobby )
				{
					if ( true == pkLobby->Locked_Join( user_itr->second ) )
					{
						m_kContUser.erase( user_itr ); // БцїьАЅ
					}
				}
				return true;
			}break;
		case PT_C_T_REQ_EXIT_LOBBY:
			{
				BM::Stream kTPacket( PT_T_T_REQ_EXIT_LOBBY );
				user_itr->second->WriteToPacket( kTPacket, WT_DEFAULT );
				::SendToCenter( kTPacket );

				g_kTotalObjMgr.ReleaseUnit( user_itr->second );
				m_kContUser.erase( user_itr );
			}break;
		default:
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << L"Error Packet Type<" << kType << L"> CharGuid<" << kCharGuid << L">" );
			}break; 
		}
	}
	return false;
}

void PgPvPLobby_AnterRoom::OnTick(void)
{
	STick kTick;
	m_kLobbys.ForEach( kTick );
}

void PgPvPLobby_AnterRoom::OnTick_Event(void)
{
	STickEvent kTick;
	m_kLobbys.ForEach( kTick );
}

void PgPvPLobby_AnterRoom::OnTick_Log(void)
{
	STickLog kTick;
	m_kLobbys.ForEach( kTick );
}
