#ifndef WEAPON_XUI_XUI_BUTTON_H
#define WEAPON_XUI_XUI_BUTTON_H

#include "XUI_Control.h"

namespace XUI
{
	class CXUI_Button
		:	public CXUI_Control
	{
	public:
		CXUI_Button(void);
	public:
		virtual ~CXUI_Button(void);
	public:

		virtual E_XUI_TYPE VType(){return E_XUI_BTN;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual bool VDisplay();
		virtual bool VOnTick( DWORD const dwCurTime );
		virtual void VOnClose();
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Button;}
		void operator = ( const CXUI_Button &rhs);
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Button *pWnd = new CXUI_Button;

			pWnd->operator =(*this);
			return pWnd;
		}

		void ButtonChangeImage(std::wstring const& wstrImgName, int iWidth=0, int iHeight=0);
		void ButtonRealeaseImage();
		virtual void VInit();
	public:	
		CLASS_DECLARATION_PTR( void*, m_pBtnImg, BtnImg );
		CLASS_DECLARATION( POINT2, m_BtnImgSize, BtnImgSize );
		CLASS_DECLARATION_INVALIDATE( int, m_BtnImgIdx, BtnImgIdx );//-1 이 초기값이므로 DWORD 로 쓰지말것.
		//CLASS_DECLARATION_NO_SET( bool, m_bDisable, Disable );

		virtual	void Disable(bool const& value);
		virtual	void IsMouseDown(bool const& value);

//		CLASS_DECLARATION_S( SSizedScale, SizedScale );
		CLASS_DECLARATION_S( std::wstring, BtnImgName );
		CLASS_DECLARATION_INVALIDATE( int, m_BtnIdx, BtnIdx );
		CLASS_DECLARATION(bool, m_bEnableChangeFontColor, EnableChangeFontColor);
		CLASS_DECLARATION(bool, m_bInvertBold, InvertBold);
		CLASS_DECLARATION_S( bool, BtnImgVertical );	//세로로 배열된 버튼인가
		CLASS_DECLARATION_S_NO_SET(int, BtnDownIdx);
		CLASS_DECLARATION_S(bool, BtnSingleImage);

		SButtonColorType m_kFontColorByState;

	private:
		CLASS_DECLARATION_S(int, FontColorType);

	private:
		bool m_bUseTwinkleImg;
	};
}

#endif // WEAPON_XUI_XUI_BUTTON_H