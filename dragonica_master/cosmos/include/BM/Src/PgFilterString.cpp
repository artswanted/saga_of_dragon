#include "stdafx.h"
#include "PgFilterString.h"
#include "BM/vstring.h"
#include "BM/LocalMgr.h"
#include "BM/Common.h"

using namespace BM;

PgFilterString::tagStringInfo::tagStringInfo(std::wstring const& rkStr)
	: kOrgStr(rkStr), kUprStr(rkStr)
{
	UPR(kUprStr);
	Build();
}

bool PgFilterString::ParseXml(std::wstring const& rkFileName)
{
	TiXmlDocument doc( MB(rkFileName) );
	bool loadOkay = doc.LoadFile();
	if(!loadOkay)
	{
		__asm int 3;
		return false;
	}

	return ParseXml(doc);
}

bool PgFilterString::ParseXml(TiXmlDocument &doc)
{
	bool loadOkay = doc.LoadFile();
	if( !loadOkay )
	{
		__asm int 3;
		return false;
	}

	bool bUTF8 = false;

	TiXmlNode const *pkRootNode = doc.FirstChild();
	while(pkRootNode && pkRootNode->Type() != TiXmlNode::ELEMENT)
	{
		int const iType = pkRootNode->Type();
		if (iType == TiXmlNode::DECLARATION)
		{				
			TiXmlDeclaration  const* pDecl = dynamic_cast<TiXmlDeclaration const*>(pkRootNode);
			
			if (NULL != pDecl)
			{
				std::string encoding = pDecl->Encoding();
				UPR(encoding);

				if (encoding.compare(std::string("UTF-8")) == 0 || encoding.compare(std::string("UTF8")) == 0)
				{
					bUTF8 = true;
				}
			}
		}
		pkRootNode = pkRootNode->NextSibling();
	}

	TiXmlNode const *pkNode = pkRootNode;

	return ParseXml(pkNode, bUTF8);
}

