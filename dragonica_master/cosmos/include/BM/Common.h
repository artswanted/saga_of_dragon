#pragma once

#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <errno.h>
#include <vector>
#include <lm.h>
#include <cassert>
#include <iostream>
#include <limits>
#include <mmsystem.h>
#include <Winsock2.h>

#include "Ace/os_ns_sys_time.h"
#include "Ace/time_value.h"
#include "lzo/lzo.h"
#include "zlib/zlib.h"

namespace BM
{
	extern DWORD GetTime32();
	extern UINT64 GetTime64();
	extern INT64 DiffTime(UINT64 const& ui64Start, UINT64 const& ui64End);
	extern bool TimeCheck(UINT64 &ui64TImeValue, UINT64 const ui64Interval);
	extern bool TimeCheck(DWORD &dwTImeValue, DWORD const dwInterval);

	extern bool Compress(char const *pSrc, unsigned long src_size, std::vector< char > &vecDest);
	extern bool UnCompress(std::vector< char > const& vecSrc, char const *pDest, unsigned long dest_size);
	extern bool Compress(std::vector< char > const& vecSrc, std::vector< char > &vecDest);
	extern bool UnCompress(std::vector< char > const& vecSrc, std::vector< char > &vecDest);//vecDest 사이즈를 셋팅 하고 넣을것
	extern HRESULT MacAddress(__int64 &iOut);
	extern HRESULT IPAddress(int &iOut);

	extern HRESULT LZOInit();
	extern bool LZOCompress(char const *pSrc, unsigned long src_size, std::vector< char > &vecDest);
	extern bool LZOCompress(std::vector< char > const& vecSrc, std::vector< char > &vecDest);
	extern bool LZOUnCompress(std::vector< char > const& vecSrc, std::vector< char > const& vecDest);

	extern int Rand_Range(int const iInMax, int const iInMin = 0);//범위값 뽑을때 주로 쓰는, max 값을 포함하는 Rand
	extern int Rand_Index(int const iInMax);//인덱스 뽑을때 주로 쓰는, max 값을 포함하지 않는 Rand(0~N)
	extern double Rand_Unit();//0.0~1.0 까지의 값을 돌려주는 Rand
	extern bool IsCorrectType(bool const &k);

}

template<typename _CountofType, size_t _SizeOfArray>
void StrMakeCorrect(_CountofType (&_Array)[_SizeOfArray])
{//char or TCHAR 배열의 마지막에 NullTerminate 처리를 해줌.
	_Array[_SizeOfArray-1] = 0;
}

template<typename _CountofType, size_t _SizeOfArray>
bool StrIsCorrect(_CountofType (&_Array)[_SizeOfArray])
{//char or TCHAR 배열의 마지막에 NullTerminate 처리를 해줌.
	
	size_t i = 0;
	while(_SizeOfArray > i)
	{
		if(0 == _Array[i])
		{
			return true;
		}
		++i;
	}
	return false;
}
