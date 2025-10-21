#pragma once
#include "stdafx.h"

	// 1. A.xml 파일, B.xml 파일을 입력 받음
	// 2. A,B파일 Parsing하여 TextMap에 저장하는데 xml의 ID값은 first(unsigned int)로,
	//    xml의 Text값은 분석하여(%를 올바로 썻는지, %타입이 무엇인지) second에 저장.

	// 3. 같은것, 다른것, 유일한것을 
	//    각 벡터 컨테이너에 저장(m_kSameVector,							//같은것(Compare기능)
	//  		    			  m_kDiffVector,							//다른것
	//						      m_kLhsUniqueVector, m_kRhsUniqueVector)	//유일한것
	// 4.Check기능
	// 	 - A,B Map의 같은 index(first)중 서로 다른 Text들을 체크하여
	//	   %문자 정보 vector를 서로 비교하여 다른부분을 표시함

class PgTextChecker
{
public:
	PgTextChecker();
	~PgTextChecker();

public:
	bool LoadXML(std::string const& kLhsXmlFileName, std::string const& kRhsXmlFileName); 	//Load 후 정보 수집 및 정리
	bool LoadXML(std::string const& kXmlFileName);

	bool ShowAllResultToFile(std::string const& kFileName) const;
	bool ShowErrorToFile(std::string const& kFileName) const;
	
	bool ShowAllResultToHTML(std::string const& kFileName) const;
	bool ShowErrorToHTML(std::string const& kFileName) const;

	void Destroy();

protected:
	// 파싱 하는 부분 
	bool ParseAndStore(SXmlInfo& kXmlinfo);
	bool ParseXml(TiXmlNode const* pkNode, CONT_DRCA_TEXTTBL_INFO&  kTextMap);
	
	// xml encoding에 따라 locale 설정
	bool SetLocaleAccordingToThis(std::wstring const& kInXmlEncodeType);

	// ParseXml에 종속.
	// Text를 분석하여, 해당 정보를 Storage에 저장하며, uiErrorCnt에 총 에러 갯수를 저장함.
	void ExamineCommand(std::wstring const& kText, SDetailTextInfo& kStorageTextInfo);
	
	// ExamineCommand에 종속
	bool CheckBraceUnit(std::wstring const& kBraceUnit) const;

	// UnitCheck에 종속
	bool CheckSplitedBraceUnit(std::wstring const& kSplitedBraceUnit) const;
			
	// Error 출력
	void ErrorToFile(FILE* pFile, CONT_DRCA_TEXTTBL_INFO const& kTextMap)	const; 	
	void ErrorToHTML(FILE* pFile, CONT_DRCA_TEXTTBL_INFO const& kTextMap)	const; 	

protected:
	
	SXmlInfo			m_kLhsXmlInfo;
	SXmlInfo			m_kRhsXmlInfo;
	
	PgTextComparer			m_kComparer;
	PgTextSameFinder		m_kSameFinder;
	PgTextUniqueFinder		m_kUniqueFinder;

	CONT_SET_LOCALEINFO	m_kLocaleInfoVector;
};