#include "stdafx.h"
#include "PgAction.h"
#include "PgExpeditionGround.h"

//
PgExpeditionGround::PgExpeditionGround()
{
}

PgExpeditionGround::~PgExpeditionGround()
{
}

EOpeningState PgExpeditionGround::Init(int const iMonsterControlID, bool const bMonsterGen)
{
	m_iLastRemainMonsterCount = 0;
	m_pBestPlayer = NULL;
	m_pBestSupporter = NULL;
	m_BestTeam = 0;
	m_FailedTime = false;
	m_TimeLimit = 0;
	m_AccumLimitTime = 0;
	m_IsEventScript = false;
	m_RemainTime = 0;

	return PgIndun::Init(iMonsterControlID, bMonsterGen);
}

bool PgExpeditionGround::Clone(PgExpeditionGround * pGround)
{
	if( pGround )
	{
		return PgIndun::Clone(dynamic_cast<PgIndun*>(pGround));
	}

	return false;
}

bool PgExpeditionGround::Clone(PgGround * pGround)
{
	if( pGround )
	{
		return Clone(dynamic_cast<PgExpeditionGround*>(pGround));
	}

	return false;
}

void PgExpeditionGround::Clear()
{
	m_iLastRemainMonsterCount = 0;
	m_pBestPlayer = NULL;
	m_pBestSupporter = NULL;
	m_BestTeam = 0;
	m_FailedTime = false;
	m_TimeLimit = 0;
	m_AccumLimitTime = 0;
	m_IsEventScript = false;
	m_RemainTime = 0;

	PgIndun::Clear();
}

T_GNDATTR PgExpeditionGround::GetAttr() const
{
	return GATTR_EXPEDITION_GROUND;
}

int PgExpeditionGround::GetGroundNo() const
{
	return PgGround::GetGroundNo();
}

bool PgExpeditionGround::RecvRecentMapMove( PgPlayer *pkUser, bool const bClear )
{
	if ( pkUser )
	{
		if( this->GetAttr() & GATTR_EXPEDITION_GROUND )
		{
			if( bClear )
			{
				SGroundKey kKey;
				pkUser->GetRecentGround( kKey, GATTR_DEFAULT );

				SReqMapMove_MT kRMM( MMET_Normal, pkUser->GetRecentPos(GATTR_DEFAULT), kKey );

				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkUser ) )
				{
					return kMapMove.DoAction();
				}
			}
			else
			{// 클리어 한게 아니라면 원정대 탈퇴 시킨다.
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUser);
				if( pPlayer )
				{
					if( BM::GUID::NullData() != pPlayer->ExpeditionGuid() )
					{
						BM::Stream LeavePacket(PT_C_N_REQ_LEAVE_EXPEDITION);
						LeavePacket.Push(pPlayer->ExpeditionGuid());
						LeavePacket.Push(pPlayer->GetID());
						SendToGlobalPartyMgr(LeavePacket);
					}
				}
			}
		}
	}
	return PgIndun::RecvRecentMapMove(pkUser);
}

