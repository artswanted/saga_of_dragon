#ifndef WEAPON_XUI_XUI_SCROLL_H
#define WEAPON_XUI_XUI_SCROLL_H

#include "XUI_Form.h"

namespace XUI
{
	class CXUI_HScroll 
		:	public CXUI_Wnd
	{
	public:
		CXUI_HScroll();
		virtual ~CXUI_HScroll();

		virtual bool VOnTick( DWORD const dwCurTime );
		virtual CXUI_Wnd* VCreate()const{return new CXUI_HScroll;}
		virtual CXUI_Wnd* VClone();
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual void VInit();
		virtual void AdjustMiddleBtnPos();
	
		HRESULT CurValue( int const iValue );
		int CurValue()const{return m_iCurValue;}
		void operator = ( const CXUI_HScroll &rhs);

	protected:
		virtual void AdjustCur();
		bool IsValueChanged()const;

		CXUI_Wnd* GetBtnControl();
		
		CLASS_DECLARATION( int, m_iMinValue, MinValue );
		CLASS_DECLARATION( int, m_iMaxValue, MaxValue );
		CLASS_DECLARATION_S(std::wstring, ScrBtnName);
	protected:
		int m_iCurValue;
		bool m_bChangeValue;
		CXUI_Wnd* m_pkBtn;
	};
}

#endif // WEAPON_XUI_XUI_SCROLL_H