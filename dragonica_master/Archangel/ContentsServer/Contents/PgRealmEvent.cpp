#include "stdafx.h"
#include "Variant/Global.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgStringUtil.h"
#include "PgServerSetMgr.h"
#include "PgRealmEvent.h"

////////////////////////////////////////////////////////
//
namespace RealmEventUtil
{
	void ParseError(TiXmlElement const* pkNode)
	{
		if( pkNode )
		{
			CAUTION_LOG(BM::LOG_LV1, L"Wrong Element["<<pkNode->Value()<<L"], Row["<<pkNode->Row()<<L"] Column["<<pkNode->Column()<<L"]");
		}
	}

	void AddBuff(int const iBuffNo)
	{
		BM::Stream kPacket(PT_N_M_NFY_ALL_USER_ADDEFFECT);
		kPacket.Push( iBuffNo );
		g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
	}

	void AddBuff(CONT_BUFF & VecBuff)
	{
		CONT_BUFF::iterator iter = VecBuff.begin();
		for( ; iter != VecBuff.end() ; ++iter)
		{
			BM::Stream kPacket(PT_N_M_NFY_ALL_USER_ADDEFFECT);
			kPacket.Push( *iter );
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
		}
	}
}

////////////////////////////////////////////////////////
// struct tagRealmEventTime
tagRealmEventTime::tagRealmEventTime()
{
	Clear();
}
tagRealmEventTime::tagRealmEventTime(tagRealmEventTime const& rhs)
	: kStartDateTime(rhs.kStartDateTime), kEndDateTime(rhs.kEndDateTime), eLoadState(rhs.eLoadState)
{
}
tagRealmEventTime::~tagRealmEventTime()
{
}
void tagRealmEventTime::Clear()
{
	kStartDateTime.year = kStartDateTime.month = kStartDateTime.day = kStartDateTime.hour = kStartDateTime.minute = kStartDateTime.second = -1;
	kEndDateTime.year = kEndDateTime.month = kEndDateTime.day = kEndDateTime.hour = kEndDateTime.minute = kEndDateTime.second = -1;
	eLoadState = EETLS_NONE;
}

