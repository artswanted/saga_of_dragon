#ifndef WEAPON_VARIANT_UNIT_PGCLASSDEFMGR_H
#define WEAPON_VARIANT_UNIT_PGCLASSDEFMGR_H

#include "defAbilType.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/LockUtil.h"

class PgClassDefMgr
{
	typedef std::map< SClassKey, CLASS_DEF_BUILT* > ContClassDef;
	typedef std::map< int, CLASS_DEF_BUILT* > ContClassLastLv;// 마지막레벨의 정보 

public:
	PgClassDefMgr(void);
	virtual ~PgClassDefMgr(void);

	typedef struct tagLvUpAddValue
	{
		tagLvUpAddValue()
			: sSP(0), sBonusStatus(0)
		{}

		short sSP;
		short sBonusStatus;
	} SLvUpAddValue;

public:
	int GetAbil( SClassKey const &rkKey, WORD const Type)const;
	__int64 GetAbil64( SClassKey const &rkKey, WORD const Type)const;

	HRESULT GetAbil( SClassKey const &rkKey, SPlayerBasicInfo &rkBasic)const;
	bool Build( CONT_DEFCLASS const *pkDef, CONT_DEFCLASS_ABIL const *pkAbil);
	bool BuildLastLevel(int const iMaxLevel = 0);
	void Clear();
	void swap(PgClassDefMgr &rRight);

	HRESULT RevisionClassKey(SClassKey &rkKey, __int64 &i64Exp)const;//변화 없으면 S_OK;
	HRESULT AccSkillPoint( SClassKey const &rkKey, int &iRet)const;
	__int64 GetExperience4Levelup(SClassKey& rkKey) const;

	bool IsLvUp( SClassKey const &kNowKey, __int64 const &i64Exp, SClassKey &kRetKey, SLvUpAddValue& kAdded)const;
	__int64 GetMaxExperience(int const iClass) const;

	CLASS_DEF_BUILT const *GetDef(const SClassKey &rkKey)const;
	static short s_sMaximumLevel;	// Player가 Levelup 할수 있는 최대값
protected:
	
	int GetAbil( CLASS_DEF_BUILT const *pkDef, WORD const wType )const;

protected:
	BM::TObjectPool<CLASS_DEF_BUILT> m_kClassDefPool;
	ContClassDef		m_kDef;

	ContClassLastLv	m_kDefLastLv;

	//mutable Loki::Mutex m_kMutex;
};

//#define g_ClassDef SINGLETON_STATIC(PgClassDefMgr)

#endif // WEAPON_VARIANT_UNIT_PGCLASSDEFMGR_H