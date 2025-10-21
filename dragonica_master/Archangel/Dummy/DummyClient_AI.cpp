#include "StdAfx.h"
#include "Variant/Global.h"
#include "ai_Action.h"
#include "DummyClient.h"
#include "VirtualWorld.h"


/*
bool CALLBACK CDummyClient::ProcessAI_Game()
{
	if(AiActionTypeSecond() != NS_IN_GAME)
	{
		return false;
	}

	switch( AiType() )
	{
	case E_AI_TYPE_DEFAULT:
		{
			DoChat();//-=> 지금은 채팅만.
	//		DoAction();
			DoMapMove();
		}break;
	case E_AI_TYPE_PVP:
		{
			DoPvP();
		}break;
	}

	return true;
}

void CDummyClient::DoChat()
{
	static std::wstring kArray[] = 
	{
		_T("1234567890abcdefghijklmnopqrstuwxyz~!@#$%^&*()_+"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	 _T("1234567890abcdefghijklmnopqrstuwxyzㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ가나다라마바사아자차카타파하갸냐댜랴먀"),
	};

	static std::wstring kPvPChat[] =
	{
		_T("빨리 시작해"),
		_T("게임 시작 안하냐?"),
		_T("인내심을 시험하는 것이군...."),
		_T("겁나서 게임 시작 안하는 거지?"),
		_T("시작 버튼 빨리 눌러!!!"),

		_T("빨리 시작해"),
		_T("게임 시작 안하냐?"),
		_T("인내심을 시험하는 것이군...."),
		_T("겁나서 게임 시작 안하는 거지?"),
		_T("시작 버튼 빨리 눌러!!!"),
	};

	std::wstring kContents;

	int iRet = BM::Rand_Index(10);
	switch ( AiType() )
	{
	case E_AI_TYPE_DEFAULT:
		{
			kContents = kArray[iRet];
		}break;
	case E_AI_TYPE_PVP:
		{
			kContents = kPvPChat[iRet];
		}break;
	}
	

//	BM::Stream kPacket(PT_C_M_REQ_CHAT);

	BM::Stream kPacket(PT_C_M_REQ_CHAT_FOR_STRESS_TEST);
	kPacket.Push((BYTE)CT_Normal);//바이트로 잘라낸다.
	kPacket.Push(kContents);
	SendToSwitch(kPacket);

//	static int i = 0;
//	INFO_LOG(BM::LOG_LV2, _T("Chatting [%d]"), i++);
}

HRESULT CDummyClient::AttackAction( SActionInfo &kInfo )
{
	kInfo.guidPilot = UnitID();

	POINT3 ptPos;
	g_kVWorld.GetData( UnitID(), UDT_LAST_POS, ptPos );

	POINT3 ptEnemyPos;
	if ( FAILED(g_kVWorld.GetData( m_kTarget, UDT_LAST_POS, ptEnemyPos )) )
	{
		if ( !g_kVWorld.FindEnemy( UnitID(), UT_PLAYER, m_kTarget ) )
		{
			return E_FAIL;
		}
	}

	float fLength = POINT3::Distance( ptPos, ptEnemyPos );
	if ( fLength > 50.0f )
	{
		POINT3 ptNomal = ptPos - ptEnemyPos;
		ptNomal.Normalize();

		kInfo.bIsDown = false;
		kInfo.byDirection = 2;
		POINT3_2_POINT3BY( ptNomal, kInfo.ptDirection );	// 서버에서 Player의 위치를 예상해 내기 위한 방향vector

		ptPos.x += rand()%100;
		ptPos.y += rand()%100;
		kInfo.ptPos = ptPos;

		kInfo.ptPathNormal = POINT2BY(-117, -22);
		kInfo.dwActionTerm = rand()%1000;
		kInfo.iActionID = 100005426;
		kInfo.iActionInstanceID = 43;
		kInfo.byActionParam = 2;
		kInfo.dwTimeStamp = 0;
		return S_OK;
	}
	return S_FALSE;
}

HRESULT CDummyClient::MakeAction(const int iCase, SActionInfo &kInfo)
{
	kInfo.guidPilot = UnitID();
	
	POINT3 ptPos;
	g_kVWorld.GetData(UnitID(), UDT_LAST_POS, ptPos);
	
	switch(iCase)
	{
	case 0:
		{
			kInfo.bIsDown = false;
			kInfo.byDirection = 2;
			kInfo.ptDirection = POINT3BY(-22, 117, 0);	// 서버에서 Player의 위치를 예상해 내기 위한 방향vector
			
			ptPos.x += rand()%100;
			ptPos.y += rand()%100;
			kInfo.ptPos = ptPos;

			kInfo.ptPathNormal = POINT2BY(-117, -22);
			kInfo.dwActionTerm = rand()%1000;
			kInfo.iActionID = 100005426;
			kInfo.iActionInstanceID = 43;
			kInfo.byActionParam = 2;
			kInfo.dwTimeStamp = 0;
		}break;
	case 1:
		{
			kInfo.bIsDown =	true;
			kInfo.byDirection	= 0;
			kInfo.ptDirection = POINT3BY(0, 0, 0);
			
			ptPos.x += rand()%100;
			ptPos.y += rand()%100;
			kInfo.ptPos = ptPos;

			kInfo.ptPathNormal = POINT2BY(-117, -22);
			kInfo.dwActionTerm = rand()%1000;
			kInfo.iActionID = 102100101;
			kInfo.iActionInstanceID = 174;
			kInfo.byActionParam = 2;
			kInfo.dwTimeStamp = 0;
		}break;
	case 2:
		{
		}break;
	}
	
	return S_OK;
}

void CDummyClient::ReqMapMove(const int map_no, const int spawn_no)
{
	AiActionTypeSecond(NS_IN_GAME_MAP_MOVE);

    if(spawn_no)
	{
		BM::Stream kPacket(PT_C_M_REQ_MAP_MOVE);
    	kPacket.Push(map_no);
    	kPacket.Push(spawn_no);
		kPacket.Push(BM::GUID::NullData());
		SendToSwitch(kPacket);
	}
	else
	{
   		BM::Stream kPacket(PT_C_M_REQ_DEFAULT_MAP_MOVE);
		SendToSwitch(kPacket);
	}
}
*/

