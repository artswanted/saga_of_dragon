#include "StdAfx.h"
#include "GameGuard/ggsrv25.h"

#ifdef HACKSHIELD_AUTO	//�߱�/�Ϻ���
#include "HackShield_AUTO/AntiCpXSvr.h"
#else	//�⺻ ����
#include "HackShield_UNAUTO/AntiCpXSvr.h"
#endif


#include "Variant/Global.h"
#include "helldart/PgProtocolFilter.h"
#include "PgHub.h"
#include "PgReloadableDataMgr.h"

#ifdef GG_ANTIHACK
#pragma comment(lib, "ggsrvlib25_win32_MT.lib")
#else
#include "GameGuard/stub.inl"
#endif

#ifdef HACKSHIELD_AUTO	//�߱�/�Ϻ���
#pragma comment(lib, "HackShield_AUTO/AntiCpXSvr.lib")
#else	//�⺻ ����
#pragma comment(lib, "AntiCpXSvr.lib")
#endif
// AntiHack �� HellDart �Լ�
void CheckAntiHackPacket(bool& bRet, BM::Stream& kPacket)
{
	if( bRet )
	{
		switch( g_kHub.AntiHackType() )
		{
		case ANTIHACK_DUMMY:
			{
			}break;
		case ANTIHACK_GAMEGUARD:
			{
				GG_AUTH_DATA kAuthData;
				bRet = (bRet && kPacket.Pop( kAuthData ));
			}break;
		case ANTIHACK_AHN_HACKSHIELD:
			{
				AHNHS_TRANS_BUFFER kBuffer;
				bRet = (bRet && kPacket.Pop( kBuffer.nLength ));
				bRet = (bRet && kPacket.PopMemory( kBuffer.byBuffer, static_cast< size_t >(kBuffer.nLength) ));
			}break;
		}
	}
}

