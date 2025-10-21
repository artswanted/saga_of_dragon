#include "stdafx.h"

#include "XUI_Full_Form.h"

using namespace XUI;

CXUI_Full_Form::CXUI_Full_Form()
{
	m_kOriginSize = POINT2::NullData();
	m_kOriginLoc = POINT2::NullData();
}

CXUI_Full_Form::~CXUI_Full_Form()
{

}

void CXUI_Full_Form::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_X == wstrName )
	{	
		Location( static_cast<int>(vValue), CXUI_Wnd::Location().y, 0);	
		SetOriginLocation( POINT2(CXUI_Wnd::Location().x, CXUI_Wnd::Location().y));
	}
	else if( ATTR_Y == wstrName )
	{	
		Location( CXUI_Wnd::Location().x, static_cast<int>(vValue), 0);	
		SetOriginLocation( POINT2(CXUI_Wnd::Location().x, CXUI_Wnd::Location().y));
	}
	else if( ATTR_W == wstrName )
	{	
		Size( static_cast<int>(vValue), Height() );	
		SetOriginSize( Size() );
	}
	else if( ATTR_H == wstrName )
	{	
		Size( Width(), static_cast<int>(vValue) );	
		SetOriginSize( Size() );
	}
}

void CXUI_Full_Form::VOnCall()
{
	CXUI_Wnd::VOnCall();

	DoFullScreen();
}

void CXUI_Full_Form::VScale(float const fScale)
{
	Scale(fScale+fScale*0.0013f);

	float fX = (float)(GetOriginSize().x)*fScale;
	float fY = (float)(GetOriginSize().y)*fScale;

	CXUI_Wnd::Location( static_cast<int>(GetOriginLocation().x*fScale), 
		static_cast<int>(GetOriginLocation().y*fScale));

	Size( static_cast<int>(GetOriginSize().x * fScale) + 1, 
		static_cast<int>(GetOriginSize().y * fScale) + 1);

	for_each( m_contControls.begin(), m_contControls.end(), ScaleControl_Func(fScale) );
}

void CXUI_Full_Form::operator = ( const CXUI_Full_Form &rhs )
{
	CXUI_Wnd::operator =(rhs);

	SetOriginLocation(rhs.GetOriginLocation());
	SetOriginSize(rhs.GetOriginSize());
}

POINT2 const& CXUI_Full_Form::GetOriginSize() const
{
	return m_kOriginSize;
}

void CXUI_Full_Form::SetOriginSize(POINT2 const& size)
{
	m_kOriginSize = size;
}

POINT2 const& CXUI_Full_Form::GetOriginLocation() const
{
	return m_kOriginLoc;
}

void CXUI_Full_Form::SetOriginLocation(POINT2 const& loc)
{
	m_kOriginLoc = loc;
}

void CXUI_Full_Form::DoFullScreen()
{
	if(NULL==Parent())
	{
		float wRate = static_cast<float>(GetResolutionSize().x) / static_cast<float>(GetOriginSize().x);
		float hRate = static_cast<float>(GetResolutionSize().y) / static_cast<float>(GetOriginSize().y);
		float rate = max(wRate,hRate);

		VScale(rate);

		POINT3 const pos((GetResolutionSize().x - Size().x)/2, (GetResolutionSize().y - Size().y)/2, CXUI_Wnd::Location().z);

		CXUI_Wnd::Location(pos);
	}
}

void CXUI_Full_Form::UnDoFullScreen()
{
	if(NULL==Parent())
	{
		VScale(1.0f);
		m_Size.x = m_Size.x + 1;
	}
}

void CXUI_Full_Form::Location(int const x, int const y, int const z, bool const bIsModifyBoundingBox)
{
	if(NULL==Parent())
	{
		m_Location.x = x; m_Location.y = y; m_Location.x = z;

		if( bIsModifyBoundingBox )
		{
			VOnLocationChange();
			SetBoundingBox( CXUI_Wnd::Location(), Size() );
		}
	}
	else
	{
		__super::Location(x,y,z,bIsModifyBoundingBox);
	}	
}

void CXUI_Full_Form::VAlign()
{
	DoFullScreen();
}