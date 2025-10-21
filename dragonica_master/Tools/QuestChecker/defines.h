#pragma once

typedef std::list< PgQuestInfoVerifyUtil::SQuestInfoError > CONT_MSG;
typedef std::map< int, CONT_MSG > ContErrorMsg;

extern	std::wstring const WSTR_QUESTLISTCOLUMN;	
extern	std::wstring const WSTR_QUESTLISTITEM;	
extern	std::wstring const WSTR_TEXTLISTCOLUMN;	
extern	std::wstring const WSTR_TEXTLISTITEM;	
extern	std::wstring const WSTR_LOGLISTCOLUMN;	
extern	std::wstring const WSTR_LOGERRLISTCOLUMN;	
extern	std::wstring const WSTR_LOGERRLISTITEM;
extern	std::wstring const WSTR_QUESTEMPTY;
extern	std::wstring const WSTR_QUEST_XML;	
extern	std::wstring const WSTR_QUEST_QTT;
extern	std::wstring const WSTR_QUEST_QTT2;
extern	std::wstring const WSTR_QUEST_QTT_NEW2;
extern	std::wstring const WSTR_QUEST_QTT_NEW3;
extern	std::wstring const WSTR_QUEST_QTT_WANTED;
extern	std::wstring const WSTR_TT;
extern	std::wstring const WSTR_TABLE;	
extern	std::wstring const WSTR_ALL_NPC;
extern	std::wstring const WSTR_NULLSTRING;
extern	std::wstring const WSTR_CONFIG_FILE;

extern PgQuestInfoVerifyUtil::LPOUTPUTERRORFUNC g_lpAddErrorFunc;
//extern PgQuestInfoVerifyUtil::ContErrorMsg g_kErrorMsg;
extern ContErrorMsg g_kErrorMsg;
extern Loki::Mutex g_kError;
extern bool g_bDBCon;

void SetLocation();

enum { CHK_LOG, CHK_NONITEM, CHK_NONMONSTER, CHK_SUCCESS, CHK_QTVIEWER, CHK_EMPTYID };

typedef enum eOutType
{
	OT_SUCCESS,
	OT_FAIL,
	OT_DB_NOT_EXIST,
}EOUTTYPE;

typedef enum eSort
{
	ST_ID		= 0,
	ST_TXTID	= 1,
	ST_NAME		= 2,
	ST_AREAID	= 3,
	ST_AREA		= 4,
	ST_MINLEVEL	= 5,
	ST_MAXLEVEL = 6,
}ESORT;


extern			HWND		g_hDlgWnd, g_hLogWnd;
extern			HINSTANCE	g_hInst;
extern			HANDLE		g_hThread;
extern	class	CQCCore		g_Core;

#include	"QCCore.h"

namespace DevelopeFunction
{
	extern int const iUseDebugDevelop; // NO_DEBUG_DEV
};