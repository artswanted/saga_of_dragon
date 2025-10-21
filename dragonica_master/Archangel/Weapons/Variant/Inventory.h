#ifndef WEAPON_VARIANT_USERINFO_INVENTORY_H
#define WEAPON_VARIANT_USERINFO_INVENTORY_H

#include <vector>
#include "constant.h"
#include "Item.h"
#include "DefAbilType.h"
#include "PgDefPropertyMgr.h"
#include "BaseItemContainer.h"

class CUnit;
class PgPlayer;

typedef std::vector<SPMOD_Modify_Count> CONT_MODIFY_COUNT;
typedef std::vector< SItemPos > CONT_EMPTY_ITEM_POS;

typedef std::map<DWORD, int> CONT_SKILL_OPT;

#define MAX_INV_MONEY 9999999999999i64

typedef struct tagItemActionEffect
{
	int iEffectNo;
	int iRate;
	int iTarget;
	tagItemActionEffect():iEffectNo(0),iRate(0),iTarget(0){}
	explicit tagItemActionEffect(int const __effectno,int const __rate,int const __target):iEffectNo(__effectno),iRate(__rate),iTarget(__target){}
	bool operator < (tagItemActionEffect const & rhs) const
	{
		return iEffectNo < rhs.iEffectNo;
	}
}tagItemActionEffect;

typedef enum E_ITEM_ACTION_EVENT
{
	IAE_N_ATTACK	= 1,	// 일반 공격
	IAE_C_ATTACK	= 2,	// 크리티컬 공격
	IAE_BEATTACKED	= 3,	// 피격
	IAE_BLOCKED		= 4		// 블러킹
}EItemActionEvent;

class PgInventory
	: public CAbilObject
{//아이템을 넣거나 뺄 수 있는지 확인은 아이템서버에 문의해서 받는 방식으로 하라.
public:

	typedef std::vector<BYTE> CONT_REDUCTION;
	typedef std::map<BM::GUID,BM::PgPackedTime>	CONT_UNBINDTIME;

	typedef struct tagSItemCoolTimeInfo
	{
		tagSItemCoolTimeInfo()
		{
			byCoolTimeType = 0;
			dwEndCoolTime = 0;
			dwTotalCoolTime = 0;
			bSaveDB = false;
		}
		BYTE byCoolTimeType;	// EItemCoolTimeType value
		DWORD dwEndCoolTime;
		DWORD dwTotalCoolTime;
		bool bSaveDB;
	} SItemCoolTimeInfo;

	typedef std::map<int, SItemCoolTimeInfo> CONT_ITEM_COOLTIME;	// <ItemNo, SItemCoolTimeInfo>
	typedef PgBaseItemContainer ItemContainer;
	typedef std::set<int> CONT_SETITEM_NO;	
	typedef std::map< EItemActionEvent, std::set<tagItemActionEffect> > CONT_ITEMACTIONEFFECT;
	
public:
	PgInventory(void);
	virtual ~PgInventory(void);

	////////////서버 구조 바뀌면서 추가된 아이템
	virtual void Init( BYTE const * pkInvExtern,BYTE const * pkExternIdx);
	void InsertItemList( CONT_SELECTED_CHARACTER_ITEM_LIST const &kItemList );
	
	// 주)Swap : OwnerGuid와 OwnerName은 Swap하지 않는다.(오너를 제외한 아이템 내용만 교체한다 : 옵션으로 오너교체기능 제공)
	void Swap( PgInventory &kInv, bool const bSwapOwner = false );
	
	PgInventory(PgInventory const &rhs);
	HRESULT MergeItem(PgItemWrapper &kCasterWrapper, PgItemWrapper &kTargetWrapper, __int64 & iCause);
	HRESULT AddCP(const int kValue, bool const bIsTest);
	HRESULT AddMoney(const  __int64 kValue, bool const bIsTest);

	HRESULT SetItem(PgBase_Item const & kItem, SItemPos const & kPos);
	HRESULT InsertItem(PgBase_Item const & kItem, SItemPos const & kPos);
	HRESULT InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, bool const bIsTest, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);//Wrapped가 있는상태
	HRESULT OverWriteItem(__int64 const iCause, PgItemWrapper const &kInItem, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);//이 랩퍼로 덮어쓴다
	HRESULT ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	HRESULT ModifyItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	HRESULT AddItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	HRESULT RemoveItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	HRESULT PopItem(__int64 const iCause, SItemPos const &rkPos, PgItemWrapper& rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	
	void ProcessTimeOutedItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, PgPlayer const * pkPlayer );
	void ProcessTimeOutedMonsterCard(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, CUnit const * pkUnit );
	void ProcessUnbindTimeItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder);

	HRESULT SetRentalSafeEnableTime(EInvType const kInvType,BM::DBTIMESTAMP_EX const & kDate);
	HRESULT SetRentalSafeEnableTime(EInvType const kInvType,__int64 const i64RentalTime);
	HRESULT CheckEnableUseRentalSafe(EInvType const kInvType);

	__int64 const GetRentalSafeUseTime(EInvType const kInvType);

	bool HasUnbindDate(BM::GUID const & kItemGuid);
	bool AddUnbindDate(BM::GUID const & kItemGuid,BM::DBTIMESTAMP_EX const & kUnbindDate);
	bool DelUnbindDate(BM::GUID const & kItemGuid);

	HRESULT GetActivateExpCardItem(PgBase_Item & kItem, SItemPos & kItemPos);

	size_t GetHomeEquipItemCount();
	void SaveGroupCoolTime(DWORD const dwCurTime, BYTE (&abyItemGroupCoolTime)[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE]) const;
	void LoadGroupCoolTime(DWORD const dwLogInGapTime, BYTE const (&abyItemGroupCoolTime)[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE]);

