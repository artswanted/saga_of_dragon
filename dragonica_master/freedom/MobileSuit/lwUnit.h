#ifndef FREEDOM_DRAGONICA_SCRIPTING_WORLDOBEJCT_ACTOR_LWUNIT_H
#define FREEDOM_DRAGONICA_SCRIPTING_WORLDOBEJCT_ACTOR_LWUNIT_H

#include "PgScripting.h"
#include "variant/Unit.h"
#include "lwGUID.h"
#include "lwPoint3.h"

class lwEffect;

LW_CLASS(CUnit, Unit)
	lwGUID GetGuid();
	lwGUID GetPartyGuid();
	int GetState();
	int GetUnitType();
	void SetState(int const iState);
	int GetAbil(WORD wType);
	int GetFrontDirection();
	bool IsEffect(int iEffectKey, bool bInGroup);
	lwEffect FindEffect(int iEffectNo);
	lwEffect GetEffect(int iEffectKey, bool bInGroup);
	bool DeleteEffect(int iEffectKey);
	lwGUID GetGuildGuid();
	void SetCaller(lwGUID kActorGUID);
	lwGUID GetCaller() const;
	lwGUID GetSelectedPet() const;
	bool IsDead()const;
	lwPoint3 GoalPos()const;
	void SetGoalPos(lwPoint3 kPos);
LW_CLASS_END;
#endif // FREEDOM_DRAGONICA_SCRIPTING_WORLDOBEJCT_ACTOR_LWUNIT_H