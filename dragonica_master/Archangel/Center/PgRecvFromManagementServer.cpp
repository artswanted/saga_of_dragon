#include "stdafx.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromManagementServer.h"
//#include "PgSessionManager.h"
#include "PgSendWrapper.h"

BM::GUID g_kMCtrlRegistGuid;
CEL::SESSION_KEY g_kMCtrlSessionKey; //Connector Guid

bool CALLBACK OnRecvFromMCtrl(CEL::CSession_Base * const pkSession, WORD wkType, BM::Stream * const pkPacket)
{
	switch( wkType )
	{
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << wkType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

int GetMaxUser()
{
	return (int)g_kSwitchAssignMgr.MaxPlayerCount();
}

int GetConnectionUser()
{
	return (int)g_kSwitchAssignMgr.NowPlayerCount();
}

void SetMaxUser(int const &ChangeCount)
{
	size_t ret = 0;
	if(ChangeCount > 0)
		ret = __min(ChangeCount, 5000 );
	else
		ret = __max(ChangeCount, 0 );
	
	g_kSwitchAssignMgr.MaxPlayerCount(ret);

	//IniЖДАПїЎ АъАеЗШѕЯ ЗСґЩ.
	wchar_t wcString[100] = {0,};
	_itow_s((int)ret, wcString, 100, 10);

	std::wstring const kPatch = g_kProcessCfg.ConfigDir() + _T("Center_Config.ini");

	BM::vstring kOldCategory(L"CHANNEL_");
	kOldCategory += g_kProcessCfg.ChannelNo();
	BM::vstring kCategory(L"R");
	kCategory += g_kProcessCfg.RealmNo();
	kCategory += L"_";
	kCategory += kOldCategory;

	::WritePrivateProfileString(kCategory.operator const wchar_t *(), L"MAX_USER_COUNT", wcString, kPatch.c_str());
	::WritePrivateProfileString(kOldCategory.operator const wchar_t *(), NULL, NULL, kPatch.c_str());
}