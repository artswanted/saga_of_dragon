#pragma once

#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgObjectMgr.h"


class PgGround
{
public:
	PgGround(SGroundKey const &rkGroundKey);
	virtual ~PgGround();

public:
	void AddUnit(CUnit* pkUnit);
protected:
	CLASS_DECLARATION_S(SGroundKey, GroundKey);
	PgObjectMgr m_kObjMgr;
};

class PgVirtualWorld
{
public:
	PgVirtualWorld();
	virtual ~PgVirtualWorld();
	
	typedef std::map<SGroundKey, PgGround*> CONT_GROUND;
public:
	HRESULT Locked_AddUnit(SGroundKey const &rkGround, UNIT_PTR_ARRAY& rkUnitArray);

protected:
	HRESULT AddUnit(PgGround* pkGround, UNIT_PTR_ARRAY &rkUnitArray);
	HRESULT AddUnit(PgGround* pkGround, CUnit const *pkUnit);


protected:
	CONT_GROUND m_kContGround;
	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kVWorld SINGLETON_STATIC(PgVirtualWorld)