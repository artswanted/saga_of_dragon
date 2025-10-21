#include "stdafx.h"
std::wstring kErrTextID;

PgTextChecker::PgTextChecker()
{
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("KOR")),	std::wstring(_T("EUC-KR"))));	// 한국어
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("CHT")),	std::wstring(_T("EUC-TW"))));	// 대만어
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("CHS")),	std::wstring(_T("EUC-CN"))));	// 중국어
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("ENG")),	std::wstring(_T("EUC-EN"))));	// 영어
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("ENG")),	std::wstring(_T("ISO-8859-1"))));	// 유럽(영/독/불)
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("JPN")),	std::wstring(_T("EUC-JP"))));	// 일본어
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("Thai_Thailand.874")),	std::wstring(_T("WINDOWS-874"))));	// 태국어	
	m_kLocaleInfoVector.push_back(SLocaleInfoAccordingToXmlEncode(std::wstring(_T("RUS")),	std::wstring(_T("WINDOWS-1251"))));	
}

PgTextChecker::~PgTextChecker()
{
	Destroy();
	m_kLocaleInfoVector.clear();
}

bool PgTextChecker::LoadXML(std::string const& kLhsXmlFileName, std::string const& kRhsXmlFileName)
{//두개의 xml파일을 입력 받아 파싱 한다.

	Destroy();

	m_kLhsXmlInfo.kFileName = kLhsXmlFileName;
	m_kRhsXmlInfo.kFileName = kRhsXmlFileName;

	if( !kLhsXmlFileName.empty() )
	{
		if(!ParseAndStore(m_kLhsXmlInfo))
		{
			return false;
		}
	}
	
	if( !kRhsXmlFileName.empty() )	
	{
		if(!ParseAndStore(m_kRhsXmlInfo))
		{
			return false;
		}
	}
	m_kComparer.Check(m_kLhsXmlInfo,m_kRhsXmlInfo);
	m_kSameFinder.FindSame(m_kLhsXmlInfo,m_kRhsXmlInfo);
	m_kUniqueFinder.FindUnique(m_kLhsXmlInfo,m_kRhsXmlInfo);
	return true;
}

bool PgTextChecker::LoadXML(std::string const& kXmlFileName)
{
	Destroy();
	m_kLhsXmlInfo.kFileName = kXmlFileName;
	if( !kXmlFileName.empty() )
	{
		if(!ParseAndStore(m_kLhsXmlInfo))
		{
			return false;
		}
	}
	return true;	
}

bool PgTextChecker::ParseAndStore(SXmlInfo& kXmlinfo)
{//하나의 xml노드를 받아 kTextMap에 저장한다.
//	try
	{
		TiXmlDocument kXmlDoc;

		if( !kXmlDoc.LoadFile(kXmlinfo.kFileName.c_str() ) )
		{
			std::string const kErrorMsg = kXmlinfo.kFileName + " file Load failed.";
			MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
			return false;
		}
		TiXmlNode const* pkNode = &kXmlDoc;
		
		if( !ParseXml(pkNode, kXmlinfo.kTextMap) ) 
		{
			std::string const kErrorMsg = kXmlinfo.kFileName + " file Load failed.";
			MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
			return false;
		}
	}
	::_wsetlocale(LC_ALL, L"");
	//catch(std::string const& kErrorMsg)
	//{
	//	//std::cout<<"<Error> :";
	//	//std::cout<<kErrorMsg<<std::endl;
	//	MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
	//	Destroy();
	//	return false;
	//}
	return true;
}

