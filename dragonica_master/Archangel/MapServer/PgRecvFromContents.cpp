#include "stdafx.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/packetstruct4map.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgMission.h"
#include "Variant/GM_const.h"
#include "Variant/PgClassDefMgr.h"
#include "Variant/PgEventView.h"
#include "variant/PgMissionInfo.h"
#include "variant/PgMyQuest.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgBattleSquare.h"
#include "Variant/PgQuestInfo.h"
#include "PgTask_MapServer.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgIndun.h"
#include "PublicMap/PgWarGround.h"
#include "PublicMap/PgWarMode.h"
#include "PgMissionGround.h"
#include "PgAction.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PgItemTradeMgr.h"
#include "PgMissionMan.h"
#include "PgQuest.h"
#include "PgActionQuest.h"
#include "PgMarryMgr.h"
#include "PgEffectAbilTable.h"
#include "PgActionAchievement.h"
#include "PgAction_Pet.h"
#include "AilePack/NC_Constant.h"
#include "PgActionJobSkill.h"
#include "PgExpeditionGround.h"
#include "PgExpeditionLobby.h"
#include "PgStaticEventGround.h"
#include "PgConstellationGround.h"
#include "Variant/PgConstellation.h"

void PgGround::RecvGndWrapped(BM::Stream* const pkPacket )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	PACKET_ID_TYPE usType = 0;
	pkPacket->Pop(usType);

	if ( !this->RecvGndWrapped( usType, pkPacket ) )
	{
		this->RecvGndWrapped_ItemPacket(usType, pkPacket );
	}
}

