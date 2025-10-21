#include "stdafx.h"
#include "PgCartMissionGround.h"
#include "AI/PgAIAction.h"
#include "Lohengrin/VariableContainer.h"

void PgCartMissionGround::SyncCartPosition(PgObjectUnit* pkObject)
{
	static int iActionId = -1;
	if (iActionId < 0) // do cache
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		iActionId = kSkillDefMgr.GetSkillNoFromActionName(L"a_serverobject_move");
	}

	SActionInfo kAction;
	kAction.guidPilot = pkObject->GetID();
	kAction.ptPos = pkObject->GetPos();
	kAction.bIsDown = true;
	kAction.byDirection = 0;
	kAction.iUnitState = pkObject->GetState();
	kAction.dwTimeStamp = GetTimeStamp();
	kAction.iActionID = iActionId;

	BM::Stream kMPacket(PT_M_C_NFY_ACTION2);
	kAction.SendToClient(kMPacket);
	kMPacket.Push(BM::GetTime32());
	kMPacket.Push(pkObject->GoalPos());
	kMPacket.Push((BYTE)0);
	kMPacket.Push((bool)false);
	SendToArea( &kMPacket, pkObject->LastAreaIndex(), BM::GUID::NullData(), SYNC_TYPE_RECV_ADD, E_SENDTYPE_NONE );
}

void PgCartMissionGround::SpawnMonsterAround(PgObjectUnit* pkObject)
{
	CONT_DEF_MONSTER_BAG const *pkContMonsterBag;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pkContMonsterBagElement;
	CONT_DEF_CART_MISSION_MONSTER const * pDefCartMissionMonster;
	g_kTblDataMgr.GetContDef(pkContMonsterBag);
	g_kTblDataMgr.GetContDef(pkContMonsterBagElement);
	g_kTblDataMgr.GetContDef(pDefCartMissionMonster);
	if(NULL == pkContMonsterBag || NULL == pkContMonsterBagElement || NULL == pDefCartMissionMonster)
	{
		return /*false*/;
	}

	CONT_DEF_CART_MISSION_MONSTER::key_type kKey(GetGroundNo(), 0, m_kMonSpawnNo);
	CONT_DEF_CART_MISSION_MONSTER::const_iterator kFind = pDefCartMissionMonster->find(kKey);
	if (pDefCartMissionMonster->end() == kFind)
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"Can't find monster spawn[" << kKey.kPriKey << "," << kKey.kSecKey << "," << kKey.kTrdKey << L"]");
		return /*false*/;
	}

	CONT_DEF_MONSTER_BAG::const_iterator iter = pkContMonsterBag->find(kFind->second.iMonBagNo);
	if(iter == pkContMonsterBag->end())
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"CONT_DEF_MONSTER_BAG isn't exist [" << kFind->second.iMonBagNo << L"]");
		return /*false*/;
	}

	CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator moniter = pkContMonsterBagElement->find((*iter).second.iElementNo);
	if(moniter == pkContMonsterBagElement->end())
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"CONT_DEF_MONSTER_BAG_ELEMENTS isn't exist [" << (*iter).second.iElementNo << L"]");
		return /*false*/;
	}

	BM::GUID kMonsterGuid;
	TBL_DEF_MAP_REGEN_POINT kRegenInfo;
	kRegenInfo.iMapNo = GetGroundNo();
	kRegenInfo.iTunningNo = (*iter).second.iTunningNo_Min;

	/// https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
	const static int SPAWN_RADIUS = 125;
	const static float R = SPAWN_RADIUS * NiSqrt((float)BM::Rand_Unit());
	float theta;

	POINT3 kPoint = pkObject->GetPos() + POINT3(100, 0, 0);
	size_t kIdx;
	for (int i = 0; i < 5; ++i)
	{
		if(!RouletteRate((*iter).second.iRateNo,kIdx,MAX_SUCCESS_RATE_ARRAY))
		{
			continue;
		}

		theta = (float)BM::Rand_Unit() * 2 * NI_PI;
		kRegenInfo.pt3Pos = kPoint + POINT3(R * NiCos(theta), R * NiSin(theta), 0.);
		InsertMonster( kRegenInfo, (*moniter).second.aElement[kIdx], kMonsterGuid );
	}

	kPoint = pkObject->GetPos() + POINT3(-100, 0, 0);
	for (int i = 0; i < 5; ++i)
	{
		if(!RouletteRate((*iter).second.iRateNo,kIdx,MAX_SUCCESS_RATE_ARRAY))
		{
			continue;
		}

		theta = (float)BM::Rand_Unit() * 2 * NI_PI;
		kRegenInfo.pt3Pos = kPoint + POINT3(R * NiCos(theta), R * NiSin(theta), 0.);
		InsertMonster( kRegenInfo, (*moniter).second.aElement[kIdx], kMonsterGuid );
	}
}

