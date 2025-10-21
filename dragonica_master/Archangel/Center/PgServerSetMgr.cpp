#include "StdAfx.h"
#include "Variant/Global.h"
#include "PgServerSetMgr.h"
#include "PgRecvFromItem.h"
#include "Variant/PgPlayer.h"
//#include "Variant/PgClassDefMgr.h"
//#include "PgGMProcessMgr.h"
#include "Lohengrin/PacketStruct4Map.h"
#include "Variant/PgMctrl.h"
#include "Variant/PgEventview.h"
#include "Variant/PgMission.h"
#include "Variant/gm_const.h"
#include "Lohengrin/packetstruct.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgControlDefMgr.h"
#include "Global.h"
#include "PgLChannelMgr.h"
#include "Variant/PgConstellation.h"

extern SERVER_IDENTITY g_kItemSI;

PgServerSetMgr::PgServerSetMgr(void)
:	m_bShutDownServer(false)
{
}

PgServerSetMgr::~PgServerSetMgr(void)
{
	Clear();
}

void PgServerSetMgr::Clear()
{
	CONT_SERVER_BALANCE::iterator bal_itr = m_kContServerBalance.begin();
	for ( ; bal_itr!=m_kContServerBalance.end() ; ++bal_itr )
	{
		SAFE_DELETE(*bal_itr);
	}
	m_kContServerBalance.clear();
}

bool PgServerSetMgr::Locked_SendToGround(BM::GUID const &kCharacterGuid, BM::Stream const &rkPacket, bool const bIsGndWrap)const
{
	BM::CAutoMutex kLock(m_kMutex);

	return SendToGround(kCharacterGuid, rkPacket, bIsGndWrap);
}

bool PgServerSetMgr::Locked_SendToGround(SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap)const
{
	BM::CAutoMutex kLock(m_kMutex);

	return SendToGround(kKey, rkPacket, bIsGndWrap);
}

bool PgServerSetMgr::SendToGround(BM::GUID const &kCharacterGuid, BM::Stream const &rkPacket, bool const bIsGndWrap)const
{
	SContentsUser kConUser;
	if(S_OK != GetPlayerInfo(kCharacterGuid, false, kConUser))
	{
		return false;
	}
	return SendToGround( kConUser.kGndKey, rkPacket, bIsGndWrap );
}

bool PgServerSetMgr::SendToGround(SGroundKey const &kKey, BM::Stream const &rkPacket, bool const bIsGndWrap)const
{
	SERVER_IDENTITY kSI;
	switch ( kKey.GroundNo() )
	{
	case PvP_Lobby_GroundNo_AnterRoom:
	case PvP_Lobby_GroundNo_Exercise:
	case PvP_Lobby_GroundNo_Ranking:
	case PvP_Lobby_GroundNo_League:
		{
			return SendToPvPLobby( rkPacket, kKey.GroundNo() );
		}break;
	default:
		{
			if( S_OK == GetServerIdentity(kKey, kSI) )
			{	
				if(bIsGndWrap)
				{
					BM::Stream kPacket(PT_A_GND_WRAPPED_PACKET, kKey );
					kPacket.Push(rkPacket);
					SendToServer(kSI, kPacket);
				}
				else
				{
					BM::Stream kPacket(PT_A_A_WRAPPED_PACKET, kKey );
					kPacket.Push(rkPacket);
					SendToServer(kSI, kPacket);
				}
				return true;
			}
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::SendToSwitch( BM::GUID const &kMemberGuid, BM::Stream const &rkPacket)const
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor = m_kContPlayer_MemberKey.find( kMemberGuid );
	if( m_kContPlayer_MemberKey.end() != itor )
	{
		return SendToServer( (*itor).second->GetSwitchServer(), rkPacket );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int Cmp_PlayerCount(void const *pOrg, void const *pTgt)
{//АЫАє°Е ё®ЕП.
	TBL_SERVERLIST const *pkOrgTbl = (TBL_SERVERLIST const *)pOrg;
	TBL_SERVERLIST const *pkTgtTbl = (TBL_SERVERLIST const *)pTgt;

	if(pkOrgTbl->nPlayerCount < pkTgtTbl->nPlayerCount)
	{
		return -1;
	}

	if(pkOrgTbl->nPlayerCount > pkTgtTbl->nPlayerCount)
	{
		return 1;
	}
	return 0;
}

void PgServerSetMgr::Recv_PT_S_T_ANS_RESERVE_MEMBER(BM::Stream * const pkPacket)const
{
	ESwitchReservePlayerResult eRet = SRPR_NONE;
	BM::GUID kSwitchKeyGuid;
	BM::GUID kMemberGuid;
	CEL::ADDR_INFO kSwitchAddr;

	pkPacket->Pop(eRet);
	pkPacket->Pop(kMemberGuid);
	pkPacket->Pop(kSwitchKeyGuid);
	pkPacket->Pop(kSwitchAddr);
	switch(eRet)
	{
	case SRPR_SUCEESS:
		{//Success
			SSwitchPlayerData kSPD;
			kSPD.ReadFromPacket(*pkPacket);

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("<08> User Assign Success From Switch ID[") << kSPD.ID() << _T("], MemberKey[") << kSPD.guidMember.str().c_str() << _T("]") );

			SSwitchReserveResult kSRR = kSPD;
			kSRR.eRet = SRPR_SUCEESS;
			kSRR.addrSwitch	= kSwitchAddr;
			kSRR.guidSwitchKey = kSPD.guidOrderKey;

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("<09> Send to LoginServer, the Switch Information") );
			
			BM::Stream kPacket(PT_T_IM_NFY_RESERVED_SWITCH_INFO);
			kSRR.WriteToPacket(kPacket);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kPacket);
		}break;
	default:
		{	//ЅЗЖР °б°ъ АьјЫ
			CAUTION_LOG( BM::LOG_LV4, __FL__ << _T("<08-2> User Assign Fail From Switch") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
	//АМ·±АПАМ???
	//		SSwitchReserveResult;
	//		kSRR.eRet = eRet;
	//		BM::Stream kPacket(PT_T_IM_NFY_RESERVED_SWITCH_INFO, kSRR);
	//		SendToImmigration(kPacket);
		}break;
	}
	return;
}

void PgServerSetMgr::ProcessRemoveUser_Common(CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData, bool bNotify)
{	
	INFO_LOG( BM::LOG_LV7, __FL__ << _T("MemberGuid[") << pCPData->GetMemberGUID().str().c_str() << _T("], Name[")
		<< pCPData->Name().c_str() << _T("], bNotify[") << bNotify << _T("]")
		);
	if (bNotify)
	{
		SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
		kSI.nServerNo = 0; kSI.nServerType = 0;
		BM::Stream kNfyPacket(PT_A_NFY_USER_DISCONNECT);
		kNfyPacket.Push(pCPData->GetMemberGUID());
		kNfyPacket.Push(pCPData->GetID());
		kSI.WriteToPacket(kNfyPacket);
		SendToContents(kNfyPacket);
	}

	ProcessRemoveUser_NotifyOtherMgr( pCPData, true );
}

void PgServerSetMgr::ProcessRemoveUser_NotifyOtherMgr( CONT_CENTER_PLAYER_BY_KEY::mapped_type const &pCPData, bool const bParty )const
{
	//ёрµз Contents Е¬·ЎЅєїЎ АЇАъ°Ў іЄ°ЁА» ѕЛё®АЪ

	// PvP Lobby
	if ( g_kProcessCfg.IsPublicChannel() )
	{
		BM::Stream kNfyPacket(PT_A_NFY_USER_DISCONNECT);
		kNfyPacket.Push(pCPData->GetMemberGUID());
		kNfyPacket.Push(pCPData->GetID());
		SendToPvPLobby( kNfyPacket, 0 );
	}

	// Party Mgr
	BM::Stream kWPacket(PT_A_NFY_USER_DISCONNECT);
	SContentsUser kUserInfo;
	pCPData->CopyTo(kUserInfo);
	kUserInfo.kGndKey.Clear(); // ·О±ЧѕЖїфАМґП±о ЗцАз А§ДЎґВ Е¬ё®ѕо.
	kUserInfo.WriteToPacket(kWPacket);

	if ( true == bParty )
	{
		::SendToGlobalPartyMgr(kWPacket);
		kWPacket.PosAdjust();
	}
	else
	{
		if ( g_kProcessCfg.IsPublicChannel() )
		{
			BM::Stream kPartyPacket( PT_T_T_SYNC_USER_PARTY2, pCPData->GetID() );
			::SendToGlobalPartyMgr( kPartyPacket );
		}
	}
}

bool PgServerSetMgr::Locked_ProcessRemoveUser( BM::GUID const &kMemberGuid, bool bNotifyContents, bool bNotifyMap )
{
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.find(kMemberGuid);
	if( m_kContPlayer_MemberKey.end() != user_itor)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData ґВ pool µҐАМЕНАМ№З·О user_itor »иБ¦ЗШµµ µК.

		if ( bNotifyMap )
		{
			BM::Stream kPacket( PT_A_NFY_USER_DISCONNECT, kMemberGuid );
			kPacket.Push( pCPData->GetID() );
			::SendToServerType(CEL::ST_MAP, kPacket);
		}

		ProcessRemoveUser_Common( pCPData, bNotifyContents );
		RemovePlayer(pCPData);
		g_kTotalObjMgr.ReleaseUnit( pCPData );
		return true;
	}

	if (bNotifyContents)
	{
		// Public Channel ·О єЇ°жЗПґВ БЯАП јцµµ АЦґЩ.
		// Contents їЎґВ АПґЬ Елєёё¦ ЗПАЪ
		SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
		kSI.nServerNo = 0; kSI.nServerType = 0;
		BM::Stream kNfyPacket(PT_A_NFY_USER_DISCONNECT);
		kNfyPacket.Push(kMemberGuid);
		kNfyPacket.Push(BM::GUID::NullData());
		kSI.WriteToPacket(kNfyPacket);
		SendToContents(kNfyPacket);
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_ProcessRemoveUser(SERVER_IDENTITY const &kSI)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	switch(kSI.nServerType)
	{
	case CEL::ST_LOG:
	case CEL::ST_MAP:
	case CEL::ST_ITEM:
	case CEL::ST_MACHINE_CONTROL:
	case CEL::ST_CENTER:
		{
		}break;
	case CEL::ST_SWITCH:
		{
			ProcessRemoveUser_BySwitch(kSI);
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Unknown SeverType[") << C2L(kSI) << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType"));
		}break;
	}
	return true;
}

size_t PgServerSetMgr::ProcessRemoveUser_ByGround(SGroundKey const &kGndKey)
{
	size_t iRet = 0;
	CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
	while( m_kContPlayer_MemberKey.end() != user_itor )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData ґВ pool µҐАМЕНАМ№З·О user_itor »иБ¦ЗШµµ µК.

		SGroundKey const &kNowGndKey = pCPData->GroundKey();
		std::wstring const &wstrID = pCPData->Name();

		if(kNowGndKey == kGndKey)
		{
			// ї©±вј­ БчБў Бцїмёй ·ОБчАМ ё№АМ ІїАМ°н,
			// є№АвЗШБшґЩ. ЅєА§ДЎЗСЕЧ ІчАё¶у°н ЕлєёЗШµµ АЯ Гіё®µЗґП±о ЅєА§ДЎєОЕН ЅГАЫЗПµµ·П ЗПАЪ.
//			ProcessRemoveUser_Common(pCPData);
//			RemovePlayer(pCPData);
//			g_kTotalObjMgr.ReleaseUnit( pCPData );			
//			user_itor = m_kContPlayer_MemberKey.begin();			
//			continue;

			BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_CenterRemoveUserByGnd) );
			kDPacket.Push( user_itor->first );
			SendToServer( pCPData->GetSwitchServer(), kDPacket );
			++iRet;
		}
		++user_itor;
	}
	return iRet;
}

void PgServerSetMgr::ProcessRemoveUser_BySwitch(SERVER_IDENTITY const &kSI)
{
	CONT_CENTER_PLAYER_BY_KEY::iterator user_itor = m_kContPlayer_MemberKey.begin();
	while( m_kContPlayer_MemberKey.end() != user_itor)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCPData = (*user_itor).second;//pData ґВ pool µҐАМЕНАМ№З·О user_itor »иБ¦ЗШµµ µК.

		SERVER_IDENTITY const &kSwitchServer = pCPData->GetSwitchServer();
//		std::wstring const &wstrID = pCPData->szID;

		if(kSI == kSwitchServer)
		{
			ProcessRemoveUser_Common(pCPData);
			RemovePlayer(pCPData);
			g_kTotalObjMgr.ReleaseUnit( pCPData );

			user_itor = m_kContPlayer_MemberKey.begin();//RemovePlayer і»єОїЎј­ АМЕН·№АМЕН ±ъБь.
			continue;
		}
		++user_itor;
	}
	return;
}