bool PgGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch(usType)
	{
	case PT_N_C_ANS_PARTY_CHANGE_MASTER:
		{
			HRESULT		hChangeMasterRet;
			BM::GUID	kCharGuid;

			pkPacket->Pop(hChangeMasterRet);
			pkPacket->Pop(kCharGuid);

			CUnit *pkUnit = GetUnit(kCharGuid);
			if(!pkUnit)
			{
				return false;
			}

			if(PRC_Success == hChangeMasterRet)
			{
				PgAddAchievementValue kMA(AT_ACHIEVEMENT_PARTY_MASTER,1,GroundKey());
				kMA.DoAction(pkUnit,NULL);
			}
		}break;
	case PT_T_M_ANS_MOVETOSUMMONER:
		{
			BM::GUID	kCharGuid;
			SGroundKey	kCastGndKey;
			HRESULT		hResult = E_FAIL;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kCastGndKey);
			pkPacket->Pop(hResult);

			CUnit *pkUnit = GetUnit(kCharGuid);
			if(!pkUnit)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_ANS_MOVETOSUMMONER);
			kPacket.Push(hResult);
			pkUnit->Send(kPacket);
		}break;
	case PT_T_M_REQ_MOVETOSUMMONER:
		{
			BM::Stream kCopyPacket = *pkPacket;

			BM::GUID	kCharGuid,
						kMemberGuid;

			SGroundKey	kCastGndKey;
			SItemPos	kItemPos;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCastGndKey);
			pkPacket->Pop(kItemPos);

			CUnit *pkUnit = GetUnit(kMemberGuid);
			if (pkUnit)
			{
				PgAction_AnsMoveToSummoner kAction(this,GroundKey(),kCopyPacket);
				kAction.DoAction(pkUnit,NULL);
			}
			else
			{
				BM::Stream kPacket(PT_M_T_ANS_MOVETOSUMMONER);
				kPacket.Push(kCharGuid);
				kPacket.Push(kCastGndKey);
				kPacket.Push(E_NOT_FOUND_MEMBER);
				SendToCenter(kPacket);
			}
		}break;
	case PT_T_M_ANS_SUMMONPARTYMEMBER:
		{
			BM::GUID	kOwnerGuid;
			SGroundKey	kCastGndKey;
			HRESULT		hRet = E_FAIL;

			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kCastGndKey);
			pkPacket->Pop(hRet);

			BM::Stream kPacket(PT_M_C_ANS_SUMMONPARTYMEMBER);
			kPacket.Push(hRet);
			
			CUnit *pkUnit = GetUnit(kOwnerGuid);
			if (pkUnit)
			{
				pkUnit->Send(kPacket);
			}
		}break;
	case PT_T_M_REQ_SUMMONPARTYMEMBER:
		{
			BM::GUID kCharGuid,
					 kMemberGuid;

			SGroundKey kGndKey;
			SItemPos kItemPos;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kGndKey);
			pkPacket->Pop(kItemPos);

			HRESULT hRet = E_FAIL;

			CUnit *pkUnit = GetUnit(kMemberGuid);
			if (pkUnit && 
				!pkUnit->GetAbil(AT_FAKE_REMOVE_UNIT) && 
				(0 == (this->GetAttr() & GATTR_INSTANCE)) && 
				(pkUnit->GetState() != US_DEAD))
			{
				BM::Stream kPacket(PT_M_C_REQ_SUMMONPARTYMEMBER);
				kPacket.Push(kCharGuid);
				kPacket.Push(kGndKey);
				kPacket.Push(kItemPos);

				pkUnit->Send(kPacket);

				hRet = S_OK;
			}
			else
			{
				hRet = E_CANNOT_MOVE_MAP;
			}

			BM::Stream kPacket(PT_M_T_ANS_SUMMONPARTYMEMBER);
			kPacket.Push(kCharGuid);
			kPacket.Push(kGndKey);
			kPacket.Push(hRet);
			SendToCenter(kPacket);
		}break;
	case PT_T_M_ANS_PARTYMEMBERPOS:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			CUnit * pkCaster = GetUnit(kOwnerGuid);
			if(!pkCaster)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ture"));
				break;
			}

			PgAction_AnsMoveToPartyMember kAction(this,GroundKey(),*pkPacket);
			kAction.DoAction(pkCaster,NULL);
		}break;
	case PT_T_M_ANS_PARTYMASTERGROUNDKEY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			CUnit* pkCaster = GetUnit(kOwnerGuid);
			if( !pkCaster )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ture"));
				break;
			}

			PgAction_AnsMoveToPartyMemberGround kAction(this, GroundKey(), this->m_kLocalPartyMgr, *pkPacket);
			kAction.DoAction(pkCaster, NULL);
		}break;
	case PT_T_M_REQ_PARTYMEMBERPOS:
		{
			BM::GUID kCharGuid,
					 kMemberGuid;

			SGroundKey kGndKey;
			SItemPos kItemPos;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kGndKey);

			BM::Stream kPacket(PT_M_T_ANS_PARTYMEMBERPOS);

			CUnit *pkUnit = GetUnit(kMemberGuid);
			if ( pkUnit && (0 == (this->GetAttr() & GATTR_INSTANCE)))
			{
				kPacket.Push(kCharGuid);
				kPacket.Push(kGndKey);
				kPacket.Push(S_OK);
				kPacket.Push(GroundKey());
				kPacket.Push(pkUnit->GetPos());
				kPacket.Push(*pkPacket);
				SendToCenter(kPacket);
			}
			else
			{
				kPacket.Push(kCharGuid);
				kPacket.Push(kGndKey);
				kPacket.Push(E_CANNOT_MOVE_MAP);
				SendToCenter(kPacket);
			}
		}break;
	case PT_T_M_REQ_PARTYMASTERGROUNDKEY:
		{
			BM::GUID kCharGuid, kMemberGuid;
			SGroundKey kGndKey;
			HRESULT hResult;
			BM::GUID kTowerGuid;
			TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kGndKey);
			pkPacket->Pop(hResult);
			pkPacket->Pop(kTowerGuid);
			pkPacket->Pop(kTargetKey);

			BM::Stream kPacket(PT_M_T_ANS_PARTYMASTERGROUNDKEY);
			CUnit* pkUnit = GetUnit(kMemberGuid);
			if( pkUnit )
			{
				kPacket.Push(kCharGuid);
				kPacket.Push(kGndKey);
				kPacket.Push(S_OK);

				if( S_OK == hResult )
				{
					kPacket.Push(GroundKey());
					kPacket.Push(pkUnit->GetPos());
				}
				else
				{
					SGroundKey kKey;
					PgPlayer* pkUser = dynamic_cast<PgPlayer*>(pkUnit);
					pkUser->GetRecentGround( kKey, GATTR_DEFAULT );
					kPacket.Push(kKey);
					kPacket.Push(pkUser->GetRecentPos(GATTR_DEFAULT));
				}
				kPacket.Push(kTowerGuid);
				kPacket.Push(kTargetKey);
				kPacket.Push(*pkPacket);
				SendToCenter(kPacket);
			}
			else
			{
				kPacket.Push(kCharGuid);
				kPacket.Push(kGndKey);
				kPacket.Push(E_CANNOT_MOVE_MAP);
				SendToCenter(kPacket);
			}
		}break;
	case PT_T_C_NFY_BULLHORN:
		{
			BM::Stream kPacket(PT_T_C_NFY_BULLHORN,*pkPacket);
			Broadcast(kPacket,BM::GUID::NullData());
		}break;
	case PT_T_C_NFY_NOTICE:
	case PT_N_C_NFY_NOTICE_PACKET:
	case PT_T_C_NFY_MARRY:
		{
			// 가지고 있는 모든 유저에게 공지를 보낸다.
			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_GROUND_MGR);
			kEventMsg.SecType(usType);
			kEventMsg.Push(*pkPacket);
			g_kTask.PutMsg(kEventMsg);
		}break;
	case PT_A_M_NFY_REQ_MAP_MOVE:// 맵이동 호출 요청을 받은 거임
		{
			SReqMapMove_CM kRMMC;
 			BM::GUID kCharGuid;

			kRMMC.ReadFromPacket( *pkPacket );
			pkPacket->Pop( kCharGuid );
			RecvMapMoveCome( kRMMC, kCharGuid );
		}break;
	case PT_T_M_REQ_MAP_MOVE:
		{
 			SReqMapMove_MT kRMM;
			pkPacket->Pop(kRMM);

			UNIT_PTR_ARRAY kUnitArray;
			kUnitArray.ReadFromPacket(*pkPacket);

			CONT_PET_MAPMOVE_DATA kContPetMapMoveData;
			PU::TLoadTable_AM( *pkPacket, kContPetMapMoveData );

			CONT_UNIT_SUMMONED_MAPMOVE_DATA kContUnitSummonedMapMoveData;
			PU::TLoadTable_AM( *pkPacket, kContUnitSummonedMapMoveData );

			CONT_PLAYER_MODIFY_ORDER kModifyOrder;
			kModifyOrder.ReadFromPacket( *pkPacket );

			this->RecvMapMove( kUnitArray, kRMM, kContPetMapMoveData, kContUnitSummonedMapMoveData, kModifyOrder );
		}break;
	case PT_T_M_ANS_MAP_MOVE_COME_FAILED:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);
			this->RecvMapMoveComeFailed( kCharGuid );
		}break;
	case PT_I_M_ANS_MYHOME_ENTER:
		{
			BM::GUID kOwnerGuid;
			SReqMapMove_MT kRMM;

			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kRMM);

			if ( !g_kProcessCfg.IsPublicChannel() )
			{
				kRMM.cType = MMET_GoToPublicGround;
			}

			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(pkPlayer)
			{
				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					kMapMove.DoAction();
				}
			}
		}break;
	case PT_N_M_ANS_MAP_MOVE_CHECK:
		{
			BM::GUID kCharGuid;
			SReqMapMove_MT kRMM;
			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( kRMM );

			PgPlayer *pkPlayer = GetUser(kCharGuid);
			if ( pkPlayer )
			{
				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					kMapMove.DoAction();
				}
			}
		}break;
	case PT_T_M_REQ_MAP_MOVE_TARGET:
		{
			BM::GUID kTargetCharGuid;
			BM::GUID kReqCharGuid;
			bool bGmCommand = false;
			pkPacket->Pop( kTargetCharGuid );
			pkPacket->Pop( kReqCharGuid );
			pkPacket->Pop( bGmCommand );
			RecvMapMoveTarget( kTargetCharGuid, kReqCharGuid, bGmCommand );
		}break;
	case PT_T_M_ANS_USEITEM_CHECK:
		{
			BM::GUID kCharGuid;
			SItemPos kCasterItemPos;
			int iErrorMsg = 0;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kCasterItemPos);
			pkPacket->Pop(iErrorMsg);
			CUnit *pkUnit = GetUnit(kCharGuid);
			if ( pkUnit )
			{
				if ( iErrorMsg )
				{
					pkUnit->SendWarnMessage(iErrorMsg);
				}
				else
				{
					BM::Stream kEmptyPacket;
					PgAction_ReqUseItem kAction(kCasterItemPos, this, true, kEmptyPacket);
					kAction.DoAction( pkUnit, pkUnit );
				}
			}
		}break;
	case PT_T_M_REQ_JOIN_PARTY_MAP_MOVE:
		{
			BM::GUID Rquester, PartyMasterGuid;
			pkPacket->Pop(Rquester);
			pkPacket->Pop(PartyMasterGuid);

			RecvMapMoveTarget(PartyMasterGuid, Rquester, false);
		}break;
	case PT_N_M_NFY_SYNC_EXPEDITION_MEMBER:
		{
			BM::GUID ExpeditionGuid;
			pkPacket->Pop(ExpeditionGuid);
			bool const bUpdated = m_kLocalPartyMgr.SyncFromContentsExpedition(ExpeditionGuid, *pkPacket);
			VEC_GUID GuidVec;
			m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), GuidVec);

			SetExpeditionGuid(GuidVec, ExpeditionGuid, PARTY_SYS_EXPEDITION);

			// 원정대는 무조건 업데이트 하자.
			BM::Stream NfyPacket;
			m_kLocalPartyMgr.WriteToPacketExpeditionName(ExpeditionGuid, NfyPacket);

			if( GetAttr() & GKIND_EXPEDITION_LOBBY )
			{	// 입장 아이템 보유 여부를 보내줌.
				VEC_GUID HaveNotItemCharList;
				PgExpeditionLobby * pGround = dynamic_cast<PgExpeditionLobby*>(this);
				if( pGround )
				{
					pGround->CheckHaveKeyItem(ExpeditionGuid, GuidVec, HaveNotItemCharList);
				}

				if( !HaveNotItemCharList.empty() )
				{
					PU::TWriteArray_A(NfyPacket, HaveNotItemCharList);
				}
			}

			SendToUser_ByGuidVec(GuidVec, NfyPacket, E_SENDTYPE_BROADALL);
		}break;
	case PT_N_M_NFY_SYNC_PARTY_MEMBER:
		{
			BM::GUID kPartyGuid;

			pkPacket->Pop( kPartyGuid );

			bool const bUpdated = m_kLocalPartyMgr.SyncFromContents(kPartyGuid, *pkPacket);

			VEC_GUID kGuidVec;
			m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kGuidVec);

			SetPartyGuid(kGuidVec, kPartyGuid, PARTY_SYS_DEFAULT);

			if( bUpdated )
			{
				BM::Stream kNfyPacket;
				m_kLocalPartyMgr.WriteToPacketPartyName(kPartyGuid, kNfyPacket);
				SendToUser_ByGuidVec(kGuidVec, kNfyPacket, E_SENDTYPE_BROADALL);
			}
		}break;
	case PT_N_M_NFY_DELETE_EXPEDITION:
	case PT_N_M_NFY_DISPERSE_EXPEDITION:
	case PT_N_M_NFY_CHANGEMASTER_EXPEDITION:
	case PT_N_M_NFY_RENAME_EXPEDITION:
	case PT_N_M_NFY_LEAVE_EXPEDITION:
	case PT_N_M_NFY_EXPEDITION_USER_MAP_MOVE:
		{
			size_t const iPrevReadPos = pkPacket->RdPos();
			m_kLocalPartyMgr.ProcessMsg(usType, GroundKey(), pkPacket);
			pkPacket->RdPos(iPrevReadPos);
			switch( usType )
			{
				case PT_N_M_NFY_RENAME_EXPEDITION:
				case PT_N_M_NFY_CHANGEMASTER_EXPEDITION:
					{
						BM::GUID ExpeditionGuid;
						pkPacket->Pop(ExpeditionGuid);

						VEC_GUID GuidVec;
						m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), GuidVec);
						
						BM::Stream Packet;
						m_kLocalPartyMgr.WriteToPacketExpeditionName(ExpeditionGuid, Packet);
						SendToUser_ByGuidVec(GuidVec, Packet, E_SENDTYPE_BROADALL);
					}break;
				case PT_N_M_NFY_DELETE_EXPEDITION:
					{
						BM::GUID ExpeditionGuid;

						pkPacket->Pop( ExpeditionGuid );

						VEC_GUID GuidVec;
						m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), GuidVec);
						SetExpeditionGuid(GuidVec, BM::GUID::NullData(), PARTY_SYS_EXPEDITION );
					}break;
				case PT_N_M_NFY_DISPERSE_EXPEDITION:
					{
						BM::GUID MasterGuid, ExpeditionGuid;
						pkPacket->Pop(MasterGuid);
						pkPacket->Pop(ExpeditionGuid);

						BM::Stream NfyPacket;
						m_kLocalPartyMgr.WriteToPacketExpeditionName(ExpeditionGuid, NfyPacket);

						VEC_GUID GuidVec;
						m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), GuidVec);

						SetPartyGuid(GuidVec, BM::GUID::NullData(), PARTY_SYS_DELETE );
						SetExpeditionGuid(GuidVec, BM::GUID::NullData(), PARTY_SYS_EXPEDITION_DELTE );
						SendToUser_ByGuidVec(GuidVec, NfyPacket, E_SENDTYPE_BROADALL);
					}break;
				case PT_N_M_NFY_LEAVE_EXPEDITION:
					{
						BM::GUID ExpeditionGuid, LeaverGuid;

						pkPacket->Pop(ExpeditionGuid);
						pkPacket->Pop(LeaverGuid);

						CUnit* pUnit = GetUnit( LeaverGuid );
						if( pUnit )
						{
							SetExpeditionGuid( pUnit, BM::GUID::NullData(), PARTY_SYS_EXPEDITION_DELTE);

							PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
							if( pPlayer )
							{
								pPlayer->ClearAccmulateValue();
							}
						}

						VEC_GUID GuidVec;
						m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), GuidVec);

						BM::Stream NfyPacket;
						m_kLocalPartyMgr.WriteToPacketExpeditionName(ExpeditionGuid, NfyPacket);
						SendToUser_ByGuidVec(GuidVec, NfyPacket, E_SENDTYPE_BROADALL);
					}break;
			}
		}break;
	case PT_N_M_NFY_LEAVE_PARTY_USER:
	case PT_N_M_NFY_PARTY_USER_MAP_MOVE:
	case PT_N_M_NFY_DELETE_PARTY:
	case PT_N_M_NFY_PARTY_CHANGE_MASTER:
	case PT_N_M_NFY_PARTY_RENAME:
		{
			size_t const iPrevReadPos = pkPacket->RdPos();
			m_kLocalPartyMgr.ProcessMsg(usType, GroundKey(), pkPacket);
			pkPacket->RdPos(iPrevReadPos);

			switch( usType )
			{
				case PT_N_M_NFY_PARTY_RENAME:
				case PT_N_M_NFY_PARTY_CHANGE_MASTER:
					{
						BM::GUID kPartyGuid;

						pkPacket->Pop(kPartyGuid);

						VEC_GUID kGuidVec;
						m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kGuidVec);

						BM::Stream kNfyPacket;
						m_kLocalPartyMgr.WriteToPacketPartyName(kPartyGuid, kNfyPacket);
						SendToUser_ByGuidVec(kGuidVec, kNfyPacket, E_SENDTYPE_BROADALL);
					}break;
				case PT_N_M_NFY_LEAVE_PARTY_USER:
					{
						BM::GUID kPartyGuid;
						BM::GUID kLeaverGuid;

						pkPacket->Pop( kPartyGuid );
						pkPacket->Pop( kLeaverGuid );

						CUnit* pkUnit = GetUnit( kLeaverGuid );
						if( pkUnit )
						{
							if ( true == LeaveSpecStatus( kLeaverGuid, PgGroundUtil::SUST_HardCoreDungeonVote ) )
							{
								FakeAddUnit( pkUnit );
							}

							SetPartyGuid( pkUnit, BM::GUID::NullData(), PARTY_SYS_DEFAULT );
						}

						BM::Stream kNfyPacket;
						m_kLocalPartyMgr.WriteToPacketPartyName(kPartyGuid, kNfyPacket);

						VEC_GUID kGuidVec;
						m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kGuidVec);
						SendToUser_ByGuidVec(kGuidVec, kNfyPacket, E_SENDTYPE_BROADALL); // 파티원에게
					}break;
				case PT_N_M_NFY_DELETE_PARTY:
					{
						BM::GUID kPartyGuid;

						pkPacket->Pop( kPartyGuid );

						VEC_GUID kGuidVec;
						m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kGuidVec);
						SetPartyGuid(kGuidVec, BM::GUID::NullData(), PARTY_SYS_DELETE );
						
						if( static_cast<int>(PI_NONE) != DefenceIsJoinParty(kPartyGuid) )
						{
							DefenceDelWaitParty(kPartyGuid);

							BYTE const byLevel = EL_Warning;
							BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, 401128 );
							kPacket.Push(byLevel);
							SendToUser_ByGuidVec(kGuidVec, kPacket, E_SENDTYPE_SELF);
						}
					}break;
			}
		}break;
	case PT_N_M_ANS_PARTY_COMMAND:
		{
			BM::GUID kCharGuid;
			BYTE cPartyCommand = 0;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(cPartyCommand);

			switch(cPartyCommand)
			{
			case PC_Summon_Member:
				{
					VEC_GUID kCharGuids;
					pkPacket->Pop(kCharGuids);

					CUnit* pkCaster = GetUnit(kCharGuid);
					if( pkCaster
					&&	!kCharGuids.empty() )
					{
						SReqMapMove_CM kRMMC( MMET_PartyWarp );
						kRMMC.pt3TargetPos = pkCaster->GetPos();
						kRMMC.kGndKey = GroundKey();
						kRMMC.nPortalNo = 0;
						ReqMapMoveCome( kRMMC, kCharGuids );
					}
				}break;
			}
		}break;
	case PT_N_M_REQ_PARTY_BUFF:
		{
			int iMasterGroundNo = 0;
			BM::GUID rkPartyGuid;
			BM::GUID kCharGuid;
			bool bBuff = false;

			pkPacket->Pop(bBuff);
			pkPacket->Pop(iMasterGroundNo);
			pkPacket->Pop(rkPartyGuid);
			pkPacket->Pop(kCharGuid);

			CUnit* pkUnit = GetUnit(kCharGuid);

			PartyBuffAction(iMasterGroundNo, rkPartyGuid, kCharGuid, pkUnit, bBuff, pkPacket);
		}break;
	case PT_N_M_ANS_ENTER_MISSION:
		{
			SReqMapMove_MT kRMM(MMET_Mission);
			kRMM.nTargetPortal = 1;

			BM::GUID kCharGuid;
			int iMissionNo;
			int iStageCount;
			int iMissionLevel = 0;
			int iTriggerType = 0;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(iMissionNo);
			pkPacket->Pop(iStageCount);
			pkPacket->Pop(kRMM.kTargetKey);
			pkPacket->Pop(iMissionLevel);
			pkPacket->Pop(iTriggerType);
			

			if( DEFENCE8_MISSION_LEVEL == (iMissionLevel+1) )
			{
				kRMM.cType = MMET_MissionDefence8;
			}

			bool bRet = true;
			PgPlayer *pkPlayer = GetUser(kCharGuid);
			if ( pkPlayer )
			{
				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					if ( m_kLocalPartyMgr.IsMaster( pkPlayer->PartyGuid(), pkPlayer->GetID()) )				
					{
						VEC_GUID kPartyList;
						if ( m_kLocalPartyMgr.GetPartyMemberGround( pkPlayer->PartyGuid(), GroundKey(), kPartyList, pkPlayer->GetID() ) )
						{
							VEC_GUID::const_iterator guid_itr = kPartyList.begin();
							for ( ; guid_itr != kPartyList.end() ; ++guid_itr )
							{
								PgPlayer *pkMemberPlayer = GetUser( *guid_itr );
								if ( pkMemberPlayer )
								{
									if ( INT_MAX > pkMemberPlayer->GetAbil( AT_MISSION_THROWUP_PENALTY ) )
									{// 패널티가 없는 놈만 데리고 들어 간다.
										kMapMove.Add( pkMemberPlayer );
									}
								}
							}
						}

						if( DEFENCE8_MISSION_LEVEL == (iMissionLevel+1) )
						{
							BM::GUID kMatchPartyGuid;
							if( true == GetDefenceModeMatchParty(pkPlayer->PartyGuid(), kMatchPartyGuid) )
							{
								// 상대방 파티가 있다.
								VEC_GUID kPartyList;
								if ( m_kLocalPartyMgr.GetPartyMemberGround( kMatchPartyGuid, GroundKey(), kPartyList ) )
								{
									VEC_GUID::const_iterator guid_itr = kPartyList.begin();
									for ( ; guid_itr != kPartyList.end() ; ++guid_itr )
									{
										PgPlayer *pkMemberPlayer = GetUser( *guid_itr );
										if ( pkMemberPlayer )
										{
											if ( INT_MAX > pkMemberPlayer->GetAbil( AT_MISSION_THROWUP_PENALTY ) )
											{// 패널티가 없는 놈만 데리고 들어 간다.
												kMapMove.Add( pkMemberPlayer );
											}
										}
									}
								}
								// 상대방 파티 아이템 깍기
								BM::GUID kOtherMasterGuid;
								if( m_kLocalPartyMgr.GetPartyMasterGuid(kMatchPartyGuid, kOtherMasterGuid) )
								{
									PgPlayer *pkMasterPlayer = GetUser(kOtherMasterGuid);
									if( pkMasterPlayer )
									{
										int iSubCount = 1;
										size_t iCount = m_kLocalPartyMgr.GetMemberCount(kMatchPartyGuid);
										if( 2 < iCount )
										{
											iSubCount = 2;											
										}
										//SPMO kIMO;
										if( true == MissionItemOrderCheck(pkMasterPlayer, kMapMove, iMissionNo, iSubCount) )
										{
											//kMapMove.AddModifyOrder(kIMO);
										}
										else
										{
											pkMasterPlayer->SendWarnMessage2(400976, iSubCount);

											// 입장 실패시
											BM::Stream kFailedPacket(PT_M_N_ANS_ENTER_MISSION_FAILED, kRMM.kTargetKey.Guid() );
											SendToMissionMgr( kFailedPacket );
											break;
										}
									}
									else
									{
										// 상대편 파티장이 같은 위치에 존재해야 된다.(메시지)
										if( pkPlayer )
										{
											BM::Stream kCPacket( PT_NFY_M_C_DEFENCE_NOTICE_MSG );
											kCPacket.Push( 401129 );
											pkPlayer->Send( kCPacket );
										}

										// 입장 실패시
										BM::Stream kFailedPacket(PT_M_N_ANS_ENTER_MISSION_FAILED, kRMM.kTargetKey.Guid() );
										SendToMissionMgr( kFailedPacket );
										break;
									}
								}
							}
							else
							{
								// 대기 상태로 유지 시킨다.
								if( BM::GUID::IsNotNull(pkPlayer->PartyGuid()) )
								{
									if( static_cast<int>(PI_NONE) == m_kDefencePartyMgr.IsJoinParty(pkPlayer->PartyGuid()) )
									{
										// 등록이 안된 파티
										if( true == DefenceAddWaitParty(pkPlayer->PartyGuid(), static_cast<int>(PI_WAIT)) )
										{
											// 등록 완료
											BM::Stream kCPacket( PT_NFY_M_C_DEFENCE_NOTICE_MSG );
											kCPacket.Push( 401130 );
											pkPlayer->Send( kCPacket );
										}
										else
										{
											// 이미 등록되어져 있으며, 대기 또는 전쟁중인 경우( PI_WAIT or PI_PLAY )
											// 오류 메시지 출력 
											pkPlayer->SendWarnMessage(401131);
										}
									}
									else
									{
										// 이미 등록되어져 있으며, 대기 또는 전쟁중인 경우( PI_WAIT or PI_PLAY )
										// 오류 메시지 출력 
										pkPlayer->SendWarnMessage(401131);
									}
								}
								else
								{
									// 파티상태가 아니므로 대전 디팬스 모드에 진입이 불가능(메시지)
									pkPlayer->SendWarnMessage(401132);
								}
								bRet = false;
							}
						}
					}
					else
					{
						if( DEFENCE8_MISSION_LEVEL == (iMissionLevel+1) )
						{
							// 대전 미션에서는 파티 안하면 안된다.
							bRet = false;
						}
					}
					if(MAX_MISSION_LEVEL == (iMissionLevel+1))
					{//f5히든 모드일경우
						int const iMissionLevelHiden = (iMissionLevel+1);
						if( true == MissionItemOrderCheck(pkPlayer, kMapMove, iMissionNo, 1,iMissionLevelHiden))
						{
							//kMapMove.AddModifyOrder(kIMO);
						}
						else
						{
							pkPlayer->SendWarnMessage2(460034, 1);

							bRet = false;
						}
					}

					if( DEFENCE_MISSION_LEVEL == (iMissionLevel+1) )
					{
						// 디팬스 모드일 경우
						//SPMO kIMO;
						if( true == MissionItemOrderCheck(pkPlayer, kMapMove, iMissionNo, 1) )
						{
							//kMapMove.AddModifyOrder(kIMO);
						}
						else
						{
							pkPlayer->SendWarnMessage2(400976, 1);

							bRet = false;
						}
					}
					else if( DEFENCE7_MISSION_LEVEL == (iMissionLevel+1) )
					{
						int iSubCount = 1;
						size_t iCount = m_kLocalPartyMgr.GetMemberCount(pkPlayer->PartyGuid());
						if( 2 < iCount )
						{
							iSubCount = 2;											
						}
						//SPMO kIMO;
						if( true == MissionItemOrderCheck(pkPlayer, kMapMove, iMissionNo, iSubCount) )
						{
							//kMapMove.AddModifyOrder(kIMO);
						}
						else
						{
							pkPlayer->SendWarnMessage2(400976, iSubCount);

							bRet = false;
						}
					}
					else if( MILT_LEVLE2 == (iMissionLevel+1) )
					{		
						if( (MT_EVENT_MISSION != iTriggerType) && (MT_EVENT_HIDDEN != iTriggerType) )
						{
							if( false == MissionChaosItemOrderCheck(pkPlayer, kMapMove, iMissionNo, 1) )
							{
								pkPlayer->SendWarnMessage(401194);
								bRet = false;
							}
						}
					}

					bRet = bRet & IsDefenceMissionLevelCheck(pkPlayer, this, iMissionNo, iMissionLevel);

					if( false == bRet )
					{
						BM::Stream kFailedPacket(PT_M_N_ANS_ENTER_MISSION_FAILED, kRMM.kTargetKey.Guid() );
						SendToMissionMgr( kFailedPacket );
						break;
					}

					kMapMove.DoAction();
				}
				else
				{
					BM::Stream kFailedPacket(PT_M_N_ANS_ENTER_MISSION_FAILED, kRMM.kTargetKey.Guid() );
					SendToMissionMgr( kFailedPacket );
				}
			}
			else
			{
				BM::Stream kFailedPacket(PT_M_N_ANS_ENTER_MISSION_FAILED, kRMM.kTargetKey.Guid() );
				SendToMissionMgr( kFailedPacket );
			}
		}break;
	case PT_T_M_NFY_DEFENCE_DELETE_PARTYINFO:
		{
			BM::GUID kPartyGuid;

			pkPacket->Pop(kPartyGuid);

			//if( static_cast<int>(PI_NONE) != pkGround->DefenceIsJoinParty(kPartyGuid) )
			{
				if( DefenceDelWaitParty(kPartyGuid) )
				{
					// 대기 파티 삭제
				}
			}
		}break;
	case PT_T_M_NFY_DEFENCE_PLAY_PARTYINFO:
		{
			BM::GUID kPartyGuid;

			pkPacket->Pop(kPartyGuid);

			bool bRet = false;
			if( static_cast<int>(PI_NONE) == DefenceIsJoinParty(kPartyGuid) )
			{
				bRet = DefenceAddWaitParty(kPartyGuid, static_cast<int>(PI_PLAY));
			}
			else
			{
				bRet = DefenceModifyWaitParty(kPartyGuid, PI_PLAY);
			}
		}break;
	case PT_N_M_ANS_MISSION_INFO:
		{
			PgPlayer_MissionData PlayerMissionData;
			BM::GUID kMemGuid;
			SMissionInfo kMission;
			bool bClearLevel[MAX_MISSION_LEVEL];
			EGTriggerType iType;
			VEC_INT	VecHiden5ClearQuestvalue;
			VEC_INT VecNeedItem;
			VEC_INT kContRequiredItem;
			
			PlayerMissionData.ReadFromPacket(*pkPacket);
			kMission.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bClearLevel);
			pkPacket->Pop(kMemGuid);
			pkPacket->Pop(iType);

			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(kMemGuid) );
			if( pkPlayer )
			{
				PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
				if( pkMyQuest )
				{
					for( int i=0; i!=MAX_MISSION_LEVEL; ++i )
					{
						int m_iMissionNo = kMission.m_iMissionNo;

						PgMissionInfo const * pkMissionInfo = NULL;
						bool bRet = g_kMissionMan.GetMission( m_iMissionNo, pkMissionInfo );
						if( bRet )
						{
							const SMissionOptionMissionOpen* pkMissionOpen = NULL;
							if( false == pkMissionInfo->GetMissionOpen(i, pkMissionOpen) )
							{
								VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"This Server Don't Have MissionLevel["<< i <<L"], Get MissionInfo Null");
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMission is Failed!"));
								break;
							}	

							VEC_INT	VecIngQuestValue;
							VecIngQuestValue.clear();
							VecHiden5ClearQuestvalue.clear();
							VecNeedItem.clear();
							pkMissionOpen->GetIngQuestIDVec(VecIngQuestValue);
							pkMissionOpen->GetClearQuestIDVec(VecHiden5ClearQuestvalue);
							pkMissionOpen->GetNeedItem(VecNeedItem);
							kContRequiredItem.push_back(pkMissionOpen->iNeedItem);

							bool const bIngRet = pkMyQuest->IsIngQuestVec(VecIngQuestValue);
							bool const bEndRet = pkMyQuest->IsEndedQuestVec(VecIngQuestValue);
							if( pkMissionOpen->PreLevelCheck(bClearLevel) )
							{
								if( bEndRet && VecIngQuestValue.size() && i == 4 )
								{
									kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLEARED;
								}
								if( (false == (bIngRet || bEndRet) && VecIngQuestValue.size() != 0) )
								{
									if( i == 4 ) // 4, 5 시나리오 완료 등록을 하지 않아서...
									{
										if( bEndRet )
										{
											kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLEARED;
										}
										else
										{
											kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
										}
									}
									else
									{
										if( kMission.m_kDoor[i] != SMissionInfo::MDOOR_CLEARED )
										{
											kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
										}
									}
								}
							}
							else
							{
								kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
							}

							if( kMission.m_kDoor[i] != SMissionInfo::MDOOR_CLEARED )
							{
								// Clear가 되지 않은 경우만 Quest 체크한다.
								if( false == (bIngRet || bEndRet) && ( VecIngQuestValue.size() != 0))
								{
									kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
								}
												
								VEC_INT	VecEffectValue;
								VecEffectValue.clear();
								pkMissionOpen->GetEffectNo(VecEffectValue);
								CUnit *pkUnit = GetUnit(kMemGuid);
								if( pkUnit )
								{
									PgAction_MissionEffectCheck kCheckAction(VecEffectValue, this, GroundKey());
									if( false == kCheckAction.DoAction( pkUnit, NULL ) )
									{
										kMission.m_kDoor[i] = SMissionInfo::MDOOR_CLOSE;
									}
								}
							}
							//히든f5모드 체크하는부분.
							if((VecHiden5ClearQuestvalue.size() != 0) 
								&& (VecNeedItem.size() != 0)
								&& (true == bEndRet ) )
							{
								for(int iIngQuest=0;iIngQuest<VecIngQuestValue.size();++iIngQuest)
								{
									for(int iHiden5ClearQuest=0;iHiden5ClearQuest < VecHiden5ClearQuestvalue.size();++iHiden5ClearQuest)
									{
										if(VecIngQuestValue.at(iIngQuest) == VecHiden5ClearQuestvalue.at(iHiden5ClearQuest))
										{
											kMission.m_kDoor[i] = SMissionInfo::MDOOR_NEWOPEN;
											break;
										}
									}

								}
							}
						}
						else
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"This Server Don't Have MissionNo["<<m_iMissionNo<<L"], Get MissionInfo Null");
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetMission is Failed!"));
							break;
						}
					}					

					VEC_INT	VecClearQuestValue;
					VecClearQuestValue.clear();
					
					VEC_INT VecLv6ClearQuestValue;
					VecLv6ClearQuestValue.clear();

					VEC_INT	VecLV5QuestValue;
					VecLV5QuestValue.clear();

					VEC_INT VecLV6QuestValue;
					VecLV6QuestValue.clear();

					PgMissionInfo const * pkMissionInfo = NULL;
					int m_iMissionNo = kMission.m_iMissionNo;					
					int iEventMission = 0;
					bool bRet = g_kMissionMan.GetMission( m_iMissionNo, pkMissionInfo );
					if( bRet )
					{
						const SMissionOptionMissionOpen* pkMissionOpen = NULL;
						if( true == pkMissionInfo->GetMissionOpen((DEFENCE7_MISSION_LEVEL-1), pkMissionOpen) )
						{
							pkMissionOpen->GetClearQuestIDVec(VecClearQuestValue);
						}
						const SMissionOptionMissionOpen* pkLv6MissionOpen = NULL;
						if( true == pkMissionInfo->GetMissionOpen((DEFENCE_MISSION_LEVEL-1), pkLv6MissionOpen) )
						{
							pkLv6MissionOpen->GetClearQuestIDVec(VecLv6ClearQuestValue);
						}

						const SMissionOptionMissionOpen* pkMissionOpenLv5 = NULL;
						if( true == pkMissionInfo->GetMissionOpen(MISSION_LEVEL5, pkMissionOpenLv5) )
						{
							pkMissionOpenLv5->GetIngQuestIDVec(VecLV5QuestValue);
						}
						const SMissionOptionMissionOpen* pkMissionOpenLv6 = NULL;
						if( true == pkMissionInfo->GetMissionOpen(MISSION_LEVEL6, pkMissionOpenLv6) )
						{
							pkMissionOpenLv6->GetIngQuestIDVec(VecLV6QuestValue);
						}
						iEventMission = pkMissionInfo->m_kBasic.iEventMission;
					}

					int Defence7MaxLevel = 0, Defence7MinLevel = 0;
					{	// 전략 디펜스(F7) 미션 진입 제한 레벨.
						CONT_MISSION_DEFENCE7_MISSION_BAG const * pMission;
						g_kTblDataMgr.GetContDef(pMission);

						if( !pMission )
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_MISSION_DEFENCE7_MISSION_BAG is NULL") );
							break;
						}

						CONT_MISSION_DEFENCE7_MISSION_BAG::key_type const Key(m_iMissionNo, MO_DEFENCE7);
						CONT_MISSION_DEFENCE7_MISSION_BAG::const_iterator iter = pMission->find(Key);
						if( iter != pMission->end() )
						{
							SMISSION_DEFENCE7_MISSION const & Value = iter->second.kCont.at(0);						
							Defence7MinLevel = Value.iMin_Level;
							Defence7MaxLevel = Value.iMax_Level;
						}
					}

					BM::Stream kPacket(PT_M_C_ANS_MISSION_INFO);
					PlayerMissionData.WriteToPacket(kPacket);
					kMission.WriteToPacket(kPacket);
					kPacket.Push(iType);
					kPacket.Push(VecClearQuestValue);
					kPacket.Push(VecLv6ClearQuestValue);
					kPacket.Push(VecLV5QuestValue);
					kPacket.Push(VecLV6QuestValue);
					kPacket.Push(Defence7MinLevel);
					kPacket.Push(Defence7MaxLevel);
					kPacket.Push(VecNeedItem);
					kPacket.Push(iEventMission);
					kPacket.Push(VecHiden5ClearQuestvalue);
					kPacket.Push(kContRequiredItem);
					
					if( pkPlayer->HaveParty() )
					{	// 파티에 속해있다면 파티원 모두에게 전송.
						VEC_GUID CharList;
						m_kLocalPartyMgr.GetPartyMemberGround(pkPlayer->PartyGuid(), GroundKey(), CharList);

						SendToUser_ByGuidVec(CharList, kPacket);
					}
					else
					{	// 아니면 자신에게만 전송.
						pkPlayer->Send(kPacket);
					}
				}
			}
		}break;
	case PT_N_M_NFY_CREATE_PET:
		{
			BM::GUID kCallerID;
			BM::GUID kPetID;
			pkPacket->Pop( kCallerID );
			pkPacket->Pop( kPetID );

			PgPlayer *pkCaller = GetUser( kCallerID );
			if ( pkCaller )
			{
				SPetMapMoveData kPetData;
				kPetData.kInventory.OwnerGuid( kPetID );
				kPetData.kInventory.ReadFromPacket( *pkPacket, WT_DEFAULT );
				PU::TLoadTable_AA( *pkPacket, kPetData.kSkillCoolTime );
				
				PgPet *pkPet = this->CreatePet( pkCaller, kPetID, kPetData );
				if ( pkPet )
				{
					// Passive skill setting						
					PgActPet_RefreshPassiveSkill kRefreshPassiveSkillAction( this );
					kRefreshPassiveSkillAction.DoAction( pkPet );
				}
			}
		}break;
	case PT_T_M_MARRY_COMMAND:
		{
			BYTE cCmdType = 0;
			BM::GUID	kOwnerGuid;
			SGroundKey	kCastGndKey;

			pkPacket->Pop(cCmdType);

			switch( cCmdType )
			{
			case MC_NpcTalk:
				{
					std::wstring kCharName;
					std::wstring kCoupleName;

					pkPacket->Pop(kCastGndKey);
					pkPacket->Pop(kCharName);
					pkPacket->Pop(kCoupleName);

					BM::Stream kPacket(PT_M_C_ANS_MARRY_COMMAND);
					kPacket.Push(static_cast<BYTE>(MC_NpcTalk));
					kPacket.Push( kCharName );
					kPacket.Push( kCoupleName );

					Broadcast(kPacket,BM::GUID::NullData());

					return true;
				}break;
			case MC_ActionPlay:
				{
					std::wstring kCharName;
					std::wstring kCoupleName;
					int Gender1 = 0;
					int Gender2 = 0;

					pkPacket->Pop(kCastGndKey);
					pkPacket->Pop(kCharName);
					pkPacket->Pop(Gender1);
					pkPacket->Pop(kCoupleName);
					pkPacket->Pop(Gender2);

					BM::Stream kPacket(PT_M_C_ANS_MARRY_COMMAND);
					kPacket.Push(static_cast<BYTE>(MC_ActionPlay));
					kPacket.Push(kCharName);
					kPacket.Push(Gender1);
					kPacket.Push(kCoupleName);
					kPacket.Push(Gender2);

					Broadcast(kPacket,BM::GUID::NullData());

					return true;
				}break;
			case MC_ActionPlayEnd:
				{
					pkPacket->Pop(kCastGndKey);

					BM::Stream kPacket(PT_M_C_ANS_MARRY_COMMAND);
					kPacket.Push(static_cast<BYTE>(MC_ActionPlayEnd));

					Broadcast(kPacket,BM::GUID::NullData());

					return true;
				}break;
			case MC_Init:
			case MC_End:
				{
					std::wstring	kName;
					std::wstring	kCoupleName;

					pkPacket->Pop( kName );
					pkPacket->Pop( kCoupleName );

					BM::Stream kPacket(PT_T_C_NFY_MARRY);
					GroundKey().WriteToPacket(kPacket);
					kPacket.Push( static_cast<BYTE>(EM_NOTICE) );
					kPacket.Push( static_cast<BYTE>(cCmdType) );
					kPacket.Push( GroundKey() );

					kPacket.Push(PT_M_C_ANS_MARRY_COMMAND);
					kPacket.Push(static_cast<BYTE>(cCmdType));					
					kPacket.Push( kName );
					kPacket.Push( kCoupleName );

					SendToGround(GroundKey(), kPacket);
					return true;
				}break;
			case MC_TimeOut:
				{
					BM::GUID kCharGuid;
					BM::GUID kCoupleGuid;					

					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( kCoupleGuid );

					// 부부정보 갱신
					if( BM::GUID::IsNotNull(kCharGuid) )
					{
						BM::Stream kNfyPacket(PT_C_N_REQ_COUPLE_COMMAND, kCharGuid);
						kNfyPacket.Push( (BYTE)CC_SendGndCoupleStatus );
						SendToCoupleMgr(kNfyPacket);
					}

					if( BM::GUID::IsNotNull(kCoupleGuid) )
					{
						BM::Stream kNfyPacket(PT_C_N_REQ_COUPLE_COMMAND, kCoupleGuid);
						kNfyPacket.Push( (BYTE)CC_SendGndCoupleStatus );
						SendToCoupleMgr(kNfyPacket);
					}
					return true;
				}break;
			default:
				{
				}
				break;
			}

			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kCastGndKey);

			CUnit *pkUnit = GetUnit(kOwnerGuid);
			if( !pkUnit )
			{
				break;
			}
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( pkUnit );
			if( pkPlayer )
			{
				switch(cCmdType)
				{
				case MC_ReqMarry:
					{						
						int const iQuestID = 588;
						int iGoldMoney = 0;
						int iMsgNo = 0;
						
						pkPacket->Pop(iGoldMoney);
						pkPacket->Pop(iMsgNo);

						if( 0 != iMsgNo )
						{
							pkPlayer->SendWarnMessage(iMsgNo);
							break;
						}

						PgMyQuest const * pkQuest = pkPlayer->GetMyQuest();
						if( !pkQuest )
						{
							break;
						}
						if( false == pkQuest->IsEndedQuest(iQuestID) )
						{
							pkPlayer->SendWarnMessage(450128);
							break;
						}

						__int64 const i64SendCost = static_cast<__int64>(iGoldMoney * iGoldToBronze);
						__int64 const iCasterMoney = pkUnit->GetAbil64(AT_MONEY);

						if( 0 > i64SendCost )
						{
							INFO_LOG(BM::LOG_LV5, __FL__ << L"[WARNING] TypeCast int_Gold is overflowed Player[" << pkUnit->Name() << L"] CurrentMoney[" << i64SendCost << L"]");
							break;
						}			

						if(iCasterMoney < i64SendCost)
						{
							// 돈이 부족하다...
							pkPlayer->SendWarnMessage(80024);
							break;
						}

						CUnit *pkCoupleUnit = GetUnit(pkPlayer->CoupleGuid());
						if( !pkCoupleUnit )
						{
							// 커플하고 같이 있어야 된다.
							pkPlayer->SendWarnMessage(450127);
							break;
						}

						PgPlayer *pkCouplePlayer = dynamic_cast<PgPlayer*>( pkCoupleUnit );

						if( !pkCouplePlayer )
						{
							break;
						}

						PgMyQuest const * pkCoupleQuest = pkCouplePlayer->GetMyQuest();
						if( !pkCoupleQuest )
						{
							break;
						}
						if( false == pkCoupleQuest->IsEndedQuest(iQuestID) )
						{
							pkPlayer->SendWarnMessage(450144);
							break;
						}

						CONT_PLAYER_MODIFY_ORDER kOrder;

						SPMOD_Add_Money kDelMoneyData(-i64SendCost);//필요머니 빼기.
						kOrder.push_back(SPMO(IMET_ADD_MONEY, pkUnit->GetID(), kDelMoneyData));

						BM::Stream kPacket;
						kPacket.Push(static_cast<BYTE>(MC_AnsMarry));
						kPacket.Push(i64SendCost);
						kPacket.Push(pkPlayer->GetID());
						kPacket.Push(pkPlayer->CoupleGuid());
						kPacket.Push(PT_M_C_ANS_MARRY_COMMAND);
						kPacket.Push(static_cast<BYTE>(MC_AnsMarry));
						kPacket.Push(true);

						PgAction_ReqModifyItem kItemModifyAction(MCE_MarryMoney, GroundKey(), kOrder, kPacket);
						kItemModifyAction.DoAction(pkUnit, NULL);
					}break;
				case MC_ReqSendMoney:
					{
						int iGoldMoney = 0;
						int iMsgNo = 0;

						pkPacket->Pop(iGoldMoney);
						pkPacket->Pop(iMsgNo);

						if( 0 != iMsgNo )
						{
							pkPlayer->SendWarnMessage(iMsgNo);
							break;
						}

						__int64 const i64SendCost = static_cast<__int64>(iGoldMoney * iGoldToBronze);
						__int64 const iCasterMoney = pkUnit->GetAbil64(AT_MONEY);

						if( 0 > i64SendCost )
						{
							INFO_LOG(BM::LOG_LV5, __FL__ << L"[WARNING] TypeCast int_Gold is overflowed Player[" << pkUnit->Name() << L"] CurrentMoney[" << i64SendCost << L"]");
							break;
						}			

						if(iCasterMoney < i64SendCost)
						{
							// 돈이 부족하다...
							pkPlayer->SendWarnMessage(80024);
							break;
						}

						CONT_PLAYER_MODIFY_ORDER kOrder;

						SPMOD_Add_Money kDelMoneyData(-i64SendCost);//필요머니 빼기.
						kOrder.push_back(SPMO(IMET_ADD_MONEY, pkUnit->GetID(), kDelMoneyData));

						// 기부하기
						BM::Stream kPacket;
						kPacket.Push(static_cast<BYTE>(MC_AnsSendMoney));
						kPacket.Push(i64SendCost);
						kPacket.Push(PT_M_C_ANS_MARRY_COMMAND);
						kPacket.Push(static_cast<BYTE>(MC_AnsSendMoney));
						kPacket.Push(true);

						PgAction_ReqModifyItem kItemModifyAction(MCE_MarryMoney, GroundKey(), kOrder, kPacket);
						kItemModifyAction.DoAction(pkUnit, NULL);
					}break;
				case MC_SetActionState:
					{
						int iType = 0;

						pkPacket->Pop(iType);

						__int64 i64SendAdd = 0;
						BM::GUID kCharGuid;
						BM::GUID kCoupleGuid;					
						std::wstring kName;
						std::wstring kCoupleName;
						int iGender1 = 0;
						int iGender2 = 0;
						int iMsgNo = 0;

						pkPacket->Pop( i64SendAdd );
						pkPacket->Pop( kCharGuid );
						pkPacket->Pop( kCoupleGuid );
						pkPacket->Pop( kName );
						pkPacket->Pop( kCoupleName );
						pkPacket->Pop( iGender1 );
						pkPacket->Pop( iGender2 );
						pkPacket->Pop( iMsgNo );

						if( 0 != iMsgNo )
						{
							pkPlayer->SendWarnMessage(iMsgNo);
							break;
						}

						if( static_cast<BYTE>(EM_TALK) == static_cast<BYTE>(iType) )
						{
							CUnit *pkCoupleUnit = GetUnit(pkPlayer->CoupleGuid());
							if( !pkCoupleUnit )
							{
								// 커플하고 같이 있어야 된다.
								pkPlayer->SendWarnMessage(450127);
								break;
							}

							if( kCoupleGuid.IsNotNull() )
							{
								m_kMarryMgr.Clear();
								m_kMarryMgr.Init( GroundKey(), kCharGuid, kCoupleGuid, kName, kCoupleName, iGender1, iGender2 );

								// 자리 이동 시켜라
								POINT3 pt3Pos1(177, 631, 1600);
								POINT3 pt3Pos2(204, 631, 1600);

								if( GWL_MALE == pkUnit->GetAbil(AT_GENDER) )
								{
									SendToPosLoc(pkUnit, pt3Pos1);
									SendToPosLoc(pkCoupleUnit, pt3Pos2);
								}
								else
								{
									SendToPosLoc(pkUnit, pt3Pos2);
									SendToPosLoc(pkCoupleUnit, pt3Pos1);
								}

								/*SendToDirection(pkUnit, DIR_UP);
								SendToDirection(pkCoupleUnit, DIR_UP);*/

								if( 0 < i64SendAdd )
								{
									if( m_kMarryMgr.SetMoney(i64SendAdd) )
									{
									}
								}							
							}
						}
						else if( static_cast<BYTE>(EM_NONE) == static_cast<BYTE>(iType) )
						{
							// 결혼식 취소. 초기화
							m_kMarryMgr.Clear();
						}
					}break;
				case MC_ActionPlayEnd:
					{
						BM::Stream kPacket(PT_M_C_ANS_MARRY_COMMAND);
						kPacket.Push(static_cast<BYTE>(MC_ActionPlayEnd));

						pkUnit->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
					}break;
				case MC_TotalMoney:
					{
						__int64 i64TotalMoney = 0;

						pkPacket->Pop(i64TotalMoney);

						BM::Stream kPacket(PT_M_C_ANS_MARRY_COMMAND);
						kPacket.Push(static_cast<BYTE>(MC_TotalMoney));
						kPacket.Push(i64TotalMoney);
						pkPlayer->Send(kPacket);
					}break;
				case MC_EffectCheck:
					{
						bool bMarryChar = false;

						pkPacket->Pop( bMarryChar );

						if( true == bMarryChar )
						{
							SActArg kArg;
							PgGroundUtil::SetActArgGround(kArg, this);

							int iEffectNo = 0;
							if( GWL_MALE == pkPlayer->GetAbil(AT_GENDER) )
							{
								iEffectNo = m_kMarryMgr.GetEffectMALE();
							}
							else
							{
								iEffectNo = m_kMarryMgr.GetEffectFEMALE();
							}

							if( 0 < iEffectNo )
							{
								SEffectCreateInfo kCreate;
								kCreate.eType = EFFECT_TYPE_NORMAL;
								kCreate.iEffectNum = iEffectNo;
								kCreate.kActArg = kArg;
								kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
								pkPlayer->AddEffect( kCreate );
							}
						}
					}break;
				default:
					{
					}break;
				}
			}
		}break;
	case PT_N_M_ANS_COUPLE_COMMAND:
		{
			BYTE cCmdType = 0;
			BYTE cCmdRet = 0;

			pkPacket->Pop(cCmdType);
			pkPacket->Pop(cCmdRet);

			switch(cCmdType)
			{
			case CC_Nfy_AddSkill:
				{
					if( CoupleCR_Success == cCmdRet )
					{
						BM::GUID kMineGuid;
						pkPacket->Pop( kMineGuid );

						CUnit *pkUnit = GetUnit(kMineGuid);
						if ( pkUnit )
						{
							std::list<int> kSkillStack;
							PU::TLoadArray_A((*pkPacket), kSkillStack);

							for(std::list<int>::const_iterator it = kSkillStack.begin(); it!=kSkillStack.end(); ++it)
							{
								//스킬은 컨텐츠 갔다가 오도록 해 주세요.
								if( 0 < (*it) )
								{
									BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, cCmdType);
									kPacket.Push( cCmdRet );
									kPacket.Push( (*it) );

									PgAction_LearnSkill kAction((*it), GroundKey(), cCmdType, kPacket);
									if ( LS_RET_SUCCEEDED == kAction.DoAction(pkUnit) )
									{	
									}
								}
							}
						}
					}
				}break;
			case CC_CoupleSkill:
				{
					if( CoupleCR_Success == cCmdRet )
					{
						BM::GUID kMineGuid;
						pkPacket->Pop( kMineGuid );

						CUnit *pkUnit = GetUnit(kMineGuid);
						if ( pkUnit )
						{
							CUnit *pkCoupleUnit = GetUnit(pkUnit->GetCoupleGuid());
							if( pkCoupleUnit )
							{
								/*BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Ans_SweetHeart_Complete);
								kPacket.Push( cCmdRet );
								kPacket.Push(pkUnit->Name());
								kPacket.Push(pkUnit->GetAbil(AT_GENDER));
								kPacket.Push(pkCoupleUnit->Name());
								kPacket.Push(pkCoupleUnit->GetAbil(AT_GENDER));*/

								std::list<int> kSkillStack;
								PU::TLoadArray_A((*pkPacket), kSkillStack);

								for(std::list<int>::const_iterator it = kSkillStack.begin(); it!=kSkillStack.end(); ++it)
								{
									BM::Stream kSendPacket;
									kSendPacket.Push(false);
									//kSendPacket.Push(kPacket);

									PgAction_LearnSkill kAction((*it), GroundKey(), CC_Ans_SweetHeart_Complete, kSendPacket);
									if ( LS_RET_SUCCEEDED == kAction.DoAction(pkUnit) )
									{	
										CONT_PLAYER_MODIFY_ORDER kOrder;

										BM::PgPackedTime kDate;
										SPMOD_SetSweetHeart kSetData(CC_CoupleSkill, pkUnit->GetID(), (*it), kDate);
										kOrder.push_back( SPMO(IMET_SET_SWEETHEART, pkUnit->GetID(), kSetData) );

										PgAction_ReqModifyItem kItemModifyAction(CIE_CoupleLearnSkill, GroundKey(), kOrder);
										kItemModifyAction.DoAction(pkUnit, NULL);
									}
								}
							}
						}
					}
				}break;
			case CC_Ans_Couple:
				{
					BM::GUID kMineGuid;
					BM::GUID kHimGuid;

					pkPacket->Pop( kMineGuid );
					pkPacket->Pop( kHimGuid );
					std::list<int> kSkillStack;
					PU::TLoadArray_A((*pkPacket), kSkillStack);

					if( CoupleCR_Success == cCmdRet )
					{
						CUnit *pkUnit = GetUnit(kMineGuid);
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
						PgPlayer *pkHim = dynamic_cast<PgPlayer*>( GetUnit(kHimGuid) );
						bool const bSameGround = (0 != pkPlayer) && (0 != pkHim);
						bool const bFirstPacket = (bSameGround)? (BM::GUID::IsNull(pkHim->CoupleGuid())): true;

						PgMySkill *pkMySkill = pkPlayer->GetMySkill();
						if( pkPlayer && pkMySkill )
						{
							// 내 일만 한다.
							pkPlayer->CoupleGuid( kHimGuid ); //WriteToPacket 시에 Guid도 쓰기때문에
							CONT_PLAYER_MODIFY_ORDER kOrder;

							BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, cCmdType);
							kPacket.Push( cCmdRet );
							kPacket.Push( kMineGuid );
							kPacket.Push( kHimGuid );
							PU::TWriteArray_A(kPacket, kSkillStack);

							for(std::list<int>::const_iterator it = kSkillStack.begin(); it!=kSkillStack.end(); ++it)
							{
								if( 0 < (*it) )
								{
									int const iNeedSP = pkMySkill->GetNeedSP((*it));
									if ( iNeedSP )
									{	// 필요한 SP가 없다면 서버로 보내지 않게 하자...
										// DB쿼리 낭비, 낭비, 낭비!!!
										SPMOD_AddAbil kAddAbilData(AT_SP, -iNeedSP);//SP 빼기
										kOrder.push_back(SPMO(IMET_ADD_ABIL, pkPlayer->GetID(), kAddAbilData));
									}				
									SPMOD_AddSkill kAddSkillData((*it));//꼭 SP를 먼저 빼라.
									kOrder.push_back(SPMO(IMET_ADD_SKILL, pkPlayer->GetID(), kAddSkillData));
								}
							}
							kPacket.Push( bFirstPacket );
							kPacket.Push( *pkPacket );

							BM::Stream kSendPacket;
							kSendPacket.Push(bFirstPacket);
							kSendPacket.Push(kPacket);

							BM::Stream kRealSendPacket;
							kRealSendPacket.Push((BYTE)cCmdType);
							kRealSendPacket.Push(kSendPacket);

							PgAction_ReqModifyItem kItemModifyAction(CIE_CoupleLearnSkill, GroundKey(), kOrder, kRealSendPacket);
							kItemModifyAction.DoAction(pkPlayer, NULL);
						}
					}
				}break;
			case CC_Req_Break:
			case CC_Nfy_TimeLimit:
				{
					BM::GUID kBreakerGuid;

					pkPacket->Pop( kBreakerGuid );

					if( CoupleCR_Success == cCmdRet
					||	CoupleCR_Notify  == cCmdRet )
					{
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(kBreakerGuid) );
						if( pkPlayer )
						{
							PgPlayer *pkHimPlayer = dynamic_cast<PgPlayer*>( GetUnit(pkPlayer->CoupleGuid()) );
							bool const bSameGround = NULL != pkHimPlayer;
							bool const bFirstPacket = (bSameGround)? BM::GUID::IsNotNull(pkHimPlayer->CoupleGuid()): true;

							pkPlayer->CoupleGuid( BM::GUID::NullData() );//WriteToPacket 시에 Guid도 쓰기때문에

							//{
							//	CONT_PLAYER_MODIFY_ORDER kOrder;

							//	SPMOD_SetGuid kSetGuidData(SGT_Couple, BM::GUID::NullData());
							//	kOrder.push_back( SPMO(IMET_SET_GUID, pkPlayer->GetID(), kSetGuidData) );

							//	PgAction_ReqModifyItem kItemModifyAction(IMEPT_COUPLE, GroundKey(), kOrder);
							//	kItemModifyAction.DoAction(pkPlayer, NULL);
							//}

							PgMySkill *pkMySkill = pkPlayer->GetMySkill();
							if( pkMySkill )
							{
								pkMySkill->EraseSkill( SDT_Couple );
							}
							// 커플 관련 아이템 지워준다.
							GET_DEF(CItemDefMgr, kItemDefMgr);
							CUnit* pkTarget = GetUnit(pkPlayer->GetID());
							if( pkTarget )
							{								
								PgGroundUtil::DeleteInvenCoupleItem(pkTarget, AT_BREAKCOUPLE_DELETE_ITEM, kItemDefMgr, GroundKey());						
							}

							BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, cCmdType);
							kPacket.Push( cCmdRet );
							kPacket.Push( kBreakerGuid );
							kPacket.Push( *pkPacket );
							DWORD const dwSendType = (bFirstPacket)? E_SENDTYPE_BROADALL: E_SENDTYPE_SELF;
							pkPlayer->Send(kPacket, dwSendType|E_SENDTYPE_MUSTSEND);


							// 결혼식 중이었다면 처리해 주자
							BM::Stream kClearPacket(PT_C_M_REQ_MARRY_COMMAND);
							kClearPacket.Push(static_cast<BYTE>(MC_SetActionState));
							kClearPacket.Push(pkPlayer->GetID());
							kClearPacket.Push(GroundKey());
							kClearPacket.Push(static_cast<BYTE>(EM_NONE));
							SendToCenter(kClearPacket);
						}
					}
				}break;
			case CC_Req_Info:
				{
					BM::GUID kCharGuid;
					BYTE cStatus = 0;
					BM::GUID kColorGuid;

					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( cStatus );
					pkPacket->Pop( kColorGuid );

					if( CoupleCR_Notify == cCmdRet )
					{
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(kCharGuid) );
						if( pkPlayer )
						{
							pkPlayer->SetCoupleStatus( cStatus );
							pkPlayer->ForceSetCoupleColorGuid( kColorGuid );
							RefreshGroundQuestInfo(pkPlayer);
						}
					}
				}break;
			case CC_Req_Warp:
				{					
					BM::GUID kCharGuid;
					BM::GUID kHimCharGuid;

					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( kHimCharGuid );

					if( 0 == (GetAttr() & GATTR_FLAG_CANT_PARTYWARP) ) // 인던은 소환 안되
					{
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(kCharGuid) );
						if( pkPlayer )
						{
							BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, kCharGuid);
							kPacket.Push( (BYTE)CC_Req_Warp1 );
							kPacket.Push( pkPlayer->GetPos() );
							kPacket.Push( GroundKey() );
							SendToCoupleMgr(kPacket);
							break;
						}
					}
					POINT3			pt3TargetPos;//내가 갈 자리
					SGroundKey		kTargetKey;

					BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, kHimCharGuid);
					kPacket.Push( (BYTE)CC_Ans_Reject );
					kPacket.Push( pt3TargetPos );
					kPacket.Push( kTargetKey );
					SendToCoupleMgr(kPacket);
				}break;
			case CC_Req_Warp1:
				{
					BM::GUID kCharGuid;
					POINT3			pt3TargetPos;//내가 갈 자리
					SGroundKey		kTargetKey;

					pkPacket->Pop( kCharGuid );
					pkPacket->Pop( pt3TargetPos );
					pkPacket->Pop( kTargetKey );

					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>( GetUnit(kCharGuid) );
					if( pkPlayer )
					{
						CONT_PLAYER_MODIFY_ORDER kOrderList;
						tagPlayerModifyOrderData_Add_Any kDelData(COUPLE_WARP_ITEM_NO, -1);

						SPMO kIMO(IMET_ADD_ANY, pkPlayer->GetID(), kDelData);
						kOrderList.push_back(kIMO);

						BM::Stream kAddonPacket;
						kAddonPacket.Push(pt3TargetPos);
						kAddonPacket.Push(kTargetKey);

						PgAction_ReqModifyItem kDeleteAction(CIE_CoupleWarp, GroundKey(), kOrderList, kAddonPacket);//워프도 죽어있으면 못 씀.
						kDeleteAction.DoAction(pkPlayer, NULL);
					}
				}break;
			case CC_Req_Warp2:
				{
					BM::GUID kHimCharGuid;
					BM::GUID kRequesterMembGuid;
					BM::GUID kHimMembGuid;
					BM::GUID kCharGuid;
					POINT3			pt3TargetPos;//내가 갈 자리
					SGroundKey		kTargetKey;
					short sChannelNo = 0;

					pkPacket->Pop( kHimCharGuid );
					pkPacket->Pop( kRequesterMembGuid );
					pkPacket->Pop( kHimMembGuid );
					pkPacket->Pop( kCharGuid );

					pkPacket->Pop( pt3TargetPos );
					pkPacket->Pop( kTargetKey );

					pkPacket->Pop( sChannelNo );

					ECoupleCommandResult eRet = CoupleCR_Reject;

					if( 0 == (GetAttr() & GATTR_FLAG_CANT_PARTYWARP) ) // 인던은 소환 안되
					{
						//PgPlayer *pkHimPlayer = dynamic_cast<PgPlayer*>( GetUnit(kHimCharGuid) );
						//if( pkHimPlayer )
						{
							VEC_GUID kVec;
							std::back_inserter(kVec) = kCharGuid;

							SReqMapMove_CM kRMMC( MMET_CoupleWarp );
							kRMMC.pt3TargetPos = pt3TargetPos;
							kRMMC.pt3TargetPos.z += 20;		// 바닥에 빠지지 않도록
							kRMMC.kGndKey = kTargetKey;
							kRMMC.nPortalNo = 0;

							ReqMapMoveCome(kRMMC, kVec);

							eRet = CoupleCR_Success;		// 성공

							/*BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Warp);
							kPacket.Push( (BYTE)CoupleCR_Notify );	// 커플이 워프 아웃 합니다.								
							SendToClient(kHimMembGuid, kPacket);
							//pkHimPlayer->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);*/
							BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND, kCharGuid);
							kPacket.Push( (BYTE)CC_Ans_Notify );
							kPacket.Push( pt3TargetPos );
							kPacket.Push( kTargetKey );
							SendToCoupleMgr(kPacket);
						}
					}
					if( CoupleCR_Success != eRet )
					{
						BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, (BYTE)CC_Req_Warp);
						kPacket.Push( (BYTE)eRet );
						SendToClient(kRequesterMembGuid, kPacket);
					}
				}break;
			case CC_Ans_SweetHeart_Quest:
				{
					int iQuestID = 0;
					BM::GUID kMineGuid;
					BM::GUID kHimGuid;

					pkPacket->Pop( kMineGuid );
					pkPacket->Pop( kHimGuid );
					pkPacket->Pop( iQuestID );

					if( CoupleCR_Success == cCmdRet )
					{
						CUnit *pkUnit = GetUnit(kMineGuid);
						PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
						if( pkPlayer )
						{
							BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, cCmdType);
							kPacket.Push( cCmdRet );
							kPacket.Push( kMineGuid );
							kPacket.Push( kHimGuid );
							kPacket.Push( *pkPacket );


							CONT_PLAYER_MODIFY_ORDER kOrder;

							SPMOD_AddIngQuest kAddQuestData(iQuestID, QS_Begin);
							kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );

							PgAction_ReqModifyItem kAction(IMEPT_QUEST_DIALOG_Accept, GroundKey(), kOrder, kPacket);
							kAction.DoAction(pkPlayer, NULL);
						}
					}
				}break;
			case CC_SweetHeartQuest_Cancel:
				{
					BM::GUID kCoupleGuid;
					int iQuestID = 0;
					
					pkPacket->Pop( kCoupleGuid );
					pkPacket->Pop( iQuestID );
					
					CUnit *pkUnit = GetUnit(kCoupleGuid);
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if( pkPlayer )
					{
						if( CoupleCR_Success == cCmdRet )
						{
							PgQuestInfo const *pkQuestInfo = NULL;
							if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
							{
								if( (QT_Couple == pkQuestInfo->Type()) || (QT_SweetHeart == pkQuestInfo->Type()) )
								{
									CONT_PLAYER_MODIFY_ORDER kOrder;

									SPMOD_AddIngQuest kAddQuestData(iQuestID, QS_None); // 포기 처리
									kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );

									PgAction_QuestDeleteItem kQuestAction(pkQuestInfo->m_kDropDeleteItem, kOrder); // 관련 아이템 삭제
									kQuestAction.DoAction(pkPlayer, NULL);

									PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST_DROP, GroundKey(), kOrder);
									kItemModifyAction.DoAction(pkPlayer, NULL);
								}
							}
						}
					}
				}break;
			case CC_SweetHeartQuest_Complete:
				{
					BM::GUID kCoupleGuid;
					
					pkPacket->Pop( kCoupleGuid );
					
					CUnit *pkUnit = GetUnit(kCoupleGuid);
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if( pkPlayer )
					{
						if( CoupleCR_Success == cCmdRet )
						{
							PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
							if( pkMyQuest )
							{
								ContUserQuestState kStateVec;
								bool const bGetQuestRet = pkMyQuest->GetQuestList(kStateVec);
								if( bGetQuestRet )
								{
									ContUserQuestState::iterator iter = kStateVec.begin();
									while(kStateVec.end() != iter)
									{
										const ContUserQuestState::value_type& rkQuestState = (*iter);
										int const iQuestID = rkQuestState.iQuestID;

										PgQuestInfo const* pkQuestInfo = NULL;
										if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
										{
											if( (QT_Couple == pkQuestInfo->Type()) || (QT_SweetHeart ==  pkQuestInfo->Type()) )
											{
												CONT_PLAYER_MODIFY_ORDER kOrder;
												
												PgAction_IncQuestParam kIncreaseParam(GroundKey(), iQuestID, pkQuestInfo->m_kDepend_Couple.iObjectNo);
												kIncreaseParam.DoAction(pkPlayer, NULL);

												BM::Stream kPacket(PT_C_N_REQ_COUPLE_COMMAND);
												kPacket.Push( pkPlayer->GetID() );
												kPacket.Push( (BYTE) CC_SweetHeartQuest_Complete );
												kPacket.Push( -1 ); // iQuestTime(Client 갱신 "연인 조건" 버튼 활성화 값, -1 아니면 변경안된다)
												kPacket.Push( pkQuestInfo->Type() );
												SendToCoupleMgr( kPacket );
											}
										}
										++iter;
									}
								}
							}
							else
							{
								CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkPlayer->GetID()<<L", Name: "<<pkPlayer->Name());
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							}
						}
					}
				}break;
			case CC_NfyStatus:
				{
					BM::GUID kMineGuid;
					BYTE cStatus = 0;
					BM::GUID kColorGuid;

					pkPacket->Pop( kMineGuid );
					pkPacket->Pop( cStatus );
					pkPacket->Pop( kColorGuid );

					CUnit* pkUnit = GetUnit(kMineGuid);
					if( pkUnit )
					{
						PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
						if( pkPlayer )
						{
							pkPlayer->SetCoupleStatus( cStatus );
							pkPlayer->ForceSetCoupleColorGuid( kColorGuid );
							RefreshGroundQuestInfo(pkPlayer);

							//BroadCasting()
							BM::Stream kPacket(PT_N_C_ANS_COUPLE_COMMAND, static_cast<BYTE>(CC_SendGndCoupleStatus));
							kPacket.Push( static_cast<BYTE>(CoupleCR_Success) );
							kPacket.Push( kMineGuid );
							kPacket.Push( kColorGuid );
							kPacket.Push( cStatus );
							pkPlayer->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
						}
					}
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
				}break;
			}
		}break;
	case PT_N_M_NFY_GUILD_COMMAND:
		{
			BM::GUID kCharGuid;
			BYTE cCmdType = 0;
			BYTE cRet = 0;
			pkPacket->Pop(cCmdType);
			pkPacket->Pop(cRet);
			switch(cCmdType)
			{
			case GC_Create:
			case GC_Join:
				{
					pkPacket->Pop(kCharGuid);

					CUnit* pkUnit = GetUnit(kCharGuid);
					if( pkUnit )
					{
						if( GCR_Success == cRet )
						{
							SGuildOtherInfo kInfo;
							std::wstring kGuildName;

							kInfo.ReadFromPacket(*pkPacket);

							PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUnit);
							if( pkPC )
							{
								pkPC->GuildGuid(kInfo.kGuid);

								//그러고 BroadCasting()
								BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
								kPacket.Push((BYTE)GCR_Success);
								kPacket.Push(kCharGuid);
								kInfo.WriteToPacket(kPacket);
								pkUnit->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
							}
						}
						else//실패
						{
							if( GC_Create == cCmdType )
							{
								//골드 보상 해줘 --;
								__int64 const iGuildPrice = GuildUtil::GetHowMuchCreateGuild(); // 길드 가격

								PgAction_ReqAddMoney kGoldAction(MCE_FailCreateGuild, iGuildPrice, GroundKey());
								kGoldAction.DoAction(pkUnit, NULL);

								CAUTION_LOG(BM::LOG_LV1, __FL__<<L"[GUILD-Create: "<<pkUnit->Name()<<L"] Failed - return money");
							}

							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
							kPacket.Push(cRet);
							pkUnit->Send(kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND);
						}
					}//end if( pkUnit )
				}break;
			case GC_M_Kick:
			case GC_M_Destroy:
			case GC_Leave:
				{
					pkPacket->Pop(kCharGuid);
					CUnit* pkUnit = GetUnit(kCharGuid);
					if( !pkUnit )
					{
						break;
					}

					PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUnit);
					if( !pkPC )
					{
						break;
					}

					pkPC->GuildGuid(BM::GUID::NullData());

					//{
					//	CONT_PLAYER_MODIFY_ORDER kOrder;

					//	SPMOD_SetGuid kSetGuidData(SGT_Guild, BM::GUID::NullData());
					//	kOrder.push_back( SPMO(IMET_SET_GUID, pkPC->GetID(), kSetGuidData) );

					//	PgAction_ReqModifyItem kItemModifyAction(IMEPT_GUILD, GroundKey(), kOrder);
					//	kItemModifyAction.DoAction(pkPC, NULL);
					//}

					PgMySkill *pkMySkill = pkPC->GetMySkill();
					if( pkMySkill )
					{
						// 길드 스킬 제거
						pkMySkill->EraseSkill(SDT_Guild);
					}

					//그러고 BroadCasting()
					BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
					kPacket.Push((BYTE)GCR_Success);
					kPacket.Push(kCharGuid);
					pkUnit->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
				}break;
			case GC_M_Rename:
			case GC_GM_Rename:
				{
					std::wstring kNewGuildName;
					pkPacket->Pop(kCharGuid);
					pkPacket->Pop(kNewGuildName);

					CUnit* pkUnit = GetUnit(kCharGuid);
					if( pkUnit )
					{
						PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUnit);
						if( pkPC )
						{
							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
							kPacket.Push((BYTE)GCR_Success);
							kPacket.Push(pkPC->GuildGuid());
							kPacket.Push(kNewGuildName);
							pkPC->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
						}
					}
				}break;
			case GC_M_ChangeMark1:
				{
					pkPacket->Pop(kCharGuid);

					CUnit* pkUnit = GetUnit(kCharGuid);
					if( pkUnit )
					{
						PgPlayer* pkPC = dynamic_cast< PgPlayer* >(pkUnit);
						if( pkPC )
						{
							switch( cRet )
							{
							case GCR_Success:
								{
									BYTE cNewGuildEmblem = 0;
									pkPacket->Pop(cNewGuildEmblem);

									BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
									kPacket.Push( (BYTE)GCR_Success );
									kPacket.Push( pkPC->GetID() );
									kPacket.Push( pkPC->GuildGuid() );
									kPacket.Push( cNewGuildEmblem );
									pkPC->Send(kPacket, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
								}break;
							case GCR_None:
								{
									BM::Stream kNfyPacket;
									kNfyPacket.Push( *pkPacket );

									pkUnit->VNotify(&kNfyPacket);
								}break;
							default:
								{
								}break;
							}
						}
					}
				}break;
			case GC_M_LvUp:
				{
					unsigned short sLevel = 0;
					pkPacket->Pop(kCharGuid);
					pkPacket->Pop(sLevel);

					CUnit* pkUnit = GetUnit(kCharGuid);
					if( pkUnit )
					{
						PgAction_ReqGuildLevelUp kActionGuildLevel(GroundKey(), sLevel);
						if( !kActionGuildLevel.DoAction(pkUnit, NULL) )
						{
							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
							kPacket.Push((BYTE)kActionGuildLevel.Result());
							pkUnit->Send(kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND);
						}
					}
				}break;
			case GC_M_InventoryCreate:
				{
					unsigned short sLevel = 0;
					pkPacket->Pop(kCharGuid);
					pkPacket->Pop(sLevel);

					CUnit* pkUnit = GetUnit(kCharGuid);
					if( pkUnit )
					{
						PgAction_ReqGuildInventoryCreate kActionGuildInventoryCreate(GroundKey(), sLevel);
						if( !kActionGuildInventoryCreate.DoAction(pkUnit, NULL) )
						{
							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
							kPacket.Push(static_cast<BYTE>(kActionGuildInventoryCreate.Result()));
							pkUnit->Send(kPacket, E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);
						}
					}
				}break;
			case GC_M_AddSkill:
				{
					pkPacket->Pop(kCharGuid);
					CUnit *pkUnit = GetUnit(kCharGuid);
					if( !pkUnit )
					{
						break;
					}

					//bool bFailed = false;
					if( GCR_None == cRet )
					{
						unsigned short sLevel = 0;
						int iSkillNo = 0;
						pkPacket->Pop(sLevel);
						pkPacket->Pop(iSkillNo);

						PgAction_ReqGuildLearnSkill kLearnGuildSkill(GroundKey(), sLevel, iSkillNo);
						if( !kLearnGuildSkill.DoAction(pkUnit, NULL) )
						{
							BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
							kPacket.Push((BYTE)kLearnGuildSkill.Result());
							pkUnit->Send(kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND);
						}
					}
					else if( GCR_Success == cRet )
					{
						int iSkillNo = 0;
						pkPacket->Pop(iSkillNo);

						//PgAction_LearnSkill kAction(iSkillNo, GroundKey(), cCmdType, *pkPacket);
						//kAction.DoAction(pkUnit);

						/*PgAction_LearnSkill kAction(iSkillNo, GroundKey(), cCmdType, kPacket);
						if (!kAction.DoAction(pkUnit))
						{
							INFO_LOG(BM::LOG_LV4, _T("[%s] GC_M_AddSkill failed SkillNo[%d]"), __FUNCTIONW__, iSkillNo);
						}*/

						BM::Stream kPacket(PT_N_C_ANS_GUILD_COMMAND, cCmdType);
						kPacket.Push((BYTE)GCR_Success);
						kPacket.Push(iSkillNo);
						kPacket.Push(*pkPacket);
						pkUnit->Send(kPacket, E_SENDTYPE_SELF| E_SENDTYPE_MUSTSEND);
					}
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
				}break;
			}
		}break;
	case PT_N_M_REQ_CREATE_ITEM:
		{
			BM::GUID kCharGuid;
			EItemModifyParentEventType kType;
			CONT_ITEM_CREATE_ORDER kConItem;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kType);
			PU::TLoadArray_M(*pkPacket, kConItem);

			PgAction_CreateItem kCreateAction(kType, GroundKey(), kConItem);
			kCreateAction.DoAction(GetUnit(kCharGuid), NULL); //NULL체크는 안에서 한다.
		}break;
	case PT_N_M_REQ_ITEM_CHANGE_GUILD:
		{
			BM::GUID kGuildGuid;
			BM::GUID kCharGuid;
			int iType;
			__int64 i64Money = 0i64;			
			SItemPos kSourcePos;
			SItemPos kTargetPos;
			PgBase_Item kCasterItem;
			PgBase_Item kTargetItem;
			DWORD dwClientTime;
			BM::GUID kNpcGuid;
			bool bAddonPacket = false;
			SGuild_Inventory_Log kLog;

			pkPacket->Pop( kGuildGuid );
			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( iType );

			switch( iType )
			{
			case EGIT_MONEY_IN:
			case EGIT_MONEY_OUT:
				{
					pkPacket->Pop(i64Money);
				}break;
			case EGIT_ITEM_IN:
			case EGIT_ITEM_OUT:
			case EGIT_ITEM_MOVE:
				{
					pkPacket->Pop( kSourcePos );
					pkPacket->Pop( kTargetPos );
					pkPacket->Pop( dwClientTime );
					pkPacket->Pop( kNpcGuid );
					kCasterItem.ReadFromPacket(*pkPacket);
					kTargetItem.ReadFromPacket(*pkPacket);
				}break;
			default:
				{
				}break;
			}
			
			pkPacket->Pop(bAddonPacket);
			if( bAddonPacket )
			{				
				kLog.ReadFromPacket(*pkPacket);
			}

			CUnit * pkUnit = GetUnit(kCharGuid);
			if( NULL == pkUnit )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
				break;
			}

			if( EGIT_MONEY_IN == iType
			||	EGIT_MONEY_OUT == iType )
			{
				// 길드머니 수정
				PgAction_MoveMoney_Guild kAction(this, kGuildGuid, iType, i64Money, bAddonPacket, kLog );
				kAction.DoAction( pkUnit, NULL );
				break;
			}
			
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				DWORD dwServerTime = 0;
				if ( S_OK == CheckClientNotifyTime(pkPlayer, dwClientTime, dwServerTime) )
				{
					CUnit* pkNpcUnit = GetUnit(kNpcGuid);
					
					if(	true == PgGroundUtil::IsCanTalkableRange(pkUnit, kNpcGuid, pkNpcUnit, GetGroundNo(), NMT_Quest, __F_P__(PT_N_M_REQ_ITEM_CHANGE_GUILD), __LINE__) )						
					{
						// 길드금고 관련 액션
						PgAction_MoveItem_Guild kAction(kSourcePos, kTargetPos, kCasterItem, kTargetItem, this, dwClientTime, iType, bAddonPacket, kLog );
						kAction.DoAction( pkUnit, NULL );
					}
				}
				else
				{
					INFO_LOG(BM::LOG_LV5, __FL__ << _T("Player kicked, Cause[CDC_SuspectedSpeedHack], Name=") << pkPlayer->Name());
					// SpeedHack이 의심스러우니 강제접속 해제 시킨다.
					BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_SuspectedSpeedHack) );
					kDPacket.Push( pkPlayer->GetMemberGUID() );
					SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
				}
			}
		}break;		
	case PT_N_M_ANS_TAKE_COUPON:
		{
			BM::GUID kCharGuid;

			int iError = 0;
			//int f_RewardItemNo = 0;
			//int f_RewardItemCount = 0;
			int f_Money = 0;
			int f_Cash = 0;
			BM::GUID f_RewardGuid;

			pkPacket->Pop(kCharGuid);

			CUnit * pkUnit = GetUnit(kCharGuid);
			if(NULL == pkUnit)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
				break;;
			}

			PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkUnit);
			if(NULL == pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
				break;;
			}

			pkPacket->Pop(iError);

			BM::Stream kPacketAns(PT_M_C_TRY_TAKE_COUPON);

			if(CRT_SUCCESS != iError)
			{
				kPacketAns.Push(iError);
				pkUnit->Send(kPacketAns);
				break;
			}

			CONT_COUPON_REWARD kContReward;
			pkPacket->Pop(f_Money);
			pkPacket->Pop(f_Cash);
			pkPacket->Pop(kContReward);

			kPacketAns.Push(iError);
			kPacketAns.Push(*pkPacket);
			pkPlayer->Send(kPacketAns);

			{
				CONT_PLAYER_MODIFY_ORDER kCont;
				for(CONT_COUPON_REWARD::iterator iter = kContReward.begin();iter != kContReward.end();++iter)
				{
					PgBase_Item kItem;
					if( SUCCEEDED(CreateSItem((*iter).iItemNo, (*iter).siItemCount, GIOT_NONE, kItem)) )
					{
						kItem.Guid((*iter).kItemGuid);
						if((*iter).siUseTime)
						{
							kItem.SetUseTime(static_cast<__int64>((*iter).bTimeType),static_cast<__int64>((*iter).siUseTime));
						}
						kCont.push_back(SPMO(IMET_INSERT_FIXED,pkPlayer->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(0,0),true)));
					}
				}
				// 아이템은 인벤에 빈공간이 부족하면 임시 인벤토리를 사용한다.
				PgAction_ReqModifyItem kItemModifyAction(CIE_Coupon, GroundKey(), kCont);
				kItemModifyAction.DoAction(pkPlayer,NULL);
			}

			{// 돈과 캐시 지급은 다른 오더를 사용 하도록 한다.
				CONT_PLAYER_MODIFY_ORDER kCont;
				if(f_Money > 0)
				{
					kCont.push_back(SPMO(IMET_ADD_MONEY,pkPlayer->GetID(),SPMOD_Add_Money(f_Money)));
				}

				PgAction_ReqModifyItem kItemModifyAction(CIE_Coupon, GroundKey(), kCont);

				if(f_Cash > 0)
				{
					kItemModifyAction.CashAdd(SPMO(IMET_ADD_CASH,pkPlayer->GetID(),SPMOD_AddCash(pkPlayer->GetMemberGUID(),static_cast<__int64>(f_Cash),pkPlayer->Name(),CIE_Coupon)));
				}

				kItemModifyAction.DoAction(pkPlayer,NULL);
			}
		}break;
	case PT_M_N_ANS_GIVE_LIMITED_ITEM:
		{
			SREQ_GIVE_LIMITED_ITEM kData;
			pkPacket->Pop(kData);

			// 로그를 남겨
			PgLogCont kLogCont(ELogMain_Contents_Monster, ELogSub_Monster_Death );
//			kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
//			kLogCont.CharacterKey( kDropItem.m_kOwnerGuid );
//			kLogCont.GroundNo( GetGroundNo() );			// GroundNo
//			kLogCont.Name( pkUnit->Name() );
//			kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

//			PgLog kLog( ELOrderMain_Monster, ELOrderSub_Death );
//			kLog.Set( 1, static_cast<int>(this->GetAttr()) );	// iValue2 그라운드 타입

//			kLogCont.Add( kLog );
			CONT_LIMITED_ITEM const * pkContLimitedItem = NULL;
			g_kTblDataMgr.GetContDef(pkContLimitedItem);

			CONT_LIMITED_ITEM::const_iterator lim_itor = pkContLimitedItem->find(kData.m_kTryLimitEventNo);

			if(lim_itor != pkContLimitedItem->end())
			{
				GET_DEF(CItemBagMgr, kItemBagMgr);
				
				PgItemBag kItemBag;
				if(S_OK == kItemBagMgr.GetItemBag( lim_itor->second.iBagNo, kData.m_nOwnerLevel, kItemBag))
				{
					int iRetItemNo = 0;
					if( S_OK == kItemBag.PopItem( static_cast<int>(kData.m_nOwnerLevel), iRetItemNo ) )
					{
						PgBase_Item kItem;
						if(S_OK == ::CreateSItem(iRetItemNo, 1, GIOT_NONE, kItem))
						{
							VEC_GUID kContOwner;
							kContOwner.push_back(kData.m_kOwnerGuid);
							InsertItemBox(kData.m_kptPos, kContOwner, NULL, kItem, 0, kLogCont );
						}
					}
				}
			}

//		CONT_LIMITED_ITEM::mapped_type const &kElememt = lim_itor->second;
			kLogCont.Commit();
		}break;
	case PT_N_M_ANS_CHECK_PENALTY:
		{
			BM::GUID kCharGuid;
			WORD wType = 0;
			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( wType );

			PgPlayer *pkPlayer = GetUser( kCharGuid );
			if ( pkPlayer )
			{
				pkPlayer->SetAbil( wType, 0, false, false );

				if ( pkPacket->RemainSize() >= sizeof(BM::Stream::DEF_STREAM_TYPE) )
				{
					WORD wType = 0;
					pkPacket->Pop( wType );
					SEventMessage kEventMsg( PMET_GROUND_MGR, wType );
					kEventMsg.Push( *pkPacket );
					g_kTask.PutMsg(kEventMsg);
				}
			}
		}break;
	case PT_N_M_NFY_EMPORIA_PORTAL_INFO:
		{
			size_t iSize = 0;
			pkPacket->Pop( iSize );
			size_t const iFixSize = iSize;

			BM::Stream kBroadPacket( PT_M_C_NFY_EMPORIA_PORTAL_INFO );
			size_t const iRDPos = kBroadPacket.RdPos();
			kBroadPacket.Push( iFixSize );

			size_t iRetSize = 0;
			while ( iSize-- )
			{
				size_t const iRDPos = pkPacket->RdPos();
				SEmporiaKey kEmporiaKey;
				pkPacket->Pop( kEmporiaKey );
				pkPacket->RdPos( iRDPos );

				CONT_EMPORIA_PORTAL::iterator po_itr = m_kContEmporiaPortal.find( kEmporiaKey );
				if ( po_itr != m_kContEmporiaPortal.end() )
				{
					po_itr->second.ReadFromPacket( *pkPacket );
					po_itr->second.WriteToPacket_ToClient( kBroadPacket );
					++iRetSize;
				}
				else
				{
					CONT_EMPORIA_PORTAL::mapped_type kElement;
					kElement.ReadFromPacket( *pkPacket );
				}
			}

			if ( iRetSize > 0 )
			{
				if ( iRetSize != iFixSize )
				{
					kBroadPacket.ModifyData( iRDPos, &iRetSize, sizeof(iRetSize) );
				}

				Broadcast( kBroadPacket );
			}
		}break;
	case PT_C_M_REQ_EVENT_QUEST_TALK:
		{
			BM::GUID kCharGuid;
			BM::GUID kNpcGuid;
			ContEventQuestTarget kTarget;

			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( kNpcGuid );
			PU::TLoadArray_M(*pkPacket, kTarget); // 서버에서 확인한 시점의 목표 아이템들로 오더를 만든다

			CUnit* pkUnit = GetUnit(kCharGuid);
			if( pkUnit )
			{
				PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
				if( pkPlayer )
				{
					CONT_PLAYER_MODIFY_ORDER kOrder;

					ContEventQuestTarget::const_iterator iter = kTarget.begin();
					while( kTarget.end() != iter )
					{
						ContEventQuestTarget::value_type const& rkTargetItem = (*iter);
						kOrder.push_back( SPMO(IMET_ADD_ANY, kCharGuid, tagPlayerModifyOrderData_Add_Any(rkTargetItem.iItemNo, -rkTargetItem.iCount)) );
						++iter;
					}

					if( !kOrder.empty() )
					{
						BM::Stream kAddonPacket;
						kAddonPacket.Push( kNpcGuid );

						PgAction_ReqModifyItem kItemModifyAction(IMEPT_EVENTQUEST, GroundKey(), kOrder, kAddonPacket);
						kItemModifyAction.DoAction(pkPlayer, NULL);
					}
				}
			}
		}break;
	case PT_N_C_NFY_BS_GND_NOTICE:
		{
			BM::Stream kPacket(PT_N_C_NFY_BS_NOTICE, *pkPacket);
			Broadcast(kPacket);
		}break;
	case PT_T_M_NFY_WANT_JOIN_BS_CHANNEL:
		{
			BM::GUID kCharGuid;

			pkPacket->Pop( kCharGuid );

			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(GetUnit(kCharGuid));
			if( pkPlayer )
			{
				int iGroundNo = 0;
				pkPacket->Pop( iGroundNo );

				SReqMapMove_MT kRMM(MMET_BATTLESQUARE);
				kRMM.kTargetKey.GroundNo(iGroundNo);
				kRMM.nTargetPortal = 1;

				PgReqMapMove kMapMove(this, kRMM, NULL);
				if( kMapMove.Add(pkPlayer) )
				{
					if( !kMapMove.DoAction() )
					{
						BM::Stream kPacket(PT_A_N_NFY_WANT_JOIN_BS_CHANNEL);
						kPacket.Push( pkPlayer->GetID() );
						kPacket.Push( BSJR_NONE );
						::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
					}
				}
			}
		}break;
	case PT_T_M_ANS_ENTER_SUPER_GROUND:
		{
			SReqMapMove_MT kRMM(MMET_SuperGround);
			BM::GUID kCharGuid;
			int iSuperGroundNo = 0, iSuperGroundMode = 0, iSpawnNo = 0;
			bool bReqUseItem = false;
			bool IsElement = false;
			SPMO kReqUseItem;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(iSuperGroundNo);
			pkPacket->Pop(iSuperGroundMode);
			kRMM.kTargetKey.ReadFromPacket(*pkPacket);
			pkPacket->Pop(iSpawnNo); kRMM.nTargetPortal = static_cast< short >(iSpawnNo);
			pkPacket->Pop(IsElement);
			pkPacket->Pop(bReqUseItem);
			if( bReqUseItem )
			{
				kReqUseItem.ReadFromPacket( *pkPacket );
			}

			PgPlayer *pkPlayer = GetUser(kCharGuid);
			if( pkPlayer )
			{
				SPMO ItemModifyOrder;

				if( IsElement )
				{	// 정령 던전일 때만 입장아이템 검사.
					if( pkPlayer->HaveParty() )
					{
						BM::GUID MasterGuid;
						m_kLocalPartyMgr.GetPartyMasterGuid(pkPlayer->PartyGuid(), MasterGuid);
						if( pkPlayer->GetID() != MasterGuid )
						{
							pkPlayer->SendWarnMessage(700064); // 파티장이어야 가능합니다.
							break;
						}
					}

					PgInventory * pInv = pkPlayer->GetInven();
					if( pInv )
					{
						GET_DEF(CItemDefMgr, ItemDefMgr);

						// 입장 아이템 체크
						ContHaveItemNoCount ContHaveItemNoCount;
						if( S_OK == pInv->GetItems(UICT_ELEMENT_ENTER_CONSUME, ContHaveItemNoCount ) )
						{	// 한 종류의 입장아이템으로 여러개의 던전 입장검사해야 되기 때문에, 입장 아이템 보유여부만 검사한다.
							ContHaveItemNoCount::const_iterator itemno_itr = ContHaveItemNoCount.begin();
							for ( ; itemno_itr != ContHaveItemNoCount.end() ; ++itemno_itr )
							{
								SItemPos KeyItemPos;
								if ( SUCCEEDED(pInv->GetFirstItem( itemno_itr->first, KeyItemPos, false, true )) )
								{
									CItemDef const * pItemDef = ItemDefMgr.GetDef(itemno_itr->first);
									if( !pItemDef )
									{
										continue;
									}

									if( pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1) )
									{
										PgBase_Item KeyItem;
										if ( S_OK == pInv->GetItem( KeyItemPos, KeyItem ) )
										{
											if( KeyItem.Count() >= 1 )
											{	// 한개 이상 있어야지 사용할 수 있다.
												SPMO IMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(KeyItem, KeyItemPos, -1));
												ItemModifyOrder = IMO;
												break;
											}
											else
											{	// 아이템이 없다.
												pkPlayer->SendWarnMessage(799940);
												break;
											}
										}
									}
								}
							}
						}
						else
						{	// 아이템이 없다.
							pkPlayer->SendWarnMessage(799940);
							break;
						}
					}
				}

				PgReqMapMove kMapMove( this, kRMM, NULL );
				if( kMapMove.Add( pkPlayer ) )
				{
					if( BM::GUID::IsNotNull(pkPlayer->PartyGuid()) )
					{
						VEC_GUID kPartyList;
						if( m_kLocalPartyMgr.GetPartyMemberGround(pkPlayer->PartyGuid(), GroundKey(), kPartyList, pkPlayer->GetID()) )
						{
							VEC_GUID::const_iterator guid_itr = kPartyList.begin();
							while( kPartyList.end() != guid_itr )
							{
								PgPlayer *pkPlayer = GetUser( *guid_itr );
								if( pkPlayer )
								{
									kMapMove.Add( pkPlayer );
								}
								++guid_itr;
							}
						}
					}
					if( bReqUseItem )
					{
						kMapMove.AddModifyOrder( kReqUseItem );
					}

					if( IsElement )
					{	// 정령던전 입장할 때 아이템 소모해줌.
						kMapMove.AddModifyOrder(ItemModifyOrder);
					}
					kMapMove.DoAction();
				}
			}
		}break;
	case PT_T_C_ANS_BS_CHANNEL_INFO:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(GetUnit(kCharGuid));
			if( pkPlayer )
			{
				BM::Stream kSendPacket(PT_T_C_ANS_BS_CHANNEL_INFO);
				kSendPacket.Push(*pkPacket);
				pkPlayer->Send(kSendPacket);
			}
		}break;
	case PT_I_M_UM_NFY_VENDOR_REFRESH_QUERY:
		{
			BM::Stream kSendPacket(PT_M_C_UM_NFY_VENDOR_REFRESH_QUERY);
			kSendPacket.Push(*pkPacket);
			Broadcast(kSendPacket, BM::GUID::NullData() );
		}break;
	case PT_C_M_GODCMD:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop( kCharGuid );
			CUnit* pkUnit = GetUnit(kCharGuid);
			if( pkUnit )
			{
				int iCmdType = 0;
				if( pkPacket->Pop(iCmdType) )
				{
					GMCommand( dynamic_cast<PgPlayer*>(pkUnit), static_cast< EGMCmdType >(iCmdType), pkPacket );
				}
			}
		}break;
	case PT_T_M_NFY_RESULT_REGIST_CHAT_BLOCK:
		{	// 차단 유저 등록 결과.
			bool Result;
			BYTE BlockType = 0;
			BM::GUID CharGuid;
			std::wstring Name;

			pkPacket->Pop(CharGuid);
			pkPacket->Pop(Name);
			pkPacket->Pop(BlockType);
			pkPacket->Pop(Result);

			PgPlayer * pPlayer = GetUser(CharGuid);
			if( pPlayer )
			{
				if( true == Result )
				{
					pPlayer->AddChatBlockList(Name, BlockType);
				}

				BM::Stream Packet(PT_M_C_NFY_RESULT_REGIST_CHAT_BLOCK);
				Packet.Push(Name);
				Packet.Push(BlockType);
				Packet.Push(Result);

				pPlayer->Send(Packet);
			}
		}break;
	case PT_N_M_NFY_JOIN_ANOTHER_CHANNEL_PARTY:
		{
			AddJoinPartyMemberList(*pkPacket);
		}break;
	case PT_T_M_ANS_RETURN_OTHER_CHANNEL_PARTY:
		{
			BM::GUID CharGuid, MasterGuid, PartyGuid;
			short ChannelNo = 0;
			bool InParty = false;

			pkPacket->Pop(CharGuid);
			pkPacket->Pop(MasterGuid);
			pkPacket->Pop(PartyGuid);
			pkPacket->Pop(ChannelNo);

			PgPlayer * pPlayer = GetUser(CharGuid);
			if( pPlayer )
			{
				if( BM::GUID::NullData() != pPlayer->PartyGuid() )
				{//파티가 있다.
					InParty = true;
				}
			}
			BM::Stream Packet(PT_M_T_ANS_RETURN_OTHER_CHANNEL_PARTY);
			Packet.Push(CharGuid);
			Packet.Push(MasterGuid);
			Packet.Push(PartyGuid);
			Packet.Push(ChannelNo);
			Packet.Push(InParty);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_T_M_ANS_JOIN_PARTYFIND_ITEM_CHECK:
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kUser;
			bool IsMyChannel = false;
			short MasterChannelNo = 0;
			SGroundKey MasterGroundKey;

			kUser.ReadFromPacket(*pkPacket);

			pkPacket->Pop(bAnsJoin);
			pkPacket->Pop(kPartyGuid);
			pkPacket->Pop(MasterGroundKey);
			pkPacket->Pop(IsMyChannel);
			pkPacket->Pop(MasterChannelNo);

			CONT_DEFMAP const * pDefMap = NULL;
			g_kTblDataMgr.GetContDef( pDefMap );

			if( NULL == pDefMap )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			CONT_DEFMAP::const_iterator iter = pDefMap->find(MasterGroundKey.GroundNo());
			if( iter == pDefMap->end() )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			PgPlayer * pPlayer = GetUser(kUser.kCharGuid);
			if( NULL == pPlayer )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			PgInventory * pInv = pPlayer->GetInven();
			if( NULL == pInv )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			int NeedItemNo = 0;
			bool IsHaveItem = false;

			CONT_NEED_KEY_ITEM::const_iterator item_iter = iter->second.ContItemNo.begin();
			while( iter->second.ContItemNo.end() != item_iter )
			{
				if( pInv->GetInvTotalCount(*item_iter) > 0 )
				{
					NeedItemNo = *item_iter;
					IsHaveItem = true;
					break;
				}
				++item_iter;
			}

			if( false == IsHaveItem )
			{
				item_iter = iter->second.ContCashItemNo.begin();
				while( iter->second.ContCashItemNo.end() != item_iter )
				{
					if( pInv->GetInvTotalCount(*item_iter) > 0 )
					{
						NeedItemNo = *item_iter;
						IsHaveItem = true;
						break;
					}
					++item_iter;
				}
			}

			if( iter->second.ContItemNo.empty() && iter->second.ContCashItemNo.empty() )
			{// 둘다 비어있으면 필요한 아이템이 없다.
				IsHaveItem = true;
			}

			if( IsMyChannel )
			{// 같은 채널
				if( IsHaveItem )
				{	
					BM::Stream Packet(PT_C_N_ANS_JOIN_PARTYFIND_2ND);
					kUser.WriteToPacket(Packet);
					Packet.Push(bAnsJoin);
					Packet.Push(kPartyGuid);
					SendToGlobalPartyMgr(Packet);
				}
				else
				{
					BM::Stream Packet(PT_T_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
					kUser.WriteToPacket(Packet);
					Packet.Push(bAnsJoin);
					Packet.Push(kPartyGuid);
					Packet.Push(IsHaveItem);
					SendToGlobalPartyMgr(Packet);
				}
			}
			else
			{// 다른 채널
				BM::Stream Packet(PT_M_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
				kUser.WriteToPacket(Packet);
				Packet.Push(bAnsJoin);
				Packet.Push(kPartyGuid);
				Packet.Push(MasterChannelNo);
				Packet.Push(IsHaveItem);
				SendToGlobalPartyMgr(Packet);

				if( false == IsHaveItem )
				{
					BM::Stream AnsPacket(PT_M_C_ANS_JOIN_PARTYFIND_ITEM_CHECK);
					AnsPacket.Push(IsHaveItem);
					AnsPacket.Push(*(iter->second.ContItemNo.begin()));
					pPlayer->Send(AnsPacket);
				}
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	return true;
}

bool PgIndun::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch( usType )
	{
	case PT_N_M_NFY_RECENT_MAP_MOVE:	// 리센트위치로 돌려보내라.
		{
			VEC_GUID kCharGuidVec;
			pkPacket->Pop( kCharGuidVec );

			PgPlayer *pkUser = NULL;
			VEC_GUID::iterator user_itr;
			for ( user_itr=kCharGuidVec.begin(); user_itr!=kCharGuidVec.end(); ++user_itr )
			{
				pkUser = GetUser( *user_itr );
				if ( pkUser )
				{
					this->RecvRecentMapMove( pkUser );
				}
			}
		}break;
	case PT_M_M_ANS_READY_HARDCORE_BOSS:
		{
			if ( GATTR_FLAG_HARDCORE_DUNGEON & this->GetAttr() )
			{
				SGroundKey kAnsGndKey;
				kAnsGndKey.ReadFromPacket( *pkPacket );
				if ( kAnsGndKey == m_kOwnerGndInfo.kOwnerGndKey )
				{
					bool bSuccess = false;
					pkPacket->Pop( bSuccess );

					if ( true == bSuccess )
					{
						pkPacket->Pop( m_kOwnerGndInfo.i64EndTime );
					}
					else
					{
						m_kOwnerGndInfo.i64EndTime = 1i64;
					}
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV0, L"Error Packet<" << usType << L">" << GroundKey().ToString() ); 
			}
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}break;
	}
	
	return true;
}

bool PgMissionGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	if ( g_kProcessCfg.IsPublicChannel() )
	{// 공용 맵서버에는 미션 그라운드는 없다.
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"This Server Don't Have Mission Ground Packet["<<usType<<L"]");
		return true;
	}

	switch( usType )
	{
	case PT_N_M_NFY_LEAVE_PARTY_USER:
		{
			size_t const iPreviewSize = pkPacket->RdPos();
			bool bChangeMaster = false;
			BM::GUID kNewMasterGuid;

			pkPacket->Pop(kNewMasterGuid);// 의미없음
			pkPacket->Pop(kNewMasterGuid);// 의미없음
			pkPacket->Pop(bChangeMaster);
			if ( bChangeMaster )
			{
				pkPacket->Pop(kNewMasterGuid);
				PgMission::SetOwner(kNewMasterGuid);
			}

			pkPacket->RdPos(iPreviewSize);
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}break;
	case PT_N_M_NFY_PARTY_CHANGE_MASTER:
		{
			size_t const iPreviewSize = pkPacket->RdPos();
			BM::GUID kPartyGuid;
			BM::GUID kNewMasterGuid;
			pkPacket->Pop(kPartyGuid);
			pkPacket->Pop(kNewMasterGuid);

			PgMission::SetOwner(kNewMasterGuid);
			pkPacket->RdPos(iPreviewSize);
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}break;
	case PT_N_M_RES_MISSIONUPDATE:
		{
			__int64 kCause;
			pkPacket->Pop(kCause);
			switch (kCause)
			{
			case IMET_END_MISSION:
				{
					EndMission(pkPacket);
				}break;
			case IMET_SWAP_MISSION:
				{
					SwapStage(pkPacket);
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"unhandled Cause["<<kCause<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
				}break;
			}
		}break;
	case PT_T_M_MISSION_RANK_RESULT_ITEM:
		{
			BM::GUID kCharGuid;
			int iNewRank = 0;

			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(iNewRank);


			//int const iCount = 1;	// 1개만 소비
			int const iLevel = (GetLevel() + 1);			
			int const iRank = iNewRank;

			PgBase_Item kItem;
			bool bRet = GetRankItemResult(iLevel, iRank, kItem);
			if( false == bRet )
			{
				//VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("RankResultItem Give Fail") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkItem is NULL"));
				break;
			}

			CUnit * pkUnit = GetUnit(kCharGuid);
			if( pkUnit )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;
				kOrder.push_back(SPMO(IMET_INSERT_FIXED,pkUnit->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(), true)));
				if( !kOrder.empty() )
				{
					PgAction_ReqModifyItem kItemModifyAction(CIE_Mission_Rank, GroundKey(), kOrder);
					kItemModifyAction.DoAction(pkUnit, NULL);

					DWORD const iItemNo = kItem.ItemNo();
					BM::Stream kPacket(PT_M_C_MISSION_RANK_RESULT_ITEM);
					kPacket.Push( iItemNo );
					kPacket.Push( iRank );
					pkUnit->Send( kPacket );
				}
			}
		}break;
	default:
		{
			return PgIndun::RecvGndWrapped( usType, pkPacket );
		}break;
	}
	return true;
}

