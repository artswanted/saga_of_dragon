#include "stdafx.h"
#include "Global.h"
#include "ai_Action.h"
#include "DummyClient.h"
#include "Network.h"
#include "DummyClientMgr.h"

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_Login
/////////////////////////////////////////////////////////////////////////////////////////

CAiActionDef::CAiActionDef()
{
	m_kStatistic.Init();
}

std::wstring CAiActionDef::GetDelayTimeString() const
{
	TCHAR chValue[20];
	_stprintf_s(chValue, 20, _T("%d"), m_dwDelayTime);
	return std::wstring(chValue);
}

EDoAction_Return CAiActionDef::DoAction(CDummyClient* pkDummy)
{
	return E_DoAction_SUCCESS;
}

void CAiActionDef::OnBegin(CDummyClient* pkDummy)
{
}

void CAiActionDef::OnEnd(CDummyClient* pkDummy)
{
	if (m_kStatistic.pkSampleClient == NULL)
	{
		// Test Begin...
		m_kStatistic.pkSampleClient = pkDummy;
		m_kStatistic.dwCallTotalCount = 0;
		m_kStatistic.dwBeginTime = ::timeGetTime();
	}

	if (m_kStatistic.pkSampleClient == pkDummy)
	{
		DWORD const dwNow = ::timeGetTime();
		if (m_kStatistic.dwLastCallTime != 0)
		{
			if (m_kStatistic.fAvarageCallInterval != 0.0)
			{
				m_kStatistic.fAvarageCallInterval = (m_kStatistic.fAvarageCallInterval + (dwNow - m_kStatistic.dwLastCallTime)) / 2.0f;
			}
			else
			{
				m_kStatistic.fAvarageCallInterval = dwNow - m_kStatistic.dwLastCallTime;
			}
		}
		m_kStatistic.dwLastCallTime = dwNow;
	}
	++m_kStatistic.dwCallTotalCount;
}

void CAiActionDef::InfoLog_Statistic(bool const bClearInfo) const
{
	DWORD const dwNow = ::timeGetTime();
	INFO_LOG(BM::LOG_LV9, _T(""));
	INFO_LOG(BM::LOG_LV9, _T("STATICTIC INFO [")<<Print()<<L"]");
	INFO_LOG(BM::LOG_LV9, _T("============================================="));
	if (m_kStatistic.dwBeginTime == 0)
	{
		INFO_LOG(BM::LOG_LV9, _T("NO INFORMATION"));
	}
	else
	{
		DWORD dwElpased = dwNow-m_kStatistic.dwBeginTime;
		INFO_LOG(BM::LOG_LV9, _T("CallCount : ")<<m_kStatistic.dwCallTotalCount<<L" / "<<dwElpased<<L" ms = "<<(m_kStatistic.dwCallTotalCount*1000.0f/dwElpased)<<L"/sec");
		INFO_LOG(BM::LOG_LV9, _T("Caller Interval : ")<<m_kStatistic.fAvarageCallInterval<<L" ms");
	}
	if (bClearInfo)
	{
		m_kStatistic.Init();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_Login
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_Login::CAiAction_Login(LPCTSTR lpszValue)
{
	_stscanf_s(lpszValue, _T("%d %d"), &m_dwDelayTime, &m_dwTimeoverLimit);
	m_eType = EAiAction_Login;
}

CAiAction_Login::~CAiAction_Login()
{
}

EDoAction_Return CAiAction_Login::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	switch (pkDummy->AiActionTypeSecond())
	{
	case NS_BEFORE_LOGIN_CONNECT:
		{// 로긴전
			TryConnectLogin(true, pkDummy);
		}break;
	case NS_IN_GAME:
		{
			DWORD dwNow = ::timeGetTime();
			if ((pkDummy->ActionBeginTime() > 0) && (dwNow > pkDummy->ActionBeginTime()+m_dwTimeoverLimit))
			{
				INFO_LOG(BM::LOG_LV5, __FL__<<L"Timeover user ElapsedTime["<<(dwNow - pkDummy->ActionBeginTime())<<L"]");
			}
			return E_DoAction_Finished;
		}break;
	}
	return E_DoAction_SUCCESS;
}

bool CAiAction_Login::TryConnectLogin(bool const bIsTryAuth, CDummyClient* pkDummy) const
{
	if(bIsTryAuth)
	{
		pkDummy->AiActionTypeSecond(NS_LOGIN_CONNECT_TRY_FOR_AUTH);//
	}
	else
	{
		pkDummy->AiActionTypeSecond(NS_LOGIN_CONNECT_TRY_FOR_LOGIN);//
	}
	CEL::SESSION_KEY kKey(g_kNetwork.SwitchConnector(), BM::GUID::NullData());
	pkDummy->SwitchSessionKey(kKey);
	CEL::SESSION_KEY kSessionKey = pkDummy->LoginSessionKey();
	bool bReturn = g_kNetwork.DoConnectLogin(kSessionKey);
	pkDummy->LoginSessionKey(kSessionKey);
	return bReturn;
}


/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_Logout
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_Logout::CAiAction_Logout(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_Logout;
}

CAiAction_Logout::~CAiAction_Logout()
{
}

EDoAction_Return CAiAction_Logout::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	switch (pkDummy->AiActionTypeSecond())
	{
	case NS_LOGOUT_TRY_DISCONNECT:
		{
		}break;
	case NS_LOGOUT_DISCONNECTED:
		{
			pkDummy->AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
			return E_DoAction_Finished;
		}
	default:
		{
			pkDummy->AiActionTypeSecond(NS_LOGOUT_TRY_DISCONNECT);
			pkDummy->Terminate();
		}break;
	}
	return E_DoAction_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_GoFirst
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_GoFirst::CAiAction_GoFirst(LPCTSTR lpszValue, int iGoTo)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAIAction_GoFirst;
	m_iGoToIndex = iGoTo;
}

CAiAction_GoFirst::~CAiAction_GoFirst()
{
}

EDoAction_Return CAiAction_GoFirst::DoAction(CDummyClient* pkDummy)
{
	return EDoAction_Return(E_DoAction_GoFirst + m_iGoToIndex);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_EndGame
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_EndGame::CAiAction_EndGame(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_EndGame;
}

CAiAction_EndGame::~CAiAction_EndGame()
{
}

EDoAction_Return CAiAction_EndGame::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	return E_DoAction_EndGame;
}



/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_NetGiveItem
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_NetGiveItem::CAiAction_NetGiveItem(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_NetGiveItem;
}

CAiAction_NetGiveItem::~CAiAction_NetGiveItem()
{
}

