#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "Lohengrin/PacketStruct.h"
#include "Collins/Log.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgExpedition.h"
#include "Variant/PgExpeditionMgr.h"
#include "Variant/PgLogUtil.h"
#include "Variant/PgHardCoreDungeonParty.h"
#include "global.h"
#include "PgTask_Contents.h"
#include "PgGlobalPartyMgr.h"


namespace PgGlobalPartyMgrUtil
{
	bool IsCanWarpGround( SChnGroundKey const& rkGndKey )
	{
		CONT_DEFMAP const* pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);

		if( !pkDefMap )
		{
			return false;
		}

		CONT_DEFMAP::const_iterator find_iter = pkDefMap->find(rkGndKey.GroundNo());
		if( pkDefMap->end() == find_iter )
		{
			return false;
		}

		if ( g_kProcessCfg.ChannelNo() != rkGndKey.Channel() )
		{
			return false;
		}

		bool bCant = false;
		if( true == g_kLocal.IsServiceRegion( LOCAL_MGR::NC_TAIWAN ) )
		{
			bCant = (0 != ((*find_iter).second.iAttr&GATTR_FLAG_CANT_PARTYWARP));
		}
		else
		{
			bCant = (0 != ((*find_iter).second.iAttr&GATTR_FLAG_CANT_WARP));
		}

		if( bCant )
		{
			return false;
		}
		return true;
	}
};


int const iPartyNameDefStrNo = 10;//
int const iPartyLimitNone = 0; // 파티 가입 제한 레벨 없음.

PgGlobalPartyMgrImpl::PgGlobalPartyMgrImpl()
	: PgPartyMgr::T_MY_BASE_MGR_TYPE(), m_kCharToParty(), m_kContPartyNameSet(), m_kCharToExpedition()
{
}

PgGlobalPartyMgrImpl::~PgGlobalPartyMgrImpl()
{
}

bool PgGlobalPartyMgrImpl::GetCharToParty(BM::GUID const &rkCharGuid, BM::GUID& rkOutGuid) const
{
	ContCharToParty::const_iterator iter = m_kCharToParty.find(rkCharGuid);
	if(m_kCharToParty.end() != iter)
	{
		rkOutGuid = (*iter).second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGlobalPartyMgrImpl::AddCharToParty(BM::GUID const &rkCharGuid, BM::GUID const &rkPartyGuid)
{
	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return false;
	}

	auto kRet = m_kCharToParty.insert( std::make_pair(rkCharGuid, rkPartyGuid) );
	return kRet.second;
}

BM::GUID PgGlobalPartyMgrImpl::DelCharToParty(BM::GUID const &rkCharGuid)
{
	BM::GUID kPartyGuid;

	ContCharToParty::iterator iter = m_kCharToParty.find(rkCharGuid);
	if( m_kCharToParty.end() != iter )
	{
		kPartyGuid = iter->second;
		m_kCharToParty.erase(iter);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULLGUID"));
	}
	
	return kPartyGuid;
}

void PgGlobalPartyMgrImpl::DelAllCharToParty(BM::GUID const & PartyGuid)
{
	ContCharToParty::iterator iter = m_kCharToParty.begin();
	while( m_kCharToParty.end() != iter )
	{
		if( iter->second == PartyGuid )
		{
			iter = m_kCharToParty.erase(iter);
			continue;
		}
		++iter;
	}
}

bool PgGlobalPartyMgrImpl::FindCharToParty(BM::GUID const &rkCharGuid) const
{
	if( BM::GUID::IsNull(rkCharGuid) )
	{
		return false;
	}

	ContCharToParty::const_iterator iter = m_kCharToParty.find(rkCharGuid);
	return (m_kCharToParty.end() != iter);
}

PgGlobalParty * PgGlobalPartyMgrImpl::GetCharToParty(BM::GUID const &rkCharGuid )const
{
	BM::GUID kPartyGuid;
	if ( true == GetCharToParty( rkCharGuid, kPartyGuid ) )
	{
		return GetParty( kPartyGuid );
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

HRESULT PgGlobalPartyMgrImpl::CheckJoinParty(SContentsUser const& rkMaster, SContentsUser const& rkUser, bool bIsDungeonParty /*= false*/)
{
	if( rkMaster.Empty() )//마스터 정보가 NULL
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( rkUser.Empty() )//상대편 정보가 NULL
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	{//PvP에 있는 유저라 파티를 가입시킬 수 없다.
		CONT_DEFMAP const *pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);
		if( !pkDefMap )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
		
		CONT_DEFMAP::const_iterator iter = pkDefMap->find(rkUser.kGndKey.GroundNo());
		if( pkDefMap->end() == iter )
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find GroundDef User[") << rkUser.Name() << _T("],GroundNo=") << rkUser.kGndKey.GroundNo());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		switch ( (*iter).second.iAttr )
		{
		case GATTR_PVP:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_PVP"));
				return PRC_Fail_PVP;
			}break;
		case GATTR_MISSION:
		case GATTR_CHAOS_MISSION:
			{
				if( !bIsDungeonParty )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Area"));
					return PRC_Fail_Area;
				}
			}break;
		case GATTR_EMPORIA:
			{
			}break;
		default:
			{
				if( 0 != ((*iter).second.iAttr & GATTR_FLAG_SUPER) )
				{
					return PRC_Fail_Area;
				}
			}break;
		}

		iter = pkDefMap->find(rkMaster.kGndKey.GroundNo());
		if( pkDefMap->end() == iter )
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find GroundDef User[") << rkUser.Name() << _T("],GroundNo=") << rkUser.kGndKey.GroundNo());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		if( ((((*iter).second.iAttr & GKIND_INDUN) && (*iter).second.iAttr == GKIND_INDUN) 
			|| ((*iter).second.iAttr & GKIND_SUPER)
			|| ((*iter).second.iAttr & GKIND_HARDCORE)) && !bIsDungeonParty)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Area"));
			return PRC_Fail_Area;
		}
	}

	if( rkMaster.kCharGuid == rkUser.kCharGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotInviteMe"));
		return PRC_Fail_NotInviteMe;//나를 초대 하지 마라
	}

	BM::GUID kUserPartyGuid;
	BM::GUID kMasterPartyGuid;
	bool const bUserFindParty = GetCharToParty(rkUser.kCharGuid, kUserPartyGuid);//이미 파티에 가입 되었는가?
	bool const bMasterFindParty = GetCharToParty(rkMaster.kCharGuid, kMasterPartyGuid);//마스터 파티가 있나?
	
	PgGlobalParty* pkParty = GetParty(kMasterPartyGuid);//마스터 파티가 실존 하면
	if( pkParty )
	{// 파티가 NULL일 경우가 있으므로 쓸때마다 꼭 검사가 필요함.
		if( pkParty->Option().GetOptionState() ) // 파티가입요청 거부 상태
		{
			return PRC_Fail_Refuse;
		}
		if( pkParty->MemberCount() >= pkParty->Option().GetOptionMaxMember() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
			return PRC_Fail_MemberCount;//인원수 제한으로 등록 불가
		}
	}

	if( bUserFindParty )//유저가 파티가 없고
	{
		if( kUserPartyGuid == kMasterPartyGuid )
		{
			if( pkParty )
			{
				bool const bIsMember = pkParty->IsMember(rkUser.kCharGuid);
				if( !bIsMember )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_AnswerWait"));
					return PRC_Fail_AnswerWait;
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MineMember"));
				return PRC_Fail_MineMember;//이미 같은 맴버
			}
		}
		else
		{
			PgGlobalParty* pkUserParty = GetParty(kUserPartyGuid);
			if( pkUserParty )
			{
				bool const bIsMember = pkUserParty->IsMember(rkUser.kCharGuid);
				if( !bIsMember )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_AnswerWait"));
					return PRC_Fail_AnswerWait;
				}				
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
				return PRC_Fail_Party;//이미 다른 파티 가입
			}			
			else if( !bMasterFindParty )//마스터에 파티가 없으면
			{
				return PRC_Success_Create;//생성하면 된다
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}
	else//유저가 파티가 있고
	{
		if( !bMasterFindParty )//마스터에 파티가 없으면
		{
			return PRC_Success_Create;//생성하면 된다
		}
		else//마스터 파티가 있으면
		{
			//다음 검증
		}
	}
	
	if( !pkParty )
	{
		DelCharToParty(rkMaster.kCharGuid);//파티 포인터가 없으면 파티 정보 삭제
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bIsMaster = pkParty->IsMaster(rkMaster.kCharGuid);
	if( !bIsMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;//마스터가 아니면 초대 안돼
	}

	int nPartyLevelLimit = pkParty->Option().GetOptionLevel();

	if( iPartyLimitNone == nPartyLevelLimit )		// 레벨 제한값이 iPartyLimitNone일 경우에는 파티에 가입 레벨 제한이 없음.
	{
		return PRC_Success;
	}

	int nPartyLevelMinus = rkMaster.sLevel - nPartyLevelLimit;	// 마스터 레벨 기준 - 파티레벨제한
	int nPartyLevelPlus = rkMaster.sLevel + nPartyLevelLimit;	// 마스터 레벨 기준 + 파티레벨제한
	if( (rkUser.sLevel > nPartyLevelPlus) || (rkUser.sLevel < nPartyLevelMinus) )	// 파티에 가입하려는 유저의 레벨이 파티 레벨 제한 사이에 있어야함.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Level"));
		return PRC_Fail_Level;// 레벨이 맞지 않음(파티방)
	}


	return PRC_Success;//초대 가능
}

