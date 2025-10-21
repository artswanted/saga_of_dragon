//
// HandOver, 강정욱 2008.01.29
//
// 맵툴의 고정형 Config를 저장 하거나 로드 하는 클래스입니다.
// 그런데 언제 부턴가 작동이 안되는 듯;
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgStreamManager.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::IO;

MPgStreamManager::MPgStreamManager(void)
{
}
MPgStreamManager::~MPgStreamManager(void)
{
}
bool MPgStreamManager::Initialize()
{
	if(LoadConfig("config.txt"))
	{
		//
	}
	else
	{
		m_strConfigVersion = "1.0.0.0";
		m_strServerIP = "211.232.145.177,2433";
		m_strServerID = "gamedesign";
		m_strServerDatabase = "DR2_Def";
		m_strServerMonsterTable = "TB_DefMonster";
		m_strServerRegenTable = "TB_DefMapMonsterRegen_01";
		m_strImageSubfolder1 = "";
		m_strImageSubfolder2 = "";
		m_strImageSubfolder3 = "";
		m_strXmlPath = "";
		m_strBinPath = "";
	}
	return true;
}
bool MPgStreamManager::SaveConfig(String *strFileName)
{
	// 실행파일 경로
	char path[260];
	GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);
	if (strlen(path)==0)
	{
		return false;
	}
	strrchr( path, '\\' )[1] = 0;
	// 실행파일 경로
	String *strConfigPath = path;
	strConfigPath = strConfigPath->Concat(strConfigPath, strFileName);

	//if (!File::Exists(strConfigPath))
	//{
	//	File::OpenWrite(strConfigPath);
	//	return false;
	//}

	StreamWriter *sw = new StreamWriter(strConfigPath);
	sw->WriteLine(m_strConfigVersion);
	sw->WriteLine(m_strServerIP);
	sw->WriteLine(m_strServerID);
	sw->WriteLine(m_strServerDatabase);
	sw->WriteLine(m_strServerMonsterTable);
	sw->WriteLine(m_strServerRegenTable);
	sw->WriteLine(m_strImageSubfolder1);
	sw->WriteLine(m_strImageSubfolder2);
	sw->WriteLine(m_strImageSubfolder3);
	sw->WriteLine(m_strXmlPath);
	sw->WriteLine(m_strBinPath);
    sw->Close();

	return true;
}
bool MPgStreamManager::LoadConfig(String *strFileName)
{
	char path[260];
	GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);
	if (strlen(path)==0)
	{
		return false;
	}
	strrchr( path, '\\' )[1] = 0;
	String *strConfigPath = path;
	strConfigPath = strConfigPath->Concat(strConfigPath, strFileName);
	
	if (!File::Exists(strConfigPath))
	{
		SaveConfig(strFileName);
		return false;
	}

	StreamReader *sr = new StreamReader(strConfigPath);
	if(NULL == sr)
	{
		return false;
	}

	String *strConfigVersion;
	strConfigVersion = sr->ReadLine();
	if (strConfigVersion != NULL)
	{
		m_strServerIP = sr->ReadLine();
		m_strServerID = sr->ReadLine();
		m_strServerDatabase = sr->ReadLine();
		m_strServerMonsterTable = sr->ReadLine();
		m_strServerRegenTable = sr->ReadLine();
		m_strImageSubfolder1 = sr->ReadLine();
		m_strImageSubfolder2 = sr->ReadLine();
		m_strImageSubfolder3 = sr->ReadLine();
		m_strXmlPath = sr->ReadLine();
		m_strBinPath = sr->ReadLine();
		if (!Directory::Exists(m_strBinPath))
		{
			m_strBinPath = 0;
		}
	}
    sr->Close();
	SaveConfig(strFileName);
	return true;
}
String* MPgStreamManager::get_ServerIP()
{
	return m_strServerIP;
}
void MPgStreamManager::set_ServerIP(String *strIP)
{
	m_strServerIP = strIP;
}
String* MPgStreamManager::get_ServerID()
{
	return m_strServerID;
}
void MPgStreamManager::set_ServerID(String *strID)
{
	m_strServerID = strID;
}
String* MPgStreamManager::get_ServerDatabase()
{
	return m_strServerDatabase;
}
void MPgStreamManager::set_ServerDatabase(String *strDatabase)
{
	m_strServerDatabase = strDatabase;
}
String* MPgStreamManager::get_ServerMonsterTable()
{
	return m_strServerMonsterTable;
}
void MPgStreamManager::set_ServerMonsterTable(String *strTable)
{
	m_strServerMonsterTable = strTable;
}
String* MPgStreamManager::get_ServerRegenTable()
{
	return m_strServerRegenTable;
}
void MPgStreamManager::set_ServerRegenTable(String *strTable)
{
	m_strServerRegenTable = strTable;
}
String* MPgStreamManager::get_BinPath()
{
	return m_strBinPath;
}
void MPgStreamManager::set_BinPath(String *strBinPath)
{
	m_strBinPath = strBinPath;
}

String* MPgStreamManager::get_ImageSubfolder1()
{
	return m_strImageSubfolder1;
}
void MPgStreamManager::set_ImageSubfolder1(String* strPath)
{
	m_strImageSubfolder1 = strPath;
}

String* MPgStreamManager::get_ImageSubfolder2()
{
	return m_strImageSubfolder2;
}
void MPgStreamManager::set_ImageSubfolder2(String* strPath)
{
	m_strImageSubfolder2 = strPath;
}

String* MPgStreamManager::get_ImageSubfolder3()
{
	return m_strImageSubfolder3;
}
void MPgStreamManager::set_ImageSubfolder3(String* strPath)
{
	m_strImageSubfolder3 = strPath;
}
String* MPgStreamManager::get_XmlPath()
{
	return m_strXmlPath;
}
void MPgStreamManager::set_XmlPath(String* strPath)
{
	m_strXmlPath = strPath;
}