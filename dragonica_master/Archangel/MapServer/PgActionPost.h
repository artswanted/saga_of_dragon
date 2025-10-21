#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTIONPOST_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTIONPOST_H

class PgPostReqMailSend
	:	public PgUtilAction
{
public:
	
	PgPostReqMailSend(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgPostReqMailSend(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	EPostMailSendResult OnProcess(CUnit* pkCaster);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgPostReqMailRecv
	: public PgUtilAction
{
public:

	PgPostReqMailRecv(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgPostReqMailRecv(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgPostReqMailModify
	: public PgUtilAction
{
public:

	PgPostReqMailModify(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgPostReqMailModify(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTIONPOST_H