#pragma once
#include "DataPack/PgDataPackManager.h"

#ifdef _MT_
	#pragma comment(lib, "DataPack_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "DataPack_MTd.lib")
#endif

#ifdef _MTo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "DataPack_MToAH.lib")
	#else
		#pragma comment(lib, "DataPack_MTo.lib")
	#endif
#endif

#ifdef _MD_
	#pragma comment(lib, "DataPack_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "DataPack_MDd.lib")
#endif

#ifdef _MDo_
	#ifdef _ANTI_HACK_
		#pragma comment(lib, "DataPack_MDoAH.lib")
	#else
		#pragma comment(lib, "DataPack_MDo.lib")
	#endif
#endif