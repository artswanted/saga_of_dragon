#ifndef MAP_MAPSERVER_ACTION_AI_PGAIMANAGER_H
#define MAP_MAPSERVER_ACTION_AI_PGAIMANAGER_H

#include "PgAIPatternManager.h"
#include "PgAIActionManager.h"

class PgGround;

class PgAIManager // -> 이걸. 글로벌 접근 개체로 놓고, ground 를 인자로 받으면 어떤가?
{
public:
	PgAIManager(void);
	~PgAIManager(void);

	bool Init(PgGround* pkGround);
	bool ReloadWaypoint(PgGround& rkGround);

	bool RunAI(CUnit* pkUnit, DWORD dwElapsed);
	void Release();

protected:
	EAIActionType GetNextActionType(CUnit* pkUnit, SUnit_AI_Info * pkAI, SActArg* pkActArg);
	void BeginNewAction(CUnit* pkUnit, EAIActionType eNewAction, int iActionType, DWORD dwElapsed, SActArg* pkActArg);
	void DoCurrentAction(CUnit* pkUnit, EAIActionType eNewAction, int iActionType, DWORD dwElapsed, SActArg* pkActArg);
	EAIActionType CheckAIEvent(CUnit* pkUnit, SUnit_AI_Info * pkAI, SActArg* pkActArg);
	EAIActionType GetCheckForceSkill(CUnit * pkUnit, EAIActionType const eNextAction, SActArg* pkActArg);

private:
	PgAIActionManager m_kActionMng;
	// Ground를 쥐고 있다 (흠...구조상 그리 좋지는 않다)
	//	GetUnit(...) 등 AI 작업을 위해서는 Ground와 상호작용이 빠질수 없다.
	//	---> 아이디어를 공모합니다.
	PgGround* m_pkGround;
	//PgMonAreaMng* m_pkMonAreaMng;
};

extern bool CheckRetreatHP(CUnit* pkUnit);

#endif // MAP_MAPSERVER_ACTION_AI_PGAIMANAGER_H