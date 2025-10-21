#include "stdafx.h"
#include "XUI_List2.h"
#include "XUI_Manager.h"

using namespace XUI;

CXUI_List2::CXUI_List2()
:	m_pWndUp(NULL)
,	m_pWndDown(NULL)
,	m_pWndMiddle(NULL)
,	m_pWndBar(NULL)
,	m_kItemHeight(0)
,	m_iDisplayStartPos(0)
,	m_bUpdateStartPos(false)
{
	ScrollValue(0);
}

CXUI_List2::~CXUI_List2()
{
	ClearList();
}

void CXUI_List2::VInit()
{
	UseOffscreen(false);
	CanClip(true);
	m_pWndUp = GetControl( ScrUpName() );
	m_pWndDown = GetControl( ScrDownName() );
	m_pWndMiddle = GetControl( ScrMiddleName() );
	m_pWndBar = GetControl(ScrBarName());

	if( m_pWndUp && m_pWndDown && m_pWndMiddle )
	{
		m_pWndUp->IsIgnoreParentRect(true);
		m_pWndDown->IsIgnoreParentRect(true);
		m_pWndMiddle->IsIgnoreParentRect(true);

		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		if( m_pWndMiddle->Location().y < iTop )
		{
			m_pWndMiddle->Location( m_pWndMiddle->Location().x, iTop );
		}
		else if( m_pWndMiddle->Location().y > iBottom )
		{
			m_pWndMiddle->Location( m_pWndMiddle->Location().x, iBottom );
		}

		AdjustMiddleBarSize();
		m_pWndUp->NoFocus(true);
		m_pWndDown->NoFocus(true);
		m_pWndMiddle->NoFocus(true);
		if(m_pWndBar)
		{
			m_pWndBar->NoFocus(true);
		}
	}
}

void CXUI_List2::VRegistAttr( std::wstring const& wstrName, std::wstring const& wstrValue )
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_ITEM_X == wstrName )			{ ItemPos( POINT2((int)vValue, ItemPos().y)); return; }
	else if( ATTR_ITEM_Y == wstrName )			{ ItemPos( POINT2(ItemPos().x, (int)vValue)); return; }
	else if( ATTR_ITEM_WIDTH == wstrName )		{ ItemSize( POINT2( (int)vValue, ItemSize().y)); return; }
	else if( ATTR_ITEM_HEIGHT == wstrName )		{ ItemSize( POINT2( ItemSize().x, (int)vValue)); return; }

	else if( ATTR_BTN_UP == wstrName )		{	ScrUpName(wstrValue); }
	else if( ATTR_BTN_DOWN == wstrName )	{	ScrDownName(wstrValue); }
	else if( ATTR_BTN_MIDDLE == wstrName )	{	ScrMiddleName(wstrValue); }
	else if( ATTR_WND_BAR == wstrName )		{	ScrBarName(wstrValue); }
	else if( ATTR_ELEMENT_ID == wstrName )	{	ElementID(wstrValue); }
	else if( ATTR_SCROLL_VALUE == wstrName )	{	ScrollValue((int)vValue); }
}

bool CXUI_List2::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick( dwCurTime ) )
	{
		return false;
	}

	CONT_CONTROL::iterator itor = m_kContDrawList.begin();
	for( ; m_kContDrawList.end() != itor ; ++itor )
	{
		(*itor)->VOnTick( dwCurTime );
	}

	return true;
}


