#include "stdafx.h"
#include "Variant/AlramMissionMgr.h"
#include "PgAction.h"
#include "PgGround.h"

namespace SORT
{
	bool CreateSummoned_OverlapClass(PgSummoned const* lhs, PgSummoned const* rhs)
	{
		if(!lhs) return false;
		if(!rhs) return false;

		if( lhs->LifeTime() < rhs->LifeTime() )	{return true;}
		if( lhs->LifeTime() > rhs->LifeTime() )	{return false;}

		return false;
	}
}

void PgGround::OnTick_AlramMission( PgPlayer * pkPlayer, DWORD const dwCurTime )
{
	using namespace ALRAM_MISSION;
	if ( true == this->IsAlramMission() )
	{
		PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();
		switch ( rkAlramMission.OnTick( dwCurTime ) )
		{
		case RET_SUCCESS:
			{
				BM::Stream kEventPacket( PT_M_C_NFY_ALRAMMISSION_SUCCESS );
				pkPlayer->Send( kEventPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );

				this->CallAlramReward( pkPlayer );
			}break;
		case RET_NEED_NEWACTION:
			{
				if ( SUCCEEDED(m_pkAlramMissionMgr->GetNewAction(pkPlayer->GetAbil(AT_CLASS), rkAlramMission)) )
				{
					// 통보 해주어야 한다.
					BM::Stream kNewAlraMPacket( PT_M_C_NFY_ALRAMMISSION_BEGIN, rkAlramMission.GetID() );
					kNewAlraMPacket.Push( rkAlramMission.GetBeginTime() );
					pkPlayer->Send( kNewAlraMPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );
				}
			}break;
		case RET_TIMEOVER:
			{
				BM::Stream kFaildPacket( PT_M_C_NFY_ALRAMMISSION_END );
				pkPlayer->Send( kFaildPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
			}break;
		default:
			{
			}break;
		}
	}
}

void PgGround::CallAlramReward( PgPlayer * pkPlayer )
{
	BM::CAutoMutex kLock(m_kRscMutex);

	PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();

	TBL_DEF_ALRAM_MISSION const * const pkDef = rkAlramMission.GetDef();
	if ( pkDef )
	{
		int const iLevel = pkPlayer->GetAbil(AT_LEVEL);

		if ( pkDef->iExp )
		{
			PgAction_AddExp kAddPlayerExpAction( GroundKey(), static_cast<__int64>(pkDef->iExp), AEC_AlramMission, this, 0 );
			kAddPlayerExpAction.DoAction( pkPlayer, NULL );
		}

		if ( pkDef->iEffect )
		{
			SActArg kArg;
			PgGroundUtil::SetActArgGround(kArg, this);
			pkPlayer->AddEffect( pkDef->iEffect, 0, &kArg, NULL );
		}

		CONT_ITEM_CREATE_ORDER kItemCreateOrder;
		for ( int i = 0; i<MAX_ALRAM_MISSION_ITEMBAG; ++i )
		{
			if ( pkDef->iItemBag[i] )
			{
				PgItemBag kItemBag;
				GET_DEF(CItemBagMgr, kItemBagMgr);
				if ( S_OK == kItemBagMgr.GetItemBag( pkDef->iItemBag[i], static_cast<short>(iLevel), kItemBag ) )
				{
					int iItemNo = 0;
					int iCount = 0;
					if ( S_OK == kItemBag.PopItem(iLevel, iItemNo, iCount) )
					{
						PgBase_Item kItem;
						if( SUCCEEDED(::CreateSItem(iItemNo, iCount, GIOT_NONE, kItem)) )
						{
							kItemCreateOrder.push_back(kItem);
						}
					}
				}
			}
		}

		if( !kItemCreateOrder.empty() )
		{
			PgAction_CreateItem kCreateAction(CIE_AlramMission, GroundKey(), kItemCreateOrder);
			kCreateAction.DoAction( pkPlayer, NULL );							
		}
	}

	if ( rkAlramMission.GetNextID() )
	{
		// 새로운 Action으로 전이해야 한다.
		if ( SUCCEEDED(m_pkAlramMissionMgr->GetNextAction( rkAlramMission )) )
		{
			BM::Stream kNewAlraMPacket( PT_M_C_NFY_ALRAMMISSION_BEGIN, rkAlramMission.GetID() );
			kNewAlraMPacket.Push( rkAlramMission.GetBeginTime() );
			pkPlayer->Send( kNewAlraMPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE );
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Next Action Failed!!! NaxtActionID<" << rkAlramMission.GetNextID() << L"> CurrentID<" << rkAlramMission.GetID() << L"> CharGuid<" << pkPlayer->GetID() << L">" );
		}
	}
}

int PgGround::GetTotalSummonedSupply(CUnit* pkCaller)const
{
	int iCount = 0;
	if(NULL==pkCaller)	//Caller가 없는 소환수는 절대 없다!
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pkCaller NULL Data.");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
        return iCount;
	}

	CUnit * pkUnit = NULL;
	PgSummoned * pkSummoned = NULL;
	VEC_SUMMONUNIT const& kContSummonUnit = pkCaller->GetSummonUnit();
	for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
	{
		pkUnit = GetUnit((*c_it).kGuid);
		if(pkUnit && pkUnit->IsUnitType(UT_SUMMONED) && (pkSummoned = dynamic_cast<PgSummoned*>(pkUnit)) && pkSummoned->IsAlive())
		{
			iCount += pkSummoned->Supply();
		}
	}

	return iCount;
}

