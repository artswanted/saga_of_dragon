//// Skill Class
// Dukguru
//

#include "stdafx.h"
#include <Mmsystem.h>
#include "constant.h"
#include "Skill.h"
#include "TableDataManager.h"
#include "DefAbilType.h"
#include "PgEventView.h"
#include "PgControlDefMgr.h"

///////////////////////////////////////////////////////////////////
//	CSkill
///////////////////////////////////////////////////////////////////
using namespace std;

int CSkill::GetSkillNo() const
{
	return m_iSkillNo;
}

int CSkill::Enter(SActArg *pActArg)
{
	return 0;
}

int CSkill::Leave(SActArg *pActArg)
{
	return 0;
}

int CSkill::CallAction(WORD wSkillActCode, SActArg *pSkillActArg)
{
	return 0;
}

void CSkill::Init()
{
	m_iSkillNo = 0;
	m_pSkillDef = NULL;
	m_ulCheckTime = 0;
	m_ulTimeLimit = 0;
	m_eStatus = ESS_NONE;
	m_kCoolTimeMap.clear();
	m_kFilterMgr.Init();

	m_iSkillCoolTime = 0;
	m_fSkillCoolTimeRate = 0.f;
	m_iSkillCastingTime = 0;

	m_kCoolTimeMap.clear();
	m_kContForceSetInfo.clear();
}

void CSkill::InitCoolTime( MAP_COOLTIME const &kCoolTimeMap )
{
	m_kCoolTimeMap = kCoolTimeMap;

	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();

	MAP_COOLTIME::iterator itr = m_kCoolTimeMap.begin();
	while ( itr != m_kCoolTimeMap.end() )
	{
		if ( itr->second > dwServerElapsedTime )
		{
			++itr;
		}
		else
		{
			itr = m_kCoolTimeMap.erase( itr );
		}
	}
}

int CSkill::GetAbil(WORD const Type) const
{
	CSkillDef const* pkSkillDef = GetSkillDef();
	return pkSkillDef ? pkSkillDef->GetAbil(Type) : 0;
}

__int64 CSkill::GetAbil64(WORD const Type) const
{
	CSkillDef const* pkSkillDef = GetSkillDef();
	return pkSkillDef ? pkSkillDef->GetAbil64(Type) : 0;
}

DWORD CSkill::GetCoolTime(int const iSkillNo) const
{
	MAP_COOLTIME::const_iterator itor = m_kCoolTimeMap.find(iSkillNo);
	if (itor != m_kCoolTimeMap.end())
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void CSkill::AddCoolTime(int const iSkill, DWORD const dwCoolTime, int const iCoolTimeRate)
{
	int const iDiffCoolTime = static_cast<int>(dwCoolTime * static_cast<double>(iCoolTimeRate) / static_cast<double>(ABILITY_RATE_VALUE));
	DWORD const dwCalcCoolTime = std::max<int>(0,dwCoolTime - iDiffCoolTime);

	//INFO_LOG(BM::LOG_LV8, _T("[%s] Skill[%d], CoolTime[%08X]"), __FUNCTIONW__, iSkill, ulCoolTime);
	auto kPair = m_kCoolTimeMap.insert( std::make_pair(iSkill, dwCalcCoolTime) );
	if ( !kPair.second )
	{
		kPair.first->second = dwCalcCoolTime;
	}
}

CSkillDef const* CSkill::GetSkillDef() const
{
	// 맨처음 Call에서는 m_iSkillNo == 0 이므로
	// NULL을 리턴하게 된다
	// NULL 리턴을 방지하기 위해 DB에 SkillNo == 0인 값을 넣어 주어야 한다.
	if (m_pSkillDef == NULL || m_pSkillDef->No() != m_iSkillNo)
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		m_pSkillDef = (CSkillDef*) kSkillDefMgr.GetDef(m_iSkillNo);
	}
	
	if( !m_pSkillDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"NotFind DefSkillNo: " << m_iSkillNo);
	}
	return m_pSkillDef;
}

int CSkill::GetEffectNo() const
{
	CSkillDef const* pkSkillDef = GetSkillDef();
	return pkSkillDef ? pkSkillDef->GetEffectNo() : 0;
}

unsigned long CSkill::GetCheckTime() const
{
	return m_ulCheckTime;
}

void CSkill::SetCheckTime(unsigned long const ulTime)
{
	m_ulCheckTime = ulTime;
}

