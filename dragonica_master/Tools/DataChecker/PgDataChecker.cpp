#include "stdafx.h"
#include "PgDataChecker.h"
#include "CEL/ConsoleCommander.h"
#include "Lohengrin/dbtables.h"
#include "Lohengrin/dbtables2.h"
#include "Lohengrin/dbtables3.h"
#include "Variant/PgControlDefMgr.h"
#include "CheckList.h"

//-----------------------------------------------------------------------------------------------
// Name: PgDataChecker()
// Desc: PgDataChecker's Constructor
//-----------------------------------------------------------------------------------------------
PgDataChecker::PgDataChecker()
{
	
}

//-----------------------------------------------------------------------------------------------
// Name: PgDataChecker()
// Desc: PgDataChecker's Destructor
//-----------------------------------------------------------------------------------------------
PgDataChecker::~PgDataChecker()
{
	
}

bool CALLBACK PgDataChecker::OnTerminate(WORD const& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG(BM::LOG_LV6, _T("[DataChecker] will be shutdown"));
	INFO_LOG(BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));

	g_kConsoleCommander.StopSignal(true);

	INFO_LOG(BM::LOG_LV6, _T("=== Shutdown END ===="));
	return false;
}

void CALLBACK PgDataChecker::OnRegist( const CEL::SRegistResult& rkArg )
{
	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		g_kCoreCenter.SvcStart();
	}
}

HRESULT CALLBACK PgDataChecker::OnDB_EXECUTE(CEL::DB_RESULT &rkResult)
{	
	if( !PgDBCache::OnDBExcute( rkResult ) )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CALLBACK PgDataChecker::OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &rkResult)
{
	return S_OK;
}

void PgDataChecker::ShowMemu()
{
	INFO_LOG(BM::LOG_LV6, _T("==================== DataChecker Menu ===================="));
	INFO_LOG(BM::LOG_LV6, _T("\tF1:\tCreate TB.Bin"));
	INFO_LOG(BM::LOG_LV6, _T("\tF2:\tLoad TB.Bin"));
	INFO_LOG(BM::LOG_LV6, _T("\tF3:\tData verify"));
	INFO_LOG(BM::LOG_LV6, _T("\tF11:\tQuit"));
	INFO_LOG(BM::LOG_LV6, _T("=========================================================="));
}

//-----------------------------------------------------------------------------------------------
// Name: RegistKeyEvent()
// Desc: 키를 눌렀을 때 호출되는 콜백 함수 등록
//-----------------------------------------------------------------------------------------------
bool PgDataChecker::RegistKeyEvent()
{	
	g_kConsoleCommander.Regist( VK_F1, OnF1 );
	g_kConsoleCommander.Regist( VK_F2, OnF2 );
	g_kConsoleCommander.Regist( VK_F3, OnF3 );
	g_kConsoleCommander.Regist( VK_F11, OnTerminate ); // 프로그램 종료
	return true;
}

bool CALLBACK PgDataChecker::OnF1( WORD const& rkInputKey )
{
	return g_kDataChecker.CreateTableBin();
}

bool CALLBACK PgDataChecker::OnF2( WORD const& rkInputKey )
{
	return g_kDataChecker.LoadTableBin();
}

bool CALLBACK PgDataChecker::OnF3( WORD const& rkInputKey )
{
	g_kDataChecker.ParseXmlForClassNo();
	g_kDataChecker.CheckForClassNo();
	g_kDataChecker.ParseXmlForPath();
	g_kDataChecker.CheckForPath();	
	
	g_kDataChecker.ShowMemu();
	g_kDataChecker.OpenLogFile();

	return true;
}

