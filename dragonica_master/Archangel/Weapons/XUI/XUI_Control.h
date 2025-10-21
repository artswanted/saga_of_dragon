#ifndef WEAPON_XUI_XUI_CONTROL_H
#define WEAPON_XUI_XUI_CONTROL_H

#include "XUI_Wnd.h"

namespace XUI
{
	class CXUI_Control
			:	public CXUI_Wnd
	{
	public:
		CXUI_Control(void);
	public:
		virtual ~CXUI_Control(void);
	};
}

#endif // WEAPON_XUI_XUI_CONTROL_H