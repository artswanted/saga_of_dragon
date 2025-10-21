#ifndef MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONGROUND_H

#include "PgIndun.h"
#include "Variant/PgConstellation.h"

class PgExpeditionGround : public PgIndun
{

	typedef std::map<int, int> CONT_TEAM_DP;

public:

	PgExpeditionGround();
	virtual ~PgExpeditionGround();

	virtual EOpeningState Init(int const iMonsterControlID = 0, bool const bMonsterGen = true);
	bool Clone(PgExpeditionGround * pGround);
	virtual bool Clone(PgGround * pGround);
	virtual void Clear();
	virtual void OnTick1s();
	virtual void SetState(EIndunState const eState, bool bAutoChange = false, bool bChangeOnlyState = false);

	virtual EGroundKind GetKind() const { return GKIND_EXPEDITION; }
	virtual T_GNDATTR GetAttr() const;
	virtual int GetGroundNo() const;

	virtual HRESULT InsertMonster(
		TBL_DEF_MAP_REGEN_POINT const & GenInfo, 
		int const MonNo, BM::GUID & OutGuid, CUnit * Caller = NULL, 
		bool DropAllItem = false, int EnchantGradeNo = 0, 
		ECREATE_HP_TYPE const Create_HP_Type = ECHT_NONE);

protected:

	virtual bool VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy);
	virtual bool RecvGndWrapped( unsigned short Type, BM::Stream* const pPacket );

	virtual bool RecvMapMove(UNIT_PTR_ARRAY & UnitArray, SReqMapMove_MT & RMM
							, CONT_PET_MAPMOVE_DATA & ContPetMapMoveData
							, CONT_UNIT_SUMMONED_MAPMOVE_DATA & ContUnitSummonedMapMoveData
							, CONT_PLAYER_MODIFY_ORDER const & ModifyOrder);

	virtual void RecvUnitDie(CUnit * pUnit);
	virtual bool ReleaseUnit(CUnit * pUnit, bool bRecursiveCall = false, bool const bSendArea = true);

	virtual bool RecvRecentMapMove( PgPlayer *pkUser, bool const bClear );	// 리센트맵 위치로 돌려 보내라!

	virtual HRESULT SetUnitDropItem(CUnit * pOwner, CUnit * pDroper, PgLogCont & LogCont );
	bool GetSpecialDropItem(CUnit * pDroper, CONT_SPECIALITEM & DropItem);

	bool SetBestResult(PgPlayer * pPlayer);

	void SendExpeditionResult(SNfyResultItemList & ResultItem, CONT_SPECIALITEM & SpecialItemList);

	CLASS_DECLARATION_NO_SET(size_t, m_iLastRemainMonsterCount, LastRemainMonsterCount);

	CLASS_DECLARATION_NO_SET(int, m_BestTeam, GetBestTeam);
	CLASS_DECLARATION_NO_SET(PgPlayer*, m_pBestPlayer, GetBestPlayer);
	CLASS_DECLARATION_NO_SET(PgPlayer*, m_pBestSupporter, GetBestSupporter);
	CLASS_DECLARATION_NO_SET(DWORD, m_TimeLimit, TimeLimit);
	CLASS_DECLARATION_NO_SET(DWORD, m_AccumLimitTime, AccumLimitTime);
	CLASS_DECLARATION_NO_SET(DWORD, m_RemainTime, RemainTime);
	CLASS_DECLARATION_NO_SET(bool, m_FailedTime, FailedTime);
	CLASS_DECLARATION_NO_SET(bool, m_IsEventScript, IsEventScript);

private:

	CONT_TEAM_DP m_TeamDP;						// first : TEAM No, Second : DP

};

#endif // MAP_MAPSERVER_MAP_GROUND_PGEXPEDITIONGROUND_H