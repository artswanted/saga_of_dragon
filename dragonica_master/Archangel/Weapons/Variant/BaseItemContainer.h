#ifndef WEAPON_VARIANT_USERINFO_BASEITEMCONTAINER_H
#define WEAPON_VARIANT_USERINFO_BASEITEMCONTAINER_H

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "Lohengrin/packetstruct.h"
#include "Lohengrin/Errorcode.h"
#include "variant/item.h"
#include "collins/Log.h"

typedef std::vector<SPMOD_Modify_Count> CONT_MODIFY_COUNT;
typedef std::vector< SItemPos > CONT_EMPTY_ITEM_POS;

typedef std::map< SItemPos, PgItemWrapper > CONT_ITEM_WRAPPER;

class CUnit;

class PgBaseItemContainer
	:	public CONT_ITEM_WRAPPER
{//아이템 인덱스가 0 부터냐? 1 부터냐?
public:
	
	explicit PgBaseItemContainer(BM::GUID const & kInOwnerGuid) // 기본 오너 guid는 캐릭터 guid를 사용한다. 
		:m_kOwnerGuid(kInOwnerGuid),m_kCharacterGuid(kInOwnerGuid),m_kReduction(0),m_kExtendMaxIdx(0)
	{}

	explicit PgBaseItemContainer(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid) // 계정 공유 창고는 맴버 guid를 오너 guid로 사용하고 결과 패킷 전송용 캐릭터 guid를 따로 설정한다.
		:m_kOwnerGuid(kInOwnerGuid),m_kCharacterGuid(kInCharacterGuid),m_kReduction(0),m_kExtendMaxIdx(0)
	{}

	virtual ~PgBaseItemContainer();

public:
	static HRESULT MergeItem(PgItemWrapper &kCasterWrapper, PgItemWrapper &kTargetWrapper, __int64& iCause);

	virtual EInvType InvType()const=0;// 이따가 가상함수로 변경
	virtual int		GetMaxIDX()const=0;// 이따가 가상함수로 변경
	virtual int		GetMaxIDXExtend()const=0;
	virtual HRESULT WriteToPacket(BM::Stream &kPacket)const;// 이따가 가상함수로 변경
	virtual HRESULT ReadFromPacket(BM::Stream &kPacket);

	virtual HRESULT OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT CanModify(PgItemWrapper const &kItemWrapper)const;
	virtual HRESULT EmptyPos(SItemPos &rkOutPos)const;
	virtual HRESULT InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT InsertItem(PgBase_Item const & kItem, SItemPos const & kPos);

	virtual HRESULT ModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT PopItem(__int64 const iCause, SItemPos const &rkPos, PgItemWrapper& rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT Release();
	virtual HRESULT SetItem(PgBase_Item const & kItem, SItemPos const & kPos);
	virtual bool EmulateModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount,CONT_MODIFY_COUNT & kContModifyCount, int & riModifyCount,bool const bIsDuration = true);
	virtual bool EnumEmptyInvPos(CONT_EMPTY_ITEM_POS & kContEmptyItemPos, int const iLimitCount = 0);

	virtual HRESULT GetItem(SItemPos const & kPos, PgBase_Item &kOutItem) const;
	virtual HRESULT GetItemID( SItemPos const & kPos, BM::GUID& kOutItemID )const;

	virtual size_t GetItemCount( BYTE const byChkState )const;

	virtual HRESULT SwapItem( SItemPos const & kPos, PgBase_Item &kItem, bool const bSameID );

	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
	void	MakeSortOrderSub(CUnit const * const pkUnit,CONT_ITEM_WRAPPER const & kOrg,CONT_ITEM_WRAPPER const & kCopy, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;

	int GetItemEnchantLevel(SItemPos const& rkPos);

	bool OnTimeOutedItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
	bool OnTimeOutedMonsterCard(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;

	void Clear();
	void Swap( PgBaseItemContainer &rhs );

	void operator = (const PgBaseItemContainer &rhs);
	PgBaseItemContainer(const PgBaseItemContainer &rhs);
protected:
//	mutable Loki::Mutex m_kMutex;//
//	typedef std::map< SItemPos, PgItemWrapper > ItemHash;

//	ItemHash m_kItemCont;

	BM::GUID const & OwnerGuid()const{return m_kOwnerGuid;}
	BM::GUID const & CharacterGuid()const{return m_kCharacterGuid;}
	
	BM::GUID const & m_kOwnerGuid;
	BM::GUID const & m_kCharacterGuid;
	
	CLASS_DECLARATION_S(BYTE,Reduction);
	CLASS_DECLARATION_S(BYTE,ExtendMaxIdx);
};

class PgFitItemContainer
	:	public PgBaseItemContainer
{
public:
	explicit PgFitItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid)
	{}
	virtual ~PgFitItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_FIT;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_FIT_IDX;}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_FIT_IDX;}
	virtual HRESULT CanModify(PgItemWrapper const &kItemWrapper)const;
};

class PgEquipItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgEquipItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid)
	{}
	virtual ~PgEquipItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_EQUIP;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_EQUIP_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_EQUIP_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgConsumeItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgConsumeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgConsumeItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_CONSUME;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_CONSUME_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_CONSUME_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgEtcItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgEtcItemContainer(BM::GUID const & kInOwnerGuid)
	:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgEtcItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_ETC;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_ETC_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_ETC_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgCashItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgCashItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgCashItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_CASH;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_CASH_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_CASH_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgSafeItemContainer
	:public PgBaseItemContainer
{
public:
	
	explicit PgSafeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}

	explicit PgSafeItemContainer(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgBaseItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgSafeItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_SAFE;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_SAFE_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_SAFE_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgCashSafeItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgCashSafeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgCashSafeItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_CASH_SAFE;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_CASH_SAFE_IDX;}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_CASH_SAFE_IDX_EXTEND;}
	virtual HRESULT MakeSortOrder(CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder) const;
};

class PgPostItemContainer
	: public PgBaseItemContainer
{
public:
	explicit PgPostItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgPostItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_POST;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_POST_IDX;}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_POST_IDX;}

	virtual HRESULT WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(size());
		return S_OK;
	}
	virtual HRESULT ReadFromPacket(BM::Stream &kPacket)
	{
		size_t const_size = 0;
		kPacket.Pop(const_size);
		return S_OK;
	}
	virtual HRESULT OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT CanModify(PgItemWrapper const &kItemWrapper)const{return S_OK;}
	virtual HRESULT EmptyPos(SItemPos &rkOutPos)const{rkOutPos.Set(IT_USER_MARKET,0);return S_OK;}
	virtual HRESULT InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return OverWriteItem(iCause, kItemWrapper, kChangeArray,kContLogMgr);}
	virtual HRESULT ModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT PopItem(__int64 const iCause, SItemPos const &rkPos, PgItemWrapper& rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT Clone(PgBaseItemContainer *pkOutInv)const{return S_OK;}
	virtual HRESULT Release(){return S_OK;}
};

class PgHomeItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgHomeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}
	virtual ~PgHomeItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_HOME;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_HOME_IDX;}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_HOME_IDX;}
};

class PgUserMarketItemContainer
	: public PgBaseItemContainer
{
public:
	explicit PgUserMarketItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}

	virtual ~PgUserMarketItemContainer(){}