bool tagRealmEventTime::ParseDate(char const* szValue, BM::DBTIMESTAMP_EX& rkOut)
{
	if( !szValue )
	{
		return false;
	}
	sscanf_s(szValue, "%4d-%2d-%2d", &rkOut.year, &rkOut.month, &rkOut.day);
	if( 0 > rkOut.year
	||	0 > rkOut.month
	||	0 > rkOut.day )
	{
		return false;
	}
	return true;
}
bool tagRealmEventTime::ParseTime(char const* szValue, BM::DBTIMESTAMP_EX& rkOut)
{
	if( !szValue )
	{
		return false;
	}
	sscanf_s(szValue, "%2d:%2d:%2d", &rkOut.hour, &rkOut.minute, &rkOut.second);
	if( 0 > rkOut.hour
	||	0 > rkOut.minute
	||	0 > rkOut.second )
	{
		return false;
	}
	return true;
}
bool tagRealmEventTime::IsCanRun(BM::DBTIMESTAMP_EX const& rkCurTime) const
{
	__int64 iStartDate = 0, iCurDate = 0, iEndDate = 0;
	__int64 iStartTime = 0, iCurTime = 0, iEndTime = 0;
	CGameTime::DBTimeEx2SecTime(kStartDateTime, iStartDate, CGameTime::OneDay);
	CGameTime::DBTimeEx2SecTime(kStartDateTime, iStartTime, CGameTime::MINUTE);
	CGameTime::DBTimeEx2SecTime(rkCurTime, iCurDate, CGameTime::OneDay);
	CGameTime::DBTimeEx2SecTime(rkCurTime, iCurTime, CGameTime::MINUTE);
	CGameTime::DBTimeEx2SecTime(kEndDateTime, iEndDate, CGameTime::OneDay);
	CGameTime::DBTimeEx2SecTime(kEndDateTime, iEndTime, CGameTime::MINUTE);
	iStartTime -= iStartDate * 60 * 24;
	iCurTime -= iCurDate * 60 * 24;
	iEndTime -= iEndDate * 60 * 24;

	bool const bInDate = (iStartDate <= iCurDate) && (iCurDate <= iEndDate);
	bool const bInTime = (iStartTime < iEndTime)? ((iStartTime <= iCurTime) && (iCurTime <= iEndTime)): !((iStartTime > iCurTime) && (iCurTime > iEndTime));
	return bInDate && bInTime;
}
bool tagRealmEventTime::ParseXml(char const* szElementName, char const* szValue)
{
	if( !szElementName
	||	!szValue )
	{
		return false;
	}

	if( NULL != strstr(szElementName, "START_DATE") )
	{
		if( true == ParseDate(szValue, kStartDateTime) )	{ eLoadState |= EETLS_STARTDATE; }
		else												{ return false; }
	}
	else if( NULL != strstr(szElementName, "END_DATE") )
	{
		if( true == ParseDate(szValue, kEndDateTime) )		{ eLoadState |= EETLS_ENDDATE; }
		else												{ return false; }
	}
	else if( NULL != strstr(szElementName, "START_TIME") )
	{
		if( true == ParseTime(szValue, kStartDateTime) )	{ eLoadState |= EETLS_STARTTIME; }
		else												{ return false; }
	}
	else if( NULL != strstr(szElementName, "END_TIME") )
	{
		if( true == ParseTime(szValue, kEndDateTime) )		{ eLoadState |= EETLS_ENDTIME; }
		else												{ return false; }
	}
	else
	{
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////
// class PgRealmQuestEvent
char const* XML_REWARD_NODE_NAME = "REWARD";
PgRealmQuestEvent::tagTargetCount::tagTargetCount()
	: iTargetCount(0), kContBuff()
{
}
PgRealmQuestEvent::tagTargetCount::tagTargetCount(tagTargetCount const& rhs)
	: iTargetCount(rhs.iTargetCount), kContBuff(rhs.kContBuff)
{
}
PgRealmQuestEvent::tagTargetCount::~tagTargetCount()
{
}
void PgRealmQuestEvent::tagTargetCount::GetBuff(CONT_BUFF & VecBuff) const
{
	VecBuff = kContBuff;
}
bool PgRealmQuestEvent::tagTargetCount::ParseXml(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	if( 0 != strcmp(XML_REWARD_NODE_NAME, pkRoot->Value()) )
	{
		return false;
	}

	CONT_DEFEFFECT const* pkDefEffect = NULL;
	g_kTblDataMgr.GetContDef(pkDefEffect);
	if( !pkDefEffect )
	{
		return false;
	}

	iTargetCount = static_cast< size_t >(PgStringUtil::SafeAtoi(pkRoot->Attribute("COUNT")));
	if( 0 == iTargetCount )
	{
		return false;
	}

	bool bRet = true;
	TiXmlElement const* pkSub = pkRoot->FirstChildElement();
	while( pkSub )
	{
		char const* szName = pkSub->Value();
		char const* szValue = pkSub->GetText();
		if( 0 == strcmp("BUFF", szName) )
		{
			int const iBuffNo = PgStringUtil::SafeAtoi(szValue);
			if( 0 == iBuffNo )
			{
				RealmEventUtil::ParseError(pkSub);
				bRet = false;
			}

			if( pkDefEffect->end() != pkDefEffect->find(iBuffNo) )
			{
				kContBuff.push_back( iBuffNo );
			}
			else
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Wrong Buff[" << iBuffNo << L"]");
				bRet = false;
			}
		}
		else
		{
			RealmEventUtil::ParseError(pkSub);
		}
		pkSub = pkSub->NextSiblingElement();
	}
	return bRet;
}
//
PgRealmQuestEvent::PgNotifyHelper::PgNotifyHelper()
	: m_kNotifyTickSec(0)
{
	Clear();
}
PgRealmQuestEvent::PgNotifyHelper::PgNotifyHelper(PgNotifyHelper const& rhs)
	: m_dwCurTickSec(rhs.m_dwCurTickSec), m_kNotifyTickSec(rhs.m_kNotifyTickSec)
{
}
PgRealmQuestEvent::PgNotifyHelper::~PgNotifyHelper()
{
}

void PgRealmQuestEvent::PgNotifyHelper::Clear()
{
	m_dwCurTickSec = 0;
}
bool PgRealmQuestEvent::PgNotifyHelper::Tick(bool const bCurStatus, DWORD const dwEleapsedTime)
{
	if( bCurStatus )
	{
		m_dwCurTickSec += dwEleapsedTime;
		if( m_kNotifyTickSec <= m_dwCurTickSec )
		{
			m_dwCurTickSec -= m_kNotifyTickSec;
			return true;
		}
	}
	return false;
}
void PgRealmQuestEvent::PgNotifyHelper::AnsInfo(BM::GUID const& rkCharGuid, SRealmQuestInfo const& rkRealmQuestInfo, BM::Stream::DEF_STREAM_TYPE const wType)
{
	BM::Stream kPacket(wType); // 특정 유저만
	rkRealmQuestInfo.WriteToPacket(kPacket);
	NfyUser(rkCharGuid, kPacket);
}
void PgRealmQuestEvent::PgNotifyHelper::NfyInfo(SRealmQuestInfo const& rkRealmQuestInfo, bool const bAllowSendToUser)
{
	BM::Stream kPacket(PT_N_C_NFY_REALM_QUEST_INFO); // 모든 서버
	rkRealmQuestInfo.WriteToPacket(kPacket);
	kPacket.Push( bAllowSendToUser );
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
}
void PgRealmQuestEvent::PgNotifyHelper::NfyUser(BM::GUID const& rkCharGuid, BM::Stream const& rkPacket) const
{
	g_kRealmUserMgr.Locked_SendToUser(rkCharGuid, rkPacket, false);
}
//
PgRealmQuestEvent::PgRewardBuffHelper::PgRewardBuffHelper()
{
	Clear();
}
PgRealmQuestEvent::PgRewardBuffHelper::PgRewardBuffHelper(PgRewardBuffHelper const& rhs)
	: m_kEleapsedTime(rhs.m_kEleapsedTime), m_kCount(rhs.m_kCount), m_kBuff(rhs.m_kBuff)
{
}
PgRealmQuestEvent::PgRewardBuffHelper::~PgRewardBuffHelper()
{
}

void PgRealmQuestEvent::PgRewardBuffHelper::Clear()
{
	m_kEleapsedTime = 0;
	m_kCount = 0;
	m_kBuff.clear();
}
void PgRealmQuestEvent::PgRewardBuffHelper::Set(size_t const iCount, CONT_BUFF & VecBuff)
{
	Clear();
	m_kCount = iCount;
	m_kBuff = VecBuff;
}
bool PgRealmQuestEvent::PgRewardBuffHelper::Tick(std::wstring const& rkRealmQuestID, DWORD const dwEleapsedTime)
{
	if( 0 != m_kCount &&	!m_kBuff.empty() )
	{
		//
		DWORD const dwOneSec = 1000;
		DWORD const dwNowSec = m_kEleapsedTime / dwOneSec;
		//
		DWORD const dwOneMinuteSec = 60;
		DWORD const dwSendSec = dwOneMinuteSec - dwNowSec;
		switch( dwNowSec )
		{
		case 0:
		case 30:
		case 50:
			{
				Notice(true, rkRealmQuestID, dwSendSec); // 공지만
			}break;
		default:
			{
				if( 60 <= dwNowSec )
				{
					Notice(true, rkRealmQuestID, dwSendSec); // 공지 + 버프
					return true;
				}
			}break;
		}
		m_kEleapsedTime += dwEleapsedTime;
	}
	return false;
}
void PgRealmQuestEvent::PgRewardBuffHelper::Notice(bool const bSuccess, std::wstring const& rkRealmQuestID, DWORD const dwNowSec)
{
	BM::Stream kPacket(PT_N_C_NFY_REALM_QUEST_REWARD);
	kPacket.Push( bSuccess );
	kPacket.Push( rkRealmQuestID );
	kPacket.Push( dwNowSec );
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
}

void PgRealmQuestEvent::PgRewardBuffHelper::GetBuff(CONT_BUFF & VecBuff)
{
	VecBuff = m_kBuff;
}

//
PgRealmQuestEvent::PgRealmQuestStatusHelper::PgRealmQuestStatusHelper()
	: m_kMngGuid()
{
	m_kMngGuid.Generate();
}
PgRealmQuestEvent::PgRealmQuestStatusHelper::PgRealmQuestStatusHelper(PgRealmQuestStatusHelper const& rhs)
	: m_kMngGuid(rhs.m_kMngGuid)
{
}
PgRealmQuestEvent::PgRealmQuestStatusHelper::~PgRealmQuestStatusHelper()
{
}
void PgRealmQuestEvent::PgRealmQuestStatusHelper::Load(SRealmQuestInfo const& rkRealmQuestInfo) const
{
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LOAD_REALM_QUEST, L"[dbo].[UP_Load_RealmQuestState]");
	kQuery.InsertQueryTarget( m_kMngGuid );
	kQuery.PushStrParam( rkRealmQuestInfo.kRealmQuestID );
	kQuery.QueryOwner(m_kMngGuid);
	g_kCoreCenter.PushQuery(kQuery);
}
void PgRealmQuestEvent::PgRealmQuestStatusHelper::Save(SRealmQuestInfo const& rkRealmQuestInfo) const
{
	__int64 const iSaveCount = rkRealmQuestInfo.iCurCount;
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_REALM_QUEST, L"[dbo].[UP_Update_RealmQuestState]");
	kQuery.InsertQueryTarget( m_kMngGuid );
	kQuery.PushStrParam( rkRealmQuestInfo.kRealmQuestID );
	kQuery.PushStrParam( iSaveCount );
	kQuery.QueryOwner(m_kMngGuid);
	g_kCoreCenter.PushQuery(kQuery);
}