std::wstring PgServerSetMgr::LoginedPlayerName(BM::GUID const &rkMemberGuid, BM::GUID const &rkCharacterGuid)
{
	CONT_CENTER_PLAYER_BY_KEY::iterator itor = m_kContPlayer_MemberKey.find(rkMemberGuid);
	if( m_kContPlayer_MemberKey.end() != itor)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*itor).second;
		if(!pData)
		{ 
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pData is NULL"));
			return std::wstring();
			//return NULL;
		}
	
		PgPlayer *pkPlayer = pData;	
		if(pkPlayer == NULL || pkPlayer->GetID() != rkCharacterGuid)
		{ 
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return Null String!"));
			return std::wstring();
			//return NULL;
		}
		//return wstrName = rElement.szName;
		return pkPlayer->Name();
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
	return std::wstring(); 
	//return NULL;
}

bool PgServerSetMgr::UpdatePlayerData( UNIT_PTR_ARRAY& rkUnitArray, SPortalWaiter *pkPortalWaiter, bool const bClearGroundKey )const
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator login_itr;
	UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
	while ( unit_itr!=rkUnitArray.end() )
	{
		switch (unit_itr->pkUnit->UnitType())
		{
		case UT_MYHOME:
			{
			}break;
		case UT_PLAYER:
			{
				PgPlayer *pkMapUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);

				login_itr = m_kContPlayer_MemberKey.find( pkMapUser->GetMemberGUID() );
				if ( login_itr != m_kContPlayer_MemberKey.end() && (login_itr->second != NULL) )
				{
					PgPlayer *pkPlayer = login_itr->second;
					pkMapUser->CopyTo( *pkPlayer, rkUnitArray.GetWriteType() );
					
					if ( pkPortalWaiter )
					{
						pkPortalWaiter->Add( pkMapUser->GetID(), pkMapUser->VolatileID() );
						if ( pkPortalWaiter->iOwnerLv < 1)
						{
							pkPortalWaiter->iOwnerLv = pkMapUser->GetAbil(AT_LEVEL);
						}
					}

					if ( true == bClearGroundKey )
					{
						// GroundKeyё¦ БцїоґЩ.
						pkPlayer->GroundKey( SGroundKey() );
					}
				}
				else
				{
					unit_itr = rkUnitArray.erase(unit_itr);
					continue;
				}
			}break;
		}
		++unit_itr;
	}

	return !(rkUnitArray.empty());
}

bool PgServerSetMgr::Locked_TargetMapMoveByGuid( bool const bGMCommand, BM::GUID const &kCharGuid, BM::GUID const &kTargetGuid, bool const bMemberGuid )const
{
	BM::CAutoMutex kLock( m_kMutex );
	return ReqTargetMapMoveByGuid( bGMCommand, kCharGuid, kTargetGuid, bMemberGuid );
}

bool PgServerSetMgr::ReqTargetMapMoveByGuid( bool const bGMCommand, BM::GUID const &kCharGuid, BM::GUID const &kTargetGuid, bool const bMemberGuid )const
{
	CONT_CENTER_PLAYER_BY_KEY::mapped_type pkTargetPlayer = NULL;

	if ( bMemberGuid )
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_MemberKey.find( kTargetGuid );
		if ( itr != m_kContPlayer_MemberKey.end() )
		{
			pkTargetPlayer = itr->second;
		}
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_CharKey.find( kTargetGuid );
		if ( itr != m_kContPlayer_CharKey.end() )
		{
			pkTargetPlayer = itr->second;
		}
	}
	
	if ( !pkTargetPlayer )
	{
		return false;
	}

	int const iGroundNo = pkTargetPlayer->GroundKey().GroundNo();
	if ( PvP_Lobby_GroundNo_Max < iGroundNo )
	{
		BM::Stream kReqPacket( PT_T_M_REQ_MAP_MOVE_TARGET, pkTargetPlayer->GetID() );
		kReqPacket.Push( kCharGuid );
		kReqPacket.Push( bGMCommand );
		return SendToGround( pkTargetPlayer->GroundKey(), kReqPacket, true );
	}
	else
	{
		T_GNDATTR const kGndAttr = (iGroundNo ? GATTR_PVP : GATTR_DEFAULT);
		BM::Stream kFailedPacket( PT_T_C_ANS_MAP_MOVE_TARGET_FAILED, bGMCommand );
		kFailedPacket.Push( kGndAttr );
		kFailedPacket.Push( true );
		kFailedPacket.Push( pkTargetPlayer->Name() );
		return this->SendToUser( kCharGuid, kFailedPacket, false );
	}
}

bool PgServerSetMgr::Locked_TargetMapMoveByName( bool const bGMCommand, BM::GUID const &kCharGuid, std::wstring const &wstrTargetName, bool const bID )const
{
	BM::CAutoMutex kLock( m_kMutex );
	return ReqTargetMapMoveByName( bGMCommand, kCharGuid, wstrTargetName, bID );
}

bool PgServerSetMgr::ReqTargetMapMoveByName( bool const bGMCommand, BM::GUID const &kCharGuid, std::wstring const &wstrTargetName, bool const bID )const
{
	CONT_CENTER_PLAYER_BY_KEY::mapped_type pkTargetPlayer = NULL;

	if ( bID )
	{
		CONT_CENTER_PLAYER_BY_ID::const_iterator itr = m_kContPlayer_MemberID.find( wstrTargetName );
		if ( itr != m_kContPlayer_MemberID.end() )
		{
			pkTargetPlayer = itr->second;
		}
	}
	else
	{
		CONT_CENTER_PLAYER_BY_ID::const_iterator itr = m_kContPlayer_CharName.find( wstrTargetName );
		if ( itr != m_kContPlayer_CharName.end() )
		{
			pkTargetPlayer = itr->second;
		}
	}

	if ( !pkTargetPlayer )
	{
		return false;
	}

	int const iGroundNo = pkTargetPlayer->GroundKey().GroundNo();
	if ( PvP_Lobby_GroundNo_Max < iGroundNo )
	{
		BM::Stream kReqPacket( PT_T_M_REQ_MAP_MOVE_TARGET, pkTargetPlayer->GetID() );
		kReqPacket.Push( kCharGuid );
		kReqPacket.Push( bGMCommand );
		return SendToGround( pkTargetPlayer->GroundKey(), kReqPacket, true );
	}
	else
	{
		T_GNDATTR const kGndAttr = (iGroundNo ? GATTR_PVP : GATTR_DEFAULT);
		BM::Stream kFailedPacket( PT_T_C_ANS_MAP_MOVE_TARGET_FAILED, bGMCommand );
		kFailedPacket.Push( kGndAttr );
		kFailedPacket.Push( true );
		kFailedPacket.Push( pkTargetPlayer->Name() );
		return this->SendToUser( kCharGuid, kFailedPacket, false );
	}
}

