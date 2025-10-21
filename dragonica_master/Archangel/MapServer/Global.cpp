#include "stdafx.h"
#include <NiPick.h>
#include "Lohengrin/VariableContainer.h"
#include "Variant/constant.h"
#include "Variant/Global.h"
#include "Variant/PgEventView.h"
#include "Variant/PgPlayer.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/Item.h"
#include "Variant/PgActionResult.h"
#include "constant.h"
#include "Global.h"
#include "PgQuest.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgGround.h"
#include "PgActionAchievement.h"
#include "PgEffectFunction.h"
#include "PgEffectAbilTable.h"
#include "PublicMap/PgBSGround.h"
#include "AI\PgElitePattenManager.h"
#include "PgSkillLinkageInfo.h"
#include "PgGroundUnitMgr.h"

int iCanAddExpMinLevel = 0;
int iCanAddExpMaxLevel = 0;
int g_iMyHomeAuctionLimitGold = 0;

extern int GetElementAbilValue(CUnit const* pkUnit, eElementDmgType const Type, 
							   EGetAbilType::Enum const getabiltype = EGetAbilType::EGAT_ALL);

namespace GlobalHelper
{
	BM::GUID FindOwnerGuid(CUnit const* pkCaster)
	{
		if( pkCaster )
		{
			// Player
			if( pkCaster->IsUnitType(UT_PLAYER) )
			{
				return pkCaster->GetID();
			}

			// Entity
			if( pkCaster->IsUnitType(UT_ENTITY) )
			{
				if( pkCaster->GetID() != pkCaster->Caller() )
				{
					return pkCaster->Caller();
				}
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NullData"));
		return BM::GUID::NullData();
	}

	bool IsAdjustSkillEffect(CEffect const* pkEffect)
	{// 이 이펙트가 스킬 조정하는 이펙트인지 확인한다
		if(!pkEffect)
		{
			return false;
		}
		if(0 >= pkEffect->GetAbil(AT_ADJUST_SKILL_CHECK_TYPE))
		{
			return false;
		}
		return true;
	}
}


void RefreshGroundQuestInfo(CUnit* pkUnit)
{
	bool bIsPlayer = pkUnit->IsUnitType(UT_PLAYER);
	if( !bIsPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bIsPlayer is NULL"));
		return;
	}

	BM::Stream kNfyPacket(PT_U_G_RUN_ACTION); // 전체 Quest 정보 Refresh
	kNfyPacket.Push( static_cast< short int>(GAN_Refresh_All_Quest) );
	pkUnit->VNotify( &kNfyPacket );
}

// Getting Path Normal vector from position
// [PARAMETER]
//	kPos : [IN/OUT] position of checking
//	pkScene : Physics scnen
//	kNormal : [OUT] normal vector
//	iLength : kPos가 Path의 HitPoint와 떨어지게 만들 거리
/*bool GetPathNormal(POINT3& kPos, NxScene* pkScene, NxVec3& kNormal, int iLength)
{
	static NxVec3 akDir[] = 
	{
		NxVec3(1.0f, 0, 0),
		NxVec3(-1.0f, 0, 0),
		NxVec3(0, -1.0f, 0),
		NxVec3(0, 1.0f, 0),
	};
	float const fPathHeight = 65.0f;
	NxRay kRay(NxVec3(kPos.x, kPos.y, kPos.z + fPathHeight), NxVec3(0, 0, 1));
	float fNear = FLT_MAX;
	NxRaycastHit kHit;
	NxVec3 kHitPoint;
	for (int i=0; i<4; i++)
	{
		kRay.dir = akDir[i];
		NxShape* pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f);
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV8, _T("[%s] - HitShapeName[%s]"), __FUNCTIONW__, UNI(pkHitShape->getName()));
#endif
		if(pkHitShape)
		{
			float fDist = kRay.orig.distance(kHit.worldImpact);
			if (fDist > 200.0f || fNear < fDist)
			{
				continue;
			}
			fNear = fDist;
			kNormal = kHit.worldNormal;
			kHitPoint = kHit.worldImpact;
		}
	}

	if (fNear < 200.0f)
	{
		NxVec3 kNewPos = NxVec3(kPos.x, kPos.y, kPos.z + fPathHeight) - NxVec3(kHitPoint);
		kNewPos.normalize();
		kNewPos *= iLength;
		kNewPos += kHitPoint;
		kPos.x = kNewPos.x; kPos.y = kNewPos.y;
		return true;
	}

	
	return false;
}*/

// Getting Path Normal vector from position
// [PARAMETER]
//	kPos : [IN/OUT] position of checking
//	pkScene : Physics scnen
//	kTarget : [IN] position of target
//	kNormal : [OUT] normal vector
bool GetPathInfo(NxScene *pkScene, NiNode* pkPathObject, POINT3 const& kPos,POINT3 const& kTarget, NxVec3& kNormal, NxVec3& kTargetVec, float* pfDistance)
{
	// 이 밑으로는 방향벡터 구하기
	//if ( kTarget.x == 0 && kTarget.y == 0 && kTarget.z == 0 )
	if ( kTarget.x == 0 && kTarget.y == 0 )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}
	
//	YonMy 20070824 PathNormal 로직 교체
//	GetPathNormal(pkScene, pkPathObject, kInterPos, kNormal);
	GetPathNormal( pkPathObject, kPos, kNormal );

	POINT3 kInterPos( kPos.x, kPos.y, 0.0f );
	POINT3 kTargetPos( kTarget.x, kTarget.y, 0.0f );
	kTargetVec.set(kTargetPos.x - kInterPos.x ,kTargetPos.y - kInterPos.y ,0.0f );
	float fDist = GetDistance( kTargetPos,kInterPos);
	if (pfDistance != NULL)
	{
		*pfDistance = fDist;
	}

	kTargetVec.normalize();
//	fNear = 30.0f;
//	if ( fDist < fNear )
//	{
//		kTargetVec.set(0,0,0);
//		return false ;
//	}

	if ( kTargetVec.x == 0.0f && kTargetVec.y == 0.0f && kTargetVec.z == 0.0f )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}
	else if ( fDist <= 0.0f )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	return true;
}

// EPSILON을 적용해야 한다.
bool RealEqual( const NiPoint3& kLeft, const NiPoint3& kRight )
{
	static float fGetPathEpsilon = 0.001f;
	return		( abs( kLeft.x - kRight.x ) <= fGetPathEpsilon )
			&&	( abs( kLeft.y - kRight.y ) <= fGetPathEpsilon )
			&&	( abs( kLeft.z - kRight.z ) <= fGetPathEpsilon );
}

// ※※※※※※※※※ *YonMy* 최적화 작업해야함!!! ※※※※※※※※※
bool GetPathNormal( NiNode* pkPath, POINT3 const& kPos, NxVec3& kNormal )
{
	static NiPoint3 akDir[] =
	{
		NiPoint3( 0.0f, -1.0f, 0.0f ),
		NiPoint3( 1.0f, 0.0f, 0.0f ),
		NiPoint3( -1.0f, 0.0f, 0.0f ),
		NiPoint3( 0.0f, 1.0f, 0.0f )
	};
	NiPick kPick;

	kPick.SetTarget( pkPath );
	kPick.SetReturnNormal( true );

	int i;
	NiPoint3 kNiPos( kPos.x, kPos.y, kPos.z );
	for ( i=0; i!=4; ++i )
	{
		// 4방향에 피킹을 해봐서...
		kPick.PickObjects( kNiPos, akDir[i], true );
	}

	NiPoint3 kNiNormal;
	bool bPick = false;
	if ( kPick.GetSize() )
	{
		// 제일 가까운 놈을 대상으로 노멀벡터를 설정한다.
		kNiNormal = kPick.GetResults().GetAt(0)->GetNormal();
		kPick.ClearResultsArray();
	}
	else
	{
		INFO_LOG( BM::LOG_LV0, __FL__<<L"Path is Bad!!!" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << bPick);
		return bPick;
	}
	
//	노멀벡터가 실제 정확한 위치의 노멀벡터인지 검증을 실시한다~
//	이 구문은 직선맵에서는 안해주어도 되나~ 곡선맵에서는 필요하다(마을등)
//	최적화 Vs 정교함 ???
	NiPoint3 kRealNormal;
	i = 0;
	do
	{	
		kRealNormal = -kNiNormal;
		if ( kPick.PickObjects( kNiPos, kRealNormal ) )
		{
			NiPick::Results& pickResults = kPick.GetResults();
			kNiNormal = pickResults.GetAt(0)->GetNormal();
			bPick = true;
		}
/*		else
		{
			if ( kPick.PickObjects( kNiPos, -kRealNormal ) )
			{
				INFO_LOG( BM::LOG_LV5, _T("This Position Is Path Back... %.2f, %.2f, %.2f"), kPos.x, kPos.y, kPos.z );
				kNormal.x = kRealNormal.x;
				kNormal.y = kRealNormal.y;
				kNormal.z = kRealNormal.z;
				return bPick;
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV3, _T("Path나 로지컬 중 문제가 있어 보인다.") );
				break;
			}	
		}*/
	} while( !RealEqual( -kNiNormal, kRealNormal ) && ++i < 5 );
	
	kNormal.x = kNiNormal.x;
	kNormal.y = kNiNormal.y;
	kNormal.z = kNiNormal.z;
	return bPick;
}

bool GetPathNormal(NxScene *pkScene, NiNode* pkPathObject, POINT3 const& kPos, NxVec3& kNormal)
{
	static NxVec3 akDir[] = 
	{
		NxVec3(1.0f, 0, 0),
		NxVec3(-1.0f, 0, 0),
		NxVec3(0, -1.0f, 0),
		NxVec3(0, 1.0f, 0),
	};

	POINT3 kInterPos(kPos.x, kPos.y, 0);	// Z축은 무시

	float const fPathHeight = 65.0f;
	NxRay kRay(NxVec3(kPos.x, kPos.y, kPos.z + fPathHeight), NxVec3(0, 0, 1));
	float fNear = FLT_MAX;
	NxRaycastHit kHit;
	bool bSuccess = false;
	for (int i=0; i<4; i++)
	{
		kRay.dir = akDir[i];
		NxShape* pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1);
		if(pkHitShape)
		{
			//char const* name = pkHitShape->getName() ;
			//NiNode* pkNode = (NiNode*)pkPathObject->GetAt(0);
			//NiAVObject* pkNode = (NiAVObject*) pkPathObject->GetObjectByName(name);
			
			//if ( !pkNode )
			//	continue ;

			//char const * name2 = pkNode->GetName();

			//if ( strcmp(name,name2) )
			//	continue ;

			float fDist = kRay.orig.distance(kHit.worldImpact);
			if (fDist > 400.0f || fNear < fDist)
			{
				continue;
			}
			fNear = fDist;
			kNormal = kHit.worldNormal;
			bSuccess = true;
		}
	}
	if( !bSuccess )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return bSuccess;
}

float GetDistance(POINT3 const& kPos1, POINT3 const& kPos2)
{
	return sqrt(pow(kPos2.x - kPos1.x, 2) + pow(kPos2.y - kPos1.y, 2) + pow(kPos2.z - kPos1.z, 2));
}

float GetDistanceQ(POINT3 const& kPos1, POINT3 const& kPos2)//sqrt를 안쓰는 Quick Vector Length
{//A^2 + B^2 = C^2, C = sqrt(A^2 + B^2), C^2 = A^2 + B^2
	return pow(kPos2.x - kPos1.x, 2) + pow(kPos2.y - kPos1.y, 2) + pow(kPos2.z - kPos1.z, 2);
}

EInRangeRet IsInRange(POINT3 const& kPos1, POINT3 const& kPos2, int const iRangeLimit, int const iZLimit, bool const bUseMinRange)
{
	POINT3 ptDist = kPos2 - kPos1;
	
	if (abs(ptDist.y) > iRangeLimit)
	{
		return ERange_Y;
	}
	if (abs(ptDist.x) > iRangeLimit)
	{
		return ERange_X;
	}

	if (bUseMinRange)
	{
		if (abs(ptDist.z) > __min(iRangeLimit, iZLimit))
		{
			return ERange_MIN;
		}
	}
	else
	{
		if (abs(ptDist.z) > iZLimit)
		{
			return ERange_Z;
		}
	}
	return ERange_OK;
}

bool IsBetweenRange(POINT3 const& kPos1, POINT3 const& kPos2, int iMinRange, int iMaxRange)
{
	float fDist = GetDistanceQ(kPos1, kPos2);
#ifdef AI_DEBUG
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Pos1[%05d,%05d,%05d], Pos2[%05d,%05d,%05d], MinRange[%d], MaxRange[%d] Range[%6.2f]"), __FUNCTIONW__,
	//	(int)kPos1.x, (int)kPos1.y, (int)kPos1.z, (int)kPos2.x, (int)kPos2.y, (int)kPos2.z, iMinRange, iMaxRange, fDist);
#endif
	if (fDist > iMaxRange*iMaxRange || fDist < iMinRange*iMinRange)
	{
		return false;
	}

	return true;
}

bool IsInCube( POINT3 const& kCubeMin, POINT3 const &kCubeMax, POINT3 const& kPos )
{
	if ( (kPos.x < kCubeMin.x) || (kPos.y < kCubeMin.y) || (kPos.z < kCubeMin.z) )
	{
		return false;
	}

	if ( (kPos.x > kCubeMax.x) || (kPos.y > kCubeMax.y) || (kPos.z > kCubeMax.z) )
	{
		return false;
	}

	return true;
}

bool FindRandomPos(POINT3& kPos,NxScene* pkScene,float fMinDist,float fMaxDist)
{
	static NxVec3 akDir[] = 
	{
		NxVec3(1.0f, 0, 0),
		NxVec3(-1.0f, 0, 0),
		NxVec3(0, -1.0f, 0),
		NxVec3(0, 1.0f, 0),
	};

	float const fPathHeight = 65.0f;
	NxRay kRay(NxVec3(kPos.x, kPos.y, kPos.z + fPathHeight), NxVec3(0, 0, 0));
	NxRaycastHit kHit;
	NxVec3 kHitPoint;

	NxVec3 akNew[4] = {};

	for (int i=0; i<4; i++)
	{
		kRay.dir = akDir[i];
		NxShape* pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, fMaxDist);
		if(pkHitShape)
		{

			float fdist = kRay.orig.distance(kHit.worldImpact);
			if (fdist < fMinDist)
			{
				akNew[i] = kHit.worldImpact;
				continue;
			}
			else if ( fdist > fMaxDist )
			{
				akNew[i] = kRay.orig + kRay.dir * NxReal(fMaxDist) ;
				continue;
			}
			else
			{
				akNew[i] = kRay.orig + kRay.dir * NxReal(fdist) ;
			}
		}
		else
		{
			akNew[i] = kRay.orig + kRay.dir * NxReal(fMaxDist) ;
		}
	}

	int randX = ( BM::Rand_Index( (int)(( akNew[0].x - akNew[1].x )))) ;
	if ( akNew[0].x - akNew[1].x < 0 )
	{
		randX = randX * -1 ;
	}
	int randY = ( BM::Rand_Index( (int)(( akNew[2].y - akNew[3].y )))) ;
	if ( akNew[2].y - akNew[3].y < 0 )
	{
		randY = randY * -1 ;
	}

	float x = (float)randX + akNew[1].x ;
	float y = (float)randY + akNew[3].y ;

	NxVec3 kVec(x,y,kPos.z);

	NxVec3 kTargetVec(kPos.x - kVec.x ,kPos.y - kVec.y ,kPos.z - kVec.z );
	kRay.orig.z -= fPathHeight ;
	float fDist = kRay.orig.distance(kVec);

	kTargetVec.normalize();
	kRay.dir = kTargetVec;

/*	if ( fDist < fMinDist || fDist > fMaxDist )
	{
		INFO_LOG(BM::LOG_LV8, _T("좌표 뽑았는데 왜 거리가 안 맞을까? MinDist[%d] / MaxDist[%d] / Dist[%4.4f]"),(int)fMinDist,(int)fMaxDist,fDist);
		return false ;
	}*/

	NxShape* pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, fMaxDist);
	if(pkHitShape)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	kRay.orig.set(x,y,kPos.z+20.0f);
	kRay.dir.set(0,0,-1.0f);

	pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 40.0f);
	if(!pkHitShape)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false ;
	}

	kPos.x = x ;
	kPos.y = y ;

	return true;
}

