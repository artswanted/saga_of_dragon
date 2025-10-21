#include "stdafx.h"
#include "XUI_Tree.h"
#include "XUI_Manager.h"
using namespace XUI;

CXUI_Tree::CXUI_Tree(void)
	: m_ElementPool(12)
{
	m_iInsertIndex = 0;
//	m_kBtnExt.Parent(this);
	m_pkBtnExt = dynamic_cast<CXUI_CheckButton*>(CXUI_Manager::CreateElement(ROOT_CHECK_BUTTON));
	bool bRet = VRegistChild( m_pkBtnExt );
	assert(bRet);

	UseDrawRect(true);
}

CXUI_Tree::~CXUI_Tree(void)
{
	DeleteAllItem();
}

void CXUI_Tree::VInit()
{
	CXUI_Form::VInit();
	UseOffscreen(false);
}

void CXUI_Tree::CheckState(bool const bIsCheck)
{
	assert(m_pkBtnExt);
	if(m_pkBtnExt)
	{
		m_pkBtnExt->Check(bIsCheck);

		//사이즈 재정렬. xxx todo 깨끗하게 만들어야됨
		ItemHash::iterator item_itor = m_ItemHash.begin();

		POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.
		while(item_itor != m_ItemHash.end())
		{
			CXUI_Wnd *pWnd= (*item_itor).second->m_pWnd;
			pt.y += pWnd->Height();
			++item_itor;
		}
	
		if(m_pkBtnExt->Check())
		{
			Size(Size().x, pt.y);
		}
		else
		{
			Size(Size().x, ItemPos().y);
		}
	}
}

bool CXUI_Tree::CheckState()
{
	assert(m_pkBtnExt);
	if(m_pkBtnExt)
	{
		return m_pkBtnExt->Check();
	}
	return false;
}

SListItem* CXUI_Tree::AddItem(std::wstring const& rkKey, void* pData)
{
	SListItem* pItem = m_ElementPool.New();

	int const iinsertIndex = m_iInsertIndex++;//static_cast<int>(m_ItemHash.size());

	if(pItem)
	{
		pItem->m_iIndex = iinsertIndex;

		if(!pItem->m_pWnd)
		{
			CXUI_Wnd *pWnd = CXUI_Manager::Create(ElementID(), true);
			pWnd->Parent(this);//이놈이 부모다.

			pWnd->SetCustomData(&pData, sizeof(void*));
			pItem->m_pWnd = pWnd;
//			pItem->m_pData = pData;
		}
		else
		{
			pItem->m_pWnd->DoScript(SCRIPT_ON_CREATE);//리스트에서는 Pool을 사용하므로 ON_CREATE를 강제로 부른다
		}
	}

	auto ret = m_ItemHash.insert(ItemHash::value_type(iinsertIndex, pItem));
	if(ret.second)
	{
		return (ret.first->second);
	}

	return NULL;
}

void CXUI_Tree::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_ITEM_X == wstrName )			{ ItemPos( POINT2((int)vValue, ItemPos().y)); return; }
	else if( ATTR_ITEM_Y == wstrName )			{ ItemPos( POINT2(ItemPos().x, (int)vValue)); return; }