EDoAction_Return CAiAction_NetGiveItem::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	BM::Stream kPacket(PT_C_M_GODCMD);
	kPacket.Push((int)1);
	kPacket.Push((int)50100130);	// 사과 Item No
	kPacket.Push(1);	// Item 개수
	pkDummy->SendToSwitch(kPacket);
	return E_DoAction_Finished;
}
/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_PartyCreate
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_PartyCreate::CAiAction_PartyCreate(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_PartyCreate;
}

CAiAction_PartyCreate::~CAiAction_PartyCreate()
{
}

EDoAction_Return CAiAction_PartyCreate::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	TCHAR chValue[20];
	_stprintf_s(chValue, 20, _T("%d"), rand()%1000);
	std::wstring rkPartyName = std::wstring(chValue);

	SPartyOption rkOption;
	rkOption.iPartyOption = 16842755;
	rkOption.iPartyLevel = 1;

	BM::Stream kPacket(PT_C_N_REQ_CREATE_PARTY);
	kPacket.Push(rkPartyName);
	rkOption.WriteToPacket(kPacket);
	pkDummy->SendToSwitch(kPacket);

	return E_DoAction_Finished;
}
/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_PartyLeave
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_PartyLeave::CAiAction_PartyLeave(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_PartyLeave;
}

CAiAction_PartyLeave::~CAiAction_PartyLeave()
{
}

EDoAction_Return CAiAction_PartyLeave::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	BM::Stream kPacket(PT_C_N_REQ_LEAVE_PARTY, true);
	pkDummy->SendToSwitch(kPacket);

	return E_DoAction_Finished;
}
/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_NetGiveRemoveItem
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_NetGiveRemoveItem::CAiAction_NetGiveRemoveItem(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_NetGiveRemoveItem;
}

CAiAction_NetGiveRemoveItem::~CAiAction_NetGiveRemoveItem()
{
}

EDoAction_Return CAiAction_NetGiveRemoveItem::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	// Net_GiveItem
	BM::Stream kPacket(PT_C_M_GODCMD);
	kPacket.Push((int)1);
	kPacket.Push((int)50100130);	// 사과 Item No
	kPacket.Push(1);	// Item 개수
	pkDummy->SendToSwitch(kPacket);
	// Net_GiveItem
	BM::Stream kPacket2(PT_C_M_GODCMD);
	kPacket2.Push((int)1);
	kPacket2.Push((int)50100130);	// 사과 Item No
	kPacket2.Push(1);	// Item 개수
	pkDummy->SendToSwitch(kPacket2);
	// RemoveItem
	SItemPos kItemPos(IT_CONSUME, rand()%MAX_CONSUME_IDX);
	BM::Stream kPacket3(PT_C_M_REMOVEITEM);
	kPacket3.Push(kItemPos);	// 지울위치 (소모아이템 첫번째것)
	kPacket3.Push((int)0);	// 0=그냥 버리기, 1=분해하기
	pkDummy->SendToSwitch(kPacket3);
	// Net_GiveItem (무기아이템)
	BM::Stream kPacket4(PT_C_M_GODCMD);
	kPacket4.Push((int)1);
	int iItemNo = 11000010 + rand()%39 * 10;	// 11000010 ~ 1100390
	kPacket4.Push((int)iItemNo);	// 사과 Item No
	kPacket4.Push(1);	// Item 개수
	pkDummy->SendToSwitch(kPacket4);
	// RemoveItem
	SItemPos kItemPos2(IT_EQUIP, rand()%MAX_EQUIP_IDX);
	BM::Stream kPacket5(PT_C_M_REMOVEITEM);
	kPacket5.Push(kItemPos2);	// 지울위치 (소모아이템 첫번째것)
	kPacket5.Push((int)1);	// 0=그냥 버리기, 1=분해하기
	pkDummy->SendToSwitch(kPacket5);

	return E_DoAction_Finished;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_MoveItem
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_MoveItem::CAiAction_MoveItem(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_MoveItem;
}

CAiAction_MoveItem::~CAiAction_MoveItem()
{
}

EDoAction_Return CAiAction_MoveItem::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	BM::Stream kPacket(PT_C_M_REQ_ITEM_CHANGE);
	SItemPos kFrom(IT_EQUIP, rand()%MAX_EQUIP_IDX);
	SItemPos kTo(IT_EQUIP, rand()%MAX_EQUIP_IDX);
	if (kFrom.y == kTo.y)
	{
		kFrom.y = 0;
		kTo.y = rand() % (MAX_EQUIP_IDX-1) + 1;
	}
	kPacket.Push(kFrom);
	kPacket.Push(kTo);
	kPacket.Push((int)0);	// GetServerElapsedTime32()
	kPacket.Push(BM::GUID::NullData()); //PgSafeInventory::kSafeInventoryNpc
	pkDummy->SendToSwitch(kPacket);
	_DETAIL_LOG INFO_LOG(BM::LOG_LV9, _T(" CAiAction_MoveItem::DoAction "));
	return E_DoAction_Finished;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_MapMove
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_MapMove::CAiAction_MapMove(LPCTSTR lpszValue)
{
	m_kMapNumVector.resize(3);
	_stscanf_s(lpszValue, _T("%d %d %d %d"), &m_dwDelayTime, &m_kMapNumVector[0], &m_kMapNumVector[1], &m_kMapNumVector[2]);

	m_eType = EAiAction_MapMove;
}

CAiAction_MapMove::~CAiAction_MapMove()
{
}

EDoAction_Return CAiAction_MapMove::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	EAiActionType_Second eSecondType = pkDummy->AiActionTypeSecond();
	int iNextMapNum = 0;
	if (eSecondType == E_AI_STEP_MAPMOVE_NONE)
	{
		int iIndex = pkDummy->GetAbil(AT_ReqMapMove_Index) + 1;
		iIndex = (m_kMapNumVector.size() > iIndex) ? iIndex  : 0;
		int const iNextMapNum = m_kMapNumVector.at(iIndex);

		pkDummy->AiActionTypeSecond(E_AI_STEP_MAPMOVE_REQ);
		pkDummy->SetAbil(AT_ReqMapMove_Index, iIndex);
		//SReqMapMove_CM kRMM;
		//kRMM.cType = MMET_Normal;
		//kRMM.nPortalNo = 1;
		//kRMM.kGndKey.Set(iNextMapNum);
		
		_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"MapMove reqto["<<iNextMapNum<<L"]");
		BM::Stream kPacket(PT_C_M_GODCMD, (int)10);
		kPacket.Push(iNextMapNum);
		kPacket.Push(1);
		pkDummy->SendToSwitch(kPacket);
	}
	else if (eSecondType == E_AI_STEP_MAPMOVE_RECVED)
	{
		return E_DoAction_Finished;
	}

	return E_DoAction_SUCCESS;
}

void CAiAction_MapMove::OnBegin(CDummyClient* pkDummy)
{
	pkDummy->AiActionTypeSecond(E_AI_STEP_MAPMOVE_NONE);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_Random_MapMove_Item
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_Random_MapMove_Item::CAiAction_Random_MapMove_Item(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_MoveItem;
}

CAiAction_Random_MapMove_Item::~CAiAction_Random_MapMove_Item()
{
}

EDoAction_Return CAiAction_Random_MapMove_Item::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	// TODO
	return E_DoAction_Finished;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_MissionMove
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_MissionMove::CAiAction_MissionMove(LPCTSTR lpszValue)	
{
	m_kMissionNum.resize(3);
	_stscanf_s(lpszValue, _T("%d %d %d %d"), &m_dwDelayTime, &m_kMissionNum[0], &m_kMissionNum[1], &m_kMissionNum[2]);

	m_eType = EAiAction_MissionMove;
}

CAiAction_MissionMove::~CAiAction_MissionMove()
{

}

EDoAction_Return CAiAction_MissionMove::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	// EAiActionType_Second eSecondType = pkDummy->AiActionTypeSecond();
	// if ( eSecondType == E_AI_STEP_MISSIONMOVE_NONE )
	// {
	// 	TB_REF_COUNT tbRef;
	// 	CONT_DEF_MISSION_ROOT const *pkMissionRoot = NULL;
	// 	g_kTblDataMgr.GetContDef( tbRef, pkMissionRoot );

	// 	if ( pkMissionRoot )
	// 	{
	// 		int iIndex = pkDummy->GetAbil(AT_ReqMissionMove_Index) + 1;
	// 		iIndex = (m_kMissionNum.size() > iIndex) ? iIndex  : 0;
	// 		int const iNextMissionNum = m_kMissionNum.at(iIndex);

	// 		CONT_DEF_MISSION_ROOT::const_iterator itr = pkMissionRoot->find( iNextMissionNum );
	// 		if ( itr != pkMissionRoot->end() )
	// 		{
	// 			pkDummy->AiActionTypeSecond(E_AI_STEP_MISSIONMOVE_REQ);
				
	// 			_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"MissionMove reqto["<<iNextMissionNum<<L"]");
	// 			BM::Stream kPacket( PT_C_M_GODCMD, 27 );
	// 			kPacket.Push( itr->second.iKey );
	// 			kPacket.Push( BM::Rand_Range(4,1) );
	// 			pkDummy->SendToSwitch(kPacket);
	// 		}

	// 		pkDummy->SetAbil(AT_ReqMissionMove_Index, iIndex);
	// 	}
	// }
	// else if (eSecondType == E_AI_STEP_MAPMOVE_RECVED)
	// {
	// 	return E_DoAction_Finished;
	// }

	return E_DoAction_SUCCESS;
}

void CAiAction_MissionMove::OnBegin(CDummyClient* pkDummy)
{
	pkDummy->AiActionTypeSecond(E_AI_STEP_MISSIONMOVE_NONE);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_EchoChat
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_EchoChat::CAiAction_EchoChat(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_Echo_Chat;
}

CAiAction_EchoChat::~CAiAction_EchoChat()
{
}

EDoAction_Return CAiAction_EchoChat::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

//	return E_DoAction_Finished;

//	INFO_LOG(BM::LOG_LV9, _T("EchoChat"));

	static std::wstring const kChatData= _T("EchoChat Data what is the problem? i think CPU!!");

//	BM::Stream kPacket(PT_C_M_REQ_ECHO_CHAT);
//	kPacket.Push(kChatData);
//	pkDummy->SendToSwitch(kPacket);

	BM::Stream kPacket(PT_C_M_REQ_CHAT);

	kPacket.Push((BYTE)CT_NORMAL);//바이트로 잘라낸다.
	kPacket.Push(kChatData);
	kPacket.Push(static_cast< DWORD >( 0xFFFFFFFF ));
	kPacket.Push(static_cast< int >( 0 ));
	pkDummy->SendToSwitch(kPacket);



	return E_DoAction_Finished;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_PsedoAction
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_PsedoAction::CAiAction_PsedoAction(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_Pseudo_Action;
}

CAiAction_PsedoAction::~CAiAction_PsedoAction()
{
}

EDoAction_Return CAiAction_PsedoAction::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	static int iActionInstanceID = 0;
	SActionInfo kActionInfo;
	kActionInfo.guidPilot = pkDummy->GetID();
	kActionInfo.bIsDown = true;
	kActionInfo.byDirection = 0;
	kActionInfo.ptDirection = POINT3BY(0, 0, 0);
	kActionInfo.ptPos = pkDummy->GetPos();
	kActionInfo.ptPathNormal = POINT2BY(0, 119);
	kActionInfo.dwActionTerm = rand() % 1000;
	kActionInfo.iActionID = 100001001;	// 100001001 = a_jump
	kActionInfo.iActionInstanceID = ++iActionInstanceID;
	kActionInfo.byActionParam = 2;
	kActionInfo.dwTimeStamp = 0;

	BM::Stream kActionPacket(PT_C_M_REQ_ACTION2, kActionInfo);
	kActionPacket.Push((DWORD)0);	// dwSyncTime
	kActionPacket.Push((DWORD)0);	// RandomSeed
	kActionPacket.Push(0);	// byTargetNum
	kActionPacket.Push((bool)false);	// bExtraData
	pkDummy->SendToSwitch(kActionPacket);

	return E_DoAction_Finished;
}


/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_ItemTrade
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_ItemTrade::CAiAction_ItemTrade(LPCTSTR lpszValue)
{
	m_dwDelayTime = _ttoi(lpszValue);
	m_eType = EAiAction_Player_Item_Trade;
}

CAiAction_ItemTrade::~CAiAction_ItemTrade()
{
}

EDoAction_Return CAiAction_ItemTrade::DoAction(CDummyClient* pkDummy)
{
	EDoAction_Return eReturn = E_DoAction_Finished;
	CAiActionDef::DoAction(pkDummy);
	switch (pkDummy->AiActionTypeSecond())
	{
	//case E_AI_STEP_TRADE_REQ:	// 상대방에게 요청 보낸 상태
	//	{
	//	}break;
	//case E_AI_STEP_TRADE_REQ_WAIT:	// 상대방의 요청을 기다리는 상태
	//	{
	//	}break;
	//case E_AI_STEP_TRADE_REQ_ANS:	// Trade 요청에 응답한 상태
	//	{
	//	}break;
	//case E_AI_STEP_TRADE_CONFIRMED:	// Confirm 한 상태
	//	{
	//	}break;
	case E_AI_STEP_TRADE_NONE:	// 처음 시작 상태
	default:
		{
			CDummyClient* pkOther = PopRandomTradeEnableDummy(pkDummy);
			if ( !pkOther )
			{
				_DETAIL_LOG INFO_LOG(BM::LOG_LV8, __FL__ << _T("Added EnableDummyPool (") << pkDummy->Name() << L")");
				AddTradeEnableDummy(pkDummy);
				return E_DoAction_Finished;
			}
			_DETAIL_LOG INFO_LOG(BM::LOG_LV8, __FL__ << _T("New Trade (") << pkDummy->Name() << L", " << pkOther->Name() << _T(")"));
			eReturn = pkDummy->DoTrade(E_AI_STEP_TRADE_NONE, NULL, pkOther->GetID());
			pkOther->AiActionTypeSecond(E_AI_STEP_TRADING_NOW);
		}break;
	}

	return eReturn;
}