// 특정위치(rkPos)로 부터 어떤방향(left or right) 얼마만큼(dwDistance) 떨어진 위치 얻어내기
//	Physix에 나란한 방향으로 찾는다.
//	예를 들면 : 현재 몬스터의 오른쪽으로 200 떨어진 위치 값 찾기
// bool GetPathNormalParellelVector(NxScene *pkScene, NiNode* pkPathObject, POINT3 const& rkPos, short int sDirection, DWORD dwDistance, POINT3& ptOut)
// {
// 	NxVec3 kNormal;
// 	if ( !GetPathNormal(pkScene, pkPathObject, rkPos, kNormal))
// 	{
// 		ptOut.x = 0.0f; ptOut.y = 0.0f; ptOut.z = 0.0f;
// 		return false;
// 	}
// 
// 	NxVec3 kZVector(0., 0., (sDirection==1) ? -1. : 1. );
// 	NxVec3 kOut = kZVector.cross( kNormal );
// 	kOut.normalize();
// 	kOut = NxVec3(rkPos.x, rkPos.y, rkPos.z) + kOut * NxReal(dwDistance);
// 	ptOut.x = kOut.x; ptOut.y = kOut.y; ptOut.z = kOut.z;
// 	return true;
// }

// YonMy 20070817
// 특정위치(rkPos)로 부터 Left / Right (sDirection), 얼마만큼(dwDistance) 떨어진 위치를 얻음 -> rkOutPos
bool GetDistanceToPosition( NiNode* pkPathObject, POINT3 const &rkPos, Direction eDirection, float const fDistance, POINT3& rkOutPos )
{
	NxVec3 kXAxis;
	GetAxisVector3( pkPathObject, rkPos, eDirection, kXAxis );
	kXAxis *= fDistance;
	rkOutPos.x = rkPos.x + kXAxis.x;
	rkOutPos.y = rkPos.y + kXAxis.y;
	rkOutPos.z = rkPos.z + kXAxis.z + 20.0f;	// z축은 좀 올려놓는다.

	return true;
}

Direction GetCrossDirection( Direction eDirection, bool const bRight )
{
	if ( true == bRight )
	{
		switch( eDirection )
		{
		case DIR_UP:{return DIR_RIGHT;}break;
		case DIR_DOWN:{return DIR_LEFT;}break;
		case DIR_LEFT:{return DIR_UP;}break;
		case DIR_RIGHT:{return DIR_DOWN;}break;
		case DIR_LEFT_DOWN:{return DIR_LEFT_UP;}break;
		case DIR_LEFT_UP:{return DIR_RIGHT_UP;}break;
		case DIR_RIGHT_UP:{return DIR_RIGHT_DOWN;}break;
		case DIR_RIGHT_DOWN:{return DIR_LEFT_DOWN;}break;
		}
	}
	else
	{
		switch( eDirection )
		{
		case DIR_UP:{return DIR_LEFT;}break;
		case DIR_DOWN:{return DIR_RIGHT;}break;
		case DIR_LEFT:{return DIR_DOWN;}break;
		case DIR_RIGHT:{return DIR_UP;}break;
		case DIR_LEFT_DOWN:{return DIR_RIGHT_DOWN;}break;
		case DIR_LEFT_UP:{return DIR_LEFT_DOWN;}break;
		case DIR_RIGHT_UP:{return DIR_LEFT_UP;}break;
		case DIR_RIGHT_DOWN:{return DIR_RIGHT_UP;}break;
		}
	}

	return eDirection;
}

Direction GetRandomDirection( bool const bAll )
{
	static Direction eDir[8] = {	DIR_LEFT,		DIR_RIGHT,		DIR_UP,		DIR_DOWN
								,	DIR_LEFT_UP,	DIR_RIGHT_UP,	DIR_LEFT_DOWN,	DIR_RIGHT_DOWN
								};


	return eDir[ BM::Rand_Index( ((true == bAll ) ? 8 : 2) ) ];
}

// 방향벡터(노말라이즈)를 리턴한다.
bool GetAxisVector3( NiNode* pkPathObject, POINT3 const &rkPos, Direction eDirection, NxVec3& rkOutAxis )
{
	static NxVec3 s_kZAxis(0.0f, 0.0f, 1.0f );
	NxVec3 kPathNormal;
	if ( !GetPathNormal( pkPathObject, rkPos, kPathNormal ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch( eDirection )
	{
	
	case DIR_LEFT:
		{
			rkOutAxis = s_kZAxis.cross( kPathNormal );
			return true;
		}break;
	case DIR_LEFT_UP:
	case DIR_LEFT_DOWN:
		{
			rkOutAxis = s_kZAxis.cross( kPathNormal );
		}break;	// Left
	case DIR_RIGHT:
		{
			rkOutAxis = kPathNormal.cross( s_kZAxis );
			return true;
		}break;
	case DIR_RIGHT_UP:
	case DIR_RIGHT_DOWN:
		{
			rkOutAxis = kPathNormal.cross( s_kZAxis );
		}break;	// Right
	case DIR_DOWN:
		{	
			rkOutAxis = -kPathNormal;
			return true;
		}break;	// Down(front)
	case DIR_UP:
		{	rkOutAxis = kPathNormal;
			return true;
		}break;	// Up	
	default:
		{
			rkOutAxis.set( 0.0f, 0.0f, 0.0f );
		}break;
	}

	if ( DIR_DOWN & eDirection )
	{
		rkOutAxis -= kPathNormal;
	}
	else if ( DIR_UP & eDirection )
	{
		rkOutAxis += kPathNormal;
	}
	return true;
}


bool SetValidGoalPos(NxScene *pkScene, CUnit *pkUnit, POINT3 &rkEndPos, float fHeight, float fMinDistance, DWORD dwFlag)
{
	if(!pkUnit)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 먼저 바닥으로 쏴서 GoalPos가 있는지 체크.
	POINT3 kCandidateEndPos(rkEndPos);
	float const fRayLength = 170.0f;		// 아래로 쏘는 거리 (얼마가 적당할지 => 단차는 약 100)
	float const fAlpha = 5.0f;			// 여분

	NxRaycastHit kHit;
	NxRay kRay(NxVec3(rkEndPos.x, rkEndPos.y, rkEndPos.z+20), NxVec3(0.0f, 0.0f, -1.0f));
	NxShape* pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, fRayLength);
	if(!pkHitShape)
	{
		// 땅이 없으므로 무효
		if (dwFlag & EFlags_SetGoalPos)
		{
			pkUnit->GoalPos(POINT3(0,0,0));
		}
		//INFO_LOG(BM::LOG_LV0, _T("[%s] GoalPos's Floor is void (%.4f, %.4f, %.4f)"), __FUNCTIONW__, rkEndPos.x, rkEndPos.y, rkEndPos.z);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	kCandidateEndPos.z = kHit.worldImpact.z;

	if(dwFlag&EFlags_CheckFromGround)	
	{
		rkEndPos.z = kCandidateEndPos.z;
	}

	if( rkEndPos==pkUnit->GetPos())
	{
		return true;
	}
	
	POINT3 kStartPos = pkUnit->GetPos();
	POINT3 ptRayDir = rkEndPos - kStartPos;
	NxVec3 kRayDir(ptRayDir.x, ptRayDir.y, ptRayDir.z);
	float fRayDirLength = kRayDir.magnitude();
	float fLength = fRayDirLength + fAlpha;	// 앞으로 쏘는 거리
	float fTangentRay = (fRayLength == 0) ? 999.f : ptRayDir.z / fRayDirLength;	// tan(direction)

	if (abs(fTangentRay) > 0.5f || abs(ptRayDir.z) > AI_Z_LIMIT)	// 기울기와 Z축 높이차로 이동할 수 있는지 결정하자.
	{
		// RayDir을 경사에 맞게 쏴주어야 한다. 일단 보류하자 (오르막일 경우에 못간다 -_-;)
		//INFO_LOG(BM::LOG_LV9, _T("[%s] GoalPos Slope is more than 0.5f [%.3f]"), __FUNCTIONW__, fTangentRay);
		if (dwFlag & EFlags_SetGoalPos)
		{
			pkUnit->GoalPos(POINT3(0,0,0));
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	kRayDir.normalize();
	kRay.orig = NxVec3(kStartPos.x, kStartPos.y, kStartPos.z + fHeight);
	kRay.dir = kRayDir;
	pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, fLength);
	if(!pkHitShape)
	{
		// 아무런 장애물이 없음.
		if (dwFlag & EFlags_SetGoalPos)
		{
			//INFO_LOG(BM::LOG_LV9, _T("[%s] GoalPos[%4.1f, %4.1f, %4.1f]"), __FUNCTIONW__,
			//	kCandidateEndPos.x, kCandidateEndPos.y, kCandidateEndPos.z);
			pkUnit->GoalPos(kCandidateEndPos);
		}
		return true;
	}

	float const fMinDist = (dwFlag & EFlags_MinDistance) ? __max(fMinDistance, 45.0f) : 45.0f;
	// 장애물에 걸렸음.
	if(kHit.distance < fMinDist)
	{
		// 갈 거리가 30 이하일때는, 벽에 걸렸다면 가지 않는다 (갔다가는 벽에 파묻힌다)
		if (dwFlag & EFlags_SetGoalPos)
		{
			pkUnit->GoalPos(POINT3(0,0,0));
		}
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Length of GoalPos is less then 30 inchs. [%s]"), __FUNCTIONW__, pkUnit->GetID().str().c_str());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 갈 곳을 벽에서 약간 떼어준다.
	if (dwFlag & EFlags_SetGoalPos)
	{
		NxVec3 kFinalGoalPos = kHit.worldImpact - (kRayDir * 30.0f);
		kFinalGoalPos.z -= fHeight;
		//INFO_LOG(BM::LOG_LV9, _T("[%s] GoalPos[%4.1f, %4.1f, %4.1f]"), __FUNCTIONW__,
		//	kFinalGoalPos.x, kFinalGoalPos.y, kFinalGoalPos.z);
		pkUnit->GoalPos(POINT3(kFinalGoalPos.x, kFinalGoalPos.y, kFinalGoalPos.z));
	}
	return true;
}

POINT3 SimulateUnitPos2(PgPlayer* pkPlayer, POINT3 const& ptPos, int iActionID, DWORD dwElapsedTime, DWORD dwLatency, PgGround const* pkGround)
{
	if(!pkPlayer)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Player is Null!!!");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
		return ptPos;
	}
	
	if(!pkPlayer->DoSimulate())
	{
		//	INFO_LOG(BM::LOG_LV9, _T("[%s] Final Pt : %.4f, %.4f, %.4f"),	
		//__FUNCTIONW__, ptPos.x, ptPos.y, ptPos.z);
		return ptPos;
	}
	
	int iMoveSpeed = pkPlayer->GetAbil(AT_C_MOVESPEED);

	// 마을인 경우 마을 이동속도가 추가 된다.
	if(pkGround)
	{
		if(pkGround->GetAttr() & GATTR_VILLAGE)
		{
			iMoveSpeed += pkPlayer->GetAbil(AT_C_VILLAGE_MOVESPEED);
		}
	}

	POINT3BY kPoint3By = pkPlayer->MoveDirection();
	NxVec3 kMovingDirection((char)kPoint3By.x, (char)kPoint3By.y, (char)kPoint3By.z);
	kMovingDirection.normalize();
	NxVec3 kMovingDelta;
	kMovingDelta = kMovingDirection * float(iMoveSpeed * (dwElapsedTime + dwLatency) / 1000.0f);
#ifdef AI_DEBUG
	//INFO_LOG(BM::LOG_LV9, _T("[%s] MoveSpeed[%d], ElapsedTime[%d], Latency[%d], Direction[%05d,%05d,%05d]"), __FUNCTIONW__,
	//	iMoveSpeed, dwElapsedTime, dwLatency, (int)kDirection.x, (int)kDirection.y, (int)kDirection.z);
#endif
	NxVec3 kCurrentPos(ptPos.x, ptPos.y, ptPos.z);
	NxVec3 kSimulatedPos;

	switch(iActionID)
	{
	case ACTIONID_A_RUN:	// a_run
		{
			kSimulatedPos = kCurrentPos + kMovingDelta;
		}break;
	case ACTIONID_A_DASH:	// a_dash
		{
			kSimulatedPos = kCurrentPos + kMovingDelta * 2.3f;
		}break;	
	case ACTIONID_A_JUMP:	// a_jump
		{
			kSimulatedPos = kCurrentPos + kMovingDelta;
			kSimulatedPos.z += 50;
		}
	case ACTIONID_A_DASH_JUMP:	// a_dash_jump
		{
			kSimulatedPos = kCurrentPos + kMovingDelta * 2.3f;
			kSimulatedPos.z += 100;
		}break;
	case 100005901:			// a_break_fall
		{
			kSimulatedPos = kCurrentPos + kMovingDelta;
		}
	default:
		{
			kSimulatedPos = kCurrentPos;
		}break;
	}

//	INFO_LOG(BM::LOG_LV9, _T("[%s] Final Pt : %.4f, %.4f, %.4f / MoveDirection : %.4f, %.4f, %.4f / Elapsed : %u / Latency : %u"),	
//		__FUNCTIONW__, kSimulatedPos.x, kSimulatedPos.y, kSimulatedPos.z, kMovingDirection.x, kMovingDirection.y, kMovingDirection.z, dwElapsedTime, dwLatency);

	return POINT3(kSimulatedPos.x, kSimulatedPos.y, kSimulatedPos.z);
}

float GetVectorDot(POINT3 const& rkPos1, POINT3 const& rkMid, POINT3 const& rkPos3)
{
	NxVec3 kVec1(rkPos1.x-rkMid.x, rkPos1.y-rkMid.y, rkPos1.z-rkMid.z);
	NxVec3 kVec2(rkPos3.x-rkMid.x, rkPos3.y-rkMid.y, rkPos3.z-rkMid.z);

	return kVec1.dot(kVec2);
}

void NfyShowQuestDialog(CUnit* pkUnit, BM::GUID const &rkNpcGUID, EQuestShowDialogType const eQuestShowDialogType, int const iQuestID, int const iDialogID)
{
	if( !pkUnit )
	{
		return;
	}
	BM::Stream kPacket(PT_U_G_RUN_ACTION, static_cast< short >(GAN_SendQuestDialog));
	kPacket.Push( rkNpcGUID );
	kPacket.Push( eQuestShowDialogType );
	kPacket.Push( iQuestID );
	kPacket.Push( iDialogID );
	pkUnit->VNotify(&kPacket);
}

void SendWarnMessage( BM::GUID const &kMemberGuid, int const iMessageNo, BYTE const byLevel )
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE, iMessageNo);
	kPacket.Push(byLevel);
	SendToClient( kMemberGuid, kPacket );
}

void SendWarnMessage2( BM::GUID const &kMemberGuid, int const iMessageNo, int const iValue, BYTE const byLevel )
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE2, iMessageNo);
	kPacket.Push(iValue);
	kPacket.Push(byLevel);
	SendToClient( kMemberGuid, kPacket );
}

void OnCustomTargetDamage(int & iDamage, CUnit* pkTarget, int const iSkillNo)
{
	if(!pkTarget || 0==iSkillNo)
	{
		return;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkillDef)
	{
		return;
	}

	EUnitType const eType = static_cast<EUnitType>(pkSkillDef->GetAbil(AT_CUSTOM_TARGET_DAMAGE_UNITTYPE));
	int const iRate = pkSkillDef->GetAbil(AT_CUSTOM_TARGET_DAMAGE_RATE);
	if( eType && iRate )
	{
		if( pkTarget->IsInUnitType(eType) )
		{
			iDamage += (iDamage*(iRate/ABILITY_RATE_VALUE_FLOAT));
		}
	}
}

