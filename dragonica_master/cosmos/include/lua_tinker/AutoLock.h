#pragma once

#define WIN32_LEAN_AND_MEAN
#include "Loki/Threads.h"

class CAutoMutex
{
public:
	CAutoMutex(Loki::Mutex& rkLock)
		:	m_pkLoki(&rkLock)
	{
		m_pkLoki->Lock();
	}


	~CAutoMutex()
	{
		m_pkLoki->Unlock();
	}

protected:
	Loki::Mutex * const m_pkLoki;
};