EActionResultCode CSkill::ReqCasting(int const iSkillNo, DWORD const dwCurrentTime,int const iCastTimeRate)
{
	if (!CheckCoolTime(iSkillNo, dwCurrentTime))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CoolTime"));
		return EActionR_Err_CoolTime;
	}
	CSkillDef const* pkSkillDef = GetSkillDef();
	int const iCastTime = pkSkillDef ? pkSkillDef->GetAbil(AT_CAST_TIME) : 0;
	int iAddCastTime = iCastTime;
	iAddCastTime -= static_cast<int>(iCastTime * (static_cast<double>(iCastTimeRate)/static_cast<double>(ABILITY_RATE_VALUE)));

	// 캐스팅 타임이 존재하는 스킬만 추가 캐스팅 시간을 적용 한다.
	if(0 < iAddCastTime)
	{
		iAddCastTime += m_iSkillCastingTime;
	}

	// 캐스팅 타임이 0보다 작아질 경우
	iAddCastTime = __max(0, iAddCastTime);
	
	m_iSkillNo = iSkillNo;
	m_eStatus = ESS_REQ_CAST;
	m_ulTimeLimit = BM::GetTime32() + iAddCastTime;
	return EActionR_Success;
}

// AI에서 Skill사용하기 전에 Reserve 해야 할 필요성이 있다.

bool CSkill::IsForceSkill()const
{
	return !m_kContForceSetInfo.empty();
}

bool CSkill::IsForceSetFlag(int const iFlag)const
{
	return IsForceSkill() ? (m_kContForceSetInfo[0].eForceSetFlag&iFlag) : false;
}

void CSkill::DoForceSkill()
{
	if( IsForceSkill() )
	{
		int const iForceSkillNo = m_kContForceSetInfo[0].iForceSkillNo;
		Reserve(iForceSkillNo,true);
		m_kContForceSetInfo.pop_front();
	}
}

int CSkill::GetForceSkillNo()const
{
	return IsForceSkill() ? m_kContForceSetInfo[0].iForceSkillNo : 0;
}

BM::Stream CSkill::GetForceSkillParam()const
{
	return IsForceSkill() ? m_kContForceSetInfo[0].kParam : BM::Stream();
}

bool CSkill::ForceReserve(int const iSkillNo, EForceSetFlag const eForceSetFlag, BM::Stream const& kParam)
{
	if(0==iSkillNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if(EFSF_CLEAR_RESERVE&eForceSetFlag)
	{
		ClearForceReserve();
	}
	CONT_FORCESETINFO::value_type kValue(iSkillNo,eForceSetFlag,kParam);
	m_kContForceSetInfo.push_back(kValue);
	return true;
}

void CSkill::ClearForceReserve()
{
	m_kContForceSetInfo.clear();
}

bool CSkill::Reserve(int const iSkillNo, bool const bForceSet, DWORD const dwCurrentTime)
{
	if (!bForceSet && 0 != iSkillNo && !CheckCoolTime(0, dwCurrentTime))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_iSkillNo = iSkillNo;
	//if (iSkillNo != 0)
	//{
	//	AddCoolTime(0, ulNow+GetSkillDef()->GetAbil(AT_GLOBAL_COOLTIME));
	//	INFO_LOG(BM::LOG_LV9, _T("[%s] Skill[%08d] Global CoolTime[%d]"), __FUNCTIONW__, m_iSkillNo, GetSkillDef()->GetAbil(AT_GLOBAL_COOLTIME));
	//}
	return true;
}

EActionResultCode CSkill::Casting(int const iSkillNo, DWORD const dwCurrentTime)
{
	m_iSkillNo = iSkillNo;
	CSkillDef const* pkSkillDef = GetSkillDef();
	int const iType = pkSkillDef ? pkSkillDef->GetAbil(AT_CASTTYPE) : 0;
	if (E_SCAST_CASTSHOT != iType)
	{
		m_iSkillNo = 0;
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Skill is not E_SCAST_CASTSHOT SkillNo["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_Casttype"));
		return EActionR_Err_CastType;
	}
	// CoolTime 검사
	if (!CheckCoolTime(iSkillNo, dwCurrentTime))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CoolTime"));
		return EActionR_Err_CoolTime;
	}
	m_eStatus = ESS_CASTTIME;
	return EActionR_Success;
}

