// WatchList.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "WatchList.h"

#include "MainFrame.h"
#include "Debugger.h"
#include "pgserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchList

CWatchList::CWatchList()
{
}

CWatchList::~CWatchList()
{
}


BEGIN_MESSAGE_MAP(CWatchList, CCJListCtrl)
	//{{AFX_MSG_MAP(CWatchList)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchList message handlers

void CWatchList::AddEmptyRow()
{
	int nItem = InsertItem(GetItemCount(), "");
	SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
}

BOOL CWatchList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LVS_EDITLABELS|LVS_NOSORTHEADER|LVS_SHOWSELALWAYS|LVS_SINGLESEL;

	return CCJListCtrl::PreCreateWindow(cs);
}

void CWatchList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SetFocus();

	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	SubItemHitTest(&lvhti);

	if (lvhti.flags & LVHT_ONITEMLABEL)
		EditLabel(lvhti.iItem);
	else
		CCJListCtrl::OnLButtonDblClk(nFlags, point);
}

void CWatchList::AddEditItem(LVITEM &item)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pFrame->GetDebugger()->Eval(item.pszText) == "")
	{
		// Send Watch List
		PgDebugEvalInfo kToSendEvalInfo;
		kToSendEvalInfo.ePacketType = PgLuaDebugPacket::PacketType_Debug_EvalData;
		strcpy_s(kToSendEvalInfo.acVariableName, item.pszText);
		kToSendEvalInfo.bIsLastEvalInfo = true;
		g_pkPgServer->Send(sizeof(PgDebugEvalInfo), ((char *)&kToSendEvalInfo));
	}

	if ( item.iItem == GetItemCount()-1 )
	{
		if ( strlen(item.pszText) == 0 )
			return;

		m_exps.Add(item.pszText);
		AddEmptyRow();
		UpdateRow(item.iItem);
	}
	else
	{
		if ( strlen(item.pszText) == 0 )
		{
			DeleteItem(item.iItem);
			m_exps.RemoveAt(item.iItem);
			return;
		}
		else
		{
			m_exps[item.iItem] = item.pszText;
			UpdateRow(item.iItem);
		}
	}
}

void CWatchList::UpdateRow(int iItem)
{
	SetItemText(iItem, 0, m_exps[iItem]);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if ( pFrame->GetMode() == CMainFrame::modeDebugBreak )
	{
		SetItemText(iItem, 1, pFrame->GetDebugger()->Eval(m_exps[iItem]));
	}
}

void CWatchList::Redraw()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	for ( int i=0; i<m_exps.GetSize(); ++i )
		SetItemText(i, 1, pFrame->GetDebugger()->Eval(m_exps[i]));
}

void CWatchList::ReSendWatchData()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	for ( int i=0; i<m_exps.GetSize(); ++i )
	{
		// Send Watch List
		PgDebugEvalInfo kToSendEvalInfo;
		kToSendEvalInfo.ePacketType = PgLuaDebugPacket::PacketType_Debug_EvalData;
		strcpy_s(kToSendEvalInfo.acVariableName, m_exps[i]);
		if (i==m_exps.GetSize()-1)
			kToSendEvalInfo.bIsLastEvalInfo = true;
		else
			kToSendEvalInfo.bIsLastEvalInfo = false;
		g_pkPgServer->Send(sizeof(PgDebugEvalInfo), ((char *)&kToSendEvalInfo));
		//SetItemText(i, 1, pFrame->GetDebugger()->Eval(m_exps[i]));
	}
}
