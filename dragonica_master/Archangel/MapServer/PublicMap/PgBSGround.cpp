#include "stdafx.h"
#include "PgGround.h"
#include "PgBSGround.h"
#include "PgAction.h"
#include "Lohengrin/ActArg.h"
#include "PgPVPEffectSelector.h"
#include "PgActionAchievement.h"

//#define REQ_DEBUG_BS 1
#ifdef REQ_DEBUG_BS
#define DEBUG_BS(a, b)	INFO_LOG(a, b)
#else
#define DEBUG_BS(a, b)	((void)0)
#endif

namespace PgGroundUtil
{
	bool IsHaveFreePVPAttr(EDynamicGroundAttr const eAttr)
	{
		return 0 != (eAttr & DGATTR_FLAG_FREEPVP);
	}
	bool IsBSGround(T_GNDATTR const eGndAttr)
	{
		return GATTR_BATTLESQUARE == eGndAttr;
	}
	void NfyBSStatus(PgBSMapGame& rkBSGame, EBS_GAME_STATUS const eStatus)
	{
		BM::Stream kPacket(PT_A_N_NFY_GAME_STATUS);
		kPacket.Push( rkBSGame.GameIDX() );
		kPacket.Push( eStatus );
		::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
	}
}


//
PgBSGround::PgBSGround()
: m_dwCalculatePointTick(0), m_dwSyncUnitPosTick(0), m_dwSyncScoreTick(0), m_eStatus(BSGS_NONE)
{
}
PgBSGround::~PgBSGround()
{
}

void PgBSGround::Clear()
{
	PgGround::Clear();
	m_kBSGame.Clear();
}

bool PgBSGround::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if( pkUnit
	&&	pkUnit->IsInUnitType(UT_PLAYER) )
	{
		LeaveBSUser(pkUnit);
		g_kPVPEffectSlector.DelEffect(pkUnit);
	}
	return PgGround::ReleaseUnit(pkUnit, bRecursiveCall, bSendArea);
}

void PgBSGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	{
		PgGround::OnTick1s();
	}

	if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		bool bSyncScore = false, bSyncScoreAll = false;
		ContGuidSet kContChangedUnit;
		if( BM::TimeCheck(m_dwSyncScoreTick, SYNC_SCORE_INTERVAL) )
		{
			DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" TICK:" << SYNC_SCORE_INTERVAL);
			bSyncScoreAll = true;
		}
		if (BM::TimeCheck(m_dwCalculatePointTick, CALCULATE_POINT_INTERVAL) )
		{
			DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" TICK:" << CALCULATE_POINT_INTERVAL);
			UpdateBSPoint(kContChangedUnit);
			bSyncScore = (0 < kContChangedUnit.size());
		}

		if( bSyncScore
		||	bSyncScoreAll )
		{
			BM::Stream kScorePacket(PT_M_C_NFY_BS_SCORE);
			size_t const iOldSize = kScorePacket.Size();
			m_kBSGame.WriteScore(kContChangedUnit, kScorePacket, bSyncScoreAll);
			if( kScorePacket.Size() != iOldSize )
			{
				Broadcast(kScorePacket);
			}
		}
	}
}
void PgBSGround::SendMapLoadComplete( PgPlayer *pkUser )
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUser? pkUser->Name(): L"NULL") << L" G:" << (pkUser? pkUser->GetID(): BM::GUID::NullData()));
	PgGround::SendMapLoadComplete( pkUser );

	m_kBSGame.AddObserverWaiter(pkUser->GetID());
}

