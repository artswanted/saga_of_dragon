#pragma once

class CDummyClient;

typedef enum
{
	EAiAction_None = 0,
	EAiAction_Login = 1,
	EAiAction_Logout = 2,
	EAiAction_NetGiveItem = 3,
	EAiAction_NetGiveRemoveItem = 4,
	EAiAction_MoveItem = 5,
	EAiAction_MapMove = 6,
	EAiAction_Random_MapMove_Item = 7,
	EAiAction_Echo_Chat = 8,
	EAiAction_Pseudo_Action = 9,
	EAiAction_MissionMove = 10,
	EAiAction_PartyCreate = 11,
	EAiAction_PartyLeave = 12,
	EAiAction_Player_Item_Trade = 13,
	EAiAction_PvPMove = 14,

	EAiAction_OM_Create = 15,	// 상점 생성
	EAiAction_OM_Open = 16,	// 상점 판매 시작
	EAiAction_OM_Edit = 17,	// 상점 수정 모드
	EAiAction_OM_Close = 18,// 상점 삭제
	EAiAction_OM_RegItem = 19,// 물품 등록
	EAiAction_OM_DeregItem = 20,// 물품 삭제
	EAiAction_OM_ReadDealing = 21,// 판매금 수령
	EAiAction_OM_Search	= 22, // 상점 검색
	EAiAction_OM_Buy = 23,	// 물품 구매
	EAiAction_OM_Enter = 24,// 오픈 마켓 매뉴 열기
	EAiAction_OM_Exit = 25,// 오픈 마켓 매뉴 닫기


	// Special value
	EAiAction_EndGame = 9002,
	EAIAction_GoFirst = 10000,
	EAIAction_Go2nd = 10001,
	EAIAction_Go3rd = 10002,
} EAiActionType;

typedef enum
{
	NS_NONE = 0,
	// EAiAction_Login state
	NS_BEFORE_LOGIN_CONNECT,//로긴전.		이 상태일때 -> TryConnectLogin() 호출됨 NS_LOGIN_CONNECT_TRY로 전이
	NS_LOGIN_CONNECT_TRY_FOR_AUTH,//로그인서버 접속 시도 했음.	-> NS_LOGIN_CONNECT_COMPLETE, NS_BEFORE_LOGIN_CONNECT로 전이/.
	NS_LOGIN_CONNECT_SUCCESS_FOR_AUTH,//로그인서버 접속 시도 했음.		TryConnectLogin()	NS_TRY_AUTH 로 전이.
	NS_LOGIN_CONNECT_TRY_FOR_LOGIN,//로그인서버 접속 시도 했음.	-> NS_LOGIN_CONNECT_COMPLETE, NS_BEFORE_LOGIN_CONNECT로 전이/.
	NS_LOGIN_CONNECT_SUCCESS_FOR_LOGIN,
	NS_CONNECTED_SWITCH,
	NS_TRY_AUTH, //인증 시도.
	NS_TRY_LOGIN,	// 로그인 시도
	NS_IN_GAME,//그라운드 입장

	// PVP 입장
	E_AI_STEP_PVP_JOINING,
	E_AI_STEP_PVP_LOBBY,
	E_AI_STEP_PVP_WAITROOM,
	E_AI_STEP_PVP_READYROOM,
	E_AI_STEP_PVP_MASTERROOM,
	E_AI_STEP_PVP_EXITROOM,
	E_AI_STEP_PVP_PLAYING,
	E_AI_STEP_PVP_RESULT,

	// EAiAction_Logout
	NS_LOGOUT_TRY_DISCONNECT,
	NS_LOGOUT_DISCONNECTED,

	// EAiAction_MapMove
	E_AI_STEP_MAPMOVE_NONE,
	E_AI_STEP_MAPMOVE_REQ,
	E_AI_STEP_MAPMOVE_RECVED,

	// EAiAction_MissionMove
	E_AI_STEP_MISSIONMOVE_NONE,
	E_AI_STEP_MISSIONMOVE_REQ,
	E_AI_STEP_MISSIONEND,

	// EAiAciton_PvPMove
	E_AI_STEP_PVPMOVE_NONE,
	E_AI_STEP_PVPMOVE_JOINNING_LOBBY,
	E_AI_STEP_PVPMOVE_JOINED_LOBBY,
	E_AI_STEP_PVPMOVE_IN_LOBBY,
	E_AI_STEP_PVPMOVE_JOINNING_ROOM,
	E_AI_STEP_PVPMOVE_JOINED_ROOM,
	E_AI_STEP_PVPMOVE_IN_ROOM,
	E_AI_STEP_PVPMOVE_REQOUT,
	E_AI_STEP_PVPMOVE_END,

	NS_IN_GAME_MAP_MOVE,

	E_AI_STEP_TRADE_NONE,	// 처음 시작 상태
	E_AI_STEP_TRADE_REQ,	// 상대방에게 요청 보낸 상태
	E_AI_STEP_TRADE_REQ_WAIT,	// 상대방의 요청을 기다리는 상태
	E_AI_STEP_TRADE_REQ_ANS,	// Trade 요청에 응답한 상태
	E_AI_STEP_TRADE_XCHANGE,	// Trade 아이템정보 보낸 상태
	E_AI_STEP_TRADE_CONFIRMED,	// Confirm 한 상태
	E_AI_STEP_TRADE_XCHANGE_READY,	// Exchange Ready 도 누른상태(최종상태)
	E_AI_STEP_TRADING_NOW,

	E_AI_STEP_OM_CREATE_REQ,
	E_AI_STEP_OM_CREATE_REQ_WAIT,
	E_AI_STEP_OM_CREATE_COMPLETE,

	E_AI_STEP_OM_DEREGITEM_REFLASH,
	E_AI_STEP_OM_DEREGITEM_REFLASH_WAIT,
	E_AI_STEP_OM_DEREGITEM_REQ,

	E_AI_STEP_OM_READDEALING_REFLASH,
	E_AI_STEP_OM_READDEALING_REFLASH_WAIT,
	E_AI_STEP_OM_READDEALING_REQ,

	E_AI_STEP_OM_BUY_SEARCH,
	E_AI_STEP_OM_BUY_SEARCH_WAIT,
	E_AI_STEP_OM_BUY_MARKET_QUERY,
	E_AI_STEP_OM_BUY_MARKET_QUERY_WAIT,
	E_AI_STEP_OM_BUY_REQ,
}EAiActionType_Second;


