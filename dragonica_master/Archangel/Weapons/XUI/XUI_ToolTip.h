#ifndef WEAPON_XUI_XUI_TOOLTIP_H
#define WEAPON_XUI_XUI_TOOLTIP_H

#include "XUI_Form.h"

namespace XUI
{
	class CXUI_ToolTip
		:	public CXUI_Form
	{
	public:
		CXUI_ToolTip(void);
	public:
		virtual ~CXUI_ToolTip(void);

	public:
		CLASS_DECLARATION( POINT2, m_PastSize, PastSize );
		virtual E_XUI_TYPE VType(){return E_XUI_TOOL_TIP;}
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual void VRefresh();
		virtual bool VDisplay();
		virtual void VInit();
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual void VOnCall();
		virtual void Close();
		virtual CXUI_Wnd* VCreate()const{return new CXUI_ToolTip;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_ToolTip *pWnd = new CXUI_ToolTip;

			*pWnd = *this;
			return pWnd;
		}
	protected:

		virtual	void ReloadText();

	protected:
		void* m_pToolTipImgBody;
		void* m_pToolTipImgTB;
		void* m_pToolTipImgLR;	
		void* m_pToolTipImgLock;
		void* m_pToolTipImgClose;
		void* m_pToolTipImgCorner;
		void* m_pImgStatic;

		int m_iToolTipImgBody;
		int m_iToolTipImgT;
		int m_iToolTipImgB;
		int m_iToolTipImgL;
		int m_iToolTipImgR;
		int m_iToolTipImgLock;
		int m_iToolTipImgClose;

		int m_iToolTipImgCorner1;
		int m_iToolTipImgCorner2;
		int m_iToolTipImgCorner3;
		int m_iToolTipImgCorner4;

		POINT2	m_kOutlineSize;

		CLASS_DECLARATION_S( bool, ReservedClose );
	};
}

#endif // WEAPON_XUI_XUI_TOOLTIP_H