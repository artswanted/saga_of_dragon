// ProjectFile.h: interface for the CProjectFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTFILE_H__737B3FC6_28E7_47DD_B9F6_D41D550C8E23__INCLUDED_)
#define AFX_PROJECTFILE_H__737B3FC6_28E7_47DD_B9F6_D41D550C8E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLuaEditor;

class CProjectFile
{
// PG
public:
	typedef enum eProjectFileType
	{
		FileType_None,
		FileType_File,
		FileType_Folder,
		FileType_Project,
	}EProjectFileType;
	EProjectFileType m_eFileType;

	EProjectFileType GetProjectFileType() { return m_eFileType; }

public:
	void UpdateRelPathName();
	void DeleteIntermediateFiles();
	BOOL Compile();
	BOOL IsModified();
	BOOL Save(CArchive& ar);
	BOOL Load(CArchive& ar);
	void RemoveBreakPoint(int nLine);
	BOOL HasFile(CString strPathName);
	void SetBreakPointsIn(CLuaEditor* pEditor);
	BOOL HasBreakPoint(int nLine);
	CProjectFile();
	virtual ~CProjectFile();

	void AddDebugLine(int nLine);
	void RemoveAllDebugLines();
	void AddBreakPoint(int nLine);
	void RemoveAllBreakPoints();

	BOOL PositionBreakPoints();
	int GetNearestDebugLine(int nLine);
	int GetPreviousDebugLine(int nLine);
	int GetNextDebugLine(int nLine);

	// PG
	int GetBreakPointCount();
	int GetBreakPointToIdx(int nIdx);

	void SetPathName(CString strPathName) { m_strPathName=strPathName; UpdateRelPathName(); };
	CString GetPathName() { return m_strPathName; };
	CString GetName();
	CString GetNameExt();
	CString GetOutputNameExt() { return GetName()+".out"; }
	CString GetOutputPathNameExt();

protected:
	CString m_strPathName, m_strRelPathName;
	CMap<int, int, BOOL, BOOL> m_breakPoints;
	int m_nMinBreakPoint, m_nMaxBreakPoint;
	CMap<int, int, BOOL, BOOL> m_debugLines;
	int m_nMinDebugLine, m_nMaxDebugLine;
	SYSTEMTIME	m_timeCompiled;
};

#endif // !defined(AFX_PROJECTFILE_H__737B3FC6_28E7_47DD_B9F6_D41D550C8E23__INCLUDED_)
