#ifndef MAP_MAPSERVER_ACTION_AI_PGELITEPATTENMANAGER_H
#define MAP_MAPSERVER_ACTION_AI_PGELITEPATTENMANAGER_H

class PgGround;
class CUnit;
class SActArg;

class PgElitePattenManager
{
public:
	typedef void (*pfnDoAct)(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkActArg);
	typedef void (*pfnDoEnt)(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkActArg);

	PgElitePattenManager();
	~PgElitePattenManager();

	bool Run(PgGround* pkGround, CUnit* pkUnit, DWORD const dwElapsed);

	void OnDamaged(CUnit* pkUnit, int const iPoint, CSkillDef const *pkSkillDef);

private:
	void OnEnter(CUnit* pkUnit, EElitePattenStateType const eCurrentState, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg& pkActArg);
	void DoAction(CUnit* pkUnit, EElitePattenStateType const eCurrentState, DWORD const dwElapsedTime, SActArg& rkActArg);
	void OnLeave(CUnit* pkUnit, EElitePattenStateType const eCurrentState, EElitePattenStateType const eNextState, DWORD const dwElapsedTime, SActArg& rkActArg);
};

#define g_kElitePatternMng SINGLETON_STATIC(PgElitePattenManager)

#endif // MAP_MAPSERVER_ACTION_AI_PGELITEPATTENMANAGER_H