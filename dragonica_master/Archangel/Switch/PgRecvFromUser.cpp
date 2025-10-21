#include "stdafx.h"
#include "PgRecvFromUser.h"

void CALLBACK OnAcceptFromUser( CEL::CSession_Base *pkSession )
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		CEL::ADDR_INFO const& rkAddr = pkSession->Addr();
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Connected user ADDR[") << rkAddr.ToString().c_str() << _T("]") );

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		g_kHub.Locked_OnDisconnectUser(pkSession);
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" Get EncryptKey Failed. Session Terminate!") );
		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Get EncryptKey Failed. Session Terminate!") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnDisconnectFromUser( CEL::CSession_Base *pkSession )
{//!  세션락
	INFO_LOG( BM::LOG_LV9, __FL__ << _T(" Disconnected User..... pkSession[") << pkSession << _T("]") );
	g_kHub.Locked_OnDisconnectUser(pkSession);
}

//유저 세션은 pkSession->pData 에다가 현재 정보를 셋팅 해 줄것이다. OnDisconnect 에서 delete 해주면 된다.
void CALLBACK OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	//유저를 찾고. 해당 유저가 hub 에 할당되어 있다면 처리.
	//유저가 할당되어 있지 않다면 패킷타입이 PT_C_S_TRY_ACCESS_SWITCH 여야함.
	g_kHub.Locked_OnRecvFromUser(pkSession, pkPacket);
}
