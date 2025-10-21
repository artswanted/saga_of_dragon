#pragma once
#include "BM/Functor.h"

namespace CEL
{
	class CConsoleCommander
		:	public BM::CFunctor< WORD, bool, WORD const >
	{
		friend struct ::Loki::CreateStatic< CConsoleCommander >;
		typedef void (CALLBACK *LPCLOSE_FUNC)();//! 종료함수,
	private:
		CConsoleCommander();
		virtual ~CConsoleCommander();

	public:
		void MainLoof();

		bool StopSignal() const { return ((m_lStopSignal)?true:false) ;}
		void StopSignal(bool const bDoStop)
		{
			::InterlockedExchange(&m_lStopSignal, bDoStop);
		}
		
		void RegistCloseFunc(const LPCLOSE_FUNC lpCloseFunc){ m_pCloseFunc = lpCloseFunc; }

	protected:
		volatile long m_lStopSignal;

		LPCLOSE_FUNC m_pCloseFunc;
	};
};

#define g_kConsoleCommander SINGLETON_STATIC(CEL::CConsoleCommander)