HRESULT CALLBACK Check_PT_C_S_TRY_ACCESS_SWITCH_SECOND(BM::Stream &kPacket)
{
	BM::GUID kTempGuid;
	unsigned long ulTemp = 0;

	bool bRet = kPacket.Pop( kTempGuid );
	bRet = (bRet && kPacket.Pop( ulTemp ));

	CheckAntiHackPacket(bRet, kPacket);

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CALLBACK Check_PT_C_S_ANS_GAME_GUARD_CHECK(BM::Stream &kPacket)
{
	unsigned long ulTemp = 0;
	bool bRet = kPacket.Pop( ulTemp );

	CheckAntiHackPacket(bRet, kPacket);

	if( bRet
	&&	0 == kPacket.RemainSize() )
	{
		return S_OK;
	}
	return E_FAIL;
}


//
namespace PgHubUtil
{
	typedef unsigned long MsgType;
	MsgType const iNoMessage = 0;
	MsgType const iBaseMsgType = 700145; // ������ ���� ������ �����ϴ�. / GM���� �������ֽñ� �ٶ��ϴ�.
	MsgType const iMaxMsgType = 700400; // iBaseMsgType + 255

	inline void DefaultSendToGround(TCHAR const* szFunc, size_t const iFuncLine, PgReserveMemberData const &rkRMD, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket, unsigned short usType, int const iValue = 0)
	{
#ifdef _GROUNDKEY_CHECK
		if (rkRMD.kClientGroundKey.GroundNo() == rkRMD.kGroundKey.GroundNo())
#endif
		{
			pkPacket->PosAdjust();
			SendToGround(rkRMD.guidCharacter, rkRMD.kSI, rkRMD.kGroundKey, *pkPacket);
		}
#ifdef _GROUNDKEY_CHECK
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			INFO_LOG( BM::LOG_LV8, __FL__ << _T(" Cannot forwarding packet["<<usType<<L", "<<iValue<<L"], User[Guid: ") << rkRMD.guidCharacter.str().c_str() << _T("] ClientGroundNo[") << rkRMD.kClientGroundKey.GroundNo() << _T("], ServerGroundNo[") << rkRMD.kGroundKey.GroundNo() << _T("]") );
		}
#endif
	}

	HRESULT UserDisconnectedLog(TCHAR const* szFunc, size_t const iFuncLine, int const iCause, PgReserveMemberData const& rkRMD, CEL::CSession_Base* pkSession, BM::vstring const& rkExtern, BYTE byReason)
	{
		MsgType iMsgNo = iNoMessage;
		HRESULT hReturn = S_OK;	// S_OK �̸� ������ Disconnect ��Ų��.
		switch( iCause )
		{
		case CDC_Normal:
		case CDC_ChangeChannel:
			{
			}break;
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_SpeedHack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_SuspectedSpeedHack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_LaziPing, rkRMD, iBaseMsgType + iCause, /*(true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL*/ S_OK, rkExtern)
//		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_ChangeChannel, rkRMD, 0, rkExtern)// ä���̵��� Ŭ���̾�Ʈ ���� �޼����� ������ �ʿ䰡 ����.
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_MapServerDown, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_MapServerClear, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_CenterRemoveUserByGnd, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_CharInfoErr, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_CharMapErr, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_NetworkErr, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_GM_CmdKick, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_GM_ToolKick, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_GM_CharEdit_Kick, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_GM_AccountBlock, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_GM_RPCKick, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_Invalid_Macro_Time, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_MoveSpeed_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_HyperMove_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_MaxTarget_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_Projectile_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_Blowup_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_ClientNotify_Hack, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_RandomSeedCallCounter, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckAntiHack, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // GameGuard / HackShield ���� �����ؼ� ������ ������ ���� ��Ų��
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckAntiHack_C, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // GameGuard / HackShield
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckAntiHack_NoReply, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // GameGuard / HackShield
		CASE_USER_DISCONNECT_CAUSE(szFunc, iFuncLine, CDC_TryLoginFromOther, rkRMD, iBaseMsgType + iCause, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_ActionCount_Hack_A, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_ActionCount_Hack_B, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_ActionCount_Hack_C, rkRMD, iBaseMsgType + iCause, (true == PgHub::ms_bDisconnectHackUser) ? S_OK : E_FAIL, rkExtern)
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckAntiHack_NoReply2, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // GameGuard / HackShield
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckHellDart, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // HellDart
		CASE_USER_DISCONNECT_HACK(szFunc, iFuncLine, CDC_CheckAntiHack_Format, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern) // GameGuard / HackShield ��Ŷ ���� Ʋ��
		CASE_USER_DISCONNECT_HACK_CAUSE(szFunc, iFuncLine, CDC_NotAuthSession, rkRMD, iBaseMsgType + iCause, S_OK, rkExtern)

		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				iMsgNo = iBaseMsgType + iCause;
				CAUTION_LOG( BM::LOG_LV1, __FL__ << _T(" general error disconnected user[Cause: ") << iCause << _T("] user[M-G: ") << rkRMD.guidMember.str().c_str()
					<< _T(", C-G: ") << rkRMD.guidCharacter.str().c_str() << _T(", A-N: ") << rkRMD.ID() << _T(", IP: ") << rkRMD.addrRemote.ToString().c_str() << _T("] force disconnected") );
			}break;
		}

		if( S_OK == hReturn && pkSession && iMsgNo )
		{
			BM::Stream kDPacket( PT_A_S_NFY_USER_DISCONNECT, iMsgNo );
			kDPacket.Push(byReason);
			pkSession->VSend(kDPacket);
		}
		return hReturn;
	}

	bool IsCanTwiceCheckRegion()
	{
		switch( g_kLocal.ServiceRegion() )
		{
		case LOCAL_MGR::NC_TAIWAN:
			{
				// ���(PC �Ǵ� ȸ��)�� ������ ������ 2�� üũ�� ���� �ʴ´�
				return false;
			}break;
		default:
			{
			}break;
		}
		return true;
	}

	bool MakeCheckAntiHackQuery(BM::Stream &rkOut, PgReserveMemberData* pkRMD, DWORD const dwTime, wchar_t const* szFunc, size_t const iLine)
	{
		if( !pkRMD )
		{
			CAUTION_LOG( BM::LOG_LV5, __FL2__(szFunc, iLine) << _T("PgReserveMemberData* pkRMD = NULL") );
			return false;
		}

		HRESULT const hRet = pkRMD->MakePacket_CheckAntiHack(rkOut);
		if( ERROR_SUCCESS != hRet )
		{
			if( !pkRMD->IsCheckAntiHack() )
			{
				return false;
			}
			CAUTION_LOG( BM::LOG_LV5, __FL2__(szFunc, iLine) << _T("Make Failed ID<") << pkRMD->ID() << _T("> ErrorCode=") << hRet );
		}

//		INFO_LOG( BM::LOG_LV5, __FL__ );

		//SendPacketToUser( pkRMD->guidMember, kAHPacket ); // �ܺο��� �����Ѵ�
		pkRMD->m_dwLastAntiHackCheckTime = dwTime;
		//pkRMD->m_dwNextAntiHackCheckTime = // Next�� �������� �ʴ´�
		pkRMD->m_bWaitAntiHackCheck = true;
		return true;
	}

	bool RecvAntiHackCheck( CEL::CSession_Base *pkSession, BM::Stream &kPacket, PgReserveMemberData* pkRMD, wchar_t const* szFunc, size_t const iLine )
	{
		unsigned long ulClientRet = ERROR_SUCCESS;
		if ( true == kPacket.Pop( ulClientRet ) )
		{
			if ( ERROR_SUCCESS == ulClientRet )
			{
				HRESULT const hRet = pkRMD->CheckAntiHack( kPacket );
				if( ERROR_SUCCESS == hRet )
				{
					DWORD const dwTime = BM::GetTime32();
					if( 0 == pkRMD->m_dwNextAntiHackCheckTime
					&&	IsCanTwiceCheckRegion() )
					{
						// ������ ù �����̸� �ι�° ������ ������ ������, ������ �ƴ� m_kReserveMember(�����)���� m_kContMemberSession(������)�� ���� �� ������
					}
					else
					{
						pkRMD->m_dwLastAntiHackCheckTime = dwTime; // �� �������� ���� �������� Ȯ��
						pkRMD->m_bWaitAntiHackCheck = false;
					}
					pkRMD->m_dwNextAntiHackCheckTime = static_cast<DWORD>( BM::Rand_Range( PgHub::ms_iAntiHackCheck_Min, PgHub::ms_iAntiHackCheck_Max ) );
					return true;
				}
				else
				{
					PgHubUtil::UserDisconnectedLog( szFunc, iLine, CDC_CheckAntiHack, *pkRMD, pkSession, BM::vstring(L" CheckAntiHack() Ret:")<<hRet);
					pkSession->VTerminate(); // Ŭ���̾�Ʈ ����
				}
			}
			else
			{
				PgHubUtil::UserDisconnectedLog( szFunc, iLine, CDC_CheckAntiHack_C, *pkRMD, pkSession, BM::vstring(L" Client Ret:")<<ulClientRet );
				pkSession->VTerminate(); // Ŭ���̾�Ʈ �������� ����(��⿡�� �Ǵ� ������ ���)
			}
		}
		else
		{
			PgHubUtil::UserDisconnectedLog( szFunc, iLine, CDC_CheckAntiHack_Format, *pkRMD, pkSession );
			pkSession->VTerminate(); //������ �ȸ������̹Ƿ� ������ ���� ����.
		}
		return false;
	}

	bool CheckAntiHackResponseTime(PgReserveMemberData& rkElement, CEL::CSession_Base* pkSession)
	{
		if( ANTIHACK_NONE == g_kHub.AntiHackType() )
		{
			return true;
		}

		DWORD const dwTime = BM::GetTime32();
		if( true == rkElement.m_bWaitAntiHackCheck )
		{
			DWORD const dwDiffTime = ::DifftimeGetTime(rkElement.m_dwLastAntiHackCheckTime, dwTime);
			if( dwDiffTime > PgHub::ms_iAntiHackCheck_Wait ) // ���� �ð��� ��������
			{
				// ���� ���� ����
				PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_CheckAntiHack_NoReply, rkElement, pkSession, BM::vstring(L" Latest:")<<rkElement.m_dwLastAntiHackCheckTime<<L" Now:"<<dwTime<<L" Diff:"<<dwDiffTime<<L" Limit:"<<PgHub::ms_iAntiHackCheck_Wait);
				if( PgHub::ms_bUseAntiHackDisconnect2nd )
				{
					pkSession->VTerminate();
					return false;
				}
			}
		}
		else
		{
			if( (true == PgHub::ms_bUseAntiHackServerBind)
			&&	(::DifftimeGetTime(rkElement.m_dwLastAntiHackCheckTime, dwTime) > rkElement.m_dwNextAntiHackCheckTime) )
			{
				BM::Stream kAHPacket(PT_S_C_REQ_GAME_GUARD_CHECK);
				if( PgHubUtil::MakeCheckAntiHackQuery(kAHPacket, &rkElement, dwTime, __FUNCTIONW__, __LINE__) )
				{
					pkSession->VSend(kAHPacket);
				}
			}
		}
		return true;
	}


};

////////////////////

int PgHub::ms_iAntiHackCheck_Min = 300000;
int PgHub::ms_iAntiHackCheck_Max = 900000;
int PgHub::ms_iAntiHackCheck_Wait = 60000;
bool PgHub::ms_bDisconnectHackUser = false;
bool PgHub::ms_bUseAntiHackServerBind = false;
bool PgHub::ms_bUseAntiHackDisconnect2nd = false;

