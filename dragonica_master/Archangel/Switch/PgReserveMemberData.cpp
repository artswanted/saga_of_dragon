#include "stdafx.h"
#include "PgReserveMemberData.h"

int PgReserveMemberData::ms_iSpeedCheckGap = 5000;

PgReserveMemberData::PgReserveMemberData(void)
:	m_bPublicConnect(false)
,	m_pkSession(NULL)
,	m_bWaitAntiHackCheck(false)
,	m_dwLastAntiHackCheckTime(0)
,	m_dwNextAntiHackCheckTime(0)
{
	m_kSendCount = 0;
}

PgReserveMemberData::PgReserveMemberData( SSwitchPlayerData const &rkSPD )
:	SSwitchPlayerData(rkSPD)
{
	PgReserveMemberData();
}

PgReserveMemberData::~PgReserveMemberData(void)
{}

PgReserveMemberData& PgReserveMemberData::operator = ( tagReqSwitchReserveMember const &rhs)
{
	SSwitchPlayerData::operator = ( rhs ) ;
	return *this;
}

void PgReserveMemberData::WriteToPacket( BM::Stream &rkPacket )const
{
	SSwitchPlayerData::WriteToPacket( rkPacket );
}

bool PgReserveMemberData::ReadFromPacket( BM::Stream &rkPacket )
{
	return SSwitchPlayerData::ReadFromPacket( rkPacket );
}

///////
PgReserveMemberData_Dummy::PgReserveMemberData_Dummy()
	: PgReserveMemberData()
{
}

PgReserveMemberData_Dummy::PgReserveMemberData_Dummy(SSwitchPlayerData const& rkSPD)
	: PgReserveMemberData(rkSPD)
{
}

HRESULT PgReserveMemberData_Dummy::MakePacket_CheckAntiHack(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return S_OK; // 무조건 체크 하자
}

HRESULT PgReserveMemberData_Dummy::CheckAntiHack(BM::Stream &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return ERROR_SUCCESS;
}

bool PgReserveMemberData_Dummy::IsCheckAntiHack() const	
{
	return 0 == byGMLevel;
}



///////
PgReserveMemberData_GameGuard::PgReserveMemberData_GameGuard(void)
	: PgReserveMemberData(), m_kCsa()
{
	m_kCsa.Init();
}

PgReserveMemberData_GameGuard::PgReserveMemberData_GameGuard( SSwitchPlayerData const &rkSPD )
:	PgReserveMemberData(rkSPD), m_kCsa()
{
	m_kCsa.Init();
}

PgReserveMemberData_GameGuard::~PgReserveMemberData_GameGuard(void)
{

}

HRESULT PgReserveMemberData_GameGuard::MakePacket_CheckAntiHack( BM::Stream &rkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( true == IsCheckAntiHack() )
	{
		unsigned int const iRet = m_kCsa.GetAuthQuery();
		if( ERROR_SUCCESS == iRet )
		{	
			GG_AUTH_DATA ggData;
			::memcpy( &ggData, &(m_kCsa.m_AuthQuery), sizeof(GG_AUTH_DATA) );
			
			if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
			{
				INFO_LOG(BM::LOG_LV1, L" GG Make: " << ggData.dwIndex << L" " << ggData.dwValue1 << L" " << ggData.dwValue2 << L" " << ggData.dwValue3);
			}

			rkPacket.Push(ggData);
		}
		return iRet;
	}
	return E_FAIL;
}

