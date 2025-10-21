#pragma once


class CLuaEditor;

// CPgFindDlg 대화 상자입니다.

class CPgFindDlg : public CDialog
{
	DECLARE_DYNAMIC(CPgFindDlg)

public:
	CPgFindDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPgFindDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_FIND };

public:
	CLuaEditor *m_pLuaEditor;

public:
	void SetLuaEditorPtr(CLuaEditor *pLuaEditor);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSearch;
	afx_msg void OnBnClickedOk();
	BOOL m_bCorrect;
};
