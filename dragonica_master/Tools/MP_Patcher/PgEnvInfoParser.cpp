#include "defines.h"
#include "PgEnvInfoParser.h"

bool PgEnvInfoParser::ParseFromPack(std::wstring const& kFile, SEnvData& kData_out)
{
	TiXmlDocument kXmlDoc;
	std::vector<char>	kData;
	if(!g_PProcess.LoadRes(kFile, kData))
	{
		WinMessageBox(NULL,L"Parsing failed for Res.dat",L"LoadFromPack", MB_ICONERROR);		
		return false;
	}

	if(kData.empty())
	{
		WinMessageBox(NULL, L"Parsing failed for Res.dat", L"LoadFromPack", MB_ICONERROR);
		return false;
	}

	size_t DataSize = kData.size();
	kData.push_back('\0');	// 파싱이 제대로 끝나는 것을 보장하기 위해서.

	kXmlDoc.Parse(&kData.at(0));

	if(kXmlDoc.Error())
	{
		WinMessageBox(NULL,L"Cannot load packed xml in Res.dat", L"PgEnvInfoParser::ParseFromPack",MB_ICONERROR);
	}
	TiXmlNode const* pkNode = &kXmlDoc;
	bool const bRet = Parse(pkNode, kData_out);
	return bRet;
}

