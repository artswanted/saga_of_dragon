#ifndef CONSENT_CONSENTSERVER_LINKAGE_GF_PGMSGWORKER_H
#define CONSENT_CONSENTSERVER_LINKAGE_GF_PGMSGWORKER_H

#include "BM/ObjectPool.h"
#include "BM/PgMsgWorker.h"
#include "BM/PgMsgDispatcher.h"

typedef struct tagMsg
{
	typedef std::set<BM::GUID> CONT_WORKDATA_TARGET;

	tagMsg(){}
	explicit tagMsg( BM::GUID const &kSessionID )
		:	m_kSessionID(kSessionID)
	{}
	~tagMsg(){}

	void Clear()
	{
		m_kSessionID.Clear();
		m_kContTarget.clear();
		m_kPacket.Clear();
	}

	void InsertTarget(BM::GUID const &rkGuid)
	{
		m_kContTarget.insert(rkGuid);
	}

	BM::GUID				m_kSessionID;
	CONT_WORKDATA_TARGET	m_kContTarget;
	BM::Stream				m_kPacket;
}SMsg;

class PgPacketWorker
	: public PgMsgWorker<SMsg>
{
public :
	PgPacketWorker();
	virtual ~PgPacketWorker();

	virtual HRESULT VProcess( SMsg *pkMsg );
};

class PgPacketDisPatcher
	:	public PgMsgDispatcher< SMsg, PgPacketWorker >
{
public:
	PgPacketDisPatcher(){}
	~PgPacketDisPatcher(){}
};

#define g_kMsgDisPatcher SINGLETON_STATIC( PgPacketDisPatcher )

#endif // CONSENT_CONSENTSERVER_LINKAGE_GF_PGMSGWORKER_H