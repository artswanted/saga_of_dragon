#pragma once

namespace QueryUtil
{
	typedef ThreadUtil::CResultObserverMgr< EDBQueryType > CObserverMgr;
	typedef CObserverMgr::CObserver CObserver;
	extern CObserverMgr kObserverMgr;
};

class CTBLoader;
class CListControl;
class CQCCore
{
public:
	typedef enum
	{
		eWork_Prepare,
		eWork_Work,
		eWork_End,
	}EWORK_STATE;	

	CQCCore(void);
	virtual ~CQCCore(void);

	bool	Load();
	void	NotifyMsg(LPARAM lParam);
	bool	WMCommand(WPARAM wParam, LPARAM lParam);
	bool	AddListItem(std::wstring const& wstrItem, EOUTTYPE OutType = OT_SUCCESS, DWORD dwFlag = CHK_LOG);
	bool	TTVWMCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
	bool	TTVWMInit(const HWND hDlg);
	static bool	CreateList(CListControl*& pList, HWND hWnd, DWORD dwID, std::wstring const& ColumnType);
	void	SetStateText(std::wstring const& wstrText)
	{
		SetWindowText(GetDlgItem(g_hDlgWnd, IDC_STATUS), wstrText.c_str());
	}

	std::wstring GetTargetFolder() { return m_TargetFolder; }

	bool		CheckerInit();
	bool		Reload();
	void		SetRegist( const bool bRegist )	{ m_bRegist = bRegist; }
	const bool	GetRegist()						{ return m_bRegist; }
	void		Start();
	void		End();	
	int			Sort( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );
	void		Export();
	void		Clear();
	void		SetWork( const bool bWork )		{ m_bWork = bWork; }
	bool		GetWork()						{ return m_bWork; }

private:
	bool	FolderSelect(const HWND& hWnd, std::wstring& wstrFolderName);
	bool	SetFolder();

	//bool	IsNewDump();// 새로운 덤프 생성할것인가?( true: DB 연결후, Dump생성 false: 기존 Dump Load )

private:
	std::wstring	m_TargetFolder;
	std::wstring	m_NotePadDir;

	bool			m_bRegist;
	bool			m_bWork;

	CLASS_DECLARATION_S(EWORK_STATE, State);

	//	메인 다이얼 로그
	CListControl*	m_pkQuestList;
	CListControl*	m_pkQFailedList;
	CListControl*	m_pkLog;
	CListControl*	m_pkLogError;
	CListControl*	m_pkDBNotExist;
	CListControl*	m_pkEmptyList;
	HWND			m_kTextView;

	//	텍스트 뷰어 다이얼 로그
	CListControl*	m_pkTextList;
};
