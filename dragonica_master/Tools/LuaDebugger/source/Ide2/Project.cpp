// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "Project.h"

#include "WorkspaceWnd.h"
#include "TreeViewFiles.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "MainFrame.h"
#include "ProjectProperties.h"
#include "ProjectNewFolder.h"
#include "ScintillaView.h"
#include "Executor.h"
#include "ProjectNew.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProject::CProject()
{
	SetModifiedFlag(FALSE);
}

CProject::~CProject()
{
	RemoveProjectFiles();
}

//--------------------------------------------------------------------------------------------------
//- file and directory functions
//--------------------------------------------------------------------------------------------------

CString CProject::GetName()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname);
}

CString CProject::GetNameExt()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname)+ext;
}

BOOL CProject::CreateIntermediateDir()
{
	DWORD Attrib=GetFileAttributes(m_strIntermediateDirRoot);
	if(Attrib == 0xFFFFFFFF) 
		return CreateDirectory(m_strIntermediateDirRoot, NULL); 
	else if(!(Attrib & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;
 
	return TRUE;
}

BOOL CProject::CreateOutputDir()
{
	DWORD Attrib=GetFileAttributes(m_strOutputDirRoot);
	if(Attrib == 0xFFFFFFFF) 
		return CreateDirectory(m_strOutputDirRoot, NULL);
	else if(!(Attrib & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;
 
	return TRUE;
}

CString CProject::GetProjectDir()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	if ( dir[strlen(dir)-1]=='\\' )
		dir[strlen(dir)-1] = '\0';
	return CString(drive)+dir;	
}

//--------------------------------------------------------------------------------------------------
//- project files functions
//--------------------------------------------------------------------------------------------------

void CProject::RedrawFilesTree()
{
	CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	//pTree->RemoveAll();

	HTREEITEM kRoot = pTree->GetRoot();
	if (!kRoot)
	{
		kRoot = pTree->AddRoot(GetName());
	}
		//pTree->AddRoot(GetName()); // Try Get Root
	m_kPgTreeRoot.kTreeItem = kRoot;
	RedrawFilesTree(&m_kPgTreeRoot, kRoot);

	//int nFiles = m_files.GetSize();
	//for ( int i=0; i<nFiles; ++i )
	//	pTree->AddProjectFile(m_files[i]->GetNameExt(), (long)m_files[i]);
}

void CProject::RedrawFilesTree(CPgTree *pkTree, HTREEITEM kParent)
{
	CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();

	CPgTree kSaveFolderTree;
	CPgTree kSaveFileTree;

	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		//CProjectFile* pkFile = m_files[i];

		// Load 순서 : File, Folder
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_File) // 파일이라면
		{
			kSaveFileTree.m_listChild.push_back(pkChild);
			pkChild->kTreeItem = 
				pTree->AddProjectFile(
					pkChild->pkProjectFile->GetNameExt(),
					(long)pkChild->pkProjectFile,
					kParent,
					pkChild->pkProjectFile);
		}
		else if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder) // 폴더
		{
			kSaveFolderTree.m_listChild.push_back(pkChild);
		}
	}

	// case 1
	//listTree.sort(CPgTree::SortFunction_greater());	// file
	// case 2
	if (!pkTree->m_bIsSorted)
	{
		pkTree->m_bIsSorted = true;
		// sort test
		CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
		CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
		pTree->GetTreeCtrl().SortChildren(kParent);
	}
	


	// 폴더는 후에 first로 넣어준다.
	listTree = kSaveFolderTree.m_listChild;
	listTree.sort(CPgTree::SortFunction_less());	// folder

	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;

		HTREEITEM kThisItem = pTree->AddProjectFile(
			pkChild->pkProjectFile->GetNameExt(),
			(long)pkChild->pkProjectFile,
			kParent,
			pkChild->pkProjectFile);

		pkChild->kTreeItem = kThisItem;
		RedrawFilesTree(pkChild, kThisItem);
	}
}

