#pragma once
#include "afxwin.h"


// ChangeMaxUserDlg 대화 상자입니다.

class ChangeMaxUserDlg : public CDialog
{
	DECLARE_DYNAMIC(ChangeMaxUserDlg)

public:
	ChangeMaxUserDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ChangeMaxUserDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHANGE_MAX_USER_DLG };

	virtual BOOL OnInitDialog();
	void InitRealm(int const iCount);
	void InitChannel(int const iCount);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	unsigned int m_uiMaxUser;
	BM::vstring m_kStrBeforeMaxUser;
	SERVER_IDENTITY m_kSI;
	
	CEdit m_kMaxUserEdit;
	CStatic m_kStaticBeforeMaxUser;
	CComboBox m_kCtrlRealm;
	CComboBox m_kCtrlChannel;
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboRealm();
	afx_msg void OnCbnSelchangeComboChannel();
};
