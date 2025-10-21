#ifndef WEAPON_XUI_XUI_ICON_H
#define WEAPON_XUI_XUI_ICON_H

#include "XUI_Wnd.h"
#include "XUI_Control.h"
#include "IconInfo.h"

namespace XUI
{
	class CXUI_Icon
		:	public CXUI_Control
	{
	public:
		CXUI_Icon(void);
	public:
		virtual ~CXUI_Icon(void);

		virtual E_XUI_TYPE VType(){return E_XUI_ICON;}

		virtual void VInit();
		virtual bool VDisplay();

		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);

		virtual CXUI_Wnd* VCreate()const{return new CXUI_Icon;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Icon *pWnd = new CXUI_Icon;

			*pWnd = *this;
			return pWnd;
		}

		void operator = ( const CXUI_Icon &rhs)
		{
			CXUI_Wnd::operator =(rhs);

			IsStaticIcon(rhs.IsStaticIcon());
			m_IconInfo = rhs.m_IconInfo;
			ClipRect(rhs.ClipRect());
		}

		bool SetIconInfo(SIconInfo const& iconInfo);
		bool GetClipRect(RECT &rkRect);
		//void RegisterWndToIconMap();
		//void UnregisterWndToIconMap();
		//static CXUI_Icon* FindIconWnd(int const iIconGrp, int const iIconKey);
	protected:
//		CLASS_DECLARATION( int, m_iIconGrp, IconGrp );//어느 그룹에 속해 있는가.
//		CLASS_DECLARATION( int, m_iIconKey, IconKey );//이놈의 키는 몇번인가.
		
		CLASS_DECLARATION_S(bool, IsStaticIcon);//이놈의 키는 몇번인가.
		CLASS_DECLARATION_NO_SET( SIconInfo, m_IconInfo, IconInfo );//이놈의 키는 몇번인가.
		CLASS_DECLARATION_S(RECT, ClipRect);

		//typedef std::map<int, CXUI_Icon*> IconWndMap;
		//static IconWndMap ms_kIconWndMap;
	};
}

#endif // WEAPON_XUI_XUI_ICON_H