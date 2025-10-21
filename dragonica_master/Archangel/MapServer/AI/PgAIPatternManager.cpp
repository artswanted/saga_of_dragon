#include "stdafx.h"
#include "PgAIPatternManager.h"

PgAIPatternManager::AIPATTERN_POOL PgAIPatternManager::m_kPatternPool(5,3);

PgAIPatternManager::PgAIPatternManager(void)
{
}

PgAIPatternManager::~PgAIPatternManager(void)
{
	Release();
}

bool PgAIPatternManager::Init()
{
	LoadAIPattern("./XML/Monster/AI_Pattern.xml");
	return true;
}

bool PgAIPatternManager::LoadAIPattern(char const* szPath)
{
	INFO_LOG(BM::LOG_LV9, __FL__<<L"Lua Call Function [MonsterAI_Pattern]");
	if (NULL == szPath)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Failed parse xml ["<<UNI(szPath)<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlDocument kDocu(szPath);
	if( !kDocu.LoadFile() )
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Failed LoadFile xml ["<<UNI(szPath)<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const TiXmlElement *pkElement = kDocu.FirstChildElement();
	if(strcmp(pkElement->Value(), "AI") != 0)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<< UNI(szPath) << L" isn't AI xml");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkElement = pkElement->FirstChildElement();

	while(pkElement)
	{
		char const *pcTagName = pkElement->Value();
		if(strcmp(pcTagName, "ACTION") == 0)
		{
			const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
			char const *pcID = 0;
			char const *pcName = 0;
			while(pkAttr)
			{
				char const *pcAttrName = pkAttr->Name();
				char const *pcAttrValue = pkAttr->Value();
				if(strcmp(pcAttrName, "ID") == 0)
				{
					pcID = pcAttrValue;
				}
				else if(strcmp(pcAttrName, "NAME") == 0)
				{
					pcName = pcAttrValue;
				}

				pkAttr = pkAttr->Next();
			}
		}
		else if(strcmp(pcTagName, "PATTERN") == 0)
		{
			TiXmlAttribute const *pkAttr = pkElement->FirstAttribute();

			char const *pcID = 0;
			char const *pcName = 0;
			while(pkAttr)
			{
				char const *pcAttrName = pkAttr->Name();
				char const *pcAttrValue = pkAttr->Value();
				if(strcmp(pcAttrName, "ID") == 0)
				{
					pcID = pcAttrValue;
				}
				else if(strcmp(pcAttrName, "NAME") == 0)
				{
					pcName = pcAttrValue;
				}

				pkAttr = pkAttr->Next();
			}

			if (!Add((int)BM::vstring(pcID), UNI((pcName))))
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			TiXmlElement const *pkChileElement = pkElement->FirstChildElement();
			while(pkChileElement)
			{	
				char const *pcTagChileNmae = pkChileElement->Value();
				if(strcmp(pcTagChileNmae, "ACT_CHANGE") == 0)
				{
					const TiXmlAttribute *pkAttr = pkChileElement->FirstAttribute();
					char const *pcFrom = 0;
					char const *pcTo = 0;
					char const *pcWeight = 0;
					while(pkAttr)
					{
						char const *pcChildAttrName = pkAttr->Name();
						char const *pcChildAttrValue = pkAttr->Value();
						if(strcmp(pcChildAttrName, "FROM") == 0)
						{
							pcFrom = pcChildAttrValue;
						}
						else if(strcmp(pcChildAttrName, "TO") == 0)
						{
							pcTo = pcChildAttrValue;
						}
						else if(strcmp(pcChildAttrName, "WEIGHT") == 0)
						{
							pcWeight = pcChildAttrValue;
						}

						pkAttr = pkAttr->Next();
					}
					if (!AddActTransit((int)BM::vstring(pcID), (int)BM::vstring(pcFrom), (int)BM::vstring(pcTo), (int)BM::vstring(pcWeight)))
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}
				pkChileElement = pkChileElement->NextSiblingElement();
			}
		}

		pkElement = pkElement->NextSiblingElement();
	}

	return true;
}

bool PgAIPatternManager::Add(short int sID, const wchar_t* pwszName)
{
	PgAIPattern* pkNew = GetPattern(sID);
	if (pkNew != NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkNew = m_kPatternPool.New();
	pkNew->SetInfo(sID, pwszName);

	auto kPair = m_kPattern.insert(std::make_pair(sID, pkNew));
	if ( !kPair.second )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("insert Failed ID=") << sID << _T(" Name=") << pwszName );
		m_kPatternPool.Delete(pkNew);
		return false;
	}
	return true;
}

PgAIPattern* PgAIPatternManager::GetPattern(short int sID)
{
	CONT_AI_PATTERN::iterator itor = m_kPattern.find(sID);
	if (itor != m_kPattern.end())
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

PgAIPattern const* PgAIPatternManager::GetPattern(short int sID) const
{
	CONT_AI_PATTERN::const_iterator itor = m_kPattern.find(sID);
	if (itor != m_kPattern.end())
	{
		return itor->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

bool PgAIPatternManager::IsPatternExist(short int const iAI_Type, int const iAction) const
{
	PgAIPattern const* pkPattern = GetPattern(iAI_Type);
	if(NULL==pkPattern)
	{
		return false;
	}

	return pkPattern->IsPatternExist(iAction);
}

bool PgAIPatternManager::AddActTransit(short int sID, int iFromAct, int iToAct, BYTE byWeight)
{
	PgAIPattern* pkNew = GetPattern(sID);
	if (pkNew == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot get Pattern ID["<<sID<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	pkNew->AddTransit(iFromAct, iToAct, byWeight);
	return true;
}

bool PgAIPatternManager::GetNextAction(short int sPattern, int iCurrentAction, PgAIPattern::VECTOR_PATTERN_ACTION_INFO const* &pkOutVector)const
{
	PgAIPattern const * pkPattern = GetPattern(sPattern);
	if (pkPattern == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Cannot get Pattern ID["<<sPattern<<L"]");
		//VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Cannot Get AI Pattern PatternNo[%hd]"), __FUNCTIONW__, sPattern);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return pkPattern->GetNextAction(iCurrentAction, pkOutVector);
}

void PgAIPatternManager::Release()
{
	CONT_AI_PATTERN::iterator itor = m_kPattern.begin();
	while (itor != m_kPattern.end())
	{
		if (itor->second != NULL)
		{
			itor->second->Release();
			m_kPatternPool.Delete(itor->second);
		}

		++itor;
	}
	m_kPattern.clear();
}