#include "stdafx.h"
#include "PgUserQuestSessionMgr.h"

//
SUserQusetSessionInfo::SUserQusetSessionInfo()
	: kSessionGuid(), iCurDialogID(0), iNextDialogID(0)
{
}

SUserQusetSessionInfo::SUserQusetSessionInfo(SUserQusetSessionInfo const& rhs)
	: kSessionGuid(rhs.kSessionGuid), iCurDialogID(rhs.iCurDialogID), iNextDialogID(rhs.iNextDialogID)
{
}

bool SUserQusetSessionInfo::operator ==(SUserQusetSessionInfo const& rhs)
{
	return kSessionGuid == rhs.kSessionGuid
		&&	iCurDialogID == rhs.iCurDialogID
		&&	iNextDialogID == rhs.iNextDialogID;
}

SUserQusetSessionInfo SUserQusetSessionInfo::CreateNewSession(int const iCurDialog)
{
	SUserQusetSessionInfo kTempSession;
	kTempSession.kSessionGuid = BM::GUID::Create();
	kTempSession.iCurDialogID = iCurDialog;
	return kTempSession;
}


//
PgUserQuestSession::PgUserQuestSession()
	: m_kMutex()
{
	Clear();
}

PgUserQuestSession::PgUserQuestSession(PgUserQuestSession const& rhs)
	: m_kMutex(), m_kNpcGuid(rhs.m_kNpcGuid), m_iQuestID(rhs.m_iQuestID), m_kUserQuest(m_kUserQuest)
{
}

PgUserQuestSession::~PgUserQuestSession()
{
}

void PgUserQuestSession::Clear()
{
	m_kNpcGuid.Clear();
	m_iQuestID = 0;
	m_kUserQuest.clear();
}

bool PgUserQuestSession::IsEmpty()
{
	return BM::GUID::IsNull(m_kNpcGuid)
		&&	0 == m_iQuestID
		&&	m_kUserQuest.empty();
}

void PgUserQuestSession::NewTalk(BM::GUID const& rkNpcGuid, int const iQuestID, int const iDialogID, BM::GUID& rkSessionGuid)
{

	BM::CAutoMutex kLock(m_kMutex);

	if( BM::GUID::IsNull(m_kNpcGuid)
	&&	0 == m_iQuestID )
	{
		m_kNpcGuid = rkNpcGuid;
		m_iQuestID = iQuestID;
	}
	else
	{
		if( m_kNpcGuid != rkNpcGuid
		||	m_iQuestID != iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Different Old[Npc:" << m_kNpcGuid << L", Quest:" << m_iQuestID << L"] New[Npc:" << rkNpcGuid << L", Quest:" << iQuestID << L"]");
			return;
		}
	}

	SUserQusetSessionInfo const kTempSession(SUserQusetSessionInfo::CreateNewSession(iDialogID));

	auto kRet = m_kUserQuest.insert( std::make_pair(kTempSession.kSessionGuid, kTempSession) );
	if( kRet.second )
	{
		rkSessionGuid = kTempSession.kSessionGuid;
	}
	else
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"incredible!!!!, duplicated Guid UserQuestSession  [NPC:" << rkNpcGuid << L", Quest:" << iQuestID << L", DialogID" << iDialogID << L"]");
	}
}

bool PgUserQuestSession::SummitNextTalk(BM::GUID const& rkNpcGuid, BM::GUID const& rkSessionGuid, int const iQuestID, int const iDialogID, int const iNextDialogID)
{

	BM::CAutoMutex kLock(m_kMutex);

	if( m_kNpcGuid != rkNpcGuid )
	{
		return false;
	}

	if( m_iQuestID != iQuestID )
	{
		return false;
	}

	ContUserQuestSessionInfo::iterator find_iter = m_kUserQuest.find( rkSessionGuid );
	if( m_kUserQuest.end() == find_iter )
	{
		return false;
	}

	SUserQusetSessionInfo& rkUserQuestSessionInfo = (*find_iter).second;
	if( 0 != rkUserQuestSessionInfo.iNextDialogID )
	{
		return false;
	}
	rkUserQuestSessionInfo.iNextDialogID = iNextDialogID;
	return true;
}


//
PgUserQuestSessionMgr::PgUserQuestSessionMgr()
{
}
PgUserQuestSessionMgr::~PgUserQuestSessionMgr()
{
}

void PgUserQuestSessionMgr::AddQuestSession(BM::GUID const& rkCharGuid)
{
	m_kSession.insert( std::make_pair(rkCharGuid, PgUserQuestSession()) );
}

void PgUserQuestSessionMgr::RemoveQuestSession(BM::GUID const& rkCharGuid)
{
	m_kSession.erase(rkCharGuid);
}

bool PgUserQuestSessionMgr::IsEmptyTalk(BM::GUID const& rkCharGuid)
{
	ContSession::iterator find_iter = m_kSession.find(rkCharGuid);
	if( m_kSession.end() != find_iter )
	{
		return (*find_iter).second.IsEmpty();
	}
	return true;
}

void PgUserQuestSessionMgr::ClearTalk(BM::GUID const& rkCharGuid)
{
	ContSession::iterator find_iter = m_kSession.find(rkCharGuid);
	if( m_kSession.end() != find_iter )
	{
		(*find_iter).second.Clear();
	}
}

void PgUserQuestSessionMgr::NewTalk(BM::GUID const& rkCharGuid, BM::GUID const& rkNpcGuid, int const iQuestID, int const iDialogID, BM::GUID& rkSessionGuid)
{
	ContSession::iterator find_iter = m_kSession.find(rkCharGuid);
	if( m_kSession.end() != find_iter )
	{
		(*find_iter).second.NewTalk(rkNpcGuid, iQuestID, iDialogID, rkSessionGuid);
	}
}

bool PgUserQuestSessionMgr::SummitNextTalk(BM::GUID const& rkCharGuid, BM::GUID const& rkNpcGuid, BM::GUID const& rkSessionGuid, int const iQuestID, int const iDialogID, int const iNextDialogID)
{
	ContSession::iterator find_iter = m_kSession.find(rkCharGuid);
	if( m_kSession.end() != find_iter )
	{
		return (*find_iter).second.SummitNextTalk(rkNpcGuid, rkSessionGuid, iQuestID, iDialogID, iNextDialogID);
	}
	return false;
}