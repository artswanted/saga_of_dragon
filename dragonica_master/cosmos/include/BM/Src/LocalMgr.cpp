// LocalizingMgr.cpp: implementation of the CLocal class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <string>
#include "BM/filesupport.h"
#include "BM/vstring.h"
#include "BM/LocalMgr.h"

using namespace LOCAL_MGR;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//INI info
static const TCHAR s_szRoot[]		= _T("LOCAL_INFO");//
static const TCHAR s_szElement_NN[]	= _T("NATION_CODE");
static const TCHAR s_szElement_R[] = _T("SERVICE_REGION");
static const TCHAR s_szElement_L[]	= _T("LANGUAGE");
static const TCHAR s_szElement_ST[]	= _T("SERVICE_TYPE");
static const TCHAR s_szElement_LC_NUMERIC[] = _T("LC_NUMERIC");


CLocal::CLocal()
{
	Clear();
}

CLocal::~CLocal()
{
	Clear();
}

void CLocal::Clear()
{
	NationCode(NC_NOT_SET);
	ServiceType(ST_NOT_SET);
}

bool CLocal::LoadFromINI(std::wstring const& strINIFileName, wchar_t const* szDefaultLocale)
{
	TCHAR szNationName[MAX_PATH] ={ _T('\0'), };
	TCHAR szRegionName[MAX_PATH] ={ _T('\0'), };
	TCHAR szLanguageName[MAX_PATH] ={ _T('\0'), };
	TCHAR szLCNumeric[MAX_PATH] ={ _T('\0'), };

	// [LOCAL_INFO]
	::GetPrivateProfileString(s_szRoot, s_szElement_L, szDefaultLocale, szLanguageName, sizeof(szLanguageName), strINIFileName.c_str() );
	::GetPrivateProfileString(s_szRoot, s_szElement_NN, NULL, szNationName, sizeof(szNationName), strINIFileName.c_str() );
	::GetPrivateProfileString(s_szRoot, s_szElement_R, NULL, szRegionName, sizeof(szRegionName), strINIFileName.c_str() );
	::GetPrivateProfileString(s_szRoot, s_szElement_LC_NUMERIC, NULL, szLCNumeric, sizeof(szLCNumeric), strINIFileName.c_str() );
	DWORD dwST = ::GetPrivateProfileInt(s_szRoot, s_szElement_ST, ST_NOT_SET, strINIFileName.c_str());

	m_strLanguage = MB(szLanguageName);
	m_eNationCode = GetNationCode(szNationName);
	m_eServiceRegion = GetNationCode(szRegionName);
	m_dwServiceTypeFlag = dwST;

	if (setlocale(LC_ALL, m_strLanguage.c_str()) == NULL)
	{
		return false;
	}

	SetLCNumeric(BM::vstring(szLCNumeric));

	return CheckValue();
}

bool CLocal::LoadFromINB(std::wstring const& strINBFileName, wchar_t const* szDefaultLocale)
{
	std::vector< char > vecConfigData;
	BM::DecLoad(strINBFileName, vecConfigData);
	std::string strConfig;
	
	if(vecConfigData.size())
	{
		strConfig = std::string(&vecConfigData.at(0));
	}

	std::wstring const kCfgData(UNI(strConfig));

	// [LOCAL_INFO]
	BM::vstring vLanguage;
	BM::vstring vstrNation;
	BM::vstring vstrRegion;
	BM::vstring vstrST;
	BM::vstring vstrLCNumeric;
	BM::GetFromINB(s_szRoot, s_szElement_L, vLanguage, BM::vstring((szDefaultLocale == NULL) ? _T("") : szDefaultLocale), kCfgData);
	BM::GetFromINB(s_szRoot, s_szElement_NN, vstrNation, BM::vstring(_T("")), kCfgData);
	BM::GetFromINB(s_szRoot, s_szElement_R, vstrRegion, BM::vstring(_T("")), kCfgData);
	BM::GetFromINB(s_szRoot, s_szElement_LC_NUMERIC, vstrLCNumeric, BM::vstring(_T("")), kCfgData);
	DWORD dwST = BM::GetFromINB(s_szRoot, s_szElement_ST, vstrST, BM::vstring(ST_NOT_SET), kCfgData);

	m_strLanguage = MB(vLanguage.operator std::wstring const&());
	m_eNationCode = GetNationCode(vstrNation);
	m_eServiceRegion = GetNationCode(vstrRegion);
	m_dwServiceTypeFlag = dwST;

	if (setlocale(LC_ALL, m_strLanguage.c_str()) == NULL)
	{
		return false;
	}

	SetLCNumeric(vstrLCNumeric);

	return CheckValue();
}

void	CLocal::SetLCNumeric(BM::vstring const &kLCNumeric)
{
	if(kLCNumeric.size() != 1)
	{
		return;
	}

	setlocale(LC_NUMERIC,BM::vstring::ConvToMultiByte(kLCNumeric).c_str());
}
std::string const& CLocal::GetLocale()const
{
	return m_strLanguage;
}

