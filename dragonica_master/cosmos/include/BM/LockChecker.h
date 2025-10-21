#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace BM
{
	class PgDebugLog;

	class CLockChecker
	{
	public:
		CLockChecker( PgDebugLog &rkLog, char const* pFunc, int const Line, int LockCount = 0 );
		~CLockChecker();
		
	protected:
		PgDebugLog &m_rklog;
		std::wstring const m_Func;
		int const m_Line;
		int const m_LockCount;
		CLockChecker& operator=( CLockChecker const&  );
	};
};

#define LockCheck(log, obj) BM::CLockChecker obj( log, __FUNCTION__, __LINE__ )
