#pragma once
#include "afxwin.h"


// ServerOnDlg 대화 상자입니다.

class ServerOnDlg : public CDialog
{
	DECLARE_DYNAMIC(ServerOnDlg)

public:
	ServerOnDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ServerOnDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER_ON_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetServerList(CONT_SERVER_STRING const& kServerList){m_kList = kServerList;}
	void SetCmdType(EMMC_CMD_TYPE eCmdType);
	
	void ShowTerminateBtn(bool bShow) { m_bShowCheckBtn = bShow; }
	bool IsTerminateMode() { return (BST_CHECKED == m_kCheckTerminate.GetCheck()); }

	afx_msg void OnLbnSelchangeServerItem();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	void SetLavelText(std::wstring const& kText);
	void SetText(std::wstring const& kText);

private:
	ContServerID m_kSelectServerList;
	bool m_bShowCheckBtn;

protected:
	CONT_SERVER_STRING m_kList;
	std::wstring m_kWinText;
	std::wstring m_kNtfText;

	CListBox m_kServerList;
	CStatic m_Text;
	CButton m_kCheckTerminate;
};
