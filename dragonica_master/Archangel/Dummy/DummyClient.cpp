#include "StdAfx.h"
#include "ai_Action.h"
#include "DummyClient.h"
#include "virtualWorld.h"
#include "Network.h"

CDummyClient::CDummyClient()
{
	AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
	ActionStep(0);
	BeforeActionStep(999);
	ActionBeginTime(0);
	NextTickTime(0);
}

CDummyClient::~CDummyClient()
{
}

void CDummyClient::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	//INFO_LOG( BM::LOG_LV5, _T("[%s] Clear"), MemberID().c_str() );
	AiActionTypeSecond(NS_NONE);
	//AiType(E_AI_TYPE_DEFAULT);
	//AiStep(E_AI_STEP_NONE);
	m_kTarget.Clear();

	Terminate();

	NextTickTime(::timeGetTime() + 1000);
	AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
	ActionBeginTime(0);
}

/*
bool CALLBACK CDummyClient::ProcessAI()
{
	if ( NS_NONE == AiActionTypeSecond() )
	{
		return false;
	}

	ProcessAI_Login();
	if ( BM::TimeCheck( m_kLastAITime, 500 ) )
	{//1초에 한번씩 액션!
		ProcessAI_Game();
	}
	return true;
}
*/

/*
bool CALLBACK CDummyClient::ProcessAI_Login()
{
	switch(AiActionTypeSecond())
	{
	case NS_BEFORE_LOGIN_CONNECT:
		{// 로긴전
			TryConnectLogin(true);
		}break;
	default:
		{
			return false;
		}break;
	}

	return true;
}
*/

void CDummyClient::SetAuth(const std::wstring &strID, const std::wstring &strPW)
{
	BM::CAutoMutex kLock(m_kMutex);
	MemberID(strID);
	MemberPW(strPW);
	AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
}

void CDummyClient::TryAuth()
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::Stream kPacket;
	switch(AiActionTypeSecond())
	{
	case NS_LOGIN_CONNECT_SUCCESS_FOR_AUTH://
		{
			AiActionTypeSecond(NS_TRY_AUTH);

			SClientTryLogin CTL;
			CTL.SetID( MemberID() );
			CTL.SetPW( MemberPW() );
			CTL.SetVersion( PACKET_VERSION_C, PACKET_VERSION_S );
			CTL.PatchVersion(g_kNetwork.GetPatchVersion().iVersion);
//			CTL.ChannelNo(g_nChannelNo);
//			CTL.LogicalChannelNo(0);

			kPacket.Push(PT_C_L_TRY_AUTH);
			kPacket.Push(CTL);
/*			
			int i = 600000;
			while(i--)
			{//부하를 위해서.
				kPacket.Push((__int64)BM::Rand_Index(4000000));
				kPacket.Push((__int64)BM::Rand_Index(4000000));
				kPacket.Push((__int64)BM::Rand_Index(4000000));
				kPacket.Push((__int64)BM::Rand_Index(4000000));
				kPacket.Push((__int64)BM::Rand_Index(4000000));
				kPacket.Push((__int64)BM::Rand_Index(4000000));
			}
//*/
			_DETAIL_LOG INFO_LOG(BM::LOG_LV3, __FL__<<L"TryAuth ID["<<MemberID()<<L"] PW["<<MemberPW()<<L"]");

//			SendToLogin(kPacket);
//			SendToLogin(kPacket);
//			SendToLogin(kPacket);
//			SendToLogin(kPacket);
//			SendToLogin(kPacket);
			
//			kPacket.SetStopSignal(true);
			SendToLogin(kPacket);
		}break;
	case NS_LOGIN_CONNECT_SUCCESS_FOR_LOGIN:
		{
			AiActionTypeSecond(NS_TRY_LOGIN);
			//iLogicalChannelNo
			SClientTryLogin CTL;
			CTL.SetID( MemberID() );
			CTL.SetPW( MemberPW() );
			CTL.SetVersion( PACKET_VERSION_C, PACKET_VERSION_S );
			CTL.PatchVersion(1);
//			CTL.ChannelNo(g_nChannelNo);
//			CTL.LogicalChannelNo(g_iLogicalChannelNo);

			kPacket.Push(PT_C_L_TRY_LOGIN);
			kPacket.Push(CTL);
			
			_DETAIL_LOG INFO_LOG(BM::LOG_LV3, __FL__<<L"TryLogin ID["<<MemberID()<<L"] PW["<<MemberPW()<<L"]");
			SendToLogin(kPacket);
		}break;	
	}
}