CUnit* PgGround::CreateSummoned(CUnit* pCaller, SCreateSummoned* pCreateInfo, LPCTSTR lpszName, SSummonedMapMoveData const& kMapMoveData, POINT3 kCreatePos)
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if(NULL==pCaller)	//Caller가 없는 소환수는 절대 없다!
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pCaller NULL Data.");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
        return NULL;
	}

	if(NULL==pCreateInfo)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"SSCreateSummoned NULL Data. Caller is "<<pCaller->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
        return NULL;
	}	

	if(UT_PLAYER!=pCaller->UnitType())	//Caller가 플레이어가 아니면 절대 안된다
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pCaller is not UT_PLAYER. "<<lpszName);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
        return NULL;
	}

	PgSummoned *pSummoned = dynamic_cast<PgSummoned*>(g_kTotalObjMgr.CreateUnit(UT_SUMMONED, pCreateInfo->kGuid));
	if(NULL==pSummoned)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot CreateSummoned Guid["<<pCreateInfo->kGuid<<L"] Caller Is "<<pCaller->GetID());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	SummonedInfo_ kInfo(pCreateInfo->kGuid, pCreateInfo->kClassKey);
	kInfo.bSyncUnit = true;
	kInfo.bEternalLife = (0==pCreateInfo->iLifeTime);	//엔티티의 AUTO_HEAL때문에 있는 변수임

	Direction eFrontDir = DIR_DOWN;

	kInfo.kCaller = pCaller->GetID();
	kInfo.SetAbil(AT_TEAM, pCaller->GetAbil(AT_TEAM));
	kInfo.SetAbil(AT_OWNER_TYPE, pCaller->GetAbil(AT_OWNER_TYPE));
	kInfo.SetAbil(AT_CALLER_TYPE, pCaller->UnitType());
	kInfo.SetAbil(AT_CALLER_LEVEL, pCaller->GetAbil(AT_LEVEL));
	kInfo.SetAbil(AT_HIT_SUCCESS_VALUE, pCaller->GetAbil(AT_C_HIT_SUCCESS_VALUE));
	kInfo.SetAbil(AT_DODGE_SUCCESS_VALUE, pCaller->GetAbil(AT_C_DODGE_SUCCESS_VALUE));
	kInfo.SetAbil(AT_PHY_ATTACK_MIN, pCaller->GetAbil(AT_C_PHY_ATTACK_MIN));
	kInfo.SetAbil(AT_PHY_ATTACK_MAX, pCaller->GetAbil(AT_C_PHY_ATTACK_MAX));
	kInfo.SetAbil(AT_MAGIC_ATTACK_MIN, pCaller->GetAbil(AT_C_MAGIC_ATTACK_MIN));
	kInfo.SetAbil(AT_MAGIC_ATTACK_MAX, pCaller->GetAbil(AT_C_MAGIC_ATTACK_MAX));
	kInfo.SetAbil(AT_PHY_DEFENCE, pCaller->GetAbil(AT_C_PHY_DEFENCE));
	kInfo.SetAbil(AT_PHY_DMG_DEC, pCaller->GetAbil(AT_C_PHY_DMG_DEC));
	kInfo.SetAbil(AT_MAGIC_DEFENCE, pCaller->GetAbil(AT_C_MAGIC_DEFENCE));
	kInfo.SetAbil(AT_MAGIC_DMG_DEC, pCaller->GetAbil(AT_C_MAGIC_DMG_DEC));
	kInfo.SetAbil(AT_ABS_ADDED_DMG_PHY, pCaller->GetAbil(AT_C_ABS_ADDED_DMG_PHY) );
	kInfo.SetAbil(AT_ABS_ADDED_DMG_MAGIC, pCaller->GetAbil(AT_C_ABS_ADDED_DMG_MAGIC) );
	kInfo.SetAbil(AT_PHY_DMG_PER, pCaller->GetAbil(AT_PHY_DMG_PER));
	kInfo.SetAbil(AT_MAGIC_DMG_PER, pCaller->GetAbil(AT_MAGIC_DMG_PER));
	kInfo.SetAbil(AT_MAX_HP, pCaller->GetAbil(AT_C_MAX_HP));
	kInfo.SetAbil(AT_DUEL, pCaller->GetAbil(AT_DUEL));

	{// 크리티컬 관계된 어빌들
		kInfo.SetAbil(AT_CRITICAL_SUCCESS_VALUE, pCaller->GetAbil(AT_C_CRITICAL_SUCCESS_VALUE));
		kInfo.SetAbil(AT_CRITICAL_ONEHIT, pCaller->GetAbil(AT_CRITICAL_ONEHIT));
		kInfo.SetAbil(AT_CRITICAL_POWER, pCaller->GetAbil(AT_C_CRITICAL_POWER));
	}

	if ( !SUCCEEDED( pSummoned->Create( &kInfo ) ))
	{
		g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pSummoned) );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Summoned->Create fail. Return NULL"));
		return NULL;
	}
	
	if ( lpszName )
	{
		pSummoned->Name( lpszName );
	}
	
	// 생성되는 위치
	int iReturnZoneRange = pSummoned->GetAbil(AT_RETURNZONE_RANGE);
	if(0==iReturnZoneRange)
	{
		iReturnZoneRange = AI_MONSTER_MIN_DISTANCE_CHASE_Q;
	}

	POINT3BY const& kPathNormalBy = pSummoned->PathNormal();
	POINT3 Oriented(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	Oriented.Normalize();

	if(POINT3::NullData() == kCreatePos)
	{
		PgCreateSpreadPos kAction(pCaller->GetPos());
		kAction.AddDir(PhysXScene()->GetPhysXScene(), Oriented, iReturnZoneRange);

		POINT3 kCreatePos;
		kAction.PopPos(kCreatePos, 4);
	}

	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(kCreatePos.x, kCreatePos.y, kCreatePos.z+30), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kCreatePos.z = kHit.worldImpact.z;
	}

	pSummoned->SetPos( kCreatePos );

	//
	//pSummoned->SetState(US_IDLE);
	pSummoned->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
	pSummoned->FrontDirection( eFrontDir );
	pSummoned->LifeTime(pCreateInfo->iLifeTime);
	pSummoned->SetAbil(AT_LIFETIME, pCreateInfo->iLifeTime);
	pSummoned->UniqueClass(pCreateInfo->bUniqueClass);
	pSummoned->Supply(pCreateInfo->sNeedSupply);
	pSummoned->SetNPC(pCreateInfo->bNPC);

	bool const bMapMoveCreate = false==kMapMoveData.IsNull();
	if(bMapMoveCreate)
	{
		pSummoned->SetAbil(AT_HP, std::min<int>(kMapMoveData.iHP, pSummoned->GetAbil(AT_C_MAX_HP)));
		pSummoned->SetAbil(AT_LIFETIME, kMapMoveData.iMaxLifeTime);
		if ( kMapMoveData.kAddOnPacket.Size() )
		{
			BM::Stream kAddPacket(kMapMoveData.kAddOnPacket);
			PgUnitEffectMgr & rkEffect = pSummoned->GetEffectMgr();
			rkEffect.ReadFromPacket( kAddPacket, false );
		}
	}

	if(pCreateInfo->bUniqueClass)
	{//유니크 클래스면 재확인
		if(pCaller->IsSummonUnitClass(pSummoned->GetClassKey().iClass) )
		{
			pCaller->SendWarnMessage(792102);
			return NULL;
		}
	}

	if(pCreateInfo->sOverlapClass)
	{
		typedef std::vector<PgSummoned*> CONT_UNIT;
		CONT_UNIT kCont;
		CUnit * pkSummondUnit = NULL;
		VEC_SUMMONUNIT const& kContSummonUnit = pCaller->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			pkSummondUnit = GetUnit((*c_it).kGuid);
			if(pkSummondUnit && pkSummondUnit->IsUnitType(UT_SUMMONED) && pkSummondUnit->GetAbil(AT_CLASS)==pSummoned->GetAbil(AT_CLASS))
			{
				kCont.push_back(dynamic_cast<PgSummoned*>(pkSummondUnit));
			}
		}

		int iCount = kCont.size() - pCreateInfo->sOverlapClass + 1;
		if(iCount > 0)
		{
			std::sort(kCont.begin(), kCont.end(), SORT::CreateSummoned_OverlapClass);
			for(CONT_UNIT::const_iterator c_it = kCont.begin(); c_it != kCont.end(); ++c_it)
			{
				if((*c_it))
				{
					::OnDamaged(NULL, (*c_it), 0, (*c_it)->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
					--iCount;

					if(iCount <= 0)
					{
						break;
					}
				}
			}
		}
	}

	if( pCaller->AddSummonUnit( pSummoned->GetID(), pSummoned->GetClassKey().iClass, ESO_IGNORE_MAXCOUNT)
	 && AddUnit(pSummoned, true) )
	{
		UNIT_PTR_ARRAY kAddUnitArray;
		kAddUnitArray.Add(pSummoned);
		BM::Stream kAddPacket(PT_M_C_ADD_UNIT);
		kAddUnitArray.WriteToPacket( kAddPacket, WT_DEFAULT );
		pCaller->Send( kAddPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );

		if ( !pCaller->IsMapLoading() )
		{
			pSummoned->SetAddSummonedToOwner();
		}

		//소환사 패시브 적용
		if(false==bMapMoveCreate)
		{
			if(PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pCaller))
			{
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, this);

				GET_DEF(CSkillDefMgr, kSkillDefMgr);

				size_t szIndex = 0;
				int iSkillNo = 0;
				while ((iSkillNo = pkPlayer->GetMySkill()->GetSkillNo(EST_PASSIVE, szIndex++)) > 0)
				{
					CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);			
					if (NULL != pkSkillDef && ESTARGET_SUMMONED==pkSkillDef->GetTargetType())
					{
						typedef std::vector<int> CONT_INT;
						CONT_INT kContClassNo;
						for(int i =0 ; i < 3; ++i)
						{
							int const iClassNo = pkSkillDef->GetAbil(AT_MON_SKILL_TARGET_NO+i);
							if(0 < iClassNo)
							{
								kContClassNo.push_back(iClassNo);
							}
						}
						
						// 0 이면 대상에게만 적용, 0보다 크면 대상에게만 적용 안함
						bool const bExceptTarget = ( 0 < pkSkillDef->GetAbil(AT_MON_SKILL_TARGET_TYPE) );
						
						bool bApplyEffect = bExceptTarget | kContClassNo.empty();  
						/*	bApplyEffect = bExceptTarget+ kContClassNo.empty()에 대한 검증.
							일부 적용 안함true + 비어있음true = 모두 적용true 
							일부 적용 안함true + 안비어있음false = 적용true로 시작해서 검증
							일부만 적용false + 비어있음true = 모두 적용true					// 헌섭씨 코드 변경전, 값 없을경우 동작 방식
							일부만 적용false + 안비어있음flase = 적용false로 시작해서 검증
							모든 경우 정상동작
						*/
						
						for(CONT_INT::const_iterator kItor = kContClassNo.begin(); kContClassNo.end() != kItor; ++kItor)
						{// 등록된 목록이 있으면 검증하는데
							if( (*kItor) == pSummoned->GetClassKey().iClass )
							{
								if(false == bExceptTarget)
								{// 포함될때 적용한다면 // 하나라도 포함되면 바로 적용하고
									bApplyEffect = true;
									break;
								}
								else
								{// 포함될때 제외 한다면// 하나라도 포함되면 바로 제외 시키고
									bApplyEffect = false;
									break;
								}
							}
						}
						
						if(false == bApplyEffect)
						{
							continue;
						}
						
						for(int i=0; i<EFFECTNUM_MAX; ++i)
						{
							int const iEffectNo = pkSkillDef->GetAbil(AT_EFFECTNUM1+i);
							if(iEffectNo)
							{
								pSummoned->AddEffect(iEffectNo, 0, &kArg, pkPlayer );
							}
							else
							{
								break;
							}
						}
					}
				}
				// 소환체 종류가 몇개인지에 따라 이펙트 걸기
				RefreshSummonEffect(pkPlayer);
			}
		}
		return pSummoned;
	}

	g_kTotalObjMgr.ReleaseUnit(pSummoned);
	pCaller->DeleteSummonUnit( pSummoned->GetID() );
	return NULL;
}

