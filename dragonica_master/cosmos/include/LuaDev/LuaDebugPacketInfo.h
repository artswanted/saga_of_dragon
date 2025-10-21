#pragma once

#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <list>
#include <map>
#include <string>

typedef struct tagLuaDebugPacket
{
	// 'S'erver -> 'C'lient = SC      'C'lient -> 'S'erver = CS
	typedef enum eLuaDebugPacketType
	{
		PacketType_None,					// None.
		PacketType_Quit,					// CS 종료 되었는지 알려준다.
		PacketType_Debug_LineOnly,			// SC 브레이크 포인트 추가시키기
		PacketType_Debug_LineClear,			// SC 브레이크 포인트 모두 삭제
		PacketType_Debug_LineOnlyRefresh,	// SC 디버그 중에 브레이크 포인트 추가/삭제
		PacketType_Debug_LineOnlyRefreshEnd,// SC 디버그 중에 브레이크 포인트 추가/삭제 끝
		PacketType_Debug_LocalData,			// CS 브레이크 포인트 걸렸을때 로컬 변수 정보들.
		PacketType_Debug_EvalData,			// SC,CS 브레이크 걸렸을때 원하는 변수 정보 전달.
		PacketType_Debug_EvalDataClear,		// CS 추가 해놓았던 변수 정보들을 지운다.
		PacketType_Command_All,				// SC 디버거가 해야할 일을 명령으로 전달해준다.
		PacketType_Debug_StackData,			// CS 디버그시 스텍 데이터를 넘겨준다. S에서 받는 즉시 처리.
		PacketType_Debug_Calltip,			// SC,CS 콜팁을 서로 넘겨준다.
		PacketType_Debug_Dofile,			// SC 수정된 파일 리스트를 넘겨줌으로써 Dofile을 한다.
	}ELuaDebugPacketType;

	ELuaDebugPacketType ePacketType;
	char acFilename[256];

	tagLuaDebugPacket() {
		memset(acFilename,0,sizeof(acFilename));
		ePacketType = PacketType_None;
	}

} PgLuaDebugPacket;

// Go, Step 등 데이터
typedef struct tagLuaDebugPacket_CommandInfo :
	public PgLuaDebugPacket
{
	typedef enum eCommandInfoType
	{
		CommandInfoType_None,
		CommandInfoType_Go,
		CommandInfoType_StepOver,
		CommandInfoType_StepInto,
		CommandInfoType_StepOut,
	}ECommandInfoType;

	ECommandInfoType eCommandType;

	tagLuaDebugPacket_CommandInfo() { eCommandType = CommandInfoType_None; }

} PgCommandInfo;

// Eval
typedef struct tagLuaDebugPacket_EvalInfo :
	public PgLuaDebugPacket
{
	bool bIsLastEvalInfo;
	char acVariableName[64];
	char acVariableType[64];
	char acVariableValue[64];
	tagLuaDebugPacket_EvalInfo()
	{ 
		bIsLastEvalInfo = false;
		memset(acVariableName,0,sizeof(acVariableName));
		memset(acVariableType,0,sizeof(acVariableType));
		memset(acVariableValue,0,sizeof(acVariableValue));
	}

} PgDebugEvalInfo;

// 디버그할 파일, 라인, 지역 변수 등 데이터
typedef struct tagLuaDebugPacket_DebugInfo :
	public PgLuaDebugPacket
{
	// Line
	unsigned char iBreakLineCount;
	int aiBreakLines[256];

	// Variable, Function, etc..
	unsigned char iLocalVariableCount;
	char acLocalVariableName[256][64];
	char acLocalVariableType[256][64];
	char acLocalVariableValue[256][64];

	tagLuaDebugPacket_DebugInfo()
	{
		iBreakLineCount = 0;
		memset(aiBreakLines, 0, sizeof(aiBreakLines));
		iLocalVariableCount = 0;
		memset(acLocalVariableName, 0, sizeof(acLocalVariableName));
		memset(acLocalVariableType, 0, sizeof(acLocalVariableType));
		memset(acLocalVariableValue, 0, sizeof(acLocalVariableValue));
	}

} PgDebugInfo;

typedef struct tagLuaDebugPacket_StackInfo :
	public PgLuaDebugPacket
{
	int iLevel;
	char acName[128][128];
	char acNamewhat[128][128];
	char acWhat[128][128];
	char acShort_src[128][128];
	char acSource[128][128];
	int aiCurrentline[128];

	tagLuaDebugPacket_StackInfo()
	{
		iLevel = 0;
		memset(acName, 0, sizeof(acName));
		memset(acNamewhat, 0, sizeof(acNamewhat));
		memset(acWhat, 0, sizeof(acWhat));
		memset(acShort_src, 0, sizeof(acShort_src));
		memset(acSource, 0, sizeof(acSource));
		memset(aiCurrentline, 0, sizeof(aiCurrentline));
	}
} PgDebugStackInfo;

typedef struct tagLuaDebugPacket_Calltip :
	public PgLuaDebugPacket
{
	bool bIsFound;
	bool bIsLocal;
	int iLevel;
	char acWord[256];
	char acName[64];
	char acType[64];
	char acValue[64];

	tagLuaDebugPacket_Calltip()
	{
		bIsFound = false;
		bIsLocal = false;
		iLevel = 0;
		memset(acWord, 0, sizeof(acWord));
		memset(acName, 0, sizeof(acName));
		memset(acType, 0, sizeof(acType));
		memset(acValue, 0, sizeof(acValue));
	}
} PgDebugCalltipInfo;

typedef struct tagLuaDebugPacket_Dofile :
	public PgLuaDebugPacket
{
	char acFilename[256];

	tagLuaDebugPacket_Dofile()
	{
		memset(acFilename, 0, sizeof(acFilename));
	}
} PgDebugDofileInfo;

	
typedef std::map< std::string, PgCommandInfo > LuaCmdPacket; // 폴더-파일 이름 // 명령 정보
typedef std::map< std::string, PgDebugInfo> LuaDbgPacket; // 폴더-파일 이름 // 디버그 정보
typedef std::vector<PgDebugEvalInfo> LuaEvalPacket;
typedef std::list<PgDebugDofileInfo> LuaDofileList;
