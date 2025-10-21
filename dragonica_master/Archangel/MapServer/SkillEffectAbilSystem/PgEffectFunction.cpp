#include "stdafx.h"
#include "Variant/Global.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgParty.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction.h"
#include "Global.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

///////////////////////////////////////////////////////////
//  Abil Function
///////////////////////////////////////////////////////////
void OnAddAbil( CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType )
{
	if(0 != iValue)
	{
		PgUnitEffectMgr &rkEffectMgr = pkUnit->GetEffectMgr();
		if ( true == IsCountAbil( wType ) )
		{
			::AddCountAbil( dynamic_cast<CAbilObject*>(&rkEffectMgr), wType, AT_CF_EFFECT, iValue > 0 );
		}
		else
		{
			// AT_CALCUATEABIL_MIN ~ AT_CALCUATEABIL_MAX 사이의 값들은
			// AT_XX / AT_R_XX / AT_C_XX 로 나눠지는데
			// 그에 따른 계산이 적용된 값을 확인 하려면 C값을 전 후로 확인해주어야 한다.
			WORD wCalcType = ::GetCalculateAbil(wType);
			if(0 == wCalcType)
			{
				wCalcType = wType;
			}

			//Abil 적용 되기 전 AT_XXXX값
			//INFO_LOG(BM::LOG_LV9, _T("[%s] Before  Type[%d] Vaule[%d]"), __FUNCTIONW__, wCalcType, pkUnit->GetAbil(wCalcType));
			//INFO_LOG(BM::LOG_LV9, _T("[%s] AddAbil Type[%d] Value[%d]"), __FUNCTIONW__, wType, iValue);

			//Abil 적용
			rkEffectMgr.AddAbil( wType, iValue );
		}
		
		pkUnit->NftChangedAbil(wType, dwSendType);
		if( AT_R_MOVESPEED==wType && 0<iValue )
		{
			pkUnit->SetAbil(AT_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_MOVESPEED));
		}
		else if(AT_R_VILLAGE_MOVESPEED==wType && 0<iValue )
		{
			pkUnit->SetAbil(AT_VILLAGE_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED));
		}
		//Abil 적용 된 후 AT_XXXX값
		//INFO_LOG(BM::LOG_LV9, _T("[%s] After   Type[%d] Value[%d]"), __FUNCTIONW__, wCalcType, pkUnit->GetAbil(wCalcType));
	}
}

void OnSetAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType)
{
	//Abil 적용 되기 전 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Before  Type[%d] Vaule[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
	//INFO_LOG(BM::LOG_LV9, _T("[%s] AddAbil Type[%d] Value[%d]"), __FUNCTIONW__, wType, iValue);

	//Abil 적용
	pkUnit->SetAbil( wType, iValue, (E_SENDTYPE_SELF & dwSendType), (E_SENDTYPE_BROADCAST & dwSendType) );

	if( AT_R_MOVESPEED==wType && pkUnit->GetAbil(AT_C_MOVESPEED)<iValue )
	{
		pkUnit->SetAbil(AT_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_MOVESPEED));
	}
	else if( AT_R_VILLAGE_MOVESPEED==wType && pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED)<iValue )
	{
		pkUnit->SetAbil(AT_VILLAGE_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED));
	}

	//Abil 적용 된 후 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] After   Type[%d] Value[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
}

void OnSetAbil2(CUnit* pkUnit, WORD const wType, int const iValue)
{
	// AT_CALCUATEABIL_MIN ~ AT_CALCUATEABIL_MAX 사이의 값들은
	// AT_XX / AT_R_XX / AT_C_XX 로 나눠지는데
	// 그에 따른 계산이 적용된 값을 확인 하려면 C값을 전 후로 확인해주어야 한다.
	WORD wCalcType = GetCalculateAbil(wType);
	if(0 == wCalcType)
	{
		wCalcType = wType;
	}

	//Abil 적용 되기 전 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Before  Type[%d] Vaule[%d]"), __FUNCTIONW__, wCalcType, pkUnit->GetAbil(wCalcType));
	//INFO_LOG(BM::LOG_LV9, _T("[%s] AddAbil Type[%d] Value[%d]"), __FUNCTIONW__, wType, iValue);

	//Abil 적용
	pkUnit->GetEffectMgr().SetAbil(wType, iValue);
	pkUnit->NftChangedAbil(wType, E_SENDTYPE_BROADALL_EFFECTABIL);

	if( AT_R_MOVESPEED==wType && pkUnit->GetAbil(AT_C_MOVESPEED)<iValue )
	{
		pkUnit->SetAbil(AT_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_MOVESPEED));
	}
	else if( AT_R_VILLAGE_MOVESPEED==wType && pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED)<iValue )
	{
		pkUnit->SetAbil(AT_VILLAGE_MOVESPEED_SAVED, pkUnit->GetAbil(AT_C_VILLAGE_MOVESPEED));
	}

	//Abil 적용 된 후 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] After   Type[%d] Value[%d]"), __FUNCTIONW__, wCalcType, pkUnit->GetAbil(wCalcType));
}

void OnSetNullAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType)
{
	//iValue = 사용하지 않음 interface상 받아 온다.

	//Abil 적용 되기 전 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Before  Type[%d] Vaule[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
	//INFO_LOG(BM::LOG_LV9, _T("[%s] AddAbil Type[%d] Value[%d]"), __FUNCTIONW__, wType, iValue);

	//Abil 적용 현재 값을 얻어서 0으로 만든다.
	int const iNowValue = pkUnit->GetAbil(wType);
	if(0 != iNowValue)
	{
		pkUnit->SetAbil(wType, 0, (E_SENDTYPE_SELF & dwSendType), (E_SENDTYPE_BROADCAST & dwSendType));
	}

	//Abil 적용 된 후 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] After   Type[%d] Value[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
}

void OnSetNullAbil2(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType )
{
	//iValue = 사용하지 않음 interface상 받아 온다.

	//Abil 적용 되기 전 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Before  Type[%d] Vaule[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
	//INFO_LOG(BM::LOG_LV9, _T("[%s] AddAbil Type[%d] Value[%d]"), __FUNCTIONW__, wType, iValue);

	//Abil 적용
	pkUnit->GetEffectMgr().SetAbil(wType, 0);
	pkUnit->NftChangedAbil(wType, dwSendType);


	//Abil 적용 된 후 AT_XXXX값
	//INFO_LOG(BM::LOG_LV9, _T("[%s] After   Type[%d] Value[%d]"), __FUNCTIONW__, wType, pkUnit->GetAbil(wType));
}

void OnAddAbil_Attack(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType )
{
	if( AT_ENABLE_CHECK_ATTACK == wType )
	{
		int const iNowValue = pkUnit->GetAbil(AT_CHECK_ATTACK_COUNT);
		if( 0 != iNowValue )
		{
			OnSetAbil(pkUnit, wType, 0, dwSendType);
			OnSetAbil(pkUnit, AT_CHECK_ATTACK_COUNT, 0, dwSendType);
		}
	}
}

void OnAddAbil_MaxHP_MaxMP(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType)
{
	bool bExec = false;
	int iNowType = 0;
	int iCurMaxType = 0;

	if(AT_R_MAX_HP == wType || AT_MAX_HP == wType)
	{
		bExec = true;
		iNowType = AT_HP;
		iCurMaxType = AT_C_MAX_HP;
	}
	else if(AT_R_MAX_MP == wType || AT_MAX_MP == wType)
	{
		bExec = true;
		iNowType = AT_MP;
		iCurMaxType = AT_C_MAX_MP;
	}

	if(bExec)
	{
		OnAddAbil(pkUnit, wType, iValue, dwSendType);

		// 최대 HP/MP가 줄어드는 경우만 검사
		if(0 > iValue)
		{
			int const iNowValue = pkUnit->GetAbil(iNowType);
			int const iNowMaxValue = pkUnit->GetAbil(iCurMaxType);
			if(iNowValue > 0)
			{// Max HP, Max MP가 변하는 경우는
				if(iNowValue > iNowMaxValue)
				{// 해당 실값을 갱신하여
					pkUnit->SetAbil(iNowType, iNowMaxValue);
				}
				// 다시 보내준다.				
				pkUnit->SendAbil(static_cast<EAbilType>(iNowType), E_SENDTYPE_BROADALL);// 주의
			}			
		}
		else
		{
			if(0 < pkUnit->GetAbil(iNowType))
			{
				pkUnit->SendAbil(static_cast<EAbilType>(iNowType), E_SENDTYPE_BROADALL);// 주의
			}
		}
	}
}

