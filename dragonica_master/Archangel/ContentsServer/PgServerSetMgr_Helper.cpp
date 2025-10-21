#include "StdAfx.h"
#include "PgServerSetMgr.h"

void PgRealmUserManager::Locked_SendWarnMessage( BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	SendWarnMessage(kGuid, iMessageNo, byLevel, bMemberGuid);
}

void PgRealmUserManager::SendWarnMessage( BM::GUID const &kGuid, int const iMessageNo, BYTE const byLevel, bool const bMemberGuid)
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE, iMessageNo);
	kPacket.Push(byLevel);
	SendToUser( kGuid, kPacket, bMemberGuid );
}

void PgRealmUserManager::Locked_SendWarnMessage2(BM::GUID const &rkGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	SendWarnMessage2( rkGuid, iMessageNo, iValue, byLevel, bMemberGuid );
}

void PgRealmUserManager::SendWarnMessage2(BM::GUID const &rkGuid, int const iMessageNo, int const iValue, BYTE const byLevel, bool const bMemberGuid )
{
	BM::Stream kPacket(PT_M_C_NFY_WARN_MESSAGE2, iMessageNo);
	kPacket.Push(iValue);
	kPacket.Push(byLevel);
	SendToUser( rkGuid, kPacket, bMemberGuid );
}

bool PgRealmUserManager::Locked_SendToUser( BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUser( kGuidKey, rkPacket, IsMemberGuid);
}

bool PgRealmUserManager::SendToUser( BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)
{
	SCenterPlayerData* pkPlayerData = NULL;
	
	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(kGuidKey, IsMemberGuid, kLockPlayer) )
	{
		SCenterPlayerData* pkPlayerData = kLockPlayer.pkCenterPlayerData;
		if (pkPlayerData != NULL)
		{
			BM::Stream kWrappedPacket(PT_A_C_WRAPPED_PACKET);
			kWrappedPacket.Push(pkPlayerData->guidMember);
			kWrappedPacket.Push(rkPacket);
			SendToChannel(pkPlayerData->ChannelNo(), kWrappedPacket);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::SendToUser(std::wstring const &kStrNick, BM::Stream const &rkPacket)
{
	// 내부에서 찾아 보기
	SCenterPlayer_Lock kLockPlayer;
	if (GetPlayerInfo(kStrNick, kLockPlayer))
	{
		if(kLockPlayer.pkCenterPlayerData->pkPlayer)
		{
			BM::Stream kWrappedPacket(PT_A_T_WRAPPED_PACKET);
			kWrappedPacket.Push(PT_S_S_SEND_TO_SWITCH_USER);
			kWrappedPacket.Push(kLockPlayer.pkCenterPlayerData->guidMember);
			kWrappedPacket.Push(rkPacket);
			SendToChannel(kLockPlayer.pkCenterPlayerData->ChannelNo(), kWrappedPacket);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::Locked_SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUser(rkVec, rkPacket, bIsMemberGuid);
}


bool PgRealmUserManager::SendToUser(VEC_GUID const &rkVec, BM::Stream const &rkPacket, bool const bIsMemberGuid)
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
	if( 0 != iSendCount )
	{
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

bool PgRealmUserManager::Locked_SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToUserGround(rkGuid, rkPacket, bIsMemberGuid, bIsGndWrap);
}

bool PgRealmUserManager::SendToUserGround(BM::GUID const &rkGuid, BM::Stream const &rkPacket, bool const bIsMemberGuid, bool const bIsGndWrap)
{
	SContentsUser kUserInfo;
	if( S_OK == GetPlayerInfo(rkGuid, bIsMemberGuid, kUserInfo) )
	{
		return SendToGround(kUserInfo.sChannel, kUserInfo.kGndKey, rkPacket, bIsGndWrap);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgRealmUserManager::Locked_SendToSwitch(BM::GUID const & MemberGuid, BM::Stream const & Packet)
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToSwitch(MemberGuid, Packet);
}

bool PgRealmUserManager::SendToSwitch( BM::GUID const &kMemberGuid, BM::Stream const &rkPacket)const
{
	SCenterPlayer_Lock kLockPlayer;
	if ( GetPlayerInfo(kMemberGuid, true, kLockPlayer) )
	{
		CONT_SERVER_HASH kCont;
		g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kCont);
		
		CONT_SERVER_HASH::const_iterator server_itor = kCont.begin();
		while(server_itor != kCont.end())
		{
			SERVER_IDENTITY const &kSI = (*server_itor).first;
			if(kLockPlayer.pkCenterPlayerData->kSwitchServer.nChannel == kSI.nChannel)
			{
//				return SendToServer( (*itor).second->kSwitchServer, rkPacket );
				BM::Stream kWrappedPacket(PT_N_T_WRAPPED_TO_SWITCH);
				kLockPlayer.pkCenterPlayerData->kSwitchServer.WriteToPacket(kWrappedPacket);
				kWrappedPacket.Push(kMemberGuid);
				kWrappedPacket.Push(rkPacket);

				return SendToServer( kSI, kWrappedPacket );
			}
			++server_itor;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
