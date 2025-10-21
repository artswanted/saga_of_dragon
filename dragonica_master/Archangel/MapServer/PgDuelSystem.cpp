
#include "stdafx.h"
#include "PgGround.h"
#include "Lohengrin/ActArg.h"
#include "PgPVPEffectSelector.h"
#include "Variant/PgPlayer.h"
#include "PgDuelSystem.h"



PgDuel::PgDuel(unsigned int iID, PgGround* pkGround, const BM::GUID& rkUser1, const BM::GUID& rkUser2) :
	m_iID(iID), m_pkGround(pkGround), m_eState(ESTATE_READY), m_kIDPlayer1(rkUser1), m_kIDPlayer2(rkUser2), m_wGameTime(m_wInitGameTime), m_kIDFlag(),
		m_wAreaTime1(m_wInitAreaTime), m_wAreaTime2(m_wInitAreaTime), m_wPrepTime(m_wInitPrepTime), m_eResult(ERES_NOT_FINISH), m_eResCauseBy(ERES_CAUSE_KO), m_bAreaOut1(false), m_bAreaOut2(false)
{ //iID에는 0이 들어가면 안된다 어빌이 0으로 셋팅되면 타겟팅 되지 않으므로...
	if(!m_pkGround)
	{
		throw InvalidGround();
	}
	if(rkUser1.IsNull() || rkUser2.IsNull())
	{
		throw InvalidPlayer();
	}
	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
	if(pkPlayer1 && pkPlayer2)
	{
		if(!pkPlayer1->IsAlive() || !pkPlayer2->IsAlive())
		{
			throw PlayerDead();
		}
	}

	m_kIDFlag.Generate();
	CreateFlag();
//	pkPlayer1->SendWarnMessage(201201, EL_Normal); //결투가 시작되었습니다.
//	pkPlayer2->SendWarnMessage(201201, EL_Normal);

	m_stPlayerName1 = pkPlayer1->Name();
	m_stPlayerName2 = pkPlayer2->Name();
}

PgDuel::~PgDuel()
{
	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
	RestorePlayer(pkPlayer1);
	RestorePlayer(pkPlayer2);
}

bool PgDuel::IsExistPlayer(const BM::GUID& rkID1, const BM::GUID& rkID2)
{
	return (rkID1 == m_kIDPlayer1 || rkID1 == m_kIDPlayer2 || rkID2 == m_kIDPlayer1 || rkID2 == m_kIDPlayer2);
}


void PgDuel::Start(void)
{
	m_eState = ESTATE_BATTLE;

	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
	if(!pkPlayer1 || !pkPlayer2)
	{
		End();
	}

	SetDuel(pkPlayer1);
	SetDuel(pkPlayer2);

	BM::Stream kPacket(PT_M_C_NFY_DUEL_BEGIN); //결투 시작되는 타이밍을 유저들에게 브로드캐스팅
	kPacket.Push(m_stPlayerName1);
	kPacket.Push(m_stPlayerName2);
	m_pkGround->Broadcast(kPacket);

}