bool AdjustFinalDamageAndApply( CUnit* pkCaster, CUnit* pkTarget, PgActionResult* pkAResult, int const iSkillNo, SActArg const *pArg, DWORD const dwTimeStamp )
{// 최종 데미지를 주기 전에 체크해서 처리 해줘야 할것이 있는지 확인해 처리함
	if(!pkAResult)
	{
		return false;
	}

	int iDamage = pkAResult->GetValue();
	if(0 >= iDamage)
	{
		return false;
	}

	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if( pkGround
		&& pkTarget
		&&	pkTarget->IsUnitType(UT_PLAYER) 
		&& UCLASS_SHAMAN == pkTarget->GetAbil(AT_BASE_CLASS) // 소환사를 때렸을때
		)
	{
		int const iShareDmgRate = pkTarget->GetAbil(AT_SHARE_DMG_WITH_SUMMONED);
		if( 0 < iShareDmgRate )
		{//자기 소환체와 데미지를 공유하는 값이 있으면
			CUnit* pkSummonUnit = NULL;
			int iDmgShareUnitCnt = 0;
			VEC_SUMMONUNIT const& kContSummonUnit = pkTarget->GetSummonUnit();
			for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
			{// 자신의 소환체 중
				pkSummonUnit = pkGround->GetUnit((*c_it).kGuid);
				if( pkSummonUnit 
					&& 0 == pkSummonUnit->GetAbil(AT_CANNOT_DAMAGE)
					)
				{// 데미지를 받을 수 있는 소환체를 세어보고
					++iDmgShareUnitCnt;
				}
			}
			
			if(0 < iDmgShareUnitCnt)
			{// 데미지를 받을수 있는 소환체가 있으면
				__int64 i64ShareDmg = iDamage;
				i64ShareDmg *= iShareDmgRate;
				i64ShareDmg /= ABILITY_RATE_VALUE;
				{// 원래 데미지를 차감한후, 알려 줄수 있게 
					iDamage -= static_cast<int>(i64ShareDmg);
					pkAResult->SetValue(iDamage);	// 저장하고
				}
				// 소환체들이 각자 받을 데미지를 계산한 후
				__int64 const i64EachShareDmg = (i64ShareDmg/iDmgShareUnitCnt)*2; // 소환체가 체력이 높아 2배의 데미지를 주는 설정
				if(0 < i64EachShareDmg)
				{
					pkSummonUnit = NULL;
					for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
					{// 각
						pkSummonUnit = pkGround->GetUnit((*c_it).kGuid);
						if( pkSummonUnit 
							&& 0 == pkSummonUnit->GetAbil(AT_CANNOT_DAMAGE)
							&& pkSummonUnit->IsUnitType(UT_SUMMONED)
							)
						{// 소환체들에게 전달하고
							//::DoFinalDamage(pkCaster, pkSummonUnit, static_cast<int>(i64EachShareDmg), 0, pArg, g_kEventView.GetServerElapsedTime()); // 데미지가 안보임 ㅠㅠ
							SkillFuncUtil::OnModifyHP(pkSummonUnit, NULL, 0, -i64EachShareDmg, pkCaster, NULL); // 데미지를 보여주기 위해 이 함수 사용
							pkSummonUnit->AddEffect(SHARE_DMG_WITH_SUMMONED_EFFECT_NO, 0, pArg, pkCaster);	// 연출적인 이펙트(EffectNo:1000)를 보여준 후에 
						}
					}
				}
			}
		}
	}
	// 대상에게 나머지 데미지를 전달 한다
	return DoFinalDamage(pkCaster, pkTarget, pkAResult->GetValue(), iSkillNo, pArg, dwTimeStamp);
}

bool DoFinalDamage( CUnit* pkCaster, CUnit* pkTarget, int const iDamage, int const iSkillNo, SActArg const *pArg, DWORD const dwTimeStamp )
{
	if(0 >= iDamage)
	{
		return false;
	}
	bool bTargetDie = false;

	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if( 0 != pkTarget->GetAbil(AT_CANNOT_DAMAGE)) // 타겟이 데미지를 입으면 안되는 상태라면
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(pkCaster && pkCaster->IsUnitType(UT_PLAYER))
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);

		if(pkPlayer)
		{
			{// 딜킹
				pkCaster->SetAbil(AT_ACHIEVEMENT_DEALKING,iDamage);
				PgCheckAchievements<> kCheckAchievements(AT_ACHIEVEMENT_DEALKING, pkPlayer->GroundKey() );
				kCheckAchievements.DoAction(pkPlayer,NULL);
			}

			{// 콤보 업적
				pkCaster->SetAbil(AT_ACHIEVEMENT_COMBO,pkGround->GetCurrentCombo(pkPlayer->GetID()));
				PgCheckAchievements<> kCheckAchievements(AT_ACHIEVEMENT_COMBO, pkPlayer->GroundKey() );
				kCheckAchievements.DoAction(pkPlayer,NULL);
			}
		}
	}

	int const iRemainHP = ::OnDamaged(pkCaster, pkTarget, iSkillNo, iDamage, pkGround, dwTimeStamp);
	if( 0 >= iRemainHP )
	{
		bTargetDie = true;

		if( pkTarget
			&&	pkTarget->IsUnitType(UT_PLAYER) )
		{
			BM::Stream kNfyPacket(PT_U_G_RUN_ACTION);
			kNfyPacket.Push( static_cast< short >(GAN_SumitLog) );
			kNfyPacket.Push( static_cast< int >(LOG_USER_DIE) );
			kNfyPacket.Push( ((pkCaster)? pkCaster->GetID(): BM::GUID::NullData()) );
			pkTarget->VNotify(&kNfyPacket);
		}
	}

	// 때릴 때 마다 특정 확률로 이펙트를 거는 기능 //타겟 or 나 둘다 가능
	if( 0 < iDamage
		&& pkCaster
		)
	{
		{//이펙트에서 데미지 정보를 참고할수 있게 추가하고
			SActArg* pkTempArg = const_cast<SActArg*>(pArg);
			pkTempArg->Set(ACT_ARG_DAMAGE, iDamage);

			// 타격시 이펙트 걸기(몬스터 카드)
			DoFinalDamageFuncSubRoutine::AddEffectWhenCasterHit(pkCaster, pkTarget, pkCaster, pkTempArg, 10, AT_ATTACK_EFFECTNUM01,AT_ATTACK_EFFECTNUM01_RATE,AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME);
			// 사용한 포션에서 타격시 걸어주는 이펙트가 있는가?
			DoFinalDamageFuncSubRoutine::AddEffectWhenCasterHit(pkCaster, pkTarget, pkCaster, pkTempArg, 50, AT_ATTACK_EFFECTNUM01_FROM_POTION,AT_ATTACK_EFFECTNUM01_RATE_FROM_POTION,AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME_FROM_POTION);
			// 장비 아이템에서 타격시 걸어주는 이펙트가 있는가?
			DoFinalDamageFuncSubRoutine::AddEffectWhenCasterHit(pkCaster, pkTarget, pkCaster->GetInven(), pkTempArg, 50, AT_ATTACK_EFFECTNUM01_FROM_EQUIP,AT_ATTACK_EFFECTNUM01_RATE_FROM_EQUIP,AT_ATTACK_EFFECTNUM01_TARGET_TYPE_IS_ME_FROM_EQUIP);
		}

		{// 타격시 HP,MP 회복 어빌이 존재 하는가
			DoFinalDamageFuncSubRoutine::RestoreHPMPWhenCasterHit(pkCaster,pkGround);
		}

		{// 피격시 HP,MP 회복 어빌이 존재 하는가
			DoFinalDamageFuncSubRoutine::RestoreHPMPWhenTargetDamage(pkCaster, pkTarget, pkGround);
		}
	}
	
	return bTargetDie;
}

void DoTickDamage(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, int const iFireEffectNo, int const iAddEffectNo, SActArg const *pArg, bool const bAddEffectImmediately)
{
	if(!pkTarget)
	{
		return;
	}
	if(!pkCaster)
	{
		return;
	}

	int const iCurHP = pkTarget->GetAbil(AT_HP);

	// 실제 데미지를 주고
	::DoFinalDamage(pkCaster, pkTarget, iDamage, 0, pArg, g_kEventView.GetServerElapsedTime());
	if(0 <= iDamage)
	{//반사 스킬 적용 받기 어빌이 존재한다면
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		CEffectDef const* pkDefEffect = kEffectDefMgr.GetDef(iFireEffectNo);
		if(pkDefEffect)
		{
			int const iEffectDmgType = pkDefEffect->GetAbil(AT_EFFECT_DMG_TYPE);
			if(0 < iEffectDmgType)  
			{
				bool const bUseSeed = pkCaster->UseRandomSeedType();
				if(bUseSeed)
				{// 기존의 설정이 시드를 사용하게 되어있으면 시드를 끄고
					pkCaster->UseRandomSeedType(false);
				}

				bool bPhysicDmg = EEDT_MAGIC == iEffectDmgType ? false : true;
				int iReflected = PgSkillHelpFunc::CalcReflectDamage(pkCaster, pkTarget, iDamage, NULL, NULL, bPhysicDmg, true, true);

				if(0 < iReflected)
				{
					int const iCurHP= pkCaster->GetAbil(AT_HP);
					::DoFinalDamage(pkTarget, pkCaster, iReflected, 0, pArg, g_kEventView.GetServerElapsedTime());
					int const iNewHP = pkCaster->GetAbil(AT_HP);
					int const iDelta = iNewHP - iCurHP;
					BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
					kPacket.Push(pkCaster->GetID());
					kPacket.Push((short)AT_HP);
					kPacket.Push(iNewHP);
					kPacket.Push(pkTarget->GetID());
					kPacket.Push(0);
					kPacket.Push(iDelta);
					pkTarget->Send(kPacket, E_SENDTYPE_BROADALL);

					EffectQueueData kData(EQT_ADD_EFFECT, REPLECT_DMG_EFFECTNO, 0, pArg, pkTarget->GetID());
					pkTarget->AddEffectQueue(kData);
				}

				if(bUseSeed)
				{// 기존의 설정이 시드를 사용하게 되어있었다면 끄고 사용했으니 다시 켜준다
					pkCaster->UseRandomSeedType(true);
				}
			}
		}
	}

	if(iAddEffectNo)
	{
		if(bAddEffectImmediately)
		{
			pkTarget->AddEffect(iAddEffectNo, 0, pArg, pkCaster);
		}else
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iAddEffectNo, 0, pArg, pkCaster->GetID());
			pkTarget->AddEffectQueue(kData);
		}
	}	

	// 클라이언트에 데미지를 알려준다.
	int const iNewHP = pkTarget->GetAbil(AT_HP);
	int const iDelta = iNewHP - iCurHP;
	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkTarget->GetID());
	kPacket.Push((short)AT_HP);
	kPacket.Push(iNewHP);
	kPacket.Push(pkCaster->GetID());
	kPacket.Push(iFireEffectNo);
	kPacket.Push(iDelta);
	pkCaster->Send(kPacket, E_SENDTYPE_BROADALL);

}

/*
bool WA_ATTACKDAMAGE_UNIT(int const iMinPow, int const iMaxPow, CUnit* pkMonster, CUnit* pkUnit, PgGround* pkGround, int const iEffectNo, int const iParam, bool const bPushback)
{
	if(pkUnit->GetState() == US_DEAD)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if(pkUnit->GetAbil(AT_HP) <= 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iCritical = BM::Rand_Index(100);
	if(iCritical > 90)
	{
		iCritical = iMinPow;
	}
	else
	{
		iCritical = 0;
	}
	
	int const iRandPow = iMaxPow - iMinPow + 1;
	int const iDamage = iMinPow + BM::Rand_Index(iRandPow) + iCritical;
	int const iRemainHP = ::OnDamaged(pkMonster, pkUnit, 0, iDamage, pkGround, g_kEventView.GetServerElapsedTime());
	BM::GUID emptyGuid = BM::GUID::NullData();
	
	BM::Stream kPacket(PT_M_C_NFY_ATTACKDAMAGE);
	if(!pkMonster)
	{
		kPacket.Push(emptyGuid);
	}
	else
	{
		kPacket.Push(pkMonster->GetID());
	}

	kPacket.Push(pkUnit->GetID());
	kPacket.Push(iDamage);
	kPacket.Push(iRemainHP);
	kPacket.Push(iEffectNo);
	kPacket.Push(iEffectNo);
	kPacket.Push(iParam);
	kPacket.Push(bPushback);
	pkGround->Broadcast(kPacket, pkUnit, E_SENDTYPE_NONE);
	
	return true;
}
*/

void AccumulateDamaged(CUnit * pkUnit, int const iDamage)
{
	if(!pkUnit || 0==iDamage)
	{
		return;
	}

	if(pkUnit->GetAbil(AT_USE_ACCUMULATE_DAMAGED))
	{
		pkUnit->AddAbil(AT_ACCUMULATE_DAMAGED_NOW, iDamage);
		pkUnit->SendAbil(AT_ACCUMULATE_DAMAGED_NOW, E_SENDTYPE_BROADALL);
	}
}

