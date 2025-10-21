#ifndef SWITCH_SWITCHSERVER_CONSTANT_H
#define SWITCH_SWITCHSERVER_CONSTANT_H

#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"

typedef enum eVar_Kind
{
	EVar_Kind_None = 0,
	EVar_Kind_TimeCheck = 1,
	EVar_UserManage = 2,
	EVar_Kind_Log = 3,
} EVar_Kind;

typedef enum eVar_TimeCheck
{
	EVar_TimeCheck_SpeedHackGap		= 1,//SpeedHack Gap
	EVar_TimeCheck_AntiHack_Min		= 2,//AntiHack Gap Min
	EVar_TimeCheck_AntiHack_Max		= 3,//AntiHack Gap Max
	EVar_TimeCheck_AntiHack_Wait	= 4,//AntiHack Wait Time 
} EVar_TimeCheck;

typedef enum
{
	EVAR_USER_MANAGE_DISCONNECT_HACKUSER = 1,
	EVAR_USERMANAGE_USEANTIHACKSERVERBIND = 2,
	EVAR_USERMANAGE_USEANTIHACK_DISCONNECT_2ND = 3,
} EVAR_USER_MANAGE;
typedef enum eVar_Log
{
	EVar_LogOutputType	= 1,	//출력 타입(파일에 쓸건가, 화면에 남길건가 등 E_OUPUT_TYPE)
	EVar_LogLevelLimit	= 2,	//로그 레벨 제한
}EVar_Log;

#endif // SWITCH_SWITCHSERVER_CONSTANT_H