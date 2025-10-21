#include "stdafx.h"
#include <tchar.h>

#include "BM/LockChecker.h"
#include "BM/vstring.h"
#include "BM/Threadobject.h"
#include "BM/LogWorker_Base.h"

using namespace BM;

CLockChecker::CLockChecker( PgDebugLog &rkLog, char const* pFunc, int const Line, int LockCount )
	:	m_rklog(rkLog), m_Func( UNI(pFunc) ), m_Line(Line), m_LockCount(LockCount)
{
	BM::vstring strLog(_T("L "));
	while(LockCount)
	{
		strLog += _T("	");
		--LockCount;
	}
	//strLog += _T("[%s]:[%d]");

//	m_klog.LogNoArg(BM::LOG_LV7, (BM::vstring)strLog << _T("[") << m_Func.c_str() << _T("]:[") << m_Line <<  _T("]") );
}

CLockChecker::~CLockChecker()
{
	int LockCount = m_LockCount;
	BM::vstring strLog(_T("U "));
	while(LockCount)
	{
		strLog += _T("	");
		--LockCount;
	}
	//strLog += _T("[%s]:[%d]");

//	m_klog.LogNoArg(BM::LOG_LV7, (BM::vstring)strLog << _T("[") << m_Func.c_str() << _T("]:[") << m_Line <<  _T("]") );
}