#include "stdafx.h"
#include "Variant/skill.h"
#include "Variant/PgActionResult.h"
#include "PgSkillOnFindTarget.h"
#include "PgGround.h"
#include "AI/Enemyselector.h"

bool CheckTargetAngle(SActionInfo const &rkAction, ESkillArea const eArea, POINT3 const &rkTargetPos,  float const fErrorDelta)
{
	if(ESArea_Sphere==eArea)
	{
		return true;//원형은 각도계산할 필요 없음
	}
	if(POINT3BY::NullData() == rkAction.ptDirection)
	{
		return false;
	}
	
	POINT3 kNewDir(rkAction.ptDirection.x, rkAction.ptDirection.y, rkAction.ptDirection.z);
	kNewDir.Normalize();
	if(UP_VECTOR == kNewDir)//업벡터랑 같으면
	{
		return true;
	}

	NiPoint3 kTargetDir(rkTargetPos.x - rkAction.ptPos.x, rkTargetPos.y - rkAction.ptPos.y, rkTargetPos.z - rkAction.ptPos.z);
	kTargetDir.Unitize();

	NiPoint3 kAttackDir(kNewDir.x, kNewDir.y, kNewDir.z);
	float const fAngle = fabs(1 - kTargetDir.Dot(kAttackDir))*90.0f;
	bool const bRet = fErrorDelta > fAngle;
	if(!bRet)
	{
		INFO_LOG(BM::LOG_LV5,__FL__<<" ActionID : "<<rkAction.iActionID<<" Angle is "<<fAngle);
	}

	return bRet;
}

int CalcRangeCheckMutiflier(int const iRange)
{
	float fRangeCheckMutiflier = 50.0f;
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_SkillRange, EHack_CVar_SkillRange_Rate, fRangeCheckMutiflier);

	return iRange + static_cast<int>(fRangeCheckMutiflier);//오차범위
}

