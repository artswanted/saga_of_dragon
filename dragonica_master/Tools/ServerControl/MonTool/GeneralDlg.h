#pragma once

#include <string>
#include <map>
#include "afxwin.h"
#include "afxcmn.h"
#include "PgServerStateDoc.h"
#include "ServerControlDlg.h"
#include "afxdtctl.h"
#include "PgLogCopyMgr.h"

typedef struct tagTreeData
{//DWORD_PTR 대신 넣는거라 꼭 DWORD_PTR 크기여야함
	tagTreeData(){}
	tagTreeData(short const nInPriKey, short const nInSecKey)
	{
		nPriKey = nInPriKey;
		nSecKey = nInSecKey;
	}

	tagTreeData(int const iData)
	{
		memcpy(this, &iData, sizeof(int));
	}

	bool operator < (tagTreeData const &rhs) const
	{
		if( nPriKey < rhs.nPriKey )	{return true;}
		if( nPriKey > rhs.nPriKey )	{return false;}
		
		if( nSecKey < rhs.nSecKey )	{return true;}
		if( nSecKey > rhs.nSecKey )	{return false;}
		return false;
	}

	operator DWORD(){return *(DWORD*)(this);}
	int nPriKey:16;
	int nSecKey:16;
}STreeData;	

typedef struct tagServerStateInfo
{
	tagServerStateInfo(){Reset();}
	~tagServerStateInfo(){}
	
	void Reset() { m_iAll = m_iOpen = m_iClose = m_iAlert = 0; }
	void AddInfo(CONT_SERVER_STATE_FOR_VIEW::mapped_type const &rkElement)
	{
		if(rkElement.bReadyToService && rkElement.kProcessInfo.IsCorrect())
			++m_iOpen;
		if(!rkElement.bReadyToService && !rkElement.kProcessInfo.IsCorrect())
			++m_iClose;
		if(rkElement.bReadyToService != rkElement.kProcessInfo.IsCorrect())
			++m_iAlert;
		++m_iAll;
	}

	int m_iAll;
	int m_iOpen;
	int m_iClose;
	int m_iAlert;
}SServerStateInfo;

typedef enum eDisPlayType
{
	DT_ALL = 0,
	DT_OPEN = 1,
	DT_CLOSE = 2,
	DT_ALERT = 3,
}EDisplayType;

typedef enum eDisplayMode
{
	DM_SERVER_STATE = 0,
	DM_LOG_INFO = 1,
	DM_CMD_HISTORY = 2
}EDisplayMode;

typedef enum eFileType
{
	FT_DUMP = -2,
	FT_CATION_HACK = -1,
}EFileType;

typedef enum eTreeType
{
	TT_CONSENT_ROOT = -3,
	TT_REALM_ROOT = -1,
	TT_SMC_ROOT = -2,
	TT_SITE_ROOT = 0,
}ETreeType;

typedef enum eSortType
{
	ST_CHECK = 0,
	ST_SERVER_NAME,
	ST_SERVER_TYPE,
	ST_MACHINIP,
	ST_SERVER_NO,
	ST_CHANNEL,
	ST_CONNECT,
	ST_PROCESS,
	ST_IP,
	ST_PORT
}ESortType;

class CGeneralDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeneralDlg)
public:
	CGeneralDlg(CWnd* pParent = NULL);   
	virtual ~CGeneralDlg();

	enum { IDD = IDD_GENERALDLG };

	virtual BOOL OnInitDialog();
	void Close();
	void Clear();

	static int CALLBACK CompareServerList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
	void InvalidateTreeView(EDisplayMode eMode = DM_SERVER_STATE);
	void InvalidateListView(bool bRecvData = false, EDisplayMode eMode = DM_SERVER_STATE);
	void CallLoginDlg();
	void SetControlBtn();

	void SetTitleText(std::wstring wstrPatchVer);
	int GetSiteNo() { return m_iSiteNo; }

	void InitDownloadInfo(size_t usTotalCount, double dTotalsize);
	void UpdateDownloadInfo(std::wstring wstrFileName, double dCurSize);
	void EndDownload();
	void StepItProcess() { m_kProgressJob.StepIt(); }
	void StartProcess() { m_kProgressJob.SetPos(1); }
	void UpdateProcessText(BM::vstring const &rkMessage) { m_kStaticProcess.SetWindowTextW(rkMessage.operator const wchar_t *()); }
	
	afx_msg HCURSOR CGeneralDlg::OnQueryDragIcon();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnColumnclickServerList  (NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnServerOn();
	afx_msg void OnBnClickedBtnServerOff();
	afx_msg void OnBnClickedBtnProcesskill();
	afx_msg void OnBnClickedCheckUsebeep();
	afx_msg void OnBnClickedBtnLogClear();
	afx_msg void OnBnClickedConfirmKill();
	afx_msg void OnFileExit();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTvnSelchangedTreeview(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnMaxUserChange();
	afx_msg void OnBnClickedBtnResync();
	afx_msg void OnBnClickedConfirmResync();
	afx_msg void OnBnClickedDisp();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnDownload();
	afx_msg void OnBnClickedBtnExpand();
	afx_msg void OnBnClickedBtnFold();
	afx_msg void OnBnClickedBtnSelectAll();
	afx_msg void OnBnClickedBtnSelectRoot();
	afx_msg void OnBnClickedBtnSelectRealm();
	afx_msg void OnBnClickedBtnNotice();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

private:
	bool IsDisplay( CONT_SERVER_STATE_FOR_VIEW::mapped_type const &kElement, EDisplayType const kDisplayType )const;

	void SetCheckListCtrl(bool bCheck);
	void SetColumnListCtrl(EDisplayMode eMode = DM_SERVER_STATE);

	bool GetTreeData(STreeData& kTreeData, EDisplayMode eMode = DM_SERVER_STATE);

	void GetCheckedList(ContServerID& rkOutServerID, CONT_SERVER_STRING& rkOutText)const;
	bool GetCheckedList(int iKeyValue, CONT_SERVER_STRING& rkOutText, SLogCopyer::CONT_LOG_INFO& rkContLogInfo);
	bool GetCheckedList(int iKeyValue, CONT_SERVER_STRING& rkOutText, SLogCopyer::CONT_DUMP_INFO& rkContDumpInfo);

	bool ServerRefresh(CONT_SERVER_STATE_FOR_VIEW const & rkContServerState);
	bool ServerRefresh(SLogCopyer::CONT_LOG_INFO const & rkContServerState, int iKeyValue);
	bool ServerRefresh(SLogCopyer::CONT_LIST_DUMP_INFO& rkContDumpInfo);

	void InvalidateServerStateToListView(STreeData& rkTreeData);
	void InvalidateServerStateToTreeView();

	void InvalidateLogInfoToListView(STreeData& rkTreeData);
	void InvalidateLogInfoToTreeView();

public:
	std::wstring m_wstrID;
	std::wstring m_wstrPW;

protected:
	mutable Loki::Mutex	m_kMutex;

	CServerControlDlg m_kLoginDlg;
	SServerStateInfo m_kServerStateInfo;
	CONT_SERVER_STATE_FOR_VIEW m_kContServerState;

	int m_iSelectedListColum;
	int	m_iSiteNo;
	int m_iSelectTreeItem;
	size_t m_iCurFileCount;
	size_t m_iTotalFileCount;
	double m_dCurFileSize;
	double m_dTotalFileSize;
	bool m_bIsDownload;
	std::wstring m_wstrTotalSize;

	//---------------------------------
	//! 이하  MFC 컨트롤 멤버
	//--------------------------------

	// control wnd container
	CONT_WND m_kContLogWnd;
	CONT_WND m_kContStateWnd;
	CONT_WND m_kContProcessWnd;

	CTabCtrl m_kTabCtrl;// tab 
	CListCtrl m_kListCtrl;// list
	CTreeCtrl m_kTreeCtrl;// tree
	CImageList m_kImageList;// image
	CMenu m_kMenu;// menu

	// tree item
	HTREEITEM m_hStateSelectTreeItem;
	HTREEITEM m_hStateLastSelectTreeItem;

	HTREEITEM m_hLogSelectTreeItem;
	HTREEITEM m_hLogLastSelectTreeItem;

	HICON m_hIcon;
	CString m_kStrCCU;

	//! Terminate Group
	CStatic m_kGroupTerminate;
	CButton m_kCheckBtnKillServer;
	CButton m_kBtnKillServer;

	//! ReSync Group
	CStatic m_kGroupReSync;
	CButton m_kCheckBtnReSync;
	CButton m_kBtnReSync;

	//! DisplayType Group
	CStatic m_kGroupDisplayType;
	CButton	m_kRadioBtn1;
	CButton	m_kRadioBtn2;
	CButton	m_kRadioBtn3;
	CButton	m_kRadioBtn4;

	//! MaxUser Group
	CStatic m_kGroupMaxUser;
	CButton m_kBtnChangeMaxUser;

	//! Download Group
	CStatic m_kGroupDownload;
	CProgressCtrl m_kProgressDownload;
	CButton m_kBtnDownload;
	CStatic m_kStrFileName;
	CStatic m_kStrFileSize;

	//! Server On/Off Group
	CStatic m_kGroupServerSwitch;
	CButton m_kBtnServerOn;
	CButton m_kBtnServerOff;
	CStatic m_kStrFileCount;

	//! Process Group
	CStatic m_kGroupProcess;
	CStatic m_kStaticProcess;
	CProgressCtrl m_kProgressJob;
	
};

#define g_kMainDlg SINGLETON_STATIC(CGeneralDlg)