// iValue에 있는 이펙트의 어빌들을 직접 세팅
void OnAddEffectAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType)
{
	int iEffectNo = iValue;
	if(0 > iValue)
	{
		// +로 바꾸어 준다.
		iEffectNo *= -1;
	}

	if(0 != iEffectNo)
	{
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		CEffectDef const* pkAddEffect = kEffectDefMgr.GetDef(iEffectNo);
		if(0 >= pkAddEffect)
		{
			return;
		}

		SAbilIterator kItor;
		pkAddEffect->FirstAbil(&kItor);
		while ( pkAddEffect->NextAbil(&kItor) )
		{
			// 0보다 큰 값이면 Begin에서 호출
			if(0 < iValue)
			{
				OnAddAbil(pkUnit, kItor.wType, kItor.iValue, dwSendType);
			}
			// 0보다 작은 값이면 End에서 호출
			else
			{
				OnAddAbil(pkUnit, kItor.wType, -kItor.iValue, dwSendType);
			}
		}
	}
}

void OnExecAbil_TTW_MSG(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType )
{
	int iSendTTW = iValue;

	if(0 > iValue)
	{
		iSendTTW = -iValue; // End에서는 -값으로 오기 때문에 반대로 바꾸어 준다.
	}
	
	pkUnit->SendWarnMessage(iSendTTW);
}

void OnHPMPDec_MaxHP_MaxMP_Rate(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType)
{
	bool bExec = false;
	int iNowType = 0;
	int iCurMaxType = 0;

	if(AT_HP_DEC_MAX_HP_RATE == wType)
	{
		bExec = true;
		iNowType = AT_HP;
		iCurMaxType = AT_C_MAX_HP;
	}
	else if(AT_MP_DEC_MAX_MP_RATE == wType)
	{
		bExec = true;
		iNowType = AT_MP;
		iCurMaxType = AT_C_MAX_MP;
	}

	if(bExec)
	{
		// 최대 HP/MP가 줄어드는 경우만 검사
		if(0 != iValue)
		{
			int const iNowValue = pkUnit->GetAbil(iNowType);
			int const iNowMaxValue = pkUnit->GetAbil(iCurMaxType);
			int const iDecValue = static_cast<int>(iNowMaxValue * static_cast<float>(iValue / ABILITY_RATE_VALUE_FLOAT));
			if(0 != iDecValue)
			{
				int iNewValue = iNowValue + iDecValue;

				iNewValue = __max(0, iNewValue);
				iNewValue = __min(iNowMaxValue, iNewValue); // Clamp 처리

				pkUnit->SetAbil(iNowType, iNewValue);
				pkUnit->SendAbil(static_cast<EAbilType>(iNowType), E_SENDTYPE_BROADALL);//주의
			}			
		}
	}
}

void OnAddEffect(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType )
{
	if(0 < iValue)
	{
		EffectQueueData kData(EQT_ADD_EFFECT, iValue);
		pkUnit->AddEffectQueue(kData);
	}
	else
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, -iValue);
		pkUnit->AddEffectQueue(kData);
	}
}

//Effect에서 Tick에서 사용하는 AT_HP 값을 구해온다. 카오스맵에 있을 경우 데미지인 경우 값이 달라져야한다. / 회복은 정상적으로 동작
int GetTick_AT_HP_Value(CEffect* pkEffect, SActArg const* pkArg, CUnit* pkUnit)
{
	int iAddHP = pkEffect->GetAbil(AT_HP);
	// 데미진 경우만 체크 한다.
	if(0 > iAddHP)
	{
		if(pkArg)
		{
			int iCasterUseChaosDebuff = 0;
			//ACTARG_EFFECT_CASTER_USE_CHAOS_DEBUFF 는 카오스맵에 있는지 여부를 체크 하도록 해놓은 값
			pkArg->Get(ACTARG_EFFECT_CASTER_USE_CHAOS_DEBUFF, iCasterUseChaosDebuff);
			
			int iCasterUnitType = 0;		// 캐스터의 유닛타입
			int iGroundAttr		= 0;		// 그라운드의 타입
			int iTargetUnitType	= 0;		// 타겟이 되는 유닛타입
			bool bDefenceModeException = false;
			if(pkUnit)
			{// 디펜스 모드에서의 예외 상황인지 확인하고
				iTargetUnitType = static_cast<int>(pkUnit->UnitType());
				PgGround* pkGround = NULL;
				pkArg->Get(ACTARG_GROUND, pkGround);
				pkArg->Get(ACTARG_EFFECT_CASTER_UNIT_TYPE, iCasterUnitType);
				if(pkGround)
				{
					if(pkUnit->GetAbil(AT_DAMAGE_IS_ONE))
					{
						CUnit * pkCaster = pkGround->GetUnit(pkEffect->GetCaster());
						if(pkCaster)
						{
							iAddHP = -(1+pkCaster->GetAbil(AT_DAMAGE_IS_ONE_ALPHA));
						}
						else
						{
							iAddHP = -1;
						}
					}
					else
					{
						bDefenceModeException = (UT_OBJECT == iTargetUnitType && UT_MONSTER == iCasterUnitType && (pkGround->GetAttr() & GATTR_MISSION));
					}
				}
			}

			// 0보다 작을 경우만 적용이 되어야 한다.
			// 0보다 작을 경우에만 카오스 맵에 버프가 걸려있는 것이다.
			if(0 > iCasterUseChaosDebuff	// 카오스 디버프가 걸려있거나
				|| bDefenceModeException	// 디펜스 모드에서의 예외 상황이라면
				)
			{//데미지를 1로 만들어 준다.
				iAddHP = -1;
			}
		}
	}

	return iAddHP;
}

///////////////////////////////////////////////////////////
//  PgEffectFunctionUtil
///////////////////////////////////////////////////////////
namespace PgEffectFunctionUtil
{
	void DeleteAllDebuff(CEffect* pkEffect, CUnit* pkUnit)
	{
		if( pkEffect && pkUnit )
		{
			int const DeleteDebuffCount = pkEffect->GetAbil(AT_EFFECT_DELETE_ALL_DEBUFF);
			if( DeleteDebuffCount > 0 )
			{
				PgUnitEffectMgr const& rEffectMgr = pkUnit->GetEffectMgr();
				int Count = 0;
				int FirstDelEffectNo = 0;
				ContEffectItor iter;
				rEffectMgr.GetFirstEffect(iter);
				CEffect* pEffect = NULL;
				while( ( pEffect = rEffectMgr.GetNextEffect(iter) ) != NULL )
				{
					if( DeleteDebuffCount <= Count )
					{
						break;
					}
					if(pEffect)
					{
						int const BuffType = pEffect->GetAbil(AT_TYPE);
						if(EFFECT_TYPE_CURSED == BuffType)
						{
							if(0 == pEffect->GetAbil(AT_CURE_NOT_DELETE) && 0 == pEffect->GetAbil(AT_NOT_DELETE_EFFECT))
							{
								pkUnit->AddEffectQueue(EffectQueueData(EQT_DELETE_EFFECT, pEffect->GetEffectNo()));
								if( 0 == FirstDelEffectNo )
								{
									FirstDelEffectNo = pEffect->GetEffectNo();
								}
								++Count;
							}
						}
					}
				}

				BM::Stream Packet(PT_M_C_ANS_DELETE_DEBUFF_SUCCESS);
				Packet.Push(FirstDelEffectNo);
				Packet.Push(Count);
				pkUnit->Send(Packet);
			}
		}
	}
}

///////////////////////////////////////////////////////////
//  PgIEffectFunction
///////////////////////////////////////////////////////////
PgIEffectFunction::PgIEffectFunction()
{}

PgIEffectFunction::~PgIEffectFunction()
{
	Release();
}

void PgIEffectFunction::Init()
{
}

void PgIEffectFunction::Release()
{
	CONT_ABIL_TYPE_VAULE_TABLE::iterator table_itor = m_kTable.begin();
	while(table_itor != m_kTable.end())
	{
		SAFE_DELETE((*table_itor).second);
		++table_itor;
	}
	m_kTable.clear();
}

void PgIEffectFunction::Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef)
{	
}

void PgIEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
}

void PgIEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
}
int PgIEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

int PgIEffectFunction::EffectReset(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_RESET;
}

///////////////////////////////////////////////////////////
//  PgDefaultEffectFunction - Default Effect Function
///////////////////////////////////////////////////////////
PgDefaultEffectFunction::PgDefaultEffectFunction()
{}

PgDefaultEffectFunction::~PgDefaultEffectFunction()
{}


