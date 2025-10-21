#include "stdafx.h"
#include "def_strings.h"

static char const* const XML_ELEMENT_STRING_TABLE_LOCAL = "STRING_TABLE_LOCAL";
static char const* const XML_ELEMENT_TEXT = "TEXT";
static char const* const XML_ATTRIBUTE_ID = "ID";
static char const* const XML_ATTRIBUTE_TEXT = "Text";

static void ParseTEXTElement(TiXmlAttribute const* pkAttribute, CONT_DEFSTRINGS &out, bool &utf8)
{
	CONT_DEFSTRINGS::mapped_type kElement;
	char const* pcText = NULL;

	while(pkAttribute)
	{
		char const* pcAttrName = pkAttribute->Name();

		if(0 == _stricmp(pcAttrName, XML_ATTRIBUTE_ID))
		{
			kElement.StringNo = pkAttribute->IntValue();
		}
		else if(0 == _stricmp(pcAttrName, XML_ATTRIBUTE_TEXT))
		{
			pcText = pkAttribute->Value();
		}

		pkAttribute = pkAttribute->Next();
	}

    kElement.strText = utf8 ? BM::vstring::ConvToUnicodeFromUTF8(pcText) : UNI(pcText);
	if( kElement.StringNo &&
		!kElement.strText.empty() )
	{
		auto kRet = out.insert( std::make_pair(kElement.StringNo, kElement) );
		if(!kRet.second)
		{
			INFO_LOG( BM::LOG_LV1, __FL__ << L"DefString Duplicated StringNo[" << kElement.StringNo << "]" );
		}
	}
}

bool defstrings::parse_def_string_xml(TiXmlNode const* pkNode, CONT_DEFSTRINGS &out, bool &utf8)
{
	while(pkNode)
	{
		switch(pkNode->Type())
		{
		case TiXmlNode::ELEMENT:
			{
				TiXmlElement* pkElement = (TiXmlElement*)pkNode;
				assert(pkElement);

				char const* pcTagName = pkElement->Value();
				if(0 == strcmp(pcTagName, XML_ELEMENT_TEXT))
				{
					ParseTEXTElement(pkElement->FirstAttribute(), out, utf8);
				}
				else if(0 == strcmp(pcTagName, XML_ELEMENT_STRING_TABLE_LOCAL))
				{
					TiXmlNode const* pkChildNode = pkNode->FirstChild();
					if(pkChildNode != NULL)
					{
						if(!parse_def_string_xml(pkChildNode, out, utf8))
						{
							return false;
						}
					}
				}
			}break;

		case TiXmlNode::DECLARATION:
			{				
				TiXmlDeclaration const* pDecl = dynamic_cast<TiXmlDeclaration const*>(pkNode);
				if(pDecl && pDecl->Encoding())
				{
					std::string strEncoding = pDecl->Encoding();
					UPR(strEncoding);

					if( 0 == strEncoding.compare(std::string("UTF-8")) || 
						0 == strEncoding.compare(std::string("UTF8")) )
					{
						utf8 = true;
					}
				}	
			}break;

		default:
			{

			}break;
		}

		pkNode = pkNode->NextSibling();
	}

	return true;
}

bool defstrings::load(const char* filepath, CONT_DEFSTRINGS &out, bool &utf8)
{
    TiXmlDocument kDoc(filepath);
    if(!kDoc.LoadFile())
    {
    	INFO_LOG( BM::LOG_LV1, __FL__ << filepath
    		<< L" XML Load Error : " << UNI(kDoc.ErrorDesc())
    		<< L" Row : " << kDoc.ErrorRow()
    		<< L" Col : " << kDoc.ErrorCol());
        kDoc.Clear();
        return false;
    }

    if (!defstrings::parse_def_string_xml(kDoc.FirstChild(), out, utf8))
    {
        INFO_LOG( BM::LOG_LV1, __FL__ << L"XML Parsing Error : " << filepath );
        kDoc.Clear();
        return false;
    }

    kDoc.Clear();
    return true;
}