EActionResultCode CSkill::Fire(int const iSkillNo, DWORD const dwCurrentTime,int const iCoolTimeRate)
{
	m_iSkillNo = iSkillNo;
	CSkillDef const* const pkSkillDef = GetSkillDef();
	if(!pkSkillDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("SkillDef is NULL"));
		return	EActionR_Err_NoSkillDef;
	}

	ESkillCastType eCastType = (ESkillCastType) pkSkillDef->GetAbil(AT_CASTTYPE);
	if (E_SCAST_CASTSHOT == eCastType)
	{
		if (m_iSkillNo != iSkillNo && ESS_CASTTIME != m_eStatus)
		{
			m_iSkillNo = 0;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CastingErr"));
			return EActionR_Err_CastingErr;
		}
	}
	// CoolTime 검사
	if (!CheckCoolTime(iSkillNo, dwCurrentTime))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CoolTime"));
		return EActionR_Err_CoolTime;
	}
	// CoolTime 시작
	int iAddCoolTime = pkSkillDef->GetAbil(ATS_COOL_TIME);

	// CoolTime이 0보다 큰 경우에 만 적용 된다.
	if(0 < iAddCoolTime)
	{
		//INFO_LOG(BM::LOG_LV9, __FL__ << _T("SkillNo=") << iSkillNo << _T(", AddCoolTime=") << iAddCoolTime << _T(", m_iSkillCoolTime=") << m_iSkillCoolTime
		//	<< _T(", CurrentTime=") << dwCurrentTime);
		//쿨타임이 0보다 큰 경우에 만 추가 쿨타임이 적용 되어야 한다.
		iAddCoolTime += m_iSkillCoolTime;
		iAddCoolTime += std::max<int>(iAddCoolTime*m_fSkillCoolTimeRate,0);

		//0보다 작을 수는 없다.
		iAddCoolTime = __max(0, iAddCoolTime);

		AddCoolTime(iSkillNo, iAddCoolTime + dwCurrentTime, iCoolTimeRate);
		
		for(int i = AT_JOINT_COOLTIME_SKILL_NO_1; i < AT_JOINT_COOLTIME_SKILL_NO_MAX; ++i)
		{// 쿨타임을 공유하는 스킬이 존재 하면
			int const iJointCoolTimeSkillNo = pkSkillDef->GetAbil(i);
			if(0 < iJointCoolTimeSkillNo)
			{// 해당 스킬을 쿨타임 적용 시킨다
				GET_DEF(CSkillDefMgr, kSkillDefMgr);
				CSkillDef const* pkTempSkillDef = kSkillDefMgr.GetDef(iJointCoolTimeSkillNo);
				if(pkTempSkillDef 
					&& CheckCoolTime(iJointCoolTimeSkillNo, dwCurrentTime)
					)
				{
					int iJointCoolTime = pkTempSkillDef->GetAbil(AT_JOINT_COOLTIME);
					iJointCoolTime = __max(0, iJointCoolTime);
					if(iJointCoolTime)
					{// 공통 쿨타임용 어빌이 존재하면 
						AddCoolTime(iJointCoolTimeSkillNo, iJointCoolTime + dwCurrentTime, iCoolTimeRate);
					}
					else
					{// 공통 쿨타임이 없으면 사용한 스킬의 쿨타임을 사용한다
						AddCoolTime(iJointCoolTimeSkillNo, iAddCoolTime + dwCurrentTime, iCoolTimeRate);
					}
				}
			}
		}
	}

	//글로벌 쿨타임 추가
	AddCoolTime(SKILL_NO_GLOBAL_COOLTIME, dwCurrentTime + pkSkillDef->GetAbil(AT_GLOBAL_COOLTIME), iCoolTimeRate);
	m_eStatus = ESS_FIRE;
	return EActionR_Success_Fire;
}

EActionResultCode CSkill::TargetModified(int const iSkillNo)
{
	// 2008.01.12 : 모든 스킬이 기본적으로 ModifedTarget 능력을 갖게 되었다.
	//if (m_iSkillNo != iSkillNo || (!GetSkillDef()->IsSkillAtt(SAT_MAINTENANCE_CAST)))
	//{
	//	m_iSkillNo = 0;
	//	return EActionR_Err_MCastAttr;
	//}
	// Toggle 스킬도 TargetModifed 가 올 수 있다.
	CSkillDef const* pkSkillDef = GetSkillDef();
	ESkillType const eType = pkSkillDef ? static_cast<ESkillType>(pkSkillDef->GetType()) : EST_NONE;
	//return (eType == EST_TOGGLE) ? (EActionR_Success) : (EActionR_Success_Fire);
	return EActionR_Success_Fire;
}

EActionResultCode CSkill::CheckSkillAction(int const iSkillNo, ESkillStatus const eStatus, DWORD const dwCurrentTime,int const iCoolTimeRate,int const iCastTimeRate)
{
	EActionResultCode eResult = EActionR_Err_SkillStatus;
	switch(eStatus)
	{
	case ESS_CASTTIME:
		{
			eResult = Casting(iSkillNo, dwCurrentTime);
		} break;
	case ESS_FIRE:
		{
			eResult = Fire(iSkillNo, dwCurrentTime, iCoolTimeRate);
		} break;
	case ESS_REQ_CAST:
		{
			eResult = ReqCasting(iSkillNo, dwCurrentTime, iCastTimeRate);
		} break;
	case ESS_TARGETLISTMODIFY:
		{
			eResult = TargetModified(iSkillNo);
		} break;
	case ESS_TOGGLE_ON:
	case ESS_TOGGLE_OFF:
		{
			eResult = ToggleOnOff(iSkillNo, eStatus, dwCurrentTime, iCoolTimeRate);
		} break;
	case ESS_TIMEWAITING:
		{
			eResult = EActionR_Success;
		} break;
	default :
		{
			m_iSkillNo = 0;
			eResult = EActionR_Err_SkillStatus;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		} break;
	}
	return eResult;
}

