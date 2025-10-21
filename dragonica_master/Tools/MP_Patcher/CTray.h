#pragma once
#include "defines.h"

#define TRAYICONID	1//				ID number for the Notify Icon
#define SWM_TRAYMSG	WM_APP//		the message ID sent to our window

namespace CTray
{

void Minimize();
void Close();
void WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

}
