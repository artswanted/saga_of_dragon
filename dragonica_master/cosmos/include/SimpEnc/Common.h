// CSIME.h: interface for the CCSIME class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "simpenc/simpenc.h"


#ifdef _MT_
	#pragma comment(lib, "simpenc_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "simpenc_MTd.lib")
#endif

#ifdef _MTo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "simpenc_MToAH.lib")
	#else
		#pragma comment(lib, "simpenc_MTo.lib")
	#endif
#endif

#ifdef _MD_
	#pragma comment(lib, "simpenc_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "simpenc_MDd.lib")
#endif

#ifdef _MDo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "simpenc_MDoAH.lib")
	#else
		#pragma comment(lib, "simpenc_MDo.lib")
	#endif
#endif