// ProjectFile.cpp: implementation of the CProjectFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "ProjectFile.h"

#include "LuaEditor.h"
#include "MainFrame.h"
#include "Executor.h"
#include "ScintillaView.h"
#include "LuaHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjectFile::CProjectFile()
{
	RemoveAllDebugLines();
	RemoveAllBreakPoints();

	GetSystemTime(&m_timeCompiled);
}

CProjectFile::~CProjectFile()
{

}

void CProjectFile::RemoveAllDebugLines()
{
	m_nMinDebugLine = 2147483647;
	m_nMaxDebugLine = 0;

	m_debugLines.RemoveAll();
}

void CProjectFile::AddDebugLine(int nLine)
{
	m_debugLines[nLine] = 1;
	int iTest = m_debugLines.GetCount();
	if ( nLine<m_nMinDebugLine )
		m_nMinDebugLine = nLine;
	if ( nLine>m_nMaxDebugLine )
		m_nMaxDebugLine = nLine;
}

void CProjectFile::RemoveAllBreakPoints()
{
	m_nMinBreakPoint = 2147483647;
	m_nMaxBreakPoint = 0;

	m_breakPoints.RemoveAll();

	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

void CProjectFile::AddBreakPoint(int nLine)
{
	m_breakPoints[nLine] = 1;
	int iTest2 = m_breakPoints.GetCount();
	if ( nLine<m_nMinBreakPoint)
		m_nMinBreakPoint = nLine;
	if ( nLine>m_nMaxBreakPoint )
		m_nMaxBreakPoint = nLine;

	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

void CProjectFile::RemoveBreakPoint(int nLine)
{
	m_breakPoints.RemoveKey(nLine);

	m_nMinBreakPoint = 2147483647;
	m_nMaxBreakPoint = 0;

	POSITION pos = m_breakPoints.GetStartPosition();
	int nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		if ( nLine<m_nMinBreakPoint)
			m_nMinBreakPoint = nLine;
		if ( nLine>m_nMaxBreakPoint )
			m_nMaxBreakPoint = nLine;
	}

	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

int CProjectFile::GetNextDebugLine(int nLine)
{
	int nTemp;
	++nLine;

	while ( nLine<=m_nMaxDebugLine )
		if ( m_debugLines.Lookup(nLine, nTemp) )
			return nLine;
		else
			++nLine;

	return 0;
}

int CProjectFile::GetPreviousDebugLine(int nLine)
{
	int nTemp;
	--nLine;

	while ( nLine>=m_nMinDebugLine )
		if ( m_debugLines.Lookup(nLine, nTemp) )
			return nLine;
		else
			--nLine;

	return 0;
}

int CProjectFile::GetNearestDebugLine(int nLine)
{
	int nTemp, nNearest;
	if ( m_debugLines.Lookup(nLine, nTemp) )
		return nLine;

	if ( (nNearest=GetNextDebugLine(nLine)) > 0 )
		return nNearest;

	if ( (nNearest=GetPreviousDebugLine(nLine)) > 0 )
		return nNearest;

	return 0;
}

int CProjectFile::GetBreakPointCount()
{
	return m_breakPoints.GetCount();
}

int CProjectFile::GetBreakPointToIdx(int nIdx)
{
	int iCount = -1;
	int iLine=0;

	while(1)
	{
		int iTemp = 0;
		m_breakPoints.Lookup(iLine, iTemp);
		if (iTemp)
		{
			iCount += 1;

			if (iCount == nIdx)
			{
				return iLine;
			}
		}

		iLine += 1;
	}

	return 0;
}

BOOL CProjectFile::PositionBreakPoints()
{
	if ( !CLuaHelper::LoadDebugLines(this) )
		return FALSE;

	BOOL bModified = FALSE;
	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp, nNearest;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		nNearest = GetNearestDebugLine(nLine);
		if ( nNearest == 0 )
		{
			m_breakPoints.RemoveKey(nLine);
			bModified = TRUE;
		}
		else if ( nLine != nNearest )
		{
			m_breakPoints.RemoveKey(nLine);
			m_breakPoints.SetAt(nNearest, 1);
			bModified = TRUE;
		}
	}

	return bModified;
}

CString CProjectFile::GetName()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname);
}

CString CProjectFile::GetNameExt()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname)+ext;
}

BOOL CProjectFile::HasBreakPoint(int nLine)
{
	int nTemp;
	return m_breakPoints.Lookup(nLine, nTemp);
}