bool PgServerSetMgr::ReqMapMove( UNIT_PTR_ARRAY& rkUnitArray, SReqMapMove_MT &rkRMM, Constellation::SConstellationMission &constellationMission, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER &kContModifyOrder )
{
	if ( rkUnitArray.empty() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("UnitArray is Empty") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		// MapMove Lock »уЕВ °Л»з
		UNIT_PTR_ARRAY::const_iterator unit_itor = rkUnitArray.begin();
		while (unit_itor != rkUnitArray.end())
		{
			BM::GUID kLockKey;
			if (GetMapMoveLock(unit_itor->pkUnit->GetID(), kLockKey))
			{
				if (kLockKey != rkRMM.kMapMoveKey)
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot MapMove, because already MapMoveLocked Player[") << unit_itor->pkUnit->GetID().str().c_str()
						<< _T("], Name[") << unit_itor->pkUnit->Name().c_str() << _T("]")
						);
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("\t\tReqKey[") << rkRMM.kMapMoveKey.str().c_str() << _T("], SavedMapMoveKey[") << kLockKey.str().c_str() << _T("]")
						);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			++unit_itor;
		}
	}

	BYTE byMapMoveType = MMT_NONE;
	switch ( rkRMM.cType )
	{
	case MMET_BackToChannel:
		{
			if ( g_kProcessCfg.IsPublicChannel() )
			{
				byMapMoveType = MMT_PUBLIC_PARTY;
				break;
			}
		} // breakА» »зїлЗПБц ѕКґВґЩ.
	case MMET_Login:
	case MMET_PublicChannelFailed:
		{
			if ( SUCCEEDED( GetServerIdentity( rkRMM.kTargetKey, rkRMM.kTargetSI ) ) )
			{
				byMapMoveType = MMT_FIRST;
			}
		}break;
	case MMET_Mission:
		{
			rkRMM.cType = MMET_Normal;// *****ѕИЗПёй №«ЗСCall
			SPortalWaiter kPortalWaiter( rkRMM, PORTAL_MISSION );
			kPortalWaiter.constellationMission = constellationMission;
			kPortalWaiter.iOwnerLv = rkRMM.iOwnerLv;
			if ( UpdatePlayerData( rkUnitArray, &kPortalWaiter, true ) )
			{
				kPortalWaiter.m_kContPetMoveData.swap( kContPetMapMoveData );
				kPortalWaiter.m_kContUnitSummonedMoveData.swap( kContUnitSummonedMapMoveData );
				kPortalWaiter.m_kModifyOrder.swap( kContModifyOrder );

				BM::Stream kMissionPortalPacket(PT_A_T_REQ_INDUN_CREATE);
				kPortalWaiter.WriteToPacket( kMissionPortalPacket );
				return SendToPortalMgr( kMissionPortalPacket );
				// return !!!
			}
		}break;
	case MMET_SuperGround:
		{
			rkRMM.cType = MMET_Normal;// *****ѕИЗПёй №«ЗСCall
			SPortalWaiter kPortalWaiter( rkRMM, PORTAL_SUPERGND );
			kPortalWaiter.constellationMission = constellationMission;
			kPortalWaiter.iOwnerLv = rkRMM.iOwnerLv;
			if ( UpdatePlayerData( rkUnitArray, &kPortalWaiter, true ) )
			{
				kPortalWaiter.m_kContPetMoveData.swap( kContPetMapMoveData );
				kPortalWaiter.m_kContUnitSummonedMoveData.swap( kContUnitSummonedMapMoveData );
				kPortalWaiter.m_kModifyOrder.swap( kContModifyOrder );

				BM::Stream kPortalPacket(PT_A_T_REQ_INDUN_CREATE);
				kPortalWaiter.WriteToPacket( kPortalPacket );
				return SendToPortalMgr( kPortalPacket );
				// return !!!
			}
		}break;
	case MMET_GoToPublic_PartyOrder:
		{
			if ( UpdatePlayerData( rkUnitArray, NULL, true  ) )
			{
				SPortalWaiter::CONT_WAIT_LIST kWaiterList;
					
				UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
				for ( ; unit_itr!=rkUnitArray.end() ; ++unit_itr )
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
					if ( pkPlayer )
					{
						kWaiterList.insert( std::make_pair(pkPlayer->GetID(), pkPlayer->VolatileID()) );
					}
				}

				if ( kWaiterList.size() )
				{
					BM::Stream kPartyMgrPacket( PT_T_T_REQ_READY_JOIN_HARDCORE );
					PU::TWriteTable_AA( kPartyMgrPacket, kWaiterList );
					PU::TWriteTable_AM( kPartyMgrPacket, kContPetMapMoveData );
					PU::TWriteTable_AM( kPartyMgrPacket, kContUnitSummonedMapMoveData );
					kContModifyOrder.WriteToPacket( kPartyMgrPacket );
					
					::SendToGlobalPartyMgr( kPartyMgrPacket );
				}

				return true;
				// return !!!
			}
		}break;
	case MMET_PvP:
		{
			if ( g_kProcessCfg.IsPublicChannel() )
			{// °шїлГ¤іОїЎј­ї©±в °Йё®ёй SelectCharacter їЎј­ ИЈГвЗС°ЕґЩ.
				BM::Stream kContents( PT_T_T_REQ_JOIN_LOBBY );
				rkUnitArray.WriteToPacket( kContents, WT_DEFAULT );

				if ( !SendToPvPLobby( kContents, rkRMM.kTargetKey.GroundNo() ) )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error") );
				}
				return true;
				// return !!!
			}
			else
			{
				byMapMoveType = MMT_PUBLIC_PERSONAL;
			}
		}break; 
	case MMET_BackToPvP:
	case MMET_KickToPvP:
		{
			if ( g_kProcessCfg.IsPublicChannel() )
			{
				byMapMoveType = MMT_DEFAULT;
				// return !!!
			}
			else
			{// АП№ЭГ¤іОїЎј­ґВ їАёй ѕИµИґЩ.
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!") );
			}
		}break;
	case MMET_GoToPublicGround:
	case MMET_GoTopublicGroundParty:
	case MMET_BATTLESQUARE:
		{
			if ( !g_kProcessCfg.IsPublicChannel() )
			{
				byMapMoveType = ( (MMET_GoTopublicGroundParty == rkRMM.cType) ? MMT_PUBLIC_PARTY : MMT_PUBLIC_PERSONAL);
				break;//break
			}

			// ЗцАз°Ў °шїлГ¤іОАМёй Г¤іО АМµїАє ѕЖґПґЩ.
			rkRMM.cType = MMET_Normal;	
		} // breakА» »зїлЗПБц ѕКґВґЩ.(АМ°ЕґЩАЅїЎґВ default:)
	default:
		{
			if ( SUCCEEDED( GetServerIdentity( rkRMM.kTargetKey, rkRMM.kTargetSI ) ) )
			{
				byMapMoveType = MMT_DEFAULT;
			}
		}break;
	}

	switch ( byMapMoveType )
	{
	case MMT_DEFAULT:
		{
			BM::Stream kMovePacket(PT_T_N_REQ_MAP_MOVE, rkRMM);
			constellationMission.WriteToPacket(kMovePacket);
			kMovePacket.Push(g_kProcessCfg.ChannelNo());
			kMovePacket.Push( rkUnitArray.size() );

			// MapMove Lock °Й±в
			UNIT_PTR_ARRAY::const_iterator unit_itr = rkUnitArray.begin();
			for ( ; unit_itr != rkUnitArray.end() ; ++unit_itr )
			{
				InsertMapMoveLock(rkRMM.kMapMoveKey, unit_itr->pkUnit->GetID());
				unit_itr->pkUnit->WriteToPacket( kMovePacket, WT_MAPMOVE_FIRST );
			}

			PU::TWriteTable_AM( kMovePacket, kContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, kContUnitSummonedMapMoveData );
			kContModifyOrder.WriteToPacket( kMovePacket );

			SendToContents(kMovePacket);
		}break;
	case MMT_FIRST:
		{
			// ·О±ЧАОАМёй Contents·О ЗС№ш ґх єёі»БЦёй і¶єсґЩ.
			BM::Stream kMovePacket(PT_T_M_REQ_MAP_MOVE, rkRMM );
			rkUnitArray.WriteToPacket( kMovePacket, WT_MAPMOVE_SERVER );

			PU::TWriteTable_AM( kMovePacket, kContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, kContUnitSummonedMapMoveData );
			kContModifyOrder.WriteToPacket( kMovePacket );

			SendToGround( rkRMM.kTargetKey, kMovePacket, true );
		}break;
	case MMT_PUBLIC_PERSONAL:
		{
			// Г¤іО АМµїАМёй ї©±вј­ БцїцѕЯ ЗПґВ°ЕѕЯ В«ЅГЕ°Бц ё»°Н!
			BM::Stream kClientPacket( PT_T_C_NFY_MAP_MOVING_CHANNEL_CHANGE );

			BM::Stream kMovePacket(PT_T_N_REQ_MAP_MOVE, rkRMM);
			constellationMission.WriteToPacket(kMovePacket);
			kMovePacket.Push(g_kProcessCfg.ChannelNo());

			size_t const iWrPos = kMovePacket.WrPos();
			kMovePacket.Push( rkUnitArray.size() );
			size_t iWriteSize = 0;

			UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
			for ( ; unit_itr != rkUnitArray.end() ; ++unit_itr )
			{
				PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
				if ( pkPlayer )
				{
					PgParty_Release::DoAction( pkPlayer, false );// ЖДЖј Б¦°Е
					pkPlayer->WriteToPacket( kMovePacket, WT_MAPMOVE_FIRST );
					++iWriteSize;

					CONT_CENTER_PLAYER_BY_KEY::iterator key_itr = m_kContPlayer_CharKey.find( pkPlayer->GetID() );
					if ( key_itr != m_kContPlayer_CharKey.end() ) 
					{
						CONT_CENTER_PLAYER_BY_KEY::mapped_type pkSavedPtr = key_itr->second;
						if ( pkSavedPtr )
						{
							// Е¬¶уАМѕрЖ®ЗСЕЧ ёКАМµїБЯ Г¤іОАМ єЇ°жµИґЩ°н ѕЛ·ББШґЩ.
							::SendToUser( pkSavedPtr->GetMemberGUID(), pkSavedPtr->GetSwitchServer(), kClientPacket );

							// јѕЕН±Ю ДБЕЩГчЗСЕЧ Елєё
							ProcessRemoveUser_NotifyOtherMgr( pkSavedPtr, true );

							// ЗГ·№АМѕо »иБ¦
							RemovePlayer(pkSavedPtr);
							g_kTotalObjMgr.ReleaseUnit( pkSavedPtr );
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pkPlayer is NULL ") << pkPlayer->GetID() );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
						}
					}
					else
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not Found Player<") << pkPlayer->Name() << _T("/") << pkPlayer->GetID() << _T(">") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
					}
				}
			}

			if ( iWriteSize != rkUnitArray.size() )
			{
				kMovePacket.ModifyData( iWrPos, &iWriteSize, sizeof(iWriteSize) );
			}

			PU::TWriteTable_AM( kMovePacket, kContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, kContUnitSummonedMapMoveData );
			kContModifyOrder.WriteToPacket( kMovePacket );

			SendToContents(kMovePacket);
		}break;
	case MMT_PUBLIC_PARTY:
		{
			BM::Stream kMovePacket(PT_T_N_REQ_MAP_MOVE, rkRMM);
			constellationMission.WriteToPacket(kMovePacket);
			kMovePacket.Push(g_kProcessCfg.ChannelNo());
			
			size_t const iWrPos = kMovePacket.WrPos();
			kMovePacket.Push( rkUnitArray.size() );
			size_t iWriteSize = 0;

			UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
			for ( ; unit_itr != rkUnitArray.end() ; ++unit_itr )
			{
				PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
				if ( pkPlayer )
				{
					PgParty_Release::DoAction( pkPlayer, true );// ЖДЖј №цЗБ Б¦°Е
					pkPlayer->WriteToPacket( kMovePacket, WT_MAPMOVE_FIRST );
					++iWriteSize;

					CONT_CENTER_PLAYER_BY_KEY::iterator key_itr = m_kContPlayer_CharKey.find( pkPlayer->GetID() );
					if ( key_itr != m_kContPlayer_CharKey.end() ) 
					{
						CONT_CENTER_PLAYER_BY_KEY::mapped_type pkSavedPtr = key_itr->second;
						if ( pkSavedPtr )
						{
							// јѕЕН±Ю ДБЕЩГчЗСЕЧ Елєё
							ProcessRemoveUser_NotifyOtherMgr( pkSavedPtr, false );

							// ЗГ·№АМѕо »иБ¦
							RemovePlayer(pkSavedPtr);
							g_kTotalObjMgr.ReleaseUnit( pkSavedPtr );
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pkPlayer is NULL ") << pkPlayer->GetID() );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
						}
					}
					else
					{
						INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not Found Player<") << pkPlayer->Name() << _T("/") << pkPlayer->GetID() << _T(">") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data!"));
					}
				}
			}

			if ( iWriteSize != rkUnitArray.size() )
			{
				kMovePacket.ModifyData( iWrPos, &iWriteSize, sizeof(iWriteSize) );
			}

			PU::TWriteTable_AM( kMovePacket, kContPetMapMoveData );
			PU::TWriteTable_AM( kMovePacket, kContUnitSummonedMapMoveData );
			kContModifyOrder.WriteToPacket( kMovePacket );

			SendToContents(kMovePacket);
		}break;
	default:
		{
			// АОґш»эјєА» ЗШѕЯ ЗПґВ °Е АП јцµµ АЦґЩ!
			// АОґшАё·О »эјє ЗТ јц АЦґВБц ГЈѕЖєёАЪ.
			SGroundKey kGndkey( rkRMM.kTargetKey.GroundNo(), BM::GUID::IsNull(rkRMM.kTargetKey.Guid()) ? BM::GUID::Create() : rkRMM.kTargetKey.Guid() );
			HRESULT const hRet = GroundLoadBalance( kGndkey, rkRMM.kTargetSI );
			if ( S_OK == hRet )
			{
				// АОґшАё·О ёёµйАЪ.
				rkRMM.kTargetKey.Guid( kGndkey.Guid() );

				if ( rkRMM.cType == MMET_Login )
				{
					rkRMM.nTargetPortal = 1;
				}

				SPortalWaiter kPortalWaiter(rkRMM, (rkRMM.cType==MMET_Login) ? PORTAL_NODELAY : PORTAL_DELAY );
				kPortalWaiter.constellationMission = constellationMission;
				kPortalWaiter.m_kContPetMoveData.swap( kContPetMapMoveData );
				kPortalWaiter.m_kContUnitSummonedMoveData.swap( kContUnitSummonedMapMoveData );
				kPortalWaiter.m_kModifyOrder.swap( kContModifyOrder );
				kPortalWaiter.iOwnerLv = rkRMM.iOwnerLv;

				if ( UpdatePlayerData( rkUnitArray, &kPortalWaiter, true ) )
				{
					BM::Stream kPortalPacket( PT_A_T_REQ_INDUN_CREATE );
					kPortalWaiter.WriteToPacket( kPortalPacket );
					SendToPortalMgr( kPortalPacket );
					return true;
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if ( g_kProcessCfg.IsPublicChannel() )
			{// Public Channel АМ¶уёй їш·ЎГ¤іО·О µ№·Бєёі»ѕЯ ЗСґЩ.
				rkRMM.cType = MMET_BackToChannel;
				rkRMM.kTargetKey.Clear();
				return ReqMapMove( rkUnitArray, rkRMM, constellationMission, kContPetMapMoveData, kContUnitSummonedMapMoveData, kContModifyOrder );
			}

			INFO_LOG( BM::LOG_LV1, __FL__ << _T("NotFound Target Ground[") << rkRMM.kTargetKey.GroundNo() << _T("-") << rkRMM.kTargetKey.Guid().str().c_str() << _T("] Next UserList") );

			// ґЩЅГ µ№·БєёіЅґЩ.	
			// №«ЗС Recursive Call А» ё·±в А§ЗШ ЗС№шґх °Л»зЗШ БЦ±в
			if( S_OK == GetServerIdentity(rkRMM.kCasterKey, rkRMM.kTargetSI ) )
			{
				if ( rkRMM.SetBackHome(MMET_Failed) )
				{
					kContModifyOrder.clear();
					return ReqMapMove( rkUnitArray, rkRMM, constellationMission, kContPetMapMoveData, kContUnitSummonedMapMoveData, kContModifyOrder );
				}			
			}

			// ёК БшАФ ЅЗЖР ЅГ, їшБ¤ґл ·ОєсёКАМіЄ їшБ¤ґл ґшАьїЎ АЦѕъґВБц И®АОЗШѕЯ µИґЩ.
			// їшБ¤ґл јУјєА» °ЎБш ёКАМѕъА» °жїмїЎґВ АП№ЭГ¤іОїЎґВ ёКАМ ї¬°бµЗѕо АЦБц ѕКѕЖј­ БўјУЗТ јц°Ў ѕшґЩ.
			CONT_DEFMAP const * pContMap = NULL;
			g_kTblDataMgr.GetContDef(pContMap);
			if( pContMap )
			{
				CONT_DEFMAP::const_iterator map_iter = pContMap->find(rkRMM.kTargetKey.GroundNo());
				if( map_iter != pContMap->end() )
				{
					const TBL_DEF_MAP & DefMap = map_iter->second;
					if( (DefMap.iAttr & GKIND_EXPEDITION)
						|| (DefMap.iAttr & GKIND_EXPEDITION_LOBBY) )	// їшБ¤ґл јУјєА» °ЎБш ёКАО °жїм.
					{	
						UNIT_PTR_ARRAY::const_iterator unit_itor = rkUnitArray.begin();
						if( unit_itor != rkUnitArray.end() )
						{
							PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(unit_itor->pkUnit);
							if( pPlayer )
							{
								SRecentPlayerNormalMapData MapData;		// АМµїЗП·БґВ АЇАъДіёЇЕН°Ў °ЎБц°н АЦґВ АП№Э ёКБ¤єёё¦ °ЎБ®їВґЩ.
								pPlayer->GetRecentNormalMap(MapData);
								SGroundKey CasterKey(MapData.GroundNo);

								if( S_OK == GetServerIdentity(CasterKey, rkRMM.kTargetSI ) )	// ґЩЅГ ёК БшАФ ЅГµµ.
								{
									rkRMM.kCasterKey = CasterKey;
									rkRMM.pt3CasterPos = MapData.RecentPos;
									if ( rkRMM.SetBackHome(MMET_Failed) )
									{
										kContModifyOrder.clear();
										return ReqMapMove( rkUnitArray, rkRMM, constellationMission, kContPetMapMoveData, kContUnitSummonedMapMoveData, kContModifyOrder );
									}			
								}
							}
						}
					}
				}
			}

			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot go back to Origin_Map because cannot find Ground[") << rkRMM.kTargetKey.GroundNo() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}

	return true;
}

void PgServerSetMgr::Locked_Recv_PT_M_T_REQ_MAP_MOVE_COME( SReqMapMove_CM const &kRMMC, VEC_GUID const &kUserList, bool const bMemberGuid )
{
	BM::CAutoMutex kLock(m_kMutex,true);
	
	UNIT_PTR_ARRAY kUnitArray;
	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr;
	VEC_GUID::const_iterator user_itr = kUserList.begin();
	for ( ; user_itr!=kUserList.end(); ++user_itr )
	{
		if ( bMemberGuid )
		{
			login_itr = m_kContPlayer_MemberKey.find( *user_itr );
			if ( login_itr == m_kContPlayer_MemberKey.end() )
			{
				BM::Stream kFailedPacket( PT_T_M_ANS_MAP_MOVE_COME_FAILED, *user_itr );
				SendToGround( kRMMC.kGndKey, kFailedPacket, true );
				continue;
			}
		}
		else
		{
			login_itr = m_kContPlayer_CharKey.find( *user_itr );
			if ( login_itr == m_kContPlayer_CharKey.end() )
			{
				BM::Stream kFailedPacket( PT_T_M_ANS_MAP_MOVE_COME_FAILED, *user_itr );
				SendToGround( kRMMC.kGndKey, kFailedPacket, true );
				continue;
			}
		}

		PgPlayer *pkUser = login_itr->second;

		// MapMove Lock »уЕВ °Л»з
		BM::GUID kLockKey;
		if (GetMapMoveLock(pkUser->GetID(), kLockKey))
		{
			if (kLockKey != BM::GUID::NullData())
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot MapMove, because already MapMoveLocked Player=") << pkUser->GetID() << _T(", Nanme=")
					<< pkUser->Name() << _T(", SavedMapMoveKey=") << kLockKey.str().c_str());

				BM::Stream kFailedPacket( PT_T_M_ANS_MAP_MOVE_COME_FAILED, *user_itr );
				SendToGround( kRMMC.kGndKey, kFailedPacket, true );
				continue;
			}
		}

		int const iGroundNo = pkUser->GroundKey().GroundNo();
		switch ( iGroundNo )
		{
		case PvP_Lobby_GroundNo_AnterRoom:
		case PvP_Lobby_GroundNo_Exercise:
		case PvP_Lobby_GroundNo_Ranking:
		case PvP_Lobby_GroundNo_League:
			{
				if ( MMET_Normal != kRMMC.cType )
				{
					break;
				}
			}// break »зїлЗПБц ѕКґВґЩ.
		case 0:
			{
				kUnitArray.Add( pkUser );
			}break;
		default:
			{
				BM::Stream kMPacket(PT_A_M_NFY_REQ_MAP_MOVE);
				kRMMC.WriteToPacket(kMPacket);
				kMPacket.Push( pkUser->GetID() );
				SendToGround( pkUser->GroundKey(), kMPacket, true );
			}break;
		}

	}

	if ( !kUnitArray.empty() )
	{
		SReqMapMove_MT kRMM((EMapMoveEventType)kRMMC.cType);
		kRMM.kTargetKey = kRMM.kCasterKey = kRMMC.kGndKey;
		kRMM.pt3TargetPos = kRMMC.pt3TargetPos;

		Constellation::SConstellationMission constellationMission;
		CONT_PET_MAPMOVE_DATA kEmptyPet;
		CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmptyUserSummoned;
		CONT_PLAYER_MODIFY_ORDER kEmptyOrder;
		ReqMapMove( kUnitArray, kRMM, constellationMission, kEmptyPet, kEmptyUserSummoned, kEmptyOrder );
	}
}

bool PgServerSetMgr::Locked_Recv_PT_M_T_REQ_MAP_MOVE( BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex, true);
	return RecvMapMove(pkPacket, false);
}