bool CXUI_List2::VDisplay()
{
	int const iTotalItemHeight = (int)m_kContMap.size() * ItemHeight();
	bool const bVisibleButton = iTotalItemHeight > Height();
	if(m_pWndUp)		{m_pWndUp->Visible(bVisibleButton);}
	if(m_pWndDown)		{m_pWndDown->Visible(bVisibleButton);}
	if(m_pWndMiddle)	{m_pWndMiddle->Visible(bVisibleButton);}
	if(m_pWndBar)		{m_pWndBar->Visible(bVisibleButton);}

	if( CXUI_Wnd::VDisplay() )
	{
		if ( m_bUpdateStartPos )
		{
			m_kContDrawList.clear();

			POINT2 pt = ItemPos();

			int iSkip = m_iDisplayStartPos;
			int const iParentHeight = Height();

			CXUI_List2_Item *pItem = NULL;
			CXUI_Wnd *pkWnd = NULL;
			int iACCHeight = 0;
			CONT_LIST::iterator display_itr = m_kContList.begin();
			while ( display_itr!=m_kContList.end() )
			{
				pItem = *display_itr;
				if ( pItem->IsClosed() )
				{
					SAFE_DELETE(pItem);
					display_itr = m_kContList.erase( display_itr );
					continue;
				}

				if ( iSkip >= ItemHeight() )
				{
					pItem->Visible( false );
					iSkip -= ItemHeight();
				}
				else if ( iParentHeight < iACCHeight )
				{
					break;
				}
				else
				{
					pkWnd = (*display_itr)->GetWnd();
					if ( !pkWnd )
					{
						pkWnd = InitItem( (*display_itr) );
					}

					if ( pkWnd )
					{
						pt.y -= iSkip;

						pkWnd->Visible( true );
						pkWnd->Location( pt );
						m_kContDrawList.push_back(pkWnd);

						iACCHeight += ItemHeight()  - iSkip;//그려진 엘레멘트에서 Skip만큼 땡긴다.
						pt.y += ItemHeight() ;//다음위치.
						iSkip = __max( 0, iSkip - ItemHeight() );//스킵만큼 땡김
					}
				}

				++display_itr;
			}
			m_bUpdateStartPos = false;
		}

		std::for_each( m_kContDrawList.begin(), m_kContDrawList.end(), DisplayControl_Func() );
		return true;
	}

	return false;
}

bool CXUI_List2::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}

	bool bRet = false;

	//나 자신
	if( m_pWndUp && m_pWndDown && m_pWndMiddle )//영역 체크
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		int const iBarLen = iBottom - iTop;

		int const iTotalItemHeight = (int)m_kContMap.size() * ItemHeight();

		bool bAdjustedByDrag = false;

		if( iTotalItemHeight > Height() )
		{
			int const AvgLen = m_kContMap.size() ? iTotalItemHeight / (int)m_kContMap.size() : 0;

			int iStartPos = m_iDisplayStartPos;

			bool bIsForceUp = false;
			bool bIsForceDown = false;
			if(		rET == IEI_MS_DOWN 
				&&	dwValue == MEI_BTN_0	)
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
			}

			if( m_kContList.size() && rPT.z && ContainsPoint(m_sMousePos))
			{
				int iZ = static_cast<int>(rPT.z * 3.0f);
				if (0!=ScrollValue())
				{
					iZ = ScrollValue();
					if (rPT.z<0)
					{
						iZ*=(-1);
					}
				}

				SetDisplayStartPos( m_iDisplayStartPos - iZ );//0 이하가 되지 않게.
				bRet = true;
			}

			//가운데 버튼
			//가운데 버튼
			bool bScrolling = false;
			if( m_pWndMiddle != NULL && m_spWndScrollBtn == m_pWndMiddle )
			{
				if( rET == IEI_MS_UP )
				{
					m_spWndScrollBtn = NULL;
				}
				bScrolling = true;
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
					POINT3I ptMid = m_pWndMiddle->Location();

					if(!rPT.x)
					{
						ptMid += rPT;//포지션.

						ptMid.y = __max(ptMid.y, iTop);
						ptMid.y = __min(ptMid.y, iBottom);

						m_pWndMiddle->Location(ptMid);//버튼 위치 보정

						int const MidPos = ptMid.y - iTop;//상대위치

						iStartPos = (int)(((float)( iTotalItemHeight - Height() ) * ((float)MidPos/(float)iBarLen)));//남은 애들에서 상대위치.

						SetDisplayStartPos(iStartPos);//리스트 위치 보정
					}
				}				
				bAdjustedByDrag = true;
			}

			//스크롤 업 버튼
			if((m_pWndUp!= NULL && m_pWndUp->VPeekEvent(rET, rPT, dwValue) )
				|| bIsForceUp )
			{
				if( bIsForceUp || m_pWndUp->IsMouseDown() )
				{	
					bRet = true;
					SetDisplayStartPos( __max( iStartPos - AvgLen, 0) );//0 이하가 되지 않게.
				}
			}

			//스크롤 다운 버튼
			if(m_pWndDown != NULL && m_pWndDown->VPeekEvent(rET, rPT, dwValue) 
				|| bIsForceDown)
			{
				if( bIsForceDown || m_pWndDown->IsMouseDown() )
				{	
					bRet = true;
					SetDisplayStartPos( __min( iStartPos + AvgLen, iTotalItemHeight - Height() ));//전체길이-리스트컨트롤의 길이를 넘어서지 않도록
				}
			}
		}
		else
		{	//szTotalItemLen <= iBarLen
			SetDisplayStartPos(0);
		}

		if (!bAdjustedByDrag)
		{
			AdjustMiddleBtnPos();
		}
	}

	if ( !bRet )
	{
		CXUI_Wnd *pkWnd = NULL;
		CONT_CONTROL::iterator display_itr = m_kContDrawList.begin();
		for ( ; display_itr!=m_kContDrawList.end() ; ++display_itr )
		{
			pkWnd = *display_itr;
			if ( pkWnd )
			{
				bRet = pkWnd->VPeekEvent(rET, rPT, dwValue);
				if( bRet )
				{
					break;
				}
			}
		}
	}
	

	if( !bRet )
	{
		bRet = VPeekEvent_Default(rET, rPT, dwValue);
	}

	return PassEvent()? false: bRet;
}

