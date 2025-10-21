#include "stdafx.h"
#include "Variant/constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "variant/PgEventview.h"
#include "BM/PgTask.h"
#include "PgPartyItemRule.h"
#include "PgStoneMgr.h"
#include "PgAction.h"
#include "PgExpeditionLobby.h"

PgExpeditionLobby::PgExpeditionLobby()
{
}

PgExpeditionLobby::~PgExpeditionLobby()
{
}

EOpeningState PgExpeditionLobby::Init(int const iMonsterControlID, bool const bMonsterGen)
{
	return PgGround::Init(iMonsterControlID, bMonsterGen);
}

T_GNDATTR PgExpeditionLobby::GetAttr() const
{
	return GATTR_EXPEDITION_LOBBY;
}

int PgExpeditionLobby::GetGroundNo() const
{
	return PgGround::GetGroundNo();
}

bool PgExpeditionLobby::Clone(PgExpeditionLobby * pGround)
{
	if( pGround )
	{
		return PgGround::Clone(dynamic_cast<PgGround*>(pGround));
	}

	return false;
}

bool PgExpeditionLobby::Clone(PgGround * pGround)
{
	if( pGround )
	{
		return Clone(dynamic_cast<PgExpeditionLobby*>(pGround));
	}

	return false;
}

void PgExpeditionLobby::Clear()
{
	PgGround::Clear();
}

void PgExpeditionLobby::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgGround::OnTick1s();
}

