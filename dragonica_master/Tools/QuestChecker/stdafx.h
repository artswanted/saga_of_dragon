#pragma once

#include	<winsock2.h>
#include	<windows.h>
#include	<commdlg.h>
#include	<shlobj.h>
#include	<string>
#include	"resource.h"

//
#include	"BM/BM.h"
#include	"BM/GUID.h"
#include	"BM/vstring.h"
#include	"BM/filesupport.h"
#include	"BM/PgFileMapping.h"
#include	"BM/ThreadObject.h"

//
#include	"CEL/Common.h"
#include	"BM/PgTask.h"

//
#include	"tinyxml/tinyxml.h"
#include	"lohengrin/ErrorCode.h"

//
//#include	"variant/dbtables.h"
#include	"variant/MonsterDefMgr.h"
#include	"variant/ItemDefMgr.h"
#include	"variant/tabledatamanager.h"
#include	"variant/pgquestinfo.h"
#include	"Variant/PgDBCache.h"

//
#include	"lohengrin/loggroup.h"
#include	"lohengrin/VariableContainer.h"

//
#include	"DataPack/packInfo.h"
#include	"DataPack/PgDataPackManager.h"

#include	<atlstr.h>

// GetFileVersion.. 등을 사용하기 위한 링크
#pragma comment( lib, "Version.lib" )


#ifdef _DEBUG
#pragma comment	(lib, "tinyxmlstl_mdd.lib")
#pragma	comment	(lib, "DataPack_mdd.lib")
#pragma	comment	(lib, "Lohengrin_mdd.lib")
#pragma	comment	(lib, "variant_mdd.lib")
#pragma comment	(lib, "BM_mdd.lib")
#pragma comment (lib, "Collins_mdd.lib")
#else
#pragma comment	(lib, "tinyxmlstl_md.lib")
#pragma	comment	(lib, "DataPack_md.lib")
#pragma	comment	(lib, "Lohengrin_md.lib")
#pragma	comment	(lib, "variant_md.lib")
#pragma comment (lib, "BM_md.lib")
#pragma comment (lib, "Collins_md.lib")
#endif

//#define	SAFE_DELETE(p)			{ if(p) { delete (p);	 (p) = NULL; } }
//#define	SAFE_DELETE_ARRAY(p)	{ if(p) { delete [] (p); (p) = NULL; } }

#include "ThreadUtil.h"
#include "defines.h"
