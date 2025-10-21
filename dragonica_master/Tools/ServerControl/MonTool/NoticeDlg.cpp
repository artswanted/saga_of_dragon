// NoticeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerControl.h"
#include "NoticeDlg.h"
#include "PgNetwork.h"
#include "PgServerStateDoc.h"
#include "GeneralDlg.h"
#include "PgServerCmdMgr.h"

// NoticeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(NoticeDlg, CDialog)

NoticeDlg::NoticeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NoticeDlg::IDD, pParent)
{

}

NoticeDlg::~NoticeDlg()
{
}

BOOL NoticeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int iCount = g_kGameServerMgr.GetServerCount(g_kMainDlg.GetSiteNo(), CEL::ST_CONTENTS);
	InitRealm(iCount);
	m_kSI.nServerType = CEL::ST_CONTENTS;
	return TRUE;
}

void NoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_REALM, m_kCtrlRealm);
	DDX_Control(pDX, IDC_COMBO_CHANNEL, m_kCtrlChannel);
	DDX_Control(pDX, IDC_EDIT1, m_kEditContent);
}

void NoticeDlg::InitRealm(int const iCount)
{
	m_kCtrlRealm.Clear();
	m_kCtrlRealm.InsertString( 0, L"ALL" );
	for( int i=1; i<iCount; ++i)
	{
		BM::vstring kName(i);
		m_kCtrlRealm.AddString(kName.operator const wchar_t *());
	}
	m_kCtrlRealm.SetCurSel(0);
	m_kCtrlRealm.Invalidate();
}

void NoticeDlg::InitChannel(int const iCount)
{
	m_kCtrlChannel.Clear();
	m_kCtrlChannel.InsertString( 0, L"ALL" );
	for( int i=1; i<iCount; ++i)
	{
		BM::vstring kName(i);
		m_kCtrlChannel.InsertString( i, kName.operator const wchar_t *() );
	}
	m_kCtrlChannel.SetCurSel(0);
	m_kCtrlChannel.Invalidate();
}

BEGIN_MESSAGE_MAP(NoticeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &NoticeDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_REALM, &NoticeDlg::OnCbnSelchangeComboRealm)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL, &NoticeDlg::OnCbnSelchangeComboChannel)
	ON_BN_CLICKED(IDSEND, &NoticeDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// NoticeDlg 메시지 처리기입니다.

void NoticeDlg::OnBnClickedOk()
{
	OnOK();
}

void NoticeDlg::OnCbnSelchangeComboRealm()
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
}

void NoticeDlg::OnCbnSelchangeComboChannel()
{
	short nRealmNo = static_cast<short>(m_kCtrlRealm.GetCurSel());
	short nChannel = static_cast<short>(m_kCtrlChannel.GetCurSel());

	m_kSI.nServerType = CEL::ST_CENTER;
	m_kSI.nRealm = nRealmNo;
	m_kSI.nChannel = nChannel;
	CONT_SERVER_STATE_EX::mapped_type kServerInfo;
	if( !g_kGameServerMgr.GetServerInfo(g_kMainDlg.GetSiteNo(), m_kSI, kServerInfo) )
	{
		return;
	}
}

void NoticeDlg::OnBnClickedSend()
{
	CString kContent;
	m_kEditContent.GetWindowTextW(kContent);
	m_kEditContent.SetWindowTextW(_T(""));
	std::wstring wstrContent = kContent;
	short nRealm = static_cast<short>(m_kCtrlRealm.GetCurSel());
	short nChannel = static_cast<short>(m_kCtrlChannel.GetCurSel());
	if( 0 == nChannel )
	{//! -1이 모든 채널이다.
		nChannel = -1;
	}

	g_kServerCmdMgr.OnReqNotice(nRealm, nChannel, wstrContent);
}