bool PgExpeditionLobby::VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy)
{
	switch( wType )
	{
	case PT_C_M_REQ_NPC_ENTER_EXPEDITION:
		{
			BM::Stream Packet(PT_M_C_NFY_NPC_ENTER_EXPEDITION);
			VEC_GUID FailCharGuid;
			HRESULT hRet = Recv_PT_C_M_REQ_NPC_ENTER_EXPEDITION(pUnit, *pNfy, FailCharGuid);
			if( PRC_Success != hRet )
			{
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
				if( pPlayer )
				{
					Packet.Push(hRet);
					if( (hRet == PRC_Fail_NoHaveKeyItem) && !FailCharGuid.empty() )
					{
						Packet.Push(FailCharGuid.size());
						VEC_GUID::const_iterator iter = FailCharGuid.begin();
						for( iter ; iter != FailCharGuid.end() ; ++iter )
						{
							Packet.Push(*iter);
						}
					}
					VEC_GUID Member_Guid;
					m_kLocalPartyMgr.GetExpeditionMemberGround(pPlayer->ExpeditionGuid(), GroundKey(), Member_Guid);
					SendToUser_ByGuidVec(Member_Guid, Packet);
				}
			}
		}break;
	case PT_C_M_REQ_LIST_USER_EXPEDITION:
		{
			Recv_PT_C_M_REQ_LIST_USER_EXPEDITION(pUnit, *pNfy);
		}break;
	case PT_C_N_REQ_CREATE_EXPEDITION:
		{
			BM::Stream Packet(PT_C_N_REQ_CREATE_EXPEDITION);
			Packet.Push(*pNfy);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_C_N_ANS_JOIN_EXPEDITION:
		{
			bool HaveKeyItem = false;
			BM::Stream Packet(PT_C_N_ANS_JOIN_EXPEDITION);
			Packet.Push(*pNfy);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_C_N_REQ_INVITE_EXPEDITION:
		{
			BM::Stream Packet(PT_C_N_REQ_INVITE_EXPEDITION);
			Packet.Push(pUnit->GetID());
			Packet.Push(*pNfy);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_C_N_ANS_INVITE_EXPEDITION:
		{
			bool HaveKeyItem = false;
			BM::Stream Packet(PT_C_N_ANS_INVITE_EXPEDITION);
			Packet.Push(*pNfy);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_C_N_REQ_TEAM_MOVE_EXPEDITION:
		{
			BM::Stream Packet(PT_C_N_REQ_TEAM_MOVE_EXPEDITION);
			Packet.Push(pUnit->GetID());
			Packet.Push(*pNfy);
			SendToGlobalPartyMgr(Packet);
		}break;
	case PT_C_M_REQ_REFRESH_NEED_ITEM_INFO:
		{
			PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
			if( pPlayer )
			{
				if( pPlayer->HaveExpedition() )
				{
					VEC_GUID MemberList;
					m_kLocalPartyMgr.GetExpeditionMember(pPlayer->ExpeditionGuid(), MemberList);

					VEC_GUID NotHaveKeyUserList;
					CheckHaveKeyItem(pPlayer->ExpeditionGuid(), MemberList, NotHaveKeyUserList);

					BM::Stream AnsPacket(PT_C_M_ANS_REFRESH_NEED_ITEM_INFO);
					if( !NotHaveKeyUserList.empty() )
					{
						PU::TWriteArray_A(AnsPacket, NotHaveKeyUserList);
					}

					pPlayer->Send(AnsPacket, E_SENDTYPE_SELF | E_SENDTYPE_MUSTSEND);
				}
			}
		}break;
	case PT_C_M_NFY_MAPLOADED:
		{
			CheckJoinExpedition(pUnit);
		}//break; 없음.
	default:
		{
			return PgGround::VUpdate(pUnit, wType, pNfy);
		}break;
	}
	return true;
}

HRESULT PgExpeditionLobby::Recv_PT_C_M_REQ_NPC_ENTER_EXPEDITION(CUnit * pUnit, BM::Stream & Packet, VEC_GUID & Vec_Fail)
{
	HRESULT hRet = PRC_Success;
	BM::GUID NpcGuid;
	Packet.Pop(NpcGuid);

	CUnit * pNpcUnit = GetNPC(NpcGuid);
	if( true == PgGroundUtil::IsCanTalkableRange(pUnit, NpcGuid, pNpcUnit, GetGroundNo(), NMT_Expedition, __F_P__(PT_C_M_REQ_NPC_ENTER_EXPEDITION), __LINE__) )
	{
		PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if( NULL == pPlayer )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return PRC_Fail"));
			return PRC_Fail;
		}

		if( false == pPlayer->HaveExpedition() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return PRC_Fail"));
			return PRC_Fail;
		}

		//여기서 맵이동 까지 처리.
		hRet = IsStartableExpedition(pPlayer->ExpeditionGuid(), NpcGuid, Vec_Fail);
		if( PRC_Success != hRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return ") << hRet);
			return hRet;
		}

		PgLocalExpedition* pExpedition = m_kLocalPartyMgr.GetLocalExpedition(pPlayer->ExpeditionGuid());
		if( NULL == pExpedition )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
		// 원정대 정보 변경을 원정대 리스트 매니저로 알림.
		SExpeditionOption exOption = pExpedition->Option();
		exOption.SetOptionState(false);
		pExpedition->Option(exOption);
		BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
		pExpedition->WrtieToPacketListInfo(ModifyPacket);
		SendToExpeditionListMgr(ModifyPacket);

		BM::Stream ModifyPacket2(PT_M_T_NFY_MODIFY_EXPEDITION_OPTION);
		ModifyPacket2.Push(pExpedition->ExpeditionGuid());
		pExpedition->Option().WriteToPacket(ModifyPacket2);
		SendToGlobalPartyMgr(ModifyPacket2);
	}

	return hRet;
}

HRESULT PgExpeditionLobby::IsStartableExpedition(BM::GUID const & ExpeditionGuid, BM::GUID const & NpcGuid, VEC_GUID & Vec_Fail)
{
	//여기서 맵이동 까지 처리.
	HRESULT hRet = m_kLocalPartyMgr.IsStartable(ExpeditionGuid);
	if( PRC_Success != hRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return ") << hRet);
		return hRet;
	}

	VEC_GUID Member_Guid;
	m_kLocalPartyMgr.GetExpeditionMemberGround(ExpeditionGuid, GroundKey(), Member_Guid);

	if( Member_Guid.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return PRC_Fail"));
		return PRC_Fail;
	}

	CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;
	int KeyItemNo = 0, ItemDecCount = 0;
	g_kTblDataMgr.GetContDef(pContExpeditionNpc);
	if( NULL == pContExpeditionNpc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return PRC_Fail"));
		return PRC_Fail;
	}

	CONT_DEF_EXPEDITION_NPC::const_iterator npc_iter = pContExpeditionNpc->find(NpcGuid);
	if( npc_iter != pContExpeditionNpc->end() )
	{
		KeyItemNo = npc_iter->second.iNeedItemNo;		// 입장 아이템 번호를 가져와서.
		ItemDecCount = npc_iter->second.iNeedItemCnt;   // 몇개씩 소모?
	}

	SReqMapMove_MT RMM(MMET_None);
	RMM.kTargetKey.GroundNo(npc_iter->second.iMapNo);	// 이동할 맵 번호.
	RMM.nTargetPortal = 1;

	PgReqMapMove MapMove( this, RMM, NULL );		// 맵 이동을 해야 됨.

	CONT_PLAYER_MODIFY_ORDER Order;		// 아이템 수량을 깍아야 됨

	VEC_GUID::const_iterator iter = Member_Guid.begin();
	while( iter != Member_Guid.end() )
	{
		PgPlayer * pPlayer = GetUser(*iter);
		if( pPlayer )
		{
			bool const IsAdd = MapMove.Add(pPlayer);	// 플레이어 추가하고.
			if( false == IsAdd )
			{	// 한명이라도 추가하지 못했다면, 출발하면 안됨.
				BM::GUID MasterGuid;
				m_kLocalPartyMgr.GetExpeditionMasterGuid(ExpeditionGuid, MasterGuid);

				PgPlayer * pMasterPlayer = GetUser(MasterGuid);
				if( pMasterPlayer )
				{
					pMasterPlayer->SendWarnMessage(720049);	// 같은 맵에 있어야 출발 할수 있다.
				}

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
				return PRC_Fail;
			}

			PgInventory * pInven = pPlayer->GetInven();
			if( pInven )
			{
				int HaveNo = pInven->GetTotalCount(KeyItemNo);
				if( 0 == HaveNo )
				{
					// 아이템이 하나도 없네. 입장아이템이 없는 캐릭터의 GUID를 vector에 넣어둠.
					Vec_Fail.push_back(pPlayer->GetID());
				}
				else
				{
					// 아이템을 가지고 있으면 Order에 넣음.
					PgBase_Item KeyItem;
					SItemPos KeyItemPos;
					if( SUCCEEDED(pInven->GetFirstItem(KeyItemNo, KeyItemPos)) )
					{
						if( SUCCEEDED(pInven->GetItem(KeyItemPos, KeyItem)) )
						{
							SPMO IMO(IMET_MODIFY_COUNT | IMC_DEC_DUR_BY_USE, pPlayer->GetID(), SPMOD_Modify_Count(KeyItem, KeyItemPos, -ItemDecCount));
							MapMove.AddModifyOrder(IMO);
						}
					}
				}
			}
		}

		++iter;
	}

	if( !Vec_Fail.empty() )		// 아이템 없는 캐릭터가 있다.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NoHaveKeyItem"));
		return PRC_Fail_NoHaveKeyItem;
	}

	if( m_kLocalPartyMgr.GetExpeditionMemberCount(ExpeditionGuid) != (Member_Guid.size()) )
	{	// 같은 그라운드에 모두 존재하지 않을 경우.
		BM::GUID MasterGuid;
		m_kLocalPartyMgr.GetExpeditionMasterGuid(ExpeditionGuid, MasterGuid);

		PgPlayer * pMasterPlayer = GetUser(MasterGuid);
		if( pMasterPlayer )
		{
			pMasterPlayer->SendWarnMessage(720049);
		}

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bRet = MapMove.DoAction();
	if( false == bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	return PRC_Success;
}

bool PgExpeditionLobby::AddJoinExpeditionWaitList(BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	BM::GUID ExpeditionGuid, CharGuid;
	Packet.Pop(CharGuid);
	Packet.Pop(ExpeditionGuid);

	auto InsertRet = m_JoinExpeditionWaitList.insert( std::make_pair( CharGuid, ExpeditionGuid ) );
	if( false == InsertRet.second )
	{
		InsertRet.first->second = ExpeditionGuid;
		INFO_LOG(BM::LOG_LV1, __FL__ << _T("m_JoinExpeditionWaitList Insert Duplecate"));
		return false;
	}

	return true;
}

bool PgExpeditionLobby::DelJoinExpeditionWaitList(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	JoinExpeditionUserList::iterator user_iter = m_JoinExpeditionWaitList.find(CharGuid);
	if( user_iter != m_JoinExpeditionWaitList.end() )
	{
		BM::Stream LeavePacket(PT_C_N_REQ_LEAVE_EXPEDITION);

		LeavePacket.Push((*user_iter).second);
		LeavePacket.Push(CharGuid);

		SendToGlobalPartyMgr(LeavePacket);

		m_JoinExpeditionWaitList.erase(user_iter);

		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("return false"));
	return false;
}

void PgExpeditionLobby::CheckJoinExpedition(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
	if( pPlayer )
	{
		JoinExpeditionUserList::iterator user_iter = m_JoinExpeditionWaitList.find(pPlayer->GetID());
		if( user_iter != m_JoinExpeditionWaitList.end() )
		{
			PgLocalExpedition * pExpedition = m_kLocalPartyMgr.GetLocalExpedition((*user_iter).second);
			if( pExpedition )
			{	// 채널 이동해서 맵으로 들어온 유저가 원정대 가입해야되는 유저라면 원정대로 가입시킴.
				BM::Stream JoinPacket(PT_C_N_ANS_JOIN_EXPEDITION);
				JoinPacket.Push((*user_iter).second);					// 원정대 GUID.
				JoinPacket.Push(pPlayer->GetID());						// 가입시킬 CharGuid
				JoinPacket.Push(true);									// 무조건 가입 시킴.
				SendToGlobalPartyMgr(JoinPacket);
				m_JoinExpeditionWaitList.erase(user_iter);
			}
		}
	}
}

void PgExpeditionLobby::Recv_PT_C_M_REQ_LIST_USER_EXPEDITION(CUnit * pUnit, BM::Stream & Packet)
{
	if( NULL == pUnit )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Unit Is NULL!"));
		return;
	}

	int KeyItemNo = 0;		// 입장 아이템 번호.
	SExpeditionOption Option;
	PgPlayer* pMasterPlayer = dynamic_cast< PgPlayer* >(pUnit);
	if( pMasterPlayer )
	{
		PgLocalExpedition* pExpedition = m_kLocalPartyMgr.GetLocalExpedition(pMasterPlayer->GetExpeditionGuid());
		if( NULL == pExpedition )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Expedition Is NULL!"));
			return;
		}
		Option = pExpedition->Option();

		bool IsMaster = pExpedition->IsMaster(pMasterPlayer->GetID());
		if( !IsMaster )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("User ") << pMasterPlayer->GetID().str() << _T(" is not expedition master."));
			return;
		}

		CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;
		g_kTblDataMgr.GetContDef(pContExpeditionNpc);
		if( NULL == pContExpeditionNpc )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Expedition NPC Container is NULL."));
			return;
		}

		BM::GUID NpcGuid = pExpedition->ExpeditionNpc();
		if( BM::GUID::IsNotNull(NpcGuid) )
		{
			CONT_DEF_EXPEDITION_NPC::const_iterator npc_iter = pContExpeditionNpc->find(NpcGuid);
			if( npc_iter != pContExpeditionNpc->end() )
			{
				KeyItemNo = npc_iter->second.iNeedItemNo;		// 입장 아이템 번호를 가져와서.
			}
		}
	}
	
	// 그라운드에 있는 모든 유저들 중 원정대 조건에 맞는 유저만 리스트로 만들어 클라로 보냄.
	// 레벨이 맞는지 검사. 파티 & 원정대에 속해 있지 않은 유저만 필터링.
	CUnit * pUser;
	ContExpeditionInviteUserInfo Vec_UserList;
	CONT_OBJECT_MGR_UNIT::iterator user_Iter;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, user_Iter);
	while((pUser = PgObjectMgr::GetNextUnit(UT_PLAYER, user_Iter)) != NULL)
	{
		PgPlayer* pPlayer = dynamic_cast< PgPlayer* >(pUser);
		if( pPlayer )
		{
			if( (true == BM::GUID::IsNull(pPlayer->ExpeditionGuid()))		// 원정대 or 파티가 아닌 유저만.
				&& (true == BM::GUID::IsNull(pPlayer->PartyGuid())) )
			{
				int iClass = pPlayer->GetAbil(AT_CLASS);
				unsigned short sLevel = pPlayer->GetAbil(AT_LEVEL);

				unsigned short sMasterLevel = pMasterPlayer->GetAbil(AT_LEVEL);

				int nPartyLevelMinus = sMasterLevel - Option.GetOptionLevel();	// 마스터 레벨 기준 - 원정대 레벨 제한
				int nPartyLevelPlus = sMasterLevel + Option.GetOptionLevel();	// 마스터 레벨 기준 + 원정대 레벨 제한

				if( (sLevel <= nPartyLevelPlus) || (sLevel >= nPartyLevelMinus) )	// 유저의 레벨이 원정대 레벨 제한 사이에 있어야함.
				{
					bool HaveItem = false;
					PgInventory * pInven = pPlayer->GetInven();
					if( pInven )
					{
						int HaveNo = pInven->GetTotalCount(KeyItemNo);
						if( HaveNo > 0 )	// 입장아이템을 한 개 이상 보유 중.
						{
							HaveItem = true;
						}
					}

					SExpeditionInviteUserInfo UserInfo(pPlayer->GetID(), pPlayer->Name(), iClass, sLevel, HaveItem);
					Vec_UserList.push_back(UserInfo);
				}
			}
		}
	}

	BM::Stream AnsPacket(PT_M_C_ANS_LIST_USER_EXPEDITION);
	AnsPacket.Push(Vec_UserList.size());
	ContExpeditionInviteUserInfo::const_iterator iter = Vec_UserList.begin();
	while( iter != Vec_UserList.end() )
	{
		(*iter).WriteToPacket(AnsPacket);
		++iter;
	}

	pUnit->Send(AnsPacket);
}

