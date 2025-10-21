#ifndef WEAPON_LOHENGRIN_DB_DBTABLES3_H
#define WEAPON_LOHENGRIN_DB_DBTABLES3_H

#include "packetstruct.h"

#pragma pack (1)// pack -->

typedef struct tagTBL_DEF_TRANSTOWER_TARGET_KEY
{
	tagTBL_DEF_TRANSTOWER_TARGET_KEY()
		:	iGroundNo(0)
		,	nTargetSpawn(0)
	{}

	bool operator < ( tagTBL_DEF_TRANSTOWER_TARGET_KEY const &rhs )const
	{
		if ( iGroundNo == rhs.iGroundNo )
		{
			return nTargetSpawn < rhs.nTargetSpawn;
		}
		return iGroundNo < rhs.iGroundNo;
	}

	bool operator > ( tagTBL_DEF_TRANSTOWER_TARGET_KEY const & rhs )const
	{
		return rhs < *this;
	}

	bool operator == ( tagTBL_DEF_TRANSTOWER_TARGET_KEY const &rhs )const
	{
		return	iGroundNo == rhs.iGroundNo
			&&	nTargetSpawn == rhs.nTargetSpawn;
	}

	int		iGroundNo;
	short	nTargetSpawn;

}TBL_DEF_TRANSTOWER_TARGET_KEY;

typedef struct tagTBL_DEF_TRANSTOWER_TARGET
:	public tagTBL_DEF_TRANSTOWER_TARGET_KEY
{
	tagTBL_DEF_TRANSTOWER_TARGET()
		:	iMemo(0)
		,	i64Price(0i64)
	{
	}

	explicit tagTBL_DEF_TRANSTOWER_TARGET( tagTBL_DEF_TRANSTOWER_TARGET_KEY const &rhs )
		:	tagTBL_DEF_TRANSTOWER_TARGET_KEY(rhs)
		,	iMemo(0)
		,	i64Price(0i64)
		,	nSort(0)
	{}

	bool operator < ( tagTBL_DEF_TRANSTOWER_TARGET const &rhs )const
	{
		return tagTBL_DEF_TRANSTOWER_TARGET_KEY::operator<( rhs );
	}

	bool operator > ( tagTBL_DEF_TRANSTOWER_TARGET const & rhs )const
	{
		return tagTBL_DEF_TRANSTOWER_TARGET_KEY::operator>( rhs );
	}

	bool operator == ( tagTBL_DEF_TRANSTOWER_TARGET const &rhs )const
	{
		return tagTBL_DEF_TRANSTOWER_TARGET_KEY::operator==( rhs );
	}

	__int64	i64Price;
	int		iMemo;
	short	nSort;
}TBL_DEF_TRANSTOWER_TARGET;

typedef struct tagTBL_DEF_PARTY_INFO_TARGET_KEY
{
	tagTBL_DEF_PARTY_INFO_TARGET_KEY()
		:	iAttribute(0)
		,	iSort(0)
	{}

	bool operator < ( tagTBL_DEF_PARTY_INFO_TARGET_KEY const &rhs )const
	{
		if ( iAttribute == rhs.iAttribute )
		{
			return iSort < rhs.iSort;
		}
		return iAttribute < rhs.iAttribute;
	}

	bool operator > ( tagTBL_DEF_PARTY_INFO_TARGET_KEY const & rhs )const
	{
		return rhs < *this;
	}

	bool operator == ( tagTBL_DEF_PARTY_INFO_TARGET_KEY const &rhs )const
	{
		return	iAttribute == rhs.iAttribute
			&&	iSort == rhs.iSort;
	}

	int		iAttribute;
	int		iSort;

}TBL_DEF_PARTY_INFO_TARGET_KEY;

typedef struct tagTBL_DEF_PARTY_INFO_TARGET
:	public tagTBL_DEF_PARTY_INFO_TARGET_KEY
{
	tagTBL_DEF_PARTY_INFO_TARGET()
		:	iArea_NameNo(0), iGroundNo(0)
	{
	}

	explicit tagTBL_DEF_PARTY_INFO_TARGET( tagTBL_DEF_PARTY_INFO_TARGET_KEY const &rhs )
		:	tagTBL_DEF_PARTY_INFO_TARGET_KEY(rhs)
		,	iArea_NameNo(0), iGroundNo(0)
	{}

	bool operator < ( tagTBL_DEF_PARTY_INFO_TARGET const &rhs )const
	{
		return tagTBL_DEF_PARTY_INFO_TARGET_KEY::operator<( rhs );
	}

	bool operator > ( tagTBL_DEF_PARTY_INFO_TARGET const & rhs )const
	{
		return tagTBL_DEF_PARTY_INFO_TARGET_KEY::operator>( rhs );
	}

	bool operator == ( tagTBL_DEF_PARTY_INFO_TARGET const &rhs )const
	{
		return tagTBL_DEF_PARTY_INFO_TARGET_KEY::operator==( rhs );
	}

	int		iArea_NameNo;
	int		iGroundNo;
}TBL_DEF_PARTY_INFO_TARGET;