int OnDamaged(CUnit* pkCaster, CUnit* pkTarget, int const iSkill, int iPower, PgGround* pkGround, DWORD const dwTimeStamp, bool const bReflectDamage)
{
	if( !pkTarget)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"pkTarget is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	BM::GUID kTargetGuid;
	if(pkCaster)
	{
		if(pkCaster->IsUnitType(UT_SUMMONED))
		{
			kTargetGuid = pkCaster->GetID();
		}
		else
		{
			kTargetGuid = pkCaster->Caller();
		}
	}

	{
		int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
		bool const bBlock = iRandValue < pkTarget->GetAbil(AT_100PERECNT_BLOCK_RATE); // 절대 Block Rate 계산

		// '헛침' 처리
		if( 0 == iPower 
			|| bBlock
			)
		{
			if(pkCaster)
			{
				pkTarget->SetTarget(kTargetGuid);
				if(pkTarget->GetState() == US_SKILL_CAST)
				{
					pkTarget->SetDelay((int)(pkTarget->GetDelay() * 0.5f));
				}
			}
			if(bBlock)
			{
				SActArg kArg;
				PgGroundUtil::SetActArgGround(kArg, pkGround);
				pkTarget->AddEffect(BLOCK_RATE_EFFECT_NO, 0, &kArg, pkTarget);
			}
			return pkTarget->GetAbil(AT_HP);
		}
	}
	
	int iComitedDamage = iPower;
	OnCustomTargetDamage(iComitedDamage, pkTarget, iSkill);
	int iHP = 0;
	if ( false == pkTarget->OnDamage( iComitedDamage, iHP ) )
	{
		return iHP;
	}

	if( pkTarget->GetAbil(AT_UNIT_HIDDEN) > 0 )//숨어있다가 맞거나/때리면 Hidden 속성이 사라짐
	{
		pkTarget->SetAbil(AT_UNIT_HIDDEN, 0);
	} 
	
	int iValue = pkTarget->GetAbil(AT_FROZEN);//Froze 상태에서 맞으면 풀린다
	if( iValue > 0 )
	{
		if (pkTarget->GetAbil(AT_FROZEN_DMG_WAKE) > 0)
		{
			pkTarget->SetAbil(AT_FROZEN, 0);
		}
	}

	if(pkCaster)
	{
		int iEnableCheckAttack = pkCaster->GetAbil(AT_ENABLE_CHECK_ATTACK);
		if(0 < iEnableCheckAttack)
		{
			pkCaster->AddAbil(AT_CHECK_ATTACK_COUNT, pkCaster->GetAbil(AT_CHECK_ATTACK_COUNT) + 1);
		}

		//{//데미지로 인해 힐을 해주는 기능이 발동가능한지 체크하고
		//	int const iActiveRate = pkCaster->GetAbil(AT_ACTIVE_RATE_HP_RESTORE_FROM_DAMAGE);
		//	int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
		//	if(iActiveRate
		//		&& iActiveRate >= iRandValue
		//		)
		//	{// 발동 가능하면 힐량을 계산한 후
		//		int const iHealRate = pkCaster->GetAbil(AT_R_HP_RESTORE_FROM_DAMAGE);
		//		int const iAddHP = (iComitedDamage*iHealRate)/ABILITY_RATE_VALUE;
		//		if(0 < iAddHP)
		//		{// 힐이 가능하면, 힐을 해준다
		//			int const iMaxHP = pkCaster->GetAbil(AT_C_MAX_HP);
		//			int const iNowHP = pkCaster->GetAbil(AT_HP);
		//			if(iMaxHP > iNowHP)
		//			{
		//				OnSetAbil(pkCaster, AT_HP, __min(iMaxHP, iNowHP + iAddHP));
		//			}
		//		}
		//	}
		//}
	}

	int const iShareRelation = pkTarget->GetAbil(AT_SHARE_HP_RELATIONSHIP);
	CUnit * pkSendShareHpChildren = NULL;
	if(ESHR_PARENT==iShareRelation && 0<pkTarget->GetSummonUnitCount())
	{
		//데미지를 입히고 데미지 정보를 자식들에게 통지(함수 마지막에)
		pkSendShareHpChildren = pkTarget;
	}
	else if(ESHR_CHILDREN==iShareRelation && pkTarget->Caller().IsNotNull())
	{
		//데미지를 부모에게 올림
		CUnit * pkParent = pkGround->GetUnit(pkTarget->Caller());
		return OnDamaged(pkCaster, pkParent, iSkill, iPower, pkGround, dwTimeStamp, bReflectDamage);
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkill);

	if( pkCaster
	&&	pkSkillDef )
	{
		BM::GUID kCasterID = pkCaster->GetID();
		if(pkCaster->GetAbil(AT_CALLER_IS_PET))
		{
			PgPlayer * pkParent = dynamic_cast<PgPlayer*>(pkGround->GetUnit(pkCaster->Caller()));
			if(pkParent)
			{
				if(pkParent->SelectedPetID().IsNotNull())
				{
					kCasterID = pkParent->SelectedPetID();
				}
			}
		}

		DWORD const dwComboDelay = static_cast<DWORD>(pkSkillDef->GetAbil(AT_COMBO_DELAY));
		//어그로 미터기 계산
		static int const iEnemyType = UT_MONSTER| UT_BOSSMONSTER| UT_OBJECT;
		static int const iPlayerType = UT_PLAYER| UT_ENTITY| UT_PET| UT_SUMMONED | UT_SUB_PLAYER;
		int const iComboCount = pkSkillDef->GetAbil(AT_COMBO_HIT_COUNT);
		if( pkTarget->IsInUnitType((EUnitType)iEnemyType)
		&&	pkCaster->IsInUnitType((EUnitType)iPlayerType) )//플레이어(소환물)가 몬스터를 치면
		{
			BM::Stream kNfyPacket(PT_U_G_NFY_ADD_AGGRO_METER, (BYTE)AMT_Damage);
			kNfyPacket.Push(dwTimeStamp);
			kNfyPacket.Push(pkTarget->GetID());
			kNfyPacket.Push(kCasterID);
			kNfyPacket.Push(iComitedDamage);
			kNfyPacket.Push(iComboCount);
			kNfyPacket.Push(pkSkillDef->No());
			kNfyPacket.Push(dwComboDelay);
			pkTarget->VNotify(&kNfyPacket);

			// BackAttack
			if((pkGround->GetAttr() & GATTR_MISSION) == GATTR_MISSION)
			{
				if( true == BackAttackCheck(pkCaster, pkTarget, pkGround) )
				{
					BM::GUID const& rkCasterGuid = (pkCaster->IsUnitType(UT_SUMMONED) || pkCaster->IsUnitType(UT_SUB_PLAYER)) ? pkCaster->Caller() : kCasterID;
					BM::Stream kNfyGndPacket(PT_U_G_NFY_ADD_MISSION_SCORE);
					kNfyGndPacket.Push( EMission_BACKATTACK );
					kNfyGndPacket.Push( rkCasterGuid );
					kNfyGndPacket.Push( pkTarget->GetID() );
					pkCaster->VNotify(&kNfyGndPacket);
				}
			}
			else if ((pkGround->GetAttr() & GATTR_BATTLESQUARE) == GATTR_BATTLESQUARE)
			{
				// Battlesquare 이면 몬스터를 때릴때 마다 점수를 줘야 한다.
				if( PgGroundUtil::IsHaveFreePVPAttr(pkGround->DynamicGndAttr()) )
				{
					BM::Stream kSPacket(PT_U_G_NFY_ATTACKED_MONSTER, pkCaster->Caller());
					pkCaster->VNotify(&kSPacket);
				}
			}
		}
		else if( pkTarget->IsUnitType(UT_PLAYER)
		&&	pkCaster->IsInUnitType((EUnitType)iEnemyType) )//몬스터가 플레이어를 치면
		{
			BM::Stream kNfyPacket(PT_U_G_NFY_ADD_AGGRO_METER, (BYTE)AMT_Hit);
			kNfyPacket.Push(dwTimeStamp);
			kNfyPacket.Push(pkCaster->GetID());
			kNfyPacket.Push(pkTarget->GetID());
			kNfyPacket.Push(iComitedDamage);
			kNfyPacket.Push(iComboCount);
			kNfyPacket.Push(pkSkillDef->No());
			kNfyPacket.Push(dwComboDelay);
			pkTarget->VNotify(&kNfyPacket);

			if((pkGround->GetAttr() & GATTR_MISSION) == GATTR_MISSION)
			{
				pkTarget->AddAbil(AT_ACHIEVEMENT_MISSION_HIT,1);
			}
		}
		//else if( pkTarget->IsUnitType(UT_SUMMONED)
		//&&	pkCaster->IsInUnitType((EUnitType)iEnemyType) )//몬스터가 소환체를 치면
		//{
		//	BM::Stream kNfyPacket(PT_U_G_NFY_ADD_AGGRO_METER, (BYTE)AMT_Hit);
		//	kNfyPacket.Push(dwTimeStamp);
		//	kNfyPacket.Push(pkCaster->GetID());
		//	kNfyPacket.Push(pkTarget->GetID());//소환체 GUID를 주면 PT_U_G_NFY_ADD_AGGRO_METER에서 변환해서 플레이어에게 적용함
		//	kNfyPacket.Push(iComitedDamage);
		//	kNfyPacket.Push(iComboCount);
		//	kNfyPacket.Push(pkSkillDef->No());
		//	kNfyPacket.Push(dwComboDelay);
		//	pkTarget->VNotify(&kNfyPacket);
		//}

		if( 0 >= iHP || (ON_DIE_SKILL_HP==iHP && pkTarget->GetAbil(AT_SKILL_ON_DIE)))//나를 죽인 범인을 저장한다.
		{
			if(pkTarget->IsUnitType(UT_PLAYER))
			{
				if(pkCaster->IsInUnitType((EUnitType)iEnemyType))//몬스터가 플레이어를 죽이면
				{
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_ZOMBI,1,pkGround->GroundKey());
					kMA.DoAction(pkTarget,NULL);

					if((pkGround->GetAttr() & GATTR_MISSION) == GATTR_MISSION)
					{
						pkTarget->AddAbil(AT_ACHIEVEMENT_MISSION_DEAD,1);
					}
					else if( pkGround->GetAttr() & GATTR_INSTANCE )
					{
						pkGround->IndunPartyDie(true);
					}
				}
				else if( pkCaster->IsUnitType(UT_PLAYER))//플레이어가 플레이어를 죽일 경우
				{
					if((pkGround->GetAttr() & GATTR_PVP) == GATTR_PVP)
					{
						pkCaster->AddAbil(AT_ACHIEVEMENT_PVP_KILL,1);
						pkTarget->AddAbil(AT_ACHIEVEMENT_PVP_DEAD,1);
	/*
						pkTarget->SetAbil(AT_ACHIEVEMENT_PVP_SLAYER,pkTarget->GetAbil(AT_ACHIEVEMENT_PVP_KILL));
						PgCheckAchievements<> kCA(AT_ACHIEVEMENT_PVP_SLAYER,pkGround->GroundKey());
						kCA.DoAction(pkTarget,NULL);
	*/
					}
				}
				else if( pkTarget->IsInUnitType((EUnitType)iEnemyType) )
				{
					if((pkGround->GetAttr() & GATTR_MISSION) == GATTR_MISSION)
					{
						int iDiffLevel = abs( (pkCaster->GetAbil(AT_LEVEL) - pkTarget->GetAbil(AT_LEVEL)) );
						if( iDiffLevel < MISSION_OVERHITLEVEL )
						{
							if( (iPower-iComitedDamage) >= ((iPower*MISSION_OVERHIT)/100) )
							{
								BM::GUID OwnerGuid;
								if( pkCaster->IsUnitType(UT_SUB_PLAYER) )
								{
									OwnerGuid = pkCaster->Caller();
								}
								else
								{
									OwnerGuid = pkCaster->GetID();
								}
								BM::Stream kNfyGndPacket(PT_U_G_NFY_ADD_MISSION_SCORE);
								kNfyGndPacket.Push( EMission_OVERHIT );
								kNfyGndPacket.Push( OwnerGuid );
								kNfyGndPacket.Push( pkTarget->GetID() );
								pkCaster->VNotify(&kNfyGndPacket);
							}
						}
					}
				}
				if(pkTarget->GetAbil(AT_DUEL) > 0 && pkCaster->GetAbil(AT_DUEL) > 0 && pkTarget->GetAbil(AT_DUEL) == pkCaster->GetAbil(AT_DUEL)) //마을/필드 결투 중일 때 결투중인 상대에게 죽었다면
				{
					EUnitType eUnitType = pkCaster->UnitType();
					pkTarget->SetAbil(AT_DUEL_DIED_BY, eUnitType); //죽인 놈의 타입을 저장 (UT_PLAYER에게 죽었다면, 결투 패배 또는 무승부)
					//pkTarget->SetAbil(AT_HP, 1);
					iHP = 1;
					pkTarget->SetAbil(AT_DUEL, 0, true, true); //결투를 끝내도록 예약

					SActArg kArg;
					PgGroundUtil::SetActArgGround(kArg, pkGround);
					pkTarget->AddEffect(EFFECTNO_CANNOT_DAMAGE, 0, &kArg, pkTarget);	// 결투 직후 두PC에게 일시적으로 무적 버프를 걸어줌
					pkCaster->AddEffect(EFFECTNO_CANNOT_DAMAGE, 0, &kArg, pkCaster);

				}
			}


			BM::GUID kCasterGuid = BM::GUID::NullData();
			bool bStyle = false;
			int const iStyleUnitType = UT_ENTITY|UT_SUMMONED|UT_SUB_PLAYER;
			// Entity or Summoned
			if( pkCaster->IsInUnitType((EUnitType)iStyleUnitType) && pkTarget->IsInUnitType((EUnitType)iEnemyType) )
			{
				if( pkCaster->GetID() != pkCaster->Caller() )
				{
					PgPlayer *pkCasterPlayer = dynamic_cast<PgPlayer *>(pkGround->GetUnit( pkCaster->Caller() ));
					if( pkCasterPlayer )
					{
						int iDiffLevel = abs( (pkCasterPlayer->GetAbil(AT_LEVEL) - pkTarget->GetAbil(AT_LEVEL)) );
						if( iDiffLevel < MISSION_STYLELEVEL )
						{
							kCasterGuid = pkCaster->Caller();
							bStyle = true;
						}
					}
				}
			}
			else
			{
				if( pkCaster->IsUnitType(UT_PLAYER) && pkTarget->IsInUnitType((EUnitType)iEnemyType) )
				{
					if( pkSkillDef )
					{
						if( pkSkillDef->GetAbil(AT_SKILL_KIND) != ESK_NONE )
						{
							int iDiffLevel = abs( (pkCaster->GetAbil(AT_LEVEL) - pkTarget->GetAbil(AT_LEVEL)) );
							if( iDiffLevel < MISSION_STYLELEVEL )
							{
								kCasterGuid = pkCaster->GetID();
								bStyle = true;
							}
						}
					}
				}
			}
			if( true == bStyle )
			{
				if((pkGround->GetAttr() & GATTR_MISSION) == GATTR_MISSION)
				{

					BM::Stream kNfyGndPacket(PT_U_G_NFY_ADD_MISSION_SCORE);
					kNfyGndPacket.Push( EMission_STYLE );
					kNfyGndPacket.Push( kCasterGuid );
					kNfyGndPacket.Push( pkTarget->GetID() );
					pkCaster->VNotify(&kNfyGndPacket);
				}
			}

			pkTarget->SetTarget(kTargetGuid);
			// 혹시 Monster가 DropItem할때, 클라이언트쪽에서 연출을 하려면,
			// ActionInstaceID를 알아야 한다.
			pkTarget->SetActionInstanceID(pkCaster->GetActionInstanceID());
		}
		// Damage 준것을 Target정보에 밀어 준다. --> CS_GetSkillResultDefault(..) 안에서 하는것으로 처리 변경
		//pkCaster->OnTargetDamaged(pkTarget->GetID());
	}

	ALRAM_MISSION::SCheckObejct< __int64 > kAlramCheckObject( ALRAM_MISSION::OBJECT_SKILL, static_cast<__int64>(GET_BASICSKILL_NUM(iSkill) ) );

	if ( pkCaster )
	{
		if( UT_PLAYER == pkCaster->UnitType() )
		{
			ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_HIT_ANYBODY>()( dynamic_cast<PgPlayer*>(pkCaster), 1, pkGround, pkTarget, kAlramCheckObject );
		}
		else
		{
			if( UT_SUMMONED == pkCaster->UnitType() )
			{
				CUnit* Caller = pkGround->GetUnit(pkCaster->Caller());
				ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_HIT_ANYBODY>()( dynamic_cast<PgPlayer*>(Caller), 1, pkGround, pkTarget, kAlramCheckObject );
			}
		}
	}

	// target null 체크는 맨위에서 했음
	if ( UT_PLAYER == pkTarget->UnitType() )
	{
		ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_STRUCT>()( dynamic_cast<PgPlayer*>(pkTarget), 1, pkGround, pkCaster, kAlramCheckObject );
	}

	//소생
	if( (iHP <= 0) && (0 < pkTarget->GetAbil(AT_REVIVE_COUNT)) )
	{
		pkTarget->GetEffectMgr().AddAbil(AT_REVIVE_COUNT, -1);
		pkTarget->NftChangedAbil(AT_REVIVE_COUNT, E_SENDTYPE_EFFECTABIL);

		if(int const iEffectNo = pkTarget->GetAbil(AT_REVIVE_EFFECT))
		{
			SActArg kActArg;
			PgGroundUtil::SetActArgGround(kActArg, pkGround);
			pkTarget->AddEffect(iEffectNo, 0, &kActArg, pkTarget);
		}

		iHP = 1;
	}

	if( iHP > 0 )
	{
		// 유닛이 Damage Action을 안하고 싶을 경우(보스)
		// 어떤 스킬에 한해서 Damage Action을 안하고 싶은 경우(궁수, 법사 기본 스킬)
		// 반사데미지 맞았다고 딜레이 주면 안됨
		if (!bReflectDamage && pkTarget->IsDamageAction())	// Damage액션을 해야 한다.
		{
			SetDamageDelay(pkSkillDef, pkCaster, pkTarget);
		}
		else
		{
			SUnit_AI_Info* pkAI = pkTarget->GetAI();
			if( pkAI
				&& pkAI->eCurrentAction != EAI_ACTION_FIRESKILL 
				&& pkAI->eCurrentAction != EAI_ACTION_CHASEENEMY
				&& pkAI->eCurrentAction != EAI_ACTION_BLOWUP)
			{
				pkAI->SetEvent(kTargetGuid, EAI_EVENT_CHASE);
			}
		}
	}
	else
	{
		if(int const iSkillNo = pkTarget->GetAbil(AT_SKILL_ON_DIE) )
		{
			iHP = ON_DIE_SKILL_HP;
			PgSkillHelpFunc::SkillOnDie(pkTarget, iSkillNo, false);
		}
		else
		{
			if ( !pkCaster )
			{
				// Caster가 없으면 자살이다.
				// Target을 클리어 한다.
				if ( pkTarget->IsUnitType(UT_PLAYER) )
				{
					pkTarget->SetTarget( BM::GUID::NullData() );
				}
			}
		}

		//소환체 제거
		PgSummoned * pkSummoned = NULL;
		VEC_SUMMONUNIT const& kContSummonUnit = pkTarget->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			if(pkSummoned = dynamic_cast<PgSummoned*>(pkGround->GetUnit((*c_it).kGuid)))	//No block
			if(false == pkSummoned->IsNPC())
			{
				pkSummoned->SetAbil(AT_VOLUNTARILY_DIE, 1, true, true);
				OnDamaged(pkTarget, pkSummoned, 0, pkSummoned->GetAbil(AT_HP), pkGround, dwTimeStamp, bReflectDamage);
			}
		}

		if(false == bReflectDamage)
		{
			if( pkCaster )
			{
				int iHealValue = pkCaster->GetAbil(AT_KILL_HEAL_HP);

				if(0 != iHealValue)
				{
					int iCasterHPHealRate = pkCaster->GetAbil(AT_KILL_HEAL_HP_RATE);

					if(0 == iCasterHPHealRate)
					{
						iCasterHPHealRate = ABILITY_RATE_VALUE;
					}

					// 적을 죽일 때 일정 HP를 회복 하는 어빌이 세팅되어 있는 경우
					int const iRate = BM::Rand_Range(ABILITY_RATE_VALUE);
					if(iRate <= iCasterHPHealRate)
					{
						pkCaster->SetAbil( AT_HP, __min(pkCaster->GetAbil(AT_HP) + iHealValue, pkCaster->GetAbil(AT_C_MAX_HP)), true, true);
					}
				}

				iHealValue = pkCaster->GetAbil(AT_KILL_HEAL_MP);

				if(0 != iHealValue)
				{
					int iCasterMPHealRate = pkCaster->GetAbil(AT_KILL_HEAL_MP_RATE);

					if(0 == iCasterMPHealRate)
					{
						iCasterMPHealRate = ABILITY_RATE_VALUE;
					}

					// 적을 죽일 때 일정 MP를 회복 하는 어빌이 세팅되어 있는 경우
					int const iRate = BM::Rand_Range(ABILITY_RATE_VALUE);
					if(iRate <= iCasterMPHealRate)
					{
						pkCaster->SetAbil( AT_MP, __min(pkCaster->GetAbil(AT_MP) + iHealValue, pkCaster->GetAbil(AT_C_MAX_MP)), true, true);
					}
				}
			}
		}
	}

	int const iDamage = pkTarget->GetAbil(AT_HP)-iHP;
	g_kElitePatternMng.OnDamaged(pkTarget, iDamage, pkSkillDef);
	AccumulateDamaged(pkTarget, iDamage);

	bool const bSend = (0<iHP) || pkTarget->IsUnitType(UT_PLAYER);
	pkTarget->SetAbil( AT_HP, iHP, bSend, bSend&&pkCaster==NULL );

	if(false == bReflectDamage)
	{
		if( pkCaster )
		{
			int iHealValue = pkCaster->GetAbil(AT_HIT_HEAL_HP);

			if(0 < iHealValue)
			{
				int iCasterHPHealRate = pkCaster->GetAbil(AT_HIT_HEAL_HP_RATE);

				if(0 == iCasterHPHealRate)
				{
					iCasterHPHealRate = ABILITY_RATE_VALUE;
				}

				// 타격시 일정 HP를 회복 하는 어빌이 세팅되어 있는 경우
				int iRate = BM::Rand_Range(ABILITY_RATE_VALUE);
				if(iRate <= iCasterHPHealRate)
				{				
					pkCaster->SetAbil( AT_HP, __min(pkCaster->GetAbil(AT_HP) + iHealValue, pkCaster->GetAbil(AT_C_MAX_HP)), true, true);

				}
			}

			iHealValue = pkCaster->GetAbil(AT_HIT_HEAL_MP);

			if(0 < iHealValue)
			{
				int iCasterMPHealRate = pkCaster->GetAbil(AT_HIT_HEAL_MP_RATE);

				if(0 == iCasterMPHealRate)
				{
					iCasterMPHealRate = ABILITY_RATE_VALUE;
				}
				// 타격시 일정 MP를 회복 하는 어빌이 세팅되어 있는 경우
				int iRate = BM::Rand_Range(ABILITY_RATE_VALUE);
				if(iRate <= iCasterMPHealRate)
				{
					pkCaster->SetAbil( AT_MP, __min(pkCaster->GetAbil(AT_MP) + iHealValue, pkCaster->GetAbil(AT_C_MAX_MP)), true, true);
				}
			}
		}
	}

	if(pkSendShareHpChildren)
	{//공유HP를 사용하는 경우 부모가 자식에게 통지
		int const iMaxUnit = pkSendShareHpChildren->GetSummonUnitCount();
		for(int i=0; i<iMaxUnit;++i)
		{
			BM::GUID const & kChildID = pkSendShareHpChildren->GetSummonUnit(i);
			CUnit * pkChild = pkGround->GetUnit(kChildID);
			if(pkChild)
			{
				pkChild->SetAbil(AT_HP, iHP);
			}
		}
	}

	PgPlayer* pkTargetPlayer = dynamic_cast<PgPlayer*>(pkTarget);
	if(pkTargetPlayer)
	{
		if(pkTargetPlayer->OpenVendor() )
		{//타겟이 노점주인이라면 닫아버리자.
			pkGround->RecvVendorClose(pkTarget);
		}
		else
		{
			if( 0 >= iHP && pkTargetPlayer->VendorGuid() != BM::GUID::NullData())
			{//손님일때 죽어버리면 닫아버리자
				pkGround->RecvVendorClose(pkTarget);
			}
		}
	}

	pkTarget->OnDamageDelEffect();

	if( NULL != pkCaster )
	{
		if( 0 != GATTR_EXPEDITION_GROUND & pkGround->GetAttr() )	// 현재 원정대 맵이고.
		{
			switch( pkCaster->UnitType() )
			{
			case UT_PLAYER:
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkCaster);
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() )	// 원정대에 속해 있을 때만.
						{
							pPlayer->AccumulateDamage(iComitedDamage);		// 데미지 누적.
						}
					}
				}break;
			case UT_ENTITY:
			case UT_PET:
			case UT_SUMMONED:
			case UT_SUB_PLAYER:
				{
					BM::GUID OwnerGuid = pkCaster->Caller();
					PgPlayer * pPlayer = pkGround->GetUser(OwnerGuid);
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() && ( pPlayer->Gattr() & GKIND_EXPEDITION) )	// 원정대에 속해 있을 때만.
						{
							pPlayer->AccumulateDamage(iComitedDamage);		// 데미지 누적.
						}
					}
				}break;
			}
		}
	}

	return iHP;
}