void PgBSGround::LeaveBSUser(CUnit* pkUnit)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if( !pkUnit )
	{
		return;
	}

	{
		BM::Stream kPacket(PT_A_N_NFY_WANT_JOIN_BS_CHANNEL);
		kPacket.Push( pkUnit->GetID() );
		kPacket.Push( BSJR_NONE );
		::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
	}

	DropAllVolatileInven(pkUnit); // 들고 있는거 다 떨궈!

	//SetOBMode_End(pkUnit->GetID()); // 옵저버 모드 종료

	m_kBSGame.Leave(pkUnit);

	pkUnit->SetAbil(AT_HIDE, 0);
	pkUnit->AddCountAbil( AT_CANNOT_ATTACK, AT_CF_BS, false, E_SENDTYPE_NONE );
	pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_BS, false, E_SENDTYPE_NONE );

	if( false == ProcessBSWaiter() )
	{
		BM::Stream kPacket(PT_M_C_NFY_WANT_JOIN_BS);
		m_kBSGame.SyncPlayerList(kPacket);
		Broadcast(kPacket);
	}

	ContGuidSet kContHasObserver;
	UnRegistHasObserver(pkUnit, NULL, &kContHasObserver); // 옵저버들 재설정
	if( false == kContHasObserver.empty() )
	{
		ContGuidSet::const_iterator guid_iter = kContHasObserver.begin();
		while( kContHasObserver.end() != guid_iter )
		{
			m_kBSGame.AddObserverWaiter((*guid_iter));
			++guid_iter;
		}
	}
}
bool PgBSGround::IsBSItem(int const iItemNo) const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
	{
		return m_kBSGame.IsBSItem(iItemNo);
	}
	return false;
}
bool PgBSGround::PushBSInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()) << L" I:" << rkItem.ItemNo());
	if( pkUnit )
	{
		if( IsBSItem(rkItem.ItemNo()) )
		{
			CONT_BS_BUFF kAddBuff;
			BM::Stream kBroadPacket(PT_M_C_NFY_BS_SCORE);
			size_t const iPreSize = kBroadPacket.Size();
			size_t const iPosRet = m_kBSGame.PushItem(pkUnit->GetID(), rkItem, kAddBuff, kBroadPacket);
			if( !kAddBuff.empty() )
			{
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, this);

				CONT_BS_BUFF::const_iterator iter = kAddBuff.begin();
				while( kAddBuff.end() != iter )
				{
					pkUnit->AddEffect( (*iter), 0, &kArg, pkUnit );
					++iter;
				}
			}
			if( iPreSize != kBroadPacket.Size() )
			{
				Broadcast(kBroadPacket);
			}

			return true;
		}
	}
	return false;
}
void PgBSGround::DeSetBSTeamUnitStatus(CUnit* pkUnit)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if( !pkUnit )
	{
		return;
	}
	if( pkUnit->IsUnitType(UT_PLAYER) )
	{
		if( 0 == pkUnit->GetCountAbil(AT_CANNOT_DAMAGE, AT_CF_BS) )
		{
			pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_BS, true, E_SENDTYPE_BROADALL_EFFECTABIL );// 타겟팅 안 되도록 설정
		}
		if( 0 == pkUnit->GetCountAbil(AT_CANNOT_ATTACK, AT_CF_BS) )
		{
			pkUnit->AddCountAbil( AT_CANNOT_ATTACK, AT_CF_BS, true, E_SENDTYPE_BROADALL_EFFECTABIL );// 공격 못 하도록 설정
		}
		if( 0 == pkUnit->GetAbil(AT_HIDE) )
		{
			pkUnit->SetAbil(AT_HIDE, 1, true, true);
		}
		pkUnit->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP));
		pkUnit->SetAbil(AT_MP, pkUnit->GetAbil(AT_C_MAX_MP));
		WORD kAbil[2] = {AT_HP, AT_MP};
		pkUnit->SendAbiles(kAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);
	}
}
void PgBSGround::SetBSTeamUnitStatus(CUnit* pkUnit)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if( !pkUnit )
	{
		return;
	}
	if( pkUnit->IsUnitType(UT_PLAYER) )
	{
		if( 0 != pkUnit->GetCountAbil(AT_CANNOT_DAMAGE, AT_CF_BS) )
		{
			pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_BS, false, E_SENDTYPE_BROADALL_EFFECTABIL );// 타겟팅 되도록 설정
		}
		if( 0 != pkUnit->GetCountAbil(AT_CANNOT_ATTACK, AT_CF_BS) )
		{
			pkUnit->AddCountAbil( AT_CANNOT_ATTACK, AT_CF_BS, false, E_SENDTYPE_BROADALL_EFFECTABIL );// 공격 하도록 설정
		}
		if( 0 != pkUnit->GetAbil(AT_HIDE) )
		{
			pkUnit->SetAbil(AT_HIDE, 0, true, true);
		}
		bool const bIsHaveFreePVP = PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr());
		if( bIsHaveFreePVP )
		{
			int const iTeam = m_kBSGame.GetTeam(pkUnit->GetID());
			if( 0 != iTeam )
			{
				SendToSpawnLoc(pkUnit, 0, true, iTeam );
				pkUnit->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP));
				pkUnit->SetAbil(AT_MP, pkUnit->GetAbil(AT_C_MAX_MP));
				WORD kAbil[2] = {AT_HP, AT_MP};
				pkUnit->SendAbiles(kAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);
			}
		}
	}
}
bool PgBSGround::ProcessBSWaiter()
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo());
	ContGuidSet kContGuid;
	if( m_kBSGame.ProcessWaiter(kContGuid) )
	{
		{ // 동기화 먼저 하고
			BM::Stream kPlayerListPacket(PT_M_C_NFY_WANT_JOIN_BS);
			m_kBSGame.SyncPlayerList(kPlayerListPacket);
			Broadcast(kPlayerListPacket);
		}

		// 팀 어빌 셋팅
		BM::Stream kAnsJoinPacket(PT_M_C_ANS_WANT_JOIN_BS);
		kAnsJoinPacket.Push( BSJR_SUCCESS );

		bool const bHaveFreePVP = PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr());
		ContGuidSet::const_iterator iter = kContGuid.begin();
		while( kContGuid.end() != iter )
		{
			//SetOBMode_End((*iter));

			CUnit* pkUnit = GetUnit(*iter);
			if( pkUnit )
			{
				SetBSTeamUnitStatus(pkUnit);
				m_kBSGame.SetTeam(pkUnit, bHaveFreePVP);
				pkUnit->Send(kAnsJoinPacket);
			}
			++iter;
		}
		return true;
	}
	return false;
}