bool PgTextChecker::ParseXml(TiXmlNode const* pkNode, CONT_DRCA_TEXTTBL_INFO& kTextMap)
{
//	try
	{
		while(pkNode)
		{
			int const iType = pkNode->Type();
			switch(iType)
			{		
			case TiXmlNode::DECLARATION:
				{				
					TiXmlDeclaration  const* pDecl = dynamic_cast<TiXmlDeclaration const*>(pkNode);
					
					if( !pDecl )
					{
						std::string const kErrorMsg("can't earn Declaration");
						MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
					}
					if( !SetLocaleAccordingToThis(UNI(pDecl->Encoding())) )
					{//지원하지 않는 encoding 타입
						std::string const kErrorMsg(std::string("not offer this xml encoding: ") + pDecl->Encoding());
						MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
						return false;
					}
		
				}break;

			case TiXmlNode::DOCUMENT:
				{
					TiXmlNode const* pChildNode = pkNode->FirstChild();
					if(pChildNode != 0)
					{
						if( !ParseXml(pChildNode,kTextMap) ) 
						{
							std::string const kErrorMsg("can't earn Node");
							MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
							return false;
						}
					}
				}break;

			case TiXmlNode::ELEMENT:
				{					
					TiXmlNode* pkTemp = const_cast<TiXmlNode*>(pkNode);
					TiXmlElement* pElement = static_cast<TiXmlElement*>(pkTemp);
					if( !pElement )
					{
						std::string const kErrorMsg("can't earn Node");
						MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
						return false;
					}

					char const* pcTagName = pElement->Value();

					if( 0 == ::strcmp(pcTagName, "TEXT_TABLE"))
					{//TextTable 일때
						if(pkNode->FirstChild() != 0)
						{
							if( !ParseXml(pkNode->FirstChild(),kTextMap) ) 
							{ 
								std::string const kErrorMsg("can't earn Node");
								MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
								return false;
							}
						}
					}
					else if( 0 == ::strcmp(pcTagName, "QUEST_TEXT_TABLE"))
					{//QuestTextTable 일때
						if(pkNode->FirstChild() != 0)
						{
							if( !ParseXml(pkNode->FirstChild(),kTextMap) ) 
							{ 
								std::string const kErrorMsg("can't earn Node");
								MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
								return false;
							}
						}
					}
					else if( 0 == ::strcmp(pcTagName, "MAP_TEXT_TABLE"))
					{//MAP_TEXT_TABLE 일때
						if(pkNode->FirstChild() != 0)
						{
							if( !ParseXml(pkNode->FirstChild(),kTextMap) ) 
							{ 
								std::string const kErrorMsg("can't earn Node");
								MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
								return false;
							}
						}
					}					

					else if( 0 == ::strcmp(pcTagName, "TEXT"))
					{
						const TiXmlAttribute *pAttr = pElement->FirstAttribute();
						unsigned long ulTextID=0;
						char const* strText=0;
						SDetailTextInfo kInfo;
						while(pAttr)
						{
							char const* pcAttrName = pAttr->Name();
							char const* pcAttrValue = pAttr->Value();
							
							if(0 == ::strcmp(pcAttrName, "ID"))
							{
								ulTextID = static_cast<unsigned long>(atol(pcAttrValue));
								kInfo.kIdStr = UNI(pcAttrValue);
							}
							else if(0 == ::strcmp(pcAttrName, "Text"))
							{
									kInfo.kText = UNI(pcAttrValue);							
							}
							else
							{
								std::string const kErrorMsg= std::string(pcAttrName) +"invalid attribute";
								MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
							}
							pAttr = pAttr->Next();
						}
						
						//커맨드단위를 찾아 정보를 구정하고, Error부분을 체크함						
						ExamineCommand(kInfo.kText, kInfo);
						auto ret = kTextMap.insert( std::make_pair(ulTextID, kInfo) );
						
						if(!ret.second)
						{//동일한 ID가 들어왔다면 
							//겹쳤음을 알린다.
							std::string const kErrorMsg =std::string("ID:\'")+ MB(kInfo.kIdStr) + std::string("\' Same ID has been detected.\n");
							MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
						}
						kErrTextID = kInfo.kIdStr;
					}
					else 
					{//다른 태그가 포함되어 있을 경우 로드 하지 않음
						std::string const kErrorMsg= std::string(pcTagName) + "invalid element.\n";
						MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);
					}
				}break;
			
			case TiXmlNode::COMMENT:
			case TiXmlNode::UNKNOWN:
			case TiXmlNode::TEXT:
			default:
				break;
			}		
			
			TiXmlNode const* pkNextNode = pkNode->NextSibling();
			pkNode = pkNextNode;	
		}
	}
//	catch(std::string const& kErrorMsg)
//	{
////		std::cout<<"<Error> :";
////		std::cout<<kErrorMsg<<std::endl;
////		MessageBox(UNI(kErrorMsg));
//		MessageBox(NULL,UNI(kErrorMsg),L"ERROR",MB_ICONERROR);		
//		Destroy();
//		return false;
//	}
	return true;

}
bool PgTextChecker::SetLocaleAccordingToThis(std::wstring const& kInXmlEncodeType)
{
	std::wstring kTempEncode = kInXmlEncodeType;
	UPR(kTempEncode);
	std::wstring const kUppercaseEncodeType = kTempEncode;

	for(CONT_SET_LOCALEINFO::const_iterator itor = m_kLocaleInfoVector.begin(); itor != m_kLocaleInfoVector.end(); ++itor)
	{
		if( kUppercaseEncodeType ==  (*itor).kXMLEncode )
		{
			std::wstring const& kLocale = (*itor).kLocale;
			::_wsetlocale(LC_ALL, kLocale.c_str());
			return true;
		}
	}
	return false;	
}

//-----------------------------------------------------------------------------------------------------------------