void PgExpeditionGround::SetState(EIndunState const eState, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( eState == m_eState)
	{
		return;
	}

	m_dwAutoStateRemainTime = 0;

	if( bChangeOnlyState )
	{
		return;
	}

	switch( eState )
	{
	case INDUN_STATE_OPEN:
	case INDUN_STATE_WAIT:	
	case INDUN_STATE_READY:
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
		}break;
	case INDUN_STATE_PLAY:
		{
			CUnit * pUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_itr;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
			pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr);
			if( pUser && (0 < m_TimeLimit) )
			{
				__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
				BM::Stream NfyPacket(PT_M_C_NFY_EXPEDITION_REMAIN_TIME);
				NfyPacket.Push(m_TimeLimit);
				NfyPacket.Push(i64NowTime);
				Broadcast(NfyPacket);

				VEC_GUID Vec_List;
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
				if( pPlayer )
				{
					if( pPlayer->HaveExpedition() )
					{
						BM::Stream Packet;
						m_kLocalPartyMgr.GetExpeditionMemberGround(pPlayer->ExpeditionGuid(), GroundKey(), Vec_List);
						m_kLocalPartyMgr.WriteToPacketExpeditionName(pPlayer->ExpeditionGuid(), Packet);
						SendToUser_ByGuidVec(Vec_List, Packet);
					}
				}
			}

			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
			return;
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
			if( false == m_FailedTime )
			{
				PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
			}
			else
			{
				PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
				bool IsActivateScript = false;
				CUnit * pUser = NULL;
				VEC_RESULTINDUN ResultList;
				CONT_OBJECT_MGR_UNIT::iterator unit_itr;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
				while ((pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
					if( pPlayer )
					{
						int const iActivateEventScriptID = pPlayer->GetAbil(AT_EVENT_SCRIPT);
						if( 0 != iActivateEventScriptID )
						{
							IsActivateScript = true;
							break;
						}
					}
				}
				if( false == IsActivateScript )
				{
					SetState(INDUN_STATE_RESULT);
				}
			}
			return;
		}break;
	case INDUN_STATE_RESULT:
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
			if( false == m_FailedTime )
			{
				CUnit * pUser = NULL;
				VEC_RESULTINDUN ResultList;
				CONT_OBJECT_MGR_UNIT::iterator unit_itr;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
				while ((pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() )
						{
							if( false == pPlayer->IsDead() )
							{
								ResultList.push_back( SNfyResultIndun(pPlayer->GetID(), pPlayer->GetAbil64(AT_EXPERIENCE), pPlayer->GetAbil(AT_LEVEL)) );
							}
							else
							{
								// 결산 시작되는데 죽어있으면 탈퇴 처리.
								BM::Stream LeavePacket(PT_C_N_REQ_LEAVE_EXPEDITION);

								LeavePacket.Push(pPlayer->ExpeditionGuid());
								LeavePacket.Push(pPlayer->GetID());
								SendToGlobalPartyMgr(LeavePacket);
							}
						}
					}
				}

				BM::Stream Packet(PT_M_N_NFY_EXPEDITION_RESULT, GroundKey());
				Packet.Push(ResultList);
				SendToResultMgr(Packet);
			}
			else
			{
				SetAutoNextState(5000);
			}
			return;
		}break;
	case INDUN_STATE_CLOSE:
		{
			VEC_GUID Vec_List;
			CUnit *pkUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_itr;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
			BM::GUID ExpeditionGuid;
			while((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
			{
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUser);
				if( pPlayer )
				{
					ExpeditionGuid = pPlayer->GetExpeditionGuid();
					BM::Stream Packet( PT_M_C_NFY_MISSION_UNLOCKINPUT );					
					pPlayer->Send(Packet);
					RecvRecentMapMove(pPlayer, true);
				}
			}

			{
				// 인던이 종료 됨을 알림.
				BM::Stream Packet(PT_M_N_NFY_EXPEDITION_INDUN_CLOSE);
				Packet.Push(GroundKey());
				SendToResultMgr(Packet);

				PgLocalExpedition* pExpedition = m_kLocalPartyMgr.GetLocalExpedition(ExpeditionGuid);
				if( pExpedition )
				{ // 원정대 정보 변경을 원정대 리스트 매니저로 알림.
					SExpeditionOption exOption = pExpedition->Option();
					exOption.SetOptionState(true);
					pExpedition->Option(exOption);
					BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
					pExpedition->WrtieToPacketListInfo(ModifyPacket);
					SendToExpeditionListMgr(ModifyPacket);

					BM::Stream ModifyPacket2(PT_M_T_NFY_MODIFY_EXPEDITION_OPTION);
					ModifyPacket2.Push(pExpedition->ExpeditionGuid());
					pExpedition->Option().WriteToPacket(ModifyPacket2);
					SendToGlobalPartyMgr(ModifyPacket2);
				}
			}
		}break;
	default:
		{
		}break;
	}
}

void PgExpeditionGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	switch( m_eState )
	{	
	case INDUN_STATE_PLAY:
		{
			DWORD const dwNow = BM::GetTime32();
			DWORD dwElapsed = 0;
			bool bTimeUpdate = true;

			if( INDUN_STATE_PLAY == m_eState )
			{
				bTimeUpdate = false;
			}
			CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsed, bTimeUpdate);

			if( (0 < m_TimeLimit) && (false == m_IsEventScript) ) // 클라이언트가 이벤트를 보고 있지 않을때만 카운트.
			{
				m_AccumLimitTime += dwElapsed;		// 누적시간.
				m_RemainTime -= dwElapsed;			// 남은시간.

				if( m_TimeLimit <= m_AccumLimitTime )
				{
					m_TimeLimit = 0;
					m_AccumLimitTime = 0;
					m_FailedTime = true;

					// 타임 오버. 실패.
					BM::Stream NfyPacket(PT_M_C_NFY_EXPEDITION_LIMITED_TIME);
					Broadcast(NfyPacket);

					SetState(INDUN_STATE_RESULT_WAIT);
				}
			}
			UpdateAutoNextState( dwNow, dwElapsed );
		}break;
	case INDUN_STATE_RESULT_WAIT:
	case INDUN_STATE_RESULT:
	case INDUN_STATE_OPEN:
	case INDUN_STATE_WAIT:
	case INDUN_STATE_READY:
	case INDUN_STATE_CLOSE:
	default:
		{
		}break;
	}

	{
		PgIndun::OnTick1s(); // 
	}
}

bool PgExpeditionGround::VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy)
{
	switch( wType )
	{
	case PT_C_M_REQ_DEFAULT_MAP_MOVE:
		{
			PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
			if( pPlayer )
			{
				// 죽어서 나가기를 선택했다면 원정대를 탈퇴 시킨다.
				// RecentMap은 탈퇴가 되면 보내준다.
				if( (GetAttr() & GKIND_EXPEDITION) 
					&&	pPlayer->HaveExpedition() )
				{
					// 탈퇴 시킴.
					BM::Stream Packet(PT_C_N_REQ_LEAVE_EXPEDITION);
					Packet.Push(pPlayer->ExpeditionGuid());
					Packet.Push(pPlayer->GetID());
					SendToGlobalPartyMgr(Packet);
				}
			}
		}break;
	case PT_C_N_REQ_EXIT_EXPEDITION_COMPLETE:
		{
			if( INDUN_STATE_RESULT == m_eState )
			{
				if( m_pkPT3ResultSpawnLoc )
				{
					if( !SendToPosLoc(pUnit, *m_pkPT3ResultSpawnLoc) )
					{
						CAUTION_LOG( BM::LOG_LV4, L"[PT_C_M_REQ_RESULT_END] Error Pos<" << m_pkPT3ResultSpawnLoc->x << L", " << m_pkPT3ResultSpawnLoc->y << L", " << m_pkPT3ResultSpawnLoc->z << L"> CharName<" << pUnit->Name() << L">" );
						SendToSpawnLoc( pUnit->GetID(), 1 );
					}
				}
				else
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
					if( pPlayer )
					{
						if( (GetAttr() & GKIND_EXPEDITION) 
							&&	pPlayer->HaveExpedition() )
						{
							// 탈퇴 시킴.
							BM::Stream Packet(PT_C_N_REQ_LEAVE_EXPEDITION);
							Packet.Push(pPlayer->ExpeditionGuid());
							Packet.Push(pPlayer->GetID());
							SendToGlobalPartyMgr(Packet);

							// 결산창 보는 도중이었다면..
							BM::Stream Stream( PT_M_C_NFY_MISSION_UNLOCKINPUT );					
							pPlayer->Send(Stream);
						}
					}
				}
			}
		}break;
	case PT_C_N_REQ_TENDER_ITEM:
		{
			BM::Stream Packet(PT_C_N_REQ_TENDER_ITEM, GroundKey());
			Packet.Push(*pNfy);

			SendToResultMgr(Packet);
		}break;
	case PT_C_N_REQ_GIVEUP_TENDER:
		{
			BM::Stream Packet(PT_C_N_REQ_GIVEUP_TENDER, GroundKey());
			Packet.Push(*pNfy);

			SendToResultMgr(Packet);
		}break;
	case PT_C_M_REQ_RUN_EVENT_SCRIPT:
		{	// 이벤트 스크립트에 진입한다면 시간카운트를 멈춰야 한다.
			PgIndun::VUpdate(pUnit, wType, pNfy);

			if( !m_IsEventScript )
			{
				m_IsEventScript = true; // 이 패킷이 오면 시간을 멈춰야 하기 때문에..
			}
		}break;
	case PT_C_M_REQ_END_EVENT_SCRIPT:
		{	// 이벤트 스크립트가 끝남. 원정대원이 모두 이벤트 스크립트를 마치면 다시 시간 카운트를 시작한다.
			PgPlayer * pPlayer = GetUser(pUnit->GetID());
			if( pPlayer )
			{
				// 먼저 하고나서 시간 카운트를 시작할지 검사.
				PgIndun::VUpdate(pUnit, wType, pNfy);

				bool IsEmpty = m_kContEventScriptPlayer.empty();	// 이벤트 스크립트를 보고있는 플레이어.
				if( IsEmpty )
				{
					m_IsEventScript = false;
					// 남은 시간을 보내줌..
					__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
					BM::Stream NfyPacket(PT_M_C_NFY_EXPEDITION_REMAIN_TIME);
					NfyPacket.Push(m_RemainTime);
					NfyPacket.Push(i64NowTime);
					Broadcast(NfyPacket);

					BM::Stream UnlockPacket(PT_M_C_REQ_UNLOCK_INPUT_EVENT_SCRIPT);
					Broadcast(UnlockPacket);		// 이벤트 스크립트를 모두 다봄.
				}
				else
				{
					BM::Stream LockPacket(PT_M_C_REQ_LOCK_INPUT_EVENT_SCRIPT);	// 모두 스크립트를 보지 않았기 때문에 인풋 막도록 보냄.
					pPlayer->Send(LockPacket, E_SENDTYPE_SELF);
				}
			}
		}break;
	default:
		{
			return PgIndun::VUpdate(pUnit, wType, pNfy);
		}break;
	}
	return true;
}