bool PgBSGround::VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy )
{
	switch( wType )
	{
	case PT_C_M_REQ_WANT_JOIN_BS:
		{
			DEBUG_BS(BM::LOG_LV1, __F_P__(PT_C_M_REQ_WANT_JOIN_BS) << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
			EBattleSquareTeam eTeam = BST_NONE;
			pkNfy->Pop( eTeam );

			if( BST_NONE != eTeam )
			{
				bool const bIsGame = PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr());
				EBS_JOIN_RETURN const eJoinRet = m_kBSGame.JoinTeam(pkUnit, eTeam);
				switch( eJoinRet )
				{
				case BSJR_SUCCESS:
				case BSJR_BALANCE:
					{
						if( false == ProcessBSWaiter() )
						{
							BM::Stream kPacket(PT_M_C_NFY_WANT_JOIN_BS);
							m_kBSGame.SyncPlayerList(kPacket);
							Broadcast(kPacket);
						}
					}break;
				default:
					{
					}break;
				}
				if( BSJR_SUCCESS == eJoinRet )
				{
					// 팀 정보 싱크 한뒤
					SetBSTeamUnitStatus(pkUnit);
					//SetOBMode_End(pkUnit->GetID());
					m_kBSGame.SetTeam(pkUnit, bIsGame);
					pkUnit->SetAbil(AT_HP, pkUnit->GetAbil(AT_C_MAX_HP));
					pkUnit->SetAbil(AT_MP, pkUnit->GetAbil(AT_C_MAX_MP));
					WORD kAbil[2] = {AT_HP, AT_MP};
					pkUnit->SendAbiles(kAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);

					SendToSpawnLoc(pkUnit, 0, true, eTeam);
				}

				SendBSPoint(pkUnit);
				// 항상 팀정보가 먼저 싱크 된다
				BM::Stream kPacket(PT_M_C_ANS_WANT_JOIN_BS);
				kPacket.Push( eJoinRet );
				pkUnit->Send( kPacket );
			}
		}break;
	case PT_C_M_REQ_BS_EXIT:
		{
			DEBUG_BS(BM::LOG_LV1, __F_P__(PT_C_M_REQ_BS_EXIT) << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
			if( pkPlayer )
			{
				LeaveBSUser(pkUnit);

				BM::Stream kPacket(PT_M_C_ANS_BS_EXIT);
				
				int const iHP = pkPlayer->GetAbil(AT_MEMORIZED_HP);
				int const iMP = pkPlayer->GetAbil(AT_MEMORIZED_MP);
				if(0 < iHP 
					&& 0 < iMP 
					)
				{
					pkPlayer->SetAbil(AT_HP		, iHP);
					pkPlayer->SetAbil(AT_MP		, iMP);
				}

				pkPlayer->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
				
				RecvRecentMapMove(pkPlayer);
			}
		}break;
	case PT_U_G_GM_ADDSCORE:
		{
			DEBUG_BS(BM::LOG_LV1, __F_P__(PT_U_G_GM_ADDSCORE) << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
			if( pkPlayer
			&&	0 < pkPlayer->GMLevel() )
			{
				int iPoint = 0;
				int iTeamPoint = 0;

				pkNfy->Pop( iPoint );
				pkNfy->Pop( iTeamPoint );

				int const iTeamNo = m_kBSGame.GetTeam(pkUnit->GetID());
				if( BST_NONE != iTeamNo )
				{
					BM::Stream kBroadPacket(PT_M_C_NFY_BS_SCORE);
					size_t const iPreSize = kBroadPacket.Size();
					m_kBSGame.AddScore(pkUnit->GetID(), iPoint, iTeamPoint, kBroadPacket);
					if( iPreSize != kBroadPacket.Size() )
					{
						Broadcast(kBroadPacket);
					}
				}
			}
		}break;
	case PT_U_G_NFY_ATTACKED_MONSTER:
		{
			DEBUG_BS(BM::LOG_LV1, __F_P__(PT_U_G_NFY_ATTACKED_MONSTER) << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
			BM::GUID kCaller;
			pkNfy->Pop(kCaller);
			CUnit* pkCaller = (kCaller != pkUnit->GetID()) ? GetUnit(kCaller) : pkUnit;
			if (pkCaller != NULL)
			{
				int const iPointPerHit = 1;
				BM::Stream kBroadPacket(PT_M_C_NFY_BS_SCORE);
				size_t const iPreSize = kBroadPacket.Size();
				m_kBSGame.AddScore(pkCaller->GetID(), iPointPerHit, iPointPerHit, kBroadPacket);
				if( iPreSize != kBroadPacket.Size() )
				{
					// 패킷이 엄청나게 불어나기 때문에 브로드 캐스팅을 하지 않는다(늘어난 만큼의 개인 점수는 틱마다 동기화 한다)
					if( pkCaller->IsInUnitType(UT_PLAYER) )
					{
						pkCaller->Send(kBroadPacket); // 개인에게만 전송
					}
				}
			}
		}break;
	default:
		{
			return PgGround::VUpdate( pkUnit, wType, pkNfy );
		}break;
	}
	return true;
}
bool PgBSGround::RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" From:" << rkRMM.kCasterKey.GroundNo() << L" T:" << rkRMM.cType );

	if (m_eStatus < BSGS_NOW_PREOPEN || m_eStatus > BSGS_NOW_GAME)
	{
		SAnsMapMove_MT kAMM((EMapMoveEventType)rkRMM.cType);
		kAMM.kSI = g_kProcessCfg.ServerIdentity();
		kAMM.kGroundKey = rkRMM.kCasterKey;
		kAMM.kAttr = this->GetAttr();
		kAMM.iPortalID = rkRMM.nTargetPortal;
		kAMM.eRet = MMR_FAILED;
		ProcessMapMoveResult( rkUnitArray, kAMM );
		return false;
	}

	if ( !PgGround::RecvMapMove( rkUnitArray, rkRMM, kContPetMapMoveData, kContUnitSummonedMapMoveData, kModifyOrder ) )
	{
		return false;
	}

	VEC_GUID kWaitList;
	VEC_JOININDUN kJoinIndunList;
	UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
	while( rkUnitArray.end() != unit_itr )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if( pkUser )
		{
			DeSetBSTeamUnitStatus(pkUser);
		}
		++unit_itr;
	}
	return true;
}

