#ifndef CENTER_CENTERSERVER_CONTENTS_PGTASK_CONTETS_H
#define CENTER_CENTERSERVER_CONTENTS_PGTASK_CONTETS_H

#include "BM/PgTask.h"

class PgTask_ChannelContents
	:public PgTask<>
{
public:
	PgTask_ChannelContents();
	virtual ~PgTask_ChannelContents();

public:
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Close();

private:
};

#define g_kCenterTask SINGLETON_STATIC(PgTask_ChannelContents)

#endif // CENTER_CENTERSERVER_CONTENTS_PGTASK_CONTETS_H