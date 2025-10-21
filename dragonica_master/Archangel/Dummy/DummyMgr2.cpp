#include "stdafx.h"
#include "DummyMgr2.h"
#include "Network.h"

void DummyMgr2::OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{
    m_kClients.find(rkGuid)->second->OnConnectFromLogin(rkGuid, bIsAlive);
}

void DummyMgr2::OnDisconnectFromLogin(CEL::CSession_Base *pSession)
{
}

void DummyMgr2::OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{
    CONT_DUMMY_CLIENT::const_iterator kItor = m_kClients.begin();
    while(kItor != m_kClients.end())
    {
        if (kItor->second->SwitchSessionKey() == rkGuid)
        {
            m_kSwitchClients.insert(std::make_pair(rkGuid, kItor->second));
            kItor->second->OnConnectFromSwitch(rkGuid, bIsAlive);
            return;
        }
        ++kItor;
    }
}

void DummyMgr2::OnDisconnectFromSwitch(CEL::CSession_Base *pSession)
{

}

void CALLBACK DummyMgr2::OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
    m_kClients.find(pSession->SessionKey())->second->OnRecvFromLogin(pSession, pPacket);
}

void CALLBACK DummyMgr2::OnRecvFromSwitch(CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
    m_kSwitchClients.find(pSession->SessionKey())->second->OnRecvFromSwitch(pSession, pPacket);
}

void DummyMgr2::AddUser(std::wstring kID, std::wstring kPW)
{
    INFO_LOG(BM::LOG_LV6, "Add new user [" << kID << "]");
    DummyClient2* pkDummyClient = new DummyClient2;
    if (!pkDummyClient)
    {
        INFO_LOG(BM::LOG_LV6, "Failed add user[" << kID << "] Can't allocate mem");
        return;
    }

    pkDummyClient->ID(kID);
    pkDummyClient->PW(kPW);

	CEL::SESSION_KEY kSessionKey;
    kSessionKey.WorkerGuid(g_kNetwork.LoginConnector());
	bool bReturn = g_kNetwork.DoConnectLogin(kSessionKey);
	pkDummyClient->LoginSessionKey(kSessionKey);
    m_kClients.insert(std::make_pair(kSessionKey, pkDummyClient));
}