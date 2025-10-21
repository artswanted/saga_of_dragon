#include "stdafx.h"
#include "PgRecvFromImmigration.h"
#include "Variant/PgMctrl.h"
#include "Lohengrin/PgRealmManager.h"
#include "Collins/Log.h"
#include "PgLogMgr.h"

extern void CALLBACK OnAcceptFromOtherServer( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisConnectToOtherServer(CEL::CSession_Base *pkSession);
extern void CALLBACK OnRecvFromOtherServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

bool RegistAcceptor(const TBL_SERVERLIST &rkServerInfo)
{
	CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrServerBind;
	CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrServerNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			return false;//Already In Use
		}

		CEL::INIT_CORE_DESC kInit;
		kInit.kBindAddr = rkBindAddr;
		kInit.kNATAddr = rkNATAddr;

		kInit.OnSessionOpen	= OnAcceptFromOtherServer;
		kInit.OnDisconnect	= OnDisConnectToOtherServer;
		kInit.OnRecv			= OnRecvFromOtherServer;
		kInit.bIsImmidiateActivate	= true;
		kInit.ServiceHandlerType(CEL::SHT_SERVER);
		
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Try Regist Acceptor [") << rkBindAddr.ToString().c_str() << _T("]") );

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);

//		CEL::INIT_FINAL_SIGNAL kFinalInit;
//		kFinalInit.kIdentity = 2;
//		g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kFinalInit);
		return true;
	}
	
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect Acceptor Addr [") << rkBindAddr.ToString().c_str() << _T("]") );
	return false;
}

void CALLBACK OnConnectToImmigration(CEL::CSession_Base *pkSession)
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if(bIsSucc)
	{	
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Connect Success [") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T(" Failed to connect to Immigration") );
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectImmigration();
	}
}

void CALLBACK OnDisConnectToImmigration(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
}

void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

#if _DEBUG
	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Recved Start[") << usType << _T("]") );
#endif

	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);
			if(SUCCEEDED(g_kProcessCfg.Locked_Read_ServerList(*pkPacket)))
			{
				if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
				{
					const SERVER_IDENTITY &kSI = g_kProcessCfg.ServerIdentity();
					TBL_SERVERLIST kServerInfo;
					if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kServerInfo))
					{
						RegistAcceptor(kServerInfo);

						//Site Log Server만 Immigration Server의 서비스 시작 로그를 기록한다.
						if(kSI.nRealm == 0)
						{
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
						} 
						else
						{
							//Imm와의 접속을 끊는다
							pkSession->VTerminate();
						}
						return;
					}
				}
			}
		}break;
	case PT_A_GAME_LOG:
		{
			PgLogCont kLogCont;
			kLogCont.ReadFromPacket(*pkPacket);
			BM::GUID const & rkGuid = (false == kLogCont.MemberKey().IsNull()) ? kLogCont.MemberKey() : pkSession->SessionKey().SessionGuid();
			g_kLogMgr.Locked_WriteLog(rkGuid, kLogCont);
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_A_HEARTBEAT_LOG:
		{
			short usRealmNo = 0;
			short usChannelNo = 0;
			short usServerNo = 0;
/*			
			pkPacket->Pop(usRealmNo);
			pkPacket->Pop(usChannelNo);
			pkPacket->Pop(usServerNo);
			SERVER_IDENTITY const &kMySI = g_kProcessCfg.ServerIdentity();
			if(usRealmNo == kMySI.nRealm)
			{
				PgLogWrapper kLogData(LOG_SYSTEM_HEARTBEAT, pkSession->SessionKey().WorkerGuid(), pkSession->SessionKey().SessionGuid() );
				kLogData.ChannelNo(usChannelNo);
				kLogData.Push(pkSession->Addr().ToString());
				kLogData.Push((int)usServerNo);

	//			WriteLog( kLogData );
				INFO_LOG(BM::LOG_LV6, _T("[HEARTBEAT:%s] No %d"), pkSession->SessionKey().SessionGuid().str().c_str(), usServerNo);
			}
*/
		}break;
	default:
		{
		}break;
	}

//	PgChannel * pChann = g_kRealmMgr.Get(pkSession->SessionKey());
//	if(pChann) 
//	{
//		pChann->OnRecvPacket(pkSession, pkPacket); 
//		return;
//	}
	//INFO_LOG(BM::LOG_LV0, _T("[%s]-[%d] Can't Find Channel"), __FUNCTIONW__, __LINE__);
}
