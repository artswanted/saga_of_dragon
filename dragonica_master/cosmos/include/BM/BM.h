#pragma once
//사용 하려면 
//C/C++ > 추가 포함 디렉터리에 절대경로 셋팅.
//링커 > 일반 > 추가 라이브러리 디렉터리 에 Commom/lib 의 절대경로를 셋팅해줄 것.
//컴파일 옵션은( 다중 스레드 DLL, 다중 스레드 디버그 DLL ) 에서만 동작함. 

//AM 위에는 Loki 가 있음. 그 외의 라이브러리는 링크 걸지 말것.
#include "./Common.h"

#ifdef _MT_
	#pragma comment(lib, "BM_MT.lib")
	#pragma comment(lib, "Loki_MT.lib")
	#pragma comment(lib, "ACE_vc8_Static_MT.lib")
	
	#pragma comment(lib, "zlib_MT.lib")	
	#pragma comment(lib, "minizip_MT.lib")
	#pragma comment(lib, "tinyxmlSTL_MT.lib")
	#pragma comment(lib, "lua_lib_MT.lib")
	#pragma comment(lib, "lua_tinker_MT.lib")
	#pragma comment(lib, "luadebuggerlib_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "BM_MTd.lib")
	#pragma comment(lib, "Loki_MTd.lib")	
	#pragma comment(lib, "ACE_vc8_Static_MTd.lib")

	#pragma comment(lib, "zlib_MTd.lib")	
	#pragma comment(lib, "minizip_MTd.lib")
	#pragma comment(lib, "tinyxmlSTL_MTd.lib")
	#pragma comment(lib, "lua_lib_MTd.lib")
	#pragma comment(lib, "lua_tinker_MTd.lib")
	#pragma comment(lib, "luadebuggerlib_MTd.lib")
#endif

#ifdef _MTo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "BM_MToAH.lib")
	#else
		#pragma comment(lib, "BM_MTo.lib")
	#endif
	
	#pragma comment(lib, "Loki_MTo.lib")
	#pragma comment(lib, "ACE_vc8_Static_MTo.lib")
	
	#pragma comment(lib, "zlib_MTo.lib")	
	#pragma comment(lib, "minizip_MTo.lib")
	#pragma comment(lib, "tinyxmlSTL_MTo.lib")
	#pragma comment(lib, "lua_lib_MTo.lib")
	#pragma comment(lib, "lua_tinker_MTo.lib")
	#pragma comment(lib, "luadebuggerlib_MTo.lib")
#endif

#ifdef _MD_
	#pragma comment(lib, "BM_MD.lib")
	#pragma comment(lib, "Loki_MD.lib")
	#pragma comment(lib, "ACE_vc8_Static_MD.lib")
	
	#pragma comment(lib, "zlib_MD.lib")	
	#pragma comment(lib, "minizip_MD.lib")
	#pragma comment(lib, "tinyxmlSTL_MD.lib")
	#pragma comment(lib, "lua_lib_MD.lib")
	#pragma comment(lib, "lua_tinker_MD.lib")
	#pragma comment(lib, "luadebuggerlib_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "BM_MDd.lib")
	#pragma comment(lib, "Loki_MDd.lib")
	#pragma comment(lib, "ACE_vc8_Static_MDd.lib")

	#pragma comment(lib, "zlib_MDd.lib")	
	#pragma comment(lib, "minizip_MDd.lib")
	#pragma comment(lib, "tinyxmlSTL_MDd.lib")
	#pragma comment(lib, "lua_lib_MDd.lib")
	#pragma comment(lib, "lua_tinker_MDd.lib")
	#pragma comment(lib, "luadebuggerlib_MDd.lib")
#endif

#ifdef _MDo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "BM_MDoAH.lib")
	#else
		#pragma comment(lib, "BM_MDo.lib")
	#endif
	
	#pragma comment(lib, "Loki_MDo.lib")
	#pragma comment(lib, "ACE_vc8_Static_MDo.lib")

	#pragma comment(lib, "zlib_MDo.lib")	
	#pragma comment(lib, "minizip_MDo.lib")
	#pragma comment(lib, "tinyxmlSTL_MDo.lib")
	#pragma comment(lib, "lua_lib_MDo.lib")
	#pragma comment(lib, "lua_tinker_MDo.lib")
	#pragma comment(lib, "luadebuggerlib_MDo.lib")
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Netapi32.lib")
