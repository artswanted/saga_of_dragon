#ifndef WEAPON_LOHENGRIN_MAINFRAME_MEMORYTRACK_H
#define WEAPON_LOHENGRIN_MAINFRAME_MEMORYTRACK_H

#include "LogGroup.h"

#ifdef _MEMORY_TRACKING

#define new_tr new (BM::vstring() << __FL__)

extern void* operator new (std::size_t size) throw(...);
extern void* operator new (std::size_t size, std::wstring const & strFunction) throw(...);
extern void operator delete(void* pkDelete) throw();
extern void* operator new[] (std::size_t size) throw(...);
extern void* operator new[] (std::size_t size, std::wstring const & strFunction) throw(...);
extern void operator delete[](void* pkDelete) throw();

#define MEMORY_TRACK_INIT_VALUE 0x83F10DC9
class PgMemoryTrack
{
public:
	explicit PgMemoryTrack() {};
	~PgMemoryTrack() {};

	void Init(BM::E_OUPUT_TYPE const eOutType, std::wstring const& strFolder, std::wstring const &strFile);

	void Add(void* pkAddress, std::wstring const & strFunction)
	{
		if (m_iInitValue == MEMORY_TRACK_INIT_VALUE)
		{
			BM::CAutoMutex kLock(m_kMutex);
			m_kNewMemory.insert(std::make_pair(pkAddress, strFunction));
		}
	}
	void Remove(void* pkAddress)
	{
		if (m_iInitValue == MEMORY_TRACK_INIT_VALUE)
		{
			BM::CAutoMutex kLock(m_kMutex);
			if (m_kNewMemory.erase(pkAddress) == 0)
			{
				//m_kLog.Log(BM::LOG_LV1, BM::vstring() << __FL__ << _T("Cannot find remove memory=") << HEX(reinterpret_cast<DWORD>(pkAddress)));
			}
		}
	}
	void AddArray(void* pkAddress, std::wstring const & strFunction)
	{
		if (m_iInitValue == MEMORY_TRACK_INIT_VALUE)
		{
			BM::CAutoMutex kLock(m_kMutex);
			m_kNewMemoryArray.insert(std::make_pair(pkAddress, strFunction));
		}
	}
	void RemoveArray(void* pkAddress)
	{
		if (m_iInitValue == MEMORY_TRACK_INIT_VALUE)
		{
			BM::CAutoMutex kLock(m_kMutex);
			if (m_kNewMemoryArray.erase(pkAddress) == 0)
			{
				//m_kLog.Log(BM::LOG_LV1, BM::vstring() << __FL__ << _T("Cannot find remove memory=") << HEX(reinterpret_cast<DWORD>(pkAddress)));
			}
		}
	}

	void DisplayState(BM::CDebugLog &kLog, bool const bShowAll = false)const;

	typedef std::map<void*, std::wstring> CONT_NEW_MEMORY;
private:
	mutable Loki::Mutex m_kMutex;
	CONT_NEW_MEMORY m_kNewMemory;
	CONT_NEW_MEMORY m_kNewMemoryArray;
	int m_iInitValue;
	BM::CDebugLog m_kLog;
};

extern PgMemoryTrack* g_pkMemoryTrack;
#else
#define new_tr new

#endif

#endif // WEAPON_LOHENGRIN_MAINFRAME_MEMORYTRACK_H