// DB Query
bool PgRealmQuestEvent::Q_DQT_LOAD_REALM_QUEST(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet )
	{
		return false;
	}

	std::wstring kRealmQuestID;
	__int64 iCount = 0;

	CEL::DB_DATA_ARRAY::const_iterator result_iter = rkResult.vecArray.begin();
	while( rkResult.vecArray.end() != result_iter )
	{
		(*result_iter).Pop( kRealmQuestID );	++result_iter;
		(*result_iter).Pop( iCount );			++result_iter;
	}

	BM::Stream kPacket( PT_N_N_NFY_LOAD_REALM_QUEST );
	kPacket.Push( kRealmQuestID );
	kPacket.Push( iCount );
	::SendToRealmContents(PMET_REALM_EVENT, kPacket);

	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}
bool PgRealmQuestEvent::Q_DQT_UPDATE_REALM_QUEST(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS != rkResult.eRet
	&&	CEL::DR_NO_RESULT != rkResult.eRet )
	{
		return false;
	}
	g_kCoreCenter.ClearQueryResult( rkResult );
	return true;
}
//
PgRealmQuestEvent::PgRealmQuestEvent()
	: m_kRealmQuestInfo(), m_kContTime(), m_kNotifyHelper(), m_kContTargetCount(), m_kRewardBuffHelper(), m_kStatusHelper()
{
}
PgRealmQuestEvent::PgRealmQuestEvent(PgRealmQuestEvent const& rhs)
	: m_kRealmQuestInfo(rhs.m_kRealmQuestInfo), m_kContTime(rhs.m_kContTime), m_kNotifyHelper(rhs.m_kNotifyHelper)
	, m_kContTargetCount(rhs.m_kContTargetCount), m_kRewardBuffHelper(rhs.m_kRewardBuffHelper), m_kStatusHelper(rhs.m_kStatusHelper)
{
}
PgRealmQuestEvent::~PgRealmQuestEvent()
{
}

