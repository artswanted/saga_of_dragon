#include "StdAfx.h"
#include "PgNoticeMgr.h"
#include "PgGMTask.h"
#include "Variant/PgNoticeAction.h"

PgNoticeMgr::PgNoticeMgr(void)
{
	NoticeLineIntervalSec(4);
	NoticeItemIntervalSec(10);
}

PgNoticeMgr::~PgNoticeMgr(void)
{
}

void PgNoticeMgr::DeleteNoticeData(SGmOrder const &kNoticeData)
{
	BM::CAutoMutex kMutex(m_kMutex);

	CONT_NOTICE_DATA::iterator Itr = m_kContNotice.find(kNoticeData.kCmdGuid);
	if(Itr == m_kContNotice.end())
	{
		if(kNoticeData.kCmdGuid.IsNotNull())
		{
			g_kGMTask.Locked_FailedOrder(kNoticeData.kCmdGuid, GE_DATA_NOT_FOUND);
		}
		return;
	}

	g_kGMTask.Locked_SendOrderState(kNoticeData.kCmdGuid, OS_DONE);
	m_kContNotice.erase(Itr);
}

void PgNoticeMgr::SetNoticeData(SGmOrder const &kNoticeData)
{
	SNotice kNewData;
	kNewData.kNoticeGuid = kNoticeData.kCmdGuid;
	PgGMTask::GetNowTime(kNewData.m_kStartTime);
	
	if(kNoticeData.dtTime[0] > kNewData.m_kStartTime)
	{
		kNewData.m_kStartTime = kNoticeData.dtTime[0];
	}

	kNewData.m_iRepeatCountSet = kNoticeData.iValue[0];
	kNewData.m_iRepeatTime = kNoticeData.iValue[1];

	NOTICE_TEXT rkText;
	MakeSendString(kNoticeData.wsString2, rkText);

	kNewData.m_kText = rkText;
	kNewData.m_usRealmNo = kNoticeData.usRealm;
	kNewData.m_usChannelNo = kNoticeData.usChannel;

	BM::CAutoMutex kMutex(m_kMutex);
	if(false==rkText.empty())
	{
		m_kContNotice.insert(std::make_pair(kNewData.kNoticeGuid, kNewData));
	}
}

void PgNoticeMgr::CheckTime()
{
	BM::CAutoMutex kMutex(m_kMutex);

	if (m_kContNotice.empty())
	{
		// 대부분의 시간이 비어 있을 것이기 때문에, 검사를 먼저 해 준다.
		return;
	}

	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();

	BM::DBTIMESTAMP_EX kNowTime;

	PgGMTask::GetNowTime(kNowTime);

	std::vector<BM::GUID> kContDelete;

	static __int64 s_i64NextNoticeTime = 0;
	static BM::GUID kCurrentNotice = BM::GUID::NullData();

	if (i64CurTime < s_i64NextNoticeTime)
	{
		return;
	}

	CONT_NOTICE_DATA::mapped_type  * pkNotice = NULL;
	if (false == kCurrentNotice.IsNull())
	{
		// 이전에 보내던 Notice 찾아 보고...
		CONT_NOTICE_DATA::iterator itor_notice = m_kContNotice.find(kCurrentNotice);
		if (itor_notice != m_kContNotice.end())
		{
			CONT_NOTICE_DATA::mapped_type  * pkElement = &((*itor_notice).second);
			if(kNowTime >= pkElement->m_kStartTime && i64CurTime >= pkElement->m_i64NextNotiTime)
			{
				pkNotice = &((*itor_notice).second);
			}
		}
	}

	if (pkNotice == NULL)
	{
		// 새로운 Notice 를 찾아라..
		CONT_NOTICE_DATA::iterator itor_notice = m_kContNotice.begin();
		while (itor_notice != m_kContNotice.end() && pkNotice == NULL)
		{
			CONT_NOTICE_DATA::mapped_type  * pkElement = &((*itor_notice).second);
			if(kNowTime >= pkElement->m_kStartTime && i64CurTime >= pkElement->m_i64NextNotiTime)
			{
				pkNotice = pkElement;
			}

			++itor_notice;
		}
	}

	if (pkNotice == NULL)
	{
		// 보낼 것이 없다.
		return;
	}

	BM::Stream kPacket(PT_GM_A_REQ_GMCOMMAND, EGMC_NOTICE);
	kPacket.Push(pkNotice->m_usRealmNo);
	kPacket.Push(pkNotice->m_usChannelNo);
	kPacket.Push(pkNotice->m_kText[pkNotice->m_usNowLineCount]);
	if(pkNotice->eOrderState == OS_DOING)
	{
		// 한번이라도 보냈으면 OS_DONE 으로 처리
		g_kGMTask.Locked_SendOrderState(pkNotice->kNoticeGuid, OS_DONE);
		pkNotice->eOrderState = OS_DONE;
	}
	SendToImmigration(kPacket);
	INFO_LOG(BM::LOG_LV9, __FL__ << _T("Time=") << i64CurTime << _T(", Notice=") << pkNotice->m_kText[pkNotice->m_usNowLineCount]);

	++pkNotice->m_usNowLineCount;

	if (pkNotice->m_usNowLineCount >= pkNotice->m_kText.size())
	{
		pkNotice->m_usNowLineCount = 0;
		pkNotice->m_i64NextNotiTime = i64CurTime + pkNotice->m_iRepeatTime;
		++pkNotice->m_usNowTextCount;
		s_i64NextNoticeTime = i64CurTime + NoticeItemIntervalSec();
	}
	else
	{
		s_i64NextNoticeTime = i64CurTime + NoticeLineIntervalSec();
		kCurrentNotice = pkNotice->kNoticeGuid;
	}

	if(pkNotice->m_usNowTextCount >= pkNotice->m_iRepeatCountSet)
	{
		m_kContNotice.erase(pkNotice->kNoticeGuid);
		kCurrentNotice = BM::GUID::NullData();
	}
}

unsigned short PgNoticeMgr::MakeSendString(std::wstring const &kText, NOTICE_TEXT &rkOut)
{
	BM::CAutoMutex kLock(m_kMutex);

	int iCount = (int)kText.size();
	if( (wcscmp(kText.c_str(), L"") == 0) || (wcscmp(kText.c_str(), L" ") == 0) )
	{
		rkOut.clear();
		return 0;
	}

	std::wstring const kToken(L"\\r\\n");
	size_t kOffset = 0;
	std::wstring kTmp;

	while(kOffset < kText.length())
	{
		size_t kIndex = kText.find(kToken.c_str(),kOffset);

		if(std::wstring::npos == kIndex)
		{
			if(kOffset < kText.length())
			{
				kTmp.assign(kText,kOffset,kText.length() - kOffset);
				rkOut.push_back(kTmp);
			}
			break;
		}

		kTmp.assign(kText,kOffset,kIndex - kOffset);

		rkOut.push_back(kTmp);

		kOffset = kIndex + kToken.length();
	}
	return (unsigned short)rkOut.size();
}