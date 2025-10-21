#include "stdafx.h"
#define DIRECTINPUT_VERSION (0x0800)


#include "../../../cosmos/DirectXSDK/Include/dinput.h"
#include "BM/STLSupport.h"
#include "BM/vstring.h"

#include "XUI_Manager.h"
#include "XUI_Manager.inl"
#include "XUI_Full_Form.h"

/*
클래스 구조.
CXUI_Wnd ->  최상위 공통
	CXUI_Form	-> 컨트롤을 소유 할 수 있는 
		CXUI_Dialog	-> 캡션이 있는 폼
	CXUI_Control	-> 컨트롤을 소유 할 수 없음
		CXUI_Button	-> 버튼
		CXUI_Combo -> 콤보
		CXUI_List -> 리스트
*/

/*
	헤더	FORM, DIALOG, BTN, LISTBOX, COMBO, BAR, (속성셋트 Parent 의 속성을 변화시킴)
	속성	ID

	헤더	ATTR
	속성	CAPTION, X,Y,W,H  IMG_SRC, SND_SRC, EFF_SRC, FONT,
			B_X,B_Y,B_W,B_H(마우스 체크(BOUND) 영역)
			ALIGN -> FORM 이하만 먹는다.(키패드에 대응)

	헤더	SCRIPT	(스크립트는 텍스트에 박는다)
	속성 	ID
*/

namespace XUI_ManagerUtil
{
	using namespace XUI;

	tagUIXMLData::ChildHash::const_iterator FindInLocal(std::wstring const& rkLocalStr, tagUIXMLData::ChildHash const& rkCurChildHash)
	{
		tagUIXMLData::ChildHash::const_iterator find_local = rkCurChildHash.end();
		tagUIXMLData::ChildHash::const_iterator default_local = rkCurChildHash.end();
		tagUIXMLData::ChildHash::const_iterator loop_iter = rkCurChildHash.begin();

		while( rkCurChildHash.end() != loop_iter )
		{
			typedef std::list< std::wstring > ContWstr;
			tagUIXMLData const& local_element = (*loop_iter).second;
			tagUIXMLData::AttrList::const_iterator local_attr_itor = local_element.m_kAttrList.begin();

			// local_attr_itor->strValue 를 sep "/"로 나누기
			std::wstring const rkCurLocalStr = local_attr_itor->strValue;

			ContWstr kList;
			BM::vstring::CutTextByKey(rkCurLocalStr, std::wstring(L"/"), kList);

#ifndef _MDo_
			// local_attr_itor->strValue 에 default와 다른 local이 같이 들어 있으면 에러
			if( 1 < kList.size()
				&&	kList.end() != std::find( kList.begin(), kList.end(), std::wstring(L"DEFAULT")) )
			{
				std::wstring const &rkErrorMsg = std::wstring(L"XUI: 'DEFAULT' is can't same use, other local tag in ") + rkCurLocalStr;
				::MessageBox(NULL, rkErrorMsg.c_str(), L"Error", MB_OK);
			}
#endif

			if( kList.end() != std::find(kList.begin(), kList.end(), rkLocalStr) )
			{
#ifndef _MDo_
				// local_attr_itor->strValue 로컬이 중복 되어있으면 오류(예: "EU/SINGAPORE", "EU" 경우)
				if( rkCurChildHash.end() != find_local )
				{
					std::wstring const& kTempStr = std::wstring(L"XUI: ") + rkLocalStr + L" local is duplicated";
					::MessageBox(NULL, kTempStr.c_str(), L"Error", MB_OK);
				}
#endif
				find_local = loop_iter;
			}
			else
			{
				if( _T("DEFAULT") == local_attr_itor->strValue )
				{
					default_local = loop_iter;
				}
			}
			++loop_iter;
		}

		if( rkCurChildHash.end() != find_local )
		{
			return find_local;
		}
		else
		{
			if( rkCurChildHash.end() == find_local
			&&	rkCurChildHash.end() != default_local )
			{
				return default_local;
			}
		}
		return rkCurChildHash.end();
	}
};

CS::CCSIME g_kSingleIME;
CS::CCSIME g_kMultiIME;

using namespace XUI;
PgUIXMLData CXUI_Manager::m_kXMLData;
CXUI_Manager::KEY_WORD_HASH CXUI_Manager::m_mapKeyWord;
CXUI_Manager::XUI_HASH CXUI_Manager::m_kBufferdHash;
CXUI_Manager::XUI_HASH CXUI_Manager::m_mapCreator;
CXUI_Manager::COLOR_HASH CXUI_Manager::m_ColorMap;
DWORD CXUI_Manager::m_FrameCount = 0;
std::wstring CXUI_Manager::m_strLocal;

__int32 CALLBACK ExecuteVK_KEY(int const iKey, CS::CCSIME *pIME, CS::CCSIME::EKeyState const KeyState)
{
	//EDIT 컨트롤에 한해서 WM_IME_ENDCOMPOSITION 메시지가 NiAction 보다 늦다 -_-;; 그래서 입력이 제대로 처리가 안된다.
	//TAB과 RETURN키에 한해서 DoScript 전용 이벤트를 발생시켜준다.
	//이 이벤트를 통해서 IME 입력과 관련 있는 이벤트를 진행 시켜야 안전하다.
	if(CS::CCSIME::KEY_STATE_UP == KeyState)//Key_UP에 해야 여러번 눌려져서 생기는 문제와 글자 조합 문제를 해결 할 수 있음. 
	{//UP을 함부로 바꾸지 말것 
		switch(iKey)
		{
		case VK_UP:
			{
				if( CXUI_Edit::GetFocusedEdit() )
				{
					CXUI_Edit::GetFocusedEdit()->DoScript(SCRIPT_ON_KEY_UP);
					return 1;
				}
			}break;
		case VK_DOWN:
			{
				if( CXUI_Edit::GetFocusedEdit() )
				{
					CXUI_Edit::GetFocusedEdit()->DoScript(SCRIPT_ON_KEY_DOWN);
					return 1;
				}
			}break;
		case VK_RETURN:
			{
				if( CXUI_Edit::GetFocusedEdit() )
				{
					CXUI_Edit::GetFocusedEdit()->DoScript(SCRIPT_ON_KEY_ENTER);
					return 1;
				}
			}break;
		case VK_TAB:
			{
				if( CXUI_Edit::GetFocusedEdit() )
				{
					CXUI_Edit::GetFocusedEdit()->DoScript(SCRIPT_ON_KEY_TAB);
					return 1;
				}
			}break;
		}
	}
	else//CS::CCSIME::KEY_STATE_UP:
	{
	}

	return 0;
}

/*
bool CXUI_Manager::SetEditAutoFocus()
{
	CXUI_Wnd* pMouseFocused = CXUI_Wnd::m_spWndMouseFocus;
	CXUI_Edit* pEditFocused = CXUI_Edit::m_spFocusedEdit;

	if( !pMouseFocused
	||	!pEditFocused )
	{
		WND_LIST::iterator itor = m_lstActive.begin();
		while( m_lstActive.end() != itor )
		{
			WND_LIST::value_type &pElement = (*itor);
			if(pElement->SetEditFocus(false))
			{
				return true;
			}
			++itor;
		}
	}
	else
	{
		DoScriptFocused();
	}
	return false;
}
*/

CXUI_Manager::CXUI_Manager(void)
:	m_bBlockGlobalScript(false)
,	m_bHideUI_Add_LastContainer(false)
{
//	m_pWarning = NULL;
//	m_pWarnList = NULL;
//	m_pToolTip = NULL;
	ReserveCreator();

	DblClickTick(500);
	DblClickBound(POINT3I(2,2,0));
	MouseSensitivity(10);
	m_dwLastXUIFlushTime = 0;
	m_kXUI_Manager_CallBackFunc = NULL;
	LocalName(_T("DEVELOP"));
}

CXUI_Manager::~CXUI_Manager(void)
{
	Destroy();
}

void CXUI_Manager::Destroy()
{
	ClearAllControl();

	//여기 있는 놈은 맵이동한다고 지워지는것이 아니다. 라고 정했다.
	ContHidePart::iterator	iter = m_kHidePart.begin();
	while( iter != m_kHidePart.end() )
	{
		CXUI_Wnd::WND_LIST::iterator	xui_iter = iter->second.begin();
		while( xui_iter != iter->second.end() )
		{
			SAFE_DELETE((*xui_iter));
			xui_iter = iter->second.erase(xui_iter);
		}
		iter = m_kHidePart.erase(iter);
	}

	XUI_HASH::iterator buf_itor = m_kBufferdHash.begin() ;//모달리스로 띄워지거나 했을때 Create를 하지 않고 버퍼에서 불러씀
	while(buf_itor != m_kBufferdHash.end())
	{
		if(buf_itor->second->VType() != E_XUI_CURSOR)
		{
			SAFE_DELETE(buf_itor->second);
		}
		++buf_itor;
	}
	m_kBufferdHash.clear();

	XUI_HASH::iterator creator_itor = m_mapCreator.begin() ;//모달리스로 띄워지거나 했을때 Create를 하지 않고 버퍼에서 불러씀
	while(creator_itor != m_mapCreator.end())
	{
		if(creator_itor->second->VType() != E_XUI_CURSOR)
		{
			SAFE_DELETE(creator_itor->second);
		}
		++creator_itor;
	}
	m_mapCreator.clear();
}