bool PgExpeditionGround::RecvMapMove(UNIT_PTR_ARRAY & UnitArray, SReqMapMove_MT & RMM
									, CONT_PET_MAPMOVE_DATA & ContPetMapMoveData
									, CONT_UNIT_SUMMONED_MAPMOVE_DATA & ContUnitSummonedMapMoveData
									, CONT_PLAYER_MODIFY_ORDER const & ModifyOrder)
{
	if ( INDUN_STATE_OPEN == m_eState )
	{
		VEC_GUID WaitList;
		VEC_JOININDUN JoinIndunList;
		UNIT_PTR_ARRAY::iterator unit_itr;
		for ( unit_itr = UnitArray.begin() ; unit_itr != UnitArray.end() ; ++unit_itr )
		{
			PgPlayer *pUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
			if( pUser )
			{
				if( pUser->HaveExpedition() )
				{
					WaitList.push_back(pUser->GetID());
					JoinIndunList.push_back( SNfyJoinIndun(pUser->GetID(), pUser->GetMemberGUID(), pUser->GetAbil64(AT_EXPERIENCE)) );
				}
			}
		}

		BM::Stream ResultMgrPacket(PT_M_T_ANS_CREATE_GROUND, GroundKey());
		PU::TWriteArray_A( ResultMgrPacket, JoinIndunList );
		ResultMgrPacket.Push(S_OK);
		::SendToChannelContents(PMET_Boss, ResultMgrPacket);

		/*BM::Stream kNPacket( PT_M_N_NFY_EXPEDITION_JOIN_PLAYER, GroundKey() );
		kNPacket.Push( JoinIndunList );
		::SendToResultMgr(kNPacket);*/
	}

	if( !PgIndun::RecvMapMove(UnitArray, RMM, ContPetMapMoveData, ContUnitSummonedMapMoveData, ModifyOrder) )
	{
		return false;
	}

	return true;
}

bool PgExpeditionGround::ReleaseUnit(CUnit * pUnit, bool bRecursiveCall, bool const bSendArea)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( UT_PLAYER == pUnit->UnitType() )
	{
		if ( INDUN_STATE_WAIT == GetState() ) 
		{
			ReleaseWaitUser( dynamic_cast<PgPlayer*>(pUnit) );
		}
		if( GetAttr() == GATTR_EXPEDITION_GROUND )
		{
			BM::Stream NPacket( PT_M_N_NFY_EXPEDITION_LEAVE_PLAYER, GroundKey() );
			NPacket.Push( pUnit->GetID() );
			SendToResultMgr(NPacket);
		}
	}

	return PgIndun::ReleaseUnit(pUnit, bRecursiveCall, bSendArea);
}

