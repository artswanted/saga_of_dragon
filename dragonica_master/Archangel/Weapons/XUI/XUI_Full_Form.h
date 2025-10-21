#ifndef WEAPON_XUI_XUI_FULL_FORM_H
#define WEAPON_XUI_XUI_FULL_FORM_H

#include "XUI_Wnd.h"

namespace XUI
{
	class CXUI_Full_Form : public CXUI_Wnd
	{
	public:
		CXUI_Full_Form();
		virtual ~CXUI_Full_Form();

		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);
		virtual void VScale(float const fScale);
		virtual void VOnCall();
		virtual E_XUI_TYPE VType(){return E_XUI_FULL_FROM;}
		virtual CXUI_Wnd* VCreate()const{return new CXUI_Full_Form;}
		virtual void Location(int const x, int const y, int const z, bool const bIsModifyBoundingBox = true);
		virtual void VAlign();

		void operator = ( const CXUI_Full_Form &rhs);

		void DoFullScreen();
		void UnDoFullScreen();
	private:
		POINT2 m_kOriginSize;
		POINT2 const& GetOriginSize() const;
		void SetOriginSize(POINT2 const& size);

		POINT2 m_kOriginLoc;
		POINT2 const& GetOriginLocation() const;
		void SetOriginLocation(POINT2 const& loc);
	};
}

#endif // WEAPON_XUI_XUI_FULL_FORM_H