// bFired : player에 의해서 스킬 사용된 것이냐? / 혹은 Skill유지 시간 경과로 자동 호출된 것이냐?
EActionResultCode CSkill::ToggleOnOff(int const iSkillNo, ESkillStatus const eStatus, DWORD const dwCurrentTime,int const iCoolTimeRate, bool const bFired)
{
	if (bFired)
	{
		m_iSkillNo = iSkillNo;
	}
	// ToggleOff 될때 부터 Cooltime 시작된다.
	if (ESS_TOGGLE_ON == eStatus)
	{
		// CoolTime 검사
		if (!CheckCoolTime(iSkillNo, dwCurrentTime))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return EActionR_Err_CoolTime"));
			return EActionR_Err_CoolTime;
		}
	}
	else	// eStatus == ESS_TOGGLE_OFF
	{
		//  (CoolTime 시작)
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if (pkSkillDef != NULL)
		{
			AddCoolTime(iSkillNo, dwCurrentTime + pkSkillDef->GetAbil(ATS_COOL_TIME),iCoolTimeRate);
			for(int i = AT_JOINT_COOLTIME_SKILL_NO_1; i < AT_JOINT_COOLTIME_SKILL_NO_MAX; ++i)
			{// 쿨타임을 공유하는 스킬이 존재 하면
				int const iJointCoolTimeSkillNo = pkSkillDef->GetAbil(i);
				if(0 < iJointCoolTimeSkillNo)
				{
					CSkillDef const* pkTempSkillDef = kSkillDefMgr.GetDef(iJointCoolTimeSkillNo);
					if(pkTempSkillDef
						&& CheckCoolTime(iJointCoolTimeSkillNo, dwCurrentTime)
						)
					{
						if(pkTempSkillDef->GetAbil(AT_JOINT_COOLTIME))
						{// 공통 쿨타임용 어빌이 존재하면 
							AddCoolTime(iJointCoolTimeSkillNo, dwCurrentTime + pkTempSkillDef->GetAbil(AT_JOINT_COOLTIME), iCoolTimeRate);
						}
						else
						{// 공통 쿨타임이 없으면 사용한 스킬의 쿨타임을 사용한다
							AddCoolTime(iJointCoolTimeSkillNo, dwCurrentTime + pkSkillDef->GetAbil(ATS_COOL_TIME), iCoolTimeRate);
						}
					}
				}
			}
		}
	}
	return EActionR_Success_Toggle;
	//return EActionR_Success;
}

bool CSkill::CheckCoolTime(int const iSkillNo, DWORD const dwCurrentTime)
{
	DWORD const dwNow = (dwCurrentTime == 0) ? g_kEventView.GetServerElapsedTime() : dwCurrentTime;
	if (0 < iSkillNo)
	{
		MAP_COOLTIME::iterator itor = m_kCoolTimeMap.find(iSkillNo);
		if (itor != m_kCoolTimeMap.end())
		{
			DWORD const dwCoolTime = itor->second;
			//INFO_LOG(BM::LOG_LV9, _T("[%s] CoolTime Skill Now[%I64d] / [%10d][%I64d]"), __FUNCTIONW__, u64Now, itor->first, itor->second);
			if ( dwNow < dwCoolTime )
			{
				m_iSkillNo = 0;
				//INFO_LOG(BM::LOG_LV8, _T("[%s] Cooltime error Skill[%d], CoolTime=%I64d, Now=%I64d"), __FUNCTIONW__, iSkillNo, u64CoolTime, u64Now);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			// CoolTime 지났으므로, 삭제 해 준다.
			m_kCoolTimeMap.erase(itor);
		}
	}

	// 글로벌 쿨타임 무시 어빌이 있을 경우
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(pkSkillDef) 
	{
		if(0 < pkSkillDef->GetAbil(AT_IGNORE_GLOBAL_COOLTIME))
		{
			return true;
		}
	}

	// Global Cool Time Checking
	if (dwNow < GetCoolTime(SKILL_NO_GLOBAL_COOLTIME))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

void CSkill::EffectFilter(bool const bAdd, int const iEffectID)
{
	if (bAdd)
	{
		m_kFilterMgr.AdaptEffect(iEffectID);
	}
	else
	{
		m_kFilterMgr.RemoveEffect(iEffectID);
	}
}

bool CSkill::CheckFilter(int const iSkillNo, SSFilter_Result* pkResult, ESkillFilterType eFilterType)
{
	/*
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	VERIFY_INFO_LOG_RUN(return false, VPARAM(pkSkillDef != NULL, BM::LOG_LV4, _T("[%s] Cannot get skilldef [%d]"), __FUNCTIONW__, iSkillNo));
	int iCount = 0;
	int iEffect = 0;
	while ((iCount++ < 10) && (iEffect = pkDef->GetAbil(AT_NEED_EFFECT_01)) > 0)
	{
	}
	*/
	return m_kFilterMgr.CheckFilter(iSkillNo, pkResult, eFilterType);
}

PgSkillFilter* CSkill::GetSkillFilter(int const iSkillNo) const
{
	return m_kFilterMgr.GetSkillFilter(iSkillNo);
}

void CSkill::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	WriteToPacket_CoolTimeMap( rkPacket );
	rkPacket.Push(m_iSkillCoolTime);
	rkPacket.Push(m_fSkillCoolTimeRate);
	rkPacket.Push(m_iSkillCastingTime);

	//현재 적용 되어 있는 스킬 필터들도 보낸다.
	m_kFilterMgr.WriteToPacket(rkPacket, kWriteType);
}

void CSkill::WriteToPacket_CoolTimeMap( BM::Stream &rkPacket )const
{
	PU::TWriteTable_AA( rkPacket, m_kCoolTimeMap );
}

void CSkill::ReadFromPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)
{
	m_kCoolTimeMap.clear();

	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();
	
	MAP_COOLTIME::key_type kKey = 0;
	MAP_COOLTIME::mapped_type dwCoolTime = 0;
	size_t iSize = 0;
	rkPacket.Pop( iSize );
	while ( iSize-- )
	{
		rkPacket.Pop( kKey );
		rkPacket.Pop( dwCoolTime );
		if ( dwCoolTime > dwServerElapsedTime )
		{
			m_kCoolTimeMap.insert( std::make_pair( kKey, dwCoolTime ) );
		}
	}

	rkPacket.Pop(m_iSkillCoolTime);
	rkPacket.Pop(m_fSkillCoolTimeRate);
	rkPacket.Pop(m_iSkillCastingTime);

	//현재 적용 되어 있는 스킬 필터들도 추가한다.
	m_kFilterMgr.ReadFromPacket(rkPacket, kWriteType);
}