// RecvFromContents
bool PgBSGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch( usType )
	{
	case PT_A_A_NFY_BS_GAME_INFO: // from Center
		{
			EBS_GAME_STATUS eStatus = BSGS_NONE;
			pkPacket->Pop( eStatus );
			m_eStatus = eStatus;

			DEBUG_BS(BM::LOG_LV1, __F_P__(PT_C_M_REQ_BS_EXIT) << L" IDX:" << m_kBSGame.GameIDX() << _T(" NewState:") << static_cast<int>(eStatus));
			switch( eStatus )
			{
			case BSGS_NOW_PREOPEN:
				{
					PgBSMapGameImpl const kNewGame(*pkPacket);
					m_kBSGame.Set( kNewGame );

					PgGroundUtil::NfyBSStatus(m_kBSGame, eStatus); // 사전 입장
				}break;
			case BSGS_NOW_GAME:
				{
					// Update
					PgBSMapGameImpl const kNewGame(*pkPacket);
					m_kBSGame.Set( kNewGame );

					if( !PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
					{
						PgGroundUtil::NfyBSStatus(m_kBSGame, eStatus); // 게임 중
						// 게임시작
						EDynamicGroundAttr const eOldAttr = DynamicGndAttr();
						DynamicGndAttr( static_cast<EDynamicGroundAttr>(DynamicGndAttr() | DGATTR_FLAG_FREEPVP) );
						if ( (DynamicGndAttr()&DGATTR_FLAG_FREEPVP) != (eOldAttr&DGATTR_FLAG_FREEPVP) )
						{
							DoStart();
						}
					}
				}break;
			case BSGS_NOW_END:
				{
					if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
					{
						DoEnd(eStatus);
						GiveReward();
					}
				}break;
			case BSGS_NONE:
				{
					PgGroundUtil::NfyBSStatus(m_kBSGame, BSGS_NONE);
					m_kBSGame.Clear();

					{ // 모든 플레이어 원래 마을로 돌려보내!
						std::list< PgPlayer * >	kOutPlayerList;
						CUnit* pkUnit = NULL;
						CONT_OBJECT_MGR_UNIT::iterator kItor;
						PgObjectMgr::GetFirstUnit(UT_PLAYER,kItor);
						while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER,kItor)) != NULL)
						{
							PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
							if( pkPlayer )
							{
								kOutPlayerList.push_back( pkPlayer );
							}
						}

						std::list< PgPlayer * >::iterator player_itr = kOutPlayerList.begin();
						for ( ; player_itr != kOutPlayerList.end() ; ++player_itr )
						{
							PgPlayer* pkPlayer = (*player_itr);
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
							this->RecvRecentMapMove( *player_itr );
						}
					}
				}break;
			};
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}break;
	}
	return true;
}
void PgBSGround::RecvUnitDie(CUnit *pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if( !pkUnit )
	{
		return;
	}

	BM::GUID const kCasterGuid = pkUnit->GetTarget();
	CUnit* pkCaster = GetUnit(kCasterGuid);
	BM::GUID const kKillerGuid( ((pkCaster)? GlobalHelper::FindOwnerGuid(pkCaster): kCasterGuid) );

	{
		PgGround::RecvUnitDie(pkUnit);
	}

	switch( pkUnit->UnitType() )
	{
	case UT_PLAYER:
		{	
			if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
			{
				BM::GUID kKillerGuid;
				DropAllVolatileInven(pkUnit, &kKillerGuid);

				CUnit* pkKiller = GetUnit(kKillerGuid);

				BM::Stream kNfyPacket(PT_M_C_NFY_BS_DEAD);
				kNfyPacket.Push( pkUnit->GetID() );
				kNfyPacket.Push( pkUnit->Name() );
				kNfyPacket.Push( (pkKiller)? pkKiller->GetID(): BM::GUID::NullData() );
				kNfyPacket.Push( (pkKiller)? pkKiller->Name(): std::wstring() );
				pkUnit->Send( kNfyPacket, E_SENDTYPE_BROADALL );
			}
		}break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
		}break;
	}

	CUnit* pkKiller = GetUnit(kKillerGuid);
	switch( pkUnit->UnitType() )
	{
	case UT_PLAYER:
		{
			m_kBSGame.AddDeadMan(pkUnit->GetID()); // 부활 대기 등록

			if( pkKiller
			&&	UT_PLAYER == pkKiller->UnitType() )
			{
				BM::Stream kBroadPacket(PT_M_C_NFY_BS_SCORE);
				size_t const iPreSize = kBroadPacket.Size();
				// 점수 누적
				if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
				{
					m_kBSGame.NfyPlayerKill(pkKiller, pkUnit, kBroadPacket);
					if( iPreSize != kBroadPacket.Size() )
					{
						Broadcast(kBroadPacket);
					}

					// 킬 EXP 지급
					__int64 const iLevelPerExp = m_kBSGame.CalcPvpExp(pkKiller->GetAbil(AT_LEVEL), pkUnit->GetAbil(AT_LEVEL));
					if( iLevelPerExp > 0 )
					{
						PgAction_AddExp kAddExp(GroundKey(), iLevelPerExp, AEC_KillMonster, this);
						kAddExp.DoAction(pkKiller, NULL);
					}

					// 죽인 넘은 죽은 카운트 초기화, 죽은 넘은 죽은 카운트 증가
					pkKiller->SetAbil(AT_SUCCESSIONAL_DEATH_COUNT, 0);
					pkUnit->SetAbil(AT_SUCCESSIONAL_DEATH_COUNT, std::min(8, pkUnit->GetAbil(AT_SUCCESSIONAL_DEATH_COUNT)+1));
					{
						BM::Stream kDeathPacket(PT_M_C_NFY_BS_DEATHCOUNT, pkUnit->GetAbil(AT_SUCCESSIONAL_DEATH_COUNT));
						pkUnit->Send(kDeathPacket);
					}
				}
			}
		}break;
	default:
		{
		}break;
	}
}
void PgBSGround::DropAllVolatileInven(CUnit* pkUnit, BM::GUID* pkKillerGuid)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()) << L" ReqKiller:" << (pkKillerGuid? L"YES": L"NO"));
	if( !pkUnit )
	{
		return;
	}

	if( UT_PLAYER != pkUnit->UnitType() )
	{
		return;
	}

	VEC_GUID kOwnerVec;

	CUnit* pkKiller = GetUnit(GlobalHelper::FindOwnerGuid(GetUnit(pkUnit->GetTarget())));
	if( pkKiller )
	{
		if( BM::GUID::IsNotNull(pkKiller->GetPartyGuid()) )
		{
			//m_kLocalPartyMgr.GetPartyMemberGround(pkKiller->GetPartyGuid(), GroundKey(), kOwnerVec);
		}
		else
		{
			//std::back_inserter(kOwnerVec) = pkKiller->GetID();
		}
		
		if( pkKillerGuid )
		{
			*pkKillerGuid = pkKiller->GetID();
		}
	}
	//std::back_inserter(kOwnerVec) = pkUnit->GetID(); // 자기 자신도 (죽었는데 바로 살아나면 먹을 수 있다)
	// 아무나 먹도록

	PgCreateSpreadPos kAction(pkUnit->GetPos());
	POINT3BY const kOriented = pkUnit->GetOrientedVector();
	POINT3 Oriented(kOriented.x, kOriented.y, kOriented.z);
	Oriented.Normalize();
	int const iSpreadRange = 30;
	kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iSpreadRange);

	POINT3 kDropPos;

	// 로그를 남겨
	PgLogCont kLogCont(ELogMain_User_Character, ELogSub_BattleSquare_Death );
	//kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
	//kLogCont.CharacterKey( pkUnit->GetID() );
	//kLogCont.GroundNo( GetGroundNo() );			// GroundNo
	//kLogCont.Name( pkUnit->Name() );
	//kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

	CONT_VOLATILE_INVEN_ITEM kVolatileInven;
	CONT_BS_BUFF kDelBuff;
	BM::Stream kWPacket(PT_M_C_NFY_BS_SCORE);
	if( m_kBSGame.SwapInven(pkUnit->GetID(), kVolatileInven, kDelBuff, kWPacket) )
	{
		CONT_VOLATILE_INVEN_ITEM::const_iterator item_iter = kVolatileInven.begin();
		while( kVolatileInven.end() != item_iter )
		{
			CONT_VOLATILE_INVEN_ITEM::mapped_type const& rkItem = (*item_iter).second;
			if( !rkItem.IsEmpty() )
			{
				kAction.PopPos( kDropPos );
				InsertItemBox(kDropPos, kOwnerVec, pkUnit, rkItem, 0, kLogCont);
			}
			++item_iter;
		}

		CONT_BS_BUFF::const_iterator buf_iter = kDelBuff.begin();
		while( kDelBuff.end() != buf_iter )
		{
			pkUnit->DeleteEffect((*buf_iter));
			++buf_iter;
		}

		Broadcast(kWPacket);
	}

	kLogCont.Commit();
}
void PgBSGround::WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo());
	m_kBSGame.WriteToMapUserPacket( rkPacket );
}
void PgBSGround::DynamicGndAttr(EDynamicGroundAttr const eNewDGAttr)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" NewDGAttr:" << static_cast< int >(eNewDGAttr));
	BM::CAutoMutex Lock(m_kRscMutex);
	bool const bSetFreePVP = PgGroundUtil::IsHaveFreePVPAttr(eNewDGAttr);
	{
		m_kDynamicGndAttr = eNewDGAttr;
		//
		BM::Stream kPacket(PT_M_C_NFY_GROUND_DYNAMIC_ATTR);
		kPacket.Push( static_cast< int >(m_kDynamicGndAttr) );
		if( bSetFreePVP )
		{
			m_kBSGame.WriteToMapUserPacket( kPacket );
		}
		Broadcast(kPacket, NULL, E_SENDTYPE_MUSTSEND);
	}
}

