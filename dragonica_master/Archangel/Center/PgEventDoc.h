#ifndef CENTER_CENTERSERVER_ETC_PGEVENTDOC_H
#define CENTER_CENTERSERVER_ETC_PGEVENTDOC_H

#include "ace/time_value.h"
#include "Variant/idobject.h"
#include "Variant/PgEventView.h"

class PgEventDoc//µҐАМЕН °ьё®
{
public:
	PgEventDoc();
	virtual ~PgEventDoc();

public:	
	bool RefreshTbl(const CONT_TBL_EVENT &rKCont);//±вБё єфµе Б¤єё ЗКїд
	void WriteToPacket(BM::Stream &kPacket)const;

protected:
	void DoNfyEvent();

protected:
	CONT_TBL_EVENT m_kContEvent;
	
};

#define g_kEventDoc SINGLETON_STATIC(PgEventDoc)

#endif // CENTER_CENTERSERVER_ETC_PGEVENTDOC_H