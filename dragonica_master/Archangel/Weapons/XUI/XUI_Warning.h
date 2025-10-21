#ifndef WEAPON_XUI_XUI_WARNING_H
#define WEAPON_XUI_XUI_WARNING_H

#include "XUI_Wnd.h"

namespace XUI
{
	class CXUI_Warning
		:	public CXUI_Wnd
	{
	public:
		CXUI_Warning(void);
		virtual ~CXUI_Warning(void);

	public:
		virtual E_XUI_TYPE VType(){return E_XUI_WARNING;}
		virtual bool VDisplay();
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);

		virtual CXUI_Wnd* VCreate()const{return new CXUI_Warning;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Warning *pWnd = new CXUI_Warning;

			*pWnd = *this;
			return pWnd;
		}
	};
}

#endif // WEAPON_XUI_XUI_WARNING_H