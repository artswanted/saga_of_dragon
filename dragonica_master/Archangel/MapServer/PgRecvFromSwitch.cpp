#include "stdafx.h"
#include "PgRecvFromSwitch.h"
#include "PgRecvFromUser.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGroundMgr.h"

void CALLBACK OnAcceptFromSwitch( CEL::CSession_Base *pkSession )
{	//!  세션락
//	static int i = 0;
//	std::cout<< "누군가 접속. 암호화 키를 보내주시오." << pkSession <<" ["<< i++ << "]"<<std::endl;
}

void CALLBACK OnConnectToSwitch( CEL::CSession_Base *pkSession )
{	//!  세션락
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL");
		return;
	}

	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		g_kProcessCfg.Locked_OnConnectServer(pkSession);
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Connect Success ["<<C2L(pkSession->Addr())<<L"]");
	}
	else
	{
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
//		INFO_LOG( BM::LOG_LV1, _T("[ConnectToSwitch] Failed [%s]"), pkSession->Addr().ToString().c_str() );
		std::wcout << L"[ConnectToSwitch] Failed : " << pkSession->Addr().ToString().c_str() << std::endl;
	}
}

void CALLBACK OnDisConnectToSwitch( CEL::CSession_Base *pkSession )
{//!  세션락
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Session is NULL");
		return;
	}

	SERVER_IDENTITY kSI;
	g_kProcessCfg.Locked_OnDisconnectServer( pkSession, &kSI );

	INFO_LOG( BM::LOG_LV1, __FL__<<L"Disconnected["<<C2L(pkSession->Addr())<<L"]" );

	g_kGndMgr.ProcessRemoveUser( kSI );

	//g_kProcessCfg.Locked_ConnectSwitch();
}

void CALLBACK OnRecvFromSwitch(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	WORD wkType = 0;
	pkPacket->Pop(wkType);

	switch( wkType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{//	
			INFO_LOG( BM::LOG_LV7, __FL__<<L"Answer From Switch" );
			
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
			{
				kRecvSI.WriteToPacket(pkSession->m_kSessionData);//이 세션의 SERVER_IDENTITY 기록
				return;
			}
			
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Add Server Session Failed Realm["<<kRecvSI.nRealm<<L"] Chn["<<kRecvSI.nChannel<<L"] No["<<kRecvSI.nServerNo<<L"] Type["<<kRecvSI.nServerType<<L"] ");
		}break;
	case PT_S_M_WRAPPED_USER_PACKET:
		{
			SGroundKey kGroundKey;
			BM::GUID kCharacterGuid;
			kGroundKey.ReadFromPacket(*pkPacket);
			pkPacket->Pop( kCharacterGuid );
			OnRecvFromUser( kGroundKey, kCharacterGuid, pkPacket );
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << wkType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}
}
