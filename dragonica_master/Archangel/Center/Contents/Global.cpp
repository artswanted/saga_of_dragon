#include "stdafx.h"
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


