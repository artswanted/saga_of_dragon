// CSIME.cpp: implementation of the CCSIME class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../CSIME.h"
#include "BM/threadobject.h"
//*
//언어 코드
#define LID_TRADITIONAL_CHINESE	0x0404
#define LID_JAPANESE			0x0411
#define LID_KOREAN				0x0412
#define LID_SIMPLIFIED_CHINESE	0x0804

#define MAX_BUFFER_SIZE 4096
//#define _CSIME_DEBUG

using namespace CS;
HWND g_hWndParent = 0;
bool g_bIsIMEKey =  false;
bool g_bCompClear = false;

class CSafeIMEContext
	: public BM::CAutoMutex
{
public:
	CSafeIMEContext(HWND const hWnd, HIMC &hIMC, Loki::Mutex &rMutex)
		:	m_hWnd(hWnd), m_rhIMC(hIMC), BM::CAutoMutex(rMutex)
	{
		m_rhIMC = ::ImmGetContext(m_hWnd);
	}
	~CSafeIMEContext()
	{
		::ImmReleaseContext(m_hWnd, m_rhIMC);
	}

private:
	HIMC &m_rhIMC;
	HWND const& m_hWnd;
};

WNDPROC CCSIME::m_oldEditWndProc = NULL;

CCSIME::CCSIME()
{
	m_hEWnd = NULL;
	m_lpCL = NULL;
	m_hImcSave = NULL;

	SetShowCandidate(false);

	SetIMEName( _T("") );
	SetLanguageType();

	m_bIsShowCandidate = false;//Candidate윈도우를 보이게 하는 플래그 
	m_bIsSymbolMode = false;//심볼 모드인지 알아 보는 플래그
	m_bIsFullShape = false;//전/반자 플래그
	m_iIsNumeric = 0;//숫자전용 모드 플래그
	m_bIsMultiLine = false;//엔터 입력을 받을 것인가?
	m_bIsComp = false;//현재 조합중인가?
	m_bIsForceEnglishIME = false;

	SetMultiLine(false, 100);
	m_strCompStr = _T("");
	m_strCompReadStr = _T("");
	m_bPasswordMode = false;

	m_pLogFunc = NULL;
	g_bIsIMEKey = false;
	g_bCompClear = false;

	Clear();
}

CCSIME::~CCSIME()
{
	m_hImcSave = NULL;
	m_pLogFunc = NULL;
	Clear(); 
}

void CCSIME::Clear()
{
	FreeCandList();
	ClearCandidateBuf();
	m_bIsComp = false;
	g_bIsIMEKey = false;
	g_bCompClear = false;
	
	m_mapKBHash.clear();

	if( GetEditHWND() )
	{
		::DestroyWindow(m_hEWnd);
		m_hEWnd = NULL;
	}
}

void CCSIME::ClearCandidateBuf()
{
	m_vtCandlist.clear();
}

bool const CCSIME::Init(HWND const hParentWnd, HINSTANCE const hInstance, short int const nMenuSize, DWORD dwStyle, LP_LOG_FUNC pLogFunc)
{
	if (0==dwStyle)
	{
		dwStyle = WS_CHILD | ES_WANTRETURN;
	}

	Clear();
	m_pLogFunc = pLogFunc;
	m_hEWnd = ::CreateWindowEx(WS_EX_TOPMOST, _T("edit"), _T("CSIME_Wnd") ,
			dwStyle,
			0, 0, 800, 400, 
			hParentWnd,NULL, hInstance, NULL);

	if (NULL == m_hEWnd)
	{
		return false;
	}

	//m_oldEditWndProc = reinterpret_cast<WNDPROC>(GetWindowLong(m_hEWnd, GWLP_WNDPROC));
	//SetWindowLong(m_hEWnd, GWLP_WNDPROC, reinterpret_cast<LONG>(EditWndProc));

#if defined(_CSIME_DEBUG) && !defined(EXTERNAL_RELEASE)
	TCHAR szBuf[4096];
	::swprintf_s(szBuf, 4096, _T("CCSIME::EditWnd [%#X]\n"), (DWORD)m_hEWnd);
	CSIME_LOG(szBuf);
#endif

	g_hWndParent = hParentWnd;
	::ShowWindow(m_hEWnd,SW_HIDE);

	SetLimitLength( nMenuSize, true );
	m_bIsComp = false;
	g_bIsIMEKey = false;
	g_bCompClear = false;

	return true;
}

void CCSIME::SetPasswordMode(bool const bIsPasswordMode)
{
	m_bPasswordMode = bIsPasswordMode;
	//::SendMessage(      // returns LRESULT in lResult     
	//	(HWND) m_hEWnd,      // handle to destination control     
	//	(UINT) EM_SETPASSWORDCHAR,      // message ID     
	//	(WPARAM)((bIsPasswordMode)?_T('*'):0),      // = (WPARAM) () wParam;    
	//	(LPARAM) 0// = 0; not used, must be zero 
	//	);  
}

void CCSIME::SetOnlyNumeric( int const iIsNumeric, bool const bDoClear)
{
	if(bDoClear)
	{
		SetString();
	}
	if (m_iIsNumeric == iIsNumeric)
	{
		return;
	}
	m_iIsNumeric = iIsNumeric;
	
	DWORD const dwBefore = ::GetWindowLong(GetEditHWND(), GWL_STYLE);
	
	if(m_iIsNumeric)
	{
		::SetWindowLong(GetEditHWND(),GWL_STYLE, dwBefore | ES_NUMBER);
	}
	else
	{
		::SetWindowLong(GetEditHWND(),GWL_STYLE, dwBefore & ~ES_NUMBER);
	}
}

