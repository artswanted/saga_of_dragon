#include "stdafx.h"
//#include "Global.h"
#include "PgStringUtil.h"
#include "PgNpcEvent.h"
#include "PgPlayer.h"
#include "PgQuestInfo.h"
#include "PgMyQuest.h"
#include "Inventory.h"

namespace PgNpcEventUtil
{
	// enum
	std::string const kNpcEventElement( "NPC_EVENT" );
	std::string const kConditionElement( "CONDITION" );
	std::string const kActivateActionElement( "ACTIVATE_ACTION" );
	std::string const kDeactivateActionElement( "DEACTIVATE_ACTION" );	
}

bool PgNpcEventCondition::Parse(TiXmlElement const* pkRoot)
{
	return false;
}

bool PgNpcEventCondition::CommonParse(std::string const& kName, std::string const& kValue)
{
	if( kName == "BINDING" )
	{
		if(kValue == "AND")
		{
			m_eOperator = NEO_AND;
		}
		else if(kValue == "OR")
		{
			m_eOperator = NEO_OR;
		}
		return true;
	}
	return false;
}

bool PgNpcEventCondition::Check(CUnit * pkUnit)
{
	return false;
}

bool PgNpcEventCondition_Quest::Parse(TiXmlElement const* pkNode)
{
	using namespace PgNpcEventUtil;

	if(!pkNode)
	{
		return false;
	}

	TiXmlAttribute const* pkAttribute = pkNode->FirstAttribute();
	while( pkAttribute )
	{
		std::string const kName(pkAttribute->Name() ? pkAttribute->Name() : "");
		std::string const kValue(pkAttribute->Value() ? pkAttribute->Value() : "");
		
		if( kName == "QUEST_NO")
		{
			m_iQuestNo = PgStringUtil::SafeAtoi(kValue);
		}
		else if( kName == "STATE" )
		{
			if(kValue == "ACCEPT")
			{
				m_eState = NEQS_ACCEPT;
			}
			else if(kValue == "COMPLETE")
			{
				m_eState = NEQS_COMPLETE;
			}
			else
			{
				m_eState = NEQS_NOT_ACCEPT;
			}
		}
		else
		{
			return CommonParse(kName, kValue);
		}

		pkAttribute = pkAttribute->Next();
	}
	return true;
}

bool PgNpcEventCondition_Quest::Check(CUnit * pkUnit)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		return false;
	}
	SUserQuestState const* pkState = pkPlayer->GetQuestState(m_iQuestNo);

	switch(m_eState)
	{
	case NEQS_NOT_ACCEPT:
		{
			return NULL==pkState;
		}break;
	case NEQS_ACCEPT:
		{
			return pkState && (QS_Ing==pkState->byQuestState);
		}break;
	case NEQS_COMPLETE:
		{
			return pkPlayer->GetMyQuest()->IsEndedQuest(m_iQuestNo);
		}break;
	}
	return false;
}

bool PgNpcEventCondition_Item::Parse(TiXmlElement const* pkNode)
{
	using namespace PgNpcEventUtil;

	if(!pkNode)
	{
		return false;
	}

	TiXmlAttribute const* pkAttribute = pkNode->FirstAttribute();
	while( pkAttribute )
	{
		std::string const kName(pkAttribute->Name() ? pkAttribute->Name() : "");
		std::string const kValue(pkAttribute->Value() ? pkAttribute->Value() : "");

		if( kName == "ITEM_NO")
		{
			m_iItemNo = PgStringUtil::SafeAtoi(kValue);
		}
		else if( kName == "IS_HAVE")
		{
			m_bIsHave = !(kValue == "FALSE");
		}
		else if( kName == "TYPE" )
		{
			if(kValue == "EQUIP")
			{
				m_ePosType = NEIE_EQUIP_SLOT;
			}
			else if(kValue == "SAFE")
			{
				m_ePosType = NEIE_SAFE;
			}
			else if(kValue == "SHARE_SAFE")
			{
				m_ePosType = NEIE_SHARE_RENTAL_SAFE;
			}
			else
			{
				m_ePosType = NEIE_INVENTORY;
			}
		}
		else
		{
			return CommonParse(kName, kValue);
		}

		pkAttribute = pkAttribute->Next();
	}
	return true;
}