//
bool PgGlobalPartyMgrImpl::ProcessMsg(SEventMessage *pkMsg)
{
	PACKET_ID_TYPE wType = 0;
	pkMsg->Pop(wType);
	switch(wType)
	{
	case PT_M_T_ANS_SUMMONPARTYMEMBER:
		{
			BM::GUID	kOwnerGuid;
			SGroundKey	kCastGndKey;
			HRESULT		hRet = E_FAIL;

			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kCastGndKey);
			pkMsg->Pop(hRet);

			BM::Stream kPacket(PT_T_M_ANS_SUMMONPARTYMEMBER);
			kPacket.Push(kOwnerGuid);
			kPacket.Push(kCastGndKey);
			kPacket.Push(hRet);
			
			g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
		}break;
	case PT_M_T_ANS_MOVETOSUMMONER:
		{
			BM::GUID	kOwnerGuid;
			SGroundKey	kCastGndKey;

			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kCastGndKey);

			BM::Stream kPacket(PT_T_M_ANS_MOVETOSUMMONER);
			kPacket.Push(kOwnerGuid);
			kPacket.Push(kCastGndKey);

			g_kServerSetMgr.Locked_SendToGround(kCastGndKey,kPacket,true);
		}break;
	case PT_M_T_REQ_PARTY_BUFF:
		{
			BM::GUID	kOwnerGuid;
			SGroundKey	kCastGndKey;
			BM::GUID	kPartyGuid;

			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kCastGndKey);

			bool const bIsParty = GetCharToParty(kOwnerGuid, kPartyGuid);
			if( bIsParty )
			{
				PartyBuffRefresh(kOwnerGuid, kPartyGuid);
			}
		}break;
	case PT_M_T_REQ_MOVETOSUMMONER:
		{
			BM::GUID kOwnerGuid,
					 kMemberGuid;

			SGroundKey	kCastGndKey,
						kGndKey;
			SItemPos	kItemPos;

			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kCastGndKey);
			pkMsg->Pop(kGndKey);
			pkMsg->Pop(kItemPos);

			BM::Stream kPacket(PT_T_M_REQ_MOVETOSUMMONER);
			kPacket.Push(kOwnerGuid);
			kPacket.Push(kMemberGuid);
			kPacket.Push(kCastGndKey);
			kPacket.Push(kItemPos);

			g_kServerSetMgr.Locked_SendToGround(kGndKey,kPacket,true);
		}break;
	case PT_M_T_REQ_SUMMONPARTYMEMBER:
		{
			BM::GUID kOwnerGuid,
					 kMemberGuid;

			SGroundKey kCasterGndKey;
			SItemPos kItemPos;
			POINT3 kCasterPos;

			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kCasterGndKey);
			pkMsg->Pop(kItemPos);

			SChnGroundKey kChnGndKey;
			if( GetMemberGroundKey( kMemberGuid, kChnGndKey ) )
			{
				if ( g_kProcessCfg.ChannelNo() == kChnGndKey.Channel() )
				{
					BM::Stream kPacket(PT_T_M_REQ_SUMMONPARTYMEMBER);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kMemberGuid);
					kPacket.Push(kCasterGndKey);
					kPacket.Push(kItemPos);
					SendToGround( kChnGndKey, kPacket );
				}
				else
				{
					BM::Stream kPacket(PT_M_T_ANS_SUMMONPARTYMEMBER);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(E_CANNOT_MOVE_MAP);
					g_kServerSetMgr.Locked_SendToGround( kCasterGndKey, kPacket, true );
				}
			}
			else
			{
				BM::Stream kPacket(PT_M_T_ANS_SUMMONPARTYMEMBER);
				kPacket.Push(kOwnerGuid);
				kPacket.Push(E_NOT_FOUND_MEMBER);
				g_kServerSetMgr.Locked_SendToGround( kCasterGndKey, kPacket, true );
			}
		}break;
	case PT_M_T_REQ_PARTYMEMBERPOS:
		{
			BM::GUID kOwnerGuid,
					 kMemberGuid;

			SGroundKey kCasterGndKey;
			SItemPos kItemPos;
			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kCasterGndKey);
			pkMsg->Pop(kItemPos);

			SChnGroundKey kChnGndKey;
			if( GetMemberGroundKey(kMemberGuid,kChnGndKey)
			&&	PgGlobalPartyMgrUtil::IsCanWarpGround(kChnGndKey) )
			{
				BM::Stream kPacket(PT_T_M_REQ_PARTYMEMBERPOS);
				kPacket.Push(kOwnerGuid);
				kPacket.Push(kMemberGuid);
				kPacket.Push(kCasterGndKey);
				kPacket.Push(kItemPos);
				SendToGround( kChnGndKey, kPacket );
			}
			else
			{
				BM::Stream kPacket(PT_T_M_ANS_PARTYMEMBERPOS);
				kPacket.Push(kOwnerGuid);
				kPacket.Push( static_cast< HRESULT >(E_CANNOT_STATE_MAPMOVE) );
				g_kServerSetMgr.Locked_SendToGround(kCasterGndKey,kPacket,true);
			}
		}break;
	case PT_M_T_REQ_PARTYMASTERGROUNDKEY:
		{
			BM::GUID kOwnerGuid, kMemberGuid;
			BM::GUID kTowerGuid;
			TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;

			SGroundKey kCasterGndKey;
			pkMsg->Pop(kOwnerGuid);
			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kCasterGndKey);
			pkMsg->Pop(kTowerGuid);
			pkMsg->Pop(kTargetKey);

			SChnGroundKey kChnGndKey;
			if( GetMemberGroundKey(kMemberGuid, kChnGndKey) )
			{
				if( PgGlobalPartyMgrUtil::IsCanWarpGround(kChnGndKey) )
				{
					BM::Stream kPacket(PT_T_M_REQ_PARTYMASTERGROUNDKEY);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kMemberGuid);
					kPacket.Push(kCasterGndKey);
					kPacket.Push(static_cast<HRESULT>(S_OK));
					kPacket.Push(kTowerGuid);
					kPacket.Push(kTargetKey);
					SendToGround(kChnGndKey, kPacket);
				}
				else
				{
					BM::Stream kPacket(PT_T_M_REQ_PARTYMASTERGROUNDKEY);
					kPacket.Push(kOwnerGuid);
					kPacket.Push(kMemberGuid);
					kPacket.Push(kCasterGndKey);
					kPacket.Push(static_cast<HRESULT>(E_CANNOT_STATE_MAPMOVE));
					kPacket.Push(kTowerGuid);
					kPacket.Push(kTargetKey);
					SendToGround(kChnGndKey, kPacket);
				}
			}
			else
			{
				BM::Stream kPacket(PT_T_M_ANS_PARTYMASTERGROUNDKEY);
				kPacket.Push(kOwnerGuid);
				kPacket.Push(static_cast<HRESULT>(E_CANNOT_STATE_MAPMOVE));
				g_kServerSetMgr.Locked_SendToGround(kCasterGndKey, kPacket, true);
			}
		}break;
	case PT_C_N_REQ_CREATE_PARTY_2ND: //생성
		{
			SContentsUser kMaster;

			kMaster.ReadFromPacket(*pkMsg);

			HRESULT hCreateRet = ReqCreateParty(kMaster, *pkMsg);
			if( PRC_Success != hCreateRet )
			{
				BM::Stream kPacket(PT_N_C_NFY_CREATE_PARTY);
				kPacket.Push(hCreateRet);
				g_kServerSetMgr.Locked_SendToUser(kMaster.kMemGuid, kPacket);
			}
		}break;
	case PT_C_N_REQ_PARTY_RENAME:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop( kCharGuid );

			ReqRenameParty( kCharGuid, *pkMsg );
		}break;

	case PT_C_N_REQ_JOIN_PARTY_2ND: //초대
		{
			BM::GUID kPartyGuid;
			SContentsUser kMaster;
			SContentsUser kUser;
			BYTE byPartyRefuse = 0;

			SPartyOption rkOption;
			
			kMaster.ReadFromPacket(*pkMsg);			
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(byPartyRefuse);

			bool const bIsParty = GetCharToParty(kMaster.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				PgGlobalParty* pkParty = GetParty(kPartyGuid);
				if( pkParty )
				{
					rkOption = pkParty->Option();
				}
			}
			rkOption.byPartyState |= byPartyRefuse;
			HRESULT const hReqJoinRet = ReqJoinParty(kMaster, kUser, rkOption);
			if( PRC_Success != hReqJoinRet )
			{
				BM::Stream kRetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
				kRetPacket.Push(hReqJoinRet);
				g_kServerSetMgr.Locked_SendToUser(kMaster.kMemGuid, kRetPacket);//결과

				PgGlobalParty* pkParty = GetParty(kPartyGuid);
				if( pkParty )
				{
					bool const bIsMember = pkParty->IsMember(kUser.kCharGuid);
					bool const bIsDestroy = pkParty->IsDestroy();
					if( bIsDestroy
						&& bIsMember )
					{
						DestroyParty(kPartyGuid);
					}
				}
			}
		}break;
	case PT_C_N_REQ_JOIN_PARTYFIND_2ND: //초대
		{
			BM::GUID kPartyGuid;
			SContentsUser kMaster;
			SContentsUser kUser;

			SPartyOption rkOption;

			kUser.ReadFromPacket(*pkMsg);
			kMaster.ReadFromPacket(*pkMsg);

			bool IsReqJoinDungeonParty = false;
			if( pkMsg->RemainSize() )
			{
				pkMsg->Pop(IsReqJoinDungeonParty);
			}

			if( 0 == kMaster.kGndKey.GroundNo() )
			{
				BM::Stream kRetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
				kRetPacket.Push(PRC_Fail_Area);
				g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, kRetPacket);//결과
			}

			bool const bIsParty = GetCharToParty(kMaster.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				PgGlobalParty* pkParty = GetParty(kPartyGuid);
				if( pkParty )
				{
					rkOption = pkParty->Option();

					HRESULT const hReqJoinRet = ReqJoinPartyFind(kMaster, kUser, rkOption, IsReqJoinDungeonParty);
					if( PRC_Success != hReqJoinRet )
					{
						ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find(kUser.kCharGuid);
						if( find_iter == m_ContOtherChannelJoinInfo.end() )
						{
							BM::Stream kRetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
							kRetPacket.Push(hReqJoinRet);
							g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, kRetPacket);//결과							
						}
						else
						{
							BM::Stream Packet(PT_T_T_ANS_JOIN_OTHER_CHANNEL_PARTY_TO_MASTER);
							Packet.Push(kUser.kCharGuid);
							Packet.Push(hReqJoinRet);

							SendToOtherChannelContents(kUser.sChannel, PMET_PARTY, Packet);
							
							if( PRC_Fail_AnswerWait != hReqJoinRet )
							{// 수락 대기중일 때는 지울 필요 없다.
								m_ContOtherChannelJoinInfo.erase(kUser.kCharGuid);
								DelCharToParty(kUser.kCharGuid);
								PgGlobalParty *pParty = GetParty( kPartyGuid );
								if( pParty )
								{
									pParty->DelWait(kUser.kCharGuid);
								}
							}
						}

						PgGlobalParty* pkParty = GetParty(kPartyGuid);
						if( pkParty )
						{
							bool const bIsMember = pkParty->IsMember(kUser.kCharGuid);
							bool const bIsDestroy = pkParty->IsDestroy();
							if( bIsDestroy
								&& bIsMember )
							{
								DestroyParty(kPartyGuid);
							}
						}
					}
				}
			}
		}break;
	case PT_C_N_ANS_JOIN_PARTY_2ND: //초대 응답
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kMaster;
			SContentsUser kUser;

			kUser.ReadFromPacket(*pkMsg);

			if( kUser.Empty() )
			{
				break;
			}

			pkMsg->Pop(bAnsJoin);
			pkMsg->Pop(kPartyGuid);

			HRESULT const hRet = AnsJoinParty(kPartyGuid, kUser, bAnsJoin, false);

			if(	bAnsJoin//허가면서
			&&	PRC_Success != hRet )//성공이 아니면
			{
				BM::Stream kAnsUserPacket(PT_N_C_ANS_JOIN_PARTY, kPartyGuid);
				kAnsUserPacket.Push(hRet);
				g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, kAnsUserPacket);

				if( PRC_Fail_MemberCount == hRet )
				{
					PgGlobalParty *pParty = GetParty(kPartyGuid);
					if( pParty )
					{
						if( kUser.kMemGuid != pParty->MasterCharGuid() )
						{
							BM::Stream RetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
							RetPacket.Push(hRet);
							g_kServerSetMgr.Locked_SendToUser(pParty->MasterCharGuid(), kAnsUserPacket, false);
						}
					}
				}
			}
			else
			{
				if( false == bAnsJoin )
				{
					BM::Stream AnsUserPacket(PT_N_C_ANS_JOIN_PARTY, kPartyGuid);
					AnsUserPacket.Push(PRC_Fail_Req_Join);
					g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, AnsUserPacket);
					
					PgGlobalParty *pParty = GetParty(kPartyGuid);
					if( pParty )
					{
						if( kUser.kMemGuid != pParty->MasterCharGuid() )
						{
							BM::Stream RetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
							RetPacket.Push(PRC_Fail_Req_Join);
							g_kServerSetMgr.Locked_SendToUser(pParty->MasterCharGuid(), RetPacket, false);
						}
					}
				}
			}
		}break;
	case PT_C_N_ANS_JOIN_PARTYFIND_2ND: //초대 응답
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kUser;

			kUser.ReadFromPacket(*pkMsg);

			pkMsg->Pop(bAnsJoin);
			pkMsg->Pop(kPartyGuid);

			if( kUser.Empty() )
			{
				break;
			}

			ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find(kUser.kCharGuid);
			if( find_iter == m_ContOtherChannelJoinInfo.end() )
			{	// 없으면 같은채널.
				HRESULT const hRet = AnsJoinParty(kPartyGuid, kUser, bAnsJoin, false);

				if( hRet != PRC_Success )
				{
					PgGlobalParty *pParty = GetParty( kPartyGuid );
					if( pParty )
					{
						BM::Stream RetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
						RetPacket.Push(hRet);
						g_kServerSetMgr.Locked_SendToUser(pParty->MasterCharGuid(), RetPacket, false);//마스터에게 거부 메시지
					}

					if( hRet != PRC_Fail_Party )
					{// 이미 파티에 가입되어 있다는 메시지는 신청자에게 보여줄 필요없다.
						BM::Stream kAnsUserPacket(PT_N_C_ANS_JOIN_PARTY, kPartyGuid);
						kAnsUserPacket.Push(hRet);
						g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, kAnsUserPacket);
					}
				}
			}
			else
			{	// 있으면 다른채널.	
				if( false == bAnsJoin )
				{// 거절일 경우 데이터 지워준다.
					m_ContOtherChannelJoinInfo.erase(kUser.kCharGuid);
					DelCharToParty(kUser.kCharGuid);
					PgGlobalParty *pParty = GetParty( kPartyGuid );
					if( pParty )
					{
						pParty->DelWait(kUser.kCharGuid);
					}
				}
				
				find_iter->second.bAccept = bAnsJoin;	// 수락 여부 저장해둔다.

				BM::Stream Packet(PT_T_T_ANS_OTHER_CHANNEL_PARTY);
				Packet.Push(kUser.kCharGuid);
				Packet.Push(find_iter->second.PartyMasterGuid);
				Packet.Push(kPartyGuid);
				Packet.Push(g_kProcessCfg.ChannelNo());
				Packet.Push(bAnsJoin);
				SendToOtherChannelContents(kUser.sChannel, PMET_PARTY, Packet);
			}
		}break;
	case PT_C_T_ANS_JOIN_PARTYFIND_ITEM_CHECK:
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kUser;
			bool IsMyChannel = false;
			short MasterChannelNo = 0;
			SGroundKey MasterGroundKey;

			kUser.ReadFromPacket(*pkMsg);

			pkMsg->Pop(bAnsJoin);
			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(MasterGroundKey);
			pkMsg->Pop(IsMyChannel);
			pkMsg->Pop(MasterChannelNo);

			if( kUser.Empty() )
			{
				break;
			}

			if( false == bAnsJoin )
			{
				BM::Stream Packet(PT_C_N_ANS_JOIN_PARTYFIND_2ND);
				kUser.WriteToPacket(Packet);
				Packet.Push(bAnsJoin);
				Packet.Push(kPartyGuid);
				SendToGlobalPartyMgr(Packet);
			}
			else
			{
				ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find(kUser.kCharGuid);
				if( find_iter == m_ContOtherChannelJoinInfo.end() )
				{// 같은채널.
					SChnGroundKey ChnGndKey(kUser.kGndKey, kUser.sChannel);
					BM::Stream Packet(PT_T_M_ANS_JOIN_PARTYFIND_ITEM_CHECK);
					kUser.WriteToPacket(Packet);
					Packet.Push(bAnsJoin);
					Packet.Push(kPartyGuid);
					Packet.Push(MasterGroundKey);
					Packet.Push(IsMyChannel);
					Packet.Push(MasterChannelNo);
					SendToGround(ChnGndKey, Packet);
				}
				else
				{// 다른채널.	
					BM::Stream Packet(PT_C_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
					kUser.WriteToPacket(Packet);
					Packet.Push(bAnsJoin);
					Packet.Push(kPartyGuid);
					Packet.Push(MasterGroundKey);
					Packet.Push(IsMyChannel);
					Packet.Push(g_kProcessCfg.ChannelNo());
					SendToOtherChannelContents(find_iter->second.ChannelNo, PMET_PARTY, Packet);
				}
			}
		}break;
	case PT_M_T_ANS_JOIN_PARTYFIND_ITEM_CHECK:
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kUser;
			short MasterChannelNo = 0;
			bool IsHaveItem = false;

			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(bAnsJoin);
			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(MasterChannelNo);
			pkMsg->Pop(IsHaveItem);

			BM::Stream Packet(PT_T_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
			kUser.WriteToPacket(Packet);
			Packet.Push(bAnsJoin);
			Packet.Push(kPartyGuid);
			Packet.Push(IsHaveItem);
			SendToOtherChannelContents(MasterChannelNo, PMET_PARTY, Packet);
		}break;
	case PT_T_T_ANS_JOIN_PARTYFIND_ITEM_CHECK:
		{
			bool bAnsJoin = false;
			BM::GUID kPartyGuid;
			SContentsUser kUser;
			bool IsHaveItem = false;

			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(bAnsJoin);
			pkMsg->Pop(kPartyGuid);
			pkMsg->Pop(IsHaveItem);

			if( IsHaveItem )
			{
				BM::Stream Packet(PT_C_N_ANS_JOIN_PARTYFIND_2ND);
				kUser.WriteToPacket(Packet);
				Packet.Push(bAnsJoin);
				Packet.Push(kPartyGuid);
				SendToGlobalPartyMgr(Packet);
			}
			else
			{// 대상이 현재 입장아이템을 가지고 있지 않다.
				m_ContOtherChannelJoinInfo.erase(kUser.kCharGuid);
				DelCharToParty(kUser.kCharGuid);

				PgGlobalParty *pParty = GetParty( kPartyGuid );
				if( pParty )
				{
					pParty->DelWait(kUser.kCharGuid);

					BM::Stream RetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
					RetPacket.Push(PRC_NeedItem_PartyBreakIn);
					g_kServerSetMgr.Locked_SendToUser(pParty->MasterCharGuid(), RetPacket, false); // 마스터에게 메시지
				}
			}
		}break;
	case PT_C_M_REQ_REGIST_PRIVATE_PARTY_FIND2ND:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*pkMsg);

			HRESULT hCreateRet = ReqCreateFindParty(kUser, *pkMsg);
			BM::Stream kPacket(PT_M_C_ANS_REGIST_PRIVATE_PARTY_FIND);
			kPacket.Push(hCreateRet);
			g_kServerSetMgr.Locked_SendToUser(kUser.kMemGuid, kPacket);
		}break;
	case PT_A_NFY_USER_DISCONNECT: // 연걸 종료 됨.
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*pkMsg);

			SFindPartyUserListInfo kOutFindPartyUserInfo;
			bool const bIsListExist = GetFindPartyList(kUser.kCharGuid, kOutFindPartyUserInfo);
			if( bIsListExist )
			{
				ReqDeleteFindParty(kUser.kCharGuid);
			}
			
			BM::GUID ExpeditionGuid;
			if( true == GetCharToExpedition(kUser.kCharGuid, ExpeditionGuid) ) // 원정대 멤버.
			{
				PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
				if( pExpedition )
				{
					HRESULT hRet = ReqLeaveExpedition(kUser.kCharGuid, ExpeditionGuid);
				}
			}
			else															   // 일반 파티 멤버.
			{
				HRESULT const hLeaveRet = LeaveParty( kUser.kCharGuid );
			}
		}break;
	case PT_C_N_REQ_LEAVE_PARTY_2ND:
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*pkMsg);

			HRESULT const hLeaveRet = LeaveParty( kUser.kCharGuid );
		}break;
	case PT_C_N_REQ_PARTY_CHANGE_OPTION_2ND://파티 옵션 변경
		{
			SContentsUser kUser;
			kUser.ReadFromPacket(*pkMsg);
			ReqChangeOptionParty(kUser, *pkMsg);
		}break;
	case PT_T_N_NFY_USER_ENTER_GROUND://맵이동
		{
			SAnsMapMove_MT kAMM;
			SContentsUser kUser;

			pkMsg->Pop(kAMM);
			kUser.ReadFromPacket(*pkMsg);

			BM::GUID kPartyGuid;
			bool const bIsParty = GetCharToParty(kUser.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				MovedProcess( kPartyGuid, kUser, g_kProcessCfg.ChannelNo() );
			}
		}break;
	case PT_T_T_NFY_USER_ENTER_GROUND:
		{
			short nChannelNo = 0;
			SContentsUser kUser;
			pkMsg->Pop( nChannelNo );
			kUser.ReadFromPacket(*pkMsg);

			BM::GUID kPartyGuid;
			bool const bIsParty = GetCharToParty(kUser.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				MovedProcess( kPartyGuid, kUser, nChannelNo );
			}
		}break;
	case PT_A_U_SEND_TOPARTY_BYCHARGUID://파티 Guid로 패킷 전송
		{
			BM::GUID kCharGuid;
			BM::Stream::STREAM_DATA kData;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kData);

			if ( kData.size() <= sizeof(BM::Stream::DEF_STREAM_TYPE) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("PT_A_U_SEND_TOPARTY_BYCHARGUID: Size Error!! Size=") << kData.size() );
			}
			else 
			{
				BM::Stream kPacket;
				kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));
				SendParty_Packet(kCharGuid, kPacket);
			}
		}break;
	case PT_M_N_NFY_PARTY_USER_PROPERTY_2ND://파티원 HP Update
		{
			BM::GUID kPartyGuid;
			SContentsUser kUser;
			BYTE cPropertyType = 0;
			unsigned short sVal = 0;

			kUser.ReadFromPacket(*pkMsg);

			bool const bIsParty = GetCharToParty(kUser.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				ChangeMemberAbility(kPartyGuid, kUser, *pkMsg);
			}
		}break;
	case PT_M_N_REQ_PARTY_COMMAND_2ND:
		{

			BYTE cCmdType = 0;
			SContentsUser kUser;
			kUser.ReadFromPacket(*pkMsg);
			pkMsg->Pop(cCmdType);

			BM::GUID kPartyGuid;
			switch(cCmdType)
			{
			case PC_Summon_Member:
				{
					if( GetCharToParty(kUser.kCharGuid, kPartyGuid) )
					{
						SummonMember(kPartyGuid, kUser);
					}
				}break;
			default:
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}
		}break;
	case PT_A_U_SEND_TOFRIEND_PARTY:
		{
			PartyFriendCheck(pkMsg);
		}break;
	case PT_A_U_SEND_TOFRIEND_PARTYINFO:
		{
			PartyFriendCheckInfo(pkMsg);
		}break;
		//>> For GM Command
	case PT_C_N_REQ_PARTY_RENAME_GM:
		{
			BM::GUID kMasterCharGuid;
			BYTE cKeyInfoType = 0;

			pkMsg->Pop(cKeyInfoType);

			BM::Stream kOPacket(PT_C_N_REQ_PARTY_RENAME_2ND);

			size_t const iPreSize = kOPacket.Size();
			switch(cKeyInfoType)
			{
			case KIT_CharGuid:// CharGuid
				{
					BM::GUID kCharGuid;
					pkMsg->Pop(kCharGuid);

					::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kOPacket);
				}break;
			case KIT_CharName://CharName
				{
					std::wstring kCharName;
					pkMsg->Pop(kCharName);
					WritePlayerInfoToPacket_ByName(kCharName, kOPacket);
				}break;
			case KIT_MembGuid://MembGuid
				{
					BM::GUID kMembGuid;
					pkMsg->Pop(kMembGuid);
					::WritePlayerInfoToPacket_ByGuid(kMembGuid, true, kOPacket);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Wrong User Info Key Type=") <<cKeyInfoType);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}break;
			}

			if( iPreSize != kOPacket.Size() )
			{
				kOPacket.Push(*(BM::Stream*)pkMsg);
				SendToGlobalPartyMgr(kOPacket);
			}
		}break;
		//<< For GM Command
	case PT_C_N_REQ_JOIN_PARTY:// 유저로부터 파티 생성/가입 요청을 받음
		{
			BM::GUID kMasterCharGuid;
			BM::GUID kUserGuid;
			BYTE cJoinType = 0;

			pkMsg->Pop(kMasterCharGuid);// 가입을 주도하는 놈의 캐릭터GUID
			pkMsg->Pop(cJoinType);

			BM::Stream kPPacket(PT_C_N_REQ_JOIN_PARTY_2ND);
			if(S_OK != ::WritePlayerInfoToPacket_ByGuid(kMasterCharGuid, false, kPPacket))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! WritePlayerInfoToPacket_ByGuid isn't S_OK"));
				break;
			}

			SContentsUser kUser;
			switch(cJoinType)
			{
			case PCT_REQJOIN_CHARGUID:
				{
					pkMsg->Pop(kUserGuid);// 가입을 당하는 놈의 캐릭터GUID

					::GetPlayerByGuid(kUserGuid, false, kUser);
				}break;
			case PCT_REQJOIN_CHARNAME:
				{
					std::wstring kCharName;
					pkMsg->Pop(kCharName);// 가입을 당하는 놈의 맴버GUID
					::GetPlayerByName(kCharName, kUser);
				}break;
			case PCT_REQJOIN_MEMBERGUID:
				{
					pkMsg->Pop(kUserGuid);// 가입을 당하는 놈의 캐릭터GUID

					::GetPlayerByGuid(kUserGuid, true, kUser);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("invalid Party join type=") << cJoinType);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}
			}
			BYTE byPartyRefuse = 0;
			pkMsg->Pop(byPartyRefuse);

			kUser.WriteToPacket(kPPacket);
			kPPacket.Push(byPartyRefuse);
			SendToGlobalPartyMgr(kPPacket);
		}break;
	case PT_C_N_REQ_JOIN_PARTYFIND:// 유저가 파티 가입을 요청
		{
			BM::GUID kMasterCharGuid;
			BM::GUID kUserGuid;
			BYTE cJoinType = 0;

			pkMsg->Pop(kMasterCharGuid);// 가입 캐릭터GUID
			pkMsg->Pop(cJoinType);

			BM::Stream kQPacket(PT_C_N_REQ_JOIN_PARTYFIND_2ND);
			if(S_OK != ::WritePlayerInfoToPacket_ByGuid(kMasterCharGuid, false, kQPacket))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! WritePlayerInfoToPacket_ByGuid isn't S_OK"));
				break;
			}

			SContentsUser kUser;
			switch(cJoinType)
			{
			case PCT_REQJOIN_CHARGUID:
				{
					pkMsg->Pop(kUserGuid);// 가입하는 파티의 마스터 캐릭터GUID

					::GetPlayerByGuid(kUserGuid, false, kUser);
				}break;
			case PCT_REQJOIN_CHARNAME:
				{
					std::wstring kCharName;
					pkMsg->Pop(kCharName);// 가입하는 파티의 맴버GUID

					::GetPlayerByName(kCharName, kUser);
				}break;
			case PCT_REQJOIN_MEMBERGUID:
				{
					pkMsg->Pop(kUserGuid);// 가입하는 파티의 캐릭터GUID

					::GetPlayerByGuid(kUserGuid, true, kUser);
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("invalid Party join type=") << cJoinType);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				}
			}

			kUser.WriteToPacket(kQPacket);
			kQPacket.Push(*pkMsg);
			SendToGlobalPartyMgr(kQPacket);
		}break;
	case PT_C_N_REQ_PARTY_CHANGE_MASTER:
		{
			BM::GUID kMasterCharGuid;
			BM::GUID kNewMasterGuid;

			pkMsg->Pop(kMasterCharGuid);
			pkMsg->Pop(kNewMasterGuid);

			ChangeMaster( kMasterCharGuid, kNewMasterGuid);
		}break;
	case PT_C_M_REQ_PARTY_LIST:
		{
			short nAliveChannelCount = 0;
			BM::GUID kCharGuid;
			BM::GUID kOrderGuid;

			pkMsg->Pop(nAliveChannelCount);
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kOrderGuid);

			BM::Stream kCPacket(PT_M_C_ANS_PARTY_LIST);
			kCPacket.Push(kOrderGuid);
			kCPacket.Push(nAliveChannelCount);
			kCPacket.Push(g_kProcessCfg.ChannelNo());
			WritePartyListInfoToPacket(*pkMsg, kCPacket);

			if(g_kServerSetMgr.Locked_IsPlayer(kCharGuid, false))
			{
				g_kServerSetMgr.Locked_SendToUser(kCharGuid, kCPacket, false);
			}
			else
			{
				BM::Stream kPacket(PT_M_C_ANS_PARTY_LIST);
				kPacket.Push(kCharGuid);
				kPacket.Push(kCPacket);

				SendToContents(kPacket);
			}
		}break;
	case PT_C_M_REQ_FIND_PARTY_USER_LIST:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			BM::Stream kCPacket(PT_M_C_ANS_FIND_PARTY_USER_LIST);
			WriteFindPartyUserListInfoToPacket(*pkMsg, kCPacket);
			g_kServerSetMgr.Locked_SendToUser(kCharGuid, kCPacket, false);
		}break;
	case PT_C_M_REQ_REGIST_PRIVATE_PARTY_FIND:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			BM::Stream kSPacket(PT_C_M_REQ_REGIST_PRIVATE_PARTY_FIND2ND);
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kSPacket) )
			{
				break;
			}
			kSPacket.Push(*(BM::Stream*)pkMsg);
			SendToGlobalPartyMgr(kSPacket);
		}break;

	case PT_N_C_REQ_UNREGIST_PRIVATE:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			ReqDeleteFindParty(kCharGuid);
		}break;
	case PT_M_N_REQ_JOIN_PARTY_REFUSE:
		{
			BM::GUID kCharGuid;
			bool bRefuse = false;
			BYTE byState = 0;
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(bRefuse);
			pkMsg->Pop(byState);
			SetPartyRefuseState(kCharGuid, bRefuse, byState);
		}break;
	case PT_C_N_ANS_JOIN_PARTY:// 유저가 파티에 가입여부를 대답
	case PT_C_N_REQ_LEAVE_PARTY:
	case PT_M_N_NFY_PARTY_USER_PROPERTY:
	case PT_C_N_REQ_CREATE_PARTY:
	case PT_M_N_REQ_PARTY_COMMAND:
	case PT_C_N_REQ_PARTY_CHANGE_OPTION:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			{
				PACKET_ID_TYPE wNewType = 0;
				switch (wType)
				{
					case PT_C_N_ANS_JOIN_PARTY:
						{
							wNewType = PT_C_N_ANS_JOIN_PARTY_2ND;
						}break;
					case PT_C_N_REQ_LEAVE_PARTY:
						{
							wNewType = PT_C_N_REQ_LEAVE_PARTY_2ND;
						}break;
					case PT_M_N_NFY_PARTY_USER_PROPERTY:
						{
							wNewType = PT_M_N_NFY_PARTY_USER_PROPERTY_2ND;
						}break;
					case PT_C_N_REQ_CREATE_PARTY:
						{
							wNewType = PT_C_N_REQ_CREATE_PARTY_2ND;
						}break;
					case PT_C_N_REQ_PARTY_CHANGE_OPTION:
						{
							wNewType = PT_C_N_REQ_PARTY_CHANGE_OPTION_2ND;
						}break;
					case PT_M_N_REQ_PARTY_COMMAND:
						{
							wNewType = PT_M_N_REQ_PARTY_COMMAND_2ND;
						}break;
					default:
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV5, __FUNCTIONW__ << _T("[PT_C_N_ANS_JOIN_PARTY] undefined packet type=") << wType);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						}break;
				}
				BM::Stream kSPacket(wNewType);
				if( S_OK != ::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kSPacket) )
				{	// 캐릭터 정보가 없다?
					// 다른 채널에서 온 요청이었는지 확인해보고,
					ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find( kCharGuid );
					if( find_iter != m_ContOtherChannelJoinInfo.end() )
					{	// 있다면, 다른 채널에서 온 요청.
						BM::Stream Packet(wNewType);
						find_iter->second.UserInfo.WriteToPacket(Packet);	// 정보를 패킷에 넣는다.
						
						Packet.Push(*(BM::Stream*)pkMsg);
						SendToGlobalPartyMgr(Packet);
						break;
					}
					else
					{	// 없다.
						break;
					}
				}

				kSPacket.Push(*(BM::Stream*)pkMsg);
				SendToGlobalPartyMgr(kSPacket);
			}
		}break;
		// PT_C_N_REQ_KICKOUT_PARTY_USER handler 어느것이 진짜?
		// CASE 1:
	case PT_C_N_ANS_JOIN_PARTYFIND:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			
			BM::Stream kSPacket(PT_C_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
			if( S_OK != ::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kSPacket) )
			{// 캐릭터 정보가 없다?
				// 다른 채널에서 온 요청이었는지 확인해보고,
				ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find( kCharGuid );
				if( find_iter != m_ContOtherChannelJoinInfo.end() )
				{// 있다면, 다른 채널에서 온 요청.
					BM::Stream Packet(PT_C_T_ANS_JOIN_PARTYFIND_ITEM_CHECK);
					find_iter->second.UserInfo.WriteToPacket(Packet);	// 정보를 패킷에 넣는다.
					
					Packet.Push(*(BM::Stream*)pkMsg);
					Packet.Push(false);
					Packet.Push(find_iter->second.ChannelNo); // 파티장 채널
					SendToGlobalPartyMgr(Packet);
					break;
				}
				else
				{	// 없다.
					break;
				}
			}
			else
			{// 같은 채널
				kSPacket.Push(*(BM::Stream*)pkMsg);
				kSPacket.Push(true);
				kSPacket.Push(g_kProcessCfg.ChannelNo()); // 파티장 채널
				SendToGlobalPartyMgr(kSPacket);
			}
		}break;
	case PT_C_N_REQ_KICKOUT_PARTY_USER:
		{
			BM::GUID kMasterCharGuid;
			BYTE cCommandType = 0;

			pkMsg->Pop(kMasterCharGuid);
			pkMsg->Pop(cCommandType);

			BM::Stream kTPacket(PT_C_N_REQ_KICKOUT_PARTY_USER_2ND);
			kTPacket.Push(cCommandType);
			bool bRet = (S_OK == ::WritePlayerInfoToPacket_ByGuid(kMasterCharGuid, false, kTPacket));

			size_t const iPreSize = kTPacket.Size();
			if( PCT_KICK_CHARGUID == cCommandType )
			{
				BM::GUID kCharGuid;
				pkMsg->Pop(kCharGuid);
				bRet = (S_OK == ::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kTPacket));
			}
			else if( PCT_KICK_CHARNAME == cCommandType )
			{
				std::wstring kCharName;
				pkMsg->Pop(kCharName);
				bRet = (S_OK == WritePlayerInfoToPacket_ByName(kCharName, kTPacket));
			}

			if( iPreSize != kTPacket.Size() )
			{
				kTPacket.Push(*(BM::Stream*)pkMsg);
				SendToGlobalPartyMgr(kTPacket);
			}
		}break;
	case PT_C_N_REQ_KICKOUT_PARTY_USER_2ND://강퇴
		{
			BYTE bCommandType = 0;
			BM::GUID kPartyGuid;
			SContentsUser kMaster;
			SContentsUser kUser;

			pkMsg->Pop(bCommandType);
			kMaster.ReadFromPacket(*pkMsg);
			kUser.ReadFromPacket(*pkMsg);

			bool const bIsParty = GetCharToParty(kMaster.kCharGuid, kPartyGuid);
			if( bIsParty )
			{
				HRESULT const hRet = KickUser(kPartyGuid, kMaster, kUser.kCharGuid, kUser.kGndKey.GroundNo(), kUser.kMemGuid, kUser.Name());
				if( PRC_Fail_KickUserNoArea == hRet )
				{
					BM::Stream kRetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
					kRetPacket.Push(hRet);
					g_kServerSetMgr.Locked_SendToUser(kMaster.kMemGuid, kRetPacket);//결과					
				}
			}
		}break;
	case PT_C_M_REQ_CHAT_INPUTNOW://일반 채팅 입력도중 외에 전부
		{
			BM::GUID kCharGuid;
			BYTE cChatMode = 0;
			bool bPopup = false;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(cChatMode);
			pkMsg->Pop(bPopup);

			{//패킷스콥
				BM::Stream kUPacket(PT_C_M_REQ_CHAT_INPUTNOW_2ND);
				kUPacket.Push(cChatMode);
				::WritePlayerInfoToPacket_ByGuid(kCharGuid, false, kUPacket);
				kUPacket.Push(bPopup);
				SendToChannelChatMgr(kUPacket);
			}
		}break;
	case PT_M_T_REQ_REGIST_HARDCORE_VOTE:
		{
			int iMode = 0;
			SGroundKey kDungeonGndKey;
			BM::GUID kCharGuid;
			pkMsg->Pop( iMode );
			kDungeonGndKey.ReadFromPacket( *pkMsg );
			pkMsg->Pop( kCharGuid );

			PgGlobalParty *pkParty = GetCharToParty( kCharGuid );
			if ( pkParty )
			{
				class PgFunction_HCDExtFunction
					:	public PgPartyContents_HardCoreDungeon::PgExtFunction
				{
				public:
					PgFunction_HCDExtFunction(){}
					virtual ~PgFunction_HCDExtFunction(){}

					virtual void Release( BYTE const byState, PgPartyContents_HardCoreDungeon::CONT_MEMBER_STATE const &kMemberState )
					{
						switch( byState )
						{
						case E_HCT_VOTE:
							{
								PgPartyContents_HardCoreDungeon::CONT_MEMBER_STATE::const_iterator member_itr = kMemberState.begin();
								for ( ; member_itr != kMemberState.end() ; ++member_itr )
								{
									if ( E_HCT_V_OK >= member_itr->second.byState )
									{
										// 취소 요청 통보
										BM::Stream kMPacket( PT_T_M_ANS_RET_HARDCORE_VOTE_CANCEL, member_itr->first );
										g_kServerSetMgr.Locked_SendToGround( member_itr->first, kMPacket, true );
									}
								}
							}break;
						case H_HCT_READY:
							{
								SPortalWaiter::CONT_WAIT_LIST kWaiterList;

								PgPartyContents_HardCoreDungeon::CONT_MEMBER_STATE::const_iterator member_itr = kMemberState.begin();
								for ( ; member_itr != kMemberState.end() ; ++member_itr )
								{
									if ( E_HCT_V_OK == member_itr->second.byState )
									{
										kWaiterList.insert( std::make_pair( member_itr->first, member_itr->second.kVolatileID ) );
									}
								}

								if ( kWaiterList.size() )
								{
									BM::Stream kFailedPacket( PT_T_T_REQ_RECENT_MAP_MOVE );
									PU::TWriteTable_AA( kFailedPacket, kWaiterList );
									::SendToCenter( kFailedPacket );
								}
							}break;
						default:
							{
							}break;
						}
					}
				};

				PgPartyContents_HardCoreDungeon *pkPartyContents = new PgPartyContents_HardCoreDungeon;
				if ( true == pkParty->AttachPartyContents( pkPartyContents ) )
				{
					pkPartyContents->SetExtFunction<PgFunction_HCDExtFunction>();
					pkPartyContents->SetMode( iMode );
					pkPartyContents->SetDungeonGndKey( kDungeonGndKey );	

					VEC_GUID kMemberGuidList;
					kMemberGuidList.reserve( pkParty->MemberCount() );
					pkParty->GetMemberCharGuidList( kMemberGuidList );

					SChnGroundKey kChnGndKey;

					VEC_GUID::const_iterator member_itr = kMemberGuidList.begin();
					for ( ; member_itr != kMemberGuidList.end() ; ++member_itr )
					{
						if ( true == pkParty->GetMemberChnGndInfo( *member_itr, kChnGndKey ) )
						{
							BM::Stream kMPacket( PT_T_M_ANS_REGIST_HARDCORE_VOTE, *member_itr );
							kMPacket.Push( static_cast<int>(0) );// No Error
							pkPartyContents->WriteToPacket( kMPacket );
							SendToGround( kChnGndKey, kMPacket );
						}
					}
				}
				else
				{
					BM::Stream kMPacket( PT_T_M_ANS_REGIST_HARDCORE_VOTE, kCharGuid );
					kMPacket.Push( static_cast<int>(402015) );// 파티의 하드코어 던젼 입장을 처리중에 있습니다.
					g_kServerSetMgr.Locked_SendToGround( kCharGuid, kMPacket, true );
				}
			}
		}break;
	case PT_M_T_REQ_RET_HARDCORE_VOTE:
		{
			BM::GUID kCharGuid;
			BYTE byState = 0;
			pkMsg->Pop( kCharGuid );
			pkMsg->Pop( byState );

			PgGlobalParty *pkParty = GetCharToParty( kCharGuid );
			if ( pkParty )
			{
				PgPartyContents_HardCoreDungeon *pkPartyContents = dynamic_cast<PgPartyContents_HardCoreDungeon*>(pkParty->GetPartyContents());
				if ( pkPartyContents )
				{
					if ( E_HCT_VOTE == pkPartyContents->GetState() )
					{
						if ( true == pkParty->IsMaster( kCharGuid ) )
						{
							if ( E_HCT_V_OK == byState )
							{
								if ( true == pkPartyContents->IsAllSetState() )
								{
									VEC_GUID kMemberGuidList;
									kMemberGuidList.reserve( pkParty->MemberCount() );
									pkParty->GetMemberCharGuidList( kMemberGuidList );

									VEC_GUID::const_iterator member_itr = kMemberGuidList.begin();
									for ( ; member_itr != kMemberGuidList.end() ; ++member_itr )
									{
										BYTE const byMemberState = pkPartyContents->GetMemberState(*member_itr);
										bool const bIsJoin = ( E_HCT_V_OK >= byMemberState );

										if ( true == bIsJoin )
										{
											BM::Stream kReqJoinPacket( PT_T_M_REQ_JOIN_HARDCORE, *member_itr );
											g_kServerSetMgr.Locked_SendToGround( *member_itr, kReqJoinPacket, true );
										}

										if ( !bIsJoin )
										{
											if ( E_HCT_V_HARDCORE_DUNGEON != byMemberState )
											{
												LeaveParty( *member_itr );
											}
										}
									}

									pkPartyContents->SetWaitMove();
								}
							}
							else
							{
								pkPartyContents = NULL;// NULL 했다!
								pkParty->DetachPartyContents();
							}
						}
						else
						{
							if ( S_OK == pkPartyContents->SetMemberState( kCharGuid, byState ) )
							{
								VEC_GUID kMemberGuidList;
								kMemberGuidList.reserve( pkParty->MemberCount() );
								pkParty->GetMemberCharGuidList( kMemberGuidList );

								VEC_GUID::const_iterator member_itr = kMemberGuidList.begin();
								for ( ; member_itr != kMemberGuidList.end() ; ++member_itr )
								{
									if ( E_HCT_V_OK >= pkPartyContents->GetMemberState(*member_itr) )
									{
										BM::Stream kMPacket( PT_T_M_ANS_REGIST_HARDCORE_VOTE, *member_itr );
										kMPacket.Push( static_cast<int>(0) );// No Error
										pkPartyContents->WriteToPacket( kMPacket );
										g_kServerSetMgr.Locked_SendToGround( *member_itr, kMPacket, true );
									}
								}
							}
						}
					}
				}
			}
		}break;
	case PT_M_T_ANS_JOIN_HARDCORE_FAILED:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop( kCharGuid );

			PgGlobalParty *pkParty = GetCharToParty( kCharGuid );
			if ( pkParty )
			{
				PgPartyContents_HardCoreDungeon *pkPartyContents = dynamic_cast<PgPartyContents_HardCoreDungeon*>(pkParty->GetPartyContents());
				if ( pkPartyContents )
				{
					if ( H_HCT_READY == pkPartyContents->GetState() )
					{
						if ( true == pkParty->IsMaster( kCharGuid ) )
						{
							pkPartyContents = NULL;//Null했다.
							pkParty->DetachPartyContents();
						}
						else
						{
							LeaveParty( kCharGuid );
						}

						if ( SUCCEEDED(pkPartyContents->SetMove()) )
						{
							SReqMapMove_MT kRMM(MMET_GoTopublicGroundParty);
							kRMM.kTargetKey = pkPartyContents->GetDungeonGndKey();

							BM::Stream kMovePacket( PT_T_T_REQ_MAP_MOVE, kRMM );
							pkPartyContents->WriteToPacket_JoinDungeonOrder( kMovePacket );
							::SendToCenter( kMovePacket );

							pkPartyContents = NULL;// Set NULL pkPartyContents
							pkParty->DetachPartyContents();
						}
					}
				}
			}
		}break;
	case PT_T_T_REQ_READY_JOIN_HARDCORE:
		{
			SPortalWaiter::CONT_WAIT_LIST kWaiterList;
			PU::TLoadTable_AA( *pkMsg, kWaiterList );

			if ( kWaiterList.size() )
			{
				SPortalWaiter::CONT_WAIT_LIST::const_iterator wait_itr = kWaiterList.begin();
				PgGlobalParty *pkParty = GetCharToParty( wait_itr->first );
				if ( pkParty )
				{
					PgPartyContents_HardCoreDungeon *pkPartyContents = dynamic_cast<PgPartyContents_HardCoreDungeon*>(pkParty->GetPartyContents());
					if ( pkPartyContents )
					{
						if ( H_HCT_READY == pkPartyContents->GetState() )
						{
							for ( ; wait_itr != kWaiterList.end() ; ++wait_itr )
							{
								pkParty->MovedGnd( wait_itr->first, SChnGroundKey() );// 초기화 해놔야 한다.
								pkPartyContents->SetMemberMoveReady( wait_itr->first, wait_itr->second );								
							}

							pkPartyContents->ReadFromPacket_MapMoveOrder( *pkMsg );

							if ( SUCCEEDED(pkPartyContents->SetMove()) )
							{
								SReqMapMove_MT kRMM(MMET_GoTopublicGroundParty);
								kRMM.kTargetKey = pkPartyContents->GetDungeonGndKey();

								BM::Stream kMovePacket( PT_T_T_REQ_MAP_MOVE, kRMM );
								pkPartyContents->WriteToPacket_JoinDungeonOrder( kMovePacket );
								::SendToCenter( kMovePacket );

								pkPartyContents = NULL;// Set NULL pkPartyContents
								pkParty->DetachPartyContents();
							}
							break;//break!!!!!!!
						}
					}
				}

				BM::Stream kFailedPacket( PT_T_T_REQ_RECENT_MAP_MOVE );
				PU::TWriteTable_AA( kFailedPacket, kWaiterList );
				::SendToCenter( kFailedPacket );
			}
		}break;
	case PT_N_T_NFY_COMMUNITY_STATE_HOMEADDR_PARTY:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop( kCharGuid );
			SHOMEADDR kHomeAddr;
			pkMsg->Pop( kHomeAddr );

			PgGlobalParty* pkParty = GetCharToParty(kCharGuid);
			if( pkParty )
			{
				if( true == pkParty->SetFriendHomeAddr(kCharGuid, kHomeAddr) )
				{
					VEC_GUID kContMember;
					pkParty->GetMemberCharGuidList(kContMember);
					VEC_GUID::const_iterator member_iter = kContMember.begin();
					while( kContMember.end() != member_iter)
					{
						SContentsUser kUser;
						if( S_OK == ::GetPlayerByGuid( (*member_iter), false, kUser) )
						{
							BM::Stream kPacket(PT_T_C_NFY_PARTY_MEMBER_MYHOME);
							kPacket.Push(kCharGuid);
							kPacket.Push(kHomeAddr);
							SendToUser(pkParty, kUser.kCharGuid, kPacket);
						}
						++member_iter;
					}
				}
			}
		}break;
	case PT_C_N_REQ_CREATE_EXPEDITION:				// 원정대 생성 요청.
		{
			BM::GUID kCharGuid;
			pkMsg->Pop(kCharGuid);
			SContentsUser kMaster;
			if( S_OK == ::GetPlayerByGuid(kCharGuid, false, kMaster) )
			{
				HRESULT hCreateRet = ReqCreateExpedition(kMaster, *pkMsg);
				if( PRC_Success != hCreateRet )
				{
					BM::Stream ResultPacket(PT_N_C_ANS_CREATE_EXPEDITION);
					ResultPacket.Push((int)hCreateRet);
					g_kServerSetMgr.Locked_SendToUser(kMaster.kCharGuid, ResultPacket, false);
				}
			}
		}break;
	case PT_C_N_REQ_JOIN_EXPEDITION:				// 원정대 가입 요청.
		{
			BM::GUID kCharGuid, kMasterGuid, kExpeditionGuid;
			int JoinType = EJT_DEFAULT;

			pkMsg->Pop(kExpeditionGuid);
			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(JoinType);
			PgGlobalExpedition* pExpedition = GetExpedition(kExpeditionGuid);
			if( pExpedition )
			{
				kMasterGuid = pExpedition->MasterCharGuid();

				HRESULT hRet = PRC_Fail;
				SContentsUser kUser, kMaster;
				if( EJT_DEFAULT == JoinType )
				{	// 일반적인 가입 요청일 경우에는 Center에서 마스터와 가입시킬 유저 정보를
					// 모두 알고 있기 때문에 바로 구할 수 있다.
					if( (S_OK == ::GetPlayerByGuid(kCharGuid, false, kUser))
						&& (S_OK == ::GetPlayerByGuid(kMasterGuid, false, kMaster)) )
					{
						hRet = ReqJoinExpedition(kMaster, kUser, kExpeditionGuid, *pkMsg);
					}
				}
				else if( EJT_CHANMOVE == JoinType )
				{
					// 다른 채널의 유저가 가입신청을 하면 마스터는 해당 Center에서,
					// 가입하려는 유저는 패킷에서 정보를 읽어온다.
					kUser.ReadFromPacket(*pkMsg);
					if( S_OK == ::GetPlayerByGuid(kMasterGuid, false, kMaster) )
					{
						hRet = ReqJoinExpedition(kMaster, kUser, kExpeditionGuid, *pkMsg);
					}
				}

				if( PRC_Success != hRet )
				{
					BM::Stream ResultPacket(PT_N_C_ANS_JOIN_EXPEDITION);
					ResultPacket.Push(hRet);
					ResultPacket.Push(kExpeditionGuid);
					if( EJT_DEFAULT == JoinType )
					{
						g_kServerSetMgr.Locked_SendToUser(kUser.kCharGuid, ResultPacket, false);
					}
					else if( EJT_CHANMOVE == JoinType )
					{
						ResultPacket.Push(kUser.kCharGuid);

						SendToExpeditionListMgr(ResultPacket);
					}
				}
			}
		}break;
	case PT_C_N_ANS_JOIN_EXPEDITION:				// 원정대 가입 요청 응답.
	case PT_C_N_ANS_INVITE_EXPEDITION:				// 원정대 초대 응답.
		{
			BM::GUID ExpeditionGuid, CharGuid, MasterGuid;
			SContentsUser User, Master;
			bool bJoin = false, bHaveKeyItem = false;

			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(CharGuid);
			pkMsg->Pop(bJoin);

			PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
			if( pExpedition )
			{
				if( S_OK == ::GetPlayerByGuid(CharGuid, false, User) )
				{
					HRESULT hRet = AnsJoinExpedition(User, ExpeditionGuid, bJoin);
					if( hRet != PRC_Success )			// 파티 가입을 못했다.
					{
						BM::GUID MasterGuid = pExpedition->MasterCharGuid();
						if( S_OK == ::GetPlayerByGuid(MasterGuid, false, Master) )
						{
							if( PT_C_N_ANS_JOIN_EXPEDITION == wType )			// 가입.
							{
								BM::Stream Packet(PT_N_C_ANS_JOIN_EXPEDITION);	// 실패 메세지 전송.
								Packet.Push((int)hRet);
								Packet.Push(ExpeditionGuid);
								Packet.Push(Master.kCharGuid);
								Packet.Push(Master.kName);

								g_kServerSetMgr.Locked_SendToUser(User.kCharGuid, Packet, false);
							}
							else if( PT_C_N_ANS_INVITE_EXPEDITION == wType )	// 초대.
							{
								BM::Stream Packet(PT_N_C_ANS_INVITE_EXPEDITION_TO_MASTER);	// 실패 메세지 전송.
								Packet.Push((int)hRet);
								Packet.Push(User.kCharGuid);
								Packet.Push(User.kName);

								g_kServerSetMgr.Locked_SendToUser(Master.kCharGuid, Packet, false);
							}
						}
					}
				}
				else
				{	// 유저 정보를 찾지 못했다면, 접속이 끊어졌거나 다른 채널에 있는 유저.
					// 다른 채널에서 신청 했을 수도 있기 때문에 컨텐츠로 보냄.
					if( PT_C_N_ANS_JOIN_EXPEDITION == wType )
					{	// 가입 요청일 경우만(다른채널 캐릭터에 대한 초대는 할 수 없기 때문에).
						BM::Stream AnsPacket(PT_C_N_ANS_JOIN_EXPEDITION);
						AnsPacket.Push(ExpeditionGuid);
						AnsPacket.Push(CharGuid);
						AnsPacket.Push(pExpedition->MasterCharGuid());
						AnsPacket.Push(bJoin);
						if( true == bJoin )
						{	// 가입 요청 수락인 경우.
							BM::GUID MasterGuid = pExpedition->MasterCharGuid();

							SPartyUserInfo MasterInfo;
							bool result = pExpedition->GetMemberInfo(MasterGuid, MasterInfo);
							if( true == result )
							{	// 가입 요청자가 채널 이동할 때 사용할 정보.
								AnsPacket.Push(g_kProcessCfg.ChannelNo());
								AnsPacket.Push(MasterInfo.kChnGndKey.GroundNo());
							}
						}
						else
						{	// 가입 요청 거절인 경우. 대기정보 삭제.
							pExpedition->DelWaitMember(CharGuid);
							DelCharToExpedition(CharGuid);
						}
						SendToExpeditionListMgr(AnsPacket);						
					}
				}
			}
		}break;
	case PT_C_N_REQ_DISPERSE_EXPEDITION:			// 원정대 해산.
		{
			BM::GUID kMasterGuid;

			pkMsg->Pop(kMasterGuid);

			HRESULT hDisperseRet = ReqDisperseExpedition(kMasterGuid);
		}break;
	case PT_C_N_REQ_LEAVE_EXPEDITION:				// 원정대 탈퇴.
		{
			BM::GUID ExpeditionGuid, CharGuid;
			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(CharGuid);

			HRESULT hRet = ReqLeaveExpedition(CharGuid, ExpeditionGuid);
			if( hRet != PRC_Success )
			{
				BM::Stream AnsPacket(PT_N_C_ANS_LEAVE_EXPEDITION);
				AnsPacket.Push(hRet);
				g_kServerSetMgr.Locked_SendToUser(CharGuid, AnsPacket, false); // 탈퇴유저에게 결과알림.
			}
		}break;
	case PT_C_N_REQ_INVITE_EXPEDITION:				// 원정대 초대
		{
			BM::GUID MasterGuid, ExpeditionGuid;
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(ExpeditionGuid);
			
			SContentsUser Master;
			if( (S_OK == ::GetPlayerByGuid(MasterGuid, false, Master)) )
			{
				HRESULT hRet = ReqInviteExpedition(Master, ExpeditionGuid, *pkMsg);
			}
		}break;
	case PT_C_N_REQ_TEAM_MOVE_EXPEDITION:
		{
			BM::GUID MasterGuid, ExpeditionGuid;
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(ExpeditionGuid);

			HRESULT hRet = ReqTeamMove(MasterGuid, ExpeditionGuid, *pkMsg);
			
			BM::Stream Packet(PT_N_C_ANS_TEAM_MOVE_EXPEDITION);
			Packet.Push(hRet);
			g_kServerSetMgr.Locked_SendToUser(MasterGuid, Packet, false); // 팀이동 결과.
		}break;
	case PT_C_N_REQ_CHANGEMASTER_EXPEDITION:
		{
			BM::GUID ExpeditionGuid, MasterGuid, NewMasterGuid;

			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(NewMasterGuid);

			HRESULT hRet = ReqChangeExpeditionMaster(ExpeditionGuid, MasterGuid, NewMasterGuid);
		}break;
	case PT_C_N_REQ_RENAME_EXPEDITION:
		{
			BM::GUID CharGuid, ExpeditionGuid;
			std::wstring NewName;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(NewName);

			HRESULT hRet = ReqExpeditoinRename(ExpeditionGuid, CharGuid, NewName);
			if( PRC_Success != hRet )
			{
				BM::Stream ErrPacket(PT_N_C_ANS_RENAME_EXPEDITION, hRet);
				g_kServerSetMgr.Locked_SendToUser(CharGuid, ErrPacket, false);
			}
		}break;
	case PT_C_N_REQ_CHANGEOPTION_EXPEDITION:
		{
			BM::GUID CharGuid, ExpeditionGuid;
			SExpeditionOption NewOption;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(ExpeditionGuid);
			NewOption.ReadFromPacket(*pkMsg);

			HRESULT hRet = ReqChangeExpeditionOption(ExpeditionGuid, CharGuid, NewOption);
		}break;
	case PT_C_N_REQ_KICKOUT_EXPEDITION:
		{
			BM::GUID MasterGuid, CharGuid, ExpeditionGuid;

			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(CharGuid);

			HRESULT hRet = ReqKickExpeditionMember(ExpeditionGuid, MasterGuid, CharGuid);
		}break;
	case PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE:
		{
			BM::GUID ExpeditionGuid, CharGuid;
			bool IsAlive;

			pkMsg->Pop(ExpeditionGuid);
			pkMsg->Pop(CharGuid);
			pkMsg->Pop(IsAlive);

			HRESULT hRet = Recv_ExpeditionMemberState(ExpeditionGuid, CharGuid, IsAlive);
		}break;
	case PT_T_T_REQ_CLEAR_JOIN_EXPEDITION_AWAITER:
		{	// 가입하지 않는 가입대기자 정리.
			ClearExpeditionJoinWaitMember(BM::GetTime32());
		}break;
	case PT_N_T_PRE_MAKE_PARTY_EVENT_GROUND:
		{
			BM::GUID PartyGuid;

			pkMsg->Pop(PartyGuid);

			CreateEventGroundParty(PartyGuid);
		}break;
	case PT_M_T_REQ_JOIN_EVENT_GROUND_PARTY:
		{
			BM::GUID CharGuid;
			BM::GUID PartyGuid;
			bool bMaster;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(PartyGuid);
			pkMsg->Pop(bMaster);

			SContentsUser UserInfo;
			if( S_OK == GetPlayerByGuid(CharGuid, false, UserInfo) )
			{
				AddEventGroundParty(PartyGuid, UserInfo, bMaster);
				//AnsJoinParty(PartyGuid, UserInfo, true, bMaster);
			}
		}break;
	case PT_C_M_REQ_INDUN_PARTY_ENTER:
		{
			short ChannelNo = 0;
			BM::GUID CharGuid, MasterGuid;
			pkMsg->Pop(CharGuid);
			pkMsg->Pop(ChannelNo);
			pkMsg->Pop(MasterGuid);

			if( (-1 == ChannelNo) || (g_kProcessCfg.ChannelNo() == ChannelNo) )
			{	// 같은 채널 파티.
				BM::Stream Msg(PT_C_N_REQ_JOIN_PARTYFIND);
				Msg.Push(CharGuid);										// Req Join CharGuid.
				Msg.Push(static_cast<BYTE>(PCT_REQJOIN_CHARGUID));		// JoinType.
				Msg.Push(MasterGuid);									// MasterGuid.
				Msg.Push(true);											// Is Req Join Dungeon Party.

				SendToGlobalPartyMgr(Msg);
			}
			else
			{	// 다른 채널 파티.
				SContentsUser UserInfo;
				if( S_OK == GetPlayerByGuid(CharGuid, false, UserInfo) )
				{					
					BM::Stream Packet(PT_T_T_REQ_JOIN_OTHER_CHANNEL_PARTY);
					Packet.Push(g_kProcessCfg.ChannelNo());	// 내 채널 번호.
					Packet.Push(MasterGuid);				// MasterGuid.
					UserInfo.WriteToPacket(Packet);			// Req Join User Info.

					SendToOtherChannelContents(ChannelNo, PMET_PARTY, Packet);
				}
			}
		}break;
	case PT_T_T_REQ_JOIN_OTHER_CHANNEL_PARTY:
		{
			short ReqChannelNo;
			SContentsUser UserInfo;
			BM::GUID MasterGuid;

			pkMsg->Pop(ReqChannelNo);			// 회신할 채널 번호.
			pkMsg->Pop(MasterGuid);				// 가입 요청할 파티 마스터.
			UserInfo.ReadFromPacket(*pkMsg);	// 가입 요청한 캐릭터 정보.

			// 파티장의 응답 여부에 따라 처리를 해주어야하는데 다른 채널의 캐릭터 정보는 알 수 없기 때문에 저장해 둠.
			SOtherChannelPartyJoinInfo JoinInfo;
			JoinInfo.UserInfo = UserInfo;
			JoinInfo.PartyMasterGuid = MasterGuid;
			JoinInfo.ChannelNo = ReqChannelNo;

			if( FindCharToParty(MasterGuid) )
			{// 파티 마스터가 파티에 속해 있는지 검사
				auto Ret = m_ContOtherChannelJoinInfo.insert( std::make_pair( UserInfo.kCharGuid, JoinInfo) );
				if( !Ret.second )
				{	// 이미 있으면.
					Ret.first->second = JoinInfo;	// 현재 정보로 갱신.
				}

				BM::Stream Msg(PT_C_N_REQ_JOIN_PARTYFIND_2ND);
				UserInfo.WriteToPacket(Msg);
				WritePlayerInfoToPacket_ByGuid(MasterGuid, false, Msg);
				Msg.Push(true);

				SendToGlobalPartyMgr(Msg);
			}
			else
			{// 파티 마스터가 파티에 없으면 유효하지 않은 파티 정보이다.
				BM::Stream Packet(PT_T_T_ANS_JOIN_OTHER_CHANNEL_PARTY_TO_MASTER);
				Packet.Push(UserInfo.kCharGuid);
				Packet.Push(PRC_Fail);
				SendToOtherChannelContents(ReqChannelNo, PMET_PARTY, Packet);//결과
			}
		}break;
	case PT_T_T_ANS_OTHER_CHANNEL_PARTY:
		{
			BM::GUID CharGuid, MasterGuid, PartyGuid;
			short ChannelNo = 0;
			bool bAnsJoin = false;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(PartyGuid);
			pkMsg->Pop(ChannelNo);
			pkMsg->Pop(bAnsJoin);

			if( bAnsJoin )
			{// 대상이 파티에 가입된 상태인지 파티 수락대기중인지 확인 해야한다.
				SContentsUser CharInfo;
				if( S_OK == GetPlayerByGuid(CharGuid, false, CharInfo) )
				{
					BM::Stream Packet(PT_T_M_ANS_RETURN_OTHER_CHANNEL_PARTY);
					Packet.Push(CharGuid);
					Packet.Push(MasterGuid);
					Packet.Push(PartyGuid);
					Packet.Push(ChannelNo);

					g_kServerSetMgr.Locked_SendToGround(CharInfo.kGndKey, Packet, true);
				}
				else
				{
					BM::Stream Packet(PT_T_T_ANS_RETURN_OTHER_CHANNEL_PARTY);
					Packet.Push(CharGuid);
					Packet.Push(MasterGuid);
					Packet.Push(PartyGuid);
					Packet.Push(PRC_Fail);

					SendToOtherChannelContents(ChannelNo, PMET_PARTY, Packet);
				}
			}
			else
			{
				BM::Stream AnsPacket(PT_T_C_ANS_OTHER_CHANNEL_PARTY);
				AnsPacket.Push(ChannelNo);
				AnsPacket.Push(false);

				g_kServerSetMgr.Locked_SendToUser(CharGuid, AnsPacket, false);

				BM::Stream Packet(PT_T_T_ANS_RETURN_OTHER_CHANNEL_PARTY);
				Packet.Push(CharGuid);
				Packet.Push(MasterGuid);
				Packet.Push(PartyGuid);
				Packet.Push(PRC_Fail_Req_Join);

				SendToOtherChannelContents(ChannelNo, PMET_PARTY, Packet);
			}
		}break;
	case PT_M_T_ANS_RETURN_OTHER_CHANNEL_PARTY:
		{
			BM::GUID CharGuid, MasterGuid, PartyGuid;
			short ChannelNo = 0;
			bool InParty = false;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(PartyGuid);
			pkMsg->Pop(ChannelNo);
			pkMsg->Pop(InParty);

			if( InParty )
			{
				BM::Stream Packet(PT_T_T_ANS_RETURN_OTHER_CHANNEL_PARTY);
				Packet.Push(CharGuid);
				Packet.Push(MasterGuid);
				Packet.Push(PartyGuid);
				Packet.Push(PRC_Fail_Party);

				SendToOtherChannelContents(ChannelNo, PMET_PARTY, Packet);
			}
			else
			{
				BM::Stream AnsPacket(PT_T_C_ANS_OTHER_CHANNEL_PARTY);
				AnsPacket.Push(ChannelNo);
				AnsPacket.Push(true);

				g_kServerSetMgr.Locked_SendToUser(CharGuid, AnsPacket, false);
			}
		}break;
	case PT_T_T_ANS_RETURN_OTHER_CHANNEL_PARTY:
		{
			BM::GUID CharGuid, MasterGuid, PartyGuid;
			HRESULT hRet;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(PartyGuid);
			pkMsg->Pop(hRet);

			BM::Stream Packet(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
			Packet.Push(hRet);

			g_kServerSetMgr.Locked_SendToUser(MasterGuid, Packet, false);

			m_ContOtherChannelJoinInfo.erase(CharGuid);
			DelCharToParty(CharGuid);
			PgGlobalParty *pParty = GetParty( PartyGuid );
			if( pParty )
			{
				pParty->DelWait(CharGuid);
			}
		}break;
	case PT_T_T_ANS_JOIN_OTHER_CHANNEL_PARTY_TO_MASTER:
		{
			BM::GUID CharGuid;
			HRESULT hRet;

			pkMsg->Pop(CharGuid);
			pkMsg->Pop(hRet);

			BM::Stream Packet(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
			Packet.Push(hRet);

			g_kServerSetMgr.Locked_SendToUser(CharGuid, Packet, false);
		}break;
	case PT_M_T_REQ_CLEAR_PARTY_WAITER:
		{
			BM::GUID MasterGuid, PartyGuid;
			pkMsg->Pop(MasterGuid);
			pkMsg->Pop(PartyGuid);

			PgGlobalParty * pParty = GetParty(PartyGuid);
			if( pParty )
			{
				DeleteOhterChannelJoinInfo_MasterLeaveParty(MasterGuid, pParty);

				VEC_GUID RecvGuidVec, WaiterVec;
				pParty->GetWaitCharGuidList(WaiterVec);
				VEC_GUID::iterator wait_iter = WaiterVec.begin();
				while(WaiterVec.end() != wait_iter)
				{
					BM::GUID const& CharGuid = (*wait_iter);
					DelCharToParty(CharGuid);
					pParty->Del(CharGuid);
					RecvGuidVec.push_back(CharGuid);//통보 목록 추가
					++wait_iter;
				}

				BM::Stream NfyPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
				NfyPacket.Push(PRC_Fail_Req_Join);
				g_kServerSetMgr.Locked_SendToUser(RecvGuidVec, NfyPacket, false);//사용자 통보
			}
		}break;
	case PT_C_T_NFY_MAPMOVE_TO_PARTYMGR:
		{	// 캐릭터가 접속했을 때, 가입할 파티가 있는지 확인.
			BM::GUID CharGuid;
			pkMsg->Pop(CharGuid);

			// 이 리스트에 guid가 있으면 가입할 파티가 있다.
			ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find(CharGuid);
			if( find_iter != m_ContOtherChannelJoinInfo.end() )
			{ // 찾았다.
				if( find_iter->second.bAccept )
				{
					BM::Stream Packet(PT_C_N_ANS_JOIN_PARTYFIND_2ND);
					if( S_OK == ::WritePlayerInfoToPacket_ByGuid(find_iter->first, false, Packet) )
					{						
						BM::GUID PartyGuid;
						GetCharToParty(find_iter->second.PartyMasterGuid, PartyGuid);
						Packet.Push(true);	// 강제로 가입 시켜야해서..
						Packet.Push(PartyGuid);

						SendToGlobalPartyMgr(Packet);

						m_ContOtherChannelJoinInfo.erase(CharGuid);
					}
				}
				else
				{
					m_ContOtherChannelJoinInfo.erase(CharGuid);
					DelCharToParty(CharGuid);
					
					BM::GUID PartyGuid;
					GetCharToParty(find_iter->second.PartyMasterGuid, PartyGuid);
					PgGlobalParty *pParty = GetParty( PartyGuid );
					if( pParty )
					{
						pParty->DelWait(CharGuid);
					}
				}
			}
		}break;
	case PT_M_T_NFY_MODIFY_EXPEDITION_OPTION:
		{
			BM::GUID ExpeditionGuid;
			SExpeditionOption Option;

			pkMsg->Pop(ExpeditionGuid);
			Option.ReadFromPacket(*pkMsg);

			PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
			if( pExpedition )
			{
				pExpedition->Option(Option);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("invalid packet type=") << wType );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}
	}
	return true;
}

