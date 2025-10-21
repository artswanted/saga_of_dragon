#include "StdAfx.h"
#include "constant.h"
#include "BM/Guid.h"
#include "TableDataManager.h"
#include "Global.h"

#include "PgQuestInfo.h"
#include "PgMyQuest.h"
#include "PgMyQuestUtil.h"
#include "PgPremiumMgr.h"
#include "PgPremium.h"

BM::TObjectPool<SUserQuestState> g_kUserQuestPool(20, 1000);

//
typedef enum eQuestSaveVer
{
	QSV_20080707 = 20080707,
	QSV_20091029 = 20091029,
	QSV_20091105 = 20091105,
	QSV_20100106 = 20100106,
	QSV_20100210 = 20100210,

	QSV_Latest = QSV_20100210,
} EQuestSaveVer;


//
PgMyQuest::PgMyQuest(void)
{
	Clear();
}

PgMyQuest::~PgMyQuest(void)
{
	Clear();
}

SUserQuestState* PgMyQuest::AddIngQuest(const SUserQuestState* pkInfo, int const iQuestType)
{
	if( pkInfo
	&&	pkInfo->iQuestID )
	{
		if( QS_Finished == pkInfo->byQuestState )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}

		bool const bIsEndedQuest = IsEndedQuest(pkInfo->iQuestID);
		if( bIsEndedQuest )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
			return NULL;
		}

		switch( iQuestType )
		{
		case QT_Scenario:
		case QT_Soul:
		case QT_BattlePass:
			{
				if( MAX_ING_SCENARIO_QUEST < m_kScenarioQuestID.size()+1 )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
					return NULL;
				}
			}break;
		case QT_MissionQuest:
			{	//DB 크기보다 작으면 갯수 제한 없다
				if( MAX_DB_INGQUEST_SIZE < ((m_kIngQuest.size()+1) * sizeof(SUserQuestState)) 
				||	MAX_TOTAL_ING_QUEST <= m_kIngQuest.size()  )//최대 갯수체크. 사이즈 문제가 있을 수도 있고.
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
					return NULL;
				}
			}break;
		case QT_Day:
		case QT_GuildTactics:
		case QT_Normal:
		case QT_Event_Normal:
		case QT_ClassChange:
		case QT_Loop:
		case QT_Event_Loop:
		case QT_Scroll:
		case QT_Normal_Day:
		case QT_Event_Normal_Day:
		case QT_Couple:
		case QT_SweetHeart:
		case QT_Random:
		case QT_RandomTactics:
		case QT_Wanted:		
		case QT_Week:
			{
				if( MAX_ING_QUESTNUM < (GetIngQuestNum()-m_kScenarioQuestID.size()+1) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
					return NULL;
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't Begin Quest[ID: ") << pkInfo->iQuestID << _T("] Type[") << iQuestType << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
				return NULL;
			}break;
		}

		SUserQuestState* pkIngQuest = _Get(pkInfo->iQuestID);
		if( !pkIngQuest )
		{
			SUserQuestState* pkNew = g_kUserQuestPool.New();

			::memcpy_s(pkNew, sizeof(SUserQuestState), pkInfo, sizeof(SUserQuestState));
			auto ret = m_kIngQuest.insert(std::make_pair(pkInfo->iQuestID, pkNew));
			if( !ret.second )
			{
				g_kUserQuestPool.Delete(pkNew);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
				return NULL;
			}

			switch( iQuestType )
			{
			case QT_Scenario:
			case QT_Soul:
				{
					m_kScenarioQuestID.insert(pkInfo->iQuestID);
				}break;
			case QT_MissionQuest:
				{
					//미션 퀘스트 ID 저장
					m_kInstanceQuestID.insert(pkInfo->iQuestID);
				}break;
			default:
				{
				}break;
			}

			return pkNew;
		}
		else
		{
			assert(false);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

bool PgMyQuest::UpdateQuest(SUserQuestState const &rkUpdated)
{
	
	SUserQuestState* pkIngQuest = _Get(rkUpdated.iQuestID);
	if( !pkIngQuest )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	::memcpy_s(pkIngQuest, sizeof(SUserQuestState), &rkUpdated, sizeof(SUserQuestState));
	return true;
}

void PgMyQuest::Clear()
{
	ClearIngQuest();

	::memset(m_byEndQuest, 0, sizeof(m_byEndQuest));

	m_kInstanceQuestID.clear();
	m_kScenarioQuestID.clear();
	m_kDayLoopQuestTime = BM::PgPackedTime::LocalTime();
	m_kContRandomQuest.clear();
	m_kContTacticsQuest.clear();
	m_kContWantedQuest.clear();
	m_kBuildedRandomQuest = m_kBuildedTacticsQuest= m_kBuildedWantedQuest = false;
	m_kBuildedRandomQuestPlayerLevel = m_kBuildedTacticsQuestPlayerLevel = m_kBuildedWantedQuestPlayerLevel = 1;
	m_kMigrationRevision = 0;
	m_kContWantedQuestClearCount.clear();
	m_kExistCanBuildWantedQuest = false;
}
void PgMyQuest::ClearIngQuest()
{
	ContUserQuest::iterator itor = m_kIngQuest.begin();
	while (m_kIngQuest.end() != itor)
	{
		g_kUserQuestPool.Delete(itor->second);
		itor++;
	}
	m_kIngQuest.clear();
}


void PgMyQuest::ReadFromBuff(PgIngBufferR const& rkIngQuest, PgEndBufferR const& rkEndQuest1, PgEndBufferR const& rkEndQuest2, PgEndExtBufferR const& rkEndQuestExt, PgEndExt2BufferR const& rkEndQuestExt2)
{
	
	Clear();

	if( !rkIngQuest.Empty() )//Ing Quest
	{
		BM::Stream kIngQuest;
		kIngQuest.Push(&rkIngQuest[0], rkIngQuest.Size());

		int iCurSaveVer = 0;
		kIngQuest.Pop( iCurSaveVer ); // Version Time

		if( QSV_20080707 <= iCurSaveVer )
		{
			//Scenario
			SUserQuestState kQuest;
			BYTE cIngScenarioQuestCount = 0;
			kIngQuest.Pop( cIngScenarioQuestCount ); // Scenario Count
			for( size_t iCur = 0; cIngScenarioQuestCount > iCur; ++iCur )
			{
				kQuest.ReadFromPacket(kIngQuest);
				AddIngQuest(&kQuest, QT_Scenario);
			}
		}

		//Normal
		SUserQuestState kQuest;
		BYTE cIngQuestCount = 0;
		kIngQuest.Pop( cIngQuestCount ); // Normal Count
		for(size_t iCur = 0; cIngQuestCount > iCur; ++iCur)
		{
			kQuest.ReadFromPacket(kIngQuest);
			AddIngQuest(&kQuest, QT_Normal);
		}

		kIngQuest.Pop( m_kDayLoopQuestTime );
		// 퀘스트 공간 늘인것을 적용하지 않으면 랜덤 퀘스트 사용 할 수 없다
		if( QSV_20091029 <= iCurSaveVer )
		{
			kIngQuest.Pop( m_kContRandomQuest );				// 192 + 4 + 0~40
			kIngQuest.Pop( m_kBuildedRandomQuest );				// 236 + 1
			kIngQuest.Pop( m_kBuildedRandomQuestPlayerLevel );	// 237 + 4
			// 241
		}
		if( QSV_20091105 <= iCurSaveVer )
		{
			kIngQuest.Pop( m_kMigrationRevision ); // 241 + 4
			// 245
		}
		if( QSV_20100106 <= iCurSaveVer )
		{
			kIngQuest.Pop( m_kContTacticsQuest );				// 245 + 4 + 0~40
			kIngQuest.Pop( m_kBuildedTacticsQuest );			// 289 + 1
			kIngQuest.Pop( m_kBuildedTacticsQuestPlayerLevel );	// 290 + 4
			// 294
		}
		if( QSV_20100210 <= iCurSaveVer )
		{
			kIngQuest.Pop( m_kContWantedQuest );				// 294 + 4 + 0~40( 현상수배 퀘스트 최대 10개 * 4 = 40 )
			kIngQuest.Pop( m_kBuildedWantedQuest );				// 338 + 1
			kIngQuest.Pop( m_kBuildedWantedQuestPlayerLevel );	// 339 + 4
			// 343
		}
	}

	//End Quest
	::memset(m_byEndQuest, 0, sizeof(m_byEndQuest));
	if( (rkEndQuest1.Size() + rkEndQuest2.Size() + rkEndQuestExt.Size() + rkEndQuestExt2.Size()) == sizeof(m_byEndQuest) )
	{
		if( !rkEndQuest1.Empty()
		&&	!rkEndQuest2.Empty()
		&&	!rkEndQuestExt.Empty()
		&&	!rkEndQuestExt2.Empty()
		&&	MAX_DB_ENDQUEST_SIZE == rkEndQuest1.Size()
		&&	MAX_DB_ENDQUEST_SIZE == rkEndQuest2.Size()
		&&	MAX_DB_ENDQUEST_EXT_SIZE == rkEndQuestExt.Size()
		&&	MAX_DB_ENDQUEST_EXT2_SIZE == rkEndQuestExt2.Size() )
		{
			::memcpy_s(m_byEndQuest, rkEndQuest1.Size(), rkEndQuest1, rkEndQuest1.Size());			
			::memcpy_s(&m_byEndQuest[rkEndQuest1.Size()], rkEndQuest2.Size(), rkEndQuest2, rkEndQuest2.Size());			
			::memcpy_s(&m_byEndQuest[rkEndQuest1.Size()+rkEndQuest2.Size()], rkEndQuestExt.Size(), rkEndQuestExt, rkEndQuestExt.Size());
			::memcpy_s(&m_byEndQuest[rkEndQuest1.Size()+rkEndQuest2.Size()+rkEndQuestExt.Size()], rkEndQuestExt2.Size(), rkEndQuestExt2, rkEndQuestExt2.Size());
		}
	}
}

void PgMyQuest::WriteIngToBuff(PgIngBuffer& rkIngQuest) const
{
	BM::Stream kIngQuest;

	//Instance가 아닌 퀘스트만 따로 뽑는다
	ContUserQuestStatePtr kSaveVec;
	std::for_each(m_kIngQuest.begin(), m_kIngQuest.end(), PgInstanceFilter(kSaveVec, m_kInstanceQuestID));

	//Save Data Version
	kIngQuest.Push( static_cast< int >(QSV_Latest) ); // Version Info, 4

	//Scenario
	kIngQuest.Push( static_cast< BYTE >(m_kScenarioQuestID.size()) ); // 4 + 1
	{
		ContUserQuestStatePtr::const_iterator quest_iter = kSaveVec.begin();
		while( kSaveVec.end() != quest_iter )
		{
			ContUserQuestStatePtr::value_type const pkQuest = (*quest_iter);
			if( m_kScenarioQuestID.end() != m_kScenarioQuestID.find(pkQuest->iQuestID) )
			{
				pkQuest->WriteToPacket(kIngQuest); // 시나리오 최대 3개, 5 + 0~42
			}
			++quest_iter;
		}
	}

	//Normal
	kIngQuest.Push( static_cast< BYTE >((kSaveVec.size()-m_kScenarioQuestID.size())) ); // 47 + 1
	{
		ContUserQuestStatePtr::const_iterator quest_iter = kSaveVec.begin();
		while( kSaveVec.end() != quest_iter )
		{
			ContUserQuestStatePtr::value_type const pkQuest = (*quest_iter);
			if( m_kScenarioQuestID.end() == m_kScenarioQuestID.find(pkQuest->iQuestID) )
			{
				pkQuest->WriteToPacket(kIngQuest); // 일반 퀘스트, 48 + 0~140 (48 + 0~98)
			}
			++quest_iter;
		}
	}
	kIngQuest.Push( m_kDayLoopQuestTime );					// 188 + 4 (146 + 4)
	// 구버젼 최대 160 byte ----- 여기까지 150, 신버젼 최대 300 byte ------ 여기까지 192
	kIngQuest.Push( m_kContRandomQuest );					// 192 + 4 + 0~40
	kIngQuest.Push( m_kBuildedRandomQuest );				// 236 + 1
	kIngQuest.Push( m_kBuildedRandomQuestPlayerLevel );		// 237 + 4
	kIngQuest.Push( m_kMigrationRevision );					// 241 + 4
	kIngQuest.Push( m_kContTacticsQuest );					// 245 + 4 + 0~40
	kIngQuest.Push( m_kBuildedTacticsQuest );				// 289 + 1
	kIngQuest.Push( m_kBuildedTacticsQuestPlayerLevel );	// 290 + 4	
	kIngQuest.Push( m_kContWantedQuest );					// 294 + 4 + 0~40
	kIngQuest.Push( m_kBuildedWantedQuest );				// 338 + 1
	kIngQuest.Push( m_kBuildedWantedQuestPlayerLevel );		// 339 + 4
	// 343


	::memcpy_s(&rkIngQuest[0], rkIngQuest.Size(), &kIngQuest.Data().at(0), kIngQuest.Data().size());
}

void PgMyQuest::WriteEndToBuff(PgEndBuffer& rkEndQuest1, PgEndBuffer& rkEndQuest2, PgEndExtBuffer& rkEndQuestExt, PgEndExt2Buffer& rkEndQuestExt2) const
{
	//Ended Quest
	if( (rkEndQuest1.Size() + rkEndQuest2.Size() + rkEndQuestExt.Size() + rkEndQuestExt2.Size()) == MAX_DB_ENDQUEST_ALL_SIZE
	&&	!rkEndQuest1.Empty()
	&&	!rkEndQuest2.Empty()
	&&	!rkEndQuestExt.Empty()
	&&	!rkEndQuestExt2.Empty() )
	{
		rkEndQuest1.Clear();
		rkEndQuest2.Clear();

		::memcpy_s(&rkEndQuest1[0], rkEndQuest1.Size(), m_byEndQuest, rkEndQuest1.Size());
		::memcpy_s(&rkEndQuest2[0], rkEndQuest2.Size(), &m_byEndQuest[rkEndQuest1.Size()], rkEndQuest2.Size());
		::memcpy_s(&rkEndQuestExt[0], rkEndQuestExt.Size(), &m_byEndQuest[rkEndQuest1.Size()+rkEndQuest2.Size()], rkEndQuestExt.Size());
		::memcpy_s(&rkEndQuestExt2[0], rkEndQuestExt2.Size(), &m_byEndQuest[rkEndQuest1.Size()+rkEndQuest2.Size()+rkEndQuestExt.Size()], rkEndQuestExt2.Size());
	}
}

void PgMyQuest::WriteToBuff(PgIngBuffer& rkIngQuest, PgEndBuffer& rkEndQuest1, PgEndBuffer& rkEndQuest2, PgEndExtBuffer& rkEndQuestExt, PgEndExt2Buffer& rkEndQuestExt2) const
{
	WriteIngToBuff(rkIngQuest);
	WriteEndToBuff(rkEndQuest1, rkEndQuest2, rkEndQuestExt, rkEndQuestExt2);
}

SUserQuestState const* PgMyQuest::Get(int const iQuestID) const
{
	ContUserQuest::const_iterator itor = m_kIngQuest.find(iQuestID);
	if(m_kIngQuest.end() != itor)
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

SUserQuestState* PgMyQuest::_Get(int const iQuestID)
{
	
	ContUserQuest::iterator itor = m_kIngQuest.find(iQuestID);
	if(m_kIngQuest.end() != itor)
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

SUserQuestState* PgMyQuest::BeginQuest(int const iQuestID, int const iQuestType)
{
	
	SUserQuestState kState(iQuestID, QS_Ing);
	return AddIngQuest(&kState, iQuestType);
}

//bool PgMyQuest::CompleteQuest(int const iQuestID, int const iQuestType, ContQuestID const* pkVec)
//{
//	ContUserQuest::iterator itor = m_kIngQuest.find(iQuestID);
//	if( m_kIngQuest.end() != itor )
//	{
//		SUserQuestState* pkState = itor->second;
//		m_kIngQuest.erase(itor);
//		g_kUserQuestPool.Delete(pkState);
//	}
//
//	// Pre
//	m_kScenarioQuestID.erase(iQuestID);
//	m_kInstanceQuestID.erase(iQuestID);
//
//	// Main
//	switch(iQuestType)
//	{
//	case QT_Scenario:
//	case QT_Normal:
//	case QT_ClassChange:
//	case QT_Loop:
//	case QT_GuildTactics:
//	case QT_Day:
//	case QT_Dummy:
//	case QT_Normal_Day:
//	case QT_Couple:
//	case QT_SweetHeart:
//	case QT_Random:
//	case QT_RandomTactics:
//	case QT_Soul:
//	case QT_Wanted:
//		{
//			AddEndQuest(iQuestID, __FUNCTIONW__);
//		}break;
//	case QT_MissionQuest:
//		{
//		}break;
//	case QT_None:
//	default:
//		{
//			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Can't Complete Quest[ID: ") << iQuestID << _T("] Type[") << iQuestType << _T("]") );
//			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
//		}break;
//	}
//
//	if(NULL != pkVec)
//	{//위에서 내 퀘스트를 클리어 했으니, 모든 그룹퀘스트가 완료이다.
//		DeEndQuest(*pkVec, __FUNCTIONW__);//전부 안한 상태로 다시 뒤집는다.
//	}
//
//	return true;
//}

bool PgMyQuest::DropQuest(int const iQuestID)
{
	

	ContUserQuest::iterator itor = m_kIngQuest.find(iQuestID);
	if (m_kIngQuest.end() == itor)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	SUserQuestState* pkState = itor->second;
	m_kIngQuest.erase(itor);
	g_kUserQuestPool.Delete(pkState);

	m_kInstanceQuestID.erase(iQuestID);
	m_kScenarioQuestID.erase(iQuestID);

	return true;
}

bool PgMyQuest::GetQuestList(ContUserQuestState &rkVec) const
{
	
	ContUserQuest::const_iterator kIter = m_kIngQuest.begin();
	for(;m_kIngQuest.end() != kIter; ++kIter)
	{
		rkVec.push_back(*kIter->second);
	}
	return !rkVec.empty();
}

void PgMyQuest::AddEndQuest(int const iQuestID, wchar_t const* szFunc)
{
	if( !MyEndQuestBitUtil::AddEndQuest(m_byEndQuest, iQuestID) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("From[") << szFunc << _T("] is can't set END QuestID[") << iQuestID << _T("]"));
	}
}

void PgMyQuest::DeEndQuest(int const iQuestID, wchar_t const* szFunc)
{
	if( !MyEndQuestBitUtil::AddDeEndQuest(m_byEndQuest, iQuestID) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("[") << szFunc << _T("] is can't set DeEND QuestID[") << iQuestID << _T("]"));
	}
}

bool PgMyQuest::IsEndedQuest(int const iQuestID) const
{
	
	int const iByteOffset = iQuestID / 8;
	int const iBitOffset = iQuestID % 8;
	BYTE const byValue = (BYTE)(0x01 << iBitOffset);
	if( 0 <= iByteOffset
	&&	MAX_DB_ENDQUEST_ALL_SIZE > iByteOffset )
	{
		return ((m_byEndQuest[iByteOffset] & byValue) == 0) ? false : true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMyQuest::IsEndedQuestVec(VEC_INT const& rkQuestID) const
{
	VEC_INT::const_iterator iter = rkQuestID.begin();
	while( rkQuestID.end() != iter )
	{
		if( true == IsEndedQuest((*iter)) )
		{
			return true;
		}
		++iter;
	}
	return false;
}

bool PgMyQuest::IsEndedQuestVec_All(VEC_INT const& rkQuestID) const
{
	VEC_INT::const_iterator iter = rkQuestID.begin();
	while( rkQuestID.end() != iter )
	{
		if( false == IsEndedQuest((*iter)) )
		{
			return false;
		}
		++iter;
	}
	return true;
}

bool PgMyQuest::IsIngQuest(int const iQuestID) const
{
	
	ContUserQuest::const_iterator kIter = m_kIngQuest.find(iQuestID);
	return (m_kIngQuest.end() != kIter);
}

bool PgMyQuest::IsIngQuestVec(VEC_INT const& rkQuestID) const
{
	VEC_INT::const_iterator iter = rkQuestID.begin();
	while( rkQuestID.end() != iter )
	{
		if( true == IsIngQuest((*iter)) )
		{
			return true;
		}
		++iter;
	}
	return false;
}

bool PgMyQuest::IsIngQuestVec_All(VEC_INT const& rkQuestID) const
{
	VEC_INT::const_iterator iter = rkQuestID.begin();
	while( rkQuestID.end() != iter )
	{
		if( false == IsIngQuest((*iter)) )
		{
			return false;
		}
		++iter;
	}
	return true;
}

void PgMyQuest::WriteToPacket(BM::Stream &rkPacket) const
{
	
	//Scenario
	rkPacket.Push( m_kScenarioQuestID.size() );
	{
		ContUserQuest::const_iterator iter = m_kIngQuest.begin();
		while(m_kIngQuest.end() != iter)
		{
			const ContUserQuest::mapped_type pkElement = (*iter).second;
			if( m_kScenarioQuestID.end() != m_kScenarioQuestID.find(pkElement->iQuestID) )
			{
				pkElement->WriteToPacket(rkPacket);
			}
			++iter;
		}
	}

	//Normal
	rkPacket.Push( (m_kIngQuest.size() - m_kScenarioQuestID.size() ) );
	{
		ContUserQuest::const_iterator iter = m_kIngQuest.begin();
		while(m_kIngQuest.end() != iter)
		{
			const ContUserQuest::mapped_type pkElement = (*iter).second;
			if( m_kScenarioQuestID.end() == m_kScenarioQuestID.find(pkElement->iQuestID) )
			{
				pkElement->WriteToPacket(rkPacket);
			}
			++iter;
		}
	}

	rkPacket.Push(m_byEndQuest);
	rkPacket.Push(m_kInstanceQuestID);
	rkPacket.Push(m_kScenarioQuestID);
	rkPacket.Push(m_kDayLoopQuestTime);
	rkPacket.Push(m_kContRandomQuest);
	rkPacket.Push(m_kBuildedRandomQuest);
	rkPacket.Push(m_kBuildedRandomQuestPlayerLevel);
	rkPacket.Push(m_kMigrationRevision);
	rkPacket.Push(m_kContTacticsQuest);
	rkPacket.Push(m_kBuildedTacticsQuest);
	rkPacket.Push(m_kBuildedTacticsQuestPlayerLevel);
	rkPacket.Push(m_kContWantedQuest);
	rkPacket.Push(m_kBuildedWantedQuest);
	rkPacket.Push(m_kBuildedWantedQuestPlayerLevel);
	rkPacket.Push(m_kContWantedQuestClearCount);
	rkPacket.Push(m_kExistCanBuildWantedQuest);
}

void PgMyQuest::ReadFromPacket(BM::Stream &rkPacket)
{
	
	Clear();
	size_t iCnt = 0;

	rkPacket.Pop(iCnt);
	for(size_t iCur = 0; iCnt > iCur; ++iCur)
	{
		SUserQuestState kNewElement;
		kNewElement.ReadFromPacket(rkPacket);

		AddIngQuest(&kNewElement, QT_Scenario);
	}

	rkPacket.Pop(iCnt);
	for(size_t iCur = 0; iCnt > iCur; ++iCur)
	{
		SUserQuestState kNewElement;
		kNewElement.ReadFromPacket(rkPacket);

		AddIngQuest(&kNewElement, QT_Normal);
	}
	rkPacket.Pop(m_byEndQuest);
	rkPacket.Pop(m_kInstanceQuestID);
	rkPacket.Pop(m_kScenarioQuestID);
	rkPacket.Pop(m_kDayLoopQuestTime);
	rkPacket.Pop(m_kContRandomQuest);
	rkPacket.Pop(m_kBuildedRandomQuest);
	rkPacket.Pop(m_kBuildedRandomQuestPlayerLevel);
	rkPacket.Pop(m_kMigrationRevision);
	rkPacket.Pop(m_kContTacticsQuest);
	rkPacket.Pop(m_kBuildedTacticsQuest);
	rkPacket.Pop(m_kBuildedTacticsQuestPlayerLevel);
	rkPacket.Pop(m_kContWantedQuest);
	rkPacket.Pop(m_kBuildedWantedQuest);
	rkPacket.Pop(m_kBuildedWantedQuestPlayerLevel);
	rkPacket.Pop(m_kContWantedQuestClearCount);
	rkPacket.Pop(m_kExistCanBuildWantedQuest);
}

void PgMyQuest::InitWantedQuestClearCount()
{
	m_kContWantedQuestClearCount.clear();
}

void PgMyQuest::AddWantedQuestClearCount(int const iQuestID)
{
	ContQuestClearCount::iterator find_itor = m_kContWantedQuestClearCount.find(iQuestID);
	if( m_kContWantedQuestClearCount.end() != find_itor )
	{
		++((*find_itor).second);
	}
	else
	{
		m_kContWantedQuestClearCount.insert( std::make_pair( iQuestID, 1 ) );
	}
}

int PgMyQuest::GetWantedQuestClearCount(int const iQuestID) const
{
	ContQuestClearCount::const_iterator find_itor = m_kContWantedQuestClearCount.find(iQuestID);
	if( m_kContWantedQuestClearCount.end() != find_itor )
	{
		return ((*find_itor).second);
	}

	return 0;
}



void PgMyQuest::RemoveInstanceQuest()
{
	
	ContQuestIDSet::const_iterator iter = m_kInstanceQuestID.begin();
	while(m_kInstanceQuestID.end() != iter)
	{
		ContUserQuest::iterator find_iter = m_kIngQuest.find((*iter));
		if(m_kIngQuest.end() != find_iter)
		{
			SUserQuestState* pkState = find_iter->second;
			if( pkState )
			{
				g_kUserQuestPool.Delete(pkState);
			}
			find_iter = m_kIngQuest.erase(find_iter);
		}
		++iter;
	}
	m_kInstanceQuestID.clear();
}

void PgMyQuest::AddEndQuest(ContQuestID const &rkVec, wchar_t const* szFunc, int const iIgnoreQuestID)
{
	
	ContQuestID::const_iterator iter = rkVec.begin();
	while( rkVec.end() != iter )
	{
		if( iIgnoreQuestID != (*iter) )
		{
			AddEndQuest(*iter, szFunc);
		}
		++iter;
	}
}
void PgMyQuest::DeEndQuest(ContQuestID const &rkVec, wchar_t const* szFunc)
{
	
	ContQuestID::const_iterator iter = rkVec.begin();
	while( rkVec.end() != iter )
	{
		DeEndQuest(*iter, szFunc);
		++iter;
	}
}
size_t PgMyQuest::GetFlagQusetCount(ContQuestID const &rkVec, bool const bClear)const
{
	
	size_t iCount = 0;
	ContQuestID::const_iterator iter = rkVec.begin();
	while( rkVec.end() != iter )
	{
		if( bClear == IsEndedQuest(*iter) )
		{
			++iCount;
		}
		++iter;
	}
	return iCount;
}

void PgMyQuest::Copy(PgMyQuest& rhs) const
{
	BM::Stream kPacket;
	WriteToPacket(kPacket);
	rhs.ReadFromPacket(kPacket);
}
void PgMyQuest::DayLoopQuestTime(BM::PgPackedTime const& rkLocalTime)
{
	m_kDayLoopQuestTime = rkLocalTime;
}
int PgMyQuest::GetRandomQuestCompleteCount(ContQuestID const& rkContQuest) const
{
	if( rkContQuest.empty() )
	{
		return MAX_QUEST_RANDOM_EXP_COUNT;
	}
	return static_cast< int >(GetFlagQusetCount(rkContQuest, true));
}
int PgMyQuest::RandomQuestCompleteCount() const
{
	return GetRandomQuestCompleteCount( m_kContRandomQuest );
}
int PgMyQuest::TacticsQuestCompleteCount() const
{
	return GetRandomQuestCompleteCount( m_kContTacticsQuest );
}
namespace MyQuestMigrationTool
{
	extern bool CheckMigrationVer(bool const bIsNewbiePlayer, ContUserQuest const& rkContUserQuest, int& iMigrationRevision, BYTE (&byEndQuest)[MAX_DB_ENDQUEST_ALL_SIZE], CONT_ERASE_QUEST_ID& rkContErase);
}

void PgMyQuest::CheckMigration(CONT_DEF_QUEST_REWARD const* pkDefQuestReward, bool const bIsNewbiePlayer)
{
	using namespace MyQuestMigrationTool;
	CONT_ERASE_QUEST_ID kContErase;
	if( !pkDefQuestReward )
	{
		return;
	}

	if( CheckMigrationVer(bIsNewbiePlayer, m_kIngQuest, m_kMigrationRevision, m_byEndQuest, kContErase) )
	{
		ContUserQuest::iterator iter = m_kIngQuest.begin();
		while( m_kIngQuest.end() != iter )
		{
			ContUserQuest::mapped_type& pkUserQuest = (*iter).second;
			if( pkUserQuest )
			{
				CONT_DEF_QUEST_REWARD::const_iterator reward_iter = pkDefQuestReward->find(pkUserQuest->iQuestID);
				if( pkDefQuestReward->end() == reward_iter )
				{
					kContErase.insert(pkUserQuest->iQuestID); // DefReward에 없는 퀘스트는 삭제
				}
			}
			++iter;
		}
	}

	CONT_ERASE_QUEST_ID::const_iterator erase_iter = kContErase.begin();
	while( kContErase.end() != erase_iter )
	{
		DropQuest((*erase_iter));
		++erase_iter;
	}
}