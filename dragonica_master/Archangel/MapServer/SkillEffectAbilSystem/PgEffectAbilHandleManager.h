#ifndef MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILHANDLEMANAGER_H
#define MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILHANDLEMANAGER_H

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <unordered_map>
#include <vector>
#include "PgEffectAbilTable.h"

class CUnit;
class CEffect;
class CEffectDef;
class SActArg;
class PgIEffectFunction;

class PgEffectAbilHandleManager
{
	typedef std::unordered_map< int, PgIEffectFunction* >	CONT_EFFECT_FUNC;

public:
	PgEffectAbilHandleManager();
	~PgEffectAbilHandleManager();

public:
	void Init();
	void Release();
	bool Build();//def

	void EffectBegin	(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg) const;
	void EffectEnd		(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg) const;
	int  EffectTick		(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg, DWORD const dwElapsed) const;
	int  EffectReset	(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg, DWORD const dwElapsed) const;

private:
	PgIEffectFunction* CreateEffectFunc(int const iFuncType)const;

	mutable ACE_RW_Thread_Mutex m_kMutex;

	CONT_EFFECT_FUNC	m_kConEffect;
	PgAbilTypeTable		m_kAbilTypeTable;
};


#define g_kEffectAbilHandleMgr SINGLETON_STATIC(PgEffectAbilHandleManager)

#endif // MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILHANDLEMANAGER_H