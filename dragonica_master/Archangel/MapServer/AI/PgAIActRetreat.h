#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTRETREAT_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTRETREAT_H

class PgAIActRetreat
{
public:
	PgAIActRetreat() {};
	~PgAIActRetreat() {};

	// 속도향상!!
	static bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	static void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);

	// TYPE 2 : 전투 중 후퇴할 줄 아는 AI
	static void OnEnter2(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction2(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTRETREAT_H