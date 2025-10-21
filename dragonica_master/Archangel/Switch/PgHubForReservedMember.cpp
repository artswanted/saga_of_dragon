#include "StdAfx.h"
#include "Variant/Global.h"
#include "PgHub.h"

namespace PgHubUtil
{
	PgReserveMemberData* CreateReservedMemberData( bool const bIsGMUser, AHNHS_SERVER_HANDLE kHackShieldHandle )
	{
		switch( g_kHub.AntiHackType() )
		{
		case ANTIHACK_DUMMY:
			{
				return new_tr PgReserveMemberData_Dummy;
			}break;
		case ANTIHACK_AHN_HACKSHIELD:
			{
				PgReserveMemberData_AhnHackShield *pData = new_tr PgReserveMemberData_AhnHackShield;
				if ( pData )
				{
					if(	!bIsGMUser
					&&	(ANTICPX_INVALID_HANDLE_VALUE != kHackShieldHandle) )
					{
						AHNHS_CLIENT_HANDLE kHandle = ::_AhnHS_CreateClientObject( kHackShieldHandle );
						if ( ANTICPX_INVALID_HANDLE_VALUE != kHandle )
						{
							pData->SetHandle( kHandle );	
						}
						else
						{
							SAFE_DELETE(pData);
							return NULL;
						}
					}
					else
					{
						pData->m_dwLastAntiHackCheckTime = ULONG_MAX;
					}

					return dynamic_cast<PgReserveMemberData*>(pData);
				}
			}break;
		case ANTIHACK_GAMEGUARD:
			{
				return new_tr PgReserveMemberData_GameGuard;
			}break;
		default:
			{
				return new_tr PgReserveMemberData;
			}break;
		}
		return NULL;
	}
}


bool PgHub::Locked_ProcessReserveMember(SReqSwitchReserveMember const &rkRSRM)
{
	BM::CAutoMutex kLock(m_kMutex, true);

	ESwitchReservePlayerResult eRet = SRPR_FAILED;
	PgReserveMemberData *pkElement = NULL;

	CONT_SWITCH_PLAYER::iterator main_itor = m_kContMemberSession.find( rkRSRM.guidMember );//메인에 할당되어 있으면 안됨.
	if( m_kContMemberSession.end() != main_itor )
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Can't duplicate reserve member[G:"<<rkRSRM.guidMember<<L", ID:") << rkRSRM.ID() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}
	else
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Try to reserve Switch member[G:"<<rkRSRM.guidMember<<L", ID:") << rkRSRM.ID() << _T("]") );

		pkElement = PgHubUtil::CreateReservedMemberData( (rkRSRM.byGMLevel > 0), m_kHackShieldHandle );
		if( !pkElement )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV6, __FL__ << _T(" Cannot Make Element") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;
		}

#ifdef _DEBUGGING
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Before Sleep(1m)") );
		Sleep(20 * 1000);
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" After Sleep(1m)") );
#endif
		auto insert_ret = m_kReserveMember.insert( std::make_pair(rkRSRM.guidOrderKey, pkElement) );
		if( true == insert_ret.second )
		{
			eRet = SRPR_SUCEESS;
			*pkElement = rkRSRM; // ServerTryLoginData 저장 해둠
			pkElement->kSwitchServer = g_kProcessCfg.ServerIdentity(); // 내 번호 보냄.
			pkElement->dwTimeLimit	 = BM::GetTime32();
		}
		else
		{
			INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Cannot INSERT to RerserveMember list, Alread exsit ID[") << rkRSRM.ID() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			SAFE_DELETE(pkElement); // delete
		}
	}


	INFO_LOG( BM::LOG_LV7, __FL__ << _T(" SendToCenter Reserve_Member result [") << eRet << _T("]") );

	BM::Stream kPacket(PT_S_T_ANS_RESERVE_MEMBER);
	kPacket.Push( eRet );
	kPacket.Push( rkRSRM.guidMember );			//멤버GUID
	kPacket.Push( rkRSRM.guidOrderKey );		//리저브 키
	m_kAcceptorAddr.WriteToPacket(kPacket);		//여기가 스위치 주소.
	if( pkElement )
	{
		pkElement->WriteToPacket( kPacket );
	}
	::SendToCenter( kPacket, false );

	return true;
}

