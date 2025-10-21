#ifndef MAP_MAPSERVER_ACTION_PGGENPOINTMGR_H
#define MAP_MAPSERVER_ACTION_PGGENPOINTMGR_H

#include "BM/ClassSupport.h"
#include "Variant/PgObjectUnit.h"

//리로드 되었을때 어떻게 하지?
//이미 해당 포인트에 이상하게 하나 나와 있으면?... 거참...
#pragma pack(1)
typedef struct tagGenGroupKey
{
	tagGenGroupKey( int const _iMapNo=0, int const _iBalance=0 )
		:	iMapNo(_iMapNo)
		,	iBalance(_iBalance)
	{
	}

	int		iMapNo;
	int		iBalance;

	bool operator < (tagGenGroupKey const &rhs)const
	{
		if( iMapNo < rhs.iMapNo )	{return true;}
		if( iMapNo > rhs.iMapNo )	{return false;}

		if( iBalance < rhs.iBalance )	{return true;}
		if( iBalance > rhs.iBalance )	{return false;}
	
		return false;
	}
}SGenGroupKey;

#pragma pack()

typedef SET_GUID	OwnChildCont;

class PgGenPoint
{ // 젠 포인트 자체의 좌표나 기타등등 정보와
	// MonBag 도 가지도록 하자.
#pragma pack(1)
	typedef struct tagMonsterElement
	{
		tagMonsterElement(int const iInMonNo, short const nInRate)
		{
			iMonNo = iInMonNo;
			nRate = nInRate;
		}

		bool IsEmpty()const
		{
			if(iMonNo && nRate)
			{
				return false;
			}
			return true;
		}
		int iMonNo;
		short nRate;
	}SMonsterElement;
#pragma pack()

	typedef std::vector< SMonsterElement > BagElementCont; //설계가 이상한거 아닌가?.. 포인트 자체를 가져야지..
public:
	PgGenPoint();
	virtual ~PgGenPoint();

	HRESULT PopMonster(int &rOutResultItemNo);
	HRESULT IsMonster(int const MonNo)const;
	bool Build(CONT_DEF_MONSTER_BAG_ELEMENTS::mapped_type const &rkMonsterBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkSuccessRateControl);
	bool Build(TBL_DEF_MAP_REGEN_POINT const &rkInfo, CONT_DEF_MONSTER_BAG_ELEMENTS::mapped_type const &rkMonsterBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkSuccessRateControl);
	bool Build(CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const & ItemBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const & SuccessRateControl);
	bool Build(TBL_DEF_MAP_REGEN_POINT const & Info, CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const & ItemBag, CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const & SuccessRateControl);

	bool CanGen()const;
	bool AddChild(BM::GUID const &kUnitGuid);
	bool RemoveChild(BM::GUID const &kUnitGuid);
	void RemoveAll();
	void GetChild(OwnChildCont & kContChild)const{kContChild = m_kOwnChild;}
	size_t ChildCount() const	{ return m_kOwnChild.size(); }

protected:
	void AccTotalRaiseValue();
	//여기서 나온놈 관리 해줘야되고. 
	//리로드 되면 GUID가 닦이는데... DB에다 GUID 넣을 수도 없고...
	//몬스터가 마더 포인트 가지려면 GUID 밖에 없고....
	//전체 갯수로 관리하기도그렇고...
	CLASS_DECLARATION_S(TBL_DEF_MAP_REGEN_POINT, Info);
	CLASS_DECLARATION_S(int, TotalRaiseRate);
	CLASS_DECLARATION_S(DWORD, LastGenTime);

protected:	
	OwnChildCont	m_kOwnChild;	
	BagElementCont	m_kContBag;	// MaxCount, RaiseRate, ItemNo 가 병합된 최종 데이터가 해당 해쉬에 세팅되어진다.
};


class PgGenPoint_Object
{
public:
	typedef std::vector<SObjUnitBaseInfo>	OwnGroupCont;

public:
	PgGenPoint_Object();
	PgGenPoint_Object(BM::GUID const &rkGuid);
	virtual ~PgGenPoint_Object();

	bool Build( TBL_DEF_MAP_REGEN_POINT const &kInfo, 
				CONT_DEF_OBJECT_BAG const &kObjBags,
				CONT_DEF_OBJECT_BAG_ELEMENTS const &kObjElements );

	void GetBaseInfo( OwnGroupCont &rkOutCont )const;

	bool CanGen(DWORD const dwCurTime)const;
	bool AddChild(BM::GUID const &kUnitGuid);
	bool RemoveChild(BM::GUID const &kUnitGuid);
	void RemoveAll();
	void Reset();
	void GetChild(OwnChildCont & kContChild){kContChild = m_kOwnChild;}
	size_t ChildCount() const	{ return m_kOwnChild.size(); }