HRESULT CheckClientNotifyTime(PgPlayer* pkPlayer, DWORD const dwClientTime, DWORD& dwServerTime)
{
	dwServerTime = g_kEventView.GetServerElapsedTime();

	if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	{
		return S_OK;
	}

	if (PgGround::ms_kAntiHackCheckVariable.bUse_GameTimeOver)
	{
		if ( pkPlayer->NotifyTime() >= dwClientTime)
		{
			BM::vstring kLogMsg;
			kLogMsg << __FL__ << _T("[HACKING][GameTimeOver] MemberID=") << pkPlayer->MemberID() << _T(", MemberGuid[") 
				<< pkPlayer->GetMemberGUID() << _T("], User[") << pkPlayer->Name() << _T("] GUID[") << pkPlayer->GetID()
				<< _T(" PlayerLastTime[") << pkPlayer->NotifyTime() << _T("], ClientTime[") << dwClientTime << _T("]");
			// 이전에 보냈던 시간보다 작거가 같은 시간을 보낼 수 없다.
			//VERIFY_INFO_LOG(false, BM::LOG_LV5, kLogMsg);
			HACKING_LOG(BM::LOG_LV5, kLogMsg);

			if (g_kProcessCfg.RunMode() | CProcessConfig::E_RunMode_Debug)
			{
				pkPlayer->SendWarnMessageStr(kLogMsg);
			}

			bool bHack = pkPlayer->SuspectHacking(EAHP_CheckGameTimeOver, PgGround::ms_kAntiHackCheckVariable.sGameTimeOver_AddIndex);
			return (bHack ? E_FAIL : E_RETURN_REJECT);// 이건 짜른다.
		}
	}

	pkPlayer->NotifyTime(dwClientTime);

	if (PgGround::ms_kAntiHackCheckVariable.bUse_GameTimeUnder)
	{
		if( abs(int(dwServerTime - dwClientTime)) > MAX_NETWORKDELAY )
		{
			BM::vstring kLogMsg;
			kLogMsg << __FL__ << _T("[HACKING][GameTimeUnder] User[") << pkPlayer->Name() << _T("] GUID[") 
				<< pkPlayer->GetID() << _T("]  MemberGUID[") << pkPlayer->GetMemberGUID() << _T("] HackingType[SpeedHack] Client NotifyTime is doubtful of SpeedHack ClientTime[") 
				<< dwClientTime << _T("], ServerTime[") << dwServerTime << _T("]");
			//VERIFY_INFO_LOG(false, BM::LOG_LV0, kLogMsg);
			HACKING_LOG(BM::LOG_LV0, kLogMsg);

			if (g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug)
			{
				pkPlayer->SendWarnMessageStr(kLogMsg);
			}

			bool bHack = pkPlayer->SuspectHacking(EAHP_CheckGameTimeUnder, PgGround::ms_kAntiHackCheckVariable.sGameTimeUnder_AddIndex);
			return (bHack ? E_FAIL : E_RETURN_REJECT);// 이건 짜른다.
		}
	}

	return S_OK;
}

int CalcComboBonusRate(unsigned int const iCombo)
{
	/*
	size_t const iMinTable = 0;
	size_t const iMaxTable = 20;		//	0		1	2		3	4		5	6		7	8		9	10
	static float const iComboRateTable[] = {1.f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 1.5f, 1.55f, 1.6f, 1.65f, 1.7f, 1.75f, 1.8f, 1.85f, 1.9f, 1.95f, 2.0f};

	if( iCombo > 200 )
	{
		return iComboRateTable[iMaxTable];
	}

	size_t const iCur = iCombo / 10;

	return iComboRateTable[iCur];
	*/
	unsigned int const iMaxCombo = 100;
	unsigned int const iNewCombo = std::min(iCombo, iMaxCombo);
	// 2009.12.08 : 경험치 공식 변경에 따라 변경
	//return 1.0f + (iNewCombo / 20) * 0.1f;
	return (iNewCombo / 20) * 10;
}

namespace GuildUtil
{
	static int iCanCreateGuildLevel = 10; // 길드 생성 레벨

	bool InitConstantValue()
	{
		if( S_OK != g_kVariableContainer.Get(EVar_Kind_Guild, EVar_Guild_CanCreateLevel, iCanCreateGuildLevel) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Get constant value [EVar_Kind_Guild][EVar_Guild_CanCreateLevel]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		return true;
	}

	__int64 GetHowMuchCreateGuild()
	{
		static __int64 iBaseLevelGuildCost = 0;
		if( !iBaseLevelGuildCost )
		{
			__int64 const iErrorGuildCost = 1000000000000i64;
			CONT_DEF_GUILD_LEVEL const *pkDefGuildLevel = NULL;
			g_kTblDataMgr.GetContDef(pkDefGuildLevel);
			if( !pkDefGuildLevel )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Can't get guild Def");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return iErrorGuildCost"));
				return iErrorGuildCost;
			}

			short const sBaseGuildLevel = 1;
			CONT_DEF_GUILD_LEVEL::const_iterator def_iter = pkDefGuildLevel->find(sBaseGuildLevel);
			if( pkDefGuildLevel->end() == def_iter )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Can't find base guild level");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return iErrorGuildCost"));
				return iErrorGuildCost;
			}

			iBaseLevelGuildCost = (*def_iter).second.iGold;
			if( !iBaseLevelGuildCost )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Guild Create[Lv.1] Cost is 0");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return iErrorGuildCost"));
				return iErrorGuildCost;
			}
		}
		return iBaseLevelGuildCost;
	}

	EGuildCommandRet CheckCreateGuild(CUnit* pkCaster)
	{
		if( !pkCaster )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
			return GCR_None;
		}

		PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
		if( !pkPC )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_None"));
			return GCR_None;
		}

		if( iCanCreateGuildLevel > pkPC->GetAbil(AT_LEVEL) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Level"));
			return GCR_Level;
		}

		if( GetHowMuchCreateGuild() > pkPC->GetAbil64(AT_MONEY) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return GCR_Money"));
			return GCR_Money;
		}

		return GCR_Success;
	}
};

void CheckDayLoopQuestTime(SGroundKey const &rkGndKey, PgPlayer *pkPlayer, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime)
{
	if( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
		return;
	}

	PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
	if( !pkMyQuest )
	{
		return;
	}

	BM::DBTIMESTAMP_EX kNowTime;
	kNowTime.SetLocalTime();

	CONT_PLAYER_MODIFY_ORDER kOrder;

	bool bNeedRefreshQuest = PgQuestInfoUtil::NeedRefreshDayQuest(*pkMyQuest, rkFailPrevTime, rkNextTime);
	ContUserQuestState kVec;
	pkMyQuest->GetQuestList(kVec);
	ContUserQuestState::const_iterator quest_iter = kVec.begin();
	while( kVec.end() != quest_iter )
	{
		ContUserQuestState::value_type const &rkElement = (*quest_iter);
		if( QS_Failed != rkElement.byQuestState )
		{
			PgQuestInfo const *pkQuestInfo = NULL;
			if( g_kQuestMan.GetQuest(rkElement.iQuestID, pkQuestInfo) )
			{
				bool bCanGo = true;
				if( PgQuestInfoUtil::IsDailySystemQuest(pkQuestInfo) )
				{
					BM::PgPackedTime const &rkSaveTime = rkElement.kTime;
					bCanGo = rkFailPrevTime < rkSaveTime && rkSaveTime < rkNextTime;
				}
				else
				{
					bCanGo = pkQuestInfo->m_kLimit.CheckTime();
				}
				
				if( 0 != pkQuestInfo->m_kDepend_Time )
				{
					BM::DBTIMESTAMP_EX const kStartTime(rkElement.kTime.operator DBTIMESTAMP());
					__int64 const iElapsedTime = CGameTime::GetElapsedTime(kStartTime, kNowTime, CGameTime::MINUTE);
					if( 0 > (pkQuestInfo->m_kDepend_Time + iElapsedTime) )
					{
						bCanGo = false;
					}
				}
				
				if( !bCanGo )
				{
					SPMOD_AddIngQuest kAddQuestData(rkElement.iQuestID, QS_Failed); // 실패 처리
					kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );

					if( (QT_Couple == pkQuestInfo->Type()) || (QT_SweetHeart == pkQuestInfo->Type()) )
					{
						SPMOD_AddIngQuest kAddQuestData(rkElement.iQuestID, QS_None); // 포기 처리
						kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );
					}
				}
			}
		}
		++quest_iter;
	}

	if( bNeedRefreshQuest )
	{
		//kOrder.push_back( SPMO(IMET_INIT_WANTED_QUEST, pkPlayer->GetID()) );	// 현상수배 퀘스트 초기화(클리어카운트)
		kOrder.push_back( SPMO(IMET_BUILD_DAYLOOP, pkPlayer->GetID()) );
	}

	if( !kOrder.empty() )
	{
		PgAction_ReqModifyItem kCreateAction(IMEPT_QUEST, rkGndKey, kOrder);
		kCreateAction.DoAction(pkPlayer, NULL);
	}
}

void CheckWeekLoopQuestTime(SGroundKey const &rkGndKey, PgPlayer *pkPlayer, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime)
{
	if( !pkPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
		return;
	}

	PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
	if( !pkMyQuest )
	{
		return;
	}

	BM::DBTIMESTAMP_EX kNowTime;
	kNowTime.SetLocalTime();

	CONT_PLAYER_MODIFY_ORDER kOrder;

	bool bNeedRefreshQuest = PgQuestInfoUtil::NeedRefreshDayQuest(*pkMyQuest, rkFailPrevTime, rkNextTime);
	ContUserQuestState kVec;
	pkMyQuest->GetQuestList(kVec);
	ContUserQuestState::const_iterator quest_iter = kVec.begin();
	while( kVec.end() != quest_iter )
	{
		ContUserQuestState::value_type const &rkElement = (*quest_iter);
		if( QS_Failed != rkElement.byQuestState )
		{
			PgQuestInfo const *pkQuestInfo = NULL;
			if( g_kQuestMan.GetQuest(rkElement.iQuestID, pkQuestInfo) )
			{
				bool bCanGo = true;
				if( PgQuestInfoUtil::IsWeeklySystemQuest(pkQuestInfo) )
				{
					BM::PgPackedTime const &rkSaveTime = rkElement.kTime;
					bCanGo = rkFailPrevTime < rkSaveTime && rkSaveTime < rkNextTime;

					if( 0 != pkQuestInfo->m_kDepend_Time )
					{
						BM::DBTIMESTAMP_EX const kStartTime(rkElement.kTime.operator DBTIMESTAMP());
						__int64 const iElapsedTime = CGameTime::GetElapsedTime(kStartTime, kNowTime, CGameTime::MINUTE);
						if( 0 > (pkQuestInfo->m_kDepend_Time + iElapsedTime) )
						{
							bCanGo = false;
						}
					}
					
					if( !bCanGo )
					{
						SPMOD_AddIngQuest kAddQuestData(rkElement.iQuestID, QS_Failed); // 실패 처리
						kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );

						if( (QT_Couple == pkQuestInfo->Type()) || (QT_SweetHeart == pkQuestInfo->Type()) )
						{
							SPMOD_AddIngQuest kAddQuestData(rkElement.iQuestID, QS_None); // 포기 처리
							kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPlayer->GetID(), kAddQuestData) );
						}
					}
				}
			}
		}
		++quest_iter;
	}

	if( bNeedRefreshQuest )
	{
		kOrder.push_back( SPMO(IMET_BUILD_WEEKLOOP, pkPlayer->GetID()) );
	}

	if( !kOrder.empty() )
	{
		PgAction_ReqModifyItem kCreateAction(IMEPT_QUEST, rkGndKey, kOrder);
		kCreateAction.DoAction(pkPlayer, NULL);
	}
}

