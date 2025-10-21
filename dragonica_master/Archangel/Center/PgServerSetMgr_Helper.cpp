#include "StdAfx.h"
#include "PgServerSetMgr.h"

void PgServerSetMgr::Locked_SendWarnMessage( BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid )const
{
	BM::CAutoMutex kLock(m_kMutex);
	SendWarnMessage( kGuid, iMessageNo, byLevel, bMemberGuid );
}

void PgServerSetMgr::SendWarnMessage(BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid)const
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE, iMessageNo);
	kPacket.Push(byLevel);
	SendToUser( kGuid, kPacket, bMemberGuid );
}

void PgServerSetMgr::Locked_SendWarnMessage2( BM::GUID const &rkGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid )const
{
	BM::CAutoMutex kLock(m_kMutex);
	SendWarnMessage2( rkGuid, iMessageNo, iValue, byLevel, bMemberGuid );
}

void PgServerSetMgr::SendWarnMessage2(BM::GUID const &rkGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid )const
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE2, iMessageNo);
	kPacket.Push(iValue);
	kPacket.Push(byLevel);
	SendToUser( rkGuid, kPacket, bMemberGuid );
}

bool PgServerSetMgr::Locked_SendToUser( BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUser( kGuidKey, rkPacket, IsMemberGuid);
}

bool PgServerSetMgr::SendToUser( BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)const
{
	PgPlayer* pkPlayerData = NULL;
	if (IsMemberGuid)
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itor = m_kContPlayer_MemberKey.find(kGuidKey);
		if( m_kContPlayer_MemberKey.end() != itor )
		{
			pkPlayerData = itor->second;
		}
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itor = m_kContPlayer_CharKey.find(kGuidKey);
		if( m_kContPlayer_CharKey.end() != itor )
		{
			pkPlayerData = itor->second;
		}
	}

	if (pkPlayerData != NULL)
	{
		::SendToUser( pkPlayerData->GetMemberGUID(), pkPlayerData->GetSwitchServer(), rkPacket );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::SendToUser(std::wstring const &kStrNick, BM::Stream const &rkPacket)const
{
	// і»єОїЎј­ ГЈѕЖ єё±в
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor = m_kContPlayer_MemberKey.begin();
	while( m_kContPlayer_MemberKey.end() != itor )
	{
		if((*itor).second)
		{
			if(0 == _tcsicmp((*itor).second->Name().c_str(), kStrNick.c_str()))
			{
				::SendToUser( (*itor).second->GetMemberGUID(), (*itor).second->GetSwitchServer(), rkPacket );
				return true;
			}
		}
		++itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUser(rkVec, rkPacket, bIsMemberGuid);
}


bool PgServerSetMgr::SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid)const
{
	VEC_GUID::const_iterator iter = rkVec.begin();
	size_t iSendCount = 0;
	while(rkVec.end() != iter)
	{
		if( SendToUser((*iter), rkPacket, bIsMemberGuid) )
		{
			++iSendCount;
		}
		++iter;
	}

	if( iSendCount ) 
	{
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

}

bool PgServerSetMgr::Locked_SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUserGround(rkGuid, rkPacket, bIsMemberGuid, bIsGndWrap);
}

bool PgServerSetMgr::SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)const
{
	SContentsUser kUserInfo;
	if( S_OK == GetPlayerInfo(rkGuid, bIsMemberGuid, kUserInfo) )
	{
		return SendToGround(kUserInfo.kGndKey, rkPacket, bIsGndWrap);
	}
	// ґЩёҐ Г¤іОїЎј­ АЇАъ ГЈѕЖ єё±в
	SRealmUserInfo kRealmUserInfo;
	bool bFind = false;
	if (bIsMemberGuid)
	{
		bFind = GetRealmUserByMemGuid(rkGuid, kRealmUserInfo);
	}
	else
	{
		bFind = GetRealmUserByCharGuid(rkGuid, kRealmUserInfo);
	}

	if (bFind)
	{
		BM::Stream kWrapPacket(PT_T_T_SEND_TO_USER_MAP, kRealmUserInfo.kMemberGuid);
		kWrapPacket.Push(bIsGndWrap);
		kWrapPacket.Push(rkPacket);
		return SendToChannel(kRealmUserInfo.sChannel, kWrapPacket);
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

// ґЩёҐ Channel(=Center) ·О ЖРЕ¶А» єёіЅґЩ.
//	sChannelNo = 0 АМёй АЪ±вАЪЅЕА» Б¦їЬЗС ёрµз Г¤іО·О єёіЅґЩ.
bool PgServerSetMgr::SendToChannel(short int sChannelNo, BM::Stream const &rkPacket)const
{
	SERVER_IDENTITY kSI = (SERVER_IDENTITY) g_kProcessCfg.ServerIdentity();
	if (sChannelNo == 0)
	{
		kSI.nChannel = 0;
		kSI.nServerType = CEL::ST_CENTER;
		kSI.nServerNo = 0;	// nServerTypeАЗ ёрµз ј­№ц·О АьјЫЗП¶у.~
	}
	else
	{
		ContServerID kConSI;
		if (S_OK != g_kProcessCfg.Locked_GetServerIdentity(g_kProcessCfg.RealmNo(), sChannelNo, CEL::ST_CENTER, kConSI))
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot Get ServerIdentity R[") << g_kProcessCfg.RealmNo() << _T("], C[") << sChannelNo << _T("], CENTER") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if (kConSI.size() <= 0)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		kSI = kConSI.at(0);
	}
	BM::Stream kIPacket(PT_A_A_WRAPPER, kSI);
	kIPacket.Push(rkPacket);
	return SendToImmigration(kIPacket);
}



