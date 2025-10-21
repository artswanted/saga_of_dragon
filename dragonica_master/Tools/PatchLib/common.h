#pragma once

#include <windows.h>

typedef enum ePatch_Result
{
	PR_OK=0, 						// 패치 성공
	PR_FILENAME_EMPTY,				// static int FetchFile(LPCTSTR pszFile, PFCALLBACK pCallback); 함수에서 파일명이 NULL 일때
	PR_NOT_FIND_PATCH_ID,			// PATCH.ID 파일을 찾을수 없을때
	PR_NOT_READ_PATCH_ID,			// PATCH.ID 파일에 오류가 있을때
	PR_ALREADY_PATCHED,				// 패치할 버전과 설치되어 있는 버전이 같을때
	PR_DIFFERENT_VERSION,			// 맞는 패치 버전이 아닐때, (예:패치버전 0.10.3->0.10.4  설치된 버전이 0.10.1 일때)
	PR_PATCH_DATA_EMPTY,			// 패치할 데이타 리스트를 못 읽었을때
	PR_NOT_OPEN_PACK_FILE,			// 팩 파일을 읽기를 실패했을때
	PR_NOT_FIND_FILE_IN_PACK,		// 파일 리스트에는 있으나 팩에 파일이 없을때
	PR_NOT_CREATE_NEW_PACK_FILE,	// 새로운 팩 파일 생성에 실패했을때
	PR_NOT_HAVE_PATCH_LIST,			// 패치 리스트에 내용이 없을때
	PR_UPDATE_PACK_VERSION_FAIL,	// 팩 파일에 새로운 버전 넣기를 실패했을때
	PR_FILE_OPEN_FAIL,				// 패치 파일을 읽는데 실패 했을때
	PR_NOT_HAVE_DATA,				// 패치 파일 내부에 패치 데이타가 없을때
}PATCH_RESULT;

typedef int (CALLBACK *PFCALLBACK)(LPCTSTR pCurFileName,float fCurrent, float fTotal);


class PatchLib;
class CPatch
{
public:
	explicit CPatch(){}
	~CPatch(){}
public:
	static int FetchFile(LPCTSTR pszFile, PFCALLBACK pCallback);
private:
	static PatchLib& GetPatchLib();
};
