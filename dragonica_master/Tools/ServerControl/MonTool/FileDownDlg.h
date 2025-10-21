#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "PgLogCopyMgr.h"


// CFileDownDlg 대화 상자입니다.

class CFileDownDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileDownDlg)

public:
	CFileDownDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileDownDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILE_DOWN_DLG };

public:
	virtual BOOL OnInitDialog();

	void Init(int iKeyValue);
	void SetDownloadList(CONT_SERVER_STRING const& rkDownloadList, SLogCopyer::CONT_LOG_INFO const& rkLogInfo);
	void SetDownloadList(CONT_SERVER_STRING const& rkDownloadList, SLogCopyer::CONT_DUMP_INFO const& rkDumpInfo);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDateSelect();
	afx_msg void OnBnClickedAllFile();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

protected:
	CONT_SERVER_STRING m_kContDownloadList;
	SLogCopyer::CONT_LOG_INFO m_kContLogInfo;
	SLogCopyer::CONT_DUMP_INFO m_kContDumpInfo;

	CONT_WND m_kContDateSelectWnd;

	int	m_iKeyValue;//! 파일 타입을 구분하는 키 값
	bool m_bUseOptionGroup;//! 날짜 옵션 그룹 사용 유무
	bool m_bUseTypeGroup;//! 타입 옵션 그룹 사용 유무

	std::wstring m_wstrTotalInfo;
	std::wstring m_wstrTypeName1;//! 첫번째 체크박스 옵션 타입 이름
	std::wstring m_wstrTypeName2;//! 두번째 체크박스 옵션 타입 이름

	//! MFC Control Member
	CStatic m_kGroupType;
	CButton m_kChkType1;
	CButton m_kChkType2;
	CButton m_kRadioAllFile;
	CDateTimeCtrl m_kCtrlStartTime;
	CDateTimeCtrl m_kCtrlEndTime;
	CListBox m_kListDownload;
	CStatic m_kTextTotalInfo;
	CStatic m_kGroupOption;
	CButton m_kRadioDateSelect;
	CStatic m_kTextPeriod;
};
