#ifndef WEAPON_VARIANT_ITEM_PGSTOREMGR_H
#define WEAPON_VARIANT_ITEM_PGSTOREMGR_H

#include "PgStore.h"

//길드전 상점이 추가되면서 CStoreKey로 기존 상점과 길드전 상점을 구분
class CStoreKey
{
public:
    CStoreKey(BM::GUID const & kGuid, BYTE const byType=0);
    ~CStoreKey(){}

    inline bool operator == (CStoreKey const &rhs)const;
	inline bool operator < (CStoreKey const &rhs)const;

private:
    BM::GUID m_kGuid;
    BYTE m_byType;
};


class PgStoreMgr
{
	typedef std::map< CStoreKey, PgStore* > CONT_STORE;//그냥 인덱스 갖고 오는거다.
public:
	PgStoreMgr();
	virtual ~PgStoreMgr();

public:
	HRESULT FindStore(BM::GUID const & kShopGuid, BYTE const byType, PgStore &rkOutStore) const;
	HRESULT FindShopNpcGuid(int const iShopNo,BM::GUID & rkShopNpcGuid) const;

	bool Build(BM::GUID const &kValueKey, CONT_TBL_SHOP_IN_EMPORIA const &kContShopInEmporia, CONT_TBL_SHOP_IN_GAME const &kContShopInGame, CONT_TBL_SHOP_IN_STOCK const &kContShopInStock, CONT_DEF_JOBSKILL_SHOP const& kContJobSkillShop,CONT_SHOPNPC_GUID const & kContShopNpcGuid);//가격표에 대한 GUID가 있어야함.

	BM::GUID ValueKey()const {return m_kValueKey;}
	void swap(PgStoreMgr &rRight);

protected:	
	template< typename T >
	bool Build_Sub( T const &kContShopInGame, BYTE const byType );
	virtual void Clear();

	CONT_STORE m_contStore;
	mutable Loki::Mutex m_kMutex;
	BM::GUID m_kValueKey;

	CONT_SHOPNPC_GUID m_contShopNpcGuid;
};

//#define g_kStoreMgr SINGLETON_STATIC(PgStoreMgr)

#endif // WEAPON_VARIANT_ITEM_PGSTOREMGR_H