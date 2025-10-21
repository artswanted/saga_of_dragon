#include "stdafx.h"
#include "PgSkillLinkageInfo.h"
int const iFirstLv = 1;
int const iMaxLv = 10;

PgSkillLinkageInfo::PgSkillLinkageInfo()
{
}

PgSkillLinkageInfo::~PgSkillLinkageInfo()
{
}

void PgSkillLinkageInfo::Release()
{
	m_kCont.clear();
}

bool PgSkillLinkageInfo::Build()
{
	return ParseXml(L"XML/SkillLinkage.xml");
}

bool PgSkillLinkageInfo::ParseXml(std::wstring const &strXmlPath)
{
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Linkage Skill Info Parsing Start");

	TiXmlDocument kXmlDoc(MB(strXmlPath.c_str()));

	if(!kXmlDoc.LoadFile())
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Failed parse xml ["<<strXmlPath<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlElement* pkNode = kXmlDoc.FirstChildElement("SKILL_LINKAGE_INFO");

	pkNode = pkNode->FirstChildElement();
	while(pkNode)
	{
		char const* szName = pkNode->Value();
		if(0 == strcmp(szName, "INFO"))
		{
			int iSkillNo = 0;
			SInfo kInfo;

			const TiXmlAttribute* pkAttr = pkNode->FirstAttribute();
			
			while(pkAttr)
			{
				char const* pcAttrName = pkAttr->Name();
				char const* pcAttrValue = pkAttr->Value();

				if(0 == _stricmp(pcAttrName, "SKILL_NO"))
				{
					iSkillNo = atoi(pcAttrValue);
				}
				pkAttr = pkAttr->Next();
			}
			
			TiXmlNode* pkChildNode =  pkNode->FirstChild();
			while(pkChildNode)
			{	
				if(0 == ::strcmp(pkChildNode->Value(), "LINKAGE_EFFECT_NO"))
				{
					TiXmlElement const* pkElem = pkChildNode->ToElement();
					if(!pkElem) { break; }	
					
					int iEffectNo = 0;
					BM::vstring kArrtName;
					int iLv=iFirstLv;
					while(iLv <= iMaxLv)
					{
						kArrtName = "LV";
						kArrtName += iLv;
						//char const* szAttrName = pkElem->Attribute( static_cast<std::string>(kArrtName).c_str() );
						pkElem->Attribute( static_cast<std::string>(kArrtName).c_str(), &iEffectNo );
						if(0 >= iEffectNo) { break; }	// LV1 LV2 ... LV10까지 중 중간에 하나라도 없다면 더이상 없는것으로 간주
						kInfo.AddInfo(iLv, iEffectNo, iSkillNo);
						//AddInfo(iSkillNo, iLv, iEffectNo);
						 ++iLv;
					}
					// 단순히 LV가 몇개가 되는지로 MaxLv를 설정하고
					kInfo.SetMaxLv(iLv-1);
				}
				pkChildNode = pkChildNode->NextSibling();
			}

			// 얻은 정보를 추가
			AddInfo(iSkillNo, kInfo);
		}
		pkNode = pkNode->NextSiblingElement();
	}
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Linkage Skill Info Parsing End");
	return true;
}

bool PgSkillLinkageInfo::CheckAbil(int const iSkillNo, int const iLinkageLevel) const
{
	int iFindKey = iSkillNo;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iFindKey);
	if(pkSkill)
	{// 부모 스킬이 Key
		iFindKey = pkSkill->GetAbil(AT_PARENT_SKILL_NUM);
	}

	CONT_LINKAGE_SKILL_INFO::const_iterator kItor = m_kCont.find(iFindKey);
	if(m_kCont.end() == kItor)
	{
		return false;
	}
	if( (*kItor).second.GetMaxLv() < iLinkageLevel)
	{
		return false;
	}
	return true;
}