/*
void CDummyClient::DoAction()
{
	SActionInfo kInfo;
	
	switch ( AiType() )
	{
	case E_AI_TYPE_DEFAULT:
		{
			MakeAction(rand()%2, kInfo);
		}break;
	case E_AI_TYPE_PVP:
		{
			if ( S_OK != AttackAction( kInfo ) )
			{
				MakeAction(rand()%2, kInfo);
			}
		}break;
	}
	
//	MakeAction(1, kInfo);

	BM::Stream kPacket(PT_C_M_REQ_ACTION2, kInfo);
	kPacket.Push((DWORD)0);
	kPacket.Push((BYTE)0);

//	SendToSwitch(kPacket);
}
*/

/*
void CDummyClient::DoMapMove()
{
	TB_REF_COUNT kLock;
	CONT_DEFMAP const *pkCont;
	g_kTblDataMgr.GetContDef(kLock, pkCont);
 	{
		size_t const No1 = 9010300;
		size_t const No2 = 9018100;

		if(rand()%1)
		{
			_DETAIL_LOG INFO_LOG(BM::LOG_LV2, _T("ReqMapMove 9018100"));
			ReqMapMove(9018100, 1);	
		}
		else
		{
			_DETAIL_LOG INFO_LOG(BM::LOG_LV1, _T("ReqMapMove 9010300"));
			ReqMapMove(9010300, 1);	
		}
	}
}
*/

/*
void CDummyClient::DoPvP()
{
	switch( AiStep() )
	{
	case E_AI_STEP_NONE:
		{
			SendToSwitch( BM::Stream(PT_C_M_REQ_JOIN_LOBBY) );
			AiStep(E_AI_STEP_PVP_JOINING);
		}break;
	case E_AI_STEP_PVP_JOINING:
		{
		}break;
	case E_AI_STEP_PVP_LOBBY:
		{
			std::vector<TBL_KEY_INT> kTempRoomList;
			CONT_PVPROOM_LIST::const_iterator room_itr;
			for ( room_itr=m_kContPvPRoomList.begin(); room_itr!=m_kContPvPRoomList.end(); ++room_itr )
			{
				if (	(room_itr->second.m_kStatus == ROOM_STATUS_LOBBY)
					&&	!room_itr->second.m_bPwd
					&&	(room_itr->second.m_ucMaxUser > room_itr->second.m_ucNowUser )
					)
				{
					kTempRoomList.push_back(room_itr->first);
				}
			}

			if ( kTempRoomList.size() )
			{
				std::random_shuffle( kTempRoomList.begin(), kTempRoomList.end() );

				BM::Stream kPacket( PT_C_PM_REQ_JOIN_ROOM, kTempRoomList.begin()->kKey );
				kPacket.Push(std::string());
				SendToSwitch(kPacket);
			}
		}break;
	case E_AI_STEP_PVP_WAITROOM:
		{
			SendToSwitch(BM::Stream(PT_C_PM_REQ_GAME_READY));
			AiStep(E_AI_STEP_PVP_READYROOM);
		}break;
	case E_AI_STEP_PVP_MASTERROOM:
		{
			DoChat();
			SendToSwitch(BM::Stream(PT_C_PM_REQ_GAME_READY));
		}break;
	case E_AI_STEP_PVP_READYROOM:
		{
			DoChat();
		}break;
	case E_AI_STEP_PVP_PLAYING:
		{

		}break;
	case E_AI_STEP_PVP_RESULT:
		{

		}break;
	}
}
*/