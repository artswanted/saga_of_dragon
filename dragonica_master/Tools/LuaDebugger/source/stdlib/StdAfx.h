// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3BE29362_0A15_441C_87F1_B28FB0980E92__INCLUDED_)
#define AFX_STDAFX_H__3BE29362_0A15_441C_87F1_B28FB0980E92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

extern "C" 
{
	#include "lua.h" 
	#include "lauxlib.h" 
	#include "lualib.h"
}

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3BE29362_0A15_441C_87F1_B28FB0980E92__INCLUDED_)
