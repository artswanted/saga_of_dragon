#include "stdafx.h"
#include "PgEffectAbilHandleManager.h"
#include "PgEffectFunction.h"
#include "PgEffectFunction_MapEntity.h"
#include "PgEffectFunction_Item.h"
#include "PgEffectFunction_StatusEffect.h"
#include "PgEffectFunction_Guild.h"
#include "PgEffectFunction_Fighter.h"
#include "PgEffectFunction_Archer.h"
#include "PgEffectFunction_Magician.h"
#include "PgEffectFunction_Thief.h"
#include "PgSkillAbilHandleManager.h"
#include "PgEffectFunction_Monster.h"
#include "PgEffectFunction_Object.h"
#include "PgEffectFunction_Pet.h"

///////////////////////////////////////////////////////////
//  PgAbilTypeHandleTable
///////////////////////////////////////////////////////////
PgEffectAbilHandleManager::PgEffectAbilHandleManager()
{
}

PgEffectAbilHandleManager::~PgEffectAbilHandleManager()
{
	Release();
}

void PgEffectAbilHandleManager::Init()
{
	m_kAbilTypeTable.Init();
}

void PgEffectAbilHandleManager::Release()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_EFFECT_FUNC::iterator effect_itor = m_kConEffect.begin();
	while(effect_itor != m_kConEffect.end())
	{
		SAFE_DELETE((*effect_itor).second);
		effect_itor = m_kConEffect.erase(effect_itor);
	}

	m_kConEffect.clear();
}

bool PgEffectAbilHandleManager::Build()
{	
	bool bReturn = true;
	BM::CAutoMutex kLock(m_kMutex, true);
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Start");

	PgFunctionTypeXmlLoader kFuncType;
	kFuncType.ParseXml(_T("XML/EffectFuncType.xml"));
	
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDefMgr::CONT_DEF kContEffectDef;
	kEffectDefMgr.GetCont(kContEffectDef);

	CEffectDefMgr::CONT_DEF::const_iterator effectDef_itor = kContEffectDef.begin();
	while(kContEffectDef.end() != effectDef_itor)
	{
		int const iEffectNo = (*effectDef_itor).first.kKey;
		CEffectDef const* pkEffectDef = (*effectDef_itor).second;

		PgIEffectFunction* kTempFunc = NULL;
		auto ret = m_kConEffect.insert(std::make_pair(iEffectNo, kTempFunc));

		if(ret.second)
		{
			PgIEffectFunction* pkEffectFunc = NULL;
			int const iCallFunctionType = kFuncType.GetFuncType(iEffectNo);

			pkEffectFunc = CreateEffectFunc(iCallFunctionType);

			if(pkEffectFunc)
			{
				pkEffectFunc->Init();
				pkEffectFunc->Build(&m_kAbilTypeTable, pkEffectDef);
				ret.first->second = pkEffectFunc;
			}
			else
			{
				//나오면 안되는 로그(New가 실패 했다는 Log)
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"PgIEffectFunction New Failed "<<iEffectNo);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffectFunc is NULL"));
				bReturn = false;
			}
		}

		++effectDef_itor;
	}
	INFO_LOG(BM::LOG_LV9, __FL__<<L"End");
	return bReturn;
}

void PgEffectAbilHandleManager::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg) const
{
	if(NULL == pkEffect)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffect is NULL Return"));
		return ;
	}

	BM::CAutoMutex kLock(m_kMutex);

	CONT_EFFECT_FUNC::const_iterator itor = m_kConEffect.find(pkEffect->GetEffectNo());

	if(itor != m_kConEffect.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkEffect->GetEffectNo());
		(*itor).second->EffectBegin(pkUnit, pkEffect, pArg);
		return;
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find EffectNo["<<pkEffect->GetEffectNo()<<L"] Func");
}

void PgEffectAbilHandleManager::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg) const
{
	if(NULL == pkEffect)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffect is NULL Return"));
		return;
	}

	BM::CAutoMutex kLock(m_kMutex);
	CONT_EFFECT_FUNC::const_iterator itor = m_kConEffect.find(pkEffect->GetEffectNo());

	if(itor != m_kConEffect.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkEffect->GetEffectNo());
		(*itor).second->EffectEnd(pkUnit, pkEffect, pArg);
		return;
	}

	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find EffectNo["<<pkEffect->GetEffectNo()<<L"] Func");
}