CProjectFile* CProject::GetProjectFile(CString strPathName)
{
	//int nSize = m_files.GetSize();
	//for ( int i=0; i<nSize; ++i )
	//	if ( m_files[i]->HasFile(strPathName) )
	//		return m_files[i];
	return GetProjectFile(&m_kPgTreeRoot, strPathName);

	//return NULL;
}

CProjectFile* CProject::GetProjectFile(CPgTree *pkTree, CString strPathName)
{
	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			CProjectFile* pkRet;
			if (pkRet = GetProjectFile(pkChild, strPathName))
			{
				return pkRet;
			}
		}

		if ( pkChild->pkProjectFile->HasFile(strPathName) )
		{
			return pkChild->pkProjectFile;
		}
	}

	return 0;
}

void CProject::AddFolder()
{
	CProjectNewFolder dlg;

	if ( dlg.DoModal() == IDOK )
	{
 		CProjectFile* pPF;
		if ( (pPF=GetProjectFile(dlg.m_strFolderName)) != NULL )
			return;
		pPF = new CProjectFile;
		pPF->SetPathName(dlg.m_strFolderName);
		pPF->m_eFileType = CProjectFile::FileType_Folder;

		CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
		CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
		
		CPgTree* pkNewTree = new CPgTree;
		pkNewTree->m_eObjectType = CPgTree::ObjectType_Folder;
		pkNewTree->pkProjectFile = pPF;
		HTREEITEM currentitem = pTree->GetCurrentFolder();
		HTREEITEM item = 
			pTree->AddProjectFile(pPF->GetNameExt(), (long)pPF, currentitem, pPF);
		pkNewTree->kTreeItem = item;

		CPgTree* pkParent = FindTree(currentitem);
		if (pkParent)
			pkParent->m_listChild.push_back(pkNewTree);

		pTree->GetTreeCtrl().SelectItem(item);

		SetModifiedFlag(TRUE);

		//RedrawFilesTree();
	}
}

void CProject::AddFiles()
{
	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT, 
		"Lua files (*.lua)|*.lua|All files (*.*)|*.*||", AfxGetMainWnd());

	if ( fd.DoModal()==IDOK )
	{
		POSITION pos = fd.GetStartPosition();
		while (pos)
		{
			CString strPathName = fd.GetNextPathName(pos);
			AddFile(strPathName);
			//RedrawFilesTree();
		}
	}

}

void CProject::AddFile(CString strPathName)
{
 	CProjectFile* pPF;
	if ( (pPF=GetProjectFile(strPathName)) != NULL )
		return;

	pPF = new CProjectFile;
	pPF->SetPathName(strPathName);

	AddFile(pPF);
}

void CProject::AddFile(CProjectFile* pPF)
{
	//m_files.Add(pPF);

	CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	
	HTREEITEM item = pTree->GetCurrentFolder();
	CPgTree *pkNewTree = new CPgTree;
	pkNewTree->m_eObjectType = CPgTree::ObjectType_File;
	pPF->m_eFileType = CProjectFile::FileType_File;
	pkNewTree->kTreeItem = item;
	pkNewTree->pkProjectFile = pPF;
	pTree->AddProjectFile(pPF->GetNameExt(), (long)pPF, item, pPF);
	//find CPgTree
	CPgTree* pkParent = FindTree(item);
	if (pkParent)
		pkParent->m_listChild.push_back(pkNewTree);

	pTree->GetTreeCtrl().Expand(item,TVE_EXPAND);
	pTree->GetTreeCtrl().SelectItem(item);

	
	//pTree->AddProjectFile(pPF->GetNameExt(), (long)pPF);

	SetModifiedFlag(TRUE);
}

CPgTree* CProject::FindTree(HTREEITEM item)
{
	return FindTree(&m_kPgTreeRoot, item);
}

