#ifndef WEAPON_VARIANT_USERINFO_PGQUICKINVENTORY_H
#define WEAPON_VARIANT_USERINFO_PGQUICKINVENTORY_H

#include <vector>
#include "Item.h"
#include "DefAbilType.h"

size_t const Q_INV_COUNT = 8;//화면에 나오는 갯수
size_t const Q_INV_LINE_COUNT = 7;//몇줄?
size_t const Q_INV_MAX_COUNT = Q_INV_COUNT * Q_INV_LINE_COUNT;//몇줄?

#pragma pack(1)

	typedef struct tagQuickInvInfo
	{//
		tagQuickInvInfo();
		void Clear();
		void Set(BYTE const byInGrp, int const iInID);
		bool IsEmpty()const;
		
		CLASS_DECLARATION_S(BYTE, Grp);//그룹
		CLASS_DECLARATION_S(int, ID);//번호
	}SQuickInvInfo;

#pragma pack()

class PgQuickInventory
{//
	typedef enum
	{
		MIN_PAGE = 0,
		MAX_PAGE = 6,//8개씩 6페이지. 0~5 페이지로 본다.
		EXTEND_PAGE = Q_INV_LINE_COUNT,
	}eValue;
public:
	PgQuickInventory();
	virtual ~PgQuickInventory();

	PgQuickInventory( PgQuickInventory const &rhs );
	PgQuickInventory& operator=( PgQuickInventory const &rhs );
public:
	void Clear();
	HRESULT Init(BYTE const abyQuickSlot[MAX_DB_QUICKSLOT_SIZE]);//디비 데이터로 초기화
	void Swap(PgQuickInventory &rhs);

	HRESULT GetItem(size_t const slot_idx, SQuickInvInfo &rkOutItem)const;
	HRESULT FindEmptySlot(size_t &rkResult)const;

	HRESULT Modify(size_t const slot_idx, SQuickInvInfo &rkInfo);
	HRESULT Modify(char const cViewPage);//서버에서만.
	HRESULT DeleteItem(size_t const slot_idx);
	HRESULT IsExist(int const iItemNo);

	HRESULT Save(BYTE *abyDBData)const;

	bool IsCorrectIDX(size_t const slot_idx)const;

	size_t AdjustSlotIDX(size_t const slot_idx)const;// -> 0~8 까지 넣으면 페이지 감안한 인덱스를 돌려줌

	void WriteToPacket(BM::Stream &rkPacket) const;
	void ReadFromPacket(BM::Stream &rkPacket);
	
public:
	int ViewPage()const;
	int IncViewPage();//결과 페이지 값 줌.
	int DecViewPage();//결과 페이지 리턴

protected:
	int AdjustViewPage();

protected:

	typedef std::vector< SQuickInvInfo > SLOT_CONT;
	SLOT_CONT m_kBuffer;
	
	char m_kViewPage;//short 를 바꾸면 memcpy 때문에 문제 날 수 있음.
};

#endif // WEAPON_VARIANT_USERINFO_PGQUICKINVENTORY_H