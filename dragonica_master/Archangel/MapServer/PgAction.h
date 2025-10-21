#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTION_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTION_H

#include "Lohengrin/PacketStruct4Map.h"
#include "Lohengrin/DBTables.h"
#include "Variant/PgDefSpendMoney.h"
#include "Variant/PgMission.h"
#include "Variant/PgMissionInfo.h"
#include "Variant/PgEmporia.h"
#include "PgLocalPartyMgr.h"
#include "PgItemTradeMgr.h"
#include "Global.h"

enum E_ITEM_REMOVE_TYPE
{
	IRT_NORMAL_REMOVE = 0,
	IRT_BREAK_REMOVE,
	IRT_UPGRADE_REMOVE,
};

typedef std::set< int > ContItemNoSet;

class CUnit;
class PgNPC;
class PgGround;

class PgUtilAction
{
public:
	PgUtilAction(void){}
	virtual ~PgUtilAction(void){}

public:
	virtual bool DoAction(CUnit* pUnit1, CUnit* pUnit2) = 0;
};

class PgAction_ReqAddMoney
	:	public PgUtilAction
{//ModifyItem 에서도 돈을 바꿀 수 있긴함.
public:
	explicit PgAction_ReqAddMoney(EItemModifyParentEventType const kCause, __int64 const iAddMoney, SGroundKey const &kGroundKey, BM::Stream const& rkPacket = BM::Stream());
	virtual ~PgAction_ReqAddMoney(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	EItemModifyParentEventType const m_kCause;
	__int64 const m_iAddMoney;
	SGroundKey m_kGndKey;

	BM::Stream const& m_kPacket;

private:// Not Use
	PgAction_ReqAddMoney();
	PgAction_ReqAddMoney( PgAction_ReqAddMoney const& );
};

class PgAction_StoreItemBuyResult
	:	public PgUtilAction
{
public:
	explicit PgAction_StoreItemBuyResult(HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, __int64 const iMoney);
	virtual ~PgAction_StoreItemBuyResult(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	HRESULT const m_hRet;
	DB_ITEM_STATE_CHANGE_ARRAY const &m_kChangeArray;
	__int64 const m_iMoney;

private:// Not Use
	PgAction_StoreItemBuyResult();
	PgAction_StoreItemBuyResult( PgAction_StoreItemBuyResult const& );
};

class PgAction_StoreItemSellResult
	:	public PgUtilAction
{
public:
	explicit PgAction_StoreItemSellResult(HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, __int64 const iMoney);
	virtual ~PgAction_StoreItemSellResult(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	HRESULT const m_hRet;
	DB_ITEM_STATE_CHANGE_ARRAY const &m_kChangeArray;
	__int64 const m_iMoney;

private:// Not Use
	PgAction_StoreItemSellResult();
	PgAction_StoreItemSellResult( PgAction_StoreItemSellResult const& );
};

class PgAction_ReqStoreItemBuy
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqStoreItemBuy(BM::GUID const &kReqShopNo, BYTE const byType, BYTE const bySecondType, int const iItemNo, int const iCount, SGroundKey const &kGroundKey, PgEventAbil const * const pkEventAbil );
	virtual ~PgAction_ReqStoreItemBuy(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	BM::GUID const m_kReqShopGuid;//NPC Guid와 동일
    BYTE const m_byType;
	BYTE const m_bySecondType;
	int const m_iItemNo;
	int const m_iCount;

	SGroundKey m_kGndKey;
	PgEventAbil const * const m_pkEventAbil;

private:// Not Use
	PgAction_ReqStoreItemBuy();
	PgAction_ReqStoreItemBuy( PgAction_ReqStoreItemBuy const& );
};

class PgAction_ReqStoreItemSell
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqStoreItemSell(SItemPos const &kItemPos, int const iItemNo, int const iCount, SGroundKey const &kGroundKey, BM::GUID const &kShopGuid, BYTE const byType, bool const bIsStockShop = false);
	virtual ~PgAction_ReqStoreItemSell(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SItemPos const m_kItemPos;
	int const m_iItemNo;
	int m_iCount;
	BM::GUID const m_kShopGuid;
    BYTE const m_byType;
	bool m_bIsStockShop;

	SGroundKey m_kGndKey;

private:// Not Use
	PgAction_ReqStoreItemSell();
	PgAction_ReqStoreItemSell( PgAction_ReqStoreItemSell const& );
};

class PgAction_ReqStoreItemList
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqStoreItemList(BM::GUID const &kReqShopGuid, BYTE const byType=0);
	virtual ~PgAction_ReqStoreItemList(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	const BM::GUID m_kReqShopGuid;
    const BYTE m_byType;

private:// Not Use
	PgAction_ReqStoreItemList();
	PgAction_ReqStoreItemList( PgAction_ReqStoreItemList const& );
};

class PgAction_SendItemChange
	:	public PgUtilAction
{
public:
	explicit PgAction_SendItemChange(EItemModifyParentEventType const kCause, SGroundKey const& kGndKey, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, PgLocalPartyMgr &kLocalPartyMgr);
	virtual ~PgAction_SendItemChange(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	CLASS_DECLARATION_S(EItemModifyParentEventType, Cause);
	CLASS_DECLARATION_S(SGroundKey, GndKey);

private:
	DB_ITEM_STATE_CHANGE_ARRAY const &m_kChangeArray;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_SendItemChange();
	PgAction_SendItemChange( PgAction_SendItemChange const& );
};

class PgEventItemSetMgr;
class PgAction_ModifyPlayerData
	:	public PgUtilAction
{
	typedef std::vector< PgBase_Item > CONT_ITEM_VEC;
public:
	explicit PgAction_ModifyPlayerData(EItemModifyParentEventType const kCause, HRESULT const hRet, DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray,
		PgGround* const pkGround, PgLocalPartyMgr &kLocalPartyMgr, PgEventItemSetMgr &rkEventItemSetMgr, BM::Stream const &kAddonPacket = BM::Stream());
	virtual ~PgAction_ModifyPlayerData(){}

	PgAction_ModifyPlayerData( PgAction_ModifyPlayerData const& rhs );

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	SGroundKey const& GndKey(void)const;

protected:
	void ResultProcess(CUnit* pkCaster, CUnit* pkTarget);
	void AddNotifyItem(DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkItemChange, CONT_ITEM_VEC& rkVec);
	void NotifyPartyMember(CONT_ITEM_VEC const &rkItemVec, CUnit* pkCaster, CUnit* pkTarget);
	void DoEquipEffect(CUnit* pkCaster, DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkItemChange);
	bool AchievementProcess(int iAbilType,int iValue,CUnit* pkCaster);
	void CheckAchievementEquipItem(CUnit * pUnit);

	CLASS_DECLARATION_S(EItemModifyParentEventType, Cause);
	CLASS_DECLARATION_S(HRESULT, Ret);
private:
	DB_ITEM_STATE_CHANGE_ARRAY const &m_kChangeArray;
	BM::Stream m_kAddonPacket;
	PgGround * const m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;
	PgEventItemSetMgr &m_kEventItemSetMgr;

private:// Not Use
	PgAction_ModifyPlayerData();
	
};

class PgAction_ModifyItem_AddOnPacketProcess
	:	public PgUtilAction
{
public:
	explicit PgAction_ModifyItem_AddOnPacketProcess(HRESULT const _hRet, PgGround * const pkGround, BM::Stream const &kAddonPacket = BM::Stream());
	virtual ~PgAction_ModifyItem_AddOnPacketProcess(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
private:

	BM::Stream m_kAddonPacket;
	PgGround * const m_pkGround;
	CLASS_DECLARATION_S(HRESULT, Ret);

private:// Not Use
	PgAction_ModifyItem_AddOnPacketProcess();
	PgAction_ModifyItem_AddOnPacketProcess( PgAction_ModifyItem_AddOnPacketProcess const& );
};


class PgAction_ItemShareCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_ItemShareCheck(PgLocalPartyMgr &kLocalPartyMgr);
	virtual ~PgAction_ItemShareCheck() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	CLASS_DECLARATION_S(EPartyOptionItem, OptionItem);
	CLASS_DECLARATION_S(EExpeditionOptionItem, ExpeditionOptionItem);

protected:
	bool Pass();

	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ItemShareCheck();
	PgAction_ItemShareCheck( PgAction_ItemShareCheck const& );
};

class PgAction_ShareItem
	:	public PgUtilAction
{
public:
	explicit PgAction_ShareItem(const EPartyOptionItem ePartyOptionItem, SGroundKey const &kGroundKey, PgLocalPartyMgr &kLocalPartyMgr);
	virtual ~PgAction_ShareItem() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	CLASS_DECLARATION_S(BM::GUID, NextItemOwner);
private:
	const EPartyOptionItem m_eOptionItem;
	SGroundKey m_kGndKey;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ShareItem();
	PgAction_ShareItem( PgAction_ShareItem const& );
};

class PgAction_ExpeditionShareItem
	:	public PgUtilAction
{

public:

	explicit PgAction_ExpeditionShareItem(EExpeditionOptionItem const ExpeditionOptionItem, SGroundKey const & GroundKey, PgLocalPartyMgr & LocalPartyMgr);
	virtual ~PgAction_ExpeditionShareItem() {};

public:

	virtual bool DoAction(CUnit* pCaster, CUnit* pTarget);

	CLASS_DECLARATION_S(BM::GUID, NextItemOwner);

private:

	const EExpeditionOptionItem m_OptionItem;
	SGroundKey m_GndKey;
	PgLocalPartyMgr & m_LocalPartyMgr;

private:// Not Use

	PgAction_ExpeditionShareItem();
	PgAction_ExpeditionShareItem( PgAction_ExpeditionShareItem const & );

};

class PgAction_CreateItem
	:	public PgUtilAction
{
public:
	explicit PgAction_CreateItem(const EItemModifyParentEventType eCause, SGroundKey const &kGroundKey, CONT_ITEM_CREATE_ORDER const &kContItemCreateOrder, BM::Stream const &kPacket = BM::Stream() );
	virtual ~PgAction_CreateItem(){}

public:
	
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	bool	DoCreateItem(CUnit* pkCaster,CONT_PLAYER_MODIFY_ORDER & kOrder) const;

private:
	const EItemModifyParentEventType m_eCause;
	SGroundKey const m_kGndKey;
	CONT_ITEM_CREATE_ORDER m_kContItemCreateOrder;

	BM::Stream const& m_kAddonPacket;

private:// Not Use
	PgAction_CreateItem();
	PgAction_CreateItem( PgAction_CreateItem const& );
};

class PgAction_PickUpItem
	:	public PgUtilAction
{
public:
	explicit PgAction_PickUpItem( PgGround * const pkGnd, PgLocalPartyMgr &rkLocalPartyMgr, CUnit* pkLooter = NULL);	//클라이언트 연출용 유닛 추가
	virtual ~PgAction_PickUpItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	static bool InitStaticValue();

private:
	int m_iErrorMessage;

	static DWORD m_dwCanAnyPickUpItemTime;

	PgLocalPartyMgr		&m_rkLocalPartyMgr;
	PgGround* const		m_pkGround;
	CUnit* const		m_pkLooter;

private:// Not Use
	PgAction_PickUpItem();
	PgAction_PickUpItem( PgAction_PickUpItem const& );
};

class PgAction_PopPetItem
	:	public PgUtilAction
{
public:
	explicit PgAction_PopPetItem( BM::GUID const &kPetID, SItemPos const &kPetPos, SItemPos const &kPlayerPos, PgGround const *pkGnd );
	virtual ~PgAction_PopPetItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit *pkNothing);

private:
	BM::GUID const m_kPetID;
	SItemPos const m_kPetPos;
	SItemPos const m_kPlayerPos;
	PgGround const *m_pkGround;
};

class PgAction_MoveItem
	:	public PgUtilAction
{
public:
	explicit PgAction_MoveItem(SItemPos const &kCasterPos, SItemPos const &kTargetPos, PgGround const *pkGnd, DWORD const dwCurrentTime, bool const bMovePet = false );
	virtual ~PgAction_MoveItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkPet);

protected:
	int IsCanEquipPet(	CUnit *pkCasterUnit
					,	SItemPos const &kCasterPos
					,	PgBase_Item const &kCasterItem
					,	CUnit *pkTargetUnit
					,	SItemPos const &kTargetPos
					,	CONT_PLAYER_MODIFY_ORDER& rkOrder
					);

	bool IsCanEquip(CUnit* pkCaster, SItemPos const &kCasterPos, SItemPos const &kTargetPos, CONT_PLAYER_MODIFY_ORDER& rkOrder);
	bool GetEmptyPos(CUnit* pkCaster, SItemPos const &kCasterPos, SItemPos const &kTargetPos, SItemPos& rkOutPos) const;
	
	typedef struct tagSItemCoolTimeInfo
	{
		tagSItemCoolTimeInfo()
		{
			byCoolTimeType = 0;
			dwCoolTime =0;
		}

		BYTE byCoolTimeType;
		DWORD dwCoolTime;
	}SItemCoolTimeInfo;

	typedef std::map<int, SItemCoolTimeInfo> CONT_ITEM_COOLTIME;	// <ItemNo, SItemCoolTimeInfo>
	CONT_ITEM_COOLTIME m_kCoolTime;
	void SaveCoolTime(int const iItemNo, BYTE const byCoolTimeType, DWORD const dwCoolTime);
	void AddCoolTime( PgInventory * pkInv, CUnit * pkCaster );
	void MakeUnEquipPetOrder( SItemPos const &kItemPos, PgBase_Item const &kUnEquipPetItem, CONT_PLAYER_MODIFY_ORDER &kOrder );



private:
	SItemPos const m_kCasterPos;
	SItemPos const m_kTargetPos;
	PgGround const *m_pkGround;
	DWORD const m_dwCurrentTime;
	bool const m_bMovePet;

private:// Not Use
	PgAction_MoveItem();
	PgAction_MoveItem( PgAction_MoveItem const& );
};

//길드금고 아이템
class PgAction_MoveItem_Guild
	:	public PgUtilAction
{
public:
	explicit PgAction_MoveItem_Guild(SItemPos const &kCasterPos, SItemPos const &kTargetPos, PgBase_Item const &rkCasterItem, PgBase_Item const &rkTargetItem,
		PgGround const *pkGnd, DWORD const dwCurrentTime, const int iType, const bool bAddonpacket, const SGuild_Inventory_Log& rkLog);
	virtual ~PgAction_MoveItem_Guild(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

protected:
	void TradeListToOrderCont(const BM::GUID& rkCasterGuid, const BM::GUID& rkTargetGuid, const PgBase_Item& rkCasterItem, const SItemPos& rkCasterItemPos, const PgBase_Item& rkTargetItem, const SItemPos& rkTargetItemPos,CONT_PLAYER_MODIFY_ORDER& rkOrder, const EOrderOwnerType eCasterType, const EOrderOwnerType eTargetType);
	bool IsMoveGuildInv(CUnit* pkCaster, SItemPos const &kCasterPos);

private:
	SItemPos const m_kCasterPos;
	SItemPos const m_kTargetPos;
	PgBase_Item const m_kCasterItem;
	PgBase_Item const m_kTargetItem;
	PgGround const *m_pkGround;
	DWORD const m_dwCurrentTime;
	int const m_iType;
	const bool m_bAddonPacket;
	SGuild_Inventory_Log m_kLog;

private:// Not Use
	PgAction_MoveItem_Guild();
	PgAction_MoveItem_Guild( PgAction_MoveItem_Guild const& );
};

//길드금고 머니
class PgAction_MoveMoney_Guild
	:	public PgUtilAction
{
public:
	explicit PgAction_MoveMoney_Guild(PgGround const *pkGnd, const BM::GUID& kGuildGuid, const int iType, const __int64 i64Money, const bool bAddonPacket, const SGuild_Inventory_Log& rkLog);
	virtual ~PgAction_MoveMoney_Guild(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:	
	PgGround const *m_pkGround;
	BM::GUID m_kGuildGuid;
	int const m_iType;
	const __int64 m_i64Money;
	const bool m_bAddonPacket;
	SGuild_Inventory_Log m_kLog;

private:// Not Use
	PgAction_MoveMoney_Guild();
	PgAction_MoveMoney_Guild( PgAction_MoveMoney_Guild const& );
};

class PgAction_UseItem
	:	public PgUtilAction
{
public:
	explicit PgAction_UseItem( PgGround * const pkGround, PgBase_Item const &kItem, WORD const usUseCount, BM::Stream const& rkPacket = BM::Stream() );
	virtual ~PgAction_UseItem(){}

	static CUnit* GetTargetUnit( CItemDef const *pkItemDef, CUnit * pkCaster, PgGround const *pkGround );
public:

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

protected:
	bool DoEffect( CItemDef const *pkDef, CUnit * pkCaster, CUnit * pkTarget );
	bool DoInstantEffect(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTarget);
	bool DoInstantEffect_Pet(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTargetPet );
	bool DoMaintenanceEffect(CItemDef const* pkDef, CUnit* pkCaster, CUnit* pkTarget);

private:
	PgBase_Item const &m_kItem;
	PgGround * const m_pkGround;
	WORD const m_usUseCount;
	BM::Stream m_kPacket;

private:// Not Use
	PgAction_UseItem();
	PgAction_UseItem( PgAction_UseItem const& );
};

class PgRegQuickSlot //퀵인벤 삭제,등록 둘다 이걸로.
	:	public PgUtilAction
{
public:
	explicit PgRegQuickSlot(size_t const slot_idx, SQuickInvInfo& kQuickInvInfo);
	virtual ~PgRegQuickSlot(){}

public:
	virtual bool DoAction(CUnit* pkUser, CUnit* pkNothing = NULL);

private:
	size_t m_slot_idx;
	SQuickInvInfo &m_rkQuickInvInfo;

private:// Not Use
	PgRegQuickSlot();
	PgRegQuickSlot( PgRegQuickSlot const& );
};

class PgRegQuickSlotViewPage //퀵인벤 삭제,등록 둘다 이걸로.
	:	public PgUtilAction
{
public:
	explicit PgRegQuickSlotViewPage(char const cViewPage);
	virtual ~PgRegQuickSlotViewPage(){}

public:
	virtual bool DoAction(CUnit* pkUser, CUnit* pkNothing = NULL);

private:
	char const m_cViewPage;

private:// Not Use
	PgRegQuickSlotViewPage();
	PgRegQuickSlotViewPage( PgRegQuickSlotViewPage const& );
};

class PgDeathPenalty//반드시 살린다음 콜 할 것. 이건 패널티를 주면서. 무적을 걸어줌.
	: public PgUtilAction
{
public :
	explicit PgDeathPenalty(SGroundKey const& kGndKey, ELogUserResurrectType const eResurrectType, bool const bIsPenalty = false);
	virtual ~PgDeathPenalty(){}

public:
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
	SGroundKey const m_kGndKey;

protected:
	void ProcessEquipPenalty(CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &kOrder);
	void DoDefDeathPenalty(CUnit* pkCaster);
	bool CheckPenalty(CUnit* pkCaster, VEC_DEFDEATHPENALTY::value_type const& rkPenalty, int const iGroundAttr);
	void DoPenalty(CUnit* pkCaster, VEC_DEFDEATHPENALTY::value_type const& rkPenalty);

	bool const m_bIsPenalty;
	ELogUserResurrectType const m_eResurrectType;

private:// Not Use
	PgDeathPenalty();
	PgDeathPenalty( PgDeathPenalty const& );
};

//Effect에서 호출하는 클래스이다 다른데서 사용하지 말것!!!


class PgThrowUpPenalty
{
public:
	static int const ms_iEffectNo = 11001;// 미션 패널티 이펙트 번호

	explicit PgThrowUpPenalty(SGroundKey const &kGndKey, bool const bMissionOutMove);
	virtual ~PgThrowUpPenalty(){}

public:
	bool DoAction(CUnit *pkUnit, int const iExpRate=0);

	SGroundKey m_kGndKey;

private:// Not Use
	PgThrowUpPenalty();
	PgThrowUpPenalty( PgThrowUpPenalty const& );
	bool m_bMissionOutMapMove;
};

class PgAction_ReqUseItem 
	: public  PgUtilAction
{
public :
	explicit PgAction_ReqUseItem(SItemPos const &kCasterPos, PgGround* const pkGround, bool const bAnsServerCheck, BM::Stream &rkOrgPacket, BM::Stream const &kAddonPacket = BM::Stream());
	virtual ~PgAction_ReqUseItem(){}

public:
	static int CheckUseTime( CItemDef const &rkItemDef );

	virtual bool DoAction( CUnit* pkItemOwner, CUnit* pkPlayer );
	void SetCoolTime(DWORD const dwCoolTime);

	typedef enum CheckItemEffectResult
	{
		CIER_NONE			= 0x00000000,
		CIER_FULL_HP		= 0x00000001,
		CIER_FULL_MP		= 0x00000002,
	} CIEffectResult;
protected:
	bool CheckItemEffect( CItemDef const *pkItemDef, CUnit* pkItemOwner, CUnit* pkTargetUnit, PgBase_Item const & kItem, DWORD & Result );
	bool CheckItemEffect_Player( CItemDef const* const pkDef, CUnit* pkCaster, PgBase_Item const & kItem, DWORD & Result );
	bool CheckItemEffect_Pet(CItemDef const* const pkDef, CUnit* pkCaster, PgPet *pkTargetPet, PgBase_Item const & kItem);
	int CheckEnableUseEffect(CItemDef const* const pkDef, int const iItemNo,int const iEffectNo,CUnit* pkTarget);

	SItemPos const m_kCasterPos;
	bool const	m_bAnsServerCheck;
	SGroundKey m_kGndKey;
	T_GNDATTR	m_kGndAttr;
	PgGround * const m_pkGround;

	BM::Stream& m_kOrgPacket;
	BM::Stream m_kAddonPacket;
	DWORD m_dwCoolTime;

	CONT_PLAYER_MODIFY_ORDER m_kOrder;

	EItemModifyParentEventType m_kCause;

	CLASS_DECLARATION_S(HRESULT, Ret);
	SPMO m_kCashAdd;

private:// Not Use
	PgAction_ReqUseItem();
	PgAction_ReqUseItem( PgAction_ReqUseItem const& );
	bool FilterQuest(CUnit * pkUnit,CItemDef const* const pItemDef);
};

//////////////////////////////////////////////////////////////////////////////
//		Shine Stone
//>>
class PgAction_BuildStone : public PgUtilAction
{
public :
	explicit PgAction_BuildStone(SGroundKey const &rkGndKey);
	virtual ~PgAction_BuildStone(){};

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:
	SGroundKey m_kGndKey;

private:// Not Use
	PgAction_BuildStone();
	PgAction_BuildStone( PgAction_BuildStone const& );
};

//
class PgAction_TriggerStone : public PgUtilAction
{
public:
	explicit PgAction_TriggerStone(SGroundKey const &rkGndKey, BM::GUID const &rkStoneGuid, POINT3 const &rkPos);
	virtual ~PgAction_TriggerStone() {};

	virtual bool DoAction(CUnit *pUser, CUnit* Nothing);

private:
	SGroundKey m_kGndKey;
	BM::GUID m_kStoneGuid;
	POINT3 m_kPos;

private:// Not Use
	PgAction_TriggerStone();
	PgAction_TriggerStone( PgAction_TriggerStone const& );
};

class PgAction_HiddenItemBag
	:	public PgUtilAction
{
public:
	
	explicit PgAction_HiddenItemBag(SGroundKey const &kGroundKey,BM::Stream & kPacket, PgEventAbil const * const pkEventAbil):m_kGndKey(kGroundKey),m_kPacket(kPacket),m_pkEventAbil(pkEventAbil){}
	virtual ~PgAction_HiddenItemBag(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

	static HRESULT PopItem(  CUnit *pUser, int const iBagGrpNoID, PgBase_Item &rkRetItem, PgEventAbil const * const pkEventAbil );
private:
	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
	PgEventAbil const * const m_pkEventAbil;
};


class PgAction_PopItemBag 
	: public PgUtilAction
{
public:
	explicit PgAction_PopItemBag(const EItemModifyParentEventType eCause, SGroundKey const &rkGndKey,int const iMode,int const iItemBagGrpNo,int const iPackItemBagNo, PgEventAbil const * const pkEventAbil,int const iRarityControl );
	virtual ~PgAction_PopItemBag() {};

	virtual bool DoAction(CUnit *pUser, CUnit* Nothing);
	bool DoAction(CUnit *pUser, size_t iCount, bool bCreate); // 항상 1개의 아이템만 생성 (ex: 강화가루1개)

	bool DoCreatePack(CUnit *pUser);// 백에 지정된 아이템 갯수 만큼 생성 (ex: 강화가루n개)
	bool DoCreate(CUnit *pUser);

	int GetRetItemNo(size_t const iAt=0)const;
	void SwapRetItemNo(RESULT_ITEM_BAG_LIST_CONT& rkConRetItemNo);
	void SwapRetItem(RESULT_SITEM_BAG_LIST_CONT & rkConRetItem);

//	RESULT_SITEM_BAG_LIST_CONT const& ConRetItem()	{ return m_kConRetItem; }
	RESULT_ITEM_BAG_LIST_CONT const& ConRetItemNo()	{ return m_kConRetItemNo; }

protected:
	CONT_ITEM_CREATE_ORDER			m_kOrderList;
	RESULT_ITEM_BAG_LIST_CONT	m_kConRetItemNo;
	RESULT_SITEM_BAG_LIST_CONT	m_kConRetItem;

	const EItemModifyParentEventType m_eCause;
	SGroundKey m_kGndKey;
	const int m_iMode;
	const int m_iItemBagGrpNo;
	const int m_iItemBagNo;
	const int m_iRarityControlNo;
	PgEventAbil const * const m_pkEventAbil;

private:// Not Use
	PgAction_PopItemBag();
	PgAction_PopItemBag( PgAction_PopItemBag const& );
};

class PgAction_PopItemContainer
	: public PgAction_PopItemBag
{
public:
	explicit PgAction_PopItemContainer(const EItemModifyParentEventType eCause, SGroundKey const &rkGndKey, int const iContainerNo, PgEventAbil const * const pkEventAbil);
	virtual ~PgAction_PopItemContainer() {};

	bool DoAction(CUnit *pUser, size_t const in_count, bool const bCreate);

private:// Not Use
	PgAction_PopItemContainer();
	PgAction_PopItemContainer( PgAction_PopItemContainer const& );
};

class PgAction_RewardItem
	:	public PgUtilAction
{
public:
	explicit PgAction_RewardItem(EItemModifyParentEventType const eCause, SGroundKey const &rkGndKey, REWARD_ITEM_LIST const& rkConItem);
	explicit PgAction_RewardItem(EItemModifyParentEventType const eCause, SGroundKey const &rkGndKey, RESULT_ITEM_BAG_LIST_CONT const& rkConItem);
	virtual ~PgAction_RewardItem() {};

	virtual bool DoAction(CUnit *pkUser, CUnit* Nothing);

	struct SAction
	{
		SAction(CONT_ITEM_CREATE_ORDER& rkOrderList,EItemModifyParentEventType	const eCause)
			:	m_rkOrderList(rkOrderList),m_eCause(eCause)
		{}
		bool operator()(REWARD_ITEM_LIST::value_type const &kValue);

		const EItemModifyParentEventType	m_eCause;
		CONT_ITEM_CREATE_ORDER&				m_rkOrderList;
	};

protected:
	const EItemModifyParentEventType	m_eCause;
	SGroundKey							m_kGndKey;
	REWARD_ITEM_LIST					m_kContItem;

private:// Not Use
	PgAction_RewardItem();
	PgAction_RewardItem( PgAction_RewardItem const& );
};

class PgAction_ItemPlusUpgrade
	:	public PgUtilAction
{//Call From NPC_Enchant Script
public:
	explicit PgAction_ItemPlusUpgrade(SGroundKey const &kGroundKey, BM::Stream & kPacket, int const iAddRate = 0);
	virtual ~PgAction_ItemPlusUpgrade();

	static int const GetRareType(int const iGroupNo, int const iRareIndex);
	static TBL_DEF_ITEMPLUSUPGRADE const *GetPlusInfo( PgBase_Item const &kTargetItem, int const iLv, EPlusItemUpgradeResult &rkOutRet );

public:
	void SendToAchievementMgr(PgPlayer * pPlayer);

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	static bool GenPlusType(TBL_DEF_ITEMPLUSUPGRADE const * pPlusInfo,int & iResultType);

private:
	SGroundKey const m_kGndKey;
	BM::Stream &m_kPacket;

	SItemPos m_kItemPos;
	SItemPos m_kBonusRateItemPos;
	SItemPos m_kInsuranceItemPos;
	bool	 m_kUseInsurance;
	int		m_iAddRate;

private:// Not Use
	PgAction_ItemPlusUpgrade();
	PgAction_ItemPlusUpgrade( PgAction_ItemPlusUpgrade const& );

private:
	
	EPlusItemUpgradeResult Process(CUnit* pkCaster,CONT_PLAYER_MODIFY_ORDER & kOrder, CUnit * pkTarget, BM::Stream * pkPacket);
	int m_iItemNo;
	int m_iNowLevel;
	int m_iNextLevel;
};

class PgAction_ItemRarityUpgrade
	:	public PgUtilAction
{//Call From NPC_Enchant Script
public:
	
	explicit PgAction_ItemRarityUpgrade(SGroundKey const &kGroundKey,BM::Stream const &kPacket, int const iAddDecSoulRate = 0)
		:m_kGndKey(kGroundKey)
		,m_kPacket(kPacket)
		,m_kPropertyType(E_PPTY_NONE)
		,m_bUseSuccessRateItem(false)
		,m_bUseInsurance(false)
		,m_bUseInsuranceItem(false)
		,m_iAddDecSoulRate(iAddDecSoulRate)
	{}
	virtual ~PgAction_ItemRarityUpgrade(){}

public:

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:

	EItemRarityUpgradeResult OnProcess(CUnit * pkCaster,CUnit * pkTarget);

private:

	SGroundKey		const		m_kGndKey;
	BM::Stream					m_kPacket;

	EPropertyType				m_kPropertyType;
	bool						m_bUseSuccessRateItem;
	bool						m_bUseInsuranceItem;
	SItemPos					m_kItemPos;
	SItemPos					m_kSuccessRateItemPos;
	SItemPos					m_kInsuranceItemPos;
	bool						m_bUseInsurance;
	bool						m_bUseSucceRate;
	int							m_iAddDecSoulRate;
private:// Not Use
	PgAction_ItemRarityUpgrade();
	PgAction_ItemRarityUpgrade( PgAction_ItemRarityUpgrade const& );
};

class PgAction_ItemDischarge
	:	public PgUtilAction
{//Call From NPC_Enchant Script
public:
	explicit PgAction_ItemDischarge(SGroundKey const &kGroundKey, SItemPos const &rkItemPos, SItemPos const &rkDischargeItemPos,CONT_PLAYER_MODIFY_ORDER & kOrder)
		:m_kGndKey(kGroundKey),m_kItemPos(rkItemPos),m_kDischargeItemPos(rkDischargeItemPos),m_kOrder(kOrder){}
	virtual ~PgAction_ItemDischarge(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:

	EItemDischargeResult OnProcess(CUnit *pkCaster, CUnit *pkTarget);

private:

	SGroundKey			const		m_kGndKey;
	SItemPos			const		m_kItemPos;
	SItemPos			const		m_kDischargeItemPos;
	CONT_PLAYER_MODIFY_ORDER	&	m_kOrder;

private:// Not Use
	PgAction_ItemDischarge();
	PgAction_ItemDischarge( PgAction_ItemDischarge const& );
};

class PgAction_ReqModifyItem
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqModifyItem(EItemModifyParentEventType const kCause, SGroundKey const &kGroundKey, CONT_PLAYER_MODIFY_ORDER const &kOrder, BM::Stream const &kPacket = BM::Stream() , bool const bIsIgnoreDeath = false);
	virtual ~PgAction_ReqModifyItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	void SetCoolTime(DWORD const dwCoolTime);	// CoolTime 적용시킬 필요가 있을 때만 호출해 주면 된다.

	bool DoAction( BM::GUID const &kCasterGuid );

private:
	SGroundKey const m_kGndKey;
	CONT_PLAYER_MODIFY_ORDER const m_kOrder;

	BM::Stream const &m_kAddonPacket;

	CLASS_DECLARATION_S(EItemModifyParentEventType, Cause);
	CLASS_DECLARATION_S(SPMO, CashAdd);	// Cash수정은 transaction 처리 안되기 때문에, 다른 형식으로 요청을 보내야 한다.

	DWORD m_dwCoolTime;
	bool const m_bIsIgnoreDeath;

private:// Not Use
	PgAction_ReqModifyItem();
	PgAction_ReqModifyItem( PgAction_ReqModifyItem const& );
};

/*
class PgAction_InitUnitDrop
	:	public PgUtilAction
{
public:
	explicit PgAction_InitUnitDrop( TBL_DQT_DEF_MAP_ITEM_BAG const &kTbl, PgEventAbil const * const pkEventAbil, T_GNDATTR const eGndAttr,  bool const bDropAllItem = false);
	virtual ~PgAction_InitUnitDrop(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
//	SGroundKey const m_kGndKey;
//	const EItemModifyCause m_keCause;
//	CONT_PLAYER_MODIFY_ORDER const m_kOrder;
	TBL_DQT_DEF_MAP_ITEM_BAG const m_kTbl;
	PgEventAbil const * const m_pkEventAbil;
	T_GNDATTR const m_eGndAttr;
	bool m_bDropAllItem;

private:// Not Use
	PgAction_InitUnitDrop();
	PgAction_InitUnitDrop( PgAction_InitUnitDrop const& );
};
*/

class PgAction_DecEquipDuration
	:	public PgUtilAction
{
	typedef std::set< int > CONT_DEC_POS;
public:
	explicit PgAction_DecEquipDuration(SGroundKey const &kGndKey, bool const bIsAttacker, int const iValue, bool const bIsDamageEvent=true);
	virtual ~PgAction_DecEquipDuration(){};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	int GetEnchantDiffRate(int const iBaseRate, int const iEnchantDiff);
	bool GetEquipRandomPos(bool const bIsAttacker, CUnit* pkCaster, CONT_DEC_POS& rkOut, size_t const iMaxCount = 1);//내구도 깎을때 포지션을 받아오는 부분

private:
	SGroundKey const m_kGndKey;
	bool m_bIsAttacker;
	bool m_bIsDamageEvent;
	int m_iValue;

private:// Not Use
	PgAction_DecEquipDuration();
	PgAction_DecEquipDuration( PgAction_DecEquipDuration const& );
};

class PgAction_ItemRepair
	:	public PgUtilAction
{
public:
	explicit PgAction_ItemRepair(SGroundKey const &kGndKey, CONT_REQ_ITEM_REPAIR const &kContRepair);
	virtual ~PgAction_ItemRepair(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SGroundKey const m_kGndKey;
	CONT_REQ_ITEM_REPAIR m_kContRepair;

private:// Not Use
	PgAction_ItemRepair();
	PgAction_ItemRepair( PgAction_ItemRepair const& );
};

class PgAction_ReqRemoveInvItem
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqRemoveInvItem(SGroundKey const &kGndKey, ContItemRemoveOrder const &kCont, int const iDeleteType = 0, int const iAddSoulCountRate = 0);
	virtual ~PgAction_ReqRemoveInvItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SGroundKey const m_kGndKey;
	ContItemRemoveOrder const m_kOrder;
	int const m_iDeleteType;
	int const m_iAddSoulCountRate;

private:// Not Use
	PgAction_ReqRemoveInvItem();
	PgAction_ReqRemoveInvItem( PgAction_ReqRemoveInvItem const& );
};

class PgAction_ItemMaking
	:	public PgUtilAction
{
public:
	typedef std::map< int, int > CONT_NEED_ITEMS;

public:
	explicit PgAction_ItemMaking(SGroundKey const &kGndKey, BM::Stream* pkPacket);
	virtual ~PgAction_ItemMaking(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	bool CreateResultItem(CUnit* pkCaster, int const iMakingType, SDefItemMakingData const& kMakingData, int const iAddSuccessRate, int const iRareRecipeItemNo, CONT_RESULT_ITEM_DATA& kContResultItems, int& iWarnMessage);
	bool CreateResultCookingItem(CUnit* pkCaster, TBL_DEF_COOKING const& kMakingData, bool const bUseOptionItem, CONT_RESULT_ITEM_DATA& kContResultItems, int& iWarnMessage);

	HRESULT Process(CUnit* pkCaster, SReqItemMaking const& rkReqItemMaking, int const& iAddSuccessRate, int const& iInsuranceType, bool& bUseInsurance, int& iMakingType, __int64& iTotalNeedMoney, CONT_NEED_ITEMS& kContNeedItems, CONT_RESULT_ITEM_DATA& kContResultItems, int& iMakingSuccessCount, int& iWarnMessage);
	HRESULT ProcessCooking(CUnit* pkCaster, SReqItemMaking const& rkReqItemMaking, __int64& iTotalNeedMoney, CONT_NEED_ITEMS& kContNeedItems, CONT_RESULT_ITEM_DATA& kContResultItems, int& iMakingSuccessCount, int& iWarnMessage);

private:
	SGroundKey const m_kGndKey;
	BM::Stream & m_kPacket;
	CItemDefMgr const& m_kItemDefMgr; // GET_DEF(CItemDefMgr, kItemDefMgr);
private:// Not Use
	PgAction_ItemMaking();
	PgAction_ItemMaking( PgAction_ItemMaking const& );
};

////////////////////////////////////////////////////////////////////
// 미션 악마의 영혼석 거래
class PgAction_SoulStoneTrade
	:	public PgUtilAction
{
public:
	explicit PgAction_SoulStoneTrade(SGroundKey const &kGndKey, BM::Stream* pkPacket);
	virtual ~PgAction_SoulStoneTrade() {}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SGroundKey const m_kGndKey;
	BM::Stream & m_kPacket;
	CItemDefMgr const & m_kItemDefMgr;

private:

	PgAction_SoulStoneTrade();
	PgAction_SoulStoneTrade(PgAction_SoulStoneTrade const &);

};

class PgAction_AddExp
	:	public PgUtilAction
{
public:
	static __int64 ms_i64TakePCExpRate;//PC로 부터 받는 경험치의 만분율

public:
	explicit PgAction_AddExp(SGroundKey const& rkGndKey, __int64 const iAddExp, const EAddExpCause eCause, PgGround *pkGround, int const iMonsterLv = 0, int const iAddedExpRate = 0);
	virtual ~PgAction_AddExp() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	__int64 ResultExp(__int64 const iAddExp, int const iAddExpRate = 100);
protected:
	bool PassAdvance(CUnit* pkCaster, CUnit* pkTarget);
	void Share_Party_Distance(CUnit* pkCaster, VEC_GUID& kVec);
	
private:
	SGroundKey const m_kGndKey;
	__int64 m_iAddExp;
	const EAddExpCause m_eCause;
	int const m_iMonsterLv;
	PgGround *m_pkGround;
	int m_iAddedExpRate;
private:// Not Use
	PgAction_AddExp();
	PgAction_AddExp( PgAction_AddExp const& );
};

class PgAction_ShareExpParty
	: public PgUtilAction
{
public:
	explicit PgAction_ShareExpParty(__int64 const iAddedExp, const EAddExpCause eCause, int const iMonsterLv, POINT3 const &rkFromPos, PgGround const * const pkGround, int const iPartyMemberCount, PgLocalPartyMgr &kLocalPartyMgr,
		int const iAddExpRate = 0);
	virtual ~PgAction_ShareExpParty() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	static float PartyMemberBonusRate(int const iPartyMemberCount);

private:
	PgGround const * const m_pkGround;
	__int64 const m_iAddedExp;
	const EAddExpCause m_eCause;
	int const m_iMonsterLv;
	POINT3 const &m_kFromPos;
	int const m_iPartyMemberCount;
	PgLocalPartyMgr &m_kLocalPartyMgr;
	int const m_iAddExpRate;

private:// Not Use
	PgAction_ShareExpParty();
	PgAction_ShareExpParty( PgAction_ShareExpParty const& );
};

class PgAction_ShareGold
	:	public PgUtilAction
{
public:
	explicit PgAction_ShareGold(PgGround* const pkGround, __int64 const iAddGold, EItemModifyParentEventType const kCause, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream const &kPacket = BM::Stream() );
	virtual ~PgAction_ShareGold() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	static __int64 PartyMemberShareGold(__int64 const iAddGold, int const iMemberCount);
	static __int64 PrivateShareGold( __int64 const iAddGold, int const iAddGoldRate, PgEventAbil const * const pkEventAbil );

protected:
	bool Pass(CUnit* pkCaster, CUnit* pkTarget);

public:
	PgGround* const m_pkGround;
	__int64 const m_iAddGold;
	EItemModifyParentEventType const m_kCause;
	BM::Stream const& m_kAddonPacket;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ShareGold();
	PgAction_ShareGold( PgAction_ShareGold const& );
};

class PgAction_DropItemBox
	:	public PgUtilAction
{
	enum EMonsterItemBagIndex // 겹침 번호 사용 하지 말것
	{
		MIBI_PRIMARY	= 0,
		MIBI_SECONDARY	= 1,
		MIBI_MAPBAG		= 2,
		MIBI_COUNT		= 3, // 위의 종류 갯수
	};

public:
	explicit PgAction_DropItemBox( PgGround const * const pkGround, TBL_DQT_DEF_MAP_ITEM_BAG const &kTbl );
	virtual ~PgAction_DropItemBox() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	
private:
	PgGround const * const m_pkGround;
	TBL_DQT_DEF_MAP_ITEM_BAG const &m_rkMapBag;

public:
	CONT_MONSTER_DROP_ITEM m_kContDropItem;

private:// Not Use
	PgAction_DropItemBox();
	PgAction_DropItemBox( PgAction_DropItemBox const& );
};

class PgAction_RaceEventRewardItem
	: public PgUtilAction
{
public:
	explicit PgAction_RaceEventRewardItem(SGroundKey const& rkGndKey, int iRewardItemGroupNo) : m_kGndKey(rkGndKey), m_iRewardItemGroupNo(iRewardItemGroupNo), m_iRewardItemNo(0) {}
	virtual ~PgAction_RaceEventRewardItem(){};
public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	int GetRewardItemNo(void) { return m_iRewardItemNo; }

protected:
	SGroundKey const m_kGndKey;
	int m_iRewardItemGroupNo;
	int m_iRewardItemNo;
};

class PgAction_DropLimitedItem
	:	public PgUtilAction
{
public:
	explicit PgAction_DropLimitedItem(SGroundKey const& rkGndKey, BM::GUID const &kOwnerGuid, POINT3 const &ptPos, int const kObjectType, int const kObjectNo);
	virtual ~PgAction_DropLimitedItem(){};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	
public:
	SGroundKey const m_kGndKey;
	POINT3 m_kptPos;
	BM::GUID m_kOwnerGuid;
	int const m_kObjectType;
	int const m_kObjectNo;

private:// Not Use
	PgAction_DropLimitedItem();
	PgAction_DropLimitedItem( PgAction_DropLimitedItem const& );
};

class PgAction_ExchangeItem
	:	public PgUtilAction
{
public:
	explicit PgAction_ExchangeItem(SGroundKey const& rkGndKey, const STradeInfo &kInfo);
	virtual ~PgAction_ExchangeItem() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	
protected:
	bool IsCorrectTradeList(PgInventory* pkInv, CONT_TRADE_ITEM const &kContTrade);
	void TradeListToOrderCont(BM::GUID const &kCaster, BM::GUID const &kTarget, CONT_TRADE_ITEM const &kContCasterTrade, CONT_TRADE_ITEM const &kContTargetTrade, CONT_PLAYER_MODIFY_ORDER &kOrder);

protected:
	const STradeInfo m_kInfo;
	SGroundKey const m_kGndKey;

private:// Not Use
	PgAction_ExchangeItem();
	PgAction_ExchangeItem( PgAction_ExchangeItem const& );
};

class PgAction_ViewOtherEquip
	:	public PgUtilAction
{
public:
	explicit PgAction_ViewOtherEquip(SGroundKey const& rkGndKey);
	virtual ~PgAction_ViewOtherEquip() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	
public:
	SGroundKey const m_kGndKey;

private:// Not Use
	PgAction_ViewOtherEquip();
	PgAction_ViewOtherEquip( PgAction_ViewOtherEquip const& );
};

class PgAction_EffectControl
	:	public PgUtilAction
{
public:
	explicit PgAction_EffectControl(SGroundKey const& rkGndKey, int const iEffectNo);
	virtual ~PgAction_EffectControl() {};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing=NULL);

protected:
	SGroundKey const	m_kGndKey;
	int const			m_iEffectNo;

private:// Not Use
	PgAction_EffectControl();
	PgAction_EffectControl( PgAction_EffectControl const& );
};

class PgAction_SpendMoney
	:	public PgUtilAction
{
public:
	explicit PgAction_SpendMoney( ESpendMoneyType const kType, PgGround const *pkGnd);
	virtual ~PgAction_SpendMoney(){};

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing=NULL);

protected:
	const ESpendMoneyType	m_kType;
	const PgGround*			m_pkGnd;

private:// Not Use
	PgAction_SpendMoney();
	PgAction_SpendMoney( PgAction_SpendMoney const& );
};

class PgCreateSpreadPos
{
public:
	explicit PgCreateSpreadPos(POINT3 const& rkStartPos);
	~PgCreateSpreadPos(){}

	void AddFrontRange(const NxScene* pkScene, POINT3 const& rkDirPos, float const fRange);
	void AddDir(const NxScene* pkScene,POINT3 const& rkDir, int const iLength, bool bBack=true);
	void PopPos(POINT3& rkOutPos, int const iMinLength=0);
protected:
	POINT3		m_kStartPos;
	POINT3		m_kDir;
	int			m_iLength;

private:// Not Use
	PgCreateSpreadPos();
	PgCreateSpreadPos( PgCreateSpreadPos const& );
};

//class PgAction_ResetSkill
//	:	public PgUtilAction
//{
//public:
//	explicit PgAction_ResetSkill(SGroundKey const &kGndKey,CONT_PLAYER_MODIFY_ORDER & rkOrder);
//	virtual ~PgAction_ResetSkill(){}
//
//public:
//
//	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
//
//private:
//	SGroundKey const m_kGndKey;
//	CONT_PLAYER_MODIFY_ORDER & m_rkOrder;
//
//private:// Not Use
//	PgAction_ResetSkill();
//	PgAction_ResetSkill( PgAction_ResetSkill const& );
//};

class PgAction_SyncSkillState
	:	public PgUtilAction
{
public:
	PgAction_SyncSkillState();
	virtual ~PgAction_SyncSkillState(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

};

class PgAction_ReqDivideItem
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqDivideItem(SGroundKey const& rkGndKey, SItemPos const &kItemPos, int const iItemNo, BM::GUID const &kItemGuid, int const iCount);
	virtual ~PgAction_ReqDivideItem(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SGroundKey const	m_kGndKey;
	SItemPos const		m_kItemPos;
	int const			m_iItemNo;
	const BM::GUID		m_kItemGuid;
	int const			m_iDivCount;

private:// Not Use
	PgAction_ReqDivideItem();
	PgAction_ReqDivideItem( PgAction_ReqDivideItem const& );
};

class PgAction_ReqSMS
	:	public PgUtilAction
{
public:
	explicit PgAction_ReqSMS(PgGround * const pkGround, SItemPos const &kItemPos, int const iItemNo, BM::GUID const &kItemGuid, BM::Stream const & kPacket);
	virtual ~PgAction_ReqSMS(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:
	SItemPos const		m_kItemPos;
	int const			m_iItemNo;
	const BM::GUID		m_kItemGuid;
	BM::Stream	m_kPacket;
	PgGround* const m_pkGround;

private:// Not Use
	PgAction_ReqSMS();
	PgAction_ReqSMS( PgAction_ReqSMS const& );
};

class PgAction_EventSystem
	:	public PgUtilAction
{
protected:
	PgAction_EventSystem();
	virtual ~PgAction_EventSystem(){}

public:
	static void DoAction();

protected:
	virtual bool DoAction(CUnit* /*pkCaster*/, CUnit* /*pkTarget*/){return true;}
};

class PgAction_FollowingMode
	: public PgUtilAction
{
public:
	explicit PgAction_FollowingMode(EPlayer_Follow_Mode eMode, bool bFriend, PgGround* pkGround);
	virtual ~PgAction_FollowingMode() {};

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

protected:
	EPlayer_Follow_Mode m_eMode;
	EPlayer_Follow_Mode m_eResult;
	PgGround* m_pkGround;
	bool m_bFriend;

private:// Not Use
	PgAction_FollowingMode();
	PgAction_FollowingMode( PgAction_FollowingMode const& );
};


//
class PgAction_ReqGuildLevelUp
	: public PgUtilAction
{
public:
	explicit PgAction_ReqGuildLevelUp(SGroundKey const &rkGndKey, unsigned short const sGuildLv);
	virtual ~PgAction_ReqGuildLevelUp() {};

	virtual bool DoAction(CUnit *pkCaster, CUnit *Nothing);

	CLASS_DECLARATION_S_NO_SET(EGuildCommandRet, Result);
private:
	unsigned short const m_sGuildLv;
	SGroundKey const &m_kGndKey;

private:// Not Use
	PgAction_ReqGuildLevelUp();
	PgAction_ReqGuildLevelUp( PgAction_ReqGuildLevelUp const& );
};

class PgAction_ReqGuildInventoryCreate
	:public PgUtilAction
{
public:
	explicit PgAction_ReqGuildInventoryCreate(SGroundKey const &rkGndKey, unsigned short const sGuildLv);
	virtual ~PgAction_ReqGuildInventoryCreate() {};

	virtual bool DoAction(CUnit *pkCaster, CUnit *Nothing);

	CLASS_DECLARATION_S_NO_SET(EGuildCommandRet, Result);
private:
	unsigned short const m_sGuildLv;
	SGroundKey const &m_kGndKey;

private:
	static int const iCreateLevel = 2; // 길드창고 생성가능한 길드레벨
	static __int64 const iCreateMoney = 100000; // 10골드

private:// Not Use
	PgAction_ReqGuildInventoryCreate();
	PgAction_ReqGuildInventoryCreate( PgAction_ReqGuildInventoryCreate const& );
};

//
class PgAction_ReqGuildLearnSkill
	: public PgUtilAction
{
public:
	explicit PgAction_ReqGuildLearnSkill(SGroundKey const &rkGndKey, unsigned short const sGuildLv, int const iSkillNo);
	virtual ~PgAction_ReqGuildLearnSkill();

	virtual bool DoAction(CUnit *pkCaster, CUnit *Nothing);

	CLASS_DECLARATION_S_NO_SET(EGuildCommandRet, Result);
private:
	unsigned short const m_sGuildLv;
	SGroundKey const &m_kGndKey;
	int const m_iSkillNo;

private:// Not Use
	PgAction_ReqGuildLearnSkill();
	PgAction_ReqGuildLearnSkill( PgAction_ReqGuildLearnSkill const& );
};

class PgAction_ReqTrapDamage
	: public PgUtilAction
{
public:
	explicit PgAction_ReqTrapDamage(SGroundKey const& rkGndKey, PgGround const* pkGround, int iTrapSkillNo);
	virtual ~PgAction_ReqTrapDamage();

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	BM::Stream& GetResultPacket();

private:
	SGroundKey const& m_kGndKey;
	PgGround const *m_pkGround;
	int m_iTrapSkillNo;

private:// Not Use
	PgAction_ReqTrapDamage();
	PgAction_ReqTrapDamage( PgAction_ReqTrapDamage const& );
};

class PgAction_ReqCollisionDamage
	: public PgUtilAction
{
public:
	explicit PgAction_ReqCollisionDamage(SGroundKey const& rkGndKey, PgGround const* pkGround=NULL);
	virtual ~PgAction_ReqCollisionDamage();

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	BM::Stream& GetResultPacket();

private:
	SGroundKey const& m_kGndKey;
	PgGround const* m_pkGround;
private:// Not Use
	PgAction_ReqCollisionDamage();
	PgAction_ReqCollisionDamage( PgAction_ReqCollisionDamage const& );
};

class PgAction_GroundEffect
	: public PgUtilAction
{
public:
	explicit PgAction_GroundEffect(PgGround const *pkGround, EEffectType const eEffectType = EFFECT_TYPE_GROUND);
	virtual ~PgAction_GroundEffect();

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);
protected:
	bool CheckGroundEffect(PgGroundEffect const *pkGEffect, CUnit* pkCaster);

private:
	PgGround const *m_pkGround;
	EEffectType const m_eEffectType;

private:// Not Use
	PgAction_GroundEffect();
	PgAction_GroundEffect( PgAction_GroundEffect const& );
};

class PgAction_GroundDeleteEffect
	: public PgUtilAction
{
public:
	explicit PgAction_GroundDeleteEffect(PgGround const *pkGround);
	virtual ~PgAction_GroundDeleteEffect();

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkNothing);

private:
	PgGround const *m_pkGround;

private:// Not Use
	PgAction_GroundDeleteEffect();
	PgAction_GroundDeleteEffect( PgAction_GroundDeleteEffect const& );
};

class PgAction_ReqAddCp
	:	public PgUtilAction
{//ModifyItem 에서도 돈을 바꿀 수 있긴함.
public:

	explicit PgAction_ReqAddCp(EItemModifyParentEventType const kCause, int const iCp, SGroundKey const& kGroundKey, BM::Stream const& rkPacket = BM::Stream());
	virtual ~PgAction_ReqAddCp(){}

public:

	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:

	EItemModifyParentEventType const m_kCause;
	__int64 const m_iAddCp;
	SGroundKey m_kGndKey;
	BM::Stream const& m_kPacket;

private:// Not Use
	PgAction_ReqAddCp();
	PgAction_ReqAddCp( PgAction_ReqAddCp const& );
};

class PgAction_Sys2Inv
	:	public PgUtilAction
{
public:

	explicit PgAction_Sys2Inv(SGroundKey const & kGndKey, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_Sys2Inv(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;

private:// Not Use
	PgAction_Sys2Inv();
	PgAction_Sys2Inv( PgAction_Sys2Inv const& );
};

class PgAction_SysItemRemove
	:	public PgUtilAction
{
public:

	explicit PgAction_SysItemRemove(SGroundKey const & kGndKey, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_SysItemRemove(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;

private:// Not Use
	PgAction_SysItemRemove();
	PgAction_SysItemRemove( PgAction_SysItemRemove const& );
};

class PgAction_LearnSkill
	:	public PgUtilAction
{
public:
	explicit PgAction_LearnSkill( int const iSkillNo, SGroundKey const &kGroundKey, BYTE const cCmdType = 0, BM::Stream const& rkPacket = BM::Stream() );
	virtual ~PgAction_LearnSkill(){}

	int DoAction( CUnit* pkCaster )const;

private:
	int const m_iSkillNo;
	SGroundKey const m_kGndKey;
	BYTE const m_cCmdType;
	BM::Stream const& m_kPacket;

private:// Not Use
	PgAction_LearnSkill();
	PgAction_LearnSkill( PgAction_LearnSkill const& );
	virtual bool DoAction(CUnit* /*pkCaster*/, CUnit* /*pkTarget*/){return false;}
};

class PgAction_ReqUserMapMove
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqUserMapMove( PgGround * const pkGround,SGroundKey const & kGndKey, BM::Stream & kPacket ):m_pkGround(pkGround),m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqUserMapMove(){}
	
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	PgGround * const m_pkGround;
	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;

private:// Not Use
	PgAction_ReqUserMapMove();
	PgAction_ReqUserMapMove( PgAction_ReqUserMapMove const& );
};

class PgAction_ReqMoveToPartyMember
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqMoveToPartyMember(PgGround * pkGround,SGroundKey const & kGndKey, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kLocalPartyMgr(kLocalPartyMgr),m_kPacket(kPacket),m_pkGround(pkGround){}
	~PgAction_ReqMoveToPartyMember(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ReqMoveToPartyMember();
	PgAction_ReqMoveToPartyMember( PgAction_ReqMoveToPartyMember const& );
};

class PgAction_AnsMoveToPartyMember
	:	public PgUtilAction
{
public:

	explicit PgAction_AnsMoveToPartyMember(PgGround * pkGround,SGroundKey const & kGndKey, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket),m_pkGround(pkGround){}
	~PgAction_AnsMoveToPartyMember(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;

private:// Not Use
	PgAction_AnsMoveToPartyMember();
	PgAction_AnsMoveToPartyMember( PgAction_AnsMoveToPartyMember const& );
};

class PgAction_ReqMoveToPartyMemberGround
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqMoveToPartyMemberGround(PgGround * pkGround,SGroundKey const & kGndKey, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream & kPacket)
		: m_kGndKey(kGndKey),m_kLocalPartyMgr(kLocalPartyMgr),m_kPacket(kPacket),m_pkGround(pkGround) { }
	~PgAction_ReqMoveToPartyMemberGround() { }
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ReqMoveToPartyMemberGround();
	PgAction_ReqMoveToPartyMemberGround( PgAction_ReqMoveToPartyMemberGround const& );
};

class PgAction_AnsMoveToPartyMemberGround
	:	public PgUtilAction
{
public:

	explicit PgAction_AnsMoveToPartyMemberGround(PgGround * pkGround,SGroundKey const & kGndKey, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream & kPacket)
		: m_kGndKey(kGndKey),m_kLocalPartyMgr(kLocalPartyMgr),m_kPacket(kPacket),m_pkGround(pkGround) { }
	~PgAction_AnsMoveToPartyMemberGround() { }
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_AnsMoveToPartyMemberGround();
	PgAction_AnsMoveToPartyMemberGround( PgAction_AnsMoveToPartyMemberGround const& );
};

class PgAction_ReqRentalSafeExtend
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqRentalSafeExtend(SGroundKey const & kGndKey, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqRentalSafeExtend(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;

private:// Not Use
	PgAction_ReqRentalSafeExtend();
	PgAction_ReqRentalSafeExtend( PgAction_ReqRentalSafeExtend const& );
};

class PgAction_ReqSummonPartyMember
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqSummonPartyMember(PgGround * pkGround,SGroundKey const & kGndKey, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kLocalPartyMgr(kLocalPartyMgr),m_kPacket(kPacket),m_pkGround(pkGround){}
	~PgAction_ReqSummonPartyMember(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ReqSummonPartyMember();
	PgAction_ReqSummonPartyMember( PgAction_ReqSummonPartyMember const& );
};

class PgAction_ReqMoveToSummoner
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqMoveToSummoner(PgGround * pkGround, SGroundKey const & kGndKey, PgLocalPartyMgr &kLocalPartyMgr, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kLocalPartyMgr(kLocalPartyMgr),m_kPacket(kPacket),m_pkGround(pkGround){}
	~PgAction_ReqMoveToSummoner(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:// Not Use
	PgAction_ReqMoveToSummoner();
	PgAction_ReqMoveToSummoner( PgAction_ReqMoveToSummoner const& );
};

class PgAction_AnsMoveToSummoner
	:	public PgUtilAction
{
public:

	explicit PgAction_AnsMoveToSummoner(PgGround * pkGround,SGroundKey const & kGndKey, BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket),m_pkGround(pkGround){}
	~PgAction_AnsMoveToSummoner(){}
	
	bool DoAction(CUnit* pkCaster, CUnit* pkTarget);
	HRESULT Process(CUnit* pkCaster);

private:

	SGroundKey m_kGndKey;
	BM::Stream & m_kPacket;
	PgGround * m_pkGround;

private:// Not Use
	PgAction_AnsMoveToSummoner();
	PgAction_AnsMoveToSummoner( PgAction_AnsMoveToSummoner const& );
};

class PgAction_EventStart 
	: public PgAction_Callback
{
public:
	PgAction_EventStart(){}
	~PgAction_EventStart(){}
public:
	void DoAction(CONT_EVENT_STATE::mapped_type & element);
};

class PgAction_EventEnd
	: public PgAction_Callback
{
public:
	PgAction_EventEnd(){}
	~PgAction_EventEnd(){}
public:
	void DoAction(CONT_EVENT_STATE::mapped_type & element);
};

class PgFitToPrimaryInv
	:	public PgUtilAction
{
public:

	explicit PgFitToPrimaryInv(SItemPos const & kItemPos,PgGround const *pkGnd):m_kItemPos(kItemPos),m_pkGround(pkGnd){}
	virtual ~PgFitToPrimaryInv(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	E_ERROR_FIT_TO_INV const Process(CUnit * pUser);

	SItemPos const & m_kItemPos;
	PgGround const * m_pkGround;
};

class PgAction_CashItem_Add_TimeLimit
	:	public PgUtilAction
{
public:

	explicit PgAction_CashItem_Add_TimeLimit(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_CashItem_Add_TimeLimit(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	ECashShopResult const Process(CUnit * pUser);

	BM::Stream & m_kPacket;
	SGroundKey const & m_kGndKey;
};


class PgAction_ReqRegUserPortal
	:	public PgUtilAction
{
public:

	explicit PgAction_ReqRegUserPortal(PgGround const * const pkGround,BM::Stream & kPacket):m_pkGround(pkGround),m_kPacket(kPacket){}
	~PgAction_ReqRegUserPortal(){}

	virtual bool DoAction(CUnit* pUser, CUnit* pkTargetUnit);

private:

	BM::Stream & m_kPacket;
	PgGround const * const m_pkGround;
	CONT_PLAYER_MODIFY_ORDER m_kOrder;
};


class PgAction_ReqGenSocket
	: public PgUtilAction
{
public:
	explicit PgAction_ReqGenSocket(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqGenSocket(){}

	bool DoAction(CUnit * pUser,CUnit * pTarget);

private:

	HRESULT Process(CUnit * pUser, CUnit * pkTarget);

	BM::Stream m_kPacket;
	SGroundKey const & m_kGndKey;
};

class PgAction_ReqSetMonsterCard
	: public PgUtilAction
{
public:
	explicit PgAction_ReqSetMonsterCard(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqSetMonsterCard(){}

	bool DoAction(CUnit * pUser,CUnit * pTarget);

private:

	HRESULT Process(CUnit * pUser);

	BM::Stream m_kPacket;
	SGroundKey const & m_kGndKey;
};

class PgAction_ReqDestroySocket
	: public PgUtilAction
{
public:
	explicit PgAction_ReqDestroySocket(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqDestroySocket(){}

	bool DoAction(CUnit * pUser,CUnit * pTarget);

private:

	HRESULT Process(CUnit * pUser);

	BM::Stream m_kPacket;
	SGroundKey const & m_kGndKey;
};

class PgAction_ReqExtractionSocket
	: public PgUtilAction
{
public:
	explicit PgAction_ReqExtractionSocket(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqExtractionSocket(){}

	bool DoAction(CUnit * pUser,CUnit * pTarget);

private:

	HRESULT Process(CUnit * pUser);

	BM::Stream m_kPacket;
	SGroundKey const & m_kGndKey;
};

class PgAction_ReqRemoveSocket
	: public PgUtilAction
{
public:
	explicit PgAction_ReqRemoveSocket(SGroundKey const & kGndKey,BM::Stream & kPacket):m_kGndKey(kGndKey),m_kPacket(kPacket){}
	~PgAction_ReqRemoveSocket(){}

	bool DoAction(CUnit * pUser,CUnit * pTarget);

private:

	HRESULT Process(CUnit * pUser);

	BM::Stream m_kPacket;
	SGroundKey const & m_kGndKey;
};

class PgAction_MissionEffectCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_MissionEffectCheck::PgAction_MissionEffectCheck(VEC_INT const& rkEffectValue, PgGround *pkGround, SGroundKey const &kGroundKey, bool const bCheckPenalty = false )
		:VecEffectValue(rkEffectValue), m_pkGround(pkGround), m_kGndKey(kGroundKey), m_bCheckPenalty(bCheckPenalty){}
	virtual ~PgAction_MissionEffectCheck(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
private:
	VEC_INT const &VecEffectValue;
	bool const m_bCheckPenalty;
	PgGround *m_pkGround;
	SGroundKey const &m_kGndKey;
};

class PgAction_MissionEventQuestCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_MissionEventQuestCheck::PgAction_MissionEventQuestCheck(VEC_INT const& rkQuestValue, PgGround *pkGround, SGroundKey const &kGroundKey)
		:VecQuestValue(rkQuestValue), m_pkGround(pkGround), m_kGndKey(kGroundKey){}
	virtual ~PgAction_MissionEventQuestCheck(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
private:
	VEC_INT const &VecQuestValue;
	PgGround *m_pkGround;
	SGroundKey const &m_kGndKey;
};

class PgAction_MissionEventHiddenLevelClearCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_MissionEventHiddenLevelClearCheck::PgAction_MissionEventHiddenLevelClearCheck(unsigned int iMissionKey, int const iLevel, PgGround *pkGround, SGroundKey const &kGroundKey)
		:m_iMissionKey(iMissionKey), m_iLevel(iLevel), m_pkGround(pkGround), m_kGndKey(kGroundKey){}
	virtual ~PgAction_MissionEventHiddenLevelClearCheck(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
private:
	unsigned int m_iMissionKey;
	int const m_iLevel;
	PgGround *m_pkGround;
	SGroundKey const &m_kGndKey;
};
class PgAction_MissionEventHiddenRankCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_MissionEventHiddenRankCheck::PgAction_MissionEventHiddenRankCheck(PgGround *pkGround, SGroundKey const &kGroundKey)
		:m_pkGround(pkGround), m_kGndKey(kGroundKey){}
	virtual ~PgAction_MissionEventHiddenRankCheck(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
private:
	PgGround *m_pkGround;
	SGroundKey const &m_kGndKey;
};

class PgAction_MissionPartyMemberCheck
	:	public PgUtilAction
{
public:
	explicit PgAction_MissionPartyMemberCheck::PgAction_MissionPartyMemberCheck(PgGround *pkGround)
		:m_pkGround(pkGround){}
	virtual ~PgAction_MissionPartyMemberCheck(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);
private:
	PgGround *m_pkGround;
};


class PgAction_ReqGemStoreBuy
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqGemStoreBuy(BM::GUID const & kNpcGuid,SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kNpcGuid(kNpcGuid),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqGemStoreBuy(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	BM::GUID const & m_kNpcGuid;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqDefGemStoreBuy
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqDefGemStoreBuy(BM::GUID const & kNpcGuid,SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kNpcGuid(kNpcGuid),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqDefGemStoreBuy(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	BM::GUID const & m_kNpcGuid;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqDefCollectAntique
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqDefCollectAntique(SGroundKey const& kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqDefCollectAntique(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqDefExchangeGemStore
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqDefExchangeGemStore(SGroundKey const& kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqDefExchangeGemStore(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqUseEmotion
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqUseEmotion(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqUseEmotion(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqRollbackEnchant
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqRollbackEnchant(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqRollbackEnchant(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqOpenLockedChest
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqOpenLockedChest(SGroundKey const &kGroundKey,BM::Stream & kPacket, PgEventAbil const * const pkEventAbil):m_kGndKey(kGroundKey),m_kPacket(kPacket),m_pkEventAbil(pkEventAbil){}
	virtual ~PgAction_ReqOpenLockedChest(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
	PgEventAbil const * const m_pkEventAbil;
};

class PgAction_ReqOpenGamble
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqOpenGamble(SGroundKey const &kGroundKey,BM::Stream & kPacket, PgEventAbil const * const pkEventAbil):m_kGndKey(kGroundKey),m_kPacket(kPacket),m_pkEventAbil(pkEventAbil){}
	virtual ~PgAction_ReqOpenGamble(){}

	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

	static HRESULT PopItem(  CUnit *pUser, int const iGamebleID, PgBase_Item &rkRetItem, PgEventAbil const * const pkEventAbil );
private:
	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
	PgEventAbil const * const m_pkEventAbil;
};

class PgAction_EventProcess
	:	public PgAction_Callback
{
public:
	PgAction_EventProcess(){}
	~PgAction_EventProcess(){}
	void DoAction(CONT_EVENT_STATE::mapped_type & kEvent,PgPlayer * pkPlayer);
};

class PgAction_ReqJoinEvent
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqJoinEvent(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqJoinEvent(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	PgAction_EventProcess m_kEventProcess;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};


class PgAction_ReqConvertItem
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqConvertItem(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqConvertItem(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqSetExpCard
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqSetExpCard(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqSetExpCard(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};


class PgAction_ReqUseExpCard
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqUseExpCard(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqUseExpCard(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_PopEmporiaCapsule
	:	public PgUtilAction
{
public:
	explicit PgAction_PopEmporiaCapsule( PgPlayer *pkPlayer, PgEmporia const &kEmporia, short const nFuncNo=EFUNC_CAPSULE_CREATURE ):m_pkPlayer(pkPlayer),m_kEmporia(kEmporia),m_nFuncNo(nFuncNo){}
	virtual ~PgAction_PopEmporiaCapsule(){}

	virtual bool PopItem( PgBase_Item & rkOutItem )const;

	static int GetEmporiaFuncValue( SEmporiaKey const &kKey, short const nFuncNo );
private:
	PgEmporia const &m_kEmporia;
	short const		m_nFuncNo;
	PgPlayer		*m_pkPlayer;

private://사용금지
	PgAction_PopEmporiaCapsule(void);
	virtual bool DoAction( CUnit *, CUnit *){return false;}
};

class PgAction_ReqMixupItem
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMixupItem(BM::GUID const & kNpcGuid,SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kNpcGuid(kNpcGuid),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMixupItem(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);
	BM::GUID const & m_kNpcGuid;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHometownEnter
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHometownEnter(PgGround * pkGround,SGroundKey const &kGroundKey,BM::Stream & kPacket):m_pkGround(pkGround),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHometownEnter(){}
	virtual bool DoAction(CUnit* pUser, CUnit* Nothing);

private:

	HRESULT const Process(CUnit* pUser);
	PgGround * m_pkGround;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHomeEquipItem
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeEquipItem(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHomeEquipItem(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHomeUnEquipItem
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeUnEquipItem(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHomeUnEquipItem(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHomeUseItemEffect
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeUseItemEffect(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHomeUseItemEffect(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqMyHomeAuctionReg
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMyHomeAuctionReg(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMyHomeAuctionReg(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	__int64 const CalcAuctionTex(__int64 i64AuctionCost);

	HRESULT const Process(CUnit* pUser);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqMyHomeVisitFlag
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMyHomeVisitFlag(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMyHomeVisitFlag(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit* pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqMyHomeBidding
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMyHomeBidding(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMyHomeBidding(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqMyHomeAuctionUnreg
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMyHomeAuctionUnreg(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMyHomeAuctionUnreg(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqMyHomePostInvitationCard
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqMyHomePostInvitationCard(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqMyHomePostInvitationCard(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHomeItemModify
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeItemModify(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHomeItemModify(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqHomeExit
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeExit(PgGround * const pkGround,SGroundKey const &kGroundKey,BM::Stream & kPacket,bool const & kHasAnsPacket = true):
	m_pkGround(pkGround),m_kGndKey(kGroundKey),m_kPacket(kPacket),m_kHasAnsPacket(kHasAnsPacket){}
	virtual ~PgAction_ReqHomeExit(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	PgGround * const m_pkGround;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
	bool const & m_kHasAnsPacket;
};

class PgAction_ReqHomeMapMove
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqHomeMapMove(PgGround * const pkGround,SGroundKey const &kGroundKey,BM::Stream & kPacket):
	m_pkGround(pkGround),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqHomeMapMove(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	PgGround * const m_pkGround;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_PartyBufRefresh
	:	public PgUtilAction
{
public:
	explicit PgAction_PartyBufRefresh(PgGround *pkGround, SGroundKey const& rkGndKey, PgLocalPartyMgr &kLocalPartyMgr, int const iPrevClass);
	virtual ~PgAction_PartyBufRefresh(){}

public:
	virtual bool DoAction(CUnit* pkCaster, CUnit* pkTarget);

	PgGround *m_pkGround;
	CLASS_DECLARATION_S(int, PrevClass);
	CLASS_DECLARATION_S(SGroundKey, GndKey);

private:
	PgLocalPartyMgr &m_kLocalPartyMgr;

private:
	PgAction_PartyBufRefresh();
};

class PgAction_ReqUseGambleMachine
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqUseGambleMachine(SGroundKey const &kGroundKey,BM::Stream & kPacket):m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqUseGambleMachine(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_ReqUseGambleMachine_CashShop
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqUseGambleMachine_CashShop(PgGround * const pkGround, SGroundKey const &kGroundKey,BM::Stream & kPacket):
	m_pkGround(pkGround),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqUseGambleMachine_CashShop(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	PgGround * const m_pkGround;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

typedef std::vector<PgBase_Item> CONT_EVENTITEM_REWARD_RESULT;

class PgAction_OpenEventItemReward
	:	public PgUtilAction
{
public:
	
	explicit PgAction_OpenEventItemReward(SGroundKey const &kGroundKey,int const iItemNo):m_kGndKey(kGroundKey),m_kItemNo(iItemNo){}
	virtual ~PgAction_OpenEventItemReward(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	bool CreateItemProcess(SEVENT_ITEM_REWARD_ELEMENT const & kElement, CONT_EVENTITEM_REWARD_RESULT & kResult);
	bool CreateItemProcess_Sub(SEVENT_ITEM_REWARD_ELEMENT const & kElement, short const siCount, PgBase_Item & kItem);
	SGroundKey const &m_kGndKey;
	int const m_kItemNo;
};

class PgAction_ReqOpenTreasureChest
{
public:
	explicit PgAction_ReqOpenTreasureChest(const SGroundKey &kGroundKey, SItemPos kChestPos, SItemPos kKeyPos) : 
		m_kGndKey(kGroundKey), m_kChestPos(kChestPos), m_kKeyPos(kKeyPos) {}
	virtual ~PgAction_ReqOpenTreasureChest() {}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:
	SGroundKey const &m_kGndKey;
	SItemPos m_kChestPos, m_kKeyPos;
	typedef CONT_EVENT_ITEM_REWARD_ELEMENT::const_iterator CONST_ELEM_ITER;

	void SendErrorPacket(CUnit* pUser, int iErrorNo);
	bool CreateItemProcess(SEVENT_ITEM_REWARD_ELEMENT const & kElement, CONT_EVENTITEM_REWARD_RESULT & kResult);
	bool CreateItemProcess_Sub(SEVENT_ITEM_REWARD_ELEMENT const & kElement, short const siCount, PgBase_Item & kItem);
};


//
class PgAction_MonsterEnchantProbCalc
{
public:
	PgAction_MonsterEnchantProbCalc(int const iMonEnchantProbGroupNo);
	~PgAction_MonsterEnchantProbCalc();

	int Get();
private:
	int m_iMonEnchantProbGroupNo;
	CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP const* m_pkDefMonEnchantGradeProb;
};

//
class PgAction_Myhome_Buy
	:	public PgUtilAction
{
public:
	
	explicit PgAction_Myhome_Buy(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_Myhome_Buy(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_BasicOptionAmp
	:	public PgUtilAction
{
public:
	
	explicit PgAction_BasicOptionAmp(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket), m_bUseInsurance(false){}
	virtual ~PgAction_BasicOptionAmp(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT OnProcess(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
	bool m_bUseInsurance;
};

class PgAction_ReqUseGambleMachine_Mixup
	:	public PgUtilAction
{
public:
	
	explicit PgAction_ReqUseGambleMachine_Mixup(PgGround * const pkGround, SGroundKey const &kGroundKey,BM::Stream & kPacket):
	m_pkGround(pkGround),m_kGndKey(kGroundKey),m_kPacket(kPacket){}
	virtual ~PgAction_ReqUseGambleMachine_Mixup(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT const Process(CUnit* pUser,CUnit * pTarget);
	PgGround * const m_pkGround;
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

class PgAction_Myhome_Sell
	:	public PgUtilAction
{
public:
	
	explicit PgAction_Myhome_Sell(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_Myhome_Sell(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

struct SEnchantShiftInfo;
class PgAction_Enchant_Shift
	:	public PgUtilAction
{
public:
	
	explicit PgAction_Enchant_Shift(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket), m_pkPlusInfo(NULL){}
	virtual ~PgAction_Enchant_Shift(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	HRESULT CheckEnableShift(SEnchantShiftInfo const & kInfo);
	HRESULT CheckInsurance(CUnit* pUser);
	bool	CheckShiftCost(CUnit* pUser, PgBase_Item const & kItem);
	bool	CheckNeedItems(CUnit* pUser, int const iNeedItemCount, ContHaveItemNoCount const & kCont);
	int		BuildRandResult(SEnchantShiftInfo const & kInfo);
	bool	IsEnableShift(eEnchantShiftRate const & eRate, SEnchantShiftInfo const & kInfo)const;

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;

	CONT_PLAYER_MODIFY_ORDER m_kOrder;
	PgBase_Item m_kItem[ESIP_NUM];
	SItemPos m_kItemPos[ESIP_NUM];
	TBL_DEF_ITEMENCHANTSHIFT const * m_pkPlusInfo;
};


extern TBL_DEF_ITEMPLUSUPGRADE const * GetPlusInfo(int const iNextLv, int const iEquipPos, bool const bIsPet );

class PgAction_JobSkill3_CreateItem
	:	public PgUtilAction
{
public:
	
	explicit PgAction_JobSkill3_CreateItem(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket), m_iSaveIdx(0){}
	virtual ~PgAction_JobSkill3_CreateItem(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;

	CONT_PLAYER_MODIFY_ORDER m_kOrder;
	int m_iSaveIdx;
};

class PgAction_SoulTransfer_Extract
	:	public PgUtilAction
{
public:
	
	explicit PgAction_SoulTransfer_Extract(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_SoulTransfer_Extract(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;

	CONT_PLAYER_MODIFY_ORDER m_kOrder;
};

class PgAction_SoulTransfer_Transition
	:	public PgUtilAction
{
public:
	
	explicit PgAction_SoulTransfer_Transition(SGroundKey const &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_SoulTransfer_Transition(){}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:

	HRESULT Process(CUnit* pUser, CUnit* pTarget);

	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;

	CONT_PLAYER_MODIFY_ORDER m_kOrder;
};

class PgAction_Manufacture
	:	public PgUtilAction
{
public:
	explicit PgAction_Manufacture(const SGroundKey &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_Manufacture() {}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;

	bool IsLearnedSkill(CUnit* pUser, CItemDef const* pItemDef);
	bool PopItemByScore(int iItemBagNo, int iLevel, BYTE byScore, int& iOutItemNo, int& iOutItemCount);
};

class PgAction_BundleManufacture
	:	public PgUtilAction
{
public:
	explicit PgAction_BundleManufacture(const SGroundKey &kGroundKey, BM::Stream & kPacket):m_kGndKey(kGroundKey), m_kPacket(kPacket){}
	virtual ~PgAction_BundleManufacture() {}
	virtual bool DoAction(CUnit* pUser, CUnit* pTarget);

private:
	SGroundKey const &m_kGndKey;
	BM::Stream & m_kPacket;
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTION_H