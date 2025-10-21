#include "StdAfx.h"
#include "PgSkillFilter.h"
#include "Effect.h"
#include "Skill.h"
#include "PgControlDefMgr.h"

//////////////////////////////////////////////
//	PgSkillFilter
//////////////////////////////////////////////

void PgSkillFilter::AddExcept(int const iSkillNo)
{
	VEC_EXCEPT_SKILL::const_iterator itor = find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor == m_kExcept.end())
	{
		m_kExcept.push_back(iSkillNo);
	}
}

void PgSkillFilter::SetID(int const iEffectID)
{
	m_iID = iEffectID;
}

void PgSkillFilter::Clear()
{
	m_kExcept.clear();
	m_iID = 0;	
}

VEC_EXCEPT_SKILL* PgSkillFilter::GetExceptList()
{
	return &m_kExcept;
}

bool PgSkillFilter_EnableAll::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor = find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgSkillFilter_DisableAll::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor =find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSkillFilter_DeleteEffect::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor =find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSkillFilter_DeleteEffect_From_Caster::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor =find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSkillFilter_CannotAttack::CheckFilter(int const iSkillNo) const
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkillDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__<<L"Cannot Get SkillDef ["<<iSkillNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (pkSkillDef->GetType() <= EST_GENERAL)
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgSkillFilter_Ignore_Action_Effect::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor =find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		return false;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}

