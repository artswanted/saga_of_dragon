#ifndef WEAPON_XUI_XUI_TAB_H
#define WEAPON_XUI_XUI_TAB_H

#include "XUI_Control.h"

namespace XUI
{
	class CXUI_Tab
		:	public CXUI_Control
	{
	public:
		CXUI_Tab();
		virtual ~CXUI_Tab();

	public:
		virtual E_XUI_TYPE VType(){return E_XUI_TAB;}
		virtual void VOnClose();

		virtual CXUI_Wnd* VCreate()const{return new CXUI_Tab;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Tab *pWnd = new CXUI_Tab;

			*pWnd = *this;
			return pWnd;
		}
	};
}

#endif // WEAPON_XUI_XUI_TAB_H