bool PgSkillLinkageInfo::GetInfo(int const iSkillNo, int const iLinkageLv, int& iLinkageEffectNo_out) const
{// iSkillNo를 사용할때, 연계레벨(iLinkageLv)에 해당하는 EfffectNo(iLinkageEffectNo_out)를 얻어옴
	int iFindKey = iSkillNo;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iFindKey);
	if(pkSkill)
	{// 부모 스킬이 Key
		iFindKey = pkSkill->GetAbil(AT_PARENT_SKILL_NUM);
	}

	CONT_LINKAGE_SKILL_INFO::const_iterator kItor = m_kCont.find(iFindKey);
	if(m_kCont.end() == kItor)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillLinkageInfo["<<iFindKey<<L"]");
		return false;
	}

	//if( (*kItor).second.iMaxLv < iLinkageLv )
	//{
	//	INFO_LOG(BM::LOG_LV0, __FL__<<L"LinkageLv["<<iLinkageLv<<L"] is Lager then MaxLinkageLevel["<<(*kItor).second.iMaxLv<<L"]");
	//	return false;
	//}

	//CONT_LINKAGE_EFFECTNO const& rkElemCont = (*kItor).second.kContEachLinkageLvEffectNo;
	//CONT_LINKAGE_EFFECTNO::const_iterator kElemItor = rkElemCont.find(iLinkageLv);
	//if(rkElemCont.end() == kElemItor)
	//{
	//	INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillLinkageInfo["<<iFindKey<<L"] LinkageLv"<<iLinkageLv);
	//	return false;
	//}
	//iLinkageEffectNo_out = (*kElemItor).second;

	//return true;
	return (*kItor).second.GetInfo(iLinkageLv, iLinkageEffectNo_out, iFindKey);
}

void PgSkillLinkageInfo::AddInfo(int const iSkillNo, int const iLinkageLv, int const iMaxLinkageLv, int const iLinkageEffectNo)
{// iSKillNo가 사용될때, 증가된 iLnkageLevel에 해당하는 iLinkageEffectNo
	//
	CONT_LINKAGE_SKILL_INFO::iterator kItor = m_kCont.find(iSkillNo);
	if(m_kCont.end() != kItor)
	{// 이미 존재하면, 기존 정보에 추가해 넣어주고
		//CONT_LINKAGE_EFFECTNO& rkElemCont = (*kItor).second.kContEachLinkageLvEffectNo;
		//auto kRet = rkElemCont.insert( std::make_pair(iLinkageLv, iLinkageEffectNo) );
		//if(!kRet.second)
		//{// 레벨이 겹치는 경우는 메세지
		//	INFO_LOG(BM::LOG_LV9, __FL__<<L"Dupliate SkillLinkageInfo["<<iSkillNo<<"L] LinkageLv"<<iLinkageLv);
		//}
		(*kItor).second.SetMaxLv(iMaxLinkageLv);
		(*kItor).second.AddInfo(iLinkageLv, iLinkageEffectNo, iSkillNo);
	}
	else
	{// 존재하지 않는다면, 새로 만들어 추가해준다
		//CONT_LINKAGE_EFFECTNO& rkElemCont = kInfo.m_kContEachLinkageLvEffectNo;
		//rkElemCont.insert( std::make_pair(iLinkageLv, iLinkageEffectNo) );
		SInfo kInfo(iMaxLinkageLv);
		kInfo.AddInfo(iLinkageLv, iLinkageEffectNo, iSkillNo);
		//m_kCont.insert( std::make_pair(iSkillNo , kInfo) );
		AddInfo(iSkillNo, kInfo);
	}
}

void PgSkillLinkageInfo::AddInfo(int const iSkillNo, SInfo const& rkInfo)
{
	auto kRet = m_kCont.insert( std::make_pair(iSkillNo , rkInfo) );
	if(!kRet.second)
	{// 레벨이 겹치는 경우는 메세지
		INFO_LOG(BM::LOG_LV9, __FL__<<L"Dupliate SkillLinkageInfo["<<iSkillNo<<L"] LinkageLv"<<rkInfo.GetMaxLv());
	}
}

void PgSkillLinkageInfo::SInfo::AddInfo(int const iLinkageLv, int const iLinkageEffectNo, int const iSkillNo)
{
	auto kRet = m_kContEachLinkageLvEffectNo.insert( std::make_pair(iLinkageLv, iLinkageEffectNo) );
	if(!kRet.second)
	{// 레벨이 겹치는 경우는 메세지
		INFO_LOG(BM::LOG_LV9, __FL__<<L"Dupliate SkillLinkageInfo["<<iSkillNo<<"L] LinkageLv"<<iLinkageLv<<" Insert Failed");
	}
}

bool PgSkillLinkageInfo::SInfo::GetInfo(int const iLinkageLv, int& iLinkageEffectNo_out, int const iSkillNo) const
{
	if( GetMaxLv() < iLinkageLv )
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"LinkageLv["<<iLinkageLv<<L"] is Lager then MaxLinkageLevel["<< GetMaxLv()<<L"]");
		return false;
	}

	CONT_LINKAGE_EFFECTNO::const_iterator kElemItor = m_kContEachLinkageLvEffectNo.find(iLinkageLv);
	if(m_kContEachLinkageLvEffectNo.end() == kElemItor)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot Find SkillLinkageInfo["<<iSkillNo<<L"] LinkageLv"<<iLinkageLv);
		return false;
	}
	iLinkageEffectNo_out = (*kElemItor).second;
	return true;
}