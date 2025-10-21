#include "PgAXPlay.h"

CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap) 
END_OBJECT_MAP() 

PgAXPlay::PgAXPlay()
: m_hWndIE(NULL)
, m_pSink(NULL)
, m_pUnkIE(NULL)
{
	AtlAxWinInit();
}
PgAXPlay::~PgAXPlay()
{
	if(m_pSink)
	{
		m_pSink->DispEventUnadvise(m_pUnkIE);
		m_pSink = NULL;
	}
	if(m_hWndIE)
	{
		SendMessage(m_hWndIE, WM_CLOSE, 0, 0);
		m_hWndIE = NULL;
	}
	if(m_pWB2)
	{
		m_pWB2.Release();
	}
	AtlAxWinTerm();	
}

bool PgAXPlay::CreateWB(HWND hParent, HINSTANCE hInstance, const std::string& Url, int const iX, int const iY, int const iWidth, int const iHeight)
{
	m_hWndIE = CreateWindow(L"AtlAxWin80", L"Shell.Explorer.2",
		WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE|WS_CLIPSIBLINGS, 
		0, 0, 1024, 768, hParent, (HMENU)0, hInstance, NULL);

	if (m_hWndIE == 0)
	{
		WinMessageBox(hParent, L"Unalbe to Initialize WB", g_PProcess.GetMsg(E_FAILEDINITWEBBR), MB_OK);
		return false;
	}

	if (AtlAxGetControl(m_hWndIE, &m_pUnkIE) == S_OK) 
	{ 
		m_pWB2 = m_pUnkIE; 
		if (m_pWB2) 
		{ 
			CComObject<PgIESink>::CreateInstance(&m_pSink); 
			m_pSink->AddRef();
			HRESULT hr = m_pSink->AdviseToIE(m_pUnkIE, m_pWB2);
			
			m_pWB2->put_Left(iX);
			m_pWB2->put_Top(iY);
			m_pWB2->put_Width(iWidth);
			m_pWB2->put_Height(iHeight);

			CComVariant urlString(Url.c_str());
			CComVariant empty; 
			m_pWB2->Navigate2(&urlString, &empty, &empty, &empty, &empty);
		
//			m_hWndIE = NULL;
			m_pUnkIE = NULL;
			m_pWB2.Release();
		}
		else 
		{
			WinMessageBox(hParent, L"Unalbe to Initialize WB", g_PProcess.GetMsg(E_FAILEDINITWEBBR), MB_OK);
			return false;
		}
	}
	else
	{
		WinMessageBox(hParent, L"Unalbe to Initialize WB", g_PProcess.GetMsg(E_FAILEDINITWEBBR), MB_OK);
		return false;
	}
	return true;
}

void _stdcall PgIESink::OnDocumentComplete(IDispatch** ppDisp, VARIANT *pvUrl)
{
	if(pvUrl == NULL || pvUrl->bstrVal == NULL)
		return;

	std::string strName(MB(pvUrl->bstrVal));
	if(strName == "about:blank")
	{
	}
}