void CCSIME::SetMultiLine(bool const bIsMultiLine, DWORD const dwMaxCaretPerLine, DWORD const dwMaxLineConut, bool const bDoClear)
{
	if (m_bIsMultiLine == bIsMultiLine && m_dwMaxCaretPerLine == dwMaxCaretPerLine && m_dwMaxLineCount == dwMaxLineConut)
	{
		return;
	}

	m_bIsMultiLine = bIsMultiLine;

	if(IsMultiLine())
	{
		SetLimitLength( ((dwMaxCaretPerLine+2) * dwMaxLineConut) -2, bDoClear);//+2 는 캐니지리턴 값, -2는 마지막줄의 캐니지리턴값
		
		m_dwMaxCaretPerLine = dwMaxCaretPerLine;
		m_dwMaxLineCount = dwMaxLineConut;

		::SendMessage(GetEditHWND(), EM_LINELENGTH,  m_dwLimitLength, NULL);
	}
}

void CCSIME::SetLimitLength(DWORD const dwLen, bool const bDoClear)
{
	if(bDoClear)
	{
		SetString();
	}
	
	if (m_dwLimitLength == dwLen)
	{
		return;
	}
	m_dwLimitLength = dwLen;

	::SendMessage(GetEditHWND(), EM_SETLIMITTEXT,  m_dwLimitLength, NULL);
}

const CARETDATA& CCSIME::GetCaretPos()
{
	CARETDATA TempCD = m_CaretData;

		::SendMessage(GetEditHWND(), EM_GETSEL,  (WPARAM)&m_CaretData.iSelectStart, (LPARAM)&m_CaretData.iSelectEnd);

		if(m_CaretData.iSelectStart != TempCD.iSelectStart)
		{
			m_CaretData.iCaretPos = m_CaretData.iSelectStart;
		}

		if(m_CaretData.iSelectEnd != TempCD.iSelectEnd)
		{
			m_CaretData.iCaretPos = m_CaretData.iSelectEnd;
		}
	return m_CaretData;
}

void CCSIME::SetCaretPos(bool const bIsHome)//캐럿을 맨앞 또는 맨뒤로 설정합니다.
{
	if(bIsHome)//맨앞
	{
		::SendMessage(GetEditHWND(), WM_KEYDOWN, VK_HOME, NULL);
	}
	else
	{
		::SendMessage(GetEditHWND(), WM_KEYDOWN, VK_END, NULL);
	}
	SetCaretPosChanged();
}

void CCSIME::SetCaretPos(__int32 const iStartPos, __int32 const iEndPos)//캐럿의 영역 설정 iRange 가 0 이 아니면 영역 설정.
{
	if (iStartPos < 0 || iEndPos < 0)
		return;

	::SendMessage(GetEditHWND(), EM_SETSEL,  (WPARAM)iStartPos, (LPARAM)iEndPos);
	
	m_CaretData.iCaretPos		= iEndPos;
	m_CaretData.iSelectStart	= iStartPos;
	m_CaretData.iSelectEnd		= iEndPos;
	SetCaretPosChanged();
}

bool CCSIME::IsCorrectString(std::wstring const& strSrc)
{
	switch( IsOnlyNumeric() )
	{
	case 0:
		{
		}break;
	case 1://앞에 0을 허용하지 않는다.
		{
			std::wstring::size_type len = strSrc.length();
			if( len )
			{
				return 0 != ::_wtoi( strSrc.c_str() );
			}
		}break;
	default:// 앞에 0을 허용한다.
		{
			std::wstring::const_iterator itr = std::find_if( strSrc.begin(), strSrc.end(), SOnlyNumeric() );
			return itr == strSrc.end();
		}break;
	}

	return true;
}

void CCSIME::GetWindowText(std::wstring &str)
{
//	str.reserve(m_dwLimitLength+2);//이전 상태가 현재 예약하려는 크기보다 크다고 해도 벡터내부에서는 다르게 동작한다.
	static std::vector< TCHAR > vt(m_dwLimitLength,NULL);//m_dwLimitLength 의 크기를 가지는 벡터 //050721_KCH 일본 IME 포커스 버그 수정
	vt.resize(m_dwLimitLength+2);

	vt.at(0) = _T('\0');//

	if (GetTextChanged())
	{
		::GetWindowText(GetEditHWND(), &vt.at(0), m_dwLimitLength+1 );
		SetTextChanged(false);
	}
	str = (const TCHAR*)&vt[0];

#ifdef _CSIME_DEBUG
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("CCSIME::GetWindowText [%s]\n"), str.c_str());
    //CSIME_LOG(szDebug);
}	
#endif
}

bool CCSIME::IsCanInputAtMultiLine(std::wstring const& strSrc, int const nVirtKey)
{
	if(!IsMultiLine()){return true;}//멀티라인이 아니면 리턴

	size_t dwLineCount = 0;
	std::wstring::size_type size = strSrc.length();

	DWORD dwNotUsedCaret = 0;//라인당 사용하지 않은 캐럿

	std::wstring strTemp(strSrc);//버퍼 할당
	size_t const dwSrcSize = strSrc.size();

	while(true)
	{
		std::wstring::size_type idx = strTemp.find( _T("\r\n") );
		if(std::wstring::npos != idx)//캐니지리턴을 찾았다
		{
			++dwLineCount;//찾았으니 일단 한줄 먹고
			std::wstring strHead = strTemp.substr(0, idx);// \n\r 의 길이가 2 이기 때문에 +1을 해야함

			dwLineCount += strHead.length() / m_dwMaxCaretPerLine;

//			if(strHead.length() % m_dwMaxCaretPerLine)
//			{
//				dwNotUsedCaret += m_dwMaxCaretPerLine - (strHead.length() % m_dwMaxCaretPerLine);
//				++dwLineCount;
//			}

			if(dwLineCount >= m_dwMaxLineCount)
			{
				return false;
			}
			if( (dwLineCount+1) == m_dwMaxLineCount && VK_RETURN == nVirtKey)//한줄더 더해기한상태에서 엔터키 들어오면
			{
				return false;
			}

			std::wstring strTail = strTemp.substr(idx+2);// \n\r 의 길이가 2 이기 때문에
			if(!strTail.empty())//비어있지 않다
			{
				 strTemp = strTail;
			}
			else
			{
				break;
			}
		}
		else //캐니지리턴을 찾지 못했다
		{
			dwLineCount += strTemp.length() / m_dwMaxCaretPerLine;
			
			if(dwLineCount >= m_dwMaxLineCount)
			{
				return false;
			}
			if( (dwLineCount+1) == m_dwMaxLineCount && VK_RETURN == nVirtKey)
			{
				return false;
			}

			if( strTemp.length() % m_dwMaxCaretPerLine)
			{
				++dwLineCount; 
			}
			break;
		}
	}
	return true;
}

