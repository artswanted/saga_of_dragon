#include "stdafx.h"
#include "Variant/PgHardCoreDungeonParty.h"
#include "PgGround.h"

bool PgGround::Recv_PT_C_M_REQ_PET_ACTION( PgPlayer *pkOwner, BM::Stream * pkNfy )
{
	if ( pkOwner )
	{
		CUnit *pkPetUnit = PgObjectMgr2::GetUnit( UT_PET, pkOwner->CreatePetID() );
		if ( pkPetUnit )
		{
			OnReqItemAction( pkOwner, pkNfy, dynamic_cast<PgPet*>(pkPetUnit) );
		}
	}
	return false;
}

void PgGround::Recv_PT_C_M_REQ_JOIN_LOBBY( CUnit *pkUnit, BM::Stream &rkPacket )
{
	int iMode = 0;
	if ( true == rkPacket.Pop( iMode ) )
	{
		if( PgGroundUtil::SUST_PvPModeSelectting == GetSpecStatus(pkUnit->GetID() ) )
		{
			switch( iMode )
			{
			case PvP_Lobby_GroundNo_AnterRoom:
			case PvP_Lobby_GroundNo_Exercise:
			case PvP_Lobby_GroundNo_Ranking:
				{
					PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkUnit);
					if ( pkUser && pkUser->HaveParty() )
					{
						// 파티가 있으면 PvP에 진입할 수 없습니다
						pkUnit->SendWarnMessage(19013);
						return;
					}
				}// No Break!!!
			case PvP_Lobby_GroundNo_League:
				{
					SaveUnit( pkUnit ); // 지우지마세요!

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if( pkPlayer )
					{
						SReqMapMove_MT kRMM(MMET_PvP);
						kRMM.kTargetKey.GroundNo( iMode );
						PgReqMapMove kMapMove( this, kRMM, NULL );
						if( kMapMove.Add( pkPlayer ) )
						{
							kMapMove.DoAction();
						}
					}
				}break;
			case HardCoreDungeon:
				{
					SItemPos kKeyItemPos;
					rkPacket.Pop( kKeyItemPos );

					PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if ( !pkPlayer )
					{
						break;
					}

					bool const bHadParty = BM::GUID::IsNotNull(pkPlayer->PartyGuid());
					if ( bHadParty && !m_kLocalPartyMgr.IsMaster( pkPlayer->PartyGuid(), pkPlayer->GetID() ) )
					{
						break;
					}

					PgBase_Item kKeyItem;
					if ( S_OK == pkUnit->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
					{
						if ( !kKeyItem.IsUseTimeOut() )
						{
							GET_DEF(CItemDefMgr, kItemDefMgr);
							CItemDef const* pkItemDef = kItemDefMgr.GetDef(kKeyItem.ItemNo());
							if ( pkItemDef )
							{
								if ( UICT_HARDCORE_KEY == pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE) )
								{
									EnterSpecStatus( pkUnit->GetID(), PgGroundUtil::SUST_HardCoreDungeonVote, true );

									BM::Stream kReqPacket( PT_M_T_REQ_REGIST_HARDCORE_VOTE, g_kProcessCfg.ChannelNo() );
									GroundKey().WriteToPacket( kReqPacket );
									kReqPacket.Push( iMode );
									kReqPacket.Push( pkPlayer->GetID() );
									kReqPacket.Push( pkPlayer->GetAbil(AT_LEVEL) );
									kReqPacket.Push( bHadParty );
									::SendToHardCoreDungeonMgr( kReqPacket );
								}
							}
						}	
					}
				}break;
			}
		}
	}
}

