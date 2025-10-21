#include "stdafx.h"

#define DIRECTINPUT_VERSION (0x0800)

#include "../../../cosmos/DirectXSDK/Include/dinput.h"
#include "XUI_Config.h"
#include "XUI_Manager.h"
#include "CSIME/CSIME.h"
#include "XUI_Edit.h"

extern CS::CCSIME g_kSingleIME;
extern CS::CCSIME g_kMultiIME;

using namespace XUI;

CXUI_Edit::CONT_EDIT_GROUP	CXUI_Edit::m_kEditGroupCont = CXUI_Edit::CONT_EDIT_GROUP();

CXUI_Edit::PgEditFocusGroup::PgEditFocusGroup(ID_TYPE const& kGrandParentID)
	:m_kParentID(kGrandParentID)
{
	m_kContEditChild.clear();
}

CXUI_Edit::PgEditFocusGroup::~PgEditFocusGroup(void)
{
	m_kContEditChild.clear();
}

bool CXUI_Edit::PgEditFocusGroup::AddControl(int const iOrderLank, XUI::CXUI_Edit* pkEdit)
{
	if( !pkEdit )
	{
		return false;
	}

	int OrderLank = iOrderLank;
	if( 0 == OrderLank )
	{
		OrderLank = static_cast<int>(m_kContEditChild.size());
	}

	DelControl(pkEdit);
	m_kContEditChild.push_back( SEditInfo( OrderLank, pkEdit ) );
	m_kContEditChild.sort();
	return true;
}

bool CXUI_Edit::PgEditFocusGroup::AddControl(XUI::CXUI_Edit* pkEdit)
{
	return this->AddControl(0, pkEdit);
}

bool CXUI_Edit::PgEditFocusGroup::DelControl(XUI::CXUI_Edit* pkEdit)
{
	CONT_EDIT_CHILD::iterator edit_itor = m_kContEditChild.begin();
	while( edit_itor != m_kContEditChild.end() )
	{
		CONT_EDIT_CHILD::value_type& kEditInfo = (*edit_itor);
		if( kEditInfo.pkEdit == pkEdit )
		{
			m_kContEditChild.erase(edit_itor);
			return true;
		}
		++edit_itor;
	}
	return false;
}

void CXUI_Edit::PgEditFocusGroup::FocusPassNextEdit(CXUI_Edit* pkEdit)
{
	if( !pkEdit )
	{
		return;
	}

	if( 1 >= m_kContEditChild.size() )
	{
		return;
	}

	CONT_EDIT_CHILD::iterator edit_itor = m_kContEditChild.begin();
	while( edit_itor != m_kContEditChild.end() )
	{
		CONT_EDIT_CHILD::value_type& pkGroupEdit = (*edit_itor);
		if( pkGroupEdit.pkEdit == pkEdit )
		{
			++edit_itor;
			while( true )
			{
				if( edit_itor != m_kContEditChild.end() )
				{
					CONT_EDIT_CHILD::value_type& pkNextEdit = (*edit_itor);
					if( pkNextEdit.pkEdit == pkEdit )
					{
						return;
					}
					else
					{
						if( pkNextEdit.pkEdit == NULL || false == pkNextEdit.pkEdit->Visible() )
						{
							++edit_itor;
							continue;
						}

						pkEdit->VLoseFocus();
						pkNextEdit.pkEdit->SetEditFocus(true);
						return;
					}
				}
				else
				{
					edit_itor = m_kContEditChild.begin();
				}
			}
		}
		++edit_itor;
	}
}

bool CXUI_Edit::PgEditFocusGroup::operator == (XUI::CXUI_Wnd* rhs) const
{
	if( !rhs )
	{
		return false;
	}

	if( rhs->Parent() == NULL )
	{
		return this->operator == ( rhs->ID() );
	}
	else
	{
		XUI::CXUI_Wnd* pkParent = rhs->Parent();
		while( NULL != pkParent->Parent() )
		{
			pkParent = pkParent->Parent();
		}

		return this->operator == ( pkParent->ID() );
	}
	return false;
}

bool CXUI_Edit::PgEditFocusGroup::operator == (ID_TYPE const& rhs) const
{
	return ( m_kParentID == rhs );
}

CXUI_Edit* CXUI_Edit::m_spFocusedEdit = NULL;
void* CXUI_Edit::m_spTextBlockBgImg = NULL;
void* CXUI_Edit::m_spCarotImg = NULL;
int CXUI_Edit::m_siCarotImgIdx = -1;
int CXUI_Edit::m_siBlockImgIdx = -2;

CXUI_Edit::CXUI_Edit(void)
{
	m_wstrInputText = _T("");
	PreviewText(_T("PreviewText") );
	m_iEndTextPos = 0;
	m_iStartTextPos = 0;
	m_wstrRealText.clear();
	m_p2DString = NULL;
	m_iPastCarotPos = 0;
	IsSecret(false);
	IsEditDisable(false);
	IsOnlyNum(0);
	m_iLimitLength = 50;
	IsMultiLine(false);
	m_kVecLine.clear();
	IsNativeIME(false);
	m_kOrderLank = 0;

	CarotBlinkTime(0);
	CarotBlink(false);
	NoWordWrap(true);
	LastAddCount(-1);
}

CXUI_Edit::~CXUI_Edit(void)
{
	RemoveAllTextBlock();
	SAFE_DELETE(m_p2DString);
}

void CXUI_Edit::VInit()
{
	CXUI_Wnd::VInit();

	m_ImgIdx = -1;
	m_kVecLine.clear();
}

void CXUI_Edit::VOnClose()
{	
	CXUI_Edit::DelGroupEdit(this);
	CXUI_Wnd::VOnClose();
	if( CXUI_Edit::GetFocusedEdit() == this )
	{
		CXUI_Edit::SetFocusedEdit(NULL);
	}

	m_iEndTextPos = 0;
	m_iStartTextPos = 0;

	SAFE_DELETE(m_p2DString);
}