void CSkill::GetFirstCoolTime(CONST_COOLTIME_INDEX& kIndex) const
{
	kIndex = m_kCoolTimeMap.begin();
}


bool CSkill::GetNextCoolTime(CONST_COOLTIME_INDEX& kIndex, SCoolTimeInfo& rkInfo) const
{
	while (m_kCoolTimeMap.end() != kIndex)
	{
		rkInfo.iSkillNo = kIndex->first;
		rkInfo.dwCoolTime = kIndex->second;
	
		++kIndex;
		return true;
	}
	rkInfo.iSkillNo = 0;
	rkInfo.dwCoolTime = 0;
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CSkill::SetSkillCoolTime(int const iAddCoolTime)
{
	m_iSkillCoolTime = iAddCoolTime;
}
void CSkill::SetSkillCoolTimeRate(int const iAddCoolTime)
{
	m_fSkillCoolTimeRate = iAddCoolTime/ABILITY_RATE_VALUE;
}
void CSkill::SetSkillCastingTime(int const iAddCastingTime)
{
	m_iSkillCastingTime = iAddCastingTime;
}

void CSkill::ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate)
{
	for(MAP_COOLTIME::iterator iter = m_kCoolTimeMap.begin();iter != m_kCoolTimeMap.end();++iter)
	{
		if(dwCurrentTime < (*iter).second)
		{
			int const iCoolTime = (*iter).second - dwCurrentTime;
			int const iDiffCoolTime = static_cast<int>(iCoolTime * static_cast<double>(iCoolTimeRate)/static_cast<double>(ABILITY_RATE_VALUE));
			(*iter).second = dwCurrentTime + std::max(0,(iCoolTime - iDiffCoolTime));
		}
	}
}

void CSkill::ClearCoolTime()
{
	m_kCoolTimeMap.clear();
}

CSkill::SKILL_STACK::value_type CSkill::PopSkillStack()
{
	if (m_kSkillStack.size() == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	SKILL_STACK::value_type kValue = m_kSkillStack.back();
	m_kSkillStack.pop_back();
	return kValue;
}

void CSkill::PushSkillStack(SKILL_STACK::value_type const& rkValue)
{
	if (m_kSkillStack.size() > MAX_SKILL_STACK_SIZE)
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Stack is too large StackSize["<<m_kSkillStack.size()<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Maximum SkillStack Size"));
		return;
	}
	m_kSkillStack.push_back(rkValue);
}

void CSkill::SwapSkillStack(SKILL_STACK& rkNewStack)
{
	m_kSkillStack.swap(rkNewStack);
}

