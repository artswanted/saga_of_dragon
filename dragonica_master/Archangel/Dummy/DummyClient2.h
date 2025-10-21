#pragma once
#include "Variant/PgPlayer.h"
#include "BM/ClassSupport.h"

class DummyClient2
{
public:

   	void OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromLogin(CEL::CSession_Base *pSession);

	void OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromSwitch(CEL::CSession_Base *pSession);

	void CALLBACK OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket);
	void CALLBACK OnRecvFromSwitch(CEL::CSession_Base *pSession, BM::Stream * const pPacket);

	bool SendToLogin(const BM::Stream &rkPacket)const;
	bool SendToSwitch(const BM::Stream &rkPacket)const;
private:
    CLASS_DECLARATION_S(std::wstring, ID);
    CLASS_DECLARATION_S(std::wstring, PW);

	CLASS_DECLARATION_S(BM::GUID, Guid);//개체 ID
	CLASS_DECLARATION_S(BM::GUID, SessionGuid);//세션 ID

	CLASS_DECLARATION_S(CEL::SESSION_KEY, LoginSessionKey);//로그인 서버 세션 ID
	CLASS_DECLARATION_S(CEL::SESSION_KEY, SwitchSessionKey);//스위치 서버 세션 ID
	CLASS_DECLARATION_S(BM::GUID, SwitchKey);//스위치 서버 Key-> 이거 틀리면 접속 거부

    mutable Loki::Mutex m_kMutex;
};
