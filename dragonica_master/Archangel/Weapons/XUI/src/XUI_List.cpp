#include "stdafx.h"
#include "XUI_List.h"
#include "XUI_Manager.h"

using namespace XUI;

CXUI_List::CXUI_List() 
	:	m_bIsSelectable(true),
		m_iSelectItemNum(NONE_SELECT_ITEM),
		m_iInsertIndex(0),
		m_ElementPool(12, 10),
		m_pWndUp(NULL),
		m_pWndDown(NULL),
		m_pWndMiddle(NULL),
		m_pWndBar(NULL),
		m_kIgnoreBtnParentRect(true)
#ifdef XUI_LIST_CHECK
,		m_bIsClearList(false)
#endif
{
	DisplayStartPos(0);
	m_ItemHash.clear();
	UseDrawRect(true);
	ScrollValue(0);
	ItemLen(0);
}
void CXUI_List::SetBoundingBox(POINT2 const& ptLocation, POINT2 const& ptSize)
{
	CXUI_Wnd::SetBoundingBox(ptLocation,ptSize);

//	POINT3I pt = TotalLocation()+ItemPos();
	POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.

	ItemHash::iterator disp_itor = m_ItemHash.begin();
//이벤트 없이 지워질 경우도. 사이즈 보정이 필요하다.
	int iSkip = DisplayStartPos();
	int const iParentHeight = Height();

	int iACCHeight = 0;//디스플레이된 엘레멘트의 누적길이
	while(disp_itor != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd = (*disp_itor).second->m_pWnd;
		int const iElementHeight = pWnd->Height();
		if( iSkip >= iElementHeight
		||	iParentHeight < iACCHeight)
		{//보여지지 않음. 
			pWnd->Visible(false);
			iSkip-=iElementHeight;
		}
		else
		{//보여짐
			pt.y -= iSkip;

			pWnd->Visible(true);
			pWnd->Location(pt - POINT2(0,0));

			iACCHeight += iElementHeight-iSkip;//그려진 엘레멘트에서 Skip만큼 땡긴다.
			pt.y += iElementHeight;//다음위치.
			iSkip = __max(0, iSkip-iElementHeight);//스킵만큼 땡김
		}
		++disp_itor;
	}
}

bool CXUI_List::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick( dwCurTime ) )
	{
		return false;
	}

	ContItemPointer kEraseVec;
	ItemHash::iterator disp_itor = m_ItemHash.begin();
	while(disp_itor != m_ItemHash.end())
	{
		ItemHash::mapped_type pkElement = (*disp_itor).second;
		CXUI_Wnd* pkItemWnd = pkElement->m_pWnd;
		if( pkItemWnd )
		{
			pkItemWnd->VOnTick( dwCurTime );

			if( pkItemWnd->IsClosed() )
			{
				kEraseVec.push_back(pkElement);//Closed된 아이템 삭제
			}
		}
		++disp_itor;
	}

	ContItemPointer::iterator wnd_iter = kEraseVec.begin();//Closed된 아이템 삭제
	while(kEraseVec.end() != wnd_iter)
	{
		SListItem* pDeleteItem = (*wnd_iter);
		DeleteItem(pDeleteItem);
		++wnd_iter;
	}
	return true;
}

void CXUI_List::Size(POINT2 const& rPT, bool const bIsModifyBoundingBox)
{
	CXUI_Wnd::Size(rPT, bIsModifyBoundingBox);

	AdjustDownBtnPos();
	AdjustMiddleBarSize();
}

void CXUI_List::VInit()
{
	UseOffscreen(false);
	m_pWndUp = GetControl( ScrUpName() );
	m_pWndDown = GetControl( ScrDownName() );
	m_pWndMiddle = GetControl( ScrMiddleName() );
	m_pWndBar = GetControl( ScrBarName() );
	
	if(m_pWndUp && m_pWndDown &&m_pWndMiddle)
	{
		m_pWndUp->IsIgnoreParentRect(IgnoreBtnParentRect());
		m_pWndDown->IsIgnoreParentRect(IgnoreBtnParentRect());
		m_pWndMiddle->IsIgnoreParentRect(IgnoreBtnParentRect());

		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		if( m_pWndMiddle != NULL && m_pWndMiddle->Location().y < iTop )
		{
			m_pWndMiddle->Location( m_pWndMiddle->Location().x, iTop );
		}
		else if( m_pWndDown != NULL && m_pWndMiddle->Location().y > iBottom )
		{
			m_pWndMiddle->Location( m_pWndMiddle->Location().x, iBottom );
		}

		AdjustMiddleBarSize();
		m_pWndUp->NoFocus(true);
		m_pWndDown->NoFocus(true);
		//m_pWndMiddle->NoFocus(true);
		if(m_pWndBar)
		{
			m_pWndBar->NoFocus(true);
		}
	}
	m_iInsertIndex = 0;
}