bool PgHub::TryAccessSwitch( CEL::CSession_Base *pkSession, STryAccessSwitch const &rkTAS )
{
	ETryLoginResult eRet = E_TLR_NONE;
	
	if( true == CProcessConfig::IsCorrectVersion( rkTAS.szVersion1, true ) )
	{
		if ( true == TryAccessSwitch2( pkSession, rkTAS.guidSwitchKey, NULL ) )
		{
			pkSession->m_kSessionData.Push( rkTAS.guidSwitchKey );
			return true;
		}

		eRet = E_TLR_NOR_RESERVED_USER;
		HACKING_LOG( BM::LOG_LV5, _T("[TryAccessSwitch] ID<") << rkTAS.ID() << _T("> IP<") << pkSession->Addr().ToString() << _T("> No Reserved") );
	}
	else
	{
		eRet = E_TLR_WRONG_VERSION;
		HACKING_LOG( BM::LOG_LV5, _T("[TryAccessSwitch] ID<") << rkTAS.ID() << _T("> IP<") << pkSession->Addr().ToString() << _T("> Wrong Version<") << rkTAS.szVersion1 << _T(">") );
	}

	BM::Stream kUserPacket(PT_S_C_TRY_ACCESS_SWITCH_RESULT);
	kUserPacket.Push(eRet);
	kUserPacket.SetStopSignal(true);
 	pkSession->VSend(kUserPacket);//끊어야됨
 
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

void PgHub::Locked_OnTimer_ClearTimeOverMember()
{
	// **** 주의 :: WRITE LOCK 사용
	BM::CAutoMutex kLock(m_kMutex, true);

	DWORD const dwNow = BM::GetTime32();

	// 한번에 모두 돌지 않도록
	CONT_SWITCH_RESERVER::iterator iter = m_kReserveMember.begin();
	while( m_kReserveMember.end() != iter )
	{
		CONT_SWITCH_RESERVER::mapped_type element = (*iter).second;

		DWORD const dwDiffTIme = ::DifftimeGetTime(element->dwTimeLimit, dwNow);
		if( RESERVEMEMBER_TIMEOUT < dwDiffTIme ) // 대기 시간 지남
		{
			if( true == element->m_bWaitAntiHackCheck  )
			{
				// AntiHack 응답 대기 기준 시간을 넘겼다
				PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_CheckAntiHack_NoReply2, *element, NULL, (BM::vstring(L" GapTime[Base:") << RESERVEMEMBER_TIMEOUT << L" < CurTime:" << dwDiffTIme << L"]")); // 세션이 없다
			}

			SAFE_DELETE(element);
			iter = m_kReserveMember.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

// LOGIN 최종 결과
void PgHub::Locked_Recv_PT_T_S_ANS_LOGINED_PLAYER(HRESULT const hRecvRet, BM::GUID const& rkMemberGuid, BM::GUID const& rkSwitchKey)
{//	쎈터에게 패킷 오면 호출 하도록.

	// **** 주의 :: WRITE LOCK 사용
	BM::CAutoMutex kLock(m_kMutex, true);

	ETryLoginResult eRet = E_TLR_NOR_RESERVED_USER;

	BM::Stream kUserPacket(PT_S_C_TRY_ACCESS_SWITCH_RESULT);

	CONT_SWITCH_PLAYER::iterator reserve_itor = m_kReserveMember.find(rkSwitchKey);
	if(	(m_kReserveMember.end() != reserve_itor) 
	&&	(reserve_itor->second != NULL))
	{
		switch( hRecvRet )
		{
		case S_OK:
			{
				eRet = E_TLR_SUCCESS;
			}break;
		default:
			{//실패일경우 끊음.
				eRet = E_TLR_ALREADY_LOGINED;
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}break;
		}
	}
	else
	{
		eRet = E_TLR_NOR_RESERVED_USER; // 예약이 안됐어.
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}

	kUserPacket.Push(eRet);
	if( E_TLR_SUCCESS == eRet )
	{
		PgReserveMemberData* pkReserveData = reserve_itor->second;

		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Member[") << pkReserveData->ID() << _T("] registered as Switch User") );

		auto insert_ret = m_kContMemberSession.insert(std::make_pair( pkReserveData->guidMember, pkReserveData) );
		if( true == insert_ret.second )
		{// 접속자에 들어갔으므로 예약대기자에서는 제거
			m_kReserveMember.erase(reserve_itor);//이동 되었으므로 SAFE_DELETE(itor->second); 를 하진 않는다.
			SendNfyPlayerCount();
		}
		else
		{
			eRet = E_TLR_NOR_RESERVED_USER;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}

		if( PgHubUtil::IsCanTwiceCheckRegion() )
		{
			// AntiHack 서버연동 활성화 시 한번더 확인하자
			if( (true == PgHub::ms_bUseAntiHackServerBind) )
			{
				BM::Stream kAHPacket(PT_S_C_REQ_GAME_GUARD_CHECK);
				if( PgHubUtil::MakeCheckAntiHackQuery( kAHPacket, pkReserveData, BM::GetTime32(), __FUNCTIONW__, __LINE__ ) ) // 
				{
					SendPacketToUser(rkMemberGuid, kAHPacket );
				}
			}
		}
		pkReserveData->DoSendPing(0);//첫 핑을 보낸다.
	}
	else
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T(" Auth Failed(Center::AddSwitchUser) hRecvRet : "<<(int)hRecvRet<<L" Ret : ")<<(int)eRet<<L" GUID : "<<rkMemberGuid.str());
		kUserPacket.SetStopSignal(true);

		BM::Stream kServerPacket(PT_S_T_TRY_ACCESS_SWITCH_RESULT, rkMemberGuid );
		g_kProcessCfg.ServerIdentity().WriteToPacket( kServerPacket );
		SendToCenter( kServerPacket, false );
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}

	SendPacketToUser(rkMemberGuid, kUserPacket);
}

void PgHub::SendNfyPlayerCount()const
{//protected:
	BM::Stream kNfyPacket(PT_S_T_NFY_USER_CONNECT_SWITCH);//유저 들어왔다고 알려줌.
	g_kProcessCfg.ServerIdentity().WriteToPacket(kNfyPacket);
	kNfyPacket.Push(m_kContMemberSession.size());
	SendToCenter( kNfyPacket, false );
}
