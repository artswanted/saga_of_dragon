#include "stdafx.h"
#include "Variant/PgMyQuest.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgQuestResetSchedule.h"

#include "PgQuestRandom.h"
#include "PgQuestMng.h"

namespace QuestMngUtil
{
	template< typename _T >
	void VerifyIsNotZero(_T const& rValue, TCHAR const* szMsg, TCHAR const* szFunc, size_t const iLine)
	{
		if( 0 == rValue )
		{
			CAUTION_LOG(BM::LOG_LV1 , __FL2__(szFunc, iLine) << szMsg << _T(" is 0"));
		}
	}

	typedef struct tagCompareMinMaxLevel : public std::unary_function< PgQuestRandom, bool >
	{
		inline tagCompareMinMaxLevel(int const iMinLevel, int const iMaxLevel)
			: m_iMinLevel(iMinLevel), m_iMaxLevel(iMaxLevel)
		{
		}
		inline result_type operator ()(argument_type const& rhs) const
		{
			return (m_iMinLevel == rhs.LevelMin()) && (m_iMaxLevel == rhs.LevelMax());
		}
	private:
		int const m_iMinLevel;
		int const m_iMaxLevel;
	} CompareMinMaxLevel;

	void BuildRandomQuest(EQuestType const eType, ContQuestRandom& rkContRandom, CONT_DEF_QUEST_RANDOM const& rkContDefRandom, wchar_t const* const szFunc, size_t const iLine)
	{
		// build
		CONT_DEF_QUEST_RANDOM::const_iterator loop_iter = rkContDefRandom.begin();
		while( rkContDefRandom.end() != loop_iter )
		{
			CONT_DEF_QUEST_RANDOM::value_type const& rkElement = (*loop_iter);
			if( rkElement.iType == eType )
			{
				VerifyIsNotZero(rkElement.iMinLevel, _T("Random Quest [MinLevel]"), szFunc, iLine);
				VerifyIsNotZero(rkElement.iMaxLevel, _T("Random Quest [MaxLevel]"), szFunc, iLine);
				VerifyIsNotZero(rkElement.iStartQuestID, _T("Random Quest [StartQuestID]"), szFunc, iLine);
				VerifyIsNotZero(rkElement.iEndQuestID, _T("Random Quest [EndQuestID]"), szFunc, iLine);

				ContQuestRandom::iterator find_iter = std::find_if(rkContRandom.begin(), rkContRandom.end(), QuestMngUtil::CompareMinMaxLevel(rkElement.iMinLevel, rkElement.iMaxLevel));
				if( rkContRandom.end() == find_iter )
				{
					find_iter = rkContRandom.insert(rkContRandom.end(), ContQuestRandom::value_type(rkElement.iMinLevel, rkElement.iMaxLevel));
				}

				if( rkContRandom.end() != find_iter )
				{
					(*find_iter).Add(rkElement);
				}
			}
			++loop_iter;
		}
	}

	void VerifyRandomQuest(ContQuestRandom& rkRandomQuest, ContQuestID& rkContAll)
	{
		//verify and build
		ContQuestRandom::iterator iter = rkRandomQuest.begin();
		while( rkRandomQuest.end() != iter )
		{
			ContQuestRandom::value_type const& rkElement = (*iter);
			ContQuestID kVec;
			rkElement.Verify(kVec); // 에러 출력은 나중에 한꺼번에

			ContQuestID::const_iterator id_iter = kVec.begin();
			while( kVec.end() != id_iter )
			{
				if( rkContAll.end() == std::find(rkContAll.begin(), rkContAll.end(), (*id_iter)) )
				{
					rkContAll.push_back( (*id_iter) );
				}
				++id_iter;
			}
			++iter;
		}
	}
}

PgQuestMng::PgQuestMng()
{
}

PgQuestMng::~PgQuestMng()
{
}

bool PgQuestMng::GetQuestDef(int const iQuestID, TBL_DEF_QUEST_REWARD const* &pkOut)
{
	CONT_DEF_QUEST_REWARD const* pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}

	CONT_DEF_QUEST_REWARD::const_iterator find_iter = pkDef->find(iQuestID);
	if( pkDef->end() != find_iter )
	{
		pkOut = &((*find_iter).second);
	}
	return pkDef->end() != find_iter;
}