HRESULT PgReserveMemberData_GameGuard::CheckAntiHack( BM::Stream &rkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	GG_AUTH_DATA ggData;
	if( true == rkPacket.Pop(ggData) )
	{
		::memcpy( &(m_kCsa.m_AuthAnswer), &ggData, sizeof(GG_AUTH_DATA) );

		if( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
		{
			INFO_LOG(BM::LOG_LV1, L" GG Auth: " << ggData.dwIndex << L" " << ggData.dwValue1 << L" " << ggData.dwValue2 << L" " << ggData.dwValue3);
		}

		return m_kCsa.CheckAuthAnswer();
	}
	return E_FAIL;
}

///////
PgReserveMemberData_AhnHackShield::PgReserveMemberData_AhnHackShield(void)
:	m_kHackShieldHandle(ANTICPX_INVALID_HANDLE_VALUE)
{
}

PgReserveMemberData_AhnHackShield::PgReserveMemberData_AhnHackShield( SSwitchPlayerData const &rkSPD )
:	PgReserveMemberData(rkSPD)
{
	PgReserveMemberData_AhnHackShield();
}

PgReserveMemberData_AhnHackShield::~PgReserveMemberData_AhnHackShield(void)
{
	if ( ANTICPX_INVALID_HANDLE_VALUE != m_kHackShieldHandle )
	{
		::_AhnHS_CloseClientHandle( m_kHackShieldHandle );
	}
}

HRESULT PgReserveMemberData_AhnHackShield::MakePacket_CheckAntiHack( BM::Stream &rkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( true == IsCheckAntiHack() )
	{
		AHNHS_TRANS_BUFFER kBuffer;
		unsigned long const ulRet = ::_AhnHS_MakeRequest( m_kHackShieldHandle, &kBuffer );
		if ( ERROR_SUCCESS == ulRet )
		{
			if ( kBuffer.nLength <= ANTICPX_TRANS_BUFFER_MAX )
			{
				rkPacket.Push( kBuffer.nLength );
				rkPacket.Push( kBuffer.byBuffer, static_cast<size_t>(kBuffer.nLength) );
				return ERROR_SUCCESS;
			}
		}
		return static_cast<HRESULT>(ulRet);
	}
	return E_FAIL;
}

HRESULT PgReserveMemberData_AhnHackShield::CheckAntiHack( BM::Stream &rkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	AHNHS_TRANS_BUFFER kBuffer;
	kBuffer.nLength = 0;

	rkPacket.Pop( kBuffer.nLength );
	if ( kBuffer.nLength <= ANTICPX_TRANS_BUFFER_MAX )
	{
		if ( true == rkPacket.PopMemory( kBuffer.byBuffer, static_cast<size_t>(kBuffer.nLength) ) )
		{
			unsigned long ulErrorCode = ERROR_SUCCESS;
			if ( ANTICPX_RECOMMAND_KEEP_SESSION == ::_AhnHS_VerifyResponseEx( m_kHackShieldHandle, kBuffer.byBuffer, kBuffer.nLength, &ulErrorCode ) )
			{
				return ERROR_SUCCESS;
			}
			return ulErrorCode;
		}
	}
	return E_FAIL;
}

HRESULT PgReserveMemberData::DoSendPing(int const iCallType)
{
	BM::CAutoMutex kLock(m_kMutex);

	HRESULT hRet = S_OK;
	SSwitchPingInfo const kPrevSendPing = m_kLastSendPing;
	
	ACE_Time_Value kNowAceTime = ACE_OS::gettimeofday();
	static const ACE_Time_Value kGabAceTime(5,0);
	
	if(	15 > m_kSendCount )	// 15개 이하로 쌓여야 하고.
	{
		if(	kNowAceTime >= (kPrevSendPing.m_kAceTime + kGabAceTime) )//5초가 지났어야함.
		{
			if( m_pkSession )
			{
				SSwitchPingInfo kNewPing;
				kNewPing.Build();

				BM::Stream kPingPacket(PT_S_C_REQ_PING);
				kPingPacket.Push( kNewPing.m_kGuid );

				if( !m_pkSession->VSend(kPingPacket) )
				{
					CAUTION_LOG(BM::LOG_LV0, _T("DoSendPing Can't Send. ID: ") << ID() << _T(" CallType:") << iCallType);
					hRet = E_FAIL;
				}
				else
				{
					++m_kSendCount;
					m_kLastSendPing = kNewPing;
					m_kContPingSendTime.push_back( m_kLastSendPing );
				}
			}
		}
	}
	else
	{// 기준 수량보다 많이 쌓였음
		hRet = E_FAIL;
	}

	return hRet;
}

HRESULT PgReserveMemberData::RecvPing(BM::GUID const &kRecvPingGuid /*, DWORD const &dwClientTime*/)
{
	//Ver1. 함수 중간에 리턴을 만들지 말 것.
	//Ver2. 보낸 핑들 중에서 모든것을 체크 한다 (중간에 return 허용)
	BM::CAutoMutex kLock(m_kMutex);
//	INFO_LOG(BM::LOG_LV1, _T("Ping Recv User 1[") << ID() << _T("] Last Ping Before ") );
	CONT_PING_INFO::iterator ping_iter = m_kContPingSendTime.end();
	if( !m_kContPingSendTime.empty() )
	{
		//룰. 제일 앞에 있는 것을 받았어야 한다.
		DWORD const dwNowServerTime = BM::GetTime32();
		ping_iter = m_kContPingSendTime.begin();
		while( m_kContPingSendTime.end() != ping_iter )
		{
			SSwitchPingInfo const &rkSrcPingInfo = (*ping_iter);
			::GUID kSrcGuid = rkSrcPingInfo.m_kGuid;
			::GUID kRcvGuid = kRecvPingGuid;
			DWORD const dwClientTime = kRcvGuid.Data1 ^ kSrcGuid.Data1; // 클라 시간 복구
			kRcvGuid.Data1 ^= dwClientTime; // GUID복구

			if(	rkSrcPingInfo.m_kGuid == kRcvGuid )//복구한GUID 사용
			{
				//마지막 받은 Ping 정보와 비교 하는것이다.
	//			int const dwServerTimeGab = kRecvPingInfo.dwServerTime - kRecvPingInfo.dwServerTime;
	//			int const dwClientTimeGab = kRecvPingInfo.dwClientTime - kRecvPingInfo.dwClientTime;

	//			문제는 최초 핑이 느리다는것.. ==> ACCESS_SWITCH 에서 핑 날리는 것으로 해결.

	//			최초 핑으로. 클라 첫 시간값을 유추.
	//			이후 핑은 첫 핑 시작 시간(서버/클라) 로 계산하여 길이 비교. 

				if(m_kPingStartLine.IsEmpty())
				{//최초 핑 오기전엔 다른 패킷이 오면 무조건 무시 시키겠음.
					m_kPingStartLine = rkSrcPingInfo;//데이터 기록.
					m_kPingStartLine.dwClientTime = dwClientTime;//클라 타임은 따로 기록.

					//보정 작업 필요함.
					DWORD const dwPingTime = dwNowServerTime - rkSrcPingInfo.dwServerTime;
					m_kPingStartLine.dwClientTime -= (dwPingTime/2);//핑 시작 시간 보정. 왔다 갔다 이므로 1/2.
				}
				else
				{//timegettime이 DWORD 라서. 값이 한번 순회 하면(49.1일) 0 부터 시작이므로 int 로 변환해서 계산.
					int const iLastTimeGab = (int)dwClientTime - (int)m_kLastRecvPing.dwClientTime;

					if(-10 > iLastTimeGab)//timegettime의 오차값이 최대 5ms 이기 때문.
					{
						HACKING_LOG(BM::LOG_LV0, L"Incorrect Time Case 3 - TimeRollBack: User["<< ID() << _T("] LastTime:") << m_kLastRecvPing.dwClientTime << _T(" NowTime:")<< dwClientTime << _T(" Gab:") << iLastTimeGab);
						m_kContPingSendTime.erase(ping_iter);
						return E_FAIL;
					}

					int const iServerTimeGab	= (int)dwNowServerTime - (int)m_kPingStartLine.dwServerTime;
					int const iClientTimeGab	= (int)dwClientTime - (int)m_kPingStartLine.dwClientTime;

					if( 0 > iClientTimeGab //타입 갭이 0 보다 작을 수 없음. (시간이 뒤돌아 갔다)
					||	(	iClientTimeGab > iServerTimeGab //클라이언트가 서버보다 갭이 크고(시간 흐르는게 빠르고)
						&&	iClientTimeGab - iServerTimeGab > ms_iSpeedCheckGap)//
					)
					{//클라가 5초 먼저갔기때문에 끊는다.
						int const iTempGab = iClientTimeGab - iServerTimeGab;
						HACKING_LOG(BM::LOG_LV0, L"Incorrect Time Case 1 - ClientFast: User["<< ID() << _T("] Client:") << iClientTimeGab << _T(" Server:")<< iServerTimeGab << _T(" Gab:") << iTempGab);
						m_kContPingSendTime.erase(ping_iter);
						return E_FAIL;
					}

					if( iServerTimeGab > iClientTimeGab
					&&	iServerTimeGab  - iClientTimeGab > ms_iSpeedCheckGap )
					{//서버가 5초 먼저갔으면 랙이다.
						int const iTempGab = iServerTimeGab  - iClientTimeGab;
						INFO_LOG(BM::LOG_LV0, L"Incorrect Time Case 2 - Network Lag: User["<< ID() << _T("] Client:") << iClientTimeGab << _T(" Server:")<< iServerTimeGab << _T(" Gab:") << iTempGab);
					}
				}

				m_kLastRecvPing = rkSrcPingInfo;
				m_kLastRecvPing.dwClientTime = dwClientTime;//클라 타임은 따로 기록.
				m_kLastRecvPing.dwPingTime = ((dwNowServerTime - rkSrcPingInfo.dwServerTime)/2);
				break; // 반드시 중단시켜야 한다
			}

			++ping_iter;
		}
	}

	if( m_kContPingSendTime.end() != ping_iter )
	{
		m_kContPingSendTime.erase(ping_iter); //pop 후에 kInfo 를 쓰면 안됨. 이걸 위로 끌어올리지 말것.
		m_kSendCount = std::max(--m_kSendCount, 0);
		return S_OK;
	}
	HACKING_LOG(BM::LOG_LV3, _T("Recv Incorrect Ping User:")<< ID() << L" Ping Guid:" << kRecvPingGuid);
	return E_FAIL;
}

bool PgReserveMemberData::DropLastRecvPing(BM::GUID const &kRecvGuid, DWORD &dwOutLatency)
{//NFY_POS 를 여러번 쏘지 못하게 하는 방어.
	BM::CAutoMutex kLock(m_kMutex);

	if(	m_kLastRecvPing.m_kGuid.IsNotNull()
	&&	m_kLastRecvPing.m_kGuid ==  kRecvGuid)
	{
		m_kLastRecvPing.m_kGuid.Clear();
		dwOutLatency = m_kLastRecvPing.dwPingTime;
		return true;
	}
	HACKING_LOG(BM::LOG_LV3, _T("Recv Incorrect Chacacter Pos:") << ID() );
	return false;
}
