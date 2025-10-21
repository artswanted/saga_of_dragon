#ifndef WEAPON_VARIANT_UNIT_PGGROUNDITEMBOX_H
#define WEAPON_VARIANT_UNIT_PGGROUNDITEMBOX_H

#include <vector>

#include "BM/ClassSupport.h"
#include "BM/point.h"
#include "BM/guid.h"
#include "item.h"
#include "Unit.h"

class PgGroundItemBox
	:	public CUnit
{
public:
	static DWORD const ms_GROUNDITEMBOX_DURATION_TIME = 30000;
	static DWORD const ms_INSTANCEITEM_DURATION_TIME = 20000;

	PgGroundItemBox(void);
	virtual ~PgGroundItemBox(void);
public:
	bool AddItem(PgBase_Item const &kItem);
	HRESULT PopItem(PgBase_Item & kItem, bool const bIsTest);
	bool AddMoney(int const iMoney);

	size_t ElementSize()const{return m_kItemArray.size();}

	bool Owner(VEC_GUID const& rkOwners);
	bool IsOwner(BM::GUID const &rkGuid);

protected:
	virtual void Init();
	virtual HRESULT Create(const void* pkInfo) { return E_FAIL; };
	virtual EUnitType UnitType()const{ return UT_GROUNDBOX; }
	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
//	virtual void WriteToPacket(BM::Stream &rkPacket, bool const bIsSimple, bool const bIsForSave)const;
//	virtual void ReadFromPacket(BM::Stream &rkPacket, bool &bIsSimple, bool &bIsForSave);
	virtual void Send(BM::Stream const &Packet, E_SENDTYPE_TYPE eType = E_SENDTYPE_SELF);
	virtual void Invalidate();
	virtual bool IsCheckZoneTime(DWORD dwElapsed);
	virtual void SendAbil(const EAbilType eAT, E_SENDTYPE_TYPE eType = E_SENDTYPE_SELF);
	virtual int Tick(unsigned long ulElapsedTime, SActArg *pActArg);
	virtual void SendAbiles(WORD const* pkAbils, BYTE byNum, E_SENDTYPE_TYPE eType = E_SENDTYPE_SELF);
	virtual int CallAction(WORD wActCode, SActArg *pActArg);
	virtual void SendAbil64(const EAbilType eAT, E_SENDTYPE_TYPE eType = E_SENDTYPE_SELF);

	int AdjustImage();


	virtual void VOnDie(){CUnit::VOnDie();}
protected:
	CLASS_DECLARATION_S(int, ActionInstanceID);//아이템을 떨어뜨리게 된 원인이 되는 ActionInstanceID (클라이언트에서 DropItem의 연출을 위해 필요하다.)
	CLASS_DECLARATION_S(BYTE, OwnerType);//아래 Guid 주인의 타입
	VEC_GUID m_kOwners;//주인들
	CLASS_DECLARATION_S(DWORD, CreateDate);//생성 날짜.
	std::list< PgBase_Item > m_kItemArray;

	CLASS_DECLARATION_S(int, Money);//떨어진돈.
	CLASS_DECLARATION_S(bool, IsInstanceItem);	// 인스턴트 아이템인가?
	CLASS_DECLARATION_S(bool, IsDropBearItem);	// 이동 도중 플레이어가 떨어뜨리는 곰 아이템인가?(러브러브모드 전용)
	CLASS_DECLARATION_S(WORD, CollectRemainTime);	// 플레이어가 떨어뜨린 인스턴트 아이템이 회수될 때 까지 남은 시간. (러브러브모드 전용).
	CLASS_DECLARATION_S(int, Team);
	CLASS_DECLARATION_S(TBL_DEF_MAP_REGEN_POINT, GenInfo);
};

#endif // WEAPON_VARIANT_UNIT_PGGROUNDITEMBOX_H