void CCSIME::SetString(std::wstring const& wstrText)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("CCSIME::SetString [%s]\n"), wstrText.c_str());
    CSIME_LOG(szDebug);
}	
#endif

	if(IsCorrectString(wstrText))
	{
		::SetWindowText(GetEditHWND(), wstrText.c_str() );
		m_strResult = wstrText;
		//if (m_bPasswordMode)
		//{
		//	int iSize = m_strResult.size();
		//	m_strResult.clear();

		//	for (int i = 0; i < iSize; i++)
		//	{
		//		m_strResult.push_back(_T('*'));
		//	}
		//}
	}
	else
	{
		::SetWindowText(GetEditHWND(), _T(""));
		m_strResult = _T("");
	}

	SetCaretPos(false);//캐럿을 맨 끝으로
}

std::wstring const& CCSIME::GetResultStr()
{
	if (GetTextChanged())
	{
		GetWindowText(m_strResult);
#if defined(_CSIME_DEBUG)
		std::wstring text;
		text.resize(4096);
		::GetWindowText(GetEditHWND(), &text.at(0), 4096);				
		std::wstring old = m_strResult;
#endif

		if( !IsCorrectString(m_strResult) )
		{
			SetString();
			m_strResult = _T("");
		}

 	if( (1 == IsOnlyNumeric()) && m_strResult.size() )
		{
			double const dbNum = ::_wtof(m_strResult.c_str());
			TCHAR szNumeric[4096] = {0,};
			::swprintf_s( szNumeric, 4096, _T("%.0f"), dbNum );
			m_strResult = szNumeric;
		}

		if(m_bIsForceEnglishIME)
		{
			if(CorrectAlphaNumeric(m_strResult))
			{
//				::SetWindowText(GetEditHWND(),m_strResult.c_str());//이거하면 comp 끝나고 앞에 글자가 지워짐
			}
		}

#if defined(_CSIME_DEBUG)
		TCHAR szDebug[4096] = {0,};
		::swprintf_s(szDebug, 4096,_T("CCSIME::GetResultStr() before:%s after:%s real:%s\n"), old.c_str(), m_strResult.c_str(), text.c_str());
		//CSIME_LOG(szDebug);
#endif

		SetTextChanged(false);

		//if (m_bPasswordMode)
		//{
		//	int iSize = m_strResult.size();
		//	m_strResult.clear();

		//	for (int i = 0; i < iSize; i++)
		//	{
		//		m_strResult.push_back(_T('*'));
		//	}
		//}
	}

	return m_strResult;
}

void CCSIME::SetEnableIME(bool const bIsEnable)
{
	if(bIsEnable)
	{
		if(m_hImcSave)
		{
#if defined(_CSIME_DEBUG)
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096,_T("CCSIME::SetEnableIme [%d]\n"), bIsEnable);
			CSIME_LOG(szDebug);
#endif
			const HKL hKL = ::GetKeyboardLayout( 0 );
			if( ::ImmIsIME( hKL ))
			{
				TCHAR szTemp[MAX_BUFFER_SIZE]= {0,};
				::ImmEscape( hKL, m_hImcSave, IME_ESC_IME_NAME, szTemp );//IME이름 가져오기
				SetIMEName(szTemp);//입력기 정보 셋팅
			}

			ImmSetCompositionString( m_hImcSave, SCS_SETSTR, NULL, 0 , NULL, 0 );
			ImmSetCompositionString( m_hImcSave, SCS_CHANGECLAUSE, NULL, 0 , NULL, 0 );
			ImmSetCompositionString( m_hImcSave, SCS_CHANGEATTR, NULL, 0 , NULL, 0 );
			ImmNotifyIME( m_hImcSave, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );

			::ImmAssociateContext(g_hWndParent, m_hImcSave);
			m_hImcSave = NULL;
		}
	}
	else
	{
		HIMC hIMC = NULL;
		
		CSafeIMEContext SafeIC(g_hWndParent, hIMC, m_kMutex);
		
		if(!hIMC){return;}


#if defined(_CSIME_DEBUG)
		TCHAR szDebug[4096] = {0,};
		::swprintf_s(szDebug, sizeof(szDebug)/sizeof(TCHAR),_T("CCSIME::SetEnableIme [%d]\n"), bIsEnable);
		CSIME_LOG(szDebug);
#endif

		m_hImcSave = hIMC;//끄기전 IME 핸들을 저장한다 
		ImmSetCompositionString( m_hImcSave, SCS_SETSTR, NULL, 0 , NULL, 0 );
		ImmSetCompositionString( m_hImcSave, SCS_CHANGECLAUSE, NULL, 0 , NULL, 0 );
		ImmSetCompositionString( m_hImcSave, SCS_CHANGEATTR, NULL, 0 , NULL, 0 );
		ImmNotifyIME( m_hImcSave, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );

		m_strCompStr = _T("");
		m_strCompReadStr = _T("");
		FreeCandList();
		ClearCandidateBuf();
		m_bIsComp = false;
		g_bIsIMEKey = false;
		g_bCompClear = false;

		::ImmAssociateContext(g_hWndParent, NULL);//IME 핸들 없엠
	}
}

void CCSIME::SetNextIME()
{
	::ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );
}

void CCSIME::SetNativeIME()//자국어 입력기로 전환.
{
	if(GetLanguageType() != LT_JAPANESE)
	{
		SetDisableEnglish();
		
		SetConversionStatus(true, IME_CMODE_NATIVE, IME_SMODE_AUTOMATIC);
	}
}