void CDummyClient::Terminate()
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::Stream kPacket;
	kPacket.SetStopSignal(true);

	if( BM::GUID::IsNotNull(SwitchSessionKey().SessionGuid()) )
	{
		SendToSwitch(kPacket);
	}

	if( BM::GUID::IsNotNull(LoginSessionKey().SessionGuid()) )
	{
		SendToLogin(kPacket);
	}
	CEL::SESSION_KEY kKey(g_kNetwork.LoginConnector(), BM::GUID::NullData());
	LoginSessionKey(kKey);
	CEL::SESSION_KEY kSwitchKey(g_kNetwork.SwitchConnector(), BM::GUID::NullData());
	SwitchSessionKey(kSwitchKey);
}

void CDummyClient::DoMapLoadComplete(BYTE const cCause, SGroundKey const &rkGroundKey)
{
	BM::CAutoMutex kLock(m_kMutex);
	_DETAIL_LOG INFO_LOG(BM::LOG_LV9, _T("Sending PT_C_M_NFY_MAPLOADED"));

	bool bMapMove = false;
	switch ( AiPattern() )
	{
	case E_AiPattern_Repeat_MapMove:
		{
			// MapMove 상태라면
			if (AiActionTypeSecond() == E_AI_STEP_MAPMOVE_REQ)
			{
				AiActionTypeSecond(E_AI_STEP_MAPMOVE_RECVED);
				bMapMove = true;
			}
			_DETAIL_LOG INFO_LOG(BM::LOG_LV9, __FL__<<L"MapMove completed");
		}break;
	case E_AiPattern_Repeat_MissionMove:
		{
			if ( AiActionTypeSecond() == E_AI_STEP_MISSIONMOVE_REQ )
			{
				AiActionTypeSecond(E_AI_STEP_MAPMOVE_RECVED);
				bMapMove = true;
			}
		}break;
	case E_AiPattern_Repeat_PvPMove:
		{
			if ( AiActionTypeSecond() == E_AI_STEP_PVPMOVE_REQOUT )
			{
				AiActionTypeSecond(E_AI_STEP_PVPMOVE_END);
				bMapMove = true;
			}
		}break;
	}

	BM::Stream kPacket( PT_C_M_NFY_MAPLOADED, (BYTE)cCause );
	rkGroundKey.WriteToPacket(kPacket);
	SendToSwitch(kPacket);

	if (!bMapMove)
	{
		AiActionTypeSecond(NS_IN_GAME);
	}
	else
	{
		// 맵로딩이 끝났으면 액션을 한번 보내야 함
		CAiAction_PsedoAction kAction( _T("1000") );
		kAction.DoAction( this );
	}
	//DoAction();
}

bool CDummyClient::SendToLogin(const BM::Stream &rkPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);
	const BM::Stream::STREAM_DATA &rkData = rkPacket.Data();
	
	if(rkData.size())
	{
		const BM::Stream::DEF_STREAM_TYPE kType = *(const BM::Stream::DEF_STREAM_TYPE*)&rkData.at(0);
	//	INFO_LOG(BM::LOG_LV3, _T("L - SendPacket Type[%d]=[0x%X] Size=[%d]"), kType, kType, pkPacket->Size());
	}

	if ( LoginSessionKey().WorkerGuid() == BM::GUID::NullData() 
		|| LoginSessionKey().SessionGuid() == BM::GUID::NullData() )
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"SessionKey is invalid");
	}
	g_kNetwork.SendToLogin(LoginSessionKey(), rkPacket);
	return false;
}

bool CDummyClient::SendToSwitch(const BM::Stream &rkPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);
	const BM::Stream::STREAM_DATA &rkData = rkPacket.Data();

	if(rkData.size())
	{
//	const BM::Stream::DEF_STREAM_TYPE kType = *(const BM::Stream::DEF_STREAM_TYPE*)&rkData.at(0);
//	INFO_LOG(BM::LOG_LV3, _T("S - SendPacket Type[%d]=[0x%X] Size=[%d]"), kType, kType, pkPacket->Size());
	}
	g_kNetwork.SendToSwitch(SwitchSessionKey(), rkPacket);
	return false;
}