void PgDuel::End(void)
{
	if(m_eState == ESTATE_FINISH)
	{ //이미 종료 상태라면 패스
		return;
	}

	if( m_wGameTime >= (m_wInitGameTime - 30))
	{ //시작 후 30초 이내에 게임이 끝났다면
		m_eResCauseBy = ERES_CAUSE_TIME_LIMIT_RULE;
		m_eResult = ERES_DRAW;
	}

	m_eState = ESTATE_FINISH;
	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);

	PgPlayer* pkWinner = NULL;
	PgPlayer* pkLooser = NULL;
	if(pkPlayer1)
	{
		if(m_eResult == ERES_P1_WIN)
		{
			pkPlayer1->SetAbil(AT_DUEL_WINS, pkPlayer1->GetAbil(AT_DUEL_WINS) + 1, true, true); //이겼다면 연승 타이틀용 카운트를 증가
			pkWinner = pkPlayer1;
			pkLooser = pkPlayer2;
		}
		else
		{
			pkPlayer1->SetAbil(AT_DUEL_WINS, 0, true, true); //지거나 비겼다면 연승 카운트 초기화
		}
	}
	if(pkPlayer2)
	{
		if(m_eResult == ERES_P2_WIN)
		{
			pkPlayer2->SetAbil(AT_DUEL_WINS, pkPlayer2->GetAbil(AT_DUEL_WINS) + 1, true, true); //이겼다면 연승 타이틀용 카운트를 증가
			pkWinner = pkPlayer2;
			pkLooser = pkPlayer1;
		}
		else
		{
			pkPlayer2->SetAbil(AT_DUEL_WINS, 0, true, true); //지거나 비겼다면 연승 카운트 초기화
		}
	}

	BM::Stream kPacket(PT_M_C_NFY_DUEL_FINISH); //결투 종료되는 타이밍을 유저들에게 브로드캐스팅
	kPacket.Push(static_cast<BYTE>(m_eResult));
	kPacket.Push(static_cast<BYTE>(m_eResCauseBy));
	kPacket.Push(m_kIDPlayer1);
	kPacket.Push(m_kIDPlayer2);
	if(pkPlayer1)
	{
		RestorePlayer(pkPlayer1);
	}
	if(pkPlayer2)
	{
		RestorePlayer(pkPlayer2);
	}
	m_pkGround->Broadcast(kPacket);


	if(m_pkGround)
	{
		CUnit* pkFlag = m_pkGround->GetUnit(m_kIDFlag);
		if(pkFlag)
		{
			m_pkGround->ReleaseUnit(pkFlag); //깃발 유닛 제거
		}
	}

	// Stat Track
	if (pkLooser && pkWinner)
	{
		BM::Stream kNfyGndPacket(PT_U_G_NFY_STAT_TRACK_INFO);
		kNfyGndPacket.Push(pkWinner->GetID());
		pkLooser->VNotify(&kNfyGndPacket);
	}
}


void PgDuel::Update(void)
{
	if(!m_pkGround)
	{
		End();
		return;
	}
	switch(m_eState)
	{
	case ESTATE_READY:
		{
			PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
			PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
			if(!pkPlayer1 || !pkPlayer2 || !pkPlayer1->IsAlive() || !pkPlayer2->IsAlive())
			{
				End();
				return;
			}

			if(m_wPrepTime == m_wInitPrepTime)
			{
				BM::Stream kPacket(PT_M_C_NFY_DUEL_START_COUNT); //클라에게 카운트를 시작하라는 통보
				kPacket.Push(static_cast<BYTE>(m_wInitPrepTime));
				pkPlayer1->Send(kPacket, E_SENDTYPE_SELF);
				pkPlayer2->Send(kPacket, E_SENDTYPE_SELF);
			}

			if(m_wPrepTime > 0)
			{
				m_wPrepTime--;
			}
			else
			{
				Start();
			}
		}break;
	case ESTATE_BATTLE:
		{
			CUnit* pkFlag = m_pkGround->GetUnit(m_kIDFlag);
			m_eResult = DecideWinner();
			if(m_eResult != ERES_NOT_FINISH)
			{
				End();
				return;
			}
		}break;
	}



}


PgPlayer* PgDuel::GetPlayer(BM::GUID const& rkID)
{
	if(!m_pkGround)
	{
		return NULL;
		//throw InvalidGround();
	}
	PgPlayer* pkRet = m_pkGround->GetUser(rkID);
	if(!pkRet || !pkRet->IsUnitType(UT_PLAYER))
	{
		return NULL;
		//throw InvalidPlayer();
	}

	return pkRet;
}

CUnit* PgDuel::GetFlag(void)
{
	if(!m_pkGround)
	{
		return NULL;
	}

	return m_pkGround->GetUnit(m_kIDFlag);
}