typedef enum
{
	E_DoAction_SUCCESS = 0,
	E_DoAction_Finished = 1,
	E_DoAction_EndGame = 2,
	E_DoAction_Max = 3,

	E_DoAction_GoFirst = 10000,
	E_DoAction_Go2nd = 10001,
	E_DoAction_Go3rd = 10002,
} EDoAction_Return;

typedef struct
{
	void Init()
	{
		dwCallTotalCount = 0;
		dwBeginTime = 0;
		pkSampleClient = NULL;
		dwLastCallTime = 0;
		fAvarageCallInterval = 0.f;
	}

	DWORD dwCallTotalCount;	//총 Call Count
	DWORD dwBeginTime;

	CDummyClient* pkSampleClient;	// Sample Dummy
	DWORD dwLastCallTime;	// SampleDummy 가 최근에 호출된 시간
	float fAvarageCallInterval;	// SampleDummy 호출 주기(평균)
} SStatistic;

class CAiActionDef
{
public:
	CAiActionDef();
	virtual ~CAiActionDef() {};

	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);
	DWORD GetDelayTime() const { return m_dwDelayTime; }
	virtual std::wstring Print() const = 0;
	std::wstring GetDelayTimeString() const;
	virtual void InfoLog_Statistic(bool const bClearInfo) const;
	virtual DWORD GetTimeover() const { return m_dwTimeoverLimit; }
	virtual void OnBegin(CDummyClient* pkDummy);
	virtual void OnEnd(CDummyClient* pkDummy);

protected:
	EAiActionType m_eType;
	DWORD m_dwDelayTime;
	DWORD m_dwTimeoverLimit;
	mutable SStatistic m_kStatistic;
};