PgHub::PgHub(void)
:	m_kAntiHackType(ANTIHACK_NONE)
,	m_kHackShieldHandle(ANTICPX_INVALID_HANDLE_VALUE)
{
}

PgHub::~PgHub(void)
{
	if ( ANTICPX_INVALID_HANDLE_VALUE != m_kHackShieldHandle )
	{
		::_AhnHS_CloseServerHandle( m_kHackShieldHandle );
		m_kHackShieldHandle = ANTICPX_INVALID_HANDLE_VALUE;
	}

	CONT_SWITCH_PLAYER::iterator member_itr = m_kContMemberSession.begin();
	for ( ; member_itr!=m_kContMemberSession.end() ; ++member_itr )
	{
		SAFE_DELETE(member_itr->second);
	}

	CONT_SWITCH_RESERVER::iterator reserve_itr = m_kReserveMember.begin();
	for ( ; reserve_itr!=m_kReserveMember.end() ; ++reserve_itr )
	{
		SAFE_DELETE(reserve_itr->second);
	}
}

HRESULT PgHub::InitAntiHack( E_ANTIHACK_TYPE const kAntiHackType )
{
	m_kAntiHackType = kAntiHackType;
	switch ( m_kAntiHackType )
	{
	case ANTIHACK_DUMMY:
		{
			INFO_LOG(BM::LOG_LV2, __FL__ << _T("Dummy AntiHack ServerBind Init Success !!!!!!!"));
		}break;
	case ANTIHACK_GAMEGUARD:
		{
			DWORD dwNumActive = 50;
			int iLogType = NPLOG_ERROR;
			if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
			{
				iLogType = NPLOG_DEBUG | NPLOG_ERROR;
				dwNumActive = 10;
			}

			DWORD const dwRet = InitGameguardAuth("./GameGuard/", dwNumActive, true, iLogType);
			if( ERROR_SUCCESS != dwRet )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("GamuGuard InitFail Ret : ") << dwRet);
				return E_FAIL;
			}

			if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
			{
				SetUpdateCondition(5, 50); // 5�е��� 50% �̻�
			}
			else
			{
				SetUpdateCondition(30, 50); // 30�е��� 50% �̻�
			}
			INFO_LOG(BM::LOG_LV2, __FL__ << _T("GamuGuard ServerBind Init Success !!!!!!!"));
		}break;
	case ANTIHACK_AHN_HACKSHIELD:
		{
			m_kHackShieldHandle = ::_AhnHS_CreateServerObject( "./HackShield/ShieldPack.hsb" );
			if ( ANTICPX_INVALID_HANDLE_VALUE == m_kHackShieldHandle )
			{
				CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Ahn HackShield CreateServerObject Failed") );
				return E_FAIL;
			}
			INFO_LOG(BM::LOG_LV2, __FL__ << _T("AntiHack ServerBind Init Success !!!!!!!"));
		}break;
	}
	return S_OK;
}

void PgHub::Locked_OnDisconnectUser(CEL::CSession_Base *pkSession)
{//�Ҵ� �ȵ� ������ ClearTimeOverPlayer�� ���ͷ� �����ִ� ������
	// **** ���� :: WRITE LOCK ���
	if( pkSession )
	{
		PgReserveMemberData* pkData1 = NULL;
		PgReserveMemberData* pkData2 = NULL;

		BM::GUID const &kCustomKey = pkSession->CustomKey();
		CONT_SWITCH_RESERVER::key_type kReserveKey;
		pkSession->m_kSessionData.Pop(kReserveKey);
		pkSession->m_kSessionData.PosAdjust();

		{
			BM::CAutoMutex kLock(m_kMutex, true);// Write Lock ��Ҵ�!!
			CONT_SWITCH_PLAYER::iterator member_itor = m_kContMemberSession.find(kCustomKey);
			if( member_itor != m_kContMemberSession.end() )
			{
				if ( member_itor->second->SessionKey() == pkSession->SessionKey() )
				{
					INFO_LOG( BM::LOG_LV6, __FL__ << L" Disconnected.....MemberID[" << (*member_itor).second->ID() << L"," << (*member_itor).second->guidMember << L"]" );

					BM::Stream kPacket(PT_A_NFY_USER_DISCONNECT, member_itor->second->guidMember);
					kPacket.Push( member_itor->second->guidCharacter );
					g_kProcessCfg.ServerIdentity().WriteToPacket( kPacket );
					SendToContents(kPacket);

					pkData1 = member_itor->second;
					m_kContMemberSession.erase(kCustomKey);//�������� ����
					SendNfyPlayerCount();
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << L" Disconnected.....MemberID[" << (*member_itor).second->ID() << L"," << (*member_itor).second->guidMember << L"] But Different SessionKey" );
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}

			CONT_SWITCH_RESERVER::iterator reserve_itor = m_kReserveMember.find(kReserveKey);
			if(reserve_itor != m_kReserveMember.end())
			{
				pkData2 = reserve_itor->second;
				if ( pkData2 )
				{
					if ( true == pkData2->m_bWaitAntiHackCheck )
					{// ��Ƽ�� üũ���ε� ������ ���� ���ȳ�
						DWORD const dwCurTime = BM::GetTime32();
						DWORD const dwGapTime = ::DifftimeGetTime( pkData2->m_dwLastAntiHackCheckTime, dwCurTime );

						PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_CheckAntiHack_NoReply2, *pkData2, NULL, (BM::vstring(L" Disconnected before, Is Hacking or Crash, GapTime[Base:") << RESERVEMEMBER_TIMEOUT << L" > CurTime:" << dwGapTime << L"]")); // ������ ����
					}
				}
				m_kReserveMember.erase(kReserveKey);//����� Ű�� Ʋ�Ⱦ���.//kCustomKey�� ����� �ȵ�. ���� ���� �غ���. // SAFE_DELETE()�� �ؿ���
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}

			pkSession->CustomKey(BM::GUID::NullData());
			pkSession->m_kSessionData.Reset();
		}

		if ( pkData1 == pkData2 )
		{
			pkData2 = NULL;
		}

		SAFE_DELETE(pkData1);
		SAFE_DELETE(pkData2);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}
}