void PgGround::Recv_PT_T_M_ANS_REGIST_HARDCORE_VOTE( BM::Stream &rkPacket )
{
	BM::GUID kCharGuid;
	int iError = 0;
	rkPacket.Pop( kCharGuid );
	rkPacket.Pop( iError );

	if ( iError )
	{
		PgPlayer * pkUser = GetUser( kCharGuid );
		if ( pkUser )
		{
			pkUser->SendWarnMessage( iError );
			ChangeSpecStatus( kCharGuid, PgGroundUtil::SUST_HardCoreDungeonVote, PgGroundUtil::SUST_PvPModeSelectting );
		}
	}
	else
	{
		T_GNDATTR const kGndAttr =  this->GetAttr();
		BYTE byRet = E_HCT_V_ERROR;
		
		PgPlayer * pkUser = GetUser( kCharGuid );
		if ( pkUser )
		{
			switch ( kGndAttr )
			{
			case GATTR_DEFAULT:
			case GATTR_VILLAGE:
			case GATTR_HIDDEN_F:
			case GATTR_CHAOS_F:
				{
					switch( GetSpecStatus( kCharGuid ) )
					{
					case PgGroundUtil::SUST_CashShop:
						{
							byRet = E_HCT_V_CASHSHOP;
						}break;
					case PgGroundUtil::SUST_OpenMarket:
						{
							byRet = E_HCT_V_OPENMARKET;
						}break;
					case PgGroundUtil::SUST_None:
						{
							FakeRemoveUnit( pkUser, FRT_HIDE_AND_NO_MSG_BUT_MAPMOVE );
						}// No break
					case PgGroundUtil::SUST_PvPModeSelectting:
						{
							EnterSpecStatus( kCharGuid, PgGroundUtil::SUST_HardCoreDungeonVote, true );
						}// No break
					case PgGroundUtil::SUST_HardCoreDungeonVote:
						{
							if ( BM::GUID::IsNotNull(pkUser->PartyGuid()) )
							{
								// 파티면 파티원의 수를 확인
								if ( 1 == m_kLocalPartyMgr.GetMemberCount( pkUser->PartyGuid() ) )
								{
									BM::Stream kReqPacket( PT_M_T_REQ_RET_HARDCORE_VOTE, pkUser->GetID() );
									kReqPacket.Push( static_cast<BYTE>(E_HCT_V_OK) );
									::SendToGlobalPartyMgr( kReqPacket );
								}
								else
								{
									BM::Stream kAnsPacket( PT_M_C_ANS_REGIST_HARDCORE_VOTE );
									kAnsPacket.Push( rkPacket );
									pkUser->Send( kAnsPacket );
								}
							}
							else
							{
								PgPartyContents_HardCoreDungeon kContents;
								kContents.ReadFromPacket( rkPacket );

								// 개인은 바로 입장
								ReqJoinHardCoreDungeon( pkUser, &(kContents.GetDungeonGndKey()) );
							}

							return; // return !!!!!!!!!
						}break;
					}
				}break;
			case GATTR_INSTANCE:
			case GATTR_BOSS:
			case GATTR_STATIC_DUNGEON:
			case GATTR_EXPEDITION_GROUND_BOSS:
				{
					byRet = E_HCT_V_INDUN;
				}break;
			case GATTR_MISSION:
			case GATTR_CHAOS_MISSION:
				{
					byRet = E_HCT_V_MISSION;
				}break;
			case GATTR_EMPORIA:
				{
					byRet = E_HCT_V_EMPORIA;
				}break;
			case GATTR_MYHOME:
			case GATTR_HOMETOWN:
				{
					byRet = E_HCT_V_MYHOME;
				}break;
			case GATTR_MARRY:
				{
					byRet = E_HCT_V_MARRY;
				}break;
			case GATTR_BATTLESQUARE:
				{
					byRet = E_HCT_V_BATTLESQUARE;
				}break;
			case GATTR_HARDCORE_DUNGEON:
			case GATTR_HARDCORE_DUNGEON_BOSS:
				{
					byRet = E_HCT_V_HARDCORE_DUNGEON;
				}break;
			default:
				{
					CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Error GroundAttr<" << kGndAttr << L"> CharGuid<" << kCharGuid << L">" << GroundKey().ToString() );
					byRet = E_HCT_V_ERROR;
				}break;
			}	
		}

		BM::Stream kReqPacket( PT_M_T_REQ_RET_HARDCORE_VOTE, kCharGuid );
		kReqPacket.Push( byRet );
		::SendToGlobalPartyMgr( kReqPacket );
	}
}

