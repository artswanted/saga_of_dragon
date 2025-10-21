#pragma once

#include "resource.h"

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include "BM/BM.h"
#include "DataPack/PgDataPackManager.h"
#include "BM/FileSupport.h"
#include "SimpEnc/SimpEnc.h"
#include "HBitmap.h"
#include "CTButton.h"
#include "CTListBox.h"

#if defined(_MTd_)
	#pragma	comment	(lib, "DataPack_MTd.lib")
	#pragma	comment	(lib, "SimpEnc_MTd.lib")
#elif defined(_MT_)
	#pragma	comment	(lib, "DataPack_MT.lib")
	#pragma	comment	(lib, "SimpEnc_MT.lib")
#elif defined(_MTo_)
	#pragma	comment	(lib, "DataPack_MTo.lib")
	#pragma	comment	(lib, "SimpEnc_MTo.lib")
#endif


typedef	enum eRequestTarget
{
	ERT_DOWNLOAD,
	ERT_PACK,
}EREQUESTTARGET;

typedef	enum eBitmapType
{
	HBT_DLG_NORMAL,
	HBT_DLG_CHS,
	HBT_PROGRESS1,
	HBT_PROGRESS2,
	HBT_END,
}EBitmapType;

typedef	enum	eProcess_State
{
	eProcess_Preparing,
	eProcess_Connecting,
	eProcess_Comparing,
	eProcess_NP_Patching,
	eProcess_P_Patching,
}EProcess_State;

typedef	std::map< UINT, CHBitmap >	kImageCont;
typedef	std::map< UINT, CCTButton >	kButtonCont;

class CPatch
{
public:
	CPatch();
	~CPatch();

public:
	kImageCont		m_kImageContainer;
	kButtonCont		m_kButtonContainer;

	CCTListBox		m_ListBoxChangeLog;

	HANDLE			m_PackThread;

	float			m_fTotalPercent;
	float			m_fFilePercent;
	size_t			m_TotalSize;
	size_t			m_FileSize;
	size_t			m_CntFileSize;
	size_t			m_CntTotalSize;

	std::wstring	m_wstrTextState;

	WCHAR m_szImgName[MAX_PATH];
	WCHAR m_szOldVersion[MAX_PATH];
	WCHAR m_szNewVersion[MAX_PATH];
	BM::VersionInfo m_kNewVersion;
	PIMAGE_NT_HEADERS m_pNtHdrs;
	DWORD m_dwOffset;
	DWORD m_dwRead;
	BYTE m_bylen2;
	BYTE m_bylen3;
	
	std::wstring m_kClientVer;

	Loki::Mutex m_kMutex;
	bool m_bThreadRun;
	bool m_bThreadEnd;
	bool m_bFinish;
	bool m_bFinishResult;

	std::wstring m_strDeleteFileName;

public:
	bool	IsVersionCompare(std::wstring const& strClient, std::wstring const& strOldVersion);
	std::wstring GetPatchID(std::wstring const& strFileName);
	void	MakePatchID(std::string strPatchID);
	bool	FolderSelect(const HWND& hWnd, TCHAR* wstrFolderName, const int& Length = MAX_PATH);
	void	PackFolderInit();
	bool	PackPatch(char* szData, DWORD dwSize);
	bool	DividePatchList(BM::FolderHash& kResultPackList, BM::FolderHash& kResultNonPackList,
							const BM::FolderHash& kPackList, const std::vector<std::wstring>& kPackFolderList);
	bool	AmendNonPackUpdate(const BM::FolderHash& PatchList, char* szData);
	bool	AmendPackUpdate(const BM::FolderHash& List, char* szData);
	bool	GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList);
	size_t	GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value = ERT_DOWNLOAD);
	bool	DecFile(std::vector<char>& vecData, BM::PgFileInfo kFileInfo);
	void	UnCompFile(std::vector<char>& vecData, size_t const& OriginalSize);
	bool	AmendPatchToOrgMerge(const std::wstring& TargetDir, const BM::FolderHash& kPatchHash, char* szData);
	bool	MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList);


	void Init();
	bool BitmapLoad(UINT Type, std::vector<char> const& kData);
	bool BitmapLoad(UINT Type, HINSTANCE hInst, UINT uiID, int iWidth, int iHeight);
	bool Process();
	bool VersionRead();
	void InitVriable();
	bool SetWindowFrame();
	bool ButtonInit(int BtnID, int iX, int iY, std::wstring const& wstrText, std::vector<char> const& kData);
	bool ButtonInit(int BtnID, int iX, int iY, int iWidth, int iHeight, std::wstring const& wstrText, HINSTANCE hInst, UINT uiID);
	bool ListBoxInit(int iListBoxID, int iX, int iY, int iWidth, int iHeight);

	bool CreatePatchThread();
	void OwnerDraw(LPDRAWITEMSTRUCT lpDIS);
	void Paint(HDC hDC);

	void SetWorkState(const std::wstring& OutText);
	void ReadChangeLog();
	void Command(WPARAM wParam, LPARAM lParam);
	bool SetFolder();
	void PatchCancel();
	void CheckRun();
	bool ClientVersionCheck();
	void CheckFinish();
	bool FindProcess(std::wstring strExeName);

	bool UpdatePackVer(BM::FolderHash& kAllPackList);
	__int64 ReadPatchVer(std::wstring strVersion);

	void	SetTotalPer(float Per)	{ m_fTotalPercent = Per; }
	void	SetTotalSize(size_t size)	
	{ 
		m_TotalSize = size; 
		m_CntTotalSize = 0;
	}
	void	SetFileSize(size_t size)	
	{ 
		m_FileSize = size; 
		m_CntFileSize = 0;
	}
	void	SetTotalCnt(size_t size)	
	{ 
		m_CntTotalSize += size; 
		m_fTotalPercent = (float)m_CntTotalSize / m_TotalSize;
	}
	void	SetFileCnt(size_t size)		
	{ 
		m_CntFileSize += size; 
		m_fFilePercent = (float)m_CntFileSize / m_FileSize;
	}
private:
	bool IsAbleToPatch(std::wstring const& kOldVer);
	BM::VersionInfo GetVerInfo(std::wstring const& kVerStr);
};