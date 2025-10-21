#ifndef WEAPON_VARIANT_UNIT_PGCLASSPETDEFMGR_H
#define WEAPON_VARIANT_UNIT_PGCLASSPETDEFMGR_H

#include "defAbilType.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/LockUtil.h"
#include "Item.h"

typedef enum ePetType
{
	EPET_TYPE_1			= 0,// 1차 펫(비성장형)
	EPET_TYPE_2			= 1,// 2차 펫(성장형)
	EPET_TYPE_3			= 2,// 3차 펫(탑승형. 성장형 레벨은 성장하나 능력치는 고정이다. 아이템 장착 불가.)
}EPetType;

typedef struct tagClassPetDefLevel
:	public TBL_DEF_CLASS_PET_LEVEL_BASE
{
	tagClassPetDefLevel(void)
	{}

	tagClassPetDefLevel( TBL_DEF_CLASS_PET_LEVEL_BASE const &rhs )
		:	TBL_DEF_CLASS_PET_LEVEL_BASE(rhs)
	{}

	CLASSDEF_ABIL_CONT	kAbil;
}SClassPetDefLevel;

class PgItem_PetInfo;

class PgClassPetDef
{
public:
	PgClassPetDef(void):m_pkDefAbil(NULL){}

	explicit PgClassPetDef( TBL_DEF_CLASS_PET const &kDefPet, SClassPetDefLevel const *pkDefAbil );
	explicit PgClassPetDef( PgClassPetDef const & );

	static int GetClassGrade( int const iClass );
	static int GetBaseClass( int const iClass );
	static HRESULT IsClassLimit( __int64 const i64ClassLimit, int const iClass );
	static __int64 GetPetUpgradeCost(PgItem_PetInfo* pkPetItemInfo);

	int GetAbil( WORD const wType )const;
	__int64 GetAbil64( WORD const wType )const;

	BYTE GetPetType(void)const{return m_kDefPet.byPetType;}
	bool GetPetItemOption( SEnchantInfo &rkOutEnchantInfo )const;

	int GetDefaultHair(void)const{return m_kDefPet.iDefaultHair;}
	int GetDefaultFace(void)const{return m_kDefPet.iDefaultFace;}
	int GetDefaultBody(void)const{return m_kDefPet.iDefaultBody;}

	int GetSkillDefID(void)const{return m_kDefPet.iSkillIndex;}

protected:
	TBL_DEF_CLASS_PET			m_kDefPet;
	SClassPetDefLevel const		*m_pkDefAbil;
};

class PgClassPetDefMgr
{
public:

	typedef std::map< SClassKey, PgClassPetDef >		ContClassPetDef;
	typedef std::map< int, PgClassPetDef >				ContClassPetLastLv;// 마지막레벨의 정보
	typedef std::map< SClassKey, SClassPetDefLevel* >	ContPoolDefLevel;

public:
	PgClassPetDefMgr(void);
	virtual ~PgClassPetDefMgr(void);

public:
	bool IsChangeClass( SClassKey const &kClassKey ) const;

public:
	void Clear();
	void swap( PgClassPetDefMgr &rRight );

	bool Build( CONT_DEFCLASS_PET const &rkDef, CONT_DEFCLASS_PET_LEVEL const &rkDefLv, CONT_DEFCLASS_PET_SKILL const &rkDefSkill, CONT_DEFCLASS_PET_ABIL const &rkAbil, short const sMaximumLevel );

	void CraeteLastLv( short const sMaximumLevel );
	short GetLastLv( int const iClass )const;
	__int64 GetMaxExp( int const iClass )const;

//	HRESULT GetAbil( SClassKey const &rkKey, SPlayerBasicInfo &rkBasic)const;
	
	HRESULT RevisionClassKey(SClassKey &rkKey, __int64 &i64Exp)const;//변화 없으면 S_OK;

	bool IsLvUp( SClassKey const &kNowKey, __int64 const &i64Exp, SClassKey &kRetKey )const;
	bool GetDef( SClassKey const &rkKey, PgClassPetDef *pkOutDef )const;
	bool CheckData( CONT_DEF_PET_HATCH const &rkDefPetHatch )const;

protected:
	ContPoolDefLevel				m_kContPool;
	ContClassPetDef					m_kDef;
	ContClassPetLastLv				m_kDefLastLv;
};


#endif // WEAPON_VARIANT_UNIT_PGCLASSPETDEFMGR_H