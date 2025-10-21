#include "stdafx.h"

#include "BM/LogWorker.h"
#include "boost/interprocess/detail/atomic.hpp"

static const int MAX_THREAD_COUNT = 64;
struct ThreadInfo
{
    bool m_kLocked;
    // void* m_kRetAddress;
    DWORD m_kThreadId;
    DWORD m_kAccessTime;
};

static struct ThreadArray
{
    ThreadInfo m_kData[MAX_THREAD_COUNT];
    boost::uint32_t m_kSize;
    ThreadArray()
    {
        memset(m_kData, sizeof(m_kData), 0);
        m_kSize = 0;
    }
} g_kThreadArray;



#ifdef ENABLE_MUTEX_PROFILE

void BM::MUTEX_PROFILE_STATS(DWORD dwLogType)
{
    const boost::uint32_t szSize = boost::interprocess::detail::atomic_read32(&g_kThreadArray.m_kSize);
    for(boost::uint32_t i = 0; i < szSize; ++i)
    {
        ThreadInfo const& kInfo = g_kThreadArray.m_kData[i]; // not thread safty, but this case is fine
        SDebugLogMessage kMsg(dwLogType, BM::LOG_LV5, BM::vstring("[MUTEX] Thread: ") << kInfo.m_kThreadId <<
            " Access Time: " << kInfo.m_kAccessTime <<
            // " Ret Address: " << kInfo.m_kRetAddress <<
            " Locked: " << kInfo.m_kLocked
        );
        g_kLogWorker.PushLog(kMsg);
    }
}

static ThreadInfo* FindOrInsert()
{
    DWORD dwThreadId = GetCurrentThreadId();
    boost::uint32_t szSize = boost::interprocess::detail::atomic_read32(&g_kThreadArray.m_kSize);
    for(boost::uint32_t i = 0; i < szSize; ++i)
    {
        ThreadInfo &kInfo = g_kThreadArray.m_kData[i];
        if (kInfo.m_kThreadId == dwThreadId) // thread safty, because thread id isn't changed
        {
            return &kInfo;
        }
    }

    szSize = boost::interprocess::detail::atomic_inc32(&g_kThreadArray.m_kSize);
    ThreadInfo &kInfo = g_kThreadArray.m_kData[szSize];
    kInfo.m_kThreadId = dwThreadId;
    return &kInfo;
}

void BM::MUTEX_PROFILE_LOCK(void* kRetAddress)
{
    ThreadInfo* thr = FindOrInsert();
    thr->m_kLocked = true;
    thr->m_kAccessTime = BM::GetTime32();
    // thr->m_kRetAddress = BM::ReturnAddress();
}

void BM::MUTEX_PROFILE_UNLOCK(void* kRetAddress)
{
    ThreadInfo* thr = FindOrInsert();
    thr->m_kLocked = false;
};

#else

// stub
void BM::MUTEX_PROFILE_LOCK(void* kRetAddress) {}
void BM::MUTEX_PROFILE_UNLOCK(void* kRetAddress) {}
void BM::MUTEX_PROFILE_STATS(DWORD dwLogType) {};
#endif