void PgHub::Locked_OnDisconnectFromCenter( bool const bPublic )
{
	BM::CAutoMutex kLock( m_kMutex, !bPublic );

	BM::Stream kPacket;
	kPacket.SetStopSignal(true);

	if( bPublic )
	{
		CONT_SWITCH_PLAYER::const_iterator member_itr = m_kContMemberSession.begin();
		for (  ; member_itr!=m_kContMemberSession.end() ; ++member_itr )
		{
			// ���⼭�� ����� �ȵȴ�.
			// ������ �������� �ߴٴ°� �ͼ� ���������� ó�� �ȴ�.
			if(	CProcessConfig::IsPublicChannel(member_itr->second->kSI.nChannel) )
			{
				CAUTION_LOG( BM::LOG_LV0, _T("* MemberID[") << member_itr->second->ID() << _T(",") << member_itr->second->guidMember.str().c_str() << _T("] Character[") << member_itr->second->guidCharacter.str().c_str()
					<< _T("] Ground[") << member_itr->second->kGroundKey.GroundNo() << _T("] CHECK ROLLBACK") );
				g_kCoreCenter.Send( member_itr->second->SessionKey(), kPacket );
			}
		}
	}
	else
	{
		// �� ������ ��.
		{
			CONT_SWITCH_PLAYER::iterator member_itr = m_kContMemberSession.begin();
			for ( ; member_itr!=m_kContMemberSession.end() ; ++member_itr )
			{
				CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("* MemberID[") << member_itr->second->ID() << _T(",") << member_itr->second->guidMember.str().c_str() << _T("] Character[") << member_itr->second->guidCharacter.str().c_str()
					<< _T("] Ground[") << member_itr->second->kGroundKey.GroundNo() << _T("] CHECK ROLLBACK") );
				g_kCoreCenter.Send( member_itr->second->SessionKey(), kPacket );

				SAFE_DELETE(member_itr->second);
			}
			m_kContMemberSession.clear();
		}

		{
			CONT_SWITCH_RESERVER::iterator member_itr = m_kReserveMember.begin();
			for ( ; member_itr!=m_kReserveMember.end() ; ++member_itr )
			{
				CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("* MemberID[") << member_itr->second->ID() << _T(",") << member_itr->second->guidMember.str().c_str() << _T("] ReserveMember") );
				g_kCoreCenter.Send( member_itr->second->SessionKey(), kPacket );
				SAFE_DELETE(member_itr->second);
			}
			m_kReserveMember.clear();
		}
	}
}

bool PgHub::TryAccessSwitch2( CEL::CSession_Base *pkSession, BM::GUID const &guidSwitchKey, BM::Stream * const pkPacket )
{// ���ϰ��� �ǹ̴� ���� m_kReserveMember�� �ִ��� �������̴�.
	CONT_SWITCH_RESERVER::iterator itr = m_kReserveMember.find( guidSwitchKey );
	if ( itr != m_kReserveMember.end() )
	{
		PgReserveMemberData *pkRMD = itr->second;

		bool bRet = false;
		if ( true == pkRMD->m_bWaitAntiHackCheck )
		{
			if ( pkPacket )
			{
				bRet = PgHubUtil::RecvAntiHackCheck( pkSession, *pkPacket, pkRMD, __FUNCTIONW__, __LINE__ );
			}
		}
		else
		{
			BM::Stream kAHPacket( PT_S_C_ANS_ACCESS_SWITCH_SECOND, guidSwitchKey );

			if( (true == PgHub::ms_bUseAntiHackServerBind)
			&&	(true == PgHubUtil::MakeCheckAntiHackQuery( kAHPacket, pkRMD, BM::GetTime32(), __FUNCTIONW__, __LINE__))
			)
			{
				pkSession->VSend( kAHPacket );
			}
			else
			{
				bRet = true;// �̷��� �ؾ� �Ѵ�.
			}
		}

		if ( true == bRet )
		{

			pkRMD->SessionKey(pkSession->SessionKey());
			pkRMD->m_pkSession = pkSession;
			pkRMD->kSI.Clear();

			pkSession->CustomKey( pkRMD->guidMember );//Set CustomKey

			BM::Stream kServerPacket(PT_S_T_NFY_RESERVED_MEMBER_LOGIN);
			pkRMD->WriteToPacket(kServerPacket);

			::SendToCenter( kServerPacket, false/*Reserve ������ ��ä�� �������׸� �뺸�ؾ� �Ѵ�!!*/ );
			INFO_LOG( BM::LOG_LV6, L"[TryAccessSwitch]" << pkSession->Addr().ToString() );
		}

		return true;// ���⼭�� ���� true
	}

	return false;
}

bool PgHub::Locked_SendPacketToUser(BM::GUID const &rkMemberGuid, BM::Stream const &rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendPacketToUser(rkMemberGuid, rkPacket);
}