void PgDefaultEffectFunction::Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef)
{
	// Abil 처리는 방식에 따라 따로 테이블에 저장 해 둔다.
	SAbilIterator kItor;
	pkDef->FirstAbil(&kItor);
	while (pkDef->NextAbil(&kItor))
	{
		int const iAbilType = pkAbilTable->GetAbilTypeTable(kItor.wType);

		CONT_ABIL_TYPE_VAULE* pkTable = NULL;		
		auto ret = m_kTable.insert(std::make_pair(iAbilType, pkTable));

		if(ret.second)
		{
			ret.first->second = new_tr CONT_ABIL_TYPE_VAULE;
		}
		
		ret.first->second->push_back( SAbil(kItor.wType, kItor.iValue) );
	}
}

void PgDefaultEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	pfnOnExecFunc pkOnExec = NULL;

	CONT_ABIL_TYPE_VAULE_TABLE::const_iterator abil_type_itor = m_kTable.begin();

	while(m_kTable.end() != abil_type_itor)
	{
		//Table Type에 따른 Abil 계산 세팅
		switch((*abil_type_itor).first)
		{
		case EAT_ADDABIL :			{ pkOnExec = &OnAddAbil; } break;
		case EAT_SETABIL :			{ pkOnExec = &OnSetAbil; } break;
		case EAT_ADDABIL_AND_NULL : { pkOnExec = &OnAddAbil; } break;
		case EAT_SETABIL_AND_NULL : { pkOnExec = &OnSetAbil; } break;
		case EAT_MAX_HP_MP :		{ pkOnExec = &OnAddAbil_MaxHP_MaxMP; } break;
		case EAT_ADD_EFFECT_ABIL :	{ pkOnExec = &OnAddEffectAbil; } break;
		case EAT_ATTACK:			{ pkOnExec = &OnSetAbil; } break;
		case EAT_BEGIN_SEND_TTW_MSG:{ pkOnExec = &OnExecAbil_TTW_MSG; } break; // Abil Set하는게 아니라 어빌로 특정 처리를 해준다.
		case EAT_END_SEND_TTW_MSG:	{ pkOnExec = NULL; } break; //Begin에서는 End을 처리하지 않음
		case EAT_HPMP_DEC_MAX_HPMP_RATE:{ pkOnExec = &OnHPMPDec_MaxHP_MaxMP_Rate; } break;
		case EAT_EFFECT_NUM :		{ pkOnExec = &OnAddEffect; } break;
		case EAT_IGNORE:			{ pkOnExec = NULL; }break;	// 아무일도 하지 않음
		default :
			{
				pkOnExec = NULL;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			} break;
		}

		if(pkOnExec)
		{
			CONT_ABIL_TYPE_VAULE* pkAbilTypeList = (*abil_type_itor).second;
			if(pkAbilTypeList)
			{
				CONT_ABIL_TYPE_VAULE::const_iterator abil_value_itor = pkAbilTypeList->begin();
				while(pkAbilTypeList->end() != abil_value_itor)
				{
					// Sub이펙트를 처리해야 하는 경우
					if(EAT_ADD_EFFECT_ABIL == (*abil_type_itor).first)
					{
						PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
						if(NULL == pkPlayer)
						{
							continue;
						}
						PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
						if(NULL == pkPlayerSkill)
						{
							continue;
						}

						// 실제로 배운 서브 이펙트 번호를 가져 온다.
						int iLearnedChildEffectNo = pkEffect->GetSubEffectNo((*abil_value_itor).m_iValue, true);
						if(0 == iLearnedChildEffectNo)
						{
							iLearnedChildEffectNo = pkPlayerSkill->GetLearnedSkill((*abil_value_itor).m_iValue);
							pkEffect->AddSubEffectNo(iLearnedChildEffectNo);
						}
						else
						{
							// 이미 이펙트가 값을 가지고 있으나 정보가 갱신 되었다면 (Restart 일 경우)
							int const iNewLearnedChildEffectNo = pkPlayerSkill->GetLearnedSkill((*abil_value_itor).m_iValue);

							if(iNewLearnedChildEffectNo != iLearnedChildEffectNo)
							{
								// 실제 정보를 갱신 하여 준다.
								pkEffect->DeleteSubEffectNo(iLearnedChildEffectNo);
								pkEffect->AddSubEffectNo(iNewLearnedChildEffectNo);

								iLearnedChildEffectNo = iNewLearnedChildEffectNo;
							}
						}

						(*pkOnExec)(pkUnit, (*abil_value_itor).m_wType , iLearnedChildEffectNo, E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
					}
					//일반적인 처리
					else
					{
						(*pkOnExec)(pkUnit, (*abil_value_itor).m_wType , (*abil_value_itor).m_iValue, E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
					}
					++abil_value_itor;
				}
			}
		}

		++abil_type_itor;
	}

	int const iForceFireSkillNo = pkEffect->GetAbil(AT_MON_BEGIN_EFFECT_FORCESKILL);
	if( 0 < iForceFireSkillNo && pkUnit->IsInUnitType(UT_MONSTER) )
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkEffect->GetAbil(AT_FORCESKILL_FLAG));
		pkUnit->GetSkill()->ForceReserve(iForceFireSkillNo, eFlag);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}

	PgEffectFunctionUtil::DeleteAllDebuff(pkEffect, pkUnit); // 디버프 삭제
}

int PgDefaultEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	CUnit* pkEffectCaster = NULL;
	PgGround *pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if ( pkGround )
	{
		pkEffectCaster = pkGround->GetUnit( pkEffect->GetCaster() );
	}
	
	//Tick 당 HP / MP 감소가 있을 경우 빼준다.
	int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
	if(iAddHP)
	{
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iNewValue = iNowHP + iAddHP;		
		int iNewHP = __max(0, iNewValue);	// 최소치 검사
		iNewHP = __min(iMaxHP, iNewHP);		// 최대치 검사
		int const iDelta = iNewHP - iNowHP;

		if(0 != iDelta)
		{//변화량이 있는데
			if(0 > iDelta)
			{// 데미지 라면
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				bool const bBlock = iRandValue < pkUnit->GetAbil(AT_100PERECNT_BLOCK_RATE); // 절대 Block Rate 계산
				if(bBlock)
				{
					pkUnit->AddEffect(BLOCK_RATE_EFFECT_NO, 0, pkArg, pkUnit);
					return ECT_MUSTDELETE;
				}
			}
			//BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
			//kPacket.Push(pkUnit->GetID());
			//kPacket.Push((short)AT_HP);
			//kPacket.Push(iNewHP);
			//kPacket.Push(pkEffect->GetCaster());
			//kPacket.Push(pkEffect->GetEffectNo());
			//kPacket.Push(iDelta);
			//pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
			//OnSetAbil(pkUnit, AT_HP, iNewHP);
			SkillFuncUtil::OnModifyHP(pkUnit, NULL, 0, iAddHP, pkEffectCaster, pkGround); // 힐하는 동작은 한 함수로 몰아 넣기 위해 이 함수를 사용

			if(0 == iNewHP)
			{
				pkUnit->SetTarget(pkEffect->GetCaster());
				return ECT_MUSTDELETE;
			}
		}
	}

	int const iAddMP = pkEffect->GetAbil(AT_MP);
	if(iAddMP)
	{
		int const iNowMP = pkUnit->GetAbil(AT_MP);
		int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
		int const iNewValue = iNowMP + iAddMP;		
		int iNewMP = __max(0, iNewValue);	// 최소치 검사
		iNewMP = __min(iMaxMP, iNewMP);		// 최대치 검사
		int const iDelta = iNewMP - iNowMP;
		if(0 != iDelta)
		{//변화량이 있다면
			//BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
			//kPacket.Push(pkUnit->GetID());
			//kPacket.Push((short)AT_MP);
			//kPacket.Push(iNewMP);
			//kPacket.Push(pkEffect->GetCaster());
			//kPacket.Push(pkEffect->GetEffectNo());
			//kPacket.Push(iDelta);
			//pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
			//OnSetAbil(pkUnit, AT_MP, iNewMP);
			SkillFuncUtil::OnModifyMP(pkUnit, NULL, 0, iAddMP, pkEffectCaster, pkGround); // MP 회복하는 동작은 한 함수로 몰아 넣기 위해 이 함수를 사용

			if(0 == iNewMP)
			{
				return ECT_MUSTDELETE;
			}
		}
	}

	return ECT_DOTICK;
}

void PgDefaultEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	pfnOnExecFunc pkOnExec = NULL;

	CONT_ABIL_TYPE_VAULE_TABLE::const_iterator abil_type_itor = m_kTable.begin();

	while(m_kTable.end() != abil_type_itor)
	{
		//Table Type에 따른 Abil 계산 세팅
		switch((*abil_type_itor).first)
		{
		case EAT_ADDABIL :			{ pkOnExec = &OnAddAbil; } break;
		case EAT_SETABIL :			{ pkOnExec = &OnSetAbil; } break;
		case EAT_ADDABIL_AND_NULL : { pkOnExec = &OnSetNullAbil2; } break;
		case EAT_SETABIL_AND_NULL : { pkOnExec = &OnSetNullAbil; } break;
		case EAT_MAX_HP_MP :		{ pkOnExec = &OnAddAbil_MaxHP_MaxMP; } break;
		case EAT_ADD_EFFECT_ABIL :	{ pkOnExec = &OnAddEffectAbil; } break;
		case EAT_ATTACK:			{ pkOnExec = &OnAddAbil_Attack; } break;
		case EAT_BEGIN_SEND_TTW_MSG: { pkOnExec = NULL; } break; //End에서는 Begin을 처리하지 않음
		case EAT_END_SEND_TTW_MSG:		{ pkOnExec = &OnExecAbil_TTW_MSG; } break; // Abil Set하는게 아니라 어빌로 특정 처리를 해준다.
		case EAT_HPMP_DEC_MAX_HPMP_RATE: { pkOnExec = NULL; } break; //End에서는 아무 처리도 하지 않음 Begin에서만 처리
		case EAT_EFFECT_NUM :		{ pkOnExec = &OnAddEffect; } break;
		case EAT_IGNORE:			{ pkOnExec = NULL; }break;	// 아무일도 하지 않음
		default :					
			{
				pkOnExec = NULL; 
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			} break;
		}

		if(pkOnExec)
		{
			CONT_ABIL_TYPE_VAULE* pkAbilTypeList = (*abil_type_itor).second;
			CONT_ABIL_TYPE_VAULE::const_iterator abil_value_itor = pkAbilTypeList->begin();
			while(pkAbilTypeList->end() != abil_value_itor)
			{
				// Sub이펙트를 처리해야 하는 경우
				if(EAT_ADD_EFFECT_ABIL == (*abil_type_itor).first)
				{
					PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if(NULL == pkPlayer)
					{
						continue;
					}
					PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
					if(NULL == pkPlayerSkill)
					{
						continue;
					}

					// 이펙트를 추가했을때 저장한 서브 이펙트 번호를 가져 온다.
					// (이펙트를 건 후 이펙트 레벨을 올리게 되는 경우가 생기므로 걸었을때 값이 있어야 한다)
					int iLearnedChildEffectNo = pkEffect->GetSubEffectNo((*abil_value_itor).m_iValue, true);

					(*pkOnExec)(pkUnit, (*abil_value_itor).m_wType , -iLearnedChildEffectNo, E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
				}
				else
				{
					(*pkOnExec)(pkUnit, (*abil_value_itor).m_wType , -(*abil_value_itor).m_iValue, E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
				}
				++abil_value_itor;
			}
		}
		++abil_type_itor;
	}

	//능력치가 깍일 때 HP / MP가 최대치보다 큰 경우 최대치로 맞춰 준다.
	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

	if(iNowHP > iMaxHP)
	{
		int const iNewHP = iMaxHP;
		OnSetAbil(pkUnit, AT_HP, iNewHP);
	}

	int const iNowMP = pkUnit->GetAbil(AT_MP);
	int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

	if(iNowMP > iMaxMP)
	{
		int const iNewMP = iMaxMP;
		OnSetAbil(pkUnit, AT_MP, iNewMP);
	}

	int const iForceFireSkillNo = pkEffect->GetAbil(AT_MON_REMOVE_EFFECT_FORCESKILL);
	if( 0 < iForceFireSkillNo && pkUnit->IsInUnitType(UT_MONSTER) )
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkEffect->GetAbil(AT_FORCESKILL_FLAG));
		pkUnit->GetSkill()->ForceReserve(iForceFireSkillNo, eFlag);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}
}

//////////////////////////////////////////////////////////
//  PgMaxHpAndMPEffectFunction - HP, MP 최대치 증가 및 최대치까지 회복
///////////////////////////////////////////////////////////
void PgMaxHpAndMPEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	int const iOldMaxHp = pkUnit->GetAbil(AT_C_MAX_HP);
	OnAddAbil(pkUnit, AT_R_MAX_HP, pkEffect->GetAbil( AT_R_MAX_HP ));
	int const iNewMaxHp = pkUnit->GetAbil(AT_C_MAX_HP);
	int iCalcHp = pkUnit->GetAbil(AT_HP) + (pkUnit->GetAbil(AT_C_MAX_HP) - iOldMaxHp);
	iCalcHp = std::min(iNewMaxHp, iCalcHp);
	iCalcHp = std::max(0, iCalcHp);
	pkUnit->SetAbil(AT_HP, iCalcHp, true, true);

	int const iOldMaxMp = pkUnit->GetAbil(AT_C_MAX_MP);
	OnAddAbil(pkUnit, AT_R_MAX_MP, pkEffect->GetAbil( AT_R_MAX_MP ));
	int const iNewMaxMp = pkUnit->GetAbil(AT_C_MAX_MP);
	int iCalcMp = pkUnit->GetAbil(AT_MP) + (pkUnit->GetAbil(AT_C_MAX_MP) - iOldMaxMp);
	iCalcMp = std::min(iNewMaxMp, iCalcMp);
	iCalcMp = std::max(0, iCalcMp);
	pkUnit->SetAbil(AT_MP, iCalcMp, true, true);
}

void PgMaxHpAndMPEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	OnAddAbil(pkUnit, AT_R_MAX_HP, -pkEffect->GetAbil( AT_R_MAX_HP ));
	pkUnit->SetAbil(AT_HP, std::min(pkUnit->GetAbil(AT_HP), pkUnit->GetAbil(AT_C_MAX_HP)), true, true);

	OnAddAbil(pkUnit, AT_R_MAX_MP, -pkEffect->GetAbil( AT_R_MAX_MP ));
	pkUnit->SetAbil(AT_MP, std::min(pkUnit->GetAbil(AT_MP), pkUnit->GetAbil(AT_C_MAX_MP)), true, true);
}

//////////////////////////////////////////////////////////
//  PgHandyCapFunction - pvp 핸디캡
///////////////////////////////////////////////////////////
void PgHandyCapFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	int const iPhyDefence = pkEffect->GetAbil( AT_PHY_DEFENCE );
	int const iMagicDefence = pkEffect->GetAbil( AT_MAGIC_DEFENCE );

	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	if ( iPhyDefence )
	{
		rkMgr.AddAbil( AT_PHY_DEFENCE, iPhyDefence );
		pkUnit->NftChangedAbil( AT_PHY_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL );
//		pkEffect->SetActArg( AT_PHY_DEFENCE, iPhyDefence );
	}
	
	if ( iMagicDefence )
	{
		rkMgr.AddAbil( AT_MAGIC_DEFENCE, iMagicDefence );
		pkUnit->NftChangedAbil( AT_MAGIC_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL );
//		pkEffect->SetActArg( AT_MAGIC_DEFENCE, iMagicDefence );
	}
}

void PgHandyCapFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int const iPhyDefence = pkEffect->GetAbil( AT_PHY_DEFENCE );
	int const iMagicDefence = pkEffect->GetAbil( AT_MAGIC_DEFENCE );

	if ( iPhyDefence )
	{
		rkMgr.AddAbil( AT_PHY_DEFENCE, -iPhyDefence);
		pkUnit->NftChangedAbil( AT_PHY_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL );
	}

	if ( iMagicDefence )
	{
		rkMgr.AddAbil( AT_MAGIC_DEFENCE, -iMagicDefence );
		pkUnit->NftChangedAbil( AT_MAGIC_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL );
	}
}

int PgHandyCapFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgMuteChat - Mute Chat
///////////////////////////////////////////////////////////
void PgMuteChat::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	pkUnit->SetAbil( AT_MUTE, 1 );
}

void PgMuteChat::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	pkUnit->SetAbil( AT_MUTE, 0 );
}

int PgMuteChat::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgChangeMahalkaEffectFunction - 마할카 변신
///////////////////////////////////////////////////////////
int PgChangeMahalkaEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV0, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());

	// 공격을 했으면 이펙트를 제거 한다.
	if( pkUnit )
	{
		if( 0 < pkUnit->GetAbil(AT_ENABLE_CHECK_ATTACK) )
		{
			if(0 < pkUnit->GetAbil(AT_CHECK_ATTACK_COUNT))
			{
				//pkUnit->SetAbil(AT_CHECK_ATTACK_COUNT, 0);
				return ECT_MUSTDELETE;
			}
		}
	}

	return ECT_DOTICK;
}

