#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTIONJOBSKILL_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTIONJOBSKILL_H

#include "stdafx.h"
#include "PgAction.h"
#include "PgActionJobSkill.h"
#include "Variant/PgJobSkill.h"

class PgAction_JobSkill_AddExpertness
	:	public PgUtilAction
{
public:
	explicit PgAction_JobSkill_AddExpertness(EItemModifyParentEventType const kCause, int const iSkillNo, int const iBase_Expertness, SGroundKey const &rkGroundKey, int iExpertnessUpVolume = 0, BM::Stream const& rkAddonPacket = BM::Stream())
		: m_kCause(kCause), m_iSkillNo(iSkillNo), m_iBase_Expertness(iBase_Expertness), m_kGndKey(rkGroundKey), m_kPacket(rkAddonPacket), m_iExpertnessUpVolume(iExpertnessUpVolume), m_iAddExpertnessRate(0)
	{
	}
	virtual ~PgAction_JobSkill_AddExpertness()
	{
	}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);

	CLASS_DECLARATION(int, m_iAddExpertnessRate, AddExpertnessRate);
private:
	EItemModifyParentEventType const m_kCause;
	int const m_iSkillNo;
	int const m_iBase_Expertness;
	SGroundKey m_kGndKey;
	int m_iExpertnessUpVolume;

	BM::Stream const& m_kPacket;

private:// Not Use
	PgAction_JobSkill_AddExpertness();
	PgAction_JobSkill_AddExpertness(PgAction_JobSkill_AddExpertness const&);
};

class PgAction_JobSkill3_AddExpertness
	:	public PgUtilAction
{
public:
	explicit PgAction_JobSkill3_AddExpertness(EItemModifyParentEventType const kCause, int const iRecipeItemNo, SGroundKey const &rkGroundKey, BM::Stream const& rkAddonPacket = BM::Stream())
		: m_kCause(kCause), m_iRecipeItemNo(iRecipeItemNo), m_kGndKey(rkGroundKey), m_kPacket(rkAddonPacket)
	{
	}
	virtual ~PgAction_JobSkill3_AddExpertness()
	{
	}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);

private:
	EItemModifyParentEventType const m_kCause;
	int const m_iRecipeItemNo;
	SGroundKey m_kGndKey;

	BM::Stream const& m_kPacket;

private:// Not Use
	PgAction_JobSkill3_AddExpertness();
	PgAction_JobSkill3_AddExpertness(PgAction_JobSkill3_AddExpertness const&);
};

class PgAction_JobSkill_Learn
	:	public PgUtilAction
{
public:
	explicit PgAction_JobSkill_Learn(SGroundKey const &rkGroundKey, BM::Stream const& rkAddonPacket = BM::Stream())
		: m_kGndKey(rkGroundKey), m_kPacket(rkAddonPacket)
	{
	}
	virtual ~PgAction_JobSkill_Learn()
	{
	}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);
private:
	SGroundKey m_kGndKey;

	BM::Stream const& m_kPacket;
};

class PgAction_JobSkill_Delete
	:	public PgUtilAction
{
public:
	explicit PgAction_JobSkill_Delete(SGroundKey const &rkGroundKey, BM::Stream const& rkAddonPacket = BM::Stream())
		: m_kGndKey(rkGroundKey), m_kPacket(rkAddonPacket)
	{
	}
	virtual ~PgAction_JobSkill_Delete()
	{
	}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);
private:
	SGroundKey m_kGndKey;

	BM::Stream const& m_kPacket;
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTIONJOBSKILL_H