void PgTextChecker::ExamineCommand(std::wstring const& kText, SDetailTextInfo& kStorageTextInfo)
{
	// kText에서 모든 커맨드의 error 유무를 체크하고 준비한다. 
		
	// {/}커맨드을 제외한 모든 커맨드를 찾을수 있는 표현식. 표현식은 무조건 1개로 해야함. 
	// 여러개로 한다면 겹치는 부분  %%%class%%name%에서 %%는 실제 1개인데 , 2개이상 찾아버리는 현상이 생김
	//static std::wstring const skExp(UNI("(%(((class|name|level|guildname|tempguildname|(param[1-5]?)|ITEM_NAME)%)|(([0-9]*([.][0-9])?f)|([I]?[0-9]*d)|([0-9]*s)|([0-9]*u))|(%)))|([0-9]{1,}%?%)"));
	static std::wstring const skExp(UNI("(%(((class|name|level|guildname|tempguildname|(param[1-5]?)|ITEM_NAME)%)|(([0-9]*([.][0-9])?f)|((I64)|([0-9]*)d)|([0-9]*s)|([0-9]*u))|(%)))|([0-9]{1,}%?%)"));	
	
	// 표현식 늘어놓고 보기
	//	(
	//		%
	//		(
	//			(
	//				(
	//					class|name|level|guildname|tempguildname|(param[1-5]?)|ITEM_NAME
	//				)%
	//			)
	//			|
	//			(
	//				([0-9]*([.][0-9])?f)|((I64)|([0-9]*)d)|([0-9]*s)|([0-9]*u)
	//			)
	//			|
	//			(
	//				[0-9]{1,}%|%%
	//			)
	//		)
	//	)
	//	|
	//	(
	//		[0-9]{1,}%?%
	//	)
	
	// 원본 문장을 복사 해둠	
	std::wstring kCopyText = kText;		

	{// 개행 문자가 있으면 보기 불편하므로 다른 문자로 교체함
		std::wstring const UNWELL_NEW_LINE_CH( _T("\x0A") );
		std::wstring const CHANGE_CH( _T(" ") );

		std::wstring::size_type pos;
		do
		{
			pos = kCopyText.find( UNWELL_NEW_LINE_CH );
			if( std::wstring::npos != pos )
			{
				kCopyText.replace(pos,CHANGE_CH.size(),CHANGE_CH );
			}

		}while( std::wstring::npos != pos );
	}
	kStorageTextInfo.kText = kCopyText;

	CONT_COMMAND_INFO& kStorageVetor = kStorageTextInfo.kCommandInfoVector;

	CONT_COMMAND_INFO kMissCommandInfoVector;	
	std::wstring::size_type specialCommandIndex = kCopyText.find(SPECIAL_COLOR_COMMAND);
	
	while(std::wstring::npos != specialCommandIndex )
	{// regex로 검색 불가능하고, regex이후 검색에서도 찾을수 없는 커맨드를 미리 찾아 교체해둠

		// {C=%s/}커맨드가 있는지 확인하여, 교체문장으로 교체하고(regex에서 에러나므로)		
		kCopyText.replace(specialCommandIndex,SPECIAL_COLOR_COMMAND.size(),SPECIAL_COLOR_SUBSTITUTION_COMMAND);
		
		// 찾은 {C=%s/}커맨드를 사전검사 커맨드 컨테이너에 저장한다.
		SCommandUnitInfo info;
		info.kUnitStr = SPECIAL_COLOR_COMMAND;
		info.uiIndex = specialCommandIndex;
		kMissCommandInfoVector.push_back(info);
		
		// {C=%s/}가 더 있는지 검사
		specialCommandIndex = kCopyText.find(SPECIAL_COLOR_COMMAND);
	}

	// regex로 찾은 커맨드를 저장하는 vector를 만든다.
	CONT_COMMAND_INFO kFoundCommandInfoVector;

	// regex에 표현을 설정
	boost::wregex kRegExp(skExp);
	boost::wsregex_iterator kRegexStartitor(kCopyText.begin(), kCopyText.end(), kRegExp);
	boost::wsregex_iterator kRegexEnditor;

	for (;kRegexStartitor != kRegexEnditor; ++kRegexStartitor)
	{// regex에 설정한 표현으로 커맨드를 검사
		unsigned int const uiBeginindex = static_cast<unsigned int>(kRegexStartitor->position());
		unsigned int const uiLength	  = static_cast<unsigned int>(kRegexStartitor->length());
		
		// 찾은 커맨드에 해당하는 문장을 잘라내어 
		std::wstring const kSplitedStr(kCopyText,uiBeginindex, uiLength);
	
		// 커맨드를 저장한다.
		SCommandUnitInfo info;
		info.kUnitStr = kSplitedStr;
		info.uiIndex = uiBeginindex;
		kFoundCommandInfoVector.push_back( info );
	}
	
	for(CONT_COMMAND_INFO::const_iterator itor = kFoundCommandInfoVector.begin(); itor != kFoundCommandInfoVector.end(); ++itor)
	{// 찾은 커맨드들을 모두 원본 문장에서 제거한다.( 공백으로 치환한다 index가 망가지지 않게)

		boost::wregex kRegExp((*itor).kUnitStr );
		boost::match_results<wchar_t const*> kMatch;
		if( boost::regex_search(kCopyText.c_str(),kMatch,kRegExp) )
		{
			unsigned int const uiBeginindex= static_cast<unsigned int>(kMatch.position());
			unsigned int const uiLength	 = static_cast<unsigned int>(kMatch.length());

			//std::wstring const kPreStr(kCopyText,0, uiBeginindex);
			//std::wstring const kSufStr(kCopyText,uiBeginindex+uiLength,kCopyText.size() );
			//kCopyText = kPreStr + kSufStr;
			std::wstring kEmptyStr;
			for(unsigned int i =0; i < uiLength; ++i)
			{
				kEmptyStr.push_back( _T(' ') );
			}
			kCopyText.replace(uiBeginindex, kEmptyStr.size(), kEmptyStr);
		}
	}

	// regex 검색이 끝난후 검색불능 커맨드를 kBraceUnitVector에 넣는다
	// 순차적으로 훑으므로 index가 낮은순으로 저장 된다
	CONT_WSTRING kBraceUnitVector;

	bool bFoundBraceCh=false;	// {} 문자 발견 여부
	unsigned int uiBegin=0;		// 문장서 커맨드 시작 index
	unsigned int uiIndexCnt=0;	// 문장 index 카운트
	for(std::wstring::const_iterator itor = kCopyText.begin(); itor != kCopyText.end(); ++itor)	
	{
		switch ((*itor))
		{
		case _T('{'):
			{
				if(!bFoundBraceCh)
				{
					bFoundBraceCh =true;
					uiBegin = uiIndexCnt;						 
				}
				else
				{// { { 이런 문장을 만났다면 무시함
					bFoundBraceCh =false;
				}

			}break;

		case _T('}'):
			{
				if(bFoundBraceCh)
				{// }을 만났다면 
					bFoundBraceCh = false;

					// {부터 }까지 문장을 {}문장 체크 함수에 넣어주고
					std::wstring const kBraceUnit(kCopyText,uiBegin,uiIndexCnt-uiBegin+1);
					if(CheckBraceUnit(kBraceUnit))
					{// 올바른 문장이라면 저장한다.
						kBraceUnitVector.push_back(kBraceUnit);
					}
				}
			}break;
		}
		++uiIndexCnt;
	}

	// regex로 찾을수 없었던 regex검색 불능 커맨드들을 저장한 kBraceUnitVector에서 
	// 원본문장.find() 하여 각자의 index를 찾아준다.
	// 단, kBraceUnitVector에 들어가있는 regex검색불능커맨드들은 낮은index순으로 저장되어 있어야한다.
	
	// 원본 문장을 복사 해둔다.
	std::wstring kTempStr = kText;
	// 찾은 문장을 삭제하면서 index를 누적해야 하기 때문에 저장소가 필요
	int iAccindex =0;
	for(CONT_WSTRING::const_iterator itor=kBraceUnitVector.begin(); itor != kBraceUnitVector.end(); ++itor)
	{
		int const iBeginindex = static_cast<int const>(kTempStr.find((*itor)));

		if( std::wstring::npos != iBeginindex)
		{// 찾는 문장이 존재하면			
			// 찾은 문장을 제외하고 재구성한다.
			std::wstring const prestr(kTempStr,0, iBeginindex);
			std::wstring const sufstr(kTempStr,iBeginindex+(*itor).size(), kTempStr.size());
			kTempStr = prestr + sufstr;

			// 원본 문장에서의 index를 추측한다.
			int const iRealindex = iBeginindex + iAccindex;
			iAccindex = iAccindex + static_cast<int const>((*itor).size());

			// regex로 찾을수 없었던 문장을 추가한다.
			SCommandUnitInfo kInfo;
			kInfo.kUnitStr = (*itor);
			kInfo.uiIndex = iRealindex;
			kFoundCommandInfoVector.push_back(kInfo);
		}
	}

	// 찾은 커맨드의 위치(info.uiIndex)가 낮은 순으로 정렬한다.	
	for(CONT_COMMAND_INFO::const_iterator itor=kMissCommandInfoVector.begin(); itor != kMissCommandInfoVector.end(); ++itor)
	{// {C=%s/} 커맨드도 결과값에 추가하여 아래에서 정렬되게 한다	
		kFoundCommandInfoVector.push_back((*itor));
	}
	
	// 저장할 컨테이너를 비우고, 저장공간의 크기를 맞춘다.
	kStorageVetor.clear();
	kStorageVetor.resize(kFoundCommandInfoVector.size());
		
	for(unsigned int i=0; i < kFoundCommandInfoVector.size(); ++i)
	{// 소팅
		unsigned int uiIndex=0;
		for(unsigned int j =0; j < kFoundCommandInfoVector.size(); ++j )
		{
			if(kFoundCommandInfoVector[i].uiIndex > kFoundCommandInfoVector[j].uiIndex)
			{
				++uiIndex;
			}
		}
		kStorageVetor[uiIndex] = kFoundCommandInfoVector[i];
	}

	for(CONT_WSTRING::const_iterator itor = kBraceUnitVector.begin(); itor != kBraceUnitVector.end(); ++itor)
	{//검색한 문장에서 찾은커맨드(올바른커맨드)들을 제거하고 ‘여과된문장’을 만든다.	
		std::wstring::size_type const BeginIndex = kCopyText.find((*itor));

		if(std::wstring::npos != BeginIndex)
		{//검색해서 존재한다면
			//여과된 문장을 만들어 나감.

			std::wstring kEmptyStr;
			
			for(unsigned int i =0; i < (*itor).size(); ++i)
			{
				kEmptyStr.push_back( _T(' ') );
			}
			kCopyText.replace(BeginIndex, kEmptyStr.size(), kEmptyStr);
			//std::wstring kPreStr(kCopyText,0, BeginIndex);
			//std::wstring kSufStr(kCopyText,BeginIndex+(*itor).size(), kCopyText.size());
			//kCopyText = kPreStr + kSufStr;
		}
	}
	
	{
		SCommandUnitInfo kErrCommandUnitInfo;
		bool bProcessing = false;
		kStorageTextInfo.uiErrorCnt =0;
		unsigned int uiIndex = 0;
		for(std::wstring::const_iterator itor=kCopyText.begin(); itor != kCopyText.end(); ++itor )
		{//완전히 ‘여과된 문장’에서 Error가 있는지 없는지만 판별한다
			//‘{‘, ‘}’, ‘%’ 중 하나만 있으면 Error로 함)
			wchar_t const cCh = *itor;
			switch (cCh)
			{
				//to do : 여기서 명령문자가 삭제된것이 아닌 ' '으로 치환된 곳에서
				//아래 문자를 만난곳에서 부터 시작해, 다시 ' '을 만나기 직전까지를 하나의 단위로 보고 저장한후
				//결과 파일 저장시 해당 값에 대해 html로 붉은색으로 출력해 준다.
			case _T('{'):
			case _T('}'):
				{
					kStorageTextInfo.uiErrFlag |= SDetailTextInfo::ERR_BRACE;
					++kStorageTextInfo.uiErrorCnt;
					if( false == bProcessing ) 
					{
						bProcessing = true;
						kErrCommandUnitInfo.uiIndex = uiIndex;
					}
					kErrCommandUnitInfo.kUnitStr.push_back( cCh );
					
				}break;
			case _T('%'):
				{
					kStorageTextInfo.uiErrFlag |= SDetailTextInfo::ERR_PERCENT;
					++kStorageTextInfo.uiErrorCnt;
					if( false == bProcessing ) 
					{
						bProcessing = true;
						kErrCommandUnitInfo.uiIndex = uiIndex;
					}
					kErrCommandUnitInfo.kUnitStr.push_back( cCh );
				}break;
			case _T(' '):
				{
					if( true == bProcessing )
					{
						bProcessing = false; 
						kStorageTextInfo.kErrCommandInfoVector.push_back( kErrCommandUnitInfo );
						kErrCommandUnitInfo.uiIndex = 0;
						kErrCommandUnitInfo.kUnitStr.clear();
					}
				}break;
			default:
				if( true == bProcessing )
				{
					kErrCommandUnitInfo.kUnitStr.push_back( cCh );
				}
				break;
			}
			++uiIndex;
		}
		if( true == bProcessing )
		{
			bProcessing = false; 
			kStorageTextInfo.kErrCommandInfoVector.push_back( kErrCommandUnitInfo );
			kErrCommandUnitInfo.uiIndex = 0;
			kErrCommandUnitInfo.kUnitStr.clear();
		}
	}
}

