#pragma once
#include "CEL/Common.h"

namespace CEL
{
	class CTimer_Base
	{
		friend class CCoreCenter;
	protected:
		typedef void (CALLBACK *LPTIMERCALLBACKFUNC)(DWORD dwUserData);
	protected:
		
		CTimer_Base()	{	}
		virtual ~CTimer_Base()	{	}

	protected:
		virtual bool VDeactivate() = 0;
		virtual bool SvcStart() = 0;
		virtual long Regist(const LPTIMERCALLBACKFUNC Func, DWORD const dwUserData, DWORD const dwMsecInterval, DWORD const dwMsecFirstInterval = 0) = 0;//! 단위:밀리세컨드, 콜백자체가 인터벌 보다 시간이 오래걸리면 인터벌이 제대로 안 먹을 수도 있습니다.(스케쥴이 밀린다는 이야기)

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const = 0;
	};
}