	CLASS_DECLARATION_NO_SET(TBL_DEF_MAP_REGEN_POINT,m_kInfo,Info);

protected:
	DWORD			m_dwRemoveAllTime;
	OwnGroupCont	m_kGroup;
	OwnChildCont	m_kOwnChild;
};

template<typename T_POINT>
class PgGenGroup
{
public:
	typedef std::map<BM::GUID,T_POINT>	ContGenPoint;
	typedef std::map<int,ContGenPoint>	ContGenGroup;

public:
	PgGenGroup(void){}
	~PgGenGroup(void){}

	void insert( int const iGenGroup, BM::GUID const& kGuid, T_POINT const& kElement )
	{
		ContGenGroup::iterator itr = m_kContGenGroup.find( iGenGroup );
		if ( itr == m_kContGenGroup.end() )
		{
			auto kRet = m_kContGenGroup.insert( std::make_pair(iGenGroup, ContGenPoint()) );
			itr = kRet.first;
		}
		itr->second.insert( std::make_pair(kGuid, kElement) );
	}

	HRESULT Get( ContGenPoint& rOutCont, bool bReset=true, int const iGenGroup=0 )const
	{
		ContGenGroup::const_iterator group_itr = m_kContGenGroup.find( iGenGroup );
		if ( group_itr != m_kContGenGroup.end() )
		{
			if ( bReset )
			{
				rOutCont.clear();
				rOutCont = group_itr->second;
			}
			else
			{
				ContGenPoint::const_iterator point_itr = group_itr->second.begin();
				for( ; point_itr!=group_itr->second.end(); ++point_itr )
				{
					rOutCont.insert( std::make_pair(point_itr->first, point_itr->second) );
					
				}
			}
			return S_OK;
		}
		return E_FAIL;
	}

private:
	ContGenGroup	m_kContGenGroup;
};

typedef std::map<BM::GUID,PgGenPoint>			ContGenPoint_Monster;
typedef std::map<BM::GUID,PgGenPoint_Object>	ContGenPoint_Object;
typedef std::map<BM::GUID,PgGenPoint>			ContGenPoint_InstanceItem;

class PgGenPointMgr
{
	typedef std::map< SGenGroupKey, PgGenGroup<PgGenPoint> >	CONT_REGEN_GROUP_MONSTER;//Key = 맵 번호;
	typedef std::map< int, PgGenGroup<PgGenPoint_Object> >		CONT_REGEN_GROUP_OBJECT;

public:
	PgGenPointMgr(void);
	virtual ~PgGenPointMgr(void);

	void Clear();
	void swap(PgGenPointMgr & rRight);
	bool Build( CONT_DEF_MAP_REGEN_POINT const &rkMonsterRegen,
				CONT_DEF_MONSTER_BAG_CONTROL const &rkMonsterBagControl,
				CONT_DEF_MONSTER_BAG const &rkMonsterBag,
				CONT_DEF_MONSTER_BAG_ELEMENTS const &rkMonsterBagElements,
				CONT_DEF_SUCCESS_RATE_CONTROL const &rkSuccessRateControl,
				CONT_DEF_OBJECT_BAG const &rkObjectBag,
				CONT_DEF_OBJECT_BAG_ELEMENTS const &rkObjectBagElements,
				CONT_DEF_ITEM_BAG const & ItemBag,
				CONT_DEF_ITEM_BAG_ELEMENTS const & ItemBagElements,
				CONT_MAP_BUILD_DATA const &rkMapBuildData);

	HRESULT GetGenPoint_Monster( ContGenPoint_Monster &rOut, CONT_REGEN_GROUP_MONSTER::key_type const &kKey, bool const bReset=true, int const iGenGroup=0 ) const;
	HRESULT GetGenPoint_Object( ContGenPoint_Object &rOut, CONT_REGEN_GROUP_OBJECT::key_type const &kKey, bool const bReset=true, int const iGenGroup=0 ) const;
	HRESULT GetGenPoint_SummonNPC( ContGenPoint_Object &rOut, CONT_REGEN_GROUP_OBJECT::key_type const &kKey, bool const bReset=true, int const iGenGroup=0 ) const;

protected:
	CONT_REGEN_GROUP_MONSTER	m_kGen_Monster;
	CONT_REGEN_GROUP_OBJECT		m_kGen_Object;
	CONT_REGEN_GROUP_OBJECT		m_kGen_SummonNPC;
};

//#define g_kRegenPointMgr SINGLETON_STATIC(PgGenPointMgr)

#endif // MAP_MAPSERVER_ACTION_PGGENPOINTMGR_H