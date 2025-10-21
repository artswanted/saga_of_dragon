#ifndef MAP_MAPSERVER_ACTION_AI_ENEMYSELECTOR_H
#define MAP_MAPSERVER_ACTION_AI_ENEMYSELECTOR_H

class PgSkillAreaChooser
{
public:
	PgSkillAreaChooser() {};
	~PgSkillAreaChooser() {};

	static bool InArea(CUnit * pkUnit, ESkillArea eType, POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iParam1, int const iParam2, int const iParam3 = 0, int const iParam4 = 0, int const iMinRange = 0);
protected:
	static bool InAreaSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iRange, int const iNothing);
	static bool InAreaCube(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iWidth);
	static bool InAreaCone(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iDegree);
	static bool InAreaFrontSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iFront, int const iRange, int const iUnitShift=0);
	static bool InAreaUnitCube(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iWidth);
	static bool InAreaFrontDoubleSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iRange, int const iFront, int const iShift, int const iUnitShift);
};

/*
template< typename T >
class TESelector
{
public:
	TESelector()
	{}
	virtual ~TESelector(){}
public:
	virtual bool VInArea(T& rkT) = 0;
};

class TE_CasterCircle
	: public TESelector< POINT3 >
{
public:
	TE_CasterCircle(POINT3 const& rkPos, int iRange );

	virtual bool VInArea(POINT3 const& rkPos);

private:
	POINT3 const m_ptPos;
	int const m_iRange;
};

class TE_CasterCube
	: public TESelector< POINT3 >
{
public:
	TE_CasterCube(POINT3 const& rkPos, int iRange );

	virtual bool VInArea(POINT3 const& rkPos);

private:
	POINT3 const m_ptPos;
	int const m_iRange;
};

class TE_CasterCone
	: public TESelector< POINT3 >
{
public:
	TE_CasterCone(POINT3 const& rkPos, int iRange );

	virtual bool VInArea(POINT3 const& rkPos);

private:
	POINT3 const m_ptPos;
	int const m_iRange;
};
*/

#endif // MAP_MAPSERVER_ACTION_AI_ENEMYSELECTOR_H