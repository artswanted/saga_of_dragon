// DataChecker.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "PgDataChecker.h"
#include "CEL/ConsoleCommander.h"


int _tmain(int argc, _TCHAR* argv[])
{
	g_kDataChecker.SetPath();
	g_kDataChecker.RegistKeyEvent();

	if( !g_kDataChecker.DataCheckerBegin() )
	{
		g_kDataChecker.DataCheckerEnd();
		return 0;
	}

	g_kDataChecker.ShowMemu();
	
	g_kConsoleCommander.MainLoof();
	g_kDataChecker.DataCheckerEnd();

	return 0;
}

