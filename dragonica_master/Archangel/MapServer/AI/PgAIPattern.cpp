#include "stdafx.h"
#include "PgAIPattern.h"

PgAIPattern::PATTERN_ACTION_INFO_POOL PgAIPattern::m_kPatternActionInfoPool(5, 5);
//PgAIPattern::VECTOR_PATTERN_ACTION_INFO PgAIPattern::m_kNullPatternActionInfo;

PgAIPattern::PgAIPattern(void)
{
}

PgAIPattern::~PgAIPattern(void)
{
	Release();
}

void PgAIPattern::SetInfo(short int sID, const wchar_t* pwszName)
{
	ID(sID);
	m_kName = pwszName;
}

bool PgAIPattern::ChangeSort(SPatternActionInfo* pkFirst, SPatternActionInfo* pkSecond)
//bool ChangeSortPatternActionInfo(SPatternActionInfo* pkFirst, SPatternActionInfo* pkSecond)
{
	if (pkFirst->byWeight > pkSecond->byWeight)
	{
		return true;
	}
	return false;
}

void PgAIPattern::AddTransit(int iFrom, int iTo, BYTE byWeight)
{
	{
		CONT_AI_ACTION_CHANGE::iterator itor = m_kChange.find(iFrom);
		if (itor == m_kChange.end())
		{
			SPatternActionInfo* pkNew = m_kPatternActionInfoPool.New();
			pkNew->Set((EAIActionType)iFrom, (EAIActionType)iTo, byWeight);
			VECTOR_PATTERN_ACTION_INFO kVector;
			kVector.push_back(pkNew);
			auto kPair = m_kChange.insert(std::make_pair(iFrom, kVector));
			if ( !kPair.second )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("insert Failed From=") << iFrom << _T(" To=") << iTo );
				m_kPatternActionInfoPool.Delete( pkNew );
			}
		}
		else
		{
			VECTOR_PATTERN_ACTION_INFO& rkToVector = itor->second;
			VECTOR_PATTERN_ACTION_INFO::iterator itor2 = rkToVector.begin();
			while (itor2 != rkToVector.end())
			{
				if ((*itor2)->eTo == (EAIActionType)iTo)
				{
					//INFO_LOG(BM::LOG_LV5, _T("[%s] Transition information already exist Pattern[%d], From[%d], To[%d]"), __FUNCTIONW__, ID(), iFrom, iTo);
					(*itor2)->byWeight = byWeight;
					return;
				}
				++itor2;
			}
			SPatternActionInfo* pkNew = m_kPatternActionInfoPool.New();
			pkNew->Set((EAIActionType)iFrom, (EAIActionType)iTo, byWeight);
			itor->second.push_back(pkNew);
			sort(itor->second.begin(), itor->second.end(), PgAIPattern::ChangeSort);
		}
	}

	//sort(m_kChange.begin(), m_kChange.end());
}

bool PgAIPattern::GetNextAction(int const iCurrentAction, PgAIPattern::VECTOR_PATTERN_ACTION_INFO const* &pkOutVector)const
{
	CONT_AI_ACTION_CHANGE::const_iterator& itor = m_kChange.find(iCurrentAction);
	if (itor == m_kChange.end())
	{
		return false;
	}
	pkOutVector = &itor->second;
	return true;
}

bool PgAIPattern::IsPatternExist(int const iAction) const
{
	CONT_AI_ACTION_CHANGE::const_iterator& itor = m_kChange.find(iAction);
	if (itor == m_kChange.end())
	{
		return false;
	}

	return true;
}

void PgAIPattern::Release()
{
	CONT_AI_ACTION_CHANGE::iterator itor = m_kChange.begin();
	while (itor != m_kChange.end())
	{
		VECTOR_PATTERN_ACTION_INFO& rkVector = itor->second;
		VECTOR_PATTERN_ACTION_INFO::iterator itor2 = rkVector.begin();
		while (itor2 != rkVector.end())
		{
			m_kPatternActionInfoPool.Delete(*itor2);
			++itor2;
		}

		++itor;
	}

	m_kChange.clear();
}

/*
const PgAIPattern::VECTOR_PATTERN_ACTION_INFO PgAIPattern::GetNullPatternActionInfo()
{
	return m_kNullPatternActionInfo;
}
*/