void PgRealmQuestEvent::Tick(SYSTEMTIME const& rkCurTime, DWORD const dwEleapsedTime)
{
	bool const bPreStatus = m_kRealmQuestInfo.bCurStatus;
	m_kRealmQuestInfo.bCurStatus = IsCanRun(BM::DBTIMESTAMP_EX(rkCurTime));

	if( false == bPreStatus
	&&	true == m_kRealmQuestInfo.bCurStatus )
	{
		// 새로 시작
		m_kRealmQuestInfo.iCurCount = 0;
		m_kRealmQuestInfo.iNextCount = 0;
		UpdateNextCount(m_kRealmQuestInfo, m_kContTargetCount); // 리셋
		m_kNotifyHelper.Clear();
		m_kRewardBuffHelper.Clear();
		m_kStatusHelper.Save(m_kRealmQuestInfo);
		m_kNotifyHelper.NfyInfo(m_kRealmQuestInfo);
	}
	if( true == bPreStatus
	&&	false == m_kRealmQuestInfo.bCurStatus )
	{
		// 종료
		size_t iCount = 0; // 달성 카운트
		CONT_BUFF VecBuff;
		CONT_TARGET_COUNT::const_iterator iter = m_kContTargetCount.begin();
		while( m_kContTargetCount.end() != iter )
		{
			CONT_TARGET_COUNT::key_type const& rkKey = (*iter).first;
			CONT_TARGET_COUNT::mapped_type const& rkValue = (*iter).second;
			if( rkKey <= m_kRealmQuestInfo.iCurCount
			&&	rkKey > iCount )
			{
				iCount = rkKey;
				rkValue.GetBuff(VecBuff);
			}
			++iter;
		}

		if( 0 != iCount	&& !VecBuff.empty() )
		{
			m_kRewardBuffHelper.Set(iCount, VecBuff); // 달성
		}
		else
		{
			m_kRewardBuffHelper.Notice(false, m_kRealmQuestInfo.kRealmQuestID);
			m_kRewardBuffHelper.Clear(); // 미달성
		}
	}

	if( m_kNotifyHelper.Tick(m_kRealmQuestInfo.bCurStatus, dwEleapsedTime) )
	{
		m_kNotifyHelper.NfyInfo(m_kRealmQuestInfo);
	}
	if( m_kRewardBuffHelper.Tick(m_kRealmQuestInfo.kRealmQuestID, dwEleapsedTime) )
	{
		// 버프
		CONT_BUFF VecBuff;
		m_kRewardBuffHelper.GetBuff(VecBuff);
		RealmEventUtil::AddBuff(VecBuff);
		m_kRewardBuffHelper.Clear();

		m_kRealmQuestInfo.bCurStatus = false; //버프 지급 후 상태 초기화
	}
}
void PgRealmQuestEvent::AddCount(BM::GUID const& rkCharGuid, size_t const iSetCount)
{
	if( !m_kRealmQuestInfo.bCurStatus )
	{
		return;
	}

	if( 0 == iSetCount )
	{
		++m_kRealmQuestInfo.iCurCount; // 정상적인 경로로
	}
	else
	{
		m_kRealmQuestInfo.iCurCount = iSetCount; // GM 툴 등으로
	}
	m_kStatusHelper.Save(m_kRealmQuestInfo);

	UpdateNextCount(m_kRealmQuestInfo, m_kContTargetCount);

	if( m_kContTargetCount.end() != m_kContTargetCount.find(m_kRealmQuestInfo.iCurCount) )
	{
		m_kNotifyHelper.NfyInfo(m_kRealmQuestInfo); // 특정 목표시마다 전체 공지
	}
	else
	{
		m_kNotifyHelper.AnsInfo(rkCharGuid, m_kRealmQuestInfo, PT_N_C_NFY_REALM_QUEST_INFO);
	}
}
void PgRealmQuestEvent::ReqInfo(BM::GUID const& rkCharGuid)
{
	m_kNotifyHelper.AnsInfo(rkCharGuid, m_kRealmQuestInfo);
}
void PgRealmQuestEvent::Load(__int64 const iCount)
{
	m_kRealmQuestInfo.bCurStatus = true;
	m_kRealmQuestInfo.iCurCount = static_cast< size_t >(iCount);
	UpdateNextCount(m_kRealmQuestInfo, m_kContTargetCount);
	m_kNotifyHelper.NfyInfo(m_kRealmQuestInfo);
}
bool PgRealmQuestEvent::ParseXml(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	m_kRealmQuestInfo.kRealmQuestID = PgStringUtil::SafeUni(pkRoot->Attribute("KEY"));
	if( m_kRealmQuestInfo.kRealmQuestID.empty() )
	{
		RealmEventUtil::ParseError(pkRoot);
		return false;
	}

	bool bRet = true;
	SRealmEventTime kNewTime;
	TiXmlElement const* pkSub = pkRoot->FirstChildElement();
	while( pkSub )
	{
		char const* szName = pkSub->Value();
		char const* szValue = pkSub->GetText();

		if( 0 == strcmp("NOTICE_TICK_SEC", szName) )
		{
			m_kNotifyHelper.NotifyTickSec( static_cast< DWORD >(PgStringUtil::SafeAtoi(szValue)) );
		}
		else if( 0 == strcmp("REWARD", szName) )
		{
			STargetCount kTemp;
			if( !kTemp.ParseXml(pkSub) )
			{
				bRet = false;
			}
			else
			{
				auto kRet = m_kContTargetCount.insert( std::make_pair(kTemp.iTargetCount, kTemp) );
				if( !kRet.second )
				{
					bRet = false;
				}
			}
		}
		else
		{
			if(true == kNewTime.ParseXml(szName, szValue) )
			{
				if(true == kNewTime.IsLoadWholeData())
				{ //시작날짜, 종료날짜, 시작시간, 종료시간 모두 로드 되었다면
					m_kContTime.push_back(kNewTime); //컨테이너에 원소 추가 후 삭제
					kNewTime.Clear();
				}
			}
			else
			{
				RealmEventUtil::ParseError(pkSub);
				bRet = false;
			}
		}

		pkSub = pkSub->NextSiblingElement();
	}
	UpdateNextCount(m_kRealmQuestInfo, m_kContTargetCount);

	m_kRealmQuestInfo.bCurStatus = IsCanRun(BM::DBTIMESTAMP_EX(BM::PgPackedTime::LocalTime()));
	if( m_kRealmQuestInfo.bCurStatus )
	{
		m_kStatusHelper.Load(m_kRealmQuestInfo);
	}
	else
	{
		m_kStatusHelper.Save(m_kRealmQuestInfo);
	}
	return bRet;
}
void PgRealmQuestEvent::UpdateNextCount(SRealmQuestInfo& rkRealmQuestInfo, CONT_TARGET_COUNT const& rkContTargetCount)
{
	if( 0 != rkRealmQuestInfo.iNextCount
	&&	rkRealmQuestInfo.iNextCount > rkRealmQuestInfo.iCurCount )
	{
		return;
	}

	size_t iMaxCount = 0;
	size_t iNextCount = 0;
	CONT_TARGET_COUNT::const_iterator iter = rkContTargetCount.begin();
	while( rkContTargetCount.end() != iter )
	{
		CONT_TARGET_COUNT::key_type const& rkKey = (*iter).first;
		iMaxCount = std::max(iMaxCount, rkKey);
		if( rkKey > rkRealmQuestInfo.iCurCount )
		{
			iNextCount = rkKey;
			break;
		}
		iMaxCount = std::max(iMaxCount, rkKey);
		++iter;
	}
	rkRealmQuestInfo.iNextCount = (0 != iNextCount)? iNextCount: iMaxCount;
}

