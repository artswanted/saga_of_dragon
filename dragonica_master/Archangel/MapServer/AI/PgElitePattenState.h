#ifndef MAP_MAPSERVER_ACTION_AI_PGELITEPATTENSTATE_H
#define MAP_MAPSERVER_ACTION_AI_PGELITEPATTENSTATE_H

class PgElitePattenInit
{
public:
	PgElitePattenInit();
	~PgElitePattenInit();

	static void OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg);
	static void DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
	static void OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
};

class PgElitePattenNormal
{
public:
	PgElitePattenNormal();
	~PgElitePattenNormal();

	static void OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg);
	static void DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
	static void OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
};

class PgElitePattenFrenzy
{
public:
	PgElitePattenFrenzy();
	~PgElitePattenFrenzy();

	static void OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg);
	static void DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
	static void OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
};

class PgElitePattenGroggy
{
public:
	PgElitePattenGroggy();
	~PgElitePattenGroggy();

	static void OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg);
	static void DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
	static void OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg);
};

#endif // MAP_MAPSERVER_ACTION_AI_PGELITEPATTENSTATE_H