bool CDummyClient::OnConnectFromLogin(const bool bIsSuccess)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(bIsSuccess)
	{//접속 시도.
		_DETAIL_LOG	INFO_LOG(BM::LOG_LV1, _T("Login - Try Connect Success"));
		switch(AiActionTypeSecond())
		{
		case NS_LOGIN_CONNECT_TRY_FOR_AUTH:
			{
				AiActionTypeSecond(NS_LOGIN_CONNECT_SUCCESS_FOR_AUTH);
			}break;
		case NS_LOGIN_CONNECT_TRY_FOR_LOGIN:
			{
				AiActionTypeSecond(NS_LOGIN_CONNECT_SUCCESS_FOR_LOGIN);
			}break;
		default:
			{
				assert(NULL);
			}break;
		}
	}
	else
	{//접속 실패.
		_DETAIL_LOG INFO_LOG(BM::LOG_LV0, _T("Login - Try Connect Failed"));
		AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
		CEL::SESSION_KEY kKey(g_kNetwork.LoginConnector(), BM::GUID::NullData());
		LoginSessionKey(kKey);
	}
	return true;
}

void CDummyClient::CheckStateAntiHack(BM::Stream& rkPacket, BM::GUID const& rkSwitchGuid)
{
	unsigned long const ulRet = 0;

	BM::Stream kAnsPacket;
	if ( BM::GUID::IsNotNull( rkSwitchGuid ) )
	{
		kAnsPacket.Push( PT_C_S_TRY_ACCESS_SWITCH_SECOND );
		kAnsPacket.Push( rkSwitchGuid );
	}
	else
	{
		kAnsPacket.Push( PT_C_S_ANS_GAME_GUARD_CHECK );
	}

	kAnsPacket.Push( ulRet );
	SendToSwitch( kAnsPacket );
}

bool CDummyClient::OnConnectFromSwitch(const bool bIsSuccess)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(bIsSuccess)
	{//접속 시도.
		AiActionTypeSecond(NS_CONNECTED_SWITCH);
/*		INFO_LOG(BM::LOG_LV1, _T("Try Connect Success"));
		switch(AiActionTypeSecond())
		{
		case NS_LOGIN_CONNECT_TRY_FOR_AUTH:
			{
				AiActionTypeSecond(NS_LOGIN_CONNECT_SUCCESS_FOR_AUTH);
			}break;
		case NS_LOGIN_CONNECT_TRY_FOR_LOGIN:
			{
				AiActionTypeSecond(NS_LOGIN_CONNECT_SUCCESS_FOR_LOGIN);
			}break;
		default:
			{
				assert(NULL);
			}break;
		}
*/
	}
	else
	{//접속 실패.
		_DETAIL_LOG INFO_LOG(BM::LOG_LV0, _T("Switch - Try Connect Failed"));
		CEL::SESSION_KEY kKey(g_kNetwork.SwitchConnector(), BM::GUID::NullData());
		SwitchSessionKey(kKey);
	}
	return true;
}