bool PgRealmQuestEvent::IsCanRun(BM::DBTIMESTAMP_EX const& rkCurTime) const
{
	CONT_REALM_EVENT_TIME::const_iterator iter = m_kContTime.begin();
	for(; iter != m_kContTime.end(); ++iter)
	{
		if(true == iter->IsCanRun(rkCurTime)) { return true; }
	}

	return false;
}


////////////////////////////////////////////////////////
// class PgDiceEvent
PgDiceEvent::tagDice::tagDice()
	: m_iMinDice(0), m_iMaxDice(0), m_iHitDice(0), m_dwTick(0), m_iMaxFailCount(0)
{
}
bool PgDiceEvent::tagDice::ParseXml(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}
	m_iMinDice = PgStringUtil::SafeAtoi(pkRoot->Attribute("MIN"));
	m_iMaxDice = PgStringUtil::SafeAtoi(pkRoot->Attribute("MAX"));
	m_iHitDice = PgStringUtil::SafeAtoi(pkRoot->Attribute("HIT"));
	m_iMaxFailCount = static_cast< size_t >(PgStringUtil::SafeAtoi(pkRoot->Attribute("MAX_FAIL")));
	m_dwTick = static_cast< DWORD >(PgStringUtil::SafeAtoi(pkRoot->Attribute("TICK")));

	if( 0 == m_iMinDice
	||	0 == m_iMaxDice
	||	0 == m_iHitDice
	||	0 == m_iMaxFailCount
	||	0 == m_dwTick )
	{
		RealmEventUtil::ParseError(pkRoot);
		return false;
	}
	return true;
}
bool PgDiceEvent::tagDice::IsCanTick(DWORD const dwEleapsedTime) const
{
	return m_dwTick <= dwEleapsedTime;
}
bool PgDiceEvent::tagDice::RunDice() const
{
	int const iCurDice = BM::Rand_Range(m_iMaxDice, m_iMinDice);
	if( m_iHitDice <= iCurDice )
	{
		return true; // 시작
	}
	return false;
}
bool PgDiceEvent::tagDice::CheckFailCount(size_t const iCurFailed) const
{
	if( 0 < m_iMaxFailCount )
	{
		return m_iMaxFailCount <= iCurFailed;
	}
	return false;
}

