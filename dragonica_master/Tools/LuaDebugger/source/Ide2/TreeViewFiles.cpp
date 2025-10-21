// TreeViewFiles.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "TreeViewFiles.h"

#include "MainFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles

IMPLEMENT_DYNCREATE(CTreeViewFiles, CTreeView)

CTreeViewFiles::CTreeViewFiles()
{
	m_hRoot = 0;
}

CTreeViewFiles::~CTreeViewFiles()
{
}


BEGIN_MESSAGE_MAP(CTreeViewFiles, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewFiles)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_PROJECT_ADD_FILES, OnProjectAddFiles)
	ON_COMMAND(ID_PROJECT_ADD_FOLDER, OnAddFolder) // 다른 함수
	ON_COMMAND(ID_PROJECT_PROPERTIES, OnProjectProperties)
	ON_COMMAND(ID_FOLDER_ADDFILESTOFOLDER, OnFolderAddFiles)
	ON_COMMAND(ID_FOLDER_ADDFOLDERTOFOLDER, OnAddFolder) // 다른 함수
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles drawing

void CTreeViewFiles::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles diagnostics

#ifdef _DEBUG
void CTreeViewFiles::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeViewFiles::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles message handlers

int CTreeViewFiles::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTree = &GetTreeCtrl();

	m_images.Create (IDB_IL_FILE, 16, 1, RGB(0,255,0));
	m_pTree->SetImageList (&m_images, TVSIL_NORMAL);

	return 0;
}

void CTreeViewFiles::RemoveAll()
{
	m_pTree->DeleteAllItems();
}

HTREEITEM CTreeViewFiles::AddRoot(CString strProject)
{
	CString strLabel;
	strLabel.Format("Project '%s'", strProject);

	TV_INSERTSTRUCT root;
	root.hParent = NULL;
	root.hInsertAfter = TVI_SORT;
	root.item.iImage = 1;
	root.item.iSelectedImage = 1;
	root.item.pszText = strLabel.GetBuffer(0);
	root.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hRoot = m_pTree->InsertItem(&root);

	//TV_INSERTSTRUCT files;
	//files.hParent = m_hRoot;
	//files.hInsertAfter = TVI_SORT;
	//files.item.iImage = 2;
	//files.item.iSelectedImage = 2;
	//files.item.pszText = "Sourece";
	//files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//m_hFilesFolder = m_pTree->InsertItem(&files);
	//m_hCurrentFolder = m_hFilesFolder;
	//m_listFolder.push_back(m_hFilesFolder);

	//files.hParent = m_hCurrentFolder;
	//files.hInsertAfter = TVI_SORT;
	//files.item.iImage = 2;
	//files.item.iSelectedImage = 2;
	//files.item.pszText = "qwer";
	//files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//m_hFilesFolder = m_pTree->InsertItem(&files);
	//m_listFolder.push_back(m_hFilesFolder);

	//files.hParent = m_hCurrentFolder;
	//files.hInsertAfter = TVI_SORT;
	//files.item.iImage = 2;
	//files.item.iSelectedImage = 2;
	//files.item.pszText = "asdf";
	//files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//m_hFilesFolder = m_pTree->InsertItem(&files);
	//m_hCurrentFolder = m_hFilesFolder;
	//m_listFolder.push_back(m_hFilesFolder);


	//files.hParent = m_hRoot;
	//files.hInsertAfter = TVI_SORT;
	//files.item.iImage = 2;
	//files.item.iSelectedImage = 2;
	//files.item.pszText = "zxcv";
	//files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//m_hFilesFolder = m_pTree->InsertItem(&files);
	//m_hCurrentFolder = m_hFilesFolder;
	//m_listFolder.push_back(m_hFilesFolder);

	return m_hRoot;

	//TV_INSERTSTRUCT files2;
	//files2.hParent = m_hFilesFolder;
	//files2.hInsertAfter = TVI_FIRST;
	//files2.item.iImage = 2;
	//files2.item.iSelectedImage = 2;
	//files2.item.pszText = "kkk";
	//files2.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//HTREEITEM a = m_pTree->InsertItem(&files2);
	//m_listFolder.push_back(a);

	//files2.hParent = m_hFilesFolder;
	//files2.hInsertAfter = TVI_FIRST;
	//files2.item.iImage = 2;
	//files2.item.iSelectedImage = 2;
	//files2.item.pszText = "zzz";
	//files2.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	//a = m_pTree->InsertItem(&files2);
	//m_listFolder.push_back(a);
}