bool PgServerSetMgr::Locked_Recv_PT_T_T_REQ_MAP_MOVE( BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SReqMapMove_MT kRMM;
	Constellation::SConstellationMission constellationMission;
	if( pkPacket->Pop(kRMM) )
	{
		constellationMission.ReadFromPacket(*pkPacket);

		SPortalWaiter::CONT_WAIT_LIST kWaitList;
		CONT_PET_MAPMOVE_DATA kPetMapMoveData;
		CONT_UNIT_SUMMONED_MAPMOVE_DATA kUserSummonedMapMoveData;
		CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
		PU::TLoadTable_AA( *pkPacket, kWaitList );
		PU::TLoadTable_AM( *pkPacket, kPetMapMoveData );
		PU::TLoadTable_AM( *pkPacket, kUserSummonedMapMoveData );
		kContModifyOrder.ReadFromPacket( *pkPacket );

		UNIT_PTR_ARRAY kUnitArray;

		SPortalWaiter::CONT_WAIT_LIST::const_iterator wait_itr = kWaitList.begin();
		for ( ; wait_itr!=kWaitList.end() ; ++wait_itr )
		{
			CONT_CENTER_PLAYER_BY_KEY::const_iterator login_itr = m_kContPlayer_CharKey.find( wait_itr->first );
			
			// ї©±вј­ ёКАМ ЗТґз ѕИµИ ірµйёё АМµїЗП°Ф °Й·ЇБЦѕоѕЯ ЗСґЩ.
			if ( login_itr != m_kContPlayer_CharKey.end() )
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type const &pkElement = login_itr->second;
				if ( pkElement->VolatileID() == wait_itr->second )
				{
					if ( pkElement->GroundKey().IsEmpty() )
					{
						kUnitArray.Add( pkElement );
					}
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV6, __FL__ << L" Character<" << pkElement->Name() << L"/" << pkElement->GetID() << L"> LogOut->OtherJoin->OldMapMove Process Excute!!" );
				}
			}
		}

		if ( kUnitArray.size() )
		{
			if( !ReqMapMove( kUnitArray, kRMM, constellationMission, kPetMapMoveData, kUserSummonedMapMoveData, kContModifyOrder ) )
			{
				if(MMET_Failed_Login == kRMM.cType)
				{
					UNIT_PTR_ARRAY::const_iterator itor_unit = kUnitArray.begin();
					while (itor_unit != kUnitArray.end())
					{
						if(PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>( (*itor_unit).pkUnit ) )
						{
							BM::Stream kNPacket( PT_T_N_NFY_SELECT_CHARACTER_FAILED, pkPlayer->GetID() );
							SendToContents( kNPacket );

							BM::Stream kPacket( PT_T_C_ANS_SELECT_CHARACTER, E_SCR_SYSTEMCHECK );
							SendToUser( pkPlayer->GetMemberGUID(), kPacket );

							RemovePlayer(pkPlayer);
						}
						++itor_unit;
					}
					kUnitArray.clear();
				}
				return false;
			}
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_Recv_PT_T_T_REQ_RECENT_MAP_MOVE( BM::Stream * const pkPacket )
{
	SPortalWaiter::CONT_WAIT_LIST kWaiterList;
	PU::TLoadTable_AA( *pkPacket, kWaiterList );
	if ( kWaiterList.size() )
	{
		BM::CAutoMutex kLock(m_kMutex, true);

		SPortalWaiter::CONT_WAIT_LIST::const_iterator wait_itr = kWaiterList.begin();
		for ( ; wait_itr != kWaiterList.end() ; ++wait_itr )
		{
			CONT_CENTER_PLAYER_BY_KEY::const_iterator login_itr = m_kContPlayer_CharKey.find( wait_itr->first );
			if ( login_itr != m_kContPlayer_CharKey.end() )
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type const &pkPlayer = login_itr->second;
				if ( pkPlayer )
				{// ї©±вј­ ёКАМ ЗТґз ѕИµИ ірµйёё АМµїЗП°Ф °Й·ЇБЦѕоѕЯ ЗСґЩ.
					if ( pkPlayer->VolatileID() == wait_itr->second )
					{
						if ( pkPlayer->GroundKey().IsEmpty() )
						{
							SReqMapMove_MT kRMM( MMET_Failed );
							kRMM.pt3TargetPos = pkPlayer->GetRecentPos(GATTR_DEFAULT);
							pkPlayer->GetRecentGround( kRMM.kTargetKey, GATTR_DEFAULT );
							kRMM.kCasterKey = kRMM.kTargetKey;

							UNIT_PTR_ARRAY kUnitArray(pkPlayer);
							Constellation::SConstellationMission EmptyConstellationMission;
							CONT_PET_MAPMOVE_DATA kEmptyPet;
							CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmptyUserSummoned;
							CONT_PLAYER_MODIFY_ORDER kEmptyOrder;
							ReqMapMove( kUnitArray, kRMM, EmptyConstellationMission, kEmptyPet, kEmptyUserSummoned, kEmptyOrder );
						}
					}
					else
					{
						CAUTION_LOG( BM::LOG_LV6, __FL__ << L" Character<" << pkPlayer->Name() << L"/" << pkPlayer->GetID() << L"> LogOut->OtherJoin->OldMapMove Process Excute!!" );
					}
				}
			}
		}
		return true;
	}
	
	return false;
}

bool PgServerSetMgr::RecvMapMove( BM::Stream * const pkPacket, bool bResMovePacket )
{
	SReqMapMove_MT kRMM;
	Constellation::SConstellationMission constellationMission;
	if( pkPacket->Pop(kRMM) )
	{
		constellationMission.ReadFromPacket(*pkPacket);

		UNIT_PTR_ARRAY kUnitArray;
		kUnitArray.ReadFromPacket(*pkPacket);

		CONT_PET_MAPMOVE_DATA kContPetMapMoveData;
		PU::TLoadTable_AM( *pkPacket, kContPetMapMoveData );

		CONT_UNIT_SUMMONED_MAPMOVE_DATA kContUserSummonedMapMoveData;
		PU::TLoadTable_AM( *pkPacket, kContUserSummonedMapMoveData );

		CONT_PLAYER_MODIFY_ORDER kContModifyOrder;
		kContModifyOrder.ReadFromPacket( *pkPacket );

		if ( true == bResMovePacket )
		{
			UNIT_PTR_ARRAY::const_iterator itor_unit = kUnitArray.begin();
			while (itor_unit != kUnitArray.end())
			{
				RemoveMapMoveLock((*itor_unit).pkUnit->GetID());
				++itor_unit;
			}

			if ( UpdatePlayerData( kUnitArray ) )
			{
				SERVER_IDENTITY kSI;
				switch ( kRMM.cType )
				{
				case MMET_PvP:
				case MMET_BackToChannel:
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Cannot go to this line TargetGroundType[") << kRMM.cType << _T("]") );
					}break;
				case MMET_BackToPvP:
				case MMET_KickToPvP:
					{
						bool const bKick = ( MMET_KickToPvP == kRMM.cType );

						BM::Stream kContents( PT_C_T_REQ_EXIT_ROOM, bKick );
						kUnitArray.WriteToPacket( kContents, WT_MAPMOVE_SERVER|WT_OP_NOHPMP );

						if ( !SendToPvPLobby( kContents, kRMM.kTargetKey.GroundNo() ) )
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Error") );
						}
					}break;
				default:
					{
						if( S_OK == GetServerIdentity( kRMM.kTargetKey, kSI ) )
						{//ј­№ц°Ў АЦѕоѕЯБц.
							BM::Stream kPacket(PT_T_M_REQ_MAP_MOVE, kRMM );
							kUnitArray.WriteToPacket( kPacket, WT_MAPMOVE_SERVER );
							PU::TWriteTable_AM( kPacket, kContPetMapMoveData );
							PU::TWriteTable_AM( kPacket, kContUserSummonedMapMoveData );
							kContModifyOrder.WriteToPacket( kPacket );
							return SendToGround( kRMM.kTargetKey, kPacket, true );
						}

						// ј­№ц°Ў ѕшАёёй ґЩЅГ µ№·Бєёі»ѕЯ ЗШ
						// ї©±в °Йё±АПАМ АЦА»±о?
						if ( kRMM.SetBackHome(MMET_Failed) )
						{
							return ReqMapMove( kUnitArray, kRMM, constellationMission, kContPetMapMoveData, kContUserSummonedMapMoveData, kContModifyOrder );
						}

						UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
						for ( ; unit_itr!=kUnitArray.end(); ++unit_itr )
						{
							INFO_LOG( BM::LOG_LV0, __FL__ << _T("[MapMove Failed] [MapNo:") << kRMM.kTargetKey.GroundNo() << _T("] ") 
								<< unit_itr->pkUnit->Name().c_str() << _T("-") << unit_itr->pkUnit->GetID().str().c_str() << _T("]") );

							PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
							if ( pkPlayer )
							{
								BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_CharMapErr) );
								kDPacket.Push( pkPlayer->GetMemberGUID() );
								SendToServer( pkPlayer->GetSwitchServer(), kDPacket );
							}
						}
					}break;
				}
			}
		}
		else
		{
			// MapАё·О єОЕН їдГ» №ЮѕТґЩ -> Contents·О єёі»БаѕЯ ЗСґЩ.
			return ReqMapMove( kUnitArray, kRMM, constellationMission, kContPetMapMoveData, kContUserSummonedMapMoveData, kContModifyOrder );
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_Recv_PT_M_T_REQ_Kick_User( BM::Stream * const pkPacket  )
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID kCmdGuid;
	BM::GUID kMemberGuid;

	pkPacket->Pop( kCmdGuid );
	pkPacket->Pop( kMemberGuid );

	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr = m_kContPlayer_MemberKey.find( kMemberGuid );
	if ( login_itr != m_kContPlayer_MemberKey.end() )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCData = login_itr->second;
		if ( pCData )
		{
//			SChannelChangeData kChannelChangeData( g_kProcessCfg.RealmNo(), nChannelNo );
//			kChannelChangeData.kCharGuid = pCData->GetID();

			BM::Stream kPacket( PT_T_N_ANS_KICK_USER);
			kPacket.Push( kCmdGuid );

			ProcessRemoveUser_Common(pCData);

			RemovePlayer(pCData);
			g_kTotalObjMgr.ReleaseUnit( pCData );

			SendToContents(kPacket);

			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgServerSetMgr::Locked_Recv_PT_M_T_REFRESH_LEVELUP(BM::Stream* const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID kCharGuid;
	int iLevel = 0;

	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(iLevel);

	CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(kCharGuid);
	if( m_kContPlayer_CharKey.end() != iter )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type const pkElement = (*iter).second;
		if( pkElement)
		{
			if( iLevel > 0  )
			{
				pkElement->SetAbil(AT_LEVEL, iLevel);
			}			
		}
	}
}

void PgServerSetMgr::Locked_Recv_PT_M_T_REFRESH_CLASS_CHANGE(BM::Stream* const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID kCharGuid;
	int iClass = 0;

	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(iClass);

	CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(kCharGuid);
	if( m_kContPlayer_CharKey.end() != iter )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type const pkElement = (*iter).second;
		if( pkElement)
		{
			if( iClass > 0  )
			{
				pkElement->SetAbil(AT_CLASS, iClass);
			}			
		}
	}
}

