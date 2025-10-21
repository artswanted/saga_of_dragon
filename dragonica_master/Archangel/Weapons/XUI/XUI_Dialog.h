#ifndef WEAPON_XUI_XUI_DIALOG_H
#define WEAPON_XUI_XUI_DIALOG_H

#include "XUI_Form.h"

namespace XUI
{
	class CXUI_Dialog
		:	public CXUI_Form
	{
	public:
		CXUI_Dialog();
		virtual ~CXUI_Dialog();

		virtual E_XUI_TYPE VType(){return E_XUI_DIALOG;}

		virtual void VInit();
//		void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
//		int GetCaptionHeight() const { return m_nCaptionHeight; }
//		void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
//		void SetCaptionText( const WCHAR *pwszText ) { m_wstrCaption = pwszText; }
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
	
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Dialog;}

		virtual CXUI_Wnd* VClone()
		{
			CXUI_Dialog *pWnd = new CXUI_Dialog;

			*pWnd = *this;
			return pWnd;
		}
	private:
//		CXUI_Wnd	m_TitleBar;
		
//		CXUI_Wnd	m_BtnMin;
//		CXUI_Wnd	m_BtnExit;
		
//		CLASS_DECLARATION( std::wstring, m_TitleFont, TitleFont );
//		CLASS_DECLARATION( std::wstring, m_TitleText, TitleText );
//		CLASS_DECLARATION( POINT2, m_TitleTextPos, TitleTextPos );
	};
}

#endif // WEAPON_XUI_XUI_DIALOG_H