bool PgFilterString::ParseXml(TiXmlNode const *pkNode, bool bUTF8)
{
	EFilterTextSetType eXmlType = ETST_NONE;
	int const iType = pkNode->Type();
	bool bUTF8Checked = bUTF8;

	while(pkNode)
	{
		switch(iType)
		{
		case TiXmlNode::DECLARATION:
			{				
				TiXmlDeclaration  const* pDecl = dynamic_cast<TiXmlDeclaration const*>(pkNode);
				
				if (pDecl && pDecl->Encoding())
				{
					std::string encoding = pDecl->Encoding();
					UPR(encoding);

					if (encoding.compare(std::string("UTF-8")) == 0 || encoding.compare(std::string("UTF8")) == 0)
					{
						bUTF8Checked = true;
					}
				}	
			}break;

		case TiXmlNode::ELEMENT:
			{
				TiXmlElement const* pkElement = (TiXmlElement const*)pkNode;
				assert(pkElement);
				
				char const *pcTagName = pkElement->Value();

				if(_stricmp(pcTagName, "BAD_WORD")==0)
				{
					eXmlType = ETST_BAD_WORD;
					// 자식 노드들을 파싱한다.
					// 첫 자식만 여기서 걸어주면, 나머지는 NextSibling에 의해서 자동으로 파싱된다.
					TiXmlNode const* pkChildNode = pkNode->FirstChild();
					pkNode = pkChildNode;
					{
						EFilterProcessType eProcessType = E_DETAIL_FILTER;
						TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
						if(pkAttr)
						{
							char const	*strText=0;
							char const *pcAttrName = pkAttr->Name();
							char const *pcAttrValue = pkAttr->Value();
							if(_stricmp(pcAttrName, "TYPE") == 0)
							{
								eProcessType= CheckFilterProcessType(atol(pcAttrValue));
							}
						}
						SetBadWordFilterProcessType(eProcessType);
					}
					continue;
				}
				else if(_stricmp(pcTagName, "GOOD_WORD")==0)
				{
					eXmlType = ETST_GOOD_WORD;
					// 자식 노드들을 파싱한다.
					// 첫 자식만 여기서 걸어주면, 나머지는 NextSibling에 의해서 자동으로 파싱된다.
					TiXmlNode const* pkChildNode = pkNode->FirstChild();
					pkNode = pkChildNode;
					continue;
				}
				else if(_stricmp(pcTagName, "IGNORE_WORD")==0)
				{
					eXmlType = ETST_IGNORE_WORD;
					// 자식 노드들을 파싱한다.
					// 첫 자식만 여기서 걸어주면, 나머지는 NextSibling에 의해서 자동으로 파싱된다.
					TiXmlNode const* pkChildNode = pkNode->FirstChild();
					pkNode = pkChildNode;
					continue;
				}
				else if(_stricmp(pcTagName, "BLOCK_NICK_NAME")==0)
				{
					eXmlType = ETST_BLOCK_NICK_NAME;
					TiXmlNode const* pkChildNode = pkNode->FirstChild();
					pkNode = pkChildNode;
					{
						EFilterProcessType eProcessType = E_DETAIL_FILTER;
						TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();
						if(pkAttr)
						{
							char const	*strText=0;
							char const *pcAttrName = pkAttr->Name();
							char const *pcAttrValue = pkAttr->Value();
							if(_stricmp(pcAttrName, "TYPE") == 0)
							{
								eProcessType= CheckFilterProcessType(atol(pcAttrValue));
							}
						}
						SetBlockNickFilterProcessType(eProcessType);
					}
					continue;
				}
				else if(_stricmp(pcTagName, "TEXT") == 0)
				{
					const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
					unsigned long Lv=0;
					char const	*strText=0;
					while(pkAttr)
					{

						char const *pcAttrName = pkAttr->Name();
						char const *pcAttrValue = pkAttr->Value();

						if(_stricmp(pcAttrName, "LV") == 0)
						{
							Lv = (unsigned long)atol(pcAttrValue);
						}
						else if(_stricmp(pcAttrName, "Text") == 0)
						{
							strText=pcAttrValue;
						}
						else
						{
							assert(!"invalid attribute");
						}

						pkAttr = pkAttr->Next();
					}				

					std::wstring uniText;
					if (bUTF8Checked)
					{
						uniText = BM::vstring::ConvToUnicodeFromUTF8(strText);
					}
					else
					{
						if(NULL != strText)
						{
							uniText = UNI(strText);
						}
					}
					switch(eXmlType)
					{
					case ETST_BAD_WORD:
						{
							RegBadString(uniText);
						}break;
					case ETST_GOOD_WORD:
						{
							RegGoodString(uniText);
						}break;
					case ETST_BLOCK_NICK_NAME:
						{
							RegBlockNickString(uniText);
						}break;
					case ETST_IGNORE_WORD:
						{
							if(!pkElement->NoChildren())
							{
								TiXmlNode const* pkChild = pkElement->FirstChild();
								pkChild->Type();
								strText = pkChild->Value();
								RegIgnoreString(UNI(strText));
							}
							else
							{
								RegIgnoreString(uniText);
							}
						}break;
					default:
						{
						}break;
					}
	//				return true;
				}
			}
		}

		TiXmlNode const* pkNextNode = pkNode->NextSibling();
		pkNode = pkNextNode;
	}
	return	true;
}

bool PgFilterString::RegGoodString(std::wstring const& str)
{
	std::wstring::const_iterator str_itor = str.begin();
	while(str_itor != str.end())
	{
		m_kContGoodWord.push_back((*str_itor));
		++str_itor;
	}
	return true;
}

std::wstring const PgFilterString::GetGoodString(size_t const size)const
{
	std::wstring kStrResult;
	if(!m_kContGoodWord.size())
	{
		kStrResult.append(size, _T(' '));
	}
	else
	{
		size_t i = size;
		while(i)
		{
			kStrResult.append(1, m_kContGoodWord.at(BM::Rand_Index(m_kContGoodWord.size())));
			--i;
		}
	}

	return kStrResult;
}

bool PgFilterString::RegBadText(std::wstring const& str, TOTAL_CONT& rkTargetCont)
{
	SStringInfo kInfo(str);
	if( !kInfo.Empty() )
	{
		std::wstring const& rkResultText = kInfo.UprStr();
		
		auto ret = rkTargetCont.insert(std::make_pair(rkResultText.at(0), CONT_STRING()));
		
		ret.first->second.push_back(kInfo);
		return true;
	}
	
	__asm int 3;
	return false;
}

