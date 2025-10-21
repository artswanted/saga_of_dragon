#pragma once
#include "DummyClient2.h"
#include <map>

typedef std::map<CEL::SESSION_KEY, DummyClient2*> CONT_DUMMY_CLIENT;
class DummyMgr2
{
public:
   	void OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromLogin(CEL::CSession_Base *pSession);

	void OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromSwitch(CEL::CSession_Base *pSession);

	void CALLBACK OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket);
	void CALLBACK OnRecvFromSwitch(CEL::CSession_Base *pSession, BM::Stream * const pPacket);

	void AddUser(std::wstring kID, std::wstring kPW);
private:
	CLASS_DECLARATION_S(CEL::SESSION_KEY, LoginSessionKey);//로그인 서버 세션 ID
	CLASS_DECLARATION_S(CEL::SESSION_KEY, SwitchSessionKey);//스위치 서버 세션 ID

	CONT_DUMMY_CLIENT m_kClients;
	CONT_DUMMY_CLIENT m_kSwitchClients;
    mutable Loki::Mutex m_kMutex;
};

#define g_kDummyMgr2 SINGLETON_STATIC(DummyMgr2)