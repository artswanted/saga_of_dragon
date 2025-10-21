#include	"defines.h"
#include	"LogMgr.h"
#include	"CheckMgr.h"
#include	"QuestChecker.h"

void	CQuestChecker::CheckItem(DWORD const dwItemNo, TiXmlElement* pkNode)
{
	if( !g_kCheckMgr.bExistItem(dwItemNo) )
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(m_kBasic.iQuestID, LT_ERROR, L"", SErrorData(ET_ITEM, pkNode->Row(), pkNode->Column(), dwItemNo)));
		bIsSuccess = false;
	}
}

void	CQuestChecker::CheckMonster(DWORD const dwMonsterID, TiXmlElement* pkNode)
{
	if( !g_kCheckMgr.bExistMonster(dwMonsterID) )
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(m_kBasic.iQuestID, LT_ERROR, L"", SErrorData(ET_MONSTER, pkNode->Row(), pkNode->Column(), dwMonsterID)));
		bIsSuccess = false;
	}
}

void	CQuestChecker::CheckGround(DWORD const dwGroundID, TiXmlElement* pkNode)
{
	if( !g_kCheckMgr.bExistGround(dwGroundID) )
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(m_kBasic.iQuestID, LT_ERROR, L"", SErrorData(ET_GROUND, pkNode->Row(), pkNode->Column(), dwGroundID)));
		bIsSuccess = false;
	}
}

void	CQuestChecker::ParseError(TiXmlNode* pkNode)
{
	g_kLogMgr.PutMsg(SLOGMESSAGE(m_kBasic.iQuestID, LT_ERROR, L"", SErrorData(ET_LOGIC, pkNode->Row(), pkNode->Column())));
}