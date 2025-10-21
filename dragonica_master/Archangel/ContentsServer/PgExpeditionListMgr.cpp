#include "stdafx.h"
#include "PgExpeditionListMgr.h"
#include "PgServerSetMgr.h"

PgExpeditionListMgr::PgExpeditionListMgr()
{
}

PgExpeditionListMgr::~PgExpeditionListMgr()
{
}

bool PgExpeditionListMgr::ProcessMsg(BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_Mutex);

	PACKET_ID_TYPE Type = 0;
	Packet.Pop(Type);
	switch(Type)
	{
	case PT_C_N_REQ_LIST_EXPEDITION:		// 클라로 원정대 정보 리스트 보냄.
		{
			BM::GUID CharGuid;
			Packet.Pop(CharGuid);

			BM::Stream SendPacket(PT_N_C_ANS_LIST_EXPEDITION);
			WriteExpeditionListInfoToPacket(Packet, SendPacket);

			g_kRealmUserMgr.Locked_SendToUser(CharGuid, SendPacket, false);
		}break;
	case PT_T_N_REQ_ADD_EXPEDITION_INFO:	// 원정대 정보 추가.
		{
			SExpeditionList Info;
			Info.ReadFromPacket(Packet);

			CreateExpeditionList(Info);
		}break;
	case PT_T_N_REQ_DEL_EXPEDITION_INFO:	// 원정대 정보 삭제.
		{
			BM::GUID ExpeditionGuid;
			Packet.Pop(ExpeditionGuid);

			DeleteExpeditionList(ExpeditionGuid);
		}break;
	case PT_T_N_NFY_MODIFY_EXPEDITION_INFO:		// 원정대 정보 변경.
		{
			SExpeditionList Info;
			Info.ReadFromPacket(Packet);

			ModifyExpeditionList(Info);
		}break;
	case PT_C_N_REQ_JOIN_EXPEDITION:		// 원정대 리스트로 가입.
		{
			BM::GUID ExpeditionGuid;
			Packet.Pop(ExpeditionGuid);
			
			JoinExpeditionList(ExpeditionGuid, Packet);
		}break;
	case PT_N_C_ANS_JOIN_EXPEDITION:	// 가입 신청 결과 실패 했을 경우. 실패 메세지를 신청자에게 보냄.
		{								// 원정대장이 거절한게 아니고 다른 이유로 인해 가입 신청을 할 수 없는 경우.
			BM::GUID CharGuid, ExpeditionGuid;
			HRESULT JoinResult;
			Packet.Pop(JoinResult);
			Packet.Pop(ExpeditionGuid);
			Packet.Pop(CharGuid);

			BM::Stream AnsPacket(PT_N_C_ANS_JOIN_EXPEDITION);
			AnsPacket.Push(JoinResult);
			AnsPacket.Push(ExpeditionGuid);
			HRESULT Ret = g_kRealmUserMgr.Locked_SendToUser(CharGuid, AnsPacket, false);
		}break;
	case PT_C_N_ANS_JOIN_EXPEDITION:
		{
			AnsJoinExpeditionList(Packet);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("invalid packet type = ") << Type );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}
	}

	return true;
}