// 각성기
//////////////////////////////////////////////////////////
//  PgAwakeNormalStateEffectFunction - 각성 노말 상태
///////////////////////////////////////////////////////////
void PgAwakeSystemEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{	
	//MapMove 시에 AT_ENABLE_AWAKE_SKILL을 미리 세팅해서 상태가 이펙트를 중복으로 걸림을 방지하기 위해
	//Passive가 처음으로 발동 되는 경우
	if(0 == pkUnit->GetAbil(AT_ENABLE_AWAKE_SKILL))
	{		
		OnSetAbil(pkUnit, AT_ENABLE_AWAKE_SKILL,	1); // 각성기 시스템 활성화

		if(EAS_NORMAL_GUAGE == pkUnit->GetAbil(AT_AWAKE_STATE))
		{
			// 처음 일 땐 Normal 상태로 만들어 준다.
			EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_NORMAL_STATE);
			pkUnit->AddEffectQueue(kData);
		}
		else if(EAS_MAX_GUAGE == pkUnit->GetAbil(AT_AWAKE_STATE))
		{
			EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_MAX_STATE);
			pkUnit->AddEffectQueue(kData);
		}
		else if(EAS_PENALTY_GUAGE == pkUnit->GetAbil(AT_AWAKE_STATE))
		{
			EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_PENALTY);
			pkUnit->AddEffectQueue(kData);
		}
	}
}

void PgAwakeSystemEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	OnSetAbil(pkUnit, AT_ENABLE_AWAKE_SKILL,	0); // 각성기 시스템 비활성화
}

int PgAwakeSystemEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgAwakeNormalStateEffectFunction - 각성 노말 상태
///////////////////////////////////////////////////////////
void PgAwakeNormalStateEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{	
	OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_NORMAL_GUAGE);
	OnAddAbil(pkUnit, AT_AWAKE_TICK_DEC_VALUE,	pkEffect->GetAbil(AT_AWAKE_TICK_DEC_VALUE));
}

void PgAwakeNormalStateEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_NORMAL_GUAGE);
	OnAddAbil(pkUnit, AT_AWAKE_TICK_DEC_VALUE,	-pkEffect->GetAbil(AT_AWAKE_TICK_DEC_VALUE));

	if(AWAKE_VALUE_MAX <= pkUnit->GetAbil(AT_AWAKE_VALUE))
	{
		EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_MAX_STATE); //Value
		pkUnit->AddEffectQueue(kData);
	}
	else
	{
		EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_PENALTY);
		pkUnit->AddEffectQueue(kData);
	}
}

int PgAwakeNormalStateEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	if(0 < pkUnit->GetAbil(AT_ENABLE_AWAKE_SKILL))
	{
		switch(pkUnit->GetAbil(AT_AWAKE_STATE))
		{
		case EAS_NORMAL_GUAGE: // 기본적인 상태 - 스킬을 사용시 마다 각성치가 채워지고, 틱마다 일정한 값만큼 줄어드는 상태
			{
				int const iNowValue = pkUnit->GetAbil(AT_AWAKE_VALUE);
				if(AWAKE_VALUE_MAX <= pkUnit->GetAbil(AT_AWAKE_VALUE))
				{
					return ECT_MUSTDELETE;
					
				}
				int const iDecValue = pkUnit->GetAbil(AT_AWAKE_TICK_DEC_VALUE);				
				int const iNewValue = __max(0, iNowValue + iDecValue);

				OnSetAbil(pkUnit, AT_AWAKE_VALUE, iNewValue);
			}break;
		case EAS_MAX_GUAGE: // Max 상태
			{
				// 감소가 없다.
			}break;
		case EAS_PENALTY_GUAGE: // Max가 된 후 스킬을 사용 한 상태
			{
				// 다른 이펙트에서 감소 처리를 한다.
			}break;
		}
	}

	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgAwakeMaxStateEffectFunction - 각성 맥스 상태
///////////////////////////////////////////////////////////
void PgAwakeMaxStateEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_MAX_GUAGE);
}

void PgAwakeMaxStateEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{	
	if(AWAKE_VALUE_MAX !=  pkUnit->GetAbil(AT_AWAKE_VALUE))
	{
		OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_PENALTY_GUAGE);
		EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_PENALTY, 1); //Value 값을 넣어서 맥스 상태에서 패널티 상태로 변경되는 것이라는걸 알린다.
		pkUnit->AddEffectQueue(kData);
	}
}

int PgAwakeMaxStateEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{

	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgAwakePenaltyStateEffectFunction - 각성 소모 상태
//////////////////////////////////////////////////////////
void PgAwakePenaltyStateEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{	
	OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_PENALTY_GUAGE);
	OnAddAbil(pkUnit, AT_AWAKE_TICK_DEC_VALUE,	pkEffect->GetAbil(AT_AWAKE_TICK_DEC_VALUE));

	if(pkEffect->GetValue())
	{
		//맥스 상태 패널티로 바뀐것이기 때문에 
		OnAddAbil(pkUnit, AT_ADD_NEED_R_AWAKE, pkEffect->GetAbil(AT_ADD_NEED_R_AWAKE));
	}
}

void PgAwakePenaltyStateEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	OnSetAbil(pkUnit, AT_AWAKE_STATE,	EAS_NORMAL_GUAGE);
	OnAddAbil(pkUnit, AT_AWAKE_TICK_DEC_VALUE,	-pkEffect->GetAbil(AT_AWAKE_TICK_DEC_VALUE));

	if(pkEffect->GetValue())
	{
		//맥스 상태 패널티로 바뀐것이기 때문에 
		OnAddAbil(pkUnit, AT_ADD_NEED_R_AWAKE, -pkEffect->GetAbil(AT_ADD_NEED_R_AWAKE));
	}

	EffectQueueData kData(EQT_ADD_EFFECT, EFFECTNO_AWAKE_NORMAL_STATE);
	pkUnit->AddEffectQueue(kData);
}

int PgAwakePenaltyStateEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	if(0 < pkUnit->GetAbil(AT_ENABLE_AWAKE_SKILL))
	{
		switch(pkUnit->GetAbil(AT_AWAKE_STATE))
		{
		case EAS_PENALTY_GUAGE: // Max가 된 후 스킬을 사용 한 상태
			{
				int const iDecValue = pkUnit->GetAbil(AT_AWAKE_TICK_DEC_VALUE);
				int const iNowValue = pkUnit->GetAbil(AT_AWAKE_VALUE);
				int const iNewValue = __max(0, iNowValue + iDecValue);

				OnSetAbil(pkUnit, AT_AWAKE_VALUE, iNewValue);

				if(0 >= pkUnit->GetAbil(AT_AWAKE_VALUE))
				{
					return ECT_MUSTDELETE;
				}		
			}break;
		}
	}
	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////
//  PgSafeBubbleExtFunction
//////////////////////////////////////////////////////////
void PgSafeBubbleExtFunction::Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef)
{
	// Abil 처리는 방식에 따라 따로 테이블에 저장 해 둔다.
	SAbilIterator kItor;
	pkDef->FirstAbil(&kItor);
	while (pkDef->NextAbil(&kItor))
	{
		int const iAbilType = pkAbilTable->GetAbilTypeTable(kItor.wType);
		if ( EAT_EFFECT_NUM != iAbilType )
		{// 얘는 AddEffect는 제외
			CONT_ABIL_TYPE_VAULE* pkTable = NULL;		
			auto ret = m_kTable.insert(std::make_pair(iAbilType, pkTable));

			if(ret.second)
			{
				ret.first->second = new_tr CONT_ABIL_TYPE_VAULE;
			}

			ret.first->second->push_back( SAbil(kItor.wType, kItor.iValue) );
		}
	}
}

void PgSafeBubbleExtFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( pkPlayer )
		{
			if ( pkArg )
			{
				PgGround *pkGround = NULL;
				pkArg->Get(ACTARG_GROUND,pkGround);
				if ( pkGround )
				{
					int const iPetEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
					if ( iPetEffectNo )
					{
						PgPet * pkPet = pkGround->GetPet( pkPlayer );
						if ( pkPet && (EPET_TYPE_2 ==pkPet->GetPetType() || EPET_TYPE_3 == pkPet->GetPetType()) )
						{
							SEffectCreateInfo kCreate;
							kCreate.eType = EFFECT_TYPE_PENALTY;
							kCreate.iEffectNum = iPetEffectNo;
							kCreate.kActArg = *pkArg;
							kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

							pkPet->AddEffect(kCreate);
						}
					}
				}
			}
		}
	}

	PgDefaultEffectFunction::EffectBegin( pkUnit, pkEffect, pkArg );
}

void PgSafeBubbleExtFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( pkPlayer )
		{
			if ( pkArg )
			{
				PgGround *pkGround = NULL;
				pkArg->Get(ACTARG_GROUND,pkGround);
				if ( pkGround )
				{
					int const iPetEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
					if ( iPetEffectNo )
					{
						PgPet * pkPet = pkGround->GetPet( pkPlayer );
						if ( pkPet )
						{
							pkPet->DeleteEffect( iPetEffectNo );
						}
					}
				}
			}
		}
	}

	PgDefaultEffectFunction::EffectEnd( pkUnit, pkEffect, pkArg );
}

//////////////////////////////////////////////////////////
//  PgSkillDmgByPhaseEffectFunction
//////////////////////////////////////////////////////////
void PgSkillDmgByPhaseEffectFunction::EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	if(!pkEffect)
	{
		return;
	}
	//필요한 값들 초기화
	SActArg* pkEffectActArg = pkEffect->GetActArg();
	pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_BEGIN_TIME, static_cast<DWORD>(0));//  시간 값을 저장할 용도로 씀
	pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_ACC_DMG,static_cast<__int64>(0));//  누적된 데미지 증감량(계산저장용)
}

void PgSkillDmgByPhaseEffectFunction::EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg)
{
	if(!pkEffect)
	{
		return;
	}
	//필요한 값들 초기화
	SActArg* pkEffectActArg = pkEffect->GetActArg();
	pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_BEGIN_TIME, static_cast<DWORD>(0));//  시간 값을 저장할 용도로 씀
	pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_ACC_DMG,static_cast<__int64>(0));//  누적된 데미지 증감량(계산저장용)
}

int PgSkillDmgByPhaseEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{

	if(!pkUnit
		|| !pkEffect)
	{
		return ECT_MUSTDELETE;
	}
	
	SActArg* pkEffectActArg = pkEffect->GetActArg();
	if(pkEffectActArg)
	{
		DWORD dwPrevTime = 0;
		pkEffectActArg->Get(ACT_ARG_DMG_BY_PASE_BEGIN_TIME, dwPrevTime);//  시간 값을 저장할 용도로 씀

		DWORD const dwPhaseTime = pkEffect->GetAbil(AT_ADJUST_SKILL_DMG_BY_PHASE_TIME);
	
		bool bAlreadyUse = false;
		pkEffectActArg->Get(ACT_ARG_CUSTOMDATA1, bAlreadyUse);
		if(bAlreadyUse)
		{
			__int64 i64AccDmg = 0;
			pkEffectActArg->Get(ACT_ARG_DMG_BY_PASE_ACC_DMG,i64AccDmg);
			pkEffectActArg->Set(ACT_ARG_CUSTOMDATA1, false);
		}
		
		if(0 < dwPrevTime
			&& 0 < dwPhaseTime
			)
		{
			DWORD const dwCurTime =	BM::GetTime32();
			if(dwCurTime - dwPrevTime >= dwPhaseTime)
			{// 유지 시간을 벗어났다면, 시간과 누적 데미지 값을 초기화
				pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_ACC_DMG, static_cast<__int64>(0));
				pkEffectActArg->Set(ACT_ARG_DMG_BY_PASE_BEGIN_TIME, static_cast<DWORD>(0));
				{
					BM::Stream kPacket(PT_M_C_NFY_EFFECT_ARGCHANGED);
					kPacket.Push(pkUnit->GetID());			// 변경될 녀석
					kPacket.Push(pkUnit->GetID());			// caster
					kPacket.Push(pkEffect->GetEffectNo());	// effectNo
					pkEffectActArg->WriteToPacket(kPacket); // Arg 값
					pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
				}
			}
		}
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAwakeChargeEffectFunction - 각성기 챠지 이펙트 
///////////////////////////////////////////////////////////
void PgAwakeChargeEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	switch(pkUnit->GetAbil(AT_AWAKE_STATE))
	{
	case EAS_PENALTY_GUAGE:
		{// 각성 패널티 상태이면 패널티 상태를 제거해 노멀 상태로 바꿔주고
			pkUnit->DeleteEffect(EFFECTNO_AWAKE_PENALTY);
		}break;
	}
	
//	OnAddAbil(pkUnit, AT_R_MOVESPEED,	-ABILITY_RATE_VALUE);	// 이동 할 수 없고
	OnSetAbil(pkUnit, AT_AWAKE_CHARGE_STATE, 1);				// 각성기 충전을 사용하고 있다는 표시
	OnSetAbil(pkUnit, AT_IS_DAMAGED_DURING_AWAKE_CHARGE, 0);	// 데미지 체크 각성기 충전중 데미지 받았는가
	
	POINT3 const kPos = pkUnit->GetPos();
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+0, static_cast<int>(kPos.x));
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+1, static_cast<int>(kPos.y));
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+2, static_cast<int>(kPos.z));

	{// 해당 유닛에게만 UI 변경 여부를 보낸다
		BM::Stream kPacket(PT_UNIT_AWAKE_CHARGE_STATE);
		kPacket.Push(EACS_UI_CHANGE);							// BYTE 형
		kPacket.Push(true);										// 각성 챠지 UI를 바꿈
		pkUnit->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
	}
}

void PgAwakeChargeEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
//	OnAddAbil(pkUnit, AT_R_MOVESPEED,	ABILITY_RATE_VALUE);	// 이동 할수 있으며
	OnSetAbil(pkUnit, AT_AWAKE_CHARGE_STATE, 0);				// 각성기 충전을 사용하고 있다는 표시
	OnSetAbil(pkUnit, AT_IS_DAMAGED_DURING_AWAKE_CHARGE, 0);	// 데미지 체크 각성기 충전중 데미지 받았는가

	{// 유닛이 a_battle_idle로 할수 있게 전체에 브로드 캐스팅하고
		BM::Stream kPacket(PT_UNIT_AWAKE_CHARGE_STATE);
		kPacket.Push(EACS_AWAKE_CHARGE_END);
		kPacket.Push(pkUnit->GetID());							// 변경될 녀석
		pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
	}

	{// 해당 유닛에게만 UI 변경 여부를 보낸다
		BM::Stream kPacket(PT_UNIT_AWAKE_CHARGE_STATE);
		kPacket.Push(EACS_UI_CHANGE);							// BYTE 형
		kPacket.Push(false);									// 바뀐 각성 챠지 UI를 원상태로 돌림
		pkUnit->Send(kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND);
	}
}

int PgAwakeChargeEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	if(!pkUnit)
	{
		return ECT_MUSTDELETE;
	}
	if(0 < pkUnit->GetAbil(AT_IS_DAMAGED_DURING_AWAKE_CHARGE))
	{
		return ECT_MUSTDELETE;
	}
	if(EAS_MAX_GUAGE == pkUnit->GetAbil(AT_AWAKE_STATE))
	{// 최대값 이상이라면 챠지 이펙트 제거
		return ECT_MUSTDELETE;
	}

	{// 플레이어가 이동했다면 이펙트 제거
		POINT3 const kPos = pkUnit->GetPos();
		int ix = 0,iy = 0, iz = 0;
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+0, ix);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+1, iy);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+2, iz);
		if(static_cast<int>(kPos.x) != ix)
		{
			return ECT_MUSTDELETE; 
		}
		if(static_cast<int>(kPos.y) != iy)
		{
			return ECT_MUSTDELETE; 
		}
		if(static_cast<int>(kPos.z) != iz)
		{
			return ECT_MUSTDELETE;
		}
	}

	// 이펙트 유지에 필요한 MP 값을 계산하기위해
	int const iNeedMPRate =  pkEffect->GetAbil(AT_R_MAX_MP);
	// 최대 MP를 얻어오고
	int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
	// 필요량을 계산 완료 한 후
	int const iTempNeedMP = (iNeedMPRate * iMaxMP)/ABILITY_RATE_VALUE;
	int const iNeedMP = std::min(iMaxMP, iTempNeedMP);

	// 현재 MP와
	int const iNowMp = pkUnit->GetAbil(AT_MP);
	if(-iNeedMP > iNowMp)
	{// 비교해서 부족하면 이펙트 제거하고
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}
	// 충분하면 MP를 감소 하고
	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMP);

	// 각성 게이지 증가 값을 얻어와 계산하고(만분률)
	int const iIncValue = (AWAKE_VALUE_MAX*pkEffect->GetAbil(AT_R_INC_AWAKE_VALUE))/ABILITY_RATE_VALUE;	
	int const iNowValue = pkUnit->GetAbil(AT_AWAKE_VALUE);
	// 최대값 한도 내에서 현재 값을 더해주고
	int const iNewValue = __min(AWAKE_VALUE_MAX, iNowValue + iIncValue);
	// 채워진 각성치 값을 넣어준다
	OnSetAbil(pkUnit, AT_AWAKE_VALUE, iNewValue);

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  때린 대미지를 기준으로 HP를 회복시키는 이펙트
///////////////////////////////////////////////////////////
void PgHealFromHitDamageEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		|| !pkArg
		)
	{
		return;
	}

	int iAddHP = 0;
	// 힐량을 계산한 후
	int const iHealRate = pkEffect->GetAbil(AT_R_HP_RESTORE_FROM_DAMAGE);
	if(0 < iHealRate) 
	{
		int iHitDamage = 0;
		pkArg->Get(ACT_ARG_DAMAGE,iHitDamage);
		iAddHP += (iHitDamage*iHealRate)/ABILITY_RATE_VALUE;
	}

	iAddHP += pkEffect->GetAbil(AT_HP);	// 절대치로 피를 채워주는 어빌이 있으면 계산하고

	if(0 < iAddHP)
	{// 힐이 가능하면, 힐을 해준다
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		if(iMaxHP > iNowHP)
		{
			OnSetAbil(pkUnit, AT_HP, __min(iMaxHP, iNowHP + iAddHP));
		}
	}
}

void PgHealFromHitDamageEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgHealFromHitDamageEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{// 1회성이기 때문에 바로 지워줌
	return ECT_MUSTDELETE;
}

///////////////////////////////////////////////////////////
//  버프 1개를 지워버리는 이펙트
///////////////////////////////////////////////////////////
void PgDeleteBuffEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		)
	{
		return;
	}
	int iDeletedEffectCnt = 0;
	std::vector<int> kDeleteEffect;
	{
		int iCnt = pkEffect->GetAbil(AT_COUNT);
		if(0 < iCnt)
		{
			CEffect* pkTempEffect = NULL;
			ContEffectItor kItor;
			PgUnitEffectMgr const& rkEffectMgr =  pkUnit->GetEffectMgr();
			rkEffectMgr.GetFirstEffect(kItor);
			//GET_DEF(CEffectDefMgr, kEffectDefMgr);

			while ((pkTempEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
			{
				if(0 < pkTempEffect->GetAbil(AT_EFFCONTROL_DEL_BY_EFFECT))
				{
					kDeleteEffect.push_back(pkTempEffect->GetKey());
					++iDeletedEffectCnt;
				}
				if(iCnt == iDeletedEffectCnt)
				{
					break;
				}
			}
		}
		else
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<L"AT_COUNT(2089) AbilType is 0. Check EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
		}

		if(int const iDelEffect = pkEffect->GetAbil(AT_DELETE_EFFECT_NO))
		{
			kDeleteEffect.push_back(iDelEffect);
			++iDeletedEffectCnt;
		}
	}

	if(iDeletedEffectCnt)
	{// 삭제될 이펙트가 있으면
		size_t const iDeleteSize = kDeleteEffect.size();
		for (size_t i = 0; i < iDeleteSize; ++i)
		{// 이펙트들을 삭제하고
			pkUnit->DeleteEffect(kDeleteEffect.at(i));
		}

		// 자신의 버프 삭제하는 이펙트가 있으면
		int const iCasterDeleteEffect = pkEffect->GetAbil(AT_WHEN_EFFECT_ACTIVATE_THEN_DELETE_THIS_EFFECTNO);
		if(0 < iCasterDeleteEffect)
		{
			PgGround* pkGround = NULL;
			if(pkArg)
			{
				pkArg->Get(ACTARG_GROUND, pkGround);
			}
			if(pkGround)
			{// 그라운드에서
				CUnit* pkCaster = pkGround->GetUnit(pkEffect->GetCaster());
				if(pkCaster)
				{// 버프 시전한 대상에게, 지워야할 이펙트를 지울수 있게 함
					pkCaster->DeleteEffect(iCasterDeleteEffect, true);
					/* 
					지울 이펙트에 등록된 AT_EFFECT_GROUP_NO = 59 타입을 검색해 같은 이펙트 들을 모두 지움
					59타입에 value로 들어가는 그룹들은 다음과 같음
					1		: ? 어쨋든 경험치 버프 어디에 쓰이는지는 모르겠음
					2		: 채널 경험치 버프
					10001	: 타격시 특정 버프를 삭제 하는 이펙트를, 발동시키는 이펙트들.
					60001	: 배틀스퀘어전투불능 버프(HP%)
					60002	: 배틀스퀘어전투불능 버프(방어력%)
					*/
				}
			}
		}
	}
}
void PgDeleteBuffEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}
int  PgDeleteBuffEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_MUSTDELETE;
}


///////////////////////////////////////////////////////////
//  채집 회복 이펙트
///////////////////////////////////////////////////////////
void PgJobSkillRecoveryFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

void PgJobSkillRecoveryFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgJobSkillRecoveryFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(!pkUnit)
	{
		return ECT_MUSTDELETE;
	}
	PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkUnit);
	if( !pkPlayer )
	{
		return ECT_MUSTDELETE;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	int const iRecoveryJSExhaustionRate = pkEffect->GetAbil(AT_JS_RECOVERY_EXHAUSTION_R);
	int const iRecoveryJSExhaustionValue = pkEffect->GetAbil(AT_JS_RECOVERY_EXHAUSTION_C);
	int const iRecoveryJSBlessPointRate = pkEffect->GetAbil(AT_JS_RECOVERY_BLESSPOINT_R);
	int const iRecoveryJSBlessPointValue = pkEffect->GetAbil(AT_JS_RECOVERY_BLESSPOINT_C);
	int const iRecoveryJSProductPointRate = pkEffect->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_R);
	int const iRecoveryJSProductPointValue = pkEffect->GetAbil(AT_JS_RECOVERY_PRODUCTPOINT_C);
	if( iRecoveryJSExhaustionRate || iRecoveryJSExhaustionValue )
	{
		int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1차 주 스킬만 피로도가 있다
		int iAddValue = (iMaxExhaustion * iRecoveryJSExhaustionRate / ABILITY_RATE_VALUE) + iRecoveryJSExhaustionValue;
		if( S_PST_JS1_RecoveryExhaustion const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_JS1_RecoveryExhaustion>() )
		{
			iAddValue += SRateControl::GetValueRate(iAddValue, pkPremium->iRate);
		}
		if( 0 != pkPlayer->JobSkillExpertness().CurExhaustion() )
		{
			SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkUnit->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // 피로도 회복
			kOrder.push_back( kIMO );
		}
	}
	if( iRecoveryJSBlessPointRate || iRecoveryJSBlessPointValue )
	{
		int const iMaxBlessPoint = JobSkillExpertnessUtil::GetMaxExhaustion_2ND(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 2차 주 스킬만 축복게이지가 있다
		int const iAddValue = (iMaxBlessPoint * iRecoveryJSBlessPointRate / ABILITY_RATE_VALUE) + iRecoveryJSBlessPointValue;
		if( 0 != pkPlayer->JobSkillExpertness().CurBlessPoint() )
		{
			SPMO kIMO(IMET_JOBSKILL_ADD_BLESSPOINT, pkUnit->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // 피로도 회복
			kOrder.push_back( kIMO );
		}
	}
	if( iRecoveryJSProductPointRate || iRecoveryJSProductPointValue )
	{
		int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness()); // 1차 주 스킬만 피로도가 있다
		int iAddValue = (iMaxExhaustion * iRecoveryJSProductPointRate / ABILITY_RATE_VALUE) + iRecoveryJSProductPointValue;
		if( S_PST_JS1_RecoveryExhaustion const* pkPremium = pkPlayer->GetPremium().GetType<S_PST_JS1_RecoveryExhaustion>() )
		{
			iAddValue += SRateControl::GetValueRate(iAddValue, pkPremium->iRate);
		}
		if( 0 != pkPlayer->JobSkillExpertness().CurExhaustion() )
		{
			SPMO kIMO(IMET_JOBSKILL_ADD_EXHAUSTION, pkUnit->GetID(), SPMOD_JobSkillExhaustion(-iAddValue)); // 피로도 회복
			kOrder.push_back( kIMO );
		}
	}
	if( false == kOrder.empty() )
	{
		BM::Stream kPacket(PT_U_G_RUN_ACTION, static_cast< short >(GAN_SumitOrder));
		kPacket.Push( CIE_JOBSKILL );
		kOrder.WriteToPacket( kPacket );
		pkUnit->VNotify(&kPacket);
	}

	return ECT_DOTICK;
}