bool PgQuestMng::BuildRandom(CONT_DEF_QUEST_RANDOM const *pkDefRandom, CONT_DEF_QUEST_RANDOM_EXP const* pkDefRandomExp, CONT_DEF_QUEST_RANDOM_TACTICS_EXP const* pkDefTacticsRandomExp)
{
	if( !pkDefRandom || !pkDefRandomExp || !pkDefTacticsRandomExp )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("RANDOM Quest Table is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("RANDOM Quest Table is NULL"));
		return false;
	}

	m_kQuestRandom.clear();
	m_kQuestRandomAll.clear();
	m_kQuestTacticsRandom.clear();
	m_kQuestTacticsRandomAll.clear();
	m_kQuestWanted.clear();
	m_kQuestWantedAll.clear();

	QuestMngUtil::BuildRandomQuest(QT_Random, m_kQuestRandom, *pkDefRandom, __FUNCTIONW__, __LINE__);
	QuestMngUtil::BuildRandomQuest(QT_RandomTactics, m_kQuestTacticsRandom, *pkDefRandom, __FUNCTIONW__, __LINE__);
	QuestMngUtil::BuildRandomQuest(QT_Wanted, m_kQuestWanted, *pkDefRandom, __FUNCTIONW__, __LINE__);

	QuestMngUtil::VerifyRandomQuest(m_kQuestRandom, m_kQuestRandomAll);
	QuestMngUtil::VerifyRandomQuest(m_kQuestTacticsRandom, m_kQuestTacticsRandomAll);
	QuestMngUtil::VerifyRandomQuest(m_kQuestWanted, m_kQuestWantedAll);

	//if( !pkDefRandomExp->empty() )
	{
		CONT_DEFCLASS const* pkClassDef = NULL;
		g_kTblDataMgr.GetContDef(pkClassDef);
		if( !pkClassDef )
		{
			return false;
		}

		int iMaxPlayerLevel = 0;
		CONT_DEFCLASS::const_iterator class_iter = pkClassDef->begin();
		while( pkClassDef->end() != class_iter )
		{
			if( iMaxPlayerLevel < (*class_iter).second.sLevel )
			{
				iMaxPlayerLevel = (*class_iter).second.sLevel;
			}
			++class_iter;
		}

		if( !iMaxPlayerLevel ) // TB_DefClass 상 최고 레벨
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(_T("[RANDOM QUEST] Can't find, Player can max level")) ));
		}
		//if( !iMinPlayerLevel ) // TB_DefQuest_RandomExp 상 최소 레벨
		//{
		//	PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(_T("[RANDOM QUEST] Can't find, Player can min level")) ));
		//}
		if( 0 < pkDefRandomExp->size() )
		{
			int const iMinPlayerLevel = pkDefRandomExp->begin()->first;
			for( int iCurLevel = iMinPlayerLevel; iMaxPlayerLevel >= iCurLevel; ++iCurLevel )
			{
				if( pkDefRandomExp->end() == pkDefRandomExp->find(iCurLevel) )
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"[TB_DefQuest_RandomExp] Level[Min:")<<iMinPlayerLevel<<L", Max:"<<iMaxPlayerLevel<<L"]Can't find, Player can level["<<iCurLevel<<L"]" ));
				}
			}
		}
		if( 0 < pkDefTacticsRandomExp->size() )
		{
			int const iMinPlayerLevel = pkDefTacticsRandomExp->begin()->first;
			for( int iCurLevel = iMinPlayerLevel; iMaxPlayerLevel >= iCurLevel; ++iCurLevel )
			{
				if( pkDefTacticsRandomExp->end() == pkDefTacticsRandomExp->find(iCurLevel) )
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"[TB_DefQuest_RandomTacticsExp] Level[Min:")<<iMinPlayerLevel<<L", Max:"<<iMaxPlayerLevel<<L"]Can't find, Player can level["<<iCurLevel<<L"]" ));
				}
			}
		}
	}
	{
		CONT_DEF_QUEST_REWARD const* pkDefQuest = NULL;
		g_kTblDataMgr.GetContDef(pkDefQuest);
		CONT_DEF_QUEST_REWARD::const_iterator iter = pkDefQuest->begin();
		while( pkDefQuest->end() != iter )
		{
			CONT_DEF_QUEST_REWARD::mapped_type const& rkQuestDef = (*iter).second;
			if(	QT_Random == rkQuestDef.iDBQuestType
			&&	m_kQuestRandomAll.end() == std::find(m_kQuestRandomAll.begin(), m_kQuestRandomAll.end(), rkQuestDef.iQuestID) )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong [TB_DefQuest_Random] Table, QuestID[")<<rkQuestDef.iQuestID<<L"] is TYPE[RANDOM] but can't find in the Table" ));
			}
			if( QT_RandomTactics == rkQuestDef.iDBQuestType
			&&	m_kQuestTacticsRandomAll.end() == std::find(m_kQuestTacticsRandomAll.begin(), m_kQuestTacticsRandomAll.end(), rkQuestDef.iQuestID) )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong [TB_DefQuest_Random] Table, QuestID[")<<rkQuestDef.iQuestID<<L"] is TYPE[TACTICS] but can't find in the Table" ));
			}
			if( QT_Wanted == rkQuestDef.iDBQuestType
				&& m_kQuestWantedAll.end() == std::find(m_kQuestWantedAll.begin(), m_kQuestWantedAll.end(), rkQuestDef.iQuestID) )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong [TB_DefQuest_Wanted] Table, QuestID[")<<rkQuestDef.iQuestID<<L"] is TYPE[WANTED] but can't find in the Table" ));
			}
			++iter;
		}
	}
	return true;
}

void PgQuestMng::GetQuestRandom(int const iLevel, ContQuestID& rkOut) const
{
	ContQuestRandom::const_iterator loop_iter = m_kQuestRandom.begin();
	while( m_kQuestRandom.end() != loop_iter )
	{
		(*loop_iter).ShuffleQuest(iLevel, rkOut, QT_Random);
		++loop_iter;
	}
}
void PgQuestMng::GetQuestTacticsRandom(int const iLevel, ContQuestID& rkOut) const
{
	ContQuestRandom::const_iterator loop_iter = m_kQuestTacticsRandom.begin();
	while( m_kQuestTacticsRandom.end() != loop_iter )
	{
		(*loop_iter).ShuffleQuest(iLevel, rkOut, QT_RandomTactics);
		++loop_iter;
	}
}

void PgQuestMng::GetQuestWanted( int const iLevel, ContQuestID& rkOut ) const
{
	ContQuestRandom::const_iterator loop_iter = m_kQuestWanted.begin();
	while( m_kQuestWanted.end() != loop_iter )
	{
		(*loop_iter).ShuffleQuest(iLevel, rkOut, QT_Wanted);
		++loop_iter;
	}
}