bool PgNpcEventCondition_Item::Check(CUnit * pkUnit)
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		return false;
	}

	PgInventory * pkInv = pkPlayer->GetInven();
	if( !pkInv )
	{
		return false;
	}

	switch(m_ePosType)
	{
	case NEIE_INVENTORY:
		{
			SItemPos kRetPos;
			HRESULT const kResult = pkInv->GetFirstItem(m_iItemNo, kRetPos, false, true);
			if(m_bIsHave)
			{
				return kResult==S_OK;
			}
			
			return !(kResult==S_OK);
		}break;
	case NEIE_EQUIP_SLOT:
		{
			SItemPos kRetPos;
			if(S_OK != pkInv->GetFirstItem(IT_FIT, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_FIT_CASH, m_iItemNo, kRetPos, true))
			{
				//금고에 없는 것임
				return false==m_bIsHave;
			}

			return m_bIsHave;
		}break;
	case NEIE_SAFE:
		{
			SItemPos kRetPos;
			if(S_OK != pkInv->GetFirstItem(IT_SAFE, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_RENTAL_SAFE1, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_RENTAL_SAFE2, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_RENTAL_SAFE3, m_iItemNo, kRetPos, true))
			{
				//금고에 없는 것임
				return false==m_bIsHave;
			}

			return m_bIsHave;
		}break;
	case NEIE_SHARE_RENTAL_SAFE:
		{
			SItemPos kRetPos;
			if(S_OK != pkInv->GetFirstItem(IT_SHARE_RENTAL_SAFE1, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_SHARE_RENTAL_SAFE2, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_SHARE_RENTAL_SAFE3, m_iItemNo, kRetPos, true))
			if(S_OK != pkInv->GetFirstItem(IT_SHARE_RENTAL_SAFE4, m_iItemNo, kRetPos, true))
			{
				//금고에 없는 것임
				return false==m_bIsHave;
			}

			return m_bIsHave;
		}break;
	}
	return false;
}

bool PgNpcEventCondition_Effect::Parse(TiXmlElement const* pkNode)
{
	using namespace PgNpcEventUtil;

	if(!pkNode)
	{
		return false;
	}

	TiXmlAttribute const* pkAttribute = pkNode->FirstAttribute();
	while( pkAttribute )
	{
		std::string const kName(pkAttribute->Name() ? pkAttribute->Name() : "");
		std::string const kValue(pkAttribute->Value() ? pkAttribute->Value() : "");

		if( kName == "EFFECT_NO")
		{
			m_iEffectNo = PgStringUtil::SafeAtoi(kValue);
		}
		else if( kName == "IS_HAVE" )
		{
			m_bIsHave = !(kValue == "FALSE");
		}
		else
		{
			return CommonParse(kName, kValue);
		}

		pkAttribute = pkAttribute->Next();
	}
	return true;
}

bool PgNpcEventCondition_Effect::Check(CUnit * pkUnit)
{
	if(!pkUnit)
	{
		return false;
	}

	CEffect const* pkEffect = pkUnit->GetEffect(m_iEffectNo);
	if(m_bIsHave)
	{
		return pkEffect;
	}

	return NULL==pkEffect;
}

//
PgNpcEvent::PgNpcEvent(WORD const& rkID)
	: m_kID(rkID)
{
}

PgNpcEvent::~PgNpcEvent()
{
}

PgNpcEvent::PgNpcEvent(PgNpcEvent const& rhs)
	: m_kID(rhs.m_kID), m_kOrCondition(rhs.m_kOrCondition)
	, m_kActivateAction(rhs.m_kActivateAction), m_kDeactivateAction(rhs.m_kDeactivateAction)
{
}