HTREEITEM CTreeViewFiles::AddProjectFile(CString strName, long lParam)
{
	TV_INSERTSTRUCT file;
	file.hParent = m_hCurrentFolder;
	file.hInsertAfter = TVI_SORT;
	file.item.iImage = 4;
	file.item.iSelectedImage = 4;
	file.item.pszText = strName.GetBuffer(0);
	file.item.lParam = lParam;
	file.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	
	return m_pTree->InsertItem(&file);
}

// 인자 2개 필요.
// 지금 파일의 부모, 폴더인지 파일인지.
HTREEITEM CTreeViewFiles::AddProjectFile(
	CString strName, 
	long lParam,
	HTREEITEM kParent,
	CProjectFile *pkProjectFile
	)
{
	TV_INSERTSTRUCT file;
	file.hParent = kParent;
	if (pkProjectFile->m_eFileType == CProjectFile::FileType_File)
	{
		file.hInsertAfter = TVI_LAST;
		file.item.iImage = 4;
		file.item.iSelectedImage = 4;
	}
	else if (pkProjectFile->m_eFileType == CProjectFile::FileType_Folder)
	{
		file.hInsertAfter = TVI_FIRST;
		file.item.iImage = 2;
		file.item.iSelectedImage = 2;
	}

	file.item.pszText = strName.GetBuffer(0);
	file.item.lParam = lParam;
	file.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	HTREEITEM kItem = m_pTree->InsertItem(&file);

	if (pkProjectFile->m_eFileType == CProjectFile::FileType_Folder)
		m_listFolder.push_back(kItem);

	return kItem;
}

BOOL CTreeViewFiles::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	
	return CTreeView::PreCreateWindow(cs);
}

void CTreeViewFiles::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ( pNMTreeView->itemNew.hItem == m_hFilesFolder )
	{
		if ( pNMTreeView->action==TVE_EXPAND )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 3, 3);
		else if ( pNMTreeView->action==TVE_COLLAPSE )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 2, 2);
	}

	*pResult = 0;
}


void CTreeViewFiles::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM item = m_pTree->HitTest(pt, &nFlags);
	
	HMENU hMenu = NULL;
	if ( item == m_hRoot )
	{
		hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_PROJECT_MENU));
	}
	else
	{
		std::list<HTREEITEM>::iterator itr;
		for (itr = m_listFolder.begin() ;
			itr != m_listFolder.end() ;
			++itr)
		{
			HTREEITEM kSrc = *itr;
			if (item == kSrc)	
			{
				hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_FOLDER_MENU));
				break;
			}
		}
	}

	if ( !hMenu )
		return;

	m_hCurrentFolder = item;

	HMENU hSubMenu = GetSubMenu(hMenu, 0);
	if (!hSubMenu) 
	{
		DestroyMenu(hMenu);
		return;
	}

	POINT mouse;
	GetCursorPos(&mouse);
	::SetForegroundWindow(m_hWnd);	
	::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, m_hWnd, NULL);

	DestroyMenu(hMenu);
	
	*pResult = 0;
}

//void CTreeViewFiles::OnProjectAddFiles() 
//{
	//CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	//pProject->AddFiles();
//}

void CTreeViewFiles::OnProjectProperties() 
{
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->Properties();
}

void CTreeViewFiles::OnAddFolder()
{
	// 폴더 추가하는 코드.
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->AddFolder();
}

void CTreeViewFiles::OnFolderAddFiles()
{
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->AddFiles();
}

void CTreeViewFiles::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM hItem = m_pTree->HitTest(pt, &nFlags);

	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	if ( pPF )
	{
		if (pPF->m_eFileType == CProjectFile::FileType_File)
		{
			CLuaView* pView = theApp.OpenProjectFilesView(pPF);
			pView->Activate();
		}
	}

	*pResult = 0;
}

void CTreeViewFiles::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	if ( hItem && pTVKeyDown->wVKey == VK_DELETE )
	{
		CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
		if ( pPF )
		{
			m_pTree->DeleteItem(hItem);
			CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
			pProject->RemoveFile(pPF);
		}
	}

	*pResult = 0;
}
