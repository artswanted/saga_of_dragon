#include <stdafx.h>
#include "..\global.h"
#include "EnemySelector.h"

NiMatrix3 GetMakeZRotation(float const radian)
{
	NiMatrix3 kRot;
	kRot.MakeZero();
	kRot.MakeZRotation(radian);
	return kRot;
}

const NiMatrix3 kRotPlus90 = GetMakeZRotation(1.571f);
const NiMatrix3 kRotMinus90 = GetMakeZRotation(-1.571f);

POINT3 GetUnitShift(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, int const iUnitShift)
{
	if(iUnitShift!=0)
	{
		POINT3 kShiftPos = rkCasterPos;
		NiPoint3 kDir((0<iUnitShift ? kRotPlus90 : kRotMinus90) * NiPoint3(rkVisDir.x,rkVisDir.y,0.0f));
		kDir.Unitize();
		kDir *= abs(static_cast<float>(iUnitShift));
		kShiftPos += POINT3(kDir.x,kDir.y,kDir.z);
		return kShiftPos;
	}
	return rkCasterPos;
}

bool PgSkillAreaChooser::InArea(CUnit * pkUnit, ESkillArea eType, POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos,
								int const iParam1, int const iParam2, int const iParam3, int const iParam4, int const iMinRange)
{
	bool bResult = false;

	if( NULL==pkUnit )
	{
		return 	bResult;
	}

	switch(eType)
	{
	case ESArea_Sphere:
		{
			int iAddRange = 0;
			if( pkUnit->GetAbil(AT_GROWTH_SKILL_RANGE) )
			{
				iAddRange = pkUnit->GetAbil(AT_UNIT_SIZE_XY);
			}
			bResult = InAreaSphere(rkCasterPos, rkVisDir, rkTargetPos, iParam1+iAddRange, iParam2);
		}break;
	case ESArea_Cube:
		{
			int iAddRange = 0;
			if( pkUnit->GetAbil(AT_GROWTH_SKILL_RANGE) )
			{
				iAddRange = pkUnit->GetAbil(AT_UNIT_SIZE_XY);
			}
			bResult = InAreaCube(rkCasterPos, rkVisDir, rkTargetPos, iParam1+iAddRange, iParam2+iAddRange);
		}break;
	case ESArea_Cone:
		{
			bResult = InAreaCone(rkCasterPos, rkVisDir, rkTargetPos, iParam1, iParam2);
		}break;
	case ESArea_Front_Sphere:
		{
			bResult = InAreaFrontSphere(rkCasterPos, rkVisDir, rkTargetPos, iParam1, iParam2, iParam3);
		}break;
	case ESArea_Unit_Cube:
		{
			bResult = InAreaUnitCube(rkCasterPos, rkVisDir, rkTargetPos, iParam1, iParam2);
		}break;
	case ESArea_Front_DoubleSphere:
		{
			bResult = InAreaFrontDoubleSphere(rkCasterPos, rkVisDir, rkTargetPos, iParam1, iParam2, iParam3, iParam4);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Not defined SkillAreaType ["<<eType<<L"]");
			bResult = false;
		}break;
	}
	if (bResult && 0<iMinRange && iMinRange > POINT3::Distance(rkCasterPos, rkTargetPos))	//안선택됬으면 말고, 선택됬고 최소거리 있으면 거리체크
	{
		bResult = false;	
	}
	if( !bResult )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return bResult;
}

bool PgSkillAreaChooser::InAreaSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iRange, int const /*iNothing*/)
{
	float fDistanceQ = GetDistanceQ(rkCasterPos, rkTargetPos);
	if (fDistanceQ > (float)iRange * iRange)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgSkillAreaChooser::InAreaCube(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iWidth)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] Not implemented"), __FUNCTIONW__);
	//INFO_LOG(BM::LOG_LV8, _T("[%s] Target Pos : %.4f, %.4f, %.4f"), __FUNCTIONW__, rkTargetPos.x, rkTargetPos.y, rkTargetPos.z);
	//return InAreaSphere(rkCasterPos, rkVisDir, rkTargetPos, iLength, iWidth);
	NxVec3 kTargetVec(rkTargetPos.x-rkCasterPos.x, rkTargetPos.y-rkCasterPos.y, rkTargetPos.z-rkCasterPos.z);
	if(false==kTargetVec.isZero())	//같은 좌표면 포함
	{
		float fDist = rkVisDir.dot(kTargetVec);
		if (fDist <= 0 || fDist > (float)iLength)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		NxVec3 kVisDirNoZ(rkVisDir.x, rkVisDir.y, 0);
		kTargetVec.z = 0;
		NxVec3 kDirVec = kVisDirNoZ * fDist;
		if (kDirVec.distance(kTargetVec) > (float)iWidth)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	return __max(iWidth, AI_Z_LIMIT) > abs(rkCasterPos.z - rkTargetPos.z);
}

