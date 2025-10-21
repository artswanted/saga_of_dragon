#include "stdafx.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgGlobalPartyMgr.h"

PgGlobalPartyMgr_Public::PgGlobalPartyMgr_Public()
{
}

PgGlobalPartyMgr_Public::~PgGlobalPartyMgr_Public()
{
}

HRESULT PgGlobalPartyMgr_Public::Add( BM::GUID const &kCharGuid, short const nChannelNo )
{
	if ( 0 == nChannelNo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"CharGuid<" << kCharGuid << L">'s Channel Error[new: " << nChannelNo << L"]" );
		return E_FAIL;
	}

	auto kPair = m_kContPartyChannel.insert( std::make_pair( kCharGuid, nChannelNo ) );
	if ( !kPair.second )
	{
		if ( kPair.first->second == nChannelNo )
		{
			return S_FALSE;
		}

		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"CharGuid<" << kCharGuid << L">'s Channel Error[old:" << kPair.first->second << L", new: " << nChannelNo << L"]" );
		return E_FAIL;
	}
	return S_OK;
}

short PgGlobalPartyMgr_Public::Del( BM::GUID const &kCharGuid )
{
	CONT_PARTY_CHANNEL::iterator itr = m_kContPartyChannel.find( kCharGuid );
	if ( itr != m_kContPartyChannel.end() )
	{
		short const nChannel = itr->second;
		m_kContPartyChannel.erase( itr );
		return nChannel;
	}
	return 0;
}

short PgGlobalPartyMgr_Public::GetChannel( BM::GUID const &kCharGuid )const
{
	CONT_PARTY_CHANNEL::const_iterator itr = m_kContPartyChannel.find( kCharGuid );
	if ( itr != m_kContPartyChannel.end() )
	{
		return itr->second;
	}
	return 0;
}

//------------------------------------------
// PgGlobalPartyMgr_Public_Wrapper
//------------------------------------------
PgGlobalPartyMgr_Public_Wrapper::PgGlobalPartyMgr_Public_Wrapper()
{
}

PgGlobalPartyMgr_Public_Wrapper::~PgGlobalPartyMgr_Public_Wrapper()
{
}

HRESULT PgGlobalPartyMgr_Public_Wrapper::Add( BM::GUID const &kCharGuid, short const nChannelNo )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->Add( kCharGuid, nChannelNo );
}

short PgGlobalPartyMgr_Public_Wrapper::Del( BM::GUID const &kCharGuid )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->Del( kCharGuid );
}

short PgGlobalPartyMgr_Public_Wrapper::GetChannel( BM::GUID const &kCharGuid )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->GetChannel( kCharGuid );
}