CXUI_List::~CXUI_List()
{
	m_ItemHash.clear();
}

SListItem* CXUI_List::AddItem(std::wstring const& rkKey, void* pData)
{
	SListItem* pItem = m_ElementPool.New();

	if(pItem)
	{
		int const iInsertIndex = m_iInsertIndex++;//static_cast<int>(m_ItemHash.size());

		pItem->m_iIndex = iInsertIndex;

		if(!pItem->m_pWnd)
		{
			CXUI_Wnd *pWnd = CXUI_Manager::Create(ElementID(), true);
			if (pWnd)
			{
				pWnd->Parent(this);//이놈이 부모다.

				pWnd->SetCustomData(&pData, sizeof(void*));
				pItem->m_pWnd = pWnd;
			}
		}
		else
		{
			pItem->m_pWnd->DoScript(SCRIPT_ON_CREATE);//리스트에서는 Pool을 사용하므로 ON_CREATE를 강제로 부른다
		}

		pItem->m_wstrKey = rkKey;

		auto ret = m_ItemHash.insert(ItemHash::value_type(iInsertIndex, pItem));
		if(ret.second)
		{
			AdjustMiddleBtnPos();
			return (ret.first->second);
		}
	}

	return NULL;
}

SListItem* CXUI_List::FirstItem()
{
	ItemHash::iterator itor = m_ItemHash.begin();

	if(itor != m_ItemHash.end())
	{
		return (*itor).second;
	}

	return NULL;
}

SListItem* CXUI_List::NextItem(SListItem* pBase)
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

SListItem* CXUI_List::PrevItem(SListItem* pBase)
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

SListItem* CXUI_List::DeleteItem(SListItem* pBase)
{
	if(!pBase){return NULL;}

	ItemHash::iterator itor = m_ItemHash.find(pBase->m_iIndex);

	if(itor != m_ItemHash.end())//시작자리가 아니라면
	{
		SListItem *pkItem = itor->second;
		if (pkItem->m_pWnd)
			SAFE_DELETE(pkItem->m_pWnd);

		itor = m_ItemHash.erase(itor);

		//위치 보정.
		

		if(itor != m_ItemHash.end())
		{
			return (*itor).second;
		}
	}
	return NULL;
}

bool CXUI_List::DeleteItem(int const& iIndex)
{
	ItemHash::iterator del_it = m_ItemHash.find(iIndex);

	if (del_it != m_ItemHash.end())
//	&&	(*del_it).second->m_pWnd)
	{
		SListItem *pkItem = del_it->second;
		if (pkItem->m_pWnd)
			SAFE_DELETE(pkItem->m_pWnd);

		m_ElementPool.Delete((*del_it).second);
		m_ItemHash.erase(del_it);
	}
	else
	{
		return false;
	}
	
	ItemHash::iterator disp_itor = m_ItemHash.begin();

	if(disp_itor != m_ItemHash.end())
	{
	}
	else
	{
	}
	
	AdjustMiddleBtnPos();

	if( SelectItemNum() == iIndex )
	{
		SelectItemNum(NONE_SELECT_ITEM);
	}
	return true;
}

bool CXUI_List::DeleteItem()
{
	ItemHash::iterator del_it = m_ItemHash.begin();
	while(del_it != m_ItemHash.end())
	{
		SListItem *pkItem = del_it->second;
		if (pkItem->m_pWnd)
			SAFE_DELETE(pkItem->m_pWnd);

		m_ElementPool.Delete((*del_it).second);
		del_it = m_ItemHash.erase(del_it);//지움.
	}

	SelectItemNum(NONE_SELECT_ITEM);
	return true;
}
void CXUI_List::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_ITEM_X == wstrName )			{ ItemPos( POINT2((int)vValue, ItemPos().y)); return; }
	else if( ATTR_ITEM_Y == wstrName )			{ ItemPos( POINT2(ItemPos().x, (int)vValue)); return; }
	else if( ATTR_ITEM_WIDTH == wstrName )		{ ItemSize( POINT2( (int)vValue, ItemSize().y)); return; }
	else if( ATTR_ITEM_HEIGHT == wstrName )		{ ItemSize( POINT2( ItemSize().x, (int)vValue)); return; }
