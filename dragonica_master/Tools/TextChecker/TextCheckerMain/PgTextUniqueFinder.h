#include <fstream>
#include "PgTextCheckerDefine.h"

class PgTextUniqueFinder
{
public:
	PgTextUniqueFinder();
	~PgTextUniqueFinder();

public:
	void FindUnique(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kInRhSXmlInfo);
	bool ShowResultToFile(FILE* kOutFS) const;
	bool ShowResultToHTML(FILE* kOutFS) const;
	
protected:
	//파일 이름 저장
	std::wstring		m_kLhsFileName;	
	std::wstring		m_kRhsFileName;

	//FindUnique에서 Lhs와 Rhs에서만 존재하는 ID와 ID의 Text를 저장
	CONT_SIMPLE_TEXTINFO m_kFoundLhsResultVector;
	CONT_SIMPLE_TEXTINFO m_kFoundRhsResultVector;
};