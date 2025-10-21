#include "stdafx.h"
#include "Variant/constant.h"
#include "PgAIActionManager.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

#include "PgAIActChaseEnemy.h"
#include "PgAIActFireSkill.h"
#include "PgAIActFireSkillDelay.h"
#include "PgAIActIdle.h"
#include "PgAIActOpening.h"
#include "PgAIActPatrol.h"
#include "PgAIActRetreat.h"
#include "PgAIActDmDelay.h"
#include "PgAIActBlowup.h"
#include "PgAIActCallHelp.h"
#include "PgAIActHide.h"
#include "PgAIActWaitCT.h"
#include "PgAIActDelay.h"
#include "PgAIActAround.h"
#include "PgAIActKeepDistance.h"
#include "PgAIActRoad.h"
#include "PgAIActFollowCaller.h"

const char* g_kAIActionName[EAI_ACTION_MAX] = {"NONE", "OPENING", "IDLE", "PATROL", "CHASE", "FIRE", "FIRE_DELAY", "RETREAT",
"DAMAGEDELAY", "BLOWUP", "CALLHELP", "HIDE", "WAIT_COOLTIME", "DELAY", "AROUND", "KEEP DISTANCE", "ROAD", "FOLLOW_CALLER"};

PgAIActionManager::PgAIActionManager(void)
{
}

PgAIActionManager::~PgAIActionManager(void)
{
}

bool PgAIActionManager::Init(PgGround* pkGround)
{
	if( !pkGround )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	//char chWayPointFile[MAX_PATH];
	//sprintf_s(chWayPointFile, MAX_PATH, "Script/Map/%d.way", pkGround->GroundKey().iGroundNo);
	//LoadWayPointFile(chWayPointFile);
	m_kWayPointMng.Init(*pkGround);
	return true;
}
bool PgAIActionManager::Reload(PgGround const& rkGround)
{
	m_kWayPointMng.Release();
	return m_kWayPointMng.Init(rkGround);
}

bool PgAIActionManager::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	if (eCurrentAction == eNextAction)
	{
		return true;
	}
	pkActArg->Set(ACTARG_WAYPOINTMNG, &m_kWayPointMng);
	pfnCanTransit pkFuncCanTransit = 0;

	switch(eCurrentAction)
	{
	case EAI_ACTION_OPENING: {pkFuncCanTransit = &PgAIActOpening::CanTransition;}break;
	case EAI_ACTION_IDLE: {pkFuncCanTransit = &PgAIActIdle::CanTransition;}break;
	case EAI_ACTION_PATROL: {pkFuncCanTransit = &PgAIActPatrol::CanTransition;}break;
	case EAI_ACTION_CHASEENEMY: {pkFuncCanTransit = &PgAIActChaseEnemy::CanTransition;}break;
	case EAI_ACTION_FIRESKILL: {pkFuncCanTransit = &PgAIActFireSkill::CanTransition;}break;
	case EAI_ACTION_FIRESKILL_DELAY: {pkFuncCanTransit = &PgAIActFireSkillDelay::CanTransition;}break;
	case EAI_ACTION_RETREAT: {pkFuncCanTransit = &PgAIActRetreat::CanTransition;}break;
	case EAI_ACTION_DAMAGEDELAY: {pkFuncCanTransit = &PgAIActDmDelay::CanTransition;}break;
	case EAI_ACTION_BLOWUP: {pkFuncCanTransit = &PgAIActBlowup::CanTransition;}break;
	case EAI_ACTION_CALLHELP: {pkFuncCanTransit = &PgAIActCallHelp::CanTransition;}break;
	case EAI_ACTION_HIDE: {pkFuncCanTransit = &PgAIActHide::CanTransition;}break;
	case EAI_ACTION_WAIT_COOLTIME: {pkFuncCanTransit = &PgAIWaitCoolTime::CanTransition;}break;
	case EAI_ACTION_DELAY: {pkFuncCanTransit = &PgAIActDelay::CanTransition;}break;
	case EAI_ACTION_AROUND : {pkFuncCanTransit = &PgAIActAround::CanTransition;}break;
	case EAI_ACTION_KEEP_DISTANCE : {pkFuncCanTransit = &PgAIActKeepDistance::CanTransition;}break;
    case EAI_ACTION_ROAD: {pkFuncCanTransit = &PgAIActRoad::CanTransition;}break;
	case EAI_ACTION_FOLLOW_CALLER: {pkFuncCanTransit = &PgAIActFollowCaller::CanTransition;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown Action ["<<eCurrentAction<<L"]");
		}break;
	}

	if(pkFuncCanTransit == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return (*pkFuncCanTransit)(pkUnit, eCurrentAction, iActionType, eNextAction, pkActArg);
}

