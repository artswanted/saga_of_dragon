#ifndef WEAPON_VARIANT_UNIT_PGOBJECTUNIT_H
#define WEAPON_VARIANT_UNIT_PGOBJECTUNIT_H

#include "Unit.h"

#pragma pack(1)
typedef struct tagObjUnitGroupIndex
{
	tagObjUnitGroupIndex(BM::GUID const &_kGroupID=BM::GUID::NullData(), int const _iIndex=0)
		:	kGroupID(_kGroupID)
		,	iIndex(_iIndex)
	{}

	void Clear()
	{
		kGroupID.Clear();
		iIndex = 0;
	}

	void Set(const tagObjUnitGroupIndex& rhs)
	{
		kGroupID = rhs.kGroupID;
		iIndex = rhs.iIndex;
	}

	bool operator<(const tagObjUnitGroupIndex& rhs)const
	{
		if ( kGroupID == rhs.kGroupID )
		{
			return iIndex < rhs.iIndex;
		}
		return kGroupID < rhs.kGroupID;
	}
	bool operator>(const tagObjUnitGroupIndex& rhs)const
	{
		return !((*this)<rhs);
	}
	bool operator==(const tagObjUnitGroupIndex& rhs)const
	{
		return (kGroupID==rhs.kGroupID) && (iIndex==rhs.iIndex);
	}
	bool operator!=(const tagObjUnitGroupIndex& rhs)const
	{
		return !((*this)==rhs);
	}

	BM::GUID	kGroupID;
	int			iIndex;		//높이에 따른 인덱스
}SObjUnitGroupIndex;

typedef struct tagObjUnitBaseInfo
{
	tagObjUnitBaseInfo( SObjUnitGroupIndex const& _kGroup, int const _iID=0 )
		:	iID(_iID)
		,	kGroup(_kGroup)
	{}

	tagObjUnitBaseInfo( tagObjUnitBaseInfo const &kInfo )
		:	iID(kInfo.iID)
		,	kGroup(kInfo.kGroup)
		,	pt3Pos(kInfo.pt3Pos)
	{}

	int					iID;
	SObjUnitGroupIndex	kGroup;
	POINT3				pt3Pos;
}SObjUnitBaseInfo;

typedef struct tagObjUnitCreateInfo
:	public tagObjUnitBaseInfo
{
	tagObjUnitCreateInfo( tagObjUnitBaseInfo const &kInfo )
		:	tagObjUnitBaseInfo(kInfo)
	{
		kGuid.Generate();
	}
	BM::GUID			kGuid;
//	std::wstring		wstrName;
	DWORD				dwAttribute;	// EObjUnit_Attr value
}SObjUnitCreateInfo;

typedef enum
{
	EObj_Attr_None			= 0x0000,
	EObj_Attr_Breakable		= 0x0001,	// 파괴오브젝트
	EObj_Attr_CanRide		= 0x0002,	// 
	EObj_Attr_UnBreakable	= 0x0004,	// 깨지지않는 오브젝트
} EObjUnit_Attr;

#pragma pack()

class PgObjectUnit
	:	public CUnit
{
public:
	PgObjectUnit(void);
	virtual ~PgObjectUnit(void);

	virtual void Init();
	virtual HRESULT Create(const void* pkInfo);

	virtual EUnitType UnitType()const{ return UT_OBJECT; }
	virtual int GetAbil(WORD const Type) const;
	virtual __int64 GetAbil64(WORD const eAT)const;
	virtual bool SetAbil(WORD const Type, int const iValue, bool const bIsSend = false, bool const bBroadcast = false);

	virtual bool OnDamage(int iPower,int& iHP);

	virtual int CallAction(WORD wActCode, SActArg *pActArg)	{return 0;}
	virtual bool IsCheckZoneTime( DWORD dwElapsed ){return true;}

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

	bool IsAttribute(DWORD const dwAttr);

	CLASS_DECLARATION_NO_SET(SObjUnitGroupIndex,m_kGroupIndex,GetGroupIndex);
	CLASS_DECLARATION_NO_SET(float,m_fHeight,GetHeight);
	CLASS_DECLARATION_S(DWORD, Attributes);

protected:
	bool SetBasicAbil();
};

#endif // WEAPON_VARIANT_UNIT_PGOBJECTUNIT_H