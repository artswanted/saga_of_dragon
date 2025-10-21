#include "stdafx.h"
#include <set>
#include <algorithm>
#include "variant/PgDbCache.h"
#include "Variant/PgControlDefMgr.h"
#include "PgServerSetMgr.h"
#include "PgSendWrapper.h"
#include "JobDispatcher.h"

void PgRealmUserManager::Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kCardMgr.Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(kSI,kGndKey,pkPacket);
}

void PgRealmUserManager::Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kCardMgr.Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(kSI,kGndKey,pkPacket);
}