CDummyClient* CAiAction_ItemTrade::PopRandomTradeEnableDummy(CDummyClient * const pkMe)
{
	BM::CAutoMutex kLock(m_kMutex);

	if (m_kTradeEnableDummy.empty())
	{
		return NULL;	// Trade 할 녀석이 없다.
	}
	SET_DUMMYCLIENT::iterator itor_index = m_kTradeEnableDummy.find(pkMe);
	if (itor_index != m_kTradeEnableDummy.end())
	{
		m_kTradeEnableDummy.erase(itor_index);
	}
	size_t const iSize = m_kTradeEnableDummy.size();
	if (iSize == 0)
	{
		return NULL;
	}
	size_t const iChoose = BM::Rand_Index(iSize);
	itor_index = m_kTradeEnableDummy.begin();
	advance(itor_index, iChoose);
	if (itor_index != m_kTradeEnableDummy.end())
	{
		CDummyClient* pkOther = (*itor_index);
		m_kTradeEnableDummy.erase(itor_index);
		return pkOther;
	}
	return NULL;
}

void CAiAction_ItemTrade::AddTradeEnableDummy(CDummyClient * const pkMe)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kTradeEnableDummy.insert(pkMe);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		PgAiActionManger
/////////////////////////////////////////////////////////////////////////////////////////

PgAiActionManager::PgAiActionManager()
{
}

PgAiActionManager::~PgAiActionManager()
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_AIACTION_DEF::const_iterator itor = m_kActionDef.begin();
	while (itor != m_kActionDef.end())
	{
		delete itor->second;
		++itor;
	}
	m_kActionDef.clear();
}

