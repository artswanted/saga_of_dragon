#include "stdafx.h"
#include "PgRecvFromMap.h"
#include "PgRecvFromUser.h"
#include "Variant/PgMCtrl.h"

void CALLBACK OnAcceptFromMap( CEL::CSession_Base *pkSession )
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		//INFO_LOG(BM::LOG_LV6, _T("[%s]-[%d] Send EncryptKey"), __FUNCTIONW__, __LINE__);

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Get EncryptKey Failed. Session Terminate!") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnDisconnectFromMap( CEL::CSession_Base *pkSession )
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);//

	if(pkSession->m_kSessionData.Size())
	{
		SERVER_IDENTITY kRecvSI;
		kRecvSI.ReadFromPacket(pkSession->m_kSessionData);
		g_kHub.Locked_RemoveMember(kRecvSI);
	}
//	허브유저 클리어 필요
}

void CALLBACK OnRecvFromMap(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	//std::cout<< "Received Packet T:[" << usType <<"] Size["<< pkPacket->Size() << "]"<< std::endl;
	switch( usType )
	{
	case PT_A_S_REQ_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			std::wstring strVersion;
			pkPacket->Pop(strVersion);
			if( !CProcessConfig::IsCorrectVersion( strVersion, false ) )
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Incorrect Version[") << strVersion.c_str() << _T("]") );
				pkSession->VTerminate();
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return;
			}
			
			if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
			{
				kRecvSI.WriteToPacket(pkSession->m_kSessionData);//이 세션의 SERVER_IDENTITY 기록

				BM::Stream kSendPacket(PT_A_S_ANS_GREETING);
				SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();
				kSendSI.WriteToPacket(kSendPacket);
				pkSession->VSend(kSendPacket);
				INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Greeting Success") );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
				pkSession->VTerminate();
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return;
			}
		}break;
	case PT_M_S_WRAPPED_USER_PACKET:
		{
			CONT_GUID_LIST kTarget;
			pkPacket->Pop(kTarget);

			BM::Stream kPacket;
			pkPacket->Pop(kPacket.Data());
			kPacket.PosAdjust();

			g_kHub.Locked_SendPacketToUserFromMap( kTarget, kPacket );
		}break;
	case PT_S_S_SEND_TO_SWITCH_USER:
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			CAUTION_LOG( BM::LOG_LV1, __FL__ << _T(" recv PT_S_S_SEND_TO_SWITCH_USER packet") );
		}break;
	case PT_M_S_NFY_CHARACTER_MAP_MOVE:	// MissionMap에서 Stage 이동
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharGuid;
			SERVER_IDENTITY kSI;
			SGroundKey kGroundKey;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharGuid);
			kSI.ReadFromPacket(*pkPacket);
			kGroundKey.ReadFromPacket(*pkPacket);
			g_kHub.Locked_ChangeMemberMapServerNo(kSI, kGroundKey, kMemberGuid, kCharGuid);
		}break;
	case PT_A_S_NFY_USER_DISCONNECT:
		{
			BM::GUID kMemberGuid;
			BYTE cCause = 0;
			pkPacket->Pop( cCause );
			pkPacket->Pop( kMemberGuid );

			g_kHub.Locked_RemoveMember( static_cast<EClientDisconnectedCause>(cCause), kMemberGuid );
		}break;
	case PT_M_L_TRY_LOGIN_CHANNELMAPMOVE:
		{
			g_kHub.Locked_RegistChannelMapMoveUser(pkPacket);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}break;
	}
}