//	if( ATTR_ITEM_WIDTH == wstrName )		{ ItemSize( POINT2( (int)vValue, ItemSize().y)); return; }
//	if( ATTR_ITEM_HEIGHT == wstrName )		{ ItemSize( POINT2( ItemSize().x, (int)vValue)); return; }
//	if( ATTR_CAN_SELECT ==  wstrName )			
//	{ 
//		if( (int)vValue )
//			CanSelect(true);
//		else 
//			CanSelect(false);
//
//		return;
//	}
//	//버튼 이름 저장
//	if( ATTR_BTN_UP == wstrName )	{	ScrUpName(wstrValue); }
//	if( ATTR_BTN_DOWN == wstrName )	{	ScrDownName(wstrValue); }
//	if( ATTR_BTN_MIDDLE == wstrName )	{	ScrMiddleName(wstrValue); }
	else if( ATTR_ELEMENT_ID == wstrName )	{	ElementID(wstrValue); }
	else if( ATTR_EXPENSION_BTN_IMG == wstrName )
	{//기본값은 설정 하도록 한다 ㅡㅡ 이전소스와 호환성 유지
		m_pkBtnExt->VRegistAttr(ATTR_BTN_IMG, wstrValue);//리소스 로딩된다.
		m_pkBtnExt->BtnImgSize(POINT2(34,34));
//		m_pkBtnExt->SizedScale(SSizedScale(POINT2(34,34), POINT2(34,34)/POINT2(2,2)));//이미지가 2by2 이므로
		m_pkBtnExt->Size(POINT2(17,17));//버튼 사이즈 -> 클릭 영역.
		m_pkBtnExt->Location(4,4);
	}
	else if(ATTR_EXP_BTN_X == wstrName)//Button Location x
	{
		POINT2 kSize = m_pkBtnExt->Location();
		kSize.x = (int)vValue;
		m_pkBtnExt->Location(kSize);
	}
	else if(ATTR_EXP_BTN_Y == wstrName)//Button Location y
	{
		POINT2 kSize = m_pkBtnExt->Location();
		kSize.y = (int)vValue;
		m_pkBtnExt->Location(kSize);
	}
	else if(ATTR_EXP_BTN_W == wstrName)//Button Size x
	{
		POINT2 kSize = m_pkBtnExt->Size();
		m_pkBtnExt->Size((int)vValue, kSize.y);
	}
	else if(ATTR_EXP_BTN_H == wstrName)//Button Size y
	{
		POINT2 kSize = m_pkBtnExt->Size();
		m_pkBtnExt->Size(kSize.x, (int)vValue);
	}
	else if(ATTR_EXP_BTN_IMG_W == wstrName)//Button Image Size x
	{
		POINT2 kSize = m_pkBtnExt->BtnImgSize();
		kSize.x = (int)vValue;
		m_pkBtnExt->BtnImgSize(kSize);
	}
	else if(ATTR_EXP_BTN_IMG_H == wstrName)//Button Image Size y
	{
		POINT2 kSize = m_pkBtnExt->BtnImgSize();
		kSize.y = (int)vValue;
		m_pkBtnExt->BtnImgSize(kSize);
	}
	else if(ATTR_EXP_COLOR_CHANGE== wstrName)//Button Can Color Changed ?
	{
		bool bRet = 0!=(int)vValue;
		m_pkBtnExt->CanColorChange(bRet);
	}
	else if(ATTR_EXP_COLOR_TYPE == wstrName)//Button Color Changed ?
	{
		m_pkBtnExt->ColorType((int)vValue);
	}
}

bool CXUI_Tree::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}

	//확장 버튼 클릭
	//접힘상태. -> element 의 사이즈 계산해서 form 길이 연장
	//				-> element display 루틴 타야함

	//열림상태. -> default 사이즈로 길이 축소
	//				-> element display 루틴 무시

//	if( m_pkBtnExt->VPeekEvent(rET, rPT, dwValue) )
//	{//트리를 열었다.
//		return true;//
//	}

	bool bRet = false;
	POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.

	ItemHash::iterator item_itor = m_ItemHash.begin();
	while(m_pkBtnExt->Check() && item_itor != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd= (*item_itor).second->m_pWnd;

		if(!bRet && pWnd->VPeekEvent(rET, rPT, dwValue))
		{
			//bRet = true;
			return true;
		}
		pt.y += pWnd->Height();
		++item_itor;
	}
	
	if(ContainsPoint( m_sMousePos ))
	{
		if(m_pkBtnExt->VPeekEvent(rET, rPT, dwValue))
		{
			if(m_pkBtnExt->Check())
			{
				Size(Size().x, pt.y);//x not change
			}
			else
			{
				Size(Size().x, ItemPos().y);//x not change
			}
			return true;
		}
	}

	//모든 자식컨트롤러에게 이벤트메시지 처리 권한 부여
	CONT_CONTROL::iterator kItor = m_contControls.begin();
	while(m_contControls.end() != kItor)
	{
		if( (*kItor)->VPeekEvent(rET, rPT, dwValue) )
		{
			return true;
		}
		++kItor;
	}

	return VPeekEvent_Default(rET, rPT, dwValue);
}

bool CXUI_Tree::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick(dwCurTime) )
	{
		return false;
	}

	ItemHash::iterator disp_itor = m_ItemHash.begin();
	while(disp_itor != m_ItemHash.end())
	{
		(*disp_itor).second->m_pWnd->VOnTick(dwCurTime);
		++disp_itor;
	}
	return true;
}

