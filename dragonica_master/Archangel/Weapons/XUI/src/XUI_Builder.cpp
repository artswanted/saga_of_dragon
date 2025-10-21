#include "stdafx.h"
#include "XUI_Builder.h"

using namespace XUI;

CXUI_Builder::CXUI_Builder(void)
{
	CountX(0);
	CountY(0);
	
	GabX(0);
	GabY(0);

	StartIndex(0);
	ReserveXY(false);
}

CXUI_Builder::~CXUI_Builder(void)
{
}

void CXUI_Builder::VInit()
{
	if (NULL == Parent())
	{
		return;
	}
	CXUI_Wnd *pkOrgWnd = Parent()->GetControl(TargetID());
	if(NULL == pkOrgWnd)
	{
		OutputDebugString(_T("Original Window is NULL\n"));
		assert(0);
		return;
	}

	int iIndex = StartIndex();

	int iCountX = CountX();
	int iCountY = CountY();
	if ( ReserveXY() )
	{
		std::swap( iCountX, iCountY );
	}

	int iRetX = 0;
	int iRetY = 0;

	for(int y = 0;iCountY>y; y++)
	{
		for(int x = 0; iCountX>x; x++)
		{
			CXUI_Wnd *pkCloneWnd = pkOrgWnd->VClone();

			//BM::vstring kStr;
			TCHAR szTemp[_MAX_PATH] = {0, };
			std::wstring wstrID;
			if(0 == TrgControlName().size())
			{//Default Type
				_stprintf_s(szTemp, _MAX_PATH, _T("%s%d"), TargetID().c_str(), iIndex);
				//kStr = TargetID();
				//kStr += iIndex;
				wstrID = szTemp;
				UPR(wstrID);
			}
			else
			{//Custome Type
				_stprintf_s(szTemp, _MAX_PATH, _T("%s%d"), TrgControlName().c_str(), iIndex);
				wstrID = szTemp;
			}

			if (pkCloneWnd)
			{
				pkCloneWnd->VInit();
				if ( ReserveXY() )
				{
					iRetX = Location().x+(y*GabX());
					iRetY = Location().y+(x*GabY());
				}
				else
				{
					iRetX = Location().x+(x*GabX());
					iRetY = Location().y+(y*GabY());
				}
				
				pkCloneWnd->ID(wstrID); 

				if ( Parent()->VRegistChild(pkCloneWnd) )
				{
					//등록을 먼저해서 Parent를 짓고가야함
					pkCloneWnd->Location(iRetX, iRetY);
					pkCloneWnd->BuildIndex(iIndex);
					pkCloneWnd->DoScript(SCRIPT_ON_BUILD);
				}
				else
				{
					SAFE_DELETE( pkCloneWnd );
				}
			}

			++iIndex;
		}
	}

	std::wstring wstrID = TargetID();

	UPR(wstrID);
	TargetID(wstrID);

	Parent()->RemoveControl(TargetID());//기준점은 지워버린다.
}

void CXUI_Builder::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{	
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);
	
	if( ATTR_TARGET_ID == wstrName ){TargetID( wstrValue );	}
	else if( ATTR_X_COUNT == wstrName ){CountX( (int)vValue );	}
	else if( ATTR_Y_COUNT == wstrName ){CountY( (int)vValue );	}
	else if( ATTR_X_GAB == wstrName ){GabX( (int)vValue );	}
	else if( ATTR_Y_GAB == wstrName ){GabY( (int)vValue );	}
	else if( ATTR_START_INDEX == wstrName ){StartIndex((int)vValue);}
	else if( ATTR_TRG_CUSTOME_ID == wstrName ){TrgControlName(vValue);}
	else if( ATTR_IS_RESERVE == wstrName ){ReserveXY((int)vValue);}
}

bool CXUI_Builder::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)//이벤트는 키 UP으로만 동작.
{//이벤트를 전혀 먹지 않는다.
	return false;
}

void CXUI_Builder::operator = ( const CXUI_Builder &rhs)
{
	CXUI_Wnd::operator =(rhs);

	TargetID(rhs.TargetID());
	CountX(rhs.CountX());
	CountY(rhs.CountY());
	GabX(rhs.GabX());
	GabY(rhs.GabY());
	StartIndex(rhs.StartIndex());
	TrgControlName(rhs.TrgControlName());
	ReserveXY(rhs.ReserveXY());
}