/*
void PgBSGround::SendPointToUser(CONT_BS_MEMBER_POINT const& kContMemberPoint, CONT_BS_HAVE_MINIMAP_ICON const& kIconInfo) const
{
	CONT_BS_MEMBER_POINT::const_iterator itor_mem = kContMemberPoint.begin();
	while (itor_mem != kContMemberPoint.end())
	{
		CUnit* pkUnit = GetUnit((*itor_mem).first);
		if (pkUnit != NULL)
		{
			CONT_BS_HAVE_MINIMAP_ICON::const_iterator itor_icon = kIconInfo.find((*itor_mem).first);

			BM::Stream kPPacket(PT_M_C_NFY_MY_POINT);
			kPPacket.Push((*itor_mem).second);
			kPPacket.Push((itor_icon != kIconInfo.end()) ? (*itor_icon).second.iCount, 0);
			pkUnit->Send(kPPacket);
		}
		++itor_mem
	}
}
*/

/*
void PgBSGround::SendTeamPoint() const
{
	BM::Stream kQPacket(PT_M_C_NFY_TEAM_POINT);
	int iPoint = 0;
	int iIcon = 0;
	m_kBSGame.GetTeamPoint(BST_BLUE, iPoint, iIcon);
	kQPacket.Push(iPoint);
	kQPacket.Push(iIcon);
	m_kBSGame.GetTeamPoint(BST_RED, iPoint, iIcon);
	kQPacket.Push(iPoint);
	kQPacket.Push(iIcon);
	Broadcast(kQPacket);	
}
*/

void PgBSGround::UpdateBSPoint(ContGuidSet& rkOutGuid)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo());
	m_kBSGame.UpdateBSPoint(rkOutGuid);
	m_kBSGame.UpdateTeamIconCount();

}

void PgBSGround::SendBSPoint(CUnit* pkUnit) const
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkUnit? pkUnit->Name(): L"NULL") << L" G:" << (pkUnit? pkUnit->GetID(): BM::GUID::NullData()));
	if (pkUnit == NULL)
	{
		return;
	}

	BM::Stream kRPacket(PT_M_C_NFY_BS_SCORE);
	m_kBSGame.WriteScore(pkUnit->GetID(), kRPacket);
	pkUnit->Send(kRPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);
}

