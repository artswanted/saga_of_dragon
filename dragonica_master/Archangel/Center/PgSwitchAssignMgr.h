#ifndef CENTER_CENTERSERVER_SERVERSET_PGSWITCHASSIGNMGR_H
#define CENTER_CENTERSERVER_SERVERSET_PGSWITCHASSIGNMGR_H

// ------------------------------
// PgServerSetMgr -> PgSwitchAssignMgrёё ИЈГв °ЎґЙ
// °ЕІЩ·О ЗПёй DEAD LOCK
// ------------------------------

class PgSwitchAssignMgr
{
public:
	typedef std::map< SERVER_IDENTITY, SET_GUID >		CONT_SWITCH_ASSIGN;

public:
	PgSwitchAssignMgr();
	virtual ~PgSwitchAssignMgr();

	bool ConnectSwitch( SERVER_IDENTITY const &kSI );
	bool DisconnectSwitch( SERVER_IDENTITY const &kSI );

	bool GetBalanceSwitch( SERVER_IDENTITY &kOutSI, bool const bIsIgnoreMax)const;
	bool InsertAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid );
	bool RemoveAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid );

	bool SendToUser( BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const;

	CLASS_DECLARATION( size_t, m_iMaxPlayerCount, MaxPlayerCount );
	size_t NowPlayerCount()const{return m_iNowPlayerCount;}

protected:

private:
	void _RefreshUserCount();

private:
	size_t					m_iNowPlayerCount;

	CONT_SWITCH_ASSIGN		m_kContSwitchServer;
};

class PgSwitchAssignMgr_Wrapper
	:	public TWrapper< PgSwitchAssignMgr >
{
public:
	PgSwitchAssignMgr_Wrapper(){}
	virtual ~PgSwitchAssignMgr_Wrapper(){}

	bool ConnectSwitch( SERVER_IDENTITY const &kSI );
	bool DisconnectSwitch( SERVER_IDENTITY const &kSI );

	bool GetBalanceSwitch( SERVER_IDENTITY &kOutSI, bool const bIsIgnoreMax)const;
	bool InsertAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid );
	bool RemoveAssignSwitch( SERVER_IDENTITY const &kSI, BM::GUID const &kMemberGuid );

	bool SendToUser( BM::GUID const &kMemberGuid, BM::Stream const &kPacket )const;

	void MaxPlayerCount( size_t const iMaxPlayerCount );
	size_t MaxPlayerCount()const;
	size_t NowPlayerCount()const;
};

#define g_kSwitchAssignMgr SINGLETON_STATIC(PgSwitchAssignMgr_Wrapper)

#endif // CENTER_CENTERSERVER_SERVERSET_PGSWITCHASSIGNMGR_H