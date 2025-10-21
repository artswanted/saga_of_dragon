#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "BM/STLSupport.h"

/*
class PgFileMapping
{
public:
	PgFileMapping();
	virtual ~PgFileMapping();

public:
	HRESULT Init(std::wstring const& wstrPackFileName, DWORD dwFileMode = OPEN_ALWAYS, size_t const fixed_file_size = 0);
	void Clear();
	
	char* Data() const {return (char*)m_pBuf;}
	size_t DataSize() const {return m_DataSize;}
protected:
	void *m_pBuf;
	size_t m_DataSize;
	HANDLE m_hPackFileHandle;
	HANDLE m_hFileMapping;

};
*/