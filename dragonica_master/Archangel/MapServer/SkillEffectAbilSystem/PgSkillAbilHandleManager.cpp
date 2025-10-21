#include "stdafx.h"
#include "PgSkillAbilHandleManager.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Guild.h"
#include "PgSkillFunction_Fighter.h"
#include "PgSkillFunction_Magician.h"
#include "PgSkillFunction_Archer.h"
#include "PgSkillFunction_Thief.h"
#include "PgSkillFunction_Monster.h"
#include "PgSkillFunction_Action.h"
#include "PgSkillFunction_Pet.h"
#include "PgSkillFunction_Summoner.h"
#include "PgEffectAbilTable.h"

///////////////////////////////////////////////////////////
//  PgSkillAbilHandleManager
///////////////////////////////////////////////////////////
PgSkillAbilHandleManager::PgSkillAbilHandleManager()
{
}

PgSkillAbilHandleManager::~PgSkillAbilHandleManager()
{
	Release();
}

void PgSkillAbilHandleManager::Init()
{
}

void PgSkillAbilHandleManager::Release()
{
	//BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SKILL_FUNC::iterator skill_itor = m_kConSkill.begin();
	while(skill_itor != m_kConSkill.end())
	{
		SAFE_DELETE((*skill_itor).second);
		skill_itor = m_kConSkill.erase(skill_itor);
	}

	m_kConSkill.clear();
//	m_kConSkillPool.clear();
}

bool PgSkillAbilHandleManager::Build()
{	
	bool bReturn = true;
	//BM::CAutoMutex kLock(m_kMutex, true);
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Start");

	PgFunctionTypeXmlLoader kFuncType;
	kFuncType.ParseXml(_T("XML/SkillFuncType.xml"));

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDefMgr::CONT_DEF kContDef;
	kSkillDefMgr.GetCont(kContDef);

	CSkillDefMgr::CONT_DEF::const_iterator def_itor = kContDef.begin();
	while(kContDef.end() != def_itor)
	{
		int iSkillNo = (*def_itor).first.kKey;	
		CSkillDef const* pkSkillDef = (*def_itor).second;

		PgISkillFunction* kTempFunc = NULL;
		auto ret = m_kConSkill.insert(std::make_pair(iSkillNo, kTempFunc));

		if(ret.second)
		{
			PgISkillFunction* pkFunc = NULL;
			int iCallFuncionType = kFuncType.GetFuncType(iSkillNo);
			if(SFT_DEFAULT == iCallFuncionType)
			{
				//테이블에 등록되어 있지 않을 경우 부모 스킬의 타입을 얻어온다.
				if(pkSkillDef->GetParentSkill())
				{
					iCallFuncionType = kFuncType.GetFuncType(pkSkillDef->GetParentSkill());
				}
			}
			pkFunc = CreateSkillFunc(iCallFuncionType);

			if(pkFunc)
			{
				pkFunc->Init();
				pkFunc->Build();
				ret.first->second = pkFunc;
			}
			else
			{
				//나오면 안되는 로그(New가 실패 했다는 Log)
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"PgISkillFunction New Failed "<<iSkillNo);
				bReturn = false;
			}
		}

		++def_itor;
	}
	INFO_LOG(BM::LOG_LV9, __FL__<<L"End");
	if( !bReturn )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return bReturn;
}

int PgSkillAbilHandleManager::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillBegin(pkUnit, iSkillNo, pArg, iStatus);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
	return 1;	
}
int PgSkillAbilHandleManager::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream* pkPacket)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillBegin(pkUnit, iSkillNo, pArg, iStatus, pkPacket);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
	return 1;	
}

int PgSkillAbilHandleManager::SkillToggle(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillToggle(pkUnit, iSkillNo, pArg, bToggleOn, pkUnitArray, pkResult);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
	return 1;
}

int PgSkillAbilHandleManager::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
	return -1;
}