void CXUI_Manager::CorrectionLocationByResolution( const POINT2 ptGab )
{
	SetGab( ptGab );// 이전 해상도와의 차이 저장	
 
 	WND_LIST::iterator kItor = m_lstActive.begin();
 	while(m_lstActive.end() != kItor)
 	{
 		WND_LIST::value_type &ekElement = (*kItor);
 		if( ekElement->Visible() && ( !ekElement->Parent() ) )
 		{			
 			if( ekElement->AlignX() < 0 && ekElement->IsAbsoluteX() == false )
 			{
 				ekElement->Location( ekElement->Location().x + ptGab.x, ekElement->Location().y );
 			}

 			if( ekElement->AlignY() < 0 && ekElement->IsAbsoluteY() == false )
 			{
 				ekElement->Location( ekElement->Location().x, ekElement->Location().y + ptGab.y );
 			}
			
			ekElement->VAlign();
			ekElement->DoScript(SCRIPT_ON_CORRECT);
			ekElement->NeedCorrectionLocation(false);
 		}
 
 		++kItor;
 	}	
	
	XUI_HASH::iterator kHashItor = m_kBufferdHash.begin();
	while( m_kBufferdHash.end() != kHashItor )
	{
		XUI_HASH::value_type &ekElement = (*kHashItor);
		CXUI_Wnd* pkWnd = ekElement.second;

		if( !pkWnd->Parent()
			&& pkWnd->NeedCorrectionLocation() ) // 액티브리스트에서 위치 보정 안된 애들은 여기서 해준다.
		{
			if( pkWnd->AlignX() < 0 && pkWnd->IsAbsoluteX() == false )
			{
				pkWnd->Location( pkWnd->Location().x + ptGab.x, pkWnd->Location().y );
			}

			if( pkWnd->AlignY() < 0 && pkWnd->IsAbsoluteY() == false )
			{
				pkWnd->Location( pkWnd->Location().x, pkWnd->Location().y + ptGab.y );
			}

			pkWnd->VAlign();
			pkWnd->DoScript(SCRIPT_ON_CORRECT);
			pkWnd->NeedCorrectionLocation(false);
		}

		++kHashItor;
	}
	
	kItor = m_lstActive.begin();
	while(m_lstActive.end() != kItor)
	{
		WND_LIST::value_type &ekElement = (*kItor);
		ekElement->NeedCorrectionLocation(true);
		++kItor;
	}

	kHashItor = m_kBufferdHash.begin();
	while( m_kBufferdHash.end() != kHashItor )
	{
		XUI_HASH::value_type &ekElement = (*kHashItor);
		CXUI_Wnd* pkWnd = ekElement.second;		
		pkWnd->NeedCorrectionLocation(true);
		++kHashItor;
	}
}

bool CXUI_Manager::VDisplay()
{//위에 있을 수록 늦게 그린다.
//	if(m_pToolTip!=NULL)
//		m_pToolTip->VDisplay();//툴팁
	//////////////////////////////////////////////////////////////////
	WND_LIST::iterator kItor = m_lstActive.begin();//visible 아닌거 없에준다
	while(m_lstActive.end() != kItor)
	{
		WND_LIST::value_type &ekElement = (*kItor);
		if(ekElement->Visible())
		{
			ekElement->VDisplay();
			ekElement->VDisplayEnd();
		}
		else
		{
			if( ekElement->IsModal()
			&&	E_XUI_CURSOR != ekElement->VType() )
			{
				SAFE_DELETE(ekElement);
			}

			if( ekElement
			&&	m_kBufferdHash.end() == m_kBufferdHash.find( ekElement->ID() ) )
			{
				SAFE_DELETE(ekElement);
			}

			kItor = m_lstActive.erase(kItor);//Close 된거 제거.
			continue;
		}
		++kItor;
	}

//	for_each( m_lstActive.begin(), m_lstActive.end(), DisplayActive_Func() ); -> 게임브리오에서 제거.
	return true;
}

CXUI_Wnd* CXUI_Manager::PickWnd(POINT2 const& ptkPickPos, bool const bIsPickToTop)
{
	CXUI_Wnd* pkResultWnd = NULL;

	WND_LIST::iterator itor = m_lstActive.begin();//visible 아닌거 없에준다
	while(m_lstActive.end() != itor)
	{
		WND_LIST::value_type element = (*itor);

		if(!element->PassEvent() && element->ContainsPoint(ptkPickPos))
		{
			pkResultWnd = element;
			break;//포인트 가장 먼저 갖고 있는놈이 pop 됨.
		}
		if( element->IsModal() )
		{
			break;
		}
		++itor;
	}

	if(pkResultWnd && bIsPickToTop)
	{
		m_lstActive.erase(itor);//지우고
		InsertActiveList(pkResultWnd);//다시 넣음.
	}

	return pkResultWnd;
}

bool CXUI_Manager::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue, int const *piKey)
{//Active 목록 돌면서 이벤트 발생.
	float MouseAccel = 1.7f;

	m_bIsOverWnd = false;

	//CXUI_Wnd::m_spWndMouseFocus = NULL;
//	CXUI_Wnd::m_spWndMouseOver = NULL;// 일회용. 계속 유지 되거나하지 않는다.
	switch(rET)
	{
	case IEI_MS_DOWN:
	case IEI_MS_UP:
	case IEI_MS_MOVE:
		{
			bool bRet = false;

//			CXUI_Wnd::m_spWndMouseOver = NULL;//초기화 시켜놓고 들어간다.

			POINT3I ptBefore = CXUI_Wnd::m_sMousePos;
			POINT3I ptOrg = CXUI_Wnd::m_sMousePos;

			if( IEI_MS_MOVE == rET )//마우스 이동시
			{//스크린 영역을 벗어나지 않기위해 Point 계산 보정
				CXUI_Wnd::CorrectionScreenPoint(ptOrg, rPT);
			}

			if( m_spWndScrollBtn != NULL && rET == IEI_MS_UP )
			{
				m_spWndScrollBtn = NULL;
			}

			POINT3I ptResult( ptOrg - ptBefore );//결과에서 원본을 빼나?

			{
				CXUI_Wnd* pkWndPrevEditFocus = NULL;
				CXUI_Wnd* pkWndPrevFocus = CXUI_Wnd::m_spWndMouseFocus;
				if((NULL != CXUI_Edit::GetFocusedEdit()) && (CXUI_Edit::m_spFocusedEdit->IsDoNotLostFocus()))
				{//포커스를 잃지 말아야 할 Edit 박스가 다른 오브젝트 클릭으로 포커스를 잃는 것을 방지
					pkWndPrevEditFocus = CXUI_Edit::GetFocusedEdit();
				}

				if(IEI_MS_DOWN == rET)//클릭된 창을 priorty를 우선순위로 하여 다시 insert
				{
					CXUI_Wnd* pkWndFocus = PickWnd(CXUI_Wnd::m_sMousePos, true);//윈도우를 맨 위로.
					if( pkWndFocus )
					{
						pkWndFocus->DoScript(SCRIPT_ON_PICK_UP);
					}

					if(NULL != pkWndPrevEditFocus)
					{//잃지 말아야 할 포커스의 Edit라면 복구
						//CXUI_Wnd::m_spWndMouseFocus = pkWndPrevEditFocus;
						VAcquireFocus(pkWndPrevEditFocus);
					}
					else if(CXUI_Wnd::m_spWndMouseFocus != CXUI_Edit::GetFocusedEdit())
					{//자기 자신이 포커스가 아니라면(Edit Focus를 해제)
						CXUI_Edit::SetFocusedEdit(NULL);
					}
				}

				WND_LIST::iterator itor = m_lstActive.begin();
				while( m_lstActive.end() != itor )
				{
					WND_LIST::value_type &pElement = (*itor);
					if( pElement->VPeekEvent(rET, ptResult, dwValue) )
					{
						bRet = true;
						break;
					}

					if( pElement->IsModal() )
					{
						break;
					}

					++itor;
				}

				if (pkWndPrevFocus && pkWndPrevFocus!=m_spWndMouseFocus)
				{
					if (!bRet && CXUI_Wnd::m_spWndMouseFocus && CXUI_Wnd::m_spWndMouseOver == NULL && IEI_MS_DOWN == rET)
					{
						CXUI_Wnd::m_spWndMouseFocus->VLoseFocus();
					}
				}
			}

			if(m_bIsOverWnd == false)
			{
				CXUI_Wnd::m_spWndMouseOver = NULL;
			}
			
			CXUI_Wnd::m_sMousePos += ptResult;//마우스 포인터 보정은 마지막.
			if(IEI_MS_UP == rET)
			{
				if(MEI_BTN_0 == dwValue)
				{
					CXUI_Wnd::m_spConfig->DoScript(SCRIPT_ON_AFTER_L_BTN_UP);
				}
				else if( MEI_BTN_1 == dwValue )
				{ 
					CXUI_Wnd::m_spConfig->DoScript(SCRIPT_ON_AFTER_R_BTN_UP);
				}
				CXUI_Cursor::m_IconInfo.Clear();
			}

			if ( IEI_MS_DOWN == rET 
				&& !bRet
				&& !CXUI_Edit::GetFocusedEdit() )
			{	// 마우스 다운 이벤트 시에
				// 리턴값이 false이고 포커스된 에디트가 없으면
				// 마우스도 GlobalHotKey가 먹게
				BM::vstring kValue(-1 * dwValue);//마우스는 -값이다.
				bRet = DoHotKey(kValue);//글로벌 Hot Key Event(Block Global Script)
			}

			return bRet;
		}break;
	case IEI_KEY_DOWN://키보드 다운
		{//업에 안하면 여러번 눌러져 버린다.	VK_ESCAPE
			bool bRet = false;
			if( NULL != piKey )
			{
				DWORD dwVal = *piKey;
				{
					CXUI_Edit* pkEdit = CXUI_Edit::GetFocusedEdit();
					if( pkEdit )//포커스있는 Edit 콘트롤 부터 우선권 부여
					{
						if( pkEdit->VPeekEvent(rET, rPT, dwVal) )
						{
							return true;
						}
					}

					//	핫키 변경자 검사
					if( m_spHotKeyModifier->IsModifier(dwVal) )
					{
						return m_spHotKeyModifier->SetModifierState(dwVal, false);
					}

					WND_LIST::iterator active_iter = m_lstActive.begin();
					while( m_lstActive.end() != active_iter )
					{
						WND_LIST::value_type &pElement = (*active_iter);
						bRet = pElement->VPeekEvent(rET, POINT3I(), dwVal);
						if( bRet )
						{
							return true;
						}

						if( pElement->IsModal() )
						{
							return false;
						}

						++active_iter;
					}
				}

				if( CXUI_Wnd::m_spWndMouseFocus//포커스 잡히고 
				&&	CXUI_Edit::GetFocusedEdit() )//edit가 살아있으면 외부로
				{
					return true;//일단 입력창 떠있으면 키보드는 다 막는다.
				}
			}

			if( !bRet//다른 UI가 처리를 하지 않았고
			&& !CXUI_Edit::GetFocusedEdit() )//포커스 된 Edit 컨트롤이 없으면
			{
				BM::vstring kValue(dwValue);//글로발은 항상
				bRet = DoGlobalScript(kValue);//글로벌 Hot Key Event
			}

			//return bRet;
		}break;
	case IEI_KEY_UP:
		{//처리 예정 없음
			if( CXUI_Edit::GetFocusedEdit() )//Edit 컨트롤에 Focus가 있으면 무조건 키입력을 자기가 먹는다.
			{
				return true;
			}

			//	핫키 변경자 검사
			if( m_spHotKeyModifier->IsModifier((*piKey)) )
			{
				return m_spHotKeyModifier->SetModifierState((*piKey), true);
			}
		}break;
	case IEI_KEY_DOWN_PRESS:
		{
		}break;
	default:
		{
			return false;
		}break;
	}
		
	return false;
}

