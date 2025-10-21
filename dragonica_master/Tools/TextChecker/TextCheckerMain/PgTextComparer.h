#pragma once
#include <fstream>
#include <vector>
#include <string>
#include "PgTextCheckerDefine.h"

class PgTextComparer
{	

	typedef struct SCheckInfo
	{//Check시에 나온 정보들을 임시적으로 저장하는 구조체

		CONT_UINT	kLhsDiffCommandSpotVector;
		SDetailTextInfo	kLhsDetailinfo;

		CONT_UINT	kRhsDiffCommandSpotVector;
		SDetailTextInfo	kRhsDetailinfo;	

		SCheckInfo(SDetailTextInfo const& _kInLhsDetailinfo, CONT_UINT const& kInLhsDiffCommandSpotVector,
			SDetailTextInfo const& _kInRhsDetailinfo, CONT_UINT const& kInRhsDiffCommandSpotVector)
			: kLhsDetailinfo(_kInLhsDetailinfo) 
			, kRhsDetailinfo(_kInRhsDetailinfo)
			, kLhsDiffCommandSpotVector(kInLhsDiffCommandSpotVector)
			, kRhsDiffCommandSpotVector(kInRhsDiffCommandSpotVector)
		{}
	}SCheckInfo;	

	typedef struct SShowCheckInfo
	{	
		std::wstring kIdStr;
		std::wstring kLhsText,kRhsText;	
		std::wstring kLhsCheckedText,kRhsCheckedText;

		SShowCheckInfo(std::wstring kInIdStr, 
			std::wstring kInLhsText, std::wstring kInLhsCheckResultText,
			std::wstring kInRhsText, std::wstring kInRhsCheckResultText)
			: kIdStr(kInIdStr)
			, kLhsText(kInLhsText)
			, kLhsCheckedText(kInLhsCheckResultText)
			, kRhsText(kInRhsText)
			, kRhsCheckedText(kInRhsCheckResultText)	
		{}
	}SShowCheckInfo;

	typedef std::vector<SShowCheckInfo> CONT_TEXTCHECK_RESULT_INFO;
	typedef std::vector<SCheckInfo> CONT_TEMP_CHECKINFO;

public:
	PgTextComparer();
	~PgTextComparer();

public:
	void Check(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kInRhSXmlInfo);
	bool ShowResultToFile(FILE* kOutFS) const;	
	bool ShowResultToHTML(FILE* kOutFS) const;	

protected:
	void FindCheckSpot(SDetailTextInfo const& kInLhsDetailTextInfo, std::wstring& kOutLhsResult,
		               SDetailTextInfo const& kInRhsDetailTextInfo, std::wstring& kOutRhsResult);

	void StoreCheckResult(SDetailTextInfo const* const pkInTextInfo, CONT_UINT const& kInDiffSpotVector, 
						  std::wstring& kOutResult);

	void StoreCheckResult_HTML_Type(SDetailTextInfo const* const pkInTextInfo, CONT_UINT const& kInDiffSpotVector, 
						  std::wstring& kOutResult);

	unsigned int const NumLenthCnt(unsigned int const& uiInNum) const;

protected:
	// 파일 이름 저장
	std::wstring	 m_kLhsFileName;	
	std::wstring	 m_kRhsFileName;	

	CONT_WSTRING	 m_kHTMLColor_Vector;
	
	// 같은 위치에 존재하는 커맨드를 비교하여 담은 결과를 저장함
	CONT_TEXTCHECK_RESULT_INFO	 m_kCheckResultVector;				
};