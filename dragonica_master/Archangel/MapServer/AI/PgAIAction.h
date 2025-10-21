#ifndef MAP_MAPSERVER_ACTION_AI_PGAIACTION_H
#define MAP_MAPSERVER_ACTION_AI_PGAIACTION_H

/*
class PgMonAreaMng;

class PgAIAction
{
public:
	PgAIAction(void) {};
	~PgAIAction(void) {};

	virtual bool CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg);
	virtual void OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	virtual void OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg);
	virtual DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg);

	void SetMonAreaMng(PgMonAreaMng* pkAreaMng);
private:
	PgMonAreaMng* m_pkMonAreaMng;
};
*/

//bool IsTargetAvalible(CUnit* pkUnit, PgGround* pkGround);
bool MovingSomething(POINT3& ptCurrent, POINT3 const& ptTarget, int const iMoveSpeed, DWORD const dwElapedTime);
bool UnitMoved(CUnit* pkUnit, POINT3 const& rkTargetPos, DWORD dwMoveTime, NxScene *pkScene, bool bCheckGround = false);
int GetMinDistanceFromTarget(CUnit* pkUnit);
extern bool IsDetailAI(CUnit *pkUnit);

#endif // MAP_MAPSERVER_ACTION_AI_PGAIACTION_H