int PgEffectAbilHandleManager::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg, DWORD const dwElapsed) const
{
	if(NULL == pkEffect)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffect is NULL Return ECT_NONE"));
		return ECT_NONE;
	}

	BM::CAutoMutex kLock(m_kMutex);
	CONT_EFFECT_FUNC::const_iterator itor = m_kConEffect.find(pkEffect->GetEffectNo());

	if(itor != m_kConEffect.end())
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s] No[%d]"), __FUNCTIONW__,pkEffect->GetEffectNo());
		return (*itor).second->EffectTick(pkUnit, pkEffect, pArg, dwElapsed);
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot Find EffectNo<") << pkEffect->GetEffectNo() << _T("> Function") );	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_NONE"));
	return ECT_NONE;
}

int PgEffectAbilHandleManager::EffectReset(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pArg, DWORD const dwElapsed) const
{
	if ( NULL == pkEffect )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffect is NULL Return ECT_NONE"));
		return ECT_NONE;
	}

	BM::CAutoMutex kLock(m_kMutex);

	CONT_EFFECT_FUNC::const_iterator itr = m_kConEffect.find(pkEffect->GetEffectNo());
	if(itr != m_kConEffect.end())
	{
		return itr->second->EffectReset(pkUnit, pkEffect, pArg, dwElapsed);
	}

	INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot Find EffectNo<") << pkEffect->GetEffectNo() << _T("> Function") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_NONE"));
	return ECT_NONE;
}

