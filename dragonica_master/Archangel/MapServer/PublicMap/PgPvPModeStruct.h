#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPMODESTRUCT_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPMODESTRUCT_H

typedef struct tagPvPHillNode
{
	tagPvPHillNode( size_t const _iNo, NiAVObject const* pkObject )
		:	iNo(_iNo)
	{
		NiBound const &kBound = pkObject->GetWorldBound();
		POINT3 pt3CenterPos( kBound.GetCenter().x, kBound.GetCenter().y ,kBound.GetCenter().z );
		pt3MinPos = pt3MaxPos = pt3CenterPos;
		pt3MinPos -= kBound.GetRadius();
		pt3MaxPos += kBound.GetRadius();
	}

	tagPvPHillNode( tagPvPHillNode const &rhs )
		:	iNo(rhs.iNo)
		,	pt3MaxPos(rhs.pt3MaxPos)
		,	pt3MinPos(rhs.pt3MinPos)
	{}

	bool InPos( POINT3 const &kPos )const
	{
		if ( (kPos > pt3MinPos) && (kPos < pt3MaxPos) )
		{
			return true;
		}
		return false;
	}

	size_t iNo;
	POINT3 pt3MinPos;
	POINT3 pt3MaxPos;
}SPvPHillNode;
typedef std::vector<SPvPHillNode>	CONT_PVPHILL;

typedef struct tagPvPHillItem
{
	tagPvPHillItem( CUnit *pkUnit )
		:	kCharGuid(pkUnit->GetID())
		,	iTeam(pkUnit->GetAbil(AT_TEAM))
	{}

	bool operator<( CUnit const  *pkUnit )const
	{
		return kCharGuid < pkUnit->GetID();
	}
	bool operator>( CUnit const *pkUnit )const
	{
		return kCharGuid > pkUnit->GetID();
	}
	bool operator==( CUnit const *pkUnit )const
	{
		return kCharGuid == pkUnit->GetID();
	}
	BM::GUID	kCharGuid;
	int	const	iTeam;
}SPvPHillItem;
typedef std::list<SPvPHillItem>		CONT_PVPHILL_ITEM;


#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPMODESTRUCT_H