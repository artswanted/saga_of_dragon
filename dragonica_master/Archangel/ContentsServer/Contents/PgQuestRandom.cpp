#include "stdafx.h"
#include "Variant/PgQuestResetSchedule.h"
#include "Variant/PgBaseQuestMng.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuestRandom.h"
#include "PgQuestMng.h"

//
namespace QuestRandomUtil
{
	typedef struct tagShuffleFunc : std::unary_function< size_t, size_t >
	{
		result_type operator ()(argument_type const iIndex) const
		{
			return BM::Rand_Index( static_cast< int >(iIndex) );
		}
	} ShuffleFunc;

	typedef struct tagCompareGroupNo : std::unary_function< PgQuestRandomElement, bool >
	{
		inline tagCompareGroupNo(int const iGroupNo)
			: m_iGroupNo(iGroupNo)
		{
		}
		inline tagCompareGroupNo(tagCompareGroupNo const& rhs)
			: m_iGroupNo(rhs.m_iGroupNo)
		{
		}
		result_type operator ()(argument_type const& rhs) const
		{
			return m_iGroupNo == rhs.GroupNo();
		}
	private:
		int const m_iGroupNo;
	} CompareGroupNo;

	template< typename _T >
	static _T ToCount(_T const Value)
	{
		return Value + 1;
	}

	typedef struct tagGetShuffle : std::unary_function< PgQuestRandomElement, void >
	{
		inline tagGetShuffle(ContQuestID& rkOut, EQuestType eQuestType )
			: m_rkOut(rkOut), m_eQuestType( eQuestType )
		{
		}
		inline tagGetShuffle(tagGetShuffle const& rhs)
			: m_rkOut(rhs.m_rkOut), m_eQuestType(rhs.m_eQuestType)
		{
		}
		result_type operator ()(argument_type const& rhs) const
		{
			rhs.Shuffle(m_rkOut, m_eQuestType);
		}
	private:
		ContQuestID& m_rkOut;
		EQuestType m_eQuestType;
	} GetShuffle;
};


//
PgQuestRandomElement::PgQuestRandomElement()
	: m_kQuestID(), m_iDisplayCount(0), m_iGroupNo(0)
{
}
PgQuestRandomElement::PgQuestRandomElement(int const iGroupNo)
	: m_kQuestID(), m_iDisplayCount(0), m_iGroupNo(iGroupNo)
{
}
PgQuestRandomElement::PgQuestRandomElement(PgQuestRandomElement const& rhs)
	: m_kQuestID(rhs.m_kQuestID), m_iDisplayCount(rhs.m_iDisplayCount), m_iGroupNo(rhs.m_iGroupNo)
{
}
PgQuestRandomElement::~PgQuestRandomElement()
{
}
bool PgQuestRandomElement::Add(int iStartID, int iEndID, int const iDisplayCount)
{
	if( iStartID > iEndID )
	{
		std::swap(iStartID, iEndID);
	}

	size_t const iCount = static_cast< size_t >(QuestRandomUtil::ToCount(iEndID - iStartID));

	m_kQuestID.reserve(m_kQuestID.size() + iCount);
	for( int iCurQuestID = iStartID; iEndID >= iCurQuestID; ++iCurQuestID )
	{
		if( m_kQuestID.end() == std::find(m_kQuestID.begin(), m_kQuestID.end(), iCurQuestID) )
		{
			std::back_inserter(m_kQuestID) = iCurQuestID;
		}
		else
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, BM::vstring(L"") << L"GroupNo["<<m_iGroupNo<<L"] StartID["<<iStartID<<L"] EndID["<<iEndID<<L"] DisplayCount["<<iDisplayCount<<L"] in QuestID["<<iCurQuestID<<L"] is Duplicate ID" ));
		}
	}

	m_iDisplayCount += ((0 > iDisplayCount)? iCount: iDisplayCount);
	return true;
}
void PgQuestRandomElement::Shuffle(ContQuestID &rkOut, EQuestType eQuestType) const
{
	switch( eQuestType )
	{
	case QT_Random:
	case QT_RandomTactics:
	case QT_Wanted://현상수배 퀘스트도 최대 10개
		{
			if( MAX_QUEST_RANDOM_EXP_COUNT <= rkOut.size() )
			{
				return;
			}
		}break;
	default:
		{
		}break;
	}
	
	if( DisplayCount() >= m_kQuestID.size() )
	{
		rkOut.reserve(rkOut.size() + m_kQuestID.size());
		rkOut.insert(rkOut.end(), m_kQuestID.begin(), m_kQuestID.end()); // 몽창
	}
	else
	{
		ContQuestID kTemp(m_kQuestID); // copy
		rkOut.reserve(rkOut.size() + DisplayCount());
		std::random_shuffle(kTemp.begin(), kTemp.end(), QuestRandomUtil::ShuffleFunc()); // shuffle		
		rkOut.insert(rkOut.end(), kTemp.begin(), kTemp.begin()+DisplayCount()); // 섞은 대로 앞에서 몇 개만
	}

	switch( eQuestType )
	{
	case QT_Random:
	case QT_RandomTactics:
	case QT_Wanted:
		{
			while( MAX_QUEST_RANDOM_EXP_COUNT < rkOut.size() )
			{
				rkOut.pop_back(); // 10개 될때 까지
			}
		}break;	
	default:
		{
		}break;
	}	
}


