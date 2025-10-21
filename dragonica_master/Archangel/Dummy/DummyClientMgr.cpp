#include "StdAfx.h"
// #include "ai_Action.h"
#include "DummyClientMgr.h"
#include "Network.h"
#include "VirtualWorld.h"


CDummyClientMgr::CDummyClientMgr(void)
{
	Started(false);
}

CDummyClientMgr::~CDummyClientMgr(void)
{
}

void CALLBACK CDummyClientMgr::DummyProc()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	static std::wstring wstrLastName = _T("");
	DWORD const dwNow = ::timeGetTime();
	CONT_CLIENT_BY_ID::iterator client_itor = m_kContClientTotal.find(wstrLastName);
	int iCount = 0;
	if (client_itor == m_kContClientTotal.end())
	{
		client_itor = m_kContClientTotal.begin();
	}
	while(iCount++ < 79 && client_itor != m_kContClientTotal.end())
	{
		CDummyClient* pkDummy = client_itor->second;
		if (dwNow < pkDummy->NextTickTime())
		{
			// 좀더 기다려야 한다.
			++client_itor;
			continue;
		}

		E_AI_Pattern const ePattern = pkDummy->AiPattern();
		CAiActionDef* const pkAction = g_kAiActionMgr.GetAction(ePattern, pkDummy->ActionStep());
		if (pkDummy->ActionStep() != pkDummy->BeforeActionStep())
		{
			pkAction->OnBegin(pkDummy);
		}
		EDoAction_Return eActionReturn = pkAction->DoAction(pkDummy);
		if (pkDummy->ActionStep() != pkDummy->BeforeActionStep())
		{
			pkDummy->BeforeActionStep(pkDummy->ActionStep());
			pkDummy->ActionBeginTime(dwNow);
		}
		switch (eActionReturn)
		{
		case E_DoAction_Finished:
			{
				// Action finished
				pkDummy->NextTickTime(dwNow + pkAction->GetDelayTime() + rand()%50);
				pkDummy->ActionStep(pkDummy->ActionStep()+1);
				pkAction->OnEnd(pkDummy);
				_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"E_DoAction_Finished, Member:"<< pkDummy->MemberID()<<L", Action:"<<pkAction->Print());
			}break;
		case E_DoAction_EndGame:
			{
				pkDummy->NextTickTime(dwNow + pkAction->GetDelayTime());
				RemoveClient(pkDummy);
				_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"E_DoAction_EndGame, Member:"<<pkDummy->MemberID()<<L", Action:"<<pkAction->Print());
			}break;
		default :
			{
				if (eActionReturn >= E_DoAction_GoFirst)
				{
					pkDummy->NextTickTime(dwNow + pkAction->GetDelayTime());
					pkDummy->ActionStep(eActionReturn - E_DoAction_GoFirst);
					_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"E_DoAction_GoFirst, Member:"<<pkDummy->MemberID()<<L", Action:"<<pkAction->Print());
				}
			}break;
		}
		++client_itor;
		
		/*
		if ( !(*client_itor).second->ProcessAI() )
		{
			delete client_itor->second;
			client_itor = m_kContClientTotal.erase(client_itor);
		}
		else
		{
			++client_itor;
		}
		*/
	}
	if ( client_itor != m_kContClientTotal.end() )
	{
		wstrLastName = client_itor->first;
	}
	else
	{
		wstrLastName = _T("");
	}
}


bool CDummyClientMgr::AddNewClient(const std::wstring &strID, const std::wstring &strPW, E_AI_Pattern const eType )
{
	_DETAIL_LOG INFO_LOG( BM::LOG_LV1, __FL__<<L"Try - AddNewClient");

	//BM::CAutoMutex kLock(m_kMutex, true);

	CDummyClient *pDummy = new CDummyClient;

	if(pDummy)
	{
		BM::GUID kGuid;
		kGuid.Generate();
		
		pDummy->Guid(kGuid);
		pDummy->SetAuth(strID, strPW);
		pDummy->AiPattern(eType);
		pDummy->SetID(kGuid);

		CEL::SESSION_KEY kKey(g_kNetwork.LoginConnector(), BM::GUID::NullData());
		pDummy->LoginSessionKey(kKey);
	
		auto ret = m_kContClientTotal.insert(std::make_pair(pDummy->MemberID(), pDummy));
		if(ret.second)
		{
			_DETAIL_LOG INFO_LOG( BM::LOG_LV1, __FL__<<L"Try - AddNewClient Success");
			return true;
		}

		SAFE_DELETE(pDummy);
	}
	
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"Try - AddNewClient Failed");
	assert(NULL);
	return false;
}