void PgTextChecker::ErrorToFile(FILE* pFile, CONT_DRCA_TEXTTBL_INFO const& kTextMap) const
{	
	for(CONT_DRCA_TEXTTBL_INFO::const_iterator itor = kTextMap.begin(); itor != kTextMap.end(); ++itor)
	{
		std::wstring const& kText = (*itor).second.kText;
		std::wstring const& kIdStr = (*itor).second.kIdStr;
		
		if( 0 < (*itor).second.uiErrorCnt)
		{//Error가 있는 문장만 출력
			//ID와 원본 문장 출력
			WriteToFile( pFile, kIdStr + COLON_STR + kText + NEWLINE  );
			
			//for(unsigned int i=0; i < kIdStr.size() + COLON_STR.size(); ++i ) 
			//{////ID를 출력한만큼 ' '를 출력하여 자리를 맞춤 				
			//	WriteToFile( pFile, _T(" ") );
			//}
			if( 0 < (*itor).second.uiErrorCnt )
			{
				WriteToFile( pFile, _T("please check this : ") );
				if( SDetailTextInfo::ERR_BRACE & (*itor).second.uiErrFlag )
				{
					WriteToFile( pFile, _T("{ } unit, ") );
				}
				if( SDetailTextInfo::ERR_PERCENT & (*itor).second.uiErrFlag )
				{
					WriteToFile( pFile, _T("% unit") );
				}
				WriteToFile( pFile, NEWLINE + NEWLINE );
			}
		}
	}
}