CXUI_Wnd* CXUI_Manager::Get(ID_TYPE const& ID)const
{
	std::wstring wstrID = ID;
	UPR(wstrID);

	WND_LIST::const_iterator itor = m_lstActive.begin();
	while( m_lstActive.end() != itor )//전체목록에서 찾아져야된다.
	{
		WND_LIST::value_type const& element = (*itor);
		if(element->ID() == wstrID)
		{
			return element;
		}
		++itor;
	}
	return NULL;
}

bool CXUI_Manager::IsActivate(ID_TYPE const& ParentID, CXUI_Wnd*& rkWnd)const 
{
	rkWnd = Get(ParentID);
	return rkWnd != NULL;
}

CXUI_Wnd* CXUI_Manager::BuildLocal(tagUIXMLData const *pUIRef, CXUI_Wnd* pWnd)
{
	if(pWnd && pUIRef)
	{
		//tagUIXMLData::AttrList::const_iterator attr_itor = pUIRef->m_kAttrList.begin();
		//while(attr_itor != pUIRef->m_kAttrList.end())
		//{//속성 등록
		//	pWnd->VRegistAttr((*attr_itor).strName, (*attr_itor).strValue);
		//	++attr_itor;
		//}

		tagUIXMLData::ScriptHash::const_iterator script_itor = pUIRef->m_kScriptHash.begin();
		while(script_itor != pUIRef->m_kScriptHash.end())
		{//스크립트 등록
			pWnd->RegistScript((*script_itor).first, (*script_itor).second);
			++script_itor;
		}

		tagUIXMLData::ScriptHash::const_iterator gscript_itor = pUIRef->m_kGlobalScriptHash.begin();
		while(gscript_itor != pUIRef->m_kGlobalScriptHash.end())
		{//스크립트 등록
			XUIMgr.RegistGlobalScript(gscript_itor->first, gscript_itor->second);//regist global function
			++gscript_itor;
		}

		tagUIXMLData::ChildHash::const_iterator child_itor = pUIRef->m_kChildHash.begin();
		while(child_itor != pUIRef->m_kChildHash.end())
		{//컨트롤 등록
			tagUIXMLData const& element = (*child_itor).second;
			if(element.Type() == _T("LOCAL"))
			{
				tagUIXMLData::ChildHash const& rkCurChildHash = element.m_kChildHash;
				tagUIXMLData::ChildHash::const_iterator find_local = XUI_ManagerUtil::FindInLocal(m_strLocal, rkCurChildHash);
				if( rkCurChildHash.end() != find_local )
				{
					tagUIXMLData const& rkElement = (*find_local).second;
					CXUI_Wnd *pControl = BuildLocal(&rkElement, pWnd);
				}

				++child_itor;
				continue;
			}
			CXUI_Wnd *pControl = Build(&element, pWnd);
			pWnd->VRegistChild(pControl);
			++child_itor;
		}
	}
	return pWnd;
}

CXUI_Wnd* CXUI_Manager::Build(tagUIXMLData const *pUIRef, CXUI_Wnd* pParent)
{
	CXUI_Wnd *pWnd = CreateElement(pUIRef->Type());

	if(pWnd && pUIRef)
	{
		pWnd->Parent(pParent);
		tagUIXMLData::AttrList::const_iterator attr_itor = pUIRef->m_kAttrList.begin();
		while(attr_itor != pUIRef->m_kAttrList.end())
		{//속성 등록
			pWnd->VRegistAttr((*attr_itor).strName, (*attr_itor).strValue);
			++attr_itor;
		}

		tagUIXMLData::ScriptHash::const_iterator script_itor = pUIRef->m_kScriptHash.begin();
		while(script_itor != pUIRef->m_kScriptHash.end())
		{//스크립트 등록
			pWnd->RegistScript((*script_itor).first, (*script_itor).second);
			++script_itor;
		}

		tagUIXMLData::ScriptHash::const_iterator gscript_itor = pUIRef->m_kGlobalScriptHash.begin();
		while(gscript_itor != pUIRef->m_kGlobalScriptHash.end())
		{//스크립트 등록
			XUIMgr.RegistGlobalScript(gscript_itor->first, gscript_itor->second);//regist global function
			++gscript_itor;
		}
		
		tagUIXMLData::ChildHash::const_iterator child_itor = pUIRef->m_kChildHash.begin();
		while(child_itor != pUIRef->m_kChildHash.end())
		{//컨트롤 등록
			tagUIXMLData const& element = (*child_itor).second;
			if(element.Type() == _T("LOCAL"))
			{
				tagUIXMLData::ChildHash const& rkCurChildHash = element.m_kChildHash;
				tagUIXMLData::ChildHash::const_iterator find_local = XUI_ManagerUtil::FindInLocal(m_strLocal, rkCurChildHash);
				if( rkCurChildHash.end() != find_local )
				{
					tagUIXMLData const& rkElement = (*find_local).second;
					CXUI_Wnd *pControl = BuildLocal(&rkElement, pWnd);
				}

				++child_itor;
				continue;
			}
			CXUI_Wnd *pControl = Build(&element, pWnd);
			pWnd->VRegistChild(pControl);
			++child_itor;
		}
	}
	return pWnd;
}

CXUI_Wnd* CXUI_Manager::Call( ID_TYPE const& ID, bool const bIsModal, ID_TYPE const& rkNewID)
{//콜 됐을때 -> 한번만 draw를 해줘야 되는데. 이걸 되게 하면 땡~.. 그다음에는 텍스쳐에서 뿌려찍기. 스프라이트 애니메이션.
	CXUI_Wnd *pWnd = Create(ID, bIsModal, rkNewID);
	if(pWnd)
	{
		InsertActiveList(pWnd);
		if( bIsModal )
		{
			pWnd->IsModal(bIsModal);
			pWnd->Priority(97);
		}

		pWnd->LastUsedTime(BM::GetTime32());
		VAcquireFocus(pWnd);
		return pWnd;
	}
	return NULL;
}

CXUI_Wnd* CXUI_Manager::Activate(ID_TYPE const& ID, bool const bIsModal, ID_TYPE const& rkNewID)
{
	XUI::CXUI_Wnd *pkWnd = NULL;
	ID_TYPE	IsID;
	if( rkNewID.empty() )
	{
		IsID = ID;
	}
	else
	{
		IsID = rkNewID;
	}

	if(XUIMgr.IsActivate(IsID, pkWnd))//이미 있는가?
	{
		// 가끔 Visible이 아닌채로 사라지기 직전의 녀석을 Activate할 때가 있어서 이런 코드를 넣어놨다.
		if( !pkWnd->Visible() )
		{
			pkWnd->Visible(true);
			pkWnd->VOnCall();
			pkWnd->VAcquireFocus(pkWnd);
		}
		else
		{
			pkWnd->CalledTime( BM::GetTime32() );
		}

		if( bIsModal != pkWnd->IsModal() )
		{
			if( bIsModal )
			{
				pkWnd->IsModal(bIsModal);
				pkWnd->Priority(97);
			}
		}
	}
	else
	{
		pkWnd = XUIMgr.Call(ID, bIsModal, rkNewID);
	}
	return pkWnd;
}