bool PgServerSetMgr::Locked_PT_T_T_REQ_EXIT_LOBBY( BM::Stream* const pkPacket )
{
	size_t const iRDPos = pkPacket->RdPos();

	pkPacket->RdPos( iRDPos + sizeof(EUnitType) );
	
	BM::GUID kCharacterGuid;
	pkPacket->Pop( kCharacterGuid );

	pkPacket->RdPos( iRDPos );

	BM::CAutoMutex kLock(m_kMutex, true);

 	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr = m_kContPlayer_CharKey.find( kCharacterGuid );
	if ( login_itr != m_kContPlayer_CharKey.end() )
	{
		PgPlayer *pkPlayer = login_itr->second;
		pkPlayer->ReadFromPacket( *pkPacket );

		SReqMapMove_MT kRMM( MMET_BackToChannel );
		kRMM.pt3TargetPos = pkPlayer->GetRecentPos(GATTR_DEFAULT);
		pkPlayer->GetRecentGround( kRMM.kTargetKey, GATTR_DEFAULT );

		UNIT_PTR_ARRAY kUnitArray(pkPlayer);
		Constellation::SConstellationMission EmptyConstellationMission;
		CONT_PET_MAPMOVE_DATA kEmptyPet;
		CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmptyUserSummoned;
		CONT_PLAYER_MODIFY_ORDER kEmptyOrder;
 		return ReqMapMove( kUnitArray, kRMM, EmptyConstellationMission, kEmptyPet, kEmptyUserSummoned, kEmptyOrder );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgServerSetMgr::Locked_PT_T_T_REQ_KICK_ROOM_USER( BM::GUID const &kCharGuid, SGroundKey const &kGndKey )const
{
	BM::CAutoMutex kLock( m_kMutex, false );

	CONT_CENTER_PLAYER_BY_KEY::const_iterator login_itr = m_kContPlayer_CharKey.find( kCharGuid );
	if ( login_itr != m_kContPlayer_CharKey.end() )
	{
		PgPlayer const *pkPlayer = login_itr->second;
		switch( pkPlayer->GroundKey().GroundNo() )
		{
		case PvP_Lobby_GroundNo_Exercise:
		case PvP_Lobby_GroundNo_Ranking:
		case PvP_Lobby_GroundNo_League:
			{
				// ·ОєсїЎёё АЦґВ °ЕґП±о
				BM::Stream kPacket( PT_ROOM_TO_LOBBY_USER, static_cast<size_t>(1) );
				kPacket.Push( kCharGuid );
				kPacket.Push( true );
				SendToPvPLobby( kPacket, pkPlayer->GroundKey().GroundNo() );
			}break;
		case PvP_Lobby_GroundNo_AnterRoom:
		case 0:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ );
				BM::Stream kPacket( PT_ROOM_TO_LOBBY_USER, static_cast<size_t>(1) );
				kPacket.Push( kCharGuid );
				kPacket.Push( true );
				SendToPvPLobby( kPacket, pkPlayer->GroundKey().GroundNo() );
				// АМ°З ѕИµЗґВµҐ.
			}break;
		default:
			{
				if ( kGndKey == pkPlayer->GroundKey() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ );
				}
				else
				{
					BM::Stream kGndPacket( PT_T_M_REQ_KICK_ROOM_USER, kCharGuid );
					SendToGround( pkPlayer->GroundKey(), kGndPacket, true );
				}
			}break;
		}
	}
}

bool PgServerSetMgr::Locked_Recv_PT_T_T_ANS_MAP_MOVE_RESULT( BM::GUID const &kCharGuid, SAnsMapMove_MT& rkAMM )
{
	BM::CAutoMutex kLock(m_kMutex, true);
	return RecvAnsMapMove( kCharGuid, rkAMM, rkAMM.kGroundKey );
}

bool PgServerSetMgr::Locked_Recv_PT_M_T_ANS_MAP_MOVE_RESULT( BM::Stream * const pkPacket )
{
	SAnsMapMove_MT kAMM;
	if ( !pkPacket->Pop(kAMM) )
	{
		return false;
	}

	UNIT_PTR_ARRAY kUnitArray;
	kUnitArray.ReadFromPacket( *pkPacket, false );

	BM::CAutoMutex kLock(m_kMutex, true);

	if( !IsAliveGround( kAMM.kGroundKey ) )
	{
		kAMM.eRet = MMR_FAILED_AGAIN;
	}

	switch( kAMM.eRet )
	{
	case MMR_NONE:		//	ГіАЅ ·О±ЧАОЗШј­ MapАМµїїЎ јє°шЗС °жїмАМґЩ.
	case MMR_SUCCESS:	//	MapАМµїїЎ јє°шЗЯАёґП±о АМµїЅГЕ°АЪ
		{
			SGroundKey kRealGndKey = kAMM.kGroundKey;
			kRealGndKey.ReadFromPacket( *pkPacket );

			if ( RecvAnsMapMove( kUnitArray, kAMM, kRealGndKey ) )
			{
				static size_t const iSize = 1;

				UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
				for ( ; unit_itr!=kUnitArray.end(); ++unit_itr )
				{
					PgPlayer* pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);

					// АЇАъїЎ°Ф Елєё
					BM::Stream kCPacket( PT_T_C_NFY_CHARACTER_MAP_MOVE );
					kCPacket.Push(kAMM.cType);
					kCPacket.Push(kRealGndKey);
					kCPacket.Push(kAMM.kAttr);
					kCPacket.Push(pkUser->GetID());
					kCPacket.Push(iSize);
					if ( MMR_NONE == kAMM.eRet )
					{
						pkUser->WriteToPacket( kCPacket, WT_MAPMOVE_FIRST );
					}
					else
					{
						pkUser->WriteToPacket( kCPacket, WT_MAPMOVE_CLIENT );
					}
					kCPacket.Push( *pkPacket );

					SendToUser( pkUser->GetMemberGUID(), kCPacket );
				}
			}
		}break;
	case MMR_FAILED:	// MapїЎј­ АМµїАМ ЅЗЖРЗЯАёґП±о АМАьёКАё·О ґЩЅГ µ№·Бєёі»АЪ
		{
			SReqMapMove_MT kRMM((EMapMoveEventType)kAMM.cType);
			kRMM.nTargetPortal = 0;
			kRMM.pt3TargetPos = kAMM.pt3Pos;
			kRMM.kCasterKey = kRMM.kTargetKey = kAMM.kGroundKey;

			Constellation::SConstellationMission EmptyConstellationMission;
			CONT_PET_MAPMOVE_DATA kEmptyPet;
			CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmptyUserSummoned;
			CONT_PLAYER_MODIFY_ORDER kEmptyOrder;
			return ReqMapMove( kUnitArray, kRMM, EmptyConstellationMission, kEmptyPet, kEmptyUserSummoned, kEmptyOrder );
		}break;
	case MMR_FAILED_AGAIN:	
		{
			UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
			for ( ; unit_itr!=kUnitArray.end(); ++unit_itr )
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("This User<") << unit_itr->pkUnit->Name() << _T("> Missing Ground") );
				PgPlayer* pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
				if ( pkUser )
				{
					if ( g_kProcessCfg.IsPublicChannel() )
					{
						// °шїлГ¤іОїЎј­ґВ АМАьГ¤іО·О є№±Н ЅГДСБЦѕоѕЯ ЗСґЩ.
						SReqMapMove_MT kRMM( MMET_BackToChannel );
						kRMM.pt3TargetPos = pkUser->GetRecentPos(GATTR_DEFAULT);
						pkUser->GetRecentGround( kRMM.kTargetKey, GATTR_DEFAULT );

						Constellation::SConstellationMission EmptyConstellationMission;
						CONT_PET_MAPMOVE_DATA kEmptyPet;
						CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmptyUserSummoned;
						CONT_PLAYER_MODIFY_ORDER kEmptyOrder;
						return ReqMapMove( kUnitArray, kRMM, EmptyConstellationMission, kEmptyPet, kEmptyUserSummoned, kEmptyOrder );
//						break;
					}
					else
					{
						// АП№Э Г¤іОАМёй БўјУА» Ічѕо№ц·Б!!
						BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, static_cast<BYTE>(CDC_CharMapErr) );
						kDPacket.Push( pkUser->GetMemberGUID() );
						::SendToServer( pkUser->GetSwitchServer(), kDPacket );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << unit_itr->pkUnit->Name() << _T("/") << unit_itr->pkUnit->GetID() << _T(" Is Not Player!!!") );
				}
			}	
		}break;
	}
	return true;
}

