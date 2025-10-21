#ifndef MAP_MAPSERVER_MAP_PGSMALLAREA_H
#define MAP_MAPSERVER_MAP_PGSMALLAREA_H

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/Observer.h"
#include "Variant/PgSmallAreaInfo.h"

class CUnit;
class PgPlayer;

/*
template< typename T >
class TVisitor
{
public:
	TVisitor()
	{}
	virtual ~TVisitor(){}
public:
	virtual bool VIsCorrect(T& rkT) = 0;
};

class CCheckUnit_IgnoreOne
	: public TVisitor< CUnit* >
{
public:
	CCheckUnit_IgnoreOne(BM::GUID const &rkGuid)
		:m_kGuid(rkGuid)
	{
	}

	virtual bool VIsCorrect(CUnit* &pkUnit)
	{
		if(pkUnit->GetID() == m_kGuid)
		{
			return false;
		}
		return true;
	}
	
	const BM::GUID m_kGuid;
};
*/

class PgSmallArea
//	:	public BM::CObserver< BM::Stream* >
{//SmallArea 의 유닛들은 Ground가 가진 애들의 참조역할일 뿐.
public:
	static int const NONE_AREA_INDEX = PgSmallAreaInfo::ERROR_INDEX;
	static int const BIG_AREA_INDEX = - 2;

	PgSmallArea();
	virtual ~PgSmallArea(){};

public:

public:
	bool Init(POINT3 const &pt3MinPos, POINT3 const &pt3Size);
	typedef std::vector<PgSmallArea*> ContSmallArea;

	HRESULT AddUnit(CUnit* pkUnit);
	CUnit* GetUnit(BM::GUID const &rkGuid);
	void RemoveUnit(BM::GUID const &rkGuid/*, bool const bSend = false*/);

	void Broadcast(BM::Stream *pkPacket,  BM::GUID const& rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag ) const;
	void Broadcast_Adjacent(BM::Stream *pkPacket,  BM::GUID const& rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag ) const;

	void GetUnitList(UNIT_PTR_ARRAY& rkArray, CUnit const *pkIgnore, EUnitType eType, BYTE const bySyncTypeCheck );
	void GetUnitList( UNIT_PTR_ARRAY& rkArray, BM::GUID const &kIgNoreGuid, EUnitType eType, BYTE const bySyncTypeCheck );
	
	bool IsAdjacentArea(PgSmallArea const* pkArea) const;
	bool IsAdjacentArea(POINT3 const ptPos) const;

	void DisplayState();
	void Clear();

	void SetAdjacentArea(ESmallArea_Direction eDir, PgSmallArea *pkSmallArea);
	PgSmallArea* GetAdjacentArea(ESmallArea_Direction eDir) const;

	bool IsBigArea()const{return Index() == BIG_AREA_INDEX; }

protected://스몰은 오브젝트 매니저를 가질 필요 없고.. 
	void Distance(POINT3 const ptSource, POINT3& rptDistance) const;
	void Distance(PgSmallArea const* pkArea, POINT3& rptDistance) const;
	void GetCenter(POINT3& rkCenter) const;

	typedef std::map< BM::GUID, CUnit* > UnitCont;
	UnitCont m_kUnitCont;
	ContSmallArea m_kAdjacentArea;

	CLASS_DECLARATION_S(POINT3, MaxPos);
	CLASS_DECLARATION_S(POINT3, MinPos);
	CLASS_DECLARATION_S(POINT3, Size);
	CLASS_DECLARATION_S(int, Index);
};

#endif // MAP_MAPSERVER_MAP_PGSMALLAREA_H