void PgNpcEvent::operator =(PgNpcEvent const& rhs)
{
	m_kID = rhs.m_kID;
	m_kOrCondition = rhs.m_kOrCondition;
	m_kActivateAction = rhs.m_kActivateAction;
	m_kDeactivateAction = rhs.m_kDeactivateAction;
}

bool PgNpcEvent::Parse(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	if( PgNpcEventUtil::kNpcEventElement != pkRoot->Value() )
	{
		return false;
	}

	TiXmlElement const* pkNode = pkRoot->FirstChildElement();
	while( pkNode )
	{
		char const* szElementName = pkNode->Value();
		if( !szElementName )
		{
			return false;
		}

		if( PgNpcEventUtil::kConditionElement == szElementName ) // CONDITION
		{
			if( !ParseCondition(pkNode) )
			{
				return false;
			}
		}
		else if( PgNpcEventUtil::kActivateActionElement == szElementName )
		{
			if( !ParseAction(pkNode, m_kActivateAction) )
			{
				return false;
			}
		}
		else if( PgNpcEventUtil::kDeactivateActionElement == szElementName )
		{
			if( !ParseAction(pkNode, m_kDeactivateAction) )
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		pkNode = pkNode->NextSiblingElement();
	}

	if( m_kOrCondition.empty() )
	{
		return false;
	}

	return true;
}

bool PgNpcEvent::ParseCondition(TiXmlElement const* pkNode)
{
	if(!pkNode)
	{
		return false;
	}

	CONT_NPC_EVENT_AND_CONDITION kContAndCondition;

	SP_NpcEventCondition spNpcEventCondition;
	TiXmlElement const* pkConditionNode = pkNode->FirstChildElement();
	while( pkConditionNode )
	{
		if( !pkConditionNode->Value() )
		{
			return false;
		}
		std::string const kElementName(pkConditionNode->Value());

		if( kElementName == "QUEST" )
		{
			SP_NpcEventCondition spQuest(new PgNpcEventCondition_Quest);
			if( spQuest->Parse(pkConditionNode) )
			{
				spNpcEventCondition = spQuest;
			}
		}
		else if( kElementName == "ITEM" )
		{
			SP_NpcEventCondition spItem(new PgNpcEventCondition_Item);
			if( spItem->Parse(pkConditionNode) )
			{
				spNpcEventCondition = spItem;
			}
		}
		else if( kElementName == "EFFECT" )
		{
			SP_NpcEventCondition spEffect(new PgNpcEventCondition_Effect);
			if( spEffect->Parse(pkConditionNode) )
			{
				spNpcEventCondition = spEffect;
			}
		}
		else
		{
			return false;
		}

		if(spNpcEventCondition)
		{
			kContAndCondition.push_back(spNpcEventCondition);			
		}
		
		pkConditionNode = pkConditionNode->NextSiblingElement();
	}

	m_kOrCondition.push_back(kContAndCondition);

	return true;
}

bool PgNpcEvent::ParseAction(TiXmlElement const* pkNode, CONT_NPC_EVENT_ACTION& rkAction)
{
	if( !pkNode )
	{
		return false;
	}

	TiXmlElement const* pkActionNode = pkNode->FirstChildElement();
	while( pkActionNode )
	{
		std::string const kElementName(pkActionNode->Value() ? pkActionNode->Value() : "");
		if( kElementName == "NPC" )
		{
			BM::GUID kNpcGuid;
			bool bHidden = false;
			TiXmlAttribute const* pkAttribute = pkActionNode->FirstAttribute();
			while( pkAttribute )
			{
				std::string const kName(pkAttribute->Name() ? pkAttribute->Name() : "");
				char const* szValue = pkAttribute->Value();

				if( kName == "NPC_GUID" )
				{
					if(szValue)
					{
						kNpcGuid.Set(szValue);
					}
				}
				else if( kName == "HIDDEN" )
				{
					bHidden = (stricmp(szValue,"TRUE")==0);
				}

				pkAttribute = pkAttribute->Next();
			}

			rkAction.push_back(SNpcEventAction(kNpcGuid, bHidden));
		}

		pkActionNode = pkActionNode->NextSiblingElement();
	}

	return true;
}

bool PgNpcEvent::CheckCondition(CUnit * pkUnit)
{
	for(CONT_NPC_EVENT_OR_CONDITION::iterator or_it=m_kOrCondition.begin(); or_it!=m_kOrCondition.end(); ++or_it)
	{
		int iCount = 0;
		for(CONT_NPC_EVENT_AND_CONDITION::iterator and_it=or_it->begin(); and_it!=or_it->end(); ++and_it)
		{
			if(false==(*and_it)->Check(pkUnit))
			{
				break;
			}
			++iCount;
		}

		if(iCount == or_it->size())
		{
			return true;
		}
	}
	return false;
}

PgNpcEventMgr::PgNpcEventMgr()
{
}

PgNpcEventMgr::~PgNpcEventMgr()
{
}

bool PgNpcEventMgr::Parse(TiXmlElement const* pkRoot, int const iMapNo)
{
	if( !pkRoot )
	{
		return false;
	}

	WORD kID = 0;
	TiXmlAttribute const* pkAttribute = pkRoot->FirstAttribute();
	while( pkAttribute )
	{
		std::string const kName(pkAttribute->Name() ? pkAttribute->Name() : "");

		if( kName == "ID" )
		{
			kID = PgStringUtil::SafeAtoi(pkAttribute->Value());
		}

		pkAttribute = pkAttribute->Next();
	}

	PgNpcEvent kEvent(kID);
	if( kEvent.Parse(pkRoot) )
	{
		auto ret = m_kContMapNpcEvent.insert(std::make_pair(iMapNo, CONT_NPC_EVENT()));
		auto ret_sec = ret.first->second.insert(std::make_pair(kID,kEvent));
	}
	return true;
}

bool PgNpcEventMgr::Check(int const iMapNo, CUnit * pkUnit, CONT_ACTIVATE_NPC_EVENT_ACTION & kContAction)
{
	CONT_MAP_NPC_EVENT::iterator map_it = m_kContMapNpcEvent.find(iMapNo);
	if(map_it!=m_kContMapNpcEvent.end())
	{
		CONT_MAP_NPC_EVENT::mapped_type::iterator event_it = (*map_it).second.begin();
		while(event_it != (*map_it).second.end())
		{
			if( true==(*event_it).second.CheckCondition(pkUnit) )
			{
				if(false==(*event_it).second.ActivateAction().empty())
				{
					kContAction.insert( std::make_pair((*event_it).second.ID(), &(*event_it).second.ActivateAction()) );
				}
			}
			else
			{
				if(false==(*event_it).second.DeactivateAction().empty())
				{
					kContAction.insert( std::make_pair((*event_it).second.ID(), &(*event_it).second.DeactivateAction()) );
				}
			}
			++event_it;
		}
		return true;
	}
	return false;
}

bool PgNpcEventMgr::IsHiddenNpc(int const iMapNo, BM::GUID const& rkNpcGuid, CUnit * pkUnit)
{
	CONT_ACTIVATE_NPC_EVENT_ACTION kContAction;
	Check(iMapNo, pkUnit, kContAction);
	return IsHiddenNpc(rkNpcGuid, kContAction);
}

bool PgNpcEventMgr::IsHiddenNpc(BM::GUID const& rkNpcGuid, CONT_ACTIVATE_NPC_EVENT_ACTION const& rkContAction)
{
	CONT_ACTIVATE_NPC_EVENT_ACTION::mapped_type pkAction = NULL;
	CONT_ACTIVATE_NPC_EVENT_ACTION::const_iterator activate_it = rkContAction.begin();
	while(activate_it != rkContAction.end())
	{
		if(pkAction = (*activate_it).second)
		{
			CONT_NPC_EVENT_ACTION::const_iterator action_it = pkAction->begin();
			while(action_it != pkAction->end())
			{
				if( rkNpcGuid == (*action_it).GetGuid() )
				{
					return (*action_it).IsHidden();
				}
				++action_it;
			}
		}

		++activate_it;
	}
	return false;
}
