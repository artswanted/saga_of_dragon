#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTFIRESKILLDELAY_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTFIRESKILLDELAY_H

class PgAIActFireSkillDelay
{
public:
	PgAIActFireSkillDelay() {};
	~PgAIActFireSkillDelay() {};

	static bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	static void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	static void DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);
};

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTFIRESKILLDELAY_H