void PgDuel::SetDuel(PgPlayer* pkPlayer)
{
	if(!pkPlayer)
	{
		return;
	}


	pkPlayer->SetAbil(AT_DUEL, m_iID, true, true);
	pkPlayer->SetAbil(AT_DUEL_DIED_BY, 0);
	pkPlayer->SetAbil(AT_MEMORIZED_HP, pkPlayer->GetAbil(AT_HP)); //처음 HP를 저장
	pkPlayer->SetAbil(AT_MEMORIZED_MP, pkPlayer->GetAbil(AT_MP)); //처음 MP를 저장
	//pkPlayer1->SetAbil(AT_MAX_HP, pkPlayer1->GetAbil(AT_MAX_HP) * 3, true, true);
	//pkPlayer1->SetAbil(AT_MAX_MP, pkPlayer1->GetAbil(AT_MAX_MP) * 3, true, true);
	//pkPlayer2->SetAbil(AT_MAX_HP, pkPlayer2->GetAbil(AT_MAX_HP) * 3, true, true);
	//pkPlayer2->SetAbil(AT_MAX_MP, pkPlayer2->GetAbil(AT_MAX_MP) * 3, true, true);
/*
	SEffectCreateInfo kCreate; //PVP 버프 (피통 3배). 현재는 그냥 결투장 맵이펙트를 쓴다.
	kCreate.eType = EFFECT_TYPE_GROUND;
	kCreate.iEffectNum = 13001;
	kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
	pkPlayer->AddEffect( kCreate );

	pkPlayer->SetAbil( AT_HP, pkPlayer->GetAbil(AT_C_MAX_HP)); //피통 뻥튀기 후 풀피로 만들어준다.
	pkPlayer->SetAbil( AT_MP, pkPlayer->GetAbil(AT_C_MAX_MP));
	WORD wType[2] = { AT_HP, AT_MP };
	pkPlayer->SendAbiles(wType,2,E_SENDTYPE_BROADALL);
*/
	//----------PVP 밸런스 적용(클래스별 PVP 차별적 이펙트) 처리------
	//----------위의 주석 처리된 코드를 아래와 같이 변경--------------
	SActArg kArg;
	PgGroundUtil::SetActArgGround(kArg, m_pkGround);
	g_kPVPEffectSlector.AddEffect(pkPlayer, &kArg);

	pkPlayer->SetAbil( AT_HP, pkPlayer->GetAbil(AT_C_MAX_HP)); //피통 뻥튀기 후 풀피로 만들어준다.
	pkPlayer->SetAbil( AT_MP, pkPlayer->GetAbil(AT_C_MAX_MP));
	WORD wType[2] = { AT_HP, AT_MP };
	pkPlayer->SendAbiles(wType,2,E_SENDTYPE_BROADALL);
	//-------------------------------------------------------------------

	VEC_SUMMONUNIT kContSummonUnit = pkPlayer->GetSummonUnit(); //소환체들에게도 결투 어빌을 셋팅해준다.
	CUnit * pkChild = NULL;
	for(VEC_SUMMONUNIT::const_iterator c_it = kContSummonUnit.begin(); c_it != kContSummonUnit.end(); ++c_it)
	{
		pkChild = m_pkGround->GetUnit((*c_it).kGuid);
		if(pkChild && (pkChild->IsUnitType(UT_SUMMONED) || pkChild->IsUnitType(UT_ENTITY)) )
		{
			pkChild->SetAbil(AT_DUEL, m_iID, true, true);
		}
	}
}

