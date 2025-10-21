#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTAROUND_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTAROUND_H

class PgAIActAround
{
public:
	PgAIActAround() {};
	~PgAIActAround() {};

	// 속도향상!!
	static bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	static void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);

protected:
	static void DoAction_ECPattern_AccessProwlArea(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_TakeProwl(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_Ended(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);

	static POINT3 GetRandomProwlArea(CUnit* pkUnit, PgGround* pkGround);
	static bool IsMoveState(EChasePattern eChasePattern);

	//static float m_fProwlAreaRange;
	//static float m_fProwlAreaRangeQ;
	static DWORD m_dwGoalPosRefreshInternval;
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTAROUND_H