public:

	virtual HRESULT RefreshAbil(CUnit const * pkUnit);
	virtual int GetAbil(WORD const wAbilType) const;
	int GetRemovedAmplyAbil(WORD const wAbilType) const;

	void Clear();
	void Clear(EInvType const eInvType);
	void WriteToPacket(BM::Stream &kPacket, EWRITETYPE const kWriteType);
	void ReadFromPacket(BM::Stream &kPacket, EWRITETYPE const kWriteType);

	bool WriteToPacket(EInvType const eInvType, BM::Stream &kPacket);
	bool ReadFromPacket(EInvType const eInvType, BM::Stream &kPacket);

	bool Modify(SItemPos const &kPos, PgBase_Item const &kItem);

	HRESULT SwapItem( SItemPos const &rPos, PgBase_Item &kItem, bool const bSameID=true );

	HRESULT GetItemID( SItemPos const &rPos, BM::GUID &kOutItemID );
	HRESULT GetItem(SItemPos const &rPos, PgBase_Item &kOutItem);
	HRESULT GetItem(BM::GUID const &rGuid, PgBase_Item &kOutItem, bool const bIsForQuickSlot = true);
	HRESULT GetItem(BM::GUID const &rGuid, PgBase_Item &kOutItem, SItemPos & kOutPos, bool const bIsForQuickSlot = true);
	HRESULT GetItem(EInvType const eInvType, BYTE const kPos, PgBase_Item &kOutItem);

	HRESULT GetFirstItem(int const iItemNo, SItemPos &rkRetPos, bool const bIsForQuickSlot = true, bool const bChkTimeOut = false );
	HRESULT GetNextItem(int const iItemNo, SItemPos &rkRetPos);
	HRESULT GetFirstItem(EInvType const eInvType, int const iItemNo, SItemPos& rkRetPos, bool const bChkTimeOut = false );

	HRESULT GetItemModifyOrder( EInvType const eInvType, int const iItemNo, size_t iSize, CONT_PLAYER_MODIFY_ORDER *pkOrder );

	size_t GetInvTotalCount(int const iItemNo)const;
	size_t GetTotalCount(int const iItemNo, bool const bIsForQuickSlot = true)const;
	bool GetFirstEmptyPos(EInvType eInvType, SItemPos& rkOutPos);	// 비어있는 첫번째 Slot 찾기
	bool GetNextEmptyPos(EInvType eInvType, SItemPos& rkOutPos);		// rkOutPos 부터 비어있는 첫번째 Slot 찾기

	HRESULT GetItems(EInvType const eInvType, CONT_ITEM_CREATE_ORDER &rOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const eInvType, CONT_HAVE_ITEM_DATA &rOut, bool const bChkTimeOut = false ); // 중복된 아이템번호는 무시됩니다.
	HRESULT GetItems(EInvType const eInvType, CONT_HAVE_ITEM_DATA_ALL &rOut, bool const bChkTimeOut = false ); // 중복된 아이템번호는 무시됩니다.
	HRESULT GetItems(EInvType const eInvType, CONT_HAVE_ITEM_NO &rOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const eInvType, ContHaveItemNoCount &rkOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const eInvType, CONT_HAVE_ITEM_POS &rOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EUseItemCustomType const eCustomType, ContHaveItemNoCount& rkOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const kInvType,EUseItemCustomType const eCustomType, ContHaveItemNoCount& rkOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const eInvType, EUseItemCustomType const eCustomType, CONT_HAVE_ITEM_DATA &rOut, bool const bChkTimeOut = false );
	HRESULT GetItems(EInvType const eInvType, EUseItemCustomType const eCustomType, CONT_HAVE_ITEM_POS &rOut, bool const bChkTimeOut = false );

	int GetItemEnchantLevel(SItemPos const& rkPos); //장착 부위의 인챈트 레벨은 반환

	EPropertyType	ItemAttr(EEquipPos const &ePos) const;//장착된 부위에서 속성을 뽑아냄.
	int				ItemAttrLevel(EEquipPos const &ePos) const;//장착된 부위에서 속성을 뽑아냄.

	HRESULT CanInsert(PgBase_Item const &kItem);
	int const GetMaxIDX(EInvType eInvType);
	int const GetEmptyPosCount(EInvType eInvType); // 비어있는 슬롯의 개수 얻기
	CONT_SETITEM_NO const & GetFullSetItemNo() const { return m_kFullSetItem; }
	void Money(__int64 const iNewMoney);	// Money Max값 제약 걸기 위해 필요
	
	typedef enum : BYTE
	{
		EICool_UseItem = 0x01,
		EICool_Equip = 0x02,
		EICool_Unequip = 0x04,
	} EItemCoolTimeType;

	bool CheckCoolTime(int const iItemNo, EItemCoolTimeType const eType, DWORD const dwCurrentTime) const;
	bool GetCoolTime(int const iItemNo,SItemCoolTimeInfo & kCoolTime) const;
	void AddCoolTime(int const iItemNo, EItemCoolTimeType const eType, DWORD const dwCurrentTime,int const iCoolTimeRate);
	void ClearCoolTime();
	void ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate);

	bool const GetReductionInfo2Bin(BYTE * const pkReductionInfo) const;
	bool const GetExtendIdx2Bin(BYTE * const pkReductionInfo) const;

	BYTE const GetInvExtendSize(EInvType const kInvType);
	BYTE const GetEnableExtendMaxIdx(EInvType const kInvType);

	bool const InvExtend(EInvType const kInvType,BYTE const bReductionNum);
	bool const ExtendMaxIdx(EInvType const kInvType,BYTE const bIncIdxNum);

	HRESULT GetSelectedPetID( BM::GUID &rkOutID );
	HRESULT GetSelectedPetItem( PgBase_Item &rkPetItem );
	size_t GetPetItemCount(void);

	void GetOverSkillOption(CONT_SKILL_OPT& kSkillOpt)const;

	HRESULT MakeSortOrder(EInvType const kInvType, CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder);

	size_t GetItemCount(EInvType const eInvType);