void CCSIME::SetEnglishIME(bool const bIsForce)
{
	if (m_bIsForceEnglishIME == bIsForce)
	{
		return;
	}
	if(GetLanguageType() != LT_JAPANESE)
	{
		m_bIsForceEnglishIME = bIsForce;
		if (m_bIsForceEnglishIME)
		{
			SetConversionStatus(true, IME_CMODE_ALPHANUMERIC, IME_SMODE_AUTOMATIC);
		}
	}
}

void CCSIME::SetOpenStatus(bool bOpen)
{
	HIMC hIMC = NULL;
	CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);
	if(!hIMC){return;}

	::ImmSetOpenStatus(hIMC, bOpen);
}

void CCSIME::SetDisableEnglish()
{
	m_bIsForceEnglishIME = false;
}

void CCSIME::SetShapeIME()
{
	switch(GetLanguageType())
	{
	case LT_SIMPLIFIED_CHINESE:
		{
			::ImmSimulateHotKey( GetEditHWND(), IME_CHOTKEY_SHAPE_TOGGLE );
		}break;

	case LT_TRADITIONAL_CHINESE:
		{
			::ImmSimulateHotKey( GetEditHWND(), IME_THOTKEY_SHAPE_TOGGLE );
		}break;
	default:
		{
			SetConversionStatus(!IsFullShape(), IME_CMODE_FULLSHAPE);
		}break;
	}	
}

void CCSIME::SetSymbolIME()
{
	switch(GetLanguageType())
	{
	case LT_SIMPLIFIED_CHINESE:
		{
			::ImmSimulateHotKey( GetEditHWND(), IME_CHOTKEY_SYMBOL_TOGGLE );
		}break;

	case LT_TRADITIONAL_CHINESE:
		{
			::ImmSimulateHotKey( GetEditHWND(), IME_THOTKEY_SYMBOL_TOGGLE );
		}break;
	default:
		{
		}break;
	}
}

void CCSIME::SetLanguageType()
{
	const HKL hKL = ::GetKeyboardLayout(0);

	switch(LOWORD(hKL))
	{
	case LID_TRADITIONAL_CHINESE:	{m_eLanguageType = LT_TRADITIONAL_CHINESE;}break;
	case LID_JAPANESE:				{m_eLanguageType = LT_JAPANESE;}break;
	case LID_KOREAN:				{m_eLanguageType = LT_KOREAN;}break;
	case LID_SIMPLIFIED_CHINESE:	{m_eLanguageType = LT_SIMPLIFIED_CHINESE;}break;
	default:						{m_eLanguageType = LT_DEFAULT;}break;
	}
}

void CCSIME::SetConversionStatus(bool const bIsOn, DWORD const dwNewConvMode, DWORD const dwNewSentMode)
{
	DWORD dwConversion = 0, dwSentence = 0;

	HIMC hIMC = NULL;
	CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);
	if(!hIMC){return;}
	
	::ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
	
	if(dwNewConvMode || dwNewSentMode)
	{
		if(bIsOn)
		{
			if(	!(dwConversion & dwNewConvMode)
			||	!(dwNewSentMode & dwSentence))
			{
				::ImmSetConversionStatus( hIMC, dwConversion | dwNewConvMode, dwSentence | dwNewSentMode );
			}
		}
		else
		{
			if(	(dwConversion & dwNewConvMode)
			||	(dwNewSentMode & dwSentence))
			{
				::ImmSetConversionStatus( hIMC, dwConversion & ~dwNewConvMode, dwSentence & ~dwNewSentMode );
			}
		}
	}

	::ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );

	m_bIsFullShape	= (( dwConversion & IME_CMODE_FULLSHAPE )? true : false);
	m_bIsSymbolMode	= (( dwConversion & IME_CMODE_SYMBOL	)? true : false);
}
void CCSIME::FreeCandList()
{
	m_vecCandListBuf.clear();
}

void CCSIME::AllocCandList( DWORD const dwSize )
{
	FreeCandList();
	if(!dwSize)
	{
		return;
	}
	
	m_vecCandListBuf.resize(dwSize);

	m_lpCL = reinterpret_cast<LPCANDIDATELIST>( &m_vecCandListBuf.at(0) );
}

bool CCSIME::SetIMECandiDateList(HWND const hWnd)//한국용
{
	HIMC hIMC = NULL;
	CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);
	if(!hIMC){return false;}

	DWORD const dwSize = ::ImmGetCandidateList(hIMC,0x0,NULL,0);//Cand 리스트의 사이즈를 가져온다

	FreeCandList();

	AllocCandList(dwSize);//받은 만큼의 길이로 CandidateList의 버퍼를 생성.Address --> m_lpCL

	ClearCandidateBuf();//스트링의 저장용 버퍼를 비움

	DWORD i= 0;
	__int32 iCount = 0;
	LPDWORD lpdwOffset = NULL;
	if( !m_lpCL ){goto __FAIL;}

	if(!::ImmGetCandidateList(hIMC, 0x0, m_lpCL, dwSize)){	goto __FAIL;}

	lpdwOffset = &m_lpCL->dwOffset[0];

	if(GetLanguageType() == LT_JAPANESE
	|| GetLanguageType() == LT_SIMPLIFIED_CHINESE
	|| GetLanguageType() == LT_TRADITIONAL_CHINESE)
	{
		if(m_lpCL->dwPageSize)
		{
			m_lpCL->dwPageStart = m_lpCL->dwPageSize*(m_lpCL->dwSelection/m_lpCL->dwPageSize);
		}
	}
	
	lpdwOffset += m_lpCL->dwPageStart;

	for (i = m_lpCL->dwPageStart; (i < m_lpCL->dwCount) && (i < m_lpCL->dwPageStart + m_lpCL->dwPageSize) ; i++)
	{
		++iCount;
		std::wstring strTemp = (TCHAR*)( (char*)m_lpCL + *lpdwOffset);
		++lpdwOffset;
		CCandidateData CD(iCount,strTemp);
		m_vtCandlist.push_back(CD);
	}

	goto __SUCCESS;

__FAIL:
	{
//		FreeCandList();
		return false;
	}
