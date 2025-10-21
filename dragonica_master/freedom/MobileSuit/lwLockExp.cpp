#include "StdAfx.h"
#include "lwLockExp.h"
#include "PgPilotMan.h"

static lwWString lwGetExpLockToolTipText(const int iStringNo)
{
	PgActor *pkActor = g_kPilotMan.GetPlayerActor();
	if (!pkActor)
	{
		return lwWString("");
	}

	CUnit *pkUnit = pkActor->GetUnit();
	if (!pkUnit)
	{
		return lwWString("");
	}

	bool bIsExpLocked = pkUnit->GetEffect(90000300);
	BM::vstring vstrText(TTW(iStringNo));
	BM::vstring vstrLockText(L"");
	if(bIsExpLocked)
	{
		vstrLockText += "{C=0xFFFF0000/}";
		vstrLockText += TTW(792205);
	}
	else
	{
		vstrLockText += "{C=0xFF00FF00/}";
		vstrLockText += TTW(792206);
	}
	vstrText.Replace(L"#EXP_STATUS#", vstrLockText);
	return lwWString(MB(vstrText));
}

static void lwLockExpOneClick()
{
	PgActor *pkActor = g_kPilotMan.GetPlayerActor();
	if (!pkActor)
	{
		return;
	}

	CUnit *pkUnit = pkActor->GetUnit();
	if (!pkUnit)
	{
		return;
	}

	bool bIsExpLocked = pkUnit->GetEffect(90000300);
	bIsExpLocked = !bIsExpLocked;
	lua_tinker::call<void, bool>("Net_PT_C_M_REQ_LOCKEXP_TOGGEL", bIsExpLocked);
}

bool lwLockExp::RegisterWrapper(lua_State *pkState)
{
	using namespace lua_tinker;
	def(pkState, "GetExpLockToolTipText", &lwGetExpLockToolTipText);
	def(pkState, "LockExpOneClick", &lwLockExpOneClick);
	return true;
}

void Req_PT_M_C_ANS_LOCKEXP_TOGGEL(BM::Stream &rkPacket)
{
	short eErrorCode;
	rkPacket.Pop(eErrorCode);
	switch((ELockEXPError)eErrorCode)
	{
		case ELEE_SUCCESS_LOCK_EXP:
		case ELEE_CANT_LOCK_EXP:
		case ELEE_EXP_ALREADY_LOCKED:
		case ELEE_EXP_ALREADY_UNLOCK:
		case ELEE_EXP_LOCK_UNKNOW:
			{		
			}break;

	}

	PgActor *pkActor = g_kPilotMan.GetPlayerActor();
	if (!pkActor)
	{
		return;
	}

	CUnit *pkUnit = pkActor->GetUnit();
	if (!pkUnit)
	{
		return;
	}

	bool bIsExpLocked = pkUnit->GetEffect(90000300);
	::Notice_Show( TTW(bIsExpLocked ? 792201 : 792202), EL_Normal );
}