bool PgGlobalPartyMgrImpl::PartyFriendCheckInfo(BM::Stream* pkPacket)
{
	BM::GUID kCharGuid;
	bool bSet = false;
	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(bSet);

	PgGlobalParty* pkParty = GetCharToParty(kCharGuid);
	if( pkParty )
	{
		SContentsUser kUser;
		if( S_OK == ::GetPlayerByGuid(kCharGuid, false, kUser) )
		{
			BM::Stream kIPacket(PT_A_U_SEND_TOFRIEND_PARTY);
			kIPacket.Push(kUser.sChannel);
			kIPacket.Push(pkParty->PartyGuid());
			kIPacket.Push(kCharGuid);
			kIPacket.Push(true);
			pkParty->WriteToMemberInfoList(kIPacket);
			SendToFriendMgr(kIPacket);
			return true;
		}
	}	
	return false;
}

bool PgGlobalPartyMgrImpl::PartyFriendCheck(BM::Stream* pkPacket)
{
	BM::GUID kCharGuid;
	BM::GUID kPartyGuid;
	VEC_UserInfo Vec_PartyUserInfo;
	bool bSet = false;

	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(bSet);
	pkPacket->Pop(kPartyGuid);
	PU::TLoadArray_M(*pkPacket, Vec_PartyUserInfo);

	PgGlobalParty* pkParty = GetParty(kPartyGuid);
	if( !pkParty )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot get Party=") << kPartyGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}


	VEC_UserInfo::iterator iter = Vec_PartyUserInfo.begin();
	while( Vec_PartyUserInfo.end() != iter )
	{
		pkParty->PartyFriendCheck(*iter);
		++iter;
	}

	SyncToLocalPartyMgr(pkParty);//Gnd Local 동기화
	return true;
}

void PgGlobalPartyMgrImpl::SetPartyOptionAreaNo(int const iGroundNo, SPartyOption& rkOption)
{
	if( 0 == rkOption.GetOptionArea_NameNo() )
	{
		CONT_DEF_PARTY_INFO const* pkDefPartyInfo = NULL;
		g_kTblDataMgr.GetContDef(pkDefPartyInfo);

		if( pkDefPartyInfo )
		{
			for(int i=POC_Area1; i<POC_Max; ++i)
			{
				CONT_DEF_PARTY_INFO::const_iterator iter = pkDefPartyInfo->find(i);
				if( pkDefPartyInfo->end() != iter )
				{
					CONT_DEF_PARTY_INFO::mapped_type const kContinentList = iter->second;
					CONT_DEF_PARTY_INFO::mapped_type::const_iterator iter_ContinentList = kContinentList.begin();
					while( kContinentList.end() != iter_ContinentList )
					{
						if( iGroundNo == iter_ContinentList->iGroundNo )
						{
							rkOption.SetOptionArea_NameNo(iter_ContinentList->iArea_NameNo);
							return;
						}							

						++iter_ContinentList;
					}
				}
			}
		}
	}
}

