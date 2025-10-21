#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTKEEPDISTANCE_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTKEEPDISTANCE_H

class PgAIActKeepDistance
{
public:
	PgAIActKeepDistance() {};
	~PgAIActKeepDistance() {};

	// 속도향상!!
	static bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	static void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);
	
protected:
	static void DoAction_ECPattern_KeepDistance(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_GotoTarget_X(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTKEEPDISTANCE_H