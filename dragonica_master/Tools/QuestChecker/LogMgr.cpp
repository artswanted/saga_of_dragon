#include "stdafx.h"
#include "LogMgr.h"

void	CLogMgr::AddLog(int QuestID, std::wstring const& wstrLog)
{
	BM::CAutoMutex kLock(m_kAddError);
	kLogContainer::iterator	iter = m_kLogCont.find( QuestID );
	if( iter == m_kLogCont.end() )
	{
		auto	Rst;
		SLogData	LogData;
		Rst = m_kLogCont.insert(std::make_pair(QuestID, LogData));
		if( Rst.second )
		{
			Rst.first->second.wstrLog = wstrLog;
		}
	}
	else
	{
		iter->second.wstrLog = wstrLog;
	}

	g_Core.AddListItem(wstrLog);
}

void	CLogMgr::AddError(int QuestID, SErrorData const& Data)
{
	kLogContainer::iterator	iter = m_kLogCont.find( QuestID );
	if( iter == m_kLogCont.end() )
	{
		auto	Rst;
		SLogData	LogData;
		Rst = m_kLogCont.insert(std::make_pair(QuestID, LogData));
		if( Rst.second )
		{
			Rst.first->second.Data.push_back(Data);
		}
	}
	else
	{
		iter->second.Data.push_back(Data);
	}
}

void	CLogMgr::OutError(int iID)
{
	kLogContainer::iterator	iter = m_kLogCont.find(iID);
	if( iter != m_kLogCont.end() )
	{
		SLogData::kDataContainer::iterator	Sub_iter = iter->second.Data.begin();
		for(; Sub_iter != iter->second.Data.end(); ++Sub_iter)
		{				
			const std::wstring szErrorMsg = Sub_iter->kErrorMsg;
			wchar_t	szTemp[MAX_PATH] = {0,};
			switch(Sub_iter->eType)
			{			
			case	PgQuestInfoVerifyUtil::ET_ParseError:
				{
					swprintf(szTemp, MAX_PATH, WSTR_LOGERRLISTITEM.c_str(), L"Xml Parse Error", Sub_iter->iQuestID, szErrorMsg.c_str());
				}break;
			case	PgQuestInfoVerifyUtil::ET_DataError:
				{
					swprintf(szTemp, MAX_PATH, WSTR_LOGERRLISTITEM.c_str(), L"Data Error", Sub_iter->iQuestID, szErrorMsg.c_str());
				}break;
			case	PgQuestInfoVerifyUtil::ET_LogicalError:
				{
					swprintf(szTemp, MAX_PATH, WSTR_LOGERRLISTITEM.c_str(), L"Logical Parse Error", Sub_iter->iQuestID, szErrorMsg.c_str());
				}break;			
			}
			g_Core.AddListItem(szTemp, OT_FAIL);
		}
	}
}