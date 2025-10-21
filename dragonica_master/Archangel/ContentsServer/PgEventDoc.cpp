#include "stdafx.h"
#include "PgEventDoc.h"
#include "Lohengrin/GameTime.h"

PgEventDoc::PgEventDoc()
{
}

PgEventDoc::~PgEventDoc()
{
}

bool PgEventDoc::RefreshTbl(const CONT_TBL_EVENT &rKCont)//기존 빌드 정보 필요
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kContEvent = rKCont;
	DoNfyEvent();
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

void PgEventDoc::WriteToPacket(BM::Stream &kPacket)const
{//EventView 에서 ReadFrom 함.
	BM::CAutoMutex kLock(m_kMutex);
	PU::TWriteTable_MM(kPacket, m_kContEvent);
}

void PgEventDoc::DoNfyEvent()
{	
	BM::Stream kPacket(PT_T_A_EVENT_SYNC);
	WriteToPacket(kPacket);
	SendToServerType(CEL::ST_CENTER, kPacket);
}