CXUI_Wnd* CXUI_Manager::PickWnd(ID_TYPE const& ID)
{
	std::wstring wstrID = ID;
	UPR(wstrID);

	CXUI_Wnd* pkResultWnd = NULL;
	WND_LIST::iterator itor = m_lstActive.begin();
	while( m_lstActive.end() != itor )//전체목록에서 찾아져야된다.
	{
		WND_LIST::value_type element = (*itor);
		if(element->ID() == wstrID)
		{
			pkResultWnd = element;
			break;
		}
		++itor;
	}

	if( pkResultWnd )
	{
		m_lstActive.erase(itor);//지우고
		InsertActiveList(pkResultWnd);//다시 넣음.
		return pkResultWnd;
	}
	return NULL;
}

CXUI_Wnd* CXUI_Manager::Create(ID_TYPE const& ID, bool const bIsModal, ID_TYPE const& rkNewID)//여기서 ActivateList에 넣어서는 안됨.
{
	bool bIsVirgin = true;
	std::wstring wstrID = ID;
	UPR(wstrID);

	CXUI_Wnd *pWnd = NULL;

	if(!bIsModal)
	{
		XUI_HASH::iterator buf_itor = m_kBufferdHash.find(wstrID);
		if(buf_itor != m_kBufferdHash.end())
		{//같은거 들어갈때 으뜨케.
			pWnd = (*buf_itor).second;
			assert(pWnd);
			if (pWnd)
			{
				pWnd->IsModal(bIsModal);
			}
			bIsVirgin = false;
		}
	}
	
	tagUIXMLData *pUIData = m_kXMLData.GetChild(wstrID);

	if(pUIData)
	{
		if(!pWnd)
		{
			pWnd = Build(pUIData);

			assert(pWnd);
			
			if( !pWnd->Parent() )
			{// 기본 해상도와의 차이만큼 보정을 해줘야 한다. 이전 해상도 아님!!( GetGab()은 이전해상도와의 차이 )
				POINT2 ptGab;
				ptGab.x = GetResolutionSize().x - XUI::EXV_DEFAULT_SCREEN_WIDTH;
				ptGab.y = GetResolutionSize().y - XUI::EXV_DEFAULT_SCREEN_HEIGHT;

				if( pWnd->AlignX() < 0 && pWnd->IsAbsoluteX() == false )
				{
					pWnd->Location( pWnd->Location().x + ptGab.x, pWnd->Location().y );
				}
				if( pWnd->AlignY() < 0 && pWnd->IsAbsoluteY() == false )
				{
					pWnd->Location( pWnd->Location().x, pWnd->Location().y + ptGab.y );
				}
			}

			if(!bIsModal && pWnd)
			{//모달리스는 버퍼로
				auto ret = m_kBufferdHash.insert(std::make_pair(wstrID, pWnd));
				assert(ret.second);
			}
		}

		if( !rkNewID.empty() )
		{
			pWnd = Build(pUIData);
			assert(pWnd);
			
			pWnd->ID(rkNewID);
			bIsVirgin = true;
		}

		if(pWnd)
		{
			if(bIsVirgin)
			{
				pWnd->VInit();
				pWnd->DoScript(SCRIPT_ON_CORRECT);
			}

			if( CheckExistUniqueLiveType( pWnd->UniqueExistType() ) )
			{
				if( m_pUniqueExistCheckNotice )
				{
					m_pUniqueExistCheckNotice( pWnd->UniqueExistType() );
					pWnd->Close(false);
					return NULL;
				}
			}

			pWnd->VAlign();
			pWnd->VScale(1);
			pWnd->VOnCall();
			pWnd->Visible( true );//보이기를 끄고 넣어주면 display가 부른다.
			return pWnd;
		}
	}

	OutputDebugString(ID.c_str());
	OutputDebugString(TEXT(" Create Failed\n"));
	return NULL;
}

CXUI_Wnd* CXUI_Manager::AddChildUI(CXUI_Wnd * const pkParent, ID_TYPE const& rkElementID, ID_TYPE const& rkNewID, bool const bRemove)
{
	CXUI_Wnd *pWnd = pkParent->GetControl(rkNewID);
	if( pWnd )
	{
		return pWnd;
	}
	
	pWnd = CXUI_Manager::Create(rkElementID, true, rkNewID);
	if (pWnd)
	{
		if( pkParent->VRegistChild(pWnd) )
		{
			pWnd->IsOutsideWnd(true);
			pkParent->IsRemoveOutside(bRemove);
			return pWnd;
		}
	}
	return NULL;
}

bool SortPriority(CXUI_Wnd const *pkLeft, CXUI_Wnd const *pkRight)
{
	return pkLeft->Priority() > pkRight->Priority();
}

void CXUI_Manager::InsertActiveList(CXUI_Wnd *pWnd)
{
	if (NULL == pWnd)
	{
		return;
	}

	pWnd->UseNewOffscreen(pWnd->UseOffscreen());

	if(!pWnd->IsModal())
	{
		WND_LIST::iterator itor = m_lstActive.begin();
		while( m_lstActive.end() != itor )
		{
			WND_LIST::value_type &element = (*itor);
			if(pWnd->ID() == element->ID())
			{
				return;//모달리스는 두번 insert 안함.
			}
			++itor;
		}
	}

	if(false==m_bHideUI_Add_LastContainer || pWnd->NotHideWnd())
	{
		m_lstActive.push_front(pWnd);
		m_lstActive.sort( SortPriority );
	}
	else
	{
		PushHide(pWnd);
	}

	pWnd->DoScript(SCRIPT_ON_ACTIVATE);
}

bool CXUI_Manager::Close( ID_TYPE const& ID )
{
	std::wstring wstrID = ID;

	UPR(wstrID);

	WND_LIST::iterator itor = m_lstActive.begin();
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &element = (*itor);
		if( element->ID() == wstrID )
		{
			element->Close();
			return true;
		}
		++itor;
	}

	if( !EmptyHide() )
	{
		ContWndIDList::iterator find_iter = std::find( m_kReserveCloseSet.begin(), m_kReserveCloseSet.end(), ID );
		if( m_kReserveCloseSet.end() == find_iter )
		{
			m_kReserveCloseSet.push_front( ID );
		}
	}
	return false;
}

void CXUI_Manager::CloseAll()
{
	while( 0 != ShowAllUI() )
	{
		// 감춰진 모든 UI를 불러들이고
	}

	WND_LIST::iterator itor = m_lstActive.begin();
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &element = (*itor);
		element->Close();
		++itor;
	}
}

bool CXUI_Manager::ReadXML(std::wstring const& wstrFileName)
{
	TiXmlDocument doc( MB(wstrFileName) );

	bool loadOkay = doc.LoadFile();
	if ( !loadOkay )
	{
		return false;
	}

	return ParseXML(doc);
}

bool CXUI_Manager::ParseXML(TiXmlDocument &rkDoc)
{
	TiXmlNode* node = rkDoc.FirstChild();
	ExplorerNode( node, &m_kXMLData );
	return true;
}

void CXUI_Manager::ClearXMLData()
{
	m_kXMLData.Clear();
}

bool CXUI_Manager::InitXUI(const XUI_INIT_INFO &rInfo, bool bInitIme)
{
	if( !rInfo.pRscMgr 
	||	!rInfo.pRenderer
	||	!rInfo.pScriptFunc
	||	!rInfo.pMediaPlayer
	||	!rInfo.pHotKeyModifier
	||	!rInfo.hWnd
	||	!rInfo.hInstance
	)
	{
		return false;
	}

	m_spRscMgr = rInfo.pRscMgr;
	m_spRenderer = rInfo.pRenderer;
	m_spMediaPlayer = rInfo.pMediaPlayer;
	m_spHotKeyModifier = rInfo.pHotKeyModifier;
	m_pScriptFunc = rInfo.pScriptFunc;
	m_pCursorCallBackFunc = rInfo.pCursorCallBackFunc;
	m_pTextClickCallBackFunc = rInfo.pTextClickCallBackFunc;
	m_spHotKeyToRealKey = rInfo.pHotKeyToRealKey;
	m_pUniqueExistCheckNotice = rInfo.pUniqueExistCheckNotice;
	m_pLogFunc = rInfo.pLogFunc;

	m_spTextTable = rInfo.pTextTable;
	m_sHwnd = rInfo.hWnd;

	if (bInitIme)
	{
		return InitIME(rInfo.hWnd, rInfo.hInstance);
	}

	return true;
}

void CXUI_Manager::Terminate()
{
#ifdef CHECK_XUI_PERFORMANCE
	CXUI_Wnd::m_kXUIStatGroup.PrintStatGroupForCSV();
#endif
}