HRESULT PgGlobalPartyMgrImpl::ReqJoinParty(SContentsUser const& rkMaster, SContentsUser const& rkUser, SPartyOption& rkOption)
{
	BM::GUID kPartyGuid;
	BM::GUID ExpeditionGuid;
	bool IsExpedition = false;

	IsExpedition = GetCharToExpedition(rkMaster.kCharGuid, ExpeditionGuid);
	if( IsExpedition )	// 원정대에 들어가 있다면 파티행동을 하면 안됨(마스터).
	{
		g_kServerSetMgr.Locked_SendWarnMessage(rkMaster.kCharGuid, 710067, EL_Warning); // 원정대 참가 중에는 파티 초대/가입을 할 수 없습니다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	IsExpedition = GetCharToExpedition(rkUser.kCharGuid, ExpeditionGuid);
	if( IsExpedition )	// 원정대에 들어가 있다면 파티행동을 하면 안됨(일반유저).
	{
		g_kServerSetMgr.Locked_SendWarnMessage(rkUser.kCharGuid, 710067, EL_Warning); // 원정대 참가 중에는 파티 초대/가입을 할 수 없습니다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	HRESULT const hCheckRet = CheckJoinParty(rkMaster, rkUser);//파티 가입조건이?
	if( PRC_Success_Create != hCheckRet
	&&	PRC_Success != hCheckRet )//생성, 성공 아니면
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCheckRet );
		return hCheckRet;//실패
	}
	
	if( PRC_Success_Create == hCheckRet )//파장이 파티가 없으면 생성
	{
		SetPartyOptionAreaNo(rkMaster.kGndKey.GroundNo(), rkOption);

		std::wstring kNewPartyName;	
		HRESULT const hCreateRet = CreateParty(kNewPartyName, kPartyGuid, rkOption);
		if( PRC_Success != hCreateRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCheckRet );
			return hCheckRet;//생성 실패
		}

		HRESULT const hMasterJoinRet = AnsJoinParty(kPartyGuid, rkMaster, true, true);//마스터를 강제로 가입
		if( PRC_Success != hMasterJoinRet )
		{
			Delete(kPartyGuid);
			DelCharToParty(rkMaster.kCharGuid);//실패면 마스터 파티 삭제
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		if( rkUser.sLevel < rkOption.GetOptionLevel() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Level"));
			return PRC_Fail_Level;// 레벨이 맞지 않음(파티방)
		}

		// Log
		PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
		kLogCont.MemberKey(rkMaster.kMemGuid);
		kLogCont.UID(rkMaster.iUID);
		kLogCont.CharacterKey(rkMaster.kCharGuid);
		kLogCont.ID(rkMaster.kAccountID);
		kLogCont.Name(rkMaster.Name());
		kLogCont.ChannelNo(static_cast<short>(rkMaster.sChannel));
		kLogCont.Class(static_cast<short>(rkMaster.iClass));
		kLogCont.Level(static_cast<short>(rkMaster.sLevel));
		kLogCont.GroundNo(static_cast<int>(rkMaster.kGndKey.GroundNo()));

		PgLog kLog(ELOrderMain_Party, ELOrderSub_Create);
		kLog.Set(0, kNewPartyName);
		std::wstring strPartyInfo = PgLogUtil::GetPartyOptionPublic(rkOption.GetOptionPublicTitle());
		strPartyInfo += _T(" ");
		strPartyInfo += PgLogUtil::GetPartyOptionItemString(rkOption.GetOptionItem());
		kLog.Set(1, strPartyInfo);
		kLog.Set(0, static_cast<int>(rkOption.GetOptionPublicTitle()));
		kLog.Set(1, static_cast<int>(rkOption.GetOptionItem()));
		kLog.Set(2, static_cast<int>(rkOption.GetOptionLevel()));
		kLog.Set(3, static_cast<int>(rkMaster.cGender) );
		kLog.Set(2, kPartyGuid.str());

		kLogCont.Add( kLog );
		kLogCont.Commit();
	}

	bool const bFindGuid = GetCharToParty(rkMaster.kCharGuid, kPartyGuid);//마스터 파티 Guid
	if( !bFindGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	PgGlobalParty* pkParty = GetParty(kPartyGuid);//마스터 파티
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bIsMaster = pkParty->IsMaster(rkMaster.kCharGuid);
	if( !bIsMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;
	}

	if(EPR_NONE != pkParty->Option().GetOptionState())
	{
		return PRC_None;
	}

	HRESULT const hAddWiatRet = pkParty->AddWait(rkUser.kCharGuid);
	if( PRC_Success != hAddWiatRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL From PartyAddWait"));
		return hAddWiatRet;//실패
	}

	AddCharToParty(rkUser.kCharGuid, kPartyGuid);//파티열 등록

	SFindPartyUserListInfo kOutFindPartyUserInfo;
	bool bIsListExist = GetFindPartyList(rkUser.kCharGuid, kOutFindPartyUserInfo);
	if( bIsListExist )
	{
		ReqDeleteFindParty(rkUser.kCharGuid);
	}

	bIsListExist = GetFindPartyList(rkMaster.kCharGuid, kOutFindPartyUserInfo);
	if( bIsListExist )
	{
		ReqDeleteFindParty(rkUser.kCharGuid);
	}

	BM::Stream kUserPacket(PT_N_C_REQ_JOIN_PARTY);//초대 메시지
	kUserPacket.Push(rkMaster.Name());
	kUserPacket.Push(kPartyGuid);
	kUserPacket.Push(rkMaster.kCharGuid);
	kUserPacket.Push(pkParty->PartyName());
	kUserPacket.Push(rkMaster.sLevel);
	kUserPacket.Push(rkMaster.iClass);
	g_kServerSetMgr.Locked_SendToUser(rkUser.kMemGuid, kUserPacket);

	return PRC_Success;//성공
}

HRESULT PgGlobalPartyMgrImpl::ReqJoinPartyFind(SContentsUser const& rkMaster, SContentsUser const& rkUser, SPartyOption const& rkOption, bool bIsDungeonParty /*= false*/)
{
	BM::GUID kPartyGuid;
	BM::GUID ExpeditionGuid;
	bool IsExpedition = false;

	IsExpedition = GetCharToExpedition(rkMaster.kCharGuid, ExpeditionGuid);
	if( IsExpedition )	// 원정대에 들어가 있다면 파티행동을 하면 안됨(마스터).
	{
		g_kServerSetMgr.Locked_SendWarnMessage(rkMaster.kCharGuid, 710067, EL_Warning); // 원정대 참가 중에는 파티 초대/가입을 할 수 없습니다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	IsExpedition = GetCharToExpedition(rkUser.kCharGuid, ExpeditionGuid);
	if( IsExpedition )	// 원정대에 들어가 있다면 파티행동을 하면 안됨(일반유저).
	{
		g_kServerSetMgr.Locked_SendWarnMessage(rkUser.kCharGuid, 710067, EL_Warning); // 원정대 참가 중에는 파티 초대/가입을 할 수 없습니다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	HRESULT const hCheckRet = CheckJoinParty(rkMaster, rkUser, bIsDungeonParty);//파티 가입조건이?
	if( PRC_Success_Create != hCheckRet
		&&	PRC_Success != hCheckRet )//생성, 성공 아니면
	{
		if( PRC_Fail_MineMember == hCheckRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PMS_Fail_DupReqJoin"));
			return PMS_Fail_DupReqJoin; // 이미 다른 파티에 가입 신청한 상태다
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCheckRet);
		return hCheckRet;//실패
	}

	bool const bFindGuid = GetCharToParty(rkMaster.kCharGuid, kPartyGuid);//마스터 파티 Guid
	if( !bFindGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	PgGlobalParty* pkParty = GetParty(kPartyGuid);//마스터 파티
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bIsMaster = pkParty->IsMaster(rkMaster.kCharGuid);
	if( !bIsMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;
	}

	HRESULT const hAddWiatRet = pkParty->AddWait(rkUser.kCharGuid);
	if( PRC_Success != hAddWiatRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hAddWiatRet);
		return hAddWiatRet;//실패
	}

	AddCharToParty(rkUser.kCharGuid, kPartyGuid);//파티열 등록

	SFindPartyUserListInfo kOutFindPartyUserInfo;
	bool bIsListExist = GetFindPartyList(rkUser.kCharGuid, kOutFindPartyUserInfo);
	if( bIsListExist )
	{
		ReqDeleteFindParty(rkUser.kCharGuid);
	}

	bIsListExist = GetFindPartyList(rkMaster.kCharGuid, kOutFindPartyUserInfo);
	if( bIsListExist )
	{
		ReqDeleteFindParty(rkUser.kCharGuid);
	}

	BM::Stream kUserPacket(PT_N_C_REQ_JOIN_PARTYFIND);//초대 메시지
	kUserPacket.Push(rkUser.Name());
	kUserPacket.Push(kPartyGuid);
	kUserPacket.Push(rkUser.kCharGuid);
	kUserPacket.Push(pkParty->PartyName());
	kUserPacket.Push(rkUser.sLevel);
	kUserPacket.Push(rkUser.iClass);
	g_kServerSetMgr.Locked_SendToUser(rkMaster.kMemGuid, kUserPacket);

	ContOtherChannelJoinInfo::iterator find_iter = m_ContOtherChannelJoinInfo.find(rkUser.kCharGuid);
	if( find_iter == m_ContOtherChannelJoinInfo.end() )
	{	
		BM::Stream kRetPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
		kRetPacket.Push( PRC_Success_PartyInvite );
		g_kServerSetMgr.Locked_SendToUser(rkUser.kMemGuid, kRetPacket);//결과
	}
	else
	{
		BM::Stream RetPacket(PT_T_T_ANS_JOIN_OTHER_CHANNEL_PARTY_TO_MASTER);
		RetPacket.Push( rkUser.kCharGuid );
		RetPacket.Push( PRC_Success_PartyInvite );
		SendToOtherChannelContents(rkUser.sChannel, PMET_PARTY, RetPacket);//결과
	}
	return PRC_Success;//성공
}

HRESULT PgGlobalPartyMgrImpl::AnsJoinParty(BM::GUID const &rkPartyGuid, SContentsUser const& rkNewPartyUserInfo, bool const bWantJoin, bool const bMaster)
{
	BM::GUID const &rkCharGuid = rkNewPartyUserInfo.kCharGuid;

	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		DelCharToParty(rkCharGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_None"));
		return PRC_None;
	}

	BM::GUID kUserPartyGuid;
	bool const bFindParty = GetCharToParty(rkNewPartyUserInfo.kCharGuid, kUserPartyGuid);
	if( bWantJoin
	&&	!bFindParty )//생자로 가입 요청 하는 사람
	{
		HRESULT const hAddWaitRet = pkParty->AddWait(rkNewPartyUserInfo.kCharGuid);
		if( PRC_Success != hAddWaitRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hAddWaitRet);
			return hAddWaitRet;
		}
	}
	else if( bFindParty )
	{
		if( rkPartyGuid != kUserPartyGuid )
		{
			PgGlobalParty* pkParty = GetParty(kUserPartyGuid);
			if( pkParty )
			{//대상에게 다른 파티 GUID가 있다면 이미 파티에 가입되었다는 뜻이므로 대기자에서 지워준다.
				pkParty->DelWait(rkCharGuid);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
				return PRC_Fail_Party;
			}
			else
			{
				DelCharToParty(rkCharGuid);
			}
		}
	}

	if( pkParty->MemberCount() >= pkParty->MaxMemberCount() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;//인원수 제한으로 등록 불가
	}	

	//승낙 / 거부
	//성공/ 실패
	HRESULT hAnsJoinRet = PRC_Success;
	if( bWantJoin//가입을 원하고
		&& pkParty //파티가 있고
		&& (bMaster || (EPR_NONE == pkParty->Option().GetOptionState())) ) // 파티가 가입가능한 상태이면
	{
		SPartyUserInfo kPartyUserInfo(rkNewPartyUserInfo);
		kPartyUserInfo.kChnGndKey.Channel( g_kProcessCfg.ChannelNo() );
		hAnsJoinRet = pkParty->Add( kPartyUserInfo, bMaster);//가입
		if( PRC_Success == hAnsJoinRet )
		{
			///////////////////////////////////
			PartyBuffMember(pkParty, rkCharGuid, true);

			SContentsUser kUser;
			if( S_OK == ::GetPlayerByGuid(rkCharGuid, false, kUser) )
			{
				BM::Stream kJPacket(PT_A_U_SEND_TOFRIEND_PARTY);
				kJPacket.Push(kUser.sChannel);
				kJPacket.Push(rkPartyGuid);
				kJPacket.Push(rkCharGuid);
				kJPacket.Push(true);
				pkParty->WriteToMemberInfoList(kJPacket);
				SendToFriendMgr(kJPacket);
			}
			///////////////////////////////////
			if( !bMaster )//마스터가 아니면
			{
				BM::Stream kNfyPacket(PT_N_C_NFY_JOIN_PARTY, rkPartyGuid);
				kPartyUserInfo.WriteToPacket(kNfyPacket);
				SendToPartyMember(pkParty, kNfyPacket, rkCharGuid);//새로운 맴버 가입
			}

			BM::Stream kAnsUserPacket(PT_N_C_ANS_JOIN_PARTY, rkPartyGuid);
			kAnsUserPacket.Push(hAnsJoinRet);
			pkParty->WriteToPacket(kAnsUserPacket);
			g_kServerSetMgr.Locked_SendToUser(rkCharGuid, kAnsUserPacket, false);//가입 환영

			AddCharToParty(rkCharGuid, rkPartyGuid);//다시 추가

			SyncToLocalPartyMgr(pkParty);//Gnd Local 동기화

			SFindPartyUserListInfo kOutFindPartyUserInfo;
			bool const bIsListExist = GetFindPartyList(rkNewPartyUserInfo.kCharGuid, kOutFindPartyUserInfo);
			if( bIsListExist )
			{
				ReqDeleteFindParty(kUser.kCharGuid);
			}

			SContentsUser kLogUser;
			std::wstring kID = std::wstring();
			if( S_OK == ::GetPlayerByGuid(rkCharGuid, false, kLogUser) )
			{
				kID = kLogUser.kAccountID;
			}

			LogPartyAction( pkParty, rkNewPartyUserInfo, ELOrderSub_Join );
			
			SContentsUser MasterInfo;
			GetPlayerByGuid(pkParty->MasterCharGuid(), false, MasterInfo);
			if( MasterInfo.kGndKey.GroundNo() != rkNewPartyUserInfo.kGndKey.GroundNo() )
			{	// 파티장 위치로 이동..
				BM::Stream Packet(PT_T_M_REQ_JOIN_PARTY_MAP_MOVE);
				Packet.Push(rkNewPartyUserInfo.kCharGuid);
				Packet.Push(MasterInfo.kCharGuid);

				SChnGroundKey GndKey(MasterInfo.kGndKey, g_kProcessCfg.ChannelNo());
				SendToGround(GndKey, Packet);
			}
		}
	}
	else
	{
		m_ContOtherChannelJoinInfo.erase(rkCharGuid);
		DelCharToParty(rkCharGuid);

		if( pkParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Req_Join"));
			hAnsJoinRet = PRC_Fail_Req_Join;//취소
			pkParty->DelWait(rkCharGuid);
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_None"));
			hAnsJoinRet = PRC_None;//파티가 증발
		}
	}

	return hAnsJoinRet;
}

void PgGlobalPartyMgrImpl::PartyWaitUserSend(PgGlobalParty* pkParty, SContentsUser const &rkUser)
{
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find Party"));
		return;
	}

	if( rkUser.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find SContentsUser Info"));
		return;
	}

	BM::GUID rkPartyGuid = pkParty->PartyGuid();
	VEC_GUID kRecvGuidVec;
	BM::Stream kNfyPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
	kNfyPacket.Push(PRC_Fail_Req_Join);

	VEC_GUID kWaiterVec;
	pkParty->GetWaitCharGuidList(kWaiterVec);
	VEC_GUID::iterator wait_iter = kWaiterVec.begin();
	while(kWaiterVec.end() != wait_iter)
	{
		BM::GUID const &rkCharGuid = (*wait_iter);
		DelCharToParty(rkCharGuid);
		pkParty->Del(rkCharGuid);
		kRecvGuidVec.push_back(rkCharGuid);//통보 목록 추가
		++wait_iter;
	}

	BM::Stream kGndNfyPacket( PT_N_M_NFY_DELETE_PARTY, rkPartyGuid );
	SendToLocalPartyMgr(pkParty, kGndNfyPacket);//그라운드 파티 삭제

	VEC_GUID kVec;
	pkParty->GetMemberCharGuidList(kVec);
	VEC_GUID::iterator iter = kVec.begin();
	while(kVec.end() != iter)
	{
		BM::GUID const &rkCharGuid = (*iter);
		DelCharToParty(rkCharGuid);
		pkParty->Del(rkCharGuid);
		kRecvGuidVec.push_back(rkCharGuid);//통보 목록 추가

		++iter;
	}

	g_kServerSetMgr.Locked_SendToUser(kRecvGuidVec, kNfyPacket, false);//사용자 통보
}

void PgGlobalPartyMgrImpl::PartyWaitUserSend_Refuse(PgGlobalParty* pkParty, SContentsUser const &rkUser)
{
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find Party"));
		return;
	}

	if( rkUser.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find SContentsUser Info"));
		return;
	}

	DeleteOhterChannelJoinInfo_MasterLeaveParty(rkUser.kCharGuid, pkParty);

	VEC_GUID kRecvGuidVec;
	BM::Stream kNfyPacket(PT_N_C_ANS_JOIN_PARTY_TO_MASTER);
	kNfyPacket.Push(PRC_Fail_Req_Join);
	VEC_GUID kWaiterVec;
	pkParty->GetWaitCharGuidList(kWaiterVec);
	VEC_GUID::iterator wait_iter = kWaiterVec.begin();
	while(kWaiterVec.end() != wait_iter)
	{
		BM::GUID const &rkCharGuid = (*wait_iter);
		DelCharToParty(rkCharGuid);
		pkParty->Del(rkCharGuid);
		kRecvGuidVec.push_back(rkCharGuid);//통보 목록 추가
		++wait_iter;
	}

	//g_kServerSetMgr.Locked_SendToUser(kRecvGuidVec, kNfyPacket, false);//사용자 통보	
}

void PgGlobalPartyMgrImpl::DestroyParty( PgGlobalParty * pkParty )
{
	BM::GUID const kPartyGuid = pkParty->PartyGuid();

	VEC_GUID kRecvGuidVec;
	BM::Stream kNfyPacket(PT_N_C_NFY_PARTY_DESTROY, kPartyGuid );

	pkParty->GetWaitCharGuidList(kRecvGuidVec);
	VEC_GUID::iterator wait_iter = kRecvGuidVec.begin();
	while(kRecvGuidVec.end() != wait_iter)
	{
		BM::GUID const &rkCharGuid = (*wait_iter);
		DelCharToParty(rkCharGuid);
		pkParty->Del(rkCharGuid);
		++wait_iter;
	}

	BM::Stream kGndNfyPacket( PT_N_M_NFY_DELETE_PARTY, kPartyGuid );
	SendToLocalPartyMgr(pkParty, kGndNfyPacket);//그라운드 파티 삭제
	
	VEC_GUID kVec;
	pkParty->GetMemberCharGuidList(kVec);
	VEC_GUID::iterator itr = kVec.begin();
	for( ; kVec.end() != itr ; ++itr )
	{
		BM::GUID const &rkCharGuid = (*itr);
		DelCharToParty(rkCharGuid);

		PartyBuffMember(pkParty, rkCharGuid, false);
		pkParty->Del(rkCharGuid);
		kRecvGuidVec.push_back(rkCharGuid);//통보 목록 추가

		// Log
		SContentsUser kLogUser;
		if( S_OK == ::GetPlayerByGuid(rkCharGuid, false, kLogUser) )
		{
			DeleteLog(kLogUser, kPartyGuid);
		}
	}

	if ( kRecvGuidVec.size() )
	{
		g_kServerSetMgr.Locked_SendToUser(kRecvGuidVec, kNfyPacket, false);//사용자 통보
	}
	
	Delete(kPartyGuid);//파티 삭제
}

void PgGlobalPartyMgrImpl::DestroyParty(BM::GUID const &rkPartyGuid)
{
	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find Party"));
		return;
	}

	DestroyParty( pkParty );
}

HRESULT PgGlobalPartyMgrImpl::LeaveParty( BM::GUID const &kCharGuid, bool const bIsExpedition )
{
	//assert(BM::GUID::NullData() != kUserInfo.kCharGuid);

	BM::GUID const kPartyGuid = DelCharToParty( kCharGuid );
	PgGlobalParty* pkParty = GetParty(kPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}	

	SPartyUserInfo kLeaveMember;
	pkParty->GetMemberInfo(kCharGuid, kLeaveMember);

	////////////////////////////////////////////////
	
	int iFriendCount = 0;
	iFriendCount = pkParty->GetFriendCount(kCharGuid);
	if( iFriendCount != 0 )
	{
		BM::Stream kKPacket(PT_A_U_SEND_TOFRIEND_PARTY);
		kKPacket.Push(g_kProcessCfg.ChannelNo());
		kKPacket.Push(kPartyGuid);
		kKPacket.Push(kCharGuid);
		kKPacket.Push(false);
		pkParty->WriteToMemberInfoList(kKPacket);
		SendToFriendMgr(kKPacket);
	}

	PartyBuffMember(pkParty, kCharGuid, false);
	// 파티장 주의에 있는 사람으로 파티장 권한이 주어진다
	HRESULT hDelRet;
	BM::GUID kNewMasterGuid;
	bool hMasterRet = pkParty->SetPartyMaster(kCharGuid, kNewMasterGuid);
	if( hMasterRet )
	{
		hDelRet = pkParty->Del(kCharGuid, kNewMasterGuid);
	}
	else
	{
		hDelRet = pkParty->Del(kCharGuid);
	}

	////////////////////////////////////////////////
	if( PRC_Success_Waiter == hDelRet)
	{
		SContentsUser kContentsUser;
		::GetPlayerByGuid( kCharGuid, false, kContentsUser );

		AnsJoinParty(pkParty->PartyGuid(), kContentsUser, false, false);//가입을 거부 했다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Req_Join"));
		return PRC_Fail_Req_Join;
	}

	bool bChangeMaster = (PRC_Success_Master == hDelRet);//마스터가 변경

	SPartyUserInfo kNewMaster;
	pkParty->GetMasterInfo(kNewMaster);//마스터 정보

	BM::Stream kMemberPacket(PT_N_C_NFY_LEAVE_PARTY_USER);//삭제됨 멤버들에게 보냄
	kMemberPacket.Push(kCharGuid);
	kMemberPacket.Push(bChangeMaster);

	BM::Stream kMapPacket(PT_N_M_NFY_LEAVE_PARTY_USER);//삭제됨 로컬파티매니저
	kMapPacket.Push(pkParty->PartyGuid());
	kMapPacket.Push(kCharGuid);
	kMapPacket.Push(bChangeMaster);

	if( bChangeMaster )
	{
		kMemberPacket.Push(kNewMaster.kName);
		kMemberPacket.Push(kNewMaster.kCharGuid);

		kMapPacket.Push(kNewMaster.kCharGuid);

		DeleteOhterChannelJoinInfo_MasterLeaveParty(kCharGuid, pkParty);
	}

	kMemberPacket.Push(bIsExpedition);

	kMapPacket.Push(bIsExpedition);

	pkParty->AllowEmpty(false);

	SendToLocalPartyMgr(pkParty, kMapPacket);//Gnd Local

	SendToGround( kLeaveMember.kChnGndKey,kMapPacket );

	SendToPartyMember(pkParty, kMemberPacket);//남은 멤버들에게
	SendToUser( kLeaveMember.kChnGndKey.Channel(), kLeaveMember.kMemberGuid, kMemberPacket );

	if( (PRC_Success_Destroy == hDelRet)
		&& (false == bIsExpedition) )
	{
		DeleteOhterChannelJoinInfo_MasterLeaveParty(kCharGuid, pkParty);
		bool const bRet = Delete(pkParty->PartyGuid());
		if( bRet )
		{
			return hDelRet;
		}
	}

	return PRC_Success;
}

