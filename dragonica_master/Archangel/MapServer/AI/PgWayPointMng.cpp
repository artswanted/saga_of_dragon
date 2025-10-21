#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "Global.h"
#include "PgWayPointMng.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

typedef enum eWayPointType
{
    EWPT_NONE = 0,
    EWPT_SEQUENCE_ONLY_ONE = 1, //순차적이동(한번)
    EWPT_SEQUENCE_INFINITE = 2, //순차적이동(무한)
    ////가-나-다-나-가-나-다..(무한)
}EWayPointType;

PgWayPointMng::WayPointPool PgWayPointMng::m_kWayPointPool(10, 5);

PgWayPointMng::PgWayPointMng(void)
{
}

PgWayPointMng::~PgWayPointMng(void)
{
	Release();
}

bool PgWayPointMng::Init(PgGround const& rkGround)
{
	m_kGroundKey = rkGround.GroundKey();
	{
		SGenGroupKey kkGenGrpKey;
		rkGround.GetGenGroupKey(kkGenGrpKey);
		char chWayPointFile[MAX_PATH];
		sprintf_s(chWayPointFile, MAX_PATH, "Script/Map/%d.way", kkGenGrpKey.iMapNo);
		LoadWayPointFile(rkGround, chWayPointFile);
	}
	return true;
}

bool PgWayPointMng::LoadWayPointFile(PgGround const& rkGround, char const* pchFilePath)
{
	//char FilePath[128] = "";
	//::sprintf_s(FilePath, 128, "Script/Map/%d.way", GroundKey().iGroundNo);

	/*
	// Sample file
		Total 14
		Group 1 11 3
		{
			1 70.000000 -3063.591064 39.330524 -55.703712
			2 70.000000 -2790.467041 36.730495 -56.403419
			3 70.000000 -2521.223633 29.667336 -56.402447
		}
	*/
	FILE * fp = NULL ;

	//if ( !(fp = fopen(FilePath,"r+")))
	if(fopen_s(&fp, pchFilePath, "r+") != 0)
	{
		//int const iGroundNo = rkGround.GroundKey().GroundNo();
		//if( 0 == (rkGround.GetAttr() & (GATTR_FLAG_VILLAGE | GATTR_FLAG_MYHOME)  ) )
		{
			//INFO_LOG(BM::LOG_LV5, __FL__ << _T("can't open WayPoint, [Ground:") << iGroundNo << _T(", File:") << UNI(pchFilePath) << _T("] but this ground is village"));
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("can't open WayPoint, [Ground:") << iGroundNo << _T(", File:") << UNI(pchFilePath) << _T("]"));
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 처음엔 그룹 갯수
	char buffer[64] = {0, };
	char buffer1[64] = {0, };
	char buffer2[64] = {0, };

	int totalgroup = 0 ;
	fscanf_s(fp,"%s %d",buffer, 64, &totalgroup);	// READ("Total 14")

	//m_kWayPoint.iGroupCount = totalgroup ;

	for ( int i = 0 ; i < totalgroup ; i ++ )
	{
		short int groupindex = 0 ;
		short int totalindex = 0 ;
		fscanf_s(fp, "%s %hd %s %hd", buffer, 64, &groupindex, buffer1, 64, &totalindex);	// READ("Group 1 11 3")

		//MAP_WAYPOINT_GROUP2 groupinfo;
		//groupinfo.iIndexCount = totalindex ;

		fscanf_s(fp, "%s", buffer, 64);	// READ("{")

		for ( int j = 0 ; j < totalindex ; j ++ )
		{
			PgWayPoint2* pkWay = m_kWayPointPool.New();

			short int index = 0 ;
			float radius  = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			fscanf_s(fp, "%hd %f %f %f %f", &index , &radius , &x ,&y ,&z );	// READ("1 70.000000 -3063.591064 39.330524 -55.703712")
			pkWay->Group(groupindex);
			pkWay->Index(index );
			pkWay->Radius((int)radius);
			pkWay->Point(POINT3(x,y,z));
			if( 0 >= radius )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"WayPoint Radius is <0 filename["<<UNI(pchFilePath)<<L"], Group["<<groupindex<<L"], Index["<<index<<L"]");
				m_kWayPointPool.Delete(pkWay);
				continue;
			}
			//const WayPointKey kWayPointKey(groupindex, index);
			auto ret = m_kWayPointCont.insert(std::make_pair(WayPointKey(groupindex, index), pkWay));
			if (!ret.second)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__<<L"Cannot insert waypoint filename["<<UNI(pchFilePath)<<L"], Group["<<groupindex<<L"], Index["<<index<<L"]");
				m_kWayPointPool.Delete(pkWay);
				continue;
			}
		}

		fscanf_s(fp, "%s", buffer, 64);	// READ("}")
	}

	fclose(fp);
	return true;
}

