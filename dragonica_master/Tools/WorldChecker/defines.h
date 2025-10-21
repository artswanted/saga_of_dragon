#pragma once

#include	<winsock2.h>
#include	<windows.h>
#include	<commdlg.h>
#include	<shlobj.h>
#include	<string>
#include	"resource.h"
#include	<stdio.h>

//
#include	"CEL/Common.h"

#include	"nilog.h"
#include    "Definitions.h"

//
#include	"BM/BM.h"
#include	"BM/GUID.h"
#include	"BM/vstring.h"
#include	"BM/filesupport.h"
#include	"BM/PgTask.h"
#include	"BM/PgFileMapping.h"
#include	"BM/ThreadObject.h"

//
#include	"tinyxml/tinyxml.h"

//
#include	"Lohengrin/dbtables.h"
#include	"variant/MonsterDefMgr.h"
#include	"variant/ItemDefMgr.h"
#include	"variant/tabledatamanager.h"
#include	"variant/pgquestinfo.h"

//
#include	"lohengrin/loggroup.h"

//
#include	"DataPack/packInfo.h"
#include	"DataPack/PgDataPackManager.h"

//
/*
#include	"nirefobject.h"
#include	"NiApplication/NiCommand.h"
#include	"PgIXmlObject.h"
#include	"PgXmlLoader.h"
*/
#include	"PgError.h"

#include    "ninode.h"
#include	"NiEntity.h"
#include	"nientitystreaming.h"
#include    "NiScene.h"
#include	"nirefobject.h"
#include	"PgNiFile.h"
#include    "NiFile.h"

#include    "nifactories.h"
#include	"NiEntityStreamingAscii.h"
//#include	"PgNiEntityStreamingAscii.h"
#include	"PgNiEntityStreamingAscii2.h"
#include	"NiPhysXManager.h"

#include	"NiGlobalStringTable.h"
#include	"NiMemObject.h"
#include	"NiTMapBase.h"
#include    "nitstringmap.h"
#include	"nxuserallocator.h"
#include    "nxuseroutputstream.h"
#include	"nistaticdatamanager.h"
#include    "bm/PgTask.h"

#include	"PgStat.h"
#include "NiImageConverter.h"
#include "BM/FileSupport.h"
#include "nitarray.h"
#include "nientityinterface.h"
#include "nientityselectionset.h"
#include "nitpointermap.h"
#include "nientitycommandmanager.h"
#include "nismartpointer.h"

/*
#include	"nidefaulterrorhandler.h"
#include	"nismartpointer.h"
*/

//#include	"nicamera.h"
//#include	"nigeometry.h"
//#include	"nizbufferproperty.h"
//#include	"nientityinterface.h"





#ifdef _DEBUG
#pragma comment	(lib, "tinyxmlstl_mdd.lib")
#pragma	comment	(lib, "DataPack_mdd.lib")
#pragma	comment	(lib, "Lohengrin_mdd.lib")
#pragma	comment	(lib, "variant_mdd.lib")
#pragma comment	(lib, "BM_mdd.lib")
#else
#pragma comment	(lib, "tinyxmlstl_md.lib")
#pragma	comment	(lib, "DataPack_md.lib")
#pragma	comment	(lib, "Lohengrin_md.lib")
#pragma	comment	(lib, "variant_md.lib")
#pragma comment (lib, "BM_md.lib")
#endif


#pragma comment(lib, "NiSystem.lib")
#pragma comment(lib, "NiMain.lib")
#pragma comment(lib, "NiInput.lib")
#pragma comment(lib, "NiAnimation.lib")
#pragma comment(lib, "NiCollision.lib")
#pragma comment(lib, "NiBinaryShaderLibDX9.lib")
#pragma comment(lib, "NSBShaderLibDX9.lib")
#pragma comment(lib, "NSFParserLibDX9.lib")
#pragma comment(lib, "NiD3DXEffectShaderLibDX9.lib")
#pragma comment(lib, "NiParticle.lib")
#pragma comment(lib, "NiMilesAudio.lib")
#pragma comment(lib, "mss32.lib")
#pragma comment(lib, "NiEntity.lib")
#pragma comment(lib, "NiOptimization.lib")



//#pragma comment(lib, "NiMetricsOutput" NI_DLL_SUFFIX ".lib")
#pragma comment(lib, "NiMetricsOutput.lib")
//#pragma comment(lib, "NiDX9Renderer" NI_DLL_SUFFIX ".lib")
#pragma comment(lib, "NiDX9Renderer.lib")
#pragma comment(lib, "NiSystem.lib")
#pragma comment(lib, "NiMain.lib")
#pragma comment(lib, "NiInput.lib")
#pragma comment(lib, "NiAnimation.lib")
#pragma comment(lib, "NiCollision.lib")
#pragma comment(lib, "NiBinaryShaderLibDX9.lib")
#pragma comment(lib, "NiD3DXEffectShaderLibDX9.lib")
#pragma comment(lib, "NiParticle.lib")
#pragma comment(lib, "NiMilesAudio.lib")
#pragma comment(lib, "NiEntity.lib")
#pragma comment(lib, "NiOptimization.lib")
#pragma comment(lib, "NiPhysX.lib")
#pragma comment(lib, "PhysXLoader.lib")


//#define	SAFE_DELETE(p)			{ if(p) { delete (p);	 (p) = NULL; } }
//#define	SAFE_DELETE_ARRAY(p)	{ if(p) { delete [] (p); (p) = NULL; } }



extern	const std::wstring WSTR_QUESTLISTCOLUMN;	
extern	const std::wstring WSTR_QUESTLISTITEM;	
extern	const std::wstring WSTR_TEXTLISTCOLUMN;	
extern	const std::wstring WSTR_TEXTLISTITEM;	

extern	const std::wstring WSTR_QUEST_XML;	
extern	const std::wstring WSTR_QUEST_QTT;	
extern	const std::wstring WSTR_TABLE;	
extern	const std::wstring WSTR_NULLSTRING;	

extern	const std::wstring WSTR_XMLPATH;	
extern  const std::wstring WSTR_XMLEFFECT;

extern	const std::wstring WSTR_FOLDERPATH_XML;
extern	const std::wstring WSTR_FOLDERPATH_GSA;
extern	const std::wstring WSTR_FOLDERPATH_NIF;

extern const std::wstring WSTR_SYSTEM_LISTCOLUMN;
extern const std::wstring WSTR_ERRORLOG_LISTCOLUMN;
extern const std::wstring WSTR_LOG_LISTCOLUMN;
extern const std::wstring WSTR_CONFIG_FILE;
extern const std::wstring WSTR_TABLE_FILE;

extern const std::wstring WSTR_LOG_LISTITEM;
extern const std::wstring WSTR_ERRORLOG_LISTITEM;

extern const TCHAR *ComboBOX_iTem[];
typedef enum eLoadTypeDataCombo
{
	DTC_PATH_XML,
	DTC_EFFECT_XML,
	DTC_MAXTYPE,
}ELOADTYPEDATACOMBO;

enum { CHK_SYSTEM, CHK_LOG, CHK_ERRLOG};

extern			HWND		g_hDlgWnd;
extern			HINSTANCE	g_hInst;
extern	class	CWCCore		g_Core;

#include	"QCCore.h"

//#include	"nilog.h"
//#include    "Definitions.h"