CUnit* CheckRecvActionPacket( CUnit* pkReqUnit, SActionInfo const &kActionInfo, PgGround *pkGnd, BM::Stream::DEF_STREAM_TYPE const kPacketType )
{
	DWORD dwServerTime = 0;
	PgPlayer *pkReqPlayer= dynamic_cast<PgPlayer*>(pkReqUnit);
	if ( !pkReqPlayer )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __F_P2__(kPacketType) << _T("Not Player<") << pkReqUnit->GetID() << _T(">") );
		return NULL;
	}

	if ( E_FAIL == CheckClientNotifyTime( pkReqPlayer, kActionInfo.dwTimeStamp, dwServerTime) )
	{
		INFO_LOG( BM::LOG_LV5, __F_P2__(kPacketType) << _T("Player kicked, Cause[CDC_SuspectedSpeedHack] Player<") << pkReqPlayer->Name() << _T("/") << pkReqPlayer->GetID() << _T(">") );
		
		// SpeedHack이 의심스러우니 강제접속 해제 시킨다.
		BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_SuspectedSpeedHack) );
		kDPacket.Push( pkReqPlayer->GetMemberGUID() );
		SendToServer( pkReqPlayer->GetSwitchServer(), kDPacket );
		return NULL;
	}

	CUnit *pkActionUnit = pkReqUnit;
	if ( pkReqUnit->GetID() != kActionInfo.guidPilot )
	{
		pkActionUnit = pkGnd->GetUnit( kActionInfo.guidPilot );
		if ( !pkActionUnit )
		{
			INFO_LOG( BM::LOG_LV5, __F_P2__(kPacketType) << _T("Not Found Action Unit<") << kActionInfo.guidPilot << _T("> Player<") << pkReqUnit->Name() << _T("/") << pkReqUnit->GetID() << _T(">") );
			return NULL;
		}

		if ( pkActionUnit->Caller() != pkReqUnit->GetID() )
		{
			CAUTION_LOG( BM::LOG_LV5, __F_P2__(kPacketType) << _T("HACKING?? NotControlUnit<") << kActionInfo.guidPilot << _T("> Player<") << pkReqUnit->Name() << _T("/") << pkReqUnit->GetID() << _T(">") );
			return NULL;
		}
	}

	if ( true == ::CheckUnitAction( pkActionUnit, kActionInfo.iActionID, pkGnd, kPacketType ) )
	{
		return pkActionUnit;
	}

	return NULL;
}

bool CheckUnitAction( CUnit *pkActionUnit, int const iActionID, PgGround const *pkGnd, BM::Stream::DEF_STREAM_TYPE const kPacketType )
{
	EUnitState const kUnitState = pkActionUnit->GetState();
	switch( kUnitState )
	{
	case US_OPENING:
		{
			pkActionUnit->CUnit::SetState(US_IDLE);// 이제부터 데미지를 받고 액션 패킷을 처리한다.
		}break;
	case US_DEAD:
		{
			if( EFFECTNO_RESURRECTION01 != iActionID )
			{//현재 죽었을때 사용가능한 스킬은 부활로 살아나는 스킬이다.!! 풀지말것(풀려면 다른 방법으로 처리 해주세요)
				if ( pkActionUnit->GetAbil( AT_HP ) > 0 )
				{
					CAUTION_LOG( BM::LOG_LV5, __F_P2__(kPacketType) << _T("Error Unit<") << pkActionUnit->Name() << _T("/") << pkActionUnit->GetID() << _T(">'s HP<") << pkActionUnit->GetAbil( AT_HP ) << _T("> But State is DEAD") );

					// 이런 버그가 일단 죽여야지..
					pkActionUnit->SetAbil( AT_HP, 0, false, false );
				}

				// 이 새끼는 죽었는데도 액션패킷을 보내고 지랄이야...
				// 텔레무브 등시에 처리문제로 발생할 수 있기는 한데.. 한번 보내주고도 계속해서 발생하면 해킹한놈이지 않을까?
				pkActionUnit->SendAbil( AT_HP, E_SENDTYPE_SELF );
				return false;
			}

			PgAddAchievementValue kMA( AT_ACHIEVEMENT_IMMORTALITY, 1, pkGnd->GroundKey() );
			kMA.DoAction( pkActionUnit, NULL );
		}break;
	case US_ACTION_LOCK:
		{
			if ( EFFECTNO_RESURRECTION01 != iActionID )
			{//현재 죽었을때 사용가능한 스킬은 부활로 살아나는 스킬이다.!! 풀지말것(풀려면 다른 방법으로 처리 해주세요)
				return false;
			}

			if ( pkActionUnit->GetAbil(AT_HP) > 0 )
			{
				// 죽지도 않았는데 왜 부활 스킬을 쓰냐?
				CAUTION_LOG( BM::LOG_LV5, __F_P2__(kPacketType) << _T("Error Player<") << pkActionUnit->Name() << _T("/") << pkActionUnit->GetID() << _T(">'s HP<") << pkActionUnit->GetAbil( AT_HP ) << _T("> State<US_ACTION_LOCK> But FireSkill<") << iActionID << _T(">") );
				return false;
			}

			PgAddAchievementValue kMA( AT_ACHIEVEMENT_IMMORTALITY, 1, pkGnd->GroundKey() );
			kMA.DoAction( pkActionUnit, NULL );
		}break;
	}

	return true;
}

float GetMaxActionMoveSpeed(CUnit * const pkUnit, int const iActionID, int const iOldMoveSpeed)
{
//	return 9999999.0f;

	float fMoveSpeed = 0.0f;
	if(0==iActionID)
	{
		fMoveSpeed = 9999999.0f;
	}
	else
	{
		fMoveSpeed =  (iOldMoveSpeed > 0) ? iOldMoveSpeed : static_cast<float>(pkUnit->GetAbil(AT_C_MOVESPEED));
	}

	// 마을인 경우 마을 이동속도가 추가 된다.
	//if(pkGround)
	//{
	//	if(pkGround->GetAttr() & GATTR_VILLAGE)
	//	{
	//		fMoveSpeed += static_cast<float>(pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED));
	//	}
	//}
	
	if(0 < pkUnit->GetAbil(AT_MOVESPEED_SAVED))
	{
		fMoveSpeed = std::max(fMoveSpeed, static_cast<float>(pkUnit->GetAbil(AT_MOVESPEED_SAVED)));
	}

	//if(pkGround)
	//{
	//	if(pkGround->GetAttr() & GATTR_VILLAGE)
	//	{
	//		if(0 < pkUnit->GetAbil(AT_VILLAGE_MOVESPEED_SAVED))
	//		{
	//			fMoveSpeed += std::max(fMoveSpeed, static_cast<float>(pkUnit->GetAbil(AT_VILLAGE_MOVESPEED_SAVED)));
	//		}
	//	}
	//}

	return std::max(fMoveSpeed, 10.0f);;//std::max(fUnitSpeed, fMoveSpeed);//건들지 마라 같은 이속증가 버프가 있을 경우도 있다. 디버프 등으로 AT_C_MOVESPEED가 0이 되는 경우가 있다.
}

bool BackAttackCheck(CUnit* pkCaster, CUnit* pkTarget, PgGround* pkGround)
{
	bool bBackAttack = false;
	BM::GUID const& rkGuid = pkTarget->GetTarget();

	if( !pkGround )
	{
		return false;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pkCaster);
	if( !pkPlayer )
	{
		return false;
	}

	if( pkCaster->GetID() != rkGuid )
	{
		VEC_GUID kVec;
		pkGround->GetPartyMember(pkPlayer->PartyGuid(), kVec);
		if( kVec.size() )
		{
			VEC_GUID::iterator iter = std::find(kVec.begin(), kVec.end(), rkGuid);
			if( kVec.end() != iter )
			{
				PgPlayer *pkTargetPlayer = dynamic_cast<PgPlayer *>(pkGround->GetUnit( rkGuid ));
				if( pkTargetPlayer )
				{		
					if( pkCaster->GetPos().x <= pkTarget->GetPos().x )
					{									
						if( pkTarget->GetPos().x <= pkTargetPlayer->GetPos().x )
						{
							bBackAttack = true;
						}
					}
					else
					{
						if( pkCaster->GetPos().x >= pkTarget->GetPos().x )
						{									
							if( pkTarget->GetPos().x >= pkTargetPlayer->GetPos().x )
							{
								bBackAttack = true;
							}
						}
					}
				}
			}
		}
	}

	return bBackAttack;
}

DWORD GetTimeStampFromActArg(SActArg const& rkActArg, wchar_t const* szFunc)
{
	// 플레이어일 경우에만, 아래의 Argument가 설정되어져 있다.
	DWORD dwTimeStamp = 0;
	if( S_OK == rkActArg.Get(ACTARG_ACTION_TIMESTAMP, dwTimeStamp) )
	{
		if( 0 != dwTimeStamp )
		{
			return dwTimeStamp;
		}
		// 프로젝타일 공격은 dwTimeStamp값이 0이다
	}
	// 없다면 몬스터 몬스터라면 바로 g_kEventView.GetServerElapsedTime() 함수를 통해서 시간값을 넣어주자
	return g_kEventView.GetServerElapsedTime();
}

bool CheckEnableTrade(PgBase_Item const &kItem,EItemCantModifyEventType const kEventType)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(	!pkItemDef 
		|| pkItemDef->IsType(ITEM_TYPE_QUEST)
		|| (pkItemDef->GetAbil(AT_ATTRIBUTE) & kEventType)
		|| kItem.EnchantInfo().IsAttached()
		|| kItem.EnchantInfo().IsBinding())
	{
		return false;
	}

	SExpCard kExpCard;
	if(true == kItem.Get(kExpCard))
	{
		if(kExpCard.CurExp() < kExpCard.MaxExp())
		{
			return false;
		}
	}
/*	//캐시거래 코드제한 부분 주석처리    2010.06.08 조현건
	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_TAIWAN:
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_FRANCE:
	case LOCAL_MGR::NC_GERMANY:
	case LOCAL_MGR::NC_USA:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_CHINA:
	case LOCAL_MGR::NC_SINGAPORE:
	case LOCAL_MGR::NC_THAILAND:
	case LOCAL_MGR::NC_INDONESIA:
	case LOCAL_MGR::NC_KOREA:
		{
		}break;
	default:
		{
			if(true == kItem.EnchantInfo().IsTimeLimit())		// 기간제 거래 불가
			{
				return false;
			}

			if(true == pkItemDef->IsType(ITEM_TYPE_AIDS))		// 캐시 아이템 거래 불가
			{
				return false;
			}
		}break;
	}
*/
	return true;
}

PgGround* GetGroundPtr(SActArg const* pkArg)
{
	if(NULL==pkArg)
	{
		return NULL;
	}

	void* pkVoid = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkVoid);
	}
	return reinterpret_cast<PgGround*>(pkVoid);
}


// EffectTick안 에서 호출 금지
void CalcAwakeValue(CUnit* pkCaster, CSkillDef const* pkSkillDef)
{
	//스킬이 발동 될 경우 각성치를 사용할 경우 각성치를 증가 시켜 준다.
	if(pkCaster->GetAbil(AT_ENABLE_AWAKE_SKILL))
	{
		switch(pkCaster->GetAbil(AT_AWAKE_STATE))
		{
		case 0:
			{
				int iNeedHP = pkSkillDef->GetAbil(AT_NEED_HP);
				int iNeedMP = pkSkillDef->GetAbil(AT_NEED_MP);
				int const iNeedHPPer = pkCaster->GetAbil(AT_ADD_R_NEED_HP); // 소모HP의 %
				int const iNeedMPPer = pkCaster->GetAbil(AT_ADD_R_NEED_MP); // 소모MP의 %
				int const iNeedHPPer2 = pkCaster->GetAbil(AT_ADD_R_NEED_HP_2); // 소모HP의 감소%
				int const iNeedMPPer2 = pkCaster->GetAbil(AT_ADD_R_NEED_MP_2); // 소모MP의 감소%

				int const iMaxHP = pkCaster->GetAbil(AT_C_MAX_HP);
				int const iMaxMP = pkCaster->GetAbil(AT_C_MAX_MP);
				int const iNeedMaxHPPer = pkSkillDef->GetAbil(AT_NEED_MAX_R_HP);
				int const iNeedMaxMPPer = pkSkillDef->GetAbil(AT_NEED_MAX_R_MP);

				//스킬 사용시 MaxHP의 %를 소모 시키는 경우 iNeedHp는 새로 세팅 된다.
				if(0 < iNeedMaxHPPer)
				{
					iNeedHP = static_cast<int>(iMaxHP * (static_cast<float>(iNeedMaxHPPer) / ABILITY_RATE_VALUE_FLOAT));			
				}
				//스킬 사용시 MaxMP의 %를 소모 시키는 경우 iNeedMp는 새로 세팅 된다.
				if(0 < iNeedMaxMPPer)
				{
					iNeedMP = static_cast<int>(iMaxMP * (static_cast<float>(iNeedMaxMPPer) / ABILITY_RATE_VALUE_FLOAT));			
				}

				if(0 < iNeedHPPer)
				{
					iNeedHP -= static_cast<int>(iNeedHP * (static_cast<float>((ABILITY_RATE_VALUE - iNeedHPPer) + iNeedHPPer2) / ABILITY_RATE_VALUE_FLOAT));
				}
				if(0 < iNeedMPPer)
				{
					iNeedMP -= static_cast<int>(iNeedMP * (static_cast<float>((ABILITY_RATE_VALUE - iNeedMPPer) + iNeedMPPer2) / ABILITY_RATE_VALUE_FLOAT));
				}

				//int const iSkillLevel = pkSkillDef->GetAbil(AT_LEVEL);
				//int const iCasterLevel = pkCaster->GetAbil(AT_LEVEL);
				//bool const bIsEmptyAwakeValue = (0 < pkSkillDef->GetAbil(AT_EMPTY_AWAKE_VALUE)) ? true : false;
				//// 각성치량을 채워주지 않는 스킬인지 체크하고
				//int iAddAWakeValue =  ( bIsEmptyAwakeValue ? 0 : (((iNeedHP + iNeedMP) / 10) * iSkillLevel) );
				//
				//if(0 < iAddAWakeValue)
				//{
				//	iAddAWakeValue += (iCasterLevel / 2);
				//}
				//
				//if(0 < iAddAWakeValue && !pkSkillDef->GetAbil(AT_NEED_AWAKE) && !pkSkillDef->GetAbil(AT_NEED_MAX_R_AWAKE) && !pkSkillDef->GetAbil(AT_ALL_NEED_AWAKE))
				//{// 증가되는 각성치량을 적용해 준다
				//	int const iNowValue = pkCaster->GetAbil(AT_AWAKE_VALUE);
				//	int const iNewValue = __min(AWAKE_VALUE_MAX, iNowValue + iAddAWakeValue);

				//	pkCaster->SetAbil(AT_AWAKE_VALUE, iNewValue, true, true);
				//}				
			}break;
		}
	}
}


