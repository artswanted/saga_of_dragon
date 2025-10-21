#include "stdafx.h"

PgTextSameFinder::PgTextSameFinder()
{
}

PgTextSameFinder::~PgTextSameFinder()
{
	m_kLhsFileName.clear();
	m_kLhsFileName.clear();
	m_kFoundResultVector.clear();
}
	
void PgTextSameFinder::FindSame(SXmlInfo const& kInLhSXmlInfo, SXmlInfo const& kInRhSXmlInfo)
{	
	// 두파일의 이름을 저장하고, 결과값을 저장할 컨테이너를 초기화 한다.
	m_kLhsFileName = UNI(kInLhSXmlInfo.kFileName);
	m_kRhsFileName = UNI(kInRhSXmlInfo.kFileName);
	m_kFoundResultVector.clear();

	for(CONT_DRCA_TEXTTBL_INFO::const_iterator Lhs_itor = kInLhSXmlInfo.kTextMap.begin();	Lhs_itor != kInLhSXmlInfo.kTextMap.end(); ++Lhs_itor)
	{//Lhs와 Rhs의 컨테이너에서

		CONT_DRCA_TEXTTBL_INFO::const_iterator Rhs_itor = kInRhSXmlInfo.kTextMap.find((*Lhs_itor).first);

		if( Rhs_itor != kInRhSXmlInfo.kTextMap.end() )
		{//서로 ID가 같고
			if( (*Lhs_itor).second.kText == (*Rhs_itor).second.kText ) 
			{//Text도 같은것을 찾아 저장한다.
				SSimpleTextInfo kSameText((*Lhs_itor).first,(*Lhs_itor).second.kIdStr, (*Lhs_itor).second.kText);
				m_kFoundResultVector.push_back( kSameText );
			}
		}
	}
}

bool PgTextSameFinder::ShowResultToFile(FILE* kOutFS) const
{	
	//스트림이 정상적인지 체크해야함
	//if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T("//    Same     //") + NEWLINE );
		WriteToFile( kOutFS, _T("/////////////////") + NEWLINE );
		WriteToFile( kOutFS, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE + NEWLINE );

		WriteToFile( kOutFS, m_kLhsFileName +NEWLINE  + m_kRhsFileName +NEWLINE + NEWLINE  );

		if(!m_kFoundResultVector.empty())
		{//결과 컨테이너가 비어있지 않다면 파일로 저장한다.

			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundResultVector.begin();itor != m_kFoundResultVector.end(); ++itor)			
			{
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE  );
			}
			WriteToFile( kOutFS, NEWLINE  );
		}
		else
		{//비어있다면 없다고 출력해준다.
			WriteToFile( kOutFS, _T("can't find same ID and TEXT.") + NEWLINE + NEWLINE );
		}
		//정상적으로 종료로 true 리턴
		return true;
	}

	//스트림이 정상적이지 못하면 false를 리턴
	return false;	
}

bool PgTextSameFinder::ShowResultToHTML(FILE* kOutFS) const
{
	//스트림이 정상적인지 체크해야함
	//if( kOutFS.is_open() )
	{
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );		
		WriteToFile( kOutFS, HTML_DecoText( _T("//     Same    //"), HTML_COLOR_RED, 5 ) + NEWLINE );		
		WriteToFile( kOutFS, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE + NEWLINE );
		
	//	WriteToFile( kOutFS, HTML_DecoText( m_kLhsFileName + NEWLINE + m_kRhsFileName + NEWLINE + NEWLINE, HTML_COLOR_BLU, 4) );

		if(!m_kFoundResultVector.empty())
		{//결과 컨테이너가 비어있지 않다면 파일로 저장한다.

			for(CONT_SIMPLE_TEXTINFO::const_iterator itor = m_kFoundResultVector.begin();itor != m_kFoundResultVector.end(); ++itor)			
			{
				WriteToFile( kOutFS, (*itor).kID + COLON_STR + (*itor).kText +NEWLINE  );
			}
			WriteToFile( kOutFS, NEWLINE  );
		}
		else
		{//비어있다면 없다고 출력해준다.
			WriteToFile( kOutFS, _T("can't find same ID and TEXT.") + NEWLINE + NEWLINE );
		}
		//정상적으로 종료로 true 리턴
		return true;
	}

	//스트림이 정상적이지 못하면 false를 리턴
	return false;	
}