size_t DefaultOnFindTarget(BM::Stream * const pkPacket, const CSkillDef *pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, int const iSkillRange2, ESkillArea const eArea, PgGround* pkGround, SActionInfo const &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	if(!pkPacket || !pkDef || !pkUnit || !pkGround || 0==byTargetNum)
	{
		return 0;
	}

	if(rkAction.byActionParam != ESS_FIRE && rkAction.byActionParam != ESS_TARGETLISTMODIFY)
	{
		return 0;
	}

	/*if(pkDef->GetTargetType() == ESTARGET_SELF)
	{
		rkArray.Add(pkUnit);
		rkActionResultVec.GetResult(pkUnit->GetID(), true)->SetCollision(0);
		return 1;
	}*/

	float fRangeCheckMutiflier = 50.0f;
	g_kAntiHackMgr.Locked_GetPolicy_GetValue(EAHP_SkillRange, EHack_CVar_SkillRange_Rate, fRangeCheckMutiflier);

	NxVec3 kVisionDir(rkAction.ptDirection.x, rkAction.ptDirection.y, rkAction.ptDirection.z); 
	kVisionDir.normalize();

	//INFO_LOG(BM::LOG_LV5, __FL__<<"kVisionDir X : "<<(int)(kVisionDir.x)<<" Y : "<<(int)(kVisionDir.y)<<" Z : "<<(int)(kVisionDir.z));

	BM::GUID kGuid;
	BYTE byCollision = 0;

	POINT3 kTargetPos;
	int const iFirstAdjustedParam2 = PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_RANGE2, pkDef->No(), pkUnit, iSkillRange2);
	int iParam2 = iFirstAdjustedParam2;
	int iParam3 = 0;
	int iparam4 = 0;

	int const iFirstAdjustedSkillRange = PgAdjustSkillFunc::CalcAdjustSkillValue(EASCT_RANGE, pkDef->No(), pkUnit, iSkillRange);
	int iRealSkillRange = iFirstAdjustedSkillRange;

	for (BYTE byI=0; byI<byTargetNum; ++byI)
	{
		pkPacket->Pop(kGuid);
		pkPacket->Pop(byCollision);
		CUnit* pkTarget = pkGround->GetUnit(kGuid);
		if (pkTarget != NULL)
		{
			if( pkTarget->IsUnitType(UT_BOSSMONSTER) )	//일단 보스는 그냥 패스
			{
				rkArray.Add(pkTarget);
				rkActionResultVec.GetResult(kGuid, true)->SetCollision(byCollision);
				continue;
			}
			if ( pkTarget != pkUnit)	// Caster 자신일때는 Range 검사 하지 않는다.
			{
				int iSizeXY = pkTarget->GetAbil(AT_UNIT_SIZE_XY);//*pkTarget->GetAbil(AT_UNIT_SCALE)/ABILITY_RATE_VALUE_FLOAT; //몬스터 충돌구 크기에 몬스터 사이즈 어빌을 곱한다.
				if(0>=iSizeXY) { iSizeXY = static_cast<int>(PG_CHARACTER_CAPSULE_RADIUS); }	//없으면 기본 5
				
				kTargetPos = pkTarget->GetPos();
				kTargetPos.z = 0;
				POINT3 kActionPos = rkAction.ptPos;	//Z축 비교는 패스
				kActionPos.z = 0;

				bool bIsOk = false;
				if(fRangeCheckMutiflier > abs(POINT3::Distance(kTargetPos, kActionPos) - iSizeXY))
				{
					bIsOk = true;
				}
				else
				{
					iRealSkillRange = iFirstAdjustedSkillRange;//매번 초기화 해주자
					if(eArea!=ESArea_Front_Sphere)//이 방식은 정확히 내 앞쪽 거리를 측정해야 함
					{
						iRealSkillRange = iFirstAdjustedSkillRange + iSizeXY;	//일단 몬스터 사이즈를 무조건 한번 더하자
					}
					iParam2 = iFirstAdjustedParam2 + iSizeXY; //폭도 몬스터 사이즈만큼 늘리자. 원으로 검사할때는 이 값을 쓰지 않으므로 무조건 더해줘도 상관없음

					if(eArea==ESArea_Cube)	//육면체로 검사할 때만 뒤로 약간 빼주자
					{
						int const iBackLen = -pkDef->GetAbil(AT_SKILL_BACK_DISTANCE) -iSizeXY - static_cast<int>(fRangeCheckMutiflier);
						kActionPos+=POINT3(kVisionDir.x*iBackLen, kVisionDir.y*iBackLen, kVisionDir.z*iBackLen);//뒤로 약간 빼고 다시 몬스터 크기만큼 빼야 함.
						iRealSkillRange += (iSizeXY+static_cast<int>(fRangeCheckMutiflier));//직육면체 일경우 뒤로 땡겨주기 때문에 한번 더 더해야 함	
					}

					bIsOk = PgSkillAreaChooser::InArea(pkTarget, eArea, kActionPos, kVisionDir, kTargetPos, iRealSkillRange, iParam2, iParam3, pkDef->GetAbil(AT_SKILL_MIN_RANGE));
				}

				rkArray.AddToRestore(pkTarget, !bIsOk);
				rkActionResultVec.GetResult(kGuid, true)->SetCollision(byCollision);

				if (!bIsOk)	//일단 구체크만
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<" Action Pos X : "<<kActionPos.x<<" Y : "<<kActionPos.y<<" Z : "<<kActionPos.z<<" Dist : "<<POINT3::Distance(kActionPos, kTargetPos)<<" ActionNo : "<<rkAction.iActionID<<" CalcedSkillRange : "<<iRealSkillRange<<" TargetName : "<<pkTarget->Name());
					pkUnit->SendWarnMessageStr((BM::vstring)__FL__<<" Action Pos X : "<<rkAction.ptPos.x<<" Y : "<<rkAction.ptPos.y<<" Z : "<<rkAction.ptPos.z<<" Dist : "<<POINT3::Distance(kActionPos, kTargetPos)<<" ActionNo : "<<rkAction.iActionID<<" CalcedSkillRange : "<<iRealSkillRange<<" TargetName : "<<pkTarget->Name());
				}
			}
			else
			{
				if(pkDef->GetTargetType() & ESTARGET_SELF)
				{
					rkArray.Add(pkTarget);
					rkActionResultVec.GetResult(kGuid, true)->SetCollision(byCollision);
				}
			}
		}
	}

	return rkArray.size();
}

bool PgSkillOnFindTarget::OnFindTarget(BM::Stream * const pkPacket, const CSkillDef *pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	ESkillArea const eAreaType = static_cast<ESkillArea>(pkDef->GetAbil(AT_1ST_AREA_TYPE));
	int iParam2 = pkDef->GetAbil(AT_1ST_AREA_PARAM2);
	if (iParam2 <= 0)
	{
		iParam2 = 50;//AI_Z_LIMIT;
	}
	iParam2 += CalcRangeCheckMutiflier(iParam2);//오차범위
	
	return DefaultOnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, iSkillRange, iParam2, eAreaType, pkGround, rkAction, rkArray, rkActionResultVec) == byTargetNum;	//두 숫자가 다르면 무언가 문제
}

