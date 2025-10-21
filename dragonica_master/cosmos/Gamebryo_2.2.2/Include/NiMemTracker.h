// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIMEMTRACKER_H
#define NIMEMTRACKER_H

#include "NiSystemLibType.h"
#include "NiAllocator.h"
#include "NiSystem.h"

#if defined(NISYSTEM_EXPORT) || defined(NISYSTEM_IMPORT)
    #define NI_FLF_FILE_LENGTH 256
    #define NI_FLF_FUNCTION_LENGTH 256
#endif

// For storing File, Line, and Function of allocation event.   
class NISYSTEM_ENTRY NiFLF
{
public:
    NiFLF();
    NiFLF(const char* pcFile, unsigned int uiLine, const char* pcFunc);
    
    void Set(const char* pcFile, unsigned int uiLine, const char* pcFunc);
    void Reset();
    bool IsValid() const;

    // Since DLL builds can potentially detach at any time,
    // it is necessary to copy the file and function strings
    // in DLL builds rather than using the pointer directly.
#if defined(NISYSTEM_EXPORT) || defined(NISYSTEM_IMPORT)
    char m_pcFile[NI_FLF_FILE_LENGTH];
    char m_pcFunc[NI_FLF_FUNCTION_LENGTH];
#else 
    const char* m_pcFile;
    const char* m_pcFunc;
#endif
    unsigned int m_uiLine;


    static const NiFLF UNKNOWN;

    const char* SourceFileStripper() const;
    const char* CreateTempFLFString();
};

class NISYSTEM_ENTRY NiAllocUnit
{
public:
    size_t m_stAllocationID;
    void* m_pvMem;
    size_t m_stAlignment;
    size_t m_stSizeRequested;
    size_t m_stSizeAllocated;

    // hash table support
    NiAllocUnit* m_pkPrev;
    NiAllocUnit* m_pkNext;

    unsigned long m_ulAllocThreadId;
    float m_fAllocTime;
    NiMemEventType m_eAllocType;

    NiFLF m_kFLF;

    void Reset();
};

class NISYSTEM_ENTRY NiMemTracker : public NiAllocator
{
    // Memory tracking interface
public:
    NiMemTracker(NiAllocator* pkActualAllocator,
        bool bWriteToLog = true,
        unsigned int uiInitialSize = 65536, 
        unsigned int uiGrowBy = 4096, bool bAlwaysValidateAll = false,
        bool bCheckArrayOverruns = true);
    virtual ~NiMemTracker();

    virtual void* Allocate(size_t& stSize, size_t& stAlignment,
        NiMemEventType eEventType, bool bProvideAccurateSizeOnDeallocate,
        const char* pcFile, int iLine, const char* pcFunction);
    virtual void Deallocate(void* pvMemory, 
        NiMemEventType eEventType, size_t stSizeinBytes);
    virtual void* Reallocate(void* pvMemory, size_t& stSize,
        size_t& stAlignment, NiMemEventType eEventType, 
        bool bProvideAccurateSizeOnDeallocate, size_t stSizeCurrent, 
        const char* pcFile, int iLine, const char* pcFunction);

    // Called by the memory manager during NiInit()
    virtual void Initialize();
    // Called by the memory manager during NiShutdown()
    virtual void Shutdown();

    virtual bool VerifyAddress(const void* pvMemory);

    // additional summary statistics
    size_t m_stActiveMemory; // in bytes
	size_t m_stPeakMemory; // in bytes
	size_t m_stAccumulatedMemory; // in bytes
    size_t m_stUnusedButAllocatedMemory; // in bytes

    size_t m_stActiveAllocationCount;
	size_t m_stPeakAllocationCount;
	size_t m_stAccumulatedAllocationCount;

	size_t m_stActiveTrackerOverhead;
	size_t m_stPeakTrackerOverhead;
	size_t m_stAccumulatedTrackerOverhead;
    
    float m_fPeakMemoryTime;
    float m_fPeakAllocationCountTime;

    size_t GetCurrentAllocationID() const;

    // Logging and reporting
    void LogAllocUnit(const NiAllocUnit *pkUnit, int iChannel,
        const char* pcPrefix = "", NiMemEventType eDeallocType = NI_UNKNOWN,
        float fDeallocTime = -FLT_MAX, unsigned long ulDeallocThreadId = 0,
        size_t stSizeUnused = 0) const;
    void LogMemoryReport() const;
    void LogSummaryStats() const;
    virtual void ResetSummaryStats();

    // tracking options
    bool GetAlwaysValidateAll() const;
    void SetAlwaysValidateAll(bool bOn);

    bool GetCheckForArrayOverruns() const;

    bool ValidateAllocUnit(const NiAllocUnit* pkUnit) const;
    bool ValidateAllAllocUnits() const;

    static size_t ms_stBreakOnAllocID;
    static void* ms_pvBreakOnAllocRangeStart;
    static void* ms_pvBreakOnAllocRangeEnd;
    static const char* ms_pcBreakOnFunctionName;
    static bool ms_bOutputLeaksToDebugStream;
    static size_t ms_stBreakOnSizeRequested;

    static NiMemTracker* Get();
protected:
    static const char* SourceFileStripper(const char *pcSourceFile);
    void GrowReservoir();
    void CreateXSLForLeaks();
    void CreateXSLForFreeReport();
    void InsertAllocUnit(NiAllocUnit* pkUnit);
    NiAllocUnit* FindAllocUnit(const void* pvMem) const;
    void RemoveAllocUnit(NiAllocUnit* pkUnit);
        
    static unsigned int AddressToHashIndex(const void* pvAddress);
    static const char* FormatForXML(const char* pcInString);
    void OutputLeakedMemoryToDebugStream() const;
    void OutputAllocUnitToDebugStream(NiAllocUnit* pkUnit) const;
    void MemoryFillWithPattern(void* pvMemory, size_t stSizeinBytes);
    size_t MemoryBytesWithPattern(void* pvMemory, size_t stSizeinBytes) const;

    // Note that MemoryFillForArrayOverrun and CheckForArrayOverrun
    // modify the address passed in to reflect the sentinel characters
    // before the allocation. PadForArrayOverrun will adjust the size
    // of an allocation to reflect the sentinel characters.
    void MemoryFillForArrayOverrun(void*& pvMemory, size_t stAlignment, 
        size_t stSizeOriginal);
    bool CheckForArrayOverrun(void*& pvMemory, size_t stAlignment, 
        size_t stSizeOriginal) const;
    size_t PadForArrayOverrun(size_t stAlignment, 
        size_t stSizeOriginal);

    static const unsigned int ms_uiHashBits = 12;
    static const unsigned int ms_uiHashSize = 1 << ms_uiHashBits;
    
    bool m_bAlwaysValidateAll;
    NiAllocUnit* m_pkReservoir;
    size_t m_stReservoirGrowBy;
    NiAllocUnit** m_ppkReservoirBuffer;
    size_t m_stReservoirBufferSize;
    NiAllocUnit* m_pkActiveMem[ms_uiHashSize];
    
    int m_iLoggerLeakChannel;
    int m_iLoggerCompleteChannel;

    virtual void LogActiveMemoryReport() const;

    NiAllocator* m_pkActualAllocator;
    size_t m_stCurrentAllocID;

    bool m_bCheckArrayOverruns;
    unsigned char m_ucFillChar;
    bool m_bWriteToLog;

    static NiMemTracker* ms_pkTracker;
    NiCriticalSection m_kCriticalSection;
};

#include "NiMemTracker.inl"

#endif // #ifndef NIMEMTRACKER_H
