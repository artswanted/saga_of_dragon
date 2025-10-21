#include "stdafx.h"

#include "BM/Guid.h"
#include "BM/vstring.h"
#include "Lohengrin/Loggroup.h"
#include "Variant/PgStringUtil.h"
#include "TableDataManager.h"
#include "Variant/PgControlDefMgr.h"
#include "PgMissionInfo.h"
#include "PgMyQuest.h"

tagMissionBasicInfo::tagMissionBasicInfo() : iMissionID(0), iGroupNo(0), iTimeMin(0), iTimeSec(0), iScript(0), iEventMission(0)
{
	memset(iItemTimeMin, 0, sizeof(iItemTimeMin));
	memset(iItemTimeSec, 0, sizeof(iItemTimeSec));
}

//tagMissionBasicInfo(int const iID, int const iGroup, int const iMin, int const iSec, int const iSt, int const iItemMin, int const iItemSec, int const iTTW)
tagMissionBasicInfo::tagMissionBasicInfo(int const iID, int const iGroup, int const iMin, int const iSec, int const iSt, int const iTTW, bool EventMission)
:iMissionID(iID), iGroupNo(iGroup), iTimeMin(iMin), iTimeSec(iSec), iScript(iSt), iErrorTTW(iTTW), iEventMission(EventMission)
{
}

void tagMissionBasicInfo::Clear()
{
	iMissionID = 0;
	iGroupNo = 0;
	iTimeMin = 0;
	iTimeSec = 0;
	iScript = 0;
	iErrorTTW = 0;
	iEventMission = 0;

	for(int i=0; i<MISSION_ITEM_TIME; i++)
	{
		iItemTimeMin[i] = 0;
		iItemTimeSec[i] = 0;
	}
}

void tagMissionBasicInfo::SetTime(VEC_WSTRING kVec)
{
	if( kVec.size() < 2 )
	{
		iTimeMin = 0;
		iTimeSec = 0;
	}
	else
	{
		iTimeMin = PgStringUtil::SafeAtoi(kVec[0]);
		iTimeSec = PgStringUtil::SafeAtoi(kVec[1]);
	}
}
void tagMissionBasicInfo::SetItemTime(VEC_WSTRING kVec, int iLevel)
{
	int iMin;
	int iSec;

	if( kVec.size() < 2 )
	{
		iMin = 0;
		iSec = 0;
	}
	else
	{
		iMin = PgStringUtil::SafeAtoi(kVec[0]);
		iSec = PgStringUtil::SafeAtoi(kVec[1]);
	}

	iItemTimeMin[iLevel] = iMin;
	iItemTimeSec[iLevel] = iSec;		
}


//
tagMissionOptionChapter::tagMissionOptionChapter()
{
	iKind = 0;
	iCount = 0;
}

tagMissionOptionChapter::tagMissionOptionChapter(int const ObjectTextNo)
{
	iObjectTextNo = ObjectTextNo;
	m_kLimit_LevelValue.clear();
	m_kLimit_KindValue.clear();
}

void tagMissionOptionChapter::GetEffectNo(VEC_INT &rkEffect) const
{
	VEC_INT::const_iterator kIter = m_kLimit_EffectValue.begin();
	for(;m_kLimit_EffectValue.end() != kIter; ++kIter)
	{
		rkEffect.push_back(*kIter);
	}
}

void tagMissionOptionChapter::GetKindNo(VEC_INT &rkValue) const
{
	VEC_INT::const_iterator kIter = m_kLimit_KindValue.begin();
	for(;m_kLimit_KindValue.end() != kIter; ++kIter)
	{
		rkValue.push_back(*kIter);
	}
}

