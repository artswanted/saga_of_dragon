#include "stdafx.h"
#include "Variant/PgMCtrl.h"
#include "Collins/Log.h"
#include "PgLogMgr.h"

void CALLBACK OnAcceptFromOtherServer( CEL::CSession_Base *pkSession )
{	//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("-----> One Server[") << pkSession->Addr().ToString().c_str() << _T("] Connected") );
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Send EncryptKey To Terminal") );

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

void CALLBACK OnDisConnectToOtherServer(CEL::CSession_Base *pkSession)
{//!  세션락
	//등록된 넘인지 확인해보고 서버리스트상에서 지워준다.
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);

	if(pkSession->m_kSessionData.Size())
	{
		SERVER_IDENTITY kRecvSI;
		kRecvSI.ReadFromPacket(pkSession->m_kSessionData);
		
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("-----> One Server[") << pkSession->Addr().ToString().c_str() << _T("] Disconnected") );

		//서비스 종료 로그
		PgLogCont kLogCont(ELogMain_System_Service, ELogSub_Service);
		kLogCont.MemberKey(pkSession->SessionKey().SessionGuid());
		kLogCont.CharacterKey(pkSession->SessionKey().WorkerGuid() );
		kLogCont.RealmNo(kRecvSI.nRealm);
		kLogCont.ChannelNo(kRecvSI.nChannel);

		PgLog kLog(ELOrderMain_Service, ELOrderSub_End);
		kLog.Set(1, pkSession->Addr().ToString());
		kLog.Set(0, kRecvSI.nServerType);
		kLog.Set(1, kRecvSI.nServerNo);
		kLogCont.Add(kLog);
		g_kLogMgr.Locked_WriteLog(pkSession->SessionKey().SessionGuid(), kLogCont);
	}

}

void CALLBACK OnRecvFromOtherServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);
	int iLoginServerNo = -1;
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
				//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] Incorrect Version[%s]"), __FUNCTIONW__, __LINE__, strVersion.c_str());
				INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Incorrect Version[") << strVersion.c_str() << _T("]") );
				pkSession->VTerminate();
				return;
			}

			if(S_OK != g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
			{
				//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] Add Server Session Failed Realm[%d] Chn[%d] No[%d] Type[%d] "), __FUNCTIONW__, __LINE__, kRecvSI.nRealm, kRecvSI.nChannel, kRecvSI.nServerNo, kRecvSI.nServerType);
				INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
				pkSession->VTerminate();//세션 끊음.
				return;
			}

			// 서비스 시작 로그 남김
			PgLogCont kLogCont(ELogMain_System_Service, ELogSub_Service);
			kLogCont.MemberKey(pkSession->SessionKey().SessionGuid());
			kLogCont.CharacterKey(pkSession->SessionKey().WorkerGuid());
			kLogCont.RealmNo(kRecvSI.nRealm);
			kLogCont.ChannelNo(kRecvSI.nChannel);

			PgLog kLog(ELOrderMain_Service, ELOrderSub_Start);
			kLog.Set(1, pkSession->Addr().ToString());
			kLog.Set(0, kRecvSI.nServerType);
			kLog.Set(1, kRecvSI.nServerNo);
			kLogCont.Add(kLog);
			g_kLogMgr.Locked_WriteLog(pkSession->SessionKey().SessionGuid(), kLogCont);


			SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();
			BM::Stream kPacket(PT_A_S_ANS_GREETING);
			kSendSI.WriteToPacket(kPacket);
			pkSession->VSend(kPacket);
			if(kSendSI.nRealm > 0
				&& kSendSI.nChannel > 0)
			{
				ReadyToService(CEL::ST_CONTENTS);
			}
			else
			{
				ReadyToService(CEL::ST_IMMIGRATION);
			}			
		}break;
	case PT_A_GAME_LOG:
		{
			PgLogCont kLogCont;
			kLogCont.ReadFromPacket(*pkPacket);
			BM::GUID const & rkGuid = (false == kLogCont.MemberKey().IsNull()) ? kLogCont.MemberKey() : pkSession->SessionKey().SessionGuid();
			g_kLogMgr.Locked_WriteLog(rkGuid, kLogCont);
		}break;
	case PT_A_HEARTBEAT_LOG:
		{
			short usRealmNo = 0;
			short usChannelNo = 0;
			short usServerNo = 0;
			
			pkPacket->Pop(usRealmNo);
			pkPacket->Pop(usChannelNo);
			pkPacket->Pop(usServerNo);
/*
			PgLogWrapper kLogData(LOG_SYSTEM_HEARTBEAT, pkSession->SessionKey().WorkerGuid(), pkSession->SessionKey().SessionGuid() );
			kLogData.ChannelNo(usChannelNo);
			kLogData.Push(pkSession->Addr().ToString());
			kLogData.Push((int)usServerNo);
			WriteLog( kLogData );
*/
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("[HEARTBEAT:") << pkSession->SessionKey().SessionGuid().str().c_str() << _T("] No ") << usServerNo );
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect Packet Type[") << usType << _T("]") );
		}break;
	}
}
