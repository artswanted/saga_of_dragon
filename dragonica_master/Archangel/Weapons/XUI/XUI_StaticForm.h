#ifndef WEAPON_XUI_XUI_STATICFORM_H
#define WEAPON_XUI_XUI_STATICFORM_H

#include "XUI_Form.h"

namespace XUI
{
	class CXUI_StaticForm
		:	public CXUI_Form
	{
	public:
		CXUI_StaticForm(void);
	public:
		virtual ~CXUI_StaticForm(void);

	public:
		virtual E_XUI_TYPE VType(){return E_XUI_STATIC_FORM;}
		virtual void VRefresh();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
//		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();
		virtual void VInit();

		virtual CXUI_Wnd* VCreate()const{return new CXUI_StaticForm;}

		void operator = ( const CXUI_StaticForm &rhs);
		virtual CXUI_Wnd* VClone();

	private:

		void	ReleaseAllResource();

	protected:

		CLASS_DECLARATION( int, m_iStaticNum, StaticNum );

		std::wstring m_wstrImgBody;
		std::wstring m_wstrImgTB;
		std::wstring m_wstrImgLR;
		std::wstring m_wstrImgCorner;

		void* m_pImgBody;
		void* m_pImgTB;
		void* m_pImgLR;
		void* m_pImgCorner;
		void* m_pImgStatic;

		int m_iImgBody;
		int m_iImgT;
		int m_iImgB;
		int m_iImgL;
		int m_iImgR;

		int m_iImgCorner1;
		int m_iImgCorner2;
		int m_iImgCorner3;
		int m_iImgCorner4;

		POINT2 m_ptBody;
		POINT2 m_ptTB;
		POINT2 m_ptLR;
		POINT2 m_ptCorner;
	};
}

#endif // WEAPON_XUI_XUI_STATICFORM_H