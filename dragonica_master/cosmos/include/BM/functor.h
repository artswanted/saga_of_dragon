#pragma once

#include "BM/STLSupport.h"
#include "BM/ThreadObject.h"
#include "Loki/Threads.h"

namespace BM
{
	template < typename T_KEY_TYPE, typename T_RETURN_TYPE, typename T_ARGUMENT_TYPE >//여러개를 넘길때에는 구조체를 넘겨라.
	class CFunctor
	{
	public:
		CFunctor(void)
		{
			Clear();
		}

		virtual ~CFunctor(void)
		{
			Clear();
		}

	public:
		typedef typename T_KEY_TYPE KEY_TYPE;
		typedef typename T_RETURN_TYPE (CALLBACK *LPBINDFUNC)( T_ARGUMENT_TYPE &rArg );//아래는 이벤트가 일어날때 불러지는 콜백 함수 디자인.
		typedef typename std::map< T_KEY_TYPE, LPBINDFUNC > FUNCTOR_HASH;//콜백 함수들의 hash

	public:
		bool Regist(T_KEY_TYPE const Key, LPBINDFUNC const lpBindFunc)
		{
			BM::CAutoMutex kLock(m_kMutex, true);

			FUNCTOR_HASH::iterator iter = m_mapFunctor.find(Key);
			if( m_mapFunctor.end() != iter )
			{
				assert(NULL && "Already Use Handler Function ");
				return false;
			} // if( m_mapFunctor.end() != iter )

			m_mapFunctor.insert( std::make_pair(Key,lpBindFunc) );
			return true;
		}

		bool RegistDefault(const LPBINDFUNC lpBindFunc)
		{
			BM::CAutoMutex kLock(m_kMutex, true);
			pDefaultFunc = lpBindFunc;
			return true;
		}

		// Execute 호출 예제.
		//	char const* argv[] = {(char*)pPacket, (char*)szStr};
		//	Execute( 0x0011, 2, argv );//실행.
		bool Execute(const T_KEY_TYPE &Key, T_ARGUMENT_TYPE& Argument)const
		{
			BM::CAutoMutex kLock(m_kMutex);

			FUNCTOR_HASH::const_iterator iter = m_mapFunctor.find(Key); 
			if( m_mapFunctor.end() == iter )	
			{
				if( pDefaultFunc )
				{
					pDefaultFunc(Argument);
					return true;
				}
				return false;
			} // if( m_mapFunctor.end() == iter )
			else
			{
				if( (*iter).second )
				{
					(*iter).second(Argument);
				}
				return true;
			}
			return false;
		}

		bool Execute(const T_KEY_TYPE &Key, T_ARGUMENT_TYPE& Argument, T_RETURN_TYPE &bFunctionRet)const
		{
			BM::CAutoMutex kLock(m_kMutex);

			FUNCTOR_HASH::const_iterator iter = m_mapFunctor.find(Key); 
			if( m_mapFunctor.end() == iter )	
			{
				if( pDefaultFunc )
				{
					bFunctionRet = pDefaultFunc(Argument);
					return true;
				}
				return false;
			} // if( m_mapFunctor.end() == iter )
			else
			{
				if( (*iter).second )
				{
					bFunctionRet = (*iter).second(Argument);
				}
				return true;
			}
			return false;
		}

	protected:
		void Clear()
		{
			m_mapFunctor.clear();
			pDefaultFunc = NULL;
		}

	protected:
		FUNCTOR_HASH	m_mapFunctor;
		LPBINDFUNC		pDefaultFunc;
		mutable ACE_RW_Thread_Mutex		m_kMutex;
	};
};