void PgBSGround::CallAlramReward( PgPlayer * pkPlayer )
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkPlayer? pkPlayer->Name(): L"NULL") << L" G:" << (pkPlayer? pkPlayer->GetID(): BM::GUID::NullData()));
	BM::CAutoMutex kLock(m_kRscMutex);
	PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();

	TBL_DEF_ALRAM_MISSION const * const pkDef = rkAlramMission.GetDef();
	if ( pkDef )
	{
		if (pkDef->nPoint != 0)
		{
			if( PgGroundUtil::IsHaveFreePVPAttr(DynamicGndAttr()) )
			{
				BM::Stream kScorePacket(PT_M_C_NFY_BS_SCORE);
				m_kBSGame.AddScore(pkPlayer->GetID(), pkDef->nPoint, pkDef->nPoint, kScorePacket);
				Broadcast(kScorePacket, BM::GUID::NullData(), E_SENDTYPE_SEND_BYFORCE);
			}
		}
	}
	PgGround::CallAlramReward(pkPlayer);
}
void PgBSGround::OnTick_AlramMission( PgPlayer *pkPlayer, DWORD const dwCurTime )
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" N:" << (pkPlayer? pkPlayer->Name(): L"NULL") << L" G:" << (pkPlayer? pkPlayer->GetID(): BM::GUID::NullData()));
	if( 0 != m_kBSGame.GetTeam(pkPlayer->GetID()) )
	{
		PgGround::OnTick_AlramMission(pkPlayer, dwCurTime);
	}
}

void PgBSGround::OnTick100ms()
{
	{
		PgGround::OnTick100ms();
	}

	{
		BM::CAutoMutex kLock(m_kRscMutex);
		DWORD const dwOldTick = m_dwSyncUnitPosTick;
		if (BM::TimeCheck(m_dwSyncUnitPosTick, SYNC_UNIT_POS_MINIMAP_INTERVAL) )
		{
			DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" 2");
			DWORD const dwNow = BM::GetTime32();
			DWORD const dwElapsedTime = dwNow - dwOldTick;
			SyncUnitPos(dwElapsedTime);
		}
	}
}

void PgBSGround::SyncUnitPos(DWORD const dwElapsedTime)
{
	// BattleSquare item pos sync
	ContGuidSet kContRevive;
	CONT_BS_HAVE_MINIMAP_ICON kPosInfo;
	if( true == m_kBSGame.Tick1s(dwElapsedTime, kPosInfo, kContRevive) )
	{
		DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" EleapsedTime:" << dwElapsedTime << L" 2");
		{
			// Revive Dead_Player
			ContGuidSet::const_iterator iter = kContRevive.begin();
			while( kContRevive.end() != iter )
			{
				ContGuidSet::value_type const& rkGuid = (*iter);
				CUnit* pkUnit = GetUnit(rkGuid);
				if( pkUnit )
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
					if( pkPlayer )
					{
						int const iTeam = m_kBSGame.GetTeam(rkGuid);
						pkPlayer->Alive(EALIVE_PVP, E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
						PgUnitEffectMgr const& rkEffectMgr = pkPlayer->GetEffectMgr();
						if (rkEffectMgr.MustAllClear())
						{
							// 지워져야 한다고 표시되어 있으면...
							pkPlayer->ClearAllEffect(true);
						}
						PgDeathPenalty kAction(GroundKey(), LURT_BattleSquare); // 부활시 무 패널티(무적 이펙트)
						kAction.DoAction(pkPlayer, NULL);

						SendToSpawnLoc(pkPlayer, 0, true, iTeam ); // 무조건 살려
						pkPlayer->SetAbil(AT_HP, pkPlayer->GetAbil(AT_C_MAX_HP));
						pkPlayer->SetAbil(AT_MP, pkPlayer->GetAbil(AT_C_MAX_MP));
						WORD kAbil[2] = {AT_HP, AT_MP};
						pkPlayer->SendAbiles(kAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);
					}
				}
				++iter;
			}
		}
		{
			// Send Player Position
			BM::Stream kPacket(PT_M_C_NFY_BS_UNIT_POS);
			size_t const iWrPos = kPacket.WrPos();
			size_t iCount = 0;
			kPacket.Push( kPosInfo.size() );
			CONT_BS_HAVE_MINIMAP_ICON::const_iterator itor_icon = kPosInfo.begin();
			while (itor_icon != kPosInfo.end())
			{
				CUnit* pkUnit = GetUnit((*itor_icon).first);
				if (pkUnit != NULL)
				{
					kPacket.Push(pkUnit->GetID());
					kPacket.Push(pkUnit->GetPos());
					kPacket.Push(static_cast<BYTE>((*itor_icon).second.iTeam));
					//kPacket.Push((*itor_icon).second.iIconNo);
					kPacket.Push(static_cast<short>((*itor_icon).second.iCount));
					++iCount;
				}
				++itor_icon;
			}
			if (kPosInfo.size() != iCount)
			{
				kPacket.ModifyData(iWrPos, &iCount, sizeof(iCount));
			}
			Broadcast(kPacket);
		}

		// 옵져버 대기 중인 사람들 옵져버 화
		PgBSObserverHelper::CONT_BS_OBSERVER_RESULT kContObserverResult;
		if( m_kBSGame.ProcessObserverWaiter(kContObserverResult) )
		{
			PgBSObserverHelper::CONT_BS_OBSERVER_RESULT::const_iterator iter = kContObserverResult.begin();
			while( kContObserverResult.end() != iter )
			{
				bool bFailed = true;
				PgPlayer* pkObserver = dynamic_cast< PgPlayer* >(GetUnit((*iter).first));
				PgPlayer* pkTarget = dynamic_cast< PgPlayer* >(GetUnit((*iter).second));
				if( pkObserver
				&&	pkTarget )
				{
					//if( S_OK == SetOBMode_Start(pkObserver, pkTarget) )
					//{
						bFailed = false;
					//}
				}

				if( bFailed )
				{
					// 옵저버 등록 실패시 대기 목록에 재 등록
					m_kBSGame.AddObserverWaiter((*iter).first);
				}
				++iter;
			}
		}

		{
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			BM::Stream kNfyPacket(PT_M_C_NFY_BS_MONSTER_POS);
			size_t const iWrPos = kNfyPacket.WrPos();
			size_t iCurCount = 0;
			kNfyPacket.Push( iCurCount );
			PgMonster const* pkMonster = NULL;
			PgObjectMgr::GetFirstUnit(UT_MONSTER, kItor);
			while ((pkMonster = dynamic_cast<PgMonster*> (PgObjectMgr::GetNextUnit(UT_MONSTER, kItor))) != NULL)
			{
				kNfyPacket.Push( pkMonster->GetID() );
				kNfyPacket.Push( pkMonster->GetPos() );
				++iCurCount;
			}
			//if( 0 != iCurCount )
			//{
				kNfyPacket.ModifyData(iWrPos, &iCurCount, sizeof(iCurCount));
				Broadcast(kNfyPacket);
			//}
		}
		//SendPointToUser(kContMemberPoint, kPosInfo);
		//SendTeamPoint();
	}
}

