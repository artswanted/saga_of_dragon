#include "stdafx.h"
#include "PgRecvFromManagementServer.h"
#include "Variant/PgMCtrl.h"
//#include "PgSessionManager.h"
#include "PgServerSetMgr.h"
#include "PgSendWrapper.h"

bool OnRecvFromMCtrl( WORD wkType, BM::Stream * const pkPacket, CEL::CSession_Base *pkSession)
{
	switch( wkType )
	{
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect Packet Type[") << wkType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true;
}

int GetMaxUser()
{
	return (int)g_kRealmUserMgr.MaxPlayerCount();
}

int GetConnectionUser()
{
	return (int)g_kRealmUserMgr.Locked_CurrentPlayerCount();
}

void SetMaxUser(int const &ChangeCount)
{
	size_t ret = 0;
	if(ChangeCount > 0)
		ret = __min(ChangeCount, 15000 );
	else
		ret = __max(ChangeCount, 0 );

	g_kRealmUserMgr.MaxPlayerCount(ret);

	//IniЖДАПїЎ АъАеЗШѕЯ ЗСґЩ.
	wchar_t wcString[100] = {0,};
	_itow_s((int)ret, wcString, 100, 10);
	std::wstring kPatch = g_kProcessCfg.ConfigDir() + _T("Contents_Config.ini");
	BM::vstring kCategory(L"REALM_");
	kCategory += g_kProcessCfg.RealmNo();
	WritePrivateProfileStringW(kCategory.operator const wchar_t *(), L"MAX_USER_COUNT", wcString, kPatch.c_str());	
}