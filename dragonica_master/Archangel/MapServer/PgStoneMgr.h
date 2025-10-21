#ifndef MAP_MAPSERVER_MAP_PGSTONEMGR_H
#define MAP_MAPSERVER_MAP_PGSTONEMGR_H

#include "Global.h"
//typedef std::map< SGroundKey, ContStoneDataVec > ContStoneData;
typedef std::list< BM::GUID > CONT_STONE_CANDIDATE;//Player stone list// GUID 만 쥐자. 먹을때 주 속성으로 아이템 만들면 되니까.
typedef std::map< BM::GUID, CONT_STONE_CANDIDATE > CONT_PLAYER_STONE;//Character Guid, Personal Stone list

//////////////////////////////////////////////////////
//		PgStoneMgr
class PgStoneControl
{//그라운드가 소유한 개체라서, 락을 안함. 쓰기전 그라운드가 락을 잡기 때문.
public:
	PgStoneControl();
	~PgStoneControl();

	void OnTick();//티켓 예약.
	void Init(TBL_DEF_MAP_STONE_CONTROL const &kTbl);

	bool AddData(SStoneRegenPoint const &kStoneRegenPoint);//스톤 리젠 포인트 추가.
	bool TryCatchStoneTicket(CUnit *pkPlayer);//유저가 티켓 받기 시도.
	bool PlayerStoneUpdate(CUnit *pkUnit, SGroundKey const & rkGndKey, BM::GUID const &rkStoneGuid);
	bool RemoveTicket(CUnit *pkPlayer);

	bool WriteToPacket(BM::GUID const &rkCharGuid, BM::Stream &rkPacket)const;//티켓 먹은놈에게 패킷 쏘기.
	typedef std::set< BM::GUID > CONT_CUSTOMER;//

protected:
	typedef std::vector< SStoneRegenPoint > CONT_STONE_REGEN_POINT;
	
	CONT_STONE_REGEN_POINT m_kStoneRegenPoint;// this map's stone regen point
	CONT_RESERVED_STONE_LIST m_kContReservedStone;//할당받은 유저의 스톤 정보.
	CONT_CUSTOMER m_kContCustomer;

	TBL_DEF_MAP_STONE_CONTROL m_kTblData;
	int m_aiRemainTicketCount[MAX_STONE_TICKET];
	DWORD m_dwTickTime;//틱에 이미 받은 후보에서 지워야 하기 때문에.
};


//#define g_kShineStoneMgr SINGLETON_STATIC(PgStoneMgr)

#endif // MAP_MAPSERVER_MAP_PGSTONEMGR_H