void PgDataChecker::OpenLogFile()
{
	{// DataChecker 폴더
		g_kDataChecker.SetPath();
		_wchdir(L"./LogFiles/DataChecker/");

		struct _wfinddatai64_t c_file;
		long hFile = (long)_wfindfirsti64( L"*.txt", &c_file );
		while( _wfindnexti64( hFile, &c_file ) == 0 )
		{
			// while 문을 빠져나오면 c_file은 가장 마지막에 생성된 파일을 가지고 있지.
		}

		::ShellExecute( NULL, L"open", c_file.name, NULL, NULL, SW_SHOW );
		_findclose(hFile);
	}

	{// CautionLog 폴더
		g_kDataChecker.SetPath();
		_wchdir(L"./LogFiles/CautionLog/");

		struct _wfinddatai64_t c_file;
		long hFile = (long)_wfindfirsti64( L"*.txt", &c_file );
		while( _wfindnexti64( hFile, &c_file ) == 0 )
		{
			// while 문을 빠져나오면 c_file은 가장 마지막에 생성된 파일을 가지고 있지.
		}

		::ShellExecute( NULL, L"open", c_file.name, NULL, NULL, SW_SHOW );
		_findclose(hFile);
	}	
}

//-----------------------------------------------------------------------------------------------
// Name: DataCheckerBegin()
// Desc: 프로그램 초기 설정 및 DB Worker 생성
//-----------------------------------------------------------------------------------------------
bool PgDataChecker::DataCheckerBegin()
{
	//	쓰레드 관련 초기화
	CEL::INIT_CENTER_DESC	kCenterInit;
	kCenterInit.eOT	= BM::OUTPUT_JUST_TRACE;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.bIsUseDBWorker = true;
	kCenterInit.dwProactorThreadCount = 0;
	g_kCoreCenter.Init(kCenterInit);

	// Log Init
	std::wstring kLogFolder;
	TCHAR chLog[MAX_PATH] = {0,};
	_stprintf_s(chLog, L"DataChecker");
	InitLog(kLogFolder, chLog, BM::OUTPUT_ALL, BM::LOG_LV9, std::wstring( L"./LogFiles/") );
	
	// Read DataChecker.ini
	BM::vstring strFileName( L"./DataChecker.ini" );
	extern bool ReadDBConfigINI(BM::vstring const vstrFileName, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut);

	CONT_DB_INIT_DESC kContDBInit;
	if( !ReadDBConfigINI( strFileName, g_kProcessCfg.ServerIdentity(), kContDBInit ) )
	{	
		INFO_LOG( BM::LOG_LV6, _T("DataChecker.ini load fail.") );
		return false;
	}

	CONT_DB_INIT_DESC::iterator dbinit_itor = kContDBInit.begin();
	CEL::INIT_DB_DESC &kDefDBInit = (*dbinit_itor);
	CEL::INIT_DB_DESC &kLocalDBInit = *(++dbinit_itor);

	kDefDBInit.bUseConsoleLog = false;
	kDefDBInit.OnDBExecute = OnDB_EXECUTE;
	kDefDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;	
	kDefDBInit.dwWorkerCount = 3;

	kLocalDBInit.bUseConsoleLog = false;
	kLocalDBInit.OnDBExecute = OnDB_EXECUTE;
	kLocalDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;	
	kLocalDBInit.dwWorkerCount = 3;

	g_kCoreCenter.Regist( CEL::RT_DB_WORKER, &kDefDBInit );		// DR2_Def
	g_kCoreCenter.Regist( CEL::RT_DB_WORKER, &kLocalDBInit );	// Dr2_Local

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 1;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	return true;
}

//-----------------------------------------------------------------------------------------------
// Name: DataCheckerEnd()
// Desc: 종료 처리
//-----------------------------------------------------------------------------------------------
void PgDataChecker::DataCheckerEnd()
{
	g_kLogWorker.VDeactivate();	
	g_kCoreCenter.Close();
}

void PgDataChecker::SetPath()
{
	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	SetCurrentDirectory(strPath.c_str());

	char fullPath[_MAX_PATH];
	if( NULL != _fullpath( fullPath, "../../SFreedom_Dev/XML", _MAX_PATH ) )
	{
		m_kXMLPath = fullPath;
		m_kXMLPath += '\\';
	}
}

