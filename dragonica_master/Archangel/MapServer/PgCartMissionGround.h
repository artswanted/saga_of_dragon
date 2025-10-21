#pragma once
#include "PgIndun.h"

class PgCartMissionGround : public PgIndun
{

public:
    virtual void OnTick100ms();
    virtual void OnTick5s();

    virtual EGroundKind GetKind() const { return GKIND_CART_MISSION; }

	virtual HRESULT InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo, 
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller = NULL, 
		bool DropAllItem = false, int EnchantGradeNo = 0, 
		ECREATE_HP_TYPE const Create_HP_Type = ECHT_NONE);

	virtual void SetState(EIndunState const eState, bool bAutoChange = false, bool bChangeOnlyState = false);

protected:
	void SyncCartPosition(PgObjectUnit* pkObject);
	void SpawnMonsterAround(PgObjectUnit* pkObject);
	virtual void RecvUnitDie(CUnit * pUnit);
	virtual void DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent);

	int m_kMonSpawnNo;
	int m_kMonCount;
	DWORD m_kMonsterSpawnTime;
	DWORD m_kMonsterSpawnDelay;
};