bool PgSkillAbilHandleManager::SkillFail(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillFail(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSkillAbilHandleManager::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	//BM::CAutoMutex kLock(m_kMutex);

//	if(pkUnit)
//	{
//		if(UT_PLAYER != pkUnit->UnitType())
//		{
//			if (!pkUnit->CheckSkillFilter(iSkillNo))
//			{
//				return false;
//			}
//		}
//	}

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillCanReserve(pkUnit, iSkillNo, pArg, pkUnitArray);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find UnitType["<<pkUnit->UnitType()<<L"] ClassNo["<<pkUnit->GetAbil(AT_CLASS)<<L"] Lv["<<pkUnit->GetAbil(AT_LEVEL)<<L"] SkillNo["<<iSkillNo<<L"] Func");
	
	PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkUnit);
	if (pkMonster)
	{
		pkMonster->AddFailedSkill(iSkillNo);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int PgSkillAbilHandleManager::SkillPassive(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillPassive(pkUnit, iSkillNo, pArg);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
	return 1;
}

int PgSkillAbilHandleManager::SkillPCheck(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)	
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(pkSkill->No());

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkSkill->No());
		return (*itor).second->SkillPCheck(pkUnit, pkSkill, pArg);
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<pkSkill->No()<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 1"));
	return 1;
}

void PgSkillAbilHandleManager::SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(pkSkill->No());

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkSkill->No());
		(*itor).second->SkillBegin(pkUnit, pkSkill, pArg);
		return;
	}
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<pkSkill->No()<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
}

void PgSkillAbilHandleManager::SkillEnd(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(pkSkill->No());

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkSkill->No());
		(*itor).second->SkillEnd(pkUnit, pkSkill, pArg);
		return;
	}
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<pkSkill->No()<<L"] Func");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
}

int PgSkillAbilHandleManager::SkillFire(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg)
{
	//BM::CAutoMutex kLock(m_kMutex);

	CONT_SKILL_FUNC::const_iterator itor = m_kConSkill.find(iSkillNo);

	if(itor != m_kConSkill.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,iSkillNo);
		return (*itor).second->SkillFire(pkUnit, iSkillNo, iStatus, pArg);
	}
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillNo["<<iSkillNo<<L"] Func");	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
	return -1;
}