void CXUI_Edit::Visible(bool const& bIsVisible)
{
	CXUI_Wnd::Visible(bIsVisible);
}
bool CXUI_Edit::Visible()
{
	XUI::CXUI_Wnd* pkParent = Parent();
	while( pkParent != NULL )
	{
		if( false == pkParent->Visible() )
		{
			return false;
		}
		else
		{
			if( NULL != pkParent->Parent() )
			{
				pkParent = pkParent->Parent();
			}
			else
			{
				break;
			}
		}
	}
	return this->CXUI_Wnd::Visible();
}
void CXUI_Edit::EditFont(std::wstring const& value)
{
	m_EditFont = value;
	Font(m_EditFont);
}
void CXUI_Edit::EditTextPos(POINT2 const& value)
{
	m_ptEditTextPos = value;
	TextPos(value);
}
bool	CALLBACK	CXUI_Edit::OnCommandCallBack(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
	switch(HIWORD(wParam))
	{
	case	EN_MAXTEXT:
		{
			CXUI_Edit*	pkEdit = dynamic_cast<CXUI_Edit*>(XUIMgr.GetFocusedEdit());
			if(!pkEdit)
			{
				return	true;
			}
			CXUI_Edit_MultiLine*	pkEditMulti = dynamic_cast<CXUI_Edit_MultiLine*>(pkEdit);
			if(pkEditMulti)
			{
				return	true;
			}

			TextBlockCont	&kTextBlocks = pkEdit->GetTextBlocks();
			if(kTextBlocks.size() == 0)
			{
				return	true;
			}

			CS::CARETDATA	kCarotPos = g_kSingleIME.GetCaretPos();

			for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); ++itor)
			{
				stTextBlock	&kTextBlock = *itor;
				//	텍스트 블럭 안에서는 글자를 쓸 수 없음.
				if(kCarotPos.iCaretPos>kTextBlock.m_iPos && kCarotPos.iCaretPos<(kTextBlock.m_iPos+kTextBlock.m_kText.length()))
				{
					return	true;
				}
				if(kTextBlock.m_iPos >= kCarotPos.iCaretPos)
				{
					kTextBlock.m_iPos -= pkEdit->LastAddCount();
				}
			}

			pkEdit->LastAddCount(0);
		}
		break;
	}
	return	true;
}
bool	CALLBACK	CXUI_Edit::OnImeCharCallBack(HWND const& hWnd, UINT const& message, WPARAM const& wParam, LPARAM const& lParam)
{
	CXUI_Edit*	pkEdit = dynamic_cast<CXUI_Edit*>(XUIMgr.GetFocusedEdit());
	if(!pkEdit)
	{
		return	false;
	}
	CXUI_Edit_MultiLine*	pkEditMulti = dynamic_cast<CXUI_Edit_MultiLine*>(pkEdit);
	if(pkEditMulti)
	{
		return	false;
	}

	TextBlockCont	&kTextBlocks = pkEdit->GetTextBlocks();
	if(kTextBlocks.size() == 0)
	{
		return	false;
	}

	int nVirtKey = LOWORD(wParam);
	BYTE	byScanCode = (BYTE)((lParam&0xff0000)>>16);

	int	iRepeatCount = (lParam&0xff);

	nVirtKey = GetVirtualKeyByScanCode(byScanCode,nVirtKey);

	bool	bIsChar = (nVirtKey>=0x30 && nVirtKey<=0x5A) ||
		nVirtKey == VK_MULTIPLY ||
		nVirtKey == VK_ADD ||
		nVirtKey == VK_SEPARATOR ||
		nVirtKey == VK_SUBTRACT ||
		nVirtKey == VK_DECIMAL ||
		nVirtKey == VK_DIVIDE ||
		nVirtKey == VK_OEM_PLUS ||
		nVirtKey == VK_OEM_COMMA ||
		nVirtKey == VK_OEM_MINUS ||
		nVirtKey == VK_OEM_PERIOD ||
		nVirtKey == VK_OEM_1 ||
		nVirtKey == VK_OEM_2 ||
		nVirtKey == VK_OEM_3 ||
		nVirtKey == VK_OEM_4 ||
		nVirtKey == VK_OEM_5 ||
		nVirtKey == VK_OEM_6 ||
		nVirtKey == VK_OEM_7 ||
		nVirtKey == VK_OEM_102 ||
		nVirtKey == VK_SPACE;

	bool	bIsPaste = false;
	if(nVirtKey == 'V')
	{
		if((GetAsyncKeyState(VK_LCONTROL)&0x8000)!=0)
		{
			bIsPaste = true;
		}
	}

	if(bIsPaste)
	{
		return	true;
	}

	if(nVirtKey == 'C')
	{
		if((GetAsyncKeyState(VK_LCONTROL)&0x8000)!=0)
		{
			return	false;
		}
	}

	CS::CARETDATA	kCarotPos = g_kSingleIME.GetCaretPos();

	if(nVirtKey == VK_DELETE)
	{
		int	iArrageLen = -1;

		if(abs(kCarotPos.iSelectStart - kCarotPos.iSelectEnd)>0)
		{
			return	true;
		}

		for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); )
		{
			stTextBlock	&kTextBlock = *itor;
			if(iArrageLen > 0)
			{
				kTextBlock.m_iPos -= iArrageLen;
			}
			else
			{
 				if(kCarotPos.iCaretPos>=(kTextBlock.m_iPos-(iRepeatCount-1)) && kCarotPos.iCaretPos<(kTextBlock.m_iPos+kTextBlock.m_kText.length()))
				{
					iArrageLen = kTextBlock.m_kText.length();

					std::wstring	const	&kOrgText = pkEdit->EditText();
					std::wstring	kNewText = kOrgText.substr(0,kTextBlock.m_iPos)+kOrgText.substr(kTextBlock.m_iPos+kTextBlock.m_kText.length(),kOrgText.length() - kTextBlock.m_iPos+kTextBlock.m_kText.length());

					pkEdit->EditText(kNewText,true);
					g_kSingleIME.SetCaretPos(kCarotPos.iCaretPos,kCarotPos.iCaretPos);

					itor = kTextBlocks.erase(itor);
					continue;
				}
			}

			++itor; 
		}

		if(iArrageLen > 0)
		{
			return	true;
		}

		int	const iBackCount = iRepeatCount;
		for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); ++itor)
		{
			stTextBlock	&kTextBlock = *itor;
			if(kCarotPos.iCaretPos<=kTextBlock.m_iPos)
			{
				kTextBlock.m_iPos -= iBackCount;
			}
		}
		return	false;
	}
	else if(nVirtKey == VK_BACK)
	{
		int	iArrageLen = -1;
		if(abs(kCarotPos.iSelectStart - kCarotPos.iSelectEnd)>0)
		{
			return	true;
		}

		if(kCarotPos.iCaretPos == 0)
		{
			return	true;
		}

		for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); )
		{
			stTextBlock	&kTextBlock = *itor;
			if(iArrageLen > 0)
			{
				kTextBlock.m_iPos -= iArrageLen;
			}
			else
			{
				if(kCarotPos.iCaretPos>kTextBlock.m_iPos && kCarotPos.iCaretPos<(kTextBlock.m_iPos+kTextBlock.m_kText.length()+1+(iRepeatCount-1)))
				{
					iArrageLen = kTextBlock.m_kText.length();

					std::wstring	const	&kOrgText = pkEdit->EditText();
					std::wstring	kNewText = kOrgText.substr(0,kTextBlock.m_iPos)+kOrgText.substr(kTextBlock.m_iPos+kTextBlock.m_kText.length(),kOrgText.length() - kTextBlock.m_iPos+kTextBlock.m_kText.length());

					pkEdit->EditText(kNewText,true);
					g_kSingleIME.SetCaretPos(kTextBlock.m_iPos,kTextBlock.m_iPos);

					itor = kTextBlocks.erase(itor);
					continue;
				}
			}

			++itor;
		}

		if(iArrageLen > 0)
		{
			return	true;
		}		

		int	const iBackCount = (iRepeatCount<=kCarotPos.iCaretPos) ? iRepeatCount : kCarotPos.iCaretPos;
		for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); ++itor)
		{
			stTextBlock	&kTextBlock = *itor;
			if(kCarotPos.iCaretPos<=kTextBlock.m_iPos)
			{
				kTextBlock.m_iPos -= iBackCount;
			}
		}

		return	false;
	}

	if(bIsChar)
	{
		if(abs(kCarotPos.iSelectStart - kCarotPos.iSelectEnd)>0)
		{
			return	true;
		}

		int	const	iLimitLength = pkEdit->LimitLength();
		int	const	iTextLength = pkEdit->EditText().length();

		int	iAddCount = iRepeatCount;

		if(iTextLength + iAddCount >iLimitLength)
		{
			iAddCount = iLimitLength - iTextLength;
		}
		if(iAddCount<=0)
		{
			return	true;
		}

		int	iArrageLen = -1;
		pkEdit->LastAddCount(iAddCount);

		for(TextBlockCont::iterator itor = kTextBlocks.begin(); itor != kTextBlocks.end(); ++itor)
		{
			stTextBlock	&kTextBlock = *itor;
			//	텍스트 블럭 안에서는 글자를 쓸 수 없음.
			if(kCarotPos.iCaretPos>kTextBlock.m_iPos && kCarotPos.iCaretPos<(kTextBlock.m_iPos+kTextBlock.m_kText.length()))
			{
				return	true;
			}
			if(kTextBlock.m_iPos >= kCarotPos.iCaretPos)
			{
				kTextBlock.m_iPos += iAddCount;
			}
		}
		return	false;
	}

	if(nVirtKey != VK_LEFT &&
		nVirtKey != VK_RIGHT &&
		nVirtKey != VK_UP &&
		nVirtKey != VK_DOWN &&
		nVirtKey != VK_HOME &&
		nVirtKey != VK_CAPITAL &&
		nVirtKey != VK_RETURN &&
		nVirtKey != VK_TAB &&
		nVirtKey != VK_ESCAPE  &&
		nVirtKey != VK_PRIOR &&
		nVirtKey != VK_NEXT &&
		nVirtKey != VK_END &&
		nVirtKey != VK_LWIN &&
		nVirtKey != VK_RWIN &&
		nVirtKey != VK_CONTROL &&
		nVirtKey != VK_LCONTROL )
	{
		return	true;
	}

	return	false;
}
unsigned	int	CXUI_Edit::GetVirtualKeyByScanCode(BYTE byScanCode,unsigned int iDefaultKeyID)
{
	if(byScanCode>=88)
	{
		return	iDefaultKeyID;
	}

	if(iDefaultKeyID>=VK_NUMPAD0 && iDefaultKeyID<=VK_DIVIDE)
	{
		return	iDefaultKeyID;
	}

	int iKeyID[]=
	{VK_ESCAPE,'1','2','3','4','5','6','7','8','9','0',-1,-1,VK_BACK,VK_TAB,'Q','W','E','R','T','Y','U','I','O','P',-1,-1,VK_RETURN,-1,
	'A','S','D','F','G','H','J','K','L',-1,-1,-1,-1,-1,'Z','X','C','V','B','N','M',-1,-1,-1,-1,-1,-1,VK_SPACE,-1,VK_F1,VK_F2,VK_F3,
	VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,-1,-1,VK_HOME,VK_UP,VK_PRIOR,-1,VK_LEFT,VK_CLEAR,VK_RIGHT,
	-1,VK_END,VK_DOWN,VK_NEXT,VK_INSERT,VK_DELETE,-1,-1,-1,VK_F11,VK_F12};

	if(iKeyID[byScanCode-1]==-1) return iDefaultKeyID;
	return	iKeyID[byScanCode-1];
}

