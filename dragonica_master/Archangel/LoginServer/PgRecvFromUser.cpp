#include "stdafx.h"
#include "PgRecvFromUser.h"
#include "PgLoginUserMgr.h"
#include "Variant/gm_const.h"
#include "variant/PgStringUtil.h"

void CALLBACK OnAcceptFromUser(CEL::CSession_Base *pkSession)
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		INFO_LOG(BM::LOG_LV8, __FL__ << L"Send EncryptKey");

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << L"Get EncryptKey Failed. Session Terminate!");
		CAUTION_LOG(BM::LOG_LV0, __FL__ << L"Get EncryptKey Failed. Session Terminate!");
		pkSession->VTerminate();
	}
}

void CALLBACK OnDisConnectToUser(CEL::CSession_Base *pkSession)
{//!  세션락
	if( g_kLoginUserMgr.Clear( pkSession ) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << L"UserDisconnect[" << pkSession->SessionKey().SessionGuid() << L"]");
	}

	if( g_kLoginUserMgr.DeleteGmLoinedData(pkSession) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << L"GM UserDisconnect[" << pkSession->SessionKey().SessionGuid() << L"]");
	}

	if( g_kLoginUserMgr.DeleteGmWaitData(pkSession) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__ << L"GM UserDisconnect[" << pkSession->SessionKey().SessionGuid() << L"]");
	}
}

//유저 세션은 pkSession->pData 에다가 현재 정보를 셋팅 해 줄것이다. OnDisconnect 에서 delete 해주면 된다.
void CALLBACK OnRecvFromUser(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	switch( usType )//세션에 스템을 기록 해서 이 스템에서 보내면 안되는 패킷이 오면 과감히 씹도록.
	{
	case PT_C_L_TRY_AUTH:
		{	//	아이디,PW 인증만 걸고. // ID 인증은 누가 해주나.. -_-;. 
			if(!pkSession->m_kSessionData.Size())
			{
				SClientTryLogin kCTL;
				std::wstring kAddonString;
				if(	kCTL.ReadFromPacket(*pkPacket)
				&&	pkPacket->Pop(kAddonString, 10000) )//1만 글자까지?.
				{
					if(!pkPacket->RemainSize())
					{
						kCTL.MakeCorrect();//스트링 깨졌을 경우를 위한 가드
						switch( g_kLocal.ServiceRegion() )
						{
						case LOCAL_MGR::NC_JAPAN:
							{
								std::wstring kTemp(kCTL.ID());
								PgStringUtil::ConvFullToHalf(kTemp);
								kCTL.SetID(kTemp);
							}break;
						}
						g_kLoginUserMgr.ProcessAuth( kCTL, pkSession, kAddonString );
						return;
					}
				}
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);

			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect Try Auth Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect Try Auth Session IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();//오류있음
		}break;
	case PT_C_L_TRY_NC_AUTH:
		{	//	아이디,PW 인증만 걸고. // ID 인증은 누가 해주나.. -_-;. 
			if(!pkSession->m_kSessionData.Size())
			{
				SClientTryLogin kCTL;
				std::wstring kSessKey;				
				
				if(kCTL.ReadFromPacket(*pkPacket)
				&& pkPacket->Pop(kSessKey))
				{
					if(!pkPacket->RemainSize())
					{
						kCTL.MakeCorrect();//스트링 깨졌을 경우를 위한 가드
						g_kLoginUserMgr.ProcessAuth_NC(kCTL, pkSession, kSessKey);
						return;
					}
				}
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);

			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect Try NC Auth Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect Try NC Auth Session IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();//오류있음
		}break;
	case PT_C_L_AUTO_AUTH:
		{
			if( !pkSession->m_kSessionData.Size() )
			{
				BM::GUID kAuthKey;
				if( pkPacket->Pop(kAuthKey) )
				{
					if( !pkPacket->RemainSize() )
					{
						g_kLoginUserMgr.ProcessAutoAuth( kAuthKey, pkSession );
						return;
					}
				}
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);

			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect Try Auth Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect Try Auth Session IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();//오류있음
		}break;
	case PT_C_L_REQ_REALM_LIST:
		{
			if( pkSession->m_kSessionData.Size() 
			&& !pkPacket->RemainSize()	) // Channel Select --> Realm Select (Back)
			{
				BM::Stream kPacket(PT_L_C_NFY_REALM_LIST);
				g_kRealmMgr.WriteToPacket(kPacket, 0, ERealm_SendType_Client_Realm);
				pkSession->VSend(kPacket);
				return;
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect PT_C_L_REQ_REALM_LIST Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect PT_C_L_REQ_REALM_LIST Session IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();
		}break;
	case PT_C_L_REQ_CHANNEL_LIST:
		{
			short nRealmNo = 0;

			if( pkPacket->Pop(nRealmNo)
			&&	nRealmNo
			&&	pkSession->m_kSessionData.Size() ) // Realm Select --> Channel Select (Go)
			{
				if(!pkPacket->RemainSize())
				{
					BM::Stream kPacket(PT_L_C_NFY_CHANNEL_LIST);
					g_kRealmMgr.WriteToPacket(kPacket, nRealmNo, ERealm_SendType_Client_Channel);
					pkSession->VSend(kPacket);
					return;
				}
			}

			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect PT_C_L_REQ_CHANNEL_LIST Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect PT_C_L_REQ_CHANNEL_LIST Session IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();
		}break;
	case PT_C_L_TRY_LOGIN:
		{	//	아이디,PW 인증만 걸고. // ID 인증은 누가 해주나..  -_-;. 
			short nTryLoginRealm = 0;
			short nTryLoginChannel = 0;

			if( pkPacket->Pop(nTryLoginRealm) 
			&&	pkPacket->Pop(nTryLoginChannel) 
			&&	pkSession->m_kSessionData.Size() )//CTL 값이 있는 세션만 TryLogin 할 수 있음.
			{
				if(!pkPacket->RemainSize())
				{
					SClientTryLogin kCTL;//TRY_AUTH 때 셋팅된 CTL 값.
					pkSession->m_kSessionData.PosAdjust();
					if(kCTL.ReadFromPacket(pkSession->m_kSessionData))
					{
						kCTL.RealmNo(nTryLoginRealm);
						kCTL.ChannelNo(nTryLoginChannel);
						if(g_kLoginUserMgr.ProcessLogin( kCTL, pkSession ) )
						{
							return;
						}
					}
				}
			}
//			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect PT_C_L_TRY_LOGIN Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect PT_C_L_TRY_LOGIN Session IP:") << pkSession->Addr().IP());
//			pkSession->VTerminate(); 얘는 끊으면 안됨 더블클릭 할 수도 있고 해서..
		}break;
	case PT_C_L_TRY_LOGIN_CHANNELMAPMOVE:
		{
			CAUTION_LOG( BM::LOG_LV0,	__FL__ << _T("Incorrect PT_C_L_TRY_LOGIN_CHANNELMAPMOVE Session IP:") << pkSession->Addr().IP());
			INFO_LOG( BM::LOG_LV0,		__FL__ << _T("Incorrect PT_C_L_TRY_LOGIN_CHANNELMAPMOVE Session IP:") << pkSession->Addr().IP());
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("User Send IncorrectPacket[") << usType << _T("] IP:") << pkSession->Addr().IP());
			pkSession->VTerminate();
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	}
}

