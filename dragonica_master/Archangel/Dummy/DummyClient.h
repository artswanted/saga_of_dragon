#pragma once
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgPlayer.h"
#include "Lohengrin/PacketStruct2.h"

typedef std::map< BM::GUID, CUnit* > CONT_UNIT;

typedef enum
{
	E_AI_TYPE_DEFAULT,
	E_AI_TYPE_PVP,
}E_AI_TYPE;

/*
typedef enum
{
	E_AI_STEP_NONE				= 0,

	E_AI_STEP_PVP_JOINING		= 1,
	E_AI_STEP_PVP_LOBBY			= 2,
	E_AI_STEP_PVP_WAITROOM		= 3,
	E_AI_STEP_PVP_READYROOM		= 4,
	E_AI_STEP_PVP_MASTERROOM	= 5,
	E_AI_STEP_PVP_EXITROOM		= 6,
	E_AI_STEP_PVP_PLAYING		= 7,
	E_AI_STEP_PVP_RESULT		= 8,
}E_AI_STEP;
*/

// Dummy Abil
typedef enum
{
	AT_DummyAbil_Begin = 40100,
	AT_ReqMapMove_Index	= 40101,
	AT_ReqMissionMove_Index = 40201,
} EDummyAbilList;

class CDummyClient :
	public PgPlayer
{
public:
	CDummyClient();
	virtual ~CDummyClient();

public:
	void Clear();

	void SetAuth(const std::wstring &strID, const std::wstring &strPW);
	void DoLoginProcess(const std::wstring &strID, const std::wstring &strPW);
	EDoAction_Return DoTrade(EAiActionType_Second const eState,BM::Stream * const pkPacket, BM::GUID const &rkOtherGuid = BM::GUID::NullData());

public:
	bool OnConnectFromLogin(const bool bIsSuccess);
	bool OnConnectFromSwitch(const bool bIsSuccess);
	bool OnRecvFromLogin(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	bool OnRecvFromSwitch(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void Terminate();

protected:

	void CheckStateAntiHack(BM::Stream& rkPacket, BM::GUID const& rkSwitchGuid);
	
	void TryAuth();
	void DoReqCharacterList();
	void DoSelectCharacter(CUnit *pkUnit);
	void DoMapLoadComplete(BYTE const cCause, SGroundKey const &rkGroundKey);
	void Recv_PT_M_C_NFY_ITEM_CHANGE(BM::Stream *pkPacket);

protected:
	CLASS_DECLARATION_S(std::wstring, MemberID);
	CLASS_DECLARATION_S(std::wstring, MemberPW);
	CLASS_DECLARATION_S(BM::GUID, Guid);//개체 ID
	CLASS_DECLARATION_S(BM::GUID, SessionGuid);//세션 ID

	CLASS_DECLARATION_S(CEL::SESSION_KEY, LoginSessionKey);//로그인 서버 세션 ID
	CLASS_DECLARATION_S(CEL::SESSION_KEY, SwitchSessionKey);//스위치 서버 세션 ID
	
	CLASS_DECLARATION_S(BM::GUID, SwitchKey);//스위치 서버 Key-> 이거 틀리면 접속 거부
	
	CLASS_DECLARATION_S(int, ServerSetNo);//접속된채널 
	CLASS_DECLARATION_S(int, ChannelNo);//논리채널
//	CLASS_DECLARATION_S(int, GameState);// 이건 필요한지 모르겠다.
	CLASS_DECLARATION_S(E_AI_Pattern, AiPattern);
	
	CLASS_DECLARATION_S(EAiActionType_Second, AiActionTypeSecond);
	CLASS_DECLARATION_S(DWORD, NextTickTime);	// 다음 Tick 예상시간 (이때까지 AiTick을 하지 않는다.)
	CLASS_DECLARATION_S(unsigned short, ActionStep);

	//이게 세션을 가져야되나?..

	bool SendToLogin(const BM::Stream &rkPacket)const;
	bool SendToSwitch(const BM::Stream &rkPacket)const;

protected:
//	PgChannelManager m_kChnnelMgr;

	mutable Loki::Mutex m_kMutex;
	CLASS_DECLARATION_S(DWORD, ActionBeginTime);
	CLASS_DECLARATION_S(unsigned short, BeforeActionStep);
	CLASS_DECLARATION_S(BM::GUID, UnitID);

	//CLASS_DECLARATION_S(E_AI_TYPE, AiType);
	//CLASS_DECLARATION_S(E_AI_STEP, AiStep);

	CONT_PVPROOM_LIST	m_kContPvPRoomList;

	BM::GUID			m_kTarget;
	CONT_UNIT			m_kContUnit;

	void ClearMarketList(){m_kContMarketList.clear();}
	CLASS_DECLARATION_S(CONT_MARKET_KEY_LIST,	ContMarketList);
	CLASS_DECLARATION_S(BM::GUID, EnterdMarketGuid);
	CLASS_DECLARATION_S(SUserMarketArticleList, ContMarketArticle);

	CLASS_DECLARATION_S(SUserMarketArticleList, ContMyArticle);
	CLASS_DECLARATION_S(SUserMarketDealingList, ContMyDealing);
};
