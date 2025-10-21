#include "stdafx.h"
#include "PgMMCManager.h"
#include "PgRecvFromSMC.h"
#include "PgMMCTask.h"
#include "PgFileCopyMgr.h"

void CALLBACK OnAcceptFromSMC(CEL::CSession_Base *pkSession)
{
	std::vector<char> kEncodeKey;//여기는 <중앙> 서브로 암호키(PT_A_ENCRYPT_KEY)를 보내자
	if( S_OK == pkSession->VGetEncodeKey(kEncodeKey) )
	{
		INFO_LOG( BM::LOG_LV6, L"Connect to SMC [" << pkSession->Addr().ToString() << L"]" );
		
		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
		return;
	}
	else
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L" Get EncryptKey Failed. Session Terminate : " << pkSession->Addr().ToString() );
	}

	pkSession->VTerminate();
}

void CALLBACK OnDisConnectToSMC(CEL::CSession_Base *pkSession)
{
	SERVER_IDENTITY_SITE kRecvSI;
	kRecvSI.ReadFromPacket(pkSession->m_kSessionData);
	g_kMMCMgr.Locked_SetConnectSMC( pkSession, kRecvSI, false );
	g_kLogCopyMgr.Locked_Disconnect( pkSession->SessionKey() );
}

void Recv_PT_A_S_REQ_GREETING(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	SERVER_IDENTITY_SITE kRecvSI;
	kRecvSI.SERVER_IDENTITY::ReadFromPacket(*pkPacket);

	std::wstring wstrVersion;
	pkPacket->Pop(wstrVersion);

	//Packet Version 체크 필요 없음
	/*
	if( wstrVersion != PACKET_VERSION )
	{
	INFO_LOG(BM::LOG_LV5, _T("Incorrect Packet Version[%s]"), wstrVersion.c_str());
	return;
	}
	*/

	if ( true == g_kMMCMgr.Locked_SetConnectSMC( pkSession, kRecvSI, true ) )
	{
		//bool bSendData = false;
		//pkPacket->Pop(bSendData);
		kRecvSI.WriteToPacket(pkSession->m_kSessionData);//이 세션의 SERVER_IDENTITY 기록

		SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();

		BM::Stream kSendPacket(PT_A_S_ANS_GREETING);
		kSendSI.WriteToPacket(kSendPacket);

		CON_SYNC_PATH kContPath;
		CONT_SERVER_HASH kServerHash;
		g_kMMCMgr.Locked_GetSyncPath(kContPath);
		g_kMMCMgr.Locked_GetServerHash( static_cast<int>(kRecvSI.nSiteNo), kServerHash );

		kSendPacket.Push(g_kProcessCfg.RunMode());//자신의 실행 모드를 전송.

		PU::TWriteTable_MM(kSendPacket, kContPath);
		PU::TWriteTable_MM(kSendPacket, kServerHash);
		kSendPacket.Push(g_kMMCMgr.UseSMCAutoPatch());
		kSendPacket.Push(g_kMMCMgr.UseSMCDataSync());
		kSendPacket.Push(g_kMMCMgr.ForceDataPath());

		pkSession->VSend(kSendPacket);

		g_kLogCopyMgr.Locked_Connect( pkSession->SessionKey() );
	}
	else
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Add Server Session Failed Realm[") <<  kRecvSI.nRealm << _T("] Chn[") 
			<< kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("] ") );
		pkSession->VTerminate();//세션 끊음.
	}
}

void CALLBACK OnRecvFromSMC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	switch ( usType )
	{
	case PT_A_S_REQ_GREETING:
		{
			Recv_PT_A_S_REQ_GREETING( pkSession, pkPacket );
		}break;
	case PT_SMC_MMC_REQ_DATA_SYNC_INFO:
		{
			g_kMMCMgr.Locked_Recv_PT_SMC_MMC_REQ_DATA_SYNC_INFO( pkSession, pkPacket );
		}break;
	case PT_SMC_MMC_ANS_DATA_SYNC_END:
		{
			bool bSMC_Sync = false;
			pkPacket->Pop(bSMC_Sync);

			if ( !bSMC_Sync )
			{
				g_kMMCMgr.Locked_SetSycnEndSMC( pkSession );
			}
		}break;
	case PT_SMC_MMC_REFRESH_STATE:
		{
			g_kMMCMgr.Locked_Recv_PT_SMC_MMC_REFRESH_STATE( pkSession, pkPacket );
		}break;
	case PT_SMC_MMC_ANS_CMD:
		{
			g_kMMCMgr.Locked_Recv_PT_SMC_MMC_ANS_CMD( pkSession, pkPacket );
		}break;

	//! 파일 관련 처리는 모두 Task에서 처리함.
	case PT_SMC_MMC_REQ_GET_FILE:
	case PT_MMC_SMC_ANS_GET_FILE:
	case PT_SMC_MMC_ANS_LOG_FILE_INFO:
		{
			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
			kEventMsg.SecType(usType);

			kEventMsg.Push( pkSession->SessionKey() );
			kEventMsg.Push( *pkPacket );
			g_kTask.PutMsg(kEventMsg);
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << L"Unknown Packet Type = " << usType );
		}break;
	}
}
