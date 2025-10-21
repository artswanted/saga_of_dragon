#include "stdafx.h"
#include "Variant/PgItemRarityUpgradeFormula.h"
#include "PgAction.h"
#include "PgSuperGround.h"

namespace SuperGroundUtil
{
	bool ReqEnterSuperGround(CUnit* pkUnit, SGroundKey const& rkGndKey, int const iSuperGroundNo, int const iSuperGroundMode, int const iSpawnNo, int const iFloorNo, SPMO const* pkModifyItem, bool IsElement)
	{
		CONT_DEF_SUPER_GROUND_GROUP const* pkDefSuperGround = NULL;
		g_kTblDataMgr.GetContDef( pkDefSuperGround );
		CONT_DEF_SUPER_GROUND_GROUP::const_iterator find_iter = pkDefSuperGround->find(iSuperGroundNo);
		if( pkDefSuperGround->end() != find_iter )
		{
			ESuperGroundMode const eMode = static_cast< ESuperGroundMode >(iSuperGroundMode);
			if( SGM_NORMAL <= eMode
			&&	SGM_RARE >= eMode )
			{
				BM::Stream kPacket(PT_M_T_REQ_ENTER_SUPER_GROUND);
				rkGndKey.WriteToPacket(kPacket);
				kPacket.Push(pkUnit->GetID());
				kPacket.Push(pkUnit->GetAbil(AT_LEVEL));
				kPacket.Push(iSuperGroundNo);
				kPacket.Push(iSuperGroundMode);
				kPacket.Push((*find_iter).second.kContSuperGround.front().iGroundNo);
				kPacket.Push(iSpawnNo);
				kPacket.Push(iFloorNo);
				kPacket.Push(IsElement);
				kPacket.Push( static_cast< bool >(pkModifyItem != NULL) );
				if( pkModifyItem )
				{
					pkModifyItem->WriteToPacket(kPacket);
				}
				::SendToCenter(kPacket);
				return true;
			}
		}
		return false;
	}
};

//
//
PgSuperGroundTimeEventMgr::SSuperGroundTickFunc::SSuperGroundTickFunc(ContGuidSet& rkContGuid)
	: m_rkContGuid(rkContGuid)
{
}
bool PgSuperGroundTimeEventMgr::SSuperGroundTickFunc::operator ()(CONT_TIME_EVENT::value_type& rkTimeEvent)
{
	switch( rkTimeEvent.eType )
	{
	case SGTET_RECENT_MAP_MOVE:
		{
			m_rkContGuid.insert( rkTimeEvent.kCharGuid );
			return true;
		}break;
	default:
		{
		}break;
	}
	return false;
}
//
PgSuperGroundTimeEventMgr::PgSuperGroundTimeEventMgr()
{
}
PgSuperGroundTimeEventMgr::~PgSuperGroundTimeEventMgr()
{
}


//
PgSuperGround::PgSuperGround()
{
}
PgSuperGround::~PgSuperGround()
{
}

EOpeningState PgSuperGround::Init( int const iMonsterControlID, bool const bMonsterGen)
{
	m_kGndResource.clear();
	m_iNowFloor = 0;
	m_iSuperGroundNo = 0;
	m_eMode = SGM_NORMAL;
	m_kContSuperGndInfo.clear();
	m_dwTimeLimit = 0;
	m_dwAccumLimitTime = 0;
	m_bFailedTime = false;
	m_iLastRemainMonsterCount = 0;
	m_kContNotCountMonster.clear();
	m_kTimeEventMgr.Clear();
	return PgIndun::Init( iMonsterControlID, bMonsterGen );
}

bool PgSuperGround::Clone( PgSuperGround* pkGround )
{
	if( pkGround )
	{
		return PgIndun::Clone(dynamic_cast< PgIndun* >(pkGround));
	}
	return false;
}
bool PgSuperGround::Clone( PgGround* pkGround )
{
	if( pkGround )
	{
		return Clone(dynamic_cast< PgSuperGround* >(pkGround));
	}
	return false;
}