bool PgFilterString::RegBadString(std::wstring const& str)
{
	return RegBadText(str, m_kBadWord);
}

bool PgFilterString::RegBlockNickString(std::wstring const& str)
{
	return RegBadText(str, m_kBlockNickName);
}

bool PgFilterString::RegIgnoreString(std::wstring const& str)
{
	std::wstring::const_iterator str_itor = str.begin();
	while(str_itor != str.end())
	{
		m_kIgnoreWord.insert((*str_itor));
		++str_itor;
	}
	
	return true;
}

bool PgFilterString::Filter(std::wstring &str, bool const bIsConvert, EFilterSourceType const eFilterType)const
{
	SStringInfo kInfo(str);
	if( !kInfo.Empty() )
	{
		bool bRet = false;
		std::wstring kCopy = str;
		
		if( eFilterType & FST_BADWORD )
		{
			if(FilterDetail(kInfo, bIsConvert, m_kBadWord, GetBadWordFilterProcessType()))
			{// xml에서 얻어온 필터링 방식으로 필터링
				bRet = true;
			}
		}

		if( eFilterType & FST_NICKNAME )
		{
			bRet = FilterDetail(kInfo, bIsConvert, m_kBlockNickName, GetBlockNickFilterProcessType());
		}

		if( bRet
		&&	bIsConvert )
		{
			str = kInfo.Str();
		}

		if( bRet )
		{
			return true;//걸러졌다
		}
	}

	return false;//아무것도 안걸렸다
}

bool PgFilterString::FilterDetail(SStringInfo& rkInput, bool const bIsConvert, TOTAL_CONT const& rkFromCont, EFilterProcessType const eProcessType) const
{
	bool bRet = false;

	CONT_STRING_ANALYSIS::const_iterator anal_itor = rkInput.m_mapAnalysis.begin();
	while(anal_itor != rkInput.m_mapAnalysis.end())
	{
		TOTAL_CONT::const_iterator total_itor = rkFromCont.find((*anal_itor).first);

		if(total_itor != rkFromCont.end())
		{
			CONT_STRING const &lstBadWord = (*total_itor).second;
			
			CONT_STRING::const_reverse_iterator word_itor = lstBadWord.rbegin();
			while(lstBadWord.rend() != word_itor)
			{
				std::wstring const& rBadWord = (*word_itor).UprStr();
				switch(eProcessType) 
				{
				case E_SIMPLE_FILTER:
					{
						if(DoFilter_Simple(rkInput, rBadWord))
						{
							bRet = true;
						}
					}break;
				case E_DETAIL_FILTER:
				default:
					{
						if(DoFilter(rkInput, rBadWord))
						{
							bRet = true;
						}
					}break;
				}
				++word_itor;
			}
		}
	
		++anal_itor;
	}
	
	return bRet;
}

size_t PgFilterString::DoFilter_Simple(SStringInfo& rkOrgInfo, std::wstring const& rkBadWord)const
{
	bool bRet = false;
	std::wstring &kUprStr = rkOrgInfo.kUprStr;
	std::wstring &kOrgStr = rkOrgInfo.kOrgStr;

	size_t const badword_size = rkBadWord.size();

	std::wstring::size_type offset = 0;

	while(offset != std::wstring::npos)
	{
		offset = kUprStr.find(rkBadWord, offset);
		
		if(offset != std::wstring::npos)
		{//욕설 전후로 IgnoreWord가 없으면. 무시함.
			bool bWillConvert = true;
			if(offset > 0)
			{
				wchar_t const side_key = kUprStr.at(offset-1);
				if(m_kIgnoreWord.end() == m_kIgnoreWord.find(side_key))//무시 할 수 없는 글자가 와있음.
				{
					bWillConvert = false;
				}
			}
			
			if(offset+badword_size < kUprStr.size())
			{
				wchar_t const side_key = kUprStr.at(offset+badword_size);
				if(m_kIgnoreWord.end() == m_kIgnoreWord.find(side_key))//무시 할 수 없는 글자가 와있음.
				{
					bWillConvert = false;
				}
			}

			if(bWillConvert)
			{
				kOrgStr.replace(offset, badword_size, GetGoodString(badword_size));
			}
			offset += badword_size+1;
			bRet = true;
		}
	}

	return bRet;
}

