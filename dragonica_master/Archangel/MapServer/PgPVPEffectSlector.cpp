#include "stdafx.h"
#include "PgPVPEffectSelector.h"

bool PgPVPEffectSelector::SData::Empty() const
{
	return kContEffectNo.empty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgPVPEffectSelector::PgPVPEffectSelector()
{
}

PgPVPEffectSelector::~PgPVPEffectSelector()
{
	Release();
}

bool PgPVPEffectSelector::Build()
{
	return ParseXml(L"XML/PVP_EFFECT.xml");
}

void PgPVPEffectSelector::Release()
{
	m_kPVPEffectCont.clear();
}

bool PgPVPEffectSelector::ParseXml(std::wstring const &kXmlPath)
{
	INFO_LOG(BM::LOG_LV9, __FL__<<L"PVP Effect Info Parsing Start");

	TiXmlDocument kXmlDoc(MB(kXmlPath.c_str()));

	if(!kXmlDoc.LoadFile())
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Failed parse xml ["<<kXmlPath<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlElement* pkNode = kXmlDoc.FirstChildElement("PVP_EFFECT");

	pkNode = pkNode->FirstChildElement();
	while(pkNode)
	{
		char const* szName = pkNode->Value();
		if(0 == strcmp(szName, "EFFECT"))
		{
			__int64 i64ClassLimit = 0;
			__int64 i64DraClassLimit = 0;
			int iEffectNo = 0;
			SData kData;

			TiXmlAttribute* pkAttr = pkNode->FirstAttribute();
			
			while(pkAttr)
			{
				char const* pcAttrName = pkAttr->Name();
				char const* pcAttrValue = pkAttr->Value();

				if(0 == _stricmp(pcAttrName, "ClassLimit"))
				{
					i64ClassLimit = atoi(pcAttrValue);
				}
				else if(0 == _stricmp(pcAttrName, "DraClassLimit"))
				{
					i64DraClassLimit = atoi(pcAttrValue);
				}
				else
				{
					//BM::vstring vStr;
					//for(int i=1; i <= 10; ++i)
					{
						//vStr = "EFFECT_NO";
						//vStr+=i;
						if(!pcAttrName)
						{
							break;
						}
						std::string const kTemp(pcAttrName);
						if( std::string::npos != kTemp.find("EFFECT_NO") ) //if( vStr == kTemp )
						{
							iEffectNo = atoi(pcAttrValue);
							if(iEffectNo)
							{
								kData.kContEffectNo.push_back(iEffectNo);
							}
						}
					}
				}
				pkAttr = pkAttr->Next();
			}
			
			if( !kData.Empty() )
			{
				kData.i64ClassLimit = i64ClassLimit | (i64DraClassLimit << DRAGONIAN_LSHIFT_VAL);
				m_kPVPEffectCont.push_back(kData);
			}
		}
		pkNode = pkNode->NextSiblingElement();
	}
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Linkage Skill Info Parsing End");
	return true;
}


bool PgPVPEffectSelector::AddEffect(CUnit* pkUnit, SActArg* pkArg) const
{
	if( m_kPVPEffectCont.empty()
		|| !pkUnit
		|| !pkArg // 반드시 Ground가 있어야 하
		)
	{
		return false;
	}
	
	for( CONT_PVP_EFFECT::const_iterator kItor = m_kPVPEffectCont.begin(); m_kPVPEffectCont.end() != kItor; ++kItor)
	{// 파싱한 PVP 버프 정보에서
		SData const& kData = (*kItor);
		if( IS_CLASS_LIMIT( kData.i64ClassLimit, pkUnit->GetAbil(AT_CLASS) ) )
		{// 해당 클래스가 맞으면
			for(CONT_INT::const_iterator kEffectItor = kData.kContEffectNo.begin(); kData.kContEffectNo.end() != kEffectItor; ++kEffectItor)
			{
				int const iEffectNo = (*kEffectItor);
				if(0 < iEffectNo)
				{// 버프를 걸어준다
					pkUnit->AddEffect(iEffectNo, 0, pkArg, NULL);
				}
			}
		}
	}
	return true;
}

bool PgPVPEffectSelector::DelEffect(CUnit* pkUnit) const
{
	if( m_kPVPEffectCont.empty()
		|| !pkUnit
		)
	{
		return false;
	}
	
	for( CONT_PVP_EFFECT::const_iterator kItor = m_kPVPEffectCont.begin(); m_kPVPEffectCont.end() != kItor; ++kItor)
	{// 파싱한 PVP 버프 정보에서
		SData const& kData = (*kItor);
		if( IS_CLASS_LIMIT( kData.i64ClassLimit, pkUnit->GetAbil(AT_CLASS) ) )
		{// 해당 클래스가 맞으면
			for(CONT_INT::const_iterator kEffectItor = kData.kContEffectNo.begin(); kData.kContEffectNo.end() != kEffectItor; ++kEffectItor)
			{
				int const iEffectNo = (*kEffectItor);
				if(0 < iEffectNo)
				{// 버프를 제거 한다
					pkUnit->DeleteEffect(iEffectNo);
				}
			}
		}
	}
	return true;
}