//////////////////////////////////////////////////////////////////
// 연계효과 유지 이펙트(연계효과의 정보를 가지고있는 이펙트)
//////////////////////////////////////////////////////////////////
void PgSkillLinkageEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		)
	{
		return;
	}
	
	int const iDmgPer2 = pkEffect->GetAbil(AT_PHY_DMG_PER2);
	if(0 < iDmgPer2)
	{
		int const iOrigDmgPer2 = pkUnit->GetAbil(AT_PHY_DMG_PER2);
		OnSetAbil(pkUnit, AT_PHY_DMG_PER2, iOrigDmgPer2+iDmgPer2);
	}

	int const iMax = AT_SKILL_LINKAGE_POINT_EFFECT_TARGET10-AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01;
	for(int i = 0; i <= iMax; ++i)
	{
		int const iTargetType = pkEffect->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01+i);
		int const iEffectNo = pkEffect->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT01+i);
		if(0 >= iTargetType
			|| 0 >= iEffectNo
			)
		{ 
			break;
		}
		OnSetAbil(pkUnit, AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01+i, iTargetType);
		OnSetAbil(pkUnit, AT_SKILL_LINKAGE_POINT_EFFECT01+i, iEffectNo);
	}
}

void PgSkillLinkageEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		)
	{
		return;
	}
	
	int const iMax = AT_SKILL_LINKAGE_POINT_EFFECT_TARGET10-AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01;
	for(int i = 0; i <= iMax; ++i)
	{
		int const iTargetType = pkEffect->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01+i);
		int const iEffectNo = pkEffect->GetAbil(AT_SKILL_LINKAGE_POINT_EFFECT01+i);
		if(0 >= iTargetType
			|| 0 >= iEffectNo
			)
		{ 
			break;
		}
		OnSetAbil(pkUnit, AT_SKILL_LINKAGE_POINT_EFFECT_TARGET01+i, 0);
		OnSetAbil(pkUnit, AT_SKILL_LINKAGE_POINT_EFFECT01+i, 0);
	}

	if(E_EDC_DELETE_BY_HIGH_LEVEL != pkEffect->GetDeleteCause())
	{
		int const iSkillLinkagePoint = pkUnit->GetAbil(AT_SKILL_LINKAGE_POINT);
		if(0 < iSkillLinkagePoint)
		{// 연계포인트 소모(SetAbil하면 안됨. OnAddAbil이 Effect컨테이너에 저장되기때문에)
			OnAddAbil(pkUnit, AT_SKILL_LINKAGE_POINT, -iSkillLinkagePoint);
		}
	}

	int const iDmgPer2 = pkEffect->GetAbil(AT_PHY_DMG_PER2);
	if(0 < iDmgPer2)
	{
		int const iNewDmgPer2 = pkUnit->GetAbil(AT_PHY_DMG_PER2) - iDmgPer2;
		OnSetAbil(pkUnit, AT_PHY_DMG_PER2, iNewDmgPer2);
	}
}
//////////////////////////////////////////////////////////////////
// HP, MP등을 흡수 하는 이펙트
//////////////////////////////////////////////////////////////////
void PgDrainEffect::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		)
	{
		return;
	}
	
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	CUnit* pkCaster = NULL;
	if(pkGround)
	{
		pkCaster = pkGround->GetUnit( pkEffect->GetCaster() );
	}
	int iHPGainRate = pkEffect->GetAbil(AT_DRAIN_HP_APPLY_RATE);	// HP흡수율
	int iMPGainRate = pkEffect->GetAbil(AT_DRAIN_MP_APPLY_RATE);	// MP흡수율

	int iHPDrainTargetType = pkEffect->GetAbil(AT_DRAIN_HP_APPLY_TARGET);
	int iMPDrainTargetType = pkEffect->GetAbil(AT_DRAIN_MP_APPLY_TARGET);
	if(pkCaster
		&& ( 0 >= iHPDrainTargetType || 0 >= iMPDrainTargetType )
		)
	{
		int const iChildSkillNo = pkEffect->GetAbil(AT_CHILD_SKILL_NUM_01);
		
		if( true == pkCaster->IsUnitType(UT_PLAYER) )
		{
			PgPlayer* pkPlayer = NULL;
			pkPlayer = dynamic_cast<PgPlayer*>(pkCaster);
			if(pkPlayer)
			{// 플레이어가
				PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
				if(pkPlayerSkill)
				{
					GET_DEF( CSkillDefMgr, kSkillDefMgr);
					GET_DEF(CEffectDefMgr, kEffectDefMgr);
					for(int i=AT_CHILD_SKILL_NUM_01; i <= AT_CHILD_SKILL_NUM_10; ++i)
					{// 추가로 영향 받는 스킬들을
						int const iChildSkillNo = pkEffect->GetAbil(i);
						if(0 >= iChildSkillNo) { break; }

						int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildSkillNo);
						if(0 >= iLearnedChildSkillNo) { break; }

						// 배웠다면
						CSkillDef const* pkChildSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
						if(!pkChildSkillDef) { break; }

						int const iChildEffectNo = pkChildSkillDef->GetAbil(AT_EFFECTNUM1);	// 일단 1개만 씀
						if(0 >= iChildEffectNo) { break; }

						CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iChildEffectNo);
						if(pkEffectDef)
						{
							if( !iHPDrainTargetType )
							{
								iHPDrainTargetType = pkEffectDef->GetAbil(AT_DRAIN_HP_APPLY_TARGET);
							}
							if( !iMPDrainTargetType )
							{
								iMPDrainTargetType = pkEffectDef->GetAbil(AT_DRAIN_MP_APPLY_TARGET);
							}
							// HP흡수율을 더해주고
							iHPGainRate += pkEffectDef->GetAbil(AT_DRAIN_HP_APPLY_RATE);
							iMPGainRate += pkEffectDef->GetAbil(AT_DRAIN_MP_APPLY_RATE);
						}
					}
				}
			}
		}
	}
	
	int const iAddHP = pkEffect->GetAbil(AT_HP);
	int const iMaxHPRate = pkEffect->GetAbil(AT_R_MAX_HP);
	if(iAddHP || iMaxHPRate)
	{//HP
		int const iRemovedHP = SkillFuncUtil::OnModifyHP(pkUnit, pkEffect, iMaxHPRate, iAddHP, pkUnit, pkGround);
		int iResultAddHP = 0;
		if(0 != iHPGainRate)
		{
			iResultAddHP = (iRemovedHP*iHPGainRate) / ABILITY_RATE_VALUE;
		}
		switch(iHPDrainTargetType)
		{
		case DVAT_CASTER:
			{// 캐스터에게 적용
				ApplyDrainHPMP(pkCaster, pkEffect, DT_HP, iResultAddHP);
			}break;
		}
	}
	int iAddMP = pkEffect->GetAbil(AT_MP);
	int const iMaxMPRate = pkEffect->GetAbil(AT_R_MAX_MP);
	if(iAddMP || iMaxMPRate)
	{//MP
		int const iRemovedMP = SkillFuncUtil::OnModifyMP(pkUnit, pkEffect, iMaxMPRate, iAddMP, pkUnit, pkGround);
		int iResultAddMP = 0;
		if(0 != iMPGainRate)
		{
			iResultAddMP = (iRemovedMP*iMPGainRate) / ABILITY_RATE_VALUE;
		}
		switch(iMPDrainTargetType)
		{
		case DVAT_CASTER:
			{// 캐스터에게 적용
				ApplyDrainHPMP(pkCaster, pkEffect, DT_MP, iResultAddMP);
			}break;
		}
	}
	
}

void PgDrainEffect::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		|| !pkEffect
		)
	{
		return;
	}
}

int PgDrainEffect::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_MUSTDELETE;
}

bool PgDrainEffect::ApplyDrainHPMP(CUnit* pkUnit, CEffect* pkEffect, int const iType, int const iDrainedValue)
{// 흡수된 값(iDrainedValue)을 중심으로 대상(pkUnit)에 적용
	if(!pkEffect
		|| 0 >= iType
		|| 0 == iDrainedValue
		)
	{
		return false;
	}

	int const iDrainVal = -iDrainedValue;

	switch(iType)
	{
	case DT_HP:
		{// HP
			SkillFuncUtil::OnModifyHP(pkUnit, pkEffect, 0, iDrainVal, pkUnit, NULL);
		}break;
	case DT_MP:
		{// MP
			SkillFuncUtil::OnModifyMP(pkUnit, pkEffect, 0, iDrainVal, pkUnit, NULL);
		}break;
	default:
		{
			return false;
		}break;
	}
	return true;
}