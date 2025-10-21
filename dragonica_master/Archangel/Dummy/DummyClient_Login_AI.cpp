#include "StdAfx.h"
#include "ai_Action.h"
#include "DummyClient.h"
#include "virtualWorld.h"


void CDummyClient::DoReqCharacterList()
{
	BM::CAutoMutex kLock(m_kMutex);
	_DETAIL_LOG INFO_LOG(BM::LOG_LV9, _T("Sending PT_C_S_REQ_CHARACTER_LIST"));

	BM::Stream kPacket(PT_C_S_REQ_CHARACTER_LIST);
	SendToSwitch(kPacket);
}

void CDummyClient::DoSelectCharacter(CUnit *pkUnit)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(!pkUnit)
	{
		return;
	}
	
	const BM::GUID &kGuid = pkUnit->GetID();
	BM::Stream kUnitPacket;
	pkUnit->WriteToPacket(kUnitPacket);
	ReadFromPacket(kUnitPacket);

	UnitID(kGuid);

	_DETAIL_LOG INFO_LOG(BM::LOG_LV9, _T("Sending PT_C_S_REQ_SELECT_CHARACTER"));
	BM::Stream kPacket(PT_C_S_REQ_SELECT_CHARACTER);
	kPacket.Push(kGuid);
	kPacket.Push(static_cast<bool>(false));

	SendToSwitch(kPacket);
}