bool PgAiActionManager::Build()
{
	std::wstring wIniFile = g_kDummyMgr.IniFile();

	TCHAR chAIDefine[1024];
	if (GetPrivateProfileString(_T("AI_DEFINE"), NULL, NULL, chAIDefine, 1024, wIniFile.c_str()) == 0)
	{
		INFO_LOG(BM::LOG_LV4, __FL__<<L"Cannot read AI_DEFINE in Dummy.ini");
		return false;
	}
	LPTSTR lpszFind = chAIDefine;

	typedef std::map<std::wstring, EAiActionType> CONT_ACTION_NAMETYPE;
	CONT_ACTION_NAMETYPE kActionDef;
	kActionDef.insert(std::make_pair(_T("LOGIN"), EAiAction_Login));
	kActionDef.insert(std::make_pair(_T("LOGOUT"), EAiAction_Logout));
	kActionDef.insert(std::make_pair(_T("S_GOFIRST"), EAIAction_GoFirst));
	kActionDef.insert(std::make_pair(_T("S_ENDGAME"), EAiAction_EndGame));
	kActionDef.insert(std::make_pair(_T("NET_GIVEITEM"), EAiAction_NetGiveItem));
	kActionDef.insert(std::make_pair(_T("NET_GIVEREMOVEITEM"), EAiAction_NetGiveRemoveItem));
	kActionDef.insert(std::make_pair(_T("MOVE_ITEM"), EAiAction_MoveItem));
	kActionDef.insert(std::make_pair(_T("MAPMOVE"), EAiAction_MapMove));
	kActionDef.insert(std::make_pair(_T("ECHO_CHAT"), EAiAction_Echo_Chat));
	kActionDef.insert(std::make_pair(_T("PSEUDO_ACTION"), EAiAction_Pseudo_Action));
	kActionDef.insert(std::make_pair(_T("MISSION_MOVE"), EAiAction_MissionMove));
	kActionDef.insert(std::make_pair(_T("PARTY_CREATE"), EAiAction_PartyCreate));	
	kActionDef.insert(std::make_pair(_T("PARTY_LEAVE"), EAiAction_PartyLeave));	
	kActionDef.insert(std::make_pair(_T("PLAYER_ITEM_TRADE"), EAiAction_Player_Item_Trade));	
	kActionDef.insert(std::make_pair(_T("PVP_MOVE"), EAiAction_PvPMove));

	kActionDef.insert(std::make_pair(_T("OM_CREATE"), EAiAction_OM_Create));
	kActionDef.insert(std::make_pair(_T("OM_OPEN"), EAiAction_OM_Open));
	kActionDef.insert(std::make_pair(_T("OM_EDIT"), EAiAction_OM_Edit));
	kActionDef.insert(std::make_pair(_T("OM_CLOSE"), EAiAction_OM_Close));
	kActionDef.insert(std::make_pair(_T("OM_REGITEM"), EAiAction_OM_RegItem));
	kActionDef.insert(std::make_pair(_T("OM_DEREGITEM"), EAiAction_OM_DeregItem));
	kActionDef.insert(std::make_pair(_T("OM_READDEALING"), EAiAction_OM_ReadDealing));
	kActionDef.insert(std::make_pair(_T("OM_SEARCH"), EAiAction_OM_Search));
	kActionDef.insert(std::make_pair(_T("OM_BUY"), EAiAction_OM_Buy));
	kActionDef.insert(std::make_pair(_T("OM_ENTER"), EAiAction_OM_Enter));
	kActionDef.insert(std::make_pair(_T("OM_EXIT"), EAiAction_OM_Exit));

	TCHAR chValue[100];
	while (lpszFind[0] != _T('\0'))
	{
		CONT_ACTION_NAMETYPE::const_iterator itor = kActionDef.find(lpszFind);
		if (itor != kActionDef.end())
		{
			GetPrivateProfileString(_T("AI_DEFINE"), lpszFind, NULL, chValue, 100, wIniFile.c_str());
			BuildAction(itor->second, chValue);
		}

		size_t iLength = _tcslen(lpszFind);
		lpszFind += (iLength+1);
	}

	// Ai Pattern building
	EAiActionType eActionList[][20] = 
	{
		{ EAiAction_None, },
		{ EAiAction_Login, EAiAction_Logout, EAIAction_GoFirst, EAiAction_None, },	// E_AiPattern_Repeat_Login
		{ EAiAction_Login, EAiAction_NetGiveItem, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Repeat_NetGiveItem
		{ EAiAction_Login, EAiAction_NetGiveRemoveItem, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Repeat_NetGiveRemoveItem
		{ EAiAction_Login, EAiAction_MoveItem, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Repeat_MoveItem
		{ EAiAction_Login, EAiAction_MoveItem, EAiAction_MoveItem, EAiAction_MoveItem, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Random_MapMoveItem
		{ EAiAction_Login, EAiAction_Pseudo_Action, EAiAction_MapMove, EAIAction_Go2nd, },	// E_AiPattern_Repeat_MapMove
		{ EAiAction_Login, EAiAction_Echo_Chat, EAIAction_Go2nd, },	// E_AiPattern_Echo_Chat
		{ EAiAction_Login, EAiAction_Pseudo_Action, EAIAction_Go2nd, },	// E_AiPattern_Repeat_PseudoAction
		{ EAiAction_Login, EAiAction_Pseudo_Action, EAiAction_MissionMove, EAIAction_Go2nd, },	// E_AiPattern_Repeat_MissionMove
		{ EAiAction_Login, EAiAction_PartyCreate, EAiAction_PartyLeave, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Repeat_PartyCreateLeave
		{ EAiAction_Login, EAiAction_Player_Item_Trade, EAIAction_Go2nd, EAiAction_None, },	// E_AiPattern_Repeat_PartyCreateLeave
		{ EAiAction_Login, EAiAction_Pseudo_Action, EAiAction_PvPMove, EAIAction_Go2nd, },	// E_AiPattern_Repeat_PvPMove
		{ EAiAction_Login, EAiAction_OM_Enter, EAiAction_OM_Edit, EAiAction_OM_DeregItem, EAiAction_OM_ReadDealing, EAiAction_OM_Close, EAiAction_OM_Create, EAiAction_OM_Edit, EAiAction_OM_DeregItem, EAiAction_OM_ReadDealing, EAiAction_OM_Close, EAiAction_OM_Exit, EAIAction_Go2nd,},// E_AiPattern_Repeat_OpenMarket_Create
		{ EAiAction_Login, EAiAction_OM_Enter, EAiAction_OM_Buy, EAiAction_OM_Exit, EAIAction_Go2nd,}, // E_AiPattern_Repeat_OpenMarket_Dealing
		{ EAiAction_None, },
	};

	CONT_AIACTION_LIST kActionList;
	for (int i=1; eActionList[i][0] != EAiAction_None; i++)
	{
		kActionList.clear();
		for (int j=0; eActionList[i][j] != EAiAction_None; j++)
		{
			kActionList.push_back(eActionList[i][j]);
		}
		m_kAiPatternDef.insert(std::make_pair((E_AI_Pattern)i, kActionList));
	}
	return true;
}

void PgAiActionManager::BuildAction(EAiActionType eType, LPCTSTR lpszValue)
{
	CAiActionDef* pkNew = NULL;
	switch (eType)
	{
	case EAiAction_Login:
		{
			pkNew = new CAiAction_Login(lpszValue);
		}break;
	case EAiAction_Logout:
		{
			pkNew = new CAiAction_Logout(lpszValue);
		}break;
	case EAIAction_GoFirst:
		{
			pkNew = new CAiAction_GoFirst(lpszValue, 0);
		}break;
	case EAiAction_EndGame:
		{
			pkNew = new CAiAction_EndGame(lpszValue);
		}break;
	case EAiAction_NetGiveItem:
		{
			pkNew = new CAiAction_NetGiveItem(lpszValue);
		}break;
	case EAiAction_NetGiveRemoveItem:
		{
			pkNew = new CAiAction_NetGiveRemoveItem(lpszValue);
		}break;
	case EAiAction_MoveItem:
		{
			pkNew = new CAiAction_MoveItem(lpszValue);
		}break;
	case EAiAction_Echo_Chat:
		{
			pkNew = new CAiAction_EchoChat(lpszValue);
		}break;
	case EAiAction_MapMove:
		{
			pkNew = new CAiAction_MapMove(lpszValue);
		}break;
	case EAiAction_Random_MapMove_Item:
		{
			pkNew = new CAiAction_Random_MapMove_Item(lpszValue);
		}break;
	case EAiAction_Pseudo_Action:
		{
			pkNew = new CAiAction_PsedoAction(lpszValue);
		}break;
	case EAiAction_MissionMove:
		{
			pkNew = new CAiAction_MissionMove(lpszValue);
		}break;
	case EAiAction_PartyCreate:
		{
			pkNew = new CAiAction_PartyCreate(lpszValue);
		}break;
	case EAiAction_PartyLeave:
		{
			pkNew = new CAiAction_PartyLeave(lpszValue);
		}break;
	case EAiAction_Player_Item_Trade:
		{
			pkNew = new CAiAction_ItemTrade(lpszValue);
		}break;
	case EAiAction_PvPMove:
		{
			pkNew = new CAiAction_PvPMove(lpszValue);
		}break;
	case EAiAction_OM_Create:
		{
			pkNew = new CAiAction_OM_Create(lpszValue);
		}break;
	case EAiAction_OM_Open:
		{
			pkNew = new CAiAction_OM_Open(lpszValue);
		}break;
	case EAiAction_OM_Edit:
		{
			pkNew = new CAiAction_OM_Edit(lpszValue);
		}break;
	case EAiAction_OM_Close:
		{
			pkNew = new CAiAction_OM_Close(lpszValue);
		}break;
	case EAiAction_OM_RegItem:
		{
			pkNew = new CAiAction_OM_RegItem(lpszValue);
		}break;
	case EAiAction_OM_DeregItem:
		{
			pkNew = new CAiAction_OM_DeregItem(lpszValue);
		}break;
	case EAiAction_OM_ReadDealing:
		{
			pkNew = new CAiAction_OM_ReadDealing(lpszValue);
		}break;
	case EAiAction_OM_Search:
		{
			pkNew = new CAiAction_OM_Search(lpszValue);
		}break;
	case EAiAction_OM_Buy:
		{
			pkNew = new CAiAction_OM_Buy(lpszValue);
		}break;
	case EAiAction_OM_Enter:
		{
			pkNew = new CAiAction_OM_Enter(lpszValue);
		}break;
	case EAiAction_OM_Exit:
		{
			pkNew = new CAiAction_OM_Exit(lpszValue);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"undefined AiAction Type["<<eType<<L"]");
		}break;
	}

	if (pkNew != NULL)
	{
		m_kActionDef.insert(std::make_pair(eType, pkNew));
	}
}

EAiActionType const PgAiActionManager::GetActionType(E_AI_Pattern const ePattern, unsigned short const usStep) const
{
	CONT_AIPATTERN_DEF::const_iterator itor = m_kAiPatternDef.find(ePattern);
	if (itor == m_kAiPatternDef.end())
	{
		return EAiAction_None;
	}
	if (usStep >= itor->second.size())
	{
		return EAiAction_None;
	}
	return itor->second.at(usStep);
}

CAiActionDef* const PgAiActionManager::GetAction(E_AI_Pattern const ePattern, unsigned short const usStep) const
{
	EAiActionType eType = GetActionType(ePattern, usStep);
	if (eType == EAiAction_None)
	{
		return NULL;
	}

	CONT_AIACTION_DEF::const_iterator itor_action = m_kActionDef.find(eType);
	if (itor_action == m_kActionDef.end())
	{
		if (eType >= EAIAction_GoFirst)
		{
			itor_action = m_kActionDef.find(EAIAction_GoFirst);
			if (itor_action != m_kActionDef.end())
			{
				std::wstring wstrDelayTime = itor_action->second->GetDelayTimeString();
				CAiActionDef* pkNew = (CAiActionDef*) new CAiAction_GoFirst(wstrDelayTime.c_str(), eType-EAIAction_GoFirst);
				m_kActionDef.insert(std::make_pair(eType, pkNew));
				return pkNew;
			}
		}
		return NULL;
	}
	return itor_action->second;
}

void PgAiActionManager::InfoLog_Statistic() const
{
	CONT_AIACTION_DEF::const_iterator itor = m_kActionDef.begin();
	while (itor != m_kActionDef.end())
	{
		itor->second->InfoLog_Statistic(true);

		++itor;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//		CAiAction_PvPMove
/////////////////////////////////////////////////////////////////////////////////////////
CAiAction_PvPMove::CAiAction_PvPMove(LPCTSTR lpszValue)	
{
	m_eType = EAiAction_PvPMove;
}

CAiAction_PvPMove::~CAiAction_PvPMove()
{

}

EDoAction_Return CAiAction_PvPMove::DoAction(CDummyClient* pkDummy)
{
	CAiActionDef::DoAction(pkDummy);

	EAiActionType_Second eSecondType = pkDummy->AiActionTypeSecond();
	switch ( eSecondType )
	{
	case E_AI_STEP_PVPMOVE_NONE:
		{
			BM::Stream kPacket(PT_C_M_REQ_JOIN_LOBBY);
			pkDummy->SendToSwitch( kPacket );
			pkDummy->AiActionTypeSecond(E_AI_STEP_PVPMOVE_JOINNING_LOBBY);
		}break;
	case E_AI_STEP_PVPMOVE_JOINED_LOBBY:
		{
			DWORD dwRandTime = (DWORD)BM::Rand_Range( 60000 );
			pkDummy->NextTickTime( ::timeGetTime() + 10000 + dwRandTime );
			pkDummy->AiActionTypeSecond(E_AI_STEP_PVPMOVE_IN_LOBBY);
		}break;
	case E_AI_STEP_PVPMOVE_IN_LOBBY:
		{
			int iRand = BM::Rand_Range( 1000 );
			if ( (iRand % 7) == 0 )
			{
				SPvPRoomBasicInfo kRoomBaseInfo(ROOM_STATUS_LOBBY);
				SPvPRoomExtInfo kRoomExtInfo;

				kRoomBaseInfo.m_bPwd = true;
				kRoomBaseInfo.m_iGndNo = 9920204;
				kRoomBaseInfo.m_kMode = PVP_MODE_TEAM;
				kRoomBaseInfo.m_kType = PVP_TYPE_DM;
				kRoomBaseInfo.m_ucGameTime = 10;
				kRoomBaseInfo.m_ucMaxUser = 10;
				kRoomBaseInfo.m_ucNowUser = 0;
				kRoomBaseInfo.m_wstrName = pkDummy->Name() + std::wstring(L"'s Test Room!!");

//				kRoomExtInfo.bUseHandyCap = false;
				kRoomExtInfo.iMaxLevel = 100;
//				kRoomExtInfo.bUseItem = false;
				kRoomExtInfo.ucRound = 1;
				kRoomExtInfo.strPassWord = std::string("1158");

				BM::Stream kPacket(PT_C_T_REQ_CREATE_ROOM);
				kRoomBaseInfo.WriteToPacket(kPacket);
				kRoomExtInfo.WriteToPacket(kPacket);
				pkDummy->SendToSwitch( kPacket );

				pkDummy->AiActionTypeSecond( E_AI_STEP_PVPMOVE_JOINNING_ROOM );
			}
			else if ( (iRand % 5) == 0 )
			{
				BM::Stream kPacket(PT_C_T_REQ_EXIT_LOBBY);
				pkDummy->SendToSwitch( kPacket );
				pkDummy->AiActionTypeSecond( E_AI_STEP_PVPMOVE_REQOUT );
			}
			else
			{
				DWORD dwRandTime = (DWORD)BM::Rand_Range( 30000 );
				pkDummy->NextTickTime( ::timeGetTime() + 10000 + dwRandTime );
			}
		}break;
	case E_AI_STEP_PVPMOVE_JOINED_ROOM:
		{
			DWORD dwRandTime = (DWORD)BM::Rand_Range( 60000 );
			pkDummy->NextTickTime( ::timeGetTime() + 10000 + dwRandTime );
			pkDummy->AiActionTypeSecond(E_AI_STEP_PVPMOVE_IN_ROOM);
		}break;
	case E_AI_STEP_PVPMOVE_IN_ROOM:
		{
			int iRand = BM::Rand_Range( 1000 );
			if ( (iRand % 3) == 0 )
			{
				BM::Stream kPacket( PT_C_T_REQ_EXIT_ROOM );
				pkDummy->SendToSwitch( kPacket );
				pkDummy->AiActionTypeSecond( E_AI_STEP_PVPMOVE_JOINNING_LOBBY );
			}
			else
			{
				DWORD dwRandTime = (DWORD)BM::Rand_Range( 30000 );
				pkDummy->NextTickTime( ::timeGetTime() + 10000 + dwRandTime );
			}
		}break;
	case E_AI_STEP_PVPMOVE_END:
		{
			DWORD dwRandTime = (DWORD)BM::Rand_Range( 60000 );
			pkDummy->NextTickTime( ::timeGetTime() + 10000 + dwRandTime );
			
			// 일정시간이후에 재시작
			this->OnBegin( pkDummy );
//			return E_DoAction_Finished;
		}break;
	}

	return E_DoAction_SUCCESS;
}

void CAiAction_PvPMove::OnBegin(CDummyClient* pkDummy)
{
	pkDummy->AiActionTypeSecond(E_AI_STEP_PVPMOVE_NONE);
}


CAiAction_OM_Create::CAiAction_OM_Create(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Create;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Create::~CAiAction_OM_Create()
{
}

EDoAction_Return CAiAction_OM_Create::DoAction(CDummyClient *pkDummy)
{
	switch(pkDummy->AiActionTypeSecond())
	{
	case E_AI_STEP_OM_CREATE_REQ:
		{
			SItemPos kRetPos;
			if(S_OK == pkDummy->GetInven()->GetFirstItem(IT_CONSUME,50100130,kRetPos))
			{
				SARTICLEINFO kArticle;
				kArticle.CostType(false);
				kArticle.ArticleCost(100);
				kArticle.Pos(kRetPos);
				
				CONT_ARTICLEINFO kArticles;
				kArticles.push_back(kArticle);

				SPT_C_M_UM_REQ_MARKET_OPEN kReq;
				kReq.Articles(kArticles);

				BM::Stream kPacket;
				kReq.WriteToPacket(kPacket);
				pkDummy->SendToSwitch(kPacket);
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_CREATE_REQ_WAIT);
				pkDummy->NextTickTime(::timeGetTime() + rand()%3131);
			}
			else
			{
				BM::Stream kPacket(PT_C_M_GODCMD);
				kPacket.Push((int)1);
				kPacket.Push((int)50100130);	// 사과 Item No
				kPacket.Push(1);	// Item 개수
				pkDummy->SendToSwitch(kPacket);
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_CREATE_REQ);
				pkDummy->NextTickTime(::timeGetTime() + 1000);
			}
		}break;
	case E_AI_STEP_OM_CREATE_REQ_WAIT:
		{
		}break;
	case E_AI_STEP_OM_CREATE_COMPLETE:
		{
			return E_DoAction_Finished;
		}break;
	}
	return E_DoAction_SUCCESS;
}

void CAiAction_OM_Create::OnBegin(CDummyClient *pkDummy)
{
	pkDummy->NextTickTime(::timeGetTime());
	pkDummy->AiActionTypeSecond(E_AI_STEP_OM_CREATE_REQ);
}

//=================================================================================================

CAiAction_OM_Open::CAiAction_OM_Open(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Open;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Open::~CAiAction_OM_Open()
{
}

EDoAction_Return CAiAction_OM_Open::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Open::OnBegin(CDummyClient *pkDummy)
{
	SPT_C_M_UM_REQ_MARKET_MODIFY_STATE	Data;
	Data.MarketState(MS_OPEN);
	BM::Stream	kPacket;
	Data.WriteToPacket(kPacket);
	pkDummy->SendToSwitch(kPacket);
	pkDummy->NextTickTime(::timeGetTime() + 1000);
}

//=================================================================================================

CAiAction_OM_Edit::CAiAction_OM_Edit(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Edit;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Edit::~CAiAction_OM_Edit()
{
}

EDoAction_Return CAiAction_OM_Edit::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Edit::OnBegin(CDummyClient *pkDummy)
{
	SPT_C_M_UM_REQ_MARKET_MODIFY_STATE	Data;
	Data.MarketState(MS_EDIT);
	BM::Stream	kPacket;
	Data.WriteToPacket(kPacket);
	pkDummy->SendToSwitch(kPacket);
	pkDummy->NextTickTime(::timeGetTime() + 1000);
}

//=================================================================================================

CAiAction_OM_Close::CAiAction_OM_Close(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Close;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Close::~CAiAction_OM_Close()
{
}

EDoAction_Return CAiAction_OM_Close::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Close::OnBegin(CDummyClient *pkDummy)
{
	SPT_C_M_REQ_MARKET_CLOSE kData;
	BM::Stream kPacket;
	kData.WriteToPacket(kPacket);
	pkDummy->SendToSwitch(kPacket);
	pkDummy->NextTickTime(::timeGetTime()+100);
}

//=================================================================================================

CAiAction_OM_RegItem::CAiAction_OM_RegItem(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_RegItem;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_RegItem::~CAiAction_OM_RegItem()
{
}

EDoAction_Return CAiAction_OM_RegItem::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_RegItem::OnBegin(CDummyClient *pkDummy)
{
	SItemPos kRetPos;
	if(S_OK == pkDummy->GetInven()->GetFirstItem(IT_CONSUME,50100130,kRetPos))
	{
		SPT_C_M_UM_REQ_ARTICLE_REG kReq;
		kReq.ArticleCost(100);
		kReq.CostType(false);
		kReq.Pos(kRetPos);

		BM::Stream kPacket;
		kReq.WriteToPacket(kPacket);
		pkDummy->SendToSwitch(kPacket);
		pkDummy->NextTickTime(::timeGetTime() + 1000);
	}
}

//=================================================================================================

CAiAction_OM_DeregItem::CAiAction_OM_DeregItem(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_DeregItem;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_DeregItem::~CAiAction_OM_DeregItem()
{
}

EDoAction_Return CAiAction_OM_DeregItem::DoAction(CDummyClient *pkDummy)
{
	CONT_USER_MARKET_ARTICLEINFO_LIST const & kCont = pkDummy->ContMyArticle().kCont;

	switch(pkDummy->AiActionTypeSecond())
	{
	case E_AI_STEP_OM_DEREGITEM_REFLASH:
		{
			SPT_C_M_UM_REQ_MY_MARKET_QUERY kReq;
			BM::Stream kPacket;
			kReq.WriteToPacket(kPacket);
			pkDummy->SendToSwitch(kPacket);
			pkDummy->NextTickTime(::timeGetTime()+1000);
			pkDummy->AiActionTypeSecond(E_AI_STEP_OM_DEREGITEM_REFLASH_WAIT);
			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_DEREGITEM_REFLASH_WAIT:
		{
			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_DEREGITEM_REQ:
		{
			if(false == kCont.empty())
			{
				CONT_USER_MARKET_ARTICLEINFO_LIST::value_type const & kArticle = kCont.front();

				SPT_C_M_UM_REQ_ARTICLE_DEREG kReq;
				kReq.ArticleGuid(kArticle.kItemGuId);

				BM::Stream kPacket;
				kReq.WriteToPacket(kPacket);
				pkDummy->SendToSwitch(kPacket);
				pkDummy->NextTickTime(::timeGetTime()+1000);
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_DEREGITEM_REFLASH);
				return E_DoAction_SUCCESS;
			}
		}break;
	}

	return E_DoAction_Finished;
}

void CAiAction_OM_DeregItem::OnBegin(CDummyClient *pkDummy)
{
	pkDummy->AiActionTypeSecond(E_AI_STEP_OM_DEREGITEM_REFLASH);
}

//=================================================================================================

CAiAction_OM_ReadDealing::CAiAction_OM_ReadDealing(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_ReadDealing;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_ReadDealing::~CAiAction_OM_ReadDealing()
{
}

EDoAction_Return CAiAction_OM_ReadDealing::DoAction(CDummyClient *pkDummy)
{
	switch(pkDummy->AiActionTypeSecond())
	{
	case E_AI_STEP_OM_READDEALING_REFLASH:
		{
			SPT_C_M_UM_REQ_MY_MARKET_QUERY kReq;
			BM::Stream kPacket;
			kReq.WriteToPacket(kPacket);
			pkDummy->SendToSwitch(kPacket);
			pkDummy->NextTickTime(::timeGetTime()+1000);
			pkDummy->AiActionTypeSecond(E_AI_STEP_OM_READDEALING_REFLASH_WAIT);
			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_READDEALING_REFLASH_WAIT:
		{
			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_READDEALING_REQ:
		{
			CONT_USER_MARKET_DEALINGINFO_LIST const & kCont = pkDummy->ContMyDealing().kCont;

			if(false == kCont.empty())
			{
				CONT_USER_MARKET_DEALINGINFO_LIST::value_type const & kDealing = kCont.front();

				SPT_C_M_UM_REQ_DEALINGS_READ kReq;
				kReq.DealingGuid(kDealing.kDealingGuid);

				BM::Stream kPacket;
				kReq.WriteToPacket(kPacket);
				pkDummy->SendToSwitch(kPacket);
				pkDummy->NextTickTime(::timeGetTime()+1000);
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_READDEALING_REFLASH);
				return E_DoAction_SUCCESS;
			}
		}break;
	}

	return E_DoAction_Finished;
}

void CAiAction_OM_ReadDealing::OnBegin(CDummyClient *pkDummy)
{
	pkDummy->AiActionTypeSecond(E_AI_STEP_OM_READDEALING_REFLASH);
}

//=================================================================================================

void SendReqSearch(CDummyClient *pkDummy)
{
	SPT_C_M_UM_REQ_MARKET_QUERY kReq;
	kReq.SearchName(_T(""));
	kReq.SearchNameType(false);
	kReq.SearchItemMode(-1);
	kReq.SearchItemModeType(-1);
	kReq.LevelMin(0);
	kReq.LevelMax(0);
	kReq.ItemGrade(IG_MAX);
	kReq.ClassLimit(0);
	kReq.CostType(false);
	kReq.ArticleIndex(0);

	BM::Stream kPacket;
	kReq.WriteToPacket(kPacket);
	pkDummy->SendToSwitch(kPacket);
}

CAiAction_OM_Search::CAiAction_OM_Search(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Search;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Search::~CAiAction_OM_Search()
{
}

EDoAction_Return CAiAction_OM_Search::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Search::OnBegin(CDummyClient *pkDummy)
{
	SendReqSearch(pkDummy);
	pkDummy->NextTickTime(::timeGetTime()+1000);
}

//=================================================================================================

CAiAction_OM_Buy::CAiAction_OM_Buy(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Buy;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Buy::~CAiAction_OM_Buy()
{
}

EDoAction_Return CAiAction_OM_Buy::DoAction(CDummyClient *pkDummy)
{
	switch(pkDummy->AiActionTypeSecond())
	{
	case E_AI_STEP_OM_BUY_SEARCH:
		{
			if(true == pkDummy->ContMarketList().empty())
			{
				SendReqSearch(pkDummy);
				pkDummy->NextTickTime(::timeGetTime()+1000);
			}
			else
			{
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_BUY_MARKET_QUERY);
			}

			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_BUY_MARKET_QUERY:
		{
			CONT_MARKET_KEY_LIST const & kCont = pkDummy->ContMarketList();
			if(false == kCont.empty())
			{
				SPT_C_M_UM_REQ_MARKET_ARTICLE_QUERY kReq;
				pkDummy->EnterdMarketGuid(kCont.front().kOwnerId);
				kReq.MarketGuId(pkDummy->EnterdMarketGuid());
				BM::Stream kPacket;
				kReq.WriteToPacket(kPacket);
				pkDummy->SendToSwitch(kPacket);
				pkDummy->NextTickTime(::timeGetTime()+1000);
				pkDummy->AiActionTypeSecond(E_AI_STEP_OM_BUY_REQ);
			}
			return E_DoAction_SUCCESS;
		}break;
	case E_AI_STEP_OM_BUY_REQ:
		{
			CONT_USER_MARKET_ARTICLEINFO_LIST const & kCont = pkDummy->ContMarketArticle().kCont;

			if(false == kCont.empty())
			{
				CONT_USER_MARKET_ARTICLEINFO_LIST::value_type const & kArticle = kCont.front();

				if(kArticle.kArticleCost > pkDummy->GetAbil64(AT_MONEY))
				{
					BM::Stream kPacket(PT_C_M_GODCMD);
					kPacket.Push((int)9);
					kPacket.Push(AT_MONEY);
					kPacket.Push(kArticle.kArticleCost);
					pkDummy->SendToSwitch(kPacket);
					pkDummy->NextTickTime(::timeGetTime() + 1000);
					return E_DoAction_SUCCESS;
				}

				SPT_C_M_UM_REQ_ARTICLE_BUY kReq;
				kReq.ArticleGuid(kArticle.kItemGuId);
				kReq.BuyNum(kArticle.kItem.Count());
				kReq.MarketGuid(pkDummy->EnterdMarketGuid());

				BM::Stream kPacket;
				kReq.WriteToPacket(kPacket);
				pkDummy->SendToSwitch(kPacket);
			}

		}break;
	}
	return E_DoAction_Finished;
}

void CAiAction_OM_Buy::OnBegin(CDummyClient *pkDummy)
{
	pkDummy->NextTickTime(::timeGetTime());
	pkDummy->ClearMarketList();
	pkDummy->AiActionTypeSecond(E_AI_STEP_OM_BUY_SEARCH);
}

//=================================================================================================

CAiAction_OM_Enter::CAiAction_OM_Enter(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Enter;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Enter::~CAiAction_OM_Enter()
{
}

EDoAction_Return CAiAction_OM_Enter::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Enter::OnBegin(CDummyClient *pkDummy)
{
	BM::Stream kPacket(PT_C_M_UM_REQ_MARKET_ENTER);
	pkDummy->SendToSwitch(kPacket);
	pkDummy->NextTickTime(::timeGetTime()+100);
}

//=================================================================================================

CAiAction_OM_Exit::CAiAction_OM_Exit(LPCTSTR lpszValue)
{
	m_eType = EAiAction_OM_Exit;
	m_dwDelayTime = _ttoi(lpszValue);
}

CAiAction_OM_Exit::~CAiAction_OM_Exit()
{
}

EDoAction_Return CAiAction_OM_Exit::DoAction(CDummyClient *pkDummy)
{
	return E_DoAction_Finished;
}

void CAiAction_OM_Exit::OnBegin(CDummyClient *pkDummy)
{
	BM::Stream kPacket(PT_C_M_UM_REQ_MARKET_EXIT);
	pkDummy->SendToSwitch(kPacket);
	pkDummy->NextTickTime(::timeGetTime()+100);
}