bool PgHub::SendPacketToUser(BM::GUID const &rkMemberGuid, BM::Stream const &rkPacket) const
{
	CONT_SWITCH_PLAYER::const_iterator itor = m_kContMemberSession.find(rkMemberGuid);//���ο� �Ҵ�Ǿ� ������ �ȵ�.
	if(m_kContMemberSession.end() != itor)
	{
		CONT_SWITCH_PLAYER::mapped_type const &pkElement = (*itor).second;

		pkElement->m_pkSession->VSend(rkPacket);
		return true;
//		return g_kCoreCenter.Send( member->SessionKey(), rkPacket );
	}
	else
	{
		BM::Stream::DEF_STREAM_TYPE usPacketType = 0;
		BM::Stream::STREAM_DATA const &kPacketData = rkPacket.Data();
		if ( kPacketData.size() >= sizeof(BM::Stream::DEF_STREAM_TYPE) )
		{
			::memcpy( &usPacketType, &(kPacketData.at(0)), sizeof(BM::Stream::DEF_STREAM_TYPE) );
		}

		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find MemberGUID=") << rkMemberGuid << _T(" PacketType=") << usPacketType );
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgHub::Locked_ChangeMemberMapServerNo( SERVER_IDENTITY const &kSI, SGroundKey const &kGndKey, BM::GUID const& kMemberGuid, BM::GUID const& kCharGuid )
{
	// **** ���� :: WRITE LOCK ���
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SWITCH_PLAYER::iterator user_itr = m_kContMemberSession.find(kMemberGuid);
	if( m_kContMemberSession.end() != user_itr )
	{
		CONT_SWITCH_PLAYER::mapped_type &pkSwitchMember = user_itr->second;
		
		pkSwitchMember->kGroundKey = kGndKey;
		pkSwitchMember->guidCharacter = kCharGuid;
		pkSwitchMember->kSI = kSI;
		pkSwitchMember->m_bPublicConnect = CProcessConfig::IsPublicChannel( kSI.nChannel );

		if ( pkSwitchMember->kSI.nServerType == CEL::ST_CENTER )
		{
			pkSwitchMember->kClientGroundKey = pkSwitchMember->kGroundKey;
		}
		return true;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << _T(" [") << kMemberGuid.str().c_str() << _T("]Member is not found") );
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgHub::Locked_RemoveMember(SERVER_IDENTITY const &kSI)
{//���� itor �� erase ��������.
	// **** ���� :: WRITE LOCK ���
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SWITCH_PLAYER::iterator itor = m_kContMemberSession.begin();
	while( m_kContMemberSession.end() != itor )
	{
		CONT_SWITCH_PLAYER::mapped_type const &pkElement = (*itor).second;
		if( pkElement->kSI == kSI )
		{
			__RemoveMember(CDC_MapServerDown, pkElement->guidMember);
		}
		++itor;
	}
	return true;
}

bool PgHub::Locked_RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &rkMemberGuid, BYTE byReason )
{
	// **** ���� :: WRITE LOCK ���
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SWITCH_PLAYER::iterator itor = m_kContMemberSession.find(rkMemberGuid);
	if( m_kContMemberSession.end() != itor )
	{
		BM::Stream kCPacket;
		return __RemoveMember(eCause, rkMemberGuid, kCPacket, byReason);
	}
	return false;
}

bool PgHub::Locked_RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &rkMemberGuid, BM::Stream &kCPacket )
{
	// **** ���� :: WRITE LOCK ���
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_SWITCH_PLAYER::iterator itor = m_kContMemberSession.find(rkMemberGuid);
	if( m_kContMemberSession.end() != itor )
	{
		return __RemoveMember(eCause, rkMemberGuid, kCPacket );
	}
	return false;
}

bool PgHub::Locked_SendPacketToUserFromMap(CONT_GUID_LIST const &kContTarget, BM::Stream const &rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_GUID_LIST::const_iterator target_itor = kContTarget.begin();

	while(kContTarget.end() != target_itor)
	{
		CONT_SWITCH_PLAYER::const_iterator itor = m_kContMemberSession.find((*target_itor));//���ο� �Ҵ�Ǿ� ������ �ȵ�.
		if(m_kContMemberSession.end() != itor)
		{
			CONT_SWITCH_PLAYER::mapped_type const &pkSwitchMember = (*itor).second;

			pkSwitchMember->m_pkSession->VSend(rkPacket);
			// ���� �ʼ������� ������ ���� ���� ���� �𸣴� ��찡 �߻��Ѵ�.
			// �� ������ �ٽ� �����ϰ� �Ǹ�(�ʼ����� �������� ���� ���¿���), �ʼ����� �ʼ����� ����ȭ ��Ŷ�� Ŭ���̾�Ʈ�� �����Ϸ� �Ѵ�.
			// �̰��� ���� ���� �ڵ�
//			g_kCoreCenter.Send( pkSwitchMember->SessionKey(), rkPacket );
		}
		++target_itor;
	}
	return true;
}

void PgHub::Locked_DisplayState() const
{
	BM::CAutoMutex kLock(m_kMutex);

	INFO_LOG( BM::LOG_LV1, _T("------------ Conntected Switch User List -----------") );
	CONT_SWITCH_PLAYER::const_iterator user_iter = m_kContMemberSession.begin();
	size_t iCur = 1;
	
	CEL::SSessionStatistics kTotalStattistics;
	CEL::SSessionStatistics kStattistics;

	while( m_kContMemberSession.end() !=  user_iter )
	{
		CONT_SWITCH_PLAYER::mapped_type const &pkElement = (*user_iter).second;

		int const iLoginNo = pkElement->kLoginServer.nServerNo;
		SERVER_IDENTITY const &kMapServer = pkElement->kSI;
		SERVER_IDENTITY const& kSwitchServer = pkElement->kSwitchServer;
		int const iGroundNo = pkElement->kGroundKey.GroundNo();

		g_kCoreCenter.Statistics(pkElement->SessionKey(), kStattistics);
		kTotalStattistics += kStattistics;
		INFO_LOG( BM::LOG_LV1, iCur << _T(" Account[") << pkElement->ID() << _T("], L[") << iLoginNo << _T("], SS[") << kSwitchServer.nServerNo << _T("], Map[R:") << kMapServer.nRealm
			<< _T(", C:") << kMapServer.nChannel << _T(", S:") << kMapServer.nServerNo << _T("]-G[") << iGroundNo << _T("]") );

		++iCur;
		++user_iter;
	}
	INFO_LOG( BM::LOG_LV1, _T("--------------------- Cut Line ---------------------") );
	
	int const iSessionCount = (int)m_kContMemberSession.size();
	int const iAvgTotal = (int)((kTotalStattistics.TotalSendCount())?kTotalStattistics.TotalSendBytes()/kTotalStattistics.TotalSendCount():0);
	int const iAvgPerSession = ((iSessionCount && iAvgTotal)?iAvgTotal/iSessionCount:0);

	INFO_LOG( BM::LOG_LV1, _T("Byte Avg/Session[") << iAvgPerSession << _T("] AvgTotal[") << iAvgTotal << _T("]") );//���Ǵ�. 
	INFO_LOG( BM::LOG_LV1, _T("Total User[") << iSessionCount << _T("] Count[") << kTotalStattistics.TotalSendCount() << _T("], Bytes[") << kTotalStattistics.TotalSendBytes() << _T("]") );

	INFO_LOG( BM::LOG_LV1, _T("--------------------- Cut Line ---------------------") );
}

void CALLBACK PgHub::ProcessUserPacketForLogined( PgReserveMemberData const &rkRMD, unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{//protected:

	// �� üũ ����, ��Ŷ ó��
	switch(usType)
	{
	case PT_C_M_CS_REQ_SIMPLE_ENTER_CASHSHOP:
	case PT_C_M_CS_REQ_ENTER_CASHSHOP:
		{
			BM::GUID kStoreValueKey;

			if(	true == pkPacket->Pop(kStoreValueKey) )
			{
				g_kReloadMgr.ReqSyncDataFromUser(kStoreValueKey, pkSession);
				PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, pkPacket, usType);
			}
		}break;
	case PT_C_L_TRY_LOGIN:
		{
			// ���⼭ ���� ä�κ��� or �ɸ��� �缱���� �ϰ� �ʹٴ� �Ŵ�.
			BM::Stream kMapPacket( PT_S_A_REQ_AUTOLOGIN );
			rkRMD.SReqSwitchReserveMember::WriteToPacket( kMapPacket );
			short channelNrealm = 0;
			pkPacket->Pop(channelNrealm);
			pkPacket->Pop(channelNrealm);
			kMapPacket.Push(*pkPacket);
			PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, &kMapPacket, usType);
		}break;
	case PT_C_S_REQ_CHARACTER_LIST:
		{
			BM::Stream kPacket(PT_S_T_REQ_CHARACTER_LIST, rkRMD.guidMember);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );//������ �θ��� ��� �ǳ�?
		}break;
	case PT_C_S_REQ_CREATE_CHARACTER:
		{
			BM::Stream kCPacket(PT_S_T_REQ_CREATE_CHARACTER, rkRMD.guidMember);
			kCPacket.Push(*pkPacket);
			::SendToCenter( kCPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP:
		{
			BM::Stream kPacket(PT_C_N_REQ_CHECK_CHARACTERNAME_OVERLAP, rkRMD.guidMember);
			kPacket.Push(*pkPacket);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_S_REQ_DELETE_CHARACTER:
		{
			BM::GUID kCharacterGuid;// ������ ĳ������ ĳ����GUID
			pkPacket->Pop(kCharacterGuid);

			BM::Stream kPacket(PT_S_T_REQ_DELETE_CHARACTER);
			kPacket.Push(rkRMD.guidMember);//�����ڸ��� ���� �� �����Ƿ�. 
			kPacket.Push(kCharacterGuid);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_N_REQ_REALM_MERGE:
		{
			BM::Stream kPacket(PT_C_N_REQ_REALM_MERGE);
			kPacket.Push(rkRMD.guidMember);
			kPacket.Push(*pkPacket);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_S_REQ_SELECT_CHARACTER:
		{
			BM::GUID kCharacterGuid;
			if(pkPacket->Pop(kCharacterGuid))
			{//���� ���� �� �ʼ����� ����� �����ϴ�.(�ι� Ŭ�� ���� �ʿ�)
				if(!rkRMD.kGroundKey.GroundNo())
				{
					BM::Stream kPacket(PT_S_T_REQ_SELECT_CHARACTER);
					kPacket.Push(rkRMD.guidMember);
					kPacket.Push(kCharacterGuid);
					kPacket.Push(*pkPacket);

					::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
					return;
				}
			}

			CAUTION_LOG( BM::LOG_LV6, __FL__ << _T(" Can't Character Selection State[") << kCharacterGuid.str().c_str() << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			pkSession->VTerminate();
		}break;
	case PT_C_T_REQ_SELECT_CHARACTER_OTHERMAP:
		{
			int iOtherMap = 0;
			if ( pkPacket->Pop(iOtherMap) )
			{
				if(!rkRMD.kGroundKey.GroundNo())
				{
					BM::Stream kPacket(PT_C_T_REQ_SELECT_CHARACTER_OTHERMAP);
					kPacket.Push(rkRMD.guidMember);
					kPacket.Push(iOtherMap);
					::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
					return;
				}
			}
			
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	case PT_C_S_ANS_ONLY_PING:
		{
			PgReserveMemberData* pkRMD = const_cast<PgReserveMemberData*>(&rkRMD);

			BM::GUID kPingGuid;
			pkPacket->Pop(kPingGuid);

			if(S_OK != pkRMD->RecvPing(kPingGuid/*, dwClientTime*/))
			{
				BM::Stream kStopPacket;
				kStopPacket.SetStopSignal(true);

				if (S_OK == PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_SpeedHack, rkRMD, pkSession))
				{
					pkSession->VSend(kStopPacket);
					pkSession->VTerminate();
				}
			}
		}break;
	case PT_C_S_NFY_UNIT_POS:
		{
			PgReserveMemberData* pkRMD = const_cast<PgReserveMemberData*>(&rkRMD);

			BM::GUID kPingGuid;
			POINT3 ptPos;
			DWORD dwLatency;
			bool bIncludeScPos;
			POINT3 ptScPos;

			pkPacket->Pop(kPingGuid);
			pkPacket->Pop(ptPos);
			pkPacket->Pop(bIncludeScPos);
			if(bIncludeScPos)
			{
				pkPacket->Pop(ptScPos);
			}

			if(pkRMD->DropLastRecvPing(kPingGuid, dwLatency))
			{
#ifdef _GROUNDKEY_CHECK
				if ( rkRMD.kSI.nServerType == CEL::ST_MAP
				&& rkRMD.kClientGroundKey.GroundNo() == rkRMD.kGroundKey.GroundNo() )
#else
				if ( rkRMD.kSI.nServerType == CEL::ST_MAP )
#endif
				{
					BM::Stream kToMapPacket(PT_C_S_NFY_UNIT_POS);
					kToMapPacket.Push(dwLatency);// ���⿡ �����Ͻ� ��갪�� ������.
					kToMapPacket.Push(ptPos);
					kToMapPacket.Push(bIncludeScPos);
					if(bIncludeScPos)
					{
						kToMapPacket.Push(ptScPos);
					}

					SendToGround( rkRMD.guidCharacter, rkRMD.kSI, rkRMD.kGroundKey, kToMapPacket );
				}
			}
		}break;
	case PT_C_T_REQ_CHANNLE_INFORMATION:
		{
			if ( !rkRMD.m_bPublicConnect )
			{
				::SendToCenter( BM::Stream( PT_C_T_REQ_CHANNLE_INFORMATION, rkRMD.guidMember ), rkRMD.m_bPublicConnect );
				return;
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	case PT_C_M_REQ_CHAT:	// CT_NORMAL�� �׶���� ������ �������� �������� ������
		{
			BYTE cChatMode = 0;
			pkPacket->Pop( cChatMode );
			switch( cChatMode )
			{
			case CT_NORMAL:
			case CT_TEAM:
				{
#ifdef _GROUNDKEY_CHECK
					if (rkRMD.kClientGroundKey.GroundNo() == rkRMD.kGroundKey.GroundNo())
#endif
					{
						pkPacket->PosAdjust();
						SendToGround(rkRMD.guidCharacter, rkRMD.kSI, rkRMD.kGroundKey, *pkPacket);
					}
				}break;
			case CT_PARTY:
				{
					pkPacket->PosAdjust();
					SendToGround(rkRMD.guidCharacter, rkRMD.kSI, rkRMD.kGroundKey, *pkPacket);

					/*BM::Stream kTPacket( usType, rkRMD.guidCharacter );
					kTPacket.Push( cChatMode );
					kTPacket.Push(*pkPacket);
					SendToChannelChatMgr( kTPacket, rkRMD.bPublicConnect);*/
				}break;
			//case CT_WHISPER_BYNAME:
			//case CT_FRIEND:
			//case CT_GUILD:
			//case CT_EVENT:
			//case CT_EVENT_SYSTEM:
			//case CT_EVENT_GAME:
			//case CT_NOTICE:
			//case CT_BATTLE:
			//case CT_ITEM:
			//case CT_COMMAND:
			default:
				{
					pkPacket->PosAdjust();
					SendToGround(rkRMD.guidCharacter, rkRMD.kSI, rkRMD.kGroundKey, *pkPacket);

					/*BM::Stream kTPacket( usType, rkRMD.guidCharacter );
					kTPacket.Push( cChatMode );
					kTPacket.Push(*pkPacket);
					SendToRealmChatMgr( kTPacket );*/
				}break;
			}
		}break;
	case PT_C_S_ANS_GAME_GUARD_CHECK:
		{
			if( true == rkRMD.m_bWaitAntiHackCheck )
			{
				PgReserveMemberData *pkRMD = const_cast<PgReserveMemberData*>(&rkRMD);
				if ( pkRMD )
				{
					PgHubUtil::RecvAntiHackCheck( pkSession, *pkPacket, pkRMD, __FUNCTIONW__, __LINE__ );
				}
			}
		}break;
	case PT_C_N_REQ_EMPORIA_STATUS_LIST:
		{//
			BM::Stream kPacket( usType, rkRMD.guidCharacter );
			kPacket.Push( *pkPacket );
			SendToRealmContents( PMET_EMPORIA, kPacket );
		}break;
	case PT_C_N_REQ_GUILD_COMMAND:
		{
			BYTE cCmdType = 0;
			if( pkPacket->Pop(cCmdType) )
			{
				switch( cCmdType )
				{
				case GC_PreCreate_Test:
				case GC_PreCreate:
				case GC_Create:
					{
						PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, pkPacket, usType);
					}break;
				default:
					{
						BM::Stream kPacket( usType, rkRMD.guidCharacter );
						kPacket.Push( cCmdType );
						kPacket.Push( *pkPacket );
						SendToRealmContents( PMET_GUILD, kPacket );
					}break;
				}
			}
		}break;
	case PT_C_M_NFY_HACKSHIELD_CALLBACK:
		{
			long lCode = 0;
			std::wstring kMsg;

			pkPacket->Pop( lCode );
			pkPacket->Pop( kMsg );

			BM::vstring const kLogMsg = BM::vstring() << __FL__ << _T("[HackShield][ClientNotify] Detection Hack Account[N: ") << rkRMD.ID() << _T(", G: ") << rkRMD.guidMember << L"] Character[G: " << rkRMD.guidCharacter << L"] Type[HackShield] CallBack Code:" << lCode << L" Msg:" << kMsg;
			CAUTION_LOG( BM::LOG_LV0, kLogMsg );
			//HACKING_LOG(BM::LOG_LV5, kNewLog);
		}break;
	case PT_C_M_NFY_MAPLOADED:
		{
			BYTE cMapMoveCause = 0;
			SGroundKey kGroundKey;
			pkPacket->Pop(cMapMoveCause);
			kGroundKey.ReadFromPacket(*pkPacket);
			PgReserveMemberData* pkRMD = const_cast<PgReserveMemberData*>(&rkRMD);
			pkRMD->kClientGroundKey = kGroundKey;

			PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, pkPacket, usType);
		}break;
	case PT_C_N_REQ_REGIST_PVPLEAGUE_TEAM:
	case PT_C_N_REQ_GIVEUP_PVPLEAGUE_TEAM:
	case PT_C_N_REQ_QUERY_PVPLEAGUE_TEAM:
		{
			BM::Stream kPacket( usType );
			kPacket.Push( *pkPacket );
			SendToRealmContents( PMET_PVP_LEAGUE, kPacket );
		}break;
	case PT_C_S_REQ_SAVE_CHARACTOR_SLOT:
		{
			BM::GUID kCharacterGuid;//���� �� ĳ������ ĳ����GUID
			int iSlot = 0;			//���� �� ���� ��ȣ
			pkPacket->Pop(kCharacterGuid);
			pkPacket->Pop(iSlot);

			BM::Stream kPacket(PT_S_T_REQ_SAVE_CHARACTOR_SLOT);
			kPacket.Push(rkRMD.guidMember);
			kPacket.Push(kCharacterGuid);
			kPacket.Push(iSlot);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_S_REQ_FIND_CHARACTOR_EXTEND_SLOT:
		{
			BM::Stream kPacket(PT_S_T_REQ_FIND_CHARACTOR_EXTEND_SLOT);
			kPacket.Push(rkRMD.guidMember);
			::SendToCenter( kPacket, rkRMD.m_bPublicConnect );
		}break;
	case PT_C_L_TRY_LOGIN_CHANNELMAPMOVE:
		{
			// ä�� �̵��ϸ鼭 �� ����.
			CONT_CHANNEL_MAPMOVE_PLAYER::iterator iter = m_ContMapMoveUser.find(rkRMD.guidCharacter);
			if( iter != m_ContMapMoveUser.end() )
			{
				BM::Stream MapPacket( PT_S_A_REQ_AUTOLOGIN );
				rkRMD.SReqSwitchReserveMember::WriteToPacket( MapPacket );
				MapPacket.Push(*pkPacket);					// ������ȣ.
				MapPacket.Push(iter->second.iChannelNo);	// ������ ä��.
				MapPacket.Push(iter->second.iGroundNo);		// ������ ��.
				PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, &MapPacket, usType);
				m_ContMapMoveUser.erase(iter);
			}
			else
			{
				BM::Stream Packet( PT_M_C_NFY_LOGIN_CHANNELMAPMOVE );
				pkSession->VSend(Packet);
			}
		}break;
	default:
		{
			PgHubUtil::DefaultSendToGround(__FUNCTIONW__, __LINE__, rkRMD, pkSession, pkPacket, usType);
		}break;
	}
}

void CALLBACK PgHub::ProcessUserPacketForNotLogin( unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{//protected:
	switch(usType)
	{
	case PT_C_S_TRY_ACCESS_SWITCH:
		{
			STryAccessSwitch kTAS;
			if(kTAS.ReadFromPacket(*pkPacket))
			{
				if(!pkPacket->RemainSize())
				{
					kTAS.MakeCorrect();//��Ʈ�� ������ ��츦 ���� ����
					TryAccessSwitch(pkSession, kTAS);
					return;
				}
			}

			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("PT_C_S_TRY_ACCESS_SWITCH Format Fault IP:")<< pkSession->Addr().IP());
			pkSession->VTerminate();//������ �ȸ������̹Ƿ� ������ ���� ����.
		}break;
	case PT_C_S_TRY_ACCESS_SWITCH_SECOND:
		{
			BM::GUID guidSwitchKey;
			pkPacket->Pop( guidSwitchKey );
			if ( !TryAccessSwitch2( pkSession, guidSwitchKey, pkPacket ) )
			{
				HACKING_LOG( BM::LOG_LV5, _T("[TryAccessSwitch] IP<") << pkSession->Addr().ToString() << _T("> No Reserved") );

				BM::Stream kUserPacket(PT_S_C_TRY_ACCESS_SWITCH_RESULT);
				kUserPacket.Push(E_TLR_NOR_RESERVED_USER);
				kUserPacket.SetStopSignal(true);
				pkSession->VSend(kUserPacket);//����ߵ�
			}
		}break;
	case PT_C_L_TRY_LOGIN:
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PT_C_L_TRY_LOGIN Recved [")<< pkSession->Addr().IP()<<L"]");
		}break;
	case PT_C_L_TRY_LOGIN_CHANNELMAPMOVE:
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PT_C_L_TRY_LOGIN_CHANNELMAPMOVE Recved [")<< pkSession->Addr().IP()<<L"]");
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			pkSession->VTerminate();//����ġ ���� �õ� ��Ŷ �ܿ��� ��� �˽�
		}break;
	}
}

void PgHub::Locked_OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::GUID const &kCustomKey = pkSession->CustomKey();

//	BM::Stream kFilterPacket(*pkPacket);
//	kFilterPacket = *pkPacket;
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);
	
	size_t const rd_pos = pkPacket->RdPos();
	if( BM::GUID::IsNotNull(kCustomKey) )
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_SWITCH_PLAYER::const_iterator member_itor = m_kContMemberSession.find(kCustomKey);
		if(member_itor != m_kContMemberSession.end())
		{// ���� ���� �� �ִ� �κ��� �ִ�.
			CONT_SWITCH_PLAYER::mapped_type pkElement = (*member_itor).second;
			if ( pkElement->m_pkSession == pkSession )
			{
				if(S_OK != g_kProtocolFilter.FilterProtocol((*pkPacket)))
				{
					PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_CheckHellDart, *pkElement, pkSession, BM::vstring(L" PacketType: ") << usType); // ������ ���´�
					pkSession->VTerminate();
					return;
				}

				pkPacket->RdPos(rd_pos);//������ �ѹ�.

				// �� ���� üũ
				if( PgHubUtil::CheckAntiHackResponseTime(*pkElement, pkSession) )
				{
					ProcessUserPacketForLogined(*pkElement, usType, pkSession, pkPacket);
				}
			}
			else
			{
				PgHubUtil::UserDisconnectedLog( __FUNCTIONW__, __LINE__, CDC_NotAuthSession, *pkElement, pkSession, BM::vstring(L" PacketType: ") << usType); // ������ ���´�
				pkSession->VTerminate();
			}
		}
	}
	else
	{
		BM::CAutoMutex kLock(m_kMutex, true);
		ProcessUserPacketForNotLogin(usType, pkSession, pkPacket);
	}
}