void	CXUI_Edit::AddTextBlock(int iPos,std::wstring const &kText,int iExtraDataLen,BYTE *pkExtraData,DWORD dwTextColor)
{
	if(iExtraDataLen <= 0)
	{
		return;
	}

	stTextBlock kNewBlock(static_cast<int>(iPos),kText,dwTextColor,iExtraDataLen,pkExtraData);

	for(TextBlockCont::iterator itor = m_kTextBlocks.begin(); itor != m_kTextBlocks.end(); ++itor)
	{
		stTextBlock	&kTextBlock = *itor;
		if(kTextBlock.m_iPos>iPos)
		{
			m_kTextBlocks.insert(itor,kNewBlock);
			return;
		}
	}
	m_kTextBlocks.push_back(kNewBlock);
}

//캐럿 출력을 위해 수정 by 하승봉
bool CXUI_Edit::VDisplay()
{
	//디버그 정보 표시용
	/*std::wstring kInput;// g_kSingleIME.GetWindowText(kInput);
	std::wstring kResult = g_kSingleIME.GetResultStr();
	std::wstring kComp = g_kSingleIME.GetCompStr();
	std::wstring kCompRead = g_kSingleIME.GetCompReadStr();
	TCHAR szTemp[255] = {0, };
	_stprintf_s(szTemp, 255, _T("%d --VDisplay ID:[%s] Input:[%s] past:[%s] Real:[%s] RealTxt:[%s]------IME WindowText[%s] Result[%s] Composition[%s] CompRead[%s]\n"),
		BM::GetTime32(), m_ID.c_str(), m_wstrInputText.c_str(), m_wstrPastInputText.c_str(), m_wstrRealString.c_str(), m_wstrRealText.c_str()
		, m_wstrInputText.c_str(), kResult.c_str(), kComp.c_str(), kCompRead.c_str());
	OutputDebugStr(szTemp);*/

	TextPos(EditTextPos());

	DWORD	dwFontFlagSave = FontFlag();

	bool	bOutLine = dwFontFlagSave&XTF_OUTLINE;
	if(!bOutLine && m_kTextBlocks.size()>0)
	{
		FontFlag(dwFontFlagSave|XTF_OUTLINE);
	}

	if(!CXUI_Wnd::VDisplay() ){return false;}

	FontFlag(dwFontFlagSave);

	CS::CARETDATA Data = g_kSingleIME.GetCaretPos();
	
	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	if(GetFocusedEdit() == this && m_spWndMouseFocus == this)	//	현재 에디트 중이라면,
	{
		m_wstrInputText = g_kSingleIME.GetResultStr();
		std::wstring szRealText;//(m_wstrInputText.size(), 0);
		int const iLine = MakeEditString(szRealText, Data.iCaretPos);

		RenderBlock(szRealText);//글자 블럭 출력

		if(CarotBlink())
		{
			POINT2 kCaretPos = CalcCaretPos(szRealText, Data, pFont);
			RenderCarot(kCaretPos);
		}
	}

	return true;
}

bool CXUI_Edit::IsFocus()const
{
	return (GetFocusedEdit() == this);
}

bool CXUI_Edit::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !CXUI_Wnd::Visible() || IsClosed() ){return false;}//자식도 안함.
	if( !Enable() ){return false;}

	//XUI_Edit만 특별 메시지 처리를 한다.
	bool bRet = false;
	m_bDBLClick = false;
	
