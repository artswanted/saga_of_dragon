#pragma once

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include "BM/BM.h"
#include "DataPack/PgDataPackManager.h"
#include "BM/FileSupport.h"
#include "SimpEnc/SimpEnc.h"
#include "common.h"

#ifdef	_DEBUG
#pragma	comment	(lib, "DataPack_MTd.lib")
#pragma	comment	(lib, "SimpEnc_MTd.lib")
#elif defined(EXTERNAL_RELEASE)
#pragma	comment	(lib, "DataPack_MD.lib")
#pragma	comment	(lib, "SimpEnc_MD.lib")
#elif defined(EXTERNAL_RELEASE_STATIC)
#pragma	comment	(lib, "DataPack_MT.lib")
#pragma	comment	(lib, "SimpEnc_MT.lib")
#else
#pragma	comment	(lib, "DataPack_MT.lib")
#pragma	comment	(lib, "SimpEnc_MT.lib")
#endif

typedef	enum eRequestTarget
{
	ERT_DOWNLOAD,
	ERT_PACK,
}EREQUESTTARGET;

class PatchLib
{
public:
	explicit PatchLib();
	~PatchLib();

private:
	size_t m_nPatchHeaderSize;
	std::wstring m_wstrTextState;

	static float			m_fTotalPercent;
	static float			m_fFilePercent;
	size_t			m_TotalSize;
	size_t			m_FileSize;
	size_t			m_CntFileSize;
	size_t			m_CntTotalSize;

public:
	PATCH_RESULT PatchProcess(std::wstring wstrFile);
	float	GetTotalPer()
	{
		return m_fTotalPercent;
	}
	float	GetFilePer()
	{
		return m_fFilePercent;
	}
	//typedef bool (CALLBACK *PFCALLBACK)(float fCurrent, float fTotal);
	void SetCallbackFunc(PFCALLBACK pStateCallback);

private:
	void Reset();
	std::wstring GetPatchID(std::wstring const& strFileName);
	PATCH_RESULT ClientVersionCheck(std::wstring const& strOldVersion, std::wstring const& strNewVersion);
	void DecFile(std::vector<char>& vecData);
	void UnCompFile(std::vector<char>& vecData, size_t const& OriginalSize);
	PATCH_RESULT AmendNonPackUpdate(const BM::FolderHash& PatchList, char* szData);
	PATCH_RESULT MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList);
	size_t GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value = ERT_DOWNLOAD);
	PATCH_RESULT AmendPatchToOrgMerge(const std::wstring& TargetDir, const BM::FolderHash& kPatchHash, char* szData);
	PATCH_RESULT AmendPackUpdate(const BM::FolderHash& PatchList, char* szData);
	PATCH_RESULT DividePatchList(BM::FolderHash& kResultPackList, BM::FolderHash& kResultNonPackList,
		const BM::FolderHash& kPackList, const std::vector<std::wstring>& kPackFolderList);
	PATCH_RESULT PackPatch(char* szData, DWORD dwSize, std::vector<std::wstring> const& kPackFolders);
	void MakePatchID(std::string strPatchID);
	__int64 ReadPatchVer(std::wstring strVersion);
	PATCH_RESULT UpdatePackVer(BM::FolderHash& kAllPackList, std::wstring strNewVersion);
	PATCH_RESULT GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList, std::vector<std::wstring>	const& kPackFolders);
	//bool Patch(wchar_t const* wszFile);
	void	SetTotalPer(float Per);
	void	SetFilePer(float Per);
	void	SetTotalSize(size_t size);
	void	SetFileSize(size_t size);
	void	SetTotalCnt(size_t size);
	void	SetFileCnt(size_t size);

	std::wstring m_strFileName;

	static PFCALLBACK m_pStateCallback;
};