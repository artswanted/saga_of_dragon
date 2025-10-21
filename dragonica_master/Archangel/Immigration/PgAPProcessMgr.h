#ifndef IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGAPPROCESSMGR_H
#define IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGAPPROCESSMGR_H

class PgAPProcessMgr
{
public:
	PgAPProcessMgr(void);
	~PgAPProcessMgr(void);

	typedef enum
	{
		EAP_2ND_NONE = 0,
		EAP_2ND_PACKET = 1,
		EAP_2ND_DBRESULT = 2,
	} EAP_TASK_TYPE;	// SEventMessage::SecondaryType

	void HandleMessage(SEventMessage *pkMsg);
protected:
	HRESULT ProcessOrder( BM::Stream &rkPacket );
	void ProcessPT_GF_AP_REQ_SET_TABLEDATA( BM::Stream * const pkPacket );
	bool Send( BM::Stream const &kPacket )const;

	//DataBase
	void Q_DQT_AP_CHECK_ACCOUNT( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_CREATE_ACCOUNT( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_GET_CASH( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_ADD_CASH( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_MODIFY_PASSWORD( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_CREATE_COUPON( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_MODIFY_MOBILELOCK( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_AP_TABLE_CONTROL( CEL::DB_RESULT &rkResult )const;
	void Q_DQT_TRY_AUTH_CHECKPW_AP( CEL::DB_RESULT &rkResult )const;

	template< typename T_ORDERBASE, typename T_REPLY >
	void SendToResult( int const iOrderType, T_ORDERBASE const &kBase, T_REPLY const &kReply )const
	{
		BM::Stream kPacket( PT_IM_AP_ANS_ORDER );
		kBase.WriteToPacket( kPacket );
		kPacket.Push( iOrderType );
		kReply.WriteToPacket( kPacket );
		Send( kPacket );
	}
};

inline bool PgAPProcessMgr::Send( BM::Stream const &kPacket )const
{
	return ::SendToConsentServer( kPacket );
}

//#define g_kAPProcessMgr SINGLETON_STATIC( PgAPProcessMgr )

#endif // IMMIGRATION_IMMIGRATIONSERVER_TASK_OBJ_PGAPPROCESSMGR_H