bool CXUI_Manager::InitIME(HWND hWnd, HINSTANCE hInstance)
{
	bool bRet1 = false;
	bool bRet2 = false;
	if( g_kSingleIME.Init( hWnd, hInstance, 512, 0, m_pLogFunc ) )
	{
		g_kSingleIME.FuncBind(VK_RETURN, ExecuteVK_KEY);
		g_kSingleIME.FuncBind(VK_TAB, ExecuteVK_KEY);
		g_kSingleIME.FuncBind(VK_UP, ExecuteVK_KEY);
		g_kSingleIME.FuncBind(VK_DOWN, ExecuteVK_KEY);
		g_kSingleIME.SetShowCandidate(false);
		bRet1 = true;
	}
	if( g_kMultiIME.Init( hWnd, hInstance, 512, WS_CHILD | ES_WANTRETURN | ES_MULTILINE, m_pLogFunc ) )
	{
		//g_kMultiIME.FuncBind(VK_RETURN, ExecuteVK_KEY);
		g_kMultiIME.FuncBind(VK_TAB, ExecuteVK_KEY);
		g_kMultiIME.SetShowCandidate(false);
		bRet2 = true;
	}

	HWNDforIME(hWnd);
	HINSTANCEforIME(hInstance);
	return bRet1 && bRet2;
}

bool CXUI_Manager::BuildXUI()
{
	m_spConfig = NULL;

	std::wstring CONFIG_STRING = _T("CONFIG");
	tagUIXMLData *pConfigXmlData = m_kXMLData.GetChild(CONFIG_STRING);
	if(pConfigXmlData)
	{
		CXUI_Wnd* pConfig = Create(CONFIG_STRING, true);
		m_spConfig = dynamic_cast<CXUI_Config*>(pConfig);
	}

	InitControls();
	ApplyAlign();

	return true;
}

bool IsIncludeName(std::wstring strFull, std::wstring strName)
{
	std::wstring const rkCurLocalStr = strFull;
	typedef std::list< std::wstring > ContWstr;
	ContWstr kList;
	BM::vstring::CutTextByKey(rkCurLocalStr, std::wstring(L"/"), kList);
	ContWstr::iterator itor = kList.begin();
	for( ; itor != kList.end(); ++itor )
	{
		if( (*itor) == strName )
		{
			return true;
		}
	}

	return false;
}

void CXUI_Manager::ExplorerNode(void *p, PgUIXMLData* pParent)
{
	if( !p ){ return; }

	TiXmlNode *pNode = (TiXmlNode *)p;

	int const iType = pNode->Type();
	TiXmlNode::NodeType const eType = (TiXmlNode::NodeType)iType;
	switch(eType)
	{
	case TiXmlNode::DOCUMENT:
	case TiXmlNode::COMMENT:
	case TiXmlNode::UNKNOWN:
	case TiXmlNode::DECLARATION:
	case TiXmlNode::TYPECOUNT:
		{//이런 타입은 엘레멘트를 안탄다.
		}break;
	case TiXmlNode::TEXT:
		{
			if( pParent )//텍스트를 포함한 윗단계를 의미한다. SCRIPT 를 제외 하고는 TEXT 를 쓰면 안된다.
			{
				PgUIXMLData *pGrandParent = pParent->Parent();//본인은 사용하질 않기때문에. 부모에게 넘기고, 본인은 지워야 한다
				
				
				if(ROOT_HOTKEY==pParent->Type() || ROOT_SCRIPT==pParent->Type())
				{
					pGrandParent->RegistScript(pParent->ID(), UNI(pNode->Value()));
				}
				else if(ROOT_GLOBAL_HOTKEY==pParent->Type())
				{
					pGrandParent->RegistGlobalScript(pParent->ID(), UNI(pNode->Value()));
				}
				else
				{
					assert(NULL && "XUI is can't accept XML Text");

				}

				std::wstring const& wstrChildID = pParent->ID();//grandparent 입장에서는 parent 도 child
				pGrandParent->RemoveChild(wstrChildID);//이 위에놈은 없에버려
			}
		}break;
	default:
		{	//노드를 엘레멘트화 시켜서 Explorer 하면 Attr도 등록이 되나?.
			E_XUI_TYPE const type = TagToType( UNI(pNode->Value()) );
			
			switch(type)
			{
			case E_XUI_ATTR:
			case E_XUI_ROOT:
				{
					ExplorerAttr(pNode, pParent);//본인 속성.
					
					TiXmlNode * pSubNode = pNode->FirstChild();
					if( pSubNode )
					{//자식 검색
						ExplorerNode(pSubNode, pParent);
					}
				}break;
			case E_XUI_HOTKEY:
				{
					if( !CheckHotKeyToModifier(pNode, pParent) )
					{
						assert(0);
					}
				}break;
			case E_XUI_HOTKEY_MODIFIER:
				{
					TiXmlElement* pElement = (TiXmlElement*)pNode;
					if( pElement )
					{
						TiXmlAttribute* pAttr = pElement->FirstAttribute();
						int	iID = 0;
						std::wstring wstrAddStr;

						while(pAttr)
						{
							std::wstring wstrName(UNI(pAttr->Name()));
							BM::vstring vstrValue(UNI(pAttr->Value()));
							if( wstrName == ATTR_ID )
							{
								iID = (int const)vstrValue;
							}
							else if( wstrName == ATTR_ADD_STR )
							{
								wstrAddStr = (std::wstring const&)vstrValue;
							}

							pAttr = pAttr->Next();
						}
						bool const bRet = m_spHotKeyModifier->AddModifierKey(iID, wstrAddStr);
					}
				}break;
			case E_XUI_LOCAL:
				{
					TiXmlNode* pSubNode = pNode->FirstChild();
					bool bFindLocal = false;
					bool bFindDefault = false;
					while(pSubNode)
					{
						PgUIXMLData kXMLData;
						kXMLData.Type(UNI(pSubNode->Value()));

						ExplorerAttr(pSubNode, &kXMLData );
						TiXmlNode* pSubNode2 = pSubNode->FirstChild();
						if(pSubNode2)
						{
							if( IsIncludeName(kXMLData.m_kAttrList.front().strValue, LocalName()) )
							{
								bFindLocal = true;
							}

							if(kXMLData.m_kAttrList.front().strValue == _T("DEFAULT"))
							{
								bFindDefault = true;
							}
						}
						pSubNode = pSubNode->NextSibling();
					}

					pSubNode = pNode->FirstChild();
					if(bFindLocal)
					{
						while(pSubNode)
						{
							PgUIXMLData kXMLData;
							kXMLData.Type(UNI(pSubNode->Value()));

							ExplorerAttr(pSubNode, &kXMLData );
							TiXmlNode* pSubNode2 = pSubNode->FirstChild();
							if(pSubNode2)
							{
								if( IsIncludeName(kXMLData.m_kAttrList.front().strValue, LocalName()) )
								{
									ExplorerNode(pSubNode2, pParent);
									break;
								}
							}
							pSubNode = pSubNode->NextSibling();
						}
					}
					else if(!bFindLocal && bFindDefault)
					{
						while(pSubNode)
						{
							PgUIXMLData kXMLData;
							kXMLData.Type(UNI(pSubNode->Value()));

							ExplorerAttr(pSubNode, &kXMLData );
							TiXmlNode* pSubNode2 = pSubNode->FirstChild();
							if(pSubNode2)
							{
								if(kXMLData.m_kAttrList.front().strValue == _T("DEFAULT"))
								{
									ExplorerNode(pSubNode2, pParent);
									break;
								}
							}
							pSubNode = pSubNode->NextSibling();
						}
					}
				}break;
			case E_XUI_UNIQUE_UI_TYPE_TO_ERRNO:
				{
					TiXmlElement* pElement = (TiXmlElement*)pNode;
					if( pElement )
					{
						TiXmlAttribute* pAttr = pElement->FirstAttribute();

						std::wstring wstrID;
						SUniqueUITypeErrInfo kErrInfo;

						while(pAttr)
						{
							std::wstring wstrName(UNI(pAttr->Name()));
							BM::vstring vstrValue(UNI(pAttr->Value()));
							if( wstrName == ATTR_ID )
							{
								wstrID = (std::wstring const&)vstrValue;
							}
							else if( wstrName == ATTR_ERR_NO )
							{
								kErrInfo.iTT_No = static_cast<int>(vstrValue);
							}
							else if( wstrName == ATTR_ERR_SOUND )
							{
								kErrInfo.kErrSoundPath = (std::wstring const&)vstrValue;
							}

							pAttr = pAttr->Next();
						}
						m_kUniqueUITypeErrCont.insert(std::make_pair(wstrID, kErrInfo));
					}
				}break;
			default:
				{//기본적으로 다른 컨트롤은 차일드 등록을한다.
					PgUIXMLData kXMLData;
					kXMLData.Type(UNI(pNode->Value()));

					ExplorerAttr(pNode, &kXMLData );//본인 속성.

					PgUIXMLData *pRet = pParent->RegistChild(kXMLData.ID(), kXMLData);//여기서 등록
					if(!pRet)
					{
						assert(NULL);
						break;
					}

					TiXmlNode * pSubNode = pNode->FirstChild();
					if( pSubNode )
					{//생성된 녀석의 자식 검색
						ExplorerNode(pSubNode, pRet);
					}
				}break;
			}
		}break;
	}
//재귀호출 형이므로 아래와 같이 처리한다.
	TiXmlNode* pNextNode = pNode->NextSibling();
	if(pNextNode)
	{
		ExplorerNode( pNextNode, pParent );
	}
}