void PgSuperGround::Clear()
{
	m_kGndResource.clear();
	m_iNowFloor = 0;
	m_iSuperGroundNo = 0;
	m_eMode = SGM_NORMAL;
	m_kContSuperGndInfo.clear();
	m_dwTimeLimit = 0;
	m_dwAccumLimitTime = 0;
	m_bFailedTime = false;
	m_iLastRemainMonsterCount = 0;
	m_kContNotCountMonster.clear();
	m_kTimeEventMgr.Clear();
	PgIndun::Clear();
}
T_GNDATTR PgSuperGround::GetAttr()const
{
	if( m_kGndResource.size() > m_iNowFloor )
	{
		PgGroundResource const* pkGndRsc = m_kGndResource.at(m_iNowFloor);
		if( pkGndRsc )
		{
			return pkGndRsc->GetAttr();
		}
	}
	return PgGround::GetAttr();
	//return GATTR_SUPER_GROUND;
}
int PgSuperGround::GetGroundNo()const
{
	if( m_kGndResource.size() > m_iNowFloor )
	{
		PgGroundResource const* pkGndRsc = m_kGndResource.at(m_iNowFloor);
		if( pkGndRsc )
		{
			return pkGndRsc->GroundKey().GroundNo();
		}
	}
	return PgGround::GetGroundNo();
}
int PgSuperGround::GetGroundItemRarityNo()const
{
	if( m_kContSuperGndInfo.empty() )
	{
		return GIOT_FIELD;
	}
	return m_kContSuperGndInfo.at(m_iNowFloor).aiItemRarityNo[m_eMode];
}
void PgSuperGround::CheckEnchantControl(PgBase_Item& rkItem)const
{
	if( !m_kContSuperGndInfo.empty() )
	{
		int const iEnchantPlusControlNo = m_kContSuperGndInfo.at(m_iNowFloor).aiItemEnchantControlNo[m_eMode];
		if( iEnchantPlusControlNo )
		{
			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const *pkItemDef = kItemDefMgr.GetDef( rkItem.ItemNo() );
			if ( !pkItemDef )
			{
				return;
			}

			int iMaxEnchantLv = 0;
			if ( true == PgItemRarityUpgradeFormula::GetMaxGradeLevel( ::GetItemGrade( rkItem ), pkItemDef->IsPetItem(), iMaxEnchantLv ) )
			{
				int const iEnchantLv = PgItemOptionMgr::DicePlusUp( iEnchantPlusControlNo, iMaxEnchantLv );
				if ( iEnchantLv > 0 )
				{
					EPlusItemUpgradeResult kRet;
					TBL_DEF_ITEMPLUSUPGRADE const *pkPlusInfo = PgAction_ItemPlusUpgrade::GetPlusInfo( rkItem, iEnchantLv, kRet );
					if ( pkPlusInfo )
					{
						SEnchantInfo kNewEnchantInfo = rkItem.EnchantInfo();

						size_t iRetIndex = 0;
						if( true == ::RouletteRate( pkPlusInfo->RareGroupSuccessRate, iRetIndex, MAX_ITEM_RARE_KIND_ARRAY) )
						{
							int const iResultType = PgAction_ItemPlusUpgrade::GetRareType( pkPlusInfo->RareGroupNo, iRetIndex );
							kNewEnchantInfo.PlusType(iResultType);
							kNewEnchantInfo.PlusLv(iEnchantLv);
							rkItem.EnchantInfo( kNewEnchantInfo );
						}
					}
				}
			}
		}
	}
}