HRESULT PgGlobalPartyMgrImpl::KickUser(BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, BM::GUID const &rkKickCharGuid, int const &KickUserGroundNo, BM::GUID const &rkKickMemberGuid, std::wstring const& rkKickName)
{
	//assert(rkPartyGuid != BM::GUID::NullData());
	if( rkUser.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	BM::GUID kPartyGuid;
	if( !GetCharToParty(rkKickCharGuid, kPartyGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( rkPartyGuid != kPartyGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool bIsMasterOrder = pkParty->IsMaster(rkUser.kCharGuid);
	if( !bIsMasterOrder )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;
	}

	bool bItsMe = rkUser.kCharGuid == rkKickCharGuid;
	if( bItsMe )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Me"));
		return PRC_Fail_Me;
	}

	SPartyUserInfo kLeaveMember;
	if( !pkParty->GetMemberInfo(rkKickCharGuid, kLeaveMember) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//킥 할 사람이 없다
	}

	{
		CONT_DEFMAP const *pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);
		if( !pkDefMap )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
		
		CONT_DEFMAP::const_iterator iter = pkDefMap->find(KickUserGroundNo);
		if( pkDefMap->end() == iter )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Cannot find GroundDef User[") << rkUser.Name().c_str() << _T("],GroundNo[") << KickUserGroundNo << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		bool bLocalValue = false;
		static LOCAL_MGR::NATION_CODE const eNation = static_cast<LOCAL_MGR::NATION_CODE>(g_kLocal.ServiceRegion());
		switch (eNation)
		{
		case LOCAL_MGR::NC_SINGAPORE:
		case LOCAL_MGR::NC_THAILAND:
		case LOCAL_MGR::NC_INDONESIA:
		case LOCAL_MGR::NC_VIETNAM:
		case LOCAL_MGR::NC_PHILIPPINES:
			{
				if( ((*iter).second.iAttr == GATTR_CHAOS_MISSION) )
				{
					bLocalValue = true;
				}
			}break;
		default:
			{
				if( ((*iter).second.iAttr == GATTR_MISSION) || ((*iter).second.iAttr == GATTR_CHAOS_MISSION) )
				{
					bLocalValue = true;
				}
			}break;
		}
		if( 0 != ((*iter).second.iAttr & GATTR_FLAG_SUPER) )
		{
			return PRC_Fail_KickUserNoArea;
		}

		if( true == bLocalValue )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_KickUserNoArea"));
			return PRC_Fail_KickUserNoArea;
		}
	}

	DelCharToParty(rkKickCharGuid);
	////////////////////////////////////////////////
	PartyBuffMember(pkParty, rkKickCharGuid, false);

	pkParty->Del(rkKickCharGuid);

	int iFriendCount = 0;
	iFriendCount = pkParty->GetFriendCount(rkKickCharGuid);
	if( iFriendCount != 0 )
	{
		SContentsUser kUser;
		if( S_OK == ::GetPlayerByGuid(rkKickCharGuid, false, kUser) )
		{
			BM::Stream kLPacket(PT_A_U_SEND_TOFRIEND_PARTY);
			kLPacket.Push(kUser.sChannel);
			kLPacket.Push(rkPartyGuid);
			kLPacket.Push(rkKickCharGuid);
			kLPacket.Push(false);
			pkParty->WriteToMemberInfoList(kLPacket);
			SendToFriendMgr(kLPacket);
		}
	}
	////////////////////////////////////////////////

	BM::Stream kMemberPacket(PT_N_C_NFY_KICKOUT_PARTY_USER);
	kMemberPacket.Push(rkKickCharGuid);
	SendToPartyMember(pkParty, kMemberPacket);// 멤버들에게 통보
	g_kServerSetMgr.Locked_SendToUser(rkKickCharGuid, kMemberPacket, false);//킥 당한 사람에게도

	BM::Stream kMapPacket(PT_N_M_NFY_LEAVE_PARTY_USER);
	kMapPacket.Push(pkParty->PartyGuid());
	kMapPacket.Push(rkKickCharGuid);
	kMapPacket.Push(false);//마스터 변경없다
	SendToLocalPartyMgr(pkParty, kMapPacket);//로컬파티에게 보냄
	SendToGround( kLeaveMember.kChnGndKey, kMapPacket );//킥 당한 유저의 Gnd LocalMng에도 전송

	// Log
	PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
	kLogCont.MemberKey(rkKickMemberGuid);
	kLogCont.CharacterKey(rkKickCharGuid);

	SContentsUser kKickUser;
	std::wstring kKickID = std::wstring();
	short iChannel = 0;
	short iClass = 0;
	short iLevel = 0;
	int iGround = 0;
	int iUID = 0;
	int iGender = 0;
	if( S_OK == ::GetPlayerByGuid(rkKickCharGuid, false, kKickUser) )
	{
		kKickID = static_cast<std::wstring>(kKickUser.kAccountID);
		iChannel = static_cast<short>(kKickUser.sChannel);
		iClass = static_cast<short>(kKickUser.iClass);
		iLevel = static_cast<short>(kKickUser.sLevel);
		iGround = static_cast<int>(kKickUser.kGndKey.GroundNo());
		iUID = kKickUser.iUID;
		iGender = static_cast<int>(kKickUser.cGender);
	}
	kLogCont.UID(iUID);
	kLogCont.ID(kKickID);
	kLogCont.Name(rkKickName);
	kLogCont.ChannelNo(static_cast<short>(iChannel));
	kLogCont.Class(static_cast<short>(iClass));
	kLogCont.Level(static_cast<short>(iLevel));
	kLogCont.GroundNo(static_cast<int>(iGround));

	PgLog kLog(ELOrderMain_Party, ELOrderSub_Exile);
	kLog.Set(0, pkParty->PartyName());	// Message1
	kLog.Set(1, rkUser.Name());	// Message2
	kLog.Set(3, iGender );
	kLog.Set(2, pkParty->PartyGuid().str());	//guidValue
	kLog.Set(3, rkUser.kCharGuid.str() );// guidValue2

	kLogCont.Add(kLog);
	kLogCont.Commit();

	return true;
}

HRESULT PgGlobalPartyMgrImpl::ChangeMaster( BM::GUID const &kMasterCharGuid, BM::GUID const &kTargetCharGuid)
{
	PgGlobalParty* pkParty = GetCharToParty( kMasterCharGuid );

	try
	{
		if( !pkParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_ChangeMaster"));
			throw (HRESULT)PRC_Fail_ChangeMaster;
		}

		bool bRet = pkParty->IsMaster(kMasterCharGuid);
		if( !bRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
			throw (HRESULT)PRC_Fail_NotMaster;
		}

		HRESULT const hRet = pkParty->IsChangeMaster();
		if ( PRC_Success != hRet )
		{
			throw (HRESULT)hRet;
		}

		bRet = pkParty->ChangeMaster(kTargetCharGuid);
		if( !bRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_ChangeMaster"));
			throw (HRESULT)PRC_Fail_ChangeMaster;
		}
	}
	catch( HRESULT eErrorCode )
	{
		BM::Stream kErrPacket( PT_N_C_ANS_PARTY_CHANGE_MASTER, eErrorCode );
		SendToUser( pkParty, kMasterCharGuid, kErrPacket );
		return eErrorCode;
	}
	
	BM::GUID const &rkNewPartyMasterGuid = pkParty->MasterCharGuid();

	BM::Stream kPacket(PT_N_C_ANS_PARTY_CHANGE_MASTER);
	kPacket.Push(static_cast<HRESULT>(PRC_Success_Master));
	kPacket.Push(rkNewPartyMasterGuid);
	SendToPartyMember(pkParty, kPacket);

	SPartyUserInfo kUserInfo;
	pkParty->GetMasterInfo(kUserInfo);
	SendToGround( kUserInfo.kChnGndKey, kPacket );

	BM::Stream kLocalGndPacket(PT_N_M_NFY_PARTY_CHANGE_MASTER);
	kLocalGndPacket.Push(pkParty->PartyGuid());
	kLocalGndPacket.Push(rkNewPartyMasterGuid);
	SendToLocalPartyMgr(pkParty, kLocalGndPacket);

	///////////////////////////////////
	PartyBuffMember(pkParty, rkNewPartyMasterGuid, true);
	///////////////////////////////////

	return PRC_Success;
}

bool PgGlobalPartyMgrImpl::MovedProcess(BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, short const nChannelNo )
{
	//assert( !rkUser.Empty() );

	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGlobalExpedition * pExpedition = NULL;
	if( pkParty->Expedition() ) 
	{
		pExpedition = GetPartyToExpedition(rkPartyGuid);
		if( NULL == pExpedition )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	PartyWaitUserSend_Refuse(pkParty, rkUser);

	bool bIsWaiter = pkParty->IsWait(rkUser.kCharGuid);
	if( bIsWaiter )//파티 초대 대상자가 맵 이동
	{//파티 난입 기능으로 인해 파티 초대 대상자가 맵을 이동해도 상관없어서 주석처리 함.
		//AnsJoinParty(rkPartyGuid, rkUser, false, false);//거부 한것으로 처리
	}
	else// 도착한 그라운드로 세팅
	{
		SPartyUserInfo kPartyUserInfo;
		if( pkParty->GetMemberInfo(rkUser.kCharGuid, kPartyUserInfo) )
		{
			SChnGroundKey const kChnGndKey( rkUser.kGndKey, nChannelNo );
			if( pkParty->MovedGnd(rkUser.kCharGuid, kChnGndKey) )
			{
				BM::Stream kMemberPacket(PT_N_C_NFY_PARTY_USER_MAP_MOVE);
				kMemberPacket.Push(rkUser.kCharGuid);
				kMemberPacket.Push(rkUser.kGndKey);
				SendToPartyMember(pkParty, kMemberPacket);// 파티원들에게 통보

				BM::Stream kOldMapPacket(PT_N_M_NFY_PARTY_USER_MAP_MOVE);
				kOldMapPacket.Push(pkParty->PartyGuid());
				kOldMapPacket.Push(rkUser.kGndKey);
				kOldMapPacket.Push(rkUser.kCharGuid);
				SendToGround( kPartyUserInfo.kChnGndKey, kOldMapPacket );//이전맵으로 통보

				SyncToLocalPartyMgr(pkParty);//맵이동 대상인 새로운 맵의 로컬파티 매니저들에게 통보
				
				if( NULL != pExpedition )
				{
					if( ES_DISPERSE != pExpedition->State() ) 
					{
						BM::Stream NfyPacket(PT_N_M_NFY_EXPEDITION_USER_MAP_MOVE);
						NfyPacket.Push(pExpedition->ExpeditionGuid());
						NfyPacket.Push(rkUser.kCharGuid);
						NfyPacket.Push(rkUser.kGndKey);
						SendToGround( kPartyUserInfo.kChnGndKey, NfyPacket);
						SyncToLocalPartyMgr(pExpedition);
					}
				}

				///////////////////////////////////
				PartyBuffMember(pkParty, rkUser.kCharGuid, true);
				///////////////////////////////////
			}
		}
	}
	return true;
}

void PgGlobalPartyMgrImpl::SendToPartyMember( PgGlobalParty const *pkParty, BM::Stream const& rkSndPacket, BM::GUID const &rkIgnore)
{
	if( !pkParty ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Party is NULL"));
		return;
	}

	VEC_GUID kVec;
	pkParty->GetMemberGuidList( kVec, true, g_kProcessCfg.ChannelNo(), rkIgnore );
	if( kVec.size() )
	{
		g_kServerSetMgr.Locked_SendToUser( kVec, rkSndPacket, true );
	}

	pkParty->GetMemberGuidList( kVec, true, CProcessConfig::GetPublicChannel(), rkIgnore );
	VEC_GUID::const_iterator guid_itr = kVec.begin();
	for ( ; guid_itr != kVec.end() ; ++guid_itr )
	{
		g_kSwitchAssignMgr.SendToUser( *guid_itr, rkSndPacket );
	}
}

void PgGlobalPartyMgrImpl::SendToPartyMember( PgParty const *pkParty, BM::Stream const& rkSndPacket, BM::GUID const &rkIgnore)
{
	if( !pkParty ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Party is NULL"));
		return;
	}

	VEC_GUID kVec;
	pkParty->GetMemberGuidList( kVec, true, g_kProcessCfg.ChannelNo(), rkIgnore );
	if( kVec.size() )
	{
		g_kServerSetMgr.Locked_SendToUser( kVec, rkSndPacket, true );
	}

	pkParty->GetMemberGuidList( kVec, true, CProcessConfig::GetPublicChannel(), rkIgnore );
	VEC_GUID::const_iterator guid_itr = kVec.begin();
	for ( ; guid_itr != kVec.end() ; ++guid_itr )
	{
		g_kSwitchAssignMgr.SendToUser( *guid_itr, rkSndPacket );
	}
}

void PgGlobalPartyMgrImpl::SyncToLocalPartyMgr(const PgGlobalParty* pkParty)
{
	if( !pkParty ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Party is NULL"));
		return;
	}


	BM::Stream kPacket(PT_N_M_NFY_SYNC_PARTY_MEMBER, pkParty->PartyGuid());

	pkParty->WriteToPacket(kPacket);

	SendToLocalPartyMgr(pkParty, kPacket);
}

void PgGlobalPartyMgrImpl::SyncToLocalPartyMgr(PgGlobalExpedition const * pExpedition)
{
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Expedition is NULL"));
		return;
	}

	BM::Stream Packet(PT_N_M_NFY_SYNC_EXPEDITION_MEMBER, pExpedition->ExpeditionGuid());
	
	// 각 파티 guid를 먼저 넣는다. 로컬에서 해당하는 파티를 찾아 로컬 원정대에 넣기 위함.
	typedef std::vector<PgGlobalParty*> ContParty;
	VEC_GUID PartyList;
	pExpedition->GetPartyList(PartyList);

	Packet.Push(PartyList.size());

	VEC_GUID::const_iterator iter = PartyList.begin();
	while( iter != PartyList.end() )
	{
		PgGlobalParty * pParty = GetParty( *iter );
		if( pParty )
		{
			Packet.Push(pParty->PartyGuid());
		}
		++iter;
	}

	pExpedition->WriteToPacket(Packet);

	SendToLocalPartyMgr(pExpedition, Packet);
}

void PgGlobalPartyMgrImpl::SendToLocalPartyMgr( PgGlobalParty const *pkParty, BM::Stream const& rkPacket )
{
	if( !pkParty ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Party is NULL"));
		return;
	}

	VEC_GUID kVec;
	pkParty->GetMemberCharGuidList(kVec);
	if( !kVec.size() ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! MemberCharGuidList Size is 0"));
		return;
	}

	CheckOverlap kCheck;
	VEC_GUID::iterator iter = kVec.begin();
	while(kVec.end() != iter)
	{
		SPartyUserInfo kPartyMember;
		if( pkParty->GetMemberInfo(*iter, kPartyMember) )
		{
			auto bRet = kCheck.insert( kPartyMember.kChnGndKey );
			if( bRet.second )
			{
				SendToGround( kPartyMember.kChnGndKey, rkPacket);
			}
		}
		++iter;
	}
}

void PgGlobalPartyMgrImpl::SendToLocalPartyMgr(PgGlobalExpedition const * pExpedition, const BM::Stream & Packet)
{
	if( !pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Expedition is NULL"));
		return;
	}

	VEC_GUID Vec_Guid;
	pExpedition->GetMemberCharGuidList(Vec_Guid);
	if( Vec_Guid.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! MemberCharGuidList Size is 0"));
		return;
	}

	CheckOverlap Check;
	VEC_GUID::iterator iter = Vec_Guid.begin();
	while( iter != Vec_Guid.end() )
	{
		SPartyUserInfo PartyMember;
		if( pExpedition->GetMemberInfo(*iter, PartyMember) )
		{
			auto bRet = Check.insert( PartyMember.kChnGndKey );
			if( bRet.second )
			{
				SendToGround( PartyMember.kChnGndKey, Packet);
			}
		}
		++iter;
	}
}

bool PgGlobalPartyMgrImpl::ChangeMemberAbility(BM::GUID const &rkPartyGuid, SContentsUser const &rkUser, BM::Stream &rkPacket)
{
	//assert(BM::GUID::NullData() != rkPartyGuid);
	//assert( !rkUser.Empty() );

	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BYTE cChangeAbilType = 0;
	rkPacket.Pop( cChangeAbilType );

	BYTE const cChangedAbil = pkParty->ChangeAbility(rkUser.kCharGuid, cChangeAbilType, rkPacket);
	if( 0 != cChangedAbil )
	{
		BM::Stream kPacket(PT_M_C_NFY_CHANGE_MEMBER_ABIL, rkUser.kCharGuid);
		if( pkParty->WriteToChangedAbil(rkUser.kCharGuid, cChangedAbil, kPacket) )
		{
			SendToPartyMember(pkParty, kPacket);
			bool const IsExpedition = pkParty->Expedition();	// 원정대에 속해 있는 파티라면.
			if( IsExpedition )
			{	// 원정대원 모두한테 패킷을 보냄(미니 원정대 정보창에 HP/MP 정보 표시용도).
				PACKET_ID_TYPE ExpeditionPacketType = PT_M_C_NFY_CHANGE_EXPEDITION_MEMBER_ABIL;
				PgGlobalExpedition * pExpedition = GetPartyToExpedition(pkParty->PartyGuid());
				kPacket.ModifyData(0, &ExpeditionPacketType, sizeof(ExpeditionPacketType));
				SendToExpedition(pExpedition, kPacket);
			}
		}
	}

	return true;
}

bool PgGlobalPartyMgrImpl::SendParty_Packet(BM::GUID const &rkCharGuid, BM::Stream const &rkPacket)
{
	BM::GUID kPartyGuid;
	if( !GetCharToParty(rkCharGuid, kPartyGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGlobalParty* pkParty = GetParty(kPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SendToPartyMember(pkParty, rkPacket);
	return true;
}

HRESULT PgGlobalPartyMgrImpl::ReqCreateParty(SContentsUser const& rkMaster, BM::Stream& rkPacket)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	BM::GUID kPartyGuid;
	bool const bMasterParty = GetCharToParty(rkMaster.kCharGuid, kPartyGuid);
	if( bMasterParty )
	{// 캐릭터가 파티에 가입되어 있거나 대기자면 가입되어있을 때는 패스, 가입대기자면 대기취소하고 생성한다.
		bool MasterParty = true;
		PgGlobalParty * pParty = GetParty(kPartyGuid);
		if( pParty )
		{
			VEC_GUID WaiterVec;
			pParty->GetWaitCharGuidList(WaiterVec);
			VEC_GUID::iterator wait_iter = WaiterVec.begin();
			while(WaiterVec.end() != wait_iter)
			{
				if( rkMaster.kCharGuid == *wait_iter )
				{// 파티 가입 대기자였다.
					DelCharToParty(*wait_iter);
					pParty->DelWait(*wait_iter);
					MasterParty = false;
					break;
				}
				++wait_iter;
			}
		}
		if( MasterParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
			return PRC_Fail_Party;//이미 파티가 있는 마스터
		}
	}

	std::wstring	kNewPartyName;
	SPartyOption	kNewOption;
	BYTE			byPartyState = 0;

	rkPacket.Pop(kNewPartyName);
	kNewOption.ReadFromPacket(rkPacket);
	rkPacket.Pop(byPartyState);
	kNewOption.byPartyState |= byPartyState;

	if( kNewPartyName.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( MAX_PARTYNAME_LEN < kNewPartyName.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Max"));
		return PRC_Fail_Max;
	}

	bool const bFiltered = g_kFilterString.Filter(kNewPartyName, false, FST_ALL);
	bool const bUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_PARTY_NAME, kNewPartyName));
	if( bFiltered
	||	bUniCodeFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_BadName"));
		return PRC_Fail_BadName;
	}

	SetPartyOptionAreaNo(rkMaster.kGndKey.GroundNo(), kNewOption);

	HRESULT const hCreateRet = CreateParty(kNewPartyName, kPartyGuid, kNewOption);
	if( PRC_Success != hCreateRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCreateRet);
		return hCreateRet;//생성 실패
	}

	HRESULT const hMasterJoinRet = AnsJoinParty(kPartyGuid, rkMaster, true, true);//마스터를 강제로 가입
	if( PRC_Success != hMasterJoinRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		Delete(kPartyGuid);
		DelCharToParty(rkMaster.kCharGuid);//실패면 마스터 파티 삭제
		return PRC_Fail;
	}

	////////////////////////////////////////////////
	EPartyReturnCode eRet = PRC_Fail_NoMaster;
	bool const bIsParty = GetCharToParty(rkMaster.kCharGuid, kPartyGuid);
	if( bIsParty )
	{
		PgGlobalParty* pkParty = GetParty(kPartyGuid);
		if( pkParty )
		{
			bool const bIsMaster = pkParty->IsMaster(rkMaster.kCharGuid);
			if( bIsMaster )
			{
				eRet = PRC_Success;

				SyncToLocalPartyMgr(pkParty);

				SendToPartyMember(pkParty, rkPacket);

				// Log
				PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
				kLogCont.MemberKey(rkMaster.kMemGuid);
				kLogCont.UID(rkMaster.iUID);
				kLogCont.CharacterKey(rkMaster.kCharGuid);
				kLogCont.ID(rkMaster.kAccountID);
				kLogCont.Name(rkMaster.Name());
				kLogCont.ChannelNo(static_cast<short>(rkMaster.sChannel));
				kLogCont.Class(static_cast<short>(rkMaster.iClass));
				kLogCont.Level(static_cast<short>(rkMaster.sLevel));
				kLogCont.GroundNo(static_cast<int>(rkMaster.kGndKey.GroundNo()));

				PgLog kLog(ELOrderMain_Party, ELOrderSub_Create);
				kLog.Set(0, static_cast<std::wstring>(pkParty->PartyName()));
				std::wstring strPartyInfo = PgLogUtil::GetPartyOptionPublic(pkParty->Option().GetOptionPublicTitle());
				strPartyInfo += _T(" ");
				strPartyInfo += PgLogUtil::GetPartyOptionItemString(pkParty->Option().GetOptionItem());
				kLog.Set(1, strPartyInfo);
				kLog.Set(0, static_cast<int>(pkParty->Option().GetOptionPublicTitle()));
				kLog.Set(1, static_cast<int>(pkParty->Option().GetOptionItem()));
				kLog.Set(2, static_cast<int>(pkParty->Option().GetOptionLevel()));
				kLog.Set(3, static_cast<int>(rkMaster.cGender) );
				kLog.Set(2, pkParty->PartyGuid().str());

				kLogCont.Add(kLog);
				kLogCont.Commit();

				BM::Stream kPacket(PT_N_C_ANS_PARTY_CHANGE_MASTER);
				kPacket.Push(PRC_Success);
				kPacket.Push(rkMaster.kCharGuid);
				g_kServerSetMgr.Locked_SendToGround(rkMaster.kGndKey, kPacket, true);
			}
		}
	}

	if( PRC_Success != eRet )//실패시 마스터만
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}
	////////////////////////////////////////////////

	return PRC_Success;
}

HRESULT PgGlobalPartyMgrImpl::ReqRenameParty( BM::GUID const &rkCharGuid, BM::Stream& rkPacket )
{
	PgGlobalParty* pkParty = GetCharToParty( rkCharGuid );
	if( pkParty )
	{
		return ReqRenameParty( pkParty, rkCharGuid, rkPacket );
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
	return PRC_Fail_NotMaster;
}

HRESULT PgGlobalPartyMgrImpl::ReqChangeOptionParty( SContentsUser const &kMaster, BM::Stream &rkPacket )
{
	PgGlobalParty* pkParty = GetCharToParty( kMaster.kCharGuid );
	if( pkParty )
	{
		if ( PRC_Success == ReqRenameParty( pkParty, kMaster.kCharGuid, rkPacket ) )
		{
			// Log
			PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
			kLogCont.MemberKey(kMaster.kMemGuid);
			kLogCont.UID(kMaster.iUID);
			kLogCont.CharacterKey(kMaster.kCharGuid);
			kLogCont.ID(kMaster.kAccountID);
			kLogCont.Name(kMaster.Name());
			kLogCont.ChannelNo(kMaster.sChannel);
			kLogCont.Class(static_cast<short>(kMaster.iClass));
			kLogCont.Level(static_cast<short>(kMaster.sLevel));
			kLogCont.GroundNo(static_cast<int>(kMaster.kGndKey.GroundNo()));

			PgLog kLog(ELOrderMain_Party, ELOrderSub_Modify);
			kLog.Set(0, static_cast<std::wstring>(pkParty->PartyName()));
			std::wstring strPartyInfo = PgLogUtil::GetPartyOptionPublic(pkParty->Option().GetOptionPublicTitle());
			strPartyInfo += _T(" ");
			strPartyInfo += PgLogUtil::GetPartyOptionItemString(pkParty->Option().GetOptionItem());
			kLog.Set(1, strPartyInfo);
			kLog.Set(0, static_cast<int>(pkParty->Option().GetOptionPublicTitle()));
			kLog.Set(1, static_cast<int>(pkParty->Option().GetOptionItem()));
			kLog.Set(2, static_cast<int>(pkParty->Option().GetOptionLevel()));
			kLog.Set(3, static_cast<int>(kMaster.cGender) );
			kLog.Set(2, pkParty->PartyGuid().str());
			kLogCont.Add(kLog);
			kLogCont.Commit();
			return PRC_Success;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
	return PRC_Fail_NotMaster;
}

HRESULT PgGlobalPartyMgrImpl::ReqRenameParty( PgGlobalParty * pkParty, BM::GUID const &rkCharGuid, BM::Stream& rkPacket )
{
	try
	{
		bool const bMaster = pkParty->IsMaster( rkCharGuid );
		if( !bMaster )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
			throw (HRESULT)PRC_Fail_NotMaster;
		}

		HRESULT const hRet = pkParty->IsChangeOption();
		if ( PRC_Success != hRet )
		{
			throw hRet;
		}

		bool bClear = false;
		std::wstring kNewPartyName;
		rkPacket.Pop(kNewPartyName);
		rkPacket.Pop(bClear);

		if( kNewPartyName.empty()
			&&	!bClear )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			throw (HRESULT)PRC_Fail;
		}

		if( MAX_PARTYNAME_LEN < kNewPartyName.size()
			&&	!bClear )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Max"));
			throw (HRESULT)PRC_Fail_Max;
		}

		bool const bFiltered = g_kFilterString.Filter(kNewPartyName, false, FST_ALL);
		bool const bUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_PARTY_NAME, kNewPartyName));
		if( bFiltered
			||	bUniCodeFiltered )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_BadName"));
			throw (HRESULT)PRC_Fail_BadName;
		}

		if( !bClear )
		{
			pkParty->PartyName(kNewPartyName);
		}
		else
		{
			//GM 초기화이면 해제 해 주지 않는다.(기존의 이름이 욕설일 수도 있으니깐)
			pkParty->PartyName(std::wstring());//Clear Name
		}
	}
	catch ( HRESULT eErrorCode )
	{
		BM::Stream kErrPacket( PT_N_C_ANS_PARTY_RENAME, eErrorCode );
		SendToUser( pkParty, rkCharGuid, kErrPacket );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! ReqRenameParty isn't Success"));
		return eErrorCode;
	}

	SPartyOption kNewOption;
	kNewOption.ReadFromPacket( rkPacket );
	kNewOption.SetOptionLevel(90);
	SPartyOption kOption = pkParty->Option();
	if(EPR_CLIENT & kNewOption.GetOptionState())
	{
		kOption.AddOptionState(EPR_CLIENT);
	}
	else
	{
		kOption.RemoveOptionState(EPR_CLIENT);
	}
	kNewOption.SetOptionState(kOption.GetOptionState());
	pkParty->Option(kNewOption);

	SendOptionParty(pkParty);

	BM::Stream kLocalPacket(PT_N_M_NFY_PARTY_RENAME);
	kLocalPacket.Push(pkParty->PartyGuid());
	kLocalPacket.Push(pkParty->PartyName());
	pkParty->Option().WriteToPacket(kLocalPacket);
	SendToLocalPartyMgr(pkParty, kLocalPacket);
	return PRC_Success;
}

bool PgGlobalPartyMgrImpl::Delete(BM::GUID const &rkPartyGuid)
{
	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

//	PartyNameSet::iterator iter = m_kContPartyNameSet.find(pkParty->PartyName());

	bool const bDelRet = PgPartyMgr< PgGlobalParty >::Delete(rkPartyGuid);
	if( bDelRet )
	{
/*
		if( m_kContPartyNameSet.end() != iter )
		{
			m_kContPartyNameSet.erase(iter);
		}
*/
	}
	return bDelRet;
}

void PgGlobalPartyMgrImpl::DeleteLog(SContentsUser const &rkCaster, BM::GUID const &rkPartyGuid)
{
	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( pkParty )
	{
		// Log
		PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
		kLogCont.MemberKey(rkCaster.kMemGuid);
		kLogCont.UID(rkCaster.iUID);
		kLogCont.CharacterKey(rkCaster.kCharGuid);
		kLogCont.ID(rkCaster.kAccountID);
		kLogCont.Name(rkCaster.Name());
		kLogCont.ChannelNo(static_cast<short>(rkCaster.sChannel));
		kLogCont.Class(static_cast<short>(rkCaster.iClass));
		kLogCont.Level(static_cast<short>(rkCaster.sLevel));
		kLogCont.GroundNo(static_cast<int>(rkCaster.kGndKey.GroundNo()));

		PgLog kLog(ELOrderMain_Party, ELOrderSub_Delete);
		kLog.Set(0, static_cast<std::wstring>(pkParty->PartyName()));
		std::wstring strPartyInfo = PgLogUtil::GetPartyOptionPublic(pkParty->Option().GetOptionPublicTitle());
		strPartyInfo += _T(" ");
		strPartyInfo += PgLogUtil::GetPartyOptionItemString(pkParty->Option().GetOptionItem());
		kLog.Set(1, strPartyInfo);
		kLog.Set(0, static_cast<int>(pkParty->Option().GetOptionPublicTitle()));
		kLog.Set(1, static_cast<int>(pkParty->Option().GetOptionItem()));
		kLog.Set(2, static_cast<int>(pkParty->Option().GetOptionLevel()));
		kLog.Set(3, static_cast<int>(rkCaster.cGender) );
		kLog.Set(2, pkParty->PartyGuid().str());

		kLogCont.Add(kLog);
		kLogCont.Commit();
	}
}

HRESULT PgGlobalPartyMgrImpl::CreateParty(std::wstring const& rkPartyName, BM::GUID& rkOutGuid, SPartyOption const& kNewOption)
{
	HRESULT const hRet = PgPartyMgr< PgGlobalParty >::CreateParty(rkPartyName, rkOutGuid, kNewOption);
	if( PRC_Success != hRet )
	{
		return hRet;
	}

	return hRet;
}

bool PgGlobalPartyMgrImpl::SummonMember(BM::GUID const &rkPartyGuid, SContentsUser const &rkCaster)
{
	if( rkCaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGlobalParty* pkParty = GetParty(rkPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	VEC_GUID kCharGuids;
	pkParty->GetMemberCharGuidList(kCharGuids, rkCaster.kCharGuid);

	BM::Stream kPacket(PT_N_M_ANS_PARTY_COMMAND);
	kPacket.Push(rkCaster.kCharGuid);
	kPacket.Push((BYTE)PC_Summon_Member);
	kPacket.Push(kCharGuids);
	g_kServerSetMgr.Locked_SendToGround(rkCaster.kGndKey, kPacket);
	return true;
}

void PgGlobalPartyMgrImpl::PartyBuffMember(PgGlobalParty* pkParty, BM::GUID const &rkCharGuid, bool bBuff)
{
	if( !pkParty )
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Cannot find Party Info Null Char=") << rkCharGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Party is NULL"));
		return;
	}

	VEC_GUID kCharGuids;
	pkParty->GetMemberCharGuidList(kCharGuids);	

	SPartyUserInfo kPartyMaster;
	if( pkParty->GetMemberInfo(pkParty->MasterCharGuid(), kPartyMaster) )
	{
		VEC_GUID::iterator iter = kCharGuids.begin();
		while( kCharGuids.end() != iter )
		{
			SPartyUserInfo kPartyMember;

			if( pkParty->GetMemberInfo((*iter), kPartyMember) )
			{		
				BM::Stream kPacket(PT_N_M_REQ_PARTY_BUFF);
				kPacket.Push(bBuff);
				kPacket.Push(kPartyMaster.GroundNo());
				kPacket.Push(pkParty->PartyGuid());
				kPacket.Push(rkCharGuid);
				kPacket.Push(kPartyMember.kCharGuid);
				pkParty->WriteToMemberInfoList(kPacket);

				SendToGround( kPartyMember.kChnGndKey, kPacket );
			}
			++iter;
		}
	}
}