void CProjectFile::SetBreakPointsIn(CLuaEditor *pEditor)
{
	pEditor->ClearAllBreakpoints();

	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		pEditor->SetBreakpoint(nLine);
	}
}

BOOL CProjectFile::HasFile(CString strPathName)
{
 	if(!m_strPathName.CompareNoCase(strPathName))
 		return TRUE;
 
 	//should actually search using the luasearch path
 	DWORD n=MAX_PATH;
 	CString sFullPath;	
 	::GetFullPathName(strPathName,n,sFullPath.GetBuffer(n),NULL);
 	sFullPath.ReleaseBuffer();
 
 	if(!m_strPathName.CompareNoCase(sFullPath))
 		return TRUE;
 	return FALSE;
}


BOOL CProjectFile::Load(CArchive &ar)
{
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	m_strPathName = pProject->GetProjectDir();

	int iType;
	ar >> iType;
	m_eFileType = (EProjectFileType)iType;
	if (m_eFileType != FileType_File)
	{
		ar >> m_strPathName;
		return true;
	}

	RemoveAllDebugLines();
	RemoveAllBreakPoints();

	ar >> m_strRelPathName;

	PathAppend(m_strPathName.GetBuffer(MAX_PATH), m_strRelPathName);

	ar >> m_nMinBreakPoint;
	ar >> m_nMaxBreakPoint;

	int nBreakPoints;
	ar >> nBreakPoints;

	for ( int i=0; i<nBreakPoints; ++i )
	{
		int nLine;
		ar >> nLine;

		m_breakPoints[nLine] = 1;
	}

	return TRUE;
}

BOOL CProjectFile::Save(CArchive &ar)
{
	ar << m_eFileType;
	if (m_eFileType != FileType_File)
	{
		ar << m_strPathName;
		return true;
	}

	ar << m_strRelPathName;
	ar << m_nMinBreakPoint;
	ar << m_nMaxBreakPoint;

	int nBreakPoints = m_breakPoints.GetCount();
	ar << nBreakPoints;

	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		ar << nLine;
	}

	return TRUE;
}

BOOL CProjectFile::IsModified()
{
	WIN32_FILE_ATTRIBUTE_DATA sourceFile, compiledFile;

	if (! ::GetFileAttributesEx(m_strPathName, GetFileExInfoStandard, &sourceFile) )
		return TRUE;

	if (! ::GetFileAttributesEx(GetOutputPathNameExt(), GetFileExInfoStandard, &compiledFile) )
		return TRUE;

	ULARGE_INTEGER sourceTime, compiledTime;
	sourceTime.LowPart = sourceFile.ftLastWriteTime.dwLowDateTime;
	sourceTime.HighPart = sourceFile.ftLastWriteTime.dwHighDateTime;
	compiledTime.LowPart = compiledFile.ftLastWriteTime.dwLowDateTime;
	compiledTime.HighPart = compiledFile.ftLastWriteTime.dwHighDateTime;

	return ( sourceTime.QuadPart > compiledTime.QuadPart );
}

BOOL CProjectFile::Compile()
{
	CExecutor m_exe;

	COutputWnd* pWnd = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	CScintillaView* pOutput = pWnd->GetOutput(COutputWnd::outputBuild);

	pOutput->Write(GetNameExt() + "\n");

	CString strCmdLine;
	strCmdLine.Format("\"%s\" -o \"%s\" \"%s\"", 
		theApp.GetModuleDir() + "\\" + "luac.exe", GetOutputPathNameExt(), GetPathName());

	m_exe.Execute(strCmdLine);
	CString strOutput = m_exe.GetOutputString();
	if ( !strOutput.IsEmpty() )
	{
		pOutput->Write(strOutput);
		return FALSE;
	}

	return TRUE;
}

void CProjectFile::DeleteIntermediateFiles()
{
	DeleteFile(GetOutputPathNameExt());
}

CString CProjectFile::GetOutputPathNameExt()
{
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();	
	return pProject->GetIntermediateDir() + "\\" + GetOutputNameExt();
}

void CProjectFile::UpdateRelPathName()
{
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();

	PathRelativePathTo(m_strRelPathName.GetBuffer(MAX_PATH), 
		pProject->GetProjectDir(), FILE_ATTRIBUTE_DIRECTORY,
		m_strPathName, 0);
	m_strRelPathName.ReleaseBuffer();
}