void PgCartMissionGround::OnTick5s()
{
    BM::CAutoMutex Lock(m_kRscMutex);
	if (m_eState != INDUN_STATE_PLAY)
	{
		PgIndun::OnTick5s();
		return;
	}

	PgObjectUnit* pkObjectUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kUnitItor;
	PgObjectMgr::GetFirstUnit(UT_OBJECT, kUnitItor);
	while((pkObjectUnit = dynamic_cast<PgObjectUnit*> (PgObjectMgr::GetNextUnit(UT_OBJECT, kUnitItor))) != NULL)
	{
		SyncCartPosition(pkObjectUnit);
	}

	PgIndun::OnTick5s();
}

void PgCartMissionGround::OnTick100ms()
{
    BM::CAutoMutex Lock(m_kRscMutex);
	if (m_eState == INDUN_STATE_FAIL || m_eState == INDUN_STATE_RESULT)
	{
		DWORD const dwNow = BM::GetTime32();
		if( dwNow - m_FailTime > 3000)
		{
			SetState(INDUN_STATE_CLOSE);
		}
	}
	if (m_eState != INDUN_STATE_PLAY)
	{
		PgIndun::OnTick100ms();
		return;
	}

	DWORD const dwNow = BM::GetTime32();
	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_100MS, dwNow, dwkElapsed, false);

	PgObjectUnit* pkObjectUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kUnitItor;
	PgObjectMgr::GetFirstUnit(UT_OBJECT, kUnitItor);
	while((pkObjectUnit = dynamic_cast<PgObjectUnit*> (PgObjectMgr::GetNextUnit(UT_OBJECT, kUnitItor))) != NULL)
	{
		EUnitState eUnitState = pkObjectUnit->GetState();
		if (eUnitState != US_MOVE)
		{
			continue;
		}

		POINT3 kPos = pkObjectUnit->GetPos();
		const POINT3 kOriginalPos = kPos;
		const POINT3 kGoalPos = pkObjectUnit->GoalPos();
		MovingSomething(kPos, kGoalPos, pkObjectUnit->GetAbil(AT_MOVESPEED), dwkElapsed);

		pkObjectUnit->SetPos(kPos);

		if (POINT3::SqrDistance(kGoalPos, kPos) <= 0.447/*0.2 ^ 2*/)
		{
			PgWorldEventMgr::DoUnitGoalTarget(GetGroundNo(), pkObjectUnit);
			SyncCartPosition(pkObjectUnit);
			pkObjectUnit->SetState(US_IDLE);

			if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
			{
				INFO_LOG(BM::LOG_LV7, "Sync cart position");
			}
		}

		if ( dwNow >= m_kMonsterSpawnTime )
		{
			SpawnMonsterAround(pkObjectUnit);
			++m_kMonSpawnNo;

			CONT_DEF_CART_MISSION_MONSTER const * pDefCartMissionMonster;
			g_kTblDataMgr.GetContDef(pDefCartMissionMonster);

			CONT_DEF_CART_MISSION_MONSTER::key_type kKey(GetGroundNo(), 0, m_kMonSpawnNo);
			if (pDefCartMissionMonster != NULL && pDefCartMissionMonster->end() == pDefCartMissionMonster->find(kKey))
			{
				m_kMonSpawnNo = 0; // Reset Spawn Point
			}
			m_kMonsterSpawnTime = dwNow + m_kMonsterSpawnDelay;
		}
	}

	PgIndun::OnTick100ms();
}