bool PgServerSetMgr::FindRandomMap( SGroundKey& kGndKey )const
{
// 	CONT_GROUND::const_iterator itrNo = m_kContAliveGround.begin();
// 	while(m_kContAliveGround.end() != itrNo)//ѕЖ№«ёКАМіЄ Г№№шВ° ёКА» ГЈѕЖј­
// 	{
// 		CONT_GROUND::mapped_type const& rkGnd = (*itrNo).second;
// 		if(	rkGnd.kAttr == GATTR_DEFAULT || rkGnd.kAttr == GATTR_VILLAGE )
// 		{
// 			kGndKey = rkGnd.kKey;
// 			return true;
// 		}
// 		++itrNo;
// 	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

// HRESULT PgServerSetMgr::Locked_UserOpenGround(BM::GUID const &rkMemberGuid, int const iGroundNo)
// {
// 	BM::CAutoMutex kLock(m_kMutex, true);
// 
// 	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr = m_kContPlayer_MemberKey.find(rkMemberGuid);
// 	if ( login_itr == m_kContPlayer_MemberKey.end() )
// 	{
// 		return E_FAIL;
// 	}
// 
// 	return login_itr->second->kMapInfo.IsOpenGround(iGroundNo);
// }

bool PgServerSetMgr::RecvAnsMapMove( BM::GUID const &kCharGuid, SAnsMapMove_MT const &rkAMM, SGroundKey const &kRealGndKey )
{
	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr=  m_kContPlayer_CharKey.find( kCharGuid );
	if ( login_itr != m_kContPlayer_CharKey.end() )
	{
		PgPlayer *pkPlayer = login_itr->second;
		if ( pkPlayer )
		{
			// ЅєА§ДЎ·О Елєё
			BM::Stream kSPacket( PT_T_S_NFY_CHARACTER_MAP_MOVE, rkAMM.kSI );
			kSPacket.Push( kRealGndKey );
			kSPacket.Push( pkPlayer->GetMemberGUID() );
			kSPacket.Push( pkPlayer->GetID() );
			SendToServer( pkPlayer->GetSwitchServer(), kSPacket );

			//INFO_LOG( BM::LOG_LV0, __FL__ << _T("[") << pkPlayer->Name().c_str() << _T("]MapServer Changed Success[") << rkAMM.kSI.nServerNo << _T("]") );
			pkPlayer->GroundKey(rkAMM.kGroundKey);//±Ч¶уїоµеЕ° АъАе. (ј­№цЗБ·ОјјЅє №шИЈґВ АъАе ѕИЗФ)

//			if ( ! rkAMM.kGroundKey.IsEmpty() )
//			{
				// ё®јѕЖ® Б¤єёёё јѕЕНј­№цАЗ ёЮёрё®їЎ ѕчµҐАМЖ® ЗШБШґЩ.
//				pkPlayer->UpdateRecentGround( rkAMM.kGroundKey, rkAMM.kAttr);// ЗКїд°Ў АЦіЄ??
//				pkPlayer->SetPos(  );
//				pkPlayer->UpdateRecentPos( rkAMM.kAttr );
//			}

			{
				// °ў°ўАЗ Contents Е¬·ЎЅє·О Елєё
				SContentsUser kContentsUserData;
				pkPlayer->CopyTo(kContentsUserData);

				BM::Stream kXPacket(PT_T_N_NFY_USER_ENTER_GROUND);
				kXPacket.Push(rkAMM);
				kContentsUserData.WriteToPacket(kXPacket);
				kXPacket.Push(pkPlayer->GuildGuid());
				::SendToGlobalPartyMgr(kXPacket);						kXPacket.PosAdjust();
				::SendToFriendMgr(kXPacket);							kXPacket.PosAdjust();
				::SendToGuildMgr(kXPacket);								kXPacket.PosAdjust();
				::SendToCoupleMgr(kXPacket);							kXPacket.PosAdjust();
				::SendToRealmContents(PMET_EVENTQUEST, kXPacket);		kXPacket.PosAdjust();
				::SendToRealmContents(PMET_BATTLESQUARE, kXPacket);		kXPacket.PosAdjust();
			}
			return true;
		}	
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("[") << kCharGuid.str().c_str() << _T("]Is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlayer is NULL"));
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//ёКїЎј­ №ЮАє АЇАъ Б¤єёё¦ ±в·ПЗШѕЯµК.
bool PgServerSetMgr::RecvAnsMapMove( UNIT_PTR_ARRAY &rkUnitArray, SAnsMapMove_MT const &rkAMM, SGroundKey const &kRealGndKey )
{
	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr;
	UNIT_PTR_ARRAY::iterator unit_itr = rkUnitArray.begin();
	while ( unit_itr!= rkUnitArray.end() )
	{
		if ( !RecvAnsMapMove( unit_itr->pkUnit->GetID(), rkAMM, kRealGndKey ) )
		{
			unit_itr = rkUnitArray.erase( unit_itr );
		}
		else
		{
			++unit_itr;
		}
	}

	return (!rkUnitArray.empty());
}

bool PgServerSetMgr::Locked_NfyReloadGameData()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::Stream kPacket(PT_T_M_NFY_RELOAD_GAMEDATA);

	INFO_LOG(BM::LOG_LV7, _T("Send StoreKey(ReloadData) ::")<< g_kControlDefMgr.StoreValueKey());
	kPacket.Push(g_kControlDefMgr.StoreValueKey());//StoreKey. іЄ°Ё.
	g_kTblDataMgr.DataToPacket(kPacket);
	
	SendToServerType(CEL::ST_MAP, kPacket);
	SendToServerType(CEL::ST_ITEM, kPacket);

	return true;
}

bool PgServerSetMgr::CheckValidBaseWear(int const iWearNo, const EKindCharacterBaseWear eType)
{
	CONT_DEFCHARACTER_BASEWEAR const* pContDefCharacterBaseWear = NULL;
	g_kTblDataMgr.GetContDef(pContDefCharacterBaseWear);

	CONT_DEFCHARACTER_BASEWEAR::const_iterator wearItor = pContDefCharacterBaseWear->find(iWearNo);
	if(pContDefCharacterBaseWear->end() != wearItor)
	{
		if(eType == (*wearItor).second.iWearType)
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgServerSetMgr::Locked_DisplayState()
{
	BM::CAutoMutex kLock(m_kMutex);
	U_STATE_LOG(BM::LOG_LV1, _T("------------ Logined User List -----------"));

	CONT_CENTER_PLAYER_BY_ID::const_iterator user_itor = m_kContPlayer_MemberID.begin();

	while(user_itor != m_kContPlayer_MemberID.end())
	{
		CONT_CENTER_PLAYER_BY_ID::mapped_type pElement = (*user_itor).second;
		if( pElement )
		{
			U_STATE_LOG(BM::LOG_LV6, L"LoginedUser ID["<<(*user_itor).first<<L"] "<<C2L(pElement->GroundKey()));
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"pElement is NULL");
		}
		++user_itor;
	}

	U_STATE_LOG(BM::LOG_LV1, _T("--------------------- Cut Line ---------------------"));
}

void PgServerSetMgr::Recv_PT_M_T_NFY_CREATE_GROUND( BM::Stream &rkPacket )
{
	GroundArr kGndArr;
	rkPacket.Pop(kGndArr);

	GroundArr::const_iterator gnd_itr = kGndArr.begin();
	for( ; gnd_itr != kGndArr.end() ; ++gnd_itr )
	{
		SGround const &rkGnd = *gnd_itr;
		if ( SUCCEEDED( Locked_RegistGnd( rkGnd ) ) )
		{
			if ( BM::GUID::IsNotNull(rkGnd.kKey.Guid()) )
			{
				switch ( rkGnd.kAttr )
				{
				case GATTR_EMPORIA:
				case GATTR_EMPORIABATTLE:
				case GATTR_HARDCORE_DUNGEON:
					{
					}break;
				case GATTR_BOSS:
				case GATTR_HARDCORE_DUNGEON_BOSS:
					{
						BM::Stream kPortalPacket( PT_M_T_ANS_CREATE_GROUND );
						rkGnd.kKey.WriteToPacket( kPortalPacket );
						kPortalPacket.Push(S_OK);
						::SendToPortalMgr( kPortalPacket );

						::SendToChannelContents( PMET_Boss, kPortalPacket );
					}break;
				default:
					{
						BM::Stream kPortalPacket( PT_M_T_ANS_CREATE_GROUND, rkGnd.kKey );
						kPortalPacket.Push(S_OK);
						::SendToPortalMgr( kPortalPacket );
					}break;
				}
			}

			BM::Stream kBSPacket(PT_A_A_REQ_BS_GAME_INFO);
			rkGnd.kKey.WriteToPacket( kBSPacket );
			::SendToRealmContents( PMET_BATTLESQUARE, kBSPacket );

			CONT_DEFGROUNDBUILDINGS const * pkContDef = NULL;
			g_kTblDataMgr.GetContDef(pkContDef);
			if(pkContDef)
			{
				CONT_DEFGROUNDBUILDINGS::const_iterator gnd_iter = pkContDef->find( rkGnd.kKey.GroundNo() );
				if ( gnd_iter != pkContDef->end() )
				{
					for(CONT_SET_BUILDINGS::const_iterator map_iter = (*gnd_iter).second.kCont.begin();map_iter != (*gnd_iter).second.kCont.end();++map_iter)
					{
						//if(0 < (*map_iter).iGrade) // ёрµз ё¶АМИЁ ·Оµщ
						{
							BM::Stream kHomeTownPacket( PT_T_N_REQ_LOAD_MYHOME );
							rkGnd.kKey.WriteToPacket( kHomeTownPacket );
							kHomeTownPacket.Push( (*map_iter).iBuildingNo );
							::SendToContents(kHomeTownPacket);
						}
					}
				}
			}
		}
	}
}

HRESULT PgServerSetMgr::Locked_RegistGnd( SGround const &rkGnd )
{
	BM::CAutoMutex kLock( m_kMutex, true );

	std::pair<CONT_GROUND::iterator, bool> ret = m_kContAliveGround.insert(std::make_pair(rkGnd.kKey, rkGnd));
    if( ret.second )
	{
		SServerBalance* pkBalance = GetServerBalance(rkGnd.kSI);
		if ( pkBalance )
		{
			pkBalance->IncGround(rkGnd.kAttr);
		}

		INFO_LOG( BM::LOG_LV6, L"[RegistGround] " << rkGnd.kKey.GroundNo() <<L" / " << rkGnd.kKey.Guid() );
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV1, L"[RegistGround]Failed "<< rkGnd.kKey.GroundNo() <<L" / " << rkGnd.kKey.Guid() );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << L"Return E_FAIL");
	return E_FAIL;
}

void PgServerSetMgr::UnregistGnd( SGround const &rkGnd )
{
	switch ( rkGnd.kAttr )
	{
	case GATTR_BOSS:
	case GATTR_HARDCORE_DUNGEON_BOSS:
	case GATTR_SUPER_GROUND:
	case GATTR_EXPEDITION_GROUND:
	case GATTR_ELEMENT_GROUND:
		{
			BM::Stream kNfyPacket( PT_M_T_NFY_DELETE_GROUND, rkGnd.kKey );
			SendToChannelContents( PMET_Boss, kNfyPacket );
		}break;
	case GATTR_MISSION:
	case GATTR_CHAOS_MISSION:
		{
			BM::Stream kNfyPacket( PT_T_T_NFY_MISSION_DELETE, rkGnd.kKey );
			SendToMissionMgr( kNfyPacket );
		}break;
	case GATTR_EMPORIA:
	case GATTR_EMPORIABATTLE:
		{
			BM::Stream kNfyPacket( PT_T_N_ANS_DELETE_PUBLICMAP, rkGnd.kKey );
			SendToRealmContents( PMET_EMPORIA, kNfyPacket );
		}break;
	case GATTR_HARDCORE_DUNGEON:
		{
			BM::Stream kNfyPacket( PT_T_N_ANS_DELETE_PUBLICMAP, rkGnd.kKey );
			SendToRealmContents( PMET_HARDCORE_DUNGEON, kNfyPacket );
		}break;
	}

//	size_t const iRemoveUserCount = ProcessRemoveUser_ByGround( kGnd.kKey );

	SServerBalance* pkBalance = GetServerBalance(rkGnd.kSI);
	if ( pkBalance )
	{
		pkBalance->DecGround( rkGnd.kAttr );
	}

	// ·О±Чё¦ іІ°Ь
// 	PgLogWrapper kLog(LOG_CONTENTS_MAP_CREATE);
// 	kLog.MemberKey( kGnd.kKey.Guid() );
// 	kLog.Push( static_cast<int>(0) );			// iValue1(»иБ¦0)
// 	kLog.Push( static_cast<int>(kGnd.kAttr) );	// iValue2(GroundType)
// 	kLog.Send();

	INFO_LOG( BM::LOG_LV6, L"[UnRegistGround] " << rkGnd.kKey.GroundNo() <<L" / " << rkGnd.kKey.Guid() );
}

HRESULT PgServerSetMgr::Locked_UnregistGnd(const GND_KEYLIST &kGndKeyList)
{
	BM::CAutoMutex kLock( m_kMutex, true );
	GND_KEYLIST::const_iterator key_itr = kGndKeyList.begin();
	for ( ; key_itr != kGndKeyList.end() ; ++key_itr )
	{
		CONT_GROUND::iterator gnd_itr = m_kContAliveGround.find(*key_itr);
		if( gnd_itr != m_kContAliveGround.end() )
		{
			UnregistGnd( gnd_itr->second );
			m_kContAliveGround.erase(gnd_itr);
		}
	}

	return S_OK;
}

HRESULT PgServerSetMgr::Locked_ReqUnregistGnd(SERVER_IDENTITY const &kSI)
{
	if( kSI.nServerType != CEL::ST_MAP )
	{
		return S_OK;
	}

	BM::CAutoMutex kLock( m_kMutex, true );
	GND_KEYLIST kGndKeyList;
	CONT_GROUND::iterator gnd_itor = m_kContAliveGround.begin();
	while( gnd_itor != m_kContAliveGround.end() )
	{
		if((*gnd_itor).second.kSI == kSI)
		{
			kGndKeyList.push_back((*gnd_itor).first);
		}
		++gnd_itor;
	}
	BM::Stream kTPacket(PT_T_N_REQ_DELETE_GROUND, g_kProcessCfg.ChannelNo());
	kTPacket.Push(kGndKeyList);
	SendToContents(kTPacket);

	return S_OK;

}

bool PgServerSetMgr::Locked_IsAliveGround(SGroundKey const &rkKey)const
{
	BM::CAutoMutex kLock( m_kMutex, false );
	return IsAliveGround( rkKey );
}

bool PgServerSetMgr::IsAliveGround( SGroundKey const &rkKey )const
{
	CONT_GROUND::const_iterator gnd_itr = m_kContAliveGround.find(rkKey);
	return m_kContAliveGround.end() != gnd_itr;
}

HRESULT PgServerSetMgr::GetServerIdentity(SGroundKey const &kGndKey, SERVER_IDENTITY &rkOutSI)const
{
	CONT_GROUND::const_iterator gnd_itor = m_kContAliveGround.find(kGndKey);

	if(gnd_itor != m_kContAliveGround.end())
	{
		rkOutSI = (*gnd_itor).second.kSI;
		return S_OK;	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

SGroundKey PgServerSetMgr::GetPlayerKey(BM::GUID const &rkMember)
{
	SGroundKey kGndKey;

	CONT_CENTER_PLAYER_BY_KEY::iterator itor = m_kContPlayer_MemberKey.find( rkMember );
	if( m_kContPlayer_MemberKey.end() != itor)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*itor).second;
		kGndKey = pData->GroundKey();
	}

	return kGndKey;
}

SGroundKey PgServerSetMgr::GetPlayerKey(std::wstring const &kId)
{
	SGroundKey kGndKey;
	
	
	CONT_CENTER_PLAYER_BY_ID::iterator itor = m_kContPlayer_MemberID.find( kId );
	if( m_kContPlayer_MemberID.end() != itor)
	{
		CONT_CENTER_PLAYER_BY_ID::mapped_type pData = (*itor).second;
		kGndKey = pData->GroundKey();
	}

	return kGndKey;
}

SServerBalance* PgServerSetMgr::GetServerBalance(SERVER_IDENTITY const &rhs)const
{
	CONT_SERVER_BALANCE::const_iterator itr;
	for ( itr=m_kContServerBalance.begin(); itr!= m_kContServerBalance.end(); ++itr )
	{
		if ( *(*itr) == rhs )
		{
			return *itr;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgServerSetMgr::Locked_Build( CONT_SERVER_HASH const &kContServerHash, CONT_MAP_CONFIG const &kContMapCfg_Static, CONT_MAP_CONFIG const &kContMapCfg_Mission )
{
	BM::CAutoMutex kLock( m_kMutex, true );

	CONT_DEFMAP const *pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef( pkContDefMap );

	short const nMyRealm = g_kProcessCfg.RealmNo();
	short const nMyChannel = g_kProcessCfg.ChannelNo();

	CONT_SERVER_HASH::const_iterator server_itr = kContServerHash.begin();
	for ( ; server_itr != kContServerHash.end() ; ++server_itr )
	{
		if (	server_itr->first.nRealm == nMyRealm
			&&	server_itr->first.nChannel == nMyChannel
			) 
		{
			SServerBalance* pkBalance = new_tr SServerBalance;
			if ( pkBalance )
			{
				pkBalance->Set(server_itr->first);
				m_kContServerBalance.push_back(pkBalance);
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Memory New Error!!!") );
			}	
		}
	}

	// ёКј­№ц Config їЎј­ ServerIdentity ·О ёёµйѕо іхґВґЩ.
	CONT_MAP_CONFIG::const_iterator map_cfg_itr = kContMapCfg_Static.begin();
	for ( ; map_cfg_itr != kContMapCfg_Static.end(); ++map_cfg_itr )
	{
		if (	map_cfg_itr->nRealmNo == nMyRealm 
			&&	map_cfg_itr->nChannelNo == nMyChannel)
		{
			CONT_DEFMAP::const_iterator defmap_itr = pkContDefMap->find( map_cfg_itr->iGroundNo );
			if ( defmap_itr != pkContDefMap->end() )
			{
				switch ( defmap_itr->second.iAttr )
				{
				case GATTR_INSTANCE:
				case GATTR_PVP:
				case GATTR_BOSS:
				case GATTR_EMPORIABATTLE:
				case GATTR_EMPORIA:
				case GATTR_MYHOME:
				case GATTR_HOMETOWN:
				case GATTR_HARDCORE_DUNGEON:
				case GATTR_HARDCORE_DUNGEON_BOSS:
				case GATTR_EXPEDITION_GROUND:
				case GATTR_EXPEDITION_GROUND_BOSS:
				case GATTR_CONSTELLATION_GROUND:
				case GATTR_CONSTELLATION_BOSS:
				case GATTR_CART_MISSION_GROUND:
					{
						SERVER_IDENTITY kSI;
						kSI.nRealm = map_cfg_itr->nRealmNo;
						kSI.nChannel = map_cfg_itr->nChannelNo;
						kSI.nServerNo = map_cfg_itr->nServerNo;
						kSI.nServerType = CEL::ST_MAP;//ёКАє °­Б¦

						std::pair<CONT_GROUND2SERVER_BALANCE::iterator, bool> ret = m_kContGround2ServerBalance.insert( std::make_pair(map_cfg_itr->iGroundNo, CONT_GROUND2SERVER_BALANCE::mapped_type()));

						SServerBalance* pkBalance = GetServerBalance(kSI);
						if ( pkBalance )
						{
							if( !(ret.first->second.insert(pkBalance).second) )//Set їЎ іЦАє°б°ъ
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GroundNo[") << map_cfg_itr->iGroundNo << _T("]'s Server SameKey Used {Realm[") << kSI.nRealm << _T("] Channel[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("]") );
							}
						}
						else
						{
							INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Found Ground[") << map_cfg_itr->iGroundNo << _T("]'s Server{Realm[") << kSI.nRealm << _T("] Channel[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("]}") );
						}
					}break;
				case GATTR_MISSION:
				case GATTR_CHAOS_MISSION:
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("MissionGround Regist at MissionNo!! plz.. GroundNo[") << map_cfg_itr->iGroundNo << _T("] {Realm[") << map_cfg_itr->nRealmNo << _T("] Channel[") << map_cfg_itr->nChannelNo << _T("] ServerNo[") << map_cfg_itr->nServerNo << _T("]}") );
					}break;
				case GATTR_DEFAULT:
				case GATTR_VILLAGE:
				case GATTR_HIDDEN_F:
				case GATTR_CHAOS_F:
				case GATTR_STATIC_DUNGEON:
				case GATTR_MARRY:
				case GATTR_BATTLESQUARE:
				case GATTR_SUPER_GROUND:
				case GATTR_ELEMENT_GROUND:
					{

					}break;
				default:
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unknown Ground[") << map_cfg_itr->iGroundNo << _T("] Attr[") << defmap_itr->second.iAttr << _T("] {Realm[") << map_cfg_itr->nRealmNo << _T("] Channel[") << map_cfg_itr->nChannelNo << _T("] ServerNo[") << map_cfg_itr->nServerNo << _T("]}") );
					}break;
				}	
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GroundNo[") << map_cfg_itr->iGroundNo << _T("] Is Bad {Realm[") << map_cfg_itr->nRealmNo << _T("] Channel[") << map_cfg_itr->nChannelNo << _T("] ServerNo[") << map_cfg_itr->nServerNo << _T("]}") );
			}
		}
	}

	// №МјЗёК
	map_cfg_itr = kContMapCfg_Mission.begin();
	for ( ; map_cfg_itr != kContMapCfg_Mission.end(); ++map_cfg_itr )
	{
		if (	map_cfg_itr->nRealmNo == nMyRealm 
			&&	map_cfg_itr->nChannelNo == nMyChannel)
		{
			std::pair<CONT_GROUND2SERVER_BALANCE::iterator, bool> ret = m_kContGround2ServerBalance.insert( std::make_pair(map_cfg_itr->iGroundNo, CONT_GROUND2SERVER_BALANCE::mapped_type()));

			SERVER_IDENTITY kSI;
			kSI.nRealm = map_cfg_itr->nRealmNo;
			kSI.nChannel = map_cfg_itr->nChannelNo;
			kSI.nServerNo = map_cfg_itr->nServerNo;
			kSI.nServerType = CEL::ST_MAP;//ёКАє °­Б¦

			SServerBalance* pkBalance = GetServerBalance(kSI);
			if ( pkBalance )
			{
				if(!(*ret.first).second.insert(pkBalance).second)//Set їЎ іЦАє°б°ъ
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("MissionNo[") << map_cfg_itr->iGroundNo << _T("]'s Server SameKey Used {Realm[") << kSI.nRealm << _T("] Channel[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("]}") );
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Found MissionNo[") << map_cfg_itr->iGroundNo << _T("]'s Server{Realm[") << kSI.nRealm << _T("] Channel[") << kSI.nChannel << _T("] ServerNo[") << kSI.nServerNo << _T("]}") );
			}
		}
	}
}

