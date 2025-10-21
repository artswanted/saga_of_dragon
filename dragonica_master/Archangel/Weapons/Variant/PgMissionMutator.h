#ifndef WEAPON_VARIANT_BASIC_MISSION_PGMISSIONMUTATOR_H
#define WEAPON_VARIANT_BASIC_MISSION_PGMISSIONMUTATOR_H
#include <set>
#include "TableDataManager.h"

#pragma pack(1)
typedef enum eMissionMutatorType
{
	MISSION_MUTATOR_TYPE_NONE		= 0, // Нету мутатора
	MISSION_MUTATOR_TYPE_POSITIVE	= 1, // Мутатор влияет положительно
	MISSION_MUTATOR_TYPE_NEGATIVE	= 2, // Мутатор влияет отричательно
}EMissionMutatorType;

typedef enum eMissionMutatorUnitType
{
	MISSION_MUTATOR_UNIT_NONE		= 0, // Не для юнита
	MISSION_MUTATOR_UNIT_MONSTER	= 1, // Для Монстра
	MISSION_MUTATOR_UNIT_PLAYER		= 2, // Для Игрока
	MISSION_MUTATOR_UNIT_OBJ		= 4, // Для объектов(разрушаемых)
	MISSION_MUTATOR_UNIT_PET		= 8, // Для Питомцев
	MISSION_MUTATOR_UNIT_SUMMONER	= 16, // Для Существ созданных шаманом
}EMissionMutatorUnitType;
#pragma pack()

typedef std::set<INT> MUTATOR_SET;

class PgMissionMutator
{
protected:
	//! Set Whit all mutator
	MUTATOR_SET m_kMutators;
public:
	PgMissionMutator();
	PgMissionMutator(PgMissionMutator const & rkData);
	virtual ~PgMissionMutator();

	//! Check exist in table bin
	bool MutatorVerify(CONT_DEF_MISSION_MUTATOR const* pkRhs);

	//! Wrote to packet
	void WriteToPacket_Mutator(BM::Stream& rkPacket) const;

	//! Read from packet
	void ReadFromPacket_Mutator(BM::Stream& rkPacket);

	//! Get Mutator Data
	MUTATOR_SET GetMutatorSet() const { return this->m_kMutators; }

	//! Clear Mutator Data
	void ClearMutator() { this->m_kMutators.clear(); }

	//! Set Mutator Data
	void SetMutatorSet(MUTATOR_SET const pkData) { this->m_kMutators = pkData;  }
};


#endif // WEAPON_VARIANT_BASIC_MISSION_PGMISSIONMUTATOR_H