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

//---------------------------------------------------------------------------
inline void NiSleep(unsigned int uiMilliseconds)
{
    Sleep(uiMilliseconds);
}
//---------------------------------------------------------------------------
inline void NiYieldThread()
{
    // Preferably, we could call SwitchToThread here, but that requires 
    // knowing that your target platform is at least WinNT.
    Sleep(0);
}
//---------------------------------------------------------------------------
inline NiInt32 NiAtomicIncrement(NiInt32& i32Value)
{
    return InterlockedIncrement((LONG*)&i32Value);
}
//---------------------------------------------------------------------------
inline NiInt32 NiAtomicDecrement(NiInt32& i32Value)
{
    return InterlockedDecrement((LONG*)&i32Value);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiAtomicIncrement(NiUInt32& ui32Value)
{
    return InterlockedIncrement((LONG*)&ui32Value);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiAtomicDecrement(NiUInt32& ui32Value)
{
    assert (ui32Value > 0);
    return InterlockedDecrement((LONG*)&ui32Value);
}
//---------------------------------------------------------------------------
inline float NiSelectGE(float fCondition, float fResultIfGreaterThanEqualZero, 
	float fResultIfLessThanZero)
{
	return fCondition >= 0 ? 
		fResultIfGreaterThanEqualZero : fResultIfLessThanZero;
}
//---------------------------------------------------------------------------
inline float NiSelectGE(int iCondition, float fResultIfGreaterThanEqualZero, 
	float fResultIfLessThanZero)
{
	return iCondition >= 0 ? 
		fResultIfGreaterThanEqualZero : fResultIfLessThanZero;
}
//---------------------------------------------------------------------------
inline int NiSelect(bool iCondition, int iResultIfTrue, int iResultIfFalse)
{
	return iCondition ? iResultIfTrue : iResultIfFalse;
}
//---------------------------------------------------------------------------
inline float NiSelect(bool iCondition, float fResultIfTrue, 
    float fResultIfFalse)
{
	return iCondition ? fResultIfTrue : fResultIfFalse;
}
//---------------------------------------------------------------------------
inline unsigned long NiGetCurrentThreadId()
{
    return (unsigned long)GetCurrentThreadId();
}
//---------------------------------------------------------------------------
inline char* NiStrcpy(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcpy_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    assert(stDestSize != 0);

    size_t stSrcLen = strlen(pcSrc);
    assert(stDestSize > stSrcLen); // > because need null character

    size_t stWrite;
    
    if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
        stWrite = stDestSize;
    else 
        stWrite = stSrcLen + 1;

    char *pcResult = strncpy(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrncpy(char* pcDest, size_t stDestSize, const char* pcSrc, 
    size_t stCount)
{
#if _MSC_VER >= 1400
    strncpy_s(pcDest, stDestSize, pcSrc, stCount);
    return pcDest;
#else // #if _MSC_VER >= 1400
    
    assert (pcDest != 0 && stDestSize != 0);
    assert (stCount < stDestSize || stCount == NI_TRUNCATE);

    if (stCount >= stDestSize)
    {
        if (stCount != NI_TRUNCATE)
        {
            pcDest[0] = '\0';
            return pcDest;
        }
        else
        {
            stCount = stDestSize - 1;
        }
    }

    char* pcResult = strncpy(pcDest, pcSrc, stCount);
    pcResult[stCount] = '\0';

    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrcat(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcat_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    size_t stSrcLen = strlen(pcSrc);
    size_t stDestLen = strlen(pcDest);
    assert(stSrcLen + stDestLen <= stDestSize - 1);

    size_t stWrite = stDestSize - 1 - stDestLen;  

    char* pcResult = strncat(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;

#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* NiStrtok(char* pcString, const char* pcDelimit, 
    char** ppcContext)
{
#if _MSC_VER >= 1400
    return strtok_s(pcString, pcDelimit, ppcContext);
#else // #if _MSC_VER >= 1400
    return strtok(pcString, pcDelimit);
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline int NiSprintf(char* pcDest, size_t stDestSize, const char* pcFormat, 
    ...)
{
    if (stDestSize == 0)
        return true;

    va_list kArgs;
    va_start(kArgs, pcFormat);
#if _MSC_VER >= 1400
    int iRet = vsprintf_s(pcDest, stDestSize, pcFormat, kArgs);
#else // #if _MSC_VER >= 1400
    int iRet = _vsnprintf(pcDest, stDestSize, pcFormat, kArgs);

    if (iRet >= 0 && ((unsigned int)iRet == stDestSize - 1) &&
        pcDest[stDestSize - 1] != '\0')
    {
        // This is a rare case where the written string fits but
        // is not null terminated. We will report this as an error.
        iRet = -1;
    }
#endif // #if _MSC_VER >= 1400
    va_end(kArgs);
       
    pcDest[stDestSize - 1] = '\0';

    return iRet;
}
//---------------------------------------------------------------------------
inline int NiMemcpy(void* pvDest, size_t stDestSize, const void* pvSrc, 
    size_t stCount)
{
    int iRet = 0;

#if _MSC_VER >= 1400
    iRet = memcpy_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
    if (stDestSize < stCount)
        iRet = -1;
    else
        memcpy(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

    assert(iRet != -1);
    return iRet;
}
//---------------------------------------------------------------------------
inline int NiMemcpy(void* pvDest, const void* pvSrc, size_t stCount)
{
    // This version of NiMemcpy is to be used to retrofit existing code,
    // however, the other version of NiMemcpy should be used whenever
    // possible.
    return NiMemcpy(pvDest, stCount, pvSrc, stCount);
}
//---------------------------------------------------------------------------
inline int NiMemmove(void* pvDest, size_t stDestSize, const void* pvSrc, 
    size_t stCount)
{
    int iRet = 0;

#if _MSC_VER >= 1400
    iRet = memmove_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
    if (stDestSize < stCount)
        iRet = -1;
    else
        memmove(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

    assert(iRet != -1);
    return iRet;
}
//---------------------------------------------------------------------------
// NiWChar version of function to copy one string to another.
inline NiWChar* NiWStrcpy(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc)
{

#if _MSC_VER >= 1400
    wcscpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
    return pkDest;
#else // #if _MSC_VER >= 1400
    assert(stDestSize != 0);

    size_t stSrcLen = wcslen(pkSrc);
    assert(stDestSize > stSrcLen); // > because need null character

    size_t stWrite;
  
    if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
        stWrite = stDestSize;
    else 
        stWrite = stSrcLen + 1;

    NiWChar* pkResult = wcsncpy(pkDest, pkSrc, stWrite);
    pkResult[stDestSize - 1] = '\0';
    return pkResult;
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to copy characters of one string to another.
inline NiWChar* NiWStrncpy(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc, size_t stCount)
{

#if _MSC_VER >= 1400
    wcsncpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc, stCount);
    return pkDest;
#else // #if _MSC_VER >= 1400
    assert (pkDest != 0 && stDestSize != 0);
    assert (stCount < stDestSize || stCount == NI_TRUNCATE);

    if (stCount >= stDestSize)
    {
        if (stCount != NI_TRUNCATE)
        {
            pkDest[0] = '\0';
            return pkDest;
        }
        else
        {
            stCount = stDestSize - 1;
        }
    }

    NiWChar* pkResult = wcsncpy(pkDest, pkSrc, stCount);
    pkResult[stCount] = '\0';

    return pkResult;
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to append characters of a string.
inline NiWChar* NiWStrcat(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkSrc)
{

#if _MSC_VER >= 1400
    wcscat_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
    return pkDest;
#else // #if _MSC_VER >= 1400
    size_t stSrcLen = wcslen(pkSrc);
    size_t stDestLen = wcslen(pkDest);
    assert(stSrcLen + stDestLen <= stDestSize - 1);

    size_t stWrite = stDestSize - 1 - stDestLen;  

    NiWChar* pkResult = wcsncat(pkDest, pkSrc, stWrite);
    pkResult[stDestSize - 1] = '\0';
    return pkResult;

#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to find the next token in a string.
inline NiWChar* NiWStrtok(NiWChar* pkString, const NiWChar* pkDelimit, 
    NiWChar** ppkContext)
{

#if _MSC_VER >= 1400
    return (NiWChar *)wcstok_s((wchar_t *)pkString,
        (const wchar_t *)pkDelimit, (wchar_t **)ppkContext);
#else // #if _MSC_VER >= 1400
    return wcstok(pkString, pkDelimit);
#endif // #if _MSC_VER >= 1400

}
//---------------------------------------------------------------------------
// NiWChar version of function to write formatted output using a pointer to a
// list of arguments.
inline int NiWSprintf(NiWChar* pkDest, size_t stDestSize, 
    const NiWChar* pkFormat, ...)
{

    if (stDestSize == 0)
        return 0;

    va_list kArgs;
    va_start(kArgs, pkFormat);

#if _MSC_VER >= 1400
    int iRet = vswprintf_s((wchar_t *)pkDest, stDestSize,
        (const wchar_t *)pkFormat, kArgs);
#else // #if _MSC_VER >= 1400
    int iRet = vswprintf(pkDest, stDestSize, pkFormat, kArgs);

    if (iRet >= 0 && ((unsigned int)iRet == stDestSize - 1) &&
        pkDest[stDestSize - 1] != '\0')
    {
        // This is a rare case where the written string fits but
        // is not null terminated. We will report this as an error.
        iRet = -1;
    }
#endif // #if _MSC_VER >= 1400

    va_end(kArgs);
       
    pkDest[stDestSize - 1] = '\0';

    return iRet;
}
//---------------------------------------------------------------------------
#ifndef NISHIPPING
inline void NiSetOutputDebugStringCallback(NIOUTPUTDEBUGSTRING_CALLBACK pfnCallback)
{
	g_NiOutputDebugStringCallBack = pfnCallback;
}
inline void NiWriteToStdErr(char* pText)
{
    fputs(pText, stderr);
}
//---------------------------------------------------------------------------
#ifndef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
inline void NiOutputDebugString(const char* pcOut)
{
    OutputDebugStringA(pcOut);
	if (g_NiOutputDebugStringCallBack)
		g_NiOutputDebugStringCallBack(pcOut);
}
#endif // #ifndef REPLACE_NIOUTPUTDEBUGSTRING_WITH_NILOG
#endif
//---------------------------------------------------------------------------