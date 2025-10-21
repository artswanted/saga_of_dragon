#include "defines.h"
#include "OSApi.h"
#include "CTray.h"

int WinMessageBox( HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    CTray::Close();
    return ::MessageBoxW(hWnd, lpText, lpCaption, uType);
}