//
PgQuestRandom::PgQuestRandom(int const iLevelMin, int const iLevelMax)
: m_iLevelMin(iLevelMin), m_iLevelMax(iLevelMax), m_kQuestRnd()
{
}
PgQuestRandom::PgQuestRandom(PgQuestRandom const& rhs)
: m_iLevelMin(rhs.m_iLevelMin), m_iLevelMax(rhs.m_iLevelMax), m_kQuestRnd(rhs.m_kQuestRnd)
{
}
PgQuestRandom::~PgQuestRandom()
{
}
void PgQuestRandom::ShuffleQuest(int const iLevel, ContQuestID& rkOut, EQuestType const eQuestType) const
{
	if( Check(iLevel) )
	{
		std::for_each(m_kQuestRnd.begin(), m_kQuestRnd.end(), QuestRandomUtil::GetShuffle(rkOut,eQuestType));
	}
}
bool PgQuestRandom::Add(TBL_DEF_QUEST_RANDOM const& rkElement)
{
	ContQuestRndElem::iterator find_iter = std::find_if(m_kQuestRnd.begin(), m_kQuestRnd.end(), QuestRandomUtil::CompareGroupNo(rkElement.iGroupNo));
	if( m_kQuestRnd.end() == find_iter )
	{
		find_iter = m_kQuestRnd.insert( m_kQuestRnd.end(), ContQuestRndElem::value_type(rkElement.iGroupNo) );
	}

	if( m_kQuestRnd.end() != find_iter )
	{
		return (*find_iter).Add(rkElement.iStartQuestID, rkElement.iEndQuestID, rkElement.iDisplayCount);
	}
	return false;
}
BM::vstring PgQuestRandom::Key() const
{
	return BM::vstring(_T("Random Quest[MinLv:")) << m_iLevelMin << _T(" ,MaxLv:") << m_iLevelMax << _T("] ");
}
bool PgQuestRandom::Verify(ContQuestID& rkAllQuestID) const
{
	CONT_DEF_QUEST_REWARD const* pkDefQuest = NULL;
	g_kTblDataMgr.GetContDef(pkDefQuest);

	bool bVerify = true;
	ContQuestRndElem::const_iterator iter = m_kQuestRnd.begin();
	while( m_kQuestRnd.end() != iter )
	{
		ContQuestRndElem::value_type const& rkElement = (*iter);
		if( rkElement.DisplayCount() > rkElement.Size() )
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, Key() << _T("Display Count: ") << rkElement.DisplayCount() << _T(" is can't Larger than Quest Count: ") << rkElement.Size() ));
			bVerify = false;
		}
		if( rkElement.Empty() )
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, Key() << _T("Random Element is Empty") ));
			bVerify = false;
		}

		ContQuestID const& rkQuestID = rkElement.QuestID();
		ContQuestID::const_iterator id_iter = rkQuestID.begin();
		while( rkQuestID.end() != id_iter )
		{
			int const iQuestID = (*id_iter);
			CONT_DEF_QUEST_REWARD::const_iterator find_iter = pkDefQuest->find(iQuestID);
			if( pkDefQuest->end() == find_iter )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, Key() << _T(" QuestID["<<iQuestID<<L"] Can't find from [TB_DefQuestReward]") ));
			}
			else
			{
				int const iQuestType = (*find_iter).second.iDBQuestType;
				switch( iQuestType )
				{
				case QT_Random:
				case QT_RandomTactics:
				case QT_Wanted:
					{
						if( g_kQuestMng.IsHaveResetSchedule(iQuestID) )
						{
							PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, Key() << _T(" QuestID["<<iQuestID<<L"] is Can't use with the [TB_DefQuest_ResetSchedule]") ));
						}
					}break;
				default:
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( PgQuestInfoVerifyUtil::ET_DataError, std::string(), 0, __FUNCTIONW__, __LINE__, Key() << _T(" QuestID["<<iQuestID<<L"] is Wrong QuestType["<<iQuestType<<L"]") ));
					}break;
				}
			}
			++id_iter;
		}

		//if( bVerify )
		//{
			rkAllQuestID.insert(rkAllQuestID.end(), rkElement.QuestID().begin(), rkElement.QuestID().end());
		//}

		++iter;
	}
	return bVerify;
}