#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	unsigned int wstrScriptKey = SCRIPT_MAX_NUM;
#else
	std::wstring wstrScriptKey;
#endif

	switch(rET)
	{
	case IEI_KEY_DOWN:
	//case IEI_KEY_UP:
		{//컨트롤에 전달 되어야함. dxxx todo 
			if(IsFocus())
			{
				if (g_kSingleIME.IsNowComp() == false)
				{
					BM::vstring vstr(dwValue);
					bRet = DoHotKey(vstr);
				}
				return true;//뭐가 어떻게 되던 포커스를 가진놈이 인풋을 먹어치워야 함
			}
			/*BM::vstring vstr = (int)dwValue;
			bRet = DoHotKey(vstr);
			if(!bRet)
			{
				return false;
			}*/
		}break;
	case IEI_MS_DOWN:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				m_spWndMouseOver = this;//Edit 전용 추가

				if(MEI_BTN_0 == dwValue)
				{
					IsMouseDown(true);
					VAcquireFocus(this);

					static DWORD dwLastDownTime = 0;
					DWORD const dwNow = BM::GetTime32();
					
					if( LastMouseDownPos()-XUIMgr.DblClickBound()<=m_sMousePos && LastMouseDownPos()+XUIMgr.DblClickBound()>=m_sMousePos )
					{//같은 좌표에서.
						if(dwLastDownTime)
						{
							//있었고 //일정 시간 내에 들어오면.
							if((dwNow - dwLastDownTime) < XUIMgr.DblClickTick())//200 밀리 이하로 들어왔다..
							{
								wstrScriptKey = SCRIPT_ON_L_BTN_DBL_DOWN;
								dwLastDownTime = 0;//이벤트후 시간은 0으로 만들어야하고.
								m_bDBLClick = true;

								bRet = true;
							}
						}
					}

					dwLastDownTime = dwNow;//뭐가 됐든 마지막 다운시간은 기록

					wstrScriptKey = SCRIPT_ON_L_BTN_DOWN;
					LastMouseDownPos(m_sMousePos);

					if(m_spWndMouseOver==this && GetFocusedEdit()!=this)//>>Edit 전용 추가
					{
						if(!m_wstrInputText.empty())//입력된 문자열이 있을 경우 초기화 시키면 안됨
						{
							g_kSingleIME.SetString(m_wstrInputText);
							g_kSingleIME.SetCaretPos(false);//캐럿을 맨 뒤로
							SetEditFocus(true);
						}
						else
						{
							SetEditFocus(false);//무조건 입력된 문자가 클리어 되므로 분리함
						}
					}
					else if(m_spWndMouseOver==this && GetFocusedEdit()==this)//포커스 먹은 상태에서 다시 왼쪽 클릭
					{
						int pos = GetClickTextPos();
						//MoveCarotToClickPos(pos, pos);
						if(m_bDBLClick)//포커스 먹은 상태에서 더블클릭되면 모두 블럭지정
						{
							g_kSingleIME.SetCaretPos(0, (int)m_wstrInputText.length());
						}
						else
						{
							g_kSingleIME.SetCaretPos(pos, pos);
						}

					}//<<Edit 전용 추가
				}
				else if( MEI_BTN_1 == dwValue )
				{ 
					wstrScriptKey = SCRIPT_ON_R_BTN_DOWN;
				}
				bRet = true;
			}
		}break;
	case IEI_MS_UP:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				if(MEI_BTN_0 == dwValue)
				{
					IsMouseDown(false);	
					wstrScriptKey = SCRIPT_ON_L_BTN_UP; 
				}
				else if( MEI_BTN_1 == dwValue )
				{ 
					wstrScriptKey = SCRIPT_ON_R_BTN_UP;
				}
				bRet = true;
			}
		}break;
	case IEI_MS_MOVE:
		{
			bool const bIsBeforeMouseOver = IsMouseOver();
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				bRet = true;
				if(m_spWndMouseOver != this)
				{
					if(m_spWndMouseOver)
					{
						m_spWndMouseOver->DoScript(SCRIPT_ON_MOUSE_OUT);//이전 마우스 오버에서 아웃 처리.
						m_spWndMouseOver->IsMouseDown(false);
					}

					m_spWndMouseOver = this;// 마우스 올라왔어.

					wstrScriptKey = SCRIPT_ON_MOUSE_OVER;
				}

				if(IsMouseDown() && CanDrag())
				{
					POINT3I ptOrg = Location();
					Location( ptOrg+rPT );
				}

				if(IsMouseDown())//>>Edit 전용 추가
				{
					int pos = GetClickTextPos();
					if(g_kSingleIME.GetCaretPos().iSelectStart > pos)
					{
						int start = g_kSingleIME.GetCaretPos().iSelectEnd;
						MoveCarotToClickPos(pos, start);
					}
					else if(g_kSingleIME.GetCaretPos().iSelectStart < pos)
					{
						int start = g_kSingleIME.GetCaretPos().iSelectStart;
						MoveCarotToClickPos(start, pos);
					}
				}//<<Edit 전용 추가
				
				if(rPT.z != 0 && ContainsPoint(m_sMousePos)) // 마우스 휠이다.
				{
					if (rPT.z > 0){wstrScriptKey = SCRIPT_ON_WHEEL_UP;}
					else{wstrScriptKey = SCRIPT_ON_WHEEL_DOWN;}
					SetCustomData(&rPT.z, sizeof(rPT.z));
					bRet = true;
				}
			}
		}break;
	default:
		{
			return true;
		}break;
	}

	bool const bScriptRet = DoScript( wstrScriptKey );//실행 안되는 현상을 방지하기 위함.

	return (bRet || bScriptRet);
}

void CXUI_Edit::OnHookEvent()
{
	//g_kSingleIME.GetWindowText(m_wstrInputText);
	m_wstrInputText = g_kSingleIME.GetResultStr();
	/*//디버그 정보 표시용
	TCHAR szTemp[255] = {0, };
	_stprintf_s(szTemp, 255, _T("OnHookEvent: ID[%s]\n"), m_ID.c_str());
	OutputDebugStr(szTemp);
	*/
}

