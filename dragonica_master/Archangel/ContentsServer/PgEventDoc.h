#ifndef CONTENTS_CONTENTSSERVER_ETC_PGEVENTDOC_H
#define CONTENTS_CONTENTSSERVER_ETC_PGEVENTDOC_H

#include "ace/time_value.h"
#include "Variant/idobject.h"
#include "Variant/PgEventView.h"

class PgEventDoc//데이터 관리
{
public:
	PgEventDoc();
	virtual ~PgEventDoc();

public:	
	bool RefreshTbl(const CONT_TBL_EVENT &rKCont);//기존 빌드 정보 필요
	void WriteToPacket(BM::Stream &kPacket)const;

protected:
	void DoNfyEvent();

protected:
	CONT_TBL_EVENT m_kContEvent;

private:
	mutable Loki::Mutex m_kMutex;
};

#define g_kEventDoc SINGLETON_STATIC(PgEventDoc)

#endif // CONTENTS_CONTENTSSERVER_ETC_PGEVENTDOC_H