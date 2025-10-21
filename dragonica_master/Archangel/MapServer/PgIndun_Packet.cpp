#include "stdafx.h"
#include "constant.h"
#include "Lohengrin/PacketStruct4Map.h"
#include "PgAction.h"
#include "PgStoneMgr.h"
#include "PgIndun.h"

bool PgIndun::VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy )
{
	switch(wType)
	{
	/*
	case PT_C_M_NFY_MAPLOAD_STATE:
		{
			unsigned char ucPer;
			pkNfy->Pop( ucPer );

			if ( ucPer )
			{
				SendMapLoadState( pkUnit->GetID(), ucPer );

				ConWaitUser::iterator user_itr;
				for ( user_itr=m_kWaitUserList.begin(); user_itr!=m_kWaitUserList.end(); ++user_itr )
				{
					if ( user_itr->kCharGuid == pkUnit->GetID() )
					{
						user_itr->ucPer = ucPer;
						break;
					}
				}
			}
		}break;
	*/
	case PT_C_M_REQ_JOIN_LOBBY:
	case PT_C_M_REQ_DUEL_PVP:
	case PT_C_M_ANS_DUEL_PVP:
	case PT_C_M_NFY_DUEL_PVP_CANCEL:
	case PT_C_M_ERROR_DUEL_PVP:
		{
			// PvP로 진입할 수 없는 장소입니다
			pkUnit->SendWarnMessage(19010);
		}break;
	case PT_S_A_REQ_AUTOLOGIN:
		{
			// 채널 변경을 요청 할 수 없는 곳입니다.
			INFO_LOG( BM::LOG_LV0, __FL__<<L"[PT_S_A_REQ_AUTOLOGIN] Ground["<<GetGroundNo()<<L"-"<<GroundKey().Guid()<<L"] User["<<pkUnit->Name()<<L"-"<<pkUnit->GetID()<<L"]" );
			pkUnit->SendWarnMessage(98);
		}break;
	case PT_C_M_REQ_INDUN_START:
		{
			if ( INDUN_STATE_READY == m_eState )
			{
				SetState( INDUN_STATE_PLAY );
				BM::Stream kCPacket( PT_M_C_ANS_INDUN_START );
				Broadcast( kCPacket );
			}
		}break;
	case PT_C_M_REQ_INDUN_START2:
		{
			BM::Stream kCPacket( PT_M_C_ANS_INDUN_START );
			Broadcast( kCPacket );
		}break;
	case PT_C_M_NFY_SELECTED_BOX:
		{
			BM::Stream kNPacket(wType, GroundKey() );
			kNPacket.Push( pkUnit->GetID() );
			kNPacket.Push( *pkNfy );
			SendToResultMgr(kNPacket);
		}break;
	case PT_C_M_REQ_RESULT_END:
		{
			if ( INDUN_STATE_RESULT == m_eState )
			{
				if ( m_pkPT3ResultSpawnLoc )
				{
					if ( !SendToPosLoc( pkUnit, *m_pkPT3ResultSpawnLoc ) )
					{
						CAUTION_LOG( BM::LOG_LV4, L"[PT_C_M_REQ_RESULT_END] Error Pos<" << m_pkPT3ResultSpawnLoc->x << L", " << m_pkPT3ResultSpawnLoc->y << L", " << m_pkPT3ResultSpawnLoc->z << L"> CharName<" << pkUnit->Name() << L">" );
						SendToSpawnLoc( pkUnit->GetID(), 1 );
					}
				}
				else
				{
					this->RecvRecentMapMove( dynamic_cast<PgPlayer*>(pkUnit) );
				}
			}
		}break;
	//Constellation
	case PT_C_M_REQ_CONSTELLATION_MISSION:
		{
			SendToParty_ConstellationMission();
		}break;
	case PT_C_M_REQ_CONSTELLATION_CASH_REWARD:
		{
			m_kConstellationResult.Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD(dynamic_cast<PgPlayer*>(pkUnit), this);
		}break;
	case PT_C_M_REQ_CONSTELLATION_CASH_REWARD_COMPLETE:
		{
			m_kConstellationResult.Recv_PT_C_M_REQ_CONSTELLATION_CASH_REWARD_COMPLETE(dynamic_cast<PgPlayer*>(pkUnit), this);
		}break;
	default:
		{
			return PgGround::VUpdate( pkUnit, wType, pkNfy );
		}break;
	}

	return true;
}
