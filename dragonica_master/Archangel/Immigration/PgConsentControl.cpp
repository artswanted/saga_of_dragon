#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Constant.h"
#include "PgSendWrapper.h"
#include "PgWaitingLobby.h"
#include "PgConsentControl.h"

PgConsentControl::PgConsentControl()
:	m_bInit(false)
,	m_bConnectConsentServer(false)
,	m_u64ConsentAnsGreetingSendTime(_UI64_MAX)
{
}

PgConsentControl::~PgConsentControl()
{
}

void PgConsentControl::SetConnect(void)
{
	m_bConnectConsentServer = true;// Consent 서버에 접속이 되었다.

	if ( true == m_bInit )
	{
	//	이전에 접속된 적이 있으므로 Greeting을 보내게 시간을 1로 만든다.
		m_u64ConsentAnsGreetingSendTime = 1i64;
	}

	Check();
}

void PgConsentControl::SetDisConnect(void)
{
	m_bConnectConsentServer = false;// 접속이 종료되었다.
	m_u64ConsentAnsGreetingSendTime = _UI64_MAX;// 시간은 무한으로 만들어 Greeting을 보내지 않게 하자.

	if ( true == g_kWaitingLobby.UseJoinSite() )
	{
		g_kWaitingLobby.SetBlockedLogin( true );
	}
}

void PgConsentControl::UpdateCheckTime( bool const bEnd )
{
	if ( !m_bInit )
	{
		//	Contents서버의 접속을 기다리는중..
		if ( true == bEnd )
		{
			// Contents서버의 접속이 모두 완료되었으니까 Consent로 Greeting을 보내도 된다.
			m_u64ConsentAnsGreetingSendTime = 1i64;
		}
		else
		{
			// Contents서버가 접속유효시간을 기다려 준다.
			ACE_UINT64 u64WaitTime = 1000i64;

			int iSec = 60;// 기본은 60초
			g_kVariableContainer.Get(EVar_Kind_Login, EVAR_Login_Contents_WaitSecond, iSec );//
			u64WaitTime *= static_cast<ACE_UINT64>(iSec);
			m_u64ConsentAnsGreetingSendTime = BM::GetTime64() + u64WaitTime;
		}
	}
}

void PgConsentControl::Check(void)
{
	if ( m_u64ConsentAnsGreetingSendTime )
	{
		ACE_UINT64 const u64NowTime = BM::GetTime64();
		if ( m_u64ConsentAnsGreetingSendTime <= u64NowTime )
		{
			if ( true == g_kWaitingLobby.UseJoinSite() )
			{
				if ( true == m_bConnectConsentServer )
				{
					SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();

					BM::Stream kSendPacket(PT_A_S_ANS_GREETING);
					kSendSI.WriteToPacket(kSendPacket);
					kSendPacket.Push(g_kLocal.ServiceRegion());
					g_kWaitingLobby.Locked_WriteToPacket_LoginUserInfoToConsent(kSendPacket);
					::SendToConsentServer( kSendPacket );

					m_bInit = true;// OK
					m_u64ConsentAnsGreetingSendTime = 0i64;// 더이상 Check를 하지 않게 0으로 만들어 준다.
				}
			}
			else
			{
				g_kWaitingLobby.SetBlockedLogin(false);
			}
		}
	}
}

void PgConsentControl::HandleMessage(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

	ECONSENT_TASK_TYPE const eSecType = static_cast<ECONSENT_TASK_TYPE const>(pkMsg->SecType());
	switch (eSecType)
	{
	case ECONSENT_2ND_UPDATECHECKTIME:
		{
			bool bEnd;
			pkMsg->Pop(bEnd);
			UpdateCheckTime(bEnd);
		}break;
	case ECONSENT_2ND_SETCONNECT:
		{
			SetConnect();
		}break;
	case ECONSENT_2ND_SETDISCONNECT:
		{
			SetDisConnect();
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("Unknown Event type=") << eSecType);
		}break;
	}
}

void PgConsentControl::Locked_Timer10s(DWORD dwUSerData)
{
	BM::CAutoMutex kLock(m_kMutex);
	Check();
}