/*	if( ATTR_CAN_SELECT ==  wstrName )
	{ 
		if( (int)vValue )
			CanSelect(true);
		else 
			CanSelect(false);

		return;
	}
*/	//버튼 이름 저장
	else if( ATTR_BTN_UP == wstrName )		{	ScrUpName(wstrValue); }
	else if( ATTR_BTN_DOWN == wstrName )	{	ScrDownName(wstrValue); }
	else if( ATTR_BTN_MIDDLE == wstrName )	{	ScrMiddleName(wstrValue); }
	else if( ATTR_WND_BAR == wstrName )		{	ScrBarName(wstrValue); }
	else if( ATTR_ELEMENT_ID == wstrName )	{	ElementID(wstrValue); }
	else if( ATTR_SCROLL_VALUE == wstrName )	{	ScrollValue((int)vValue); }
	else if( ATTR_IGNORE_BTN_PARENTRECT == wstrName )	{ m_kIgnoreBtnParentRect = 0 != (int)vValue; }
}

bool CXUI_List::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if(!Visible() || IsClosed()){return false;}

	bool bRet = false;
	
	//나 자신
	if(m_pWndUp && m_pWndDown && m_pWndMiddle)//영역 체크
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		int const iBarLen = iBottom - iTop;

		size_t const szTotalItemLen = GetTotalItemLen();
		size_t const szTotalItemCount = GetTotalItemCount();
		ItemLen(szTotalItemLen);

		bool bAdjustedByDrag = false;

		if(szTotalItemLen > (unsigned int)Height())
		{
			int const AvgLen = (int)((szTotalItemCount)?szTotalItemLen/szTotalItemCount:0);
			
			int iStartPos = DisplayStartPos();

//			if( rET == IEI_MS_MOVE 
//			&& dwValue == MEI_BTN_0	)

			bool bIsForceUp = false;
			bool bIsForceDown = false;
			if( rET == IEI_MS_DOWN 
			&& dwValue == MEI_BTN_0	)
			{
				int const iWidth = m_pWndUp->Width();//넓이.
				RECT rc;
				POINT3I const& kStartPos = m_pWndUp->TotalLocation();
				POINT3I const& kEndPos = m_pWndDown->TotalLocation();
				::SetRect(&rc, kStartPos.x, kStartPos.y, kStartPos.x+iWidth, kEndPos.y);

				if(PtInRect(&rc, (POINT2 const)m_sMousePos) && !m_pWndMiddle->ContainsPoint(m_sMousePos))
				{
					if(m_pWndMiddle->TotalLocation().y < m_sMousePos.y)
					{
						bIsForceDown = true;
					}
					else
					{
						bIsForceUp = true;
					}
				}
//				{//스크롤 바 포인트 내부.
//					iStartPos = (float)(szTotalItemLen-Height()) * ((float)(m_sMousePos.y - kStartPos.y)/(float)iBarLen);//남은 애들에서 상대위치.
//					DisplayStartPos(iStartPos);//리스트 위치 보정
//				}
			}

			if(m_ItemHash.size() && rPT.z && ContainsPoint(m_sMousePos))
			{
				int iZ = rPT.z * 3.0f;
				if (0!=ScrollValue())
				{
					iZ = ScrollValue();
					if (rPT.z<0)
					{
						iZ*=(-1);
					}
				}
				DisplayStartPos( DisplayStartPos()-iZ);//0 이하가 되지 않게.
				bRet = true;
			}

			//가운데 버튼
			bool bScrolling = false;
			if( m_pWndMiddle != NULL && m_spWndScrollBtn == m_pWndMiddle )
			{
				if( rET == IEI_MS_UP )
				{
					m_spWndScrollBtn = NULL;
				}
				bScrolling = true;
				bRet = m_pWndMiddle->VPeekEvent(rET, rPT, dwValue);
			}
			else 
			{
				if(m_spWndScrollBtn == NULL && m_pWndMiddle != NULL && m_pWndMiddle->VPeekEvent(rET, rPT, dwValue) )
				{
					if( m_pWndMiddle->IsMouseDown() )
					{
						m_spWndScrollBtn = m_pWndMiddle;
						bScrolling = true;
					}
					bRet = true;
				}
			}

			if( bScrolling )
			{
				POINT3I ptMid = m_pWndMiddle->Location();

				int iTotalTop = TotalLocation().y + iTop;
				int iTotalBotton = TotalLocation().y + iBottom;

				if( m_sMousePos.y >= iTotalTop && m_sMousePos.y <= iTotalBotton )
				{
					if(!rPT.x)
					{
						ptMid += rPT;//포지션.

						ptMid.y = __max(ptMid.y, iTop);
						ptMid.y = __min(ptMid.y, iBottom);

						m_pWndMiddle->Location(ptMid);//버튼 위치 보정
						
						int const MidPos = ptMid.y - iTop;//상대위치

						iStartPos = (int)(((float)((int)szTotalItemLen-Height()) * ((float)MidPos/(float)iBarLen)));//남은 애들에서 상대위치.

						DisplayStartPos(iStartPos);//리스트 위치 보정
					}
				}				
				bAdjustedByDrag = true;
			}

			//스크롤 업 버튼
			if((m_pWndUp!= NULL && m_pWndUp->VPeekEvent(rET, rPT, dwValue) )
			|| bIsForceUp )
			{
				if( bIsForceUp || m_pWndUp->IsMouseDown() )
				{	bRet = true;
					DisplayStartPos( __max( iStartPos - AvgLen, 0) );//0 이하가 되지 않게.
				}
			}

			//스크롤 다운 버튼
			if(m_pWndDown != NULL && m_pWndDown->VPeekEvent(rET, rPT, dwValue) 
			|| bIsForceDown)
			{
				if( bIsForceDown || m_pWndDown->IsMouseDown() )
				{	bRet = true;
					DisplayStartPos(__min(iStartPos+AvgLen, (int)szTotalItemLen-Height()));//전체길이-리스트컨트롤의 길이를 넘어서지 않도록
				}
			}
		}
		else
		{	//szTotalItemLen <= iBarLen
			DisplayStartPos(0);
		}

		if (!bAdjustedByDrag)
		{
			AdjustMiddleBtnPos();
		}
	}

	//리스트 아이템 클릭시
