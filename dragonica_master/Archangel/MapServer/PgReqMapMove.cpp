#include "stdafx.h"
#include "Variant/PgPortalAccess.h"
#include "PgGround.h"
#include "PgReqMapMove.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"
#include "PgAction.h"
#include "PgConstellationMgr.h"

PgReqMapMove::PgReqMapMove( PgGround * const pkGround, SReqMapMove_MT const &kRMM, PgPortalAccess const * const pkAccess )
:	m_pkGround(pkGround)
,	m_kRMM(kRMM)
,	m_pkAccess(pkAccess)
{
	m_kRMM.kCasterKey = pkGround->GroundKey();
	m_kRMM.kCasterSI = g_kProcessCfg.ServerIdentity();
	if(GATTR_INSTANCE & pkGround->GetAttr())
	{
		m_ConstellationKey = pkGround->GetConstellationKey();
	}

	if ( m_pkAccess )
	{
		m_pkAccess->Get( m_kRMM );
	}
}

PgReqMapMove::~PgReqMapMove(void)
{
}

bool PgReqMapMove::Add( PgPlayer *pkPlayer )
{
	if ( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( pkPlayer->IsMapLoading() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iHideType = pkPlayer->GetAbil( AT_FAKE_REMOVE_UNIT );

	switch(iHideType)
	{
	case FRT_ONLY_HIDE:
		{
			BM::Stream kCPacket( PT_T_C_NFY_CHARACTER_MAP_MOVE );
			kCPacket.Push(static_cast<BYTE>(MMET_Failed));
			kCPacket.Push(m_pkGround->GroundKey());
			kCPacket.Push(m_pkGround->GetAttr());
			kCPacket.Push(pkPlayer->GetID());
			kCPacket.Push(static_cast<size_t>(1));
			pkPlayer->WriteToPacket( kCPacket, WT_MAPMOVE_CLIENT );
			pkPlayer->Send(kCPacket);
			return false;
		}break;
	case FRT_NONE:
	case FRT_HIDE_AND_NO_MSG_BUT_MAPMOVE:
		{
		}break;
	case FRT_HIDE_AND_NO_MSG:
	default:
		{
			// CashShop에 있는 경우이다.
			return false;
		}break;
	}

	bool const bMaster = m_kContPlayer.empty();
	if ( m_pkAccess )
	{
		if ( !m_pkAccess->IsAccess( pkPlayer, bMaster, &m_kStandByItemOrder ) )
		{
			if ( !bMaster )
			{
				(*m_kContPlayer.begin())->SendWarnMessage( 802 );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if ( bMaster )
	{
		m_kRMM.pt3CasterPos = pkPlayer->GetPos();
	}

	if ( PgGround::ms_kAntiHackCheckActionCount.m_bUseAntiHackCheckActionCount )
	{
		PLAYER_ACTION_TIME_COUNT_RESULT kResult;
		if(pkPlayer->ResultActionPacketCount(true, &kResult, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime, PgGround::ms_kAntiHackCheckActionCount.m_iCheckCountSum, PgGround::ms_kAntiHackCheckActionCount.m_iActionCount_CheckTime_Mutiflier))
		{	
			if (PgGround::ms_kAntiHackCheckActionCount.m_bForcedResultView || kResult.m_bHackingWarning )
			{
				int iGroundNo = 0;
				if(m_pkGround)
				{
					iGroundNo = m_pkGround->GroundKey().GroundNo();
				}

				BM::vstring kLogMsg;
				kLogMsg << __FL__ << "[Hacking][ActionCount] User[" << pkPlayer->Name() << "] GUID[" 
					<< pkPlayer->GetID() << "]  MemberGUID[" << pkPlayer->GetMemberGUID() << _T("] GroundNo[") << iGroundNo 
					<< _T("] HackingType[FastAction] RecvPacketCount[") << kResult.m_iRecvPacketCount << _T("] TotalAniTime[")  << kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
				//VERIFY_INFO_LOG(false, BM::LOG_LV1, kLogMsg) ;
				HACKING_LOG(BM::LOG_LV0, kLogMsg);

				//Player에게 결과를 보여줄 경우(Debug)
				if((g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) && PgGround::ms_kAntiHackCheckActionCount.m_bResultToPlayer)
				{
					BM::vstring kLogMsg; kLogMsg << _T("HackingType[FastAction] RecvPacketCount[") << kResult.m_iRecvPacketCount << _T("] TotalAniTime[") << kResult.m_iTotalActionAniTime << _T("] ClientElapsedTime[") << kResult.m_iClientElapsedTime << _T("] ServerElapsedTime[") << kResult.m_iServerElapsedTime << _T("]");
					pkPlayer->SendWarnMessageStr(kLogMsg);
				}

				//해킹 포인트를 사용 할 경우
				//일단 맵이동시에는 로그만 남기고 점수를 주지 않는다.
				//if(kResult.m_bHackingWarning && PgGround::ms_kAntiHackCheckActionCount.m_bHackingIndex)
				//{
				//	if ( pkPlayer->SuspectHacking(EAHP_ActionCount, static_cast<short>(PgGround::ms_kAntiHackCheckActionCount.m_iAddHackingPoint)) )
				//	{
				//		// SpeedHack이 의심스러우니 강제접속 해제 시킨다.
				//		BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_ActionCount_Hack_A) );
				//		kDPacket.Push( pkPlayer->GetMemberGUID() );
				//		SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
				//	}
				//}
			}
		}
	}

	// by reOiL now get max lv in party
	int iLevel = 0;
	if (m_pkGround->GetPartyMaxLv(pkPlayer->GetPartyGuid(), iLevel))
	{
		m_kRMM.iOwnerLv = std::max(iLevel, pkPlayer->GetAbil(AT_LEVEL));
	}
	/*
	// PartyOwner Lv
	BM::GUID kMasterGuid;
	if( (false == m_pkGround->GetPartyMasterGuid(pkPlayer->GetPartyGuid(), kMasterGuid))
	 || (kMasterGuid == pkPlayer->GetID() && m_kRMM.iOwnerLv < pkPlayer->GetAbil(AT_LEVEL)) )
	{
		m_kRMM.iOwnerLv = pkPlayer->GetAbil(AT_LEVEL);
	}
	*/

	m_kContPlayer.push_back( pkPlayer );
	return true;
}

void PgReqMapMove::AddModifyOrder(SPMO const & kOrder)
{
	m_kStandByItemOrder.push_back(kOrder);
}

bool PgReqMapMove::DoAction(void)
{
	if ( m_kContPlayer.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{// 콤보 저장
		CONT_PLAYER::iterator itr = m_kContPlayer.begin();
		while ( itr != m_kContPlayer.end() )
		{
			PgPlayer *pkPlayer = *itr;
			if( pkPlayer )
			{
				BM::Stream kPacket( PT_U_G_RUN_ACTION );
				kPacket.Push( static_cast< short >( GAN_SaveCombo ) );
				kPacket.Push( pkPlayer->GetID() );
				pkPlayer->VNotify( &kPacket );
			}

			++itr;
		}
	}

	if( m_pkAccess
	&&	!m_kStandByItemOrder.empty() ) // 트리거에 의한 경우 아이템을 선처리 해준다
	{
		VEC_GUID kVec;

		CUnit* pkMasterPlayer = m_kContPlayer.front();
		CONT_PLAYER::const_iterator iter = m_kContPlayer.begin();
		while( m_kContPlayer.end() != iter )
		{
			kVec.push_back( (*iter)->GetID() );
			++iter;
		}

		BM::Stream kAddonPacket;
		kAddonPacket.Push( m_pkAccess->GetErrorMessage() );
		kAddonPacket.Push( static_cast< WORD >(PT_U_G_RUN_ACTION) );
		kAddonPacket.Push( static_cast< short >(GAN_MapMove) );
		kAddonPacket.Push( m_kRMM );
		kAddonPacket.Push( kVec );

		PgAction_ReqModifyItem kAction(CIE_GateWayUnLock, m_pkGround->GroundKey(), m_kStandByItemOrder, kAddonPacket);
		return kAction.DoAction(pkMasterPlayer, NULL);
	}
	else
	{
		EWRITETYPE kWriteType = WT_MAPMOVE_SERVER;

		// To Client
		BM::Stream kCPacket( PT_M_C_NFY_MAP_MOVING, m_kRMM.kTargetKey.GroundNo() );

		// To Center
		BM::Stream kTPacket(PT_M_T_REQ_MAP_MOVE, m_kRMM);

		Constellation::SConstellationMission constellationMission;
		GetConstellationMission( m_ConstellationKey, constellationMission );
		constellationMission.WriteToPacket(kTPacket);

		// UNIT_PTR_ARRAY WriteToPacket이랑 동일하게 동작해야 한다!!
		size_t const iWRPos = kTPacket.WrPos();
		size_t const iOrgSize = m_kContPlayer.size();
		size_t iWriteSize = 0;

		kTPacket.Push( iOrgSize );

		size_t iPetSize = 0;
		BM::Stream kPetPacket;
		size_t iUnitSummonedSize = 0;
		BM::Stream kUnitSummonedPacket;

		CONT_PLAYER::iterator itr = m_kContPlayer.begin();
		while ( itr!=m_kContPlayer.end() )
		{
			PgPlayer *pkPlayer = *itr;
			if ( pkPlayer )
			{
				pkPlayer->SetTargetGroundNo(m_kRMM.kTargetKey.GroundNo());

				// GroundDeleteEffect
				PgAction_GroundDeleteEffect kAction(m_pkGround);
				kAction.DoAction(pkPlayer, NULL);

				switch( m_kRMM.cType )
				{
				case MMET_BackToPvP:
				case MMET_KickToPvP:
					{
						kWriteType |= WT_OP_NOHPMP;
					}// break을 사용하지 않는다.
				case MMET_PvP:
				case MMET_BATTLESQUARE:
					{
						pkPlayer->ClearAllEffect( true, false, true );
						pkPlayer->SetSyncType( SYNC_TYPE_DEFAULT, true );
					}break;
				default:
					{
						if ( GKIND_WAR == m_pkGround->GetKind() )
						{
							pkPlayer->ClearAllEffect( true, false, true );
						}
						else
						{
							DeleteElemStatusEffect(pkPlayer);
						}
					}break;
				}

				if( (m_kRMM.cType != MMET_MissionDefence8) && (BM::GUID::IsNotNull(pkPlayer->PartyGuid())) )
				{
					if( static_cast<int>(PI_PLAY) != m_pkGround->DefenceIsJoinParty(pkPlayer->PartyGuid()) )
					{
						if( true == m_pkGround->DefenceDelWaitParty(pkPlayer->PartyGuid()) )
						{
							// 등록 취소 메시지
							pkPlayer->SendWarnMessage(401128);
						}
					}
				}

				pkPlayer->SetPos( m_kRMM.pt3CasterPos );
				if( pkPlayer->OpenVendor() || pkPlayer->VendorGuid().IsNotNull() )
				{
					m_pkGround->RecvVendorClose(pkPlayer);
				}
	//			m_pkGround->FakeAddUnit( pkPlayer );
                int const iTeam = pkPlayer->GetAbil(AT_TEAM);

				if( m_pkGround->GetAttr() & GATTR_INSTANCE ) // 던전에서 피격 회수 업적 검사용 어빌 저장
				{
					COMBO_TYPE const iAttackedCount = m_pkGround->GetCurrentCombo( pkPlayer->GetID(), ECOUNT_TOTAL_DAMAGE ) + pkPlayer->GetAbil( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT );
					pkPlayer->SetAbil( AT_ACHIEVEMENT_DUNGEON_ATTACKED_COUNT, iAttackedCount );
				}
				
				pkPlayer->SetAbil( AT_CALL_MARKET, 0 );// 맵이동을 하면 아이템으로 부여된 마켓,창고,상점 오픈 어빌 삭제된다.
				pkPlayer->SetAbil( AT_CALL_SAFE, 0 );
				pkPlayer->SetAbil( AT_CALL_STORE, 0 );
				pkPlayer->SetAbil( AT_BATTLE_LEVEL, 0 );
				pkPlayer->SetAbil( AT_TEAM, 0 );
				pkPlayer->SetAbil( AT_CALL_SHARE_SAFE, 0 );
				pkPlayer->Send( kCPacket );

				//	반드시 순서대로 진행을 해야 한다!!
				pkPlayer->ReadyNetwork( false );

				// 1. Player를 패킷으로 만든다.
				pkPlayer->WriteToPacket( kTPacket, kWriteType );

				// 2. Pet이 있나 확인하여 Pet을 패킷으로 만들고, 지운다.
				PgPet *pkPet = m_pkGround->GetPet( pkPlayer );
				if ( pkPet )
				{	
					pkPet->WriteToPacket_SPetMapMoveData( kPetPacket );
					++iPetSize;
				}

				// 3. 소환체가 있나 확인하여 소환체를 패킷으로 만들고, 지운다.
				size_t iSummonedSize = 0;
				BM::Stream kSummonedPacket;
				CUnit * pkUnit = NULL;
				PgSummoned * pkSummoned = NULL;
				VEC_SUMMONUNIT const& kContSummonUnit = pkPlayer->GetSummonUnit();
				for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
				{
					pkUnit = m_pkGround->GetUnit((*c_it).kGuid);
					if(pkUnit && pkUnit->IsUnitType(UT_SUMMONED) && (pkSummoned = dynamic_cast<PgSummoned*>(pkUnit)) && (false==pkSummoned->IsNPC()))
					{
						pkSummoned->WriteToPacket_MapMoveData( kSummonedPacket );
						++iSummonedSize;
					}
				}
				if(iSummonedSize)
				{
					kUnitSummonedPacket.Push(pkPlayer->GetID());
					kUnitSummonedPacket.Push(iSummonedSize);
					kUnitSummonedPacket.Push(kSummonedPacket);
					++iUnitSummonedSize;
				}

				// 4. Player를 지운다.(펫은 이속에서 지운다)
                m_pkGround->SetTeam(pkPlayer->GetID(), iTeam);
				m_pkGround->ReleaseUnit( pkPlayer );

				++iWriteSize;
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!! NULL Player") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
			}

			++itr;
		}
		m_kContPlayer.clear();

		if ( iOrgSize != iWriteSize )
		{
			if ( !iWriteSize )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			kTPacket.ModifyData( iWRPos, &iWriteSize, sizeof(size_t) );
		}

		kTPacket.Push( iPetSize );
		kTPacket.Push( kPetPacket );

		kTPacket.Push( iUnitSummonedSize );
		kTPacket.Push( kUnitSummonedPacket );
		
		m_kStandByItemOrder.WriteToPacket( kTPacket );
		::SendToCenter( kTPacket );
	}
	return true;
}


PgAction_MapLoaded::PgAction_MapLoaded(PgGround* const pkGround)
	:	m_pkGround(pkGround)
{
}

void PgAction_MapLoaded::RefreshPassiveSkill( PgControlUnit *pkControlUnit, PgGround* const pkGround, PgControlUnit *pkCallerUnit )
{
	if ( !pkControlUnit )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"ControlUnit is NULL" );
		return;
	}

	SActArg kArg;
	PgGroundUtil::SetActArgGround(kArg, pkGround);

	if ( pkCallerUnit )
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);

		int i = 0;
		int iSkill = 0;
		while ((iSkill = pkControlUnit->GetMySkill()->GetSkillNo(EST_PASSIVE, i++)) > 0)
		{
			CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkill);
			if(pkSkillDef)
			{
				if ( ESTARGET_SELF & pkSkillDef->GetTargetType() )
				{
					int const iOverLevel = pkCallerUnit->GetMySkill()->GetOverSkillLevel( iSkill );
					CEffect const* pkEffect = pkControlUnit->GetEffect( iSkill + iOverLevel, true );
					if ( !pkEffect )
					{
						g_kSkillAbilHandleMgr.SkillPassive( pkControlUnit, iSkill + iOverLevel, &kArg );
					}
				}

				if ( ESTARGET_CASTER & pkSkillDef->GetTargetType() )
				{// Caller한테도 걸어주어야 한다.
					if ( pkCallerUnit )
					{
						int const iOverLevel = pkCallerUnit->GetMySkill()->GetOverSkillLevel( iSkill );
						CEffect const* pkEffect = pkCallerUnit->GetEffect( iSkill + iOverLevel, true );
						if ( !pkEffect )
						{
							g_kSkillAbilHandleMgr.SkillPassive( pkCallerUnit, iSkill + iOverLevel, &kArg );
						}
					}
				}
			}
		}
	}
	else
	{
		int i = 0;
		int iSkill = 0;
		while ((iSkill = pkControlUnit->GetMySkill()->GetSkillNo(EST_PASSIVE, i++)) > 0)
		{
			int const iOverLevel = pkControlUnit->GetMySkill()->GetOverSkillLevel(iSkill);
			g_kSkillAbilHandleMgr.SkillPassive( pkControlUnit, iSkill + iOverLevel, &kArg );
		}
	}
}

bool PgAction_MapLoaded::DoAction(CUnit* pkCaster, CUnit*)
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
	if (pkPlayer == NULL || NULL == m_pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkPlayer->ClearInstantAbils(m_pkGround->GetAttr());

	pkPlayer->ReadyNetwork(false);
	pkPlayer->GroundKey(m_pkGround->GroundKey());

	// Active Skill Setting
	// NOTICE : 아래의 Skill_Passive에서도 Effect를 생성하기 때문에
	//	아래의 DoEffect를 Check_Passive보다 먼저 해야 만 한다.
	pkPlayer->DoEffect();

	int const iLevelRank = pkPlayer->GetAbil( AT_LEVEL_RANK );
	if ( iLevelRank )
	{
		int const iNewEffectNo = ( EFFECTNO_CHARACTER_BONUS_EFFECT_BASENO + iLevelRank - 1 );

		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, m_pkGround);

		pkPlayer->AddEffect( iNewEffectNo, 0, &kArg, NULL, EFFECT_TYPE_ABIL );
	}

	{
		// 몇몇 Effect는 Ground에 의하여 제약이 걸린다.
		ContEffectItor kItor;
		CEffect* pkEffect = NULL;
		PgUnitEffectMgr const& rkEffectMgr = pkPlayer->GetEffectMgr();
		rkEffectMgr.GetFirstEffect(kItor);
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		std::vector<int> kDeleteEffect;
		while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
		{
			if(kEffectDefMgr.GetAbil(pkEffect->GetEffectNo(), AT_EXCEPT_GROUND_TYPE) & m_pkGround->GetAttr())
			{
				kDeleteEffect.push_back(pkEffect->GetKey());
			}
		}

		size_t const iDeleteSize = kDeleteEffect.size();
		for (size_t i = 0; i < iDeleteSize; ++i)
		{
			pkPlayer->DeleteEffect(kDeleteEffect.at(i));
		}
	}

	// PvP 속성이 있으면 각성상태를 초기화 시킨다.
	if(GATTR_FLAG_PVP & m_pkGround->GetAttr())
	{
		pkPlayer->SetAbil(AT_AWAKE_STATE, EAS_NORMAL_GUAGE);
		pkPlayer->SetAbil(AT_AWAKE_VALUE, 0);
	}

	// Passive skill setting
	RefreshPassiveSkill( pkPlayer, m_pkGround, NULL );
	
	int const iNowHP = pkPlayer->GetAbil(AT_HP);
	if (iNowHP == HP_VALUE_MISSION_MAP_PENALTY)
	{
		// MissionMap 에서 죽은 것이다. (MissionMap Penalty 적용)
		PgThrowUpPenalty kAction(m_pkGround->GroundKey(), true);
		kAction.DoAction(pkPlayer, -5000);
	}
	else if (iNowHP <= 0 )
	{// 죽어있으면 살려야 한다.
		pkPlayer->Alive( EALIVE_NONE, E_SENDTYPE_BROADALL );

		if( pkPlayer->HaveExpedition() )	// 원정대에 가입되어 있다면, 자신의 상태 변화를 알려야 함.
		{
			BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
			Packet.Push(pkPlayer->ExpeditionGuid());
			Packet.Push(pkPlayer->GetID());
			Packet.Push(pkPlayer->IsAlive());
			::SendToGlobalPartyMgr(Packet);
		}

		PgDeathPenalty kAction(m_pkGround->GroundKey(), LURT_MapMove, m_pkGround->IsDeathPenalty() );//이렇게 살아나면 그냥 패널티를 준다.
		kAction.DoAction(pkPlayer, NULL);// 데스 패널티를 준다.
	}

	pkPlayer->SetAbil(AT_IGNORE_MACRO_COUNT, 0);	//매크로방지 무시 카운트를 초기화 한다
	return true;
}