void PgTextChecker::Destroy()
{
	m_kLhsXmlInfo.kFileName.clear();
	m_kLhsXmlInfo.kTextMap.clear();

	m_kRhsXmlInfo.kFileName.clear();
	m_kRhsXmlInfo.kTextMap.clear();	
}

bool PgTextChecker::CheckBraceUnit(std::wstring const& kBraceUnit) const
{	/*
		들어오는 문장은 {에서 부터 }이 발견되는데 까지다.
		
		에러 체크 
			kUnitStr[kUnitStr.size()-2]에 '/'가 있는가?

			형식은 다음과 같다 {C=0xFFFFFF/O=BIU/T=Font_Text/}
			kUnitStr[1]에서 부터 시작해, '/'를 만나면 하나의 단위로 만들어 
			다음 부분들에게 넘겨준다.
			
				C= 부분 체크
					0x하고 0-F까지 

				O= 부분 체크
					B,I,U을 제외한 다른문자가 있는가?

				T= 부분체크
					폰트이기 때문에 문자만 가지고는 어떻게 체크할수가 없다.

			셋중 어떠한 검사에서도 true를 받지 못한다면 
			이문장은 error이다.
	*/
	bool bFoundCh = false;	
	unsigned int iBegin=0;
	for(unsigned int i=1; i < kBraceUnit.size()-1; ++i)
	{
		switch(kBraceUnit[i])
		{		
		case L'T':	
			{// 폰트 명칭이기 때문에 COT든, 뭐든 다 들어올수 있다.
				if(!bFoundCh) 
				{
					bFoundCh = true;
					iBegin = i;
				}
			}break;

		// 0x0~F 까지 가능하므로 C가 또 들어올수 있다. 때문에 bFoundCh가 이미 true라고 해도 에러가 아닐수 있다.
		case L'C':
		case L'O':
			{
				//T 검사중이 아닐때만 갱신
				if(!bFoundCh)
				{
					bFoundCh = true;
					iBegin = i;
				}
			}break;

		case L'/':
			{
				if(bFoundCh)
				{// 맺음 문자를 만났으므로 체크할 문장을 만듬
					std::wstring kSplitedBraceUnit(kBraceUnit,iBegin, i-iBegin+1);					
					if(!CheckSplitedBraceUnit(kSplitedBraceUnit))
					{// Error이면
						return false;
					}
					bFoundCh = false;
				}
			}break;
		}
	}

	if(bFoundCh)
	{// 맺음 문자가 끝나지 않았다면 Error
		return false;
	}	
	return true;
}

