// PatcherPatchDlg.h : 헤더 파일
//

#pragma once


// CPatcherPatchDlg 대화 상자
class CPatcherPatchDlg : public CDialog
{
// 생성입니다.
public:
	CPatcherPatchDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PATCHERPATCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 사용자 정의 함수
	BOOL RunPatcher();					// 패처 실행
	BOOL FindProcess(CString sExeName);	// 실행파일 이름으로 특정 프로세스 찾기
	BOOL KillProcess(CString sExeName);	// 실행파일 이름으로 특정 프로세스 죽이기

	// 보조 함수
	LPTSTR LoadRCString(UINT wID);		// 해당 리소스의 문자열을 반환한다

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