bool PgDataChecker::CreateTableBin()
{
	CTableDataManager kTemp;
	g_kTblDataMgr.swap(kTemp); // 이전 데이터 백업
	g_kTblDataMgr.Clear();// 테이블 날리고

	if( !PgDBCache::TableDataQuery() )
	{
		g_kTblDataMgr.Clear();
		g_kTblDataMgr.swap( kTemp ); // 백업한 데이터로 되돌림

		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Complete DB Load = Failed(DB Query)!!") );
		return false;
	}

	if( PgDBCache::DisplayErrorMsg() )
	{
		return false; // 에러 있음
	}

	// Def Building
	int iDefLoad = 
		PgControlDefMgr::EDef_MonsterDefMgr
		|	PgControlDefMgr::EDef_SkillDefMgr
		|	PgControlDefMgr::EDef_ItemDefMgr
		|	PgControlDefMgr::EDef_RareDefMgr
		|	PgControlDefMgr::EDef_ItemEnchantDefMgr
		|	PgControlDefMgr::EDef_ClassDefMgr
		|	PgControlDefMgr::EDef_ItemBagMgr
		|	PgControlDefMgr::EDef_StoreMgr
		|	PgControlDefMgr::EDef_ItemOptionMgr
		|	PgControlDefMgr::EDef_ItemSetDefMgr
		|	PgControlDefMgr::EDef_EffectDefMgr
		|	PgControlDefMgr::EDef_PropertyMgr
		|	PgControlDefMgr::EDef_GroundEffect
		|	PgControlDefMgr::EDef_RecommendationItem
		|	PgControlDefMgr::EDef_Pet
		|	PgControlDefMgr::EDef_MapDefMgr
		|	PgControlDefMgr::EDef_Default
		|	PgControlDefMgr::EDef_OnlyFirst;

	SReloadDef sReloadDef;
	g_kTblDataMgr.GetReloadDef(sReloadDef, iDefLoad);
	g_kControlDefMgr.StoreValueKey(BM::GUID::Create());
	INFO_LOG(BM::LOG_LV7, _T("Set StoreKey LoadDB::")<< g_kControlDefMgr.StoreValueKey());

	if( !g_kControlDefMgr.Update( sReloadDef, iDefLoad ) )
	{
		g_kConsoleCommander.StopSignal(true);
		return false;
	}

	if( !g_kTblDataMgr.Dump( L"./Table/" ) )
	{
		INFO_LOG( BM::LOG_LV6, _T("Create TB.Bin fail.") );
		return false;
	}

	INFO_LOG( BM::LOG_LV6, _T("Create TB.Bin complete.") );
	return true;
}

bool PgDataChecker::LoadTableBin()
{
	if( g_kTblDataMgr.LoadDump( L"./Table/" ) )
	{
		INFO_LOG( BM::LOG_LV6, _T("TB.Bin load complete.") );
		return true;
	}
	
	INFO_LOG( BM::LOG_LV6, _T("TB.Bin load failed.") );
	return false;
}

bool PgDataChecker::ParseXmlForClassNo()
{
	std::string kPath = GetXMLPath() + "ClassNo.xml";

	TiXmlDocument kDoc;
	if( !kDoc.LoadFile( kPath.c_str() ) )
	{
		INFO_LOG( BM::LOG_LV6, _T("ClassNo.xml load fail.") );
		return false;
	}

	const TiXmlElement *pkElement = kDoc.FirstChildElement();
	if( strcmp(pkElement->Value(), "CLASS") == 0 )
	{
		pkElement = pkElement->FirstChildElement();
		while( pkElement )
		{
			char const *pcTagName = pkElement->Value();

			if( strcmp(pcTagName, "ITEM") == 0 )
			{
				const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();

				int iNo = 0;
				std::string kActorID;
				std::string kPilotXmlPath;

				while(pkAttr)
				{
					std::string pcAttrName( pkAttr->Name() );
					std::string pcAttrValue( pkAttr->Value() );

					if( pcAttrName == "NO" )
					{
						iNo = atoi(pcAttrValue.c_str());
					}
					else if( pcAttrName == "ID" )
					{
						kActorID = pcAttrValue;
					}
					else if( pcAttrName == "PILOT_PATH" )
					{
						kPilotXmlPath = pcAttrValue;
					}
					else
					{
						INFO_LOG( BM::LOG_LV6, _T("ClassNo.xml parse error. Because incorrect attr [") << pkAttr->Name() << "]" );
						return false;
					}

					pkAttr = pkAttr->Next();
				}

				if( !iNo || kActorID.empty() )
				{
					INFO_LOG( BM::LOG_LV6, _T("ClassNo or ActorID empty.") );
					return false;
				}
				else
				{
					UPR(kActorID);
					UPR(kPilotXmlPath);

					m_kCheckClassInfo.insert( std::make_pair( iNo, SClassInfo( kActorID, kPilotXmlPath ) ) );
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV6, _T("ClassNo.xml parse error. because incorrect attr [") << pcTagName << "]" );
				return false;
			}

			pkElement = pkElement->NextSiblingElement();
		}

		return true;
	}

	return false;
}