bool PgWayPointMng::FindBeforeWayPointRandomPos(WayPointKey& kKey, POINT3& pt3NextPos)
{
	if ( kKey.sIndex <= 1 ) // iIndex - 1 이 0 이하이면 안된다.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	const PgWayPoint2* pkWayPoint = GetWayPoint(WayPointKey(kKey.sGroup, kKey.sIndex-1));
	if ( pkWayPoint == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	pkWayPoint->GetRandomPos(pt3NextPos);
	--kKey.sIndex;

	return true;
}

bool PgWayPointMng::FindNearWayPoint(POINT3 const& pt3Pos, WayPointKey& kKey, float const fZLimit)
{
	float fNearest = FLT_MAX;
	//float const fZHeight = 30.0f; // Z축 30.0f 차이내로 검사

	bool bFind = false;
	WAYPOINT_CONT::const_iterator itor = m_kWayPointCont.begin();
	while(m_kWayPointCont.end() != itor)
	{
		const WAYPOINT_CONT::mapped_type& rkElement = (*itor).second;
		float const fDiffZ = abs(pt3Pos.z - rkElement->Point().z);
		if((fZLimit <= 0.0f) || (fZLimit > fDiffZ))
		{
			float const fDist = GetDistanceQ(pt3Pos, rkElement->Point());
			if (fNearest > fDist)
			{
				fNearest = fDist;
				kKey = itor->first;
				bFind = true;
			}
		}
		++itor;
	}
	return bFind;
}

bool PgWayPointMng::FindWayPos(CUnit* pkUnit, PgGround* pkGround)
{
    if(!pkUnit) return false;
    if(!pkGround) return false;
	
    int const iGroup = pkUnit->GetAbil(AT_WAYPOINT_GROUP);
	int const iIndex = pkUnit->GetAbil(AT_WAYPOINT_INDEX);
	short int sFromIndex = (short int)pkUnit->GetAbil(AT_WAYPOINT_INDEX_FROM);

    POINT3 kPos(0,0,0);
	WayPointKey kKey(iGroup, iIndex);
	if ( iGroup <= 0 || iIndex <= 0 )
	{
		if ( FindNearWayPoint(pkUnit->GetPos(), kKey) )
		{
			FindWayPointRandomPos(kKey, kPos);
			pkUnit->SetAbil(AT_WAYPOINT_GROUP, kKey.sGroup);
			pkUnit->SetAbil(AT_WAYPOINT_INDEX, kKey.sIndex);
			pkUnit->SetAbil(AT_WAYPOINT_INDEX_FROM, kKey.sIndex);
			SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kPos);
			//INFO_LOG(BM::LOG_LV9, _T("[%s] [0,0] --> New WayPoint[%hd,%hd]"), __FUNCTIONW__, kKey.sGroup, kKey.sIndex);
			return true;
		}
		else
		{
			POINT3 ptPos = pkUnit->GetPos();
			if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
			{
				INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot find Nearest WayPoint Map["<<m_kGroundKey.GroundNo()<<L"], UnitPos["<<ptPos);
			}
			// Z축 높이차이 무시하고 무조건 제일 가까운 WayPoint를 찾아서 그쪽으로 옮겨주자.
			if (FindNearWayPoint(ptPos, kKey, 0))
			{
				if (FindWayPointRandomPos(kKey, ptPos))
				{
					pkUnit->SetPos(ptPos);
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	bool bSuccess = false ;
	// Random 하게 앞/뒤 WayPoint 선택
	if (iIndex <= 1)	// 한개짜리 WayPoint 인지 검사하자
	{
		const PgWayPoint2* pkWayPoint = GetWayPoint(WayPointKey(iGroup, iIndex+1));
		if (pkWayPoint == NULL)
		{
			// 한개짜리 WayPoint 이므로 WayPoint 안에서 움직여야 한다.
			bSuccess = FindWayPointRandomPos(kKey, kPos);
		}
	}

	if (!bSuccess)
	{
		bSuccess = true;
		// kKey.sIndex != iFromIndex (WayPoint 중간 GoalPos로 이동중이다.)
		// kKey.sIndex == iFromIndex (WayPoint(kKey.sIndex)의 GoalPos로 이동중이다.)
		bool bNext = true;
		if (kKey.sIndex != sFromIndex)
		{
			bNext = (kKey.sIndex > sFromIndex) ? true : false;
		}
		else // if (kKey.sIndex == (short int)iFromIndex)
		{
			bNext = (BM::Rand_Index(2) == 0) ? false : true;
		}

		if (bNext)	// Next로 이동
		{
			if ( !FindNextWayPointRandomPos(kKey, kPos) )
			{
				if ( !FindBeforeWayPointRandomPos(kKey, kPos) )
				{
					bSuccess = false;
				}
			}
			if ((kKey.sIndex == sFromIndex) && bSuccess)
			{
				POINT3 ptNext = kPos;
				GetMidwayPoint(pkUnit->GetPos(), ptNext, kPos);
			}
			else	// if (kKey.sIndex != sFromIndex)
			{
				sFromIndex = kKey.sIndex;
			}
		}
		else	// Before 로 이동
		{
			if ( !FindBeforeWayPointRandomPos(kKey, kPos) )
			{
				if ( !FindNextWayPointRandomPos(kKey, kPos) )
				{
					bSuccess = false;
				}
			}
			if ((kKey.sIndex == sFromIndex) && bSuccess)
			{
				POINT3 ptNext = kPos;
				GetMidwayPoint(pkUnit->GetPos(), ptNext, kPos);
			}
			else	// if (kKey.sIndex != sFromIndex)
			{
				sFromIndex = kKey.sIndex;
			}
		}
	}
	if ( bSuccess )
	{
		pkUnit->SetAbil(AT_WAYPOINT_GROUP, kKey.sGroup);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX, kKey.sIndex);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX_FROM, sFromIndex);
		SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kPos);
		//INFO_LOG(BM::LOG_LV9, _T("[%s] %hd,%hd --> New WayPoint[%hd,%hd]"), __FUNCTIONW__, iGroup, iIndex, kKey.sGroup, kKey.sIndex);

		return true ;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot get waypoint Unit["<<pkUnit->GetID()<<L"], Current WayPoint Group["<<iGroup<<L"], Index["<<iIndex<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false ;
}

bool PgWayPointMng::FindRoadPos(CUnit* pkUnit, DWORD const dwElapsedTime)
{
    if(!pkUnit) return false;

    int const iGroup = pkUnit->GetAbil(AT_ROADPOINT_GROUP);
	int const iIndex = pkUnit->GetAbil(AT_ROADPOINT_INDEX);

    WayPointKey kKey(iGroup, iIndex);
    const PgWayPoint2* pkWayPoint = GetWayPoint(kKey);
	if ( pkWayPoint == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

    if( !pkWayPoint->IsInPos(pkUnit->GetPos()) )
    {
        if(pkUnit->RoadPos()==POINT3::NullData())
        {
            POINT3 ptNext;
            if( FindRoadPointRandomPos(kKey, ptNext) )
            {
                int const iDelayTime = pkUnit->GetAbil(AT_ROADPOINT_DELAY_RESERVED);
                pkUnit->SetAbil(AT_ROADPOINT_DELAY, iDelayTime);
                pkUnit->RoadPos(ptNext);
            }
        }
        pkUnit->GoalPos(pkUnit->RoadPos());
		return true;
    }

    int const iDelay = pkUnit->GetAbil(AT_ROADPOINT_DELAY);
    if( iDelay )
    {
        pkUnit->SetAbil(AT_ROADPOINT_DELAY, std::max((int)(iDelay - dwElapsedTime), 0));
        return false; //RoadPoint에서 딜레이 시간(전략요소)
    }

    POINT3 ptNext;
	if( FindNextRoadPointRandomPos(kKey, ptNext) )
    {
        volatile static int const iTestValue = 0;
        int const iDelayTime = pkUnit->GetAbil(AT_ROADPOINT_DELAY_RESERVED) + iTestValue;
        pkUnit->SetAbil(AT_ROADPOINT_GROUP, kKey.sGroup);
		pkUnit->SetAbil(AT_ROADPOINT_INDEX, kKey.sIndex);
		pkUnit->SetAbil(AT_ROADPOINT_INDEX_FROM, iIndex);
        pkUnit->SetAbil(AT_ROADPOINT_DELAY, iDelayTime);
        pkUnit->RoadPos(ptNext);
		pkUnit->GoalPos(ptNext);
		return true;
    }

    pkUnit->SetAbil(AT_ROADPOINT_GROUP, 0);
	pkUnit->SetAbil(AT_ROADPOINT_INDEX, 0);
    pkUnit->SetAbil(AT_ROADPOINT_INDEX_FROM, iIndex);
    pkUnit->SetAbil(AT_ROADPOINT_DELAY, 0);
	//VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot get waypoint Unit["<<pkUnit->GetID()<<L"], Current WayPoint Group["<<iGroup<<L"], Index["<<iIndex<<L"]");
	//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false ;
}

bool PgWayPointMng::FindNextWayPointRandomPos(WayPointKey& rkKey, POINT3& pt3NextPos)
{
	if ( rkKey.sIndex <= 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	const PgWayPoint2* pkWayPoint = GetWayPoint(WayPointKey(rkKey.sGroup, rkKey.sIndex+1));
	if ( pkWayPoint == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}
	
	++rkKey.sIndex;
	pkWayPoint->GetRandomPos(pt3NextPos);
	return true;
}

bool PgWayPointMng::FindNextRoadPointRandomPos(WayPointKey& rkKey, POINT3& pt3NextPos)
{
	if ( rkKey.sIndex <= 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	const PgWayPoint2* pkWayPoint = GetWayPoint(WayPointKey(rkKey.sGroup, rkKey.sIndex+1));
	if ( pkWayPoint == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}
	
	++rkKey.sIndex;
	pkWayPoint->GetRandomPos2(pt3NextPos);
	return true;
}

bool PgWayPointMng::FindRoadPointRandomPos(const WayPointKey& rkKey, POINT3& pt3Pos)
{
	const PgWayPoint2* pkWayPoint = GetWayPoint(rkKey);
	if ( pkWayPoint == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	pkWayPoint->GetRandomPos2(pt3Pos);
	return true;
}
	
const PgWayPoint2* PgWayPointMng::GetWayPoint(const WayPointKey& rkKey)
{
	if ( rkKey.sGroup <= 0 || rkKey.sIndex <= 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL ;
	}

	WAYPOINT_CONT::iterator itor = m_kWayPointCont.find(rkKey);

	if ( itor != m_kWayPointCont.end())
	{
		return itor->second;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL ;
}

/*
int PgWayPointMng::GetWayPointGroupCount()
{
	return m_kWayPoint.iGroupCount ;
}
*/

bool PgWayPointMng::FindWayPointRandomPos(const WayPointKey& rkKey, POINT3& rkPos)
{
	const PgWayPoint2* pkWayPoint = GetWayPoint(rkKey);
	if (pkWayPoint == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkWayPoint->GetRandomPos(rkPos);
	return true;
}

void PgWayPointMng::Release()
{
	WAYPOINT_CONT::iterator itor = m_kWayPointCont.begin();
	while (itor != m_kWayPointCont.end())
	{
		if (itor->second != NULL)
		{
			m_kWayPointPool.Delete(itor->second);
		}
		++itor;
	}
	m_kWayPointCont.clear();
}

void PgWayPointMng::GetMidwayPoint(POINT3 const& rkPos1, POINT3 const& rkPos2, POINT3& rkOut)
{
	int iDistance = (int)GetDistance(rkPos1, rkPos2);
	NxVec3 kDirection(rkPos2.x-rkPos1.x, rkPos2.y-rkPos2.y, rkPos2.z-rkPos2.z);
	kDirection.normalize();
	kDirection *= NxReal(BM::Rand_Index(iDistance));
	rkOut.x = rkPos1.x + kDirection.x; rkOut.y = rkPos1.y + kDirection.y; rkOut.z = rkPos1.z + kDirection.z;
}

bool PgWayPointMng::GetRunawayPos(CUnit* pkUnit, POINT3 const& rkEnemyPos)
{
	int iGroup = pkUnit->GetAbil(AT_WAYPOINT_GROUP);
	if (iGroup == 0)	// First Call
	{
		WayPointKey kKey;
		if (!FindNearWayPoint(pkUnit->GetPos(), kKey))
		{
			INFO_LOG(BM::LOG_LV0, L"Cannot find NearWayPoint from["<<pkUnit->GetPos()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		const PgWayPoint2* pkWayPoint = GetWayPoint(kKey);
		// rkEnemyPos 로 부터 가장 먼 위치의 WayPos 값을 구하자
		POINT3 ptResult;
		pkWayPoint->GetOppositePos(rkEnemyPos, ptResult);

		pkUnit->SetAbil(AT_WAYPOINT_GROUP, kKey.sGroup);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX, kKey.sIndex);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX_FROM, 0);
		pkUnit->GoalPos(ptResult);
		return true;
	}
	// 방법 1. NextWayPoint / BeforeWayPoint 중에서 rkEnemyPos와 먼쪽으로 도망가면 된다.
	// 방법 2. rkEnemyPos와 내적을 구해서, 각도가 적은 쪽으로 도망간다.
	int iIndex = pkUnit->GetAbil(AT_WAYPOINT_INDEX);
	const WayPointKey kKey((short int)iGroup, (short int)iIndex);
	const PgWayPoint2* pkCurr = GetWayPoint(kKey);
	const PgWayPoint2* pkNext = GetNextWayPoint(kKey);
	const PgWayPoint2* pkPrev = GetBeforeWayPoint(kKey);
	if (pkCurr == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot GetWayPoint Group["<<kKey.sGroup<<L"], Index["<<kKey.sIndex<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if (pkNext == NULL && pkPrev == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;	// 도망갈 곳이 없다.
	}

	POINT3 ptCurr, ptNext, ptPrev;
	pkCurr->GetOppositePos(rkEnemyPos, ptCurr);
	float fNext = 0.0, fPrev = 0.0;
	if (pkNext != NULL)
	{
		pkNext->GetOppositePos(rkEnemyPos, ptNext);
		fNext = GetVectorDot(rkEnemyPos, ptCurr, ptNext);
	}
	else
	{
		ptNext = rkEnemyPos;
	}
	if (pkCurr != NULL)
	{
		pkPrev->GetOppositePos(rkEnemyPos, ptPrev);
		fPrev = GetVectorDot(rkEnemyPos, ptCurr, ptPrev);
	}
	else
	{
		ptPrev = rkEnemyPos;
	}

	if (fNext > fPrev && fPrev < 0)	// Prev쪽으로 가라
	{
		pkUnit->SetAbil(AT_WAYPOINT_GROUP, kKey.sGroup);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX, kKey.sIndex-1);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX_FROM, 0);
		return true;
	}
	else if (fNext < fPrev && fNext < 0) // Next쪽으로 가라
	{
		pkUnit->SetAbil(AT_WAYPOINT_GROUP, kKey.sGroup);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX, kKey.sIndex+1);
		pkUnit->SetAbil(AT_WAYPOINT_INDEX_FROM, 0);
		return true;
	}
	// else if (fNext == fPrev && fNext < 0) <---- float 형인데 이렇게 될수 있을까?
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

const PgWayPoint2* PgWayPointMng::GetNextWayPoint(const WayPointKey& rkKey)
{
	WayPointKey kKey(rkKey.sGroup, rkKey.sIndex+1);
	return GetWayPoint(kKey);
}

const PgWayPoint2* PgWayPointMng::GetBeforeWayPoint(const WayPointKey& rkKey)
{
	WayPointKey kKey(rkKey.sGroup, rkKey.sIndex-1);
	return GetWayPoint(kKey);
}