void CDummyClientMgr::CloseAll(bool bRemove)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	INFO_LOG(BM::LOG_LV7, __FL__<<L"Remove Dummy");
	CONT_CLIENT::iterator switch_itor = m_kContClientSwitch.begin();
	while(switch_itor != m_kContClientSwitch.end())
	{
		CDummyClient* pClient = (*switch_itor).second;

		if(pClient)
		{
			pClient->Terminate();
		}

		++switch_itor;
	}
	m_kContClientSwitch.clear();

	CONT_CLIENT::iterator login_itor = m_kContClientLogin.begin();
	while(login_itor != m_kContClientLogin.end())
	{
		CDummyClient* pClient = (*login_itor).second;

		if(pClient)
		{
			pClient->Terminate();
		}

		++login_itor;
	}
	m_kContClientLogin.clear();

	if (bRemove)
	{
		CONT_CLIENT_BY_ID::const_iterator itor_total = m_kContClientTotal.begin();
		while (itor_total != m_kContClientTotal.end())
		{
			delete itor_total->second;
			
			++itor_total;
		}
		m_kContClientTotal.clear();
	}

	Started(false);
}


void CDummyClientMgr::OnConnectFromLogin(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{
	BM::CAutoMutex kLock(m_kMutex, true);

//	CAUTION_LOG(BM::LOG_LV1, _T("[%s]-[%d] Connect [%s]"), __FUNCTIONW__, __LINE__ , rkGuid.SessionGuid().str().c_str());

	CONT_CLIENT_BY_ID::iterator client_itor = m_kContClientTotal.begin();
	while(client_itor != m_kContClientTotal.end())
	{
		CDummyClient *pkElement = (*client_itor).second;
		if(pkElement->LoginSessionKey() == rkGuid)
		{
			if(bIsAlive)
			{
				auto ret = m_kContClientLogin.insert(std::make_pair(pkElement->LoginSessionKey(), pkElement));
				if(!ret.second)
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Session Insert Failed");
					assert(NULL);
					return;
				}
//				CAUTION_LOG(BM::LOG_LV1, _T("[%s]-[%d] Connect Insert Success[%s]"), __FUNCTIONW__, __LINE__ , rkGuid.SessionGuid(). str().c_str());
			}

			pkElement->OnConnectFromLogin(bIsAlive);
		}
		++client_itor;
	}
}

void CDummyClientMgr::OnDisconnectFromLogin(CEL::CSession_Base *pSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);//디스커넥이 LoginAuth 보다 빨리콜 될 경우. 더미 세션이 사라짐
	
	const CEL::SESSION_KEY &kSessionKey = pSession->SessionKey();

	CONT_CLIENT::iterator total_itor = m_kContClientLogin.find(kSessionKey);

//	CAUTION_LOG(BM::LOG_LV1, _T("[%s]-[%d] Disconnect [%s]"), __FUNCTIONW__, __LINE__ , kSessionKey.SessionGuid().str().c_str());

	if(m_kContClientLogin.end() != total_itor)
	{
		CDummyClient *pkElement = (*total_itor).second;

		EAiActionType const eActionType = g_kAiActionMgr.GetActionType(pkElement->AiPattern(), pkElement->ActionStep());
		if (pkElement->SwitchSessionKey().SessionGuid() != BM::GUID::NullData())
		{
			// 이미 스위치 접속 상태이거나 시도한 상태
			CEL::SESSION_KEY kKey(g_kNetwork.LoginConnector(), BM::GUID::NullData());
			pkElement->LoginSessionKey(kKey);
		}
		else if (eActionType == EAiAction_Logout)
		{
			pkElement->AiActionTypeSecond(NS_LOGOUT_DISCONNECTED);
		}
		else if (pkElement->AiPattern() == E_AiPattern_Repeat_Login || eActionType == EAiAction_Login)
		{
			pkElement->AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
			pkElement->ActionStep(0);
			pkElement->OnConnectFromLogin(false);
		}

		m_kContClientLogin.erase(kSessionKey);
		CONT_CLIENT_BY_GUID::iterator itor_remove = m_kContClientRemoveReady.find(pkElement->Guid());
		if (itor_remove != m_kContClientRemoveReady.end())
		{
			_RemoveClient(pkElement);
			m_kContClientRemoveReady.erase(itor_remove);
		}
	}
	else
	{//open 실패할경우 들어오기도함.
		CAUTION_LOG( BM::LOG_LV1, __FL__<<L"Can't Find In TotalCont");
//		assert(NULL);
	}

}

void CALLBACK CDummyClientMgr::OnRecvFromLogin(CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_CLIENT::iterator client_itor = m_kContClientLogin.find(pSession->SessionKey());
	if(client_itor != m_kContClientLogin.end())
	{
		CDummyClient *pkElement = (*client_itor).second;
		pkElement->OnRecvFromLogin(pSession, pPacket);
	}
	else
	{
		assert(NULL);
	}
}

