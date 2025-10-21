#ifndef MAP_MAPSERVER_MAP_GROUND_PGSUPERGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGSUPERGROUND_H

#include "PgIndun.h"
#include "PgBSTimeEvent.h"
#include "Variant/PgConstellation.h"

namespace SuperGroundUtil
{
	bool ReqEnterSuperGround(CUnit* pkUnit, SGroundKey const& rkGndKey, int const iSuperGroundNo, int const iSuperGroundMode, int const iSpawnNo = 1, int const iFloorNo = 1, SPMO const* pkModifyItem = NULL, bool IsElement = false);
}

//
typedef enum eSuperGroundTimeEventType
{
	SGTET_RECENT_MAP_MOVE = 0,
} ESuperGroundTimeEventType;
//
class PgSuperGroundTimeEventMgr : public PgTimeEventMgr< ESuperGroundTimeEventType >
{
public:
	struct SSuperGroundTickFunc
	{
		SSuperGroundTickFunc(ContGuidSet& rkContGuid);
		bool operator ()(CONT_TIME_EVENT::value_type& rkTimeEvent);
	private:
		ContGuidSet& m_rkContGuid;
	};

	PgSuperGroundTimeEventMgr();
	~PgSuperGroundTimeEventMgr();
};


//
class PgSuperGround : public PgIndun
{
public:
	typedef std::vector<const PgGroundResource*>			ContFloorResource;
public:
	PgSuperGround();
	virtual ~PgSuperGround();

	virtual EOpeningState Init( int const iMonsterControlID=0, bool const bMonsterGen=true );
	bool Clone( PgSuperGround* pkGround );
	virtual bool Clone( PgGround* pkGround );
	virtual void Clear();
	virtual void OnTick1s();
	virtual void SetState( EIndunState const eState, bool bAutoChange=false, bool bChangeOnlyState=false);

	virtual EGroundKind GetKind()const{	return GKIND_SUPER;	}
	virtual T_GNDATTR GetAttr()const;
	virtual int GetGroundNo()const;
	virtual int GetGroundItemRarityNo()const;
	virtual void CheckEnchantControl(PgBase_Item& rkItem)const;

	virtual HRESULT InsertMonster(TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, int const iMonNo, BM::GUID &rkOutGuid, CUnit* pkCaller=NULL, bool bDropAllItem = false, int iEnchantGradeNo = 0, ECREATE_HP_TYPE const eCreate_HP_Type = ECHT_NONE);
	bool NextFloor(int iNextFloor, int const iSpawnNo, bool const bForce = false);
	void AddFloor(ContFloorResource& rkContFloorResource);
	void SetSuperGroundMode(int const iSuperGroundNo, ESuperGroundMode const eMode, size_t const iFloorIdx);
	void UseSuperGroundFeather(CUnit* pkCasterUnit, int const iHPRate, int const iMPRate);

protected:
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual void WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const;
	virtual void RecvUnitDie(CUnit *pkUnit);
	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );

	CLASS_DECLARATION_NO_SET(size_t, m_iNowFloor, NowFloor);
	CLASS_DECLARATION_NO_SET(DWORD, m_dwTimeLimit, TimeLimit);
	CLASS_DECLARATION_NO_SET(DWORD, m_dwAccumLimitTime, AccumLimitTime);
	CLASS_DECLARATION_NO_SET(size_t, m_iLastRemainMonsterCount, LastRemainMonsterCount);
	CLASS_DECLARATION_NO_SET(ContGuidSet, m_kContNotCountMonster, NotCountMonster);
	CLASS_DECLARATION_NO_SET(bool, m_bFailedTime, FailedTime);
private:
	ContFloorResource m_kGndResource;
	int m_iSuperGroundNo;
	ESuperGroundMode m_eMode;
	CONT_SUPER_GROUND m_kContSuperGndInfo;
	PgSuperGroundTimeEventMgr m_kTimeEventMgr;
};

#endif // MAP_MAPSERVER_MAP_GROUND_PGSUPERGROUND_H