#ifndef MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGBSGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGBSGROUND_H

#include "Variant/PgBattleSquare.h"
#include "PgBSTeamMng.h"
#include "PgVolatileInven.h"
#include "PgBSTimeEvent.h"
#include "PgBSMapGame.h"

namespace PgGroundUtil
{
	bool IsHaveFreePVPAttr(EDynamicGroundAttr const eAttr);
	bool IsBSGround(T_GNDATTR const eGndAttr);
};

class PgBSGround : public PgGround
{
public:
	PgBSGround();
	virtual ~PgBSGround();

	virtual void Clear();
	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );
	virtual void OnTick1s();
	virtual void OnTick100ms();

	virtual bool IsBSItem(int const iItemNo) const;
	virtual void CallAlramReward( PgPlayer * pkPlayer );

	virtual int GetMapItemBagGroundNo() const { return m_kBSGame.GetMapItemBagNo(); };
	virtual void GetGenGroupKey(SGenGroupKey& rkkGenGrpKey)const;
protected:
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual void RecvUnitDie(CUnit *pkUnit);
	virtual void WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const;
	using PgGround::DynamicGndAttr;
	virtual void DynamicGndAttr(EDynamicGroundAttr const eNewDGAttr);
	virtual bool IsAlramMission(void)const{return ((NULL != m_pkAlramMissionMgr) && (BSGS_NOW_GAME == m_eStatus));}
	virtual bool PushBSInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem);
	void LeaveBSUser(CUnit* pkUnit);
	virtual void SendMapLoadComplete( PgPlayer *pkUser );
	virtual void OnTick_AlramMission( PgPlayer *pkPlayer, DWORD const dwCurTime );

private:
	void DeSetBSTeamUnitStatus(CUnit* pkUnit);
	void SetBSTeamUnitStatus(CUnit* pkUnit);
	bool ProcessBSWaiter();
	void DropAllVolatileInven(CUnit* pkUnit, BM::GUID* pkKillerGuid = NULL);
	//void SendPointToUser(CONT_BS_MEMBER_POINT const& kContMemberPoint, CONT_BS_HAVE_MINIMAP_ICON const& kIconInfo) const;
	//void SendTeamPoint() const;
	void UpdateBSPoint(ContGuidSet& rkOutGuid);
	void SendBSPoint(CUnit* pkUnit) const;
	void SyncUnitPos(DWORD const dwElapsedTime);
	void DoStart();
	void DoEnd(EBS_GAME_STATUS const eStatus);
	void GiveReward();

private:
	PgBSMapGame			m_kBSGame;
	EBS_GAME_STATUS		m_eStatus;
	DWORD				m_dwCalculatePointTick;
	DWORD				m_dwSyncUnitPosTick;
	DWORD				m_dwSyncScoreTick;
	static const		DWORD CALCULATE_POINT_INTERVAL = 10000;	// 10초
	static const		DWORD SYNC_UNIT_POS_MINIMAP_INTERVAL = 777;
	static const		DWORD SYNC_SCORE_INTERVAL = 1000;
};

#endif // MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGBSGROUND_H