#pragma once

namespace ThreadUtil
{
	typedef void (*RESULT_CALL_BACK)();

	template< typename _T_RESULT_TYPE, typename _T_CALL_BACK = RESULT_CALL_BACK, typename _T_COUNT_TYPE = size_t >
	class CResultObserverMgr
	{
	public:
		class CObserver
		{
			typedef std::map< _T_RESULT_TYPE, _T_COUNT_TYPE > CONT_RESULT;
		public:
			CObserver()
				: pFunc(NULL), kObserverQuery()
			{
			}
			CObserver(_T_CALL_BACK pResultFunc)
				: pFunc(pResultFunc), kObserverQuery()
			{
			}
			CObserver(CObserver const& rhs)
				: pFunc(rhs.pFunc), kObserverQuery(rhs.kObserverQuery)
			{
			}
			~CObserver()
			{
			}

			void AddCount(_T_RESULT_TYPE const eType, size_t const iCount)
			{
				auto kRet = kObserverQuery.insert( std::make_pair(eType, iCount) );
				if( !kRet.second )
				{
					(*kRet.first).second += iCount;
				}
			}
			void Add(_T_RESULT_TYPE const eType)
			{
				AddCount(eType, 1);
			}
			CObserver& operator <<(_T_RESULT_TYPE const eType)
			{
				Add(eType);
				return *this;
			}
			bool Process(_T_RESULT_TYPE const eType)
			{
				CONT_RESULT::iterator find_iter = kObserverQuery.find(eType);
				if( kObserverQuery.end() != find_iter )
				{
					if( 0 == --(*find_iter).second )
					{
						kObserverQuery.erase(find_iter);
					}
				}
				if( kObserverQuery.empty() )
				{
					if( pFunc )
					{
						(*pFunc)();
					}
					return true;
				}
				return false;
			}
			bool IsEmpty() const
			{
				return kObserverQuery.empty();
			}

		private:
			CONT_RESULT kObserverQuery;
			_T_CALL_BACK pFunc;
		};
		typedef std::list< CObserver > CONT_RESULT_OBSERVER;

	public:
		CResultObserverMgr()
		{
		}
		~CResultObserverMgr()
		{
		}

		void Add(CObserver const& rkObserver)
		{
			BM::CAutoMutex kLock(m_kMutex);
			if( !rkObserver.IsEmpty() )
			{
				kContResultObserver.push_back(rkObserver);
			}
		}
		void Process(_T_RESULT_TYPE const eType)
		{
			BM::CAutoMutex kLock(m_kMutex);
			CONT_RESULT_OBSERVER::iterator iter = kContResultObserver.begin();
			while( kContResultObserver.end() != iter )
			{
				if( (*iter).Process(eType) )
				{
					iter = kContResultObserver.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	private:
		Loki::Mutex m_kMutex;
		CONT_RESULT_OBSERVER kContResultObserver;
	};
};