void PgSuperGround::SetState( EIndunState const eState, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	if(eState == m_eState)
	{
		return;
	}

	switch( eState ) // 특정 상태는 PgIndun::SetState() 사용 한다
	{
	case INDUN_STATE_OPEN:
	case INDUN_STATE_WAIT:
	case INDUN_STATE_PLAY:
	case INDUN_STATE_RESULT_WAIT:
		{
			PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
			return;
		}break;
	case INDUN_STATE_READY:
	case INDUN_STATE_RESULT:
		{
			if( 0 != (GetAttr() & GATTR_FLAG_BOSS) )
			{
				PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
				return;
			}
		}break;
	default:
		{
		}break;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = eState;

	switch( m_eState )
	{
	case INDUN_STATE_READY:
		{
			{
				GroundWeight( std::max<int>( static_cast<int>(PgObjectMgr::GetUnitCount(UT_PLAYER)) - 1, 0 ) );
				bool const bOnce = true;
				bool const bNotReset = false;
				int iCopyCount = m_iGroundWeight;
				switch( m_iGroundWeight )
				{
				case 3:
				case 4:
					{
						iCopyCount = 1; // 2배
					}break;
				case 1: // 1명
				case 2:
				default:
					{
						iCopyCount = 0; // 1배
					}break;
				}
				ActivateMonsterGenGroup( 0, bNotReset, bOnce, iCopyCount );
			}
			PgGround::OnActivateEventMonsterGroup();

			MapLoadComplete();
			if ( !OpeningMovie() )
			{
				SetState( INDUN_STATE_PLAY );
			}
		}break;
	default:
		{
		}break;
	}
}

void PgSuperGround::OnTick1s()
{
	BM::CAutoMutex kLock(m_kRscMutex);

	DWORD const dwNow = BM::GetTime32();
	DWORD dwElapsed = 0;
	bool bTimeUpdate = true;

	if( INDUN_STATE_PLAY == m_eState )
	{
		bTimeUpdate = false;
	}
	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsed, bTimeUpdate);

	switch( m_eState )
	{
	case INDUN_STATE_PLAY:
		{
			if( 0 < m_dwTimeLimit )
			{
				m_dwAccumLimitTime += dwElapsed;
				if( m_dwTimeLimit <= m_dwAccumLimitTime )
				{
					m_dwTimeLimit = 0;
					m_dwAccumLimitTime = 0;
					m_bFailedTime = true;
					BM::Stream kNfyPacket(NFY_M_C_LIMITED_TIME_MSEC); // 실패
					Broadcast(kNfyPacket);

					// 플레이어 죽이고
					CUnit* pkUser = NULL;
					CONT_OBJECT_MGR_UNIT::iterator unit_iter;
					PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
					while(( pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL)
					{
						if( pkUser->IsAlive() )
						{
							//::OnDamaged(NULL, pkUser, 0, pkUser->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime()); // 죽어라
							pkUser->ClearAllEffect();
							pkUser->SetAbil(AT_HP, 0, true, true);
						}
					}
				}
			}
			size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER ) - m_kContNotCountMonster.size();
			if( iLiveMonsterCount != m_iLastRemainMonsterCount )
			{
				m_iLastRemainMonsterCount = iLiveMonsterCount;
				BM::Stream kPacket(NFY_M_C_REMAIN_MONSTER, m_iLastRemainMonsterCount);
				Broadcast(kPacket);
			}

			// 죽어서 1분 지나면 최근 그라운드로
			ContGuidSet kContGuid;
			m_kTimeEventMgr.Tick(dwElapsed, PgSuperGroundTimeEventMgr::SSuperGroundTickFunc(kContGuid));
			ContGuidSet::const_iterator guid_iter = kContGuid.begin();
			while( kContGuid.end() != guid_iter )
			{
				CUnit* pkUnit = GetUnit((*guid_iter));
				if( pkUnit )
				{
					PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
					if( pkPlayer
					&&	pkPlayer->IsDead() )
					{
						RecvRecentMapMove(pkPlayer);
					}
				}
				++guid_iter;
			}
		}break;
	case INDUN_STATE_OPEN:
	case INDUN_STATE_WAIT:
	case INDUN_STATE_READY:
	case INDUN_STATE_RESULT_WAIT:
	case INDUN_STATE_RESULT:
	case INDUN_STATE_CLOSE:
	default:
		{
		}break;
	}
	{
		PgIndun::OnTick1s(); // 
	}

	UpdateAutoNextState( dwNow, dwElapsed );
}
bool PgSuperGround::VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy )
{
	switch( wType )
	{
	case PT_C_M_REQ_EXIT_SUPER_GROUND:
		{
			PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
			if( pkPlayer )
			{
				BM::GUID const kPartyGuid( pkPlayer->GetPartyGuid() );
				BM::GUID const kCharGuid( pkPlayer->GetID() );
				if( BM::GUID::IsNotNull(kPartyGuid) )
				{
					size_t const iMinSize = 1;
					VEC_GUID kContGuid;
					m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kContGuid, kCharGuid);
					if( m_kLocalPartyMgr.IsMaster(kPartyGuid, kCharGuid)
					&&	iMinSize <= kContGuid.size() )
					{
						BM::Stream kNPacket(PT_C_N_REQ_PARTY_CHANGE_MASTER, kCharGuid);
						kNPacket.Push( (*kContGuid.begin()) );
						::SendToGlobalPartyMgr(kNPacket);
					}
				}

				RecvRecentMapMove(pkPlayer);
			}
		}break;
	default:
		{
			return PgIndun::VUpdate( pkUnit, wType, pkNfy );
		}break;
	}
	return true;
}
bool PgSuperGround::RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder )
{
	//BM::CAutoMutex Lock(m_kRscMutex);
	if ( !PgIndun::RecvMapMove( rkUnitArray, rkRMM, kContPetMapMoveData, kContUnitSummonedMapMoveData, kModifyOrder ) )
	{
		return false;
	}
	return true;
}
bool PgSuperGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	//switch( usType )
	//{
	//default:
	//	{
	//	}break;
	//}
	//return true;
	return PgIndun::RecvGndWrapped( usType, pkPacket );
}
void PgSuperGround::WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const
{
	size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	rkPacket.Push( m_iSuperGroundNo );
	rkPacket.Push( static_cast< int >(m_eMode) );
	rkPacket.Push( m_iNowFloor );
	rkPacket.Push( iLiveMonsterCount );
	rkPacket.Push( m_kGndResource.size() ); // 첫 층일 때에만 전체 층을 전송한다
}
bool PgSuperGround::NextFloor(int iNextFloor, int const iSpawnNo, bool const bForce)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	if( m_eState != INDUN_STATE_PLAY )
	{
		return false;
	}
	if( !bForce
	&&	m_iNowFloor >= iNextFloor )
	{
		return false;
	}
	if( bForce )
	{
		if( 0 > iNextFloor )
		{
			iNextFloor = 0;
		}
		else if( m_kGndResource.size() <= iNextFloor )
		{
			iNextFloor = m_kGndResource.size() - 1;
		}
	}
	if( m_kGndResource.size() <= iNextFloor )
	{
		return false;
	}

	size_t const iLiveMonsterCount = PgObjectMgr::GetUnitCount( UT_MONSTER );
	if( !bForce
	&&	0 < iLiveMonsterCount )
	{
		int const iMessageNo = 400234;
		BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, iMessageNo );
		kPacket.Push( static_cast< BYTE >(EL_Warning) );
		Broadcast(kPacket);
		return false;
	}

	// 층 변경 시작
	m_dwTimeLimit = 0;
	m_dwAccumLimitTime = 0;
	m_bFailedTime = false;
	m_iLastRemainMonsterCount = 0;
	m_kContNotCountMonster.clear();
	m_kTimeEventMgr.Clear();
	PgWorldEventMgr::ClearGenPointCount();

	//
	BM::Stream kTempMovePacket;

	{// 1. Player를 Packet으로 만들기
		UNIT_PTR_ARRAY kUnitArray;

		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_PLAYER, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_PLAYER, kItor)) != NULL )
		{
			kUnitArray.Add( pkUnit, false, false, false );
		}

		if ( kUnitArray.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		kUnitArray.WriteToPacket( kTempMovePacket, WT_MAPMOVE_SERVER );
	}

	{// 2. Pet을 Packet으로 만들기
		size_t iPetSize = 0;
		size_t const iWRPos = kTempMovePacket.WrPos();
		kTempMovePacket.Push( iPetSize );

		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_PET, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_PET, kItor)) != NULL )
		{
			PgPet * pkPet = dynamic_cast<PgPet*>(pkUnit);
			if ( pkPet )
			{
				pkPet->WriteToPacket_SPetMapMoveData(kTempMovePacket);
				++iPetSize;
			}
		}

		kTempMovePacket.ModifyData( iWRPos, &iPetSize, sizeof(iPetSize) );
	}

	CONT_UNIT_SUMMONED_MAPMOVE_DATA kContUnitSummonedData;
	{// 3. 소환체 컨테이너를 만듬
		//CONT_UNIT_SUMMONED_MAPMOVE_DATA	//BM::GUID(Player), 소환체들
		//CONT_SUMMONED_MAPMOVE_DATA		//BM::GUID(Summoned), 소환체		
		CUnit *pkUnit = NULL;
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectMgr2::GetFirstUnit(UT_SUMMONED, kItor);
		while ( (pkUnit = PgObjectMgr2::GetNextUnit(UT_SUMMONED, kItor)) != NULL )
		{
			PgSummoned * pkSummoned = dynamic_cast<PgSummoned*>(pkUnit);
			if ( pkSummoned && pkSummoned->IsAlive() )
			{
				auto kPair = kContUnitSummonedData.insert(std::make_pair(pkSummoned->Caller(), SUserSummonedMapMoveData()));
				CONT_SUMMONED_MAPMOVE_DATA & kContSummoned = kPair.first->second.kSummonedMapMoveData;

				BM::Stream kSummonedPacket;
				pkSummoned->WriteToPacket_MapMoveData(kSummonedPacket);

				BM::GUID kGuid;
				kSummonedPacket.Pop(kGuid);
				if(kGuid==pkSummoned->GetID())
				{
					SSummonedMapMoveData kValue;
					kValue.ReadFromPacket(kSummonedPacket);
					
					kContSummoned.insert(std::make_pair(pkSummoned->GetID(), kValue));
				}
			}
		}
	}

	// 4. Release / Init
	SGroundKey kTempGroundKey = m_kGroundKey;
	m_iNowFloor = iNextFloor;
	PgGroundResource const *pkGndRsc = m_kGndResource.at(m_iNowFloor);

	ReleaseZone();
	ReleaseAllUnit();
	m_kContGenPoint_Monster.clear();
	m_kContGenPoint_Object.clear();
	m_kContGenPoint_SummonNPC.clear();
	m_kAIMng.Release();
	CloneResource( pkGndRsc );
	if( 0 != (GetAttr() & GATTR_FLAG_BOSS) )
	{
		PgIndun::Init( m_iMonsterControlID, true );
	}
	else
	{
		PgIndun::Init( m_iMonsterControlID, false );
	}

	POINT3 pt3TargetPos;
	if( FAILED(FindSpawnLoc(iSpawnNo, pt3TargetPos)) )
	{
		if( FAILED(FindSpawnLoc(1, pt3TargetPos)) )
		{
			return false;
		}
	}

	// 5. 패킷으로 만든 Player들 뽑아오기
	UNIT_PTR_ARRAY kTempUnitArray;
	CONT_PET_MAPMOVE_DATA kContPetData;
	kTempUnitArray.ReadFromPacket( kTempMovePacket );
	PU::TLoadTable_AM( kTempMovePacket, kContPetData );

	// 6. 패킷에서 뽑아온 Player들과 Pet, Summoned 복구시켜놓기
	VEC_JOININDUN kJoinIndunList;
	VEC_GUID kWaitList;
	UNIT_PTR_ARRAY::iterator unit_itr = kTempUnitArray.begin();
	for ( ; unit_itr!= kTempUnitArray.end(); ++unit_itr )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( pkUser )
		{
			kJoinIndunList.push_back( SNfyJoinIndun(pkUser->GetID(),pkUser->GetMemberGUID(),pkUser->GetAbil64(AT_EXPERIENCE)) );
			if ( 0 == m_iNowFloor )
			{
				// 콤보 카운터 초기화
				PgComboCounter* pkCounter = NULL;
				if( GetComboCounter(pkUser->GetID(), pkCounter) )
				{
					pkCounter->Clear(false);
				}
			}

			pkUser->SetPos(pt3TargetPos);
			pkUser->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);//에어리어가 이걸로 강제
			pkUser->MapLoading();	// PT_C_M_NFY_MAPLOADED 받을때 까지 어떠한 Action도 금지~
			AddUnit( unit_itr->pkUnit, false );
			unit_itr->bAutoRemove = false;	// 자동삭제는 막아야지...
			
			kWaitList.push_back( pkUser->GetID() );

			BM::GUID const kSelectedPetID = pkUser->SelectedPetID();
			if ( kSelectedPetID.IsNotNull() )
			{
				CONT_PET_MAPMOVE_DATA::iterator pet_move_itr = kContPetData.find( kSelectedPetID );
				if ( pet_move_itr != kContPetData.end() )
				{
					this->CreatePet( pkUser, kSelectedPetID, pet_move_itr->second );
				}
// 				else
// 				{
// 					CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Not Found SelectedPet<" << kSelectedPetID << L">Info From User<" << pkUser->Name() << L"/" << pkUser->GetID() << L">" );
// 				}
			}

			//소환체 생성
			CONT_UNIT_SUMMONED_MAPMOVE_DATA::iterator cont_summoned = kContUnitSummonedData.find(pkUser->GetID());
			if(cont_summoned != kContUnitSummonedData.end())
			{
				CONT_SUMMONED_MAPMOVE_DATA const& kSummonedMapMoveData = cont_summoned->second.kSummonedMapMoveData;
				CONT_SUMMONED_MAPMOVE_DATA::const_iterator c_it = kSummonedMapMoveData.begin();
				while(c_it != kSummonedMapMoveData.end())
				{
					SCreateSummoned kCreateInfo;
					kCreateInfo.kGuid = c_it->first;
					kCreateInfo.kClassKey.iClass = c_it->second.iClass;
					kCreateInfo.kClassKey.nLv = c_it->second.iLevel;
					kCreateInfo.bUniqueClass = c_it->second.bUniqueClass;
					kCreateInfo.sNeedSupply = c_it->second.sNeedSupply;
					kCreateInfo.iLifeTime = c_it->second.iLifeTime;

					CreateSummoned(pkUser, &kCreateInfo, L"Summoned", c_it->second);

					++c_it;
				}
			}
		}
	}

	// 7. WaitList를 초기화 하고
	if ( !kWaitList.empty() )
	{
		InitWaitUser( kWaitList );
	}

	// 8. Player한테 적절한 패킷(일반 맵이동과 같은)보내기
	for ( unit_itr=kTempUnitArray.begin(); unit_itr!=kTempUnitArray.end(); ++unit_itr )
	{
		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( pkUser )
		{
			// SwitchServer 에 User Ground 변경을 알려주자.
			BM::Stream kDPacket( PT_M_S_NFY_CHARACTER_MAP_MOVE);
			kDPacket.Push(pkUser->GetMemberGUID());
			kDPacket.Push(pkUser->GetID());
			g_kProcessCfg.ServerIdentity().WriteToPacket(kDPacket);
			GroundKey().WriteToPacket(kDPacket);
			SendToServer( pkUser->GetSwitchServer(), kDPacket );

			// client에 맵로딩 요청하기
			BM::Stream kCPacket( PT_T_C_NFY_CHARACTER_MAP_MOVE );
			kCPacket.Push((BYTE)MMET_Normal);
			kCPacket.Push(GroundKey());
			kCPacket.Push(this->GetAttr());
			kCPacket.Push(pkUser->GetID());
			kCPacket.Push(static_cast<size_t>(1));
			pkUser->WriteToPacket( kCPacket, WT_MAPMOVE_CLIENT );//현재 내 상태도 뒤에 붙임

			m_kSmallAreaInfo.WriteToPacket( kCPacket );// Zone 정보를 붙여서 보내야 한다.
			pkUser->Send( kCPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );

			// HP,MP가 Client가 잘못 계산하여 올바르게 안보이는 버그가 있어, 어빌값을 한번 더 보내 준다.
			WORD const wAbil[2] = { AT_HP, AT_MP };
			pkUser->SendAbiles( wAbil, 2, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);

			if(S_OK!=pkUser->IsOpenWorldMap(m_kGroundKey.GroundNo()))
			{//이 맵에 왔다 갔다고 업데이트
				CONT_PLAYER_MODIFY_ORDER kOrder;
				tagPlayerModifyOrderData_AddWorldMap kWorldMap(m_kGroundKey.GroundNo());
				kOrder.push_back( SPMO(IMET_ADD_WORLD_MAP, pkUser->GetID(), kWorldMap) );
				PgAction_ReqModifyItem kItemModifyAction(IMEPT_WORLD_MAP, kTempGroundKey, kOrder);
				kItemModifyAction.DoAction(pkUser, NULL);
			}
		}
	}

	// 다시 원래의 그라운드 번호를 셋팅해 주어야 한다.
	if( !m_kContSuperGndInfo.empty() )
	{
		m_kGroundKey.GroundNo( m_kContSuperGndInfo.at(0).iGroundNo );
	}

	// 그외 처리
	if( 0 != (GetAttr() & GATTR_FLAG_BOSS) )
	{
		BM::Stream kResultMgrPacket(PT_M_T_ANS_CREATE_GROUND, GroundKey());
		PU::TWriteArray_A(kResultMgrPacket ,kJoinIndunList );
		kResultMgrPacket.Push(S_OK);
		::SendToChannelContents(PMET_Boss, kResultMgrPacket);

		/*BM::Stream kNPacket( PT_M_N_NFY_JOIN_INDUN_PLAYER, GroundKey() );
		kNPacket.Push( kJoinIndunList );
		::SendToResultMgr(kNPacket);*/
	}
	return true;
}
void PgSuperGround::AddFloor(ContFloorResource& rkContFloorResource)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	m_kGndResource.swap(rkContFloorResource);
}