void PgExpeditionLobby::CheckHaveKeyItem(BM::GUID ExpeditionGuid, VEC_GUID & CharList, VEC_GUID & OutList)
{
	int KeyItemNo = 0;

	PgLocalExpedition * pExpedition = m_kLocalPartyMgr.GetLocalExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Expedition pointer is NULL."));
		return;
	}

	BM::GUID NpcGuid = pExpedition->ExpeditionNpc();

	if( NpcGuid.IsNotNull() )
	{
		// 아이템 보유 여부를 체크해서 같이 전송해 준다.
		CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;
		g_kTblDataMgr.GetContDef(pContExpeditionNpc);
		if( NULL == pContExpeditionNpc )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Expedition NPC Container is NULL."));
			return;
		}

		CONT_DEF_EXPEDITION_NPC::const_iterator npc_iter = pContExpeditionNpc->find(NpcGuid);
		if( npc_iter != pContExpeditionNpc->end() )
		{
			KeyItemNo = npc_iter->second.iNeedItemNo;		// 입장 아이템 번호를 가져온다.
		}
	
		VEC_GUID::iterator char_iter = CharList.begin();
		for( ; char_iter != CharList.end() ; ++char_iter )
		{
			PgPlayer * pPlayer = GetUser((*char_iter));
			if( pPlayer )
			{
				PgInventory * pInven = pPlayer->GetInven();
				if( pInven )
				{
					int HaveNo = pInven->GetTotalCount(KeyItemNo);
					if( 0 == HaveNo )		// 입장아이템 없음.
					{
						OutList.push_back((*char_iter));
					}
				}
			}
		}
	}
}