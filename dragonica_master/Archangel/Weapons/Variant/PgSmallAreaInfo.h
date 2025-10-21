#ifndef WEAPON_VARIANT_BASIC_GROUND_PGSMALLAREAINFO_H
#define WEAPON_VARIANT_BASIC_GROUND_PGSMALLAREAINFO_H

#include "Lohengrin/dbtables.h"

typedef enum
{
	SaDir_Begin				= 0,

	SaDir_LEFT_FRONT_DOWN	= 0,
	SaDir_LEFT_FRONT_		= 1,
	SaDir_LEFT_FRONT_UP		= 2,
	SaDir_LEFT__DOWN		= 3,
	SaDir_LEFT__			= 4,
	SaDir_LEFT__UP			= 5,
	SaDir_LEFT_BACK_DOWN	= 6,
	SaDir_LEFT_BACK_		= 7,
	SaDir_LEFT_BACK_UP		= 8,

	SaDir__FRONT_DOWN		= 9,
	SaDir__FRONT_			= 10,
	SaDir__FRONT_UP			= 11,
	SaDir___DOWN			= 12,
	SaDir___				= 13,
	SaDir___UP				= 14,
	SaDir__BACK_DOWN		= 15,
	SaDir__BACK_			= 16,
	SaDir__BACK_UP			= 17,

	SaDir_RIGHT_FRONT_DOWN	= 18,
	SaDir_RIGHT_FRONT_		= 19,
	SaDir_RIGHT_FRONT_UP	= 20,
	SaDir_RIGHT__DOWN		= 21,
	SaDir_RIGHT__			= 22,
	SaDir_RIGHT__UP			= 23,
	SaDir_RIGHT_BACK_DOWN	= 24,
	SaDir_RIGHT_BACK_		= 25,
	SaDir_RIGHT_BACK_UP		= 26,

	SaDir_Max				= 27
} ESmallArea_Direction;

#define GET_REVERSE_AREA_DIRECTION(eDir) (SaDir_Max-1-eDir)
//#define GET_AREA_DIR_DIFF(eDir) POINT3I(int(eDir/9)-1, int((eDir%9)/3)-1, int(eDir%3)-1)

class PgSmallAreaInfo
{
public:
	static int const ERROR_INDEX = -1;

public:
	PgSmallAreaInfo();
	~PgSmallAreaInfo();

	HRESULT Init( int const iMapNo, CONT_DEFMAP const &rkContDefMap, POINT3 const &pt3Min, POINT3 const &pt3Max );

	void WriteToPacket( BM::Stream &rkPacket )const;
	void ReadFromPacket( BM::Stream &rkPacket );

	POINT3I const &GetCoordMin()const{return m_pt3Min;}
	POINT3I const &GetCoordMax()const{return m_pt3Max;}
	POINT3I const &GetAreaSize()const{return m_pt3AreaSize;}
	POINT3I const &GetTotalCount()const{return m_pt3Count;}

	int const GetAreaIndexFromIndex3( POINT3I const &pt3Index )const;
	int const GetAreaIndexFromPos( POINT3 const &rkPos )const;
	
	int GetRelativeIndexFromIndex( int const iIndex, ESmallArea_Direction const eDir )const;
	int GetRelativeIndexFromIndex3( POINT3I const &pt3Index, ESmallArea_Direction const eDir )const;
	void GetRelativeIndex3FromIndex3( POINT3I const &pt3Index, ESmallArea_Direction const eDir, POINT3I &rkOutIndex3 )const;

	bool GetAreaIndex3( int const iIndex, POINT3I &rkOutIndex )const;
	bool GetAreaMinPos( int const iIndex, POINT3 &rkOutPos )const;
	bool GetAreaMaxPos( int const iIndex, POINT3 &rkOutPos )const;
	
	int const GetTotalSize()const{return m_pt3Count.x * m_pt3Count.y * m_pt3Count.z;}

protected:
	POINT3I	m_pt3Min;		// Map Minimum Coordinates
	POINT3I	m_pt3Max;		// Map Maximum Coordinates
	POINT3I	m_pt3AreaSize;	// SmallArea Size 
	POINT3I	m_pt3Count;		// SmallArea Total Count
};

inline int const PgSmallAreaInfo::GetAreaIndexFromIndex3( POINT3I const &pt3Index )const
{
	if (	pt3Index.x < 0 || pt3Index.x >= m_pt3Count.x
		||	pt3Index.y < 0 || pt3Index.y >= m_pt3Count.y
		||	pt3Index.z < 0 || pt3Index.z >= m_pt3Count.z
		)
	{
		return ERROR_INDEX;
	}

	int const iIndex = pt3Index.x * m_pt3Count.y * m_pt3Count.z + pt3Index.y * m_pt3Count.z + pt3Index.z;
	return iIndex;
}

inline int const PgSmallAreaInfo::GetAreaIndexFromPos( POINT3 const &rkPos )const
{
	if(	!m_pt3AreaSize.x
	||	!m_pt3AreaSize.y
	||	!m_pt3AreaSize.z)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Value Is 0") );
		return 0;
	}

	POINT3I pt3Index(	int(rkPos.x - m_pt3Min.x) / m_pt3AreaSize.x
					,	int(rkPos.y - m_pt3Min.y) / m_pt3AreaSize.y
					,	int(rkPos.z - m_pt3Min.z) / m_pt3AreaSize.z
					);

	return GetAreaIndexFromIndex3( pt3Index );
}

inline void PgSmallAreaInfo::GetRelativeIndex3FromIndex3( POINT3I const &pt3Index, ESmallArea_Direction const eDir, POINT3I &rkOutIndex3 )const
{
	rkOutIndex3.x = int(eDir / 9) - 1;
	rkOutIndex3.y = int((eDir % 9) / 3) -1;
	rkOutIndex3.z = int(eDir % 3) -1;
	rkOutIndex3 += pt3Index;
}

#endif // WEAPON_VARIANT_BASIC_GROUND_PGSMALLAREAINFO_H