bool PgSkillFilter_Disable_OnEffect::CheckFilter(int const iSkillNo) const
{
	VEC_EXCEPT_SKILL::const_iterator itor =find(m_kExcept.begin(), m_kExcept.end(), iSkillNo);
	if (itor != m_kExcept.end())
	{
		return false;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//	PgSkillFilterMgr
//////////////////////////////////////////////
PgSkillFilterMgr::FilterEnableAllPool PgSkillFilterMgr::m_kEnableAllPool(10,10);
PgSkillFilterMgr::FilterDisableAllPool PgSkillFilterMgr::m_kDisableAllPool(10,10);
PgSkillFilterMgr::FilterDeleteEffectPool PgSkillFilterMgr::m_kDeleteEffectPool(10,10);
PgSkillFilterMgr::FilterDeleteEffectFromCasterPool PgSkillFilterMgr::m_kDeleteEffectFromCasterPool(10,10);
PgSkillFilterMgr::FilterCannotAttackPool PgSkillFilterMgr::m_kCannotAttackPool(10,10);
PgSkillFilterMgr::FilterIgnoreActionEffectPool PgSkillFilterMgr::m_kIgnoreActionEffectPool(10,10);
PgSkillFilterMgr::FilterDisableOnEffectPool PgSkillFilterMgr::m_kDisableOnEffectPool(10,10);

void PgSkillFilterMgr::Init()
{
	CONT_SKILL_FILTER::iterator itor = m_kFilterEffect.begin();
	while (itor != m_kFilterEffect.end())
	{
		PgSkillFilter* pkFilter = itor->second;
		switch(pkFilter->Type())
		{
		case ESFilter_Enable_All:
			{
				PgSkillFilter_EnableAll* pkEnable = dynamic_cast<PgSkillFilter_EnableAll*>(pkFilter);
				if (pkEnable)
				{
					m_kEnableAllPool.Delete(pkEnable);
				}
			}break;
		case ESFilter_Disable_All:
			{
				PgSkillFilter_DisableAll* pkDisable = dynamic_cast<PgSkillFilter_DisableAll*>(pkFilter);
				if (pkDisable)
				{
					m_kDisableAllPool.Delete(pkDisable);
				}
			}break;
		case ESFilter_Delete_Effect:
			{
				PgSkillFilter_DeleteEffect* pkDeleteEffect = dynamic_cast<PgSkillFilter_DeleteEffect*>(pkFilter);
				if (pkDeleteEffect)
				{
					m_kDeleteEffectPool.Delete(pkDeleteEffect);
				}
			}break;
		case ESFilter_Delete_Effect_From_Caster:
			{
				PgSkillFilter_DeleteEffect_From_Caster* pkDeleteEffect = dynamic_cast<PgSkillFilter_DeleteEffect_From_Caster*>(pkFilter);
				if (pkDeleteEffect)
				{
					m_kDeleteEffectFromCasterPool.Delete(pkDeleteEffect);
				}
			}break;
		case ESFilter_CannotAttack:
			{
				PgSkillFilter_CannotAttack* pkCannotAttack = dynamic_cast<PgSkillFilter_CannotAttack*>(pkFilter);
				if (pkCannotAttack)
				{
					m_kCannotAttackPool.Delete(pkCannotAttack);
				}
			}break;
		case ESFilter_Ignore_Action_Effect:
			{
				PgSkillFilter_Ignore_Action_Effect* pkIgnoreActionEffect = dynamic_cast<PgSkillFilter_Ignore_Action_Effect*>(pkFilter);
				if (pkIgnoreActionEffect)
				{
					m_kIgnoreActionEffectPool.Delete(pkIgnoreActionEffect);
				}
			}break;
		case ESFilter_Disable_OnEffect:
			{
				PgSkillFilter_Disable_OnEffect* pkDisable_OnEffect = dynamic_cast<PgSkillFilter_Disable_OnEffect*>(pkFilter);
				if (pkDisable_OnEffect)
				{
					m_kDisableOnEffectPool.Delete(pkDisable_OnEffect);
				}
			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				INFO_LOG(BM::LOG_LV5, __FL__<<L"unknown SkillFilter type ["<<static_cast<int>(pkFilter->Type())<<L"]");
			}break;
		}

		++itor;
	}
	m_kFilterEffect.clear();
	m_kReserveFilter.clear();
}

bool PgSkillFilterMgr::AdaptEffect(int const iEffectNo)
{
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iEffectNo);
	if( !pkEffectDef )
	{
		CAUTION_LOG( BM::LOG_LV4, __FL__ << _T("Cannot get EffectDef<") << iEffectNo << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEffectDef is NULL"));
		return false;
	}

	ESkillFilterType eFilterType = (ESkillFilterType) pkEffectDef->GetAbil(AT_SKILL_FILTER_TYPE);
	if (ESFilter_None == eFilterType)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get Abil Failed!"));
		return false;
	}
	PgSkillFilter* pkNew = CreateNew(iEffectNo, eFilterType);
	if (NULL == pkNew)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkNew is NULL"));
		return false;
	}
	int iCount = 0;
	int iExcept = 0;
	while ((10 > iCount) &&  0 < (iExcept = pkEffectDef->GetAbil(AT_FILTER_EXCEPT_01+iCount)))
	{
		pkNew->AddExcept(iExcept);
		++iCount;
	}

	iCount = 0;
	while ((100 > iCount) && 0 < (iExcept = pkEffectDef->GetAbil(AT_FILTER_EXCEPT_EX_01+iCount)))
	{
		pkNew->AddExcept(iExcept);
		++iCount;
	}

	auto kRet = m_kFilterEffect.insert(std::make_pair(iEffectNo, pkNew));
	if( !kRet.second )
	{
		//CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't insert Effect[ID: "<<iEffectNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
		RemovePool(pkNew);
	}

	return true;
}

void PgSkillFilterMgr::RemoveEffect(int const iEffectNo)
{
	CONT_SKILL_FILTER::iterator itor = m_kFilterEffect.find(iEffectNo);
	if (itor != m_kFilterEffect.end())
	{
		RemovePool(itor->second);
		m_kFilterEffect.erase(itor);
	}
}