void	CXUI_Edit::ApplyTextBlockToText(std::wstring const &kSrcText,std::wstring::size_type const &kOffset,std::wstring::size_type const &kLength,std::wstring &koutText) const
{
	if(m_kTextBlocks.size() == 0)
	{
		koutText = kSrcText;
		return;
	}

	koutText.clear();

	std::wstring::size_type	kPrevPos = kOffset;
	std::wstring::size_type kPrevTextLen = 0;

	std::wstring::size_type	kTextBlockAdj = 0;

	int	iBlockIndex = 0;
	for(TextBlockCont::const_iterator itor = m_kTextBlocks.begin(); itor != m_kTextBlocks.end(); ++itor,++iBlockIndex)
	{
		stTextBlock	const	&kTextBlock = *itor;
		if(kTextBlock.m_iPos+kTextBlock.m_kText.length()-1<kOffset)
		{
			continue;
		}
		if(kTextBlock.m_iPos>=kOffset + kLength)
		{
			break;
		}

		std::wstring	kBlockText;

		kTextBlockAdj = 0;
		if(kTextBlock.m_iPos>=kOffset)
		{
			if((kTextBlock.m_iPos+kTextBlock.m_kText.length())<=(kOffset+kLength))
			{
				kBlockText = kTextBlock.m_kText;
			}
			else if((kTextBlock.m_iPos+kTextBlock.m_kText.length())>(kOffset+kLength))
			{
				kBlockText = kTextBlock.m_kText.substr(0,(kOffset+kLength) - kTextBlock.m_iPos);
			}
		}
		else if(kTextBlock.m_iPos<kOffset)
		{
			if((kTextBlock.m_iPos+kTextBlock.m_kText.length())<=(kOffset+kLength))
			{
				kTextBlockAdj = kOffset -kTextBlock.m_iPos;
				kBlockText = kTextBlock.m_kText.substr(kTextBlockAdj,(kTextBlock.m_iPos+kTextBlock.m_kText.length()) -  kOffset);
			}
			else if((kTextBlock.m_iPos+kTextBlock.m_kText.length())>(kOffset+kLength))
			{
				kTextBlockAdj = kOffset -kTextBlock.m_iPos;
				kBlockText = kTextBlock.m_kText.substr(kTextBlockAdj,kLength);
			}
		}

		if(kBlockText.length() == 0)
		{
			continue;
		}


		kPrevTextLen = (kTextBlock.m_iPos+kTextBlockAdj) - kPrevPos;

		TCHAR	temp[1000];
		swprintf_s(temp,1000,_T("%s{C=%s/C=0x%08x/EP=%d/O=U/}%s{C=%s/O=0/}"),(kPrevTextLen>0) ? (kSrcText.substr(kPrevPos-kOffset,kPrevTextLen).c_str()) : _T(""),XUI_SAVE_COLOR,kTextBlock.m_dwTextColor,iBlockIndex,kBlockText.c_str(),XUI_RESTORE_COLOR);
		koutText += std::wstring(temp);

		kPrevPos += kBlockText.length()+(kPrevTextLen);
	}

	kPrevTextLen = kSrcText.length() - (kPrevPos-kOffset);

	if(kPrevTextLen>0)
	{
		koutText += kSrcText.substr(kPrevPos-kOffset,kPrevTextLen);
	}

}
bool CXUI_Edit::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick( dwCurTime ) )
	{
		return false;
	}

	CS::CARETDATA Data = g_kSingleIME.GetCaretPos();
	
	SRenderTextInfo kRenderTextInfo;

	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	if( GetFocusedEdit() == this && m_spWndMouseFocus == this )	//글만 적어놓고 다른 윈도우를 클릭할 수 있으므로 m_spWndMouseFocus도 체크해야 함
	{//포커싱 되었다.
		//g_kSingleIME.GetWindowText(m_wstrInputText);
		m_wstrInputText = g_kSingleIME.GetResultStr();  

		std::wstring szRealText;//(m_wstrInputText.size(), 0);
		std::wstring kText;

		int const iLine = MakeEditString(szRealText, Data.iCaretPos);
		
		for (VEC_LINE::const_iterator line_it = m_kVecLine.begin();
			m_kVecLine.end() != line_it; ++line_it)
		{		
			kText += (*line_it).m_kWstr;
			VEC_LINE::const_iterator next_it = line_it;
			++next_it;
			if (m_kVecLine.end() != next_it)
			{
				kText += _T('\n');
			}
		}

		if(m_kVecLine.size() == 1)
		{
			std::wstring kTempText;
			ApplyTextBlockToText(kText,m_iStartTextPos,kText.length(),kTempText);

			kText = kTempText;
		}

		Text(kText);

		if( dwCurTime - CarotBlinkTime() > 400 )	//깜빡임 속도조절
		{
			CarotBlink(!CarotBlink());
			CarotBlinkTime(dwCurTime);
		}


		SetInvalidate();

	}//CHSIME 사용법
	else if( m_wstrInputText.size() )
	{//입력된 텍스트가 있다.
		std::wstring szRealText;
	
		MakeEditString( szRealText, Data.iCaretPos );
		Text(szRealText);

		if(CarotBlink())
		{
			CarotBlink(false);
			SetInvalidate();
		}
	}
	else
	{
		Text(PreviewText());
		if(CarotBlink())
		{
			CarotBlink(false);
			SetInvalidate();
		}
	}
	return true;
}

void CXUI_Edit::EditText(std::wstring const& wstrValue,bool bKeepTextBlock)
{
	m_wstrInputText = wstrValue;
	g_kSingleIME.SetOnlyNumeric(IsOnlyNum(), false);
	g_kSingleIME.SetString( m_wstrInputText );
	g_kSingleIME.SetCaretPos(false);//캐럿을 맨 뒤로
	g_kSingleIME.SetLimitLength(m_iLimitLength, false);//문제 제한을 변경한다.

	if(!bKeepTextBlock)
	{
		RemoveAllTextBlock();
	}
}
bool CXUI_Edit::GetEditText_TextBlockApplied(CXUI_Style_String &koutString)const
{
	std::wstring	kResultStr;


	CXUI_Font	*pFont = g_kFontMgr.GetFont(Font());
	if(!pFont)
	{
		return	false;
	}

	ApplyTextBlockToText(m_wstrInputText,0,m_wstrInputText.length(),kResultStr);

	koutString.Clear();
	koutString.CreateStyleString(XUI::PgFontDef(pFont),kResultStr);

	if(m_kTextBlocks.size()>0)
	{
		for(TextBlockCont::const_iterator itor = m_kTextBlocks.begin(); itor != m_kTextBlocks.end(); ++itor)
		{
			stTextBlock	const &kTextBlock = *itor;
			tagExtraData::CONT_BINARY_ARRAY	kData(kTextBlock.m_iExtraDataLen);
			memcpy(&kData.at(0),kTextBlock.m_pkExtraData,kTextBlock.m_iExtraDataLen);

			PgExtraDataPack	kExtraDataPack;
			kExtraDataPack.AddExtraData(_T("IT"),kData);
			koutString.GetExtraDataPackInfo().AddExtraDataPack(kExtraDataPack);
		}	
	}

	return	true;
}

void CXUI_Edit::FocusingPassGroupEdit(CXUI_Edit* pkEdit)
{
	if( !pkEdit )
	{
		return;
	}

	CONT_EDIT_GROUP::iterator group_itor = std::find(m_kEditGroupCont.begin(), m_kEditGroupCont.end(), pkEdit);
	if( group_itor != m_kEditGroupCont.end() )
	{
		CONT_EDIT_GROUP::value_type& kGroup = (*group_itor);
		kGroup.FocusPassNextEdit(pkEdit);
	}
}

