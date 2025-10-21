#pragma once

#include "DummyClient.h"

class CDummyClientMgr
{
public:
	CDummyClientMgr(void);
	virtual ~CDummyClientMgr(void);

public:
	void CALLBACK DummyProc();

	void OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromLogin(CEL::CSession_Base *pSession);

	void OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive);
	void OnDisconnectFromSwitch(CEL::CSession_Base *pSession);

	void CALLBACK OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket);
	void CALLBACK OnRecvFromSwitch(CEL::CSession_Base *pSession, BM::Stream * const pPacket);

	void CloseAll(bool bRemove);
	void Begin(const std::wstring &strID, const std::wstring &strPW);
	bool Build();

	void InfoLog_Statistic() const;

	typedef std::map<E_AI_Pattern, int> CONT_AI_PATTERN_CLIENT;
protected:

	bool AddNewClient(const std::wstring &strID, const std::wstring &strPW, E_AI_Pattern const eType);
	void RemoveClient(CDummyClient* pkDummy);
	void _RemoveClient(CDummyClient* pkDummy);
	
protected:
	typedef std::map< CEL::SESSION_KEY, CDummyClient* > CONT_CLIENT;
	typedef std::map< std::wstring, CDummyClient* > CONT_CLIENT_BY_ID;//ID 로 찾기.
	typedef std::map< BM::GUID, CDummyClient* > CONT_CLIENT_BY_GUID;//MemberGuid로
	
	CLASS_DECLARATION_S(bool, Started);

	CONT_CLIENT_BY_ID m_kContClientTotal;//모든 세션
	CONT_CLIENT m_kContClientLogin;//로그인에 엮인 세션
	CONT_CLIENT m_kContClientSwitch;//스위치에 엮인 세션
	CONT_CLIENT_BY_GUID m_kContClientRemoveReady;	// Session 끊어지면 지워져야 할 Client
	CONT_AI_PATTERN_CLIENT m_kContAiPatternClientDef;
	CLASS_DECLARATION_S(std::wstring, IniFile);

	mutable ACE_RW_Thread_Mutex m_kMutex;
};

#define g_kDummyMgr SINGLETON_STATIC(CDummyClientMgr)