bool CXUI_Tree::VDisplay()
{
	if(!CXUI_Wnd::VDisplay()){return false;}
	m_pkBtnExt->VDisplay();

	POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.

	if(m_pkBtnExt->Check())
	{
		ItemHash::iterator item_itor = m_ItemHash.begin();
		while(item_itor != m_ItemHash.end())
		{
			CXUI_Wnd *pWnd= (*item_itor).second->m_pWnd;
			int const iHeight = pWnd->Height();

			pWnd->Visible(true);
			pWnd->Location(pt - POINT2(0,0));
			pWnd->VDisplay();
			
			pt.y += iHeight;//다음위치.
			++item_itor;
		}
	}
	return true;
}

bool CXUI_Tree::VDisplayEnd()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	CXUI_Wnd::VDisplayEnd();

	ItemHash::iterator kIter = m_ItemHash.begin();
	while(m_ItemHash.end() != kIter)
	{
		CXUI_Wnd *pWnd= (*kIter).second->m_pWnd;
		if (pWnd)
			pWnd->VDisplayEnd();
		++kIter;
	}
#endif
	return true;
}

void CXUI_Tree::VRefresh()
{
	CXUI_Wnd::VRefresh();

	ItemHash::iterator kIter = m_ItemHash.begin();
	while(m_ItemHash.end() != kIter)
	{
		CXUI_Wnd *pWnd= (*kIter).second->m_pWnd;
		if (pWnd)
			pWnd->VRefresh();
		++kIter;
	}
}

void CXUI_Tree::InvalidateChild()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	CXUI_Wnd::InvalidateChild();

	ItemHash::iterator kIter = m_ItemHash.begin();
	while(kIter != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd = (*kIter).second->m_pWnd;
		if (pWnd)		
			pWnd->InvalidateChild();
		++kIter;
	}
#endif
}

SListItem* CXUI_Tree::FirstItem()
{
	ItemHash::iterator itor = m_ItemHash.begin();

	if(itor != m_ItemHash.end())
	{
		return (*itor).second;
	}

	return NULL;
}

SListItem* CXUI_Tree::NextItem(SListItem* pBase)
{
	if(!pBase){return NULL;}

	ItemHash::iterator itor = m_ItemHash.find(pBase->m_iIndex);

	if(itor != m_ItemHash.end())
	{
		++itor;//다음놈.
		if(itor != m_ItemHash.end())
		{
			return (*itor).second;		
		}
	}

	return NULL;
}

SListItem* CXUI_Tree::PrevItem(SListItem* pBase)
{
	if(!pBase){return NULL;}

	ItemHash::iterator itor = m_ItemHash.find(pBase->m_iIndex);

	if(itor != m_ItemHash.begin()//시작자리가 아니라면
	&& itor != m_ItemHash.end())//못찾거나.
	{
		--itor;//이전놈.
		return (*itor).second;		
	}

	return NULL;
}

SListItem* CXUI_Tree::DeleteItem(SListItem* pBase)
{
	if(!pBase){return NULL;}

	ItemHash::iterator itor = m_ItemHash.find(pBase->m_iIndex);

	if(itor != m_ItemHash.end())//시작자리가 아니라면
	{
		SListItem *pkItem = itor->second;
		if (pkItem->m_pWnd)
			SAFE_DELETE(pkItem->m_pWnd);

		m_ElementPool.Delete(pkItem);
		itor = m_ItemHash.erase(itor);

		if(itor != m_ItemHash.end())
		{
			return (*itor).second;
		}
	}
	return NULL;
}


void CXUI_Tree::DeleteAllItem()
{
	ItemHash::iterator kIter = m_ItemHash.begin();
	while(m_ItemHash.end() != kIter)
	{
		SListItem *pkItem = kIter->second;
		if (pkItem->m_pWnd)
			SAFE_DELETE(pkItem->m_pWnd);

		m_ElementPool.Delete(kIter->second);//반환
		++kIter;
	}
	m_ItemHash.clear();
}
int CXUI_Tree::GetItemCount()
{
	return (int)m_ItemHash.size();
}

SListItem* CXUI_Tree::FindItem(std::wstring const& wstrName)
{
	for(ItemHash::iterator it = m_ItemHash.begin(); it != m_ItemHash.end(); ++it )
	{
		if( (*it).second->m_wstrKey == wstrName )
			return (*it).second;
	}

	return NULL;
}