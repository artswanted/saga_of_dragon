#ifndef __LIBAPI_H__
#define __LIBAPI_H__

BOOL InitLibAPI(HWND hWnd);

// output string to debugger output window
void Trace(LPCSTR szMsg);

// attach std io console
void AttachConsole();

#endif