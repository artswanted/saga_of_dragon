#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGACTIONEVENTPROCESS_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGACTIONEVENTPROCESS_H

#include "Lohengrin/packetstruct.h"
#include "variant/PgEventView.h"

class PgActionEventProcess : public PgAction_Callback
{
public:
	PgActionEventProcess(){}
	~PgActionEventProcess(){}
	virtual void DoAction(CONT_EVENT_STATE::mapped_type &,PgPlayer *);
};

class PgActionCouponEvent
{
public:
	explicit PgActionCouponEvent(E_USER_EVENT_TYPE const,PgPlayer *);
	~PgActionCouponEvent(){}
private:
	PgActionEventProcess m_kEventProcess;
};

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGACTIONEVENTPROCESS_H