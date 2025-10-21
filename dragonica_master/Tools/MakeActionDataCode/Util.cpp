#include "stdafx.h"
#include "Util.h"

void ConvToUPR(std::string &strSrc)
{// 대문자로 변환
	std::transform( strSrc.begin(), strSrc.end(), strSrc.begin(), towupper );
}

bool IsActionType(char const* pcStr) 
{
	if(pcStr
		&& ::strcmp(pcStr, "ACTION") == 0 )
	{
		return true;
	}
	return false;
}


bool ParsePathXML(std::string const kPath, CONT_ACTION_PATH& rkOutCont)
{// ActionID : Path 내용 얻어오는 부분
	if( kPath.empty() )
	{
		return false;
	}
	
	TiXmlDocument kXmlDoc;
	kXmlDoc.LoadFile( kPath.c_str() );
	if( kXmlDoc.Error() )
	{
		return false;
	}

	TiXmlNode *pkRootNode = kXmlDoc.FirstChild();
	while(pkRootNode && pkRootNode->Type() != TiXmlNode::ELEMENT)
	{
		pkRootNode = pkRootNode->NextSibling();
	}

	if(!pkRootNode)
	{// 최상위 노드가 없다면,
		return false;
	}
	TiXmlElement *pkElement = pkRootNode->FirstChildElement();
	while(pkElement)
	{
		char const *pcType = NULL;
		pcType = pkElement->Value();

		if( false == IsActionType( pcType ) )
		{// Action이 아니면 넘어가고
			pkElement = pkElement->NextSiblingElement();
			continue;
		}

		char const *pcID = NULL;
		TiXmlAttribute* pkAttr = pkElement->FirstAttribute();
		//std::cout<< pkElement->Value() << std::endl;

		while(pkAttr)
		{
			char const *pcAttrName = pkAttr->Name();
			char const *pcAttrValue = pkAttr->Value();

			if(::strcmp(pcAttrName, "ID") == 0)
			{// ActionID를 얻어오고
				pcID = pcAttrValue;
			}
			pkAttr = pkAttr->Next();
		}
		// Path를 얻어와
		char const* pcPath = pkElement->GetText();

		if( ( pcID != NULL && strlen(pcID) > 0)			// ActionID와
			&& pcPath != NULL && strlen(pcPath) > 0		// Path가 모두 존재하면
			)
		{// 경로 컨테이너에
			// 대문자로 변환해서
			std::string strPath = pcPath;	ConvToUPR(strPath);
			std::string strID = pcID;		ConvToUPR(strID);
			//std::cout<< strID.c_str() <<" : "<<strPath.c_str()  <<std::endl;

			// 삽입 하고
			rkOutCont.insert( std::make_pair(strID, strPath) );
		}

		// 다음 내용을 얻어온다
		pkElement = pkElement->NextSiblingElement();
	}
	return true;
}

void WriteToFile(FILE* pFile, std::string const& kStr)
{
	if(NULL == pFile) 
	{
		return;
	}
	fwrite(kStr.c_str(), sizeof(std::string::value_type), kStr.size(), pFile);	
}

std::string  UTF16ToMBCS(std::wstring const& kUTF16)
{// utf16 -> mbcs
	if(kUTF16.empty()) { return std::string(""); }

	int const iLen16 = static_cast<int>(kUTF16.length());
	int const iLenMB  = WideCharToMultiByte(CP_ACP, 0, kUTF16.c_str(), iLen16, NULL, 0, NULL, NULL);

	if (iLenMB == 0) {  return std::string(""); }

	std::vector<std::string::value_type> buffer(iLenMB+1);   // '\0'
	int const iBufsize = static_cast<int>(buffer.size());
	WideCharToMultiByte(CP_ACP, 0, kUTF16.c_str(), iLen16, &buffer[0], iBufsize, NULL, NULL);

	return &buffer[0];
}