namespace BroadcastUtil
{
	void EraseDuplicate(CONT_BROAD_LIST::mapped_type& rkContList)
	{
		CONT_BROAD_LIST::mapped_type::iterator iter = rkContList.begin();
		while( rkContList.end() != iter )
		{
			CONT_BROAD_LIST::mapped_type::iterator sec_iter = iter;
			++sec_iter;
			while( rkContList.end() != sec_iter )
			{
				if( (*iter) == (*sec_iter) )
				{
					sec_iter = rkContList.erase(sec_iter);
				}
				else
				{
					++sec_iter;
				}
			}
			++iter;
		}
	}
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, SERVER_IDENTITY const& rkSwithServer, BM::GUID const& rkMemberGuid)
	{
		CONT_BROAD_LIST::iterator find_iter = rkContOut.find( rkSwithServer );
		if( rkContOut.end() == find_iter )
		{
			auto kRet = rkContOut.insert( std::make_pair(rkSwithServer, CONT_BROAD_LIST::mapped_type()) );
			find_iter = kRet.first;
		}
		if( rkContOut.end() != find_iter )
		{
			(*find_iter).second.push_back( rkMemberGuid );
		}
	}
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, CONT_OBSERVER_SWITH_MEMBER_LIST const& rkCont)
	{
		CONT_OBSERVER_SWITH_MEMBER_LIST::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			AddSwitchInfo(rkContOut, (*iter).first, (*iter).second);
			++iter;
		}
	}
	void AddSwitchInfo(CONT_BROAD_LIST& rkContOut, PgPlayer const* pkPlayer, bool const bIgnoreSelf)
	{
		if( pkPlayer )
		{
			if( false == bIgnoreSelf ) // self
			{
				AddSwitchInfo(rkContOut, pkPlayer->GetSwitchServer(), pkPlayer->GetMemberGUID());
			}

			// observer
			CONT_OBSERVER_SWITH_MEMBER_LIST kContList;
			pkPlayer->GetObserverMemberList(kContList);
			AddSwitchInfo(rkContOut, kContList);
		}
	}

	void Broadcast(CONT_BROAD_LIST& rkContTarget, BM::Stream const& rkSrcPacket)
	{
		if( rkContTarget.empty() )
		{
			return;
		}

		BM::Stream kPurePacket;
		kPurePacket.Push(rkSrcPacket);//rd_pos 가 바뀌거나 했을 수 있으므로 

		CONT_BROAD_LIST::iterator iter = rkContTarget.begin();
		while( rkContTarget.end() != iter )
		{
			CONT_BROAD_LIST::key_type const& rkKey = (*iter).first;
			CONT_BROAD_LIST::mapped_type& rkVal = (*iter).second;

			EraseDuplicate(rkVal); // 중복 멤버guid 제거

			if( false == rkVal.empty() )
			{
				BM::Stream kWrappedPacket(PT_M_S_WRAPPED_USER_PACKET);
				kWrappedPacket.Push( rkVal );
				kWrappedPacket.Push( kPurePacket.Data() );
				g_kProcessCfg.Locked_SendToServer(rkKey, kWrappedPacket);
			}
			++iter;
		}
	}
};

namespace DoFinalDamageFuncSubRoutine
{
	void AddEffectWhenCasterHit(CUnit* pkCaster, CUnit* pkTarget,CAbilObject* pkCheckAbilObj, SActArg* pkArg, int const iAddEffectMax, int const iStartEffectNumAbil, int const iStartRateAbil, int const iStartTargetAbil)
	{// 타격시 이펙트 걸기
		if(!iAddEffectMax
			|| !pkCaster
			|| !pkTarget
			|| !pkCheckAbilObj
			)
		{
			return;
		}

		int i = 0;
		while(iAddEffectMax > i)
		{
			int const iAddEffect = pkCheckAbilObj->GetAbil(i + iStartEffectNumAbil);
			if(0 == iAddEffect)
			{
				//i = ((i/10)+1)*10+0;//현재 값이 없으면 10단위로 체크 한다.(5700이 없으면 5710, 5720, 5730 순으로 검색하게 함)
				++i;
				continue;
			}
			
			int iAddEffectRate = pkCheckAbilObj->GetAbil(i + iStartRateAbil);
			if(0 == iAddEffectRate)
			{// 전체 적용 확률이 없다면 100% 걸리고
				iAddEffectRate = ABILITY_RATE_VALUE;
			}

			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iAddEffectRate)
			{// 진짜 타겟을 확인 후
				CUnit* pkRealTarget = pkTarget;
				if(0 < pkCheckAbilObj->GetAbil(i + iStartTargetAbil))
				{
					pkRealTarget = pkCaster;
				}
				// 타겟에게 이펙트를 걸어 준다.
				pkArg->Set(ACTARG_CUSTOM_UNIT1, pkCaster->GetID());		// 이펙트를 걸어준 Unit을 알려준다
				pkRealTarget->AddEffect(iAddEffect, 0, pkArg, pkCaster);
			}
			++i;
		}
	}

	void RestoreHPMPWhenCasterHit(CUnit* pkCaster, PgGround* pkGround)
	{// 타격시(때릴때)
		if(!pkCaster || !pkGround)
		{
			return;
		}

		CUnit* pkTarget = NULL;
		if(ESTARGET_CASTER==pkCaster->GetAbil(AT_RESTORE_WHEN_HIT_TYPE))
		{
			pkTarget = pkGround->GetUnit(pkCaster->Caller());
		}

		PgSubPlayer* pkSubPlayer;
		if (pkCaster->IsUnitType(UT_SUB_PLAYER) &&
			(pkSubPlayer = static_cast<PgSubPlayer*>( pkCaster )))
		{
			pkTarget = pkSubPlayer->GetCallerUnit();
		}

		if(!pkTarget)
		{
			pkTarget = pkCaster;
		}
		
		int const iOutbreakRateOfHPRestore = pkCaster->GetAbil(AT_C_HP_RESTORE_WHEN_HIT_RATE);
		if(iOutbreakRateOfHPRestore)
		{
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iOutbreakRateOfHPRestore)
			{
				int const iAddHP = pkCaster->GetAbil(AT_C_HP_RESTORE_WHEN_HIT);
				if(iAddHP)
				{// HP회복이 가능하면, HP 회복을 시켜준다
					int const iMaxHP = pkTarget->GetAbil(AT_C_MAX_HP);
					int const iNowHP = pkTarget->GetAbil(AT_HP);
					if(iMaxHP > iNowHP)
					{
						OnSetAbil(pkTarget, AT_HP, __min(iMaxHP, iNowHP + iAddHP));
					}
				}
			}
		}

		int const iOutbreakRateOfMPRestore = pkCaster->GetAbil(AT_C_MP_RESTORE_WHEN_HIT_RATE);
		if(iOutbreakRateOfMPRestore)
		{
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iOutbreakRateOfMPRestore)
			{
				int const iAddMP = pkCaster->GetAbil(AT_C_MP_RESTORE_WHEN_HIT);
				if(iAddMP)
				{// MP회복이 가능 하면 MP를 회복시켜준다
					int const iMaxMP = pkTarget->GetAbil(AT_C_MAX_MP);
					int const iNowMP = pkTarget->GetAbil(AT_MP);
					if(iMaxMP > iNowMP)
					{
						OnSetAbil(pkTarget, AT_MP, __min(iMaxMP, iNowMP + iAddMP));
					}
				}
			}
		}
	}

	void RestoreHPMPWhenTargetDamage(CUnit* pkCaster, CUnit* pkTarget, PgGround* pkGround)
	{// 피격시(맞을때)
		if(!pkCaster || !pkTarget || !pkGround)
		{
			return;
		}

		CUnit * pkUnit = NULL;
		if(ESTARGET_CASTER==pkTarget->GetAbil(AT_RESTORE_WHEN_DAMAGE_TYPE))
		{
			pkUnit = pkGround->GetUnit(pkTarget->Caller());
		}

		PgSubPlayer* pkSubPlayer;
		if (pkCaster->IsUnitType(UT_SUB_PLAYER) &&
			(pkSubPlayer = static_cast<PgSubPlayer*>( pkCaster )))
		{
			pkTarget = pkSubPlayer->GetCallerUnit();
		}

		if(!pkTarget)
		{
			pkUnit = pkTarget;
		}
		
		bool bRestore = false;
		int const iOutbreakRateOfHPRestore = pkTarget->GetAbil(AT_C_HP_RESTORE_WHEN_DAMAGE_RATE);
		if(iOutbreakRateOfHPRestore)
		{
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iOutbreakRateOfHPRestore)
			{
				int const iAddHP = pkTarget->GetAbil(AT_C_HP_RESTORE_WHEN_DAMAGE);
				if(iAddHP)
				{// HP회복이 가능하면, HP 회복을 시켜준다
					int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
					int const iNowHP = pkUnit->GetAbil(AT_HP);
					OnSetAbil(pkUnit, AT_HP, std::min<int>(iMaxHP, iNowHP+iAddHP));
					bRestore = pkUnit->GetAbil(AT_HP) != iNowHP;
				}
			}
		}

		int const iOutbreakRateOfMPRestore = pkTarget->GetAbil(AT_C_MP_RESTORE_WHEN_DAMAGE_RATE);
		if(iOutbreakRateOfMPRestore)
		{
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iOutbreakRateOfMPRestore)
			{
				int const iAddMP = pkTarget->GetAbil(AT_C_MP_RESTORE_WHEN_DAMAGE);
				if(iAddMP)
				{// MP회복이 가능 하면 MP를 회복시켜준다
					int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
					int const iNowMP = pkUnit->GetAbil(AT_MP);
					OnSetAbil(pkUnit, AT_MP, std::min<int>(iMaxMP, iNowMP+iAddMP));
					bRestore = pkUnit->GetAbil(AT_MP) != iNowMP;
				}
			}
		}

		if(bRestore)
		{
			SActArg kArg;
			PgGroundUtil::SetActArgGround(kArg, pkGround);

			if(int const iDamageEffectNo = pkTarget->GetAbil(AT_RESTORE_WHEN_DAMAGE_DAMAGE_EFFECT))
			{
				pkTarget->AddEffect(iDamageEffectNo, 0, &kArg, pkCaster);
			}
			if(int const iRestoreEffectNo = pkTarget->GetAbil(AT_RESTORE_WHEN_DAMAGE_RESTORE_EFFECT))
			{
				pkUnit->AddEffect(iRestoreEffectNo, 0, &kArg, pkCaster);
			}
		}
	}
};


namespace SkillFuncUtil
{
	int OnModifyHP(CUnit* pkTarget,  CEffect* pkEffect, int const iMaxHPRate, int const iAddHP, CUnit* pkCaster, PgGround* pkGournd)
	{
		if(!pkTarget || !pkTarget->IsAlive()
			|| ( 0 == iMaxHPRate && 0 == iAddHP)
			)
		{
			return 0;
		}
		
		int const iMaxHP = pkTarget->GetAbil(AT_C_MAX_HP);
		int const iCurHP = pkTarget->GetAbil(AT_HP);
		__int64 i64ResultAddHP = iMaxHP;
		i64ResultAddHP *= iMaxHPRate;
		i64ResultAddHP /= ABILITY_RATE_VALUE;
		i64ResultAddHP += iAddHP;
		
		//int const iIncrease = pkTarget->GetAbil(절대값_값소어빌);
		int const iIncreaseRate = pkTarget->GetAbil(AT_C_ADD_HP_RESTORE_RATE);
		if(0 < i64ResultAddHP	// HP가 회복 되는데
			&& ( 0 != iIncreaseRate /*|| 0 != iIncrease*/) // 회복 감소 어빌이 있으면
			)
		{// 감소 어빌을 적용 시켜주고
			__int64 i64IncreaseHP = i64ResultAddHP;
			i64IncreaseHP *= iIncreaseRate;
			i64IncreaseHP /= ABILITY_RATE_VALUE;
			
			i64ResultAddHP += i64IncreaseHP;
			//i64ResultAddHP += iIncrease;
		}

		//iAddHP = std::min(iMaxHP-iCurHP, iAddHP);
		int const iAddedValue = NotifyChangedHPMPDelta( pkTarget, pkEffect, static_cast<short>(AT_HP), iMaxHP, iCurHP, static_cast<int>(i64ResultAddHP) );
		if( pkGournd 
			&& 0 < iAddedValue
			)
		{	// 힐량 누적 점수를 계산 하기 위해
			pkGournd->AccumulatePlayerHeal(pkCaster, iAddedValue);
		}
		return iAddedValue;
	}

	int OnModifyMP(CUnit* pkTarget, CEffect* pkEffect, int const iMaxMPRate, int const iAddMP, CUnit* pkCaster, PgGround* pkGournd)
	{
		if(!pkTarget
			|| ( 0 == iMaxMPRate && 0 == iAddMP)
			)
		{
			return 0;
		}
		int const iMaxMP = pkTarget->GetAbil(AT_C_MAX_MP);
		int const iCurMP = pkTarget->GetAbil(AT_MP);
				
		__int64 i64ResultAddMP = iMaxMP;
		i64ResultAddMP *= iMaxMPRate;
		i64ResultAddMP /= ABILITY_RATE_VALUE;
		i64ResultAddMP += iAddMP;
		
		/*int const iIncrease = pkTarget->GetAbil(절대값_값소어빌);
		int const iIncreaseRate = pkTarget->GetAbil(AT_C_ADD_MP_RESTORE_RATE);
		if(0 < i64ResultAddMP	// MP가 회복 되는데
			&& ( 0 != iIncreaseRate || 0 != iIncrease) // 회복 감소 어빌이 있으면
			)
		{// 감소 어빌을 적용 시켜주고
			__int64 i64IncreaseMP = i64ResultAddMP;
			i64IncreaseMP *= iIncreaseRate;
			i64IncreaseMP /= ABILITY_RATE_VALUE;
			
			i64ResultAddMP += i64IncreaseMP;
			//i64ResultAddHP += iIncrease;
		}*/

		//iAddMP = std::min(iMaxMP-iCurMP, iAddMP);
		int const iAddedValue = NotifyChangedHPMPDelta( pkTarget, pkEffect, static_cast<short>(AT_MP), iMaxMP, iCurMP, static_cast<int>(i64ResultAddMP) );
		return iAddedValue;
	}

	int NotifyChangedHPMPDelta(CUnit* pkUnit, CEffect* pkEffect, short const siType, int const iMax, int const iCur, int const iAdd)
	{// 변화량을 알려줌(HP,MP회복량이 표시될수 있게)
		if(!pkUnit)
		{
			return 0;
		}

		int const iEffectNo = pkEffect ? pkEffect->GetEffectNo() : 0;
		BM::GUID const& rkCasterGuid = pkEffect ? pkEffect->GetCaster() : pkUnit->GetID();
		
		int iNew = __min(iMax, iCur + iAdd);
		iNew = __max(iNew, 0);

		int const iDelta = iNew - iCur;
		if(0 != iDelta)
		{//변화량이 있다면
			BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
			kPacket.Push(pkUnit->GetID());
			kPacket.Push(siType);
			kPacket.Push(iNew);
			kPacket.Push(iEffectNo);
			kPacket.Push(rkCasterGuid);
			kPacket.Push(iDelta);
			pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
			OnSetAbil(pkUnit, siType, iNew);
			return iDelta;
		}
		return 0;
	}
	
	void OnAddSkillLinkagePoint(CUnit* pkUnit, CSkillDef const* pkSkill, PgGround* pkGround)
	{
		if(!pkUnit 
			|| !pkSkill
			|| !pkGround
			)
		{
			return;
		}
		switch( pkUnit->UnitType() )
		{
		case UT_SUB_PLAYER:
			{
				pkUnit = pkGround->GetUnit( pkUnit->Caller() );
				if( !pkUnit )
				{ 
					return;
				}
			}break;
		}

		int const iAddPoint = pkSkill->GetAbil(AT_SKILL_LINKAGE_POINT);
		__int64 const i64SkillLinkageFlag = pkSkill->GetAbil(AT_SKILL_LINKAGE_FLAG);
		__int64 const i64SkillLinkagePointCheckFlag = pkSkill->GetAbil(AT_SKILL_LINAKGE_POINT_CHECK_FLAG);
		__int64 const i64FlagRemove = pkSkill->GetAbil(AT_SKILL_LINAKGE_FLAG_REMOVE);
		if( 0 < pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT_SETED)
			 || (0 == iAddPoint) && (0 == i64SkillLinkageFlag) && (0 == i64SkillLinkagePointCheckFlag) && (0 == i64FlagRemove)
			//|| 0 == pkUnit->GetAbil(AT_BEGINL_SKILL_LINKAGE) 
			)
		{// 연계 포인트를 누적할수 없거나, 연계 flag 관련 값이 모두 없다면
			return;
		}

		SetSkillLinkage_StampMark(pkUnit, pkSkill);

