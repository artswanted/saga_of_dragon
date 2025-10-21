#pragma once

#include	"defines.h"

typedef	enum eRequestTarget
{
	ERT_DOWNLOAD,
	ERT_PACK,
}EREQUESTTARGET;

typedef enum eThrowType
{
	eTHROW_SUCCESS	= 0,
	eTHROW_FAILED	= 1,
	eTHROW_CANCEL,
	eTHROW_TRYRECOVERY,
}ETHROW_TYPE;

extern	unsigned __stdcall OnlinePatch(void* pArgument);
//extern	unsigned __stdcall OfflinePatch(void* pArgument);
extern	std::vector<std::wstring>	g_kPackFolders;

const std::wstring	g_wstrGuardini = L"DRAGONICA.INI";
const std::wstring	g_wstrGuarddes = L"GAMEGUARD.DES";

typedef std::map<__int64, BM::FolderHash> PATCH_HASH;
//────────────────────────────────────────
//	패치 - 공통
//────────────────────────────────────────
void	PatcherCheck();
void	PatcherPatch();
void	PackFolderInit();
void	ConnectServer();
void	PatchOptionINI();
//bool	GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList);
void	CHSOpenLuncher();
void	InitLogReport();
void	NewPatchMethod();

//────────────────────────────────────────
//	기타 - 공통
//────────────────────────────────────────
BM::VersionInfo	GetPatchID(std::wstring const& strFileName);
std::wstring	GetPatchNumber(std::wstring const& PatchID, bool const bIsOffline = false, int const nPos = 0);
std::wstring	GetClientVer(std::wstring const& PatchID);
size_t	GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value = ERT_DOWNLOAD);
size_t	GetListDataSize(PATCH_HASH const& kPatchHash);
bool	UpdatePackVer();
bool	DecFile(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncrypted, std::vector<char> const& vecData, std::vector<char>& vecOut);
bool	CreateIDFile(std::wstring const& wstrID);
bool	UnCompFile(size_t const& OriginalSize, std::vector<char>& vecData, std::vector<char>& vecOut);
bool	MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList);