public:

	virtual EInvType InvType()const{return IT_USER_MARKET;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_USER_MARKET_IDX;}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_USER_MARKET_IDX;}

	virtual HRESULT WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(size());
		return S_OK;
	}
	virtual HRESULT ReadFromPacket(BM::Stream &kPacket)
	{
		size_t const_size = 0;
		kPacket.Pop(const_size);
		return S_OK;
	}
	virtual HRESULT OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr);
	virtual HRESULT CanModify(PgItemWrapper const &kItemWrapper)const{return S_OK;}
	virtual HRESULT EmptyPos(SItemPos &rkOutPos)const{rkOutPos.Set(IT_USER_MARKET,0);return S_OK;}
	virtual HRESULT InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return OverWriteItem(iCause,kItemWrapper,kChangeArray,kContLogMgr);}
	virtual HRESULT ModifyCount(__int64 const iCause, int const iItemNo, int const iAddCount, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT PopItem(__int64 const iCause, SItemPos const &rkPos, PgItemWrapper& rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr){return S_OK;}
	virtual HRESULT Clone(PgBaseItemContainer *pkOutInv)const{return S_OK;}
	virtual HRESULT Release(){return S_OK;}
};

class PgFitCashItemContainer : public PgFitItemContainer
{
public:
	explicit PgFitCashItemContainer(BM::GUID const & kInOwnerGuid)
		:PgFitItemContainer(kInOwnerGuid){}
	virtual ~PgFitCashItemContainer(){}
public:

	virtual EInvType InvType()const{return IT_FIT_CASH;}
};

class PgFitCostumeItemContainer : public PgFitItemContainer
{
public:
	explicit PgFitCostumeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgFitItemContainer(kInOwnerGuid) {}
	virtual ~PgFitCostumeItemContainer() {}
public:

	virtual EInvType InvType()const { return IT_FIT_COSTUME; }
};


class PgRentalSafeItemContainer
	:public PgSafeItemContainer
{
public:
	explicit PgRentalSafeItemContainer(BM::GUID const & kInOwnerGuid)
		:PgSafeItemContainer(kInOwnerGuid){}
	explicit PgRentalSafeItemContainer(BM::GUID const & kInOwnerGuid, BM::GUID const & kInCharacterGuid)
		:PgSafeItemContainer(kInOwnerGuid, kInCharacterGuid){}
	virtual ~PgRentalSafeItemContainer(){}
public:
	virtual int GetMaxIDX()const{return MAX_TIME_SAFE_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_TIME_SAFE_IDX_EXTEND;}
	void SetEnableUseTime(__int64 const i64RentalTime)
	{
		m_i64EnableUseTime = i64RentalTime;
	}
	bool CheckEnableUse()
	{
		if(m_i64EnableUseTime < g_kEventView.GetLocalSecTime())
		{
			return false;
		}
		return true;
	}
	virtual HRESULT WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(m_i64EnableUseTime);
		return PgBaseItemContainer::WriteToPacket(kPacket);
	}

	virtual HRESULT ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_i64EnableUseTime);
		return PgBaseItemContainer::ReadFromPacket(kPacket);
	}

	void operator = (const PgRentalSafeItemContainer &rhs)
	{
		PgBaseItemContainer::operator = (rhs);
		m_i64EnableUseTime = rhs.GetEanbleUseTime();
	}

	__int64 const GetEanbleUseTime()const{return m_i64EnableUseTime;}

	void Swap( PgRentalSafeItemContainer &rhs )
	{
		PgBaseItemContainer::Swap( rhs );
		std::swap( m_i64EnableUseTime, rhs.m_i64EnableUseTime );
	}
protected:

	__int64 m_i64EnableUseTime;
};

class PgRentalSafeItemContainer_1 : public PgRentalSafeItemContainer
{
public:
	explicit PgRentalSafeItemContainer_1(BM::GUID const & kInOwnerGuid)
		:PgRentalSafeItemContainer(kInOwnerGuid){}
	virtual ~PgRentalSafeItemContainer_1(){}
public:
	virtual EInvType InvType()const{return IT_RENTAL_SAFE1;}
};

class PgRentalSafeItemContainer_2 : public PgRentalSafeItemContainer
{
public:
	explicit PgRentalSafeItemContainer_2(BM::GUID const & kInOwnerGuid)
		:PgRentalSafeItemContainer(kInOwnerGuid){}
	virtual ~PgRentalSafeItemContainer_2(){}
public:
	virtual EInvType InvType()const{return IT_RENTAL_SAFE2;}
};