bool PgDataChecker::CheckForClassNo()
{
	const CONT_DEFCLASS* pkDefClass = NULL;
	g_kTblDataMgr.GetContDef( pkDefClass );
	if( !pkDefClass )
	{
		INFO_LOG( BM::LOG_LV6, _T("Get CONT_DEFCLASS failed.") );
		return false;
	}

	if( pkDefClass->empty() )
	{
		INFO_LOG( BM::LOG_LV6, _T("You must first create the TB.Bin or load TB.Bin") );
		return false;
	}

	CONT_DEFCLASS::const_iterator def_iter = pkDefClass->begin();
	while( pkDefClass->end() != def_iter )
	{
		CONT_DEFCLASS::mapped_type kElement = def_iter->second;

		if( m_kCheckClassInfo.end() == m_kCheckClassInfo.find( kElement.iClass ) )
		{
			INFO_LOG( BM::LOG_LV5, _T( "ClassNo [" << kElement.iClass << "] has not exist in ClassNo.xml" ) );			
		}

		++def_iter;
	}

	return true;
}


bool PgDataChecker::ParseXmlForPath()
{
	std::string kPath = GetXMLPath() + "Path.xml";

	TiXmlDocument kDoc;
	if( !kDoc.LoadFile( kPath.c_str() ) )
	{
		INFO_LOG( BM::LOG_LV6, _T("Path.xml load fail.") );
		return false;
	}

	const TiXmlElement *pkElement = kDoc.FirstChildElement();
	if( strcmp(pkElement->Value(), "PATH") == 0 )
	{
		pkElement = pkElement->FirstChildElement();
		while( pkElement )
		{
			char const *pcID = 0;
			
			const TiXmlAttribute* pkAttr = pkElement->FirstAttribute();
			while(pkAttr)
			{
				char const *pcAttrName = pkAttr->Name();
				char const *pcAttrValue = pkAttr->Value();

				if(strcmp(pcAttrName, "ID") == 0)
				{
					pcID = pcAttrValue;
				}				
				else
				{
					INFO_LOG( BM::LOG_LV6, _T("ClassNo.xml parse error. Because incorrect attr [") << pcAttrName << "]" );
				}

				pkAttr = pkAttr->Next();
			}

			char const* pcPath = pkElement->GetText();

			if( pcID != NULL && pcPath != NULL && strlen(pcID) > 0 && strlen(pcPath) > 0 )
			{	
				std::string strPath = pcPath;
				std::string strID = pcID;

				 UPR(strID);
				 UPR(strPath);

				m_kCheckPathInfo.insert( std::make_pair( strID, SPathInfo( strID, strPath ) ) );
			}
			
			pkElement = pkElement->NextSiblingElement();
		}

		return true;
	}

	return false;
}

