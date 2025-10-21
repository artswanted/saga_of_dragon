// MakeCRCDlg.h : header file
//

#pragma once

#include "stdafx.h"


// CMakeCRCDlg dialog
class CMakeCRCDlg : public CDialog
{
// Construction
public:
	CMakeCRCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MAKECRC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConvToPatcher();
	afx_msg void OnBnClickedBtnConvToPatchPatcher();

private:
	bool MakeExeCRC(const std::wstring& FileName, const std::wstring& ResultFileName);

private:
	std::vector<char> m_vecCRC;
};