bool CXUI_Manager::CheckHotKeyToModifier(TiXmlNode *pNode, PgUIXMLData* pParent)
{
	PgUIXMLData kXMLData;
	kXMLData.Type(UNI(pNode->Value()));

	TiXmlElement *pElement = (TiXmlElement *)pNode;
	if( pElement )
	{
		TiXmlAttribute* pAttr = pElement->FirstAttribute();
		std::wstring wstrID = UNI(pAttr->Name());
		std::wstring wstrVal;
		while(pAttr)
		{			
			std::wstring wstrKey(UNI(pAttr->Name()));
			std::wstring wstrValue(UNI(pAttr->Value()));
			pAttr = pAttr->Next();

			if(ATTR_ID == wstrKey)
			{
				wstrVal = wstrValue;
				continue;
			}

			if( !m_spHotKeyModifier->SetModifierState(wstrKey, true) )
			{
				assert(0);
				return false;
			}
		}
		wstrVal = m_spHotKeyModifier->GetModifierName() + wstrVal;
		m_spHotKeyModifier->ReSetModifierState();
		kXMLData.RegistAttr(wstrID, wstrVal);
	}

	PgUIXMLData *pRet = pParent->RegistChild(kXMLData.ID(), kXMLData);//여기서 등록
	if(!pRet)
	{
		assert(NULL);
		return false;
	}

	TiXmlNode * pSubNode = pNode->FirstChild();
	if( pSubNode )
	{//생성된 녀석의 자식 검색
		ExplorerNode(pSubNode, pRet);
	}
	return true;
}

void CXUI_Manager::ExplorerAttr(void *pNode, PgUIXMLData* pTarget)
{//해달 엘리먼트의 속성치 부여.

	TiXmlElement *pElement = (TiXmlElement *)pNode;
	if( pElement && pElement->Type() != TiXmlNode::COMMENT)
	{
		TiXmlAttribute* pAttr = pElement->FirstAttribute();
		while(pAttr)
		{
			std::wstring wstrName(UNI(pAttr->Name()));
			std::wstring wstrValue(UNI(pAttr->Value()));
			
			if(ATTR_ID == wstrName)
			{
				UPR(wstrValue);
			}

			if( pTarget->RegistAttr(wstrName, wstrValue) )
			{
				pAttr = pAttr->Next();
				continue;
			}
			assert(NULL && __FUNCTION__);
			break;
		}
	}
}

void CXUI_Manager::ReserveCreator()
{//스트링이 Wnd 상속받은 객체로 만들수 있는것만 등록. 나머지는 E_XUI_NONE
	m_mapKeyWord.insert( std::make_pair(ROOT_XUI,		E_XUI_ROOT) );//특별
	m_mapKeyWord.insert( std::make_pair(ROOT_ATTR,		E_XUI_ATTR) );//특별
	m_mapKeyWord.insert( std::make_pair(ROOT_HOTKEY_MODIFIER, E_XUI_HOTKEY_MODIFIER));
	m_mapKeyWord.insert( std::make_pair(ROOT_LOCAL, E_XUI_LOCAL));
	m_mapKeyWord.insert( std::make_pair(ROOT_UNIQUE_UI_TYPE_TO_ERRNO, E_XUI_UNIQUE_UI_TYPE_TO_ERRNO));

	RegistCreator(ROOT_FORM, CXUI_Form());
	RegistCreator(ROOT_DUMMY, CXUI_Dummy());
	RegistCreator(ROOT_DIALOG, CXUI_Dialog());
	RegistCreator(ROOT_BUTTON, CXUI_Button());
	RegistCreator(ROOT_ANI_BAR, CXUI_AniBar());
	RegistCreator(ROOT_EDIT, CXUI_Edit());
	RegistCreator(ROOT_EDIT_MULTI, CXUI_Edit_MultiLine());
	RegistCreator(ROOT_SCRIPT, CXUI_Script());
	RegistCreator(ROOT_HOTKEY, CXUI_HotKey());
	RegistCreator(ROOT_GLOBAL_HOTKEY, CXUI_GlobalHotkey());
	RegistCreator(ROOT_IMG, CXUI_Image());
	RegistCreator(ROOT_TAB, CXUI_Tab());
	RegistCreator(ROOT_TAB_ELEMENT, CXUI_Tab_Element());
	RegistCreator(ROOT_BUILD, CXUI_Builder());
	RegistCreator(ROOT_ICON, CXUI_Icon());
	RegistCreator(ROOT_CURSOR, CXUI_Cursor());
	RegistCreator(ROOT_MSG_BOX, CXUI_MsgBox());
	RegistCreator(ROOT_TOOL_TIP, CXUI_ToolTip());
	RegistCreator(ROOT_WARNING, CXUI_Warning());
	RegistCreator(ROOT_CHECK_BUTTON, CXUI_CheckButton());
	RegistCreator(ROOT_LIST, CXUI_List());//List 는 Button을 가지므로 Button 아래에 포진.
	RegistCreator(ROOT_LIST2, CXUI_List2());
	RegistCreator(ROOT_TREE, CXUI_Tree());//트리는 Checkbutton을 가지므로 CXUI_CheckButton 아래에 포진.
	RegistCreator(ROOT_CONFIG, CXUI_Config());//트리는 Checkbutton을 가지므로 CXUI_CheckButton 아래에 포진.
	RegistCreator(ROOT_STATIC_FORM, CXUI_StaticForm());
	RegistCreator(ROOT_HSCROLL, CXUI_HScroll());

	RegistCreator(ROOT_FULL_FORM, CXUI_Full_Form());
}

//생성 명령을 내릴때.
// Key(String)을 쏘고. 해당 String 에.
// 클래스 생성 함수가 엮임.

// 즉. -> ID, Create 함수 포인터를 엮어줘야함.

CXUI_Wnd* CXUI_Manager::CreateElement(ID_TYPE const& TAG)//클래스 껍데기 생성.
{
	XUI_HASH::iterator buf_itor =  m_mapCreator.find(TAG);

	if(buf_itor != m_mapCreator.end())
	{
		return (*buf_itor).second->VCreate();
	}
	
	assert(NULL);
	return NULL;
}

bool CXUI_Manager::RegistCreator(ID_TYPE const& ID, CXUI_Wnd &rkBase)
{
	std::wstring wstrID = ID;
	UPR(wstrID);

	auto ret = m_mapCreator.insert(std::make_pair(wstrID, rkBase.VCreate()));

	if(!ret.second)
	{	
		assert(NULL);
		return false;
	}

	auto keyword_ret = m_mapKeyWord.insert( std::make_pair(wstrID,	rkBase.VType()) );

	if(!keyword_ret.second)
	{	
		assert(NULL);
		return false;
	}

	return true;
}

CXUI_Manager::KEY_WORD_HASH::mapped_type CXUI_Manager::TagToType(const KEY_WORD_HASH::key_type &strTag)
{
	KEY_WORD_HASH::iterator itor = m_mapKeyWord.find( strTag );
	
	if( m_mapKeyWord.end() != itor )
	{
		return (*itor).second;
	}
	assert(NULL);
	return E_XUI_NONE;
}

void CXUI_Manager::VScale( float const scale )
{
//	for_each( m_mapIF.begin(), m_mapIF.end(), Scale_Func(scale) );	
}

void CXUI_Manager::InitControls()
{
//	for_each( m_mapIF.begin(), m_mapIF.end(), Init_Func() );
}

void CXUI_Manager::ApplyAlign()
{
//	for_each( m_mapIF.begin(), m_mapIF.end(), Align_Func() );	
}

void CXUI_Manager::VOnTick()
{
	DWORD const dwCurTime = BM::GetTime32();

	m_FrameCount++;
	WND_LIST::iterator itor = m_lstActive.begin();
	
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &pElement = (*itor);
		pElement->VOnTick(dwCurTime);
		++itor;
	}
	
//	if(m_pToolTip!=NULL)
//		m_pToolTip->VOnTick();
//	if(m_pWarning!=NULL)
//		m_pWarning->VOnTick();
}

void CXUI_Manager::VRefresh()
{
	XUI_HASH::iterator itor = m_kBufferdHash.begin();

	while( m_kBufferdHash.end() != itor )
	{
		if (itor->second != NULL)
			itor->second->VRefresh();
		++itor;
	}
}

bool CALLBACK CXUI_Manager::VHookMessage(HWND &hWnd, UINT &message, WPARAM &wParam, LPARAM &lParam, bool& bSkipDefProc)
{
	bool bIsDontPost = true;
	if (NULL == CXUI_Edit::GetFocusedEdit())
	{
		return false;
	}

	CXUI_Edit* pkFocusedEdit = CXUI_Edit::GetFocusedEdit();

	bIsDontPost = false;
	
	E_XUI_TYPE const kType = CXUI_Edit::GetFocusedEdit()->VType();

	bool bRet = false;
	if(E_XUI_EDIT == kType)
	{
		bRet = g_kSingleIME.HookMessage( hWnd, message, wParam, lParam, bIsDontPost, bSkipDefProc);
	}
	else if(E_XUI_EDIT_MULTILINE == kType)
	{
		bRet = g_kMultiIME.HookMessage( hWnd, message, wParam, lParam, bIsDontPost, bSkipDefProc);
	}

	if( bRet )//내부에서 포커스가 변할 수 있다.
	{
		if (CXUI_Edit::GetFocusedEdit() != pkFocusedEdit && CXUI_Edit::m_spFocusedEdit != NULL)
		{
			CXUI_Edit::GetFocusedEdit()->OnHookEvent();
			return true;
		}
	}

	return false;
}