CPgTree* CProject::FindTree(CPgTree* pkTree, HTREEITEM item)
{
	if (pkTree->kTreeItem == item)
	{
		return pkTree;
	}

	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree* pkChild = *itr;
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			CPgTree *pkRet;
			pkRet = FindTree(pkChild, item);
			if ( pkRet &&
				pkRet->m_eObjectType != CPgTree::ObjectType_None )
			{
				return pkRet;
			}
		}

		if (pkChild->kTreeItem == item)
		{
			return pkChild;
		}
	}

	return 0;
}

void CProject::RemoveFile(CProjectFile *pPF)
{
	//int nSize = m_files.GetSize();
	//for ( int i=0; i<nSize; ++i )
	//	if ( m_files[i] == pPF )
	//	{
	//		m_files.RemoveAt(i);
	//		break;
	//	}
	RemoveFile(&m_kPgTreeRoot, pPF);

	SetModifiedFlag(TRUE);
}

bool CProject::RemoveFile(CPgTree *pkTree, CProjectFile* pPF)
{
	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			if ( RemoveFile(pkChild, pPF) )
			{
				return true;
			}
		}

		if (pkChild->pkProjectFile == pPF)
		{
			// 하위의 항목 모두 지워주게 하기.
			pkTree->m_listChild.remove(pkChild);

			RemoveUnderAll(pkChild);
			delete pkChild->pkProjectFile;
			delete pkChild;

			return true;
		}
	}

	return false;
}

bool CProject::RemoveUnderAll(CPgTree *pkTree)
{
	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;

		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			RemoveUnderAll(pkChild);
		}

		delete pkChild->pkProjectFile;
		delete pkChild;
	}

	return true;
}

void CProject::RemoveProjectFiles()
{
	RemoveProjectFiles(&m_kPgTreeRoot);
	//int nSize = m_files.GetSize();
	//for ( int i=0; i<nSize; ++i )
	//{
	//	CLuaView *pView = theApp.FindProjectFilesView(m_files[i]);
	//	if ( pView )
	//		pView->CloseFrame();
	//}

	//for (int i=0; i<nSize; ++i )
	//	delete m_files[i];

	//m_files.RemoveAll();
}

void CProject::RemoveProjectFiles(CPgTree *pkTree)
{
	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			RemoveProjectFiles(pkChild);
		}

		CLuaView *pView = theApp.FindProjectFilesView(pkChild->pkProjectFile);
		if ( pView )
			pView->CloseFrame();
		
		delete pkChild->pkProjectFile;
	}
}

CPgTree CProject::GetFiles()
{
	CPgTree kTreeFiles;

	GetFile(&m_kPgTreeRoot, &kTreeFiles);

	return kTreeFiles;
}

void CProject::GetFile(CPgTree *pkTree, CPgTree *pkoutTree)
{
	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			GetFile(pkChild, pkoutTree);
		}
		else if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_File)
		{
			pkoutTree->m_listChild.push_back(pkChild);
		}
	}
}

void CProject::Properties()
{
	CProjectProperties dlg;

	dlg.m_bGenerateListing = m_bGenerateListing;
	dlg.m_strIntermediateDir = m_strIntermediateDir;
	dlg.m_strOutputDir = m_strOutputDir;
	dlg.m_strOutputPrefix = m_strOutputPrefix;
	dlg.m_strExecuteDir = m_strExecuteDir;
	dlg.m_strExecute = m_strExecute;

	if ( dlg.DoModal() == IDOK )
	{
		m_bGenerateListing = dlg.m_bGenerateListing;
		m_strIntermediateDir = dlg.m_strIntermediateDir;
		m_strOutputDir = dlg.m_strOutputDir;
		m_strOutputPrefix = dlg.m_strOutputPrefix;
		m_strExecuteDir = dlg.m_strExecuteDir;
		m_strExecute = dlg.m_strExecute;

		UpdateDirectories();
		SetModifiedFlag(TRUE);
	}
}