bool tagMissionOptionChapter::Build(std::wstring const &rkType, int const &Count, std::wstring const &rkLevel, std::wstring const &rkEffect, std::wstring const &rkValue)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkLevel, kVec, _T("/"));

	int iCnt = 0;
	size_t iSize = kVec.size();
	while( iSize-- )
	{
		if( PgStringUtil::SafeAtoi(kVec[iCnt]) == 0 ) continue;
		m_kLimit_LevelValue.push_back(PgStringUtil::SafeAtoi(kVec[iCnt++]));
	}

	VEC_WSTRING kVecEffect;
	PgStringUtil::BreakSep(rkEffect, kVecEffect, _T("/"));
	iCnt = 0;
	iSize = kVecEffect.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecEffect[iCnt]) == 0 ) continue;
		m_kLimit_EffectValue.push_back(PgStringUtil::SafeAtoi(kVecEffect[iCnt++]));
	}
	
	VEC_WSTRING kVecValue;
	PgStringUtil::BreakSep(rkValue, kVecValue, _T("/"));
	iCnt = 0;
	iSize = kVecValue.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecValue[iCnt]) == 0 ) continue;
		m_kLimit_KindValue.push_back(PgStringUtil::SafeAtoi(kVecValue[iCnt++]));
	}

	if( _T("MC_NONE") == rkType )
	{
		iType = MC_NONE;
	}
	else if( _T("MC_MON_ALL_KILL") == rkType )
	{
		iType = MC_MON_ALL_KILL;
	}
	else if( _T("MC_MON_TARGET_KILL") == rkType )
	{
		iType = MC_MON_TARGET_KILL;
	}
	else if( _T("MC_ITEM_GET") == rkType )
	{
		iType = MC_ITEM_GET;
	}
	else if( _T("MC_OBJECT_KICK") == rkType )
	{
		iType = MC_OBJECT_KICK;
	}
	else if( _T("MC_TIME_ATTACK") == rkType )
	{
		iType = MC_TIME_ATTACK;
	}
	else if( _T("MC_EFFECT") == rkType )
	{
		iType = MC_EFFECT;
	}
	else if( _T("MC_OBJECT_TARGET_KILL") == rkType )
	{
		iType = MC_OBJECT_TARGET_KILL;
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" wrong MissionChapter event type[") << rkType.c_str() << _T("]") );
	}

	iCount = Count;
	return true;
}



//
tagMissionOptionMission::tagMissionOptionMission()
{
	iCount= 0;
}

tagMissionOptionMission::tagMissionOptionMission(int const ObjectTextNo)
{
	iObjectTextNo = ObjectTextNo;
	m_kLimit_LevelValue.clear();
	m_kLimit_KindValue.clear();
}

void tagMissionOptionMission::GetKindNo(VEC_INT &rkValue) const
{
	VEC_INT::const_iterator kIter = m_kLimit_KindValue.begin();
	for(;m_kLimit_KindValue.end() != kIter; ++kIter)
	{
		rkValue.push_back(*kIter);
	}
}

bool tagMissionOptionMission::Build(std::wstring const &rkType, int const &Count, std::wstring const &rkLevel, std::wstring const &rkValue)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkLevel, kVec, _T("/"));

	int iCnt = 0;
	size_t iSize = kVec.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVec[iCnt]) == 0 ) continue;
		m_kLimit_LevelValue.push_back(PgStringUtil::SafeAtoi(kVec[iCnt++]));
	}

	VEC_WSTRING kVecValue;
	PgStringUtil::BreakSep(rkValue, kVecValue, _T("/"));
	iCnt = 0;
	iSize = kVecValue.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecValue[iCnt]) == 0 ) continue;
		m_kLimit_KindValue.push_back(PgStringUtil::SafeAtoi(kVecValue[iCnt++]));
	}

	if( _T("MM_NONE") == rkType )
	{
		iType = MM_NONE;
	}
	else if( _T("MM_MON_TARGET_KILL") == rkType )
	{
		iType = MM_MON_TARGET_KILL;
	}
	else if( _T("MM_OBJECT_KICK") == rkType )
	{
		iType = MM_OBJECT_KICK;
	}
	else if( _T("MM_TIME_ATTACK") == rkType )
	{
		iType = MM_TIME_ATTACK;
	}
	else if( _T("MM_MON_BOSS_KILL") == rkType )
	{
		iType = MM_MON_BOSS_KILL;
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" wrong Mission event type[") << rkType.c_str() << _T("]") );
	}

	iCount = Count;
	return true;
}


//
tagMissionOptionMissionOpen::tagMissionOptionMissionOpen()
{
	iLevel=0;
}

