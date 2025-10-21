#ifndef CONTENTS_CONTENTSSERVER_SERVERSET_JOBWORKER_H
#define CONTENTS_CONTENTSSERVER_SERVERSET_JOBWORKER_H

#include "BM\PgMsgWorker.h"
#include "PgServerSetMgr.h"

class PgJobWorker
	: public PgMsgWorker<SActionOrder>
{
public :
	PgJobWorker();
	virtual ~PgJobWorker();

	virtual HRESULT VProcess(SActionOrder *pkJob);

	virtual SActionOrder* Alloc(){return AllocJob();}
	virtual void Free( SActionOrder *& pkJob );

	static SActionOrder* AllocJob();
	static void FreeJob( SActionOrder *& pkJob );
private:
	typedef BM::TObjectPool< SActionOrder > ActionJobPool;
	static ActionJobPool ms_kJobPool;
};

#endif // CONTENTS_CONTENTSSERVER_SERVERSET_JOBWORKER_H