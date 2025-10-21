#pragma once

#include "CEL/common.h"
#include "BM/PgTask.h"
#include "DataPack/PgDataPackManager.h"

enum E_WORK_STATE
{
	EWT_PREPARE		= 0,
	EWT_PROGRESS	= 1,
	EWT_END,
};

enum E_PROCESS_TYPE
{
	EPT_NONE		= 0,
	EPT_DIFF		= 1,
	EPT_PACK		= 2,
	EPT_CREATELIST	= 3,
	EPT_HEADERMERGE	= 4,
	EPT_BIND		= 5,
	EPT_UNPACK		= 6,
	EPT_MAKEINB		= 7,
	EPT_EXPORT_LIST	= 8,
	EPT_CREATE_ID	= 9,
	EPT_DAT_VERSION_UP = 10,
	EPT_DAT_VERSION_CHECK = 11,
	EPT_DAT_CONVERT = 12,
	EPT_MAKE_MANUAL_PATCH = 13,
	EPT_MAKE_AUTO_PATCH = 14,
	EPT_END,
};

enum E_PROCESS_ADDON_TYPE
{
	EPAT_TARGET_DIR		= 0,
	EPAT_CURRENT_DIR	= 1,
	EPAT_OUTPUT_DIR		= 2,
};

enum E_DIFF_OPTION
{
	EDO_DEFAULT		= 0x00000000,
	EDO_FILETOBIND	= 0x00000001 << 0,
	EDO_FILETOPACK	= 0x00000001 << 1,
	EDO_DELLIST		= 0x00000001 << 2,
};

enum E_CREATELIST_OPTION
{
	ECO_DEFAULT			= 0,
	ECO_TGT_PACKRESULT	= 1,
};

enum E_LOG_MSG
{
	ELM_NONE			= 0,
	ELM_START			= 1,
	ELM_END				= 2,
	ELM_FAIL,
	ELM_FOLDER_SEEK_FAIL,
	ELM_FILE_LIST_CREATE_FAIL,
	ELM_FILE_STATE_UPDATE_FAIL,
	ELM_FILE_TO_PACK_FAIL,
	ELM_CREATE_EXPORT_FILE_FAIL,
	ELM_NOT_EXIST_NEW_HEADER_LIST,
	ELM_NOT_EXIST_OLD_HEADER_LIST,
	ELM_NOT_EXIST_UNPACK_FILE_LIST,
	ELM_NOT_EXIST_UNPACK_FOLDER_DATA,
	ELM_NOT_EXIST_UNPACK_FILE_DATA,
	ELM_PATCHID_VERSION_INFO,
	ELM_BINDPACK_VERSION_INFO,
	ELM_PROGRESS,
	ELM_ENUM_END,
};

enum E_VERSION_TYPE
{
	EVT_PREV_VERSION = 0,
	EVT_PATCH_VERSION = 1,
};

struct S_PROCESS_MSG
{
	S_PROCESS_MSG(){};
	explicit S_PROCESS_MSG(E_PROCESS_TYPE const Type) : kType(Type){};

	E_PROCESS_TYPE	kType;
	std::wstring kTGTDir;
	std::wstring kCRTDir;
	std::wstring kOUTDir;
	DWORD dwOption;
};

class IOutPutLoger
{
public:
	virtual void ShowMessage(E_WORK_STATE const State, E_PROCESS_TYPE const Type, std::wstring const& kMessage) = 0;
	virtual void LogMessage(E_PROCESS_TYPE const Type, E_LOG_MSG const LogTypeNo, std::wstring const& kAddonText = L"") = 0;
};

class MainWorker;
class PackTask
	: public PgTask< S_PROCESS_MSG >
{
	typedef std::wstring const& RCWSTR;
public:
	virtual	void HandleMessage(MSG *rkMsg);
	static void CALLBACK OnRegist(CEL::SRegistResult const& rkArg){};

	void SetPrevVersionInfo(BM::VersionInfo const& VInfo){ m_PrevVersionInfo = VInfo; };
	void SetPatchVersionInfo(BM::VersionInfo const& VInfo){ m_PatchVersionInfo = VInfo; };
	bool SetMainWorker(IOutPutLoger* pkLoger);

	PackTask(void);
	virtual ~PackTask(void);

protected:
	bool StartDiff(RCWSTR kDestDir, RCWSTR kSourDir, RCWSTR kOutDir, DWORD dwOption);
	bool StartPack(RCWSTR kTgtDir, RCWSTR kOutDir);
	bool StartCreateList(RCWSTR kTgtDir, RCWSTR kOutDir, DWORD dwOption);
	bool StartHeaderMerge(RCWSTR kNewFileDir, RCWSTR kOldFileDir, RCWSTR kOutDir);
	bool StartBind(RCWSTR kTgtDir, RCWSTR kOutDir);
	bool StartUnPack(RCWSTR kDatFileDir, RCWSTR kOutDir);
	bool StartMakeINB(RCWSTR kFileDir, RCWSTR kOutDir);
	bool StartExportList(RCWSTR kDatFileDir, RCWSTR kOutDir);
	bool CreatePatchID(RCWSTR kOutFileDir, std::wstring& kAddonText);
	bool UpdateDATVersionInfo(RCWSTR kOutFileDir, std::wstring& kAddonText);
	bool ConvertDatFile(RCWSTR kFile);
	bool CheckDATVersion(RCWSTR kFile, std::wstring& kAddonText);
	bool MakeManualPatch(RCWSTR kDestDir, RCWSTR kOutDir);
	bool MakeAutoPatch(RCWSTR kDestDir, RCWSTR kDatFileDir, RCWSTR kOutDir, std::wstring& kAddonText);

	bool RecursiveDiff(RCWSTR kDestDir, RCWSTR kSourDir, RCWSTR kOutDir, RCWSTR kSubDir = std::wstring(L"\\"));
	bool RecursivePack(RCWSTR kTgtDir, RCWSTR kOutDir, RCWSTR kSubDir = std::wstring(L"\\"), bool const bIsPack = false);
	bool RecursiveCreateList(RCWSTR kTgtDir, RCWSTR kOutDir, RCWSTR kSubDir = std::wstring(L"\\"), bool const bIsPack = false);
	bool RecursiveCreateListMerge(RCWSTR kTgtDir, RCWSTR kFileName);

	void GetPatchID(E_VERSION_TYPE eVersionType, std::wstring& rkAddonText);

protected:
	BM::VersionInfo m_PatchVersionInfo;
	BM::VersionInfo m_PrevVersionInfo;
	IOutPutLoger*	m_pkLoger;
};