bool PgGround::Recv_PT_I_M_MYHOME_MOVE_TO_HOMETOWN(BM::Stream* const pkPacket )
{
	BM::GUID kGuid;
	pkPacket->Pop(kGuid);
	PgMyHome * pkHome = dynamic_cast<PgMyHome *>(GetUnit(kGuid));
	if(!pkHome)
	{
		return false;
	}

	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		BM::Stream kTempPacket;
		kTempPacket.Push(true);
		PgAction_ReqHomeExit kAction(this,GroundKey(),kTempPacket,false);
		kAction.DoAction(pkUnit,pkHome);
	}

	return true;
}

bool PgGround::MissionItemOrderCheck(PgPlayer *pkPlayer, CONT_PLAYER_MODIFY_ORDER & kOrder, int const iMissionNo, int const iCount,int const iMissionLevel)
{
	if( !pkPlayer )
	{
		return false;
	}

	if(GATTR_FLAG_CONSTELLATION & GetAttr())
	{//별자리 던전은 아이템 소비 무시
		return true;
	}

	int iSubCount = iCount;

	if(MAX_MISSION_LEVEL == iMissionLevel)
	{
		VEC_INT rkNeedItem;
		int const iHidenLevel = 5;
		PgMissionInfo const *pkMissionInfo = NULL;	
		PgMission_Base const *pkMissionBase = NULL;
		bool bRet = g_kMissionMan.GetMission( iMissionNo, pkMissionInfo );
		if(bRet)
		{
			SMissionOptionMissionOpen const *pkMissionOpen = NULL;
			pkMissionInfo->GetMissionOpen(iHidenLevel, pkMissionOpen);

			if(NULL != pkMissionOpen) 
			{
				pkMissionOpen->GetNeedItem(rkNeedItem);
			}
		}

		if(2 == rkNeedItem.size())
		{
			int const iInGameItem = *rkNeedItem.rbegin();
			int const iCashGameItem = *rkNeedItem.begin();
			int const iMyInGameCount = pkPlayer->GetInven()->GetTotalCount(iInGameItem);
			int const iMyCashGameCount = pkPlayer->GetInven()->GetTotalCount(iCashGameItem);
			int const iNoHindenModEnterNumber = 1;//원래 사용하던 레벨 6이 히든모드와 충돌이나서 아이템체크시 튕겨낸다.그것을 막기위해 아이템 1이 들어올경우는 아이템체크를 하지 않고 입장할수 있다.
			SItemPos kKeyItemPos;
			
			if((iNoHindenModEnterNumber == iInGameItem) && (iNoHindenModEnterNumber == iCashGameItem))
			{
				return true;
			}

			if(iMyInGameCount != 0)
			{
				if ( SUCCEEDED(pkPlayer->GetInven()->GetFirstItem( iInGameItem, kKeyItemPos, false, true )) )
				{
					PgBase_Item kKeyItem;
					if ( S_OK == pkPlayer->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
					{
						if( kKeyItem.Count() >= iSubCount )
						{
							SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -iSubCount));
							kOrder.push_back(kIMO_Temp);

							return true;
						}
					}
				}
			}
			else if((iMyCashGameCount != 0) && (iMyInGameCount == 0))
			{
				if ( SUCCEEDED(pkPlayer->GetInven()->GetFirstItem( iCashGameItem, kKeyItemPos, false, true )) )
				{
					PgBase_Item kKeyItem;
					if ( S_OK == pkPlayer->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
					{
						if( kKeyItem.Count() >= iSubCount )
						{
							SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -iSubCount));
							kOrder.push_back(kIMO_Temp);

							return true;
						}
					}
				}	
			}

		}
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	ContHaveItemNoCount	kItemCont;
	// 각 미션에 해당하는 아이템
	if( SUCCEEDED(pkPlayer->GetInven()->GetItems( UICT_MISSION_DEFENCE_CONSUME, kItemCont, true ) ) )
	{
		ContHaveItemNoCount::const_iterator itemno_itr = kItemCont.begin();
		for ( ; itemno_itr != kItemCont.end() ; ++itemno_itr )
		{
			SItemPos kKeyItemPos;
			if ( SUCCEEDED(pkPlayer->GetInven()->GetFirstItem( itemno_itr->first, kKeyItemPos, false, true )) )
			{
				CItemDef const * pItemDef = kItemDefMgr.GetDef(itemno_itr->first);
				if( !pItemDef )
				{
					continue;
				}

				int const iMissionNoItem = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

				if( iMissionNoItem == iMissionNo )
				{
					PgBase_Item kKeyItem;
					if ( S_OK == pkPlayer->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
					{
						if( kKeyItem.Count() >= iSubCount )
						{
							SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -iSubCount));
							kOrder.push_back(kIMO_Temp);

							return true;
						}
						else
						{
							SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -1));
							kOrder.push_back(kIMO_Temp);

							--iSubCount;
							break;
						}
					}
				}
			}
		}
	}

	

	// 어느 미션이든 사용가능한 아이템
	kItemCont.clear();

	if( SUCCEEDED(pkPlayer->GetInven()->GetItems( UICT_MISSION_DEFENCE_CONSUME_ALL, kItemCont, true ) ) )
	{
		ContHaveItemNoCount::const_iterator itemno_itr = kItemCont.begin();
		if( kItemCont.end() != itemno_itr )
		{
			SItemPos kKeyItemPos;
			if ( SUCCEEDED(pkPlayer->GetInven()->GetFirstItem( itemno_itr->first, kKeyItemPos, false, true )) )
			{
				PgBase_Item kKeyItem;
				if ( S_OK == pkPlayer->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
				{
					if( kKeyItem.Count() >= iSubCount )
					{
						SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -iSubCount));
						kOrder.push_back(kIMO_Temp);

						return true;
					}
					return false;
				}
			}
		}
	}


	return false;
}