bool PgGlobalPartyMgrImpl::GetMemberGroundKey( BM::GUID const & rkCharGuid, SChnGroundKey & rkChnGndKey )const
{
	BM::GUID kPartyGuid;
	if(!GetCharToParty(rkCharGuid,kPartyGuid))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgGlobalParty* pkParty = GetParty(kPartyGuid);
	if( !pkParty )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SPartyUserInfo kPartyUserInfo;
	if(!pkParty->GetMemberInfo(rkCharGuid,kPartyUserInfo))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkChnGndKey = kPartyUserInfo.kChnGndKey;
	return true;
}

bool PgGlobalPartyMgrImpl::LogPartyAction( PgParty* const pkParty,  SContentsUser const& kUserInfo, EOrderSubType const kType )const
{
	PgLogCont kLogCont(ELogMain_Contents_Party, ELogSub_Party);				
	kLogCont.MemberKey(kUserInfo.kMemGuid);
	kLogCont.UID(kUserInfo.iUID);
	kLogCont.CharacterKey(kUserInfo.kCharGuid);
	kLogCont.ID(kUserInfo.kAccountID);
	kLogCont.Name(kUserInfo.kName);
	kLogCont.ChannelNo( kUserInfo.sChannel);
	kLogCont.Class(static_cast<short>(kUserInfo.iClass));
	kLogCont.Level(static_cast<short>(kUserInfo.sLevel));
	kLogCont.GroundNo(static_cast<int>(kUserInfo.kGndKey.GroundNo()));

	PgLog kLog(ELOrderMain_Party, kType);
	switch ( kType )
	{
	case ELOrderSub_Join:
		{
			std::wstring strPartyInfo = PgLogUtil::GetPartyOptionPublic(pkParty->Option().GetOptionPublicTitle());
			strPartyInfo += _T(" ");
			strPartyInfo += PgLogUtil::GetPartyOptionItemString(pkParty->Option().GetOptionItem()
				);
			kLog.Set(3, static_cast<int>(kUserInfo.cGender) );
			kLog.Set(1, strPartyInfo);
			kLog.Set(0, static_cast<std::wstring>(pkParty->PartyName()));
			kLog.Set(2, pkParty->PartyGuid().str());
		}break;
	case ELOrderSub_Break:
		{
			kLog.Set(3, static_cast<int>(kUserInfo.cGender) );
			kLog.Set(0, static_cast<std::wstring>(pkParty->PartyName()));
			kLog.Set(2, pkParty->PartyGuid().str());
			
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Unknown Type=") << static_cast<int>(kType) );
			return false;
		}break;
	}

	kLogCont.Add(kLog);
	kLogCont.Commit();
	return true;
}