__SUCCESS:
	{
//		FreeCandList();
		return true;
	}	
}

int CCSIME::GetCandidateIndex() const
{
	if (m_lpCL)
	{
		return m_lpCL->dwSelection - m_lpCL->dwPageStart;
	}
	return 0;
}


LRESULT CALLBACK CCSIME::OnWM_IME_NOTIFY(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{//이미 처리할것이 있었다면 이후의 기본 처리는 하지않는다.
	//return true;//이후에 기본 프로시저를 무시하라는 의미
	//return false;//이후에 기본 프로시저를 호출 하라는 의미
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096, _T("WM_IME_NOTIFY [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif

	switch(wParam)
	{
	case IMN_CLOSESTATUSWINDOW://0x0001
		{
		}break;
	case IMN_OPENSTATUSWINDOW://0x0002
		{
		}break;
	case IMN_CLOSECANDIDATE:			//0x0004 //캔디데이트가 닫힌다.
		{
			ClearCandidateBuf();
		}break;
	case IMN_CHANGECANDIDATE://0x0003
	case IMN_OPENCANDIDATE://0x0005
		{
			SetIMECandiDateList(hWnd);
#if defined(_CSIME_DEBUG)
{
		if( m_lpCL && m_lpCL->dwCount ) 
		{
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096, _T("Count[%d] PageSize[%d] PageStart[%d] Selection[%d]\n"),
				m_lpCL->dwCount, m_lpCL->dwPageSize, m_lpCL->dwPageStart, m_lpCL->dwSelection);
			CSIME_LOG(szDebug);
		}
}
#endif
			if(!IsShowCandidate())//주음 입력법 뒤에 나오는 쉬프트 리스트 아이들
			{
				return true;
			}
		}break;
	case IMN_SETCONVERSIONMODE://0x0006 문자 변환 모드로 전환. IMN_SETOPENSTATUS 다음 들어옴
		{
			SetConversionStatus();
		}break;
	case IMN_SETSENTENCEMODE://0x0007
		{
			//ImmGetConversionStatus
		}break;			
	case IMN_SETOPENSTATUS://0x0008 스테이터스 창이 열렸다.
		{
		}break;
	case IMN_SETCANDIDATEPOS:{}			//0x0009
	case IMN_SETCOMPOSITIONFONT:		//0x000A
		{
		}break;
	case IMN_SETCOMPOSITIONWINDOW:        //0x000B //컴포지션 윈도우가 뭔지 몰라도 -_-;.. 셋된대..
		{
			ClearCandidateBuf();
		}break;
	case IMN_SETSTATUSWINDOWPOS:{}		//0x000C
	case IMN_GUIDELINE://0x000D
		{
		}break;
	case IMN_PRIVATE://이것은 고칠 수가 없다
		{
#if defined(_CSIME_DEBUG)
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096, _T("IMN_PRIVATE %d, %d\n"), wParam, lParam);
			CSIME_LOG(szDebug);
#endif
		}break;
	case 269:
	case 270:
		{
#if defined(_CSIME_DEBUG)
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096, _T("IMN unkown %d, %d\n"), wParam, lParam);
			CSIME_LOG(szDebug);
#endif
		}
		break;
	default:
		{
			return false;
		}
	}
    return false;
}

LRESULT CALLBACK CCSIME::OnWM_IME_COMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096, _T("WM_IME_COMPOSITION [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif

	HIMC hIMC = NULL;
	CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);
	if(!hIMC){return 0;}

	LONG retValue = 0;
	TCHAR szTemp[MAX_BUFFER_SIZE]= {0,};

	if( (lParam & GCS_RESULTSTR)
	||	!lParam )//일본판의 경우 0으로 들어오면 Comp 를 끝낸다는 메세지다.
	{
		m_strCompStr = _T("");
		m_strCompReadStr = _T("");
		m_CaretData.iImeCursor = 0;
		g_bIsIMEKey = false;
		goto __RETURN_FALSE;
	}
	if( lParam & GCS_COMPSTR )
	{
		if(GetLanguageType() == LT_KOREAN)
		{
			goto __RETURN_FALSE;
		}

		retValue = ::ImmGetCompositionString( hIMC, GCS_COMPSTR, szTemp, MAX_BUFFER_SIZE - 1);
		if (retValue < 0)
		{
#if defined(_CSIME_DEBUG)
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096, _T("ImmGetCompositionString GCS_COMPSTR failed %d\n"), retValue);
			CSIME_LOG(szDebug);
#endif
		}
		else
		{
			m_strCompStr = szTemp;
		}

		// Get composition attribute and cursor position in composition std::wstring
		if (lParam & GCS_COMPATTR)//조합중인 구성 요소를 받아 들임.
		{
			//dwSize = ::ImmGetCompositionString(hIMC, GCS_COMPATTR, szTemp, MAX_BUFFER_SIZE - 1);
		}
		if (lParam & GCS_COMPCLAUSE)
		{
			//DWORD adwTemp[4096] = {0,};
			//::ImmGetCompositionString(hIMC, GCS_COMPCLAUSE, adwTemp, sizeof(adwTemp));
		}

		if( lParam & GCS_CURSORPOS )
		{
			retValue = ::ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
			if (retValue < 0)
			{
#if defined(_CSIME_DEBUG)
				TCHAR szDebug[4096] = {0,};
				::swprintf_s(szDebug, 4096, _T("ImmGetCompositionString GCS_CURSORPOS failed %d\n"), retValue);
				CSIME_LOG(szDebug);
#endif
			}
			else
			{
				m_CaretData.iImeCursor = 0xffff & retValue;
			}
		}

		if( lParam & GCS_DELTASTART )
		{
			retValue = ::ImmGetCompositionString( hIMC, GCS_DELTASTART, NULL, 0 );
			if (retValue < 0)
			{
#if defined(_CSIME_DEBUG)
				TCHAR szDebug[4096] = {0,};
				::swprintf_s(szDebug, 4096, _T("ImmGetCompositionString GCS_DELTASTART failed %d\n"), retValue);
				CSIME_LOG(szDebug);
#endif
			}
			else
			{
				m_CaretData.iImeDeltaStart = 0xffff & retValue;
			}
		}
	}

	if( lParam & GCS_COMPREADSTR )
	{
		memset(szTemp, 0, MAX_BUFFER_SIZE * sizeof(TCHAR));
		retValue = ::ImmGetCompositionString( hIMC, GCS_COMPREADSTR, szTemp, MAX_BUFFER_SIZE - 1);
		if (retValue < 0)
		{
#if defined(_CSIME_DEBUG)
			TCHAR szDebug[4096] = {0,};
			::swprintf_s(szDebug, 4096, _T("ImmGetCompositionString GCS_COMPSTR failed %d\n"), retValue);
			CSIME_LOG(szDebug);
#endif
		}
		else
		{
			m_strCompReadStr = szTemp;
		}
	}

	if(m_strCompReadStr == m_strCompStr)
	{
		m_strCompReadStr = _T("");
	}
	goto __RETURN_TRUE;