NATION_CODE CLocal::GetNationCode(wchar_t const* szNation) const
{
	if (szNation == NULL)
	{
		return NC_NOT_SET;
	}
	// ServiceRegion Code 를 위로
	if (0 == _tcsicmp(_T("KOREA"), szNation)) { return NC_KOREA; }
	if (0 == _tcsicmp(_T("CHINA"), szNation)) { return NC_CHINA; }
	if (0 == _tcsicmp(_T("TAIWAN"), szNation)) { return NC_TAIWAN; }
	if (0 == _tcsicmp(_T("AMERICA"), szNation)) { return NC_USA; }
	if (0 == _tcsicmp(_T("EUROPE"), szNation)) { return NC_EU; }
	if (0 == _tcsicmp(_T("JAPAN"), szNation)) { return NC_JAPAN; }
	if (0 == _tcsicmp(_T("SINGAPORE"), szNation)) { return NC_SINGAPORE; }
	if (0 == _tcsicmp(_T("RUSSIA"), szNation)) { return NC_RUSSIA; }
	if (0 == _tcsicmp(_T("DEVELOP"), szNation)) { return NC_DEVELOP; }

	// 사용 가능성 높은것을 위로
	if (0 == _tcsicmp(_T("ENGLAND"), szNation)) { return NC_ENGLAND; }
	if (0 == _tcsicmp(_T("CANADA"), szNation)) { return NC_CANADA; }
	if (0 == _tcsicmp(_T("THAILAND"), szNation)) { return NC_THAILAND; }
	if (0 == _tcsicmp(_T("HONGKONG"), szNation)) { return NC_HONGKONG; }
	if (0 == _tcsicmp(_T("FRANCE"), szNation)) { return NC_FRANCE; }
	if (0 == _tcsicmp(_T("GERMANY"), szNation)) { return NC_GERMANY; }
	if (0 == _tcsicmp(_T("MALAYSIA"), szNation)) { return NC_MALAYSIA; }
	if (0 == _tcsicmp(_T("PHILIPPINES"), szNation)) { return NC_PHILIPPINES; }
	if (0 == _tcsicmp(_T("AUSTRALIA"), szNation)) { return NC_AUSTRALIA; }
	if (0 == _tcsicmp(_T("INDONESIA"), szNation)) { return NC_INDONESIA; }
	if (0 == _tcsicmp(_T("VIETNAM"), szNation)) { return NC_VIETNAM; }
	if (0 == _tcsicmp(_T("BRAZIL"), szNation)) { return NC_BRAZIL; }
	if (0 == _tcsicmp(_T("SPAIN"), szNation)) { return NC_SPAIN; }
	if (0 == _tcsicmp(_T("NEW_ZEALAND"), szNation)) { return NC_NEW_ZEALAND; }
	if (0 == _tcsicmp(_T("LAOS"), szNation)) { return NC_LAOS; }
	if (0 == _tcsicmp(_T("CAMBODIA"), szNation)) { return NC_CAMBODIA; }
	if (0 == _tcsicmp(_T("MACAO"), szNation)) { return NC_MACAO; }
	if (0 == _tcsicmp(_T("MEXICO"), szNation)) { return NC_MEXICO; }
	return NC_NOT_SET;
}

char const* CLocal::GetNationCodeStr() const
{
	switch (m_eNationCode)
	{
		case NC_KOREA:		{ return "KOREA"; }
		case NC_CHINA:		{ return "CHINA"; }
		case NC_TAIWAN:		{ return "TAIWAN"; }
		case NC_THAILAND:	{ return "THAILAND"; }
		case NC_HONGKONG:	{ return "HONGKONG"; }
		case NC_USA:		{ return "AMERICA"; }
		case NC_JAPAN:		{ return "JAPAN"; }
		case NC_EU:			{ return "EUROPE"; }
		case NC_SINGAPORE:	{ return "SINGAPORE"; }
		case NC_FRANCE:		{ return "FRANCE"; }
		case NC_GERMANY:	{ return "GERMANY"; }
		case NC_MALAYSIA:	{ return "MALAYSIA"; }
		case NC_PHILIPPINES:{ return "PHILIPPINES"; }
		case NC_AUSTRALIA:	{ return "AUSTRALIA"; }
		case NC_INDONESIA:	{ return "INDONESIA"; }
		case NC_VIETNAM:	{ return "VIETNAM"; }
		case NC_NEW_ZEALAND:{ return "NEW_ZEALAND"; }
		case NC_LAOS:		{ return "LAOS"; }
		case NC_CAMBODIA:	{ return "CAMBODIA"; }
		case NC_MACAO:		{ return "MACAO"; }
		case NC_CANADA:		{ return "CANADA"; }
		case NC_MEXICO:		{ return "MEXICO"; }
		case NC_BRAZIL:		{ return "BRAZIL"; }
		case NC_ENGLAND:	{ return "ENGLAND"; }
		case NC_RUSSIA:		{ return "RUSSIA"; }
		case NC_SPAIN:		{ return "SPAIN"; }
		case NC_DEVELOP:	{ return "DEVELOP"; }
	}
	return "DEFAULT";
}

bool CLocal::CheckValue()
{
	if (m_strLanguage.length() <= 0)
	{
		return false;
	}
	if (m_eNationCode == NC_NOT_SET)
	{
		return false;
	}
	if (m_eServiceRegion == NC_NOT_SET)
	{
		return false;
	}
	return true;
}

bool CLocal::IsAbleServiceType(DWORD const dwFlag) const
{
	return (( ServiceType() & dwFlag )?true:false);
}

bool CLocal::IsAbleServiceName(wchar_t const* szServiceName) const
{
	if( m_eServiceRegion == GetNationCode(szServiceName) )
		return true;
	return false;
}

bool CLocal::IsAbleNationCode(wchar_t const* szServiceName) const
{
	if( m_eNationCode == GetNationCode(szServiceName) )
		return true;
	return false;
}


char const* CLocal::GetSystemNationCode()
{
	LANGID iLang = GetUserDefaultLangID();
	// Warning! GetUserDefaultLangID return a multiply language
	// please order lang code right
	if ((iLang & LANG_RUSSIAN) == LANG_RUSSIAN)
    	return "RUSSIA";
	if ((iLang & LANG_FRENCH) == LANG_FRENCH)
    	return "FRANCE";
    if ((iLang & LANG_ENGLISH) == LANG_ENGLISH)
    	return "EUROPE";
	return "EUROPE"; // by default
}