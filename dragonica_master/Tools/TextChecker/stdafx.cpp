// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TextChecker.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

#pragma comment (lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment (lib, "../../cosmos/lib/tinyxmlSTL_MDd")
#pragma comment (lib, "../../cosmos/lib/BM_MDd.lib")
#pragma comment (lib, "../../cosmos/lib/ACE_vc8_Static_MDd.lib")
#pragma comment (lib, "DataPack_MDd.lib")
#else
#pragma comment (lib, "../../cosmos/lib/tinyxmlSTL_MD")
#pragma comment (lib, "../../cosmos/lib/BM_MD.lib")
#pragma comment (lib, "../../cosmos/lib/ACE_vc8_Static_MD.lib")
#pragma comment (lib, "DataPack_MD.lib")
#endif