void PgExpeditionGround::RecvUnitDie(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgIndun::RecvUnitDie(pUnit);

	if( pUnit )
	{
		EUnitType UnitType = pUnit->UnitType();
		switch(UnitType)
		{
		case UT_PLAYER:
			{
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
				if( pPlayer )
				{
					if( pPlayer->HaveExpedition() )
					{
						BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
						Packet.Push(pPlayer->ExpeditionGuid());
						Packet.Push(pPlayer->GetID());
						Packet.Push(pPlayer->IsAlive());
						SendToGlobalPartyMgr(Packet);
					}
				}
			}break;
		case UT_BOSSMONSTER:
		case UT_MONSTER:
			{
				CUnit * pMonster = GetUnit(pUnit->GetID());
				if( pMonster )
				{
					switch( pMonster->GetAbil(AT_GRADE) )
					{
					case EMGRADE_BOSS:
					case EMGRADE_ELITE:
						{
							if( m_TimeLimit > 0 )	// 시간제한이 있는 경우에는 시간 카운트를 멈춰야한다.
							{
								m_IsEventScript = true; // 시간제한이 있을 때 정예 or 보스가 죽으면 이벤트 스크립트가 돌아감.
							}
						}break;
					default:
						{
						}break;
					}
				}
			}break;
		default:
			{
			}break;
		}
	}
}

