#ifndef MAP_MAPSERVER_MAP_GROUND_PGCONSTELLATIONGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGCONSTELLATIONGROUND_H

#include "PgIndun.h"
#include "Variant/PgConstellation.h"


class PgConstellationGround : public PgIndun
{
public:
	PgConstellationGround();
	virtual ~PgConstellationGround();

	EOpeningState Init(int const iMonsterControlID = 0, bool const bMonsterGen = true);
	bool Clone(PgConstellationGround * pGround);
	virtual bool Clone(PgGround * pGround);

	void Clear();
	virtual void OnTick1s();
	virtual void SetState(EIndunState const eState, bool bAutoChange = false, bool bChangeOnlyState = false);

	virtual EGroundKind GetKind() const { return GKIND_CONSTELLATION; }
	virtual int GetGroundNo() const;
	virtual void GetGenGroupKey(SGenGroupKey& rkkGenGrpKey)const;

	virtual HRESULT InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo, 
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller = NULL, 
		bool DropAllItem = false, int EnchantGradeNo = 0, 
		ECREATE_HP_TYPE const Create_HP_Type = ECHT_NONE);

	virtual void ProcessGateWayUnLock(HRESULT const hRet, CUnit * pkCaster, BM::Stream & rkAddonPacket);

protected:

	virtual bool RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket );
	virtual void SendMapLoadComplete( PgPlayer *pkUser );

	virtual void RecvUnitDie(CUnit * pUnit);
	virtual bool RecvRecentMapMove( PgPlayer *pkUser );	// 리센트맵 위치로 돌려 보내라!

	virtual HRESULT SetUnitDropItem(CUnit * pOwner, CUnit * pDroper, PgLogCont & LogCont );
	virtual bool IsMonsterTickOK() const;

	void IndunState_Play_Action();

private:

};

#endif