void CProject::UpdateDirectories()
{
	if ( !PathIsRelative(m_strOutputDir) )
	{
		PathRemoveBackslash(m_strOutputDir.GetBuffer(0));
		m_strOutputDir.ReleaseBuffer();
		m_strOutputDirRoot = m_strOutputDir;
	}
	else
	{
		m_strOutputDirRoot = GetProjectDir();
		PathAppend(m_strOutputDirRoot.GetBuffer(MAX_PATH), m_strOutputDir);
		m_strOutputDirRoot.ReleaseBuffer();
		PathRemoveBackslash(m_strOutputDirRoot.GetBuffer(0));
		m_strOutputDirRoot.ReleaseBuffer();
	}

	if ( !PathIsRelative(m_strIntermediateDir) )
	{
		PathRemoveBackslash(m_strIntermediateDir.GetBuffer(0));
		m_strIntermediateDir.ReleaseBuffer();
		m_strIntermediateDirRoot = m_strIntermediateDir;
	}
	else
	{
		m_strIntermediateDirRoot = GetProjectDir();
		PathAppend(m_strIntermediateDirRoot.GetBuffer(MAX_PATH), m_strIntermediateDir);
		m_strIntermediateDirRoot.ReleaseBuffer();
		PathRemoveBackslash(m_strIntermediateDirRoot.GetBuffer(0));
		m_strIntermediateDirRoot.ReleaseBuffer();
	}
}

//--------------------------------------------------------------------------------------------------
//- project new/save/load/close functions
//--------------------------------------------------------------------------------------------------

void CProject::NewProject()
{
	GetCurrentDirectory(MAX_PATH, m_strPathName.GetBuffer(MAX_PATH));
	m_strPathName.ReleaseBuffer();
	m_strPathName += "\\project1.lpr";

	m_strOutputDir = "Output";
	m_strIntermediateDir = "Intermediate";
	m_strOutputPrefix = "project1";
	m_strExecuteDir = "D:/Dev/ProjectG/SFreedom_Dev/Debug/";
	m_strExecute = "MobileSuit.exe";
	m_bGenerateListing = FALSE;

	UpdateDirectories();
	RedrawFilesTree();

	SetModifiedFlag(FALSE);
}

BOOL CProject::New()
{
	SaveModified();

	CProjectNew pn;
	if ( pn.DoModal()!=IDOK )
		return FALSE;

	m_strPathName = pn.GetProjectPathName();
	m_strOutputPrefix = pn.GetProjectName();

	m_strOutputDir = "Output";
	m_strIntermediateDir = "Intermediate";
	m_strExecuteDir = "D:/Dev/ProjectG/SFreedom_Dev/Debug/";
	m_strExecute = "MobileSuit.exe";
	m_bGenerateListing = FALSE;

	UpdateDirectories();

	// make
	CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->AddRoot(GetName());

	RedrawFilesTree();

	pn.CreateByType(this);

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;
}

BOOL CProject::Close()
{
	SaveModified();

	m_strPathName = "";

	RemoveProjectFiles();

	CWorkspaceWnd* pWorkspace = ((CMainFrame*)AfxGetMainWnd())->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->RemoveAll();

	return TRUE;
}

