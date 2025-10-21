#ifndef WEAPON_XUI_XUI_CURSOR_H
#define WEAPON_XUI_XUI_CURSOR_H

#include "IconInfo.h"
#include "XUI_Control.h"

#include "XUI_ICON.h"

namespace XUI
{
	enum E_CURSOR_STATE
	{
		CS_NONE = 0,
		CS_STATE_1 = 1,
		CS_STATE_2 = 2,
		CS_STATE_3 = 3,
		CS_STATE_4 = 4,
	};

	class CXUI_Cursor
		:	public CXUI_Wnd
	{
	public:
		CXUI_Cursor(void);
	public:
		virtual ~CXUI_Cursor(void);

		virtual E_XUI_TYPE VType(){return E_XUI_CURSOR;}
		virtual void VInit();
		virtual bool VDisplay();

		bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		void ReleaseDefaultImg();
		virtual CXUI_Wnd* VCreate()const;

		static CXUI_Wnd* GetInstance();

		SIconInfo& IconInfo(){return m_IconInfo;}
		virtual CXUI_Wnd* VClone()
		{
			return NULL;
		}
		
		void ResetCursorState();

	public:
		CLASS_DECLARATION_S(E_CURSOR_STATE, CursorState);
		static SIconInfo m_IconInfo;//이놈의 키는 몇번인가.

		eInputEventIndex m_kBtn1State;
		eInputEventIndex m_kBtn2State;
	};
}

#endif // WEAPON_XUI_XUI_CURSOR_H