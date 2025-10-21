#ifndef WEAPON_XUI_XUI_EDIT_MULTILINE_H
#define WEAPON_XUI_XUI_EDIT_MULTILINE_H

#include "XUI_Edit.h"
//#include "CSIME/CSIME.h"

namespace XUI
{
	class CXUI_Edit_MultiLine : public CXUI_Edit
	{
	public:
		CXUI_Edit_MultiLine(void);
		~CXUI_Edit_MultiLine(void);
		virtual E_XUI_TYPE VType(){return E_XUI_EDIT_MULTILINE;}

		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VDisplay();
		virtual void VLoseFocus(bool const bUpToParent = false);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Edit_MultiLine;}

		virtual void OnHookEvent();

		virtual bool VOnTick( DWORD const dwCurTime );

		virtual void EditText(std::wstring const& wstrValue,bool bKeepTextBlock = false);
		virtual bool SetEditFocus(bool const bIsJustFocus);

		virtual void VOnClose();

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Edit_MultiLine *pWnd = new CXUI_Edit_MultiLine;

			pWnd->operator =(*this);
			return pWnd;
		}

		void operator = ( const CXUI_Edit_MultiLine &rhs)
		{
			CXUI_Wnd::operator =(rhs);

			MultiLineCount(rhs.MultiLineCount());
		}
	protected:
		
		virtual int const MakeEditString(std::wstring & Val, int const iCarot);
		virtual void RenderBlock(std::wstring & Val);	//선택영역 블록 출력 
		virtual POINT2 CalcCaretPos(std::wstring const& wstrReal, CS::CARETDATA const& rkData, XUI::CXUI_Font* pFont, int const iLine = 1);
		virtual void MoveCarotToClickPos(int &iStart, int &iEnd);	
		virtual int GetClickTextPos();					//마우스 클릭한 위치(int)얻기 //만들어야 된다
		virtual void RenderCarot(POINT2& pt);	//캐럿출력

		CLASS_DECLARATION_S(int, MultiLineCount);
		bool m_bIsSame;
		std::wstring m_kOldVal;
		CS::CARETDATA m_kOldCaretData;
	private:
		
	};
}

#endif // WEAPON_XUI_XUI_EDIT_MULTILINE_H