#ifndef MAP_MAPSERVER_ACTION_AI_PGWAYPOINT_H
#define MAP_MAPSERVER_ACTION_AI_PGWAYPOINT_H

class PgWayPoint2
{
public:
	PgWayPoint2()
	{
		Group(0);
		Index(0);
		Radius(0);
	}

	~PgWayPoint2(){}

	void Set(const PgWayPoint2* pkWayPoint);
	void GetRandomPos(POINT3& rkPos) const;
    void GetRandomPos2(POINT3& rkPos) const;
	void GetOppositePos(POINT3 const& rkPos, POINT3& rkOut) const;
    bool IsInPos(POINT3 const & pt3Pos)const;

protected:
	CLASS_DECLARATION_S(short int, Group);
	CLASS_DECLARATION_S(short int, Index);
	CLASS_DECLARATION_S(int, Radius);//WayPoint가 가진 폭(y축으로 랜덤이동 및 낙하 방지)
	CLASS_DECLARATION_S(POINT3, Point);
};

//typedef std::map< int, PgWayPoint2* > WAYPOINT_CONT;

//typedef struct
//{
//	int iIndexCount ;
//	WAYPOINT_CONT kWayPointIndex ;
//}MAP_WAYPOINT_GROUP2;

//typedef std::map< int, WAYPOINT_CONT > WAYPOINT_GROUP_CONT;

//typedef struct
//{
//	int iGroupCount;
//	WAYPOINT_GROUP_CONT kWayPointGroup ;
//}MAP_WAYPOINT2;

#endif // MAP_MAPSERVER_ACTION_AI_PGWAYPOINT_H