void CXUI_List2::VRefresh()
{
	CXUI_Wnd::VRefresh();

	CXUI_Wnd *pkWnd = NULL;
	CONT_LIST::const_iterator display_itr = m_kContList.begin();
	for ( ; display_itr!=m_kContList.end() ; ++display_itr )
	{
		pkWnd = (*display_itr)->GetWnd();
		if ( pkWnd )
		{
			pkWnd->VRefresh();
		}
	}
}

void CXUI_List2::RemoveAllControls(bool const bIsClosed)
{
	CXUI_Wnd::RemoveAllControls(bIsClosed);
	m_pWndDown = NULL;
	m_pWndMiddle = NULL;
	m_pWndUp = NULL;
	m_pWndBar = NULL;
	ClearList();
}

void CXUI_List2::ClearList()
{	
	m_kSelectedItemGuid.Clear();
	m_kContDrawList.clear();
	m_kContMap.clear();
	CONT_LIST::iterator display_itr = m_kContList.begin();
	for ( ; display_itr!=m_kContList.end() ; ++display_itr )
	{
		SAFE_DELETE( *display_itr );
	}
	m_kContList.clear();

	if( m_pWndUp && m_pWndMiddle )
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		m_pWndMiddle->Location( m_pWndMiddle->Location().x, iTop );
	}
}

bool CXUI_List2::AdjustMiddleBarSize()
{
	if( m_pWndUp && m_pWndDown && m_pWndMiddle )
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iX = m_pWndUp->Location().x + (m_pWndUp->Width() - m_pWndBar->Width())/2  + 1;
		m_pWndBar->Location(iX, iTop, -3);
		m_pWndBar->Size(m_pWndBar->Width(), __max(0, m_pWndDown->Location().y - iTop));
		m_pWndBar->ImgSize(m_pWndBar->Size());
		return true;
	}
	return false;
}