bool PgSkillFilterMgr::CheckFilter(int const iSkillNo, SSFilter_Result* pkResult, ESkillFilterType eFilterType) const
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	int iBasicSkillNo = kSkillDefMgr.GetAbil(iSkillNo, AT_PARENT_SKILL_NUM);
	if (iBasicSkillNo == 0)
	{
		iBasicSkillNo = iSkillNo;
		//INFO_LOG(BM::LOG_LV5, _T("[%s] ParentSkill num is ZERO SkillID[%d]"), __FUNCTIONW__, iSkillNo);
	}

	if( 1 == kSkillDefMgr.GetAbil(iSkillNo, AT_IS_CHAIN_COMBO) 
		|| 1 == kSkillDefMgr.GetAbil(iSkillNo, AT_IS_OLD_COMBO) )
	{
		return true;
	}

	CONT_SKILL_FILTER::const_iterator itor = m_kFilterEffect.begin();
	if(ESFilter_None == eFilterType)
	{
		while (itor != m_kFilterEffect.end())
		{
			if (ESFilter_Delete_Effect != itor->second->Type() && 
				ESFilter_Delete_Effect_From_Caster != itor->second->Type() && 
				ESFilter_Ignore_Action_Effect != itor->second->Type())
			{
				if ( !itor->second->CheckFilter(iBasicSkillNo) )
				{
					if (NULL != pkResult)
					{
						pkResult->eResult = SSFilter_Result::ESFResult_LimitEffect;
						pkResult->iCauseID = itor->first;
					}
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			++itor;
		}
	}
	else
	{
		while (itor != m_kFilterEffect.end())
		{
			if (eFilterType == itor->second->Type())
			{
				if ( !itor->second->CheckFilter(iBasicSkillNo) )
				{
					if (NULL != pkResult)
					{
						pkResult->eResult = SSFilter_Result::ESFResult_LimitEffect;
						pkResult->iCauseID = itor->first;
					}
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			++itor;
		}
	}

	return true;
}

PgSkillFilter* PgSkillFilterMgr::CreateNew(int const iEffectNo, ESkillFilterType eType)
{
	PgSkillFilter* pkNew = NULL;
	switch(eType)
	{
	case ESFilter_Enable_All:
		{
			pkNew = (PgSkillFilter*) m_kEnableAllPool.New();			
		}break;
	case ESFilter_Disable_All:
		{
			pkNew = (PgSkillFilter*) m_kDisableAllPool.New();
		}break;
	case ESFilter_Delete_Effect:
		{
			pkNew = (PgSkillFilter*) m_kDeleteEffectPool.New();			
		}break;
	case ESFilter_Delete_Effect_From_Caster:
		{
			pkNew = (PgSkillFilter*) m_kDeleteEffectFromCasterPool.New();
		}break;
	case ESFilter_CannotAttack:
		{
			pkNew = (PgSkillFilter*) m_kCannotAttackPool.New();
		}break;
	case ESFilter_Ignore_Action_Effect:
		{
			pkNew = (PgSkillFilter*) m_kIgnoreActionEffectPool.New();
		}break;
	case ESFilter_Disable_OnEffect:
		{
			pkNew = (PgSkillFilter*) m_kDisableOnEffectPool.New();
		}break;
	
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Unknown SkillFilter type ["<<static_cast<int>(eType)<<L"]");
		}break;
	}

	if (pkNew != NULL)
	{
		pkNew->Clear();
		pkNew->SetID(iEffectNo);
	}
	return pkNew;
}

void PgSkillFilterMgr::RemovePool(PgSkillFilter* pkFilter)
{
	if (pkFilter == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFilter is NULL"));
		return;
	}
	switch(pkFilter->Type())
	{
	case ESFilter_Enable_All:
		{
			PgSkillFilter_EnableAll* pkEnable = dynamic_cast<PgSkillFilter_EnableAll*>(pkFilter);
			if (pkEnable)
			{
				m_kEnableAllPool.Delete(pkEnable);
			}
		}break;
	case ESFilter_Disable_All:
		{
			PgSkillFilter_DisableAll* pkDisable = dynamic_cast<PgSkillFilter_DisableAll*>(pkFilter);
			if (pkDisable)
			{
				m_kDisableAllPool.Delete(pkDisable);
			}
		}break;
	case ESFilter_Delete_Effect:
		{
			PgSkillFilter_DeleteEffect* pkDeleteEffect = dynamic_cast<PgSkillFilter_DeleteEffect*>(pkFilter);
			if (pkDeleteEffect)
			{
				m_kDeleteEffectPool.Delete(pkDeleteEffect);
			}
		}break;
	case ESFilter_Delete_Effect_From_Caster:
		{
			PgSkillFilter_DeleteEffect_From_Caster* pkDeleteEffect = dynamic_cast<PgSkillFilter_DeleteEffect_From_Caster*>(pkFilter);
			if (pkDeleteEffect)
			{
				m_kDeleteEffectFromCasterPool.Delete(pkDeleteEffect);
			}
		}break;
	case ESFilter_CannotAttack:
		{
			PgSkillFilter_CannotAttack* pkCannotAttack = dynamic_cast<PgSkillFilter_CannotAttack*>(pkFilter);
			if (pkCannotAttack)
			{
				m_kCannotAttackPool.Delete(pkCannotAttack);
			}
		}break;
	case ESFilter_Ignore_Action_Effect:
		{
			PgSkillFilter_Ignore_Action_Effect* pkIgnoreActionEffect = dynamic_cast<PgSkillFilter_Ignore_Action_Effect*>(pkFilter);
			if (pkIgnoreActionEffect)
			{
				m_kIgnoreActionEffectPool.Delete(pkIgnoreActionEffect);
			}
		}break;
	case ESFilter_Disable_OnEffect:
		{
			PgSkillFilter_Disable_OnEffect* pkDisable_OnEffect = dynamic_cast<PgSkillFilter_Disable_OnEffect*>(pkFilter);
			if (pkDisable_OnEffect)
			{
				m_kDisableOnEffectPool.Delete(pkDisable_OnEffect);
			}
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Unknown SkillFilter type ["<<static_cast<int>(pkFilter->Type())<<L"]");
		}break;
	}
}

PgSkillFilter* PgSkillFilterMgr::GetSkillFilter(int const nSkillNo) const
{
	CONT_SKILL_FILTER::const_iterator itor = m_kFilterEffect.find(nSkillNo);
	if (itor != m_kFilterEffect.end())
	{
		return (*itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgSkillFilterMgr::WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)const
{
	//실제로 걸리지 않고 예약된 필터들이 있을 경우
	if(m_kReserveFilter.empty())
	{
		rkPacket.Push((int)m_kFilterEffect.size());
		for(CONT_SKILL_FILTER::const_iterator itor = m_kFilterEffect.begin(); itor != m_kFilterEffect.end(); ++itor)
		{
			rkPacket.Push((*itor).first); //필터 번호만 보내주면 된다.
		}
	}
	else
	{
		rkPacket.Push((int)m_kReserveFilter.size());
		for(CONT_RESERVE_FILTER::const_iterator itor = m_kReserveFilter.begin(); itor != m_kReserveFilter.end(); ++itor)
		{
			rkPacket.Push((*itor)); //필터 번호만 보내주면 된다.
		}
	}
}

void PgSkillFilterMgr::ReadFromPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType)
{
	Init();

	int iSize = 0;
	rkPacket.Pop( iSize );
	for(int i = 0; i < iSize; ++i)
	{
		int iFilterEffectNo = 0;
		rkPacket.Pop( iFilterEffectNo );
		if(0 < iFilterEffectNo)
		{
			//컨텐츠서버에서는 실제로 필터가 추가 되지 못한다.
			bool bRet = AdaptEffect(iFilterEffectNo);
			if(false == bRet)
			{
				//실패시에는 예약 한다.
				m_kReserveFilter.push_back(iFilterEffectNo);
			}
		}
	}
}