CUnit* PgGround::CreateCustomUnit(int iClassNo, POINT3 const& ptPos, Direction const& rkDirFront)
{
	BM::CAutoMutex kLock( m_kRscMutex );
	{
		BM::GUID kID(" ");
		kID.Generate();
		CUnit *pkUnit = g_kTotalObjMgr.CreateUnit(UT_CUSTOM_UNIT, kID);
		if (pkUnit == NULL)
		{
			INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot CreateCustomObject Guid[" << kID << L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}

		SClassKey kKey(iClassNo, 1);
		SCustomUnitInfo kInfo(kID, kKey, ptPos);
		pkUnit->Create(&kInfo);
		pkUnit->Name(_T(" "));
		pkUnit->SetState(US_OPENING);
		pkUnit->LastAreaIndex( PgSmallArea::NONE_AREA_INDEX );
		pkUnit->FrontDirection( rkDirFront );
		pkUnit->SetPos(ptPos);
		AddUnit(pkUnit, true);

		return pkUnit;
	}


	return NULL;

}

void PgGround::RefreshSummonEffect(PgPlayer* pkPlayer) const
{// 소환체 종류가 몇개나 되는가에 따라 이펙트를 거는 패시브 스킬을 위한 함수
	if( !pkPlayer )
	{
		return;
	}
	
	int const iLearnSkillNo = pkPlayer->GetAbil(AT_SUMMON_EFFECT_LEARN_CHECK_SKILL_NO);
	if( 0 < iLearnSkillNo )
	{// 소환체 종류에 따라 이펙트를 거는 스킬을 배웠다면
		PgMySkill* pkMySkill = pkPlayer->GetMySkill();
		if( pkMySkill&& 
			0 < pkMySkill->GetLearnedSkill(iLearnSkillNo)
			)
		{
			SET_INT kContClassNo;
			int const iMaxCnt = pkPlayer->GetSummonUnitCount();
			for(int i=0; i < iMaxCnt; ++i)
			{// 소환체 종류가 몇갠지에 따라
				BM::GUID const kSummonGuid = pkPlayer->GetSummonUnit(i);
				CUnit* pkSummonedUnit = GetUnit(kSummonGuid);
				if(pkSummonedUnit
					&& pkSummonedUnit->IsUnitType(UT_SUMMONED)
					&& 0 == pkSummonedUnit->GetAbil( AT_CREATE_SUMMONED_IGNORE_SUMMON_EFFECT )	// 소환체가 제외 하는 어빌이 없으면
					)
				{
					kContClassNo.insert( pkSummonedUnit->GetAbil(AT_CLASS) );
				}
			}
			int const iBaseEffectNo = pkPlayer->GetAbil(AT_SUMMON_EFFECT_BASE_EFFECT_NO);
			if( 0 < iBaseEffectNo )
			{// 자신에게 걸어야 하는 이펙트가 있다
				pkPlayer->DeleteEffect(iBaseEffectNo, true);
				if( 0 < kContClassNo.size() )
				{
					SActArg kArg;
					PgGroundUtil::SetActArgGround(kArg, this);
					// 이펙트의 최대 레벨을 구해두고
					int iTemp = pkPlayer->GetAbil( AT_SUMMON_EFFECT_MAX_EFFECT_CNT );
					int const iMax = 0 < iTemp ? iTemp : 1;								// 안적어두었으면 최대 레벨은 1
					// 소환체 종류가 얼마나 되는지 확인 해서 이펙트 레벨을 구하고
					int iLevel = static_cast<int>(kContClassNo.size()) - 1; 
					// 최대 레벨을 넘지 않는지 검증한 다음
					iLevel = std::min( iMax, iLevel);
					int iEffectNo = iBaseEffectNo + iLevel;
					// 알맞은 이펙트 번호를 걸어 준다
					pkPlayer->AddEffect(iEffectNo, 0, &kArg, pkPlayer);
				}
			}
		}
	}
}