bool CXUI_List2::AdjustMiddleBtnPos()
{
	if( m_pWndUp && m_pWndDown && m_pWndMiddle )
	{
		int const iTotalItemHeight = (int)m_kContMap.size() * ItemHeight();
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();
		int const iBarLen = iBottom - iTop;

		if( iTotalItemHeight <= iBarLen )
		{
			SetDisplayStartPos(0);
		}

		int const iPrevPos = m_iDisplayStartPos;
		SetDisplayStartPos( __max(iPrevPos, 0));//0 이하가 되지 않게.
		SetDisplayStartPos( __min(m_iDisplayStartPos, __max( iTotalItemHeight - Height(), 0)) );//전체길이-리스트컨트롤의 길이를 넘어서지 않도록

		float fScrRate = 0.0f;
		if( iTotalItemHeight > Height() )
		{
			fScrRate = (float)m_iDisplayStartPos / (float)(iTotalItemHeight - Height());//현재 포지션을, 디스플레이 되지않는 애들의 비율로 나눔.
		}

		m_pWndMiddle->Location( m_pWndMiddle->Location().x, (int)( iTop+( (iBottom - iTop ) * fScrRate ) ) );//최고점 + 전체길이/비율
		return true;
	}
	return false;
}

CXUI_Wnd* CXUI_List2::InitItem( CXUI_List2_Item* pItem )
{
	if( pItem )
	{
		CXUI_Wnd *pkWnd = pItem->GetWnd();
		if ( !pkWnd )
		{
			pkWnd = CXUI_Manager::Create(ElementID(), true);
			pItem->m_pWnd = pkWnd;
			assert(pkWnd);
		}

		if ( pkWnd )
		{
			pkWnd->OwnerGuid( pItem->OwnerGuid() );
			pkWnd->Parent( this );
			pkWnd->DoScript(SCRIPT_ON_CREATE);//리스트에서는 Pool을 사용하므로 ON_CREATE를 강제로 부른다

			if ( !ItemHeight() )
			{
				ItemHeight( pkWnd->Height() );
			}

			pkWnd->SetColorSet();
			pItem->Refresh();
			return pkWnd;
		}
	}
	return NULL;
}

CXUI_List2_Item* CXUI_List2::GetItem( BM::GUID const& kOwnerGuid )
{
	CONT_MAP::iterator itr = m_kContMap.find( kOwnerGuid );
	if ( itr != m_kContMap.end() )
	{
		return itr->second;
	}
	return NULL;
}

bool CXUI_List2::DeleteItem( BM::GUID const& kOwnerGuid )
{
	CONT_MAP::iterator itr = m_kContMap.find( kOwnerGuid );
	if ( itr != m_kContMap.end() )
	{
		itr->second->Close();
		m_kContMap.erase( itr );
		m_bUpdateStartPos = true;

		if ( m_kSelectedItemGuid == kOwnerGuid )
		{
			m_kSelectedItemGuid.Clear();
		}
		return true;
	}
	return false;
}

bool CXUI_List2::SetSelect( BM::GUID const& kOwnerGuid )
{
	CXUI_List2_Item* pItem = GetItem( kOwnerGuid );
	if ( pItem )
	{
		CXUI_List2_Item *pOldItem = GetItem( m_kSelectedItemGuid );
		if ( pOldItem )
		{
			pOldItem->DelSelect();
		}
		pItem->SetSelect();
		m_kSelectedItemGuid = kOwnerGuid;
		return true;
	}
	return false;
}

void CXUI_List2::SetBoundingBox( POINT2 const& ptLocation, POINT2 const& ptSize )
{
	CXUI_Wnd::SetBoundingBox(ptLocation,ptSize);

	//	POINT3I pt = TotalLocation()+ItemPos();
	POINT2 pt = ItemPos();//Parent 가 있으므로. ItemPos만 해도 됨.

// 	ItemHash::iterator disp_itor = m_ItemHash.begin();
// 	//이벤트 없이 지워질 경우도. 사이즈 보정이 필요하다.
// 	int iSkip = DisplayStartPos();
// 	int const iParentHeight = Height();
// 
// 	int iACCHeight = 0;//디스플레이된 엘레멘트의 누적길이
// 	while(disp_itor != m_ItemHash.end())
// 	{
// 		CXUI_Wnd *pWnd = (*disp_itor).second->m_pWnd;
// 		int const iElementHeight = pWnd->Height();
// 		if( iSkip >= iElementHeight
// 			||	iParentHeight < iACCHeight)
// 		{//보여지지 않음. 
// 			pWnd->Visible(false);
// 			iSkip-=iElementHeight;
// 		}
// 		else
// 		{//보여짐
// 			pt.y -= iSkip;
// 
// 			pWnd->Visible(true);
// 			pWnd->Location(pt - POINT2(0,0));
// 
// 			iACCHeight += iElementHeight-iSkip;//그려진 엘레멘트에서 Skip만큼 땡긴다.
// 			pt.y += iElementHeight;//다음위치.
// 			iSkip = max(0, iSkip-iElementHeight);//스킵만큼 땡김
// 		}
// 		++disp_itor;
// 	}
}

