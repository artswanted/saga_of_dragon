#include "stdafx.h"
#include "PgWayPoint.h"

void PgWayPoint2::Set(const PgWayPoint2* pkWayPoint)
{
	Group(pkWayPoint->Group());
	Index(pkWayPoint->Index());
	Radius(pkWayPoint->Radius());
	Point(pkWayPoint->Point());
}

void PgWayPoint2::GetRandomPos(POINT3& rkPos) const
{
	rkPos.x = Point().x - Radius() + BM::Rand_Index(Radius()*2);
	rkPos.y = Point().y - Radius() + BM::Rand_Index(Radius()*2);
	rkPos.z = Point().z;
}

void PgWayPoint2::GetRandomPos2(POINT3& rkPos) const
{
    int iRandX = BM::Rand_Index(Radius());
    iRandX *= BM::Rand_Index(2) ? 1 : -1;
    int iRandY = BM::Rand_Index(Radius());
    iRandY *= BM::Rand_Index(2) ? 1 : -1;
	rkPos.x = Point().x + iRandX;
	rkPos.y = Point().y + iRandY;
	rkPos.z = Point().z;
}

void PgWayPoint2::GetOppositePos(POINT3 const& rkPos, POINT3& rkOut) const
{
	POINT3 ptCenter = Point();
	NxVec3 kDir(ptCenter.x-rkPos.x, ptCenter.y-rkPos.y, ptCenter.z-rkPos.z);
	kDir.normalize();
	kDir *= (NxReal)Radius();
	// OUTPUT
	rkOut.x = ptCenter.x + kDir.x;
	rkOut.y = ptCenter.y + kDir.y;
	rkOut.z = ptCenter.z + kDir.z;
}

bool PgWayPoint2::IsInPos(POINT3 const & pt3Pos)const
{
    POINT3 const ptMin = Point() - static_cast<float>(Radius());
	POINT3 const ptMax = Point() + static_cast<float>(Radius());

    return (pt3Pos <= ptMax) && (pt3Pos >= ptMin);
}