void CXUI_Manager::VOnResize(POINT2 const& ptScrSize)
{
	SetResolutionSize( ptScrSize );
	ApplyAlign();//얼라인 조절
}
/*
bool CXUI_Manager::GetControlValue(ID_TYPE const& ParentID, ID_TYPE const& ControlID, EValuetype const eVT, std::wstring &wstrOut)
{
	std::wstring wstrID = ParentID;

	UPR(wstrID);

	WND_LIST::iterator itor = m_lstActive.begin();
	
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &pElement = (*itor);
		if( pElement->ID() == wstrID )
		{
			CXUI_Wnd *pControl = pElement->GetControl(ControlID);
			if( pControl)
			{
				return pControl->GetValue( eVT, wstrOut );
			}
			else
			{//더 찾을 필요 없잖아.
				break;
			}
		}
		++itor;
	}
	return false;
}

bool CXUI_Manager::SetControlValue(ID_TYPE const& ParentID, ID_TYPE const& ControlID, EValuetype const eVT, std::wstring const& wstrValue)
{
	std::wstring wstrID = ParentID;

	UPR(wstrID);

	WND_LIST::iterator itor = m_lstActive.begin();
	
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &pElement = (*itor);
		if( pElement->ID() == wstrID )
		{
			CXUI_Wnd *pControl = pElement->GetControl(ControlID);
			if( pControl)
			{
				return pControl->SetValue( eVT, wstrValue );
			}
			else
			{//더 찾을 필요 없잖아.
				break;
			}
		}
		++itor;
	}
	return false;
}
*/
CXUI_Wnd*	CXUI_Manager::GetFocusedEdit()
{
	return	dynamic_cast<CXUI_Wnd*>(CXUI_Edit::GetFocusedEdit());
}

bool CXUI_Manager::SetEditFocus(ID_TYPE const& ParentID, ID_TYPE const& ControlID)
{
	std::wstring wstrID = ParentID;

	UPR(wstrID);

	WND_LIST::iterator itor = m_lstActive.begin();
	
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &pElement = (*itor);
		if( pElement->ID() == wstrID )
		{
			CXUI_Wnd *pControl = pElement->GetControl(ControlID);
			if(pControl)
			{
				if( E_XUI_EDIT == pControl->VType() )
				{
					return pControl->SetEditFocus(false);
				}
			}
			else
			{//더 찾을 필요 없잖아.
				break;
			}
		}
		++itor;
	}
	return false;
}

bool CXUI_Manager::SendScriptEvent(ID_TYPE const& ParentID, ID_TYPE const& ControlID, std::wstring const& wstrScriptKey)
{
	std::wstring wstrID = ParentID;

	UPR(wstrID);

	WND_LIST::iterator itor = m_lstActive.begin();
	
	while( m_lstActive.end() != itor )
	{
		WND_LIST::value_type &pElement = (*itor);
		if( pElement->ID() == wstrID )
		{
			CXUI_Wnd *pControl = pElement->GetControl(ControlID);
			if (ControlID.empty())
			{
				pControl = pElement;	//ControlID 에 아무것도 안적으면 ParentID의 스크립트를 실행
			}
			if( pControl)
			{
				return pControl->DoHotKey(wstrScriptKey);
			}
			else
			{//더 찾을 필요 없잖아.
				break;
			}
		}
		++itor;
	}
	return false;
}

bool CXUI_Manager::ClearEditFocus()
{
	if (CXUI_Edit::GetFocusedEdit() == CXUI_Wnd::m_spWndMouseFocus)
	{
		if (CXUI_Wnd::m_spWndMouseFocus)
		{
			CXUI_Wnd::m_spWndMouseFocus->VLoseFocus();
		}
	}
	CXUI_Edit::SetFocusedEdit(NULL);
	return true;
}

/*! 
	\brief UI해쉬안의 모든 컨트롤러들을 제거하고 메모리를 해제한다.
    \param 
	\last update 2006.12.20 by 하승봉
*/
void CXUI_Manager::ClearAllControl()
{
	CXUI_Wnd::m_spWndMouseOver = NULL;
	CXUI_Wnd::m_spWndMouseFocus = NULL;
	CXUI_Wnd::m_spWndScrollBtn = NULL;
	CXUI_Dummy::m_spDummyMouseOver = NULL;

	m_SavedList.clear();								//메모리를 날리기전에 현재 동작중인 애들 인덱스 저장
	
	WND_LIST::iterator it = m_lstActive.begin();
	while( m_lstActive.end() != it )
	{
		m_SavedList.push_back( (*it)->ID() );
		++it;
	}

	CXUI_Edit::ReleaseGlobalImage();	// static variable에 저장되어 있는 image날리기.	
	CloseAll();
	VDisplay();//닫기후 

	XUI_HASH::iterator buf_itor = m_kBufferdHash.begin();

	while( m_kBufferdHash.end() != buf_itor )
	{
		XUI::CXUI_Wnd* pkWnd = (*buf_itor).second;
		if(pkWnd && pkWnd->VType() != E_XUI_CURSOR)
		{
			pkWnd->IsClosed(true);	//종료시 ON_CLOSE스크립트가 한번 더 호출 되지 않도록 수정. 진짜 종료니까
			
			SAFE_DELETE(pkWnd);
		}
		else
		{
			CXUI_Cursor* pCursor = dynamic_cast<CXUI_Cursor*>(pkWnd);
			if (pCursor)
				pCursor->ReleaseDefaultImg();
		}
		++buf_itor;
	}
	m_lstActive.clear();
	m_kReserveCloseSet.clear();
	m_kBufferdHash.clear();
	m_kUniqueUITypeErrCont.clear();
	m_bBlockGlobalScript = false;
	m_bHideUI_Add_LastContainer = false;
}

void CXUI_Manager::ReleaseAllControl()
{
	CXUI_Wnd::m_spWndMouseOver = NULL;
	CXUI_Wnd::m_spWndMouseFocus = NULL;
	CXUI_Wnd::m_spWndScrollBtn = NULL;

	m_SavedList.clear();
	m_lstActive.clear();
	m_kReserveCloseSet.clear();

	CloseAll();
	VDisplay();//닫기후 

	XUI_HASH::iterator buf_itor = m_kBufferdHash.begin();

	while( m_kBufferdHash.end() != buf_itor )
	{
		if(buf_itor->second->VType() != E_XUI_CURSOR)
		{
			SAFE_DELETE((*buf_itor).second);
			buf_itor = m_kBufferdHash.erase(buf_itor);
		}
		else
			++buf_itor;
	}
	m_kUniqueUITypeErrCont.clear();
}

void CXUI_Manager::FlushUnusedControls()
{
	DWORD curTime = BM::GetTime32();
	if (curTime - m_dwLastXUIFlushTime >= 2500)
	{
		XUI_HASH::iterator buf_itor = m_kBufferdHash.begin();

		while (m_kBufferdHash.end() != buf_itor)
		{
			bool bDelete = false;
			CXUI_Wnd* pWnd = buf_itor->second;
			if (pWnd && pWnd->IsClosed() && curTime - pWnd->LastUsedTime() > 60000)
			{
				if(pWnd->VType() != E_XUI_CURSOR)
				{
#ifndef EXTERNAL_RELEASE
					bool bFound = false;
					for (WND_LIST::iterator it = m_lstActive.begin(); it != m_lstActive.end(); it++)
					{
						if (*it == pWnd)
						{
							bFound = true;
							wchar_t buf[256];
							swprintf(buf, 256, L"Unused Control in ActiveList %s, %d\n", pWnd->ID().c_str(), curTime - pWnd->LastUsedTime());
							OutputDebugString(buf);
							break;
						}
					}
					assert(bFound);
					if (bFound)
					{
						++buf_itor;
						continue;
					}
#endif
					wchar_t buf[256];
					swprintf(buf, 256, L"Unused Control removed %s, %d\n", pWnd->ID().c_str(), curTime - pWnd->LastUsedTime());
					OutputDebugString(buf);

					SAFE_DELETE(pWnd);
					bDelete = true;
					buf_itor = m_kBufferdHash.erase(buf_itor);
				}
			}

			if (bDelete == false)
				++buf_itor;
		}
	}
}

/*! 
	\brief 저장된 액티브리스트 인덱스드를 다시 콜.
    \param 
	\last update 2006.12.20 by 하승봉
*/
void CXUI_Manager::ReCallControl()
{
	CXUI_Edit::ClearGroupEdit();//저장된 포커스 그룹 날림.
	
	POINT2 ptGab;
	ptGab.x = GetResolutionSize().x - XUI::EXV_DEFAULT_SCREEN_WIDTH;
	ptGab.y = GetResolutionSize().y - XUI::EXV_DEFAULT_SCREEN_HEIGHT;
	SetGab( ptGab );

	SAVED_ACTIVEID_LIST::reverse_iterator itor = m_SavedList.rbegin();	//저장된 순서 반대로 콜해야 함.
	while( m_SavedList.rend() != itor )
	{
		Call((*itor));
		++itor;
	}
}