bool PgGlobalPartyMgr_Public_Wrapper::ProcessMsg(SEventMessage *pkMsg)
{
	PACKET_ID_TYPE wType = 0;
	pkMsg->Pop(wType);
	switch(wType)
	{
	case PT_M_T_ANS_SUMMONPARTYMEMBER:
	case PT_M_T_ANS_MOVETOSUMMONER:
	case PT_M_T_REQ_MOVETOSUMMONER:
	case PT_M_T_REQ_SUMMONPARTYMEMBER:
	case PT_M_T_REQ_PARTYMEMBERPOS:
	case PT_M_T_REQ_PARTYMASTERGROUNDKEY:
	case PT_C_N_REQ_PARTY_RENAME_GM:
	case PT_C_N_REQ_JOIN_PARTY:
	case PT_C_N_REQ_JOIN_PARTYFIND:
	case PT_C_N_ANS_JOIN_PARTY:// 유저가 파티에 가입여부를 대답
	case PT_C_N_ANS_JOIN_PARTYFIND:
	case PT_C_N_REQ_CREATE_PARTY:
	case PT_M_N_REQ_PARTY_COMMAND:
	case PT_C_N_REQ_KICKOUT_PARTY_USER:
	case PT_M_T_REQ_REGIST_HARDCORE_VOTE:
	case PT_M_T_ANS_JOIN_HARDCORE_FAILED:
	case PT_T_T_REQ_READY_JOIN_HARDCORE:
		{
			CAUTION_LOG( BM::LOG_LV2, __FL__ << L"Never No Recv This Packet = " << wType ); 
			// 이건 공용채널에서는 오면 안되는데..
		}break;
	case PT_C_N_REQ_LEAVE_PARTY:
		{
			BM::Stream *pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkMsgPacket )
			{
				BM::GUID kCharGuid;
				pkMsgPacket->Pop( kCharGuid );

				short const nChannelNo = GetChannel( kCharGuid );
				if ( nChannelNo )
				{
					BM::Stream kPassPacket( PT_C_N_REQ_LEAVE_PARTY_2ND );
					if( S_OK == ::WritePlayerInfoToPacket_ByGuid( kCharGuid, false, kPassPacket) )
					{
						kPassPacket.Push( *pkMsgPacket );
						::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPassPacket );
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, L"ERROR MSG CAST!!!" );
			}
		}break;
	case PT_M_N_NFY_PARTY_USER_PROPERTY:
		{
			BM::Stream *pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkMsgPacket )
			{
				BM::GUID kCharGuid;
				pkMsgPacket->Pop( kCharGuid );

				short const nChannelNo = GetChannel( kCharGuid );
				if ( nChannelNo )
				{
					BM::Stream kPassPacket( PT_M_N_NFY_PARTY_USER_PROPERTY_2ND );
					if( S_OK == ::WritePlayerInfoToPacket_ByGuid( kCharGuid, false, kPassPacket) )
					{
						kPassPacket.Push( *pkMsgPacket );
						::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPassPacket );
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, L"ERROR MSG CAST!!!" );
			}
		}break;
	case PT_C_N_REQ_PARTY_CHANGE_OPTION:
		{
			BM::Stream *pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkMsgPacket )
			{
				BM::GUID kCharGuid;
				pkMsgPacket->Pop( kCharGuid );

				short const nChannelNo = GetChannel( kCharGuid );
				if ( nChannelNo )
				{
					BM::Stream kPassPacket( PT_C_N_REQ_PARTY_CHANGE_OPTION_2ND );
					if( S_OK == ::WritePlayerInfoToPacket_ByGuid( kCharGuid, false, kPassPacket) )
					{
						kPassPacket.Push( *pkMsgPacket );
						::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPassPacket );
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, L"ERROR MSG CAST!!!" );
			}
		}break;
	case PT_C_N_REQ_PARTY_RENAME:
	case PT_C_N_REQ_PARTY_CHANGE_MASTER:
	case PT_M_T_REQ_PARTY_BUFF:
	case PT_A_U_SEND_TOPARTY_BYCHARGUID:
	case PT_A_U_SEND_TOFRIEND_PARTY:
	case PT_A_U_SEND_TOFRIEND_PARTYINFO:
	case PT_M_T_REQ_RET_HARDCORE_VOTE:
		{
			BM::Stream *pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkMsgPacket )
			{
				BM::GUID kCharGuid;
				pkMsgPacket->Pop( kCharGuid );

				short const nChannelNo = GetChannel( kCharGuid );
				if ( nChannelNo )
				{
					BM::Stream kPacket( wType, kCharGuid );
					kPacket.Push( *pkMsgPacket );
					::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPacket );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, L"ERROR MSG CAST!!!" );
			}
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			BM::Stream *pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkMsgPacket )
			{
				SContentsUser kUser;
				kUser.ReadFromPacket(*pkMsgPacket);

				short const nChannelNo = Del( kUser.kCharGuid );
				if ( nChannelNo )
				{
					BM::Stream kPacket( wType );
					kUser.WriteToPacket( kPacket );
					kPacket.Push( *pkMsgPacket );
					::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPacket );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, L"ERROR MSG CAST!!!" );
			}
		}break;
	case PT_T_N_NFY_USER_ENTER_GROUND:
		{
			SAnsMapMove_MT kAMM;
			SContentsUser kUser;
			pkMsg->Pop(kAMM);
			kUser.ReadFromPacket(*pkMsg);

			short const nChannelNo = GetChannel( kUser.kCharGuid );
			if ( nChannelNo )
			{
				BM::Stream kPacket( PT_T_T_NFY_USER_ENTER_GROUND, g_kProcessCfg.ChannelNo() );
				kUser.WriteToPacket( kPacket );
				::SendToOtherChannelContents( nChannelNo, PMET_PARTY, kPacket );
			}
		}break;
	case PT_T_T_SYNC_USER_PARTY:
		{
			BM::GUID kCharGuid;
			short nChannelNo = 0;
			pkMsg->Pop( kCharGuid );
			pkMsg->Pop( nChannelNo );
			Add( kCharGuid, nChannelNo );	
		}break;
	case PT_T_T_SYNC_USER_PARTY2:
		{
			BM::GUID kCharGuid;
			pkMsg->Pop( kCharGuid );
			Del( kCharGuid );
		}break;
	case PT_C_T_NFY_MAPMOVE_TO_PARTYMGR:
		{	// 에러 제거용.
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("invalid packet type=") << wType );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}

bool PgGlobalPartyMgr_Public_Wrapper::GetPartyMemberList( VEC_GUID &rkOutCharGuidList, BM::GUID const &kGuid, bool const bPartyGuid, bool const bIgnoreCharGuid )const
{
	return true;
}

