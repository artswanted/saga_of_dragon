#ifndef MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILTABLE_H
#define MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILTABLE_H

#include <unordered_map>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

enum eEffectAbilType
{
	EAT_ADDABIL = 0,
	EAT_SETABIL,
	EAT_ADDABIL_AND_NULL,	//! End시 0으로 만들기
	EAT_SETABIL_AND_NULL,	//! End시 0으로 만들기
	EAT_ADD_EFFECT_ABIL,	//! EffectDef에 있는 abil을 직접 Add 처리
	EAT_EXCEPTION,
	EAT_MAX_HP_MP,			//! MAXP HP/MP의 처리
	EAT_TICK_ABIL,
	EAT_IGNORE,				//! 처리 하지 않아도 되는 AbilType
	EAT_ATTACK,
	EAT_BEGIN_SEND_TTW_MSG,
	EAT_END_SEND_TTW_MSG,
	EAT_HPMP_DEC_MAX_HPMP_RATE,
	EAT_EFFECT_NUM,
	EAT_MAX,
};

class PgAbilTypeTable
{
	friend class PgEffectAbilHandleManager;//PgEffectAbilHandleManager 만이 소유 할 수 있음.
	typedef std::unordered_map< WORD, WORD >	CONT_ABIL_DEFINE;
protected:
	PgAbilTypeTable();
	~PgAbilTypeTable();

public :
	void Init(); //! AbilType에 따라 처리 해야되는 경우가 있으면 추가 해주는 곳
	WORD GetAbilTypeTable(WORD const wType) const;

protected:
	CONT_ABIL_DEFINE	m_kConAbilTable;
};

#endif // MAP_MAPSERVER_ACTION_SKILL_PGEFFECTABILTABLE_H