void PgGround::Recv_PT_C_M_REQ_RET_HARDCORE_VOTE( CUnit *pkUnit, BM::Stream &rkPacket )
{
	if( PgGroundUtil::SUST_HardCoreDungeonVote == GetSpecStatus(pkUnit->GetID() ) )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( pkPlayer )
		{
			bool bRet = false;
			rkPacket.Pop( bRet );

			if ( BM::GUID::IsNotNull(pkPlayer->PartyGuid()) )
			{
				BYTE const byState = ( bRet ? E_HCT_V_OK : E_HCT_V_CANCEL );

				BM::Stream kReqPacket( PT_M_T_REQ_RET_HARDCORE_VOTE, pkUnit->GetID() );
				kReqPacket.Push( byState );
				::SendToGlobalPartyMgr( kReqPacket );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"did not have party User<" << pkUnit->GetID() << L">" );
				bRet = false;
			}

			if ( !bRet )
			{
				LeaveSpecStatus( pkUnit->GetID(), PgGroundUtil::SUST_HardCoreDungeonVote );
				FakeAddUnit( pkUnit );
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"PgPlayer Casting Error!!! " << pkUnit->GetID() );
		}
	}
}

void PgGround::Recv_PT_T_M_ANS_RET_HARDCORE_VOTE_CANCEL( BM::Stream &rkPacket )
{
	BM::GUID kCharGuid;
	rkPacket.Pop( kCharGuid );

	CUnit * pkUnit = PgObjectMgr2::GetUnit( UT_PLAYER, kCharGuid );
	if ( pkUnit )
	{
		if ( true == LeaveSpecStatus( pkUnit->GetID(), PgGroundUtil::SUST_HardCoreDungeonVote ) )
		{
			BM::Stream kCPacket( PT_M_C_ANS_RET_HARDCORE_VOTE_CANCEL );
			pkUnit->Send( kCPacket );

			FakeAddUnit( pkUnit );
		}
	}
}

bool PgGround::ReqJoinHardCoreDungeon( PgPlayer * pkUser, SGroundKey const *pkDungeonGndKey )
{
	SReqMapMove_MT kRMM(MMET_GoToPublic_PartyOrder);
	if ( pkDungeonGndKey )
	{
		kRMM.cType = MMET_GoToPublicGround;
		kRMM.kTargetKey = *pkDungeonGndKey;
	}
	
	PgReqMapMove kMapMove( this, kRMM, NULL );
	if ( !kMapMove.Add( pkUser ) )
	{
		return false;
	}

	bool const bHasParty = BM::GUID::IsNotNull(pkUser->PartyGuid());

	if (	!bHasParty
		||	(true == m_kLocalPartyMgr.IsMaster( pkUser->PartyGuid(), pkUser->GetID() ))
		)
	{
		ContHaveItemNoCount	kItemCont;
		if( FAILED(pkUser->GetInven()->GetItems( UICT_HARDCORE_KEY, kItemCont, true ) ) )
		{
			return false;
		}

		bool bFindKeyItem = false;
		ContHaveItemNoCount::const_iterator itemno_itr = kItemCont.begin();
		for ( ; itemno_itr != kItemCont.end() ; ++itemno_itr )
		{
			SItemPos kKeyItemPos;
			if ( SUCCEEDED(pkUser->GetInven()->GetFirstItem( itemno_itr->first, kKeyItemPos, false, true )) )
			{
				PgBase_Item kKeyItem;
				if ( S_OK == pkUser->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
				{
					kMapMove.AddModifyOrder( SPMO(IMET_MODIFY_COUNT, pkUser->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -1)));
					bFindKeyItem = true;
					break;							
				}
			}
		}

		if ( !bFindKeyItem )
		{
			return false;
		}

	}// Is Master?

	pkUser->SetSyncType( SYNC_TYPE_DEFAULT, true );
	return kMapMove.DoAction();
}

void PgGround::Recv_PT_T_M_REQ_JOIN_HARDCORE( BM::Stream &rkPacket )
{
	BM::GUID kCharGuid;
	rkPacket.Pop( kCharGuid );

	PgPlayer * pkUser = GetUser( kCharGuid );
	if ( pkUser )
	{
		if ( true == ReqJoinHardCoreDungeon( pkUser, NULL ) )
		{
			return;//return !!
		}

		if ( true == LeaveSpecStatus( pkUser->GetID(), PgGroundUtil::SUST_HardCoreDungeonVote ) )
		{
			FakeAddUnit( pkUser );
		}
	}
		
	BM::Stream kAnsPacket( PT_M_T_ANS_JOIN_HARDCORE_FAILED, kCharGuid );
	::SendToGlobalPartyMgr( kAnsPacket );
}
