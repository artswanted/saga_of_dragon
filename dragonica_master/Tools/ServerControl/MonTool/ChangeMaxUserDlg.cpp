// hangeMaxUserDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerControl.h"
#include "ChangeMaxUserDlg.h"
#include "PgNetwork.h"
#include "PgServerStateDoc.h"
#include "GeneralDlg.h"
#include "PgServerCmdMgr.h"

// ChangeMaxUserDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ChangeMaxUserDlg, CDialog)

ChangeMaxUserDlg::ChangeMaxUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ChangeMaxUserDlg::IDD, pParent)
{

}

ChangeMaxUserDlg::~ChangeMaxUserDlg()
{
}

void ChangeMaxUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MAX_USER, m_kMaxUserEdit);
	DDX_Control(pDX, IDC_TEXT_BEFORE_COUNT, m_kStaticBeforeMaxUser);
	DDX_Control(pDX, IDC_COMBO_REALM, m_kCtrlRealm);
	DDX_Control(pDX, IDC_COMBO_CHANNEL, m_kCtrlChannel);

	m_kStaticBeforeMaxUser.SetWindowText(m_kStrBeforeMaxUser.operator wchar_t const*());
}

BOOL ChangeMaxUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int iCount = g_kGameServerMgr.GetServerCount(g_kMainDlg.GetSiteNo(), CEL::ST_CONTENTS);
	InitRealm(iCount);
	m_kSI.nServerType = CEL::ST_CONTENTS;
	return TRUE;
}

void ChangeMaxUserDlg::InitRealm(int const iCount)
{
	m_kCtrlRealm.Clear();
	m_kCtrlRealm.InsertString( 0, L"ALL" );
	for( int i=1; i<=iCount; ++i)
	{
		BM::vstring kName(i);
		m_kCtrlRealm.AddString(kName.operator const wchar_t *());
	}
	m_kCtrlRealm.SetCurSel(0);
	m_kCtrlRealm.Invalidate();
}

void ChangeMaxUserDlg::InitChannel(int const iCount)
{
	m_kCtrlChannel.Clear();
	m_kCtrlChannel.InsertString( 0, L"ALL" );
	m_kCtrlChannel.InsertString( 1, L"Public" );
	for( int i=2; i<=iCount+1; ++i)
	{
		BM::vstring kName(i);
		m_kCtrlChannel.InsertString( i, kName.operator const wchar_t *() );
	}
	m_kCtrlChannel.SetCurSel(0);
	m_kCtrlChannel.Invalidate();
}

BEGIN_MESSAGE_MAP(ChangeMaxUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &ChangeMaxUserDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_REALM, &ChangeMaxUserDlg::OnCbnSelchangeComboRealm)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL, &ChangeMaxUserDlg::OnCbnSelchangeComboChannel)
END_MESSAGE_MAP()


// ChangeMaxUserDlg 메시지 처리기입니다.

void ChangeMaxUserDlg::OnBnClickedOk()
{
	CString kStrMaxUser;
	m_kMaxUserEdit.GetWindowText(kStrMaxUser);
	int iMaxUser = _wtoi(kStrMaxUser.GetBuffer());
	int iRealmNo = m_kCtrlRealm.GetCurSel();
	int iChannelNo = m_kCtrlChannel.GetCurSel();
	if( 0 >= iMaxUser  )
	{//잘못된 입력이다
		return;
	}

	g_kServerCmdMgr.OnChangeMaxUser(m_kSI, iMaxUser);
	
	OnOK();
}

void ChangeMaxUserDlg::OnCbnSelchangeComboRealm()
{
	short nRealmNo = static_cast<short>(m_kCtrlRealm.GetCurSel());
	int iCount = 0;
	if( nRealmNo )
	{
		iCount = g_kGameServerMgr.GetServerCount(g_kMainDlg.GetSiteNo(), CEL::ST_CENTER, nRealmNo);
	}

	InitChannel(iCount);

	m_kSI.nServerType = CEL::ST_CONTENTS;
	m_kSI.nRealm = nRealmNo;
	CONT_SERVER_STATE_EX::mapped_type kServerInfo;
	if( !g_kGameServerMgr.GetServerInfo(g_kMainDlg.GetSiteNo(), m_kSI, kServerInfo) )
	{
		return;
	}

	m_kStrBeforeMaxUser = kServerInfo.uiMaxUsers;
	CDialog::UpdateData(false);
}

void ChangeMaxUserDlg::OnCbnSelchangeComboChannel()
{
	short nRealmNo = static_cast<short>(m_kCtrlRealm.GetCurSel());
	short nChannel = static_cast<short>(m_kCtrlChannel.GetCurSel());

	m_kSI.nServerType = CEL::ST_CENTER;
	m_kSI.nRealm = nRealmNo;
	m_kSI.nChannel = nChannel-1;
	CONT_SERVER_STATE_EX::mapped_type kServerInfo;
	if( !g_kGameServerMgr.GetServerInfo(g_kMainDlg.GetSiteNo(), m_kSI, kServerInfo) )
	{
		return;
	}

	m_kStrBeforeMaxUser = kServerInfo.uiMaxUsers;
	CDialog::UpdateData(false);
}