bool PgGround::MissionItemOrderCheck(PgPlayer *pkPlayer, PgReqMapMove& kMapMove, int const iMissionNo, int const iCount,int const iMissionLevel)
{
	CONT_PLAYER_MODIFY_ORDER kOrder;
	bool const bRet = MissionItemOrderCheck(pkPlayer, kOrder, iMissionNo, iCount,iMissionLevel);
	
	for(CONT_PLAYER_MODIFY_ORDER::const_iterator order_it = kOrder.begin(); order_it != kOrder.end(); ++order_it)
	{
		kMapMove.AddModifyOrder(*order_it);
	}
	return bRet;
}

bool PgGround::MissionChaosItemOrderCheck(PgPlayer *pkPlayer, CONT_PLAYER_MODIFY_ORDER & kOrder, int const iMissionNo, int iCount)
{
	if( !pkPlayer )
	{
		return false;
	}

	int iSubCount = iCount;

	GET_DEF(CItemDefMgr, kItemDefMgr);

	ContHaveItemNoCount	kItemCont;
	// 각 미션에 해당하는 아이템
	if( SUCCEEDED(pkPlayer->GetInven()->GetItems( UICT_CHAOS_PORTAL, kItemCont, true ) ) )
	{
		ContHaveItemNoCount::const_iterator itemno_itr = kItemCont.begin();
		for ( ; itemno_itr != kItemCont.end() ; ++itemno_itr )
		{
			SItemPos kKeyItemPos;
			if ( SUCCEEDED(pkPlayer->GetInven()->GetFirstItem( itemno_itr->first, kKeyItemPos, false, true )) )
			{
				CItemDef const * pItemDef = kItemDefMgr.GetDef(itemno_itr->first);
				if( !pItemDef )
				{
					continue;
				}

				int const iMissionNoItem = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);

				if( iMissionNoItem == iMissionNo )
				{
					PgBase_Item kKeyItem;
					if ( S_OK == pkPlayer->GetInven()->GetItem( kKeyItemPos, kKeyItem ) )
					{
						if( kKeyItem.Count() >= iSubCount )
						{
							SPMO kIMO_Temp(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), SPMOD_Modify_Count(kKeyItem, kKeyItemPos, -iSubCount));
							kOrder.push_back(kIMO_Temp);

							return true;
						}
						return false;
					}
				}
			}
		}
	}

	return false;
}