bool PgTextChecker::CheckSplitedBraceUnit(std::wstring const& kSplitedBraceUnit) const
{	
	if( 3 >= kSplitedBraceUnit.size())
	{// 적당한 크기가 아니라면 더이상 진행하지 않는다
		return false;
	}
	
	if(L'/' != kSplitedBraceUnit[kSplitedBraceUnit.size()-1])
	{// 마지막 문자가 '/'아니라면 error
		return false;
	}

	if(L'=' != kSplitedBraceUnit[1])
	{// 두번째 문자가 '=' 아니라면 error
		return false;
	}

	// {C=0xFFFFFF/O=BIU/T=Font_Text/}
	switch(kSplitedBraceUnit[0])
	{
	case L'C':
		{
			if(GENERIC_COLOR_COMMAND_SIZE != kSplitedBraceUnit.size())
			{// F는 길이가 고정임 std::wstring(C=0xAARRGGBB/)의 size는 13이므로
				return false; 
			}
			else
			{
				for(unsigned int i=4; i < kSplitedBraceUnit.size()-1; ++i)
				{// 'C=0x' 다음부터 '/'전까지 검사
					if(L'a' > kSplitedBraceUnit[i] || L'f' < kSplitedBraceUnit[i])
					{// 'a'-'f' 범위를 벗어난다면 error ('a':97, 'f':102)
						if(L'0' > kSplitedBraceUnit[i] || L'9' < kSplitedBraceUnit[i])
						{// 숫자 범위를 벗어나고 ('0':48, '9':57)
							if(L'A' > kSplitedBraceUnit[i] || L'F' < kSplitedBraceUnit[i])
							{// A-F 범위를 벗어난다면 Error ('A':65, 'F':70)
								return false;
							}
						}
					}
				}
			}
		}break;
	case L'O':
		{				
			for(unsigned int i=2; i < kSplitedBraceUnit.size()-1; ++i)
			{// 'O=' 다음부터 '/'전까지 검사
				switch(kSplitedBraceUnit[i])
				{
				case L'B':
				case L'I':
				case L'U':
					{
						continue;
					}break;
				default :
					{
						return false;
					}break;
				}
			}
		}
		break;
	case L'T':
		{
			if( 3 >= kSplitedBraceUnit.size() )  
			{// 폰트 이름에 대해선 별다른 체크를 할수 없으므로, 'T=/'만 아니면 ok.
				return false;
			}			
		}break;
	}
	// Error 체크에 걸리지 않았다면 error가 아님
	return true;
}