void PgBSGround::DoStart()
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo());
	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator unit_itr;
	GetFirstUnit(UT_PLAYER, unit_itr);
	while((pkUnit = GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
	{
		if( pkUnit )
		{
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
			if( pkPlayer )
			{
				// 시작시
					// 팀 없으면 공격 못하고, 내 유닛 보이지 않는 것 유지
					// 팀 있으면 공격 시작 하고 팀 위치로 리젠
				// 종료시
					// 옵져버 해제
					// 공격 못하는 어빌 해제
					// 팀 해제
				int const iTeam = m_kBSGame.GetTeam(pkUnit->GetID());
				if( 0 != iTeam )
				{
					m_kBSGame.SetTeam(pkUnit, true);
					if( pkPlayer->IsDead() )
					{
						pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
					}
					pkPlayer->SetAbil(AT_SUCCESSIONAL_DEATH_COUNT, 0);
					pkPlayer->SetAbil(AT_HP, pkPlayer->GetAbil(AT_C_MAX_HP));
					pkPlayer->SetAbil(AT_MP, pkPlayer->GetAbil(AT_C_MAX_MP));
					WORD kAbil[2] = {AT_HP, AT_MP};
					pkPlayer->SendAbiles(kAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);

					PgDeathPenalty kAction(GroundKey(), LURT_BattleSquare); // 부활시 무 패널티(무적 이펙트)
					kAction.DoAction(pkPlayer, NULL);

					SendToSpawnLoc(pkUnit, 0, true, iTeam );
				}
			}
		}
	}

	m_kAIMng.ReloadWaypoint(*this);
	ActivateMonsterGenGroup( 0, true, false, -1 );
	{
		//CONT_DEF_MAP_REGEN_POINT const* pkDefRegenPoint = NULL;
		//g_kTblDataMgr.GetContDef(pkDefRegenPoint);
		CONT_DEF_MONSTER_BAG_CONTROL const* pkDefMonBagControl = NULL;
		CONT_DEF_MONSTER_BAG const* pkDefMonBag = NULL;
		CONT_DEF_SUCCESS_RATE_CONTROL const* pkDefSuccessRate = NULL;
		CONT_DEF_MONSTER_BAG_ELEMENTS const* pkDefMonBagElements = NULL;
		g_kTblDataMgr.GetContDef(pkDefMonBagControl);
		g_kTblDataMgr.GetContDef(pkDefMonBag);
		g_kTblDataMgr.GetContDef(pkDefSuccessRate);
		g_kTblDataMgr.GetContDef(pkDefMonBagElements);

		ContGenPoint_Monster::iterator iter = m_kContGenPoint_Monster.begin();
		while( m_kContGenPoint_Monster.end() != iter )
		{
			TBL_DEF_MAP_REGEN_POINT kRegenPointInfo = (*iter).second.Info();
			kRegenPointInfo.iBagControlNo = m_kBSGame.GetBagControlNo();
			CONT_DEF_MONSTER_BAG_CONTROL::const_iterator monbagctrl_iter = pkDefMonBagControl->find(kRegenPointInfo.iBagControlNo);
			if( pkDefMonBagControl->end() != monbagctrl_iter )
			{
				for( int iCur = 0; MAX_MONSTERBAG_ELEMENT > iCur; ++iCur )
				{
					int const iMonBagNo = (*monbagctrl_iter).second.aBagElement[iCur];
					if( iMonBagNo )
					{
						CONT_DEF_MONSTER_BAG::const_iterator monbag_iter = pkDefMonBag->find(iMonBagNo);
						if( pkDefMonBag->end() != monbag_iter )
						{
							CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator rate_iter = pkDefSuccessRate->find((*monbag_iter).second.iRateNo);
							CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator ele_iter = pkDefMonBagElements->find((*monbag_iter).second.iElementNo);
							if( pkDefSuccessRate->end() != rate_iter
							&&	pkDefMonBagElements->end() != ele_iter )
							{
								ContGenPoint_Monster::mapped_type kNewGenPoint;
								kNewGenPoint.Build(kRegenPointInfo, (*ele_iter).second, (*rate_iter).second);
								(*iter).second = kNewGenPoint;
							}
						}
					}
				}
			}
			++iter;
		}
	}
}


void PgBSGround::DoEnd(EBS_GAME_STATUS const eStatus)
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo() << L" EndStatus:" << static_cast< int >(eStatus));
	PgGroundUtil::NfyBSStatus(m_kBSGame, eStatus); // 보상 지급 중

	// 게임 종료
	DynamicGndAttr( static_cast< EDynamicGroundAttr >(DynamicGndAttr() & (DGATTR_FLAG_ALL ^ DGATTR_FLAG_FREEPVP)) );
	// 모든 몬스터 삭제
	RemoveAllMonster(false);
	PgGroundUtil::InitGenPointChild(m_kContGenPoint_Monster);
	PgGroundUtil::InitGenPointChild(m_kContGenPoint_Object);
	{ // 모든 그라운드 아이템 제거
		CUnit* pkBox = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr::GetFirstUnit(UT_GROUNDBOX,kItor);
		UNIT_PTR_ARRAY kDeleteUnit;
		while((pkBox = PgObjectMgr::GetNextUnit(UT_GROUNDBOX,kItor)) != NULL)
		{
			kDeleteUnit.Add(pkBox);
		}

		UNIT_PTR_ARRAY::const_iterator del_itor = kDeleteUnit.begin();
		while (del_itor != kDeleteUnit.end())
		{
			ReleaseUnit(del_itor->pkUnit);
			++del_itor;
		}
	}
}