void PgAIActionManager::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"UntiType["<<pkUnit->UnitType()<<L"], NewAction["<<eAction<<L"], PrevAction["<<ePrevAction<<L"], ElapsedTime["<<dwElapsedTime<<L"]");
#endif
	pkActArg->Set(ACTARG_WAYPOINTMNG, &m_kWayPointMng);

	pfnAct pkFuncOnEnter = 0;
	switch(eAction)
	{
	case EAI_ACTION_OPENING: {pkFuncOnEnter = &PgAIActOpening::OnEnter;}break;
	case EAI_ACTION_IDLE: {pkFuncOnEnter = &PgAIActIdle::OnEnter;}break;
	case EAI_ACTION_PATROL: {pkFuncOnEnter = &PgAIActPatrol::OnEnter;}break;
	case EAI_ACTION_CHASEENEMY: {pkFuncOnEnter = &PgAIActChaseEnemy::OnEnter;}break;
	case EAI_ACTION_FIRESKILL: {pkFuncOnEnter = &PgAIActFireSkill::OnEnter;}break;
	case EAI_ACTION_FIRESKILL_DELAY: {pkFuncOnEnter = &PgAIActFireSkillDelay::OnEnter;}break;
	case EAI_ACTION_RETREAT: {pkFuncOnEnter = &PgAIActRetreat::OnEnter;}break;
	case EAI_ACTION_DAMAGEDELAY: {pkFuncOnEnter = &PgAIActDmDelay::OnEnter;}break;
	case EAI_ACTION_BLOWUP: {pkFuncOnEnter = &PgAIActBlowup::OnEnter;}break;
	case EAI_ACTION_CALLHELP: {pkFuncOnEnter = &PgAIActCallHelp::OnEnter;}break;
	case EAI_ACTION_HIDE: {pkFuncOnEnter = &PgAIActHide::OnEnter;}break;
	case EAI_ACTION_WAIT_COOLTIME: {pkFuncOnEnter = &PgAIWaitCoolTime::OnEnter;}break;
	case EAI_ACTION_DELAY: {pkFuncOnEnter = &PgAIActDelay::OnEnter;}break;
	case EAI_ACTION_AROUND : {pkFuncOnEnter = &PgAIActAround::OnEnter;}break;
	case EAI_ACTION_KEEP_DISTANCE : {pkFuncOnEnter = &PgAIActKeepDistance::OnEnter;}break;
    case EAI_ACTION_ROAD: {pkFuncOnEnter = &PgAIActRoad::OnEnter;}break;
	case EAI_ACTION_FOLLOW_CALLER: {pkFuncOnEnter = &PgAIActFollowCaller::OnEnter;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown Action ["<<eAction<<L"]");
		}break;
	}

	if(pkFuncOnEnter == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFuncOnEnter is 0"));
		return;
	}

	(*pkFuncOnEnter)(pkUnit, eAction, iActionType, ePrevAction, dwElapsedTime, pkActArg);
}

void PgAIActionManager::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	pkActArg->Set(ACTARG_WAYPOINTMNG, &m_kWayPointMng);

	pfnAct pkFuncOnLeave = 0;
	switch(eAction)
	{
	case EAI_ACTION_OPENING: {pkFuncOnLeave = &PgAIActOpening::OnLeave;}break;
	case EAI_ACTION_IDLE: {pkFuncOnLeave = &PgAIActIdle::OnLeave;}break;
	case EAI_ACTION_PATROL: {pkFuncOnLeave = &PgAIActPatrol::OnLeave;}break;
	case EAI_ACTION_CHASEENEMY: {pkFuncOnLeave = &PgAIActChaseEnemy::OnLeave;}break;
	case EAI_ACTION_FIRESKILL: {pkFuncOnLeave = &PgAIActFireSkill::OnLeave;}break;
	case EAI_ACTION_FIRESKILL_DELAY: {pkFuncOnLeave = &PgAIActFireSkillDelay::OnLeave;}break;
	case EAI_ACTION_RETREAT: {pkFuncOnLeave = &PgAIActRetreat::OnLeave;}break;
	case EAI_ACTION_DAMAGEDELAY: {pkFuncOnLeave = &PgAIActDmDelay::OnLeave;}break;
	case EAI_ACTION_BLOWUP: {pkFuncOnLeave = &PgAIActBlowup::OnLeave;}break;
	case EAI_ACTION_CALLHELP: {pkFuncOnLeave = &PgAIActCallHelp::OnLeave;}break;
	case EAI_ACTION_HIDE: {pkFuncOnLeave = &PgAIActHide::OnLeave;}break;
	case EAI_ACTION_WAIT_COOLTIME: {pkFuncOnLeave = &PgAIWaitCoolTime::OnLeave;}break;
	case EAI_ACTION_DELAY: {pkFuncOnLeave = &PgAIActDelay::OnLeave;}break;
	case EAI_ACTION_AROUND : {pkFuncOnLeave = &PgAIActAround::OnLeave;}break;
	case EAI_ACTION_KEEP_DISTANCE : {pkFuncOnLeave = &PgAIActKeepDistance::OnLeave;}break;
    case EAI_ACTION_ROAD: {pkFuncOnLeave = &PgAIActRoad::OnLeave;}break;
	case EAI_ACTION_FOLLOW_CALLER: {pkFuncOnLeave = &PgAIActFollowCaller::OnLeave;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown Action ["<<eAction<<L"]");
		}break;
	}

	if(pkFuncOnLeave == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFuncOnLeave is 0"));
		return;
	}

	(*pkFuncOnLeave)(pkUnit, eAction, iActionType, eNextAction, dwElapsedTime, pkActArg);
}

