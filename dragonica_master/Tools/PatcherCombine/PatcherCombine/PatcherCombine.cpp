// PatcherCombine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

FILE* g_pfOut = NULL;

void ShowHelp()
{
     _tprintf(_T("Usage: PatcherCombine DiffPackFile PatchExe ChangeLog OutPutFileName OldVersionNumber PatchVersionNumber\n"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc!=7)
	{
		ShowHelp();
		return(-1);
	}

	if(!CopyFile(argv[2], argv[4], FALSE)) 
	{
		_ftprintf(stderr, _T("Cannot produce output file.\n"));
		return(-1);
	}

	g_pfOut = _tfopen(argv[4], _T("r+b"));
	DWORD dwAddr = (DWORD)g_pfOut;

	if(!g_pfOut) 
	{
		_ftprintf(stderr, _T("Cannot produce output file.\n"));
		return(-1);
	}

	_tprintf(_T("Please wait..\n"));

	IMAGE_DOS_HEADER DosHdr;
	fread(&DosHdr, sizeof(DosHdr), 1, g_pfOut);
	fseek(g_pfOut, 0, SEEK_END);
	DWORD dwOffset = ftell(g_pfOut);

	BYTE pdnlen = wcslen(argv[5])*sizeof(wchar_t);
	fwrite(&pdnlen, sizeof(pdnlen), 1, g_pfOut);
	fwrite(argv[5], pdnlen, 1, g_pfOut);

	pdnlen = wcslen(argv[6])*sizeof(wchar_t);
	fwrite(&pdnlen, sizeof(pdnlen), 1, g_pfOut);
	fwrite(argv[6], pdnlen, 1, g_pfOut);

	//pdnlen = _tcslen(argv[7]);
	//fwrite(&pdnlen, sizeof(pdnlen), 1, g_pfOut);
	//fwrite(argv[7], pdnlen, 1, g_pfOut);

	FILE* pfLog = _tfopen(argv[3], _T("r+t"));
	if(!pfLog)
	{
		_ftprintf(stderr, _T("Cannot find changelog.txt file\n"));
		return(-1);
	}

	{
		IMAGE_DOS_HEADER DosHdrPack;
		fread(&DosHdrPack, sizeof(DosHdrPack), 1, pfLog);
		fseek(pfLog, 0, SEEK_END);
		DWORD dwPackleng = ftell(pfLog);
		fseek(pfLog, 0, SEEK_SET);

		char* szPack = new char[dwPackleng];

		fwrite(&dwPackleng, sizeof(dwPackleng), 1, g_pfOut);
		fread(szPack, dwPackleng, 1, pfLog);
		fwrite(szPack, dwPackleng, 1, g_pfOut);

		delete[] szPack;
	}
	
	FILE* pfPack = _tfopen(argv[1], _T("r+b"));
	if(!pfPack)
	{
		_ftprintf(stderr, _T("Cannot find DiffPackFile.\n"));
		return(-1);
	}

	
	IMAGE_DOS_HEADER DosHdrPack;
	fread(&DosHdrPack, sizeof(DosHdrPack), 1, pfPack);
	fseek(pfPack, 0, SEEK_END);
	DWORD dwPackleng = ftell(pfPack);
	fseek(pfPack, 0, SEEK_SET);
	char* szPack = new char[dwPackleng];

	fread(szPack, dwPackleng, 1, pfPack);
	fwrite(szPack, dwPackleng, 1, g_pfOut);

	delete[] szPack;
	
	IMAGE_NT_HEADERS32 pNtHdrs;
	DWORD dwPos = ((DWORD)&pNtHdrs.FileHeader.PointerToSymbolTable) - ((DWORD)&pNtHdrs);
	fseek(g_pfOut, DosHdr.e_lfanew+dwPos, SEEK_SET);
	fwrite(&dwOffset, sizeof(dwOffset), 1, g_pfOut);

	fclose(pfPack);
	fclose(g_pfOut);

	_tprintf(_T("Complete.\n"));

	return 0;
}

