#ifndef WEAPON_XUI_XUI_TEXTTABLE_BASE_h
#define WEAPON_XUI_XUI_TEXTTABLE_BASE_h

namespace XUI
{
	class CXUI_TextTable_Base
	{
	public:
		CXUI_TextTable_Base(){}
		virtual ~CXUI_TextTable_Base(){}
	
	public:
		virtual const TCHAR *GetTextW(unsigned long ulTextNo)const = 0;
	};
}

#endif // WEAPON_XUI_XUI_TEXTTABLE_BASE_h