void CXUI_Manager::PrintControls()
{
	wchar_t buf[1024];
	int index = 0;
	WND_LIST::iterator it = m_lstActive.begin();
	while( m_lstActive.end() != it )
	{
		swprintf(buf, 1024, L"ActiveList %d:%s\n", index, (*it)->ID().c_str());
		OutputDebugString(buf);
		++it;
		index++;
	}

	XUI_HASH::iterator buf_itor = m_kBufferdHash.begin();

	index = 0;
	while( m_kBufferdHash.end() != buf_itor )
	{
		swprintf(buf, 1024, L"BufferedHash %d:%s(%d)\n", index, buf_itor->second->ID().c_str(), buf_itor->first);
		OutputDebugString(buf);
		++buf_itor;
		index++;
	}
}

int CXUI_Manager::HideAllUI(bool const bAdd_LastContainer)
{
	if(false==m_bHideUI_Add_LastContainer)
	{
		m_bHideUI_Add_LastContainer = bAdd_LastContainer;
	}

	WND_LIST::iterator itor = m_lstActive.begin();
	int iModalCount=0;
	while( m_lstActive.end() != itor )
	{
		const WND_LIST::value_type &pElement = (*itor);
		if(pElement != NULL && pElement->ID() != STR_XUI_CURSOR )
		{
			pElement->Visible(false);
		}
		++itor;
	}

	PushHide( m_lstActive );

	itor = m_lstActive.begin();
	while( m_lstActive.end() != itor )
	{
		const WND_LIST::value_type &pElement = (*itor);
		if(pElement != NULL && pElement->ID() != STR_XUI_CURSOR )
		{
			if( pElement->IsModal() )//지금 요놈이 모달이면
			{
				++iModalCount;
				itor = m_lstActive.erase(itor);
				continue;
			}
			
			if( m_kBufferdHash.end() == m_kBufferdHash.find(pElement->ID()) ) // 새로운 아이디로 복사되서 호출된 UI면
			{
				itor = m_lstActive.erase(itor);
				continue;
			}
		}
		++itor;
	}

	return (int)m_lstActive.size() + iModalCount;
}

int CXUI_Manager::ShowAllUI()
{
	m_bHideUI_Add_LastContainer = false;

	WND_LIST kHideList;
	PopHide( kHideList );

	if( kHideList.empty() )
	{
		return 0;
	}

	WND_LIST::const_iterator itor = kHideList.begin();
	while( kHideList.end() != itor )
	{
		const WND_LIST::value_type &pElement = (*itor);
		if(pElement != NULL && pElement->ID() != STR_XUI_CURSOR && !pElement->IsClosed() )
		{
			pElement->Visible(true);
			if( !Get(pElement->ID()) )
			{
				InsertActiveList(pElement);
			}
		}
	
		++itor;
	}

	if( !m_kReserveCloseSet.empty() )
	{
		ContWndIDList kReserveList;
		kReserveList.swap( m_kReserveCloseSet );

		ContWndIDList::iterator close_iter = kReserveList.begin();
		while( kReserveList.end() != close_iter )
		{
			CXUI_Wnd *pkWnd = Get( *close_iter );
			if( pkWnd )
			{
				if( !pkWnd->IsClosed() )
				{
					//아직 닫혀지지 않은 상태면 닫자
					pkWnd->Close();

					close_iter = kReserveList.erase( close_iter );
					continue;
				}
			}

			++close_iter;
		}

		if( !EmptyHide() )
		{
			m_kReserveCloseSet.swap( kReserveList );
		}
	}

	//int iWndNum = ;
	//m_HideList.clear();
	//return iWndNum;
	return (int)kHideList.size();
}
int CXUI_Manager::HidePartUI(std::wstring const& HideKey, ContWndIDList const& rkList)
{
	WND_LIST	kHideWnd;

	//맵이동에 문제가 생기는UI는 알아서 쓰지 말자;;;(World와 관련 있는 케릭터 정보창 같은거)
	ContWndIDList::const_iterator c_iter = rkList.begin();
	while( c_iter != rkList.end() )
	{//숨길 아이디 리스트
		WND_LIST::iterator	wnd_iter = m_lstActive.begin();
		while( m_lstActive.end() != wnd_iter )
		{
			if( (*wnd_iter)->IsModal() )
			{//모달은 숨길 수 없다//HideAllUI만 허용(숨길필요있음 그때가서 수정하자)
				++wnd_iter;
				continue;
			}

			if( (*wnd_iter)->ID() == (*c_iter) && (*wnd_iter)->ID() != STR_XUI_CURSOR)
			{//액티브에 있는가?
				(*wnd_iter)->Visible(false);
				kHideWnd.push_back((*wnd_iter));

				XUI_HASH::iterator hash_iter = m_kBufferdHash.find((*wnd_iter)->ID());
				if( hash_iter != m_kBufferdHash.end() )
				{//해쉬에서 임시로 제거한다
					m_kBufferdHash.erase(hash_iter);
				}

				wnd_iter = m_lstActive.erase(wnd_iter);
				continue;
			}
			++wnd_iter;
		}
		++c_iter;
	}

	if( PushHidePart(HideKey, kHideWnd) )
	{//몇개 숨겼다
		return static_cast<int>(kHideWnd.size());
	}

	return 0;
}
int CXUI_Manager::ShowPartUI(std::wstring const& HideKey)
{
	WND_LIST	kHideWnd;

	PopHidePart(HideKey, kHideWnd);
	if( kHideWnd.empty() )
	{//숨긴것이 없다
		return 0;
	}

	int const iShowCount = static_cast<int>(kHideWnd.size());

	WND_LIST::iterator iter = kHideWnd.begin();
	while( iter != kHideWnd.end() )
	{//원상 복구 시켜주자
		if( NULL != (*iter) )
		{
			XUI::CXUI_Wnd* pWnd = Get((*iter)->ID());
			if( !pWnd )
			{//열어 줘// 해쉬에 다시 넣는다.
				(*iter)->Visible(true);
				InsertActiveList((*iter));

				if( !(*iter)->IsModal() )
				{
					auto ret = m_kBufferdHash.insert(std::make_pair((*iter)->ID(), (*iter)));
					assert(ret.second);
				}
			}
			else
			{//다시 열어 버렸다. 이전것은 지워버리고 (그래도 되나...)
				SAFE_DELETE((*iter));
				//새로운것이 닫혀있음 열어준다
				if( pWnd->IsClosed() )
				{
					pWnd->VOnCall();
				}
				else if( !pWnd->Visible() )
				{
					pWnd->Visible(true);
				}
			}
		}
		iter = kHideWnd.erase(iter);
	}

	return iShowCount;
}
bool CXUI_Manager::AddColor(int iIdx, DWORD dwColor)
{
	auto ret = m_ColorMap.insert( std::make_pair(iIdx, dwColor) );
	if (!ret.second)
	{
		ret.first->second = dwColor;
	}
	return ret.second;
}
DWORD CXUI_Manager::GetColor(int iIdx)
{
	if (m_ColorMap.empty())
	{
		return 0xffffff;
	}

	COLOR_HASH::const_iterator itor = m_ColorMap.find(iIdx);
	if (itor != m_ColorMap.end())
	{
		return itor->second;
	}

	return 0xffffff;
}

void CXUI_Manager::SetColorSet()
{
	XUI_HASH::iterator itor = m_kBufferdHash.begin();

	while( m_kBufferdHash.end() != itor )
	{
		if (itor->second != NULL)
			itor->second->SetColorSet();
		++itor;
	}
}

void CXUI_Manager::RegistGlobalScript(SCRIPT_HASH::key_type const& rKey, SCRIPT_HASH::mapped_type const& rScript)
{
	RegistScript(rKey, rScript);
}

bool CXUI_Manager::DoGlobalScript(SCRIPT_HASH::key_type const& rScriptKey)
{
	if ( !m_bBlockGlobalScript )	return DoHotKey(rScriptKey);
	return false;
}

CS::CCSIME& CXUI_Manager::GetIME() const
{
	E_XUI_TYPE kType = E_XUI_EDIT;

	if (CXUI_Edit::GetFocusedEdit())
		kType = CXUI_Edit::GetFocusedEdit()->VType();

	if(E_XUI_EDIT == kType)
	{
		return g_kSingleIME;
	}
	//else if(E_XUI_EDIT_MULTILINE == kType)
	return g_kMultiIME;
}
void CXUI_Manager::LocalName(std::wstring strName)
{
	m_strLocal = strName;
}
std::wstring CXUI_Manager::LocalName()
{
	return m_strLocal;
}

SUniqueUITypeErrInfo const& CXUI_Manager::ConvertUniqueTypeUIToErrNo(std::wstring const& kUIType) const
{
	static SUniqueUITypeErrInfo kInfo;
	kUniqueUITypeErrCont::const_iterator errno_itor = m_kUniqueUITypeErrCont.find(kUIType);
	if( errno_itor != m_kUniqueUITypeErrCont.end() )
	{
		kUniqueUITypeErrCont::mapped_type const& kErrNo = errno_itor->second;
		return kErrNo;
	}
	return kInfo;
}