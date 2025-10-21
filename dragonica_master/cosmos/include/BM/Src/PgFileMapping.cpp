#include "stdafx.h"
#include "BM/PgFileMapping.h"
#include "BM/Filesupport.h"

using namespace BM;
/*
PgFileMapping::PgFileMapping()
{
	m_pBuf = NULL;
	m_hPackFileHandle = INVALID_HANDLE_VALUE;
	m_hFileMapping = INVALID_HANDLE_VALUE;
	m_DataSize =0;
}

PgFileMapping::~PgFileMapping()
{
	Clear();
}

HRESULT PgFileMapping::Init(std::wstring const& wstrPackFileName, DWORD dwFileMode, size_t const fixed_file_size)
{
	Clear();
	size_t const szPackFileSize = ((fixed_file_size)?fixed_file_size:BM::GetFileSize(wstrPackFileName));

	m_hPackFileHandle = ::CreateFile(wstrPackFileName.c_str(),
													GENERIC_READ | GENERIC_WRITE,
													FILE_SHARE_READ,
													NULL,
													dwFileMode,//OPEN_ALWAYS,//CREATE_ALWAYS, 
													FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 
													NULL);
	if(m_hPackFileHandle == INVALID_HANDLE_VALUE)
	{
		HRESULT const hRet = GetLastError();
		assert(NULL);
		__asm int 3;
		return hRet;
	}

	if(szPackFileSize)
	{//파일 사이즈가 있어야 맵핑이 됨.
		m_hFileMapping
			= CreateFileMapping(
					m_hPackFileHandle,			// use paging file
					NULL,						// default security 
					PAGE_READWRITE,			// read/write access
					0,							// max. object size 
					(DWORD)szPackFileSize,// BUF_SIZE,             // buffer size  
					NULL);	// name of mapping object
	 
		if(m_hFileMapping == NULL || m_hFileMapping == INVALID_HANDLE_VALUE) 
		{ 
			HRESULT const hRet = GetLastError();
			assert(NULL);
			__asm int 3;
			return hRet;
		}

		m_pBuf = MapViewOfFile(m_hFileMapping,	// handle to map object
								SECTION_EXTEND_SIZE|SECTION_MAP_READ|SECTION_MAP_WRITE, // read/write permission
								0,                   
								0,                   
								szPackFileSize);
		if(m_pBuf == NULL) 
		{ 
			HRESULT const hRet = GetLastError();
			assert(NULL);
			__asm int 3;
			return hRet;
		}

		m_DataSize = szPackFileSize;
		return S_OK;
	}
	__asm int 3;
	return E_FAIL;
}

void PgFileMapping::Clear()
{
	if(m_pBuf)
	{
		if(!UnmapViewOfFile(m_pBuf))
		{
//			HRESULT const hRet = GetLastError();
			assert(NULL);
			__asm int 3;
		}
		m_pBuf = NULL;
		m_DataSize = 0;
	}

	if(m_hFileMapping != INVALID_HANDLE_VALUE)
	{
		if(!CloseHandle(m_hFileMapping))//맵핑 먼저 닫고
		{
//			HRESULT const hRet = GetLastError();
			assert(NULL);
			__asm int 3;
		}
		m_hFileMapping = INVALID_HANDLE_VALUE;
	}

	if(m_hPackFileHandle != INVALID_HANDLE_VALUE)
	{
		if(!CloseHandle(m_hPackFileHandle))//파일 자체를 닫는다.
		{
//			HRESULT const hRet = GetLastError();
			assert(NULL);
			__asm int 3;
		}
		m_hPackFileHandle = INVALID_HANDLE_VALUE;
	}
}

*/