//
PgDiceEvent::PgDiceEvent()
	: m_dwLastTick(0), m_iCurFailCount(0), m_iSumOfDiceBuffRate(0), m_kInfo(), m_kTime(), m_kContDiceBuff()
{
}
PgDiceEvent::~PgDiceEvent()
{
}

void PgDiceEvent::Tick(SYSTEMTIME const& rkCurTime, DWORD const dwEleapsedTime)
{
	if( m_kTime.IsCanRun(BM::DBTIMESTAMP_EX(rkCurTime)) )
	{
		m_dwLastTick += dwEleapsedTime;
		if( m_kInfo.IsCanTick(m_dwLastTick) )
		{
			m_dwLastTick = 0;
			DiceEvent();
		}
	}
}
bool PgDiceEvent::DiceEvent()
{
	if( m_kInfo.RunDice() )
	{
		m_iCurFailCount = 0;
		NfyAddBuff(); // 주사위 굴려서 나오면 시작
		return true;
	}
	else
	{
		++m_iCurFailCount;
		if( m_kInfo.CheckFailCount(m_iCurFailCount) )
		{
			m_iCurFailCount = 0;
			NfyAddBuff(); // 실패가 특정 수 넘으면 강제 시작
			return true;
		}
	}
	return false;
}
void PgDiceEvent::NfyAddBuff()
{
	// 서버로 버프 알림
	int const iCount = static_cast< int >(m_kContDiceBuff.size());
	int const iIndex = BM::Rand_Index(iCount);
	if( 0 <= iIndex
	&&	iCount > iIndex )
	{
		{
			int const iBuffNo = m_kContDiceBuff.at(iIndex);
			RealmEventUtil::AddBuff(iBuffNo);
		}
	}
	if( WEST_NONE != m_kWorldEnvStatus.eType )
	{
		BM::Stream kPacket(PT_N_M_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS);
		m_kWorldEnvStatus.iStartGameTime = g_kEventView.GetGameSecTime(); // 시작 시간 업데이트
		m_kWorldEnvStatus.WriteToPacket( kPacket );
		g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kPacket);
	}
}
bool PgDiceEvent::ParseXml(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	CONT_DEFEFFECT const* pkDefEffect = NULL;
	g_kTblDataMgr.GetContDef(pkDefEffect);
	if( !pkDefEffect )
	{
		return false;
	}

	bool bRet = true;
	TiXmlElement const* pkCurNode = pkRoot->FirstChildElement();
	while( NULL != pkCurNode )
	{
		char const* szName = pkCurNode->Value();
		char const* szValue = pkCurNode->GetText();
		if( 0 == strcmp("DICE", szName) )
		{
			if( !m_kInfo.ParseXml(pkCurNode) )
			{
				bRet = false;
			}
		}
		else if( 0 == strcmp("BUFF", szName) )
		{
			int const iBuffNo = PgStringUtil::SafeAtoi(szValue);
			if( 0 == iBuffNo )
			{
				RealmEventUtil::ParseError(pkCurNode);
				bRet = false;
			}

			if( pkDefEffect->end() != pkDefEffect->find(iBuffNo) )
			{
				m_kContDiceBuff.push_back( iBuffNo );
			}
			else
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Wrong Buff["<<iBuffNo<<L"]");
				bRet = false;
			}
		}
		else if( 0 == strcmp(szWORLD_ENVIRONMENT_STATUS_ELEMENT_NAME, szName) )
		{
			if( !m_kWorldEnvStatus.ParseXml(pkCurNode) )
			{
				RealmEventUtil::ParseError(pkCurNode);
				bRet = false;
			}
		}
		else
		{
			if( !m_kTime.ParseXml(szName, szValue) )
			{
				RealmEventUtil::ParseError(pkCurNode);
				bRet = false;
			}
		}

		pkCurNode = pkCurNode->NextSiblingElement();
	}

	if( m_kContDiceBuff.empty() )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Buff is Empty");
	}
	return bRet;
}


