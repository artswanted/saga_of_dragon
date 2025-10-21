#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <map>
#include "Loki/Threads.h"
#include "Ace/RW_Thread_Mutex.h"
#include "Ace/Token.h"

#include <intrin.h>
#pragma intrinsic(_ReturnAddress)
#define PROFILE_MUTEX(BaseCls) BM::Profile_##BaseCls

namespace BM
{

	__forceinline void* ReturnAddress()
	{
		return _ReturnAddress();
	}

	void MUTEX_PROFILE_LOCK(void* kRetAddress = 0x0);
	void MUTEX_PROFILE_UNLOCK(void* kRetAddress = 0x0);
	void MUTEX_PROFILE_STATS(DWORD dwLogType);

	class ACE_RW_Thread_Mutex_Ext : public ACE_RW_Thread_Mutex
	{
	public:
		ACE_RW_Thread_Mutex_Ext()
		{
			m_ulWriteLockThread = 0;
			m_sWriteLockCount = 0;
		}

		typedef std::map<unsigned long, short int> CONT_LOCKTHREAD;	// <ThreadId, ReferenceCount>

		void acquire_read()
		{
			unsigned long ulThread = GetCurrentThreadId();
			short int iRefCount = GetReferenceReadCount(ulThread);
			if (iRefCount == 0)
			{
				ACE_RW_Thread_Mutex::acquire_read();
			}
			IncreaseRefReadCount(ulThread);
		}

		void acquire_write()
		{
			unsigned long ulThread = GetCurrentThreadId();
			short int iRefCount = GetReferenceWriteCount(ulThread);
			if (iRefCount == 0)
			{
				ACE_RW_Thread_Mutex::acquire_write();
			}
			IncreaseRefWriteCount(ulThread);
		}

		void release_read()
		{
			unsigned long ulThread = GetCurrentThreadId();
			short int iRefCount = GetReferenceReadCount(ulThread);
			if (iRefCount == 1)
			{
				ACE_RW_Thread_Mutex::release();
			}
			DecreaseRefReadCount(ulThread);
		}

		void release_write()
		{
			if (DecreaseRefWriteCount() == 0)
			{
				ACE_RW_Thread_Mutex::release();
			}
			/*
			short int iRefCount = GetReferenceWriteCount(ulThread);
			if (iRefCount == 1)
			{
				ACE_RW_Thread_Mutex::release();
			}
			DecreaseRefWriteCount(ulThread);
			*/
		}

	protected:
		inline short int GetReferenceReadCount(unsigned long const ulThread)
		{
			m_kInternalLock.Lock();

			CONT_LOCKTHREAD::const_iterator itor = m_kReadLockInfo.find(ulThread);
			if (m_kReadLockInfo.end() == itor)
			{
				m_kInternalLock.Unlock();
				return 0;
			}

			short int sCount = itor->second;
			m_kInternalLock.Unlock();
			return sCount;
		}
		
		inline void IncreaseRefReadCount(unsigned long const ulThread)
		{
			m_kInternalLock.Lock();

			CONT_LOCKTHREAD::iterator itor = m_kReadLockInfo.find(ulThread);
			if (m_kReadLockInfo.end() == itor)
			{
				m_kReadLockInfo.insert(std::make_pair(ulThread, 1));
				m_kInternalLock.Unlock();
				return;
			}
			itor->second++;
			m_kInternalLock.Unlock();

		}

		inline void DecreaseRefReadCount(unsigned long const ulThread)
		{
			m_kInternalLock.Lock();

			CONT_LOCKTHREAD::iterator itor = m_kReadLockInfo.find(ulThread);
			if (m_kReadLockInfo.end() == itor)
			{
				assert(false);
				m_kInternalLock.Unlock();
				return;
			}
			if (itor->second == 1)
			{
				m_kReadLockInfo.erase(itor);
			}
			else
			{
				itor->second--;
			}

			m_kInternalLock.Unlock();
		}

		inline short int GetReferenceWriteCount(unsigned long const ulThread)
		{
			m_kInternalLock.Lock();
			if (m_ulWriteLockThread == ulThread)
			{
				m_kInternalLock.Unlock();
				return m_sWriteLockCount;
			}
			m_kInternalLock.Unlock();
			return 0;
		}
		
		inline void IncreaseRefWriteCount(unsigned long const ulThread)
		{
			// WriteLock 은 동시에 한개만 들어 올수 있기 때문에 Thread ID 검사 할 필요 없다.
			m_kInternalLock.Lock();
			++m_sWriteLockCount;
			m_ulWriteLockThread = ulThread;
			m_kInternalLock.Unlock();

		}

		inline short int DecreaseRefWriteCount()
		{
			m_kInternalLock.Lock();
			short int sCount = --m_sWriteLockCount;
			if (m_sWriteLockCount == 0)
			{
				m_ulWriteLockThread = 0;
			}
			m_kInternalLock.Unlock();
			return sCount;
		}

	protected:
		CONT_LOCKTHREAD m_kReadLockInfo;
		Loki::Mutex m_kInternalLock;
		unsigned long m_ulWriteLockThread;
		short int m_sWriteLockCount;
	};

	struct Profile_ACE_RW_Thread_Mutex
	{
		Profile_ACE_RW_Thread_Mutex()
		{
		}

		mutable ACE_RW_Thread_Mutex m_kMutex; // Native Mutex
	};

	class CAutoMutex
	{
		typedef enum
		{
			ELockType_Loki = 1,
			ELockType_AceRw = 2,
			ELockType_AceToken = 3,
			ELockType_AceRw_Ext = 4,
		} ELockObjectType;