void PgDuel::RestorePlayer(PgPlayer* pkPlayer)
{
	if(!pkPlayer)
	{
		return;
	}
	//시작전의 PC 상태로 복구해주자(어빌, HP, MP, ...등등)
	/*
	if(pkPlayer->FindEffect(13001))
	{
		pkPlayer->DeleteEffect( 13001 ); //PVP 버프 제거. 피통을 원래대로 복구
	}
	*/
	//PVP 밸런스 수정. 위 코드를 아래 코드로 변경
	g_kPVPEffectSlector.DelEffect(pkPlayer);


	//HP를 원래대로 복구

	if(pkPlayer->IsDead()) //PVP가 아닌 외부 요인으로 사망했을 경우는 살려주지 않는다.
	{
//		pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
	}
	if(pkPlayer->GetAbil(AT_MEMORIZED_HP) && pkPlayer->GetAbil(AT_MEMORIZED_MP))
	{
		pkPlayer->SetAbil(AT_HP, pkPlayer->GetAbil(AT_MEMORIZED_HP), true, true); //시작전 HP로 복구
		pkPlayer->SetAbil(AT_MP, pkPlayer->GetAbil(AT_MEMORIZED_MP), true, true); //시작전 MP로 복구
		pkPlayer->SetAbil(AT_MEMORIZED_HP, 0);
		pkPlayer->SetAbil(AT_MEMORIZED_MP, 0);
	}

	if(pkPlayer->GetAbil(AT_DUEL) > 0)
	{
		pkPlayer->SetAbil(AT_DUEL, 0, true, true);
	}

	{ //결투 종료 후 플레이어의 디버프 모두 삭제
		PgUnitEffectMgr& rkEffectMgr = pkPlayer->GetEffectMgr();
		ContEffectItor effect_iter;
		rkEffectMgr.GetFirstEffect(effect_iter);
		CEffect* pkEffect = NULL;
		while( (pkEffect = rkEffectMgr.GetNextEffect(effect_iter)) != NULL )
		{
			if( pkEffect != NULL && pkEffect->GetType() == EFFECT_TYPE_CURSED )
			{
				pkPlayer->DeleteEffect(pkEffect->GetEffectNo());
			}
		}
	}

	VEC_SUMMONUNIT kContSummonUnit = pkPlayer->GetSummonUnit(); //소환체들에게도 결투 어빌을 해제해준다.
	CUnit * pkChild = NULL;
	for(VEC_SUMMONUNIT::const_iterator c_it = kContSummonUnit.begin(); c_it != kContSummonUnit.end(); ++c_it)
	{
		pkChild = m_pkGround->GetUnit((*c_it).kGuid);
		if(pkChild && (pkChild->IsUnitType(UT_SUMMONED) || pkChild->IsUnitType(UT_ENTITY)) )
		{
			pkChild->SetAbil(AT_DUEL, 0, true, true);
		}
	}


}

void PgDuel::CreateFlag(void)
{
	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
	if(!pkPlayer1 || !pkPlayer2)
	{
		return;
	}

	POINT3 ptCenter = pkPlayer1->GetPos() + ((pkPlayer2->GetPos() - pkPlayer1->GetPos()) / 2);
	NxRaycastHit kHit;
	NxRay kRay( NxVec3(ptCenter.x, ptCenter.y, ptCenter.z+20.0f ), NxVec3(0, 0, -1.0f) );
	NxShape *pkHitShape = m_pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		ptCenter.z = kHit.worldImpact.z;
	}

	CUnit* pkFlag = m_pkGround->CreateCustomUnit(100000000, ptCenter, DIR_DOWN);
	if(pkFlag)
	{
		m_kIDFlag = pkFlag->GetID();
	}
}

bool PgDuel::CheckOutOfArea(void)
{
	CUnit* const pkUnitFlag = GetFlag();
	if(!pkUnitFlag)
	{
		return false;
	}
	PgPlayer* const pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* const pkPlayer2 = GetPlayer(m_kIDPlayer2);
	if(!pkPlayer1 || !pkPlayer2)
	{
		return false;
	}

	POINT3 ptFlag(pkUnitFlag->GetPos().x, pkUnitFlag->GetPos().y, 0);
	POINT3 ptPlayer1(pkPlayer1->GetPos().x, pkPlayer1->GetPos().y, 0);
	POINT3 ptPlayer2(pkPlayer2->GetPos().x, pkPlayer2->GetPos().y, 0);

	float fDistance = 0;
	//1번유저
	fDistance = POINT3::Distance(ptFlag, ptPlayer1);
	if(fDistance > static_cast<float>(m_wOutDist))
	{
		if(!m_bAreaOut1) //통보
		{
			m_bAreaOut1 = true;
			m_wAreaTime1 = m_wInitAreaTime;
			BM::Stream kPacket(PT_M_C_NFY_DUEL_OUT_COUNT); //필드범위를 벗어났당! 제한시간안에 안들어가면 넌 루저다
			kPacket.Push(static_cast<BYTE>(m_wAreaTime1));
			pkPlayer1->Send(kPacket, E_SENDTYPE_SELF);
		}
		else
		{
			m_wAreaTime1--;
		}
	}
	else if(m_wAreaTime1 != m_wInitAreaTime)
	{ //나갔다가 다시 들어갔다면...
		m_bAreaOut1 = false;
		BM::Stream kPacket(PT_M_C_NFY_DUEL_CANCEL_OUT_COUNT); //다시 들어갔다. 카운트다운 취소해!
		pkPlayer1->Send(kPacket, E_SENDTYPE_SELF);
	}

	//2번유저
	fDistance = POINT3::Distance(ptFlag, ptPlayer2);
	if(fDistance > static_cast<float>(m_wOutDist))
	{
		if(!m_bAreaOut2)
		{
			m_bAreaOut2 = true;
			m_wAreaTime2 = m_wInitAreaTime;
			BM::Stream kPacket(PT_M_C_NFY_DUEL_OUT_COUNT); //필드범위를 벗어났당! 제한시간안에 안들어가면 넌 루저다
			kPacket.Push(static_cast<BYTE>(m_wAreaTime2));
			pkPlayer2->Send(kPacket, E_SENDTYPE_SELF);
		}
		else
		{
			m_wAreaTime2--;
		}
	}
	else if(m_wAreaTime2 != m_wInitAreaTime)
	{
		m_bAreaOut2 = false;
		BM::Stream kPacket(PT_M_C_NFY_DUEL_CANCEL_OUT_COUNT); //다시 들어갔다. 카운트다운 취소해!
		pkPlayer2->Send(kPacket, E_SENDTYPE_SELF);
	}

	return (m_bAreaOut1 || m_bAreaOut2);
}