tagMissionOptionMissionOpen::tagMissionOptionMissionOpen(int const Level,int const NeedItem, std::wstring const &rkLevel, std::wstring const &rkIngQuest, std::wstring const &rkClearQuest, std::wstring const &rkEffect,std::wstring const &rkNeedItem) : iNeedItem(NeedItem)
{
	iLevel = Level;
	m_kLimit_IngQuestValue.clear();	
	m_kLimit_ClearQuestValue.clear();
	m_kLimit_PreLevelValue.clear();
	m_kLimit_EffectValue.clear();
	m_kLimit_NeedItem.clear();

	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkLevel, kVec, _T("/"));

	int iCnt = 0;
	size_t iSize = kVec.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVec[iCnt]) == 0 ) continue;
		m_kLimit_PreLevelValue.push_back(PgStringUtil::SafeAtoi(kVec[iCnt++]));
	}

	VEC_WSTRING kVecEffect;
	PgStringUtil::BreakSep(rkEffect, kVecEffect, _T("/"));
	iCnt = 0;
	iSize = kVecEffect.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecEffect[iCnt]) == 0 ) continue;
		m_kLimit_EffectValue.push_back(PgStringUtil::SafeAtoi(kVecEffect[iCnt++]));
	}

	VEC_WSTRING kVecIngQuest;
	PgStringUtil::BreakSep(rkIngQuest, kVecIngQuest, _T("/"));
	iCnt = 0;
	iSize = kVecIngQuest.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecIngQuest[iCnt]) == 0 ) continue;
		m_kLimit_IngQuestValue.push_back(PgStringUtil::SafeAtoi(kVecIngQuest[iCnt++]));
	}

	VEC_WSTRING kVecClearQuest;
	PgStringUtil::BreakSep(rkClearQuest, kVecClearQuest, _T("/"));
	iCnt = 0;
	iSize = kVecClearQuest.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecClearQuest[iCnt]) == 0 ) continue;
		m_kLimit_ClearQuestValue.push_back(PgStringUtil::SafeAtoi(kVecClearQuest[iCnt++]));
	}
	
	
	VEC_WSTRING kVecNeedItem;
	PgStringUtil::BreakSep(rkNeedItem, kVecNeedItem, _T("/"));
	iCnt = 0;
	iSize = kVecNeedItem.size();
	while( iSize-- > 0)
	{
		if( PgStringUtil::SafeAtoi(kVecNeedItem[iCnt]) == 0 ) continue;
		m_kLimit_NeedItem.push_back(PgStringUtil::SafeAtoi(kVecNeedItem[iCnt++]));
	}

}

