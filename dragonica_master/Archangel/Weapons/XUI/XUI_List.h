#ifndef WEAPON_XUI_XUI_LIST_H
#define WEAPON_XUI_XUI_LIST_H

#include "XUI_Button.h"
#include "XUI_Form.h"
#include "BM/ObjectPool.h"

#define NONE_SELECT_ITEM	-1	//아이템이 선택안됬을때, 혹은 초기값

#define XUI_LIST_CHECK

//리스트 클래스
namespace XUI
{
	class CXUI_List
		:	public CXUI_Form
	{
	public:
		CXUI_List();
		virtual ~CXUI_List();
		virtual E_XUI_TYPE VType(){return E_XUI_LIST;}
	public:
		virtual bool VOnTick( DWORD const dwCurTime );		
		SListItem* AddItem(std::wstring const& rkKey, void* pData = NULL);
		SListItem* FirstItem();
		SListItem* NextItem(SListItem* pBase);
		SListItem* PrevItem(SListItem* pBase);
		SListItem* DeleteItem(SListItem* pBase);
		bool DeleteItem(int const& iIndex);
		bool DeleteItem();
		virtual CXUI_Wnd* VCreate()const{return new CXUI_List;}
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();
		virtual bool VDisplayEnd();
		virtual void VRefresh();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual void VInit();
		virtual void ClearList();
		virtual int const GetTotalItemCount()const{ return static_cast<int>( m_ItemHash.size() ); }
		virtual bool const empty()				{ return m_ItemHash.empty(); }
		virtual size_t GetTotalItemLen()const;
		virtual bool AdjustMiddleBtnPos();
		virtual bool AdjustMiddleBarSize();
		virtual bool AdjustDownBtnPos();
		virtual SListItem* FindItem(std::wstring const& wstrName);
		virtual SListItem* GetItemAt(int const& iNum);
		virtual void SetColorSet();	//XUI에 디폴트로 지정된 셋팅 값들을 로딩
		virtual void Size(POINT2 const& rPT, bool const bIsModifyBoundingBox = true );
		virtual POINT2 const& Size()const{ return CXUI_Wnd::Size();}
		virtual void RemoveAllControls(bool const bIsClosed = false);
		void DeleteAllItem();

	protected:
		virtual void InvalidateChild();

		typedef std::map<int, SListItem*> ItemHash;//리스트는 선택가능해야 하므로 버튼으로 이루어짐
		typedef std::vector< SListItem* > ContItemPointer;

		virtual	void SetBoundingBox(POINT2 const& ptLocation, POINT2 const& ptSize);

		CLASS_DECLARATION( POINT2, m_ItemPos, ItemPos );
		CLASS_DECLARATION( POINT2, m_ItemSize, ItemSize );
//		CLASS_DECLARATION( bool, m_bCanSelect, CanSelect );
		CLASS_DECLARATION( bool, m_bCanMultiSelect, CanMultiSelect );	//다중 선택
		CLASS_DECLARATION_S_INVALIDATE( int, DisplayStartPos );//픽셀단위 디스플레이 시작 포지션
		CLASS_DECLARATION( POINT2, m_ItemImgSize, ItemImgSize );
		CLASS_DECLARATION( int, m_iSelectItemNum, SelectItemNum );
		CLASS_DECLARATION_S(std::wstring, ScrUpName);
		CLASS_DECLARATION_S(std::wstring, ScrDownName);
		CLASS_DECLARATION_S(std::wstring, ScrMiddleName);
		CLASS_DECLARATION_S(std::wstring, ScrBarName);
		CLASS_DECLARATION_S(std::wstring, ElementID);//리스트에 들어가는 컨트롤의 아이디
		CLASS_DECLARATION_S( int, ScrollValue );
		CLASS_DECLARATION_S( size_t, ItemLen );
		CLASS_DECLARATION_S_NO_SET(bool, IgnoreBtnParentRect);

	protected:
#ifdef XUI_LIST_CHECK
		bool			m_bIsClearList;
#endif
		bool			m_bIsSelectable;	//선택 가능한지
		ItemHash		m_ItemHash;
		int				m_iInsertIndex;
		CXUI_Wnd*		m_pWndUp;
		CXUI_Wnd*		m_pWndDown;
		CXUI_Wnd*		m_pWndMiddle;
		CXUI_Wnd*		m_pWndBar;


		BM::TObjectPool<SListItem> m_ElementPool;
		
	};
}

#endif // WEAPON_XUI_XUI_LIST_H