bool PgHub::__RemoveMember(EClientDisconnectedCause const eCause, BM::GUID const &rkMemberGuid, BM::Stream& rkPacket, BYTE byReason)
{//protected:
	////////// �ܺ� ���� �Լ�.
	// Lock�� �ܺο��� �ɾ� �ְ� ���;� �Ѵ�.
	//////////
	CONT_SWITCH_PLAYER::iterator itor = m_kContMemberSession.find(rkMemberGuid);
	if( m_kContMemberSession.end() != itor )
	{
		CONT_SWITCH_PLAYER::mapped_type &pkElement = (*itor).second;
		pkElement->dwTimeLimit = BM::GetTime32();
		INFO_LOG(BM::LOG_LV7, __FL__<< _T(" ID= ") << pkElement->ID() << _T(" MemberGUID= ")<< rkMemberGuid << _T(", CAUSE=") << eCause);

		if( pkElement )
		{
			if (S_OK == PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, eCause, *pkElement, pkElement->m_pkSession,  BM::vstring(), byReason))
			{
				rkPacket.SetStopSignal(true);
				g_kCoreCenter.Send(pkElement->SessionKey(), rkPacket);//���� �ñ׳��� ����.

				// ���⼭ �ٷ� m_kContMemberSession.erase ȣ�� �ϸ�, Center�� ���� ���� ���� ��Ŷ�� �� ������ �ȴ�.
				// OnDisconnectUser ���� ��������.
				// ������ Channel �����϶��� �̹� ��� ����� �Դ�.
				// ���⼭ ������ �ָ� �ٽ� �����°� �ö�(�ٸ����� �������� ���� �����ؿ� ������.. �̹̱׷��̼ǿ��� �α׾ƿ� ó���� �Ǿ� ���� �� �ִ�)
				if ( CDC_ChangeChannel == eCause )
				{
					SAFE_DELETE(pkElement);// �޸� ���� �ߴ�.
					m_kContMemberSession.erase(itor);// ������.
				}
			}
		}
		return true;
	}