//	ItemHash::iterator element_itor =  m_ItemHash.begin();

//	int const iIndex = DisplayStartIndex();

#ifdef XUI_LIST_CHECK
	m_bIsClearList = false;
#endif

	ItemHash::iterator disp_itor = m_ItemHash.begin();
	while(!bRet && disp_itor != m_ItemHash.end())
	{
		XUI::CXUI_Wnd* pWnd = (*disp_itor).second->m_pWnd;
		if (pWnd)
		{
			bRet = pWnd->VPeekEvent(rET, rPT, dwValue);
			if(bRet)
			{
				break;
			}
		}

#ifdef XUI_LIST_CHECK
		if ( m_bIsClearList )
		{
			assert( false );
			break;
		}
#endif

		++disp_itor;
	}

	if(!bRet)
	{
		bRet = VPeekEvent_Default(rET, rPT, dwValue);
	}

	return PassEvent()? false: bRet;
}

bool CXUI_List::VDisplay()
{
	bool const bVisibleButton = (ItemLen() > static_cast<size_t>(Height()));
	
	if(m_pWndUp		&& m_pWndUp->Visible()!=bVisibleButton)			{m_pWndUp->Visible(bVisibleButton);}
	if(m_pWndDown	&& m_pWndDown->Visible()!=bVisibleButton)		{m_pWndDown->Visible(bVisibleButton);}
	if(m_pWndMiddle	&& m_pWndMiddle->Visible()!=bVisibleButton)		{m_pWndMiddle->Visible(bVisibleButton);}
	if(m_pWndBar	&& m_pWndBar->Visible()!=bVisibleButton)		{m_pWndBar->Visible(bVisibleButton);}
		
	if(!CXUI_Wnd::VDisplay() ){return false;}

//	POINT3I pt = TotalLocation()+ItemPos();
	POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.

	ItemHash::iterator disp_itor = m_ItemHash.begin();
//이벤트 없이 지워질 경우도. 사이즈 보정이 필요하다.
	int iSkip = DisplayStartPos();
	int const iParentHeight = Height();

	int iACCHeight = 0;//디스플레이된 엘레멘트의 누적길이
	while(disp_itor != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd = (*disp_itor).second->m_pWnd;
		int const iElementHeight = pWnd->Height();
		if( iSkip >= iElementHeight
		||	iParentHeight < iACCHeight)
		{//보여지지 않음. 
			pWnd->Visible() != false ? pWnd->Visible(false) : 0;
			iSkip-=iElementHeight;
		}
		else
		{//보여짐
			pt.y -= iSkip;

			pWnd->Visible() != true ? pWnd->Visible(true) : 0;
			pWnd->Location(pt - POINT2(0,0));
			pWnd->VDisplay();

			iACCHeight += iElementHeight-iSkip;//그려진 엘레멘트에서 Skip만큼 땡긴다.
			pt.y += iElementHeight;//다음위치.
			iSkip = __max(0, iSkip-iElementHeight);//스킵만큼 땡김
		}
		++disp_itor;
	}
	return true;
}

bool CXUI_List::VDisplayEnd()
{
	return true;
}