bool PgTextChecker::ShowAllResultToFile(std::string const& kFileName) const
{		
	FILE * pFile;
	fopen_s(&pFile, kFileName.c_str(), "wb");

	if(pFile)
	{
		WriteToFile( pFile, UTF_16_BOM );
		
		//error
		WriteToFile( pFile, _T("/////////////////") + NEWLINE );
		WriteToFile( pFile, _T("// Error Text  //") + NEWLINE );
		WriteToFile( pFile, _T("/////////////////") + NEWLINE );
		WriteToFile( pFile, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE );
		WriteToFile( pFile, UNI(m_kLhsXmlInfo.kFileName) + NEWLINE + NEWLINE);
		ErrorToFile( pFile, m_kLhsXmlInfo.kTextMap);

		WriteToFile( pFile, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE );
		WriteToFile( pFile, UNI(m_kRhsXmlInfo.kFileName) + NEWLINE + NEWLINE);
	
		ErrorToFile( pFile, m_kRhsXmlInfo.kTextMap );
		WriteToFile( pFile, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE + NEWLINE );
		
		if( !m_kComparer.ShowResultToFile(pFile) )
		{//compare
			fclose(pFile);
			return false;			
		}
		
		if( !m_kSameFinder.ShowResultToFile(pFile) )
		{//same
			fclose(pFile);
			return false;			
		}
		//
		if( !m_kUniqueFinder.ShowResultToFile(pFile) )
		{//unique
			fclose(pFile);
			return false;			
		}

		fclose(pFile);
		return true;
	}

	return false;
}

bool PgTextChecker::ShowErrorToFile(std::string const& kFileName) const
{
	FILE* pFile;
	fopen_s(&pFile, kFileName.c_str(), "wb");
	if( pFile )
	{
		WriteToFile( pFile, UTF_16_BOM );
		
		WriteToFile( pFile, _T("/////////////////") + NEWLINE );
		WriteToFile( pFile, _T("// Error Text  //") + NEWLINE );
		WriteToFile( pFile, _T("/////////////////") + NEWLINE );
		WriteToFile( pFile, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE );

		std::wstring const kFileName = UNI(m_kLhsXmlInfo.kFileName) + NEWLINE + NEWLINE;
		WriteToFile( pFile, kFileName);
		
		ErrorToFile( pFile, m_kLhsXmlInfo.kTextMap );	
		
		WriteToFile( pFile, _T(" - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") + NEWLINE );
		fclose(pFile);

		return true;
	}
	return false;
}