bool CDummyClient::OnRecvFromLogin(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::Stream::DEF_STREAM_TYPE kType;
	pkPacket->Pop(kType);
	
//	INFO_LOG(BM::LOG_LV4, _T("L-RecvPacket Type[%d]=[0x%X] Size=[%d]"), kType, kType, pkPacket->Size());
	
	switch(kType)
	{
	case PT_A_ENCRYPT_KEY:
		{
			std::vector<char> kKey;
			pkPacket->Pop(kKey);
			
			if(S_OK == pkSession->VSetEncodeKey(kKey))
			{
				TryAuth();
			}
			else
			{
				AiActionTypeSecond(NS_BEFORE_LOGIN_CONNECT);
				pkSession->VTerminate();
			}
		}break;
	case PT_L_C_NFY_REALM_LIST:
		{
			_DETAIL_LOG INFO_LOG(BM::LOG_LV4, _T("PT_L_C_NFY_REALM_LIST"));

			g_kRealmMgr.ReadFromPacket(*pkPacket);
		
			CONT_REALM kContRealm;
			g_kRealmMgr.GetRealmCont(kContRealm);
			kContRealm.erase(0);//0번 렐름 지워.
			
			CONT_REALM::const_iterator realm_itor = kContRealm.end();
			if (g_kNetwork.GetTestInfo()->sRealm == 0)
			{
				if (S_OK != ::RandomElement(kContRealm, realm_itor))
				{
					realm_itor = kContRealm.begin();
				}
			}
			else
			{
				realm_itor = kContRealm.find(g_kNetwork.GetTestInfo()->sRealm);
			}
			if (realm_itor != kContRealm.end())
			{
				BM::Stream kPacket(PT_C_L_REQ_CHANNEL_LIST);
				kPacket.Push((*realm_itor).first);//렐름 번호 보냄.
				SendToLogin(kPacket);
				return 0;
			}

			VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("Three is No Tryable Channel"));
		}break;
	case PT_L_C_NFY_CHANNEL_LIST:
		{
			_DETAIL_LOG INFO_LOG(BM::LOG_LV4, _T("PT_L_C_NFY_CHANNEL_LIST"));

			g_kRealmMgr.ReadFromPacket(*pkPacket);
		
			CONT_REALM kContRealm;
			g_kRealmMgr.GetRealmCont(kContRealm);
			kContRealm.erase(0);//0번 렐름 지워.
			
			CONT_REALM::const_iterator realm_itor = kContRealm.end();
			if (g_kNetwork.GetTestInfo()->sRealm == 0)
			{
				if (S_OK == ::RandomElement(kContRealm, realm_itor))
				{
					CONT_CHANNEL kContChannel;
					(*realm_itor).second.GetChannelCont(kContChannel);
					CONT_CHANNEL::const_iterator channel_itor = kContChannel.end();
					if(S_OK == ::RandomElement(kContChannel, channel_itor))
					{
						if ((*channel_itor).second.IsAlive())
						{
							const short nRealmNo = (*realm_itor).second.RealmNo();
							const short nChannelNo = (*channel_itor).second.ChannelNo();

							BM::Stream kPacket(PT_C_L_TRY_LOGIN);
							kPacket.Push(nRealmNo);
							kPacket.Push(nChannelNo);
							SendToLogin(kPacket);
							return 0;
						}
						else
						{
							if (realm_itor != kContRealm.end())
							{
								if (S_OK == ::RandomElement(kContRealm, realm_itor))
								{
									BM::Stream kPacket(PT_C_L_REQ_CHANNEL_LIST);
									kPacket.Push((*realm_itor).first);//렐름 번호 보냄.
									SendToLogin(kPacket);
									return 0;
								}
							}
						}
					}
				}
			}
			else
			{
				realm_itor = kContRealm.find(g_kNetwork.GetTestInfo()->sRealm);
				if (realm_itor == kContRealm.end())
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot find Realm["<<g_kNetwork.GetTestInfo()->sRealm<<L"]");
					return 0;
				}
				CONT_CHANNEL kContChannel;
				(*realm_itor).second.GetChannelCont(kContChannel);
				CONT_CHANNEL::const_iterator channel_itor = kContChannel.find(g_kNetwork.GetTestInfo()->sChannel);
				if (channel_itor == kContChannel.end())
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot find sChannel["<<g_kNetwork.GetTestInfo()->sChannel<<L"]");
					return 0;
				}
				if ((*channel_itor).second.IsAlive())
				{
					BM::Stream kPacket(PT_C_L_TRY_LOGIN);
					kPacket.Push(g_kNetwork.GetTestInfo()->sRealm);
					kPacket.Push(g_kNetwork.GetTestInfo()->sChannel);
					SendToLogin(kPacket);
				}
				else
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"Realm["<<g_kNetwork.GetTestInfo()->sRealm<<L"]Channel["<<g_kNetwork.GetTestInfo()->sChannel<<L"] is not ALIVE");
				}
				return 0;
			}
			VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("Three is No Tryable Channel"));
		}break;
	case PT_L_C_TRY_LOGIN_RESULT:
		{
			int iError = 0;
			pkPacket->Pop(iError);
			_DETAIL_LOG INFO_LOG(BM::LOG_LV4, __FL__<<L"LoginFailed Cause["<<iError<<L"] ID=["<<MemberID()<<L"] PW=["<<MemberPW()<<L"]");
			Clear();
		}break;
	case PT_L_C_NFY_RESERVED_SWITCH_INFO:
		{// 100016 PT_L_C_NFY_RESERVED_SWITCH_INFO
			Sleep(50 + rand()%1000);

			SSwitchReserveResult kSRR;	
			pkPacket->Pop(kSRR);

			CEL::ADDR_INFO SwitchAddr;
			const struct in_addr addr = kSRR.addrSwitch.ip;
			std::string strIP = inet_ntoa(addr);

			SwitchAddr.Set(UNI(strIP), kSRR.addrSwitch.wPort );
			SwitchKey(kSRR.guidSwitchKey);

			_DETAIL_LOG INFO_LOG( BM::LOG_LV1, __FL__<<L"Switch Info Recved ["<<C2L(SwitchAddr)<<L"]");
			BM::GUID guid;

			m_kSwitchSessionKey.WorkerGuid(g_kNetwork.SwitchConnector());
			g_kNetwork.DoConnectSwitch(SwitchAddr, m_kSwitchSessionKey);
		}break;
	default:
		{
		}break;
	}
	return true;
}