class CAiAction_Login
	: public CAiActionDef
{
public:
	CAiAction_Login(LPCTSTR lpszValue);
	virtual ~CAiAction_Login();
	virtual std::wstring Print() const { return _T("CAiAction_Login"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
	bool TryConnectLogin(bool const bIsTryAuth, CDummyClient* pkDummy) const;
};

class CAiAction_GoFirst
	: public CAiActionDef
{
public:
	CAiAction_GoFirst(LPCTSTR lpszValue, int iGoTo);
	virtual ~CAiAction_GoFirst();
	virtual std::wstring Print() const { return _T("CAiAction_GoFirst"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
	int m_iGoToIndex;
};


class CAiAction_EndGame
	: public CAiActionDef
{
public:
	CAiAction_EndGame(LPCTSTR lpszValue);
	virtual ~CAiAction_EndGame();
	virtual std::wstring Print() const { return _T("CAiAction_EndGame"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_Logout
	: public CAiActionDef
{
public:
	CAiAction_Logout(LPCTSTR lpszValue);
	virtual ~CAiAction_Logout();
	virtual std::wstring Print() const { return _T("CAiAction_Logout"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_NetGiveItem
	: public CAiActionDef
{
public:
	CAiAction_NetGiveItem(LPCTSTR lpszValue);
	virtual ~CAiAction_NetGiveItem();
	virtual std::wstring Print() const { return _T("CAiAction_NetGiveItem"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_PartyCreate
	: public CAiActionDef
{
public:
	CAiAction_PartyCreate(LPCTSTR lpszValue);
	virtual ~CAiAction_PartyCreate();
	virtual std::wstring Print() const { return _T("CAiAction_PartyCreate"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_PartyLeave
	: public CAiActionDef
{
public:
	CAiAction_PartyLeave(LPCTSTR lpszValue);
	virtual ~CAiAction_PartyLeave();
	virtual std::wstring Print() const { return _T("CAiAction_PartyLeave"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_NetGiveRemoveItem
	: public CAiActionDef
{
public:
	CAiAction_NetGiveRemoveItem(LPCTSTR lpszValue);
	virtual ~CAiAction_NetGiveRemoveItem();
	virtual std::wstring Print() const { return _T("CAiAction_NetGiveRemoveItem"); }

public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_MoveItem
	: public CAiActionDef
{
public:
	CAiAction_MoveItem(LPCTSTR lpszValue);
	virtual ~CAiAction_MoveItem();
	virtual std::wstring Print() const { return _T("CAiAction_MoveItem"); }
public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);
};

class CAiAction_MapMove
	: public CAiActionDef
{
public:
	CAiAction_MapMove(LPCTSTR lpszValue);
	virtual ~CAiAction_MapMove();
	virtual std::wstring Print() const { return _T("CAiAction_MapMove"); }
public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);
	typedef std::vector<int> VEC_MAPNUM;
	void OnBegin(CDummyClient* pkDummy);
protected:
	VEC_MAPNUM m_kMapNumVector;
};

class CAiAction_Random_MapMove_Item
	: public CAiActionDef
{
public:
	CAiAction_Random_MapMove_Item(LPCTSTR lpszValue);
	virtual ~CAiAction_Random_MapMove_Item();
	virtual std::wstring Print() const { return _T("CAiAction_Random_MapMove_Item"); }
public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_MissionMove
	: public CAiActionDef
{
public:
	CAiAction_MissionMove(LPCTSTR lpszValue);
	virtual ~CAiAction_MissionMove();
	virtual std::wstring Print() const { return _T("CAiAction_MissionMove"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	typedef std::vector<int> VEC_MISSIONNUM;
	void OnBegin(CDummyClient *pkDummy);
protected:
	VEC_MISSIONNUM	m_kMissionNum;
};

class CAiAction_PsedoAction
	: public CAiActionDef
{
public:
	CAiAction_PsedoAction(LPCTSTR lpszValue);
	virtual ~CAiAction_PsedoAction();
	virtual std::wstring Print() const { return _T("CAiAction_PsedoAction"); }
public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_EchoChat
	: public CAiActionDef
{
public:
	CAiAction_EchoChat(LPCTSTR lpszValue);
	virtual ~CAiAction_EchoChat();
	virtual std::wstring Print() const { return _T("CAiAction_EchoChat"); }
public :
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
};

class CAiAction_ItemTrade
	: public CAiActionDef
{
public:
	CAiAction_ItemTrade(LPCTSTR lpszValue);
	virtual ~CAiAction_ItemTrade();
	virtual std::wstring Print() const { return _T("CAiAction_ItemTrade"); }
public :
	typedef std::set<CDummyClient*> SET_DUMMYCLIENT;
	virtual EDoAction_Return DoAction(CDummyClient* pkDummy);

protected:
	CDummyClient* PopRandomTradeEnableDummy(CDummyClient * const pkMe);
	void AddTradeEnableDummy(CDummyClient * const pkMe);
	Loki::Mutex m_kMutex;
private:
	SET_DUMMYCLIENT m_kTradeEnableDummy;	// Trade 를 할 수 있는 dummy index
};


typedef enum
{
	E_AiPattern_None = 0,
	E_AiPattern_Repeat_Login = 1,
	E_AiPattern_Repeat_NetGiveItem = 2,
	E_AiPattern_Repeat_NetGiveRemoveItem = 3,
	E_AiPattern_Repeat_MoveItem = 4,
	E_AiPattern_Random_MapMoveItem = 5,
	E_AiPattern_Repeat_MapMove = 6,
	E_AiPattern_Echo_Chat = 7,
	E_AiPattern_Repeat_PseudoAction = 8,
	E_AiPattern_Repeat_MissionMove = 9,
	E_AiPattern_Repeat_PartyCreateLeave = 10,
	E_AiPattern_Repeat_Player_Item_Trade = 11, 
	E_AiPattern_Repeat_PvPMove = 12,
	E_AiPattern_Repeat_OpenMarket_Create = 13,
	E_AiPattern_Repeat_OpenMarket_Dealing = 14,

	E_AiPattern_Max,
} E_AI_Pattern;

////////////////////////////////////////////////////////////////
class PgAiActionManager
{
public:
	PgAiActionManager();
	virtual ~PgAiActionManager();

	typedef std::map<EAiActionType, CAiActionDef*> CONT_AIACTION_DEF;
	typedef std::vector<EAiActionType> CONT_AIACTION_LIST;
	typedef std::map<E_AI_Pattern, CONT_AIACTION_LIST> CONT_AIPATTERN_DEF;

	bool Build();
	EAiActionType const GetActionType(E_AI_Pattern const ePattern, unsigned short const usStep) const;
	CAiActionDef* const GetAction(E_AI_Pattern const ePattern, unsigned short const usStep) const;
	void InfoLog_Statistic() const;

protected:
	void BuildAction(EAiActionType eType, LPCTSTR lpszValue);
protected:
	mutable Loki::Mutex m_kMutex;
private:
	mutable CONT_AIACTION_DEF m_kActionDef;
	CONT_AIPATTERN_DEF m_kAiPatternDef;
};

class CAiAction_PvPMove
	: public CAiActionDef
{
public:
	CAiAction_PvPMove(LPCTSTR lpszValue);
	virtual ~CAiAction_PvPMove();
	virtual std::wstring Print() const { return _T("CAiAction_PvPMove"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_InPvPRoom
{
public:
	CAiAction_InPvPRoom();
	virtual ~CAiAction_InPvPRoom();
	virtual std::wstring Print() const { return _T("CAiAction_InPvPRoom");}

public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);
};


class CAiAction_OM_Create
	: public CAiActionDef
{
public:
	CAiAction_OM_Create(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Create();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Create"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Open
	: public CAiActionDef
{
public:
	CAiAction_OM_Open(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Open();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Open"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Edit
	: public CAiActionDef
{
public:
	CAiAction_OM_Edit(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Edit();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Edit"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Close
	: public CAiActionDef
{
public:
	CAiAction_OM_Close(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Close();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Close"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_RegItem
	: public CAiActionDef
{
public:
	CAiAction_OM_RegItem(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_RegItem();
	virtual std::wstring Print() const { return _T("CAiAction_OM_RegItem"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_DeregItem
	: public CAiActionDef
{
public:
	CAiAction_OM_DeregItem(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_DeregItem();
	virtual std::wstring Print() const { return _T("CAiAction_OM_DeregItem"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_ReadDealing
	: public CAiActionDef
{
public:
	CAiAction_OM_ReadDealing(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_ReadDealing();
	virtual std::wstring Print() const { return _T("CAiAction_OM_ReadDealing"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Search
	: public CAiActionDef
{
public:
	CAiAction_OM_Search(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Search();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Search"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Buy
	: public CAiActionDef
{
public:
	CAiAction_OM_Buy(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Buy();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Buy"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Enter
	: public CAiActionDef
{
public:
	CAiAction_OM_Enter(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Enter();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Enter"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

class CAiAction_OM_Exit
	: public CAiActionDef
{
public:
	CAiAction_OM_Exit(LPCTSTR lpszValue);
	virtual ~CAiAction_OM_Exit();
	virtual std::wstring Print() const { return _T("CAiAction_OM_Enter"); }
public:
	virtual EDoAction_Return DoAction(CDummyClient *pkDummy);
	void OnBegin(CDummyClient *pkDummy);

protected:
};

#define g_kAiActionMgr SINGLETON_STATIC(PgAiActionManager)