bool CXUI_Edit::AddGroupEdit(CXUI_Edit* pkEdit, int const iOrderLank)
{
	if( !pkEdit )
	{
		return false;
	}

	CONT_EDIT_GROUP::iterator group_itor = std::find(m_kEditGroupCont.begin(), m_kEditGroupCont.end(), pkEdit);
	if( group_itor != m_kEditGroupCont.end() )
	{
		CONT_EDIT_GROUP::value_type& kGroup = (*group_itor);
		return kGroup.AddControl(iOrderLank, pkEdit);
	}
	else
	{
		ID_TYPE kID;
		if( !pkEdit->Parent() )
		{
			kID = pkEdit->ID();
		}
		else
		{
			CXUI_Wnd* pkParent = pkEdit->Parent();
			while( pkParent->Parent() )
			{
				pkParent = pkParent->Parent();
			}

			kID = pkParent->ID();
		}

		CONT_EDIT_GROUP::iterator group_itor = m_kEditGroupCont.insert( m_kEditGroupCont.end(), PgEditFocusGroup(kID) );
		if( group_itor != m_kEditGroupCont.end() )
		{
			CONT_EDIT_GROUP::value_type& kGroup = (*group_itor);
			return kGroup.AddControl(iOrderLank, pkEdit);
		}
	}
	return false;
}

void CXUI_Edit::DelGroupEdit(CXUI_Edit* pkEdit)
{
	if( !pkEdit )
	{
		return;
	}

	CONT_EDIT_GROUP::iterator group_itor = std::find(m_kEditGroupCont.begin(), m_kEditGroupCont.end(), pkEdit);
	if( group_itor != m_kEditGroupCont.end() )
	{
		CONT_EDIT_GROUP::value_type& kGroup = (*group_itor);
		kGroup.DelControl(pkEdit);
		if( 0 == kGroup.GetEditControlCount() )
		{
			m_kEditGroupCont.erase( group_itor );
		}
	}
}

void CXUI_Edit::ClearGroupEdit()
{
	m_kEditGroupCont.clear();
}

std::wstring const& CXUI_Edit::EditText()const
{
	return m_wstrInputText;
}

void	CXUI_Edit::SetFocusedEdit(CXUI_Edit *pkEdit)
{
	if(pkEdit)
	{
		g_kSingleIME.SetEnableIME(true);
		g_kMultiIME.SetEnableIME(true);
	}
	else
	{
		g_kSingleIME.SetEnableIME(false);
		g_kMultiIME.SetEnableIME(false);
	}

	m_spFocusedEdit = pkEdit;
}
void CXUI_Edit::ReleaseGlobalImage()
{
	if (m_spTextBlockBgImg && m_spRscMgr)
	{
		m_spRscMgr->ReleaseRsc(m_spTextBlockBgImg);
	}

	if (m_spCarotImg && m_spRscMgr)
	{
		m_spRscMgr->ReleaseRsc(m_spCarotImg);
	}
}

void CXUI_Edit::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_EDIT_T_X == wstrName )
	{
		POINT2 pt = EditTextPos();
		pt.x = (int)vValue;
	
		EditTextPos( pt );	
	}

	else if( ATTR_EDIT_T_Y == wstrName )
	{
		POINT2 pt = EditTextPos();
		pt.y = (int)vValue;
	
		EditTextPos( pt );	
	}

	else if( ATTR_EDIT_FONT == wstrName )
	{
		EditFont( wstrValue );	
	}
	
	else if( ATTR_EDIT_PREVIEW == wstrName )
	{
		PreviewText( XUI_TT((int)vValue) );	
	}

	//선택영역 그림
	else if( ATTR_EDIT_BLOCK_IMG == wstrName )
	{
		m_spRscMgr->ReleaseRsc(m_spTextBlockBgImg);
		m_spTextBlockBgImg = m_spRscMgr->GetRsc( vValue );
		m_siBlockImgIdx = -2;

		m_wstrBlockPath = static_cast<std::wstring>(vValue);
	}

	//캐럿 그림
	else if( ATTR_EDIT_CAROT_IMG == wstrName )
	{
		m_spRscMgr->ReleaseRsc(m_spCarotImg);
		m_spCarotImg = m_spRscMgr->GetRsc( vValue );
		m_siCarotImgIdx = -1;

		m_wstrCarotPath = static_cast<std::wstring>(vValue);
	}

	else if( ATTR_EDIT_IS_SECRET == wstrName )
	{
		if(0 == (int)vValue)
		{
			IsSecret(false);
		}
		else
		{
			IsSecret(true);
		}
	}

	else if(ATTR_EDIT_EDITTING_DISABLE == wstrName)
	{
		if(0 == (int)vValue)
		{
			IsEditDisable(false);
		}
		else
		{
			IsEditDisable(true);
		}
	}
	else if(ATTR_EDIT_LIMIT_LENGTH == wstrName)
	{
		int iVal = (int)vValue;
		if(iVal)
		{
			m_iLimitLength = iVal;
		}
	}
	else if(ATTR_EDIT_ONLY_NUM == wstrName)
	{
		IsOnlyNum( (int)vValue );
	}
	else if( ATTR_EDIT_ORDER_LANK == wstrName )
	{
		m_kOrderLank = (int)vValue;
	}
}

void CXUI_Edit::VOnCall()
{
	this->CXUI_Wnd::VOnCall();
	CXUI_Edit::AddGroupEdit(this, OrderLank());
}

bool CXUI_Edit::SetEditFocus(bool const bIsJustFocus)
{
	//g_kSingleIME.SetDisableEnglish();
	if(IsEditDisable())
	{//수정불가능!!
		return false;
	}

	/* //디버그 정보 표시용
	TCHAR szTemp[255] = {0, };
	if(CXUI_Edit::m_spFocusedEdit)
	{
		_stprintf_s(szTemp, 255, _T("Previous Edit: ID[%s], Now Edit: ID[%s]\n"), CXUI_Edit::m_spFocusedEdit->m_ID.c_str(), this->m_ID.c_str());
	}
	else
	{
		_stprintf_s(szTemp, 255, _T("Now Edit: ID[%s]\n"), this->m_ID.c_str());
	}
	OutputDebugStr(szTemp);*/

	if( !m_wstrBlockPath.empty() )
	{
		m_spRscMgr->ReleaseRsc(CXUI_Edit::m_spTextBlockBgImg);
		CXUI_Edit::m_spTextBlockBgImg = m_spRscMgr->GetRsc( m_wstrBlockPath );
		m_siBlockImgIdx = -2;
	}

	if( !m_wstrCarotPath.empty() )
	{
		m_spRscMgr->ReleaseRsc(CXUI_Edit::m_spCarotImg);
		CXUI_Edit::m_spCarotImg = m_spRscMgr->GetRsc( m_wstrCarotPath );
		m_siCarotImgIdx = -1;
	}

	VAcquireFocus(this);
	CXUI_Edit::SetFocusedEdit(this);

	if(!bIsJustFocus)
	{
		m_wstrInputText = _T("");
	}

	g_kSingleIME.SetLimitLength(m_iLimitLength, false);//문제 제한을 변경한다.
	g_kSingleIME.SetOnlyNumeric(IsOnlyNum(), false);
	g_kSingleIME.SetString( m_wstrInputText );
	g_kSingleIME.SetPasswordMode(IsSecret());
	g_kSingleIME.SetEnglishIME(false);
	g_kSingleIME.SetEnableIME(true);
	g_kSingleIME.SetOnImeCharCallBackFunc(OnImeCharCallBack);
	g_kSingleIME.SetOnCommandCallBackFunc(OnCommandCallBack);

	if (IsNativeIME())
	{
		g_kSingleIME.SetNativeIME();
	}

	DoScript(SCRIPT_ON_FOCUS);	//포커스를 가졌을때 (ON_FOCUS)
	return true;
}