bool PgSkillAreaChooser::InAreaCone(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iDegree)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] Not implemented"), __FUNCTIONW__);
	return InAreaSphere(rkCasterPos, rkVisDir, rkTargetPos, iLength, iDegree);
}

bool PgSkillAreaChooser::InAreaFrontSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iFront, int const iRange, int const iUnitShift)
{
	POINT3 const kShiftCasterPos = GetUnitShift(rkCasterPos, rkVisDir, iUnitShift);

	POINT3 kCasterPos(rkVisDir.x, rkVisDir.y, rkVisDir.z);
	kCasterPos*=static_cast<float>(iFront);
	return InAreaSphere(kShiftCasterPos+kCasterPos, rkVisDir, rkTargetPos, iRange, 0);
}

bool PgSkillAreaChooser::InAreaUnitCube(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iLength, int const iWidth)
{
	NxVec3 const kTargetVec(rkTargetPos.x-rkCasterPos.x, rkTargetPos.y-rkCasterPos.y, rkTargetPos.z-rkCasterPos.z);
	if(abs(kTargetVec.x) > iLength)	{ return false; }
	if(abs(kTargetVec.y) > iWidth)	{ return false; }
	if(abs(kTargetVec.z) > iWidth)	{ return false; }
	return true;
}

bool PgSkillAreaChooser::InAreaFrontDoubleSphere(POINT3 const& rkCasterPos, NxVec3 const& rkVisDir, POINT3 const& rkTargetPos, int const iRange, int const iFront, int const iShift, int const iUnitShift)
{
	POINT3 const kShiftCasterPos = GetUnitShift(rkCasterPos, rkVisDir, iUnitShift);

	NiPoint3 kRightDir(kRotPlus90 * NiPoint3(rkVisDir.x,rkVisDir.y,0.0f));
	kRightDir.Unitize();
	kRightDir *= static_cast<float>(iShift);

	NiPoint3 kLeftDir(kRotMinus90 * NiPoint3(rkVisDir.x,rkVisDir.y,0.0f));
	kLeftDir.Unitize();
	kLeftDir *= static_cast<float>(iShift);

	bool bResult = false;
	bResult |= InAreaFrontSphere(kShiftCasterPos+POINT3(kRightDir.x,kRightDir.y,kRightDir.z), rkVisDir, rkTargetPos, iFront, iRange);
	bResult |= InAreaFrontSphere(kShiftCasterPos+POINT3(kLeftDir.x,kLeftDir.y,kLeftDir.z), rkVisDir, rkTargetPos, iFront, iRange);
	return bResult;
}

/*
TE_CasterCircle(POINT3 const& rkPos, int iRange)
	:m_ptPos(rkPos), m_iRange(iRange)
{
}

bool TE_CasterCircle::VInArea(POINT3 const& rkPos)
{
	float fDistQ = DistanceQ(rkPos, m_ptPos);
	if (fDistQ > m_iRange*m_iRange)
	{
		return false;
	}
	return false;
}

TE_CasterCube(POINT3 const& rkPos, int iRange)
	:m_ptPos(rkPos), m_iRange(iRange)
{
}

bool TE_CasterCube::VInArea(POINT3 const& rkPos)
{
	INFO_LOG(BM::LOG_LV5, _T("[%s] Not implemented SkillArea type"), __FUNCTIONW__);
	float fDistQ = DistanceQ(rkPos, m_ptPos);
	if (fDistQ > m_iRange*m_iRange)
	{
		return false;
	}
	return false;
}

TE_CasterCone(POINT3 const& rkPos, int iRange)
	:m_ptPos(rkPos), m_iRange(iRange)
{
}

bool TE_CasterCone::VInArea(POINT3 const& rkPos)
{
	INFO_LOG(BM::LOG_LV5, _T("[%s] Not implemented SkillArea type"), __FUNCTIONW__);
	float fDistQ = DistanceQ(rkPos, m_ptPos);
	if (fDistQ > m_iRange*m_iRange)
	{
		return false;
	}
	return false;
}
*/