bool CDummyClient::OnRecvFromSwitch(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	BM::Stream::DEF_STREAM_TYPE kType;
	pkPacket->Pop(kType);
	
//	INFO_LOG(BM::LOG_LV4, _T("S-RecvPacket Type[%d]=[0x%X] Size=[%d]"), kType, kType, pkPacket->Size());
	
	switch(kType)
	{
	case PT_M_C_NFY_STATE_CHANGE64:
		{
			BM::GUID kCharacterGuid;
			BYTE byCount;

			pkPacket->Pop(kCharacterGuid);
			
			if(this->GetID() == kCharacterGuid)
			{
				pkPacket->Pop(byCount);

				while(byCount--)
				{
					SAbilInfo64 kAbilInfo;
					pkPacket->Pop(kAbilInfo);
					this->SetAbil64(kAbilInfo.wType, kAbilInfo.iValue);
				}
			}
		}break;
	case PT_M_C_NFY_ITEM_CHANGE:
		{
			Recv_PT_M_C_NFY_ITEM_CHANGE(pkPacket);
		}break;
	case PT_M_C_UM_ANS_MARKET_ARTICLE_QUERY:
		{
			SPT_M_C_UM_ANS_MARKET_ARTICLE_QUERY kData;
			kData.ReadFromPacket(*pkPacket);
			if(kData.Result())
			{
				break;
			}
			ContMarketArticle(kData.ContArticle());
		}break;
	case PT_M_C_UM_ANS_MARKET_OPEN:
		{
			SPT_M_C_UM_ANS_MARKET_OPEN kAns;
			kAns.ReadFromPacket(*pkPacket);

			if(UMR_SUCCESS == kAns.Error())
			{
				AiActionTypeSecond(E_AI_STEP_OM_CREATE_COMPLETE);
			}
		}break;
	case PT_S_C_ANS_ACCESS_SWITCH_SECOND:
		{
			BM::GUID guidSwitchKey;
			pkPacket->Pop( guidSwitchKey );
			CheckStateAntiHack(*pkPacket, guidSwitchKey);
		}break;
	case PT_S_C_REQ_GAME_GUARD_CHECK:
		{
			CheckStateAntiHack(*pkPacket, BM::GUID::NullData());
		}break;
	case PT_A_ENCRYPT_KEY:
		{
			std::vector<char> kKey;
			pkPacket->Pop(kKey);
			
			if(S_OK == pkSession->VSetEncodeKey(kKey))
			{
				//스위치에 인증 해달라고 보냄.
				STryAccessSwitch kTAS;
				kTAS.SetVersion(PACKET_VERSION_C, PACKET_VERSION_S);
				kTAS.SetID( MemberID() );
				kTAS.SetPW( MemberPW());
				kTAS.guidSwitchKey = SwitchKey();

				BM::Stream Packet(PT_C_S_TRY_ACCESS_SWITCH, kTAS);
				pkSession->VSend( Packet );
			}
			else
			{//스위치랑 끊어지니.. Clear() 해야지.

			}
		}break;
	case PT_S_C_TRY_ACCESS_SWITCH_RESULT:
		{
			DoReqCharacterList();
		}break;
	case PT_S_C_ANS_CHARACTER_LIST:
		{
			UNIT_PTR_ARRAY kUnitArray;
			kUnitArray.ReadFromPacket(*pkPacket);
			
			if(!kUnitArray.empty())
			{
				UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin();
				DoSelectCharacter(itor->pkUnit);
			}
			else
			{
				CAUTION_LOG(BM::LOG_LV0, __FL__<<L"No Character");
			}
		}break;
	case PT_T_C_ANS_SELECT_CHARACTER://진입불가 상태임
		{
			ESelectCharacterRet eCause;
			pkPacket->Pop(eCause);
		
			INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't MapMove Cause["<<eCause<<L"]");
			switch(eCause)
			{
			case E_SCR_SUCCESS:
				{
					assert(NULL);
				}break;
			default:
				{
					BM::GUID kUnitGuid;
					pkPacket->Pop(kUnitGuid);
				}break;
			}
		}break;
	case PT_T_C_NFY_CHARACTER_MAP_MOVE://맵이동 완료
		{
			BYTE cMoveCause;
			SGroundKey kGroundKey;
			pkPacket->Pop(cMoveCause);
			pkPacket->Pop(kGroundKey);
			DoMapLoadComplete(cMoveCause, kGroundKey);
		}break;
	case PT_T_C_ANS_JOIN_LOBBY:
		{
			if ( E_AiPattern_Repeat_PvPMove == AiPattern() )
			{
				if ( AiActionTypeSecond() == E_AI_STEP_PVPMOVE_JOINNING_LOBBY )
				{
					AiActionTypeSecond(E_AI_STEP_PVPMOVE_JOINED_LOBBY);
				}
			}
		}break;
	case PT_T_C_ANS_JOIN_ROOM:
		{
			if ( E_AiPattern_Repeat_PvPMove == AiPattern() )
			{
				if ( AiActionTypeSecond() == E_AI_STEP_PVPMOVE_JOINNING_ROOM )
				{
					AiActionTypeSecond(E_AI_STEP_PVPMOVE_JOINED_ROOM);
				}
			}
		}break;
	case PT_S_C_REQ_PING://핑에 캐릭 정보가 가는 이유는??
		{
			BM::GUID kGUID;
			pkPacket->Pop(kGUID);

			{
				DWORD const dwTime = BM::GetTime32();//timegettime 으로 바꾸면, 시간이 5ms 내외에서 뒤로 돌아가거나 할 수도 있으니 주의.
				::GUID kTempGuid = kGUID;
				kTempGuid.Data1 ^= dwTime;//시간값 XOR 로 줌.
				BM::Stream kResPacket(PT_C_S_ANS_ONLY_PING, kTempGuid);
				SendToSwitch(kResPacket);
			}

			if( NS_IN_GAME == AiActionTypeSecond() )
			{
				BM::Stream kResPacket(PT_C_S_NFY_UNIT_POS, kGUID);
				kResPacket.Push( GetPos() );
				SendToSwitch(kResPacket);
			}
		}break;
	case PT_M_C_ADD_UNIT:
		{

		}break;

	case PT_M_C_RES_ACTION2:
	case PT_M_C_NFY_ACTION2:
		{//액션 패킷.
		}break;
	case PT_M_C_NFY_CHAT:
		{
		}break;
	//초기 로딩 패킷
//	case PT_M_C_ANS_QUEST:
	case PT_M_C_NFY_MAPLOADED://맵 준비됨 패킷
	case PT_N_C_NFY_MSN_ELEMENT_MODIFY:
		{
		}break;

	case PT_M_C_NFY_WARN_MESSAGE:
		{// 이동 불가 맵 
			int iMsgNo = 0;
			pkPacket->Pop(iMsgNo);
			switch(iMsgNo)
			{
			case 1://이동 불가 맵이요
				{
					AiActionTypeSecond(NS_IN_GAME);
				}break;
			}
		}break;
	case PT_M_C_UNIT_POS_CHANGE:
		{
			BYTE byType;
			BM::GUID kGuid;
			POINT3 ptPos;
			pkPacket->Pop(byType);
			pkPacket->Pop(kGuid);
			pkPacket->Pop(ptPos);
			SetPos(ptPos);
		}break;
	case PT_M_C_NFY_EXCHANGE_ITEM_REQ:
		{
			// Trade 요청
			DoTrade(E_AI_STEP_TRADE_REQ_WAIT, pkPacket);
		}break;
	case PT_M_C_NFY_EXCHANGE_ITEM:
		{
			DoTrade(E_AI_STEP_TRADE_REQ_ANS, pkPacket);
		}break;
	case PT_M_C_NFY_EXCHANGE_ITEM_ITEM:
		{
			DoTrade(E_AI_STEP_TRADE_XCHANGE, pkPacket);
		}break;
	case PT_M_C_NFY_EXCHANGE_ITEM_READY:
		{
			DoTrade(E_AI_STEP_TRADE_XCHANGE_READY, pkPacket);
		}break;
	default:
		{
//			INFO_LOG(BM::LOG_LV4, _T(" Unknown S-RecvPacket Type[%d]=[0x%X] Size=[%d]"), kType, kType, pkPacket->Size());
		}break;
	}
	return true;
}

