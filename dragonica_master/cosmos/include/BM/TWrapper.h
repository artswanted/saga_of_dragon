#pragma once
#include "Ace/RW_Thread_Mutex.h"
#include "Loki/threads.h"
#include "BM/ThreadObject.h"

template< typename T, typename T_LOCK = ACE_RW_Thread_Mutex >
class TWrapper
{
public:
	typedef typename T			value_type;
	typedef typename T_LOCK		lock_type;

public:
	TWrapper()
		:m_pkImpl(NULL)
	{
	}

	virtual ~TWrapper()
	{
		SAFE_DELETE(m_pkImpl);
	}

protected:
	virtual T* Instance()const
	{
		BM::CAutoMutex kLock(m_kMutex_Instance_);
		if(!m_pkImpl)
		{
			m_pkImpl = new T;
		}
		return m_pkImpl;
	}

	mutable T_LOCK m_kMutex_Wrapper_;//랩퍼 자체의 락.

private:
	mutable Loki::Mutex m_kMutex_Instance_;//인스턴스 함수의 락.
	mutable T * m_pkImpl;

	TWrapper< T, T_LOCK >& operator=( TWrapper< T, T_LOCK > const& );//대입연산자 사용 불가.
	TWrapper< T, T_LOCK >( TWrapper< T, T_LOCK > const& );//복사생성자 사용 불가.
};
