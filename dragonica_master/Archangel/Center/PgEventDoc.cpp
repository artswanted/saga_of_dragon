#include "stdafx.h"
#include "PgEventDoc.h"
#include "Lohengrin/GameTime.h"

PgEventDoc::PgEventDoc()
{
}

PgEventDoc::~PgEventDoc()
{
}

bool PgEventDoc::RefreshTbl(const CONT_TBL_EVENT &rKCont)//±вБё єфµе Б¤єё ЗКїд
{
	m_kContEvent = rKCont;
	DoNfyEvent();
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgEventDoc::WriteToPacket(BM::Stream &kPacket)const
{//EventView їЎј­ ReadFrom ЗФ.
	PU::TWriteTable_MM(kPacket, m_kContEvent);
}

void PgEventDoc::DoNfyEvent()
{	
	BM::Stream kPacket(PT_T_A_EVENT_SYNC);
	WriteToPacket(kPacket);
	SendToServerType(CEL::ST_MAP, kPacket);
}
