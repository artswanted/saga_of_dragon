#include "stdafx.h"

void PrintConsole_SimpleTexinfoVector(CONT_SIMPLE_TEXTINFO const& kInSimpleTextInfoVector)
{
	for(CONT_SIMPLE_TEXTINFO::const_iterator itor = kInSimpleTextInfoVector.begin(); itor != kInSimpleTextInfoVector.end(); ++itor)
	{
		std::wstring const& kId	 = (*itor).kID;
		std::wstring const& kText = (*itor).kText;

		std::cout<<kId.c_str()<<COLON_STR.c_str()<<kText.c_str()<<std::endl;
	}
	std::cout<<std::endl;
}

//void FindDiffernce(CONT_DRCA_TEXTTBL_INFO const& kLhsTextMap, CONT_DRCA_TEXTTBL_INFO const& kRhsTextMap, CONT_TEXTDIFF_RESULT_INFO& kResultStorage)
//{
//	for(CONT_DRCA_TEXTTBL_INFO::const_iterator Lhs_itor = kLhsTextMap.begin(); Lhs_itor != kLhsTextMap.end(); ++Lhs_itor)
//	{
//		unsigned long const& ulLhsIndex = (*Lhs_itor).first;		
//		CONT_DRCA_TEXTTBL_INFO::const_iterator Rhs_itor = kRhsTextMap.find(ulLhsIndex);		
//		
//		if(Rhs_itor != kRhsTextMap.end())
//		{//서로 ID가 같고
//			std::wstring const& kLhsText = (*Lhs_itor).second.kText;
//			std::wstring const& kRhsText =(*Rhs_itor).second.kText;			
//			if(kLhsText != kRhsText) 
//			{//Text는 다른것				
//				std::wstring const& kLhsId = (*Lhs_itor).second.kIdStr;
//
//				SDifference kSDiff(ulLhsIndex, kLhsId, kLhsText, kRhsText);
//				kResultStorage.push_back( kSDiff );
//			}
//		}			
//	}
//}

void WriteToFile(FILE* pFile, std::wstring const& kStr)
{	
	if(NULL == pFile) 
	{
		return;
	}
	fwrite(kStr.c_str(), sizeof(std::wstring::value_type), kStr.size(), pFile);	
}

void WriteToFile(FILE* pFile, std::string const& kStr)
{
	if(NULL == pFile) 
	{
		return;
	}
	fwrite(kStr.c_str(), sizeof(std::string::value_type), kStr.size(), pFile);	
}