class PgRentalSafeItemContainer_3 : public PgRentalSafeItemContainer
{
public:
	explicit PgRentalSafeItemContainer_3(BM::GUID const & kInOwnerGuid)
		:PgRentalSafeItemContainer(kInOwnerGuid){}
	virtual ~PgRentalSafeItemContainer_3(){}
public:
	virtual EInvType InvType()const{return IT_RENTAL_SAFE3;}
};

class PgPetItemContainer
	:public PgBaseItemContainer
{
public:
	explicit PgPetItemContainer(BM::GUID const & kInOwnerGuid)
		:	PgBaseItemContainer(kInOwnerGuid){}

	virtual ~PgPetItemContainer(){}

public:
	virtual EInvType InvType()const{return IT_PET;}
	virtual int GetMaxIDX()const{return MAX_PET_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return MAX_PET_IDX_EXTEND;}
};


class PgShareRentalSafeItemContainer : public PgRentalSafeItemContainer
{
public:
	explicit PgShareRentalSafeItemContainer(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgRentalSafeItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgShareRentalSafeItemContainer(){}
public:
	virtual int GetMaxIDX()const{return MAX_SHARE_TIME_SAFE_IDX + ExtendMaxIdx();}// 계정 공유 창고는 12개 부터 시작한다. 축소 기능 없으며 확장만 지원함
	virtual int	GetMaxIDXExtend()const{return MAX_SHARE_TIME_SAFE_EXTEND;}
};

class PgShareRentalSafeItemContainer_1 : public PgShareRentalSafeItemContainer
{
public:
	explicit PgShareRentalSafeItemContainer_1(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgShareRentalSafeItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgShareRentalSafeItemContainer_1(){}
public:
	virtual EInvType InvType()const{return IT_SHARE_RENTAL_SAFE1;}
};

class PgShareRentalSafeItemContainer_2 : public PgShareRentalSafeItemContainer
{
public:
	explicit PgShareRentalSafeItemContainer_2(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgShareRentalSafeItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgShareRentalSafeItemContainer_2(){}
public:
	virtual EInvType InvType()const{return IT_SHARE_RENTAL_SAFE2;}
};

class PgShareRentalSafeItemContainer_3 : public PgShareRentalSafeItemContainer
{
public:
	explicit PgShareRentalSafeItemContainer_3(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgShareRentalSafeItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgShareRentalSafeItemContainer_3(){}
public:
	virtual EInvType InvType()const{return IT_SHARE_RENTAL_SAFE3;}
};

class PgShareRentalSafeItemContainer_4 : public PgShareRentalSafeItemContainer
{
public:
	explicit PgShareRentalSafeItemContainer_4(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgShareRentalSafeItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgShareRentalSafeItemContainer_4(){}
public:
	virtual EInvType InvType()const{return IT_SHARE_RENTAL_SAFE4;}
};

class PgSafeAddItemContainer
	:public PgBaseItemContainer
{
public:

	explicit PgSafeAddItemContainer(BM::GUID const & kInOwnerGuid)
		:PgBaseItemContainer(kInOwnerGuid){}

	explicit PgSafeAddItemContainer(BM::GUID const & kInOwnerGuid,BM::GUID const & kInCharacterGuid)
		:PgBaseItemContainer(kInOwnerGuid,kInCharacterGuid){}
	virtual ~PgSafeAddItemContainer(){}
public:
	virtual EInvType InvType()const{return IT_SAFE_ADDITION;}// 이따가 가상함수로 변경
	virtual int GetMaxIDX()const{return MAX_SAFE_ADD_IDX - Reduction() + ExtendMaxIdx();}// 이따가 가상함수로 변경
	virtual int	GetMaxIDXExtend()const{return 0;}
};

#endif // WEAPON_VARIANT_USERINFO_BASEITEMCONTAINER_H