void PgServerSetMgr::Locked_ShutDown(void)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_bShutDownServer = true;

	CONT_SERVER_HASH kCont;
	g_kProcessCfg.Locked_GetServerInfo( CEL::ST_MAP, kCont );

	CONT_SERVER_HASH::const_iterator server_itr = kCont.begin();
	for ( ; server_itr != kCont.end() ; ++server_itr )
	{
		SERVER_IDENTITY const &kSI = server_itr->first;
		if(		g_kProcessCfg.ServerIdentity().nRealm == kSI.nRealm
			&&	g_kProcessCfg.ServerIdentity().nChannel == kSI.nChannel
			)
		{
			BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_SERVER_COMMAND);
			kPacket.Push(MCC_Shutdown_Service);
			kSI.WriteToPacket(kPacket);

			g_kProcessCfg.Locked_SendToServer( kSI, kPacket );
		}
	}
}

void PgServerSetMgr::Locked_CheckShutDown(void)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if ( true == m_bShutDownServer )
	{
		size_t const iRemainUserCount = m_kContPlayer_MemberKey.size();
		if ( iRemainUserCount > 0 )
		{
			INFO_LOG( BM::LOG_LV5, L"Server Terminate Wait Remain User Count : " << iRemainUserCount );
		}
		else
		{
			INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
			INFO_LOG( BM::LOG_LV6, _T("[CenterServer] will be shutdown") );
			INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING...") );
			INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
			g_kConsoleCommander.StopSignal(true);
			INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
		}
	}
}