bool tagMissionOptionMissionOpen::PreLevelCheck(int const iPreLevel) const
{
	VEC_INT::const_iterator iter = m_kLimit_PreLevelValue.begin();
	while( m_kLimit_PreLevelValue.end() != iter )
	{
		if( (*iter) == (iPreLevel+1) )
		{
			return true;
		}
		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool tagMissionOptionMissionOpen::PreLevelCheck(bool bClearLevel[]) const
{
	VEC_INT::const_iterator iter = m_kLimit_PreLevelValue.begin();
	while( m_kLimit_PreLevelValue.end() != iter )
	{
		if( bClearLevel[(*iter)-1] )
		{
			++iter;
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}

void tagMissionOptionMissionOpen::GetIngQuestIDVec(VEC_INT &rkIngQuest) const
{	
	VEC_INT::const_iterator kIter = m_kLimit_IngQuestValue.begin();
	for(;m_kLimit_IngQuestValue.end() != kIter; ++kIter)
	{
		rkIngQuest.push_back(*kIter);
	}
}

void tagMissionOptionMissionOpen::GetClearQuestIDVec(VEC_INT &rkClearQuest) const
{	
	VEC_INT::const_iterator kIter = m_kLimit_ClearQuestValue.begin();
	for(;m_kLimit_ClearQuestValue.end() != kIter; ++kIter)
	{
		rkClearQuest.push_back(*kIter);
	}
}

void tagMissionOptionMissionOpen::GetNeedItem(VEC_INT &rkNeedItem) const
{
	VEC_INT::const_iterator kIter = m_kLimit_NeedItem.begin();
	for(;m_kLimit_NeedItem.end() != kIter; ++kIter)
	{
		rkNeedItem.push_back(*kIter);
	}
}

//
tagMissionObject::tagMissionObject()
{
	Clear();
}

void tagMissionObject::Clear()
{
	for(int i=0;i<MISSION_PARAMNUM;i++)
	{
		iObjectCount[i] = 0;
		iObjectText[i] = 0;
	}
}

int tagMissionObject::GetEndCount(int const iObjectNo) const
{
	if( 0 <= iObjectNo
		&&	MISSION_PARAMNUM > iObjectNo )
	{
		return iObjectCount[iObjectNo];
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

void tagMissionObject::SetObject(int iObjectIndex, int iText, int iCount)
{
	iObjectText[iObjectIndex] = iText;
	iObjectCount[iObjectIndex] = iCount;
}

int tagMissionObject::GetTextNumber(int const iObjectNo) const
{
	if( 0 <= iObjectNo
		&&	MISSION_PARAMNUM > iObjectNo )
	{
		return iObjectText[iObjectNo];
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}




//
PgMissionInfo::PgMissionInfo()
{
	Clear();
}

PgMissionInfo::~PgMissionInfo()
{
}

void PgMissionInfo::Clear()
{
	m_kBasic.Clear();
	m_kObjectChapter.Clear();
	m_kObjectMission.Clear();

	m_kDepend_MissionOptionChapterList.clear();
	m_kDepend_MissionOptionMissionList.clear();
	m_kDepend_MissionOptionChapterLevelList.clear();
	m_kDepend_MissionOptionMissionLevelList.clear();

	m_bIsHaveArcade = true;
}

void PgMissionInfo::Clone(PgMissionInfo& rkOut) const
{
	rkOut.m_kBasic = m_kBasic;
	rkOut.m_kObjectChapter = m_kObjectChapter;
	rkOut.m_kObjectMission = m_kObjectMission;
	
	rkOut.m_kDepend_MissionOptionChapterList = m_kDepend_MissionOptionChapterList;
	rkOut.m_kDepend_MissionOptionMissionList = m_kDepend_MissionOptionMissionList;
	rkOut.m_kDepend_MissionOptionChapterLevelList = m_kDepend_MissionOptionChapterLevelList;
	rkOut.m_kDepend_MissionOptionMissionLevelList = m_kDepend_MissionOptionMissionLevelList;
}

void PgMissionInfo::ParseError(TiXmlNode* pkNode)
{
	char const* szName = pkNode->Value();
	int const iRow = pkNode->Row();
	int const iColumn = pkNode->Column();
	int const iMissionID = m_kBasic.iMissionID;
	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Mission ID[") << iMissionID << _T("] Unknown Element Name:[") << PgStringUtil::SafeUni(szName)
		<< _T("] Row:[") << iRow << _T("] Column:[") << iColumn << _T("]") );

}

bool PgMissionInfo::ParseXml(TiXmlElement *pkNode)
{
	if( !pkNode )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kBasic.iMissionID = PgStringUtil::SafeAtoi(pkNode->Attribute("ID"));
	if( !m_kBasic.iMissionID )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Can't Mission ID = 0") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring const kTime = PgStringUtil::SafeUni(pkNode->Attribute("TIME"));

	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(kTime, kVec, _T(":"));
	m_kBasic.SetTime(kVec);


	for(int i=0;i<4; i++)
	{
		int iLevel = i + 1;
		char kItemTimeName[100] = {0,};
		sprintf_s(kItemTimeName, sizeof(kItemTimeName), "ITEM_TIME%d", iLevel);
		std::wstring const kItemTime = PgStringUtil::SafeUni(pkNode->Attribute(kItemTimeName));
		VEC_WSTRING kItemVec;
		PgStringUtil::BreakSep(kItemTime, kItemVec, _T(":"));
		m_kBasic.SetItemTime(kItemVec, i);
	}

	m_kBasic.iScript = PgStringUtil::SafeAtoi(pkNode->Attribute("EVENT_SCRIPT_TIMEATTACK_END"));

	m_kBasic.iErrorTTW = PgStringUtil::SafeAtoi(pkNode->Attribute("MISSION_FAIL"));
	if ( 0 == m_kBasic.iErrorTTW )
	{
		// 디폴트로 진입할 수 없는 미션입니다.
		m_kBasic.iErrorTTW = 400225;
	}

	m_kBasic.iGadaCoinCount = PgStringUtil::SafeAtoi(pkNode->Attribute("GADACOIN_COUNT"));
	if( 0 == m_kBasic.iGadaCoinCount )
	{
		m_kBasic.iGadaCoinCount = 1;
	}

	TiXmlElement *pkChild = pkNode->FirstChildElement();
	while(pkChild)
	{
		char const* szName = pkChild->Value();

		bool bRet = true;

		if( 0 == strcmp(szName, "CHAPTER_COMPLETE") )
		{
			bRet = ParseXml_Chapter(pkChild);
		}
		else if( 0 == strcmp(szName, "MISSION_COMPLETE") )
		{
			bRet = ParseXml_Mission(pkChild);
		}
		else if( 0 == strcmp(szName, "MISSION_OPEN") )
		{
			bRet = ParseXml_MissionOpen(pkChild);
		}
		else if( 0 == strcmp(szName, "MISSION_TYPE") )
		{
			bRet = ParseXml_MissionType(pkChild);
		}
		else
		{
			ParseError(pkChild);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( !bRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		pkChild = pkChild->NextSiblingElement();
	}

	return true;
}

bool PgMissionInfo::ParseXml_Chapter(TiXmlElement *pkNode)
{
	if( !pkNode )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlElement *pkChild = pkNode->FirstChildElement();
	while(pkChild)
	{
		char const* szName = pkChild->Value();
		bool bRet = true;

		if( 0 == strcmp(szName, "GROUND") )
		{
			m_kBasic.iGroupNo = PgStringUtil::SafeAtoi(pkChild->Attribute("ID"));
			bRet = ParseXml_Options(pkChild);
		}
		else
		{
			ParseError(pkChild);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( !bRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;//실패
		}

		pkChild = pkChild->NextSiblingElement();
	}
	return true;
}

bool PgMissionInfo::ParseXml_Mission(TiXmlElement *pkNode)
{
	ContMissionOptionChapter m_kDepend_MissionOptionChapter;
	m_kDepend_MissionOptionChapter.clear();

	TiXmlElement* pkChild = pkNode->FirstChildElement();
	while(pkChild)
	{
		char const* szName = pkChild->Value();

		int const iObjectNo = PgStringUtil::SafeAtoi(pkChild->Attribute("TEXT"));
		std::wstring const kType = PgStringUtil::SafeUni(pkChild->Attribute("TYPE"));		
		std::wstring const kValue = PgStringUtil::SafeUni(pkChild->Attribute("VALUE"));
		int const iCount = PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT"));
		std::wstring const kLevel = PgStringUtil::SafeUni(pkChild->Attribute("LEVEL"));

		if( 0 == strcmp(szName, "OPTION") )
		{
			ContMissionOptionMissionList::value_type kOption(iObjectNo);
			kOption.Build(kType, iCount, kLevel, kValue);			

			m_kDepend_MissionOptionMissionList.push_back(kOption);
		}

		pkChild = pkChild->NextSiblingElement();
	}

	return true;
}

bool PgMissionInfo::ParseXml_MissionOpen(TiXmlElement *pkNode)
{
	if(NULL == pkNode) { return false; }
	m_kBasic.iEventMission = (PgStringUtil::SafeAtoi(pkNode->Attribute("EVENT_MISSION")));

	TiXmlElement* pkChild = pkNode->FirstChildElement();
	while(pkChild)
	{
		char const* szName = pkChild->Value();

		int const iLevel = PgStringUtil::SafeAtoi(pkChild->Attribute("LEVEL"));
		std::wstring const kPreLevel = PgStringUtil::SafeUni(pkChild->Attribute("PRE_MISSION_LEVEL"));
		std::wstring const kIngQuest = PgStringUtil::SafeUni(pkChild->Attribute("ING_QUEST"));
		std::wstring const kClearQuest = PgStringUtil::SafeUni(pkChild->Attribute("CLEAR_QUEST"));
		std::wstring const kEffect = PgStringUtil::SafeUni(pkChild->Attribute("EFFECT"));		
		std::wstring const kNeedItem = PgStringUtil::SafeUni(pkChild->Attribute("NEED_ITEM"));	
		int const iNeedItem = PgStringUtil::SafeAtoi(pkChild->Attribute("NEED_ITEM"));

		if( 0 == strcmp(szName, "MISSION") )
		{
			ContMissionOptionMissionOpenList::value_type kOption(iLevel,iNeedItem, kPreLevel, kIngQuest, kClearQuest, kEffect,kNeedItem);
			
			m_kDepend_MissionOptionMissionOpenList.push_back(kOption);
		}

		pkChild = pkChild->NextSiblingElement();
	}

	return true;
}

bool PgMissionInfo::ParseXml_MissionType(TiXmlElement *pkNode)
{
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	TiXmlAttribute const *pkAttribute = pkNode->FirstAttribute();
	while(pkAttribute)
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( 0 == strcmp(pcAttrName, "IS_HAVE_ARCADE") )
		{
			m_bIsHaveArcade = ( PgStringUtil::SafeAtoi(pcAttrValue) ? true : false );
		}
		pkAttribute = pkAttribute->Next();
	}

	return true;
}

bool PgMissionInfo::ParseXml_Options(TiXmlElement* pkNode)
{
	ContMissionOptionChapter m_kDepend_MissionOptionChapter;
	m_kDepend_MissionOptionMissionList.clear();

	TiXmlElement* pkChild = pkNode->FirstChildElement();
	while(pkChild)
	{
		char const* szName = pkChild->Value();

		int const iObjectNo = PgStringUtil::SafeAtoi(pkChild->Attribute("TEXT"));
		std::wstring const kType = PgStringUtil::SafeUni(pkChild->Attribute("TYPE"));
		std::wstring const kEffect = PgStringUtil::SafeUni(pkChild->Attribute("EFFECT"));
		std::wstring const kValue = PgStringUtil::SafeUni(pkChild->Attribute("VALUE"));
		int const iCount = PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT"));
		std::wstring const kLevel = PgStringUtil::SafeUni(pkChild->Attribute("LEVEL"));

		if( 0 == strcmp(szName, "OPTION") )
		{
			ContMissionOptionChapter::value_type kOption(iObjectNo);
			kOption.Build(kType, iCount, kLevel, kEffect, kValue);			

			m_kDepend_MissionOptionChapter.push_back(kOption);
		}

		pkChild = pkChild->NextSiblingElement();
	}

	auto ret = m_kDepend_MissionOptionChapterList.insert( std::make_pair(GroupNo(), m_kDepend_MissionOptionChapter) );
	if( !ret.second )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Mission Add m_kDepend_MissionOptionChapterList insert iGroupNo[") << m_kBasic.iGroupNo << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data!"));
	}

	return true;
}

int PgMissionInfo::GetParamEndCondition(eMissionOptionKind eKind, int iParamNo) const
{
	return ( eKind == MO_CHAPTER ) ? m_kObjectChapter.GetEndCount(iParamNo) : m_kObjectMission.GetEndCount(iParamNo);
}

bool PgMissionInfo::GetMissionChapterList(int const iGround, ContMissionOptionChapter const *& pkOut) const
{
	ContMissionOptionChapterList::const_iterator iter = m_kDepend_MissionOptionChapterList.find(iGround);
	while( m_kDepend_MissionOptionChapterList.end() != iter )
	{
		pkOut = &(*iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionInfo::GetMissionChapterListCheck(int const iGround) const
{
	ContMissionOptionChapterList::const_iterator iter = m_kDepend_MissionOptionChapterList.find(iGround);
	while( m_kDepend_MissionOptionChapterList.end() != iter )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

/*
bool PgMissionInfo::SetMissionOptionChapter(int const iGround)
{
	m_kDepend_MissionOptionChapterLevelList.clear();

	const ContMissionOptionChapter* pkMissionChapter = NULL;
	bool bRet = GetMissionChapterList(iGround, 	pkMissionChapter);

	if( !bRet )
	{
		INFO_LOG(BM::LOG_LV0, _T("[%s]-[%d] Mission ChapterListInfo fail - GroundInfo[%d]"), __FUNCTIONW__, __LINE__, iGround);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	
	for(int iLevel=0; iLevel!=MAX_MISSION_LEVEL; ++iLevel)
	{
		ContMissionOptionChapter::const_iterator iter = pkMissionChapter->begin();

		ContMissionOptionChapter m_kDepend_MissionOptionChapter;
		m_kDepend_MissionOptionChapter.clear();

		while( pkMissionChapter->end() != iter )
		{			
			VEC_INT::const_iterator itor = (*iter).m_kLimit_LevelValue.begin();
			while((*iter).m_kLimit_LevelValue.end() != itor)
			{
				if( (*itor) == (iLevel+1) )
				{
					m_kDepend_MissionOptionChapter.push_back(*iter);
				}
				++itor;
			}
			++iter;
		}
		if( m_kDepend_MissionOptionChapter.size() )
		{
			auto ret = m_kDepend_MissionOptionChapterLevelList.insert( std::make_pair(iLevel, m_kDepend_MissionOptionChapter) );
			if( !ret.second )
			{
				INFO_LOG(BM::LOG_LV0, _T("[%s] Mission Add m_kDepend_MissionOptionChapterLevelList insert Level[%d]"), __FUNCTIONW__, iLevel);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	return true;
}
*/

bool PgMissionInfo::GetMissionOptionChapter( int const iGroundNo, int const iLevel, ContMissionOptionChapter& rkOut) const
{
	ContMissionOptionChapter const *pkMissionChapter = NULL;
	bool const bRet = GetMissionChapterList( iGroundNo, pkMissionChapter );
	if( !bRet || !pkMissionChapter )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Mission ChapterListInfo fail - GroundNo=") << iGroundNo );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iCheckVelue = (iLevel + 1);
	ContMissionOptionChapter::const_iterator chapter_itr = pkMissionChapter->begin();
	for ( ; chapter_itr != pkMissionChapter->end() ; ++chapter_itr )
	{
		VEC_INT::const_iterator limitlevel_itr = chapter_itr->m_kLimit_LevelValue.begin();
		for ( ; limitlevel_itr!=chapter_itr->m_kLimit_LevelValue.end() ; ++limitlevel_itr )
		{
			if ( iCheckVelue == (*limitlevel_itr) )
			{
				rkOut.push_back( *chapter_itr );
			}
		}
	}

	if( 0 == rkOut.size() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Mission ContMissionOptionChapter is Empty! - Level=") << iCheckVelue );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgMissionInfo::SetMissionOptionMission()
{
	m_kDepend_MissionOptionMissionLevelList.clear();

	for(int iLevel=0; iLevel!=MAX_MISSION_LEVEL; ++iLevel)
	{
		ContMissionOptionMissionList::const_iterator iter = m_kDepend_MissionOptionMissionList.begin();

		ContMissionOptionMissionList m_kDepend_MissionOptionMission;
		m_kDepend_MissionOptionMission.clear();

		while( m_kDepend_MissionOptionMissionList.end() != iter )
		{
			VEC_INT::const_iterator itor = (*iter).m_kLimit_LevelValue.begin();
			while((*iter).m_kLimit_LevelValue.end() != itor)
			{
				if( (*itor) == (iLevel+1) )
				{
					m_kDepend_MissionOptionMission.push_back(*iter);
				}
				++itor;
			}
			++iter;
		}

		if( m_kDepend_MissionOptionMission.size() )
		{
			auto ret = m_kDepend_MissionOptionMissionLevelList.insert( std::make_pair(iLevel, m_kDepend_MissionOptionMission) );
			if( !ret.second )
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Mission Add m_kDepend_MissionOptionMissionLevelList insert Level[") << iLevel << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	return true;
}

bool PgMissionInfo::GetMissionOptionMission(int const iLevel, ContMissionOptionMissionList const*& pkOut) const
{
	ContMissionOptionMissionLevelList::const_iterator iter = m_kDepend_MissionOptionMissionLevelList.find(iLevel);
	while( m_kDepend_MissionOptionMissionLevelList.end() != iter )
	{
		pkOut = &(*iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionInfo::GetMissionOpen(int const iLevel, SMissionOptionMissionOpen const*& pkOut) const
{
	ContMissionOptionMissionOpenList::const_iterator iter = m_kDepend_MissionOptionMissionOpenList.begin();
	while( m_kDepend_MissionOptionMissionOpenList.end() != iter )
	{
		if( (*iter).iLevel == (iLevel+1) )
		{
			pkOut = &(*iter);
			return true;
		}
		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void tagMissionOptionMissionOpen::GetEffectNo(VEC_INT &rkEffect) const
{	
	VEC_INT::const_iterator kIter = m_kLimit_EffectValue.begin();
	for(;m_kLimit_EffectValue.end() != kIter; ++kIter)
	{
		rkEffect.push_back(*kIter);
	}
}
PgDefenceMissionSkill::PgDefenceMissionSkill()
{
}
PgDefenceMissionSkill::~PgDefenceMissionSkill()
{
}

void PgDefenceMissionSkill::Clear()
{
	m_kContDefenceSkill.clear();
}

void PgDefenceMissionSkill::Set(CONT_DEFENCE_SKILL & kCont)
{
	m_kContDefenceSkill = kCont;
}

void PgDefenceMissionSkill::Get(CONT_DEFENCE_SKILL& kCont)const
{
	kCont = m_kContDefenceSkill;
}

bool PgDefenceMissionSkill::IsLearn(int const iTeam, int const iSkillNo)const
{
	CONT_DEFENCE_SKILL::const_iterator team_it = m_kContDefenceSkill.find(iTeam);
	if(m_kContDefenceSkill.end() == team_it)
	{
		return false;
	}

	CONT_SET_DATA::const_iterator learn_it = team_it->second.find(iSkillNo);
	return (learn_it!=team_it->second.end());
}

int const MAX_DEFENCE7_SKILL = 20;
int PgDefenceMissionSkill::GetNextSkillNo(int const iTeam, int const iSkillNo)const
{
	const CONT_DEFSKILL *pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);
	if( !pkContDefMap )
	{ 
		return 0; 
	}

	int iParentSkill = iSkillNo;
	CONT_DEFSKILL::const_iterator c_iter = pkContDefMap->find(iSkillNo);
	if( c_iter!=pkContDefMap->end() )
	{
		iParentSkill = c_iter->second.iParentSkill;
	}

	CONT_DEFENCE_SKILL::const_iterator team_it = m_kContDefenceSkill.find(iTeam);
	if(m_kContDefenceSkill.end() == team_it)
	{
		return iParentSkill;
	}

	int iLastNo = iParentSkill;
	for(int i=0; i<MAX_DEFENCE7_SKILL; ++i)
	{
		int const iNext = iParentSkill+i;
		CONT_DEFSKILL::const_iterator skill_it = pkContDefMap->find(iNext);
		if( skill_it==pkContDefMap->end() )
		{
			return iLastNo;
		}

		iLastNo = iNext;

		CONT_SET_DATA::const_iterator learn_it = team_it->second.find(iNext);
		if( learn_it==team_it->second.end() )
		{
			return iNext;
		}
	}

	return 0;
}

int PgDefenceMissionSkill::GetGuardianSkillPoint(int const iGuardianNo, int const iSkillNo)
{
	if(IsGuardianSkill(iSkillNo))
	{
		return _GetGuardianSkillPoint(iGuardianNo,iSkillNo);
	}
	
	return _GetGuardianSkillPoint(iSkillNo);
}

int PgDefenceMissionSkill::_GetGuardianSkillPoint(int const iSkillNo)
{
	if(0==iSkillNo)
	{
		return 0;
	}

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);
	
	if( !pkGuardian )
	{
		return 0;
	}

	CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator guardian_it = pkGuardian->find(iSkillNo);
	if(guardian_it != pkGuardian->end())
	{
		return guardian_it->second.kCont.at(0).iNeed_StrategicPoint;
	}

	return 0;
}

int PgDefenceMissionSkill::_GetGuardianSkillPoint(int const iGuardianNo, int const iSkillNo)
{
	if(0==iGuardianNo || 0==iSkillNo)
	{
		return 0;
	}

	const CONT_MISSION_DEFENCE7_GUARDIAN_BAG *pkGuardian;
	g_kTblDataMgr.GetContDef(pkGuardian);
	
	if( !pkGuardian )
	{
		return 0;
	}

	CONT_MISSION_DEFENCE7_GUARDIAN_BAG::const_iterator guardian_it = pkGuardian->find(iGuardianNo);
	if(guardian_it != pkGuardian->end())
	{
		SMISSION_DEFENCE7_GUARDIAN const & kGuardian = guardian_it->second.kCont.at(0);
		if( iSkillNo==kGuardian.Skill01_No )
		{
			return kGuardian.Skill01_StrategicPoint;
		}
		if( iSkillNo==kGuardian.Skill02_No )
		{
			return kGuardian.Skill02_StrategicPoint;
		}
	}

	return 0;
}

bool PgDefenceMissionSkill::IsGuardianSkill(int const iSkillNo)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if( pkSkill )
	{
		GET_DEF(CEffectDefMgr, kEffectDefMgr);
		CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(pkSkill->GetEffectNo());
		if( pkEffectDef )
		{
			return pkEffectDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1);
		}
	}

	return false;
}

bool PgDefenceMissionSkill::SetSkillInsert(int const iTeam, int const iSkillNo)
{
	CONT_DEFENCE_SKILL::iterator iter = m_kContDefenceSkill.find(iTeam);
	if( m_kContDefenceSkill.end() == iter )
	{
		auto kPair = m_kContDefenceSkill.insert(std::make_pair(iTeam, CONT_DEFENCE_SKILL::mapped_type()));
		iter = kPair.first;
	}
	else
	{
		CONT_DEFENCE_SKILL::mapped_type& kValue = iter->second;
		CONT_DEFENCE_SKILL::mapped_type::iterator find_iter = kValue.find(iSkillNo);
		if( kValue.end() != find_iter )
		{
			// 이미 습득한 스킬
			return false;
		}
	}

	(*iter).second.insert(iSkillNo);

	return true;
}