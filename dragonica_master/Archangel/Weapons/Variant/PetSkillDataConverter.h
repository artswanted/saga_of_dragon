#ifndef WEAPON_VARIANT_PET_PETSKILLDATACONVERTER_H
#define WEAPON_VARIANT_PET_PETSKILLDATACONVERTER_H

#include "Item.h"

class PgPet;

class PgPetSkillDataSet
{
//	static __int64 const ms_i64MakeEffectTime = ( CGameTime::OneWeek * 4i64 / CGameTime::MINUTE );

public:
	static BYTE const SKiLL_BINARY_VERSION = 0;

public:
	static bool InitSkill( CONT_DEFCLASS_PET_SKILL::mapped_type::value_type const &kDefPetSkill, PgPet *pkPet, PgItem_PetInfo * pkPetItemInfo );
	static int SetSkill( PgPet *pkPet, PgItem_PetInfo::SStateValue const &kSkillStateValue );

protected:
	static bool AddEffect( __int64 const i64NowMinTime, __int64 const i64SkillTime, size_t const iIndex, int const iSkillNo, PgPet *pkPet );
};

#endif // WEAPON_VARIANT_PET_PETSKILLDATACONVERTER_H