#ifndef WEAPON_XUI_XUI_BUILDER_H
#define WEAPON_XUI_XUI_BUILDER_H

#include "XUI_Wnd.h"

namespace XUI 
{
	class CXUI_Builder
		: public CXUI_Wnd
	{
	public:
		CXUI_Builder(void);
	public:
		virtual ~CXUI_Builder(void);

		virtual void VInit();
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);//이벤트는 키 UP으로만 동작.
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Builder;}
		virtual CXUI_Wnd* VClone()
		{
			CXUI_Builder *pWnd = new CXUI_Builder;

			pWnd->operator =(*this);
			return pWnd;
		}
		void operator = ( const CXUI_Builder &rhs);
	public:
		CLASS_DECLARATION_S( std::wstring, TargetID );
		CLASS_DECLARATION_S( int, CountX );
		CLASS_DECLARATION_S( int, CountY );
		
		CLASS_DECLARATION_S( int, GabX );
		CLASS_DECLARATION_S( int, GabY );

		CLASS_DECLARATION_S(int, StartIndex);
		CLASS_DECLARATION_S(std::wstring, TrgControlName);
		CLASS_DECLARATION_S(bool, ReserveXY);
	};
};

#endif // WEAPON_XUI_XUI_BUILDER_H