typedef struct tagSFunctionFindPartyListForEach : public PgGlobalPartyMgrImpl::SFunctionForEach
{
	tagSFunctionFindPartyListForEach(BM::Stream &rkPacket, BM::Stream& rkSendPacket)
		: m_rkPacket(rkPacket), m_rkSendPacket(rkSendPacket)
	{
	}

	bool GetPartyMasterGroundAttribute(PgGlobalParty* pkParty, int &rkAtrribute)
	{
		if( pkParty )
		{
			SPartyUserInfo kMaster;
			if( true == pkParty->GetMasterInfo(kMaster) )
			{
				CONT_DEFMAP const* pkDefMap = NULL;
				g_kTblDataMgr.GetContDef(pkDefMap);

				if( pkDefMap )
				{
					CONT_DEFMAP::const_iterator find_iter = pkDefMap->find(kMaster.GroundNo());
					if( pkDefMap->end() != find_iter )
					{
						rkAtrribute = (*find_iter).second.iAttr;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool PartySearchCheck(PgGlobalParty* pkParty, int iPartyAttribute, int iPartyContinent, int iPartyArea_NameNo)
	{
		if( pkParty )
		{
			// 원정대 파티면 검색되면 안됨.
			if( true == pkParty->Expedition() ) 
			{
				return false;
			}
			int iGroundAttrubute = 0;
			if( false == GetPartyMasterGroundAttribute(pkParty, iGroundAttrubute) )
			{
				return false;
			}

			if( (GATTR_FLAG_NOPARTY) & iGroundAttrubute )
			{
				return false;
			}

			if( (iPartyAttribute == pkParty->Option().GetOptionAttribute()) || (0 == iPartyAttribute) )
			{
				if( iPartyContinent == pkParty->Option().GetOptionContinent() )
				{
					if( (iPartyArea_NameNo == pkParty->Option().GetOptionArea_NameNo()) 
						|| (0 == iPartyAttribute)  
						|| (480801 == pkParty->Option().GetOptionArea_NameNo() ) )
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	BYTE GetPartyState(PgGlobalParty* pkParty, int& iMasterMapNo)
	{
		if( pkParty )
		{
			BM::GUID const& kMasterGuid = pkParty->MasterCharGuid();

			SContentsUser kUser;
			if( S_OK == ::GetPlayerByGuid(kMasterGuid, false, kUser) )
			{
				CONT_DEFMAP const* pkDefMap = NULL;
				g_kTblDataMgr.GetContDef(pkDefMap);

				iMasterMapNo = kUser.kGndKey.GroundNo();
				if( 0 == iMasterMapNo )
				{
					return EPS_REFUSE;	// 맵 이동중.
				}

				CONT_DEFMAP::const_iterator find_iter = pkDefMap->find(iMasterMapNo);
				if( pkDefMap->end() == find_iter )
				{
					return EPS_ERROR;
				}

				if( pkParty->Option().GetOptionState() )		// 파티 가입 거부.
				{
					return EPS_REFUSE;
				}

				if( (find_iter->second.iAttr & GKIND_MISSION)
					|| (find_iter->second.iAttr & GKIND_INDUN)
					|| (find_iter->second.iAttr & GATTR_FLAG_STATIC_DUNGEON) )
				{
					return EPS_RUNNING;
				}
				else if( (find_iter->second.iAttr & GKIND_SUPER)
					|| (find_iter->second.iAttr & GKIND_HARDCORE) )
				{
					return EPS_REFUSE;
				}
				else
				{
					return EPS_WAITING;
				}
			}
		}

		return EPS_ERROR;
	}

	virtual void operator() (PgGlobalPartyMgrImpl::PartyHash const& rkContParty)
	{
		int iPartyAttribute = 0;
		int iPartyContinent = 0;
		int iPartyArea_NameNo = 0;
		int iMasterMapNo = 0;

		m_rkPacket.Pop(iPartyAttribute);
		m_rkPacket.Pop(iPartyContinent);
		m_rkPacket.Pop(iPartyArea_NameNo);

		size_t const iRetWrPos = m_rkSendPacket.WrPos();
		m_rkSendPacket.Push( static_cast< BYTE >(PRC_Fail_NoMaster) ); // 없는걸 기본으로
		size_t const iCountWrPos = m_rkSendPacket.WrPos();
		int iCount = 0;
		m_rkSendPacket.Push( iCount );

		SPartyUserInfo kPartyMember;
		PgGlobalPartyMgrImpl::PartyHash::const_iterator iter = rkContParty.begin();
		while( rkContParty.end() != iter )
		{
			PgGlobalPartyMgrImpl::PartyHash::mapped_type const pkParty = iter->second;
			if( pkParty )
			{
				if( (0 == pkParty->Option().GetOptionState())
					&& (false == pkParty->Expedition()) )
				{
					SPartyUserInfo MasterInfo;
					pkParty->GetMasterInfo(MasterInfo);//마스터 정보

					m_rkSendPacket.Push( pkParty->PartyGuid() );
					m_rkSendPacket.Push( pkParty->PartyName() );
					m_rkSendPacket.Push( static_cast< BYTE >(pkParty->MemberCount()) );
					m_rkSendPacket.Push( static_cast< BYTE >(pkParty->MaxMemberCount()) );
					m_rkSendPacket.Push( pkParty->Option().iPartyOption );
					m_rkSendPacket.Push( pkParty->Option().iPartyLevel );
					m_rkSendPacket.Push( pkParty->Option().PartySubName() );
					m_rkSendPacket.Push( pkParty->Option().iPartyAttribute );
					m_rkSendPacket.Push( pkParty->Option().iPartyContinent );
					m_rkSendPacket.Push( pkParty->Option().iPartyArea_NameNo );
					m_rkSendPacket.Push( GetPartyState(pkParty, iMasterMapNo) );
					m_rkSendPacket.Push( pkParty->MasterCharGuid() );
					m_rkSendPacket.Push( MasterInfo.kName );
					m_rkSendPacket.Push( MasterInfo.iClass );
					m_rkSendPacket.Push( MasterInfo.sLevel );
					m_rkSendPacket.Push( iMasterMapNo );

					VEC_GUID kVec;
					pkParty->GetMemberCharGuidList(kVec, pkParty->MasterCharGuid());

					int const iMemberSize = static_cast<int>(PV_MAX_MEMBER_CNT-1);
					int iMemberCount = 0;
					VEC_GUID::const_iterator Member_iter = kVec.begin();
					while(kVec.end() != Member_iter)
					{
						if( pkParty->GetMemberInfo(*Member_iter, kPartyMember) )
						{
							kPartyMember.WriteToPacket(m_rkSendPacket);
							++iMemberCount;
						}
						if(iMemberSize <= iMemberCount)
						{
							break;
						}
						++Member_iter;
					}
					for(int i=iMemberCount; i<iMemberSize; ++i)
					{
						static const SPartyUserInfo NullData;
						NullData.WriteToPacket(m_rkSendPacket);
					}

					++iCount;
					if( PV_MAX_PARTY_LIST_CNT <= iCount )
					{
						break;
					}
				}
			}

			++iter;
		}

		if( 0 < iCount )
		{
			BYTE const ucRet = PRC_Success;
			m_rkSendPacket.ModifyData(iRetWrPos, &ucRet, sizeof(ucRet));
			m_rkSendPacket.ModifyData(iCountWrPos, &iCount, sizeof(iCount));
		}
	}
private:
	BM::Stream& m_rkPacket;
	BM::Stream& m_rkSendPacket;
} SFunctionFindPartyListForEach;

void PgGlobalPartyMgrImpl::WritePartyListInfoToPacket(BM::Stream &rkPacket, BM::Stream& rkSendPacket)
{
	ForEach( SFunctionFindPartyListForEach(rkPacket, rkSendPacket) );
}

typedef struct tagSFunctionForEachFindPartyUserList : public PgGlobalPartyMgrImpl::FunctionForEachFindPartyUserList
{
	tagSFunctionForEachFindPartyUserList(BM::Stream& rkPacket, BM::Stream& rkSendPacket)
		: m_rkPacket(rkPacket), m_rkSendPacket(rkSendPacket)
	{
	}

	virtual void operator() (PgGlobalPartyMgrImpl::FindPartyUserInfo const& rkContFindPartyUser)
	{
		int iClass = 0;
		unsigned short sHLevel = 0, sLLevel = 0;

		m_rkPacket.Pop(iClass);
		m_rkPacket.Pop(sLLevel);
		m_rkPacket.Pop(sHLevel);

		size_t const iRetWrPos = m_rkSendPacket.WrPos();
		m_rkSendPacket.Push(static_cast<BYTE>(FPRC_Fail));
		size_t const iCountWrPos = m_rkSendPacket.WrPos();
		int iCount = 0;
		m_rkSendPacket.Push(iCount);

		PgGlobalPartyMgrImpl::FindPartyUserInfo::const_iterator iter = rkContFindPartyUser.begin();
		while( iter != rkContFindPartyUser.end() )
		{
			PgGlobalPartyMgrImpl::FindPartyUserInfo::mapped_type const pkFindPartyUser = iter->second;
			if( pkFindPartyUser )
			{
				if( FindPartyUserSearchCheck(pkFindPartyUser, iClass, sHLevel, sLLevel) )
				{
					m_rkSendPacket.Push(pkFindPartyUser->kCharGuid);
					m_rkSendPacket.Push(pkFindPartyUser->iClass);
					m_rkSendPacket.Push(pkFindPartyUser->iOriginalClass);
					m_rkSendPacket.Push(pkFindPartyUser->sLevel);
					m_rkSendPacket.Push(pkFindPartyUser->kGndKey);
					m_rkSendPacket.Push(pkFindPartyUser->kName);
					m_rkSendPacket.Push(pkFindPartyUser->kContents);
					++iCount;
				}				
			}
			++iter;
		}

		if( 0 < iCount )
		{
			BYTE const ucRet = FPRC_Success;
			m_rkSendPacket.ModifyData(iRetWrPos, &ucRet, sizeof(ucRet));
			m_rkSendPacket.ModifyData(iCountWrPos, &iCount, sizeof(iCount));
		}
	}

	bool FindPartyUserSearchCheck(SFindPartyUserListInfo* pkFindPartyUser, int iClass, unsigned short sHLevel, unsigned short sLLevel)
	{
		if( pkFindPartyUser )
		{
			if( iClass != pkFindPartyUser->iClass )
			{
				if( 0 != iClass )
				{
					return false;
				}
			}

			if( (sHLevel < pkFindPartyUser->sLevel) ||
				(sLLevel > pkFindPartyUser->sLevel) )
			{
				if( 0 != sHLevel && 0 != sLLevel)
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

private:
	BM::Stream& m_rkPacket;
	BM::Stream& m_rkSendPacket;
}SFunctionForEachFindPartyUserList;

void PgGlobalPartyMgrImpl::WriteFindPartyUserListInfoToPacket(BM::Stream &rkPacket, BM::Stream& rkSendPacket)
{
	ForEach( SFunctionForEachFindPartyUserList(rkPacket, rkSendPacket) );
}

bool PgGlobalPartyMgrImpl::GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid, bool const bIgnoreCharGuid )const
{
	BM::GUID kPartyGuid = kGuid;
	if ( !bPartyGuid )
	{
		if ( !GetCharToParty( kGuid, kPartyGuid ) )
		{
			return false;
		}
	}

	PgGlobalPartyMgrImpl *pkGlobalPartyMgr = const_cast<PgGlobalPartyMgrImpl*>(this);
	PgGlobalParty *pkParty = pkGlobalPartyMgr->GetParty( kPartyGuid );
	if ( pkParty )
	{
		pkParty->GetMemberCharGuidList( rkOutCharGuidList, (true == bIgnoreCharGuid ? kGuid : BM::GUID::NullData()) );
		return true;
	}
	return false;
}

bool PgGlobalPartyMgrImpl::PartyBuffRefresh(BM::GUID const &kGuid, BM::GUID const &rkPartyGuid)
{
	SContentsUser kUser;
	if( S_OK == ::GetPlayerByGuid(kGuid, false, kUser) )
	{
		PgGlobalParty* pkParty = GetParty(rkPartyGuid);
		if( !pkParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PartyBuffMember(pkParty, kUser.kCharGuid, true);
	}
	return true;
}

bool PgGlobalPartyMgrImpl::SendToGround( SChnGroundKey const &kChnGndkey, BM::Stream const &kPacket )const
{
	if ( kChnGndkey.IsEmpty() )
	{
		return false;
	}

	return ::SendToChannelGround( kChnGndkey.Channel(), static_cast<SGroundKey const>(kChnGndkey), kPacket );
}

bool PgGlobalPartyMgrImpl::SendToUser( PgParty *pkParty, BM::GUID const &rkGuid, BM::Stream const &kPacket )const
{
	SPartyUserInfo kUserInfo;
	if ( pkParty && pkParty->GetMemberInfo( rkGuid, kUserInfo ) )
	{
		return SendToUser( kUserInfo.kChnGndKey.Channel(), kUserInfo.kMemberGuid, kPacket );
	}
	return false;
}

bool PgGlobalPartyMgrImpl::SendToUser( short const nChannel, BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const
{
	if ( nChannel )
	{
		return g_kServerSetMgr.Locked_SendToUser( kMemberGuid, kPacket, true );//나간사람에게
	}

	return g_kSwitchAssignMgr.SendToUser( kMemberGuid, kPacket );
}

void PgGlobalPartyMgrImpl::SendOptionParty(PgGlobalParty * pkParty)
{
	if(NULL==pkParty)
	{
		return;
	}

	BM::Stream kPacket(PT_N_C_ANS_PARTY_CHANGE_OPTION);
	kPacket.Push((BYTE)PRC_Success);
	pkParty->Option().WriteToPacket(kPacket);

	SyncToLocalPartyMgr(pkParty);
	SendToPartyMember(pkParty, kPacket);
}

HRESULT PgGlobalPartyMgrImpl::ReqCreateFindParty(SContentsUser const & rkChar, BM::Stream &rkPacket)
{
	if( rkChar.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SFindPartyUserListInfo kOutFindPartyUserInfo;
	bool const bIsListExist = GetFindPartyList(rkChar.kCharGuid, kOutFindPartyUserInfo);
	if( bIsListExist )
	{
		ReqDeleteFindParty(rkChar.kCharGuid);
	}

	std::wstring kContents;
	rkPacket.Pop(kContents);

	if( kContents.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FPRC_Fail"));
		return FPRC_Fail;
	}

	if( FPV_MAX_CONTENTS_LENGTH < kContents.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FPRC_Fail_Max"));
		return FPRC_Fail_Max;
	}

	bool const bFiltered = g_kFilterString.Filter(kContents, false, FST_ALL);
	bool const bUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_PARTY_NAME, kContents));
	if( bFiltered || bUniCodeFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FPRC_Fail_BadName"));
		return FPRC_Fail_BadName;
	}

	HRESULT const hCreateRet = CreateFindParty(rkChar, kContents);
	if( FPRC_Success != hCreateRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return hCreateRet"));
		return hCreateRet;//생성 실패
	}

	return FPRC_Success;
}

HRESULT PgGlobalPartyMgrImpl::CreateFindParty(SContentsUser const& rkChar, std::wstring& rkContents)
{
	SFindPartyUserListInfo* pkNewFindPartyUser = m_kFindPartyUserInfoPool.New();
	if( !pkNewFindPartyUser )
	{
		return FPRC_Fail;
	}

	pkNewFindPartyUser->Clear();

	auto kRet = m_kContFindPartyUserInfo.insert(std::make_pair(rkChar.kCharGuid, pkNewFindPartyUser));
	if( !kRet.second )
	{
		m_kFindPartyUserInfoPool.Delete(pkNewFindPartyUser);
		return FPRC_Fail;
	}

	pkNewFindPartyUser->iClass = rkChar.iBaseClass;
	pkNewFindPartyUser->iOriginalClass = rkChar.iClass;
	pkNewFindPartyUser->sLevel = rkChar.sLevel;
	pkNewFindPartyUser->kCharGuid = rkChar.kCharGuid;
	pkNewFindPartyUser->kGndKey = rkChar.kGndKey;
	pkNewFindPartyUser->kName = rkChar.Name();
	pkNewFindPartyUser->kContents = rkContents;

	return FPRC_Success;
}

bool PgGlobalPartyMgrImpl::ReqDeleteFindParty(BM::GUID const& rkCharGuid)
{
	FindPartyUserInfo::iterator Del_iter = m_kContFindPartyUserInfo.find(rkCharGuid);

	if( Del_iter == m_kContFindPartyUserInfo.end() )
	{
		return false;
	}

	FindPartyUserInfo::mapped_type pkDelUserInfo = (*Del_iter).second;
	if( pkDelUserInfo )
	{
		m_kContFindPartyUserInfo.erase(rkCharGuid);
		pkDelUserInfo->Clear();
		m_kFindPartyUserInfoPool.Delete(pkDelUserInfo);

		BM::Stream NfyUnRegistPacket(PT_N_C_NFY_UNREGIST_PRIVATE);
		g_kServerSetMgr.Locked_SendToUser(rkCharGuid, NfyUnRegistPacket, false);

		return true;
	}

	return false;
}

bool PgGlobalPartyMgrImpl::GetFindPartyList(BM::GUID const& rkCharGuid, SFindPartyUserListInfo& rkOutFindPartyUserInfo)
{
	PgGlobalPartyMgrImpl::FindPartyUserInfo::iterator iter = m_kContFindPartyUserInfo.find( rkCharGuid );
	if( iter == m_kContFindPartyUserInfo.end() )
	{
		return false;
	}

	rkOutFindPartyUserInfo = (*(*iter).second);
	return true;
}

void PgGlobalPartyMgrImpl::SetPartyRefuseState(BM::GUID const& kCharGuid, bool const byRefuse, BYTE const byState)
{
	PgGlobalParty * pkParty = GetCharToParty(kCharGuid);
	if( pkParty && pkParty->IsMaster(kCharGuid) )
	{
		SPartyOption kPartyOption = pkParty->Option();
		BYTE const byOldState = kPartyOption.GetOptionState();
		if(byRefuse)
		{
			kPartyOption.AddOptionState(byState);
		}
		else
		{
			kPartyOption.RemoveOptionState(byState);
		}
		pkParty->Option(kPartyOption);
		if(byOldState != kPartyOption.GetOptionState())
		{
			if((EPR_CLIENT == byState))
			{
				SendOptionParty(pkParty);
			}
			else
			{
				SyncToLocalPartyMgr(pkParty);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Expedition 원정대

// 원정대 생성
HRESULT PgGlobalPartyMgrImpl::ReqCreateExpedition(SContentsUser const & rkMaster, BM::Stream& rkPacket)
{
	if( rkMaster.Empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	CONT_DEFMAP const *pkDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkDefMap);
	if( !pkDefMap )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	CONT_DEFMAP::const_iterator iter = pkDefMap->find(rkMaster.kGndKey.GroundNo());
	if( pkDefMap->end() == iter )
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find GroundDef User[") << rkMaster.Name() << _T("],GroundNo=") << rkMaster.kGndKey.GroundNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( 0 == (iter->second.iAttr & GATTR_EXPEDITION_LOBBY) )	//원정대 로비가 아니면 생성 하면 안됨
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Area"));
		return PRC_Fail_Area;
	}

	BM::GUID kPartyGuid;
	bool const bMasterParty = GetCharToParty(rkMaster.kCharGuid, kPartyGuid);
	if( !bMasterParty )
	{
		// 파티장이 아니면 원정대 만들기 실패
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
		return PRC_Fail_NotMaster;
	}

	{
		BM::GUID kExpeditionGuid;
		bool const bIsExpedition = GetCharToExpedition(rkMaster.kCharGuid, kExpeditionGuid);
		if( bIsExpedition )
		{
			// 이미 원정대에 속해 있으면 원정대 만들기 실패
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
			return PRC_Fail_Party;
		}
	}

	std::wstring		kNewExpeditionName;
	SExpeditionOption	kNewOption;
	BM::GUID NpcGuid;

	rkPacket.Pop(NpcGuid);
	kNewOption.ReadFromPacket(rkPacket);
	rkPacket.Pop(kNewExpeditionName);

	if( BM::GUID::NullData() == NpcGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( kNewExpeditionName.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( MAX_PARTYNAME_LEN < kNewExpeditionName.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Max"));
		return PRC_Fail_Max;
	}

	bool const bFiltered = g_kFilterString.Filter(kNewExpeditionName, false, FST_ALL);
	bool const bUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_PARTY_NAME, kNewExpeditionName));
	if( bFiltered
		||	bUniCodeFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_BadName"));
		return PRC_Fail_BadName;
	}

	BM::GUID kExpeditionGuid;
	PgGlobalExpedition* pkExpedition = NULL;
	HRESULT const hCreateRet = CreateExpedition(kNewExpeditionName, pkExpedition, kExpeditionGuid, kNewOption);
	if( PRC_Success != hCreateRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCreateRet);
		return hCreateRet;//생성 실패
	}

	if( NULL == pkExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( true == BM::GUID::IsNotNull(NpcGuid) )
	{
		pkExpedition->ExpeditionNpc(NpcGuid);
	}

	CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;
	g_kTblDataMgr.GetContDef(pContExpeditionNpc);
	if( NULL == pContExpeditionNpc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_EXPEDITION_NPC::const_iterator npc_iter = pContExpeditionNpc->find(NpcGuid);
	if( npc_iter == pContExpeditionNpc->end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CONT_DEF_EXPEDITION_NPC::mapped_type const & Element = npc_iter->second;

	pkExpedition->SetPartyHash( &m_kContPartyHash );
	pkExpedition->MaxMemberCount(Element.iMaxPlayerCnt);	// 최대 인원수 설정.
	pkExpedition->MaxTeamCount(Element.iMaxPlayerCnt / DEF_PV_MAX_MEMBER_CNT);  // 최대 팀수 설정.

	HRESULT hRet;
	SPartyOption kOption;
	BM::GUID kTempGuid;
	PgGlobalParty* pkParty = NULL;

	// 원정대장의 파티를 먼저 원정대에 넣는다.
	pkParty = GetParty(kPartyGuid);
	if( !pkExpedition->AddParty(pkParty) ) 
	{
		// 원정대에 파티를 넣지 못함.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		DeleteExpedition(kExpeditionGuid);
		return PRC_Fail;
	}

	AddPartyToExpedition(pkParty->PartyGuid(), pkExpedition->ExpeditionGuid());

	// 원정대장 캐릭터 GUID 설정.
	pkExpedition->MasterCharGuid(rkMaster.kCharGuid);

	// 파티에 가입되어있는 캐릭터들의 정보를 이용하여 원정대원 정보를 채움.
	SExpeditionUserInfo UserInfo;
	VEC_EPUSERINFO ExpeditionUserList;
	VEC_GUID kCharList;
	pkParty->GetMemberCharGuidList(kCharList);
	VEC_GUID::const_iterator it = kCharList.begin();
	while( it != kCharList.end() )
	{
		SPartyUserInfo* PartyUserInfo;
		pkParty->Get((*it), PartyUserInfo);
		
		PartyUserInfo->kChnGndKey.Channel(g_kProcessCfg.ChannelNo());
	
		pkExpedition->AddExpeditionMemberInfo(PartyUserInfo, UserInfo, EET_TEAM_1, true);

		AddCharToExpedition((*it), kExpeditionGuid);

		++it;
	}

	// 두 번째 파티부터 미리 성생해서 원정대에 넣어 두고 원정대원이 추가 될 때 빈 파티에 차례대로 넣는다.
	PgGlobalParty* pGlobalParty = NULL;
	for(int iMaxPartyCount = 0 ; iMaxPartyCount < pkExpedition->MaxTeamCount() - 1 ; ++iMaxPartyCount)
	{
		kTempGuid = BM::GUID::Create();
		kOption.Default();
		hRet = PgPartyMgr<PgGlobalParty>::NewParty(kTempGuid, pGlobalParty, kOption);
		if( hRet == PRC_Success )
		{
			if( !pkExpedition->AddParty(pGlobalParty) )
			{
				// 원정대에 파티를 넣지 못함.
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
				DeleteExpedition(kExpeditionGuid);
				return PRC_Fail;
			}
			pGlobalParty->Expedition(true);
			SyncToLocalPartyMgr(pGlobalParty);									// 로컬 파티매니저 Sync.
			AddPartyToExpedition(pGlobalParty->PartyGuid(), pkExpedition->ExpeditionGuid());
		}
	}
	
	BM::Stream ResultPacket(PT_N_C_ANS_CREATE_EXPEDITION);
	ResultPacket.Push((int)PRC_Success);
	pkExpedition->WriteToPacketMemberInfo(ResultPacket);

	g_kServerSetMgr.Locked_SendToUser(rkMaster.kCharGuid, ResultPacket, false);

	BM::Stream NfyPacket(PT_N_C_NFY_CREATE_EXPEDITION);
	pkExpedition->WriteToPacketMemberInfo(NfyPacket);
	NfyPacket.Push(pkExpedition->ExpeditionNpc());

	SendToPartyMember(pkParty, NfyPacket);

	SyncToLocalPartyMgr(pkExpedition);

	BM::Stream InfoPacket(PT_T_N_REQ_ADD_EXPEDITION_INFO);

	pkExpedition->WrtieToPacketListInfo(InfoPacket);

	SendToExpeditionListMgr(InfoPacket);

	//Log
	PgLogCont LogCont(ELogMain_Contents_Expedition, ELogSub_Expedition);
	LogCont.MemberKey(rkMaster.kMemGuid);
	LogCont.UID(rkMaster.iUID);
	LogCont.CharacterKey(rkMaster.kCharGuid);
	LogCont.ID(rkMaster.kAccountID);
	LogCont.Name(rkMaster.Name());
	LogCont.ChannelNo(static_cast<short>(rkMaster.sChannel));
	LogCont.Class(static_cast<short>(rkMaster.iClass));
	LogCont.Level(static_cast<short>(rkMaster.sLevel));
	LogCont.GroundNo(static_cast<short>(rkMaster.kGndKey.GroundNo()));

	PgLog Log(ELOrderMain_Expedition, ELOrderSub_Create);
	Log.Set(0, static_cast<std::wstring>(pkExpedition->ExpeditionName()));
	std::wstring strExpeditionInfo = PgLogUtil::GetExpeditionOptionPublic(pkExpedition->Option().GetOptionPublicTitle());
	strExpeditionInfo += _T(" ");
	strExpeditionInfo += PgLogUtil::GetExpeditionOptionItemString(pkExpedition->Option().GetOptionItem());
	Log.Set(1, strExpeditionInfo);
	Log.Set(0, static_cast<int>(pkExpedition->Option().GetOptionPublicTitle()));
	Log.Set(1, static_cast<int>(pkExpedition->Option().GetOptionItem()));
	Log.Set(2, static_cast<int>(pkExpedition->Option().GetOptionLevel()));
	Log.Set(3, static_cast<int>(rkMaster.cGender));
	Log.Set(2, pkExpedition->ExpeditionGuid().str());
	
	// 원정대에 속해있는 파티 guid 로그로 남김.
	int LogIdx = 3;
	VEC_GUID PartyList;
	pkExpedition->GetPartyList(PartyList);
	VEC_GUID::const_iterator party_iter = PartyList.begin();
	for( ; party_iter != PartyList.end() ; ++party_iter )
	{
		PgGlobalParty * pParty = GetParty( *party_iter );
		if( NULL != pParty )
		{
			Log.Set(LogIdx, pParty->PartyGuid().str());
			++LogIdx;
		}
	}

	LogCont.Add(Log);
	LogCont.Commit();

	return PRC_Success;
}

// 원정대 가입 요청.
HRESULT PgGlobalPartyMgrImpl::ReqJoinExpedition(SContentsUser const & rkMaster, SContentsUser const &rkUser, BM::GUID &rkExpeditionGuid, BM::Stream &rkPacket)
{
	HRESULT const hCheck = CheckJoinExpedition(rkMaster, rkUser);
	if( PRC_Success != hCheck )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hCheckRet );
		return hCheck;//실패
	}

	PgGlobalExpedition* pkExpedition = GetExpedition(rkExpeditionGuid);
	if( NULL == pkExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool IsMemeber = pkExpedition->IsMember(rkUser.kCharGuid);
	if( true == IsMemeber )
	{	// 이미 원정대 멤버.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MineMember") );
		return PRC_Fail_MineMember;
	}

	bool const IsInExpedition = GetCharToExpedition(rkUser.kCharGuid, BM::GUID::GUID());
	if( true == IsInExpedition )
	{	// 다른 원정대에 가입 대기 중.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Waiter") );
		return PRC_Fail_Waiter;
	}

	if( pkExpedition->MaxMemberCount() == pkExpedition->GetExpeditionMemberCount() )
	{
		// 풀방.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;
	}

	bool bIsMaster = pkExpedition->IsMaster(rkMaster.kCharGuid);
	if( !bIsMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;
	}

	HRESULT hAddWaitRet = pkExpedition->AddWaitMember(rkUser.kCharGuid);
	if( PRC_Success != hAddWaitRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hAddWaitRet);
		return hAddWaitRet;	// 실패.
	}

	BM::Stream kUserPacket(PT_N_C_REQ_JOIN_EXPEDITION);
	kUserPacket.Push(rkUser.Name());
	kUserPacket.Push(rkUser.kCharGuid);
	kUserPacket.Push(rkUser.sLevel);
	kUserPacket.Push(rkUser.iClass);
	kUserPacket.Push(rkPacket);
	g_kServerSetMgr.Locked_SendToUser(rkMaster.kMemGuid, kUserPacket);

	return PRC_Success;
}

// 원정대 초대/가입 요청에 대한 응답.
HRESULT PgGlobalPartyMgrImpl::AnsJoinExpedition(SContentsUser const & rkUser, BM::GUID & rkExpeditionGuid, bool bJoin)
{
	PgGlobalExpedition* pkExpedition = GetExpedition(rkExpeditionGuid);
	if( NULL == pkExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( false == bJoin )		// 원정대 초대/가입 요청 거절.
	{
		pkExpedition->DelWaitMember(rkUser.kCharGuid);

		return PRC_Fail_Refuse; // 거절일 경우 더 이상 진행 할 필요 없음.
	}

	BM::GUID PartyGuid, ExpeditionGuid;
	GetCharToParty(rkUser.kCharGuid, PartyGuid);
	GetCharToExpedition(rkUser.kCharGuid, ExpeditionGuid);

	// 파티 또는 원정대에 가입되어 있지 않은 경우만.. 초대..
	if( BM::GUID::IsNotNull(PartyGuid) || BM::GUID::IsNotNull(ExpeditionGuid) )
	{
		pkExpedition->DelWaitMember(rkUser.kCharGuid);
		DelCharToExpedition(rkUser.kCharGuid);

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
		return PRC_Fail_Party;
	}

	bool bIsMember = IsExpeditionPartyMember(rkExpeditionGuid, rkUser.kCharGuid);
	if( true == bIsMember )
	{
		pkExpedition->DelWaitMember(rkUser.kCharGuid);
		DelCharToExpedition(rkUser.kCharGuid);

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Party"));
		return PRC_Fail_Party;
	}

	if( pkExpedition->MaxMemberCount() == pkExpedition->GetExpeditionMemberCount() )
	{
		pkExpedition->DelWaitMember(rkUser.kCharGuid);
		DelCharToExpedition(rkUser.kCharGuid);

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;//인원수 제한으로 등록 불가
	}

	PgGlobalParty* pParty = NULL;
	int JoinTeamNo = 0;				// 들어간 팀 번호.
	HRESULT hAnsJoinRet = PRC_Success;
	hAnsJoinRet = pkExpedition->Add(rkUser, pParty, JoinTeamNo);
	if( PRC_Success == hAnsJoinRet )
	{	
		BM::Stream NfyPacket(PT_N_C_NFY_JOIN_EXPEDITION);
		SExpeditionUserInfo JoinUserInfo;
		pkExpedition->AddExpeditionMemberInfo(rkUser, JoinUserInfo, static_cast<EExpeditionTeam>(JoinTeamNo), true);
		JoinUserInfo.WriteToPacket(NfyPacket);
	
		AddCharToExpedition(rkUser.kCharGuid, rkExpeditionGuid);		// 캐릭터 guid 추가.

		SendToExpedition(pkExpedition, NfyPacket, rkUser.kCharGuid);		// 새로 가입한 원정대원 정보 보냄.

		BM::Stream ExNfyPacket(PT_N_C_ANS_JOIN_EXPEDITION);
		ExNfyPacket.Push((int)PRC_Success);
		pkExpedition->WriteToPacketMemberInfo(ExNfyPacket);
		ExNfyPacket.Push(pkExpedition->ExpeditionNpc());

		g_kServerSetMgr.Locked_SendToUser(rkUser.kCharGuid, ExNfyPacket, false);	// 새로 가입한 유저에게 원정대 리스트 정보 보냄.

		////////////////////////////////////////////////////
		PartyBuffMember(pParty, rkUser.kCharGuid, true);

		BM::Stream PartyNfyPacket(PT_N_C_NFY_JOIN_PARTY, pParty->PartyGuid());
		SPartyUserInfo UserInfo;
		pParty->GetMemberInfo(rkUser.kCharGuid, UserInfo);
		UserInfo.WriteToPacket(PartyNfyPacket);
		SendToPartyMember(pParty, PartyNfyPacket, pParty->PartyGuid());  // 원정대의 해당 파티원들에게 파티 새로운 멤버 가입을 알림.

		BM::Stream AnsUserPacket(PT_N_C_ANS_JOIN_PARTY, pParty->PartyGuid());
		AnsUserPacket.Push(hAnsJoinRet);
		pParty->WriteToPacket(AnsUserPacket);
		g_kServerSetMgr.Locked_SendToUser(rkUser.kCharGuid, AnsUserPacket, false); // 파티 가입 환영.

		AddCharToParty(rkUser.kCharGuid, pParty->PartyGuid());

		SyncToLocalPartyMgr(pParty);		// 로컬 파티 매니저 Sync.

		SyncToLocalPartyMgr(pkExpedition);

		// 원정대 정보 변경을 원정대 리스트 매니저로 알림.
		BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
		pkExpedition->WrtieToPacketListInfo(ModifyPacket);
		SendToExpeditionListMgr(ModifyPacket);

		LogExpeditionAction(pkExpedition, rkUser, ELOrderSub_Join);
	}

	return PRC_Success;
}

// 원정대 탈퇴
HRESULT PgGlobalPartyMgrImpl::ReqLeaveExpedition(BM::GUID const & rkCharGuid, BM::GUID const & ExpeditionGuid)
{
	HRESULT hRet;
	PgGlobalExpedition* pkExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pkExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( 1 == pkExpedition->GetExpeditionMemberCount() )
	{	// 탈퇴하려는데 한명이면 해산하고 끝내면 된다.
		HRESULT hResult = ReqDisperseExpedition(rkCharGuid);
		if( PRC_Success == hResult )
		{
			return PRC_Success_Destroy;
		}
	}

	SPartyUserInfo LeaveUserInfo;
	if( false == pkExpedition->GetMemberInfo(rkCharGuid, LeaveUserInfo) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( false == pkExpedition->DelPositionWaitList(rkCharGuid) )		// 원정대 탈퇴 요청자가 교체 대기 중이면 교체 대기에서 삭제.
	{
		hRet = LeaveParty(rkCharGuid, true);							// 교체 대기자가 아니면 속해 있는 파티에서 삭제.
		if( PRC_Success != hRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
	}

	SContentsUser LeaverInfo;
	if( S_OK == ::GetPlayerByGuid(rkCharGuid, false, LeaverInfo) )
	{
		LogExpeditionAction(pkExpedition, LeaverInfo, ELOrderSub_Break);
	}

	BM::GUID const CharExpeditionGuid = DelCharToExpedition(rkCharGuid);
	pkExpedition->DelExpeditionMemberInfo(rkCharGuid); // 원정대원 정보도 삭제함.

	if( true == pkExpedition->IsMaster(rkCharGuid) )
	{	// 탈퇴 신청자가 원정대장이면 원정대장 교체.		
		ReqChangeExpeditionMaster(ExpeditionGuid, rkCharGuid, BM::GUID::NullData());
	}

	BM::Stream NfyPacket(PT_N_C_NFY_LEAVE_EXPEDITION, rkCharGuid);
	SendToExpedition(pkExpedition, NfyPacket);		// 탈퇴 정보 알림.

	BM::Stream AnsPacket(PT_N_C_ANS_LEAVE_EXPEDITION);
	AnsPacket.Push(PRC_Success);
	g_kServerSetMgr.Locked_SendToUser(rkCharGuid, AnsPacket, false); // 탈퇴유저에게 결과알림.

	BM::Stream GndPacket(PT_N_M_NFY_LEAVE_EXPEDITION);
	GndPacket.Push(ExpeditionGuid);
	GndPacket.Push(rkCharGuid);
	SendToLocalPartyMgr(pkExpedition, GndPacket);
	SendToGround(LeaveUserInfo.kChnGndKey, GndPacket);

	SyncToLocalPartyMgr(pkExpedition); // 로컬 Sync.

	// 원정대 정보 변경을 원정대 리스트 매니저로 알림.
	BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
	pkExpedition->WrtieToPacketListInfo(ModifyPacket);
	SendToExpeditionListMgr(ModifyPacket);

	return PRC_Success;
}

// 원정대 초대.
HRESULT PgGlobalPartyMgrImpl::ReqInviteExpedition(SContentsUser const & Master, BM::GUID const & ExpeditionGuid, BM::Stream & Packet)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	VEC_GUID Guid_List;
	Packet.Pop(Guid_List);
	if( Guid_List.empty() )		// 캐릭터 GUID 리스트가 비었으면 처리 할게 없음.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( pExpedition->MaxMemberCount() < pExpedition->GetExpeditionMemberCount() + Guid_List.size() )
	{
		// 초대하려는 인원이 최대 인원보다 넘치면 안됨.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_MemberCount"));
		return PRC_Fail_MemberCount;
	}

	VEC_GUID Invite_List;	// 실제로 초대 요청을 할 리스트.
	// 초대 가능한지 검사.
	VEC_GUID::const_iterator guid_iter = Guid_List.begin();
	while( guid_iter != Guid_List.end() )
	{
		BM::GUID PartyGuid, ExpeditionGuid;
		GetCharToParty((*guid_iter), PartyGuid);
		GetCharToExpedition((*guid_iter), ExpeditionGuid);

		// 파티 또는 원정대에 가입되어 있지 않은 경우만.. 초대..
		if( BM::GUID::IsNull(PartyGuid) && BM::GUID::IsNull(ExpeditionGuid) )
		{
			Invite_List.push_back((*guid_iter));
		}

		++guid_iter;
	}


	BM::Stream SendPacket(PT_N_C_REQ_INVITE_EXPEDITION);		// 초대 패킷.
	SendPacket.Push(ExpeditionGuid);
	SendPacket.Push(pExpedition->ExpeditionName());
	SendPacket.Push(Master.kCharGuid);
	SendPacket.Push(Master.kName);
	SendPacket.Push(Master.sLevel);
	SendPacket.Push(Master.iClass);

	VEC_GUID::const_iterator iter = Invite_List.begin();
	while( iter != Invite_List.end() )
	{
		pExpedition->AddWaitMember(*iter);								// 원정대 가입대기로 등록
		g_kServerSetMgr.Locked_SendToUser(*iter, SendPacket, false);	// 초대하려는 유저들에게 초대메세지 보냄.
		++iter;
	}

	return PRC_Success;
}

// 원정대 팀원 팀 교체 요청.
HRESULT PgGlobalPartyMgrImpl::ReqTeamMove(BM::GUID const & MasterGuid, BM::GUID const & ExpeditionGuid, BM::Stream & Packet)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	BM::GUID CharGuid;	// 팀바꿀 캐릭터의 Guid.
	int iTeam;			// 옮길 팀 번호.

	Packet.Pop(CharGuid);
	Packet.Pop(iTeam);

	if( CharGuid == BM::GUID::NullData() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( EET_TEAM_0 == iTeam )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( false == pExpedition->IsMember(CharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMember"));
		return PRC_Fail_NotMember; // 멤버가 아님.
	}

	if( false == pExpedition->CheckMovable(iTeam) )
	{
		// 이동 못함. 이동할 팀(파티)에 자리가 없다.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMember"));
		return PRC_Fail_MemberCount;
	}

	SPartyUserInfo UserInfo;
	if( false == pExpedition->GetMemberInfo(CharGuid, UserInfo) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( true == pExpedition->IsPositionWait(CharGuid) )		// 교체 대기 중이면 교체 리스트에서 빼냄.
	{
		bool bDel = pExpedition->DelPositionWaitList(CharGuid);
		if( false == bDel )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail!"));
			return PRC_Fail;
		}
	}
	else
	{
		HRESULT hRet = LeaveParty(CharGuid, true);			// 아니면, 현재 속한 파티에서 탈퇴 시킴.
		if( hRet != PRC_Success )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return") << hRet );
			return hRet;	// 파티에서 빼지 못함.
		}
	}

	PgGlobalParty * pNewParty = NULL;

	if( iTeam < EET_TEAM_SHIFT ) // 1~4번만 원정대 팀. 5번은 교체 대기 리스트로 가야됨.
	{
		HRESULT hRet = pExpedition->AddToTeam(UserInfo, iTeam, pNewParty);	// 파티에 넣고.
		if( hRet != PRC_Success )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return") << hRet );
			return hRet;
		}
		AddCharToParty(CharGuid, pNewParty->PartyGuid());

		if( pNewParty->MemberCount() > 1 ) // 멤버가 1명보다 많음.
		{
			BM::Stream NfyPacket(PT_N_C_NFY_JOIN_PARTY, pNewParty->PartyGuid());
			UserInfo.WriteToPacket(NfyPacket);
			SendToPartyMember(pNewParty, NfyPacket, UserInfo.kCharGuid);//새로운 맴버 가입
		}

		///////////////////////////////////////////////////
		PartyBuffMember(pNewParty, CharGuid, true);

		BM::Stream AnsUserPacket(PT_N_C_ANS_JOIN_PARTY, pNewParty->PartyGuid());
		AnsUserPacket.Push(hRet);
		pNewParty->WriteToPacket(AnsUserPacket);
		g_kServerSetMgr.Locked_SendToUser(UserInfo.kCharGuid, AnsUserPacket, false);

		SyncToLocalPartyMgr(pNewParty);
	}
	else if( EET_TEAM_SHIFT == iTeam )
	{
		if( false == pExpedition->AddPositionWait(UserInfo) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail!"));
			return PRC_Fail;
		}
	}

	// 여기 까지 왔으면 팀 이동 성공. 원정대원들에게 통보.
	SExpeditionUserInfo ExpeditionMemberInfo(UserInfo, iTeam, true);
	BM::Stream NfyPacket(PT_N_C_NFY_TEAM_MOVE_EXPEDITION);
	ExpeditionMemberInfo.WriteToPacket(NfyPacket);

	SendToExpedition(pExpedition, NfyPacket);

	SyncToLocalPartyMgr(pExpedition);

	return PRC_Success;
}

HRESULT PgGlobalPartyMgrImpl::CreateExpedition(std::wstring const & rkExpeditionName, PgGlobalExpedition *&pkExpedition, BM::GUID & rkOutGuid, SExpeditionOption const & rkOption)
{
	HRESULT const hRet = PgExpeditionMgr< PgGlobalExpedition >::CreateExpedition(rkExpeditionName, pkExpedition, rkOutGuid, rkOption);

	return hRet;
}

bool PgGlobalPartyMgrImpl::DeleteExpedition(BM::GUID const & ExpeditionGuid)
{
	PgGlobalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bDelRet = PgExpeditionMgr< PgGlobalExpedition >::DeleteExpedition(ExpeditionGuid);

	return bDelRet;
}

bool PgGlobalPartyMgrImpl::GetCharToExpedition(BM::GUID const & rkCharGuid, BM::GUID & rkOutGuid) const
{
	ContCharToExpedition::const_iterator iter = m_kCharToExpedition.find(rkCharGuid);
	if( iter != m_kCharToExpedition.end() )
	{
		rkOutGuid = (*iter).second;
		return true;
	}

	return false;
}

bool PgGlobalPartyMgrImpl::AddCharToExpedition(BM::GUID const & rkCharGuid, BM::GUID const & rkExpeditionGuid)
{
	if( BM::GUID::IsNull(rkCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));

		return false;
	}

	if( BM::GUID::IsNull(rkExpeditionGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));

		return false;
	}

	auto kRet = m_kCharToExpedition.insert(std::make_pair(rkCharGuid, rkExpeditionGuid));
	return kRet.second;	
}

BM::GUID PgGlobalPartyMgrImpl::DelCharToExpedition(BM::GUID const & rkCharGuid)
{
	BM::GUID kExpeditionGuid;

	ContCharToExpedition::iterator iter = m_kCharToExpedition.find(rkCharGuid);
	if( m_kCharToExpedition.end() != iter )
	{
		kExpeditionGuid = iter->second;
		m_kCharToExpedition.erase(iter);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULLGUID"));
	}

	return kExpeditionGuid;
}

bool PgGlobalPartyMgrImpl::GetPartyToExpedition(BM::GUID const & rkPartyGuid, BM::GUID & rkOutGuid) const
{
	ContPartyToExpedition::const_iterator iter = m_kPartyToExpedition.find(rkPartyGuid);
	if( iter != m_kPartyToExpedition.end() )
	{
		rkOutGuid = (*iter).second;
		return true;
	}

	return false;
}

bool PgGlobalPartyMgrImpl::AddPartyToExpedition(BM::GUID const & rkPartyGuid, BM::GUID const & rkExpeditionGuid)
{
	if( BM::GUID::IsNull(rkPartyGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));

		return false;
	}

	if( BM::GUID::IsNull(rkExpeditionGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));

		return false;
	}

	auto kRet = m_kPartyToExpedition.insert(std::make_pair(rkPartyGuid, rkExpeditionGuid));
	return kRet.second;
}

BM::GUID PgGlobalPartyMgrImpl::DelPartyToExpedition(BM::GUID const & rkPartyGUid)
{
	BM::GUID ExpeditionGuid;

	ContPartyToExpedition::iterator iter = m_kPartyToExpedition.find(rkPartyGUid);
	if( m_kPartyToExpedition.end() != iter )
	{
		ExpeditionGuid = iter->second;
		m_kPartyToExpedition.erase(iter);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULLGUID"));
	}

	return ExpeditionGuid;
}

PgGlobalExpedition* PgGlobalPartyMgrImpl::GetPartyToExpedition(BM::GUID const & rkPartyGuid)
{
	BM::GUID ExpeditionGuid;
	if( true == GetPartyToExpedition(rkPartyGuid, ExpeditionGuid) )
	{
		return GetExpedition(ExpeditionGuid);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

// 원정대에 속해있는지 확인.
bool PgGlobalPartyMgrImpl::IsExpeditionPartyMember(BM::GUID const & ExpeditionGuid, BM::GUID const & rkCharGuid) const
{
	PgGlobalExpedition* pkExpedition = GetExpedition(ExpeditionGuid);
	if( pkExpedition )
	{
		return pkExpedition->IsMember(rkCharGuid);
	}

	return false;
}

// 원정대 해산.
HRESULT PgGlobalPartyMgrImpl::ReqDisperseExpedition(BM::GUID const & rkCharGuid)
{
	if( rkCharGuid.IsNull() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	BM::GUID ExpeditionGuid;
	if( false == GetCharToExpedition(rkCharGuid, ExpeditionGuid) )			// 해체를 요청하는 캐릭터가 속한 원정대의 GUID를 얻어 옴.
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	PgGlobalExpedition* pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( false == pExpedition->IsMaster(rkCharGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail_NotMaster;
	}

	pExpedition->State(ES_DISPERSE); // 해체 예정인 원정대.

	SPartyUserInfo MasterInfo;
	pExpedition->GetMemberInfo(pExpedition->MasterCharGuid(), MasterInfo);

	BM::Stream MapPacket(PT_N_M_NFY_DISPERSE_EXPEDITION);				// 원정대의 해산을 원정대장이 있는 맵으로 브로드캐스팅.
	MapPacket.Push(MasterInfo.kCharGuid);
	MapPacket.Push(ExpeditionGuid);
	SendToGround( MasterInfo.kChnGndKey, MapPacket );

	BM::Stream Packet(PT_N_M_NFY_DELETE_EXPEDITION, ExpeditionGuid);
	SendToLocalPartyMgr(pExpedition, Packet);

	BM::Stream SendPacket(PT_N_C_NFY_DISPERSE_EXPEDITION);		// 원정대 해산 패킷
	SendPacket.Push(pExpedition->ExpeditionGuid());
	
	typedef std::vector<PgGlobalParty*> ContParty;
	VEC_GUID PartyList;
	pExpedition->GetPartyList(PartyList);
	VEC_GUID::iterator party_iter = PartyList.begin();
	while( party_iter != PartyList.end() )
	{
		PgGlobalParty * pParty = GetParty( *party_iter );
		if( NULL != pParty )
		{
			VEC_GUID MemberList;
			pParty->GetMemberCharGuidList(MemberList);			// 파티 멤버 guid list.
			
			DelPartyToExpedition(pParty->PartyGuid());			// 파티 guid 제거.

			SPartyUserInfo UserInfo;
			VEC_GUID::const_iterator member_iter = MemberList.begin();
			while( member_iter != MemberList.end() )
			{
				DelCharToExpedition((*member_iter));					// 캐릭터 guid 제거.

				g_kServerSetMgr.Locked_SendToUser((*member_iter), SendPacket, false);	// 해산 패킷 전송.

				++member_iter;
			}

			DestroyParty(pParty);					// 원정대 파티 삭제.
		}
		++party_iter;
	}

	SContentsUser Master;
	if( S_OK == ::GetPlayerByGuid(MasterInfo.kCharGuid, false, Master) )
	{
		//Log
		PgLogCont LogCont(ELogMain_Contents_Expedition, ELogSub_Expedition);
		LogCont.MemberKey(Master.kMemGuid);
		LogCont.UID(Master.iUID);
		LogCont.CharacterKey(Master.kCharGuid);
		LogCont.ID(Master.kAccountID);
		LogCont.Name(Master.Name());
		LogCont.ChannelNo(static_cast<short>(Master.sChannel));
		LogCont.Class(static_cast<short>(Master.iClass));
		LogCont.Level(static_cast<short>(Master.sLevel));
		LogCont.GroundNo(static_cast<short>(Master.kGndKey.GroundNo()));

		PgLog Log(ELOrderMain_Expedition, ELOrderSub_Delete);
		Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
		std::wstring strExpeditionInfo = PgLogUtil::GetExpeditionOptionPublic(pExpedition->Option().GetOptionPublicTitle());
		strExpeditionInfo += _T(" ");
		strExpeditionInfo += PgLogUtil::GetExpeditionOptionItemString(pExpedition->Option().GetOptionItem());
		Log.Set(1, strExpeditionInfo);
		Log.Set(0, static_cast<int>(pExpedition->Option().GetOptionPublicTitle()));
		Log.Set(1, static_cast<int>(pExpedition->Option().GetOptionItem()));
		Log.Set(2, static_cast<int>(pExpedition->Option().GetOptionLevel()));
		Log.Set(3, static_cast<int>(Master.cGender));
		Log.Set(2, pExpedition->ExpeditionGuid().str());

		// 원정대에 속해있는 파티 guid 로그로 남김.
		int LogIdx = 3;
		VEC_GUID PartyList;
		pExpedition->GetPartyList(PartyList);
		VEC_GUID::const_iterator party_iter = PartyList.begin();
		for( ; party_iter != PartyList.end() ; ++party_iter )
		{
			PgGlobalParty * pParty = GetParty( *party_iter );
			if( NULL != pParty )
			{
				Log.Set(LogIdx, pParty->PartyGuid().str());
				++LogIdx;
			}
		}

		LogCont.Add(Log);
		LogCont.Commit();
	}

	VEC_CHANGEPOS_USER PositionWaitList;
	pExpedition->GetPositionWaitList(PositionWaitList);
	VEC_CHANGEPOS_USER::iterator pos_iter = PositionWaitList.begin();
	while( PositionWaitList.end() != pos_iter )
	{
		g_kServerSetMgr.Locked_SendToUser((*pos_iter).kCharGuid, SendPacket, false);	// 해산 패킷 전송.

		DelCharToExpedition((*pos_iter).kCharGuid);

		++pos_iter;
	}

	ContWaitJoinList JoinWaitList;
	pExpedition->GetJoinWaitList(JoinWaitList);
	ContWaitJoinList::iterator join_iter = JoinWaitList.begin();
	while( JoinWaitList.end() != join_iter )
	{
		BM::Stream FailPacket(PT_N_C_ANS_JOIN_EXPEDITION);	// 실패 메세지 전송.
		FailPacket.Push(PRC_Fail_Refuse);

		DelCharToExpedition((*join_iter).first);
		++join_iter;
	}

	bool bRet = pExpedition->DisperseExpedition();					// 원정대에 들어있는 캐릭터 정보들을 모두 제거.
	if( false == bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}
	
	//	원정대 리스트에서 해산되는 원정대 정보 제거.
	BM::Stream DelPacket(PT_T_N_REQ_DEL_EXPEDITION_INFO, pExpedition->ExpeditionGuid());
	SendToExpeditionListMgr(DelPacket);

	bool bDelRet = DeleteExpedition(pExpedition->ExpeditionGuid());		// 원정대 삭제.
	if( false == bDelRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	return PRC_Success;
}

HRESULT PgGlobalPartyMgrImpl::CheckJoinExpedition(SContentsUser const& rkMaster, SContentsUser const& rkUser)
{
	if( rkMaster.Empty() )//마스터 정보가 NULL
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( rkUser.Empty() )//상대편 정보가 NULL
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	{
		CONT_DEFMAP const *pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);
		if( !pkDefMap )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
		
		CONT_DEFMAP::const_iterator iter = pkDefMap->find(rkUser.kGndKey.GroundNo());
		if( pkDefMap->end() == iter )
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find GroundDef User[") << rkUser.Name() << _T("],GroundNo=") << rkUser.kGndKey.GroundNo());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		switch ( (*iter).second.iAttr )
		{
		case GATTR_PVP:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_PVP"));
				return PRC_Fail_PVP;
			}break;
		case GATTR_MISSION:
		case GATTR_CHAOS_MISSION:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Area"));
				return PRC_Fail_Area;
			}break;
		case GATTR_EMPORIA:
			{
			}break;
		default:
			{
				if( 0 != ((*iter).second.iAttr & GATTR_FLAG_SUPER) )
				{
					return PRC_Fail_Area;
				}
			}break;
		}

		if( 0 != ((*iter).second.iAttr & GKIND_INDUN ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Area"));
			return PRC_Fail_Area;
		}
		
		iter = pkDefMap->find(rkMaster.kGndKey.GroundNo());
		if( pkDefMap->end() == iter )
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot find GroundDef User[") << rkUser.Name() << _T("],GroundNo=") << rkUser.kGndKey.GroundNo());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}
	}

	if( rkMaster.kCharGuid == rkUser.kCharGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotInviteMe"));
		return PRC_Fail_NotInviteMe;//나를 초대 하지 마라
	}

	BM::GUID kUserPartyGuid;
	BM::GUID kUserExpeditionGuid;
	BM::GUID kMasterExpeditionGuid;
	bool const bUserFindParty = GetCharToParty(rkUser.kCharGuid, kUserPartyGuid);						// 이미 파티에 가입 되었는가?
	bool const bUserFindExpedition = GetCharToExpedition(rkUser.kCharGuid, kUserExpeditionGuid);		// 이미 원정대에 가입 되었는가?
	bool const bMasterFindExpedition = GetCharToExpedition(rkMaster.kCharGuid, kMasterExpeditionGuid);	// 마스터 원정대가 있나?

	PgGlobalExpedition* pExpedition = GetExpedition(kMasterExpeditionGuid);
	if( bMasterFindExpedition && pExpedition )
	{
		int nPartyLevelLimit = pExpedition->Option().GetOptionLevel();

		if( iPartyLimitNone == nPartyLevelLimit )		// 레벨 제한값이 iPartyLimitNone일 경우에는 원정대 가입 레벨 제한이 없음.
		{
			return PRC_Success;
		}

		int nPartyLevelMinus = rkMaster.sLevel - nPartyLevelLimit;	// 마스터 레벨 기준 - 원정대 레벨 제한
		int nPartyLevelPlus = rkMaster.sLevel + nPartyLevelLimit;	// 마스터 레벨 기준 + 원정대 레벨 제한
		if( (rkUser.sLevel > nPartyLevelPlus) || (rkUser.sLevel < nPartyLevelMinus) )	// 원정대에 가입하려는 유저의 레벨이 원정대 레벨 제한 사이에 있어야함.
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Level"));
			return PRC_Fail_Level;// 레벨이 맞지 않음(원정대)
		}

		// 원정대에 가입하려는 유저는 원정대도 없고 파티도 없어야 함.
		if( bUserFindParty && bUserFindExpedition )
		{
			return PRC_Fail_Party; // 이미 원정대 또는 파티에 가입되어 있음.
		}

		return PRC_Success;		
	}

	return PRC_Fail;
}

void PgGlobalPartyMgrImpl::SendToExpedition(PgGlobalExpedition const * pExpedition, BM::Stream const & SendPacket, BM::GUID const & rkIgnore)
{
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Expedition Pointer is NULL!"));
		return;
	}

	typedef std::vector<PgGlobalParty*> ContParty;
	VEC_GUID PartyList;
	pExpedition->GetPartyList(PartyList);
	VEC_GUID::iterator party_iter = PartyList.begin();
	while( party_iter != PartyList.end() )
	{
		PgGlobalParty * pParty = GetParty( *party_iter );
		if( NULL != pParty )
		{
			SendToPartyMember(pParty, SendPacket, rkIgnore);
		}
		++party_iter;
	}

	VEC_CHANGEPOS_USER PositionWaitList;
	pExpedition->GetPositionWaitList(PositionWaitList);
	VEC_CHANGEPOS_USER::iterator pos_iter = PositionWaitList.begin();
	while( PositionWaitList.end() != pos_iter )
	{
		if( (*pos_iter).kCharGuid != rkIgnore )
		{
			g_kServerSetMgr.Locked_SendToUser((*pos_iter).kCharGuid, SendPacket, false);
		}
		++pos_iter;
	}
}
// 원정대장 교체.
HRESULT PgGlobalPartyMgrImpl::ReqChangeExpeditionMaster(BM::GUID const & ExpeditionGuid, BM::GUID const & MasterGuid, BM::GUID const & NewMasterGuid)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);

	try 
	{
		if( NULL == pExpedition )
		{
			INFO_LOG(BM::LOG_LV1, __FL__ << _T("pExpedition Pointer is NULL!"));
			throw (HRESULT)PRC_Fail;
		}

		if( BM::GUID::NullData() == MasterGuid )
		{
			INFO_LOG(BM::LOG_LV1, __FL__ << _T("NewMasterGuid is NullData!"));
			throw (HRESULT)PRC_Fail;
		}

		bool bRet = pExpedition->ChangeMaster(NewMasterGuid);
		if( !bRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_ChangeMaster"));
			throw (HRESULT)PRC_Fail_ChangeMaster;
		}
	}

	catch(HRESULT eErrorCode )
	{
		BM::Stream ErrPacket(PT_N_C_ANS_CHANGEMASTER_EXPEDITION, eErrorCode);
		g_kServerSetMgr.Locked_SendToUser(MasterGuid, ErrPacket, false);
	}

	BM::Stream Packet(PT_N_C_NFY_CHANGEMASTER_EXPEDITION);
	Packet.Push(pExpedition->MasterCharGuid());
	SendToExpedition(pExpedition, Packet);

	BM::Stream LocalGndPacket(PT_N_M_NFY_CHANGEMASTER_EXPEDITION);
	LocalGndPacket.Push(pExpedition->ExpeditionGuid());
	LocalGndPacket.Push(pExpedition->MasterCharGuid());
	SendToLocalPartyMgr(pExpedition, LocalGndPacket);

	// 원정대 정보 변경을 원정대 리스트 매니저로 알림.
	BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
	pExpedition->WrtieToPacketListInfo(ModifyPacket);
	SendToExpeditionListMgr(ModifyPacket);

	return PRC_Success;
}

// 원정대 이름 변경.
HRESULT PgGlobalPartyMgrImpl::ReqExpeditoinRename(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, std::wstring & NewName)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	bool const bMaster = pExpedition->IsMaster(CharGuid);
	if( !bMaster )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;
	}

	if( NewName.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( MAX_PARTYNAME_LEN < NewName.size() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_Max"));
		return PRC_Fail_Max;
	}

	bool const bFiltered = g_kFilterString.Filter(NewName, false, FST_ALL);
	bool const bUniCodeFiltered = (false == g_kUnicodeFilter.IsCorrect(UFFC_PARTY_NAME, NewName));
	if( bFiltered
		||	bUniCodeFiltered )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_BadName"));
		return PRC_Fail_BadName;
	}

	pExpedition->ExpeditionName(NewName);

	BM::Stream LocalPacket(PT_N_M_NFY_RENAME_EXPEDITION);
	LocalPacket.Push(pExpedition->ExpeditionGuid());
	LocalPacket.Push(pExpedition->ExpeditionName());
	pExpedition->Option().WriteToPacket(LocalPacket);
	SendToLocalPartyMgr(pExpedition, LocalPacket);

	SContentsUser MasterInfo;
	if( S_OK == ::GetPlayerByGuid(CharGuid, false, MasterInfo) )
	{
		PgLogCont LogCont(ELogMain_Contents_Expedition, ELogSub_Expedition);
		LogCont.MemberKey(MasterInfo.kMemGuid);
		LogCont.UID(MasterInfo.iUID);
		LogCont.CharacterKey(MasterInfo.kCharGuid);
		LogCont.ID(MasterInfo.kAccountID);
		LogCont.Name(MasterInfo.Name());
		LogCont.ChannelNo(MasterInfo.sChannel);
		LogCont.Class(static_cast<short>(MasterInfo.iClass));
		LogCont.Level(static_cast<short>(MasterInfo.sLevel));
		LogCont.GroundNo(static_cast<int>(MasterInfo.kGndKey.GroundNo()));

		PgLog Log(ELOrderMain_Expedition, ELOrderSub_Modify);
		Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
		std::wstring strExpeditionInfo = PgLogUtil::GetExpeditionOptionPublic(pExpedition->Option().GetOptionPublicTitle());
		strExpeditionInfo += _T(" ");
		strExpeditionInfo += PgLogUtil::GetExpeditionOptionItemString(pExpedition->Option().GetOptionItem());
		Log.Set(1, strExpeditionInfo);
		Log.Set(0, static_cast<int>(pExpedition->Option().GetOptionPublicTitle()));
		Log.Set(1, static_cast<int>(pExpedition->Option().GetOptionItem()));
		Log.Set(2, static_cast<int>(pExpedition->Option().GetOptionLevel()));
		Log.Set(3, static_cast<int>(MasterInfo.cGender));
		Log.Set(2, pExpedition->ExpeditionGuid().str());

		// 원정대에 속해있는 파티 guid 로그로 남김.
		int LogIdx = 3;
		VEC_GUID PartyList;
		pExpedition->GetPartyList(PartyList);
		VEC_GUID::const_iterator party_iter = PartyList.begin();
		for( ; party_iter != PartyList.end() ; ++party_iter )
		{
			PgGlobalParty * pParty = GetParty( *party_iter );
			if( NULL != pParty )
			{
				Log.Set(LogIdx, pParty->PartyGuid().str());
				++LogIdx;
			}
		}
		
		LogCont.Add(Log);
		LogCont.Commit();

		return PRC_Success;
	}

	return PRC_Success;
}

// 원정대 옵션 변경.
HRESULT PgGlobalPartyMgrImpl::ReqChangeExpeditionOption(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, SExpeditionOption const & NewOption)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	try
	{
		bool const bMaster = pExpedition->IsMaster(CharGuid);
		if( !bMaster )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
			throw (HRESULT)PRC_Fail_NotMaster;
		}

		pExpedition->Option(NewOption);
	}

	catch( HRESULT eErrorCode )
	{
		BM::Stream ErrPacket(PT_N_C_ANS_CHANGEOPTION_EXPEDITION, eErrorCode);
		g_kServerSetMgr.Locked_SendToUser(CharGuid, ErrPacket, false);
		INFO_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! ReqExpeditoinRename isn't Success"));
		return eErrorCode;
	}

	BM::Stream NfyPacket(PT_N_C_NFY_CHANGEOPTION_EXPEDITION);
	pExpedition->Option().WriteToPacket(NfyPacket);
	SendToExpedition(pExpedition, NfyPacket);

	BM::Stream LocalPacket(PT_N_M_NFY_RENAME_EXPEDITION);
	LocalPacket.Push(pExpedition->ExpeditionGuid());
	LocalPacket.Push(pExpedition->ExpeditionName());
	pExpedition->Option().WriteToPacket(LocalPacket);
	SendToLocalPartyMgr(pExpedition, LocalPacket);

	// 원정대 정보 변경을 원정대 리스트 매니저로 알림.
	BM::Stream ModifyPacket(PT_T_N_NFY_MODIFY_EXPEDITION_INFO);
	pExpedition->WrtieToPacketListInfo(ModifyPacket);
	SendToExpeditionListMgr(ModifyPacket);

	SContentsUser MasterInfo;
	if( S_OK == ::GetPlayerByGuid(CharGuid, false, MasterInfo) )
	{
		PgLogCont LogCont(ELogMain_Contents_Expedition, ELogSub_Expedition);
		LogCont.MemberKey(MasterInfo.kMemGuid);
		LogCont.UID(MasterInfo.iUID);
		LogCont.CharacterKey(MasterInfo.kCharGuid);
		LogCont.ID(MasterInfo.kAccountID);
		LogCont.Name(MasterInfo.Name());
		LogCont.ChannelNo(MasterInfo.sChannel);
		LogCont.Class(static_cast<short>(MasterInfo.iClass));
		LogCont.Level(static_cast<short>(MasterInfo.sLevel));
		LogCont.GroundNo(static_cast<int>(MasterInfo.kGndKey.GroundNo()));

		PgLog Log(ELOrderMain_Expedition, ELOrderSub_Modify);
		Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
		std::wstring strExpeditionInfo = PgLogUtil::GetExpeditionOptionPublic(pExpedition->Option().GetOptionPublicTitle());
		strExpeditionInfo += _T(" ");
		strExpeditionInfo += PgLogUtil::GetExpeditionOptionItemString(pExpedition->Option().GetOptionItem());
		Log.Set(1, strExpeditionInfo);
		Log.Set(0, static_cast<int>(pExpedition->Option().GetOptionPublicTitle()));
		Log.Set(1, static_cast<int>(pExpedition->Option().GetOptionItem()));
		Log.Set(2, static_cast<int>(pExpedition->Option().GetOptionLevel()));
		Log.Set(3, static_cast<int>(MasterInfo.cGender));
		Log.Set(2, pExpedition->ExpeditionGuid().str());

		// 원정대에 속해있는 파티 guid 로그로 남김.
		int LogIdx = 3;
		VEC_GUID PartyList;
		pExpedition->GetPartyList(PartyList);
		VEC_GUID::const_iterator party_iter = PartyList.begin();
		for( ; party_iter != PartyList.end() ; ++party_iter )
		{
			PgGlobalParty * pParty = GetParty( *party_iter );
			if( NULL != pParty )
			{
				Log.Set(LogIdx, pParty->PartyGuid().str());
				++LogIdx;
			}
		}

		LogCont.Add(Log);
		LogCont.Commit();

		return PRC_Success;
	}

	return PRC_Success;
}

// 원정대 멤버 추방 요청.
HRESULT PgGlobalPartyMgrImpl::ReqKickExpeditionMember(BM::GUID const & ExpeditionGuid, BM::GUID const & MasterGuid, BM::GUID const & KickUserGuid)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( false == pExpedition->IsMaster(MasterGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMaster"));
		return PRC_Fail_NotMaster;	// 강퇴 요청한 유저가 원정대장이 아님.
	}

	if( false == pExpedition->IsMember(KickUserGuid) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail_NotMember"));
		return PRC_Fail_NotMember;	// 멤버가 아님.
	}

	SPartyUserInfo KickUserInfo;
	if( false == pExpedition->GetMemberInfo(KickUserGuid, KickUserInfo) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	SContentsUser KickUser;
	if( S_OK == ::GetPlayerByGuid(KickUserInfo.kCharGuid, false, KickUser) )
	{
		LogExpeditionAction(pExpedition, KickUser, ELOrderSub_Exile);
	}

	if( false == pExpedition->DelPositionWaitList(KickUserGuid) )				// 교체 대기자 일경우 바로 삭제.
	{
		PgGlobalParty* pParty = GetCharToParty(KickUserGuid);					// 파티에 속해있다면 해당 파티에서 제거.
		if( NULL == pParty )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
			return PRC_Fail;
		}

		DelCharToParty(KickUserGuid);
		/////////////////////////////////////////////
		PartyBuffMember(pParty, KickUserGuid, false);

		pParty->Del(KickUserGuid);

		BM::Stream MemberPacket(PT_N_C_NFY_KICKOUT_PARTY_USER);
		MemberPacket.Push(KickUserGuid);
		SendToPartyMember(pParty, MemberPacket);	// 남은 파티 멤버에게 보냄.

		BM::Stream MapPacket(PT_N_M_NFY_LEAVE_PARTY_USER);
		MapPacket.Push(pParty->PartyGuid());
		MapPacket.Push(KickUserGuid);
		MapPacket.Push(false);
		SendToLocalPartyMgr(pParty, MapPacket); // 로컬 파티로 보냄.
		SendToGround(KickUserInfo.kChnGndKey, MapPacket); // 강퇴 당한 유저 그라운드 로컬매니저에게 전송.
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BM::GUID const CharExpeditionGuid = DelCharToExpedition(KickUserGuid);
	pExpedition->DelExpeditionMemberInfo(KickUserGuid); // 원정대원 정보도 삭제함.

	BM::Stream NfyPacket(PT_N_C_NFY_KICKOUT_EXPEDITION, KickUserGuid);
	SendToExpedition(pExpedition, NfyPacket);		// 강퇴 정보 알림.

	BM::Stream AnsPacket(PT_N_C_ANS_KICKOUT_EXPEDITION);
	g_kServerSetMgr.Locked_SendToUser(KickUserGuid, AnsPacket, false); // 탈퇴유저에게 결과알림.

	BM::Stream GndPacket(PT_N_M_NFY_LEAVE_EXPEDITION);
	GndPacket.Push(ExpeditionGuid);
	GndPacket.Push(KickUserGuid);
	SendToLocalPartyMgr(pExpedition, GndPacket);
	SendToGround(KickUserInfo.kChnGndKey, GndPacket);

	SyncToLocalPartyMgr(pExpedition); // 로컬 Sync.

	return PRC_Success;
}

// 원정대 멤버의 현재 생존 여부 알림.
HRESULT PgGlobalPartyMgrImpl::Recv_ExpeditionMemberState(BM::GUID const & ExpeditionGuid, BM::GUID const & CharGuid, bool const & IsAlive)
{
	PgGlobalExpedition * pExpedition = GetExpedition(ExpeditionGuid);
	if( NULL == pExpedition )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	if( BM::GUID::NullData() == CharGuid )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
		return PRC_Fail;
	}

	SExpeditionUserInfo MemberInfo;
	bool FindMember = pExpedition->GetMemberInfo(CharGuid, MemberInfo);

	if( FindMember )
	{
		MemberInfo.bAlive = IsAlive;
		
		pExpedition->ModifyMemberState(MemberInfo);

		BM::Stream Packet(PT_N_C_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
		Packet.Push(CharGuid);
		Packet.Push(IsAlive);

		SendToExpedition(pExpedition, Packet);

		SyncToLocalPartyMgr(pExpedition);

		return PRC_Success;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return PRC_Fail"));
	return PRC_Fail;
}

typedef struct tagSFunctionClearExpeditionJoinWaitMember : public PgGlobalPartyMgrImpl::SFunctionExpeditionForEach
{
	tagSFunctionClearExpeditionJoinWaitMember(DWORD Time)
		: m_Time(Time)
	{
	}

	virtual void operator () (PgGlobalPartyMgrImpl::ExpeditionHash const & ContExpedition)
	{
		if( ContExpedition.empty() )
		{	// 원정대가 하나도 없다면 더 이상 진행 할 필요없음.
			return;
		}

		PgGlobalPartyMgrImpl::ExpeditionHash::const_iterator expedition_iter = ContExpedition.begin();
		for( ; expedition_iter != ContExpedition.end() ; ++expedition_iter )
		{
			PgGlobalPartyMgrImpl::ExpeditionHash::mapped_type const & Element = expedition_iter->second;
			if( Element )
			{
				Element->ClearExpeditionJoinWaitMember(m_Time);
			}
		}
	}

private:

	DWORD & m_Time;

}SFunctionClearExpeditionJoinWaitMember;

void PgGlobalPartyMgrImpl::ClearExpeditionJoinWaitMember(DWORD Time)
{
	ExpeditionForEach( SFunctionClearExpeditionJoinWaitMember(Time) );
}

bool PgGlobalPartyMgrImpl::LogExpeditionAction(PgGlobalExpedition * const pExpedition, SContentsUser const & UserInfo, EOrderSubType const Type) const
{
	PgLogCont LogCont(ELogMain_Contents_Expedition, ELogSub_Expedition);
	LogCont.MemberKey(UserInfo.kMemGuid);
	LogCont.UID(UserInfo.iUID);
	LogCont.CharacterKey(UserInfo.kCharGuid);
	LogCont.ID(UserInfo.kAccountID);
	LogCont.Name(UserInfo.kName);
	LogCont.ChannelNo(UserInfo.sChannel);
	LogCont.Class(static_cast<short>(UserInfo.iClass));
	LogCont.Level(static_cast<short>(UserInfo.sLevel));
	LogCont.GroundNo(static_cast<int>(UserInfo.kGndKey.GroundNo()));

	PgLog Log(ELOrderMain_Expedition, Type);
	switch(Type)
	{
	case ELOrderSub_Join:
		{
			std::wstring strExpeditionInfo = PgLogUtil::GetExpeditionOptionPublic(pExpedition->Option().GetOptionPublicTitle());
			strExpeditionInfo += _T(" ");
			strExpeditionInfo += PgLogUtil::GetExpeditionOptionItemString(pExpedition->Option().GetOptionItem());
			Log.Set(3, static_cast<int>(UserInfo.cGender));
			Log.Set(1, strExpeditionInfo);
			Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
			Log.Set(2, pExpedition->ExpeditionGuid().str());
		}break;
	case ELOrderSub_Break:
		{
			Log.Set(3, static_cast<int>(UserInfo.cGender));
			Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
			Log.Set(2, pExpedition->ExpeditionGuid().str());
		}break;
	case ELOrderSub_Exile:
		{
			Log.Set(3, static_cast<int>(UserInfo.cGender));
			Log.Set(0, static_cast<std::wstring>(pExpedition->ExpeditionName()));
			Log.Set(2, pExpedition->ExpeditionGuid().str());
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Unknown type = ") << static_cast<int>(Type));
		}break;
	}

	// 가입/탈퇴/추방한 파티 guid 기록.
	VEC_GUID PartyList;
	pExpedition->GetPartyList(PartyList);
	if( !PartyList.empty() )
	{
		VEC_GUID::const_iterator iter = PartyList.begin();
		for( ; iter != PartyList.end() ; ++iter)
		{
			PgGlobalParty * pParty = GetParty( *iter );
			if( NULL != pParty )
			{
				bool IsMember = pParty->IsMember(UserInfo.kCharGuid);
				if( IsMember )
				{
					Log.Set(3, pParty->PartyGuid().str());
					break;
				}
			}
		}
	}

	LogCont.Add(Log);
	LogCont.Commit();
	return true;
}

void PgGlobalPartyMgrImpl::CreateEventGroundParty(BM::GUID const & PartyGuid)
{	// 빈 파티를 미리 만들어 둔다.(파티 통채로 채널 이동)
	SPartyOption Option;

	Option.Default();
	PgGlobalParty* pGlobalParty = NULL;
	HRESULT hRet = PgPartyMgr<PgGlobalParty>::NewParty(PartyGuid, pGlobalParty, Option);
	if( hRet == PRC_Success )
	{
		pGlobalParty->AllowEmpty(true);
		SyncToLocalPartyMgr(pGlobalParty);
	}
}

void PgGlobalPartyMgrImpl::AddEventGroundParty(BM::GUID const & PartyGuid, SContentsUser const & NewPartyUserInfo, bool bMaster)
{
	PgGlobalParty * pParty = GetParty(PartyGuid);
	if( pParty )
	{
		SPartyUserInfo MemberInfo(NewPartyUserInfo);
		MemberInfo.kChnGndKey.Channel( g_kProcessCfg.ChannelNo() );
		HRESULT JoinRet = pParty->PgParty::Add(MemberInfo, bMaster);

		///////////////////////////////////
		PartyBuffMember(pParty, NewPartyUserInfo.kCharGuid, true);

		AddCharToParty(NewPartyUserInfo.kCharGuid, PartyGuid);

		BM::Stream NfyPacket(PT_N_C_NFY_JOIN_PARTY, PartyGuid);
		MemberInfo.WriteToPacket(NfyPacket);
		SendToPartyMember(pParty, NfyPacket, NewPartyUserInfo.kCharGuid);//새로운 맴버 가입

		BM::Stream AnsUserPacket(PT_N_C_ANS_JOIN_PARTY, PartyGuid);
		AnsUserPacket.Push(JoinRet);
		pParty->WriteToPacket(AnsUserPacket);
		g_kServerSetMgr.Locked_SendToUser(NewPartyUserInfo.kCharGuid, AnsUserPacket, false);//가입 환영


		SyncToLocalPartyMgr(pParty);//Gnd Local 동기화
	}
}

void PgGlobalPartyMgrImpl::DeleteOhterChannelJoinInfo_MasterLeaveParty(BM::GUID const& MasterGuid, PgGlobalParty * pParty)
{
	ContOtherChannelJoinInfo::iterator iter = m_ContOtherChannelJoinInfo.begin();
	while(iter != m_ContOtherChannelJoinInfo.end())
	{
		if( MasterGuid == iter->second.PartyMasterGuid )
		{
			BM::Stream Packet(PT_T_T_ANS_JOIN_OTHER_CHANNEL_PARTY_TO_MASTER);
			Packet.Push(iter->first);
			Packet.Push(PRC_Fail_Req_Join);
			SendToOtherChannelContents(iter->second.ChannelNo, PMET_PARTY, Packet);

			if( pParty )
			{
				pParty->DelWait(iter->first);
			}
			DelCharToParty(iter->first);
			iter = m_ContOtherChannelJoinInfo.erase(iter);
			continue;
		}
		++iter;
	}
}

void PgGlobalPartyMgrImpl::OnTick(DWORD Time)
{
	
}

//
//
PgGlobalPartyMgr::PgGlobalPartyMgr()
{
}
PgGlobalPartyMgr::~PgGlobalPartyMgr()
{
}
bool PgGlobalPartyMgr::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->ProcessMsg(pkMsg);
}
bool PgGlobalPartyMgr::GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid, bool const bIgnoreCharGuid)const
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_);
	return Instance()->GetPartyMemberList(rkOutCharGuidList, kGuid, bPartyGuid, bIgnoreCharGuid);
}

void PgGlobalPartyMgr::OnTick(DWORD Time)
{
	BM::CAutoMutex kLock(m_kMutex_Wrapper_, true);
	return Instance()->OnTick(Time);
}