void CXUI_List2::SetDisplayStartItem( size_t iIndex )
{
	if ( iIndex >= m_kContMap.size() )
	{
		iIndex = m_kContMap.size() - 1;
	}

	SetDisplayStartPos( static_cast<int>(iIndex) * ItemHeight() );

	if( m_pWndUp && m_pWndDown && m_pWndMiddle )
	{
		int const iTop = m_pWndUp->Location().y + m_pWndUp->Height();
		int const iBottom = m_pWndDown->Location().y - m_pWndMiddle->Height();

		int const iTotalItemHeight = static_cast<int>(m_kContMap.size()) * ItemHeight();
		float const fHeight = static_cast<float>(iTotalItemHeight - Height());
		float const fBarLen = iBottom - iTop;
		int iMidPos = static_cast<int>(static_cast<float>(m_iDisplayStartPos) / fHeight * fBarLen);

		POINT3I ptMid = m_pWndMiddle->Location();
		ptMid.y = iMidPos + iTop;
		m_pWndMiddle->Location( ptMid );
	}
}

void CXUI_List2::SetDisplayStartPos( int const iStartPos )
{
	if ( iStartPos != m_iDisplayStartPos )
	{
		m_iDisplayStartPos = iStartPos;
		m_bUpdateStartPos = true;
	}
}

void CXUI_List2::Export( CExport &kExport )
{
	CXUI_List2_Item *pItem = NULL;
	XUI::CXUI_Wnd *pkWnd = NULL;
	CONT_LIST::iterator itr = m_kContList.begin();
	while ( itr!=m_kContList.end() )
	{
		pItem = *itr;
		pkWnd = (*itr)->GetWnd();
		if (	!pkWnd
			||	pkWnd->IsClosed() )
		{
			if (pkWnd && pkWnd->OwnerGuid() == m_kSelectedItemGuid )
			{
				m_kSelectedItemGuid.Clear();
			}

			m_kContMap.erase( pItem->OwnerGuid() );
			SAFE_DELETE( pItem );
			itr = m_kContList.erase( itr );
		}
		else
		{
			pkWnd->Parent(NULL);
			++itr;
		}
	}

	kExport.m_kSelectedItemGuid = m_kSelectedItemGuid;
	m_kContList.swap( kExport.m_kContList );
	m_kContMap.swap( kExport.m_kContMap );

	ClearList();
	AdjustMiddleBtnPos();
}

void CXUI_List2::Import( CExport &kExport )
{
	ClearList();
	m_kSelectedItemGuid = kExport.m_kSelectedItemGuid;
	m_kContList.swap( kExport.m_kContList );
	m_kContMap.swap( kExport.m_kContMap );

	CXUI_List2_Item *pItem = NULL;
	XUI::CXUI_Wnd *pkWnd = NULL;
	CONT_LIST::iterator itr = m_kContList.begin();
	while ( itr!=m_kContList.end() )
	{
		pItem = *itr;
		pkWnd = (*itr)->GetWnd();
		if (	!pkWnd
			||	pkWnd->IsClosed() )
		{
			m_kContMap.erase( pItem->OwnerGuid() );
			SAFE_DELETE( pItem );
			itr = m_kContList.erase( itr );
		}
		else
		{
			if ( !ItemHeight() )
			{
				ItemHeight( pkWnd->Height() );
			}

			InitItem( pItem );
			++itr;
		}
	}

	AdjustMiddleBtnPos();
	m_bUpdateStartPos = true;
}