#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgStreamManager
	{
	public:
		MPgStreamManager(void);
		virtual ~MPgStreamManager(void);
		bool Initialize();

	public:
		String* m_strConfigVersion;
		String* m_strServerIP;
		String* m_strServerID;
		String* m_strServerDatabase;
		String* m_strServerMonsterTable;
		String* m_strServerRegenTable;
		String* m_strBinPath;
		String* m_strImageSubfolder1;//!/ 맵텍스쳐 위치 저장
		String* m_strImageSubfolder2;
		String* m_strImageSubfolder3;
		String* m_strXmlPath;


	public:
		bool SaveConfig(String *strFileName);
		bool LoadConfig(String *strFileName);

	public:
		__property String* get_ServerIP();
		__property void set_ServerIP(String *strIP);
		__property String* get_ServerID();
		__property void set_ServerID(String *strID);
		__property String* get_ServerDatabase();
		__property void set_ServerDatabase(String *strDatabase);
		__property String* get_ServerMonsterTable();
		__property void set_ServerMonsterTable(String *strTable);
		__property String* get_ServerRegenTable();
		__property void set_ServerRegenTable(String *strTable);
		__property String* get_BinPath();
		__property void set_BinPath(String *strXMLPath);
		__property String* get_ImageSubfolder1();
		__property void set_ImageSubfolder1(String* strPath);
		__property String* get_ImageSubfolder2();
		__property void set_ImageSubfolder2(String* strPath);
		__property String* get_ImageSubfolder3();
		__property void set_ImageSubfolder3(String* strPath);
		__property String* MPgStreamManager::get_XmlPath();
		__property void MPgStreamManager::set_XmlPath(String* strPath);		
	};
}}}}