void PgAIActionManager::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	pkActArg->Set(ACTARG_WAYPOINTMNG, &m_kWayPointMng);

	pfnDoAct pkFuncDoAction = 0;
	switch(eAction)
	{
	case EAI_ACTION_OPENING: {pkFuncDoAction = &PgAIActOpening::DoAction;}break;
	case EAI_ACTION_IDLE: {pkFuncDoAction = &PgAIActIdle::DoAction;}break;
	case EAI_ACTION_PATROL: {pkFuncDoAction = &PgAIActPatrol::DoAction;}break;
	case EAI_ACTION_CHASEENEMY: {pkFuncDoAction = &PgAIActChaseEnemy::DoAction;}break;
	case EAI_ACTION_FIRESKILL: {pkFuncDoAction = &PgAIActFireSkill::DoAction;}break;
	case EAI_ACTION_FIRESKILL_DELAY: {pkFuncDoAction = &PgAIActFireSkillDelay::DoAction;}break;
	case EAI_ACTION_RETREAT: {pkFuncDoAction = &PgAIActRetreat::DoAction;}break;
	case EAI_ACTION_DAMAGEDELAY: {pkFuncDoAction = &PgAIActDmDelay::DoAction;}break;
	case EAI_ACTION_BLOWUP: {pkFuncDoAction = &PgAIActBlowup::DoAction;}break;
	case EAI_ACTION_CALLHELP: {pkFuncDoAction = &PgAIActCallHelp::DoAction;}break;
	case EAI_ACTION_HIDE: {pkFuncDoAction = &PgAIActHide::DoAction;}break;
	case EAI_ACTION_WAIT_COOLTIME: {pkFuncDoAction = &PgAIWaitCoolTime::DoAction;}break;
	case EAI_ACTION_DELAY: {pkFuncDoAction = &PgAIActDelay::DoAction;}break;
	case EAI_ACTION_AROUND : {pkFuncDoAction = &PgAIActAround::DoAction;}break;
	case EAI_ACTION_KEEP_DISTANCE : {pkFuncDoAction = &PgAIActKeepDistance::DoAction;}break;
    case EAI_ACTION_ROAD: {pkFuncDoAction = &PgAIActRoad::DoAction;}break;
	case EAI_ACTION_FOLLOW_CALLER: {pkFuncDoAction = &PgAIActFollowCaller::DoAction;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown Action ["<<eAction<<L"]");
		}break;
	}

	if(pkFuncDoAction == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFuncDoAction is 0"));
		return;
	}

	(*pkFuncDoAction)(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg);
	//INFO_LOG(BM::LOG_LV5, "DoAction : "<<g_kAIActionName[eAction]);
    //INFO_LOG(BM::LOG_LV5, "x: " << pkUnit->GetPos().x << ", y: " << pkUnit->GetPos().y<< ", z: "  << pkUnit->GetPos().z);
}

/*
bool PgAIActionManager::LoadWayPointFile(char const* pchFilePath)
{
	//char FilePath[128] = "";
	//::sprintf_s(FilePath, 128, "Script/Map/%d.way", GroundKey().iGroundNo);

	FILE * fp = NULL ;

	//if ( !(fp = fopen(FilePath,"r+")))
	if (fopen_s(&fp, pchFilePath, "r+") != 0)
	{
		INFO_LOG(BM::LOG_LV7, _T("[%s] Cannot Open WayPointFile.....File[%s]"), __FUNCTIONW__, UNI(FilePath));
		return false;
	}

	// 처음엔 그룹 갯수
	char buffer[64] = "";
	char buffer1[64] = "";
	char buffer2[64] = "";

	int totalgroup = 0 ;
	fscanf_s(fp,"%s %d",buffer, 64, &totalgroup);

	m_kWayPoint.iGroupCount = totalgroup ;

	for ( int i = 0 ; i < totalgroup ; i ++ )
	{
		int groupindex = 0 ;
		int totalindex = 0 ;
		fscanf_s(fp, "%s %d %s %d", buffer, 64, &groupindex, buffer1, 64, &totalindex);

		MAP_WAYPOINT_GROUP2 groupinfo;
		groupinfo.iIndexCount = totalindex ;

		fscanf_s(fp, "%s", buffer, 64);

		for ( int j = 0 ; j < totalindex ; j ++ )
		{
			PgWayPoint2* pkWay = m_kWayPointPool.New();

			int index = 0 ;
			float radius  = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			fscanf_s(fp, "%d %f %f %f %f", &index , &radius , &x ,&y ,&z );
			pkWay->Index(index );
			pkWay->Radius((int)radius);
			pkWay->Point(POINT3(x,y,z));

			groupinfo.kWayPointIndex.insert(std::make_pair(index,pkWay));
		}

		fscanf_s(fp, "%s", buffer, 64);

		m_kWayPoint.kWayPointGroup.insert(std::make_pair(groupindex,groupinfo));
	}

	fclose(fp);
	return true;
}
*/

void PgAIActionManager::Release()
{
	m_kWayPointMng.Release();
}