		// 유닛의 연계 플래그를 얻어와
		__int64 const i64UnitLinkageFlag = pkUnit->GetAbil64(AT_SKILL_LINKAGE_FLAG); 
		__int64 i64NewLinkadFlag = 0;
		{// 스킬 각인을 할수 있는지 체크해서
			if(0 < i64SkillLinkageFlag)
			{// 각인시킬 스킬의flag값이 존재한다면
				bool bAddFlag = true;
				__int64 const i64SkillLinkageCheckFlag = pkSkill->GetAbil(AT_SKILL_LINKAGE_CHECK_FLAG);
				if(0 < i64SkillLinkageCheckFlag)
				{// 각인 check flag값을 비교해서 각인 시킬수 있는지 확인한후에
					bAddFlag = i64SkillLinkageCheckFlag&i64UnitLinkageFlag;
				}
				if(bAddFlag)
				{// 각인 시키고
					i64NewLinkadFlag = i64SkillLinkageFlag|i64UnitLinkageFlag;
				}
			}
		}

		{// 연계 포인트
			bool bLinkagePointRefrash = true;
			if(0 < i64SkillLinkagePointCheckFlag)
			{// 체크 값이 있다면 갱신 할수있는지 확인하고
				bLinkagePointRefrash = i64SkillLinkagePointCheckFlag&i64NewLinkadFlag;
				if(bLinkagePointRefrash)
				{// 갱신이 가능하다면 연계 포인트 체크 값을 제거 하고
					__int64 const i64Temp = i64NewLinkadFlag ^ i64SkillLinkagePointCheckFlag; // xor으로 제거
					i64NewLinkadFlag = i64NewLinkadFlag & i64Temp; // 혹 xor으로 기존에 없던 flag가 추가되는것을 방지하기 위해 한번더 검증
				}
			}
			if(bLinkagePointRefrash)
			{
				int const iCurSkillLinkageLevel = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT);
				int const iNextSkillLinkageLevel = iCurSkillLinkageLevel+iAddPoint;
				
				int const iMax = pkUnit->GetAbil( AT_SKILL_LINKAGE_POINT_MAX );
				bool const bExistNext = g_kSkillLinkageInfo.CheckAbil(pkSkill->No(), iNextSkillLinkageLevel);
				
				// Max체크가 나중에 들어가서 패스해야 하는 경우가 생김에 따라, 그전에 만들어진 광대 계열만 pass 시켜준다( 아오 너무 싫은 하드코딩 ㅠㅠ )
				bool const bMaxNoneCheckClass = IS_CLASS_LIMIT( UCLIMIT_COMMON_CLOWN, pkUnit->GetAbil(AT_CLASS) );

				if( bExistNext											// 다음 연계 포인트값이 존재하고
					&& ( bMaxNoneCheckClass || iNextSkillLinkageLevel <= iMax )	// Unit에 저장된 Max값이 없거나, Max값 이하라면
					)
				{// 연계 포인트의 Max값을 누적하고
					OnAddAbil(pkUnit, AT_SKILL_LINKAGE_POINT, iAddPoint);
				}

				int iEffectNo = 0;
				int const iSkillLinkageLevel = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT);
				if(g_kSkillLinkageInfo.GetInfo(pkSkill->No(), iSkillLinkageLevel, iEffectNo))
				{// 연계포인트에 해당하는 이펙트를 걸어주고
					SActArg kArg;
					PgGroundUtil::SetActArgGround(kArg, pkGround);
					pkUnit->AddEffect(iEffectNo, 0, &kArg, pkUnit);
				}
			}
		}
		
		if(0 < i64FlagRemove)
		{// 제거 flag가 있다면 제거해주고
			i64NewLinkadFlag &= (!i64FlagRemove);
		}
		if(i64UnitLinkageFlag != i64NewLinkadFlag)
		{// 연계 flag값이 변했다면 유닛에 다시 저장해준다
			pkUnit->SetAbil64(AT_SKILL_LINKAGE_FLAG, i64NewLinkadFlag, true/*, true*/ );
		}
	}

	void SetSkillLinkage_StampMark(CUnit* pkUnit, CSkillDef const* pkSkill, bool const bSetStamp)
	{// 연계포인트 더하는 과정에 중복을 방지하기위한 체크값 설정
		if(!pkUnit 
			|| !pkSkill
			)
		{
			return;
		}
		//switch( pkUnit->UnitType() )
		//{
		//case UT_SUB_PLAYER:
		//	{
		//		pkUnit = pkGround->GetUnit( pkUnit->Caller() );
		//		if( !pkUnit )
		//		{ 
		//			return;
		//		}
		//	}break;
		//}
		if(0 < pkSkill->GetAbil(AT_SKILL_LINKAGE_POINT))
		{// 연계 포인트를 누적할수 있는 스킬이라면
			OnSetAbil(pkUnit, AT_SKILL_LINKAGE_POINT_SETED, bSetStamp ? 1 : 0);
		}
	}

	void UseSkillLinkagePoint(CUnit* pkUnit, CSkillDef const* pkSkill, PgGround* pkGround)
	{// pkSkill이 연계포인트를 사용하는(최종으로) 스킬이라면 연계포인트 값을 초기화
		if(!pkUnit 
			|| 0 == pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT)
			|| !pkSkill
			|| 0 == pkSkill->GetAbil(AT_USE_SKILL_LINKAGE_POINT)
			|| !pkGround
			)
		{
			return;
		}
		switch( pkUnit->UnitType() )
		{
		case UT_SUB_PLAYER:
			{
				pkUnit = pkGround->GetUnit( pkUnit->Caller() );
				if( !pkUnit )
				{ 
					return;
				}
			}break;
		}
		// 연계 포인트를 사용하는 스킬이라면
		int const iSkillRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
		int const iMax = AT_SKILL_LINKAGE_POINT_EFFECT_TARGET10-AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01;

		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, pkGround);

		for(int i = 0; i <= iMax; ++i)
		{// 연계효과를 걸어줄 대상과, Effect 번호를 얻어와
			int const iTargetType = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01+i);
			int const iEffectNo = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT01+i);
			if(0 >= iTargetType
				|| 0 >= iEffectNo
				)
			{ 
				break;
			}

			UNIT_PTR_ARRAY kUnitArray;
			pkGround->GetUnitTargetList(pkUnit, kUnitArray, iTargetType, iSkillRange, static_cast<int>(AI_Z_LIMIT));
			if(!kUnitArray.empty())
			{
				UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin();
				while(kUnitArray.end() != itor)
				{
					CUnit* pkTarget = (*itor).pkUnit;
					if(pkTarget)
					{// 걸어주고
						pkTarget->AddEffect(iEffectNo, 0, &kArg, pkUnit);
					}
					++itor;
				}
			}
		}
		int const iSkillLinkagePoint = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT);
		OnAddAbil(pkUnit, AT_SKILL_LINKAGE_POINT, -iSkillLinkagePoint);			// 연계포인트 소모(SetAbil하면 안됨. OnAddAbil이 Effect컨테이너에 저장되기때문에)

		int const iDelEffectNo = pkSkill->GetAbil(AT_DELETE_EFFECT_NO);
		if(0 < iDelEffectNo)
		{// 연계 스킬 정보가 들어있는 이펙트를 삭제해야 함
			pkUnit->DeleteEffect(iDelEffectNo, true);
		}
	}

	bool DoChildSkillEffect(PgPlayer* pkPlayer, int const iSkillNo, bool bDeleteEffect, SActArg* pkArg)
	{// iSkillNo가 체크해야할 ChildSkill의 effect를 Add하거나 Delete 한다
		if(!pkPlayer)
		{
			return false;
		}
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
		if(!pkSkill)
		{
			return false;
		}
		PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
		if(!pkPlayerSkill)
		{
			return false;
		}

		for(int i = AT_CHILD_SKILL_NUM_01; i <= AT_CHILD_SKILL_NUM_10; ++i)
		{// 플레이어가 해당 스킬들을
			int const iChildSkillNo = pkSkill->GetAbil(i);
			if(0 >= iChildSkillNo)
			{
				break;
			}
			int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildSkillNo);
			if(iLearnedChildSkillNo)
			{// 배웠다면
				CSkillDef const* pkChildSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
				if(pkChildSkillDef)
				{// 배운 스킬이 걸어줄 이펙트들을 얻어오고
					for(int j = AT_EFFECTNUM1; i <= AT_EFFECTNUM10; ++j)
					{// 플레이어가 해당 스킬들을
						int const iChildEffectNo = pkChildSkillDef->GetAbil(j);
						if(0 >= iChildEffectNo)
						{
							break;
						}
						if(bDeleteEffect)
						{
							pkPlayer->DeleteEffect(iChildEffectNo, true);
						}
						else
						{
							pkPlayer->AddEffect(iChildEffectNo, 0, pkArg, pkPlayer);
						}
					}
				}
			}
		}
		return true;
	}

	void AddEffectWhenHit(CSkillDef const* pkSkill, SActArg const* pkArg, CUnit* pkCaster)
	{// 타격 했을때 이펙트를 걸수 있게 하는 어빌(기본 콤보 타격시 버프 걸게 하는 용도로 최초 생성)
		if( !pkSkill
			|| !pkCaster
			)
		{
			return;
		}
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		// 대상을 선택하는 타입
		for(int i = AT_EFFECTNUM1_WHEN_HIT; i <= AT_EFFECTNUM10_WHEN_HIT; ++i)
		{
			int const iEffectNo = pkSkill->GetAbil( i );
			if(0 == iEffectNo) 
			{
				break;
			}
			CEffectDef const* pkDefEffect = kEffectDefMgr.GetDef(iEffectNo);
			if( NULL == pkDefEffect )
			{
				break;
			}
			EEffectTypeWhenHit eEffectType = static_cast<EEffectTypeWhenHit>(pkDefEffect->GetAbil( AT_EFFECT_TYPE_WHEN_HIT ));
			CUnit* pkApplyUnit = NULL;
			switch( eEffectType )
			{
			case ETWH_ME:
				{
					if( UT_SUB_PLAYER == pkCaster->UnitType() )
					{//캐스터가 보조 캐릭이면 적용 대상은 메인 캐릭터로
						PgGround* pkGround = NULL;
						if( NULL == pkArg)
						{
							return;
						}
						pkArg->Get(ACTARG_GROUND, pkGround);
						if( NULL == pkGround )
						{
							return;
						}
						pkApplyUnit = pkGround->GetUnit( pkCaster->Caller() );
					}
					else
					{
						pkApplyUnit = pkCaster;
					}
				}break;
			default:
				{
					pkApplyUnit = pkCaster;
				}break;
			}
			if( NULL == pkApplyUnit )
			{
				return;
			}
			pkApplyUnit->AddEffect(iEffectNo, 0, pkArg, pkApplyUnit);
		}
	}
};

void RefrashElemStatusEffect(CUnit* pkUnit, PgGround* pkGround)
{
	if(!pkUnit 
		|| !pkGround
		)
	{
		return;
	}
	
	DeleteElemStatusEffect(pkUnit);
	{// 무기 이펙트
		int const iDestroy = GetElementAbilValue(pkUnit, ET_D_DESTROY);
		int const iFire = GetElementAbilValue(pkUnit, ET_D_FIRE);
		int const iIce = GetElementAbilValue(pkUnit, ET_D_ICE);
		int const iNature = GetElementAbilValue(pkUnit, ET_D_NATURE);
		int const iCurse = GetElementAbilValue(pkUnit, ET_D_CURSE);
		if( !(0 == iDestroy && 0 == iFire && 0 == iIce	&& 0 == iNature && 0 == iCurse) )
		{
			std::vector<SElemDmgSortData> kTempCont;
			kTempCont.push_back( SElemDmgSortData(ET_D_DESTROY, iDestroy) );
			kTempCont.push_back( SElemDmgSortData(ET_D_FIRE, iFire) );
			kTempCont.push_back( SElemDmgSortData(ET_D_ICE, iIce) );
			kTempCont.push_back( SElemDmgSortData(ET_D_NATURE, iNature) );
			kTempCont.push_back( SElemDmgSortData(ET_D_CURSE, iCurse) );
			std::sort(kTempCont.begin(), kTempCont.end());
			std::vector<SElemDmgSortData>::const_iterator kItor = kTempCont.begin();
			if( kTempCont.end() != kItor )
			{
				int iStatusEffectNo = 0;
				switch( (*kItor).eDmgType )
				{
				case ET_D_DESTROY:	{ iStatusEffectNo = WP_DESTROY_EFFECT_NO; }break;
				case ET_D_FIRE:		{ iStatusEffectNo = WP_FIRE_EFFECT_NO; }break;
				case ET_D_ICE:		{ iStatusEffectNo = WP_ICE_EFFECT_NO; }break;
				case ET_D_NATURE:	{ iStatusEffectNo = WP_NATURE_EFFECT_NO; }break;
				case ET_D_CURSE:	{ iStatusEffectNo = WP_CURSE_EFFECT_NO; }break;
				}
				if(iStatusEffectNo)
				{
					SActArg kActArg;
					PgGroundUtil::SetActArgGround(kActArg, pkGround);
					pkUnit->AddEffect(iStatusEffectNo, 0, &kActArg, pkUnit);
				}
			}
		}
	}
	{//방어 이펙트
		int const iDestroy = GetElementAbilValue(pkUnit, ET_R_DESTROY);
		int const iFire = GetElementAbilValue(pkUnit, ET_R_FIRE);
		int const iIce = GetElementAbilValue(pkUnit, ET_R_ICE);
		int const iNature = GetElementAbilValue(pkUnit, ET_R_NATURE);
		int const iCurse = GetElementAbilValue(pkUnit, ET_R_CURSE);
		if( !(0 == iDestroy && 0 == iFire && 0 == iIce	&& 0 == iNature && 0 == iCurse) )
		{
			std::vector<SElemDmgSortData> kTempCont;
			kTempCont.push_back( SElemDmgSortData(ET_R_DESTROY, iDestroy) );
			kTempCont.push_back( SElemDmgSortData(ET_R_FIRE, iFire) );
			kTempCont.push_back( SElemDmgSortData(ET_R_ICE, iIce) );
			kTempCont.push_back( SElemDmgSortData(ET_R_NATURE, iNature) );
			kTempCont.push_back( SElemDmgSortData(ET_R_CURSE, iCurse) );
			std::sort(kTempCont.begin(), kTempCont.end());
			std::vector<SElemDmgSortData>::const_iterator kItor = kTempCont.begin();
			if( kTempCont.end() != kItor )
			{
				int iStatusEffectNo = 0;
				switch( (*kItor).eDmgType )
				{
				case ET_R_DESTROY:	{ iStatusEffectNo = DF_DESTROY_EFFECT_NO; }break;
				case ET_R_FIRE:		{ iStatusEffectNo = DF_FIRE_EFFECT_NO; }break;
				case ET_R_ICE:		{ iStatusEffectNo = DF_ICE_EFFECT_NO; }break;
				case ET_R_NATURE:	{ iStatusEffectNo = DF_NATURE_EFFECT_NO; }break;
				case ET_R_CURSE:	{ iStatusEffectNo = DF_CURSE_EFFECT_NO; }break;
				}
				if(iStatusEffectNo)
				{
					SActArg kActArg;
					PgGroundUtil::SetActArgGround(kActArg, pkGround);
					pkUnit->AddEffect(iStatusEffectNo, 0, &kActArg, pkUnit);
				}
			}
		}
	}
}

void DeleteElemStatusEffect(CUnit* pkUnit)
{
	if(!pkUnit)
	{
		return;
	}
	pkUnit->DeleteEffect(WP_DESTROY_EFFECT_NO);
	pkUnit->DeleteEffect(WP_FIRE_EFFECT_NO);
	pkUnit->DeleteEffect(WP_ICE_EFFECT_NO);
	pkUnit->DeleteEffect(WP_NATURE_EFFECT_NO);
	pkUnit->DeleteEffect(WP_CURSE_EFFECT_NO);

	pkUnit->DeleteEffect(DF_DESTROY_EFFECT_NO);
	pkUnit->DeleteEffect(DF_FIRE_EFFECT_NO);
	pkUnit->DeleteEffect(DF_ICE_EFFECT_NO);
	pkUnit->DeleteEffect(DF_NATURE_EFFECT_NO);
	pkUnit->DeleteEffect(DF_CURSE_EFFECT_NO);
}

int GetMonsterBagTunningNo(int const iMin,int const iMax)
{
	if(0==iMax)
	{
		return iMin;
	}
	return BM::Rand_Range(iMax,iMin);
}