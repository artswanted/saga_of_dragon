#ifndef WEAPON_XUI_XUI_TREE_H
#define WEAPON_XUI_XUI_TREE_H

#include "XUI_CheckButton.h"
#include "XUI_Form.h"
#include "BM/ObjectPool.h"

//리스트 클래스
namespace XUI
{
	class CXUI_Tree//기본 + - 달고 form을 아래에 추가 가능 하도록.
		:	public CXUI_Form
	{
	public:
		CXUI_Tree(void);
	public:
		~CXUI_Tree(void);

	public:
		virtual void VInit();
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual bool VDisplay();
		virtual bool VDisplayEnd();
		virtual void VRefresh();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		
		SListItem* AddItem(std::wstring const& rkKey, void* pData = NULL);//내 아이 하나를 가져 올 수 있다.
		SListItem* FirstItem();
		SListItem* NextItem(SListItem* pBase);
		SListItem* PrevItem(SListItem* pBase);
		SListItem* DeleteItem(SListItem* pBase);
		void DeleteAllItem();
		int GetItemCount();

		void CheckState(bool const bIsCheck);
		bool CheckState();
		
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Tree;}
		virtual SListItem* FindItem(std::wstring const& wstrName);

		CLASS_DECLARATION_S(std::wstring, ElementID);//리스트에 들어가는 컨트롤의 아이디
		CLASS_DECLARATION( POINT2, m_ItemPos, ItemPos );
	protected:
		virtual void InvalidateChild();

		typedef std::map<int, SListItem*> ItemHash;//리스트는 선택가능해야 하므로 버튼으로 이루어짐

		int m_iInsertIndex;
		ItemHash			m_ItemHash;
		BM::TObjectPool<SListItem> m_ElementPool;

		CXUI_CheckButton	 *m_pkBtnExt;
	};
}

#endif // WEAPON_XUI_XUI_TREE_H