#include "stdafx.h"
#include "../Common.h"//! 로그 객체
#include "../Header/Session_Manager.h"//! 로그 객체

using namespace CEL;

//! 락을 걸게 된다면. 다른 어떠한 멤버락 보다도 우선하게 하라.
//! CSessionMgrLock			g_SessionMgrLock;//! 세션 해쉬 내용이 변하지 않아야 하는곳. 세션 해쉬 내용이 변하게 하는곳에서만 락을 걸라.

void MakeEndian(int& rDest, int const iValue)
{
	char *pTemp = (char*)(&rDest);
	*pTemp = (iValue >> 24	)& 0xFF;	++pTemp;
	*pTemp = (iValue >> 16	)& 0xFF;	++pTemp;
	*pTemp = (iValue >> 8	)& 0xFF;	++pTemp;
	*pTemp = (iValue >> 0	)& 0xFF;	
}

void ReadEndian(int const rSrc, int &iRet)
{
	char *pResult = (char*)(&iRet);
	*pResult = (rSrc >> 24	)& 0xFF;	++pResult;
	*pResult = (rSrc >> 16	)& 0xFF;	++pResult;
	*pResult = (rSrc >> 8	)& 0xFF;	++pResult;
	*pResult = (rSrc >> 0	)& 0xFF;
}

