#pragma once
#include <string>
#include <vector>
#include <map>
//#include "BM/STLSupport.h"
//#include "BM/LocalMgr.h"
#include "BM/vstring.h"
#include "HTML_Define.h"


#ifdef _DEBUG
#pragma comment (lib, "../../cosmos/lib/tinyxmlSTL_MDd")
#pragma comment (lib, "../../cosmos/lib/BM_MDd.lib")
#pragma comment (lib, "../../cosmos/lib/ACE_vc8_Static_MDd.lib")
#else
#pragma comment (lib, "../../cosmos/lib/tinyxmlSTL_MD")
#pragma comment (lib, "../../cosmos/lib/BM_MD.lib")
#pragma comment (lib, "../../cosmos/lib/ACE_vc8_Static_MD.lib")
#endif

std::wstring const SPECIAL_COLOR_SUBSTITUTION_COMMAND( UNI("       ") );	// {C=%s/} 커맨드 교체 문자열. 표현이 Regex에 들어가면 에러가 나므로
std::wstring const SPECIAL_COLOR_COMMAND( UNI("{C=%s/}") );

std::wstring const COLON_STR( UNI(": ") );
std::wstring const NEWLINE(_T("\x0D\x0A"));			// \n만 으로는 개행 문자가 이상하게 표현되기 때문에 이 개행문자를 씀

std::string const UTF_16_BOM("\xFF\xFE");			// 파일에 유니코드임을 명기하기 위한값, 반드시 string 이어야 한다.

unsigned int const GENERIC_COLOR_COMMAND_SIZE = 13;	// C=0xAARRGGBB/ 이므로 13.
unsigned int const DIFFSPOT_MAX_LENGTH = 4;			// 한 문장에서 커맨드가 서로다른것의 최대 카운트 자리(1000대 자릿수)
int const DEC = 10;									// 10진수

typedef std::vector<unsigned int> CONT_UINT;
typedef std::vector<std::wstring> CONT_WSTRING;

typedef struct SCommandUnitInfo
{// 커맨드의 타입과, 문장에서의 위치를 저장하기 위한 구조체
	size_t uiIndex;			// Text에 커맨드가 있는 위치	
	std::wstring kUnitStr;	// 커맨드 단위 문장

	SCommandUnitInfo()
		: uiIndex(-1)
	{}

	SCommandUnitInfo(SCommandUnitInfo const& rhs)
		: uiIndex(rhs.uiIndex)
		, kUnitStr(rhs.kUnitStr)
	{}
}SCommandUnitInfo;
typedef std::vector<SCommandUnitInfo> CONT_COMMAND_INFO;

typedef struct SDetailTextInfo
{//xml에서 얻어온 ID,Text, Text문자열에 존재하는 모든 커맨드의 정보

	typedef enum 
	{
		ERR_NONE=0, 
		ERR_BRACE=1,
		ERR_PERCENT=2,
	}EErrorType;

	std::wstring kText;										//xml의 Text 부분
	std::wstring kIdStr;									//ID를 문자열로 저장
	CONT_COMMAND_INFO kCommandInfoVector;	
	CONT_COMMAND_INFO kErrCommandInfoVector;

	unsigned int uiErrFlag;
	unsigned int uiErrorCnt;								//Error부분 cnt
	
	SDetailTextInfo()
		: uiErrorCnt(0)
		, uiErrFlag(ERR_NONE)
	{}
	
	SDetailTextInfo(SDetailTextInfo const& rhs)
		: kText(rhs.kText)
		, kIdStr(rhs.kIdStr)
		, uiErrorCnt(rhs.uiErrorCnt)
		, kCommandInfoVector(rhs.kCommandInfoVector)
		, uiErrFlag(rhs.uiErrFlag)
		, kErrCommandInfoVector(rhs.kErrCommandInfoVector)
	{}

}SDetailTextInfo;

typedef struct SSimpleTextInfo
{//한 xml에만 존재하는 ID,Text저장용 
 //두 xml에 같은 ID, 같은 Text를 가진 내용 저장용 구조체

	unsigned long ID;
	std::wstring kText;
	std::wstring kID;

	SSimpleTextInfo(unsigned long _ID, std::wstring _kID, std::wstring _kText)
		: ID(_ID)
		, kID(_kID)
		, kText(_kText)
	{}
}SSimpleTextInfo;
typedef std::vector<SSimpleTextInfo> CONT_SIMPLE_TEXTINFO;

typedef std::map< unsigned long, SDetailTextInfo>	CONT_DRCA_TEXTTBL_INFO;

typedef struct SXmlInfo
{
	std::string	kFileName;
	CONT_DRCA_TEXTTBL_INFO	kTextMap;
}SXmlInfo;

typedef struct SLocaleInfoAccordingToXmlEncode
{
	std::wstring	kLocale;
	std::wstring	kXMLEncode;
	SLocaleInfoAccordingToXmlEncode(std::wstring const& kInLocale, std::wstring const& kInXMLEncode)
									: kLocale(kInLocale)
									, kXMLEncode(kInXMLEncode)
	{}
}SLocaleInfoAccordingToXmlEncode;

typedef std::vector<SLocaleInfoAccordingToXmlEncode> CONT_SET_LOCALEINFO;

//함수
extern void PrintConsole_SimpleTexinfoVector(CONT_SIMPLE_TEXTINFO const& kInSimpleTextInfoVector);

//extern void FindDiffernce(CONT_DRCA_TEXTTBL_INFO const& kLhsTextMap, CONT_DRCA_TEXTTBL_INFO const& kRhsTextMap, CONT_TEXTDIFF_RESULT_INFO& kResultStorage);

extern void WriteToFile(FILE* pFile, std::wstring const& kStr);
extern void WriteToFile(FILE* pFile, std::string const& kStr);
