// CSIME.h: interface for the CCSIME class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "CSIME.h"


#ifdef _MT_
	#pragma comment(lib, "CSIME_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "CSIME_MTd.lib")
#endif

#ifdef _MTo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CSIME_MToAH.lib")
	#else
		#pragma comment(lib, "CSIME_MTo.lib")
	#endif
#endif

#ifdef _MD_
	#pragma comment(lib, "CSIME_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "CSIME_MDd.lib")
#endif

#ifdef _MDo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "CSIME_MDoAH.lib")
	#else
		#pragma comment(lib, "CSIME_MDo.lib")
	#endif
#endif

#pragma comment(lib,"imm32.lib")
