#include "ConfigXml.h"

CConfigXml::CConfigXml(void)
: m_DocConfig(0)
{
	Seek();
}

CConfigXml::~CConfigXml(void)
{
	if(m_DocConfig)
	{
		m_DocConfig->Clear();
		SAFE_DELETE(m_DocConfig);
	}
}

bool CConfigXml::Seek()
{
	std::vector<char> vecData;
	std::vector<char> vecOut;
	std::vector<char>& rkVecData = vecOut;

	if(_access(g_szConfigFileName, 0) == -1)
	{
		//	Config.xml 
		//	없네 팩에서 꺼낸 후 파싱하자
		//	팩을 읽는 기야
		//size_t	iHeadSize = BM::PgDataPackManager::ReadHeader(L"./XML.DAT", kHeadHash);
		//PgFileMapping	kXmlData;
		//kXmlData.Init(L"./XML.DAT");

		BM::PgDataPackManager::PackFileCache_new kXmlCache;
		if(!kXmlCache.Open(L"./xml.dat"))
		{
			return false;
		}
		const size_t	iHeadSize = BM::PgDataPackManager::ReadHeader(kXmlCache);
		BM::FolderHash const& kHeadHash = kXmlCache.mFolderHash;

		//	읽었냐?
		if(!kHeadHash.size())
		{
			return	false;
		}

		//	파일을 찾는 기야
		const BM::PgFileInfo*	pFileInfo = NULL;
		if(!(*kHeadHash.begin()).second->Find(UNI(g_szDefaultConfigFileName), pFileInfo))
		{
			return	false;
		}

		if( !pFileInfo )
		{
			return false;
		}

		size_t	iEncHSize = 0;
		if(pFileInfo->bIsEncrypt)
		{
			iEncHSize	= BM::ENC_HEADER_SIZE;
		}

		//	파일의 위치로 가는 기야
		//const char* pSrc = kXmlData.Data() + iHeadSize + pFileInfo->offset;

		//	파일의 정보를 읽는 기야
		if(pFileInfo->bIsZipped)
		{
			vecData.resize(pFileInfo->zipped_file_size + iEncHSize);
			//memcpy(&vecData.at(0), pSrc, pFileInfo->zipped_file_size + iEncHSize);
			kXmlCache.ReadData(iHeadSize + pFileInfo->offset, &vecData.at(0), pFileInfo->zipped_file_size + iEncHSize);

			if(pFileInfo->bIsEncrypt)
			{
				if (false == DecryptData(pFileInfo->org_file_size, pFileInfo->zipped_file_size, pFileInfo->bIsZipped, pFileInfo->bIsEncrypt, vecData, vecOut))
				{
					return false;
				}
			}
			else
			{
				if (pFileInfo->bIsZipped)
				{
					if (false == UnCompressData(pFileInfo->org_file_size, vecData, vecOut))
					{
						return false;
					}
				}
				else
				{
					rkVecData = vecData;
				}
			}
		}
		else
		{
			vecData.resize(pFileInfo->org_file_size + iEncHSize);
			//memcpy(&vecData.at(0), pSrc, pFileInfo->org_file_size + iEncHSize);
			kXmlCache.ReadData(iHeadSize + pFileInfo->offset, &vecData.at(0), pFileInfo->org_file_size + iEncHSize);

			if(pFileInfo->bIsEncrypt)
			{
				if (false == DecryptData(pFileInfo->org_file_size, pFileInfo->zipped_file_size, pFileInfo->bIsZipped, pFileInfo->bIsEncrypt, vecData, vecOut))
				{
					return false;
				}
			}
			else
			{
				rkVecData = vecData;
			}
		}
		//	안전제일!
		rkVecData.push_back('\n');
		Parse(rkVecData);
		DelElement(m_DocConfig);
	}
	else
	{
		//	있네 읽어서 파싱하는 기야
		vecData.clear();
		BM::FileToMem(UNI(g_szConfigFileName), vecData);
		vecData.push_back('\n');	//	안전 제일
		Parse(vecData);
	}

	GetDocOptionVal();

	return	true;
}

