// ServerOnDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerControl.h"
#include "ServerOnDlg.h"


// ServerOnDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ServerOnDlg, CDialog)

ServerOnDlg::ServerOnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ServerOnDlg::IDD, pParent)
{

}

ServerOnDlg::~ServerOnDlg()
{
}

void ServerOnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_ITEM, m_kServerList);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDC_CHECK2, m_kCheckTerminate);

	CONT_SERVER_STRING::iterator itr = m_kList.begin();
	for( ; itr != m_kList.end() ; ++itr )
	{
		m_kServerList.AddString( itr->operator wchar_t const*() );
	}

	 m_kCheckTerminate.EnableWindow(m_bShowCheckBtn); 
	 m_kCheckTerminate.ShowWindow(m_bShowCheckBtn);
	m_Text.SetWindowText(m_kNtfText.c_str());
	this->SetWindowText(m_kWinText.c_str());
}


BEGIN_MESSAGE_MAP(ServerOnDlg, CDialog)
	ON_BN_CLICKED(IDOK, &ServerOnDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ServerOnDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void ServerOnDlg::SetLavelText(std::wstring const& kText)
{
	m_kNtfText = kText;
}

void ServerOnDlg::SetText(std::wstring const& kText)
{
	m_kWinText = kText;
}

void ServerOnDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

void ServerOnDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void ServerOnDlg::SetCmdType( EMMC_CMD_TYPE eCmdType )
{
	switch( eCmdType )
	{
	case MCT_SERVER_ON:
		{
			SetText(_T("Server On warning!!"));
			SetLavelText(_T("Game Server On ?"));
		}break;
	case MCT_SERVER_OFF:
		{
			SetText(_T("Server Off warning!!"));
			SetLavelText(_T("Game Server Off ?"));
		}break;
	case MCT_SERVER_SHUTDOWN:
		{
			SetText(_T("Process Kill warning!!"));
			SetLavelText(_T("Game Server Kill ?"));
		}break;
	}
}