typedef struct tagTBL_DEF_ALRAM_MISSION_ACTION
{
	tagTBL_DEF_ALRAM_MISSION_ACTION()
		:	iDiscriptionNo(0)
		,	iActionType(0)
		,	iObjectType(0)
		,	i64ObjectValue(0i64)
		,	iCount(0)
		,	byFailType(0)
	{}

	DEFAULT_TBL_PACKET_FUNC();

	int iDiscriptionNo;
	int iActionType;
	int iObjectType;
	__int64 i64ObjectValue;
	int iCount;
	BYTE byFailType;
}TBL_DEF_ALRAM_MISSION_ACTION;
typedef std::vector< TBL_DEF_ALRAM_MISSION_ACTION >	VEC_ALRAM_MISSION_ACTION;

int const MAX_ALRAM_MISSION_ITEMBAG = 2;
typedef struct tagTBL_DEF_ALRAM_MISSION
{
	tagTBL_DEF_ALRAM_MISSION()
		:	iAlramID(0)
		,	iAlramID_Next(0)
		,	nIsDraClass(0)
		,	i64ClassLimit(0i64)
		,	iType(0)
		,	iTitleNo(0)
		,	iTime(0)
		,	nPoint(0)
		,	iExp(0)
		,	iEffect(0)
	{
		::memset( iItemBag, 0, sizeof(iItemBag) );
	}

	tagTBL_DEF_ALRAM_MISSION( tagTBL_DEF_ALRAM_MISSION const &rhs )
	{
		this->operator =( rhs );
	}

	tagTBL_DEF_ALRAM_MISSION& operator = ( tagTBL_DEF_ALRAM_MISSION const &rhs )
	{
		iAlramID = rhs.iAlramID;
		iAlramID_Next = rhs.iAlramID_Next;
		nIsDraClass = rhs.nIsDraClass;
		i64ClassLimit = rhs.i64ClassLimit;
		iType = rhs.iType;
		iTitleNo = rhs.iTitleNo;
		iTime = rhs.iTime;
		nPoint = rhs.nPoint;
		iExp = rhs.iExp;
		iEffect = rhs.iEffect;
		::memcpy( iItemBag, rhs.iItemBag, sizeof(iItemBag) );

		{
			VEC_ALRAM_MISSION_ACTION vec;
			kActionList.swap(vec);
		}

		kActionList.reserve( rhs.kActionList.size() );
		VEC_ALRAM_MISSION_ACTION::const_iterator itr = rhs.kActionList.begin();
		for ( ; itr != rhs.kActionList.end() ; ++itr )
		{
			kActionList.push_back( *itr );
		}
		return *this;
	}

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( iAlramID );
		kPacket.Push( iAlramID_Next );
		kPacket.Push( nIsDraClass );
		kPacket.Push( i64ClassLimit );
		kPacket.Push( iType );
		kPacket.Push( iTitleNo );
		kPacket.Push( iTime );
		kPacket.Push( nPoint );
		kPacket.Push( iExp );
		kPacket.Push( iEffect );
		kPacket.Push( iItemBag, sizeof(iItemBag) );
		PU::TWriteArray_M( kPacket, kActionList );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		kPacket.Pop( iAlramID );
		kPacket.Pop( iAlramID_Next );
		kPacket.Pop( nIsDraClass );
		kPacket.Pop( i64ClassLimit );
		kPacket.Pop( iType );
		kPacket.Pop( iTitleNo );
		kPacket.Pop( iTime );
		kPacket.Pop( nPoint );
		kPacket.Pop( iExp );
		kPacket.Pop( iEffect );
		if ( true == kPacket.PopMemory( iItemBag, sizeof(iItemBag) ) )
		{
			kActionList.clear();
			PU::TLoadArray_M( kPacket, kActionList );
			return true;
		}
		return false;
	}

	size_t min_size()const
	{
		return	sizeof(int) * 9
			+	sizeof(i64ClassLimit)
			+	sizeof(nPoint)
			+	sizeof(nIsDraClass)
			+	sizeof(size_t);
	}

	int iAlramID;
	int iAlramID_Next;
	short nIsDraClass;
	__int64 i64ClassLimit;
	int iType;
	int iTitleNo;
	int iTime;//sec
	short nPoint;
	int iExp;
	int iEffect;
	int iItemBag[MAX_ALRAM_MISSION_ITEMBAG];
	VEC_ALRAM_MISSION_ACTION kActionList;

}TBL_DEF_ALRAM_MISSION;

struct SDefMapToMission
{
	SDefMapToMission() : iMapNo(0), iMissionNo(0)
	{
		::memset(iConnectedMapNo, 0, sizeof(iConnectedMapNo));
	}

	int	iMapNo;
	int	iMissionNo;
	int	iConnectedMapNo[5];
};

#pragma pack ()// <-- pack


typedef std::set< TBL_DEF_TRANSTOWER_TARGET >						CONT_DEF_TRANSTOWER_TARGET;
typedef std::map< BM::GUID, CONT_DEF_TRANSTOWER_TARGET >			CONT_DEF_TRANSTOWER;
typedef std::map< int, TBL_DEF_ALRAM_MISSION >						CONT_DEF_ALRAM_MISSION;

typedef std::set< TBL_DEF_PARTY_INFO_TARGET >						CONT_DEF_PARTY_INFO_TARGET;
typedef std::map< int, CONT_DEF_PARTY_INFO_TARGET >					CONT_DEF_PARTY_INFO;

typedef std::map<int, SDefMapToMission> CONT_DEFMAP_TOMISSION;

#endif // WEAPON_LOHENGRIN_DB_DBTABLES3_H