void PgCartMissionGround::SetState(EIndunState const eState, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( eState == m_eState)
	{
		return;
	}

	if( bChangeOnlyState )
	{
		return;
	}

	if ( eState == INDUN_STATE_PLAY )
	{
		m_kMonSpawnNo = 0;
		m_kMonCount = 0;

		CONT_OBJECT_MGR_UNIT::iterator kItor;

		PgPlayer* pkPlayer = NULL;
		CUnit* pkSummoned = NULL;
		PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
		while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
		{
			pkPlayer->SetAbil(AT_TEAM, TEAM_RED, true, true);

			VEC_SUMMONUNIT const& kContSummonUnit = pkPlayer->GetSummonUnit();
			for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
			{
				pkSummoned = GetUnit((*c_it).kGuid);
				if(pkSummoned)
				{
					pkSummoned->SetAbil(AT_TEAM, TEAM_RED, true, true);
				}
			}
		}

		PgObjectUnit* pkObjectUnit = NULL;
		PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
		while((pkObjectUnit = dynamic_cast<PgObjectUnit*> (PgObjectMgr::GetNextUnit(UT_OBJECT, kItor))) != NULL)
		{
			pkObjectUnit->SetAbil(AT_MOVESPEED, 20, true, true);
			pkObjectUnit->SetAbil(AT_TEAM, TEAM_RED, true, true);
			SyncCartPosition(pkObjectUnit);
		}

		g_kVariableContainer.Get( EVar_Kind_Mission, EVar_Mission_EscortMonSpawnDelay, m_kMonsterSpawnDelay );
		m_kMonsterSpawnTime = (m_kMonsterSpawnDelay / 2) + BM::GetTime32();
	}
	else if ( eState == INDUN_STATE_RESULT )
	{
		m_FailTime = BM::GetTime32();
		CONT_OBJECT_MGR_UNIT::iterator kItor;
		PgObjectUnit* pkObjectUnit = NULL;
		PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
		while((pkObjectUnit = dynamic_cast<PgObjectUnit*> (PgObjectMgr::GetNextUnit(UT_OBJECT, kItor))) != NULL)
		{
			pkObjectUnit->SetState(US_IDLE);
			SyncCartPosition(pkObjectUnit);
		}
	}

	PgIndun::SetState(eState, bAutoChange, bChangeOnlyState);
}

HRESULT PgCartMissionGround::InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo,
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller,
		bool DropAllItem, int EnchantGradeNo,
		ECREATE_HP_TYPE const Create_HP_Type)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	HRESULT const hRet = PgGround::InsertMonster(GenInfo, MonNo, OutGuid, Caller, DropAllItem, EnchantGradeNo, Create_HP_Type);
	if( !SUCCEEDED(hRet) )
	{
		return hRet;
	}

	CUnit * pkMonster = GetUnit(OutGuid);
	pkMonster->SetAbil(AT_MONSTER_FIRST_TARGET, static_cast<int>(UT_OBJECT));
	pkMonster->SetAbil(AT_AI_TARGETTING_RATE, 10000/*%*/);
	pkMonster->SetAbil(AT_USENOT_SMALLAREA, 1);
	pkMonster->SetAbil(AT_DETECT_RANGE, 5000);
	pkMonster->SetAbil(AT_TEAM, TEAM_BLUE, true, true);
	pkMonster->SetAbil(AT_MOVESPEED, (int)(pkMonster->GetAbil(AT_MOVESPEED) * 2.));
	++m_kMonCount;
	return hRet;
}

void PgCartMissionGround::RecvUnitDie(CUnit *pkUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if( !pkUnit )
	{
		return;
	}

	PgGround::RecvUnitDie(pkUnit);
	if (pkUnit->UnitType() == UT_OBJECT)
	{
		m_FailTime = BM::GetTime32();
		SetState(INDUN_STATE_FAIL);
	}
	else if (pkUnit->UnitType() == UT_MONSTER)
	{
		--m_kMonCount;
	}
}

void PgCartMissionGround::DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent)
{
	switch(rkAction.TargetType())
	{
	case WorldEventTarget::WETT_UnitGoalTrigger:
		{
			POINT3 kTargetPos;
			if( S_OK != PgGround::FindTriggerLoc(MB(rkAction.ActionValue()), kTargetPos, true) )
			{
				INFO_LOG(BM::LOG_LV1, __FL__ << "[UNIT_GOAL_TARGET] Can't find object in gas [" << rkAction.ActionValue() << "]");
				return;
			}

			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_OBJECT, kItor);
			PgObjectUnit* pkObjectUnit = NULL;
			while((pkObjectUnit = dynamic_cast<PgObjectUnit*> (PgObjectMgr::GetNextUnit(UT_OBJECT, kItor))) != NULL)
			{
				pkObjectUnit->GoalPos(kTargetPos);
				pkObjectUnit->SetState(US_MOVE);
			}
			return;
		}break;
	case WorldEventTarget::WETT_IndunState:
		{
			switch( rkAction.ActionType() )
			{
			case WorldEventAction::AT_Status:
				{
					SetState(static_cast< EIndunState >(rkAction.ActionValueAsInt()));
					return;
				}break;
			default:
				{
					INFO_LOG(BM::LOG_LV1, __FL__ << "Use INDUN_STATE with EVENT=STATUS");
				} break;
			}
		}break;
	}
	PgGround::DoWorldEventAction(iGroundNo, rkAction, rkWorldEvent);
}