bool PgExpeditionListMgr::CreateExpeditionList( SExpeditionList const & ListInfo )
{
	SExpeditionList * pExpeditionInfo = m_ExpeditionListPool.New();

	if( NULL == pExpeditionInfo )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pExpeditionInfo->Clear();

	auto Ret = m_ExpeditionListHash.insert( std::make_pair( ListInfo.ExpeditionGuid, pExpeditionInfo ) );
	if( !Ret.second )
	{
		m_ExpeditionListPool.Delete(pExpeditionInfo);

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 정보 채움.
	*pExpeditionInfo = ListInfo;

	return true;
}

bool PgExpeditionListMgr::DeleteExpeditionList( BM::GUID const & ExpeditionGuid )
{
	ExpeditionListHash::iterator del_iter = m_ExpeditionListHash.find( ExpeditionGuid );
	if( del_iter == m_ExpeditionListHash.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ExpeditionListHash::mapped_type pExpeditionList = (*del_iter).second;
	if( pExpeditionList )
	{
		m_ExpeditionListHash.erase( ExpeditionGuid );
		pExpeditionList->Clear();
		m_ExpeditionListPool.Delete(pExpeditionList);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionListMgr::JoinExpeditionList( BM::GUID const & ExpeditionGuid, BM::Stream & Packet )
{
	ExpeditionListHash::iterator del_iter = m_ExpeditionListHash.find( ExpeditionGuid );
	if( del_iter == m_ExpeditionListHash.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ExpeditionListHash::mapped_type pExpeditionList = (*del_iter).second;
	if( pExpeditionList )
	{
		PgDoc_Player MasterPlayer;
		g_kRealmUserMgr.Locked_GetDoc_Player( pExpeditionList->MasterGuid, false, MasterPlayer);

		if( 0 != MasterPlayer.GetChannel() )
		{
			BM::GUID JoinCharGuid;
			Packet.Pop(JoinCharGuid); // 가입 신청한 캐릭터 GUID.

			SContentsUser JoinUser;
			HRESULT Ret = g_kRealmUserMgr.Locked_GetPlayerInfo(JoinCharGuid, false, JoinUser);
			if( S_OK == Ret )
			{
				BM::Stream JoinPacket(PT_C_N_REQ_JOIN_EXPEDITION); // 해당 센터 서버로 보냄.
				JoinPacket.Push(ExpeditionGuid);
				JoinPacket.Push(JoinCharGuid);
				JoinPacket.Push((int)EJT_CHANMOVE);
				JoinUser.WriteToPacket(JoinPacket);

				SendToGlobalPartyMgr(MasterPlayer.GetChannel(), JoinPacket);
			}
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionListMgr::ModifyExpeditionList(SExpeditionList const & ListInfo)
{
	BM::GUID ExpeditionGuid = ListInfo.ExpeditionGuid;

	ExpeditionListHash::iterator list_iter = m_ExpeditionListHash.find(ExpeditionGuid);
	if( list_iter != m_ExpeditionListHash.end() )
	{
		ExpeditionListHash::mapped_type & Element = (*list_iter).second;
		(*Element) = ListInfo;		// 새로 바뀐정보로 바꿈.
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgExpeditionListMgr::AnsJoinExpeditionList(BM::Stream & Packet)
{
	BM::GUID ExpeditionGuid, CharGuid, MasterGuid;
	bool JoinResult = false;

	Packet.Pop(ExpeditionGuid);
	Packet.Pop(CharGuid);
	Packet.Pop(MasterGuid);
	Packet.Pop(JoinResult);

	ExpeditionListHash::iterator iter = m_ExpeditionListHash.find(ExpeditionGuid);
	if( iter == m_ExpeditionListHash.end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	ExpeditionListHash::mapped_type pExpeditionList = (*iter).second;
	if( pExpeditionList )
	{
		PgDoc_Player JoinPlayer;
		bool Ret = g_kRealmUserMgr.Locked_GetDoc_Player(CharGuid, false, JoinPlayer);
		if( true == Ret )
		{
			if( true == JoinResult )
			{
				SChannelMapMove MoveInfo;
				Packet.Pop(MoveInfo.iChannelNo);
				Packet.Pop(MoveInfo.iGroundNo);

				// 스위치로 보냄.
				BM::Stream MovePacket(PT_M_L_TRY_LOGIN_CHANNELMAPMOVE);
				MovePacket.Push(CharGuid);
				MovePacket.Push(MoveInfo);
				g_kRealmUserMgr.Locked_SendToSwitch(JoinPlayer.GetMemberGUID(), MovePacket);

				// 클라로 보냄.
				BM::Stream UserPacket(PT_M_C_TRY_LOGIN_CHANNELMAPMOVE);
				UserPacket.Push(MoveInfo);
				g_kRealmUserMgr.Locked_SendToUser(CharGuid, UserPacket, false);

				// 가입할 원정대 로비 맵으로 보냄. 가입할 캐릭터의 로딩이 끝나면 원정대에 가입을 시켜야 됨.
				BM::Stream RegistPacket(PT_N_M_NFY_JOIN_EXPEDITION_AWAITER);
				RegistPacket.Push(CharGuid);						// 가입할 유저 GUID.
				RegistPacket.Push(pExpeditionList->ExpeditionGuid);	// 가입할 원정대 GUID.
				g_kRealmUserMgr.Locked_SendToUserGround(pExpeditionList->MasterGuid, RegistPacket, false, true);
			}
			else
			{
				PgDoc_Player MasterPlayer;
				bool Ret = g_kRealmUserMgr.Locked_GetDoc_Player(MasterGuid, false, MasterPlayer);
				if( Ret )
				{
					BM::Stream FailPacket(PT_N_C_ANS_JOIN_EXPEDITION);	// 실패 메세지 전송.
					FailPacket.Push((int)PRC_Fail_Refuse);
					FailPacket.Push(pExpeditionList->ExpeditionGuid);
					FailPacket.Push(MasterPlayer.GetID());
					FailPacket.Push(MasterPlayer.Name());

					g_kRealmUserMgr.Locked_SendToUser(CharGuid, FailPacket, false);
				}
			}

			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgExpeditionListMgr::WriteExpeditionListInfoToPacket(BM::Stream & Packet, BM::Stream & SendPacket)
{
	ExpeditionListForEach( SFunctionForEach(Packet, SendPacket) );
}

void PgExpeditionListMgr::ExpeditionListForEach(SFunctionForEach & Function)
{
	Function(m_ExpeditionListHash);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void PgExpeditionListMgr::SFunctionForEach::operator ()(const PgExpeditionListMgr::ExpeditionListHash &ContExpeditionList)
{
	int Continent = 0;

	m_Packet.Pop(Continent);

	size_t const RetWrPos = m_SendPacket.WrPos();
	m_SendPacket.Push(PRC_Fail);
	int Count = 0;

	ExpeditionInfoList InfoList;	// 클라로 보낼 컨테이너.
	ExpeditionListHash::const_iterator iter = ContExpeditionList.begin();
	while( iter != ContExpeditionList.end() )
	{
		ExpeditionListHash::mapped_type const pExpeditionInfo = iter->second;
		if( pExpeditionInfo && pExpeditionInfo->Option.GetState() )
		{
			if( pExpeditionInfo->Option.GetOptionPublicTitle() == EOT_Public
				&& ((pExpeditionInfo->Option.GetContinent() == Continent) || (0 == Continent)) )	// 0 == Continent : 전체.
			{
				SExpeditionListInfo Info;
				Info = (*pExpeditionInfo);

				InfoList.push_back(Info);
				++Count;
			}
		}
		++iter;
	}

	if( 0 < Count )
	{
		BYTE const Ret = PRC_Success;
		m_SendPacket.ModifyData(RetWrPos, &Ret, sizeof(Ret));
		PU::TWriteArray_M(m_SendPacket, InfoList);
	}
}