void CDummyClientMgr::OnConnectFromSwitch(const CEL::SESSION_KEY &rkGuid, const bool bIsAlive)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	CONT_CLIENT_BY_ID::iterator client_itor = m_kContClientTotal.begin();
	while(client_itor != m_kContClientTotal.end())
	{
		CDummyClient *pkElement = (*client_itor).second;
		if(pkElement->SwitchSessionKey() == rkGuid)
		{
			if(bIsAlive)
			{
				auto ret = m_kContClientSwitch.insert(std::make_pair(pkElement->SwitchSessionKey(), pkElement));
				if(!ret.second)
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Session Insert Failed");
					assert(NULL);
					return;
				}
			}

			pkElement->OnConnectFromSwitch(bIsAlive);
		}
		++client_itor;
	}
}

void CDummyClientMgr::OnDisconnectFromSwitch(CEL::CSession_Base *pSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	
	const CEL::SESSION_KEY &kSessionKey = pSession->SessionKey();

	CONT_CLIENT::iterator switch_itor = m_kContClientSwitch.find(kSessionKey);
	if(m_kContClientSwitch.end() != switch_itor)
	{
		CDummyClient *pkElement = (*switch_itor).second;

		m_kContClientSwitch.erase(kSessionKey);
		EAiActionType const eActionType = g_kAiActionMgr.GetActionType(pkElement->AiPattern(), pkElement->ActionStep());
		if (eActionType == EAiAction_Logout)
		{
			pkElement->AiActionTypeSecond(NS_LOGOUT_DISCONNECTED);
		}
		else if (pkElement->AiPattern() == E_AiPattern_Repeat_Login || eActionType == EAiAction_Login)
		{
			pkElement->AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
			pkElement->ActionStep(0);
			pkElement->OnConnectFromSwitch(false);
		}

		CONT_CLIENT_BY_GUID::iterator itor_remove = m_kContClientRemoveReady.find(pkElement->Guid());
		if (itor_remove != m_kContClientRemoveReady.end())
		{
			_RemoveClient(pkElement);
			m_kContClientRemoveReady.erase(itor_remove);
		}
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__<<L"Can't Find In TotalCont");
//		assert(NULL);
	}
}

void CALLBACK CDummyClientMgr::OnRecvFromSwitch(CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_CLIENT::iterator client_itor = m_kContClientSwitch.find(pSession->SessionKey());
	if(client_itor != m_kContClientSwitch.end())
	{
		CDummyClient *pkElement = (*client_itor).second;
		pkElement->OnRecvFromSwitch(pSession, pPacket);
	}
	else
	{
		assert(NULL);
	}
}

void CDummyClientMgr::Begin(const std::wstring &strID, const std::wstring &strPW)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	INFO_LOG(BM::LOG_LV7, __FL__<<L"STARTING....");
	if(Started())
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Already started");
		return;
	}

	Started(true);


	INFO_LOG(BM::LOG_LV7, __FL__<<L"Add Dummy");
	int iIndex = g_kNetwork.GetTestInfo()->sBeginIndex;
	CONT_AI_PATTERN_CLIENT::const_iterator itor_pattern_def = m_kContAiPatternClientDef.begin();
	while (itor_pattern_def != m_kContAiPatternClientDef.end())
	{
		int const iMax = itor_pattern_def->second;
		for (int i=0; i<iMax; i++)
		{
			BM::vstring kID(strID);
			kID += iIndex++;
			AddNewClient(kID, strPW, itor_pattern_def->first);
			//Sleep(50 + rand()%50);
		}

		++itor_pattern_def;
	}
}