bool PgDataChecker::CheckForPath()
{	
	CONT_CHECK_CLASSINFO::const_iterator class_iter = m_kCheckClassInfo.begin();
	while( m_kCheckClassInfo.end() != class_iter )
	{
		CONT_CHECK_CLASSINFO::mapped_type kElement = class_iter->second;

		if( m_kCheckPathInfo.end() == m_kCheckPathInfo.find( kElement.m_kActorID ) )
		{
			INFO_LOG( BM::LOG_LV5, _T( "[ClassNo.xml] Path ID [" << kElement.m_kActorID <<"] has not exists in Path.xml" ) );
		}

		++class_iter;
	}

	{
		CONT_CHECK_PATHINFO::const_iterator path_iter = m_kCheckPathInfo.begin();
		while( m_kCheckPathInfo.end() != path_iter )
		{
			CONT_CHECK_PATHINFO::mapped_type kElement = path_iter->second;

			std::string kPath = GetXMLPath() + kElement.m_kPath;
			TiXmlDocument kDoc;
			if( !kDoc.LoadFile( kPath.c_str() ) )
			{
				INFO_LOG( BM::LOG_LV6, _T( "Path.xml[" << kElement.m_kID << "][" << kElement.m_kPath << "] has not exist" ) );
			}

			++path_iter;
		}
	}

	{
		CONT_CHECK_PATHINFO::const_iterator path_iter = m_kCheckPathInfo.begin();
		while( m_kCheckPathInfo.end() != path_iter )
		{
			CONT_CHECK_PATHINFO::mapped_type kElement = path_iter->second;

			std::string kPath = GetXMLPath() + kElement.m_kPath;
			TiXmlDocument kDoc;
			if( kDoc.LoadFile( kPath.c_str() ) )
			{
				TiXmlNode* pkRootNode = kDoc.FirstChild();
				while( pkRootNode && pkRootNode->Type() != TiXmlNode::ELEMENT )
				{
					pkRootNode = pkRootNode->NextSibling();
				}

				if( !pkRootNode )
				{
					INFO_LOG( BM::LOG_LV3, _T("Error!! This file has not top node [" << kElement.m_kPath << "]" ) );
					return false;
				}

				if( strcmp( pkRootNode->Value(), "STATUS_EFFECT") == 0 )
				{
					
				}				
				else if( strcmp( pkRootNode->Value(), "PROJECTILE") == 0 )
				{
					
				}
				else if( strcmp( pkRootNode->Value(), "ACTION") == 0 )
				{
					
				}
				else if( strcmp( pkRootNode->Value(), "ACTOR") == 0 )
				{
					ParseXmlForActor( kElement.m_kPath, pkRootNode );
				}
				else if( strcmp( pkRootNode->Value(), "DROPBOX") == 0 )
				{
					
				}
				else
				{

				}
			}

			++path_iter;
		}
	}

	return true;
}

bool PgDataChecker::ParseXmlForStatusEffect( const std::string& rkXml, TiXmlNode* pkRootNode )
{
	return true;
}

bool PgDataChecker::ParseXmlForProjectile( const std::string& rkXml, TiXmlNode* pkRootNode )
{
	return true;
}

bool PgDataChecker::ParseXmlForAction( const std::string& rkXml, TiXmlNode* pkRootNode )
{
	return true;
}

bool PgDataChecker::ParseXmlForActor( const std::string& rkXml, TiXmlNode* pkRootNode )
{
	int const iType = pkRootNode->Type();
	switch( iType )
	{
	case TiXmlNode::ELEMENT:
		{
			TiXmlElement* pkElement = (TiXmlElement*)pkRootNode;
			pkElement = pkElement->FirstChildElement();
			while( pkElement )
			{
				// SkillActor, Mon, Mon2, Pet
				if( strcmp( "WORLDOBJECT", pkElement->Value() ) == 0 )
				{
					TiXmlElement* pkChild = pkElement->FirstChildElement();
					while( pkChild )
					{
						if( strcmp( "KFMPATH", pkChild->Value() ) == 0 )
						{	
							char const* pcPath = pkChild->GetText();							

							if( !IsExistFile( pcPath ) )
							{
								INFO_LOG( BM::LOG_LV3, _T("[" << rkXml << "] has not resource [" << pcPath << "]") );
							}
						}

						pkChild = pkChild->NextSiblingElement();
					}
				}

				if( strcmp( "KFMPATH", pkElement->Value() ) == 0 )
				{	
					char const* pcPath = pkElement->GetText();					

					if( !IsExistFile( pcPath ) )
					{
						INFO_LOG( BM::LOG_LV3, _T("[" << rkXml << "] has not resource [" << pcPath << "]") );
					}
				}

				pkElement = pkElement->NextSiblingElement();
			}
		}break;
	default:
		{
		}break;
	}

	return true;
}



bool PgDataChecker::ParseXmlForDropBox( const std::string& rkXml, TiXmlNode* pkRootNode )
{
	return true;
}

bool PgDataChecker::IsExistFile( char const* pcPath )
{
	std::string kPath = GetXMLPath() + pcPath;

	if( 0 == ::_access( kPath.c_str(), F_OK ) )
	{	
		return true;
	}
	else
	{	
		return false;
	}
}