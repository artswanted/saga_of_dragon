// ProjectNewFolder.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ide2.h"
#include "ProjectNewFolder.h"


// CProjectNewFolder 대화 상자입니다.

IMPLEMENT_DYNAMIC(CProjectNewFolder, CDialog)

CProjectNewFolder::CProjectNewFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectNewFolder::IDD, pParent)
	, m_strFolderName(_T(""))
{

}

CProjectNewFolder::~CProjectNewFolder()
{
}

void CProjectNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEW_DIRECTORY_NAME, m_strFolderName);
	DDV_MaxChars(pDX, m_strFolderName, 20);
	DDX_Control(pDX, IDOK, m_btnOk);
}


BEGIN_MESSAGE_MAP(CProjectNewFolder, CDialog)
END_MESSAGE_MAP()


// CProjectNewFolder 메시지 처리기입니다.