bool CDummyClientMgr::Build()
{
	BM::CAutoMutex kLock(m_kMutex);
	
	std::wstring wIniFile = GetMainDirectory();
	wIniFile += _T("\\Dummy.ini");

	TCHAR chAIDefine[1024];
	if (GetPrivateProfileString(_T("AI_PATTERN"), NULL, NULL, chAIDefine, 1024, wIniFile.c_str()) == 0)
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Cannot read AI_PATTERN in Dummy.ini");
		return false;
	}
	LPTSTR lpszFind = chAIDefine;

	typedef std::map<std::wstring, E_AI_Pattern> CONT_ACTION_NAMETYPE;
	CONT_ACTION_NAMETYPE kActionDef;
	kActionDef.insert(std::make_pair(_T("REPEAT_LOGIN"), E_AiPattern_Repeat_Login));
	kActionDef.insert(std::make_pair(_T("REPEAT_NETGIVEITEM"), E_AiPattern_Repeat_NetGiveItem));
	kActionDef.insert(std::make_pair(_T("REPEAT_NETGIVEREMOVEITEM"), E_AiPattern_Repeat_NetGiveRemoveItem));
	kActionDef.insert(std::make_pair(_T("REPEAT_MOVEITEM"), E_AiPattern_Repeat_MoveItem));
	kActionDef.insert(std::make_pair(_T("RANDOM_MAPMOVE_ITEM"), E_AiPattern_Random_MapMoveItem));
	kActionDef.insert(std::make_pair(_T("REPEAT_MAPMOVE"), E_AiPattern_Repeat_MapMove));
	kActionDef.insert(std::make_pair(_T("REPEAT_ECHO_CHAT"), E_AiPattern_Echo_Chat));
	kActionDef.insert(std::make_pair(_T("REPEAT_PSEUDOACTION"), E_AiPattern_Repeat_PseudoAction));
	kActionDef.insert(std::make_pair(_T("REPEAT_MISSIONMOVE"), E_AiPattern_Repeat_MissionMove));
	kActionDef.insert(std::make_pair(_T("REPEAT_PARTY_CREATE_LEAVE"), E_AiPattern_Repeat_PartyCreateLeave));
	kActionDef.insert(std::make_pair(_T("REPEAT_PLAYER_ITEM_TRADE"), E_AiPattern_Repeat_Player_Item_Trade));
	kActionDef.insert(std::make_pair(_T("REPEAT_PVPMOVE"), E_AiPattern_Repeat_PvPMove));

	kActionDef.insert(std::make_pair(_T("REPEAT_OPENMARKET_CREATE"), E_AiPattern_Repeat_OpenMarket_Create));
	kActionDef.insert(std::make_pair(_T("REPEAT_OPENMARKET_DEALING"), E_AiPattern_Repeat_OpenMarket_Dealing));

	while (lpszFind[0] != _T('\0'))
	{
		CONT_ACTION_NAMETYPE::const_iterator itor = kActionDef.find(lpszFind);
		if (itor != kActionDef.end())
		{
			int iValue = GetPrivateProfileInt(_T("AI_PATTERN"), lpszFind, 0, wIniFile.c_str());
			m_kContAiPatternClientDef.insert(std::make_pair(itor->second, iValue));
		}

		size_t iLength = _tcslen(lpszFind);
		lpszFind += (iLength+1);
	}
	return true;
}

void CDummyClientMgr::RemoveClient(CDummyClient* pkDummy)
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kContClientRemoveReady.insert(std::make_pair(pkDummy->Guid(), pkDummy));
	pkDummy->Terminate();
}

void CDummyClientMgr::_RemoveClient(CDummyClient* pkDummy)
{
	BM::CAutoMutex kLock(m_kMutex);

	m_kContClientLogin.erase(pkDummy->LoginSessionKey());
	m_kContClientSwitch.erase(pkDummy->SwitchSessionKey());
	m_kContClientTotal.erase(pkDummy->MemberID());

	delete pkDummy;
	pkDummy = NULL;
}

void CDummyClientMgr::InfoLog_Statistic() const
{
	BM::CAutoMutex kLock(m_kMutex);
	INFO_LOG(BM::LOG_LV6, _T(""));
	INFO_LOG(BM::LOG_LV6, _T("========================================="));
	INFO_LOG(BM::LOG_LV6, _T("=============== STATISTIC ==============="));
	g_kAiActionMgr.InfoLog_Statistic();

	CONT_CLIENT_BY_ID::const_iterator client_itor = m_kContClientTotal.begin();
	
	DWORD const dwNow = ::timeGetTime();
	typedef std::map<CAiActionDef*, int> CONT_AI_Pattern_Timeover;
	CONT_AI_Pattern_Timeover kTimeover;
	while(client_itor != m_kContClientTotal.end())
	{
		CDummyClient* pkDummy = client_itor->second;

		E_AI_Pattern const ePattern = pkDummy->AiPattern();
		CAiActionDef* const pkAction = g_kAiActionMgr.GetAction(ePattern, pkDummy->ActionStep());
		if (pkAction != NULL)
		{
			if ((pkDummy->ActionBeginTime() > 0) && (dwNow > pkDummy->ActionBeginTime() + pkAction->GetTimeover()))
			{
				CONT_AI_Pattern_Timeover::iterator itor_timeover = kTimeover.find(pkAction);
				if (itor_timeover != kTimeover.end())
				{
					itor_timeover->second++;
				}
				else
				{
					kTimeover.insert(std::make_pair(pkAction, 1));
				}
			}
		}
		++client_itor;
	}
	CONT_AI_Pattern_Timeover::const_iterator timeover_itor = kTimeover.begin();
	while (timeover_itor != kTimeover.end())
	{
		INFO_LOG(BM::LOG_LV9, __FL__<<L"TIMEOVER Action["<<timeover_itor->first->Print()<<L"] Count["<<timeover_itor->second<<L"]");
		++timeover_itor;
	}
}