bool PgGround::MissionChaosItemOrderCheck(PgPlayer *pkPlayer, PgReqMapMove& kMapMove, int const iMissionNo, int const iCount)
{
	CONT_PLAYER_MODIFY_ORDER kOrder;
	bool const bRet = MissionChaosItemOrderCheck(pkPlayer, kOrder, iMissionNo, iCount);

	for(CONT_PLAYER_MODIFY_ORDER::const_iterator order_it = kOrder.begin(); order_it != kOrder.end(); ++order_it)
	{
		kMapMove.AddModifyOrder(*order_it);
	}
	return bRet;
}

bool PgGround::RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket )
{
	switch(usType)
	{
	case PT_I_M_ANS_HOME_VISITLOG_ADD:
		{
			HRESULT kErr;
			pkPacket->Pop(kErr);
			BM::GUID kOwnerGuid;
			BM::GUID kHomeGuid;
			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kHomeGuid);

			CONT_ITEMPOS kCont;
			PU::TLoadArray_A(*pkPacket, kCont);

			CUnit * pkUnit = GetUnit(kOwnerGuid);
			if(!pkUnit)
			{
				return false;
			}

			PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pkUnit);
			if(pkUser)
			{
				PgMyHome * pkHome = dynamic_cast<PgMyHome*>(GetUnit(kHomeGuid));
				if(pkHome)
				{
					for(CONT_ITEMPOS::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
					{
						BM::Stream kPacket;
						kPacket.Push((*iter));
						PgAction_ReqHomeUseItemEffect kAction(GroundKey(),kPacket);
						kAction.DoAction(pkUser,pkHome);
					}
				}
			}

			BM::Stream kPacket(PT_M_C_ANS_HOME_VISITLOG_ADD);
			kPacket.Push(S_OK);
			pkUnit->Send(kPacket);
		}break;
	case PT_I_M_MYHOME_MODIFY_ABIL:
		{
			BM::GUID kGuid;
			pkPacket->Pop(kGuid);
			VEC_ABILINFO kCont;
			PU::TLoadArray_A(*pkPacket,kCont);
			CUnit * pkUnit = GetUnit(kGuid);
			if(pkUnit)
			{
				for(VEC_ABILINFO::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
				{
					pkUnit->SetAbil((*iter).wType,(*iter).iValue);
					pkUnit->SendAbil(static_cast<EAbilType>((*iter).wType),E_SENDTYPE_BROADALL);
				}
			}
		}break;
	case PT_I_M_MYHOME_MOVE_TO_HOMETOWN:
		{
			Recv_PT_I_M_MYHOME_MOVE_TO_HOMETOWN(pkPacket);
		}break;
	case PT_I_M_MYHOME_NOTI_MODIFY_OWNER:
		{
		}break;
	case PT_I_M_REQ_HOME_CREATE:
		{
			PgMyHome kMyHome;
			kMyHome.ReadFromPacket(*pkPacket);
			BM::Stream kPacket;
			kMyHome.WriteToPacket(kPacket);
			PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(g_kTotalObjMgr.CreateUnit(UT_MYHOME, kMyHome.GetID() ) );
			if ( pkMyHome )
			{
				pkMyHome->ReadFromPacket(kPacket);
				pkMyHome->SetState(US_IDLE);
				pkMyHome->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
				if( false == AddUnit( pkMyHome, false ) )
				{
					g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pkMyHome) );
				}
			}
		}break;
	case PT_I_M_ANS_HOMETOWN_ENTER:
		{
			BM::GUID kOwnerGuid;
			SReqMapMove_MT kRMM;

			pkPacket->Pop(kOwnerGuid);
			pkPacket->Pop(kRMM);

			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(pkPlayer)
			{
				BM::Stream kPacket(PT_M_C_ANS_HOMETOWN_ENTER);
				kPacket.Push(*pkPacket);
				pkPlayer->Send(kPacket);

				if ( !g_kProcessCfg.IsPublicChannel() )
				{
					kRMM.cType = MMET_GoToPublicGround;
				}

				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					kMapMove.DoAction();
				}
			}
		}break;
	case PT_I_M_CS_NOTI_CASH_MODIFY_NC:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			unsigned short usPointId = 0;
			unsigned __int64 i64Point = 0i64;
			unsigned __int64 uiPointDifference = 0i64;
			
			pkPacket->Pop(usPointId);
			pkPacket->Pop(i64Point);
			pkPacket->Pop(uiPointDifference);

			INFO_LOG(BM::LOG_LV7, __FL__ << _T("Recv PT_I_M_CS_NOTI_CASH_MODIFY_NC Point ID:") << usPointId << _T(", Point Amount: ") << i64Point);

			PgPlayer * pkPlayer = GetUser(kOwnerGuid);
			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_CS_NOTI_CASH_MODIFY);

			if( NC::SAPT_CASH == usPointId )
			{
				pkPlayer->SetAbil64(AT_CASH,i64Point);
				pkPlayer->SendAbil64(AT_CASH);
				
				if( uiPointDifference )
				{
					kPacket.Push(uiPointDifference);
					kPacket.Push(static_cast<__int64>(0));
					pkPlayer->Send(kPacket);
				}
			}
			else if( NC::SAPT_HAPPYCOIN == usPointId )
			{
				pkPlayer->SetAbil64(AT_BONUS_CASH,i64Point);
				pkPlayer->SendAbil64(AT_BONUS_CASH);
				
				if( uiPointDifference )
				{
					kPacket.Push(static_cast<__int64>(0));
					kPacket.Push(uiPointDifference);
					pkPlayer->Send(kPacket);
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}			
		}break;
	case PT_I_M_CS_NOTI_CASH_MODIFY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			__int64 i64Cash = 0,
				i64DiffCash = 0,
				i64Bonus = 0,
				i64DiffBonus = 0;
			pkPacket->Pop(i64Cash);
			pkPacket->Pop(i64Bonus);
			pkPacket->Pop(i64DiffCash);
			pkPacket->Pop(i64DiffBonus);

			PgPlayer * pkPlayer = GetUser(kOwnerGuid);
			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if(i64DiffCash || i64DiffBonus)
			{
				BM::Stream kPacket(PT_M_C_CS_NOTI_CASH_MODIFY);
				kPacket.Push(i64DiffCash);
				kPacket.Push(i64DiffBonus);
				pkPlayer->Send(kPacket);
			}

			pkPlayer->SetAbil64(AT_CASH,i64Cash);
			pkPlayer->SetAbil64(AT_BONUS_CASH,i64Bonus);
			pkPlayer->SendAbil64(AT_CASH);
			pkPlayer->SendAbil64(AT_BONUS_CASH);

			PgSyncClinetAchievementHandler<> kSA(AT_ACHIEVEMENT_CASH_BONUS, i64Bonus, GroundKey());
			kSA.DoAction(pkPlayer,NULL);
		}break;
	case PT_I_M_UM_ANS_ARTICLE_REG:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_REG);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_ARTICLE_DEREG:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_DEREG);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_MARKET_QUERY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_MARKET_QUERY);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_MY_MARKET_QUERY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_MY_MARKET_QUERY);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_ARTICLE_BUY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_BUY);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_DEALINGS_READ:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_DEALINGS_READ);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_UM_ANS_MINIMUM_COST_QUERY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_UM_ANS_MINIMUM_COST_QUERY);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_POST_ANS_MAIL_MODIFY:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_POST_ANS_MAIL_MODIFY);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_POST_ANS_MAIL_MIN:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_POST_ANS_MAIL_MIN);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_POST_ANS_MAIL_RECV:
		{
			BM::GUID kOwnerGuid;

			pkPacket->Pop(kOwnerGuid);
			PgPlayer * pkPlayer = GetUser(kOwnerGuid);

			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket(PT_M_C_POST_ANS_MAIL_RECV);
			kPacket.Push(*pkPacket);
			pkPlayer->Send(kPacket);
		}break;
	case PT_I_M_POST_ANS_MAIL_SEND:
		{
			BM::GUID kOwnerGuid;
			pkPacket->Pop(kOwnerGuid);

			CUnit *pkCaster = GetUnit(kOwnerGuid);
			if(!pkCaster)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			BM::Stream kPacket;
			kPacket.Push(PT_M_C_POST_ANS_MAIL_SEND);
			kPacket.Push(*pkPacket);
			pkCaster->Send(kPacket);
		}break;
	case PT_I_M_ANS_MODIFY_ITEM:
		{
			BM::GUID kCasterGuid;
			BM::GUID kOwnerGuid;
			DB_ITEM_STATE_CHANGE_ARRAY kChangeArray;

			EItemModifyParentEventType kCause;
			HRESULT hRet = E_FAIL;
			bool bIsAddonPacket = false;

			pkPacket->Pop(kCause);//
			pkPacket->Pop(hRet);//
			pkPacket->Pop(kCasterGuid);//
			pkPacket->Pop(kOwnerGuid);//
			PU::TLoadArray_M(*pkPacket,kChangeArray);
			pkPacket->Pop(bIsAddonPacket);

			BM::Stream kAddonPacket;
			if(bIsAddonPacket)
			{
				BM::Stream::STREAM_DATA kData;
				pkPacket->Pop(kData);
				kAddonPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));
				//size_t const remain_size = pkPacket->RemainSize();
				//kAddonPacket.Resize(remain_size);
				//pkPacket->Pop(kAddonPacket.Data());
				//kAddonPacket.PosAdjust();
			}

			if ( kOwnerGuid == kCasterGuid )
			{// Player or MyHome 유닛이다.
				CUnit *pkCaster = PgObjectMgr2::GetUnit( UT_PLAYER, kCasterGuid );

 				if( !pkCaster )
 				{
					pkCaster = PgObjectMgr2::GetUnit( UT_MYHOME, kCasterGuid );
				}

				if(pkCaster)
				{
					PgAction_ModifyPlayerData kAction(kCause, hRet, kChangeArray, this, this->m_kLocalPartyMgr, m_kEventItemSetMgr, kAddonPacket);
					kAction.DoAction(pkCaster, NULL);
				}
				else
				{
					//로그아웃한 경우
					switch(kCause)
					{
					case CIE_GateWayUnLock:
						{
							ProcessGateWayUnLock(hRet, NULL, kAddonPacket);
						}break;
					}
				}
			}
			else
			{// Pet이다.
				CUnit *pkCaster = PgObjectMgr2::GetUnit( UT_PET, kCasterGuid );
				if ( pkCaster )
				{
					PgAction_ModifyPlayerData kAction(kCause, hRet, kChangeArray, this, this->m_kLocalPartyMgr, m_kEventItemSetMgr, kAddonPacket);
					kAction.DoAction(pkCaster, NULL);
				}
				else
				{
					// 없네.(이경우는 죽은 펫의 아이템을 뺀 경우이다.
					CUnit *pkCasterOwner = PgObjectMgr2::GetUnit( UT_PLAYER, kOwnerGuid );
					if( pkCasterOwner )
					{
						BM::Stream kPacket( PT_M_C_NFY_PET_INVENTORY_ACTION, kCasterGuid );
						kPacket.Push(kCause);
						PU::TWriteArray_M( kPacket, kChangeArray);
						pkCasterOwner->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
					}
				}
			}

		}break;
	case PT_T_M_REQ_CHANGE_CHARACTER_POS:
		{
			BM::GUID		CharacterId;
			int				iMapNo;
			float			fPosX, fPosY, fPosZ;

			pkPacket->Pop(CharacterId);
			pkPacket->Pop(iMapNo);	
			pkPacket->Pop(fPosX);		
			pkPacket->Pop(fPosY);		
			pkPacket->Pop(fPosZ);		
			POINT3 ptPos(fPosX, fPosY, fPosZ);

			CUnit *pkUnit = GetUnit(CharacterId);
			if(!pkUnit)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			SReqMapMove_MT kRMM;
			kRMM.kCasterKey.GroundNo(pkPlayer->GetRecentMapNo(GATTR_DEFAULT));
			kRMM.kTargetKey.GroundNo(iMapNo);
			kRMM.pt3TargetPos = ptPos;

			PgReqMapMove kMapMove( this, kRMM, NULL );
			if ( kMapMove.Add( pkPlayer ) )
			{
				kMapMove.DoAction();
			}
		}break;
	case PT_T_M_REQ_CHANGE_SKILL_INFO:
	case PT_T_M_REQ_CHANGE_QUEST_INFO:
		{
		}break;
	case PT_T_M_REQ_USER_MUTE:
		{
			BM::GUID kTargetGuid;
			int iSecTime = 0;
			pkPacket->Pop(kTargetGuid);
			pkPacket->Pop(iSecTime);

			PgPlayer* pkPlayer = GetUser(kTargetGuid);
			if( pkPlayer )
			{
				if ( 0 > iSecTime )
				{
					pkPlayer->DeleteEffect( EFFECTNO_MUTECHAT );
				}
				else
				{
					// 해제
					SEffectCreateInfo kCreate;
					kCreate.eType = EFFECT_TYPE_PENALTY;
					kCreate.iEffectNum = EFFECTNO_MUTECHAT;
					kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					PgGroundUtil::SetActArgGround(kCreate.kActArg, this);

					if ( iSecTime )
					{
						SYSTEMTIME kLocalTime;
						g_kEventView.GetLocalTime(&kLocalTime);
						CGameTime::AddTime(kLocalTime, iSecTime * CGameTime::SECOND );
						kCreate.kWorldExpireTime = BM::DBTIMESTAMP_EX(kLocalTime);
					}
					else
					{
						kCreate.kWorldExpireTime.year = 2063;
						kCreate.kWorldExpireTime.month = 10;
						kCreate.kWorldExpireTime.day = 23;
					}

					pkPlayer->AddEffect( kCreate );
				}
			}	
		}break;
	case PT_T_M_REQ_CHARACTER_BASEINFO:
		{
			BM::GUID kCharacterId;
			SNcGMCommandPack kNcPack;
			int iErrorcode;
			pkPacket->Pop(iErrorcode);
			kNcPack.ReadFromPacket(pkPacket);
			
			CUnit *pkUnit = GetUnit(kNcPack.kGuid[0]);
			if(!pkUnit)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if(!pkPlayer)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			//맵서버에서 체크해야할 것들은 실시간적으로 바뀔 수 있는것들(레벨, 직업, HP,MP등등이다.)
			kNcPack.byValues[0] = (BYTE)pkPlayer->GMLevel();
			kNcPack.sValues[1] = (short)pkPlayer->GetAbil(AT_LEVEL);
			kNcPack.byValues[1] = (BYTE)pkPlayer->GetAbil(AT_GENDER);
			kNcPack.iValues[3] = pkPlayer->GetAbil(AT_HP);
//			kNcPack.sValues[2] = pkPlayer->Get
			kNcPack.iValues[4] = pkPlayer->GetAbil(AT_MP);
//			kNcPack.sValues[3] = pkPlayer->Get
			kNcPack.biValues[0] = pkPlayer->GetAbil64(AT_EXPERIENCE);
			kNcPack.sValues[4] = (short)pkPlayer->GetAbil(AT_SP);
			kNcPack.byValues[2] = (BYTE)pkPlayer->GetAbil(AT_CLASS);
			kNcPack.iValues[5] = pkPlayer->GetRecentMapNo(GATTR_DEFAULT);
			POINT3 ptPos = pkPlayer->GetRecentPos(GATTR_DEFAULT);
			kNcPack.iValues[6] = (int)ptPos.x;
			kNcPack.iValues[7] = (int)ptPos.y;
			kNcPack.iValues[8] = (int)ptPos.z;

			BM::Stream kPacket(PT_M_T_ANS_CHARACTER_BASEINFO);
			kPacket.Push(iErrorcode);
			kNcPack.WriteToPacket(kPacket);
			SendToCenter(kPacket);
		}break;
	case PT_T_M_ANS_REGIST_HARDCORE_VOTE:
		{
			Recv_PT_T_M_ANS_REGIST_HARDCORE_VOTE( *pkPacket );
		}break;
	case PT_T_M_ANS_RET_HARDCORE_VOTE_CANCEL:
		{
			Recv_PT_T_M_ANS_RET_HARDCORE_VOTE_CANCEL( *pkPacket );
		}break;
	case PT_T_M_REQ_JOIN_HARDCORE:
		{
			Recv_PT_T_M_REQ_JOIN_HARDCORE( *pkPacket );
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

bool PgIndun::RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket)
{
	return PgGround::RecvGndWrapped_ItemPacket( usType, pkPacket );
}


