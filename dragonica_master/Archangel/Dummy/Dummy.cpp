#include "stdafx.h"
#include "KeyEvent.h"
#include "BM/LocalMgr.h"
#include "Network.h"
#include "Lohengrin/VariableContainer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	g_kCoreCenter;//인스턴스 생성.
	g_kLogWorker.StartSvc(1);

	g_kProcessCfg.Locked_SetConfigDirectory();
	if( !g_kLocal.LoadFromINI() )
	{
		std::cout << "Load From \"local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}

	std::wstring kLogFolder;
	InitLog(kLogFolder, _T(""), BM::OUTPUT_ALL, BM::LOG_LV9, _T("./LogFiles"));

	g_kNetwork.ParseArg(argc, argv);
	const TEST_INFO* pkInfo = g_kNetwork.GetTestInfo();
	if (_tcslen(pkInfo->chID) == 0)
	{
		// ini 못 읽었으면 기본 INI 읽기
		const std::wstring kIniFile = _T("./Dummy.ini");
		// g_kDummyMgr.IniFile(kIniFile);
		g_kNetwork.ReadConfigFile(kIniFile.c_str());
	}
	// g_kDummyMgr.Build();
	g_kNetwork.Init();


	if( !RegistKeyEvent() ){ return 0; }

	std::cout<< "시작" << std::endl;
	g_kConsoleCommander.MainLoof();

	// g_ClientMng.WriteStatistic();
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
//! 	::CoUninitialize();
	return 0;
}

