#ifndef MACHINE_MMC_PGSITECONTROL_H
#define MACHINE_MMC_PGSITECONTROL_H

typedef struct tagEx2SingleServerStatus
	:	public SExSingleServerStatus
{
	tagEx2SingleServerStatus()
		:	pkSMCSession(NULL)
	{}

	void SetDisconnected(void)
	{
		bReadyToService = false;
		uiMaxUsers = 0;
		uiConnectionUsers = 0;
	}

	CEL::CSession_Base *pkSMCSession;
}SEx2SingleServerStatus;
typedef std::map< SERVER_IDENTITY, SEx2SingleServerStatus > CONT_SERVER_STATE_EX2;

typedef std::vector< SERVER_IDENTITY > CONT_SERVER_ID;
typedef enum eServerStateType
{
	ST_TYPE_CODE = 100,
	ST_AUTH_SERVER = CEL::ST_IMMIGRATION,
	ST_MAIN_SERVER = CEL::ST_CONTENTS,
	ST_GAME_SERVER = CEL::ST_MAP,
	ST_LOG_SERVER = CEL::ST_LOG,
	ST_CONN_USER_COUNT = 1000,
	ST_PLAY_USER_COUNT = 1001,
	ST_LOGIN_USER_COUNT = 1002,
	ST_CHANNEL_CODE = 1003,
	ST_CHANNEL_NAME = 1004,
	ST_CHANNEL_STATE = 1005,
	ST_CHANNEL_USER_COUNT = 1006,
	ST_MACHINE_NAME = 1007,
	ST_MACHINE_SERVER = 1008,
	ST_WORLD_ID = 1009,
	ST_WORLD_NAME = 1010,
}EServerStateType;

typedef struct tagChannelIdentity
{
	tagChannelIdentity()
		:	nRealm(0)
		,	nChannel(0)
	{
	}
	tagChannelIdentity(SERVER_IDENTITY const &rkSI)
	{
		nRealm = rkSI.nRealm;
		nChannel = rkSI.nChannel;
	}

	bool operator < (tagChannelIdentity const &rhs) const
	{
		if( nRealm < rhs.nRealm )	{return true;}
		if( nRealm > rhs.nRealm )	{return false;}

		if( nChannel < rhs.nChannel )	{return true;}
		if( nChannel > rhs.nChannel )	{return false;}

		return false;
	}

	bool operator == (tagChannelIdentity const &rhs) const
	{
		return		(nRealm == rhs.nRealm)
				&&	(nChannel == rhs.nChannel);
	}

	bool operator != (tagChannelIdentity const &rhs) const
	{
		return !(*this == rhs);
	}

	short nRealm;
	short nChannel;
}SChannelIdentity;

typedef std::map< SChannelIdentity, SEx2SingleServerStatus > CONT_CHANNELSTATE;
typedef std::map< short, BM::vstring > CONT_TYPENAME;
typedef std::map< short, int > CONT_SERVERSTATE;

class PgSiteControl
{
public:
	PgSiteControl( int const iDBIndex );
	~PgSiteControl(void);

	bool InitRealmMgr();

	void SetRealmCandi( CONT_REALM_CANDIDATE const &kRealmCandi ){m_kRealmCandi=kRealmCandi;}
	void GetRealmCandi( CONT_REALM_CANDIDATE &rkOutRealmCandi)const{rkOutRealmCandi=m_kRealmCandi;}

	bool AddServerInfo( TBL_SERVERLIST const &rkInfo );
	void CheckServerInfo(void);

	void GetServerList( short const nServerType, ContServerSiteID &rkOut )const;
	void GetServerHash( CONT_SERVER_HASH &rkOutServerHash )const;
	void GetPatchServerList( CEL::CSession_Base *pkSMCSession, ContServerID &rkOutID )const;

	void Connect( BM::GUID const &kConnecterID );
	bool SetConnect( CEL::CSession_Base* pkSession, bool const bConnect, SERVER_IDENTITY &rkSI );
	bool SetConnectSMC( CEL::CSession_Base* pkSession, SERVER_IDENTITY_SITE const &kRecvSI, bool const bConnect );
	bool SetSyncEndSMC( CEL::CSession_Base* pkSession );

	void WriteToPacket_MonTool( BM::Stream &rkPacket )const;
	
	bool WriteToPacket_AdminGateway( BM::Stream &rkPacket, const int iRealmNo );
	bool GetTotalServerState( const int iRealmNo );
	
	bool AddTotalServerInfo(BM::vstring &rkOut);
	bool AddChannelServerInfo(BM::vstring &rkOut);
	bool AddMachineServerInfo(BM::vstring &rkOut);

	bool GetTypeName(EServerStateType const eType, BM::vstring &rkOut);
	bool GetTypeValue(EServerStateType const eType, int &riValue);
	bool AddTypeInfo(EServerStateType const eType, BM::vstring &rkOut);
	bool AddChannelInfo(SChannelIdentity const &rkChannelSI, SEx2SingleServerStatus const &rkServerInfo, BM::vstring &rkOut);


	void ServerControl( CEL::SESSION_KEY const &kCmdOwner, ContServerID const &kCommandVec, EMMC_CMD_TYPE const eCmdType );
	bool ServerControlRet( SERVER_IDENTITY const kServerIdentity, EMMC_CMD_TYPE const eCmdType );
	bool Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND( CEL::CSession_Base *pkSession, E_IMM_MCC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket, CONT_SERVER_ID& rkContServerID );
	void RefreshState( BM::Stream * const pkPacket );

	bool CheckHeartBeat( DWORD const dwCurTime, DWORD const dwMaxTime, CONT_SERVER_ID &rkContServerId );

	void BroadCast_SMC( BM::Stream const &rkPacket );
	void Disconnect_SMC();
	void Clear_SMC();
	bool SendToImmigration( BM::Stream const &rkPacket )const;
	bool IsRefresh() { return m_bRefresh; }

protected:
	int	const				m_iDBIndex;

	bool					m_bReqConnect;
	CEL::ADDR_INFO			m_kImmAddr;
	CEL::CSession_Base*		m_pkSession;

	CONT_REALM_CANDIDATE	m_kRealmCandi;
	PgRealmManager			m_kRealmMgr;

	CONT_SERVER_STATE_EX2	m_kTotalServerState;

	bool					m_bRefresh;

	CONT_TYPENAME m_kContTypeName;
	CONT_SERVERSTATE m_kContServerState;
	CONT_CHANNELSTATE m_kContChannelState;
private:
	PgSiteControl(void);
};

inline bool PgSiteControl::SendToImmigration( BM::Stream const &rkPacket )const
{
	if ( m_pkSession )
	{
		return m_pkSession->VSend( rkPacket );
	}
	return false;
}

#endif // MACHINE_MMC_PGSITECONTROL_H