__RETURN_TRUE:
	{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_COMPOSITION Return TRUE\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
		return true;
	}
__RETURN_FALSE:
	{
#if defined(_CSIME_DEBUG) && !defined(EXTERNAL_RELEASE)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_COMPOSITION Return FALSE\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
		return false;
	}
}

LRESULT	CALLBACK CCSIME::OnWM_IME_SETCONTEXT(HWND const& hWnd, UINT const& message, WPARAM &wParam, LPARAM &lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_SETCONTEXT [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif

	SetLanguageType();	//050721_KCH 일본 IME 포커스 버그 수정

	lParam = 0;
	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_INPUTLANGCHANGEREQUEST(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_INPUTLANGCHANGEREQUEST [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_INPUTLANGCHANGE(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{//언어가 바뀐 정보를 받는다. 내부에서는 언어에 따른 폰트를 바꾸면 좋다.
	//wParam	Specifies the TCHARacter set of the new locale. 
	//lParam	Input locale identifier. For more information, see Languages, Locales, and Keyboard Layouts. 
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_INPUTLANGCHANGE [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
	const HKL hKL = ::GetKeyboardLayout( 0 );
	if( ::ImmIsIME( hKL ))
	{
		HIMC hIMC = NULL;
		CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);
		if(!hIMC){return false;}
		TCHAR szTemp[4096] = {0,};
		::ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, szTemp );//IME이름 가져오기
		
		SetIMEName(szTemp);//입력기 정보 셋팅
		
		SetConversionStatus();

		SetLanguageType();

		FreeCandList();
		ClearCandidateBuf();
		m_bIsComp = false;
		g_bIsIMEKey = false;
		g_bCompClear = false;
	}
	else
	{
		SetIMEName( _T("") );//이름 없음
	}
	//SetOpenStatus(false);
	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_IME_STARTCOMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096, _T("WM_IME_STARTCOMPOSITION [%08X][%08X]\n"), wParam, lParam);
    CSIME_LOG(szDebug);
}	
#endif
	m_bIsComp = true;

	if(!IsShowCandidate())//주음 입력법 조합중인 문자 나오지 않게
	{
		return true;
	}	
	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_IME_ENDCOMPOSITION(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_ENDCOMPOSITION [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif

	if (LT_TRADITIONAL_CHINESE == GetLanguageType()
	||	LT_JAPANESE == GetLanguageType()		)
	{
		g_bCompClear = true;
		//m_bIsComp = false;		
	}
	else
	{
		m_bIsComp = false;
	}
	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_IME_CONTROL(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_CONTROL [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
	return false;
}
LRESULT	CALLBACK CCSIME::OnWM_COMMAND(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
	if(m_kOnCommandCallBackFunc)
	{
		return	m_kOnCommandCallBackFunc(hWnd,message,wParam,lParam);
	}
	return	true;
}

LRESULT	CALLBACK CCSIME::OnWM_IME_CHAR(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096, _T("WM_CHAR [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif



	bool	bCancel = false;
//	int const nVirtKey = ( int )wParam;	// virtual-key code
	int const nVirtKey = LOWORD(wParam);	// virtual-key code kch bug 수정

	if(IsMultiLine() && nVirtKey != VK_BACK)
	{
		std::wstring str = _T("");
		GetWindowText(str);
		
		if( !IsCanInputAtMultiLine(str, nVirtKey) )
		{
			bCancel = true;
		}
	}

	bool bIsOk = false;
	if('0' <= nVirtKey && '9' >= nVirtKey ){bIsOk = true;}
	if('A' <= nVirtKey && 'Z' >= nVirtKey ){bIsOk = true;}
	if('a' <= nVirtKey && 'z' >= nVirtKey ){bIsOk = true;}
	if( VK_BACK == nVirtKey ){bIsOk = true; }
	if( VK_HOME == nVirtKey ){bIsOk = true; }
	if( VK_END  == nVirtKey ){bIsOk = true; }
	if( VK_INSERT  == nVirtKey ){bIsOk = true; }
	if( VK_DELETE  == nVirtKey ){bIsOk = true; }
	
	if(m_bIsForceEnglishIME && !bIsOk)
	{
		bCancel = true;
	}

	switch( nVirtKey )//엔터나 탭 치면 삑삑 소리나는거 방지
	{
	case VK_RETURN: 
		{
			bCancel = bCancel || (IsMultiLine()==false);
		}break;
	case VK_TAB: 
		{
			bCancel = true;
		}
		break;
	}

	if(bCancel)
	{
		return	true;
	}

	if(m_kOnImeCharCallBackFunc)
	{
		if(m_kOnImeCharCallBackFunc(hWnd,message,wParam,lParam))
		{
			return	true;
		}
	}

	return false;
}

LRESULT	CALLBACK CCSIME::OnWM_IME_SELECT(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
#if defined(_CSIME_DEBUG)
{
	TCHAR szDebug[4096] = {0,};
	::swprintf_s(szDebug, 4096,_T("WM_IME_SELECT [%08X][%08X]\n"), wParam,lParam);
    CSIME_LOG(szDebug);
}	
#endif
	return false;
}

bool CALLBACK CCSIME::HandleMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam)
{//return false 면 그냥 본편이 처리
	switch( message )//(WINVER >= 0x0400)
	{
	case	WM_COMMAND:
		{
			if(OnWM_COMMAND( hWnd, message, wParam, lParam)){return true;}
		}
		break;
	case WM_KEYDOWN:
		{
			if (IsNowComp() == false)
			{
				int const exec_ret = Execute( (int)wParam, KEY_STATE_DOWN);//false 면 parent 로 간다

				if (exec_ret)//키다운에서 Setstring을 물리면 조합중인 글자가 남게 되는 현상이 생기는데 그걸 해결 할 수 없다.
				{
	#if defined(_CSIME_DEBUG)
					if(exec_ret)
					{
						TCHAR szDebug[4096] = {0,};
						::swprintf_s(szDebug, 4096, _T("HandleMessage Execute Called\n"));
						 CSIME_LOG(szDebug);
					}
	#endif
					return true;
				}
			}

			const LRESULT ime_ret = OnWM_IME_CHAR( hWnd, message, wParam, lParam);
			if(ime_ret)
			{
				return true;
			}
		}break;
	case WM_KEYUP:
		{
			if (IsNowComp() == false && g_bIsIMEKey == false)
			{
				if( Execute( (int)wParam, KEY_STATE_UP) )
				{
					return true;
				}
			}
			g_bIsIMEKey = false;
		}break;
	case WM_CHAR:
		{
//			if(OnWM_CHAR( hWnd, message, wParam, lParam)){return true;}
			return false;
		}break;
    case WM_IME_SETCONTEXT:
		{
			if(OnWM_IME_SETCONTEXT( hWnd, message, wParam, lParam)){return true;}
		}break;
	case WM_IME_NOTIFY:
		{
			if(OnWM_IME_NOTIFY( hWnd, message, wParam, lParam)){return true;}
			return true;
		}break;
	case WM_IME_CONTROL:
		{
			if(OnWM_IME_CONTROL( hWnd, message, wParam, lParam)){return true;}
		}break;
	case WM_IME_COMPOSITIONFULL:
		{
		}break;
	case WM_IME_SELECT:
		{
			if(OnWM_IME_SELECT( hWnd, message, wParam, lParam)){return true;}
			return true;
		}break;
	case WM_IME_CHAR:
		{
			//if(OnWM_IME_CHAR( hWnd, message, wParam, lParam)){return true;}
			return true;
		}break;
	case WM_IME_REQUEST:
		{
		}break;
	case WM_IME_KEYDOWN:
		{
			return true;
		}break;
	case WM_IME_KEYUP:
		{
			return true;
		}break;
	case WM_IME_STARTCOMPOSITION://주음입력
		{
			if(OnWM_IME_STARTCOMPOSITION( hWnd, message, wParam, lParam)){return true;}
		}break;
	case WM_IME_ENDCOMPOSITION:
		{
			if(OnWM_IME_ENDCOMPOSITION( hWnd, message, wParam, lParam)){return true;}
		}break;
	case WM_IME_COMPOSITION ://조합중인 문자의 정보를 받을 수 있다.
		{
			if(OnWM_IME_COMPOSITION( hWnd, message, wParam, lParam)){return true;}
		}break;
	case WM_INPUTLANGCHANGEREQUEST:
		{
			if(OnWM_INPUTLANGCHANGEREQUEST( hWnd, message, wParam, lParam)){return true;}
			return true;
		}break;
	case WM_INPUTLANGCHANGE:
		{
			if(OnWM_INPUTLANGCHANGE(hWnd, message, wParam, lParam)){return true;}
			return true;
		}break;
	default:
		{
			return false;
		}break;
	}
	return false;//기본 false -> parent 가 처리 한다는얘기.
}

bool CALLBACK CCSIME::HookMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam, bool const bDontPost/* 키 펑션만 실행 하려 할때 */, bool& bSkipDefProc)
{
	SetTextChanged();
	SetCaretPosChanged();

	bool bProcess = false;
	switch( message )
	{
	case WM_KEYDOWN://백스페이스 같은 특수키 처리해줘야됨
		if (g_bCompClear)
		{
			m_bIsComp = false;
			g_bCompClear = false;
		}

		bProcess = true;
		if (wParam == VK_PROCESSKEY)
			g_bIsIMEKey = true;
		break;
	case WM_KEYUP:
		//g_bIsIMEKey = false; //HandleMessgae 안에서 false로 변경하도록 수정. Execute 피해가려고;
		bProcess = true;
		break;
	case WM_CHAR:
		if (g_bIsIMEKey == false)
		{
			return false;
		}
		else
		{
			g_bIsIMEKey = false;
			if (m_bIsComp == false && (GetLanguageType() == LT_JAPANESE || GetLanguageType() == LT_KOREAN))
			{
				return false;
			}
		}
		bProcess = true;
		break;
	case WM_IME_CHAR:
		bProcess = false;
		bSkipDefProc = true;	// WM_IME_CHAR를 DefWinProc에 전달하면 WM_CHAR가 또 오게 된다. 그래서 Skip함.
		break;
	case WM_IME_SETCONTEXT:
	case WM_IME_NOTIFY:
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_SELECT:
	case WM_IME_REQUEST:
	case WM_IME_KEYDOWN:
	case WM_IME_KEYUP:
	case WM_IME_STARTCOMPOSITION://주음입력
	case WM_IME_ENDCOMPOSITION://== WM_IME_KEYLAST == WM_IME_COMPOSITION
	case WM_IME_COMPOSITION ://조합중인 문자의 정보를 받을 수 있다.
	case WM_INPUTLANGCHANGEREQUEST:
	case WM_INPUTLANGCHANGE:
		bProcess = true;
		break;
	case	WM_COMMAND:
		{
			bProcess = true;
		}
		break;

	default:
		{
			return false;
		}break;
	}

	if (bProcess)
	{
		bool bHandleRet = HandleMessage(hWnd, message, wParam, lParam );
		if( !bHandleRet )//내가 처리하고 온것은
		{
			if(!bDontPost)
			{
#if defined(_CSIME_DEBUG)
				{
					UINT iStart, iEnd, iLimit;
					std::wstring text;
					text.resize(4096);
					::GetWindowText(GetEditHWND(), &text.at(0), 4096);
					iLimit = ::SendMessage(GetEditHWND(), EM_GETLIMITTEXT,  0, 0);
					::SendMessage(GetEditHWND(), EM_GETSEL,  (WPARAM)&iStart, (LPARAM)&iEnd);
					TCHAR szDebug[4096] = {0,};
					::swprintf_s(szDebug, 4096,_T("CCSIME::before post message %#X message(%d,%d) caret:%d,%d, text:%s(%d) \n"), message, wParam, lParam, iStart, iEnd, text.c_str(), iLimit);
					CSIME_LOG(szDebug);
				}
#endif

				::PostMessage(m_hEWnd, message, wParam, lParam);
#if defined(_CSIME_DEBUG)
				{
					UINT iStart, iEnd, iLimit;
					std::wstring text;
					text.resize(4096);
					::GetWindowText(GetEditHWND(), &text.at(0), 4096);
					iLimit = ::SendMessage(GetEditHWND(), EM_GETLIMITTEXT,  0, 0);
					::SendMessage(GetEditHWND(), EM_GETSEL,  (WPARAM)&iStart, (LPARAM)&iEnd);
					TCHAR szDebug[4096] = {0,};
					::swprintf_s(szDebug, 4096,_T("CCSIME::after post message %#X message(%d,%d) caret:%d,%d, text:%s \n"), message, wParam, lParam, iStart, iEnd, text.c_str());
					CSIME_LOG(szDebug);
				}
#endif
			}
		}
	}
	return true;
}

bool CCSIME::FuncBind(__int32 const iIndex, LPKBFUNC const lpFunc)
{
	const KB_HASH::iterator iter = m_mapKBHash.find(iIndex); 

	if( m_mapKBHash.end() != iter )
	{
		return false;
	}

	m_mapKBHash.insert( KB_HASH::value_type(iIndex,lpFunc) );
	return true;
}

__int32 CCSIME::Execute(__int32 const iIndex, eKeyState const KeyState)
{
	const KB_HASH::iterator iter = m_mapKBHash.find(iIndex); 
	if( m_mapKBHash.end() == iter )
	{
		return 0;
	}
	return (*iter->second)(iIndex, this, KeyState);
}

bool CCSIME::CorrectAlphaNumeric(std::wstring &str)
{
	std::wstring::iterator itor = str.begin();

	bool bIsChange = false;
	while( str.end() != itor )
	{
		const TCHAR value = (*itor);
		/*		if( 0x80 & value )	//멀티바이트 일때 한글 검사
		{
		itor = str.erase(itor);//2글자 삭제
		if( str.end() != itor)
		{
		itor = str.erase(itor);
		}
		bIsChange = true;
		continue;
		}*/
		bool bIsOk = false;
		if(iswdigit((int)value))			{bIsOk = true;}
		if(L'A' <= value && L'Z' >= value )	{bIsOk = true;}	//Isalpha가 한글 한자를 통과시킴;
		if(L'a' <= value && L'z' >= value )	{bIsOk = true;}
		if (IsMultiLine())	//멀티라인 일 때만 개행문자 통과
		{
			if(L'\n' == value || L'\r' >= value ){bIsOk = true;}
		}
		if(!bIsOk)
		{
			itor = str.erase(itor);
			bIsChange = true;
			continue;
		}
		++itor;
	}

	return bIsChange;
}

void CCSIME::SetEnableIME(bool const bIsEnable, HWND hWnd, HIMC &hIMC)
{
	if(bIsEnable)
	{
		if(hIMC)
		{
			::ImmAssociateContext(hWnd, hIMC);
			hIMC = NULL;
		}
	}
	else
	{
		HIMC hIMC = NULL;
		CSafeIMEContext SafeIC( hWnd, hIMC, m_kMutex);
		if(!hIMC){return;}

		const HKL hKL = ::GetKeyboardLayout( 0 );
		if( ::ImmIsIME( hKL ))
		{
			TCHAR szTemp[MAX_BUFFER_SIZE]= {0,};
			::ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, szTemp );//IME이름 가져오기
			SetIMEName(szTemp);//입력기 정보 셋팅
		}

		ImmSetCompositionString( hIMC, SCS_SETSTR, NULL, 0 , NULL, 0 );
		ImmSetCompositionString( hIMC, SCS_CHANGECLAUSE, NULL, 0 , NULL, 0 );
		ImmSetCompositionString( hIMC, SCS_CHANGEATTR, NULL, 0 , NULL, 0 );
		ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0 );

		m_strCompStr = _T("");
		m_strCompReadStr = _T("");
		ClearCandidateBuf();
		m_bIsComp = false;
		g_bIsIMEKey = false;
		g_bCompClear = false;

		::ImmAssociateContext(hWnd, NULL);//IME 핸들 없엠
	}
}

DWORD const CCSIME::GetIME_CMODE()
{
	HIMC hIMC = NULL;

	CSafeIMEContext SafeIC(GetEditHWND(), hIMC, m_kMutex);

	if(!hIMC){return -1;} //0이면 영문

	DWORD Con, Sen;
	ImmGetConversionStatus(hIMC, &Con, &Sen);
	
	return Con;
}

LRESULT CALLBACK CCSIME::EditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//check for key press
	switch (uMsg)
	{
	case WM_IME_SETCONTEXT:
		lParam = 0;
		break;
	case WM_SETFOCUS:
		return 0;
	}

	//let WindowProc handle all other messages
	if (m_oldEditWndProc) {
		return CallWindowProc (m_oldEditWndProc, hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc (hWnd, uMsg, wParam, lParam);
	}
}
