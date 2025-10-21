#include "stdafx.h"

class PgTextSameFinder
{
public:
	PgTextSameFinder();
	~PgTextSameFinder();

public:	
	void FindSame(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kInRhSXmlInfo);
	bool ShowResultToFile(FILE* kOutFS) const;
	bool ShowResultToHTML(FILE* kOutFS) const;	

protected:
	//파일 이름 저장
	std::wstring		m_kLhsFileName;
	std::wstring		m_kRhsFileName;
	
	//FindSame에서 찾은 결과를 저장
	CONT_SIMPLE_TEXTINFO m_kFoundResultVector;
};