//	암호화 해제
bool CConfigXml::DecryptData(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncrypted, std::vector<char> const& rkVecData, std::vector<char>& rkVecOut) const
{
	//std::vector<char> vecSrc;
	//vecSrc.resize(Result.size());
	//memcpy(&vecSrc.at(0), &Result.at(0), Result.size());
	return BM::DecLoadFromMem(FileOrgSize, FileZipSize, bIsCompressed, bIsEncrypted, rkVecData, rkVecOut);
}

//	압축 해제
bool CConfigXml::UnCompressData(const size_t& OrgFileSize, std::vector<char>& rkVecData, std::vector<char>& rkVecOut) const
{
	//std::vector<char> vecTemp;
	//vecTemp.resize(OrgFileSize);
	rkVecOut.resize(OrgFileSize);
	return BM::UnCompress(rkVecOut, rkVecData);
	//Result = vecTemp;
}

//	파싱해서 읽는거
bool CConfigXml::Parse(std::vector<char>& Data)
{
	//	없으면 하지마
	if(!Data.size()){ return false; }

	TiXmlDocument*	pkDocConfig = NULL;
	//	준비하자
	try
	{
		pkDocConfig	= new TiXmlDocument(g_szConfigFileName);
		pkDocConfig->Clear();
		pkDocConfig->Parse(&Data.at(0));
	}
	catch(...)
	{
		SAFE_DELETE(pkDocConfig);
		//	파싱 도중 에러 발생
	}

	//	에러가 있다
	if( pkDocConfig->Error() )
	{
		SAFE_DELETE(pkDocConfig);
		return false;
	}

	if( m_DocConfig )
	{
		SAFE_DELETE(m_DocConfig);
	}

	m_DocConfig = pkDocConfig;

	return	true;
}

bool CConfigXml::DelElement(TiXmlDocument*& Docu) const
{
	//	필요없는거 지워라
	TiXmlNode*	Node = Docu->FirstChild(XML_GROUP_CONFIG);
	Node->FirstChild(XML_ELEMENT_CAMERA)->Clear();
	Node->FirstChild(XML_ELEMENT_GRAPHIC1)->Clear();
	Node->FirstChild(XML_ELEMENT_GRAPHIC2)->Clear();
	Node->FirstChild(XML_ELEMENT_GRAPHIC3)->Clear();
	Node->FirstChild(XML_ELEMENT_KEYSET)->Clear();
	Node->FirstChild(XML_ELEMENT_ETC)->Clear();
	Node->FirstChild(XML_ELEMENT_USER_UI)->Clear();
	Node->FirstChild(XML_ELEMENT_VERSION)->Clear();

	return	true;
}

//	기본 설정값 얻기
bool CConfigXml::GetDocOptionVal()
{
	if( !m_DocConfig )
	{
		return false;
	}

	//	대분류 얻기
	TiXmlNode*	Node = m_DocConfig->FirstChild(XML_GROUP_CONFIG);
	if( !Node )
	{
		return false;
	}

	TiXmlNode*	ChildNode = Node->FirstChild();
	while(ChildNode != NULL)
	{
		//	소분류 얻기
		TiXmlNode*	AttrNode = ChildNode->FirstChild();
		while(AttrNode != NULL)
		{
			const TiXmlElement*	Elem = AttrNode->ToElement();
			if( Elem )
			{

				const char*	szName	= Elem->Attribute(XML_ATTR_NAME);
				int			nValue	= 0;
				Elem->Attribute(XML_ATTR_VALUE, &nValue);

				//	세부 설정 얻기
				if(strcmp(ChildNode->Value(), XML_ELEMENT_GAME) == 0)
				{
					if(strcmp(szName, "FULL_SCREEN") == 0)	
					{ 
						m_ConfigVal.FullMode = (nValue == 0)?(false):(true); 
					}
				}
				else if(strcmp(ChildNode->Value(), XML_ELEMENT_OPTION) == 0)
				{
					if(strcmp(szName, "GRAPHIC_SET") == 0)	
					{ 
						m_ConfigVal.GraphicMode = (nValue == 1)?(false):(true); 
					}				
				}
				else if(strcmp(ChildNode->Value(), XML_ELEMENT_SOUND) == 0)
				{
					if(strcmp(szName, "BGM_VOLUME") == 0)	
					{ 
						m_ConfigVal.BgmVolume = nValue; 
					}				
					else if(strcmp(szName, "EFFECT_VOLUME") == 0)	
					{ 
						m_ConfigVal.SeVolume = nValue; 
					}				
					else if(strcmp(szName, "MUTE_BGM_VOLUME") == 0)	
					{ 
						m_ConfigVal.BgmMute = (nValue == 0)?(false):(true); 
					}				
					else if(strcmp(szName, "MUTE_EFFECT_VOLUME") == 0)
					{ 
						m_ConfigVal.SeMute = (nValue == 0)?(false):(true); 
					}				
				}
			}

			AttrNode = AttrNode->NextSibling();
		}

		ChildNode = ChildNode->NextSibling();
	}

	return	true;
}