int const CXUI_Edit::MakeEditString(std::wstring & Val, int const iCarot)
{
	int iLineCount = 1;
	if( m_wstrInputText.empty() )
	{
		m_kVecLine.clear();
		m_wstrRealString.clear();
		return iLineCount;
	}

	if( m_wstrInputText == m_wstrPastInputText && m_iPastCarotPos == iCarot && !m_wstrRealString.empty())
	{	//텍스트와 케럿 위치가 변경되지 않았을 때 또 계산을 해 주지 않게 하기 위해.
		Val = m_wstrRealString;
		iLineCount = GetLineCount(Val, iCarot);
		if(IsSecret())
		{
			Val.replace(Val.begin(), Val.end(), Val.size(), _T('*'));
		}
		return iLineCount;
	}
	else
	{
		CarotBlink(true); 
		CarotBlinkTime(BM::GetTime32());

		m_wstrPastInputText = m_wstrInputText;
		m_iPastCarotPos = iCarot;
	}
	
	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	m_kVecLine.clear();

	if( GetFocusedEdit() != this || m_spWndMouseFocus != this )
	{	//포커스를 안갖고 있으면
		std::wstring::const_iterator itor = m_wstrInputText.begin();

		std::wstring szTemp;
		for(int i = 0; itor != m_wstrInputText.end(); )
		{
			szTemp+=(*itor);
			if(pFont->CalcWidth(szTemp) + EditTextPos().x <= Width() )
			{
				Val+=(*itor);
				++i;
				++itor;
			}
			else
			{
				m_iEndTextPos = m_iStartTextPos = 0;//+ i;
				break;
			}
		}

		m_wstrRealString = Val;
		SLineInfo kResult;
		kResult.Set(Val, pFont->CalcWidth(Val));
		m_kVecLine.push_back(kResult);
		if(IsSecret())
		{
			Val.replace(Val.begin(), Val.end(), Val.size(), _T('*'));
			MakeSecretVecLine();
		}
		return 1;
		
	}

	{
		int const iTextLen = pFont->CalcWidth(m_wstrInputText);
		SLineInfo kTemp;
		kTemp.Set(m_wstrInputText, iTextLen);
		
		int iTotalLength = iTextLen + EditTextPos().x;

		if( iTotalLength <= m_Size.x )
		{
			Val = m_wstrInputText;
			m_wstrRealString = m_wstrInputText;
			m_iEndTextPos = (int)Val.size();
			m_iStartTextPos = 0;
			m_kVecLine.push_back(kTemp);

			if(IsSecret())
			{
				Val.replace(Val.begin(), Val.end(), Val.size(), _T('*'));
				MakeSecretVecLine();
			}
			return iLineCount;
		}
	}

	std::wstring szTemp;
	//szTemp.reserve(m_wstrInputText.size());
	if( iCarot == m_wstrInputText.size() )	////오른쪽 끝에서 지울때 항상 에디트박스에 텍스트가 꽉차 있도록
	{
		m_iEndTextPos = iCarot;
	}
	//비교를 첨부터 할거냐 끝에서부터 할거냐
	if( m_iEndTextPos <= iCarot )	//오른쪽으로 갱신
	{
		m_iEndTextPos = iCarot;
		
		int i = m_iEndTextPos;
		int iCount = 0;
		for(; i > 0;) 
		{
			/*if (i>=(int)m_wstrInputText.length())
			{
				break;
			}*/
			szTemp+=m_wstrInputText[i];
			if(pFont->CalcWidth(szTemp) + EditTextPos().x <= Width() )
			{
				Val.insert(0, m_wstrInputText.substr(i,1) );
				--i;
				++iCount;
			}
			else
			{
				break;
			}
		}

		m_iStartTextPos = i + 1;
	}
	else// 
	{
		if( m_iStartTextPos > iCarot )	//왼쪽으로 갱신
			m_iStartTextPos = iCarot;
		
		//일반적인 경우까지
		std::wstring::iterator itor = m_wstrInputText.begin()+m_iStartTextPos;

		for(int i = 0; itor != m_wstrInputText.end(); )
		{
			szTemp+=(*itor);
			if(pFont->CalcWidth(szTemp) + EditTextPos().x <= Width() )
			{
				Val+=(*itor);
				++i;
				++itor;
			}
			else
			{
				m_iEndTextPos = m_iStartTextPos + i;
				break;
			}
		}
	}
	if(IsSecret())
	{
		Val.replace(Val.begin(), Val.end(), Val.size(), _T('*'));
		MakeSecretVecLine();
	}

	m_wstrRealString = Val;
	SLineInfo kResult;
	kResult.Set(Val, pFont->CalcWidth(Val));
	m_kVecLine.push_back(kResult);

	return iLineCount;
}

void CXUI_Edit::RenderBlock(std::wstring & Val)
{
	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	if( ! CXUI_Edit::m_spTextBlockBgImg ){return;}
	if( m_wstrInputText.empty() ){return;}

	CS::CARETDATA const Data = g_kSingleIME.GetCaretPos();

	if( Data.iSelectStart == Data.iSelectEnd ){return;}//선택 영역 없음.

	POINT3I pt = TotalLocation()+EditTextPos();

	SRenderInfo kRenderInfo;

	kRenderInfo.bGrayScale = GrayScale();

	SSizedScale &rSS = kRenderInfo.kSizedScale;
	rSS.ptSrcSize = POINT2(128,12);//xxx todo 하드코딩
	kRenderInfo.fAlpha = Alpha();
	m_siBlockImgIdx = -2;

	std::wstring szFront;
	szFront.clear();
	int n = 0;
	for( ; n < Data.iSelectStart-m_iStartTextPos; ++n)
	{
		szFront+=Val[n];
	}
	pt.x += pFont->CalcWidth(szFront);

	szFront.clear();

	//텍스트 범위를 벗어나지 않게
	int iBStart = Data.iSelectStart;
	int iBEnd = Data.iSelectEnd;
	if( Data.iSelectStart < m_iStartTextPos ){iBStart = m_iStartTextPos;}
	if( Data.iSelectEnd > m_iEndTextPos ){iBEnd = m_iEndTextPos;}

	for(int i = 0; i < iBEnd - iBStart ; ++i)
	{
		szFront += Val[n+i];
	}

	rSS.ptDrawSize = POINT2(pFont->CalcWidth(szFront), pFont->GetHeight());

	kRenderInfo.kUVInfo;// = UVInfo();
	kRenderInfo.kLoc = pt;
	GetParentDrawRect(kRenderInfo.rcDrawable);
	m_spRenderer->RenderSprite( CXUI_Edit::m_spTextBlockBgImg, m_siBlockImgIdx, kRenderInfo);
}