bool PgMissionGround::RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket)
{
	switch( usType )
	{
	case 0:
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"usType is ZERO");
		}break;
	default:
		{
			return PgIndun::RecvGndWrapped_ItemPacket( usType, pkPacket );
		}break;
	}
	return true;
}

bool PgWarGround::RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket )
{
	switch( usType )
	{
	case PT_I_M_ANS_MODIFY_ITEM:
		{
			size_t const iPos = pkPacket->RdPos();

			EItemModifyParentEventType kCause;
			HRESULT hRet = E_FAIL;
			pkPacket->Pop(kCause);//
			pkPacket->Pop(hRet);

			if ( (IMEPT_PVP == kCause) && (S_OK == hRet) )
			{
				if ( true == IsModeType( PVP_TYPE_ALL ) )
				{
					pkPacket->RdPos( iPos - sizeof(BM::Stream::DEF_STREAM_TYPE) );
					SendToPvPLobby( *pkPacket, m_pkMode->GetPvPLobbyID() );
				}
			}
			pkPacket->RdPos( iPos );
			
		}// no break
	default:
		{
			return PgGround::RecvGndWrapped_ItemPacket( usType, pkPacket );
		}break;
	}
	return true;
}

bool PgExpeditionGround::RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket )
{
	switch( Type)
	{
	case PT_N_M_NFY_EXPEDITION_RESULT_END:
		{
			BM::Stream Packet(PT_M_C_NFY_EXPEDITION_RESULT_END);	// 결산 종료 됨을 알림.

			CUnit * pUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_iter;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
			while( (pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
			{
				pUser->Send(Packet);
			}

			SetAutoNextState(10000);	// 10초 있다가 던젼 닫음.
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( Type, pPacket );
		}break;
	}
	return true;
}

bool PgConstellationGround::RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket )
{
	switch( Type)
	{
	default:
		{
			return PgGround::RecvGndWrapped( Type, pPacket );
		}break;
	}
	return true;
}

bool PgExpeditionLobby::RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket )
{
	switch( Type )
	{
	case PT_N_M_NFY_JOIN_EXPEDITION_AWAITER:
		{
			AddJoinExpeditionWaitList(*pPacket);
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( Type, pPacket );
		}break;
	}
	return true;
}

bool PgStaticEventGround::RecvGndWrapped(unsigned short Type, BM::Stream* const pPacket)
{
	switch(Type)
	{
	case PT_T_M_NFY_COERCION_START_EVENT:
		{
			int EventNo = 0;
			pPacket->Pop(EventNo);
			GMCommandStartEvent(EventNo);
		}break;
	default:
		{
			return PgGround::RecvGndWrapped(Type, pPacket);
		}break;
	}

	return true;
}