//	저장하는거
bool CConfigXml::Save()
{
	SetDocOptionVal();

	if( !m_DocConfig )
	{
		return false;
	}

	if(!m_DocConfig->SaveFile())
	{
		return	false;
	}

	return	true;
}

//	저장전에 업데이트
bool CConfigXml::SetDocOptionVal() const
{
	if( !m_DocConfig )
	{
		return false;
	}

	//	대분류 얻기
	TiXmlNode*	Node = m_DocConfig->FirstChild(XML_GROUP_CONFIG);
	if( !Node )
	{
		return false;
	}

	TiXmlNode*	ChildNode = Node->FirstChild();
	while(ChildNode != NULL)
	{
		//	소분류 얻기
		TiXmlNode*	AttrNode = ChildNode->FirstChild();
		while(AttrNode != NULL)
		{
			TiXmlElement*	Elem = dynamic_cast<TiXmlElement*>(AttrNode->ToElement());

			if( Elem )
			{
				const char*	szName	= Elem->Attribute(XML_ATTR_NAME);
				int			nValue	= 0;
				Elem->Attribute(XML_ATTR_VALUE, &nValue);

				//	세부 설정 저장
				if(strcmp(ChildNode->Value(), XML_ELEMENT_GAME) == 0)
				{
					if(strcmp(szName, "FULL_SCREEN") == 0)
					{
						nValue	= ((m_ConfigVal.FullMode == true)?(1):(0));
						Elem->SetAttribute(XML_ATTR_VALUE, nValue); 
					}
				}
				else if(strcmp(ChildNode->Value(), XML_ELEMENT_OPTION) == 0)
				{
					if(strcmp(szName, "GRAPHIC_SET") == 0)	
					{ 
						nValue	= ((m_ConfigVal.GraphicMode == true)?(2):(1));
						Elem->SetAttribute(XML_ATTR_VALUE, nValue);
					}				
				}
				else if(strcmp(ChildNode->Value(), XML_ELEMENT_SOUND) == 0)
				{
					if(strcmp(szName, "BGM_VOLUME") == 0)	
					{ 
						Elem->SetAttribute(XML_ATTR_VALUE, m_ConfigVal.BgmVolume);
					}				
					if(strcmp(szName, "EFFECT_VOLUME") == 0)	
					{ 
						Elem->SetAttribute(XML_ATTR_VALUE, m_ConfigVal.SeVolume);
					}				
					if(strcmp(szName, "MUTE_BGM_VOLUME") == 0)	
					{ 
						nValue	= ((m_ConfigVal.BgmMute == true)?(1):(0));
						Elem->SetAttribute(XML_ATTR_VALUE, nValue);
					}				
					if(strcmp(szName, "MUTE_EFFECT_VOLUME") == 0)	
					{ 
						nValue	= ((m_ConfigVal.SeMute == true)?(1):(0));
						Elem->SetAttribute(XML_ATTR_VALUE, nValue);
					}				
				}
			}
			AttrNode = AttrNode->NextSibling();
		}

		ChildNode = ChildNode->NextSibling();
	}

	return	true;
}
