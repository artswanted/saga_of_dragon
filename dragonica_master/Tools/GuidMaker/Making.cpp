#include "stdafx.h"

#include "BM/BM.h"
#include "BM/DebugLog.h"
#include "BM/GUID.h"

void MakeGuid(const int iCount)
{
	if( iCount > 0 )
	{
		BM::CDebugLog Log;

		Log.Init(BM::OUTPUT_JUST_FILE, BM::LFC_WHITE, _T("Log"), _T("GUID.csv") );

		BM::GUID guid;

		int i = iCount;
		while( i)
		{
			guid.Generate();
			Log.Log(BM::LOG_LV1, _T("{%s}"), guid.str().c_str() );
			--i;
		}
	}
}