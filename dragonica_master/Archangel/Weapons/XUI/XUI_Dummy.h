#ifndef WEAPON_XUI_XUI_DUMMY_H
#define WEAPON_XUI_XUI_DUMMY_H

#include "XUI_Wnd.h"
#include "XUI_Control.h"

namespace XUI
{
	class CXUI_Dummy
		:	public CXUI_Wnd
	{
	public:
		CXUI_Dummy(void);
	public:
		virtual  ~CXUI_Dummy(void);
	public:
		virtual void VInit();

		virtual E_XUI_TYPE VType(){return E_XUI_DUMMY;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Dummy;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Dummy *pWnd = new CXUI_Dummy;

			*pWnd = *this;
			return pWnd;
		}
		virtual bool VDisplay();

		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);//이벤트를 처리 했다면 true 리턴
		virtual bool VPeekEvent_Default(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);

		static CXUI_Wnd* m_spDummyMouseOver;

	protected:
		CXUI_Control* m_pControlMouseOver;// The control which is hovered over//마우스가 올려진 컨트롤
		CLASS_DECLARATION_S(bool, ChildDisplay);
	};
}

#endif // WEAPON_XUI_XUI_DUMMY_H