////////////////////////////////////////////////////////
// class PgRealmEventImpl
PgRealmEventMngImpl::PgRealmEventMngImpl()
	: m_kDiceEvent(), m_kContRealmQuest(), m_dwTickTime(BM::GetTime32())
{
}
PgRealmEventMngImpl::~PgRealmEventMngImpl()
{
}

bool PgRealmEventMngImpl::ParseXml()
{
	char const* szXmlPath = "./XML/Setting/RealmEvent.xml";
	TiXmlDocument kDocu( szXmlPath );
	if( !kDocu.LoadFile() )
	{
		if( kDocu.Error() )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't open file: " << szXmlPath << L"Error Msg["<<kDocu.ErrorDesc()<<L"]");
		}
		else
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't open file: " << szXmlPath);
		}
		return false;
	}

	TiXmlElement const* pkRootNode = kDocu.FirstChildElement();

	return ParseXmlImp(pkRootNode);
}

bool PgRealmEventMngImpl::ParseXmlImp(TiXmlElement const *pkElement )
{
	bool bRet = true;
	
	while( NULL != pkElement )
	{
		char const* szElementName = pkElement->Value();

		if ( 0 == ::strcmp( szElementName, "LOCAL") )
		{
			TiXmlNode const* pkFindLocalNode = PgXmlLocalUtil::FindInLocal(g_kLocal, pkElement);
			if( pkFindLocalNode )
			{
				TiXmlElement const* pkResultNode = pkFindLocalNode->FirstChildElement();
				if( pkResultNode )
				{
					ParseXmlImp( pkResultNode );
				}
			}
		}
		else if( 0 == strcmp(szElementName, "DICE_EVENT") )
		{
			if( !m_kDiceEvent.ParseXml(pkElement) )
			{
				bRet = false;
			}
		}
		else if( 0 == strcmp(szElementName, "REALM_QUEST") )
		{
			CONT_REALM_QUEST::mapped_type kNewRealmQuest;
			if( kNewRealmQuest.ParseXml(pkElement) )
			{
				auto kRet = m_kContRealmQuest.insert( std::make_pair(kNewRealmQuest.RealmQuestID(), kNewRealmQuest) );
				if( !kRet.second )
				{
					CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Duplicate Realm Quest ID: " << kNewRealmQuest.RealmQuestID());
					bRet = false;
				}
			}
			else
			{
				bRet = false;
			}
		}
		else
		{
			RealmEventUtil::ParseError(pkElement);
			bRet = false;
		}

		pkElement = pkElement->NextSiblingElement();
	}

	return bRet;
}
void PgRealmEventMngImpl::Tick()
{
	DWORD const dwNow = BM::GetTime32();
	DWORD const dwEleapsedTime = dwNow - m_dwTickTime;
	m_dwTickTime = dwNow;

	SYSTEMTIME kCurTime;
	::GetLocalTime(&kCurTime);
	m_kDiceEvent.Tick(kCurTime, dwEleapsedTime);
	{
		CONT_REALM_QUEST::iterator iter = m_kContRealmQuest.begin();
		while( m_kContRealmQuest.end() != iter )
		{
			(*iter).second.Tick(kCurTime, dwEleapsedTime);
			++iter;
		}
	}
}
bool PgRealmEventMngImpl::ProcessPacket(BM::Stream& rkPacket)
{
	BM::Stream::DEF_STREAM_TYPE wType = 0;
	rkPacket.Pop( wType );

	bool bRet = true;
	switch( wType )
	{
	case PT_C_N_REQ_REALM_QUEST_INFO:
		{
			BM::GUID kCharGuid;
			std::wstring kRealmQuestID;
			rkPacket.Pop( kCharGuid );
			rkPacket.Pop( kRealmQuestID );

			CONT_REALM_QUEST::iterator find_iter = m_kContRealmQuest.find(kRealmQuestID);
			if( m_kContRealmQuest.end() != find_iter )
			{
				(*find_iter).second.ReqInfo(kCharGuid);
			}
		}break;
	case PT_N_N_REQ_REALM_QUEST_ADD_COUNT:
		{
			BM::GUID kCharGuid;
			std::wstring kRealmQuestID;
			size_t iSetCount = 0;
			rkPacket.Pop( kCharGuid );
			rkPacket.Pop( kRealmQuestID );
			rkPacket.Pop( iSetCount );

			CONT_REALM_QUEST::iterator find_iter = m_kContRealmQuest.find(kRealmQuestID);
			if( m_kContRealmQuest.end() != find_iter )
			{
				(*find_iter).second.AddCount(kCharGuid, iSetCount);
			}
		}break;
	case PT_N_N_NFY_LOAD_REALM_QUEST:
		{
			std::wstring kRealmQuestID;
			__int64 iCount = 0;
			rkPacket.Pop( kRealmQuestID );
			rkPacket.Pop( iCount );

			CONT_REALM_QUEST::iterator find_iter = m_kContRealmQuest.find(kRealmQuestID);
			if( m_kContRealmQuest.end() != find_iter )
			{
				(*find_iter).second.Load(iCount);
			}
		}break;
	default:
		{
			return false;
		}break;
	}
	return bRet;
}

////////////////////////////////////////////////////////
// class PgRealmEventMng
PgRealmEventMng::PgRealmEventMng()
{
}
PgRealmEventMng::~PgRealmEventMng()
{
}
bool PgRealmEventMng::ParseXml()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ParseXml();
}
bool PgRealmEventMng::ProcessPacket(BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->ProcessPacket(rkPacket);
}
void PgRealmEventMng::Tick()
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	Instance()->Tick();
}