bool PgEnvInfoParser::Parse(TiXmlNode const* pkNode, SEnvData& kData_out)
{	
	while(pkNode)
	{
		int const iType = pkNode->Type();
		switch(iType)
		{
		case TiXmlNode::DECLARATION:
			{				
				//TiXmlDeclaration  const* pDecl = dynamic_cast<TiXmlDeclaration const*>(pkNode);
				//if( pDecl )
				//{
				//	SetLocaleAccordingToXMLEncode(pDecl->Encoding());
				//}
			}break;
		case TiXmlNode::DOCUMENT:
			{
				TiXmlNode const* pChildNode = pkNode->FirstChild();
				if(pChildNode)
				{
					if(!Parse(pChildNode, kData_out))
					{
						return false;
					}
				}
			}break;
		case TiXmlNode::ELEMENT:
			{
				TiXmlNode* pkTemp = const_cast<TiXmlNode*>(pkNode);
				TiXmlElement* pElement = static_cast<TiXmlElement*>(pkTemp);
				if(!pElement )
				{
					return false;
				}
				char const* pcTagName = pElement->Value();
				TiXmlAttribute const *pAttr = pElement->FirstAttribute();
				if(0 == ::strcmp(pcTagName, "ENVIROMENT_SETTING"))
				{
					if(!Parse(pkNode->FirstChild(), kData_out))
					{
						return false;
					}
				}
				else if(0 == ::strcmp(pcTagName, "MAIN_BG"))
				{// 메인 배경 정보를 찾았다면
									
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "IMG"))
						{// 이미지 이름을 저장하고
							kData_out.kBgImg = CUR_DIR_MARK+UNI(pcAttrValue);
						}
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "SUB_BG"))
				{// 메인 배경 정보를 찾았다면
									
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "IMG"))
						{// 이미지 이름을 저장하고
							kData_out.kSubBgImg = CUR_DIR_MARK+UNI(pcAttrValue);
						}
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "MSG"))
				{
					
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "X"))
						{
							kData_out.kMainMsgInfo.kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{
							kData_out.kMainMsgInfo.kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TEXT_COLOR"))
						{// 글자 색을 저장한다
							DWORD dwHexValue = 0;
							::sscanf_s(pcAttrValue, "%x", &kData_out.kMainMsgInfo.kTextColor);
							kData_out.kMainMsgInfo.kTextColor = RGBtoBGR(kData_out.kMainMsgInfo.kTextColor);
						}
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "VERSION"))
				{
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "X"))
						{
							kData_out.kVerMsgInfo.kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{
							kData_out.kVerMsgInfo.kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TEXT_COLOR"))
						{// 글자 색을 저장한다
							DWORD dwHexValue = 0;
							::sscanf_s(pcAttrValue, "%x", &kData_out.kVerMsgInfo.kTextColor);
							kData_out.kVerMsgInfo.kTextColor = RGBtoBGR(kData_out.kVerMsgInfo.kTextColor);
						}
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "PROGRASS1")
						|| 0 == ::strcmp(pcTagName, "PROGRASS2")
					)
				{// 프로그래스 이미지를 찾았다면					
					SPrograssBarInfo* pkPrograssInfo=NULL;
					if(0 == ::strcmp(pcTagName, "PROGRASS1"))
					{
						pkPrograssInfo = &kData_out.kProgressImg1;
					}
					else
					{
						pkPrograssInfo = &kData_out.kProgressImg2;
					}
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "IMG"))
						{// 이미지 이름을 저장하고							
							pkPrograssInfo->kImg = CUR_DIR_MARK+UNI(pcAttrValue);							
						}
						else if(0 == ::strcmp(pcAttrName, "X"))
						{// 위치값을 저장한다 
							pkPrograssInfo->kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{// 위치값을 저장한다 
							pkPrograssInfo->kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TEXT_COLOR"))
						{// 글자 색을 저장한다
							DWORD dwHexValue = 0;
							::sscanf_s(pcAttrValue, "%x", &pkPrograssInfo->kTextColor);
							pkPrograssInfo->kTextColor = RGBtoBGR(pkPrograssInfo->kTextColor);
						}						
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "BROWSER"))
				{// 브라우저 정보를 찾았다면					
					SBRInfo& rkBRInfo = kData_out.kBRInfo;
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						if(0 == ::strcmp(pcAttrName, "X"))
						{// 위치X,Y 그리고
							rkBRInfo.kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{
							rkBRInfo.kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "WIDTH"))
						{// 가로 세로 폭을 저장한다
							rkBRInfo.iWidth = ::atoi(pcAttrValue);					
						}
						else if(0 == ::strcmp(pcAttrName, "HEIGHT"))
						{
							rkBRInfo.iHeight = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "LINK"))
						{
							rkBRInfo.kLink = UNI(pcAttrValue);
						}
						pAttr = pAttr->Next();
					}
				}
				else if(0 == ::strcmp(pcTagName, "EDIT"))
				{
					SEditInfo kEditInfo;
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						
						if(0 == ::strcmp(pcAttrName, "ID"))
						{// ID
							kEditInfo.iID = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TYPE"))
						{// 버튼의 타입
							kEditInfo.iType = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "X"))
						{// 위치값
							kEditInfo.kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{
							kEditInfo.kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "W"))
						{// 위치값
							kEditInfo.kSize.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "H"))
						{
							kEditInfo.kSize.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "BG_COLOR"))
						{// 위치값
							sscanf(pcAttrValue, "%d,%d,%d", &kEditInfo.BGR, &kEditInfo.BGG, &kEditInfo.BGB);
						}
						else if(0 == ::strcmp(pcAttrName, "TEXT_COLOR"))
						{
							sscanf(pcAttrValue, "%d,%d,%d", &kEditInfo.TB, &kEditInfo.TG, &kEditInfo.TR);
						}
						else if(0 == ::strcmp(pcAttrName, "LIMIT_LEN"))
						{
							kEditInfo.iLimitLen = static_cast<DWORD>(::atoi(pcAttrValue));
						}
						pAttr = pAttr->Next();

					}
					//등록하고
					SEnvData::CONT_EDITINFO& rkContEditInfo = kData_out.kContEditInfo;
					auto ret = rkContEditInfo.insert(std::make_pair(kEditInfo.iID, kEditInfo));
					if(!ret.second)
					{//동일한 ID가 들어왔다면 알린다.
						WinMessageBox(NULL, L"Button with the same ID has been detected in packed xml in Res.dat. Please modify it with other ID", L"PgEnvInfoParser::Parse", MB_ICONERROR);
					}					
				}
				else if(0 == ::strcmp(pcTagName, "BTN"))
				{// 버튼 정보를 찾았다면
									
					SBtnInfo kBtnInfo;
					while(pAttr)
					{
						char const* pcAttrName = pAttr->Name();
						char const* pcAttrValue = pAttr->Value();
						
						if(0 == ::strcmp(pcAttrName, "ID"))
						{// ID
							kBtnInfo.iID = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TYPE"))
						{// 버튼의 타입
							kBtnInfo.iType = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "X"))
						{// 위치값
							kBtnInfo.kPos.x = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "Y"))
						{
							kBtnInfo.kPos.y = ::atoi(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "IMG"))
						{// Image경로
							kBtnInfo.kImg = CUR_DIR_MARK+UNI(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "STATE_CNT"))
						{// 한 Image에 들어있는 버튼 모양 갯수
							kBtnInfo.iShapeNum = ::atoi(pcAttrValue);
						}						
						else if(0 == ::strcmp(pcAttrName, "LINK"))
						{// 버튼이 링크될 주소
							kBtnInfo.kLink = UNI(pcAttrValue);
						}
						else if(0 == ::strcmp(pcAttrName, "TEXT"))
						{// 버튼의 텍스트를
							kBtnInfo.kText = UNI(pcAttrValue);							
						}
						pAttr = pAttr->Next();
					}
					//등록하고
					SEnvData::CONT_BTNINFO& rkContBtnInfo = kData_out.kContBtnInfo;
					auto ret = rkContBtnInfo.insert(std::make_pair(kBtnInfo.iID, kBtnInfo));
					if(!ret.second)
					{//동일한 ID가 들어왔다면 알린다.
						WinMessageBox(NULL, L"Button with the same ID has been detected in packed xml in Res.dat. Please modify it with other ID", L"PgEnvInfoParser::Parse", MB_ICONERROR);
					}
				}
				else if(0 == ::strcmp(pcTagName, "SITECONTROL1"))
				{
					g_kSiteControl1.ParseXML(g_WinApp->GetHandle(), g_WinApp->GetInstance(), pElement->FirstChildElement());
				}
				else if(0 == ::strcmp(pcTagName, "SITECONTROL2"))
				{
					g_kSiteControl2.ParseXML(g_WinApp->GetHandle(), g_WinApp->GetInstance(), pElement->FirstChildElement());
				}
			}
			default:
				{
				}break;
		}
		pkNode = pkNode->NextSibling();
	}
	return true;
}

DWORD PgEnvInfoParser::RGBtoBGR(DWORD const& rdwRGB)
{
	DWORD dwB = (rdwRGB&0x0000FF)<<16;
	DWORD dwG = (rdwRGB&0x00FF00);
	DWORD dwR = (rdwRGB&0xFF0000)>>16;
	DWORD dwColor = (dwR|dwG|dwB);
	return dwColor;
}