bool PgDuel::CheckTimeOut(void)
{
	if(m_wGameTime > 0)
	{
		m_wGameTime--;
	}

	if(m_wGameTime == m_wImpendTime)
	{ //클라에게 맞짱시간 종료 임박을 통보. 카운트 시작하거라
		PgPlayer* const pkPlayer1 = GetPlayer(m_kIDPlayer1);
		PgPlayer* const pkPlayer2 = GetPlayer(m_kIDPlayer2);
		if(!pkPlayer1 || !pkPlayer2)
		{
			return false;
		}

		BM::Stream kPacket(PT_M_C_NFY_DUEL_END_COUNT); 
		kPacket.Push(static_cast<BYTE>(m_wGameTime));
		pkPlayer1->Send(kPacket, E_SENDTYPE_SELF);
		pkPlayer2->Send(kPacket, E_SENDTYPE_SELF);
	}
	if(m_wGameTime <= 0)
	{
		return true;
	}
	return false;
}

PgDuel::EGAME_RESULT PgDuel::DecideWinner(void)
{
	PgPlayer* pkPlayer1 = GetPlayer(m_kIDPlayer1);
	PgPlayer* pkPlayer2 = GetPlayer(m_kIDPlayer2);
	if(!pkPlayer1 && !pkPlayer2) //STEP1: 접속이 드랍된 경우
	{
		m_eResCauseBy = ERES_CAUSE_ZONE_OUT;
		return ERES_DRAW;
	}
	else if(!pkPlayer1)
	{
		m_eResCauseBy = ERES_CAUSE_ZONE_OUT;
		return ERES_P2_WIN;
	}
	else if(!pkPlayer2)
	{
		m_eResCauseBy = ERES_CAUSE_ZONE_OUT;
		return ERES_P1_WIN;
	}

	if((pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_PLAYER || pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_SUMMONED ||
		pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_ENTITY || pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_SUB_PLAYER) &&
		(pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_PLAYER || pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_SUMMONED ||
		pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_ENTITY || pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_SUB_PLAYER)) //체력이 다해 KO승한 경우 (AT_DUEL_DIED_BY)
	{
		m_eResCauseBy = ERES_CAUSE_KO;
		return ERES_DRAW;
	}
	else if(pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_PLAYER || pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_SUMMONED ||
			pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_ENTITY || pkPlayer1->GetAbil(AT_DUEL_DIED_BY) == UT_SUB_PLAYER)
	{
		m_eResCauseBy = ERES_CAUSE_KO;
		return ERES_P2_WIN;
	}
	else if(pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_PLAYER || pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_SUMMONED ||
			pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_ENTITY || pkPlayer2->GetAbil(AT_DUEL_DIED_BY) == UT_SUB_PLAYER)
	{
		m_eResCauseBy = ERES_CAUSE_KO;
		return ERES_P1_WIN;
	}

	if(pkPlayer1->GetAbil(AT_HP) == 0 && pkPlayer2->GetAbil(AT_HP) == 0) //제3자에게 진짜로 죽은 경우 (AT_DUEL_DIED_BY)
	{
		m_eResCauseBy = ERES_CAUSE_DIE;
		return ERES_DRAW;
	}
	else if(pkPlayer1->GetAbil(AT_HP) == 0)
	{
		m_eResCauseBy = ERES_CAUSE_DIE;
		return ERES_P2_WIN;
	}
	else if(pkPlayer2->GetAbil(AT_HP) == 0)
	{
		m_eResCauseBy = ERES_CAUSE_DIE;
		return ERES_P1_WIN;
	}

	CheckOutOfArea();
	if(m_wAreaTime1 == 0 && m_wAreaTime2 == 0) //STEP2: 장외 판정승
	{
		m_eResCauseBy = ERES_CAUSE_RING_OUT;
		return ERES_DRAW;
	}
	else if(m_wAreaTime1 == 0)
	{
		m_eResCauseBy = ERES_CAUSE_RING_OUT;
		return ERES_P2_WIN;
	}
	else if(m_wAreaTime2 == 0)
	{
		m_eResCauseBy = ERES_CAUSE_RING_OUT;
		return ERES_P1_WIN;
	}

	if(CheckTimeOut()) //STEP3: 시간이 다 되어 피통이 큰쪽을 승자로 간주
	{
		float iRateHp1 = static_cast<float>(pkPlayer1->GetAbil(AT_HP)) / pkPlayer1->GetAbil(AT_C_MAX_HP);
		float iRateHp2 = static_cast<float>(pkPlayer2->GetAbil(AT_HP)) / pkPlayer2->GetAbil(AT_C_MAX_HP);
		if(iRateHp1 == iRateHp2)
		{
			return ERES_DRAW;
		}
		else if(iRateHp1 < iRateHp2)
		{
			return ERES_P2_WIN;
		}
		else
		{
			return ERES_P1_WIN;
		}
	}

	return ERES_NOT_FINISH;
}