void CXUI_List::VRefresh()
{
	CXUI_Wnd::VRefresh();
	ItemHash::iterator disp_itor = m_ItemHash.begin();
	while(disp_itor != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd = (*disp_itor).second->m_pWnd;
		if (pWnd)		
			pWnd->VRefresh();
		++disp_itor;
	}
}

void CXUI_List::InvalidateChild()
{
}

void CXUI_List::ClearList()
{ 
	DeleteAllItem();

	if(m_pWndUp && m_pWndMiddle)
	{
		int iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		m_pWndMiddle->Location( m_pWndMiddle->Location().x, iTop );
	}
	m_iInsertIndex = 0;
}

size_t CXUI_List::GetTotalItemLen()const//화면상 길이
{
	size_t total_length = 0;
	ItemHash::const_iterator itor = m_ItemHash.begin();
	while( itor != m_ItemHash.end())
	{
		total_length += (*itor).second->m_pWnd->Height();
		++itor;
	}
	return total_length;
}

bool CXUI_List::AdjustMiddleBtnPos()
{
	size_t const szTotalItemLen = GetTotalItemLen();
	size_t const szTotalItemCount = GetTotalItemCount();
	ItemLen(szTotalItemLen);

	if(m_pWndUp && m_pWndDown && m_pWndMiddle)
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		int const iBarLen = iBottom - iTop;

		if((int)szTotalItemLen <= iBarLen)
		{
			DisplayStartPos(0);
		}

		int const iPrevPos = DisplayStartPos();

		DisplayStartPos(__max(iPrevPos, 0));//0 이하가 되지 않게.
		DisplayStartPos(__min(DisplayStartPos(),__max((int)szTotalItemLen-Height(), 0)));//전체길이-리스트컨트롤의 길이를 넘어서지 않도록

		float fScrRate = 0.0f;
		if((int)szTotalItemLen > Height())
		{
			fScrRate = (float)DisplayStartPos()/(float)(szTotalItemLen-Height());//현재 포지션을, 디스플레이 되지않는 애들의 비율로 나눔.
		}
		
		m_pWndMiddle->Location(m_pWndMiddle->Location().x, (int)(iTop+((iBottom-iTop)*fScrRate)));//최고점 + 전체길이/비율
		return true;
	}
	return false;
}

bool CXUI_List::AdjustDownBtnPos()
{
	if (m_pWndDown)
	{
		m_pWndDown->Location(m_pWndDown->Location().x, Size().y - m_pWndDown->Size().y);
		return true;
	}
	return false;	
}

bool CXUI_List::AdjustMiddleBarSize()
{
	if (!m_pWndBar || !m_pWndUp || !m_pWndDown)
	{
		return false;
	}

	int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
	int const iX = m_pWndUp->Location().x + (m_pWndUp->Width() - m_pWndBar->Width())/2  + 1;
	m_pWndBar->Location(iX, iTop, -3);
	m_pWndBar->Size(m_pWndBar->Width(), __max(0, m_pWndDown->Location().y - iTop));
	m_pWndBar->ImgSize(m_pWndBar->Size());
	return true;
}

SListItem* CXUI_List::FindItem(std::wstring const& wstrName)
{
	for(ItemHash::iterator it = m_ItemHash.begin(); it != m_ItemHash.end(); ++it )
	{
		if( (*it).second->m_wstrKey == wstrName )
			return (*it).second;
	}

	return NULL;
}

SListItem* CXUI_List::GetItemAt(int const& iNum)
{
	if( m_ItemHash.empty() )
	{
		return NULL;
	}

	ItemHash::iterator it = m_ItemHash.begin();

	for(unsigned int i = 0; i < (unsigned int)iNum; ++i)
	{
		if( i < m_ItemHash.size() )
		{
			++it;
		}
		if( it == m_ItemHash.end() )	//범위내 없음
		{
			return NULL;
		}
	}

	return (*it).second;
	
	return NULL;
}

void CXUI_List::SetColorSet()	//XUI에 디폴트로 지정된 셋팅 값들을 로딩
{
	CXUI_Wnd::SetColorSet();

	ItemHash::iterator disp_itor = m_ItemHash.begin();
	while(disp_itor != m_ItemHash.end())
	{
		CXUI_Wnd *pWnd = (*disp_itor).second->m_pWnd;
		if (pWnd)
		{
			pWnd->SetColorSet();
		}
		++disp_itor;
	}
}
void CXUI_List::RemoveAllControls(bool const bIsClosed)
{
	CXUI_Wnd::RemoveAllControls(bIsClosed);

	DeleteAllItem();
}

void CXUI_List::DeleteAllItem()
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
	m_bIsClearList = true;
}