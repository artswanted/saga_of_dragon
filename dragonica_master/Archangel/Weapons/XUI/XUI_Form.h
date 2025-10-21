#ifndef WEAPON_XUI_XUI_FORM_H
#define WEAPON_XUI_XUI_FORM_H

#include "XUI_Wnd.h"
#include "XUI_Control.h"

namespace XUI
{
	typedef enum eFormFlag
	{
		FF_NO_BG = 0x00000001,
	}EFormFlag;

	class CXUI_Form
		:	public CXUI_Wnd
	{
	public:
		CXUI_Form(void);
	public:
		virtual  ~CXUI_Form(void);
	public:
		virtual void VInit();

		virtual E_XUI_TYPE VType(){return E_XUI_FORM;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Form;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Form *pWnd = new CXUI_Form;

			pWnd->operator =(*this);
			return pWnd;
		}

		void operator = ( const CXUI_Form &rhs)
		{
			CXUI_Wnd::operator =(rhs);

			IsHaveBG(rhs.IsHaveBG());
		}

	protected:
		CXUI_Control* m_pControlMouseOver;// The control which is hovered over//마우스가 올려진 컨트롤

		CLASS_DECLARATION( bool, m_bIsHaveBG, IsHaveBG );		
	};
}

#endif // WEAPON_XUI_XUI_FORM_H