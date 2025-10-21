#pragma once

#include	"defines.h"
#ifdef USE_TORRENT
# include "libupdate/libupdate.h"
#endif

#define GET_FILE_RETRY_COUNT	3

typedef enum ePatchVersionCheck
{
	E_PVC_NOT_FOUND = -1,
	E_PVC_FAIL = 0,
	E_PVC_SUCCESS = 1
} PatchVersionCheckResult;

//────────────────────────────────────────
//	패치
//────────────────────────────────────────
void	RecoveryPatchCore();
//void	RecoveryPatch();

//	공통
namespace PgPatcherRscUtil
{
	void ReplaceRsc();
	void ClearTempRsc();
};

//bool	GetServerList(BM::FolderHash& PackList, BM::FolderHash& NonPackList);
namespace PgPatchCoreUtil
{
	bool	OriginalNonPackUpdate(const BM::FolderHash& List);
	bool	OriginalPackUpdate(const BM::FolderHash& List);
	bool	OriginalPatchToOrgMerge(const std::wstring& TargetDir);
	bool	OriginalPatchIDCheck();
};

namespace PgNewPatchCoreUtil
{
	bool	NewNonPackUpdate(const PATCH_HASH& kPatchHash);
	bool	NewPackUpdate(const PATCH_HASH& kPatchHash);
};

//
class PgHeaderMergePatch
{
public:
	PgHeaderMergePatch();
	virtual ~PgHeaderMergePatch();

public:
	void Do();

private:
	PatchVersionCheckResult GetServerList(PATCH_HASH& PackList, PATCH_HASH& NonPackList);
	bool GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList);
};

//
#ifdef USE_TORRENT
class PgTorrentPatch
{
public:
	PgTorrentPatch() {}
	~PgTorrentPatch()
	{
		libupdate::torrent::deinit();
	}

public:
	void Do();
};
#endif

//
class PgRecoveryPatch
{
public:
	PgRecoveryPatch();
	virtual ~PgRecoveryPatch();

public:
	void Do();

private:
	void OptimizeList(PATCH_HASH& rkList);
	bool GetServerList(PATCH_HASH& PackList, PATCH_HASH& NonPackList);
	bool GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList);
	bool MakeDiffList(PATCH_HASH const& ServerList, BM::FolderHash& ClientList, PATCH_HASH& ResultList);
};

//────────────────────────────────────────
//	기타
//────────────────────────────────────────