#include "stdafx.h"
#include "BM/LocalMgr.h"
#include "MemoryTrack.h"

#ifdef _MEMORY_TRACKING

PgMemoryTrack* g_pkMemoryTrack = NULL;

void PgMemoryTrack::Init(BM::E_OUPUT_TYPE const eOutType, std::wstring const& strFolder, std::wstring const &strFile)
{
	m_iInitValue = MEMORY_TRACK_INIT_VALUE;
	m_kLog.Init(eOutType, BM::LFC_WHITE, strFolder, strFile, g_kLocal.GetLocale());
}

void PgMemoryTrack::DisplayState(BM::CDebugLog &kLog, bool const bShowAll)const
{
	BM::CAutoMutex kLock(m_kMutex);
	kLog.Log(BM::LOG_LV6, BM::vstring() << _T("Remain Memory Count=") <<  m_kNewMemory.size());
	kLog.Log(BM::LOG_LV6, BM::vstring() << _T("Remain MemoryArray Count=") <<  m_kNewMemoryArray.size());

	if (bShowAll)
	{
		kLog.Log(BM::LOG_LV6, BM::vstring() << _T("============================"));
		kLog.Log(BM::LOG_LV6, BM::vstring() << _T("\tDisplay All Allocated Memory"));
		CONT_NEW_MEMORY::const_iterator itor_mem = m_kNewMemory.begin();
		while(m_kNewMemory.end() != itor_mem)
		{
			kLog.Log(BM::LOG_LV6, BM::vstring() << _T("\t") << itor_mem->second << _COMMA_ << HEX(reinterpret_cast<DWORD>(itor_mem->first)));
			++itor_mem;
		}

		kLog.Log(BM::LOG_LV6, BM::vstring() << _T("============================"));
		kLog.Log(BM::LOG_LV6, BM::vstring() << _T("\tDisplay All Allocated Memory (Array)"));
		CONT_NEW_MEMORY::const_iterator itor_memarr = m_kNewMemoryArray.begin();
		while(m_kNewMemoryArray.end() != itor_memarr)
		{
			kLog.Log(BM::LOG_LV6, BM::vstring() << _T("\t") << itor_memarr->second << _COMMA_ << HEX(reinterpret_cast<DWORD>(itor_memarr->first)));
			++itor_memarr;
		}

	}
}

void* operator new (std::size_t size) throw(...)
{
	using namespace std;
	if (size == 0)
	{
		size = 1;
	}

	while (true)
	{
		void* pkNew = malloc(size);
		if (pkNew)
		{
			return pkNew;
		}
		new_handler globalHandler = set_new_handler(0);
		set_new_handler(globalHandler);

		if (globalHandler)
		{
			(*globalHandler)();
		}
		else
		{
			throw std::bad_alloc();
		}
	}
}

void* operator new (std::size_t size, std::wstring const & strFunction) throw(...)
{
	using namespace std;
	if (size == 0)
	{
		size = 1;
	}

	void* pkAddress = operator new(size);
	if (pkAddress && g_pkMemoryTrack)
	{
		g_pkMemoryTrack->Add(pkAddress, strFunction);
	}
	return pkAddress;
}

void operator delete(void* pkDelete) throw()
{
	if (!pkDelete)
	{
		return;
	}
	if (g_pkMemoryTrack)
	{
		g_pkMemoryTrack->Remove(pkDelete);
	}
	free(pkDelete);
}

void* operator new [](std::size_t size) throw(...)
{
	return operator new(size);
}

void* operator new [](std::size_t size, std::wstring const & strFunction) throw(...)
{
	void* pkAddress = operator new(size);
	if (pkAddress && g_pkMemoryTrack)
	{
		g_pkMemoryTrack->AddArray(pkAddress, strFunction);
	}
	return pkAddress;
}

void operator delete[](void* pkDelete) throw()
{
	if (!pkDelete)
	{
		return;
	}
	if (g_pkMemoryTrack)
	{
		g_pkMemoryTrack->RemoveArray(pkDelete);
	}
	free(pkDelete);
}

#endif