EDoAction_Return CDummyClient::DoTrade(EAiActionType_Second const eState,BM::Stream * const pkPacket, BM::GUID const &rkOtherGuid)
{
	EDoAction_Return eReturn = E_DoAction_Finished;
	switch (eState)
	{
	case E_AI_STEP_TRADE_REQ:	// 상대방에게 요청 보낸 상태
		{
		}break;
	case E_AI_STEP_TRADE_REQ_WAIT:	// 상대방의 요청을 기다리는 상태
		{
			// Trade 요청에 대한 응답 보내기
			BM::GUID kTradeGuid;
			BM::GUID kOtherPlayer;
			ETRADERESULT eTrade;
			pkPacket->Pop(kOtherPlayer);
			pkPacket->Pop(eTrade);
			pkPacket->Pop(kTradeGuid);
			// 일부러 eTrade 값에 상관없이 정보를 보내보자.
			if (TR_REQUEST == eTrade)
			{
				// packet 이 요청자/상대방 모두에게 오므로, 구별해서 보내준다.
				BM::Stream kSendPacket(PT_C_M_NFY_EXCHANGE_ITEM_ANS, kTradeGuid);
				kSendPacket.Push((bool)true);	// Trade 승락
				SendToSwitch(kSendPacket);
			}
		}break;
	case E_AI_STEP_TRADE_REQ_ANS:	// Trade 요청에 응답한 상태
		{
			// Trade 시작할 준비 완료
			BM::GUID kTradeGuid;
			BM::GUID kOtherPlayer;
			ETRADERESULT eTrade;
			pkPacket->Pop(kTradeGuid);
			pkPacket->Pop(kOtherPlayer);
			pkPacket->Pop(eTrade);
			// 일부러 eTrade 값에 상관없이 정보를 보내보자.
			// Trade : 교환할 아이템 정보 보내기
			BM::Stream kSendPacket(PT_C_M_NFY_EXCHANGE_ITEM_ITEM);
			STradeInfoPacket kInfoPacket;
			kInfoPacket.kTradeGuid = kTradeGuid;
			if (rand() % 10 == 0)
			{
				// 가끔씩 kOwnerGuid 를 남의것으로 해서 어떻게 되나 보자.
				kInfoPacket.kOwnerGuid = kOtherPlayer;
			}
			else
			{
				kInfoPacket.kOwnerGuid = GetID();
			}
			kInfoPacket.kMoney = 100;	// 고정된 Money만 Trade 하여 나중에 Money != 100 인 Dummy를 확인하자.
			kInfoPacket.WriteToPacket(kSendPacket);
			bool bConfirmOrModify = true;
			bool bIsConfirm = true;
			kSendPacket.Push(bConfirmOrModify);
			kSendPacket.Push(bIsConfirm);
			SendToSwitch(kSendPacket);
		}break;
	case E_AI_STEP_TRADE_XCHANGE:
		{
			// 상대편 Trade 정보 읽기.
			int iResult;
			STradeInfoPacket kInfoPacket;
			pkPacket->Pop(iResult);
			kInfoPacket.ReadFromPacket(*pkPacket);
			// Trade Ready 상태 만들기
			if (kInfoPacket.kOwnerGuid == GetID())
			{
				BM::Stream kSendPacket(PT_C_M_NFY_EXCHANGE_ITEM_READY, kInfoPacket.kTradeGuid);
				SendToSwitch(kSendPacket);
			}
		}break;
	case E_AI_STEP_TRADE_CONFIRMED:	// Confirm 한 상태
		{
		}break;
	case E_AI_STEP_TRADE_XCHANGE_READY:	// Exchange Ready 도 누른상태(최종상태)
		{
			// 양쪽모두 READY 했다면, 아이템 변경 정보는 자동으로 보내질 것이다.
			_DETAIL_LOG INFO_LOG(BM::LOG_LV8, __FL__ << _T("E_AI_STEP_TRADE_XCHANGE_READY ") << Name());
			//AiActionTypeSecond(E_AI_STEP_TRADE_NONE);
		}break;
	case E_AI_STEP_TRADE_NONE:	// 처음 시작 상태
	default:
		{
			// pkOther 과 거래를 시작하자.
			BM::Stream kDPacket(PT_C_M_REQ_EXCHANGE_ITEM_REQ, rkOtherGuid);
			SendToSwitch(kDPacket);
			AiActionTypeSecond(E_AI_STEP_TRADE_REQ);
			eReturn = E_DoAction_Finished;
		}break;
	}
	return eReturn;
}


