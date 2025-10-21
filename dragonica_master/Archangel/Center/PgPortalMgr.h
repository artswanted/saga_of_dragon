#ifndef CENTER_CENTERSERVER_CONTENTS_PGPORTALMGR_H
#define CENTER_CENTERSERVER_CONTENTS_PGPORTALMGR_H

#include "PgTask_Contents.h"
#include "Variant/PgConstellation.h"

typedef enum
{
	PORTAL_DELAY,
	PORTAL_NODELAY,
	PORTAL_PVP,
	PORTAL_MISSION,
	PORTAL_SUPERGND,
}E_PORTAL_TYPE;

typedef struct tagPortalWaiter
{
	typedef std::map< BM::GUID, BM::GUID >		CONT_WAIT_LIST;// first: charguid, second:volatile_id

	tagPortalWaiter()
	{
		Clear();
	}

	tagPortalWaiter( BYTE const byPortalType )
		:	byType(byPortalType)
		,	dwRegistTime(0)
		,	bReq(false)
		,	iOwnerLv(0)
	{}

	tagPortalWaiter( SReqMapMove_MT const &_kRMM, BYTE const byPortalType=PORTAL_DELAY )
		:	kRMM(_kRMM)
		,	byType(byPortalType)
		,	dwRegistTime(0)
		,	bReq(false)
		,	iOwnerLv(0)
	{}

	void Clear()
	{
		kRMM.Clear();
		constellationMission.Clear();
		kWaiterList.clear();
		byType = PORTAL_DELAY;
		dwRegistTime = 0;
		bReq = false;
		iOwnerLv = 0;
		ClearAddData();
	}

	void ClearAddData()
	{
		m_kContPetMoveData.clear();
		m_kContUnitSummonedMoveData.clear();
		m_kModifyOrder.clear();
	}

	void Set( SERVER_IDENTITY const &kTargetSI, SGroundKey const &kTargetGroundKey )
	{
		kRMM.kTargetSI = kTargetSI;
		kRMM.kTargetKey = kTargetGroundKey;
	}

	void Add( BM::GUID const &kCharGuid, BM::GUID const &kVolatileID )
	{
		kWaiterList.insert( std::make_pair( kCharGuid, kVolatileID ) );
	}

	void Add( tagPortalWaiter const &rhs )
	{
		kWaiterList.insert( rhs.kWaiterList.begin(), rhs.kWaiterList.end() );
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push(byType);
		kPacket.Push(kRMM);
		constellationMission.WriteToPacket(kPacket);
		kPacket.Push(iOwnerLv);
		PU::TWriteTable_AA( kPacket, kWaiterList );
		PU::TWriteTable_AM( kPacket, m_kContPetMoveData );
		PU::TWriteTable_AM( kPacket, m_kContUnitSummonedMoveData );
		m_kModifyOrder.WriteToPacket(kPacket);
	}

	void ReadFromPacket( BM::Stream &kPacket )
	{
		kWaiterList.clear();
		kPacket.Pop(byType);
		kPacket.Pop(kRMM);
		constellationMission.ReadFromPacket(kPacket);
		kPacket.Pop(iOwnerLv);
		PU::TLoadTable_AA( kPacket, kWaiterList );
		
		ClearAddData();
		PU::TLoadTable_AM( kPacket, m_kContPetMoveData );
		PU::TLoadTable_AM( kPacket, m_kContUnitSummonedMoveData );
		m_kModifyOrder.ReadFromPacket(kPacket);
	}

	SReqMapMove_MT	kRMM;
	Constellation::SConstellationMission constellationMission;
	CONT_WAIT_LIST	kWaiterList;
	BYTE			byType;				// Portal »эјє ЕёАФ
	DWORD			dwRegistTime;
	bool			bReq;
	int				iOwnerLv;
	CONT_PET_MAPMOVE_DATA		m_kContPetMoveData;
	CONT_UNIT_SUMMONED_MAPMOVE_DATA	m_kContUnitSummonedMoveData;
	CONT_PLAYER_MODIFY_ORDER	m_kModifyOrder;
}SPortalWaiter;

class PgPortalMgr
{
public:
	typedef std::map< SGroundKey, SPortalWaiter* >		CONT_CREATE_WAITER;
	typedef std::queue< SPortalWaiter* >				CONT_TIME_WAITER;

	typedef BM::TObjectPool<SPortalWaiter>				PoolWaiter;

public:
	PgPortalMgr();
	~PgPortalMgr();

	bool ProcessMsg(SEventMessage *pkMsg);

	HRESULT Respone( SGroundKey const &kKey, HRESULT const hResult );
	void OnTick( DWORD const dwCurTime );

private:
	HRESULT Regist( SPortalWaiter *pkWaiter, BM::Stream * const pkAddPacket = NULL );
	HRESULT ReqCreateGround( SGroundKey const &kGndKey, BM::Stream * const pkAddPacket = NULL );
	HRESULT ReqCreateGround( SPortalWaiter *pkWaiter, BM::Stream * const pkAddPacket = NULL );

private:
	mutable Loki::Mutex		m_kMutex;

	CONT_CREATE_WAITER		m_kContCreateWaiter;
	CONT_TIME_WAITER		m_kContTimeWaiter;

	PoolWaiter				m_kPoolWaiter;
	DWORD					m_dwOldCheckTime;
};

#define g_kPortalMgr SINGLETON_STATIC(PgPortalMgr)

#endif // CENTER_CENTERSERVER_CONTENTS_PGPORTALMGR_H