bool PgSkillOnFindTargetPointBurst::OnFindTarget(BM::Stream * const pkPacket, const CSkillDef *pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	ESkillArea const eAreaType = static_cast<ESkillArea>(pkDef->GetAbil(AT_1ST_AREA_TYPE));

	int iParam2 = pkDef->GetAbil(AT_1ST_AREA_PARAM2);
	if (iParam2 <= 0)
	{
		iParam2 = 50;//AI_Z_LIMIT;
	}

	iParam2 = CalcRangeCheckMutiflier(iParam2);//오차범위

	return DefaultOnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, pkDef->GetAbil(AT_ATTACK_RANGE), iParam2, ESArea_Front_Sphere, pkGround, rkAction, rkArray, rkActionResultVec) == byTargetNum;	//두 숫자가 다르면 무언가 문제
}

bool PgSkillOnFindTargetStormBlade::OnFindTarget(BM::Stream *const pkPacket, const CSkillDef *pkDef, CUnit *pkUnit, const BYTE byTargetNum, const int iSkillRange, PgGround *pkGround, const SActionInfo &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	return DefaultOnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, iSkillRange, 0, ESArea_Sphere, pkGround, rkAction, rkArray, rkActionResultVec) == byTargetNum;
}

bool PgSkillOnFindTargetChainLight::OnFindTarget(BM::Stream *const pkPacket, const CSkillDef *pkDef, CUnit *pkUnit, const BYTE byTargetNum, const int iSkillRange, PgGround *pkGround, const SActionInfo &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	int iParam2 = pkDef->GetAbil(AT_1ST_AREA_PARAM2)+pkDef->GetAbil(AT_2ND_AREA_PARAM1);
	iParam2 += CalcRangeCheckMutiflier(iParam2);//오차범위

	SActionInfo kNewAction = rkAction;
	NiPoint3 kDir(kNewAction.ptDirection.x, kNewAction.ptDirection.y, kNewAction.ptDirection.z);
	kDir.Unitize();
	kDir*=static_cast<float>(pkDef->GetAbil(AT_2ND_AREA_PARAM1));
	kNewAction.ptPos.x-=kDir.x;
	kNewAction.ptPos.y-=kDir.y;
	kNewAction.ptPos.z-=kDir.z;
	int iRealSkillRange = pkDef->m_sRange + pkDef->GetAbil(AT_2ND_AREA_PARAM1)*2;
	iRealSkillRange += CalcRangeCheckMutiflier(iRealSkillRange);//오차범위


	return DefaultOnFindTarget(pkPacket, pkDef, pkUnit, byTargetNum, iRealSkillRange, iParam2, static_cast<ESkillArea>(pkDef->GetAbil(AT_1ST_AREA_TYPE)), pkGround, kNewAction, rkArray, rkActionResultVec) == byTargetNum;
}

bool PgSkillOnFindTargetSummoned::OnFindTarget(BM::Stream *const pkPacket, const CSkillDef *pkDef, CUnit *pkUnit, const BYTE byTargetNum, const int iSkillRange, PgGround *pkGround, const SActionInfo &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec)
{
	if(!pkPacket || !pkDef || !pkUnit || !pkGround || 0==byTargetNum)
	{
		return false;
	}

	if(rkAction.byActionParam != ESS_FIRE && rkAction.byActionParam != ESS_TARGETLISTMODIFY)
	{
		return false;
	}

	int const iRange = pkDef->GetAbil(AT_ATTACK_RANGE);

	CUnit* pkSummoned = NULL;
	BM::GUID kGuid;
	bool byCollision = false;
	for (BYTE byI=0; byI<byTargetNum; ++byI)
	{
		pkPacket->Pop(kGuid);
		pkPacket->Pop(byCollision);

		if(pkUnit->IsSummonUnit(kGuid) && (pkSummoned = pkGround->GetUnit(kGuid)))
		{
			if(iRange && iRange<GetDistance(pkUnit->GetPos(), pkSummoned->GetPos()))
			{
				continue;
			}

			rkArray.Add(pkSummoned);
			rkActionResultVec.GetResult(kGuid, true)->SetCollision(byCollision);
		}
	}

	return rkArray.size();
}