HRESULT PgSuperGround::InsertMonster(TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, int const iMonNo, BM::GUID &rkOutGuid, CUnit* pkCaller, bool bDropAllItem, int iEnchantGradeNo, ECREATE_HP_TYPE const eCreate_HP_Type)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	int const iMonsterEnchantGradeProbNo = (m_kContSuperGndInfo.empty()? 0: m_kContSuperGndInfo.at(m_iNowFloor).aiMonsterEnchantGroupNo[m_eMode]);
	int const iNewEnchantGradeNo = (iEnchantGradeNo? iEnchantGradeNo: PgAction_MonsterEnchantProbCalc(iMonsterEnchantGradeProbNo).Get());
	HRESULT const hRet = PgGround::InsertMonster(rkGenInfo, iMonNo, rkOutGuid, pkCaller, bDropAllItem, iNewEnchantGradeNo);
	if( SUCCEEDED(hRet) )
	{
		CUnit* pkMonster = GetUnit(rkOutGuid);
		if( pkMonster )
		{
			switch( pkMonster->GetAbil(AT_GRADE) )
			{
			case EMGRADE_ELITE:
			case EMGRADE_BOSS:
				{
					int const iTimeLimit = pkMonster->GetAbil(AT_TIME_LIMIT);
					if( 0 < iTimeLimit )
					{
						m_dwTimeLimit = static_cast< DWORD >(iTimeLimit);
						m_dwAccumLimitTime = 0;
						m_bFailedTime = false;
						BM::Stream kNfyPacket(NFY_M_C_REMAIN_TIME_MSEC, m_dwTimeLimit);
						Broadcast(kNfyPacket);
					}

					int const iBossItemContainerNo = m_kContSuperGndInfo.at(m_iNowFloor).aiBossItemContainerNo[m_eMode];
					if( 0 < iBossItemContainerNo )
					{
						pkMonster->SetAbil(AT_ITEM_DROP_CONTAINER, iBossItemContainerNo);
					}
				}break;
			default:
				{
				}break;
			}

			if( 0 != pkMonster->GetAbil(AT_NOT_COUNT) )
			{
				m_kContNotCountMonster.insert( pkMonster->GetID() );
			}
		}
	}
	return hRet;
}
void PgSuperGround::SetSuperGroundMode(int const iSuperGroundNo, ESuperGroundMode const eMode, size_t const iFloorIdx)
{
	CONT_DEF_SUPER_GROUND_GROUP const* pkDefSuperGround = NULL;
	g_kTblDataMgr.GetContDef(pkDefSuperGround);

	CONT_DEF_SUPER_GROUND_GROUP::const_iterator find_iter = pkDefSuperGround->find(iSuperGroundNo);
	if( pkDefSuperGround->end() != find_iter )
	{
		m_iSuperGroundNo = iSuperGroundNo;
		if( SGM_NORMAL <= eMode
		&&	SGM_RARE >= eMode )
		{
			m_eMode = eMode;
		}
		CONT_DEF_SUPER_GROUND_GROUP::mapped_type const& rkDefSuperGnd = (*find_iter).second;
		m_kContSuperGndInfo = rkDefSuperGnd.kContSuperGround;
	}
	m_iNowFloor = iFloorIdx;
}
void PgSuperGround::RecvUnitDie(CUnit *pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	PgIndun::RecvUnitDie(pkUnit);
	if( pkUnit )
	{
		if( pkUnit->IsInUnitType(UT_MONSTER) )
		{
			if( 0 != pkUnit->GetAbil(AT_NOT_COUNT) )
			{
				m_kContNotCountMonster.erase( pkUnit->GetID() );
			}
			if( 0 < pkUnit->GetAbil(AT_TIME_LIMIT) )
			{
				m_dwTimeLimit = 0;
				m_dwAccumLimitTime = 0;
				m_bFailedTime = false;
				BM::Stream kNfyPacket(NFY_M_C_REMAIN_TIME_MSEC, m_dwTimeLimit);
				Broadcast(kNfyPacket);
			}
		}
		if( pkUnit->IsUnitType(UT_PLAYER) )
		{
			DWORD const dwWaitReviveTime = 60000; // 어떻게 죽던 1분
			m_kTimeEventMgr.DelType(pkUnit->GetID(), SGTET_RECENT_MAP_MOVE); // 지웠다
			m_kTimeEventMgr.Add(pkUnit->GetID(), SGTET_RECENT_MAP_MOVE, dwWaitReviveTime); // 다시 넣자 (초기화)
		}
	}
}
bool PgSuperGround::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if( pkUnit )
	{
		m_kTimeEventMgr.DelAll(pkUnit->GetID());
	}
	return PgIndun::ReleaseUnit(pkUnit, bRecursiveCall, bSendArea);
}
void PgSuperGround::UseSuperGroundFeather(CUnit* pkCasterUnit, int const iHPRate, int const iMPRate)
{
	if( !pkCasterUnit )
	{
		return;
	}

	int const iMinValue = 1;
	if( m_bFailedTime )
	{
		// 시간으로 실패시 전원 살리고, 현재 맵 재 시도
		CUnit* pkUser = NULL;
		CONT_OBJECT_MGR_UNIT::iterator unit_iter;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
		while(( pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL)
		{		
			if( pkUser->IsDead() )
			{
				PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUser); // 혼자만 부활
				if(pkPlayer)
				{
					int const iNewHP = pkPlayer->GetAbil(AT_C_MAX_HP) * iHPRate / ABILITY_RATE_VALUE;
					int const iNewMP = pkPlayer->GetAbil(AT_C_MAX_MP) * iMPRate / ABILITY_RATE_VALUE;
					pkPlayer->Alive(EALIVE_INSURANCE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, std::max(iNewHP,iMinValue), std::max(iNewMP,iMinValue));

					PgDeathPenalty kAction(GroundKey(), LURT_Item, false);
					kAction.DoAction(pkPlayer, NULL);
				}
			}
		}
		NextFloor(m_iNowFloor, 1, true); // 재시도
	}
	else
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkCasterUnit); // 혼자만 부활
		if(pkPlayer)
		{
			int const iNewHP = pkPlayer->GetAbil(AT_C_MAX_HP) * iHPRate / ABILITY_RATE_VALUE;
			int const iNewMP = pkPlayer->GetAbil(AT_C_MAX_MP) * iMPRate / ABILITY_RATE_VALUE;
			pkPlayer->Alive(EALIVE_INSURANCE, E_SENDTYPE_BROADALL | E_SENDTYPE_MUSTSEND, std::max(iNewHP,iMinValue), std::max(iNewMP,iMinValue));

			PgDeathPenalty kAction(GroundKey(), LURT_Item, false);
			kAction.DoAction(pkPlayer, NULL);
		}
	}
}