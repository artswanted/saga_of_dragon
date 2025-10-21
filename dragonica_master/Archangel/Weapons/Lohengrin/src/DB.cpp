#include "stdafx.h"
#include "BM/vstring.h"
#include "BM/FileSupport.h"
#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "LogGroup.h"
#include "PacketStruct.h"
#include "DB.h"

//! 쏀터만이 DB에 접근 한다.
using namespace BM;

void GetDBConfigSectionHead(SERVER_IDENTITY const& rkSI, std::wstring& rkOutString)
{
	TCHAR chTemp[100];
	_stprintf_s(chTemp, 100, _T("R%dC%d_"), rkSI.nRealm, rkSI.nChannel);
	rkOutString = chTemp;
}

bool ReadDBConfigINI(vstring const vstrFileName, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut)
{
	std::wstring wstrSectionHead;
	GetDBConfigSectionHead(rkSI, wstrSectionHead);
	std::wstring wstrTemp = wstrSectionHead + _T("COMMON");
	static vstring const SECTION_COMMON(wstrTemp);
	static vstring const ELEMENT_MAX_DB(L"MAX_DB");
	wstrTemp = wstrSectionHead + _T("DB_INFO_");
	static vstring const SECTION_SERVER_INFO(wstrTemp);
	static vstring const ELEMENT_TYPE(L"TYPE");
	static vstring const ELEMENT_ADDR(L"ADDR");
	static vstring const ELEMENT_NAME(L"NAME");
	static vstring const ELEMENT_ID(L"ID");
	static vstring const ELEMENT_PW(L"PW");
	static vstring const ELEMENT_WORKER_COUNT(L"WORKER_COUNT");

	int const iCount = ::GetPrivateProfileInt(SECTION_COMMON, ELEMENT_MAX_DB, 0, vstrFileName);
	if( !iCount )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int i = 0;
	while(iCount > i)
	{
		++i;
		TCHAR szAddr[MAX_PATH]={0,},szName[MAX_PATH]={0,}, szID[MAX_PATH]={0,}, szPW[MAX_PATH]={0,};

		int const iType = ::GetPrivateProfileInt( SECTION_SERVER_INFO+i ,			ELEMENT_TYPE,	0, vstrFileName);
		::GetPrivateProfileString( SECTION_SERVER_INFO+i ,			ELEMENT_ADDR,	L"", szAddr, sizeof(szAddr)/sizeof(TCHAR), vstrFileName);
		::GetPrivateProfileString( SECTION_SERVER_INFO+i ,			ELEMENT_NAME,	L"", szName, sizeof(szName)/sizeof(TCHAR), vstrFileName);
		::GetPrivateProfileString( SECTION_SERVER_INFO+i ,			ELEMENT_ID,		L"", szID, sizeof(szID)/sizeof(TCHAR), vstrFileName);
		::GetPrivateProfileString( SECTION_SERVER_INFO+i ,			ELEMENT_PW,		L"", szPW, sizeof(szPW)/sizeof(TCHAR), vstrFileName);

		int const iWorkerCount = ::GetPrivateProfileInt( SECTION_SERVER_INFO+i ,			ELEMENT_WORKER_COUNT,1, vstrFileName);

		CEL::INIT_DB_DESC kDesc;
		kDesc.kDBInfo.SetInfo(iType, BM::vstring(szName), BM::vstring(szAddr), BM::vstring(szID), BM::vstring(szPW));
		kDesc.dwWorkerCount = iWorkerCount;
		
		rkContOut.push_back(kDesc);
	}
	return true;
}

