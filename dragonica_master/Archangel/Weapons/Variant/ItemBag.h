#ifndef WEAPON_VARIANT_ITEM_ITEMBAG_H
#define WEAPON_VARIANT_ITEM_ITEMBAG_H

//전체 확률 더해줘야되고.
//아이템 빠지면 갱신도 해줘야되고.
//아이템 제거 쉬워야 되고.
// 등등.. 조건을 만족해야함.

#include <map>
#include <vector>

#include "Lohengrin/dbtables.h"
#include "BM/ClassSupport.h"

HRESULT const E_NO_ITEM = S_FALSE;

class PgItemBag
{
public:

#pragma pack(1)
	typedef struct tagBagElement
	{
		tagBagElement(int const _iItemNo, short const _nCount, short const _nRate, bool const _bIsLevelBalance )
			:	iItemNo(_iItemNo)
			,	nCount(_nCount)
			,	nRate(_nRate)
			,	bIsLevelBalance(_bIsLevelBalance)
		{}

		bool IsEmpty()const
		{
			if(!nCount
			&& !nRate)
			{//확률과 갯수가 없으면 무시. 이건 빈것.
				return true;
			}
			return false;
		}

		int GetItemNo( int const iLevel )const;

		int		iItemNo;
		short	nCount;
		short	nRate;
		bool	bIsLevelBalance;// 레벨 벨런스(iItemNo가 아이템번호가 아니고 ItemByLevel테이블의 인덱스 이다)
	}SBagElement;
#pragma pack()

	typedef std::vector< SBagElement > BagElementCont;

public:
	PgItemBag();
	~PgItemBag();

public:
	HRESULT PopItem( int const iLevel, int &rOutResultItemNo, int &rOutCount );
	HRESULT PopItem( int const iLevel, int &rOutResultItemNo );
	HRESULT PopItemToIndex( int const iLevel, int &rOutResultItemNo, size_t& rOutResultItemCount, int &iIndex );
	HRESULT PopMoney(int &rOutResultMoney, int const iAddDropRate = 0);
	
	bool Build(	CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const &rkItemBag, 
					CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkContRaise, 
					CONT_DEF_COUNT_CONTROL::mapped_type const &rkContCount,
					CONT_DEF_DROP_MONEY_CONTROL::mapped_type const &rkMoneyDrop);
	
	BagElementCont const & GetElements()const{return m_kContBag;}
	size_t GetElementsCount()const{return m_kContBag.size();}

	void AddDropRate(int const iAddDropRate);

protected:
	void AccTotalRaiseValue();

protected:
	BagElementCont m_kContBag;	// MaxCount, RaiseRate, ItemNo 가 병합된 최종 데이터가 해당 해쉬에 세팅되어진다.
	
 	CLASS_DECLARATION_S(int, TotalRaiseRate);

	CLASS_DECLARATION_S(int, MoneyMin);
	CLASS_DECLARATION_S(int, MoneyRand);
	CLASS_DECLARATION_S(int, MoneyRate);
};

class PgItemBagByLevel
	:	public PgItemBag
{
public:
	PgItemBagByLevel();
	explicit PgItemBagByLevel( short const nLevel );
	~PgItemBagByLevel();

	bool operator < ( PgItemBagByLevel const & rhs )const;
	bool operator > ( PgItemBagByLevel const & rhs )const;
	bool operator <= ( PgItemBagByLevel const & rhs )const;
	bool operator >= ( PgItemBagByLevel const & rhs )const;
	bool operator == ( PgItemBagByLevel const & rhs )const;

	short GetLevel()const{return m_nLevel;}

private:
	short		m_nLevel;
};


class PgItemContainer
{//미션 결과용.
public:
	PgItemContainer();
	~PgItemContainer();
	
public:
	void Clear();
	bool Build(TBL_DEF_ITEM_CONTAINER const &rkTbl);

	HRESULT PopItem(int &rOutResultItemNo);
	HRESULT PopMoney(int &rOutResultMoney);
	HRESULT PopBag(PgItemBag& _rkItemBag)const;

	TBL_DEF_ITEM_CONTAINER m_kTbl;
};

#endif // WEAPON_VARIANT_ITEM_ITEMBAG_H