void CDummyClient::Recv_PT_M_C_NFY_ITEM_CHANGE(BM::Stream *pkPacket)
{
	EItemModifyParentEventType kEventCause = IMEPT_NONE;
	BM::GUID kGuid;
	DB_ITEM_STATE_CHANGE_ARRAY kArray;

	pkPacket->Pop(kEventCause);
	pkPacket->Pop(kGuid);
	PU::TLoadArray_M( *pkPacket, kArray);

	bool bModifyPos = false;
	bool bRefreshAbil = false;
	bool bNeedQuestUIUpdate = false;
	bool bNeedReBuildSkillTree = false;
	bool bSafeFoamUseClear = false;

	PgInventory * pkInv = GetInven();

	DB_ITEM_STATE_CHANGE_ARRAY::const_iterator item_itor = kArray.begin();
	while(kArray.end() != item_itor )
	{
		DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkCurModify = (*item_itor);

		BM::Stream kAddonData = rkCurModify.kAddonData;

		switch( rkCurModify.State() )
		{
		case DISCT_REMOVE_SYSTEM:
		case DISCT_SET_RENTALSAFETIME:
		case DISCT_SET_ACHIEVEMENT:
		case DISCT_ACHIEVEMENT2INV:
		case DISCT_INV2ACHIEVEMENT:
		case DISCT_COMPLETE_ACHIEVEMENT:
		case DISCT_MODIFY_VALUE:
		case DISCT_MODIFY_SKILL:
		case DISCT_MODIFY_CP:
		case DISCT_MODIFY_QUEST_ING: //Begin/Drop(none)/Complete(finished)/Fail만
		case DISCT_MODIFY_QUEST_END:
		case DISCT_MODIFY_QUEST_PARAM:
		case DISCT_MODIFY_SET_GUID:
			{
			}break;
		case DISCT_MODIFY_MONEY:
			{
//				__int64 iMoney = 0;
//				kAddonData.Pop(iMoney);
//				this->SetAbil64(AT_MONEY,iMoney);
			}break;
		default:
			{
				PgItemWrapper const &rkItemWrapper = rkCurModify.ItemWrapper();//Current data
				PgItemWrapper const &rkPrevItemWrapper = rkCurModify.PrevItemWrapper();//Current data
				bool const bInvModifyRet = pkInv->Modify(rkItemWrapper.Pos(), rkItemWrapper);//Apply modify item state
			}break;
		}
		
		++item_itor;
	}
}