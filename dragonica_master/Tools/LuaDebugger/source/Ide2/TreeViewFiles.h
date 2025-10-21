#if !defined(AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_)
#define AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewFiles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles view

class CTreeViewFiles : public CTreeView
{
protected:
	CTreeViewFiles();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeViewFiles)

// Attributes
public:

// Operations
public:
	void RemoveAll();
	HTREEITEM AddProjectFile(CString strName, long lParam);
	HTREEITEM AddRoot(CString strProject);

// PG variable
public:
	std::list<HTREEITEM> m_listFolder;
	HTREEITEM m_hCurrentFolder;

// PG function
public:
	HTREEITEM AddProjectFile(CString strName, long lParam, HTREEITEM kParent, CProjectFile* pkProjectFile);
	HTREEITEM GetCurrentFolder() { return m_hCurrentFolder; }
	HTREEITEM GetRoot() { return m_hRoot; }

protected:
	CTreeCtrl* m_pTree;
	HTREEITEM m_hRoot, m_hFilesFolder;
	CImageList m_images;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewFiles)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//virtual void OnProjectAddFiles();
	virtual void OnProjectProperties();
	virtual void OnAddFolder();
	virtual void OnFolderAddFiles();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeViewFiles();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewFiles)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWFILES_H__D5303282_718E_4AB7_A667_017E34E216BB__INCLUDED_)
