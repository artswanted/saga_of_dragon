#pragma once

#include "./Common.h"

#ifdef _MT_
	#pragma comment(lib, "ACE_vc8_Static_MT.lib")
	#pragma comment(lib, "CEL_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "ACE_vc8_Static_MTd.lib")
	#pragma comment(lib, "CEL_MTd.lib")
#endif

#ifdef _MTo_
	#pragma comment(lib, "ACE_vc8_Static_MTo.lib")
	
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CEL_MToAH.lib")
	#else
		#pragma comment(lib, "CEL_MTo.lib")
	#endif
#endif

#ifdef _MD_
	#pragma comment(lib, "ACE_vc8_Static_MD.lib")
	#pragma comment(lib, "CEL_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "ACE_vc8_Static_MDd.lib")
	#pragma comment(lib, "CEL_MDd.lib")
#endif

#ifdef _MDo_
	#pragma comment(lib, "ACE_vc8_Static_MDo.lib")
	
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CEL_MDoAH.lib")
	#else
		#pragma comment(lib, "CEL_MDo.lib")
	#endif
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comsupp.lib")

//! 세션정보 관리
//! 스레드 세이핑
//! 패킷 길이 변동에 있어서 줄줄이 붙게 만들기.
//! 파일 전송 되기


