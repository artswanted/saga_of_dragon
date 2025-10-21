#pragma once

#include "BM/BM.h"
#include "SimpEnc/SimpEnc.h"
#include "BM/filesupport.h"


#ifdef	_DEBUG
	#pragma comment (lib, "zlib_MDd.lib")
	#pragma	comment	(lib, "DataPack_MDd.lib")
	#pragma	comment	(lib, "BM_MDd.lib")
	#pragma	comment	(lib, "CEL_MDd.lib")
	#pragma	comment	(lib, "ACE_vc8_Static_MDd.lib")
	#pragma	comment	(lib, "SimpEnc_MDd.lib")
#else
	#pragma comment (lib, "zlib_MD.lib")
	#pragma	comment	(lib, "DataPack_MD.lib")
	#pragma	comment	(lib, "BM_MD.lib")
	#pragma	comment	(lib, "CEL_MD.lib")
	#pragma	comment	(lib, "ACE_vc8_Static_MD.lib")
	#pragma	comment	(lib, "SimpEnc_MD.lib")
#endif

void CreatePatchID(std::wstring strVersionNumber, std::wstring strOutputFolder);
void LoadPatchID(std::wstring strPatchID);