HRESULT PgExpeditionGround::SetUnitDropItem(CUnit * pOwner, CUnit * pDroper, PgLogCont & LogCont )
{
	if ( INDUN_STATE_PLAY != m_eState )
	{// 그냥 S_OK로 리턴~
		return S_OK;
	}

	//Item & Gold
	switch( pDroper->GetAbil(AT_GRADE) )
	{
	case EMGRADE_BOSS:
		{			
			SetState(INDUN_STATE_RESULT_WAIT);

			SNfyResultItemList ResultItemList( pDroper->GetAbil(AT_MAX_DROP_ITEM_COUNT) );

			CONT_DEF_MAP_ITEM_BAG const * pContMapItemBag = NULL;
			g_kTblDataMgr.GetContDef(pContMapItemBag);
			if( NULL == pContMapItemBag )
			{
				return S_OK;
			}

			CONT_DEF_MAP_ITEM_BAG::const_iterator map_item_bag_iter = pContMapItemBag->find( GetMapItemBagGroundNo() );

			VEC_GUID Vec_List;
			CUnit * pUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_iter;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
			while( (pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
			{
				SNfyResultItem UserItem(pUser->GetID());
				if( map_item_bag_iter != pContMapItemBag->end() )
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
					if( pPlayer->HaveExpedition() )
					{
						if( pPlayer && (true == pPlayer->IsTakeUpItem()) )
						{
							SetBestResult(pPlayer);
							PgAction_DropItemBox DropItem(this, map_item_bag_iter->second);
							if( true == DropItem.DoAction(pDroper, pUser) )
							{
								CONT_MONSTER_DROP_ITEM::const_iterator bag_iter = DropItem.m_kContDropItem.begin();
								for( ; bag_iter != DropItem.m_kContDropItem.end() ; ++bag_iter)
								{
									if( bag_iter->ItemNo() )
									{
										UserItem.kItemList.push_back(*bag_iter);
									}
								}
							}
						}
					}
				}
				ResultItemList.push_back(UserItem);
			}

			CONT_SPECIALITEM SpecialItem;
			GetSpecialDropItem(pDroper, SpecialItem);

			if( !ResultItemList.empty() && !SpecialItem.empty() )
			{
				SendExpeditionResult(ResultItemList, SpecialItem);	// 결과를 보내고.

				PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
				while( (pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
					if( pPlayer )
					{	// 각 플레이어의 dp, hp 누적 점수를 초기화 한다.
						pPlayer->ClearAccmulateValue();
					}
				}
			}
			return S_FALSE;
		}break;
	default:
		{
			return PgGround::SetUnitDropItem(pOwner, pDroper, LogCont );
		}break;
	}
	return S_OK;
}

bool PgExpeditionGround::GetSpecialDropItem(CUnit * pDroper, CONT_SPECIALITEM & DropItem)
{
	CONT_OBJECT_MGR_UNIT::iterator unit_iter;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
	CUnit * pUser = pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter);

	PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUser);
	if( NULL == pPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	BM::GUID MasterGuid;
	m_kLocalPartyMgr.GetExpeditionMasterGuid(pPlayer->GetExpeditionGuid(), MasterGuid);

	PgPlayer * pMasterPlayer = GetUser(MasterGuid);
	if( NULL == pMasterPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CItemBagMgr, ItemBagMgr);

	CONT_DEF_ITEM_CONTAINER const *pContContainer = NULL;
	g_kTblDataMgr.GetContDef(pContContainer);

	int Level = pMasterPlayer->GetAbil(AT_LEVEL);

	int MinCount = std::max<int>(0, pDroper->GetAbil(AT_MIN_DROP_ITEM_COUNT_EXPENSION));
	int MaxCount = std::min<int>(50, pDroper->GetAbil(AT_MAX_DROP_ITEM_COUNT_EXPENSION));

	if( MaxCount <= 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( MinCount > MaxCount )
	{
		std::swap( MinCount, MaxCount );
	}

	int const RetCount = BM::Rand_Range(MaxCount, MinCount);
	bool const DropAllItem = (0 < pDroper->GetAbil(AT_MON_DROP_ALLITEM));

	CONT_DEF_ITEM_CONTAINER::const_iterator item_cont_iter = pContContainer->find(pDroper->GetAbil(AT_ITEM_DROP_CONTAINER_EXPENSION));
	if( item_cont_iter != pContContainer->end() )
	{
		if( DropAllItem )
		{	// ItemBag의 모든 아이템을 ADD
			for( size_t RetIdx = 0 ; RetIdx < MaxCount ; ++RetIdx )
			{
				PgItemBag ItemBag;
				int const ResultBagGroupNo = (*item_cont_iter).second.aiItemBagGrpNo[RetIdx];
				if( S_OK == ItemBagMgr.GetItemBagByGrp(ResultBagGroupNo, static_cast<short>(Level), ItemBag) )
				{
					int ResultItem = 0;
					size_t ResultItemCount = 1;
					for(int BagIndex = 0 ; BagIndex < ItemBag.GetElementsCount() ; ++BagIndex)
					{
						if( S_OK == ItemBag.PopItemToIndex(Level, ResultItem, ResultItemCount, BagIndex) )
						{
							if(ResultItemCount <= 0)
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Expedition Drop Item: ItemBag Element Item Count Error\n") << _T("ItemNo: ") << ResultItem << _T(", ItemCount: ") << ResultItemCount);
								return false;
							}
							PgBase_Item Item;
							if( S_OK == CreateSItem(ResultItem, 1, this->GetGroundItemRarityNo(), Item) )
							{
								//
								SSPECIALRESULTITEM SPItem(ResultBagGroupNo);
								SPItem.ResultItem = Item;
								DropItem.push_back(SPItem);
							}
						}
					}
				}
			}
		}
		else
		{
			for(int count = 0 ; count < RetCount ; ++count)
			{
				size_t RetIdx = 0;
				if( ::RouletteRate((*item_cont_iter).second.iSuccessRateControlNo, RetIdx, MAX_ITEM_CONTAINER_LIST) )
				{
					PgItemBag ItemBag;
					int const ResultBagGroupNo = (*item_cont_iter).second.aiItemBagGrpNo[RetIdx];
					if( S_OK == ItemBagMgr.GetItemBagByGrp(ResultBagGroupNo, static_cast<short>(Level), ItemBag) )
					{
						int ResultItem = 0;
						int itemcount = 1;
						if( S_OK == ItemBag.PopItem(Level, ResultItem, itemcount) )
						{
							PgBase_Item Item;
							if( S_OK == CreateSItem(ResultItem, itemcount, this->GetGroundItemRarityNo(), Item) )
							{
								//
								SSPECIALRESULTITEM SPItem(ResultBagGroupNo);
								SPItem.ResultItem = Item;
								DropItem.push_back(SPItem);
							}
						}
					}
				}
			}
		}
	}
	return true;
}

bool PgExpeditionGround::SetBestResult(PgPlayer * pPlayer)
{
	if( NULL == pPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int TeamNo = m_kLocalPartyMgr.GetMemberTeamNo(pPlayer->ExpeditionGuid(), pPlayer->GetID());
	int MaxTeamCount = m_kLocalPartyMgr.GetMaxTeamCount(pPlayer->ExpeditionGuid());
	if( (TeamNo <= 0)  || (TeamNo > MaxTeamCount) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int PrivateDP = pPlayer->GetAccruedDamage();
	auto Ret = m_TeamDP.insert( std::make_pair(TeamNo, PrivateDP) );
	if( !Ret.second )
	{
		(*Ret.first).second += PrivateDP;
	}
	
	// 베스트 플레이어.
	if( NULL != m_pBestPlayer )
	{
		if( m_pBestPlayer->GetAccruedDamage() < pPlayer->GetAccruedDamage() )
		{
			m_pBestPlayer = pPlayer;
		}
	}
	else
	{
		m_pBestPlayer = pPlayer;
	}
	
	// 베스트 서포터.
	if( NULL != m_pBestSupporter )
	{
		if( m_pBestSupporter->GetAccruedHeal() < pPlayer->GetAccruedHeal() )
		{
			m_pBestSupporter = pPlayer;
		}
	}
	else
	{
		m_pBestSupporter = pPlayer;
	}

	return true;
}

void PgExpeditionGround::SendExpeditionResult(SNfyResultItemList & ResultItem, CONT_SPECIALITEM & SpecialItemList)
{
	BM::Stream Packet(PT_M_N_NFY_EXPEDITION_RESULTITEM, GroundKey());

	if( !m_TeamDP.empty() )
	{
		// 베스트 팀 구하기.
		CONT_TEAM_DP::const_iterator best_iter = m_TeamDP.begin();
		CONT_TEAM_DP::const_iterator team_iter = m_TeamDP.begin();
		for( ; team_iter != m_TeamDP.end() ; ++team_iter )
		{
			if( best_iter->second < team_iter->second )
			{
				best_iter = team_iter;
			}
		}

		int BestTeamNo = best_iter->first;
		int BestTeamDP = best_iter->second;

		Packet.Push( BestTeamNo );	// 베스트 팀 번호.
		Packet.Push( BestTeamDP ); // 베스트 팀 DP.
	}
	else
	{
		Packet.Push(0);
		Packet.Push(0);
	}

	if( NULL != m_pBestPlayer
		&& (0 < m_pBestPlayer->GetAccruedDamage()) )
	{
		Packet.Push( m_pBestPlayer->GetID() );	// 베스트 플레이어 guid.
		Packet.Push( m_pBestPlayer->GetAccruedDamage() );		// 베스트 플레이어 DP.
	}
	else
	{
		Packet.Push(BM::GUID::NullData());
		Packet.Push(0);
	}

	if( (NULL != m_pBestSupporter)
		&& (0 < m_pBestSupporter->GetAccruedHeal()) )
	{
		Packet.Push( m_pBestSupporter->GetID() );			// 베스트 서포터 guid.
		Packet.Push( m_pBestSupporter->GetAccruedHeal() );	// 베스트 서포터 HP.
	}
	else
	{
		Packet.Push(BM::GUID::NullData());
		Packet.Push(0);
	}

	ResultItem.WriteFromPacket(Packet);			// 일반 보상 아이템.

	Packet.Push(SpecialItemList.size());			// 특별 보상 아이템.
	PU::TWriteArray_M(Packet, SpecialItemList);

	::SendToResultMgr(Packet);
}

HRESULT PgExpeditionGround::InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo, 
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller, 
		bool DropAllItem, int EnchantGradeNo, 
		ECREATE_HP_TYPE const Create_HP_Type)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( true == m_FailedTime )
	{	// 던전 공략시간이 다되어 실패 했으므로 더이상 몬스터를 추가할 필요 없음..
		return E_FAIL;
	}

	HRESULT const Ret = PgGround::InsertMonster(GenInfo, MonNo, OutGuid, Caller, DropAllItem, EnchantGradeNo, Create_HP_Type);
	if( SUCCEEDED(Ret) )
	{
		CUnit * pMonster = GetUnit(OutGuid);
		if( pMonster )
		{
			switch( pMonster->GetAbil(AT_GRADE) )
			{
			case EMGRADE_ELITE:
			case EMGRADE_BOSS:
				{
					DWORD const TimeLimit = pMonster->GetAbil(AT_TIME_LIMIT);
					if( 0 < TimeLimit )
					{
						m_TimeLimit = TimeLimit;
						m_RemainTime = m_TimeLimit;
						m_AccumLimitTime = 0;
						m_FailedTime = false;
					}
				}break;
			default:
				{
				}break;
			}
		}
	}

	return Ret;
}