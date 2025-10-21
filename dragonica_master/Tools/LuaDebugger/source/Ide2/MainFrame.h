// MainFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRAME_H__CCF97055_F1B3_48F3_B536_F23CF34360E9__INCLUDED_)
#define AFX_MAINFRAME_H__CCF97055_F1B3_48F3_B536_F23CF34360E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WorkspaceWnd.h"
#include "OutputWnd.h"
#include "CallStack.h"
#include "Project.h"
#include "MDIClientWnd.h"
#include "Debugger.h"
#include "VariablesBar.h"
#include "WatchBar.h"

class CMainFrame : public CCJMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	enum {
		modeNoProject,
		modeBuild,
		modeDebug,
		modeDebugBreak
	} appMode;

protected:
	CWorkspaceWnd	m_wndWorkspace;
	COutputWnd		m_wndOutput;
	CCallStack		m_wndCallStack;
	CProject m_project;
	CDebugger m_debug;
	CMDIClientWnd	m_wndMDIClient;
	CVariablesBar	m_wndLocals;
	CWatchBar		m_wndWatches;
	HACCEL m_hAccelBuild, m_hAccelDebug, m_hAccelDebugBreak, m_hAccelNoProject;
	int m_nAppMode;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetCalltip(const char* szWord, CString& strCalltip);
	void SetMode(int nMode);
	void GotoFileLine(const char* szFile, int nLine);
	void GotoFileLineForDebug(const char* szFile, int nLine);
	LRESULT DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	CLuaView* GetActiveView();
	int GetMode() { return m_nAppMode; };
	COutputWnd* GetOutputWnd() { return &m_wndOutput; };
	CProject* GetProject() { return &m_project; };
	CWorkspaceWnd* GetWorkspaceWnd() { return &m_wndWorkspace; };
	CCallStack* GetCallStack() { return &m_wndCallStack; };
	CDebugger* GetDebugger() { return & m_debug; };
	BOOL InitDockingWindows();
	void UpdateFrameTitleForDocument(LPCTSTR lpszDocName);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpenproject();
	afx_msg void OnFileSaveproject();
	afx_msg void OnFileSaveprojectas();
	afx_msg void OnProjectAddFiles();
	afx_msg void OnProjectProperties();
	afx_msg void OnClose();
	afx_msg void OnBuildBuild();
	afx_msg void OnUpdateBuildCompile(CCmdUI* pCmdUI);
	afx_msg void OnBuildCompile();
	afx_msg void OnUpdateBuildBuild(CCmdUI* pCmdUI);
	afx_msg void OnBuildRebuildall();
	afx_msg void OnBuildClean();
	afx_msg void OnBuildGo();
	afx_msg void OnUpdateDebugBreak(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDebug(CCmdUI* pCmdUI);
	afx_msg void OnDebugGo();
	afx_msg void OnDebugStepinto();
	afx_msg void OnDebugStepover();
	afx_msg void OnDebugStepout();
	afx_msg void OnDebugRuntocursor();
	afx_msg void OnDebugBreak();
	afx_msg void OnDebugStopdebugging();
	afx_msg void OnDebugRestart();
	afx_msg void OnUpdateCmdForProject(CCmdUI* pCmdUI);
	afx_msg void OnFileNewproject();
	afx_msg void OnFileCloseproject();
	afx_msg void OnBuildExecute();
	afx_msg void OnHelpContactauthor();
	afx_msg void OnHelpLuahelppdf();
	afx_msg void OnHelpVisithomepage();
	afx_msg void OnHelpLuahomepage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__CCF97055_F1B3_48F3_B536_F23CF34360E9__INCLUDED_)