void CXUI_Edit::RenderCarot(POINT2& pt)	//캐럿출력
{
	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	if(CXUI_Edit::m_spTextBlockBgImg == NULL
	||	CXUI_Edit::m_spCarotImg == NULL 
	||	!pFont)
	{
		assert(NULL);
		return;
	}

	void *pImg = NULL;
	int *pImgIdx = NULL;
	SRenderInfo kRenderInfo;
	int iAdd = 0;

	if( g_kSingleIME.IsNowComp() )	//한글이면
	{
		pt.x -= pFont->GetHeight();
 		
		pImg = CXUI_Edit::m_spTextBlockBgImg;
		pImgIdx = &m_siBlockImgIdx;

		SSizedScale &rSS = kRenderInfo.kSizedScale;
		rSS.ptSrcSize = POINT2(16,16);//xxx todo 하드코딩
		rSS.ptDrawSize = POINT2(pFont->GetHeight()-4, pFont->GetHeight());
		iAdd = 4;//xxx todo 하드코딩
	}
	else
	{	
		pImg = CXUI_Edit::m_spCarotImg;
		pImgIdx = &m_siCarotImgIdx;

		SSizedScale &rSS = kRenderInfo.kSizedScale;
		rSS.ptSrcSize = POINT2(16,16);//xxx todo 하드코딩
		rSS.ptDrawSize = POINT2(1, pFont->GetHeight());
	}

	kRenderInfo.kUVInfo = UVInfo();
	kRenderInfo.kLoc = pt;
	kRenderInfo.kLoc.x+=iAdd;
	GetParentDrawRect(kRenderInfo.rcDrawable);
	kRenderInfo.fAlpha = Alpha();

	if( pImg )
	{
		m_spRenderer->RenderSprite( pImg, *pImgIdx, kRenderInfo);
	}
}

int CXUI_Edit::GetClickTextPos()
{
	if( m_wstrInputText.empty() )
	{
		return 0;
	}

	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(EditFont());

	std::wstring szTemp; 
	szTemp.reserve(m_wstrInputText.size());

	POINT2 ptLastPos = TotalLocation()+EditTextPos();
	int iSize = 0;
	int const iStartPos = __min(m_iStartTextPos, (int)m_wstrInputText.size());
	int const iEndPos = __min(m_iEndTextPos, (int)m_wstrInputText.size());
	for(int i = iStartPos; i < iEndPos; ++i)
	{
		szTemp+=m_wstrInputText[i];
		iSize = pFont->CalcWidth(szTemp);
		if( ptLastPos.x + iSize > m_sMousePos.x )
		{
			if( (ptLastPos.x + iSize) - pFont->GetWidth((unsigned short)m_wstrInputText[i]) / 2.0f > m_sMousePos.x )	//글자 하나를 반으로 나눠서 
			{
				return i;	//왼쪽
			}
			else 
			{
				return i+1;		//오른쪽
			}
		}
	}

	return (int)m_wstrInputText.size();
}

//선택한 위치로 캐럿 이동.
void CXUI_Edit::MoveCarotToClickPos(int &iStart, int &iEnd)
{
	if( m_wstrInputText.empty() )
	{
		g_kSingleIME.SetCaretPos(0,0);
		return; 
	}

	if( iStart < m_iStartTextPos )
		iStart = m_iStartTextPos;

	if( iStart > m_iEndTextPos )
		iEnd = m_iEndTextPos;

	g_kSingleIME.SetCaretPos(iStart,iEnd);
}

void CXUI_Edit::VLoseFocus(bool const bUpToParent)
{
	if (E_XUI_EDIT != m_spWndMouseFocus->VType())
	{
		return;
	}
	CXUI_Wnd::VLoseFocus(bUpToParent);
	if (this == CXUI_Edit::GetFocusedEdit())
	{
		CXUI_Edit::SetFocusedEdit(NULL);
		
		IsNativeIME(g_kSingleIME.GetIME_CMODE() == IME_CMODE_NATIVE);
		g_kSingleIME.SetEnglishIME(true);
		g_kSingleIME.SetEnableIME(false);
	}
	g_kSingleIME.SetOnlyNumeric(false, false);	
} 

int const CXUI_Edit::GetLineCount(std::wstring &krString, int const iCarot)
{
	if (0>=iCarot || krString.empty())
	{
		return 1;
	}

	int iCount = 1;
	int iCharCount = 0;
	int const iLength = (int)krString.length();

	VEC_LINE::const_iterator line_it = m_kVecLine.begin();
	int iTempCaret = iCarot;
	while (m_kVecLine.end() != line_it)
	{
		std::wstring const wstrLine = (*line_it).m_kWstr;
		int const iLineLen = (int)((*line_it).m_kWstr.length());
		if (iTempCaret <= iLineLen)
		{
			break;
		}
		else
		{
			iTempCaret-=iLineLen;
			if ((*line_it).m_bReturn)
			{
				iTempCaret-=2;
			}
		}
		++iCount;
		++line_it;
	}

	return iCount;
}

void CXUI_Edit::MakeSecretVecLine()
{
	VEC_LINE::iterator line_it = m_kVecLine.begin();
	while (m_kVecLine.end() != line_it)
	{
		std::wstring &kLine = (*line_it).m_kWstr;
		kLine.replace(kLine.begin(), kLine.end(), kLine.size(), _T('*'));	
		++line_it;
	}
}

POINT2 CXUI_Edit::CalcCaretPos(std::wstring const& wstrReal, CS::CARETDATA const& rkData, XUI::CXUI_Font* pFont, int const iLine)
{
	POINT2 ptLastPos = TotalLocation()+EditTextPos();

	if (!wstrReal.empty() || pFont)
	{
		std::wstring szFront;

		if( !m_wstrInputText.empty() )
		{
			int const iLen = __min((int)wstrReal.size(), rkData.iCaretPos-m_iStartTextPos);
			for(int i = 0; i < iLen; ++i)
			{
				szFront+=wstrReal[i];
			}

			ptLastPos.x+= pFont->CalcWidth(szFront);//약간 앞으로 당겨줘야 함
		}
	}

	return ptLastPos;
}

bool CXUI_Edit::SetLimitLength(int const iLimitLength)
{
	if(iLimitLength < 1)
	{
		return false;
	}

	m_iLimitLength = iLimitLength;
	return true;
}

void CXUI_Edit::operator = ( const CXUI_Edit &rhs)
{
	CXUI_Wnd::operator =(rhs);

	CarotBlinkTime(rhs.CarotBlinkTime());
	CarotBlink(rhs.CarotBlink());
	IsSecret(rhs.IsSecret());
	IsEditDisable(rhs.IsEditDisable());
	IsOnlyNum(rhs.IsOnlyNum());
	IsMultiLine(rhs.IsMultiLine());
	IsNativeIME(rhs.IsNativeIME());
	LastAddCount(rhs.LastAddCount());
}