BOOL CProject::Load()
{
	SaveModified();

	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", AfxGetMainWnd());

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	CFile fin;
	if ( !fin.Open(fd.GetPathName(), CFile::modeRead) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	m_strPathName = fd.GetPathName();

	CArchive ar(&fin, CArchive::load);

	RemoveProjectFiles();
	
	BOOL bResult = Load(ar);

	SetModifiedFlag(FALSE);

	RedrawFilesTree();
	UpdateDirectories();

	return bResult;
}

BOOL CProject::Load(CArchive &ar)
{
	ar >> m_strOutputDir;
	ar >> m_strIntermediateDir;
	ar >> m_strOutputPrefix;
	ar >> m_strExecuteDir;
	ar >> m_strExecute;
	ar >> m_bGenerateListing;

	Load(ar, &m_kPgTreeRoot);

	return TRUE;
}

// 여기 다시 확인해보고 고치자.
BOOL CProject::Load(CArchive& ar, CPgTree *pkTree)
{
	//int iSize = kTree.m_listChild.size();
	int iSize;
	ar >> iSize;

	for (int i=0 ; i<iSize ; i++)
	{
		CProjectFile *pkProjectFile = new CProjectFile;

		CPgTree *pkChild = new CPgTree;
		pkChild->pkProjectFile = pkProjectFile;

		int iType;
		ar >> iType;

		if ((CProjectFile::EProjectFileType)iType == CProjectFile::FileType_Folder)
		{
			Load(ar, pkChild);
		}

		pkProjectFile->Load(ar);
		pkChild->m_eObjectType = (CPgTree::EObjectType)iType;
		pkTree->m_listChild.push_back(pkChild);
	}

	return true;
}

BOOL CProject::Save()
{
	return Save(m_strPathName);
}

BOOL CProject::Save(CString strPathName)
{
	CFile fout;
	if ( !fout.Open(strPathName, CFile::modeCreate|CFile::modeWrite) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	CArchive ar(&fout, CArchive::store);

	BOOL bResult = Save(ar);

	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CProject::Save(CArchive &ar)
{
	ar << m_strOutputDir;
	ar << m_strIntermediateDir;
	ar << m_strOutputPrefix;
	ar << m_strExecuteDir;
	ar << m_strExecute;
	ar << m_bGenerateListing;

	Save(ar, &m_kPgTreeRoot);

	return TRUE;
}

BOOL CProject::Save(CArchive& ar, CPgTree *pkTree)
{
	int iSize = pkTree->m_listChild.size();
	ar << iSize;

	std::list<CPgTree*>::iterator itr;
	std::list<CPgTree*> listTree = pkTree->m_listChild;
	for (itr = listTree.begin() ;
		itr != listTree.end() ;
		++itr)
	{
		CPgTree *pkChild = *itr;
		ar << (int)pkChild->pkProjectFile->GetProjectFileType();

		if (pkChild->pkProjectFile->GetProjectFileType() == CProjectFile::FileType_Folder)
		{
			Save(ar, pkChild);
		}

		pkChild->pkProjectFile->Save(ar);
	}

	return true;
}

BOOL CProject::SaveAs()
{
	CFileDialog fd(FALSE, "lpr", m_strPathName, OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", AfxGetMainWnd());

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	m_strPathName = fd.GetPathName();

	BOOL bResult = Save(m_strPathName);

	UpdateDirectories();

	return bResult;
}


void CProject::SaveModified()
{
	if ( m_bModified )
	{
		if ( AfxMessageBox("Your project has changed. Do you want to save it?", MB_YESNO)==IDYES )
			Save();
	}
}

//--------------------------------------------------------------------------------------------------
//- compile/build functions
//--------------------------------------------------------------------------------------------------

BOOL CProject::PositionBreakPoints()
{
	BOOL bModified = FALSE;
	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
	{
		if ( m_files[i]->PositionBreakPoints() )
		{
			bModified = TRUE;
			CLuaView* pView = theApp.FindProjectFilesView(m_files[i]);
			if ( pView )
				m_files[i]->SetBreakPointsIn(pView->GetEditor());
		}
	}

	if ( bModified )
		SetModifiedFlag(TRUE);

	return bModified;
}


BOOL CProject::BuildIntermediateFiles()
{
	long nFiles = m_files.GetSize();
	int nErrors = 0;
	for ( int i=0; i<nFiles; ++i )
	{
		if ( m_files[i]->IsModified() )
		{
			if ( !m_files[i]->Compile() )
				++nErrors;
		}
	}

	return nErrors;
}

BOOL CProject::CheckBeforeBuild()
{
	COutputWnd* pWnd = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	pWnd->SetActiveOutput(COutputWnd::outputBuild);
	CScintillaView* pOutput = pWnd->GetOutput(COutputWnd::outputBuild);

	pOutput->Clear();

	CString strMsg;
	strMsg.Format("------------------ Project %s -------------------\n", GetName());
	pOutput->Write(strMsg);

	if ( !CreateIntermediateDir() )
	{
		pOutput->Write("Cannot create intermediate directory\n");
		return FALSE;
	}

	if ( !CreateOutputDir() )
	{
		pOutput->Write("Cannot create output directory\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CProject::Compile(CProjectFile *pPF)
{
	if ( !CheckBeforeBuild() )
		return FALSE;

	return pPF->Compile();
}

BOOL CProject::BuildOutputFiles()
{
	COutputWnd* pWnd = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	CScintillaView* pOutput = pWnd->GetOutput(COutputWnd::outputBuild);

	CExecutor m_exe;
	CString strCmdLine, strOutput;

	long nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		if ( theApp.FirstFileIsNewer(m_files[i]->GetOutputPathNameExt(), GetDebugPathNameExt()) )
		{
			pOutput->Write("Linking\n");

			strCmdLine.Format("\"%s\" -o \"%s\"", 
				theApp.GetModuleDir() + "\\" + "luac.exe", GetDebugPathNameExt());

			long nFiles = m_files.GetSize();
			for ( int i=0; i<nFiles; ++i )
				strCmdLine += " \"" + m_files[i]->GetOutputPathNameExt() + "\"";


			m_exe.Execute(strCmdLine);
			strOutput = m_exe.GetOutputString();
			if ( !strOutput.IsEmpty() )
			{
				pOutput->Write(strOutput);
				return FALSE;
			}

			break;
		}

	if ( theApp.FirstFileIsNewer(GetDebugPathNameExt(), GetReleasePathNameExt()) )
	{
		strCmdLine.Format("\"%s\" -s -o \"%s\" \"%s\"", 
			theApp.GetModuleDir() + "\\" + "luac.exe", GetReleasePathNameExt(), GetDebugPathNameExt());

		m_exe.Execute(strCmdLine);
		strOutput = m_exe.GetOutputString();
		if ( !strOutput.IsEmpty() )
		{
			pOutput->Write(strOutput);
			return FALSE;
		}
	}

	if ( m_bGenerateListing && theApp.FirstFileIsNewer(GetDebugPathNameExt(), GetListingPathNameExt()) )
	{
		strCmdLine.Format("\"%s\" -p -l \"%s\"", 
			theApp.GetModuleDir() + "\\" + "luac.exe", GetDebugPathNameExt());

		m_exe.Execute(strCmdLine);
		m_exe.SaveOutput(GetListingPathNameExt());
	}

	pOutput->Write("Link complete\n");
	return TRUE;
}

BOOL CProject::Build()
{
	COutputWnd* pWnd = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	CScintillaView* pOutput = pWnd->GetOutput(COutputWnd::outputBuild);

	CString strMsg;
	strMsg.Format("------------------ Building project %s -------------------\n", GetName());
	pOutput->Write(strMsg);

	if ( !CheckBeforeBuild() )
		return FALSE;

	int nErrors = BuildIntermediateFiles();
	
	if ( nErrors!=0 )
	{
		strMsg.Format("%d error(s), build terminated\n", nErrors);
		pOutput->Write(strMsg);
		return FALSE;
	}

	return BuildOutputFiles();
}


void CProject::CleanIntermediateAndOutputFiles()
{
	COutputWnd* pWnd = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	CScintillaView* pOutput = pWnd->GetOutput(COutputWnd::outputBuild);

	pOutput->Clear();

	pOutput->Write("Deleting intermediate files\n");
	long nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		m_files[i]->DeleteIntermediateFiles();

	pOutput->Write("Deleting output files\n");

	DeleteFile(GetDebugPathNameExt());
	DeleteFile(GetReleasePathNameExt());
}


BOOL CProject::HasBreakPoint(const char *szFile, int nLine)
{
	CProjectFile *pPF = GetProjectFile(szFile);
	if ( pPF==NULL )
		return FALSE;

	return pPF->HasBreakPoint(nLine);
}