void CSkill::SaveToDBCoolTimeMap(MAP_COOLTIME &kOutCont)
{
	MAP_COOLTIME::const_iterator iter = m_kCoolTimeMap.begin();
	GET_DEF(CSkillDefMgr, kSkillDefMgr);

	DWORD const dwCurTime = g_kEventView.GetServerElapsedTime();
	
	while( iter != m_kCoolTimeMap.end() )
	{
		int const iSkillNo = (*iter).first;
		CSkillDef const* pSkillDef = kSkillDefMgr.GetDef(iSkillNo);
		if(pSkillDef)
		{
			DWORD const dwCanSaveTime = 1000 * 10;
			DWORD const dwGap =  (*iter).second - dwCurTime;
			if( pSkillDef->GetAbil(AT_SAVE_DB_SKILL_COOLTIME)
			&&	dwCanSaveTime < dwGap )
			{//DB 저장 어빌이 있는것만
				kOutCont.insert( std::make_pair((*iter).first, dwGap) );
			}
		}
		++iter;
	}
}

void CSkill::LoadFromDBCoolTimeMap( MAP_COOLTIME const &kOutCont )
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);

	MAP_COOLTIME::const_iterator iter = kOutCont.begin();
	while( kOutCont.end() != iter )
	{
		int const iSkillNo = (*iter).first;
		CSkillDef const* pDef = kSkillDefMgr.GetDef(iSkillNo);
		if(pDef)
		{
			m_kCoolTimeMap.insert( std::make_pair((*iter).first, (*iter).second+g_kEventView.GetServerElapsedTime() ) );
		}
		++iter;
	}
}

///////////////////////////////////////////////////////////////////
//	CSkillDef
///////////////////////////////////////////////////////////////////
int CSkillDef::GetAbil(WORD const wAbilType) const
{
	int iValue = 0;
	switch(wAbilType)
	{
	case AT_LEVEL:			{ iValue = m_byLv; }break;
	case AT_NEED_MP:		{ iValue = m_sMP; }break;
	case AT_NEED_HP:		{ iValue = m_sHP; }break;
	case AT_TYPE:			{ iValue = m_byType; }break;
	case AT_LEVELLIMIT:		{ iValue = m_sLevelLimit; }break;
	case AT_STATELIMIT:		{ iValue = m_iStateLimit; }break;
	case AT_WEAPON_LIMIT:	{ iValue = m_iWeaponLimit; }break;
	case AT_CAST_TIME:		{ iValue = m_sCastTime; }break;
	case AT_ATTACK_RANGE:	{ iValue = m_sRange; }break;
	//case AT_HIT_SUCCESS_VALUE:		{ iValue = m_sHitRate; }break;
	case ATS_COOL_TIME:		{ iValue = m_sCoolTime; }break;
	case AT_ANIMATION_TIME:	{ iValue = m_iAnimationTime; }break;
	case AT_PARENT_SKILL_NUM: { iValue = m_iParentSkill; }break;
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"GetAbil64 처리하시오");
		}break;
	case AT_R_HIT_SUCCESS_VALUE:
	case AT_C_HIT_SUCCESS_VALUE:
	case AT_R_DODGE_SUCCESS_VALUE:
	case AT_C_DODGE_SUCCESS_VALUE:
	case AT_R_BLOCK_SUCCESS_VALUE:
	case AT_C_BLOCK_SUCCESS_VALUE:
	case AT_R_CRITICAL_SUCCESS_VALUE:
	case AT_C_CRITICAL_SUCCESS_VALUE:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"스킬에선 동작하지 않는 어빌입니다.");
		}break;
	case AT_GLOBAL_COOLTIME:
		{
			iValue = CAbilObject::GetAbil(wAbilType);

			// 액티브나 토글일 경우 디폴트 값은 0.1초이다.
			if(EST_ACTIVE == m_byType || EST_TOGGLE == m_byType)
			{
				if(0 == iValue)
				{
					iValue = 100;
				}
			}
		}break;
	default:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
		}break;
	}
	return iValue;
}