size_t PgFilterString::DoFilter(SStringInfo& rkOrgInfo, std::wstring const& rkBadWord)const
{
	if(!rkBadWord.size())
	{
		return 0;
	}
	
	std::wstring::const_iterator key_itor = rkBadWord.begin();
	std::wstring::value_type first_key = (*key_itor);

	if(!rkOrgInfo.IsExist(first_key))
	{//첫글자가 존재 안함.
		return 0;
	}

	std::wstring &kUpStr = rkOrgInfo.kUprStr;//UprStr로 필터링 작업

	size_t offset_find = 0;
	bool bIsStop = false;
	size_t ret_count = 0;
	while(!bIsStop)
	{
		if(!rkBadWord.size())
		{
			bIsStop = true;
			break;
		}

		CONT_FILTER_POS kContFilterPos;

		std::wstring::const_iterator bad_word_itor = rkBadWord.begin();
		while(rkBadWord.end() != bad_word_itor)
		{
			CONT_FILTER_POS::value_type kDelPos;
			{
				std::wstring::value_type const &kBadElement = (*bad_word_itor);
				std::wstring::size_type const found_ret = kUpStr.find(kBadElement, offset_find);
				if(std::wstring::npos != found_ret)
				{
					kDelPos.SetStart(found_ret);
					offset_find = found_ret +1;//옵셋 이동.
				}
				else
				{//욕설 구성 글자를 찾을 수 없음
					bIsStop = true;
					break;
				}
			}

			++bad_word_itor;//다음 글자

			if(rkBadWord.end() != bad_word_itor)
			{//다음글자 있음.
				std::wstring::value_type const &kBadElement = (*bad_word_itor);
				std::wstring::size_type const found_ret = kUpStr.find(kBadElement, offset_find);
				if(std::wstring::npos != found_ret)
				{
					kDelPos.SetEnd(found_ret);//찾았음
					offset_find = found_ret;//옵셋 이동. 이때는 이 글자가 또 검색 되어야 하므로. +1 하지 않는다.
				}
				else
				{// 찾을 수 없다면 욕이 아님.
					bIsStop = true;
					break;
				}
			}
			//++ 연산은 위쪽에 있음.
			kContFilterPos.push_back(kDelPos);//첫글자 찾
		}

		//욕설 오프셋 합산(kContFilterPos)을 체크 해야함.
		if(!bIsStop)//멈춤신호가 안들어왔으면 해봄.
		{
			bool bIsFilter = true;
			std::wstring &kOrgStr = rkOrgInfo.kOrgStr;//UprStr로 필터링 작업
			CONT_FILTER_POS::const_iterator filter_itor = kContFilterPos.begin();
			while(	bIsFilter
				&&	filter_itor != kContFilterPos.end())
			{
				//	kUpStr
				//	kOrgStr
				std::wstring kParseStr;
				CONT_FILTER_POS::value_type const &kDelPos = (*filter_itor);
				if(S_OK == kDelPos.SubStrForIgnoreCheck(kUpStr, kParseStr))
				{
					std::wstring::const_iterator parse_itor = kParseStr.begin();
					while(kParseStr.end() != parse_itor)
					{
						if( m_kIgnoreWord.end() == m_kIgnoreWord.find((*parse_itor)))
						{//Ignore 워드가 아니므로. 필터링을 하지마라.
							bIsFilter = false;
							break;
						}
						++parse_itor;
					}
				}
				++filter_itor;
			}

			if(bIsFilter)
			{
				CONT_FILTER_POS::const_iterator filter_start_itor = kContFilterPos.begin();
				CONT_FILTER_POS::const_reverse_iterator filter_end_itor = kContFilterPos.rbegin();

				std::wstring::size_type const offset_start = (*filter_start_itor).StartOffset();
				std::wstring::size_type const offset_end = (*filter_end_itor).EndOffset();
				if(	std::wstring::npos != offset_start 
				&&	std::wstring::npos != offset_end
				&&	offset_end >= offset_start )
				{
					size_t const filter_count = offset_end - offset_start +1;
					kOrgStr.replace(offset_start, filter_count, GetGoodString(filter_count));
					kUpStr.replace(offset_start, filter_count, GetGoodString(filter_count));
				}
				++ret_count;
			}
		}
	}
	return ret_count;//이게 0으로 리턴 되지 않아야 필터링 되는것이다.
}

