#include "stdafx.h"
#include "variant/PgLogUtil.h"
#include "PgGambleUserMgr.h"
#include "Item/PgPostManager.h"
#include "Global.h"

bool PgGambleUserMgrImpl::IsGambling(BM::GUID const & kGuid)
{
	return m_kCont.find(kGuid) != m_kCont.end();
}

bool PgGambleUserMgrImpl::Get(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo)
{
	CONT_GAMBLE_USER::const_iterator iter = m_kCont.find(kCharGuid);
	if(iter == m_kCont.end())
	{
		return false;
	}

	kGambleInfo = (*iter).second;
	return true;
}

bool PgGambleUserMgrImpl::Set(BM::GUID const & kCharGuid, SGambleUserInfo & kGambleInfo)
{
	CONT_GAMBLE_USER::iterator iter = m_kCont.find(kCharGuid);
	if(iter == m_kCont.end())
	{
		return false;
	}
	(*iter).second = kGambleInfo;
	return true;
}

bool PgGambleUserMgrImpl::Add(BM::GUID const & kCharGuid, int const iRouletteCount, int const iMixPoint)
{
	return m_kCont.insert(std::make_pair(kCharGuid, SGambleUserInfo(kCharGuid, iRouletteCount, iMixPoint))).second;
}

bool PgGambleUserMgrImpl::Remove(BM::GUID const & kCharGuid)
{
	CONT_GAMBLE_USER::iterator iter = m_kCont.find(kCharGuid);
	if(iter == m_kCont.end())
	{
		return false;
	}
	m_kCont.erase(iter);
	return true;
}

void PgGambleUserMgrImpl::LogOut(SContentsUser const& rkCharInfo,int const iFrom,int const iTitle,int const iText)
{
	SGambleUserInfo kGambleInfo;
	if(false == Get(rkCharInfo.kCharGuid, kGambleInfo))
	{
		return;
	}

	if(true == kGambleInfo.kContResult.empty())
	{
		return;
	}

	std::wstring wstrFrom, wstrTitle, wstrText;
	GetDefString(iFrom, wstrFrom);
	GetDefString(iTitle, wstrTitle);
	GetDefString(iText, wstrText);

	PgLogUtil::PgLogWrapperContents kLogCont(ELogMain_Contents_Item, ELogSub_Gamblemachine_Cash, rkCharInfo);
	{
		PgLog kLog(ELOrderMain_Mail, ELOrderSub_Forwarding);
		kLog.Set( PgLogUtil::AtIndex(1), rkCharInfo.kName );
		kLog.Set( PgLogUtil::AtIndex(2), std::wstring(wstrTitle + L"\r" + wstrText) );
		kLogCont.Add( kLog );
	}

	for(CONT_GAMBLEMACHINERESULT::const_iterator iter = kGambleInfo.kContResult.begin();iter != kGambleInfo.kContResult.end();++iter)
	{
		PgBase_Item const& rkItem = (*iter);
		PgItemLogUtil::CreateLog(kLogCont, PgItemWrapper(rkItem, SItemPos(IT_POST, 0)));
		g_kPostMgr.PostSystemMailByGuid(rkCharInfo.kCharGuid, wstrFrom, wstrTitle, wstrText, rkItem, 0);
	}
	kLogCont.Commit();

	Remove(rkCharInfo.kCharGuid);
}