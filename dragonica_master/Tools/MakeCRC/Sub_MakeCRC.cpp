#include "stdafx.h"

#include "BM/bm.h"
#include "BM/FileSupport.h"

void foo()
{
	std::vector< char > vecData;

	BM::FileToMem(_T("./Config.ini"), vecData);

	BM::EncSave(_T("./Config.inb"), vecData);
}

void voo()
{
	std::vector< char > vecData;

	BM::DecLoad(_T("./Config.inb"), vecData);

	BM::MemToFile(_T("./Config.ini"), vecData);
}