__int64 CSkillDef::GetAbil64(WORD const wAbilType) const
{
	__int64 i64Value = 0;
	switch(wAbilType)
	{
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_DIE_EXP:
		{
			i64Value = CAbilObject::GetAbil64(wAbilType);
		}break;
	case AT_CLASSLIMIT:
		{
			return m_i64ClassLimit;
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"int64의 ["<<wAbilType<<L"]타입이 맞습니까?");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return i64Value;
}

bool CSkillDef::IsSkillAtt(ESkillAttribute eType) const
{
	return ((GetAbil(AT_SKILL_ATT) & eType) == eType) ? true : false;
}

///////////////////////////////////////////////////////////////////
//	CSkillDefMgr
///////////////////////////////////////////////////////////////////

CSkillDefMgr::CSkillDefMgr()
{
}

CSkillDefMgr::~CSkillDefMgr()
{
	Clear();
}

void CSkillDefMgr::swap(CSkillDefMgr &rkRight)
{
	m_contDef.swap(rkRight.m_contDef);
	m_DefPool.swap(rkRight.m_DefPool);
	m_kSkillDef_ActionName.swap(rkRight.m_kSkillDef_ActionName);
}

void CSkillDefMgr::Clear()
{
	T_MY_BASE_TYPE::Clear();
	m_kSkillDef_ActionName.clear();
}

bool CSkillDefMgr::Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil)
{
	bool bReturn = true;
	Clear();
	
	CONT_BASE::const_iterator base_itor = tblBase.begin();
	
	while( tblBase.end() != base_itor )
	{
		DEF *pDef = NewDef();

		pDef->No( (*base_itor).second.SkillNo );
		pDef->NameNo((*base_itor).second.NameNo);
		pDef->RscNameNo((*base_itor).second.RscNameNo);
		pDef->m_byLv = (*base_itor).second.Lv;
		//memcpy_s(pDef->m_chActionName, sizeof(TCHAR)*30, (*base_itor).second.chActionName, sizeof(TCHAR)*30); 
		SAFE_STRNCPY(pDef->m_chActionName, (*base_itor).second.chActionName);
		pDef->m_byType = (*base_itor).second.byType;


		pDef->m_i64ClassLimit = (*base_itor).second.i64ClassLimit | ( (*base_itor).second.i64DraClassLimit << DRAGONIAN_LSHIFT_VAL );


		pDef->m_sLevelLimit = (*base_itor).second.sLevelLimit;
		pDef->m_iWeaponLimit = (*base_itor).second.iWeaponLimit;
		pDef->m_iStateLimit = (*base_itor).second.iStateLimit;
		pDef->m_iParentSkill = (*base_itor).second.iParentSkill;
		pDef->m_iTarget = (*base_itor).second.iTarget;
		//pDef->m_bySelect = (*base_itor).second.bySelect;
		pDef->m_sRange = (*base_itor).second.sRange;
		pDef->m_sCastTime = (*base_itor).second.sCastTime;
		pDef->m_sCoolTime = (*base_itor).second.sCoolTime;
		pDef->m_iAnimationTime = (*base_itor).second.iAnimationTime;
		//pDef->m_sHate = (*base_itor).second.sHate;
		pDef->m_sMP = (*base_itor).second.sMP;
		pDef->m_sHP = (*base_itor).second.sHP;
		//pDef->m_iNeedSkill = (*base_itor).second.iNeedSkill;
		//pDef->m_iToggleSkill = (*base_itor).second.iToggleSkill;
		//pDef->m_sHitRate = (*base_itor).second.sHitRate;
		pDef->m_iEffectID = (*base_itor).second.iEffectID;
		//pDef->m_iEventID = (*base_itor).second.iEventID;

		for( int iIndex = 0; MAX_SKILL_ABIL_LIST > iIndex ; ++iIndex )
		{//기본 리스트 번호 찾고 
			int const iAbilNo = (*base_itor).second.aAbil[iIndex];
			if( iAbilNo )
			{
				CONT_ABIL::const_iterator abil_itor = tblAbil.find( iAbilNo );

				if( abil_itor != tblAbil.end() )
				{
					for( int j = 0; MAX_SKILL_ABIL_ARRAY > j ; ++j)
					{
						int const iType = (*abil_itor).second.aType[j];
						int const iValue = (*abil_itor).second.aValue[j];
						if(0 != iType)
						{
							switch(iType)
							{
							case AT_EXPERIENCE:
							case AT_MONEY:
							case AT_DIE_EXP:
							case AT_CLASSLIMIT:
								{
									VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Error !!");
									pDef->SetAbil64(iType, iValue);
								}break;
							default:
								{
									switch( iType )
									{
									case AT_COMBO_DELAY:
										{
											if( 0 > iValue )
											{
												VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"AT_COMBO_DELAY("<<static_cast<int>(AT_COMBO_DELAY)<<L"is can't small than 0, Value["<<iValue<<L"");
												bReturn = false;
											}
										}break;
									}


									pDef->SetAbil(iType, iValue);
								}break;
							}
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Abil List Skill["<<(*base_itor).second.SkillNo<<L"], AbilNo["<<iAbilNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
					bReturn = false;
				}
			}
		}

		auto kRet = m_contDef.insert( std::make_pair(pDef->No(), pDef) );
		if( !kRet.second )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Duplicater SkillID ["<<pDef->No()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
			bReturn = false;
		}

		std::wstring wstrActionName( pDef->GetActionName() );
		if ( wstrActionName.size() && wstrActionName != std::wstring(L"0") )
		{
			m_kSkillDef_ActionName.insert(std::make_pair(pDef->GetActionName(), pDef));
		}
		++base_itor;
	}
	return bReturn;
}

int CSkillDefMgr::GetSkillNum(std::wstring const &rkSkillName) const
{
	int iSkillID = 0;

	CONT_DEF::const_iterator itor = m_contDef.begin();
	while (m_contDef.end() != itor)
	{
		wchar_t const* pszName = itor->second->GetActionName();
		if (NULL != pszName && 0 == _wcsicmp(pszName, rkSkillName.c_str()))
		{
			return itor->first;
		}
		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << iSkillID);
	return iSkillID ;
}

int CSkillDefMgr::GetAbil(int const iDefNo, WORD const wType) const
{
	switch(wType)
	{
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_CLASSLIMIT:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"__int64 처리해야 함.");
		}break;
	}

	CSkillDef const* pkDef = GetDef(iDefNo);
	if(!pkDef)
	{ 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0; 
	}
	return pkDef->GetAbil(wType);
}

