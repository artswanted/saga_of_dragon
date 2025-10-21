#ifndef WEAPON_XUI_XUI_CHECKBUTTON_H
#define WEAPON_XUI_XUI_CHECKBUTTON_H

#include "XUI_Button.h"

namespace XUI
{
	class CXUI_CheckButton
		:	public CXUI_Button
	{
	public:
		CXUI_CheckButton(void);
	public:
		virtual ~CXUI_CheckButton(void);
	public:

		virtual E_XUI_TYPE VType(){return E_XUI_CHECKBTN;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();
		virtual bool VOnTick( DWORD const dwCurTime );

		virtual CXUI_Wnd* VCreate()const{return new CXUI_CheckButton;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Button *pWnd = new CXUI_CheckButton;

			pWnd->operator =(*this);
			return pWnd;
		}

		void operator = ( const CXUI_CheckButton &rhs);
	public:	
		virtual	void Disable(bool const& value);

		CLASS_DECLARATION_NO_SET( bool, m_bCheck, Check );
		void Check(bool const& value);

		CLASS_DECLARATION( bool, m_bClickLock, ClickLock );	//잠그면 클릭해도 무반응
	private:
		int m_iIndex;
	};
}

#endif // WEAPON_XUI_XUI_CHECKBUTTON_H