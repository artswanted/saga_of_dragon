#include "stdafx.h"
#include "lohengrin/DBtables.h"
#include "lohengrin/DBtables2.h"
#include "PgMyQuest.h"
#include "PgQuestInfo.h"
#include "PgQuestResetSchedule.h"
#include "PgBaseQuestMng.h"

PgBaseQuestMng::PgBaseQuestMng()
{
}

PgBaseQuestMng::~PgBaseQuestMng()
{
}

void PgBaseQuestMng::Clear()
{
	m_kQuestGroup.clear();
	m_kDayQuestAll.clear();
	m_kWeekQuestAll.clear();
	m_kDeletedQuestAll.clear();
	m_kCoupleQuestAll.clear();
	m_kBattlePassQuestAll.clear();
}

bool PgBaseQuestMng::Build(CONT_DEF_QUEST_REWARD const* pkDefCont, CONT_DEF_QUEST_RESET_SCHEDULE const* pkDefResetSchedule)
{
	if( !pkDefCont )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Critical Error [CONT_DEF_QUEST_REWARD] is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDefCont is NULL"));
		return false;
	}

	if( !pkDefResetSchedule )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Critical Error [CONT_DEF_QUEST_RESET_SCHEDULE] is NULL");
		return false;
	}

	Clear();

	{
		CONT_DEF_QUEST_REWARD::const_iterator loop_iter = pkDefCont->begin();
		while( pkDefCont->end() != loop_iter )
		{
			CONT_DEF_QUEST_REWARD::mapped_type const &rkDef = (*loop_iter).second;

			if( rkDef.iGroupNo )
			{
				auto kRet = m_kQuestGroup.insert( std::make_pair(rkDef.iGroupNo, ContQuestGroup::mapped_type()) );
				ContQuestGroup::iterator iter = kRet.first;

				if( m_kQuestGroup.end() != iter )
				{
					ContQuestGroup::mapped_type &rkVec = (*iter).second;

					ContQuestGroup::mapped_type::const_iterator find_iter = std::find(rkVec.begin(), rkVec.end(), rkDef.iQuestID);
					if( rkVec.end() == find_iter )
					{
						std::back_inserter(rkVec) = rkDef.iQuestID;
					}
				}
			}

			if( QT_Day == rkDef.iDBQuestType
			||	QT_GuildTactics == rkDef.iDBQuestType
			||	QT_Normal_Day == rkDef.iDBQuestType
			||	QT_Event_Normal_Day == rkDef.iDBQuestType )
			{
				if( m_kDayQuestAll.end() == std::find(m_kDayQuestAll.begin(), m_kDayQuestAll.end(), rkDef.iQuestID) )
				{
					std::back_inserter(m_kDayQuestAll) = rkDef.iQuestID;
				}
			}
			if( QT_Week == rkDef.iDBQuestType )
			{
				if( m_kWeekQuestAll.end() == std::find(m_kWeekQuestAll.begin(), m_kWeekQuestAll.end(), rkDef.iQuestID) )
				{
					std::back_inserter(m_kWeekQuestAll) = rkDef.iQuestID;
				}
			}
			if( QT_BattlePass == rkDef.iDBQuestType )
			{
				if( m_kBattlePassQuestAll.end() == std::find(m_kBattlePassQuestAll.begin(), m_kBattlePassQuestAll.end(), rkDef.iQuestID) )
				{
					std::back_inserter(m_kBattlePassQuestAll) = rkDef.iQuestID;
				}
			}
			if( QT_Deleted == rkDef.iDBQuestType )
			{
				if( m_kDeletedQuestAll.end() == std::find(m_kDeletedQuestAll.begin(), m_kDeletedQuestAll.end(), rkDef.iQuestID) )
				{
					std::back_inserter(m_kDeletedQuestAll) = rkDef.iQuestID;
				}
			}
			if( (QT_Couple == rkDef.iDBQuestType) || (QT_SweetHeart == rkDef.iDBQuestType) )
			{
				if( m_kCoupleQuestAll.end() == std::find(m_kCoupleQuestAll.begin(), m_kCoupleQuestAll.end(), rkDef.iQuestID) )
				{
					std::back_inserter(m_kCoupleQuestAll) = rkDef.iQuestID;
				}
			}

			++loop_iter;
		}
	}

	{
		bool bRet = true;
		CONT_DEF_QUEST_RESET_SCHEDULE::const_iterator loop_iter = pkDefResetSchedule->begin();
		while( pkDefResetSchedule->end() != loop_iter )
		{
			CONT_DEF_QUEST_RESET_SCHEDULE::value_type const& rkDefSchedule = (*loop_iter);

			CONT_DEF_QUEST_REWARD::const_iterator find_iter = pkDefCont->find( rkDefSchedule.iQuestID );
			if( pkDefCont->end() == find_iter )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L" Schedule Quest[" << rkDefSchedule.iQuestID << L"] is can't find in [TB_DefQuest_Reward]");
				bRet = false;
			}
			else
			{
				int const iDBQuestType = (*find_iter).second.iDBQuestType;
				switch( iDBQuestType )
				{
				case QT_Normal:
				case QT_Event_Normal:
				case QT_Dummy:
				case QT_Couple:
				case QT_SweetHeart:
					{
					}break;
				default:
					{
						CAUTION_LOG(BM::LOG_LV1, __FL__ << L" is can't apply Scheduled quest type, Quest[ID: " << rkDefSchedule.iQuestID << L", Type: " << iDBQuestType << L"]");
						bRet = false;
					}break;
				}
			}

			ContQuestResetSchedule::value_type kNewElement(rkDefSchedule);

			if( 0 == kNewElement.DateToDay() )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L" wrong reset date, Schedule Quest[" << rkDefSchedule.iQuestID << L"] wrong schdule date" << kNewElement.ResetBaseDate().operator DBTIMESTAMP() );
				bRet = false;
			}

			if( 0 == kNewElement.ResetLoopDay() )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L" 0 reset loop day, Schedule Quest[" << rkDefSchedule.iQuestID << L"] current[" << kNewElement.ResetLoopDay() << L"], can't set 0");
				bRet = false;
			}

			m_kResetSchedule.push_back(kNewElement);
			
			++loop_iter;
		}

		m_kResetSchedule.sort();

		if( !bRet )
		{
			return false;
		}
	}
	return true;
}

ContQuestID const* PgBaseQuestMng::GetGroup(int const iGroupNo)const
{
	ContQuestGroup::const_iterator find_iter = m_kQuestGroup.find(iGroupNo);
	if( m_kQuestGroup.end() != find_iter )
	{
		return &(*find_iter).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}
bool PgBaseQuestMng::IsHaveResetSchedule(int const iQuestID) const
{
	ContQuestResetSchedule::const_iterator iter = m_kResetSchedule.begin();
	while( m_kResetSchedule.end() != iter )
	{
		ContQuestResetSchedule::value_type const& rkElement = (*iter);
		if( iQuestID == rkElement.QuestID() )
		{
			return true;
		}
		++iter;
	}
	return false;
}