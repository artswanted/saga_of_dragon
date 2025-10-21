#ifndef CONSENT_CONSENTSERVER_LINKAGE_GF_BILLING_PGBILLINGAPSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_GF_BILLING_PGBILLINGAPSERVER_H

#pragma pack(1)

#include "PgGFServer.h"

typedef enum
{
	REQ_ORDER_SUCCESS				= S_OK,
	REQ_ORDER_SERVERERROR			= E_HANDLE,
	REQ_ORDER_DATAERROR_STRINGSIZE	= E_ABORT,
	REQ_ORDER_DATAERROR_PACKETSIZE	= E_FAIL,	
}E_REQUEST_ORDER_RET;

typedef struct tagLinkageServerInfo
{
	tagLinkageServerInfo()
	{}

	bool IsAccess( CEL::ADDR_INFO const &_kAddr, std::wstring const &_wstrID, std::wstring const &_wstrPW )const
	{
		if ( _kAddr.ip.s_addr == kAddr.ip.s_addr )
		{
			if ( _wstrID == wstrID )
			{
				if ( _wstrPW == wstrPW )
				{
					return true;
				}
			}
		}
		return false;
	}

	CEL::ADDR_INFO			kAddr;
	std::wstring			wstrID;
	std::wstring			wstrPW;
}SLinkageServerInfo;

#pragma pack()

class PgBillingAPServer
{
public:
	static DWORD const ms_dwMaxDelayPingTime = 40 * 1000;// 40초

public:
	PgBillingAPServer(void);
	virtual ~PgBillingAPServer(void);

	static void CALLBACK OnAcceptFromBillingAP( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisConnectFromBillingAP( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromBillingAP( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );


	void Locked_OnAcceptServer( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectServer( CEL::CSession_Base *pkSession );
	HRESULT Locked_OnRecvFromImmigration( BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream &kPacket );
	bool Locked_Send( BM::Stream const &kPacket )const;

protected:
	void RecvFromServer( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket );
	template< typename T_RECV >
	int ProcessPacketAction( int const iType, BM::Stream &rkPacket, T_RECV &kRecvData )const
	{
		if ( true == kRecvData.ReadFromPacket( rkPacket ) )
		{
			if ( 0 == rkPacket.RemainSize() )
			{
				typename T_RECV::REQ_ORDER_TYPE kReqData( m_pkServerSession->SessionKey().SessionGuid() );
				int iErr = GFAP_RET_SUCCESS;

				if ( true == kRecvData.CopyTo( kReqData, iErr ) )
				{
					BM::Stream kPacket( PT_AP_IM_REQ_ORDER, iType );
					kReqData.WriteToPacket( kPacket );
					if ( true == g_kGFServer.Locked_SendToImmigrationOne( kPacket ) )
					{
						return GFAP_RET_SUCCESS;
					}

					CAUTION_LOG( BM::LOG_LV5, _T("[RecvAPPacket] Ready Immigration Count is 0 Type<") << iType << _T(">") );
					return GFAP_RET_SERVERERROR;
				}

				CAUTION_LOG( BM::LOG_LV5, _T("[RecvAPPAcket] String Size Error Type<") << iType << _T(">") );
				return iErr;
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, _T("[RecvAPPacket] Packet Size Error Type<") << iType << _T(">") );
			}
		}
		return GFAP_RET_VALUEERROR;
	}

	template< typename T_REPLY >
	HRESULT Locked_ProcessReplyOrder( BM::Stream::DEF_STREAM_TYPE const kReplyPacketType, BM::GUID const &kOrderID, BM::Stream &rkPacket )
	{
		typename T_REPLY::REPLY_ORDER_TYPE kRecvOrder;
		if ( true == kRecvOrder.ReadFromPacket( rkPacket) )
		{
			T_REPLY kReplyOrder( kOrderID, kRecvOrder );

			BM::Stream kPacket( kReplyPacketType );
			kReplyOrder.WriteToPacket( kPacket );

			Locked_Send(kPacket);
			return S_OK;
		}
		return E_FAIL;
	}

protected:
	bool ReadFromConfig( LPCTSTR lpszFileName );

private:
	mutable Loki::Mutex		m_kMutex;
	SLinkageServerInfo		m_kServerInfo;
	bool					m_bAuthSuccess;
	DWORD					m_dwLastRecvPingTime;
	CEL::CSession_Base*		m_pkServerSession;
};

inline bool PgBillingAPServer::Locked_Send( BM::Stream const &kPacket )const
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( m_pkServerSession )
	{
		return m_pkServerSession->VSend( kPacket );
	}
	return false;
}
#define g_kBillingAPServer SINGLETON_STATIC(PgBillingAPServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_GF_BILLING_PGBILLINGAPSERVER_H