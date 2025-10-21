#include "stdafx.h"
#include "Lohengrin\PgRealmManager.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/Global.h"
#include "PgTask_Contents.h"

bool GetDefString(int const iTextNo, std::wstring& rkOut)
{
	const CONT_DEFSTRINGS* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);

	CONT_DEFSTRINGS::const_iterator str_itor = pContDefStrings->find(iTextNo);
	if(str_itor != pContDefStrings->end())
	{
		const CONT_DEFSTRINGS::mapped_type& rkElement = (*str_itor).second;
		//pString = (*str_itor).second.strText.c_str();
		rkOut = rkElement.strText;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool GetDefString( int const iTextNo, BM::vstring& rkOut )
{
	const CONT_DEFSTRINGS* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);

	CONT_DEFSTRINGS::const_iterator str_itr = pContDefStrings->find(iTextNo);
	if(str_itr != pContDefStrings->end())
	{
		rkOut = str_itr->second.strText;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool GetItemString(int const iItemNo, std::wstring & rkOut)
{
	return ::GetItemName(iItemNo, rkOut);
}

HRESULT ReadRealmStateFromIni(LPCTSTR lpszFilename)
{
	SERVER_IDENTITY const kSI = g_kProcessCfg.ServerIdentity();
	BM::vstring vSection(_T("Realm_"));
	vSection += kSI.nRealm;
	short sValue = static_cast<short>(GetPrivateProfileInt(vSection.operator const wchar_t *(), _T("STATE"), 0, lpszFilename));
	if (S_OK != g_kRealmMgr.SetRealmState(kSI.nRealm, static_cast<short>(sValue)))
	{
		CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Cannot set Realm State Realm=") << kSI.nRealm << _T(", State=") << sValue);
		return E_FAIL;
	}
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Realm [") << kSI.nRealm << _T("] State = ") << sValue);

	BM::Stream kAPacket(PT_A_A_NFY_REALM_STATE);
	kSI.WriteToPacket(kAPacket);
	kAPacket.Push(sValue);
	SendToImmigration(kAPacket);

	return S_OK;
}

bool IsUseLevelRank()
{
	int iUse = 0;
	g_kVariableContainer.Get( EVar_Kind_Contents, EVar_UseLevelRank, iUse );
	
	return iUse ? true : false;
}