protected:

	void ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate,CONT_ITEM_COOLTIME * pkCont);

	void GetReductionInfo2Bin(ItemContainer const * pkCont,CONT_REDUCTION & kCont) const;
	void GetExtendMaxIdx2Bin(ItemContainer const * pkCont,CONT_REDUCTION & kCont) const;

	HRESULT GetFirstItem(int const iItemNo, SItemPos &rkRetPos, ItemContainer const *pkCont, bool const bChkTimeOut );
	HRESULT GetContainer(EInvType const eInvType, ItemContainer *&pkCont);
	size_t GetTotalCount(int const iItemNo, ItemContainer const *pkCont)const;

	void ReadFromPacket(BM::Stream &kPacket, ItemContainer *pkCont);
	void WriteToPacket(BM::Stream &kPacket, ItemContainer *pkCont);
	
	HRESULT GetItem(BM::GUID const & rGuid, SItemPos &rkRetPos, ItemContainer const *pkCont);

	void AddOverSkillOption(int const iSkillNo, int const iAddLevel);	

	PgRentalSafeItemContainer * GetRentalSafe(EInvType const kInvType);

protected:
////////////////////////
// 디비 저장 되어야함.
	PgEquipItemContainer m_kEquipInv;
	PgConsumeItemContainer m_kConsumeInv;
	PgEtcItemContainer m_kEtcInv;
	PgCashItemContainer m_kCashInv;
	PgSafeItemContainer m_kSafeInv;

	PgCashSafeItemContainer m_kCashSafeInv;
	PgHomeItemContainer m_kHomeInv;
	PgFitItemContainer m_kFit;//입고있다.
	PgPostItemContainer	m_kPostInv;
	PgUserMarketItemContainer m_kUserMarketInv; 
	PgFitCashItemContainer m_kFitCash;//입고있다.
	PgFitCostumeItemContainer m_kFitCostume; // Costume, update 03.08.2018

	PgRentalSafeItemContainer_1 m_kRentalSafe1;
	PgRentalSafeItemContainer_2 m_kRentalSafe2;
	PgRentalSafeItemContainer_3 m_kRentalSafe3;

	PgShareRentalSafeItemContainer_1 m_kShareRentalSafe1;
	PgShareRentalSafeItemContainer_2 m_kShareRentalSafe2;
	PgShareRentalSafeItemContainer_3 m_kShareRentalSafe3;
	PgShareRentalSafeItemContainer_4 m_kShareRentalSafe4;

	PgPetItemContainer	m_kPetItem;

	PgSafeAddItemContainer m_kSafeAddInv;

	CONT_UNBINDTIME				m_kContUnbindTime;

	CLASS_DECLARATION_S_NO_SET(__int64, Money);
	CLASS_DECLARATION_S(int, CP);
	CLASS_DECLARATION_S(BM::GUID, OwnerGuid);
	CLASS_DECLARATION_S(BM::GUID, MemberGuid);
	CLASS_DECLARATION_S(std::wstring, OwnerName);
	CLASS_DECLARATION_S(BM::GUID, ExpCardItem);

// ///////////////////////////위는 동기화 패킷으로 감.

	CONT_ITEM_COOLTIME m_kItemCoolTime;
	CONT_ITEM_COOLTIME m_kGroupCoolTime;

	CONT_SETITEM_NO m_kFullSetItem;

	CONT_SKILL_OPT	m_kOverSkill;

	CLASS_DECLARATION_S(CONT_ITEMACTIONEFFECT, ItemActionEffect);
	
	CAbilObject m_kAmplyOptionValue;	// PvP시 제거되어야할 어빌들(옵션 증폭으로 증가된 어빌들)
};

extern void CastSDBItemDataToSItem(SDBItemData const & kDBItemData, PgBase_Item&  kItem, SItemPos & kItemPos);

#endif // WEAPON_VARIANT_USERINFO_INVENTORY_H