// stub for msvc2022 better to full remove gg from game because this anti hack is outdated
#include "ggsrv25.h"

unsigned long __CDECL InitGameguardAuth(char*, unsigned long, int, int)
{
    return ERROR_INVALID_FUNCTION;
}

unsigned long __CDECL SetUpdateCondition(int, int)
{
    return ERROR_INVALID_FUNCTION;
}

void  __CDECL CleanupGameguardAuth()
{
}

unsigned long __CDECL GGAuthUpdateTimer()
{
    return ERROR_INVALID_FUNCTION;
}

CCSAuth2::CCSAuth2()
{
}

CCSAuth2::~CCSAuth2()
{
}

void CCSAuth2::Init()
{
}

unsigned long CCSAuth2::GetAuthQuery()
{
    return ERROR_INVALID_FUNCTION;
}

unsigned long CCSAuth2::CheckAuthAnswer()
{
    return ERROR_INVALID_FUNCTION;
}