//	else	//���� ����� ���� �Ǳ� ������. -> ���Ͱ� PT_A_NFY_USER_DISCONNECT�� ������ ���� ���� ��Ų��.
//	{
//		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<< _T("No MemberGUID= ") << rkMemberGuid);
//	}
	
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

void PgHub::Locked_OnTimer_CheckPing()
{
	// **** ���� :: WRITE LOCK ���
	//ACE_Time_Value const kNowTime = ACE_OS::gettimeofday();
	//static ACE_Time_Value const kInterval(5, 0);//5��
	//static ACE_Time_Value const kKeepAliveTime(60, 0);//60��

//	LV-00,22:45:35,[KeepAlive] Order Disconnect User[DTEST100] Last Ping Before 60953msec

	BM::CAutoMutex kLock(m_kMutex);
	// Ready User
	CONT_SWITCH_PLAYER::iterator member_itor = m_kContMemberSession.begin();
	//DWORD const dwNow = BM::GetTime32();
	while( m_kContMemberSession.end() != member_itor )
	{
		CONT_SWITCH_PLAYER::mapped_type pkElement = (*member_itor).second;

		if(S_OK != pkElement->DoSendPing(1))
		{
			if( pkElement->m_pkSession->IsAlive() )
			{
				if (S_OK == PgHubUtil::UserDisconnectedLog(__FUNCTIONW__, __LINE__, CDC_LaziPing, *pkElement, pkElement->m_pkSession))
				{
					INFO_LOG(BM::LOG_LV0, _T("[KeepAlive] Order Disconnect User[")<< pkElement->ID() );
					pkElement->m_pkSession->VTerminate();
				}
			}
		}
		++member_itor;
	}
}

int PgHub::Locked_GetSwitchConnectionPlayerCount() const
{
	BM::CAutoMutex kLock(m_kMutex);

	return (int)m_kContMemberSession.size();
}

void PgHub::Locked_RegistChannelMapMoveUser(BM::Stream * const pPacket)
{
	BM::CAutoMutex Lock(m_kMutex);

	BM::GUID CharGuid;
	SChannelMapMove MoveInfo;

	pPacket->Pop(CharGuid);
	pPacket->Pop(MoveInfo);

	auto Ret = m_ContMapMoveUser.insert( std::make_pair(CharGuid, MoveInfo) );
	if( false == Ret.second )
	{
		INFO_LOG(BM::LOG_LV0, _T("Duplecate : ") << CharGuid );
	}
}