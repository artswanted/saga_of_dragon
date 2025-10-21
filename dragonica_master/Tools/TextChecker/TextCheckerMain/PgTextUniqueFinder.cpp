#include "stdafx.h"

PgTextUniqueFinder::PgTextUniqueFinder()
{
}

PgTextUniqueFinder::~PgTextUniqueFinder()
{
	m_kFoundLhsResultVector.clear();
	m_kFoundRhsResultVector.clear();
}

void PgTextUniqueFinder::FindUnique(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kRhSXmlInfo)
{
	// 두파일의 이름을 저장하고, 결과값을 저장할 컨테이너를 초기화 한다.
	m_kLhsFileName = UNI(kInLhSXmlInfo.kFileName);
	m_kRhsFileName = UNI(kRhSXmlInfo.kFileName);
	m_kFoundLhsResultVector.clear();
	m_kFoundRhsResultVector.clear();


	for(CONT_DRCA_TEXTTBL_INFO::const_iterator Lhs_itor = kInLhSXmlInfo.kTextMap.begin(); Lhs_itor != kInLhSXmlInfo.kTextMap.end(); ++Lhs_itor)
	{
		CONT_DRCA_TEXTTBL_INFO::const_iterator Rhs_itor = kRhSXmlInfo.kTextMap.find((*Lhs_itor).first);		

		if( Rhs_itor == kRhSXmlInfo.kTextMap.end() )
		{//A에만 존재하는 ID 모으기
			SSimpleTextInfo kAUniqueText((*Lhs_itor).first, (*Lhs_itor).second.kIdStr, (*Lhs_itor).second.kText);
			m_kFoundLhsResultVector.push_back( kAUniqueText );
		}
	}

	for(CONT_DRCA_TEXTTBL_INFO::const_iterator Rhs_itor = kRhSXmlInfo.kTextMap.begin(); Rhs_itor != kRhSXmlInfo.kTextMap.end(); ++Rhs_itor)
	{//B에만 존재하는 ID 모으기
		CONT_DRCA_TEXTTBL_INFO::const_iterator Lhs_itor = kInLhSXmlInfo.kTextMap.find((*Rhs_itor).first);

		if( Lhs_itor == kInLhSXmlInfo.kTextMap.end() )
		{
			SSimpleTextInfo kBUniqueText((*Rhs_itor).first,(*Rhs_itor).second.kIdStr,(*Rhs_itor).second.kText);
			m_kFoundRhsResultVector.push_back( kBUniqueText );
		}
	}
}

bool PgTextUniqueFinder::ShowResultToFile(FILE* kOutFS) const
{

//	if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T("//    Unique   //") + NEWLINE );
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE + NEWLINE );
		//값이 존재하면 값 출력, 없으면 없다고 표시
		if(!m_kFoundLhsResultVector.empty())
		{
			WriteToFile( kOutFS, _T("Unique ID ") + m_kLhsFileName + _T(" - - - - - - - - -") + NEWLINE + NEWLINE );
			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundLhsResultVector.begin(); itor != m_kFoundLhsResultVector.end(); ++itor)
			{				
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE );
			}
			WriteToFile( kOutFS, NEWLINE  );
		}	
		else
		{
			WriteToFile( kOutFS, _T("none exist UniqueID in ") + m_kLhsFileName + _T(" - - - - - - - - -") + NEWLINE + NEWLINE );
		}

		if(!m_kFoundRhsResultVector.empty())
		{
			WriteToFile( kOutFS, _T("Unique ID ") + m_kRhsFileName + _T(" - - - - - - - - -" + NEWLINE + NEWLINE) );

			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundRhsResultVector.begin(); itor != m_kFoundRhsResultVector.end(); ++itor)
			{				
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE );
			}
			WriteToFile( kOutFS, NEWLINE  );
		}
		else
		{			
			WriteToFile( kOutFS, _T("none exist UniqueID in ") + m_kRhsFileName + _T(" - - - - - - - - -") + NEWLINE + NEWLINE );
		}
		return true;
	}
	return false;
}

bool PgTextUniqueFinder::ShowResultToHTML(FILE* kOutFS) const
{
	
//	if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( kOutFS, HTML_DecoText( _T("//  Unique ID  //"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );

		//값이 존재하면 값 출력, 없으면 없다고 표시
		WriteToFile( kOutFS, HTML_OPEN_MARK + HTML_FONT + HTML_Color(HTML_COLOR_BLU) + HTML_CLOSE_MARK );
		if(!m_kFoundLhsResultVector.empty())
		{
			//WriteToFile( kOutFS, HTML_DecoText( _T("Unique ID in "), HTML_COLOR_GRE, 4 ) + HTML_DecoText( m_kLhsFileName, HTML_COLOR_BLU, 4 )  + NEWLINE + NEWLINE );
			WriteToFile( kOutFS, HTML_DecoText( m_kLhsFileName, HTML_COLOR_BLU, 4 ) + NEWLINE + NEWLINE );
						
			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundLhsResultVector.begin(); itor != m_kFoundLhsResultVector.end(); ++itor)
			{
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE +NEWLINE);
			}
			WriteToFile( kOutFS, NEWLINE  );
		}	
		else
		{
			WriteToFile( kOutFS, _T("none exist UniqueID in ") + m_kLhsFileName + NEWLINE + NEWLINE );
		}
		WriteToFile( kOutFS, HTML_END_OPEN_MARK + HTML_FONT + HTML_CLOSE_MARK );
		

		WriteToFile( kOutFS, HTML_OPEN_MARK + HTML_FONT + HTML_Color(HTML_COLOR_IND) + HTML_CLOSE_MARK );
		if(!m_kFoundRhsResultVector.empty())
		{
			//WriteToFile( kOutFS, HTML_DecoText( _T("Unique ID in "), HTML_COLOR_GRE, 4 ) + HTML_DecoText( m_kRhsFileName, HTML_COLOR_BLU, 4 ) + NEWLINE + NEWLINE );
			WriteToFile( kOutFS, HTML_DecoText( m_kRhsFileName, HTML_COLOR_IND, 4 ) + NEWLINE + NEWLINE );

			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundRhsResultVector.begin(); itor != m_kFoundRhsResultVector.end(); ++itor)
			{
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE +NEWLINE );
			}
			WriteToFile( kOutFS, NEWLINE  );
		}
		else
		{
			WriteToFile( kOutFS, _T("none exist UniqueID in ") + m_kRhsFileName + NEWLINE + NEWLINE );
		}
		WriteToFile( kOutFS, HTML_END_OPEN_MARK + HTML_FONT + HTML_CLOSE_MARK );
		return true;
	}
	return false;
}