bool ReadDBConfigINB(vstring const vstrFileName, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut)
{
	std::vector<char>	buffer;

	if( !DecLoad( vstrFileName, buffer ) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"can't Load INB file");
	}

	std::string strConfig;

	if(buffer.size())
	{
		buffer.push_back('\0');
		strConfig = std::string(&buffer.at(0) );
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"can't Load INB file");
	}


	std::wstring const m_strDATA(UNI(strConfig));

	std::wstring wstrSectionHead;
	GetDBConfigSectionHead(rkSI, wstrSectionHead);
	std::wstring wstrTemp = wstrSectionHead + _T("COMMON");
	vstring const SECTION_COMMON(wstrTemp);
	vstring const ELEMENT_MAX_DB(L"MAX_DB");
	vstring const ELEMENT_CHANNEL(L"CHANNEL");
	wstrTemp = wstrSectionHead + _T("DB_INFO_");
	vstring const SECTION_SERVER_INFO(wstrTemp);
	vstring const ELEMENT_TYPE(L"TYPE");
	vstring const ELEMENT_ADDR(L"ADDR");
	vstring const ELEMENT_NAME(L"NAME");
	vstring const ELEMENT_ID(L"ID");
	vstring const ELEMENT_PW(L"PW");
	vstring const ELEMENT_WORKER_COUNT(L"WORKER_COUNT");

	vstring vstrCount;
	::GetFromINB(SECTION_COMMON, ELEMENT_MAX_DB, vstrCount, BM::vstring(0), m_strDATA);

	int iCount = vstrCount;
	if( !iCount )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int i = 0;
	while(iCount > i)
	{
		++i;
		vstring strType, strAddr,strName, strID, strPW, strWorkerCount, strConn, strIndex;

		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_TYPE,	strType , BM::vstring(0), m_strDATA);

		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_ADDR,	strAddr, BM::vstring(0), m_strDATA);
		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_NAME,	strName, BM::vstring(0), m_strDATA);
		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_ID,		strID, BM::vstring(0), m_strDATA);
		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_PW,		strPW, BM::vstring(0), m_strDATA);

		::GetFromINB( SECTION_SERVER_INFO+i ,			ELEMENT_WORKER_COUNT, strWorkerCount, BM::vstring(1), vstrFileName);

		CEL::INIT_DB_DESC kDesc;
		kDesc.kDBInfo.SetInfo((int)strType, strName, strAddr, strID, strPW);
		kDesc.dwWorkerCount = (int)(strWorkerCount);
	
		rkContOut.push_back(kDesc);
	}

	return true;
}

// Read Config file (DB_Config.ini or DB_Config.inb)
// [PARAMETER]
//	eConfigType : Config file type
//	rkSI : SERVER_IDENTITY
//	rInit : [OUTPUT] DB Configuration value
// [RETURN]
//	success ?
bool ReadDBConfig(EDBConfig_Type const eConfigType, std::wstring const &rkPatch, SERVER_IDENTITY const &rkSI, CONT_DB_INIT_DESC &rkContOut)
{	
	bool ret = false;
	vstring szfileName;
	switch(rkSI.nServerType)
	{
	case CEL::ST_IMMIGRATION:
		{
			szfileName = rkPatch + L"Immigration_DB_Config";
		}break;
	case CEL::ST_CONTENTS:
		{
			szfileName = rkPatch + L"Contents_DB_Config";
		}break;
	case CEL::ST_LOG:
		{
			szfileName = rkPatch + L"Log_DB_Config";
		}break;
	case CEL::ST_GMSERVER:
		{
			szfileName = rkPatch + L"GM_DB_Config";
		}break;
	default:
		{
			szfileName = rkPatch + L"./DB_Config";			
		}break;;
	}
	

	switch (eConfigType)
	{
	case EConfigType_Inb:
		{
			vstring szINB(_T(".inb"));	szfileName += szINB;
			ret = ReadDBConfigINB(szfileName, rkSI, rkContOut);
		}break;
	case EConfigType_Ini:
	default:
		{
			vstring szINI(_T(".ini"));	szfileName += szINI;
		ret =  ReadDBConfigINI(szfileName, rkSI, rkContOut);
		}break;
	}

	if( !ret ) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return ret;
}