std::wstring MBCSToUTF16(std::string const& kMBCS)
{// mbcs -> utf16
	if(kMBCS.empty()) { return std::wstring(L"");    }
	// 한글은 ANSI code page나 OEM code page 아무거나 써도 된다.   
	// Code Pages Supported by Windows - OEM Code Pages 참고   
	// http://www.microsoft.com/globaldev/reference/oem.mspx   
	// default로 설정된 코드 페이지를 사용하자. 
	int const iLenMB =  static_cast<int>(kMBCS.length());    
	int const iLen16 = MultiByteToWideChar(CP_ACP, 0, kMBCS.c_str(), iLenMB, 0, 0);

	if (iLen16 == 0) {  return std::wstring(L""); }

	std::vector<std::wstring::value_type> buffer(iLen16+1);
	int const iBufsize = static_cast<int>(buffer.size());
	MultiByteToWideChar(CP_ACP, 0, kMBCS.c_str(), iLenMB, &buffer[0], iBufsize);   

	return &buffer[0];
}  

bool ReplaceStringOneAtaTime(std::string const& kOrigStr, std::string const& kReplaceStr, std::string const& kBeginStr, std::string const& kEndStr, std::string& rkOutStr)
{// 한번에 하나씩만 바꾼다
	rkOutStr = kOrigStr;
	std::string::size_type stBeginPos = rkOutStr.find( kBeginStr ); 
	if( std::string::npos != stBeginPos )
	{
		std::string::size_type stEndPos = rkOutStr.find( kEndStr, stBeginPos );
		if( std::string::npos == stEndPos )
		{// 종료 문자가 있는지 확인하고
			return false;
		}
		if( stEndPos+kEndStr.size() > rkOutStr.size() )
		{// 종료 문자에서 찾은 index에 +size을 해주어야 찾은 문자열 우측을 분별 할수 있고
			return false;	
		}
		stEndPos+= kEndStr.size();

		// 찾은 문자열의 좌측,
		std::string kLeft( rkOutStr, 0, stBeginPos);
		// 우측과
		std::string kRight( rkOutStr, stEndPos, rkOutStr.size() );
		// 교체 문제열을 가지고 최종 문자열을 만든다.
		rkOutStr = kLeft + kReplaceStr + kRight;
		return true;
	}
	return false;
}

void ReplaceString(std::string const& kOrigStr, std::string const& kReplaceStr, std::string const& kBeginStr, std::string const& kEndStr, std::string& rkOutStr)
{
	rkOutStr = kOrigStr;
	//for(std::string::size_type stBeginPos = rkOutStr.find( kBeginStr ); std::string::npos != stBeginPos; stBeginPos = rkOutStr.find( kBeginStr ) )
	//{
	//	std::string::size_type stEndPos = rkOutStr.find( kEndStr, stBeginPos );
	//	if( std::string::npos == stEndPos )
	//	{// 종료 문자가 있는지 확인하고
	//		break;
	//	}
	//	if( stEndPos+kEndStr.size() > rkOutStr.size() )
	//	{// 종료 문자에서 찾은 index에 +size을 해주어야 찾은 문자열 우측을 분별 할수 있고
	//		break;	
	//	}
	//	stEndPos+= kEndStr.size();

	//	// 찾은 문자열의 좌측,
	//	std::string kLeft( rkOutStr, 0, stBeginPos);
	//	// 우측과
	//	std::string kRight( rkOutStr, stEndPos, rkOutStr.size() );
	//	// 교체 문제열을 가지고 최종 문자열을 만든다.
	//	rkOutStr = kLeft + kReplaceStr + kRight;
	//}
	while( ReplaceStringOneAtaTime(rkOutStr, kReplaceStr, kBeginStr, kEndStr, rkOutStr) );
}

void GetBetweenStringOneAtaTime(std::string const& kOrigStr, std::string const& kBeginStr, std::string const& kEndStr, CONT_STR& rkContOut)
{// 해당 문자열들 사이에 있는 문자열들을 모두 얻어온다
	std::string rkOutStr = kOrigStr;
	std::string::size_type stNextPos = 0;
	while( std::string::npos !=stNextPos )
	{
		std::string::size_type stBeginPos = rkOutStr.find( kBeginStr, stNextPos); 
		if( std::string::npos == stBeginPos )
		{
			break;
		}
		stBeginPos += kBeginStr.size();
		if( stBeginPos > rkOutStr.size() )
		{
			break;
		}

		stNextPos = rkOutStr.find( kEndStr, stBeginPos );
		if( std::string::npos == stNextPos )
		{
			break;
		}
		std::string kElem( rkOutStr, stBeginPos, stNextPos-stBeginPos);
		rkContOut.push_back( kElem );
	}
}