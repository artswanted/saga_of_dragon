#pragma once
#include "afxwin.h"


// CProjectNewFolder 대화 상자입니다.

class CProjectNewFolder : public CDialog
{
	DECLARE_DYNAMIC(CProjectNewFolder)

public:
	CProjectNewFolder(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CProjectNewFolder();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NEW_DIRECTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolderName;
	CButton m_btnOk;
};