PgISkillFunction* PgSkillAbilHandleManager::CreateSkillFunc(int const iFuncType)const
{
	PgISkillFunction* kTempFunc = NULL;
//	auto ret = m_kConSkillPool.insert(std::make_pair(iFuncType, kTempFunc));

//	if(ret.second)
	{
		switch(iFuncType)
		{
		case SFT_DEFAULT :						{ kTempFunc = new_tr PgDefaultSkillFunction;				} break;
		case SFT_DEFAULT2_ONLY_ADDEFFECT :		{ kTempFunc = new_tr PgDefaultOnlyAddeffectSkillFunction;	} break;
		case SFT_DEFAULT3_ONLY_DAMAGE :			{ kTempFunc = new_tr PgDefaultOnlyDamageSkillFunction;		} break;
		case SFT_DEFAULT4_ONLY_DAMAGE_MON :		{ kTempFunc = new_tr PgDefaultOnlyDamageMonSkillFunction;	} break;
		case SFT_DEFAULT5_ONLY_ADDEFFECT_MON :	{ kTempFunc = new_tr PgDefaultOnlyAddeffectMonSkillFunction;} break;
		case SFT_DEFAULT6_DAMAGE_AND_ADDEFFECT:	{ kTempFunc = new_tr PgDefaultDamageAndEffectSkillFunction;	} break;
		case SFT_DEFAULT7_ONLY_ADDEFFECT:		{ kTempFunc = new_tr PgDefaultOnly_Add_EffectSkillFunction;	} break;	//Caster에 DefSkill의 EffectID를, Target에 Abil의 Effect를 적용
		case 8 :								{ kTempFunc = new_tr PgDefaultDamageAndEffectSkillFunction2;} break;	//Begin때 Caster에 DefSkill의 EffectID를, Fire때 Target에 데미지와 Abil의 Effect를 적용
		case 9 :								{ kTempFunc = new_tr PgDefaultDamageAndEffectSkillFunction3;} break;	//Fire때 Target이 없어도 시전자에게 이펙트 적용
		case 142 :								{ kTempFunc = new_tr PgTargetLinkSkillFunction;				} break;	//타겟링크
		case 143 :								{ kTempFunc = new_tr PgTargetLink2SkillFunction;			} break;	//타겟링크2
		case EFFECTNO_RESURRECTION01 :			{ kTempFunc = new_tr PgResurrection01SkillFunction;			} break;	
		case 3000501:							{ kTempFunc = new_tr PgRePositionFunction;					} break;
		case 3531020:							{ kTempFunc = new_tr PgUnitForceFileSkillFunction;			} break;	//유닛에게 설정된 강제 스킬 발동
		case 6000852 :							{ kTempFunc = new_tr PgFireProjectileServerFunction;		} break;	// 발사체의 갯수, 방향을 결정해서 직선으로 서버에서 발사
		case 6000853 :							{ kTempFunc = new_tr PgTeleportMostDistantFunction;			} break;	// 가장 먼 더미 위치로 순간이동한다.
		case 6000854 :							{ kTempFunc = new_tr PgTargetLocProjectileFunction;			} break;	// 발사체, 서버에서 타겟, 발사체 갯수 지정
		case 6000900 :							{ kTempFunc = new_tr PgLavalonBreathSkillFunction;			} break;	//라발론 브레스
		case 6000903 :							{ kTempFunc = new_tr PgLavalonBlazeFunction;				} break;	//라발론 화염폭풍
		case 6000930 :							{ kTempFunc = new_tr PgTargetByDummySkillFunction;			} break;	//맵더미1
		case 6000931 :							{ kTempFunc = new_tr PgTargetByDummySkill2Function;			} break;	//맵더미2
		case 6000940 :							{ kTempFunc = new_tr PgForceFireSkillFunction;				} break;	//특정 Grade의 몹에게 특정 스킬을 강제로 쓰게함
		case 4701400 :							{ kTempFunc = new_tr PgForceSequenseFireSkillFunction;		} break;	//스퀀스 스킬을 강제로 쓰게함
		case 3542340 :							{ kTempFunc = new_tr PgTargetForceSequenseFireSkillFunction;} break;	//타겟 유닛이 스퀀스 스킬을 강제로 쓰게함
		case 6000944 :							{ kTempFunc = new_tr PgMultiHomingSkillFunction;			} break;	//멀티 원거리 유도
		case 6000946 :							{ kTempFunc = new_tr PgTeleportToDummySkillFunction;		} break;	//맵의 더미있는 곳으로 텔레포트
		case 6000955 :							{ kTempFunc = new_tr PgClearAllEffectFunction;				} break;	//이펙트를 모두 지운다.
		case 6000982 :							{ kTempFunc = new_tr PgSummonMonsterRegenPointFunction;		} break;	//원래 리젠 위치에 몬스터 소환
		case 3531030 :							{ kTempFunc = new_tr PgSummonMonsterRegenPointChooseFunction;	} break;	//원래 리젠 위치에 몬스터 소환(타입에 따라 선택)
		case 3571500 :							{ kTempFunc = new_tr PgSummonMonsterRegenPoint_Loop_Function;	} break;	//리젠 위치에 몬스터 소환(스킬 발동 횟수에 따라 BagNo 호출)
		case 6000983 :							{ kTempFunc = new_tr PgDarkBreathFunction;					} break;	//본드래곤 검은브레스
		case 6000988 :							{ kTempFunc = new_tr PgSummonBoneFunction;					} break;	//본드래곤 뼈소환
		case 60009881 :							{ kTempFunc = new_tr PgForceMoveFunction;					} break;	//강제로 이동
		case 6003101 :							{ kTempFunc = new_tr PgDamageAndEffectSkillFunction;		} break;	//거미 독살포. 데미지+이펙트(100%확률)
		case 6004201 :							{ kTempFunc = new_tr PgDashAfterAttackSkillFunction;		} break;	//돌진 후 공격
		case 6004904 :							{ kTempFunc = new_tr PgDashAttackSkillFunction;				} break;	//일반 돌진
		case 6052630 :							{ kTempFunc = new_tr PgMoveToFarhestTargetFunction;				} break;	//특정 위치까지 이동
		case 6009602 :							{ kTempFunc = new_tr PgEnergyExplosionSkillFunction;		} break;	//데스마스터 기모았다 발산, 불가사리 비 소환
		case 6009604 :							{ kTempFunc = new_tr PgEarthQuakeSkillFunction;				} break;	//데스마스터 대지의 분노
		case 6013904 :							{ kTempFunc = new_tr PgMonsterHealSkillFunction;			} break;	//몬스터 힐
		case 6013905 :							{ kTempFunc = new_tr PgMonsterMassiveHealSkillFunction;		} break;	//몬스터 단체 힐
		case 6018904 :							{ kTempFunc = new_tr PgSummonThornSkillFunction;			} break;	//우마이 엔티티 소환
		case 6019004 :							{ kTempFunc = new_tr PgSummonEntitySkillFunction;			} break;	//바라보는 방향으로 SkillRange 위치에 엔티티 소환
		case 6000932 :							{ kTempFunc = new_tr PgSummonThornsSkillFunction;			} break;	//타겟 갯수만큼 엔티티 소환
		case 6025904 :							{ kTempFunc = new_tr PgSummonMonsterSkillFunction;			} break;	//자코 소환
		case 6031105 :							{ kTempFunc = new_tr PgHomingSkillFunction;					} break;	//원거리 유도
		case 6033201 :							{ kTempFunc = new_tr PgLavalonMeteorFunction;				} break;	//라발론 메테오
		case 6033202 :							{ kTempFunc = new_tr PgLavalonMeteorBlueFunction;			} break;	//라발론 파란 메테오
		case 6041503:							{ kTempFunc = new_tr PgMassiveProjectile2SkillFunction;		} break;	//여러군데에 발사체 낙하
		case 6053100 :							{ kTempFunc = new_tr PgKamikazeSkillFunction;				} break;	//자폭공격 스킬
		case 6074304 :							{ kTempFunc = new_tr PgExplosionBlinkFunction;				} break;	//폭발 점멸(순간이동)
		case 6074305 :							{ kTempFunc = new_tr PgBlackholeFunction;					} break;	//블랙홀
		case 6089100 :							{ kTempFunc = new_tr PgAddEffectByEntitySkillFunction;		} break;	//가고일 깨우기 스킬
		case 6089101 :							{ kTempFunc = new_tr PgSetHpSkillFunction;					} break;	//공격당한 타겟의 HP를 특정 값으로 설정
		case 6089300 :							{ kTempFunc = new_tr PgAddRemoveEffectTargetMonsterFunction;} break;	//지정 몬스터에게 이펙트를 걸거나 해제한다.
		case 6089400 :							{ kTempFunc = new_tr PgMoveToTargetDummyFunction;			} break;	//특정 몹의 위치까지 이동
		case 6089401 :							{ kTempFunc = new_tr PgHealToTargetMonsterFunction;			} break;	//특정 몹에게 HP 회복하기
		case 6089411 :							{ kTempFunc = new_tr PgHealToTargetMonster_2_Function;		} break;	//특정 몹에게 HP 회복하기2			
		case 6091103 :							{ kTempFunc = new_tr PgMoveAndTrapFunction;					} break;	// 이동후 트랩을 설치한다(더미 위치에 순서대로)
		case 60911031 :							{ kTempFunc = new_tr PgEntityKamikazeSkillFunction;			} break;	//엔터티 자폭공격스킬
		case 6091302 :							{ kTempFunc = new_tr PgCrossAttackFunction;					} break;	//몬스터를 중심으로 X 자 형태의 데미지를 준다.
		case 6091304 :							{ kTempFunc = new_tr PgAttackWithoutSafetyZoneFunction;		} break;	// 안전영역을 만들고, 그곳을 제외한 부분을 공격한다.
		case 6091370 :							{ kTempFunc = new_tr PgDummyEntityZoneFunction;				} break;	// 지정된 더미지역에 엔티티 소환
		case 60913041 :							{ kTempFunc = new_tr PgCheckUserAndAttackTargetMonster;		} break;	// 유닛 주위에 유저가 있을시 타겟 몬스터를 공격한다.
		case 6091502 :							{ kTempFunc = new_tr PgRandomTeleportTargettoDummyFunction;	} break;	//타겟들을 다른 더미위치로 랜덤하게 이동 시킨다.
		case 6126110 :							{ kTempFunc = new_tr PgSummonEntitiesSkillFunction;			} break;	//엔티티을 여러개 랜덤위치로 소환
		case 6091505 :							{ kTempFunc = new_tr PgTeleportTargettoDummyFunction;		} break;	//타겟을 더미위치로 이동 시킨다.
		case 6000954 :							{ kTempFunc = new_tr PgTeleportTargetFunction;				} break;	//타겟을 유닛위치로 순간이동 시킨다.
		case 3542108 :							{ kTempFunc = new_tr PgTeleportTargetDamageFunction;		} break;	//폭발 점멸(순간이동, 캐스팅때 이동위치를 클라에 전달)
		case 80000701 :							{ kTempFunc = new_tr PgGoldRushSkillFunction;				} break;	//골드러쉬
		case 80007001:							{ kTempFunc = new_tr PgPetProduceItem;						} break;	//정해진 시간 마다 아이템을 지급 (펫=>플레이어)
		case 90000001 :							{ kTempFunc = new_tr PgCoupleLoverSkillFunction;			} break;	//커플 스킬		
		case 100005426 :						{ kTempFunc = new_tr PgRunSkillFunction;					} break;	//걷기
		case 101000101 :						{ kTempFunc = new_tr PgHammerCrushSkillFunction;			} break;	//해머 크러쉬
		case 101000701 :						{ kTempFunc = new_tr PgArmorBreakSkillFunction;				} break;	//아머브레이크
		case 101000801 :						{ kTempFunc = new_tr PgStormbladeSkillFunction;				} break;	//스톰블레이드
		case 110001901 :						{ kTempFunc = new_tr PgMultiShotSkillFunction;				} break;	//멀티 샷
		case 101300101 :						{ kTempFunc = new_tr PgSwordMasterySkillFunction;			} break;	//스워드마스터리
		case 102000201 :						{ kTempFunc = new_tr PgTransformationSkillFunction;			} break;	//마녀의저주
		case 102000301 :						{ kTempFunc = new_tr PgDetectionSkillFunction;				} break;	//디텍션
		case 102000501 :						{ kTempFunc = new_tr PgHealisSelfSkillFunction;				} break;	//힐은셀프
		case 102002401 :						{ kTempFunc = new_tr PgSlowHealSkillFunction;				} break;	//슬로우 힐			
		case 103000301 :						{ kTempFunc = new_tr PgBeautifulGirlSkillFunction;			} break;	//멋진언니
		case 103201101 :						{ kTempFunc = new_tr PgThiefDefaultHitSkillFunction;		} break;	//도둑 평타(2/4타 제외)
		case 103201201 :						{ kTempFunc = new_tr PgThiefDefault24HitSkillFunction;		} break;	//도둑 2/4 평타
		case 104300201 :						{ kTempFunc = new_tr PgStripWeaponSkillFunction;			} break;	//스트립웨폰
		case 104302101 :						{ kTempFunc = new_tr PgDexteritySkillFunction;				} break;	//덱스터리티
		case 105300501 :						{ kTempFunc = new_tr PgJointBreakSkillFunction;				} break;	//조인트브레이크
		case 105501601 :						{ kTempFunc = new_tr PgBugsLifeSkillFunction;				} break;	//바퀴벌레 인생			
		case 106000101 :						{ kTempFunc = new_tr PgRouletteSkillFunction;				} break;	//룰렛
		case 106300401 :						{ kTempFunc = new_tr PgStumblebumSkillFunction;				} break;	//분노의외침		
		case 107000301 :						{ kTempFunc = new_tr PgWideHealSkillFunction;				} break;	//와이드 힐
		case 107000501 :						{ kTempFunc = new_tr PgCureSkillFunction;					} break;	//큐어
		case 108000101 :						{ kTempFunc = new_tr PgBlizzardSkillFunction;				} break;	//블리자드
		case 109000401 :						{ kTempFunc = new_tr PgQuagmireSkillFunction;				} break;	//콰그마이어		
		case 109002301 :						{ kTempFunc = new_tr PgSnowBluesFunction;					} break;	//스노우브루스
		case 109001001 :						{ kTempFunc = new_tr PgResurrectionSkillFunction;			} break;	//부활
		case 109001501 :						{ kTempFunc = new_tr PgChainLightingSkillFunction;			} break;	//체인 라이트닝
		case 109001601 :						{ kTempFunc = new_tr PgHPRestoreSkillFunction;				} break;	//HP리스토어
		case 109001701 :						{ kTempFunc = new_tr PgMPTransitionSkillFunction;			} break;	//MP 전이
		case 109001801 :						{ kTempFunc = new_tr PgBarrierSkillFunction;				} break;	//배리어				
		case 109002101 :						{ kTempFunc = new_tr PgResourceConverterSkillFunction;		} break;	//리소스 컨버터				
		case 110000401 :						{ kTempFunc = new_tr PgBleedingSkillFunction;				} break;	//모서리로 찍으세요
		case 110001501 :						{ kTempFunc = new_tr PgAutoShootSystemFunction;				} break;	//자동사격시스템
		case 150000601 :						{ kTempFunc = new_tr PgFlashBangSkillFunction;				} break;	//플래시뱅
		case 150000901 :						{ kTempFunc = new_tr PgAtropineSkillFunction;				} break;	//아트로핀
		case 150001101 :						{ kTempFunc = new_tr PgCarpetBombingFunction;				} break;	//융단폭격
		case 150001701 :						{ kTempFunc = new_tr PgValcan300Function;					} break;	//발칸사격
		case 160000501 :						{ kTempFunc = new_tr PgShoutSkillFunction;					} break;	//기합
		case 170000301 :						{ kTempFunc = new_tr PgSleepSmellSkillFunction;				} break;	//미혼향		
		case 1100028011 :						{ kTempFunc = new_tr PgMPZeroTrapSkillFunction;				} break;	//mp제로트랩
		case 1090004011 :						{ kTempFunc = new_tr PgQuagmireSetSkillFunction;			} break;	//콰그마이어 설치
		case 2000410101 :						{ kTempFunc = new_tr PgMagmaPistonFunction;					} break;	//마그마피스톤
		case 2000820101 :						{ kTempFunc = new_tr PgShadowBladeFunction;					} break;	//쉐도우블레이드 닌자 소환
		case 2009810101 :						{ kTempFunc = new_tr PgPhantomCloneSkillFunction;			} break;	//(궁극기) 환영 분신술
		case 2000302001 :						{ kTempFunc = new_tr PgHealingWaveFunction;					} break;	//힐링 웨이브
		case 90000701	:						{ kTempFunc = new_tr PgCoupleVicinityEffectFunction;		} break;	//커플 스킬, 꿩먹고 알먹고
		case 300101101 :						{ kTempFunc = new_tr PgSummonFunction;						} break;	//소환체 소환
		case 300101001 :						{ kTempFunc = new_tr PgReGainSummonedSkillFunction;			} break;	//소환체 전체를 없애고 남은 체력을 마나로 환원해줌
		case 300200201 :						{ kTempFunc = new_tr PgReCallSummonedSkillFunction;			} break;	//소환체를 유닛 주변으로 순간이동
		case 300301101 :						{ kTempFunc = new_tr PgEffectTargetSummonedSkillFunction;	} break;	//소환체에게 이펙트 적용
		case 3531100 :							{ kTempFunc = new_tr PgRollingTargetingSkillFunction;		} break;	//순차적으로 이동가능한 타겟팅 대상들의 위치를 구하고 스킬 발동
		case 3531110 :							{ kTempFunc = new_tr PgSummonShotEntitySkillFunction;		} break;	//엔티티 소환(클라에 위치정보 패킷 전달)
		case 3542219 :							{ kTempFunc = new_tr PgRandomSummonMonsterSkillFunction;	} break;	//랜덤위치에 몬스터 소환
		case 3542230 :							{ kTempFunc = new_tr PgElgaSummonEyeSkillFunction;			} break;	//엘가 눈동자 소환 전용 스킬
		case 3542329 :							{ kTempFunc = new_tr PgElga03_GroggySkillFunction;			} break;	//엘가3차 그로기 상태 스킬
		case 300300401 :						{ kTempFunc = new_tr PgAttackSummonedSkillFunction;			} break;	//소환체에게 특정 몹을 집중 공격
		case 5872200:							{ kTempFunc = new_tr PgKOH_area_bomb;						} break;	//지역에 있는 가디언 중립, 적 유닛 죽이기
		case 7282700:							{ kTempFunc = new_tr PgComboAttackSkillFunction;			} break;	//콤보 공격
		case 7281645:							{ kTempFunc = new_tr PgTeamPlaySkillFunction;				} break;	//협동 스킬
		case 7282109:							{ kTempFunc = new_tr PgHydraEarthWrathSkillFunction;		} break;	//히드라 대지의 분노
		default :
			{
				kTempFunc = new_tr PgDefaultSkillFunction;
			}break;
		}

//		ret.first->second = kTempFunc;
	}

//	return ret.first->second;
	if( !kTempFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return kTempFunc;

}

///////////////////////////////////////////////////////////
//  PgFunctionTypeXmlLoader
///////////////////////////////////////////////////////////
PgFunctionTypeXmlLoader::PgFunctionTypeXmlLoader()
{}

PgFunctionTypeXmlLoader::~PgFunctionTypeXmlLoader()
{}

bool PgFunctionTypeXmlLoader::ParseXml(std::wstring const &strXmlPath)
{
	//! XML에는 예외적인 것들만 추가 한다.
	if(!m_kConFuncType.empty())
	{
		m_kConFuncType.clear();
	}

	TiXmlDocument kXmlDoc(MB(strXmlPath.c_str()));

	if(!kXmlDoc.LoadFile())
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Failed parse xml ["<<strXmlPath<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlElement* pkNode = kXmlDoc.FirstChildElement("FUNCTION_NO");
	
	pkNode = pkNode->FirstChildElement();
	while(pkNode)
	{
		char const* szName = pkNode->Value();
		if(0 == strcmp(szName, "FUNCTION"))
		{
			int iNo = 0;
			int iFunctionType = 0;

			const TiXmlAttribute* pkAttr = pkNode->FirstAttribute();
			while(pkAttr)
			{				
				char const* pcAttrName = pkAttr->Name();
				char const* pcAttrValue = pkAttr->Value();

				if(0 == _stricmp(pcAttrName, "No"))
				{
					iNo = atoi(pcAttrValue);
				}
				else if(0 == _stricmp(pcAttrName, "FUNCTION_TYPE"))
				{
					iFunctionType = atoi(pcAttrValue);
				}

				pkAttr = pkAttr->Next();
			}

			if ( iNo )
			{
				auto ret = m_kConFuncType.insert(std::make_pair(iNo, iFunctionType));
				if(!ret.second)
				{
					INFO_LOG(BM::LOG_LV0, __FL__<<L"No["<<iNo<<L"] is Existence");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"No is 0 or Not Found \"No\" Error!!!");
			}
		}

		pkNode = pkNode->NextSiblingElement();
	}

	return true;
}
int PgFunctionTypeXmlLoader::GetFuncType(int const iNo) const
{
	CONT_FUNC_TYPE_TABLE::const_iterator func_itor =  m_kConFuncType.find(iNo);
	if(m_kConFuncType.end() != func_itor)
	{
		return (*func_itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0; //! 없으면 Default
}
