// TextCheckerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CTextCheckerDlg 대화 상자
class CTextCheckerDlg : public CDialog
{
// 생성입니다.
public:
	CTextCheckerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TEXTCHECKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSetXml1();	
	afx_msg void OnBnClickedButtonSetXml2();
	afx_msg void OnBnClickedButtonSetfolder();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedRadioErrchk();
	afx_msg void OnBnClickedRadioCompare();

private:
	CEdit m_EdtXML1;
	CEdit m_EdtXML2;

	CEdit m_EdtFolder;
	CEdit m_EdtResultFile;

	CButton m_SetXmlBtn2;
	CButton m_kBtnRun;

	bool m_bErrChkMode;	
	PgTextChecker m_kTxtChkMain;
};