__int64 CSkillDefMgr::GetAbil64(int const iDefNo, WORD const wType) const
{
	switch(wType)
	{
	case AT_EXPERIENCE:
	case AT_MONEY:
	case AT_CLASSLIMIT:
		{
			CSkillDef const* pkDef = GetDef(iDefNo);
			if(!pkDef)
			{ 
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
				return 0; 
			}
			return pkDef->GetAbil64(wType);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"GetAbil()처리 해야함. SkillNo["<<iDefNo<<L"], AbilType["<<wType<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return 0;
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int CSkillDefMgr::GetSkillNoFromActionName(std::wstring const &rkActionName, bool const bIgnoreNameCase) const
{
	if( !bIgnoreNameCase )
	{
		CONT_SKILLDEF_NAME::const_iterator itor = m_kSkillDef_ActionName.find(rkActionName);
		if (itor != m_kSkillDef_ActionName.end())
		{
			return itor->second->No();
		}
	}
	else
	{
		CONT_DEF::const_iterator iter = m_contDef.begin();
		while( m_contDef.end() != iter )
		{
			CONT_DEF::mapped_type pkSkillDef = (*iter).second ;

			if( 0 == _wcsicmp(rkActionName.c_str(), pkSkillDef->GetActionName()) )
			{
				return (*iter).first;
			}
			++iter;
		}
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Cannot find ActionName ["<<rkActionName<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

std::wstring const CSkillDefMgr::GetActionName(int const iDefNo) const
{
	std::wstring kAction;

	CSkillDef const* pkDef = GetDef(iDefNo);
	if (pkDef)
	{
		kAction = pkDef->GetActionName();
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Null String"));
	}

	return kAction;
}

int CSkillDefMgr::GetLevelupSkill(int const iID, int const iLevel) const
{
	CSkillDef const* pkSkillDef = GetDef(iID);
	if (NULL == pkSkillDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	int const iParentSkill = pkSkillDef->GetParentSkill();
	CONT_DEF::const_iterator itor = m_contDef.begin();
	while (m_contDef.end() != itor)
	{
		if (itor->second->GetParentSkill() == iParentSkill && itor->second->GetAbil(AT_LEVEL) == iLevel)
		{
			return itor->second->No();
		}

		itor++;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int CSkillDefMgr::GetCallSkillNum(int const iID) const
{
	int iBasic = GetAbil(iID, AT_TEMPLATE_SKILLNO);
	if (0 == iBasic)
	{
		iBasic = GetAbil(iID, AT_PARENT_SKILL_NUM);
		if (0 == iBasic)
		{
			CSkillDef const* pkDef = GetDef(iID);
			if (pkDef != NULL && pkDef->GetAbil(AT_LEVEL) <= 1)
			{
				iBasic = iID;
			}
			else
			{
				iBasic = GetLevelupSkill(iID, 1);
			}
		}
	}
	return iBasic;
}

int CSkillDefMgr::GetNeedSkillDepth(int const iSkillNo, int& iCallDepth) const
{
	int iResult = 0;
	if (20 < ++iCallDepth)	// 설마 20개 넘는 NeedSkill Depth가 있을까?
	{
		// 너무 많은 CallDepth.. 뭔가 문제가 있다.
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"function CallDepth is too big CurrentSkillNo["<<iSkillNo<<L"], CallDepth["<<iCallDepth<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	int iParent = GetAbil(iSkillNo, AT_PARENT_SKILL_NUM);
	iParent = (0 < iParent) ? (iParent) : (iSkillNo);
	CSkillDef const* pkDef = GetDef(iParent);
	if (NULL == pkDef)
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Cannot get SkillDef ["<<iParent<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	int iDepth = 0;
	for (int i=0; i<10; ++i)
	{
		int const iNeedSkillNo = pkDef->GetAbil(AT_NEED_SKILL_01+i);
		if (iNeedSkillNo != 0)
		{
			iDepth = GetNeedSkillDepth(iNeedSkillNo, iCallDepth) + 1;
			if (iResult < iDepth)
			{
				iResult = iDepth;	// 최대 Depth 값을 저장한다.
			}
		}
		else
		{
			break;
		}
	}
	return iResult;
}
