#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTIONAMANAGER_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTIONAMANAGER_H

#include "PgWayPointMng.h"

class PgAIActionManager
{
public:
	PgAIActionManager(void);
	~PgAIActionManager(void);

	bool Init(PgGround* pkGround);
	bool Reload(PgGround const& rkGround);
	bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);

	void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);

	void Release();

	typedef bool (*pfnCanTransit)(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	typedef void (*pfnDoAct)(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);
	typedef void (*pfnAct)(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	//bool LoadWayPointFile(char const* pchFilePath);
private:
	//MAP_WAYPOINT2 m_kWayPoint;
	//typedef BM::TObjectPool< PgWayPoint2 > WayPointPool;
	//static WayPointPool m_kWayPointPool;
	PgWayPointMng m_kWayPointMng;
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTIONAMANAGER_H