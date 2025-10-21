#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <cassert>
#include <time.h>
#include <algorithm>
#include <errno.h>
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <bitset>
#include <fstream>
#include <iostream>
#include <xutility>
#include "Loki/Singleton.h"

#define SINGLETON_STATIC(cls_name)  Loki::SingletonHolder< cls_name, ::Loki::CreateStatic >::Instance()
#define SINGLETON_CUSTOM(cls_name, allocator)  Loki::SingletonHolder< cls_name, allocator >::Instance()

#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(p)			{ if(p) { delete (p);		(p)=NULL; } }
#endif
#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);		(p)=NULL; } }
#endif
#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL; } }
#endif
#if !defined(SAFE_FCLOSE)
	#define SAFE_FCLOSE(p)			{ if(p) { ::fclose(p);		(p)=NULL; } }
#endif
#if !defined(SAFE_RELEASE2)
	#define SAFE_RELEASE2(p)			{ if(p) { (p)->release();	(p)=NULL; } }
#endif


template < typename T_DEST >//Null Terminate 가 되지 않는 경우의 방어.
errno_t SAFE_STRNCPY( T_DEST &T_Dest, char const* pSrc )
{//pSrc 가 NULL 이 들어와도 strncpy_s가 방어 한다.
	if(T_Dest && pSrc)
	{
		size_t const dest_memory_size = sizeof(T_Dest);
		size_t const dest_element_count = dest_memory_size/sizeof(char);
		size_t const dest_last_index = dest_element_count-1;

		errno_t const ret = ::strncpy_s(T_Dest, dest_element_count, pSrc, dest_last_index ); //복사는 Null 문자 감안하고 해라.
		T_Dest[dest_last_index] = '\0';
		return ret;
	}
	return -1;
}

template < typename T_DEST >//Null Terminate 가 되지 않는 경우의 방어.
errno_t SAFE_STRNCPY( T_DEST &T_Dest, wchar_t const* pSrc )
{//pSrc 가 NULL 이 들어와도 strncpy_s가 방어 한다.
	if(T_Dest && pSrc)
	{
		size_t const dest_memory_size = sizeof(T_Dest);
		size_t const dest_element_count = dest_memory_size/sizeof(wchar_t);
		size_t const dest_last_index = dest_element_count-1;

		errno_t const ret = ::wcsncpy_s(T_Dest, dest_element_count, pSrc, dest_last_index ); //복사는 Null 문자 감안하고 해라.
		T_Dest[dest_last_index] = '\0';
		return ret;
	}
	return -1;
}

template < typename T_DEST, typename T_SRC >//T_SRC = string, wstring 만 쓰라.  string 을 넣는 실수를 범해도 커버 된다.. 
errno_t SAFE_STRNCPY( T_DEST &T_Dest, T_SRC const& rSrc )
{
	if(!T_Dest){return -1;}

	size_t const dest_memory_size = sizeof(T_Dest);//카운트를 뺄 수가 없네 -_-;..

	if( dest_memory_size > rSrc.size() * sizeof(T_SRC::value_type) )//메모리 공간 체크 
	{//같아도 안됨. Null Terminate 때문에
		return SAFE_STRNCPY(T_Dest, rSrc.c_str() );
	}
	return -1;
}

//map 엘리멘트 삭제 하는 클래스 샘플.
class DestroyElement
{
public:
	template< typename T1 >
	void operator ()(T1 &t1)
	{
		SAFE_DELETE(t1.second);
	}
};

template< typename T_CONT >
HRESULT RandomElement(const T_CONT &kCont, typename T_CONT::const_iterator &out_itor)
{
	if(!kCont.size())
	{
		return E_FAIL;
	}

	std::vector< T_CONT::const_iterator > kContItor;
	kContItor.reserve(kCont.size());
	
	T_CONT::const_iterator itor = kCont.begin();
	while(itor != kCont.end())
	{
		kContItor.push_back(itor);
		++itor;
	}

	size_t const index = BM::Rand_Index(kCont.size());

	out_itor = kContItor.at(index);
	return S_OK;
}
