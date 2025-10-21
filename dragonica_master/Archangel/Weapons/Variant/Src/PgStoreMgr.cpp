#include "StdAfx.h"
#include "..\PgStoreMgr.h"

typedef std::vector< TBL_SHOP_IN_GAME > CONT_LIST_TBL_SHOP_IN_GAME;

CStoreKey::CStoreKey(BM::GUID const & kGuid, BYTE const byType)
    : m_kGuid(kGuid)
{
    switch(byType)
    {
    case STORE_TYPE_EMPORIA:    { m_byType = STORE_TYPE_EMPORIA;      }break;
    default:                    { m_byType = 0;                       }break;
    }
}

bool CStoreKey::operator == (CStoreKey const &rhs)const
{
	return (m_kGuid == rhs.m_kGuid) && (m_byType == rhs.m_byType); 
}

bool CStoreKey::operator < (CStoreKey const &rhs)const
{
    if( m_kGuid < rhs.m_kGuid )	    {return true;}
    if( m_kGuid > rhs.m_kGuid )     {return false;}

	if( m_byType < rhs.m_byType )	{return true;}
    if( m_byType > rhs.m_byType )	{return false;}

	return false;
}

PgStoreMgr::PgStoreMgr()
{
}

PgStoreMgr::~PgStoreMgr()
{
	Clear();
}

template< typename T >
bool PgStoreMgr::Build_Sub( T const &kContShopInGame, BYTE const byType )
{
	bool bRet = true;
	T::const_iterator tbl_itor = kContShopInGame.begin();
	while(tbl_itor != kContShopInGame.end())
	{
		T::mapped_type const &rTbl = (*tbl_itor).second;

		auto ret = m_contStore.insert(std::make_pair(CStoreKey(rTbl.kShopGuid,byType), (PgStore *)NULL));

		if(!ret.first->second)
		{//상점 개체가 없음
			PgStore *pkStore = new_tr PgStore(byType);
			pkStore->Init(rTbl.kShopGuid);
			ret.first->second = pkStore;
		}

		PgStore *pkStore = ret.first->second;

		if(pkStore)
		{//여기서 샵 이터레이터가 없으면 에러
			if(S_OK != pkStore->AddGoods(rTbl))
			{
				assert(NULL);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				bRet = false;
			}
		}
		else
		{
			bRet = false;
		}

		++tbl_itor;
	}
	return bRet;
}

HRESULT PgStoreMgr::FindStore(BM::GUID const & kShopGuid, BYTE const byType, PgStore &rkOutStore) const
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_STORE::const_iterator shop_itor = m_contStore.find(CStoreKey(kShopGuid,byType));
	if(shop_itor != m_contStore.end())
	{
		rkOutStore = *(*shop_itor).second;
		return S_OK;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgStoreMgr::FindShopNpcGuid(int const iShopNo,BM::GUID & rkShopNpcGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SHOPNPC_GUID::const_iterator npc_iter = m_contShopNpcGuid.find(iShopNo);
	if(npc_iter != m_contShopNpcGuid.end())
	{
		rkShopNpcGuid = (*npc_iter).second;
		return S_OK;
	}
	return E_FAIL;
}

bool PgStoreMgr::Build(BM::GUID const &kValueKey
					   , CONT_TBL_SHOP_IN_EMPORIA const &kContShopInEmporia
					   , CONT_TBL_SHOP_IN_GAME const &kContShopInGame
					   , CONT_TBL_SHOP_IN_STOCK const &kContShopInStock
					   , CONT_DEF_JOBSKILL_SHOP const& kContJobSkillShop
					   , CONT_SHOPNPC_GUID const & kContShopNpcGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	Clear();

	m_kValueKey = kValueKey;//가격표 키.
	m_contShopNpcGuid = kContShopNpcGuid;

	if(		Build_Sub(kContShopInGame, STORE_TYPE_DEFAULT)
        &&  Build_Sub(kContShopInEmporia, STORE_TYPE_EMPORIA)
		&&	Build_Sub(kContShopInStock, STORE_TYPE_STOCK)
		&&	Build_Sub(kContJobSkillShop, STORE_TYPE_JOBKSILL)
	)
	{
		return true;
	}

	return false;
}

void PgStoreMgr::Clear()
{
	m_kValueKey.Clear();

 	CONT_STORE::iterator itor = m_contStore.begin();
 	while(itor != m_contStore.end() )
 	{
 		SAFE_DELETE((*itor).second);
		++itor;
 	}
	m_contStore.clear();
	m_contShopNpcGuid.clear();
}

void PgStoreMgr::swap(PgStoreMgr &rRight)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	m_kValueKey = rRight.m_kValueKey;
	m_contStore.swap(rRight.m_contStore);
	m_contShopNpcGuid.swap(rRight.m_contShopNpcGuid);
}