HRESULT PgServerSetMgr::Locked_OnGreetingServer(SERVER_IDENTITY const &kRecvSI, CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if ( kRecvSI.nServerType == CEL::ST_MAP )
	{
		SServerBalance* pkBalance = GetServerBalance(kRecvSI);
		if ( pkBalance )
		{
			pkBalance->Live(true);
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Not Found Realm[") << kRecvSI.nRealm << _T("] Channel[") << kRecvSI.nChannel << _T("] ServerNo[") << kRecvSI.nServerNo << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT PgServerSetMgr::GroundLoadBalance( SGroundKey const &kKey, SERVER_IDENTITY &kOutSI )const
{
	if ( IsAliveGround(kKey) )
	{
		// АМ№М БёАзЗПґВ ±Ч¶уїоµеґЩ.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
		return S_FALSE;
	}

	CONT_GROUND2SERVER_BALANCE::const_iterator gnd_svr_itr = m_kContGround2ServerBalance.find(kKey.GroundNo());
	SServerBalance* pServerBalance = NULL;

	if( gnd_svr_itr != m_kContGround2ServerBalance.end())
	{
		CONT_GROUND2SERVER_BALANCE::mapped_type const &kElement = gnd_svr_itr->second;

		CONT_GROUND2SERVER_BALANCE::mapped_type::const_iterator candi_itor = kElement.begin();
		while(candi_itor != kElement.end())
		{
			if ( !pServerBalance || (pServerBalance->Point() > (*candi_itor)->Point()) )
			{
				pServerBalance = (*candi_itor);
			}
			++candi_itor;
		}

		if( pServerBalance )
		{
			pServerBalance->Get(kOutSI);
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgServerSetMgr::Locked_GroundLoadBalance( SGroundKey const &kKey, SERVER_IDENTITY &kOutSI )const
{//ѕоµрїЎґЩ ёёµйѕоѕЯ ЗПґВБц.
	BM::CAutoMutex kLock( m_kMutex, false );
	return GroundLoadBalance( kKey, kOutSI );
}

bool PgServerSetMgr::Locked_IsPlayer(BM::GUID const &rkGuid, bool const bIsMemberGuid)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return IsPlayer(rkGuid, bIsMemberGuid);
}

bool PgServerSetMgr::IsPlayer(BM::GUID const &rkGuid, bool const bIsMemberGuid)const
{
	if( bIsMemberGuid )
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_MemberKey.find(rkGuid);
		return m_kContPlayer_MemberKey.end() != iter;
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(rkGuid);
		return m_kContPlayer_CharKey.end() != iter;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return false;
}

HRESULT PgServerSetMgr::Locked_GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetPlayerInfo(rkGuid, bIsMemberGuid, rkOut);
}

HRESULT PgServerSetMgr::GetPlayerInfo(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut) const
{
	if( bIsMemberGuid )
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_MemberKey.find(rkGuid);
		if( m_kContPlayer_MemberKey.end() != iter )
		{
			CONT_CENTER_PLAYER_BY_KEY::mapped_type const pkElement = (*iter).second;
			if( pkElement)
			{
				pkElement->CopyTo(rkOut);
				return S_OK;
			}
		}
	}
	else
	{
		CONT_CENTER_PLAYER_BY_KEY::const_iterator iter = m_kContPlayer_CharKey.find(rkGuid);
		if( m_kContPlayer_CharKey.end() != iter )
		{
			CONT_CENTER_PLAYER_BY_KEY::mapped_type const pkElement = (*iter).second;
			if( pkElement)
			{
				pkElement->CopyTo(rkOut);
				return S_OK;
			}
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgServerSetMgr::Locked_GetPlayerInfo(std::wstring const &rkCharName, SContentsUser &rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_CENTER_PLAYER_BY_ID::const_iterator iter = m_kContPlayer_CharName.find( rkCharName );
	if( m_kContPlayer_CharName.end() != iter)
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type const pkElement = (*iter).second;
		if( pkElement)
		{
			pkElement->CopyTo(rkOut);
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

/*
HRESULT PgServerSetMgr::Locked_GetPlayerInfo_OtherChannel(std::wstring const &rkCharName, SContentsUser &rkOut) const
{
	BM::CAutoMutex kLock(m_kMutex);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}
*/

/*
void PgServerSetMgr::Locked_WriteToPacket_User(BM::Stream& rkPacket, EServerSetMgrPacketOp const eOp)const
{
}
*/

size_t PgServerSetMgr::Locked_CurrentPlayerCount()const
{
	BM::CAutoMutex kLock(m_kMutex);
	int iHomeSize = 0;
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itor_pl = m_kContPlayer_MemberKey.begin();
	while (m_kContPlayer_MemberKey.end() != itor_pl)
	{
		if (NULL != (*itor_pl).second )
		{
			EUnitType const eType = (*itor_pl).second->UnitType();
			if (eType == UT_MYHOME)
			{
				++iHomeSize;	
			}
		}
		++itor_pl;
	}
	return m_kContPlayer_MemberKey.size() - iHomeSize;
}

/*
void PgServerSetMgr::Locked_RecvRealmUserMgrPacket(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
}
*/

bool PgServerSetMgr::Locked_GetRealmUserByCharGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetRealmUserByCharGuid(rkCharGuid, rkRealmInfo);
}

bool PgServerSetMgr::GetRealmUserByCharGuid(BM::GUID const &rkCharGuid, SRealmUserInfo& rkRealmInfo)const
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_CharKey.find( rkCharGuid );
	if ( itr != m_kContPlayer_CharKey.end() )
	{
		PgPlayer *pkPlayer = itr->second;
		rkRealmInfo.Set( *pkPlayer );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_GetRealmUserByMemGuid(BM::GUID const &rkMemberGuid, SRealmUserInfo& rkRealmInfo)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetRealmUserByMemGuid( rkMemberGuid, rkRealmInfo);
}

bool PgServerSetMgr::GetRealmUserByMemGuid(BM::GUID const &rkMemberGuid, SRealmUserInfo& rkRealmInfo)const
{
	CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContPlayer_MemberKey.find( rkMemberGuid );
	if ( itr != m_kContPlayer_MemberKey.end() )
	{
		PgPlayer *pkPlayer = itr->second;
		rkRealmInfo.Set( *pkPlayer );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_GetRealmUserByNickname(std::wstring const& wstrNickname, SRealmUserInfo& rkRealmInfo)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CENTER_PLAYER_BY_ID::const_iterator itr = m_kContPlayer_CharName.find( wstrNickname );
	if ( itr != m_kContPlayer_CharName.end() )
	{
		PgPlayer *pkPlayer = itr->second;
		rkRealmInfo.Set( *pkPlayer );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgServerSetMgr::Locked_Recv_PT_N_T_NFY_SELECT_CHARACTER( BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex, true );

	SReqMapMove_MT kRMM;
	pkPacket->Pop(kRMM);

	UNIT_PTR_ARRAY kUnitArray;
	EWRITETYPE const eWriteType = kUnitArray.ReadFromPacket( *pkPacket );

	CONT_PET_MAPMOVE_DATA kContPetMapMoveData;
	PU::TLoadTable_AM( *pkPacket, kContPetMapMoveData );

	CONT_PLAYER_MODIFY_ORDER kContModifyItemOrder;
	kContModifyItemOrder.ReadFromPacket( *pkPacket );

	UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
	for ( ; unit_itr!=kUnitArray.end() ; ++unit_itr )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
		if ( pkPlayer )
		{
			CONT_PET_MAPMOVE_DATA::mapped_type const *pkPetMapMoveData = NULL;
			CONT_PET_MAPMOVE_DATA::const_iterator pet_itr = kContPetMapMoveData.find( pkPlayer->SelectedPetID() );
			if ( pet_itr != kContPetMapMoveData.end() )
			{
				pkPetMapMoveData = &(pet_itr->second);
			}

			if ( true == Recv_PT_N_T_NFY_SELECT_CHARACTER( pkPlayer, kRMM, eWriteType, pkPetMapMoveData, kContModifyItemOrder ) )
			{
				// Г№№шВ° ірЗСЕЧёё kContModifyItemOrderё¦ єёі»°н іЄёУБцЗСЕЧґВ єёі»Бц ё»ѕЖѕЯ ЗСґЩ.
				kContModifyItemOrder.clear(); // ±Ч·ЇґП±о АМДЪµеґВ ёВґЩ
				unit_itr->bAutoRemove = false;// AutoRemoveѕИµЗ°ФЗШѕЯЗСґЩ.
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unit[") << unit_itr->pkUnit->Name().c_str() << _T("-") << unit_itr->pkUnit->GetID().str().c_str() << _T("] Not PgPlayer") );
		}
	}
}

bool PgServerSetMgr::InsertPlayer(PgPlayer *pkPlayer)
{
	std::pair<CONT_CENTER_PLAYER_BY_KEY::iterator, bool> kGuidRet = m_kContPlayer_MemberKey.insert( std::make_pair(pkPlayer->GetMemberGUID(), pkPlayer) );
    if ( kGuidRet.second )
	{
		kGuidRet = m_kContPlayer_CharKey.insert( std::make_pair(pkPlayer->GetID(), pkPlayer) );
		if ( kGuidRet.second )
		{
			std::pair<CONT_CENTER_PLAYER_BY_ID::iterator, bool> kStringRet = m_kContPlayer_MemberID.insert( std::make_pair(pkPlayer->MemberID(), pkPlayer) );
            if ( kStringRet.second )
			{
				kStringRet = m_kContPlayer_CharName.insert(std::make_pair(pkPlayer->Name(), pkPlayer));
				return kStringRet.second;
			}
		}
		RemovePlayer( pkPlayer );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::RemovePlayer(PgPlayer *pkPlayer)
{
	size_t iCount = m_kContPlayer_MemberKey.erase(pkPlayer->GetMemberGUID());
	iCount += m_kContPlayer_CharKey.erase(pkPlayer->GetID());
	iCount += m_kContPlayer_MemberID.erase(pkPlayer->MemberID());
	iCount += m_kContPlayer_CharName.erase(pkPlayer->Name());

	RemoveMapMoveLock(pkPlayer->GetID());
	return iCount == 4;
}

bool PgServerSetMgr::Recv_PT_N_T_NFY_SELECT_CHARACTER(PgPlayer *pkPlayer, SReqMapMove_MT & rkRMM, EWRITETYPE const eWriteType, CONT_PET_MAPMOVE_DATA::mapped_type const * const pkPetMapMoveData, CONT_PLAYER_MODIFY_ORDER &kContModifyItemOrder )
{
	if (pkPlayer == NULL)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("player is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPlaer is NULL"));
		return false;
	}

	if ( true == m_bShutDownServer )
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("ShutDownServer Error [") << pkPlayer->Name() << _T("-") << pkPlayer->GetID() << _T("]") );
		return false;
	}

	RemoveMapMoveLock(pkPlayer->GetID());
	
	if( !InsertPlayer(pkPlayer) )
	{
		//VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[%s] Insert Error [%s-%s]"), __FUNCTIONW__, pkNew->Name().c_str(), pkNew->GetID().str().c_str() );
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Insert Error [") << pkPlayer->Name().c_str() << _T("-") << pkPlayer->GetID().str().c_str() << _T("]") );
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Insert Error [") << pkPlayer->Name().c_str() << _T("-") << pkPlayer->GetID().str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("InsertPlayer is Failed!"));
		return false;//ЅЗЖР.
	}

	if ( pkPlayer->HaveParty() )
	{
		if ( g_kProcessCfg.IsPublicChannel() )
		{
			BM::Stream kPartyPacket( PT_T_T_SYNC_USER_PARTY, pkPlayer->GetID() );
			kPartyPacket.Push( pkPlayer->GetChannel() );
			::SendToGlobalPartyMgr( kPartyPacket );
		}
// 		else
// 		{
// 			INFO_LOG( BM::LOG_LV5, _T("[Recv_SELECT_CHARACTER] Player<") << pkPlayer->Name() << _T("/") << pkPlayer->GetID() << _T("> Have PartyGuid<") << pkPlayer->PartyGuid() << _T(">") );
// 			pkPlayer->PartyGuid( BM::GUID::NullData() );
// 		}
	}

	UNIT_PTR_ARRAY kUnitArray;
	kUnitArray.Add( pkPlayer );
	kUnitArray.SetWriteType(eWriteType);

	CONT_PET_MAPMOVE_DATA kContPetMapMoveData;
	if ( pkPetMapMoveData )
	{
		kContPetMapMoveData.insert( std::make_pair( pkPlayer->SelectedPetID(), *pkPetMapMoveData ) );
	}

	Constellation::SConstellationMission EmptyConstellationMission;
	CONT_UNIT_SUMMONED_MAPMOVE_DATA kEmtpyUserSummoned;
	if ( !ReqMapMove( kUnitArray, rkRMM, EmptyConstellationMission, kContPetMapMoveData, kEmtpyUserSummoned, kContModifyItemOrder ) )
	{
		BM::Stream kNPacket( PT_T_N_NFY_SELECT_CHARACTER_FAILED, pkPlayer->GetID() );
		SendToContents( kNPacket );

		BM::Stream kPacket( PT_T_C_ANS_SELECT_CHARACTER, E_SCR_LASTVILLAGE_ERROR );
		kPacket.Push( rkRMM.kTargetKey.GroundNo() );
		SendToUser( pkPlayer->GetMemberGUID(), kPacket );

		RemovePlayer(pkPlayer);
		return false;
	}
	return true;
}

bool PgServerSetMgr::Locked_Recv_PT_N_T_RES_MAP_MOVE( BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex, true);
	return RecvMapMove(pkPacket, true);
}

void PgServerSetMgr::Recv_PT_M_T_ANS_MAP_MOVE_TARGET_FAILED( BM::Stream * const pkPacket )const
{
	BM::GUID kReqCharGuid;
	BM::GUID kTargetCharGuid;
//	bool bGMCommand = false;
//	T_GNDATTR kGndAttr = GATTR_DEFAULT;
	pkPacket->Pop( kReqCharGuid );
	pkPacket->Pop( kTargetCharGuid );
//	pkPacket->Pop( bGMCommand );
//	pkPacket->Pop( kGndAttr );

	BM::Stream kAnsPacket( PT_T_C_ANS_MAP_MOVE_TARGET_FAILED, *pkPacket );

	SRealmUserInfo kRealmUserInfo;
	if ( true == Locked_GetRealmUserByCharGuid( kTargetCharGuid, kRealmUserInfo ) )
	{
		kAnsPacket.Push( true );
		kAnsPacket.Push( kRealmUserInfo.wName );
	}
	else
	{
		kAnsPacket.Push( false );
	}

	Locked_SendToUser( kReqCharGuid, kAnsPacket, false );
}

bool PgServerSetMgr::InsertMapMoveLock(BM::GUID const& rkMoveGuid, BM::GUID const& rkCharacterGuid)
{
	std::pair<CONT_PLAYER_MAPMOVE_LOCK::iterator, bool> ibRet = m_kPlayerMapMoveLock.insert(std::make_pair(rkCharacterGuid, rkMoveGuid));
    return ibRet.second;
}

void PgServerSetMgr::RemoveMapMoveLock(BM::GUID const& rkCharacterGuid)
{
	m_kPlayerMapMoveLock.erase(rkCharacterGuid);
}

bool PgServerSetMgr::GetMapMoveLock(BM::GUID const& rkCharacterGuid, BM::GUID& rkOutLock)
{
	CONT_PLAYER_MAPMOVE_LOCK::const_iterator itor_lock = m_kPlayerMapMoveLock.find(rkCharacterGuid);
	if (itor_lock != m_kPlayerMapMoveLock.end())
	{
		rkOutLock = itor_lock->second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_Recv_PT_I_M_REQ_HOME_CREATE(BM::Stream * pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	SReqMapMove_MT kRMM;
	pkPacket->Pop(kRMM);
	PgMyHome kMyHome;
	kMyHome.ReadFromPacket(*pkPacket);

	PgMyHome * pkMyHome = dynamic_cast<PgMyHome*>(g_kTotalObjMgr.CreateUnit(UT_MYHOME, kMyHome.GetID() ) );
	if ( pkMyHome )
	{
		BM::Stream kPacket;
		kMyHome.WriteToPacket(kPacket);
		pkMyHome->ReadFromPacket(kPacket);
		if(true == InsertPlayer(pkMyHome))
		{
			BM::Stream kPacket(PT_I_M_REQ_HOME_CREATE);
			kMyHome.WriteToPacket(kPacket);
			return SendToGround(kRMM.kTargetKey,kPacket,true);
		}
		g_kTotalObjMgr.ReleaseUnit( dynamic_cast<CUnit*>(pkMyHome) );
	}

	return false;
}

bool PgServerSetMgr::Locked_Recv_PT_I_M_REQ_HOME_DELETE( BM::Stream * const pkPacket  )
{
	BM::CAutoMutex kLock(m_kMutex, true);

	BM::GUID kMemberGuid;
	pkPacket->Pop( kMemberGuid );

	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr = m_kContPlayer_MemberKey.find( kMemberGuid );
	if ( login_itr != m_kContPlayer_MemberKey.end() )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCData = login_itr->second;
		if ( pCData )
		{
			RemovePlayer(pCData);
			g_kTotalObjMgr.ReleaseUnit(pCData);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgServerSetMgr::Locked_Recv_PT_M_M_UPDATE_PLAYERPLAYTIME( BM::Stream * const pkPacket )
{
	SGroundKey kGroundKey;
	BM::GUID kCharGuid;
	int iAccConSec = 0;
	int iAccDicSec = 0;
	__int64 i64SelectSec = 0;

	pkPacket->Pop(kGroundKey);
	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(iAccConSec);
	pkPacket->Pop(iAccDicSec);
	pkPacket->Pop(i64SelectSec);


	BM::Stream kPacket(PT_M_M_UPDATE_PLAYERPLAYTIME);
	kPacket.Push(kGroundKey);
	kPacket.Push(kCharGuid);
	kPacket.Push(iAccConSec);
	kPacket.Push(iAccDicSec);
	kPacket.Push(i64SelectSec);
	SendToServerType(CEL::ST_MAP, kPacket);


	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_CENTER_PLAYER_BY_KEY::iterator login_itr = m_kContPlayer_CharKey.find( kCharGuid );
	if ( login_itr != m_kContPlayer_CharKey.end() )
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pCData = login_itr->second;
		if ( pCData )
		{
			pCData->SetPlayTime(iAccConSec,iAccDicSec);
			pCData->SetSelectCharacterSec(i64SelectSec);
			pCData->SetLasSpecificRewardSec(i64SelectSec);
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