		/*
		typedef enum : BYTE {
			ELockMethod_acquire_read = 1,
			ELockMethod_acquire_wirte = 2,
			ELockMethod_tryacquire_write_upgrade = 3,
		} ELockMethodType;
		*/

	public:
		//CAutoMutex( T_LOCK& rkLock, bool bWriteLock = false)
		CAutoMutex(Loki::Mutex& rkLock)
			:	m_pkLoki(&rkLock)
			,m_eType(ELockType_Loki)
			, m_pkAceRwExt(NULL)
			,m_pkAceRw(NULL)
			,m_pkAceToken(NULL)
			,m_bProfile(false)
		{
			m_pkLoki->Lock();
		}

		// ACE_RW_Thread_Mutex 제약 사항
		// Recursive 호출 안됨.
		// 별도의 Thread에서 ReadLock 동시 접근 가능
		//CAutoMutex(ACE_RW_Thread_Mutex& rkLock, ELockMethodType eCallType = ELockMethod_acquire_read)
		CAutoMutex(ACE_RW_Thread_Mutex& rkLock, bool bWriteLock = false)
			:	m_pkAceRw(&rkLock)
			, m_eType(ELockType_AceRw)
			, m_pkAceRwExt(NULL)
			, m_pkLoki(NULL)
			, m_pkAceToken(NULL)
			, m_bProfile(false)
		{
			/*
			switch(eCallType)
			{
			case ELockMethod_acquire_wirte:
				{
					m_pkAceRw->acquire_write();
				}break;
			case ELockMethod_tryacquire_write_upgrade:
				{
					m_pkAceRw->tryacquire_write_upgrade();
				}break;
			case ELockMethod_acquire_read:
			default:
				{
					m_pkAceRw->acquire_read();
				}break;
			}
			*/
			if (bWriteLock)
			{
				m_pkAceRw->acquire_write();
			}
			else
			{
				m_pkAceRw->acquire_read();
			}
		}

		// ACE_RW_Thread_Mutex 의 제약사항 극복하기 위한 확장
		// R-Lock Recursive 가능
		// W-Lock Recursive 가능
		// R-Lock 호출후 같은 Thread에서 W-Lock 호출 불가
		// W-Lock 호출후 같은 Thread에서 R-Lock 호출 불가
		CAutoMutex(ACE_RW_Thread_Mutex_Ext& rkLock, bool bWriteLock = false)
			: m_pkAceRwExt(&rkLock)
			, m_eType(ELockType_AceRw_Ext)
			, m_pkAceRw(NULL)
			, m_pkLoki(NULL)
			, m_pkAceToken(NULL)
			, m_bWrite(bWriteLock)
			, m_bProfile(false)
		{
			if (bWriteLock)
			{
				m_pkAceRwExt->acquire_write();
			}
			else
			{
				m_pkAceRwExt->acquire_read();
			}
		}

		// ACE_Token 제약 사항
		// Recursive 호출 가능
		// R-W lock 중복 호출 가능 (여러쓰레드에서의 복잡한 상황 아직 파악 안됨)
		// 다른 쓰레드에서 R-Lock 여러번 호출 : 오직 한개의 Thread 만 접근 가능
		// 결론 : 일반 Mutex와 차이 없음(R-W 함수는 있지만, 기능 작동 안함)
		CAutoMutex(ACE_Token& rkLock, bool bWriteLock = false)
			:	m_pkAceToken(&rkLock)
			, m_eType(ELockType_AceToken)
			, m_pkAceRwExt(NULL)
			, m_pkLoki(NULL)
			, m_pkAceRw(NULL)
			, m_bWrite(bWriteLock)
			, m_bProfile(false)
		{
			if (bWriteLock)
			{
				m_pkAceToken->acquire_write();
			}
			else
			{
				m_pkAceToken->acquire_read();
			}
		}

		// Thread Profiler Impl
		CAutoMutex(Profile_ACE_RW_Thread_Mutex& rkLock, bool bWriteLock = false)
			: m_pkAceRw(&rkLock.m_kMutex)
			, m_eType(ELockType_AceRw)
			, m_pkAceRwExt(NULL)
			, m_pkLoki(NULL)
			, m_pkAceToken(NULL)
			, m_bProfile(true)
		{
			MUTEX_PROFILE_LOCK(ReturnAddress());
			if (bWriteLock)
			{
				m_pkAceRw->acquire_write();
			}
			else
			{
				m_pkAceRw->acquire_read();
			}
		}

		~CAutoMutex()
		{
			if (m_bProfile)
			{
				MUTEX_PROFILE_UNLOCK(ReturnAddress());
			}

			switch(m_eType)
			{
			case ELockType_Loki:
				{
					m_pkLoki->Unlock();
				}break;
			case ELockType_AceRw:
				{
					m_pkAceRw->release();
				}break;
			case ELockType_AceRw_Ext:
				{
					if (m_bWrite)
					{
						m_pkAceRwExt->release_write();
					}
					else
					{
						m_pkAceRwExt->release_read();
					}
				}break;
			case ELockType_AceToken:
				{
					m_pkAceToken->release();
				}break;
			default:
				{
					assert(false && "Undfined Lock type");
				}break;
			}
		}

	protected:
		Loki::Mutex * const m_pkLoki;
		ACE_RW_Thread_Mutex * const m_pkAceRw;
		ACE_Token* const m_pkAceToken;
		ACE_RW_Thread_Mutex_Ext* const m_pkAceRwExt;
		bool m_bWrite;

		ELockObjectType const m_eType;

		bool m_bProfile;
	};
};

