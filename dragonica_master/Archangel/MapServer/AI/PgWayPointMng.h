#ifndef MAP_MAPSERVER_ACTION_AI_PGWAYPOINTMNG_H
#define MAP_MAPSERVER_ACTION_AI_PGWAYPOINTMNG_H

#include "PgWayPoint.h"

typedef struct tagWayPointKey
{
	short int sGroup;
	short int sIndex;
	
	tagWayPointKey()
	{
		Set(0,0);
	}

	void operator = (const tagWayPointKey& rhs)
	{
		sGroup = rhs.sGroup;
		sIndex = rhs.sIndex;
	}

	tagWayPointKey(short int const sGroupNum, short int const sIndexNum)
	{
		Set(sGroupNum, sIndexNum);
	}

	void Set(short int const sGroupNum, short const sIndexNum)
	{
		sGroup = sGroupNum;
		sIndex = sIndexNum;
	}

	bool operator < (const tagWayPointKey &rhs)const
	{
		if( sGroup < rhs.sGroup )	{return true;}
		if( sGroup > rhs.sGroup)	{return false;}

		if( sIndex < rhs.sIndex )	{return true;}
		if( sIndex > rhs.sIndex )	{return false;}

		return false;
	}
/*	
	operator size_t ()const
	{
		return MAKELONG(sGroup, sIndex);
	}
*/
}WayPointKey;

class PgGround;

class PgWayPointMng
{
public:
	PgWayPointMng(void);
	~PgWayPointMng(void);
	
	bool Init(PgGround const& rkGround);
	bool LoadWayPointFile(PgGround const& rkGround, char const* pchFilePath);
	
	bool FindWayPos(CUnit* pkUnit, PgGround* pkGround);
    bool FindRoadPos(CUnit* pkUnit, DWORD const dwElapsedTime);
	//bool FindWayPos(CUnit* pkUnit, POINT3 const& rkToPos);
	bool GetRunawayPos(CUnit* pkUnit, POINT3 const& rkEnemyPos);

	void Release();

protected:
	const PgWayPoint2* GetWayPoint(const WayPointKey& rkKey);
	bool FindNearWayPoint(POINT3 const& pt3Pos, WayPointKey& kKey, float const fZLimit = 50.0f);
	bool FindBeforeWayPointRandomPos(WayPointKey& kKey, POINT3& pt3NextPos);
	bool FindNextWayPointRandomPos(WayPointKey& rkKey, POINT3& pt3NextPos);
    bool FindNextRoadPointRandomPos(WayPointKey& rkKey, POINT3& pt3NextPos);
    bool FindRoadPointRandomPos(const WayPointKey& rkKey, POINT3& pt3Pos);
	bool FindWayPointRandomPos(const WayPointKey& rkKey, POINT3& rkPos);
	void GetMidwayPoint(POINT3 const& rkPos1, POINT3 const& rkPos2, POINT3& rkOut);
	const PgWayPoint2* GetNextWayPoint(const WayPointKey& rkKey);
	const PgWayPoint2* GetBeforeWayPoint(const WayPointKey& rkKey);

private:
	typedef std::map< WayPointKey, PgWayPoint2* > WAYPOINT_CONT;
	//MAP_WAYPOINT2 m_kWayPoint;
	WAYPOINT_CONT m_kWayPointCont;
	SGroundKey m_kGroundKey;	// 특별히 쓸일은 없는데, Log를 찍을 때 필요해서 넣었음.
	typedef BM::TObjectPool< PgWayPoint2 > WayPointPool;
	static WayPointPool m_kWayPointPool;
};

#endif // MAP_MAPSERVER_ACTION_AI_PGWAYPOINTMNG_H