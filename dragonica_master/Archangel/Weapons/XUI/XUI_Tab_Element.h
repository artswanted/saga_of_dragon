#ifndef WEAPON_XUI_XUI_TAB_ELEMENT_H
#define WEAPON_XUI_XUI_TAB_ELEMENT_H

#include "XUI_Control.h"

namespace XUI
{
	class CXUI_Tab_Element
		:	public CXUI_Control
	{
	public:
		CXUI_Tab_Element();
		virtual ~CXUI_Tab_Element();

	public:
		virtual E_XUI_TYPE VType(){return E_XUI_TAB_ELEMENT;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VDisplay();

	protected:
		CLASS_DECLARATION_PTR( void*, m_pTabImg, TabImg );
		CLASS_DECLARATION( POINT2, m_TabImgSize, TabImgSize );
		CLASS_DECLARATION( int, m_TabImgIdx, TabImgIdx );//-1 이 초기값이므로 DWORD 로 쓰지말것.

		virtual void VOnClose();
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Tab_Element;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Tab_Element *pWnd = new CXUI_Tab_Element;

			*pWnd = *this;
			return pWnd;
		}
	};
}

#endif // WEAPON_XUI_XUI_TAB_ELEMENT_H