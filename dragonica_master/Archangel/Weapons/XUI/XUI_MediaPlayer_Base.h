#ifndef WEAPON_XUI_XUI_MEDIAPLAYER_BASE_H
#define WEAPON_XUI_XUI_MEDIAPLAYER_BASE_H

#include <tchar.h>
#include <string>
#include "BM/Point.h"


namespace XUI
{
	class CXUI_MediaPlayer_Base
	{
	public:
		CXUI_MediaPlayer_Base(){}
		virtual ~CXUI_MediaPlayer_Base(){}
	
	public:
		virtual bool PlaySound(std::wstring const& wstrFileName) = 0;
		virtual bool PlaySoundByID(std::wstring const& wstrFileName) = 0;
	};
}

#endif // WEAPON_XUI_XUI_MEDIAPLAYER_BASE_H