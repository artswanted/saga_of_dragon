#pragma once
#include "afxwin.h"


// NoticeDlg 대화 상자입니다.

class NoticeDlg : public CDialog
{
	DECLARE_DYNAMIC(NoticeDlg)

public:
	NoticeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~NoticeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NOTICE_DLG };

	virtual BOOL OnInitDialog();

	void InitRealm(int const iCount);
	void InitChannel(int const iCount);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	SERVER_IDENTITY m_kSI;

	CComboBox m_kCtrlRealm;
	CComboBox m_kCtrlChannel;
	CEdit m_kEditContent;

	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboRealm();
	afx_msg void OnCbnSelchangeComboChannel();
	afx_msg void OnBnClickedSend();
};