bool PgTextChecker::ShowAllResultToHTML(std::string const& kFileName) const
{	
	FILE* pFile;
	std::string const kHTMLFileName = kFileName;
	
	fopen_s(&pFile, kFileName.c_str(), "wb");
	if(pFile)
	{
		WriteToFile( pFile, UTF_16_BOM );
		WriteToFile( pFile, HTML_OPEN_MARK + HTML_HTML + HTML_CLOSE_MARK );
		WriteToFile( pFile, HTML_OPEN_MARK + HTML_PRE + HTML_CLOSE_MARK );
		
		//error check
		WriteToFile( pFile, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( pFile, HTML_DecoText( _T("// Error Text  //"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( pFile, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		
		WriteToFile( pFile, HTML_DecoText( UNI( m_kLhsXmlInfo.kFileName ), HTML_COLOR_BLU, 4 )  + NEWLINE + NEWLINE );
		ErrorToHTML( pFile, m_kLhsXmlInfo.kTextMap);

		WriteToFile( pFile, HTML_DecoText( UNI( m_kRhsXmlInfo.kFileName ), HTML_COLOR_BLU, 4 )  + NEWLINE + NEWLINE );	
		ErrorToHTML( pFile, m_kRhsXmlInfo.kTextMap );
				
		WriteToFile( pFile, HTML_DecoText( _T("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"), HTML_COLOR_RED, 5 )+ NEWLINE + NEWLINE );

		if( !m_kComparer.ShowResultToHTML(pFile) )
		{//compare
			fclose(pFile);
			return false;
		}
		WriteToFile( pFile, HTML_DecoText( _T("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"), HTML_COLOR_RED, 5 )+ NEWLINE + NEWLINE );

		
		//if( !m_kSameFinder.ShowResultToHTML(pFile) )
		//{//same
		//	fclose(pFile)
		//	return false;			
		//}
		//WriteToFile( pFile, HTML_DecoText( _T("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -") )+ NEWLINE + NEWLINE );

		if( !m_kUniqueFinder.ShowResultToHTML(pFile) )
		{//unique
			fclose(pFile);
			return false;			
		}

		WriteToFile( pFile, HTML_END_OPEN_MARK + HTML_PRE + HTML_CLOSE_MARK );
		WriteToFile( pFile, HTML_END_OPEN_MARK + HTML_HTML + HTML_CLOSE_MARK );

		fclose(pFile);
		return true;
	}
	return false;
}

void PgTextChecker::ErrorToHTML(FILE* pFile, CONT_DRCA_TEXTTBL_INFO const& kTextMap) const
{
	for(CONT_DRCA_TEXTTBL_INFO::const_iterator itor = kTextMap.begin(); itor != kTextMap.end(); ++itor)
	{
		std::wstring const& kText = (*itor).second.kText;
		std::wstring const& kIdStr = (*itor).second.kIdStr;
		CONT_COMMAND_INFO const& kErrCommandInfoVector = (*itor).second.kErrCommandInfoVector;
		if( 0 < (*itor).second.uiErrorCnt)
		{//Error가 있는 문장만 출력
			//ID와 원본 문장 출력
			//WriteToFile( pFile, kIdStr + COLON_STR + kText + NEWLINE  );
			
			//for(unsigned int i=0; i < kIdStr.size() + COLON_STR.size(); ++i ) 
			//{////ID를 출력한만큼 ' '를 출력하여 자리를 맞춤 				
			//	WriteToFile( pFile, _T(" ") );
			//}

///////////////////////////////////////////////////////////////////////////////////////////
			
			size_t accIndex = 0;
			unsigned int uiCheckIndex=0;
			std::wstring kCopyText = kText;

			for(std::wstring::size_type i=0; i < kText.size(); ++i)
			{
				size_t Index = kErrCommandInfoVector[uiCheckIndex].uiIndex;

				if(i ==  kErrCommandInfoVector[uiCheckIndex].uiIndex)
				{
					//html 명령어 올리기
					std::wstring kTempHTMLCmd = HTML_OPEN_MARK + HTML_FONT + HTML_Color( HTML_COLOR_RED )  + HTML_CLOSE_MARK;
					kCopyText.insert(i+accIndex, kTempHTMLCmd.c_str());
					accIndex += kTempHTMLCmd.size();

					//html 명령어 막기
					std::wstring::size_type UnitSize = kErrCommandInfoVector[uiCheckIndex].kUnitStr.size();
					kTempHTMLCmd = HTML_Sup(uiCheckIndex+1) + HTML_END_OPEN_MARK + HTML_FONT + HTML_CLOSE_MARK;

					kCopyText.insert(i+accIndex+UnitSize, kTempHTMLCmd.c_str());				
					accIndex += kTempHTMLCmd.size();

					++uiCheckIndex;
					if( kErrCommandInfoVector.size() <= uiCheckIndex )
					{//출력했으면 끝낸다.
						break;
					}
				}
			}

			WriteToFile( pFile, kIdStr + COLON_STR + kCopyText + NEWLINE + NEWLINE );

///////////////////////////////////////////////////////////////////////////////////////////
		//	if( 0 < (*itor).second.uiErrorCnt )
		//	{
		//		WriteToFile( pFile, _T("please check this : ") );
		//		if( SDetailTextInfo::ERR_BRACE & (*itor).second.uiErrFlag )
		//		{
		//			WriteToFile( pFile, _T("{ } unit, ") );
		//		}
		//		if( SDetailTextInfo::ERR_PERCENT & (*itor).second.uiErrFlag )
		//		{
		//			WriteToFile( pFile, _T("% unit") );
		//		}
		//		WriteToFile( pFile, NEWLINE + NEWLINE );
		//	}
		}
	}
}

bool PgTextChecker::ShowErrorToHTML(std::string const& kFileName) const
{
	FILE* pFile = NULL;
	std::string const kHTMLFileName = kFileName;

	fopen_s(&pFile, kFileName.c_str(), "wb");

	if(pFile)
	{
		WriteToFile( pFile, UTF_16_BOM );
		WriteToFile( pFile, HTML_OPEN_MARK + HTML_HTML + HTML_CLOSE_MARK );
		WriteToFile( pFile, HTML_OPEN_MARK + HTML_PRE + HTML_CLOSE_MARK );
		
		//error check
		WriteToFile( pFile, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( pFile, HTML_DecoText( _T("// Error Text  //"), HTML_COLOR_RED, 5 ) + NEWLINE );
		WriteToFile( pFile, HTML_DecoText( _T("/////////////////"), HTML_COLOR_RED, 5 ) + NEWLINE );
		
		WriteToFile( pFile, HTML_DecoText( UNI( m_kLhsXmlInfo.kFileName ), HTML_COLOR_BLU, 4 )  + NEWLINE + NEWLINE );
		ErrorToHTML( pFile, m_kLhsXmlInfo.kTextMap);
		
		WriteToFile( pFile, HTML_END_OPEN_MARK + HTML_PRE + HTML_CLOSE_MARK );
		WriteToFile( pFile, HTML_END_OPEN_MARK + HTML_HTML + HTML_CLOSE_MARK );

		fclose(pFile);
		return true;
	}
	return false;
}