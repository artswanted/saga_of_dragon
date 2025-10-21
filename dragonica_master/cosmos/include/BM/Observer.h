#pragma once
#include <list>
#include "loki/Threads.h"

namespace BM
{
	template< typename T_NFY >
	class CSubject;//Observer 패턴의 서브젝트 객체.

	template< typename T_NFY >
	class CObserver
	{
	protected:
		explicit CObserver(){}

	public: 
		virtual ~CObserver(){}
		virtual void VUpdate( CSubject< T_NFY > * const pChangedSubject, T_NFY rkNfy ) = 0;
	};

	template< typename T_NFY >
	class CSubject//Observer 패턴의 서브젝트 객체.
	//	:	public CCS_Thread_Safe
	{
	protected:
		explicit CSubject()
		{
			ClearObserver();
		}

		explicit CSubject(CSubject const& lhs)
		{
			m_lstObservers = lhs.m_lstObservers;
		}

	public:
		virtual ~CSubject()
		{
		}
	
	public:	
		virtual void VAttach(CObserver< T_NFY > *pkObserver)//VNotify내에서 호출 금지
		{
			BM::CAutoMutex Lock( m_subject_mutex, true );
			
			m_lstObservers.push_back(pkObserver);
		}

		virtual void VDetach(const CObserver< T_NFY > *pObserver)//VNotify내에서 호출 금지
		{
			BM::CAutoMutex Lock( m_subject_mutex, true );

			OBSERVER_LIST::iterator Itor = find( m_lstObservers.begin(), m_lstObservers.end(), pObserver);

			if(Itor != m_lstObservers.end())
			{
				m_lstObservers.erase(Itor);
			}
		}

		template< typename T_NFY >
		class NotifyElement
		{
		public:
			NotifyElement(CSubject< T_NFY > * const pSubject, T_NFY const& rkNfy)
				:	m_pSubject(pSubject)
				,	m_kNfy(rkNfy)
			{
			}
		public:
			template<typename T_ELEMENT>
			void operator ()(T_ELEMENT &TElement)
			{
				TElement->VUpdate( m_pSubject, m_kNfy );
			}

			CSubject< T_NFY > * const m_pSubject;
			T_NFY const m_kNfy;
		};

		virtual void VNotify(T_NFY const& rkNfy) //알리기.
		{
			BM::CAutoMutex Lock( m_subject_mutex );
			for_each( m_lstObservers.begin(), m_lstObservers.end(), NotifyElement< T_NFY >(this, rkNfy) );
		}
	public:
		void ClearObserver()
		{
			BM::CAutoMutex Lock( m_subject_mutex, true );
			m_lstObservers.clear();
		}

		CSubject& operator = (CSubject const& rhs)
		{
			if(this != &rhs)
			{
				m_lstObservers = rhs;
			}
			return *this;
		}
	private://순서가 선언된 메모리 순으로 간다.//List로 변경하자.
		typedef typename std::list< CObserver< T_NFY >* > OBSERVER_LIST;//List 보다는 중복 삽입이 안되는 map쪽이 좋다.
		OBSERVER_LIST m_lstObservers;

	protected:
		mutable ACE_RW_Thread_Mutex m_subject_mutex;
	};
};