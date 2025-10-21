#ifndef MAP_MAPSERVER_ACTION_SKILL_PGSKILLONFINDTARGETMANAGER_H
#define MAP_MAPSERVER_ACTION_SKILL_PGSKILLONFINDTARGETMANAGER_H

#include <map>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

class BM::Stream;
class CSkillDef;
class PgActionResultVector;
class PgISkillOnFindTarget;
class PgGround;

class PgSkillOnFindTargetManager
{
public:
	PgSkillOnFindTargetManager();
	~PgSkillOnFindTargetManager();
	void Init();
	void Release();
	bool Build();

	bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
private:
	PgISkillOnFindTarget* CreateOnTargetFunc(int const iFuncType) const;
	typedef std::unordered_map< int, PgISkillOnFindTarget* >	CONT_SKILL_FIND_TARGET;
	CONT_SKILL_FIND_TARGET m_kContFindTarget;
	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kSkillOnFindTargetMgr SINGLETON_STATIC(PgSkillOnFindTargetManager)

#endif // MAP_MAPSERVER_ACTION_SKILL_PGSKILLONFINDTARGETMANAGER_H