PgIEffectFunction* PgEffectAbilHandleManager::CreateEffectFunc(int const iFuncType)const
{
	PgIEffectFunction* pkEffectFunc = NULL;
	switch(iFuncType)
	{
	case EFFECTNO_DEFAULT :			{ pkEffectFunc = new_tr PgDefaultEffectFunction;		}break;
	case EFFECTNO_MAX_HP_AND_MP:	{ pkEffectFunc = new_tr PgMaxHpAndMPEffectFunction;		}break;
	case 2501:						{ pkEffectFunc = new_tr PgVenomFunction;				}break;	// 거미독
	case 2601:						{ pkEffectFunc = new_tr PgIncreaseSightFunction;		}break;	// 시야증가
	case 11001:						{ pkEffectFunc = new_tr PgMissionPenaltyFunction;		}break;	// 미션 페널티
	case EFFECTNO_CANNOT_DAMAGE:	{ pkEffectFunc = new_tr PgCannotDamageFunction;			}break;	// 부활 무적
	case 12001:						{ pkEffectFunc = new_tr PgItemEffectFunction;			}break;	// 아이템 이펙트
	case 12101:						{ pkEffectFunc = new_tr PgSafetyCapFunction;			}break;	// 병아리 안전모
	case 13001:						{ pkEffectFunc = new_tr PgDefenceCorrectionFunction;	}break;	// PvP 방어력 보정
	case EFFECTNO_PVP_HANDYCAP:		{ pkEffectFunc = new_tr PgHandyCapFunction;				}break;	// PvP Handy Cap
	case EFFECTNO_MUTECHAT:			{ pkEffectFunc = new_tr PgMuteChat;						}break;	// 채팅금지
	case 25501:			{ pkEffectFunc = new_tr PgHPMPRestoreEffectFunction;		}break; // MAXP의 %로 회복하는 이펙트
	case 300001:		{ pkEffectFunc = new_tr PgParelPoisonAura;				}break; // 파렐경 맵에 엔티티가 사용하는 독 오오라
	case 300201:		{ pkEffectFunc = new_tr PgChaosMapAuraEffect;			}break; // 카오스 맵에 엔티티가 사용하는 디버프 오오라
	case 302401:		{ pkEffectFunc = new_tr PgRestEffectFunction;			}break; // 휴식의자에서 사용하는 이펙트		
	case 302402:		{ pkEffectFunc = new_tr PgRestExpEffectFunction;		}break; // 휴식 경험치 이펙트
	case 303801:		{ pkEffectFunc = new_tr PgLoveBalloonEffectFunction;	}break; // 사랑의 열기구 실제 이펙트 거는 이펙트(오오라 같은 것)		
	case 3000011:		{ pkEffectFunc = new_tr PgParelPoison;					}break; // 파렐경 엔티티가 건 독 효과
	case 3000101:		{ pkEffectFunc = new_tr PgChaosMapEffect;				}break; // 카오스 엔티티가 건 디버프 효과
	case 300101:		{ pkEffectFunc = new_tr PgGasMaskEffectFunction;		}break; // 방독면 이펙트(파렐경 엔티티 독 효과 중화)
	case 300401:		{ pkEffectFunc = new_tr PgPotionEffectFunction;			}break; // 포션 아이템 효과
	case 403701:		{ pkEffectFunc = new_tr PgGaugeDecreaseValEffectFunction;}break;	// 마력포 무적 해제 할 수 있도록 이끌어 주는 이펙트
	case 531301:		{ pkEffectFunc = new_tr PgReviveEffectFunction;			}break;	// 소생 이펙트
	case 531401:		{ pkEffectFunc = new_tr PgAddEffectDurationTimeFunction;}break;	// 이펙트 유지시간 증감
	case 532101:		{ pkEffectFunc = new_tr PgEffectApplyArea;				}break;	// 지역 적용 이펙트
	case 6000914:		{ pkEffectFunc = new_tr PgDependAbilTickEffectFunction;	}break; // 마력포 무적 이펙트
	case 6009401:		{ pkEffectFunc = new_tr PgBlockingFunction;				}break;	// 블럭킹
	case 6013401:		{ pkEffectFunc = new_tr PgNetBinderFunction;			}break; // 넷바인더
	case 6044603:		{ pkEffectFunc = new_tr PgAutoTickEffectFunction;		}break; // 벨라 자신에게 독걸기
	case 6000900:		{ pkEffectFunc = new_tr PgLavalonBreathFunction;		}break; // 라발론 브레스
	case 6091200:		{ pkEffectFunc = new_tr PgMetaMorphosisEffectFunction;	}break; // 몬스터 변신 버프
	case 9020200:		{ pkEffectFunc = new_tr PgHardDungeonEffectFunction;	}break;	// 하드 던전 몬스터(이펙트) 몬스터 능력치 향상 후 HP/MP max로 올려주는 이펙트
	case 9030500:		{ pkEffectFunc = new_tr PgDontJumpFunction;				}break;	// 점프 못하게	
	case 80000101:		{ pkEffectFunc = new_tr PgAutoLootByPetFunction;		}break; // 이건 내꺼(펫)
	case 80001101:		{ pkEffectFunc = new_tr PgNurseByPetFunction;			}break; // 간호(펫)
	case 80010001:		{ pkEffectFunc = new_tr PgSafeBubbleExtFunction;		}break;	// 안전거품, 펫에게 능력치 주기
	case 80010101:		{ pkEffectFunc = new_tr PgMetaAblityToPetFunction;		}break;	// 펫에게 능력치 전이
	case EFFECTNO_PETSKILL_ACTIVATE_BASE:{ pkEffectFunc = new_tr PgActivateSkillPetFunction;	}break;// 펫 스킬 활성화
	case 90000001:		{ pkEffectFunc = new_tr PgLovePowerFunction;			}break; // 사랑의 힘으로
	case 100007101:		{ pkEffectFunc = new_tr PgChangeMahalkaEffectFunction;	}break; // 마할카 변신(퀘스트)		
	case 110001001:		{ pkEffectFunc = new_tr PgAcidArrowEffectFunction;		}break; // 애시드 애로우
	case 105500901:		{ pkEffectFunc = new_tr PgPaladinAura;					}break; // 오라 발동
	case 150000901:		{ pkEffectFunc = new_tr PgAtropineFireFunction;			}break; // 아트로핀 발동	
	case 1500009101:	{ pkEffectFunc = new_tr PgAtropineEffectFunction;		}break; // 아트로핀 효과		 
	case 110002501:		{ pkEffectFunc = new_tr PgDonTouchMeFunction;			}break; // 건들지마라
	case 101020101:		{ pkEffectFunc = new_tr PgStunStatusFunction;			}break; // 기절
	case 100010001:		{ pkEffectFunc = new_tr PgBleedStatusFunction;			}break; // 출혈
	case 100010101:		{ pkEffectFunc = new_tr PgBleedStatusFunction;			}break; // 과다 출혈
	case 100010201:		{ pkEffectFunc = new_tr PgCurseStatusFunction;			}break; // 저주
	case 100010301:		{ pkEffectFunc = new_tr PgColdStatusFunction;			}break; // 동빙
	case 100010401:		{ pkEffectFunc = new_tr PgColdStatusFunction;			}break; // 결빙	
	case 100010501:		{ pkEffectFunc = new_tr PgSilenceStatusFunction;		}break; // 침묵
	case 100010601:		{ pkEffectFunc = new_tr PgPoisonStatusFunction;			}break; // 중독
	case 100010701:		{ pkEffectFunc = new_tr PgPoisonStatusFunction;			}break; // 심각한 중독
	case 100010801:		{ pkEffectFunc = new_tr PgSleepStatusFunction;			}break; // 수면
	case 100010901:		{ pkEffectFunc = new_tr PgFleshtoStoneStatusFunction;	}break; // 석화
	case 100011001:		{ pkEffectFunc = new_tr PgBurnStatusFunction;			}break; // 화상
	case 100011101:		{ pkEffectFunc = new_tr PgBurnStatusFunction;			}break; // 심각한 화상
	case 100011201:		{ pkEffectFunc = new_tr PgBlindStatusFunction;			}break; // 실명
	case 100011501:		{ pkEffectFunc = new_tr PgSlowStatusFunction;			}break; // 슬로우
	case 102000201:		{ pkEffectFunc = new_tr PgTransformationEffectFunction;	}break; // 마녀의 저주		
	case 102000401:		{ pkEffectFunc = new_tr PgBurberrianEffectFunction;		}break; // 바바리맨	
	case 102000701:		{ pkEffectFunc = new_tr PgManaShieldEffectFunction;		}break; // 마나실드
	case 102000801:		{ pkEffectFunc = new_tr PgConcentrationEffectFunction;	}break; // 정신일도하사불성
	case 103000301:		{ pkEffectFunc = new_tr PgBeautifulGirlFunction;		}break; // 멋진언니
	case 103000401:		{ pkEffectFunc = new_tr PgIncreasedStatusFunction;		}break; // 버드 워칭
	case 103300101:		{ pkEffectFunc = new_tr PgBowMasteryFunction;			}break; // 보우마스터리
	case 110000401:		{ pkEffectFunc = new_tr PgBleedingFunction;				}break; // 모서리로 찍으세요
	case 104300201:		{ pkEffectFunc = new_tr PgStripWeaponEffectFunction;	}break; // 스트립 웨폰
	case 104000201:		{ pkEffectFunc = new_tr PgExitEffectFunction;			}break; // 보고도 못본척
	case 104000601:		{ pkEffectFunc = new_tr PgBurrowEffectFunction;			}break; // 버로우	
	case 104301301:		{ pkEffectFunc = new_tr PgShadowWalkEffectFunction;		}break; // 쉐도우 워크
	case 105300601:		{ pkEffectFunc = new_tr PgRevengeEffectFunction;		}break; // 리벤지
	case 105501301:		{ pkEffectFunc = new_tr PgHolyArmorEffectFunction;		}break; // 성스러운 보호
	case 105501801:		{ pkEffectFunc = new_tr PgTauntEffectFunction;			}break; // 도발
	case 106300301:		{ pkEffectFunc = new_tr PgAnnihilationEffectFunction;	}break;	// 섬멸			
	case 106501201:		{ pkEffectFunc = new_tr PgBloodyLoreEffectFunction;		}break; // 블러디 로어	
	case 107000501:		{ pkEffectFunc = new_tr PgCureEffectFunction;			}break; // 큐어(바리케이트에서 사용)
	case 107000701:		{ pkEffectFunc = new_tr PgDiseaseEffectFunction;		}break; // 감염(법사)	
	case 109001401:		{ pkEffectFunc = new_tr PgThunderBreakEffectFunction;	}break; // 썬더브레이크			
	case 109001501:		{ pkEffectFunc = new_tr PgChainLightingEffectFunction;	}break; // 체인라이트닝
	case 109001601:		{ pkEffectFunc = new_tr PgHPRestoreEffectFunction;		}break; // HP리스토어		
	case 109001801:		{ pkEffectFunc = new_tr PgBarrierEffectFunction;		}break; // 배리어		
	case 109002001:		{ pkEffectFunc = new_tr PgMirrorImageEffectFunction;	}break; // 미러이미지	
	case 110001701:		{ pkEffectFunc = new_tr PgCamouflageEffectFunction;		}break; // 카모플라쥬
	case 170001101:		{ pkEffectFunc = new_tr PgChangeNinjaEffectFunction;	}break; // 닌자 변신
	case 1055013011:	{ pkEffectFunc = new_tr PgAuraTargetEffectFunction;		}break; // 오오라 타겟효과
	case 1090028011:	{ pkEffectFunc = new_tr PgSantuaryEffectFunction;		}break; // 생츄어리 효과
	case 1100029011:	{ pkEffectFunc = new_tr PgSmokeGranadeActiveEffectFunction;}break; // 스모크그레네이드 효과	
	case 1500008011:	{ pkEffectFunc = new_tr PgWPGranadeFireEffectFunction;	}break; // WP그레네이드 설치
	case 2000802001:	{ pkEffectFunc = new_tr PgProtectEdgeEffectFunction;	}break; // 프로텍트엣지
	case 90000301:		{ pkEffectFunc = new_tr PgRecoveryPoolEffect;			}break;// 회복우물
	case 90000401:		{ pkEffectFunc = new_tr PgCoupleRingEffectFunction;		}break; // 커플링
	case 2000000001:	{ pkEffectFunc = new_tr PgAwakeSystemEffectFunction;	}break; // 각성 시스템		
	case 2000000101:	{ pkEffectFunc = new_tr PgAwakeNormalStateEffectFunction;	}break; // 각성 게이지 일반
	case 2000000201:	{ pkEffectFunc = new_tr PgAwakeMaxStateEffectFunction;		}break; // 각성 게이지 맥스
	case 2000000301:	{ pkEffectFunc = new_tr PgAwakePenaltyStateEffectFunction;	}break; // 각성 게이지 페널티
	case 900001	   :	{ pkEffectFunc = new_tr PgSkillDmgByPhaseEffectFunction;	}break; // 단계별 데미지 조절 이펙트
	case 2000002001	:	{ pkEffectFunc = new_tr PgAwakeChargeEffectFunction;	}break; // 각성 챠지
	case 455001	:		{ pkEffectFunc = new_tr PgHealFromHitDamageEffectFunction;	}break; // 때린 데미지로 인해 힐 받기
	case 455101	:		{ pkEffectFunc = new_tr PgDeleteBuffEffectFunction;	}break; // 버프 하나를 지우고 사라지는 이펙트
	case 1000001 :		{ pkEffectFunc = new_tr PgJobSkillRecoveryFunction; }break; // 채집 회복 이펙트
	case 159999901:		{ pkEffectFunc = new_tr PgSkillLinkageEffectFunction;		}break;	// 연계 스킬 이펙트 번호
	case 300303901:		{ pkEffectFunc = new_tr PgDrainEffect;		}break;	// HP, MP 흡수
	case 3531040:		{ pkEffectFunc = new_tr PgEffectHealToTargetFunction;		}break;	// 특정몬스터의 HP 수정
	case 3531050:		{ pkEffectFunc = new_tr PgElga_room_of_heart;				}break;	// 엘가 심장의 방 엔티티가 사용하는 이펙트
	case 3531060:		{ pkEffectFunc = new_tr PgTeleportToDummyEffect;			}break;	// 플레이어 더미 위치로 강제 이동
	case 3531070:		{ pkEffectFunc = new_tr PgElgaStunStatusFunction;			}break; // 엘가기절
	case 3531080:		{ pkEffectFunc = new_tr PgElgaDealingTimeEffectFunction;	}break; // 엘가2차 공중폭격시 딜링타임동안 적용될 이펙트
	case 3531090:		{ pkEffectFunc = new_tr PgTargetToDeleteEffectFunction;		}break; // 타겟의 이펙트를 제거한다
	case 5872000:		{ pkEffectFunc = new_tr PgKingOfHill_Guardian;				}break; // 점령전 가디언의 범위안 플레이어 점수 계산되는 이펙트
	case 464801:		{ pkEffectFunc = new_tr PgKingOfHill_Bomberman;				}break; // 점령전 봄버맨
	case 89006930:		{ pkEffectFunc = new_tr PgFilterExceptEffectFunction;		}break; // 지속적인 디버프 제거기능
	case 90001:			{ pkEffectFunc = new_tr PgRangeBySkillFireEffectFunction;	}break; // 범위안에 들어오면 지정된 유닛이 스킬을 발동하도록 하는 이펙트
	case 7281660:		{ pkEffectFunc = new_tr PgDistanceWallFunction;				}break; // 유닛과 유닛2의 거리에 장벽을 만들어 검출되는 타겟에게 데미지를 줌
	case 9980100:		{ pkEffectFunc = new_tr PgRandomAttachEffectFunction;		}break; // 랜덤하게 이펙트를 붙임
	case 9980400:		{ pkEffectFunc = new_tr PgSummonEntityEffectFunction;		}break; // 엔티티 소환
	case 7903300:		{ pkEffectFunc = new_tr PgChangeAIEffectFunction;			}break; // AI 변경 이펙트
	default :			{ pkEffectFunc = new_tr PgDefaultEffectFunction;		}break;
	}
	
	if( !pkEffectFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffectFunc is NULL"));
	}

	return pkEffectFunc;
}