bool PgDuelMgr::Create(PgGround* pkGround, BM::GUID const & rkID1, BM::GUID const & rkID2)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(!pkGround || rkID1.IsNull() || rkID2.IsNull() )
	{
		return false;
	}


	try
	{
		unsigned int id = GenerateID();
		auto pairib = m_kContDuel.insert(std::make_pair(id, PgDuel(id, pkGround, rkID1, rkID2) )); //id는 순서대로 1부터 시작
	}
	catch(const PgDuel::Exception& e)
	{
		return false;
	}

	return true;
}

bool PgDuelMgr::FindExistPlayer(BM::GUID const& rkID1, BM::GUID const& rkID2)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_DUEL::iterator iterDuel = m_kContDuel.begin();
	for( ; iterDuel != m_kContDuel.end(); ++iterDuel)
	{
		if(iterDuel->second.IsExistPlayer(rkID1, rkID2))
		{
			return true;
		}
	}

	return false;
}

unsigned int PgDuelMgr::GenerateID(void)
{
	unsigned int iNum = 1;
	CONT_DUEL::const_iterator iterDuel = m_kContDuel.begin();
	for(; iterDuel != m_kContDuel.end(); ++iterDuel)
	{
		if(iterDuel->first != iNum)
		{
			break;
		}
		++iNum;
	}
	return iNum;
}

void PgDuelMgr::Update(void)
{
	BM::CAutoMutex kLock(m_kMutex);
	PgDuel::ESTATE eState;
	CONT_DUEL::iterator iterDuel = m_kContDuel.begin();
	try
	{
		while(iterDuel != m_kContDuel.end())
		{
			iterDuel->second.Update();
			eState = iterDuel->second.GetState();
			if(eState == PgDuel::ESTATE_FINISH)
			{
				m_kContDuel.erase(iterDuel++);
			}
			else
			{
				++iterDuel;
			}
		}
	}
	catch(PgDuel::Exception& e)
	{
		//.....예외 처리 후...
		m_kContDuel.erase(iterDuel); //삭제해주자
	}
}