PgFilterString::EFilterProcessType PgFilterString::CheckFilterProcessType(int const iXMLFilterType)
{	
	if( E_UNKNOWN_FILTER >= iXMLFilterType 
		|| E_MAX_FILTER_NUM <= iXMLFilterType)
	{// 적절한 값이 아니면 
		return E_DETAIL_FILTER;
	}
	return static_cast<EFilterProcessType>(iXMLFilterType);
}

void PgFilterString::SetBadWordFilterProcessType(EFilterProcessType const eProcessType)
{ 
	m_eBadWordFilterProcessType = eProcessType;	 
}

void PgFilterString::SetBlockNickFilterProcessType(EFilterProcessType const eProcessType) 
{ 
	m_eBlockNameFilterProcessType = eProcessType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgUnicodeFilter::PgUnicodeFilter()
{
}

PgUnicodeFilter::~PgUnicodeFilter()
{
}

bool PgUnicodeFilter::IsCorrect(int const iFuncCode, std::wstring const& kOrgStr)const
{
	CONT_ERROR_RESULT kOutResult;
	return IsCorrect(iFuncCode, kOrgStr, kOutResult);
}
		
bool PgUnicodeFilter::IsCorrect(int const iFuncCode, std::wstring const& kOrgStr, CONT_ERROR_RESULT &kOutResult)const
{
	std::wstring::const_iterator itor = kOrgStr.begin();
	while(itor != kOrgStr.end())
	{
		wchar_t const& kElement = (*itor);
		{// 개행 문자나, 캐리지 리턴 문자는 필터링 대상에서 열외 시키고
			if( true == IsInvalid_Sub(iFuncCode, kElement)
				||	false == IsCorrect_Sub(iFuncCode, kElement) )
			{// 문자가 속한 범주를 체크한다
				kOutResult.insert(kElement);
			}
		}

		++itor;
	}

	if(kOutResult.empty())
	{
		return true;
	}
	return false;
}

bool PgUnicodeFilter::AddRange(int const iFuncCode, bool const bIsInvalid, wchar_t const kStart, wchar_t const kEnd)
{
	CONT_WORD_HASH::value_type kNewElement(iFuncCode, kStart, kEnd);
	if(false == bIsInvalid)
	{
		if( m_kContCorrectRange.end() == std::find(m_kContCorrectRange.begin(), m_kContCorrectRange.end(), kNewElement) )
		{
			m_kContCorrectRange.push_back( kNewElement );
			return true;
		}
	}
	else
	{
		if( m_kContInvalidRange.end() == std::find(m_kContInvalidRange.begin(), m_kContInvalidRange.end(), kNewElement) )
		{
			m_kContInvalidRange.push_back( kNewElement );
			return true;
		}
	}

	__asm int 3;
	return false;
}

bool PgUnicodeFilter::IsCorrect_Sub(int const iFuncCode, wchar_t const kWord)const 
{
	if( m_kContCorrectRange.empty() )
	{
		return true;
	}

	size_t iCount = 0;
	CONT_WORD_HASH::const_iterator itor = m_kContCorrectRange.begin();
	while(itor != m_kContCorrectRange.end())
	{
		CONT_WORD_HASH::value_type const& rkFilter = (*itor);
		if(	0 != (rkFilter.iFuncCode & iFuncCode) )
		{
			++iCount;
			if(	kWord >= rkFilter.cStartChar
			&&	kWord <= rkFilter.cEndChar )
			{//영역 OK~
				return true;
			}
		}
		++itor;
	}
	if( iCount == 0 )
	{
		return true;
	}
	return false;
}

bool PgUnicodeFilter::IsInvalid_Sub(int const iFuncCode, wchar_t const kWord)const 
{
	CONT_WORD_HASH::const_iterator itor = m_kContInvalidRange.begin();
	while(itor != m_kContInvalidRange.end())
	{
		CONT_WORD_HASH::value_type const& rkFilter = (*itor);
		if(	0 != (rkFilter.iFuncCode & iFuncCode)
		&&	kWord >= rkFilter.cStartChar
		&&	kWord <= rkFilter.cEndChar)
		{//영역 OK~
			return true;
		}
		++itor;
	}
	return false;
}
