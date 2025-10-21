#ifndef WEAPON_XUI_XUI_SCRIPT_H
#define WEAPON_XUI_XUI_SCRIPT_H

#include "XUI_Wnd.h"
namespace XUI
{
	class CXUI_Script
		:	public CXUI_Wnd
	{
	public:
		CXUI_Script(void);
	public:
		virtual ~CXUI_Script(void);
		virtual E_XUI_TYPE VType(){return E_XUI_SCRIPT;}
	public:
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue){return false;}
		virtual bool VDisplay(){return false;}
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Script;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Script *pWnd = new CXUI_Script;

			*pWnd = *this;
			return pWnd;
		}
	};

	class CXUI_HotKey
		:	public CXUI_Wnd
	{
	public:
		CXUI_HotKey(void);
	public:
		virtual ~CXUI_HotKey(void);
		virtual E_XUI_TYPE VType(){return E_XUI_HOTKEY;}
	public:
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue){return false;}
		virtual bool VDisplay(){return false;}
		virtual CXUI_Wnd* VCreate()const{return new CXUI_HotKey;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_HotKey *pWnd = new CXUI_HotKey;

			*pWnd = *this;
			return pWnd;
		}
	};

	class CXUI_GlobalHotkey
		:	public CXUI_HotKey
	{
	public:
		CXUI_GlobalHotkey(void);
	public:
		virtual ~CXUI_GlobalHotkey(void);
	public:
		virtual CXUI_Wnd* VCreate() const {return new CXUI_GlobalHotkey;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_GlobalHotkey *pWnd = new CXUI_GlobalHotkey;
			*pWnd = *this;
			return pWnd;
		}
	};
}

#endif // WEAPON_XUI_XUI_SCRIPT_H