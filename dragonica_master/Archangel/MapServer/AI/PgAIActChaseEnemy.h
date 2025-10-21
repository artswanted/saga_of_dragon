#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTCHASEENEMY_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTCHASEENEMY_H

class PgAIActChaseEnemy
{
public:
	PgAIActChaseEnemy() {};
	~PgAIActChaseEnemy() {};

	// 속도향상!!
	static bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	static void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);

	static void DoAction_ECPattern_GotoTarget_X(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_GotoTarget(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_BattleIdle(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);
	static void DoAction_ECPattern_Ended(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround);

	static bool IsMoveState(EChasePattern eChasePattern);

protected:
	//static float m_fProwlAreaRange;
	//static float m_fProwlAreaRangeQ;
	static DWORD m_dwGoalPosRefreshInternval;
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTCHASEENEMY_H