void PgBSGround::GiveReward()
{
	DEBUG_BS(BM::LOG_LV1, __FL__ << GetGroundNo());
	CONT_BS_USER_REWARD kContWinTeamBonusReward;
	m_kBSGame.MakeWinTeamBonusReward(kContWinTeamBonusReward);

	int const iWinTeamNo = m_kBSGame.GetWinTeam();
	// 죽은 플레이어 모두 부활, 보상처리
	CUnit* pkUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER,kItor);
	while((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER,kItor)) != NULL)
	{
		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
		if( pkPlayer
		&&	pkUnit->IsDead() ) // 죽었으면 살려
		{
			pkPlayer->Alive(EALIVE_MONEY, E_SENDTYPE_SELF| E_SENDTYPE_BROADALL| E_SENDTYPE_MUSTSEND);
		}
		pkPlayer->SetAbil(AT_SUCCESSIONAL_DEATH_COUNT, 0);
		//SetOBMode_End(pkUnit->GetID());

		CONT_PLAYER_MODIFY_ORDER kItemOrder;

		if( pkPlayer )
		{ // 임시 인벤 내용물 제거 / 임시 인벤 버프 제거
			CONT_VOLATILE_INVEN_ITEM kContVITemp;
			CONT_BS_BUFF kContDeleteBuff;
			BM::Stream kTempPacket(PT_M_C_NFY_BS_SCORE);
			m_kBSGame.SwapInven(pkUnit->GetID(), kContVITemp, kContDeleteBuff, kTempPacket);
			{
				CONT_BS_BUFF::const_iterator buff_iter = kContDeleteBuff.begin();
				while( kContDeleteBuff.end() != buff_iter )
				{
					pkPlayer->DeleteEffect((*buff_iter));
					++buff_iter;
				}
			}
		}

		const int iMyTeam = m_kBSGame.GetTeam(pkUnit->GetID());
		if (iMyTeam == iWinTeamNo)
		{
			PgAddAchievementValue kMA( AT_ACHIEVEMENT_BSQ_RESULT_WIN, 1, GroundKey());
			kMA.DoAction( pkPlayer, NULL );
		}

		// 포인트 대비 보상 지급
		CONT_BS_REWARD kContReward;
		if( m_kBSGame.CalcReward(pkUnit->GetID(), kContReward) )
		{
			CONT_BS_REWARD::const_iterator reward_iter = kContReward.begin();
			while( kContReward.end() != reward_iter )
			{
				if( 0 != (*reward_iter).iItemNo
				&&	0 != (*reward_iter).iCount )
				{
					PgBase_Item kItem;
					if( SUCCEEDED(CreateSItem((*reward_iter).iItemNo, (*reward_iter).iCount, GIOT_NONE, kItem)) )
					{
						kItemOrder.push_back(SPMO(IMET_INSERT_FIXED,pkUnit->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(0,0),true)));
					}
				}
				++reward_iter;
			}
		}

		CONT_BS_USER_REWARD::const_iterator bonus_iter = kContWinTeamBonusReward.find(pkUnit->GetID());
		if( kContWinTeamBonusReward.end() != bonus_iter )
		{
			CONT_BS_REWARD::const_iterator item_iter = (*bonus_iter).second.begin();
			while( (*bonus_iter).second.end() != item_iter )
			{
				PgBase_Item kItem;
				if( SUCCEEDED(CreateSItem((*item_iter).iItemNo, (*item_iter).iCount, GIOT_NONE, kItem)) )
				{
					kItemOrder.push_back(SPMO(IMET_INSERT_FIXED,pkUnit->GetID(),SPMOD_Insert_Fixed(kItem,SItemPos(0,0),true)));
				}
				++item_iter;
			}
		}
		if( !kItemOrder.empty() )
		{
			PgAction_ReqModifyItem kItemModifyAction(IMEPT_BATTLESQUARE_REWARD, GroundKey(), kItemOrder);
			kItemModifyAction.DoAction(pkUnit,NULL);
		}

		BM::Stream kPacket(PT_M_C_NFY_BS_REWARD);
		m_kBSGame.SyncPlayerList(kPacket);
		PU::TWriteArray_A(kPacket, kContReward);
		PU::TWriteTable_AA(kPacket, kContWinTeamBonusReward);
		pkUnit->Send( kPacket );
	}


}

void PgBSGround::GetGenGroupKey(SGenGroupKey& rkGenGrpKey)const
{
	rkGenGrpKey.iMapNo = m_kBSGame.GetGenGroupGroundNo();
	rkGenGrpKey.iBalance = GetMapLevel();//이걸 바꾸면 다른 젠 테이블이
}