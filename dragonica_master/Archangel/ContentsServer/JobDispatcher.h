#ifndef CONTENTS_CONTENTSSERVER_SERVERSET_JOBDISPATCHER_H
#define CONTENTS_CONTENTSSERVER_SERVERSET_JOBDISPATCHER_H

#include "BM/PgMsgDispatcher.h"
#include "JobWorker.h"
#include "PgServerSetMgr.h"

class PgJobDispatcher
	: public PgMsgDispatcher<SActionOrder, PgJobWorker>
{
	friend struct ::Loki::CreateStatic< PgJobDispatcher >;
public :
	PgJobDispatcher();
	virtual ~PgJobDispatcher();

	virtual HRESULT VPush( SActionOrder const * pkJob, unsigned long priority = 0 );
};

#define g_kJobDispatcher SINGLETON_STATIC(PgJobDispatcher)

#endif // CONTENTS_CONTENTSSERVER_SERVERSET_JOBDISPATCHER_H