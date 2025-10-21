// MakePatchIDConsole.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MakePatchIDConsole.h"

void ShowHelp()
{
     _tprintf(_T("Usage: MakePatchIDConsole.exe VersionNumber OutputFolder\n"));
	 _tprintf(_T("Usage: MakePatchIDConsole.exe PatchIDFolder\n"));
}


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc == 3)
	{
		std::wstring strVersionNumber = argv[1];
		std::wstring strOutputFolder = argv[2];
		CreatePatchID(strVersionNumber, strOutputFolder);
	}
	else if(argc == 2)
	{
		std::wstring strPatchID = argv[1];
		LoadPatchID(strPatchID);
	}
	else
	{
		ShowHelp();
		return(-1);
	}
	return 0;
}

void CreatePatchID(std::wstring strVersionNumber, std::wstring strOutputFolder)
{
	SetCurrentDirectoryW(strOutputFolder.c_str());
	std::string strPatchID = MB(strVersionNumber);

	std::vector<char> vecPatchID;
	vecPatchID.resize(strPatchID.size());
	::memcpy(&vecPatchID.at(0), strPatchID.c_str(), strPatchID.size());
	//	저장
	BM::SimpleEncrypt(vecPatchID, vecPatchID);
	BM::MemToFile(L"PATCH.ID", vecPatchID, false);
	//BM::MemToFileNotReserveFolder(L"PATCH.ID", vecPatchID);

	_tprintf(L"Complete");
}

void LoadPatchID(std::wstring strPatchID)
{
	SetCurrentDirectoryW(strPatchID.c_str());
	std::vector<char>	vec_PatchID;	//암호화된 ID
	BM::FileToMem(L"PATCH.ID", vec_PatchID);
	if(vec_PatchID.size())
	{
		BM::SimpleDecrypt(vec_PatchID, vec_PatchID);
		vec_PatchID.push_back('\0');

		char*	pIDSrc	= new char[vec_PatchID.size()];
		memcpy(pIDSrc, &vec_PatchID.at(0), vec_PatchID.size());

		std::wstring strOut = UNI(pIDSrc);

		_tprintf(strOut.c_str());

		delete [] pIDSrc;
	}
}