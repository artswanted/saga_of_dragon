// PgFindDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ide2.h"
#include "LuaEditor.h"
#include "PgFindDlg.h"


// CPgFindDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPgFindDlg, CDialog)

CPgFindDlg::CPgFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPgFindDlg::IDD, pParent)
	, m_strSearch(_T(""))
	, m_bCorrect(FALSE)
{
	m_pLuaEditor = 0;
}

CPgFindDlg::~CPgFindDlg()
{
}

void CPgFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FIND, m_strSearch);
	DDX_Check(pDX, IDC_SEARCH_CORRECT, m_bCorrect);
}


BEGIN_MESSAGE_MAP(CPgFindDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPgFindDlg::OnBnClickedOk)
END_MESSAGE_MAP()


void CPgFindDlg::SetLuaEditorPtr(CLuaEditor *pLuaEditor)
{
	m_pLuaEditor = pLuaEditor;
}

// CPgFindDlg 메시지 처리기입니다.

void CPgFindDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	char acTemp[256];
	strcpy_s(acTemp, m_strSearch);
	
	if ( !m_pLuaEditor->Find(acTemp, m_bCorrect) )
	{
		MessageBox("지정한 텍스트를 찾을수 없습니다.");
	}
}
