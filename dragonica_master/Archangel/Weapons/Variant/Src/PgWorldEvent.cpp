#include "stdafx.h"
#include "Global.h"
#include "PgStringUtil.h"
#include "PgWorldEvent.h"

namespace PgWorldEventUtil
{
	// enum
	std::string const kWorldEventElement( "WORLD_EVENT" );
	std::string const kConditionElement( "CONDITION" );
	std::string const kActivateActionElement( "ACTIVATE_ACTION" );
	std::string const kDeactivateActionElement( "DEACTIVATE_ACTION" );
	std::string const kActionElement( "ACTION" );
	std::string const kAttributeEnalbe( "ENABLE" );
	std::string const kAttributeTargetType( "TARGET_TYPE" );
	std::string const kAttributeBinding( "BINDING" );
	std::string const kAttributeTargetID( "TARGET_ID" );
	std::string const kAttributeEventType( "EVENT" );
	std::string const kAttributeEventValue( "VALUE" );


	//
	SErrorInfo::SErrorInfo(PgWorldEvent const* pkWorldEvent, wchar_t const* szFunc, size_t const iLine, BM::vstring const& rkErrorMsg, TiXmlElement const* pkNode)
		: m_szFunc(szFunc), m_iLine(iLine)
	{
		if( pkNode )
		{
			TiXmlDocument const* pkDocu = pkNode->GetDocument();
			std::string const kDocuName = (NULL != pkDocu)? pkDocu->Value(): std::string();

			m_kErrorMsg = BM::vstring(L"Docu[") << kDocuName << L"] Row[" << pkNode->Row() << L"] Column[" << pkNode->Column() << L"] Element[" << pkNode->Value() << L"] ";
		}
		
		if( pkWorldEvent )
		{
			m_kErrorMsg << L"WorldEvent ID[" << pkWorldEvent->ID() << L"] ";
		}
		m_kErrorMsg << rkErrorMsg;
	}

	SErrorInfo::SErrorInfo(SErrorInfo const& rhs)
		: m_kErrorMsg(rhs.m_kErrorMsg), m_szFunc(rhs.m_szFunc), m_iLine(rhs.m_iLine)
	{
	}

	//
	typedef std::list< SErrorInfo > ContErrorInfo;
	ContErrorInfo g_kError;


	//
	void AddError(SErrorInfo const& rkErrorInfo)
	{
		g_kError.push_back( rkErrorInfo );
	}

	void OutError(SErrorInfo const& rkErrorInfo)
	{
		CAUTION_LOG(BM::LOG_LV1, rkErrorInfo.m_kErrorMsg);
	}

	void Verify()
	{
	}

	bool DisplayResult()
	{
		if( !g_kError.empty() )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("------------------ WorldEvent Error Display Start ------------------"));
			std::for_each(g_kError.begin(), g_kError.end(), OutError);
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("------------------  WorldEvent Error Display End  ------------------"));

			g_kError.clear();
			return false; // Don't Pass
		}
		return true; // Pass
	}

	
	//
	template< typename _EnumType >
	struct SWorldEventParsedBase
	{
		typedef typename _EnumType MyEnumType;
		typedef typename std::string MyStrType;

		SWorldEventParsedBase(MyEnumType const& Type, MyStrType const& rkType, bool const NeedValue = false)
			: iType(Type), kType(rkType), bNeedValue(NeedValue)
		{}

		SWorldEventParsedBase(SWorldEventParsedBase const& rhs)
			: iType(rhs.iType), kType(rhs.kType), bNeedValue(rhs.bNeedValue)
		{}

		bool operator ==(MyEnumType const& rhs) const			{ return iType == rhs; }
		bool operator ==(MyStrType const& rhs) const			{ return kType == rhs; }

		MyEnumType const iType;
		MyStrType const kType;
		bool const bNeedValue;
	};

	//
	typedef enum EUseType
	{
		UT_All				= 0xFF,
		UT_OnlyCondition	= 0x01,
		UT_OnlyAction		= 0x02,
	} EUseType;

	template< typename _EnumType >
	struct SWorldEventTargetParsedBase : public SWorldEventParsedBase< _EnumType >
	{
		typedef typename SWorldEventParsedBase< _EnumType > _MyBase;
		typedef typename _MyBase::MyEnumType MyEnumType;
		typedef typename _MyBase::MyStrType MyStrType;
		SWorldEventTargetParsedBase(MyEnumType const& Type, MyStrType const& rkType, EUseType const UseType, bool const NeedValue = false)
			: _MyBase(Type, rkType, NeedValue), eUseType(UseType)
		{}

		SWorldEventTargetParsedBase(SWorldEventTargetParsedBase const& rhs)
			: _MyBase(rhs), eUseType(UseType)
		{}

		bool operator ==(MyEnumType const& rhs) const			{ return _MyBase::operator ==(rhs); }
		bool operator ==(MyStrType const& rhs) const			{ return _MyBase::operator ==(rhs); }

		EUseType const eUseType;
	};
};
using PgWorldEventUtil::SErrorInfo;


/// ======================================================
/// == in XML parse class
// Definition Condition
namespace WorldEventTarget
{
	using namespace PgWorldEventUtil;
	typedef PgWorldEventUtil::SWorldEventTargetParsedBase< ETargetType >		SWorldEventTargetType;

	SWorldEventTargetType const pkTargetArray[] = {	SWorldEventTargetType(WETT_WorldEvent, "WORLD_EVENT", UT_All),
													SWorldEventTargetType(WETT_WEClientObject, "CLIENT_OBJECT", UT_All),
													SWorldEventTargetType(WETT_Trigger, "TRIGGER", UT_All),
													SWorldEventTargetType(WETT_MonRegenPoint, "MON_REGEN_POINT", UT_All),
													SWorldEventTargetType(WETT_MonRegenPointGroup, "MON_REGEN_GROUP", UT_All),
													SWorldEventTargetType(WETT_ObjectRegenPointGroup, "OBJECT_REGEN_GROUP", UT_All),
													SWorldEventTargetType(WETT_ObjectRegenPoint, "OBJECT_REGEN_POINT", UT_All),
													SWorldEventTargetType(WETT_SummonNPC_RegenPoint, "SUMMON_NPC_REGEN_POINT", UT_All),
													//SWorldEventTargetType(WETT_RegenPointGroup, "REGEN_POINT_GROUP", UT_All),
													SWorldEventTargetType(WETT_Item, "ITEM", UT_OnlyCondition),
													//SWorldEventTargetType(WETT_Buff, "BUFF", UT_OnlyCondition),
													SWorldEventTargetType(WETT_Quest, "QUEST", UT_OnlyCondition),
													SWorldEventTargetType(WETT_MonEffect, "MON_EFFECT", UT_OnlyCondition),
													SWorldEventTargetType(WETT_MoveUnit, "MOVE_UNIT", UT_OnlyAction),
													SWorldEventTargetType(WETT_AliveUnit, "ALIVE_UNIT", UT_OnlyCondition),
													SWorldEventTargetType(WETT_UnitLevel, "UNIT_LEVEL", UT_OnlyCondition),
													SWorldEventTargetType(WETT_IndunState, "INDUN_STATE", UT_All),
													SWorldEventTargetType(WETT_EventGroundState, "EVENT_GROUND_STATE", UT_OnlyCondition),
													SWorldEventTargetType(WETT_UnitGoalTrigger, "UNIT_GOAL_TARGET", UT_All),

													};

	bool ConvertStrTargetTypeToEnum(std::string const& rkStrType, EUseType const eUseType, ETargetType& iOut)
	{
		SWorldEventTargetType const* pkFind = std::find(pkTargetArray, PgArrayUtil::GetEndArray(pkTargetArray), rkStrType);
		if( PgArrayUtil::IsInArray(pkFind, pkTargetArray) )
		{
			if( 0 != ((*pkFind).eUseType & eUseType) )
			{
				iOut = (*pkFind).iType;
				return true;
			}
		}
		return false;
	}
	std::string ConvertTargetTypeToStr(ETargetType const iTargetType)
	{
		SWorldEventTargetType const* pkFind = std::find(pkTargetArray, PgArrayUtil::GetEndArray(pkTargetArray), iTargetType);
		if( PgArrayUtil::IsInArray(pkFind, pkTargetArray) )
		{
			return (*pkFind).kType;
		}
		return std::string("Unknown");
	}
};

namespace WorldEventCondition
{
	typedef PgWorldEventUtil::SWorldEventParsedBase< EConditionType >			SWorldEventConditionType;
	
	SWorldEventConditionType const pkConditionArray[] = {	SWorldEventConditionType(CT_Status, "STATUS", true),					// All target status<int> (not same enable)
															SWorldEventConditionType(CT_Enter, "ENTER", false),						// Trigger enter
															SWorldEventConditionType(CT_Leave, "LEAVE", false),						// Trigger leave
															SWorldEventConditionType(CT_Action, "ACTION", false),					// Trigger action (Z)
															SWorldEventConditionType(CT_Equal, "EQUAL", true),						// == (signed / unsigned DWORD)
															SWorldEventConditionType(CT_Have, "HAVE", true),						// <= (signed / unsigned DWORD)
															SWorldEventConditionType(CT_Over, "OVER", true),						// < (signed / unsigned DWORD)
															SWorldEventConditionType(CT_Less, "LESS", true),						// > (signed / unsigned DWORD)
															SWorldEventConditionType(CT_Interval, "INTERVAL", true),				// 레벨 구간. ( ex) "40/49" => 40~49 사이에 있을 때만..).
															SWorldEventConditionType(CT_ActAfterTime, "ACT_AFTER_TIME", true),		// Time (DWORD)
															SWorldEventConditionType(CT_Ing, "ING", true),							// Quest ing
															SWorldEventConditionType(CT_End, "END", true) };						// Quest ended

	bool ConvertStrConditionTypeToEnum(std::string const& rkStrType, EConditionType& iOut)
	{
		SWorldEventConditionType const* pkFind = std::find(pkConditionArray, PgArrayUtil::GetEndArray(pkConditionArray), rkStrType);
		if( PgArrayUtil::IsInArray(pkFind, pkConditionArray) )
		{
			iOut = (*pkFind).iType;
			return true;
		}
		return false;
	}
	bool IsConditionTypeNeedValue(EConditionType const iConditionType)
	{
		SWorldEventConditionType const* pkFind = std::find(pkConditionArray, PgArrayUtil::GetEndArray(pkConditionArray), iConditionType);
		if( PgArrayUtil::IsInArray(pkFind, pkConditionArray) )
		{
			return (*pkFind).bNeedValue;
		}
		return false;
	}
	std::string ConvertConditionTypeToStr(EConditionType const iConditionType)
	{
		SWorldEventConditionType const* pkFind = std::find(pkConditionArray, PgArrayUtil::GetEndArray(pkConditionArray), iConditionType);
		if( PgArrayUtil::IsInArray(pkFind, pkConditionArray) )
		{
			return (*pkFind).kType;
		}
		return std::string();
	}
};


// Definition Action
namespace WorldEventAction
{
	typedef PgWorldEventUtil::SWorldEventParsedBase< EActionType >				SWorldEventActionType;
	
	SWorldEventActionType const pkEventArray[] = {	SWorldEventActionType(AT_Status, "STATUS", true),		// change status(status value<int>)
													SWorldEventActionType(AT_Enable, "ENABLE", true),		// enabled / disable
													SWorldEventActionType(AT_Show, "SHOW", true),			// visible / invisible
													SWorldEventActionType(AT_Active, "ACTIVE", true),		// Active(Count)
													SWorldEventActionType(AT_Remove, "REMOVE"),				// remove(del)
													SWorldEventActionType(AT_Script, "SCRIPT") };			// script
													//SWorldEventActionType(AT_Use, "USE") };

	bool ConvertStrActionTypeToEnum(std::string const& rkStrType, EActionType& iOut)
	{
		SWorldEventActionType const* pkFind = std::find(pkEventArray, PgArrayUtil::GetEndArray(pkEventArray), rkStrType);
		if( PgArrayUtil::IsInArray(pkFind, pkEventArray) )
		{
			iOut = (*pkFind).iType;
			return true;
		}
		return false;
	}
	bool IsActionTypeNeedValue(EActionType const iActionType)
	{
		SWorldEventActionType const* pkFind = std::find(pkEventArray, PgArrayUtil::GetEndArray(pkEventArray), iActionType);
		if( PgArrayUtil::IsInArray(pkFind, pkEventArray) )
		{
			return (*pkFind).bNeedValue;
		}
		return false;
	}
	std::string ConvertActionTypeToStr(EActionType const iActionType)
	{
		SWorldEventActionType const* pkFind = std::find(pkEventArray, PgArrayUtil::GetEndArray(pkEventArray), iActionType);
		if( PgArrayUtil::IsInArray(pkFind, pkEventArray) )
		{
			return (*pkFind).kType;
		}
		return std::string("Unknown");
	}
};


/// ======================================================
/// == in game class
//
PgWorldEventState::PgWorldEventState()
	: m_kID(0), m_kEnable(true), m_kStatus(false)
{
}

PgWorldEventState::PgWorldEventState(WORD const& rkID, bool const bEnable)
	: m_kID(rkID), m_kEnable(bEnable), m_kStatus(false)
{
}
PgWorldEventState::PgWorldEventState(PgWorldEventState const& rhs)
	: m_kID(rhs.m_kID), m_kEnable(rhs.m_kEnable), m_kStatus(rhs.m_kStatus)
{
}

PgWorldEventState::~PgWorldEventState()
{
}

void PgWorldEventState::operator =(PgWorldEventState const& rhs)
{
	m_kID = rhs.m_kID;
	m_kEnable = rhs.m_kEnable;
	m_kStatus = rhs.m_kStatus;
}

void PgWorldEventState::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kID );
	rkPacket.Push( m_kEnable );
	rkPacket.Push( m_kStatus );
}

bool PgWorldEventState::ReadFromPacket(BM::Stream& rkPacket)
{
	bool bRet = rkPacket.Pop( m_kID )
			&&	rkPacket.Pop( m_kEnable )
			&&	rkPacket.Pop( m_kStatus );
	return bRet;
}




//
PgWorldEvent::PgWorldEvent(WORD const& rkID, bool const bEnable, DWORD const dwResetTime)
	: PgWorldEventState(rkID, bEnable), m_kResetTime(dwResetTime), m_kAccumActivatedTime(0), m_kMutex()
{
}

PgWorldEvent::~PgWorldEvent()
{
}

PgWorldEvent::PgWorldEvent(PgWorldEvent const& rhs)
	: PgWorldEventState(rhs), m_kResetTime(rhs.m_kResetTime), m_kAccumActivatedTime(0)
	, m_kAndCondition(rhs.m_kAndCondition), m_kOrCondition(rhs.m_kOrCondition)
	, m_kActivateAction(rhs.m_kActivateAction), m_kDeactivateAction(rhs.m_kDeactivateAction), m_kMutex()
{
}

void PgWorldEvent::operator =(PgWorldEvent const& rhs)
{
	// never try to copy
}

bool PgWorldEvent::Parse(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	if( PgWorldEventUtil::kWorldEventElement != pkRoot->Value() )
	{
		return false;
	}

	TiXmlElement const* pkNode = pkRoot->FirstChildElement();
	while( pkNode )
	{
		char const* szElementName = pkNode->Value();
		if( !szElementName )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Element name is NULL") , pkRoot) );
			return false;
		}

		if( PgWorldEventUtil::kConditionElement == szElementName ) // CONDITION
		{
			if( !ParseCondition(pkNode) )
			{
				return false;
			}
		}
		else if( PgWorldEventUtil::kActivateActionElement == szElementName )
		{
			if( !ParseAction(m_kActivateAction, pkNode) )
			{
				return false;
			}
		}
		else if( PgWorldEventUtil::kDeactivateActionElement == szElementName )
		{
			if( !ParseAction(m_kDeactivateAction, pkNode) )
			{
				return false;
			}
		}
		else
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"wrong Element Name"), pkNode) );
			return false;
		}

		pkNode = pkNode->NextSiblingElement();
	}

	if( m_kAndCondition.empty()
	&&	m_kOrCondition.empty() )
	{
		PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"have Condition count 0"), pkRoot) );
		return false;
	}

	return true;
}

bool PgWorldEvent::ParseCondition(TiXmlElement const* pkNode)
{
	using namespace WorldEventTarget;
	using namespace WorldEventCondition;

	if( !pkNode )
	{
		return false;
	}

	std::wstring kBinding;
	ETargetType iTargetType = WETT_None;
	BM::vstring kTargetID;
	EConditionType iConditionType = CT_None;
	BM::vstring kConditionValue;

	TiXmlAttribute const* pkAttribute = pkNode->FirstAttribute();
	while( pkAttribute )
	{
		char const* szName = pkAttribute->Name();
		char const* szValue = pkAttribute->Value();
		if( !szName )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute name is NULL"), pkNode) );
			return false;
		}
		if( !szValue )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute name is NULL"), pkNode) );
			return false;
		}

		if( PgWorldEventUtil::kAttributeBinding == szName )
		{
			kBinding = PgStringUtil::SafeUni(szValue);
		}
		else if( PgWorldEventUtil::kAttributeTargetType == szName )
		{
			if( !ConvertStrTargetTypeToEnum(std::string(szValue), PgWorldEventUtil::UT_OnlyCondition ,iTargetType) )
			{
				PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Condition Target Type[") << szValue << L"] is unknown" , pkNode) );
				return false;
			}
		}
		else if( PgWorldEventUtil::kAttributeTargetID == szName )
		{
			kTargetID = PgStringUtil::SafeUni(szValue);
			if( 0 == kTargetID.size() )
			{
				PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Condition Target ID is Empty"), pkNode) );
				return false;
			}
		}
		else if( PgWorldEventUtil::kAttributeEventType == szName )
		{
			if( !ConvertStrConditionTypeToEnum(std::string(szValue), iConditionType) )
			{
				PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Condition Type[") << szValue << L"] is unknown" , pkNode) );
				return false;
			}
		}
		else if( PgWorldEventUtil::kAttributeEventValue == szName )
		{
			kConditionValue = PgStringUtil::SafeUni(szValue);
		}
		else
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute[") << szName << L"] is unknown" , pkNode) );
			return false;
		}

		pkAttribute = pkAttribute->Next();
	}

	if( IsConditionTypeNeedValue(iConditionType)
	&&	0 == kConditionValue.size() )
	{
		PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Condition Type[") << ConvertConditionTypeToStr(iConditionType) << L"] is Need value, Value is Empty" , pkNode) );
		return false;
	}

	SWorldEventCondition const kNewCondition(iTargetType, kTargetID, iConditionType, kConditionValue);
	if( kBinding.empty() )
	{
		PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"<... BINDING='AND/OR' ...> Element is not found"), pkNode) );
		return false;
	}

	bool bInsertRet = true;
	if( L"AND" == kBinding )
	{
		bInsertRet = m_kAndCondition.end() == std::find(m_kAndCondition.begin(), m_kAndCondition.end(), kNewCondition);
		if( bInsertRet )
		{
			m_kAndCondition.push_back( kNewCondition );
		}
	}
	else if( L"OR" == kBinding )
	{
		bInsertRet = m_kOrCondition.end() == std::find(m_kOrCondition.begin(), m_kOrCondition.end(), kNewCondition);
		if( bInsertRet )
		{
			m_kOrCondition.push_back( kNewCondition );
		}
	}
	else
	{
		PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"wrong BINDING type[") << kBinding << L"]" , pkNode) );
		return false;
	}

	if( !bInsertRet )
	{
		PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Duplicate Condition Item, Binding[") << kBinding << L"] TargetType[" << ConvertTargetTypeToStr(iTargetType) << L"] TargetID[" << kTargetID << L"] Condition[" << ConvertConditionTypeToStr(iConditionType) << L"] ConditionValue[" << kConditionValue << L"]"  , pkNode) );
	}
	return bInsertRet;
}

bool PgWorldEvent::ParseAction(ContWorldEventAction& rkAction, TiXmlElement const* pkNode)
{
	using namespace WorldEventTarget;
	using namespace WorldEventAction;
	if( !pkNode )
	{
		return false;
	}

	TiXmlElement const* pkActionNode = pkNode->FirstChildElement();
	while( pkActionNode )
	{
		typedef std::list< BM::vstring > ContTargetID;
		ContTargetID kTargetIDs;
		ETargetType iTargetType = WETT_None;
		EActionType iActionType = AT_None;
		BM::vstring kActionValue;

		char const* szElementName = pkActionNode->Value();
		if( !szElementName )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Element name is NULL"), pkActionNode) );
			return false;
		}

		if( PgWorldEventUtil::kActionElement == szElementName )
		{
			TiXmlAttribute const* pkAttribute = pkActionNode->FirstAttribute();
			while( pkAttribute )
			{
				char const* szName = pkAttribute->Name();
				char const* szValue = pkAttribute->Value();
				if( !szName )
				{
					PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute name is NULL"), pkActionNode) );
					return false;
				}
				if( !szValue )
				{
					PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute value is NULL") , pkActionNode) );
					return false;
				}

				if( PgWorldEventUtil::kAttributeTargetType == szName )
				{
					if( !ConvertStrTargetTypeToEnum(std::string(szValue), PgWorldEventUtil::UT_OnlyAction, iTargetType) )
					{
						PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Target Type[") << szValue << L"] is unknown" , pkActionNode) );
						return false;
					}
				}
				else if( PgWorldEventUtil::kAttributeTargetID == szName )
				{
					BM::vstring kTargetID( PgStringUtil::SafeUni(szValue) );
					if( 0 != kTargetID.size() )
					{
						kTargetIDs.push_back( kTargetID );
					}
				}
				else if( PgWorldEventUtil::kAttributeEventType == szName )
				{
					if( !ConvertStrActionTypeToEnum(std::string(szValue), iActionType) )
					{
						PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Action Type[") << szValue << L"] is unknown" , pkActionNode) );
						return false;
					}
				}
				else if( PgWorldEventUtil::kAttributeEventValue == szName )
				{
					kActionValue = PgStringUtil::SafeUni(szValue);
				}
				else
				{
					PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Attribute[") << szName << L"] is unknown" , pkNode) );
					return false;
				}

				pkAttribute = pkAttribute->Next();
			}
		}
		else
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"unknown ELEMENT Name[") << szElementName << L"]" , pkActionNode) );
			return false;
		}

		{ // Element TargetID
			TiXmlElement const* pkTargetNode = pkActionNode->FirstChildElement( PgWorldEventUtil::kAttributeTargetID.c_str() );
			while( pkTargetNode )
			{
				BM::vstring const kTempTargetID( PgStringUtil::SafeUni(pkTargetNode->GetText()) );

				if( 0 == kTempTargetID.size() )
				{
					PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Target ID is empty"), pkTargetNode) );
				}
				else
				{
					kTargetIDs.push_back( kTempTargetID );
				}

				pkTargetNode = pkTargetNode->NextSiblingElement();
			}
		}

		if( kTargetIDs.empty() )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Target ID is empty"), pkActionNode) );
			return false;
		}

		if( IsActionTypeNeedValue(iActionType)
		&&	0 == kActionValue.size() )
		{
			PgWorldEventUtil::AddError( SErrorInfo(this, __FUNCTIONW__, __LINE__, BM::vstring(L"Action Type[") << ConvertActionTypeToStr(iActionType) << L"] is Need value, Value is Empty" , pkNode) );
			return false;
		}

		// 같은 액션을, 다른 타겟ID 들로 복사
		ContTargetID::const_iterator id_iter = kTargetIDs.begin();
		while( kTargetIDs.end() != id_iter )
		{
			SWorldEventAction const kNewAction(iTargetType, (*id_iter), iActionType, kActionValue);
			rkAction.push_back( kNewAction );

			++id_iter;
		}
		
		pkActionNode = pkActionNode->NextSiblingElement();
	}

	return true;
}

bool PgWorldEvent::Status(bool const bNewStatus, ContWorldEventAction const*& pkOut, bool const bByAction)
{
	BM::CAutoMutex kLock(m_kMutex);
	pkOut = NULL;
	if( !m_kEnable )
	{
		return false;
	}

	if( m_kStatus != bNewStatus )
	{
		if( true == bNewStatus )
		{
			pkOut = &m_kActivateAction;
		}
		else
		{
			pkOut = &m_kDeactivateAction;
		}

		m_kAccumActivatedTime = 0;
	}
	m_kStatus = bNewStatus;

	return pkOut != NULL;
}

bool PgWorldEvent::Tick(DWORD const dwEleapsedTime, bool& bNextStatus)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !m_kEnable )
	{
		return false;
	}

	m_kAccumActivatedTime += dwEleapsedTime; // 시간은 계속 누적

	if( m_kStatus
	&&	0 < m_kResetTime ) // 황설화 상태면
	{
		if( m_kResetTime <= m_kAccumActivatedTime ) // 리셋 타임이 지나면 비 활성화로
		{
			bNextStatus = false;
			return true; // 상태가 변했다.
		}
	}
	return false; // 변하지 않았다.
}

bool PgWorldEvent::SetCondition(ContWorldEventCondition& rkContCondition, SWorldEventCondition const& rkWhatCondition, bool const bStatus, bool& bUpdated)
{
	if( !m_kEnable )
	{
		return true;
	}

	ContWorldEventCondition::iterator find_iter = std::find(rkContCondition.begin(), rkContCondition.end(), rkWhatCondition);
	if( rkContCondition.end() != find_iter )
	{
		ContWorldEventCondition::value_type& rkCondition = (*find_iter);
		switch( rkCondition.TargetType() )
		{
		case WorldEventTarget::WETT_Trigger:
		case WorldEventTarget::WETT_Quest:
		case WorldEventTarget::WETT_IndunState:
		case WorldEventTarget::WETT_EventGroundState:
		case WorldEventTarget::WETT_UnitGoalTrigger:
			{
				bUpdated = true;
			}break;
		default:
			{
				bUpdated = rkCondition.Status() != bStatus;
			}break;
		}
		rkCondition.Status(bStatus);
		return true;
	}
	return false;
}

bool PgWorldEvent::SetCondition(SWorldEventCondition const& rkWhatCondition, bool const bStatus)
{
	BM::CAutoMutex kLock(m_kMutex);
	if( !m_kEnable )
	{
		return true;
	}

	bool bUpdated1 = false, bUpdated2 = false;;
	bool const bFind1 = SetCondition(m_kAndCondition, rkWhatCondition, bStatus, bUpdated1);
	bool const bFind2 = SetCondition(m_kOrCondition, rkWhatCondition, bStatus, bUpdated2);
	return (bUpdated1 || bUpdated2) && (bFind1 || bFind2);
}

bool PgWorldEvent::CheckCondition() const
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !m_kEnable )
	{
		return m_kStatus;
	}

	{
		ContWorldEventCondition::const_iterator iter = m_kAndCondition.begin();
		while( m_kAndCondition.end() != iter )
		{
			if( !(*iter).Status() )
			{
				return false;
			}
			++iter;
		}
	}
	// 여기까지 오면 And 조건은 True

	bool bOrStatus = m_kOrCondition.empty();
	{
		ContWorldEventCondition::const_iterator iter = m_kOrCondition.begin();
		while( m_kOrCondition.end() != iter )
		{
			bOrStatus = (*iter).Status() || bOrStatus;
			if( bOrStatus )
			{
				break;
			}
			++iter;
		}
	}
	return bOrStatus;
}






//
namespace PgWorldEventMgrUtil
{
	SWorldEventLinkedCondition::SWorldEventLinkedCondition(PgWorldEvent const& rkWorldEvent, SWorldEventCondition const& rkCondition)
		: kWorldEvnet(rkWorldEvent), kCondition(rkCondition)
	{}

	SWorldEventLinkedCondition::SWorldEventLinkedCondition(SWorldEventLinkedCondition const& rhs)
		: kWorldEvnet(rhs.kWorldEvnet), kCondition(rhs.kCondition)
	{}

	template< typename _KeyType, typename _ValType >
	bool PushBackListMapElement(_KeyType const& rkKey, _ValType const& rkNewVal, std::map< _KeyType, std::list< _ValType > >& rkMap)
	{
		typedef typename std::map< _KeyType, std::list< _ValType > > _MapType;
		_MapType::iterator find_iter = rkMap.find( rkKey );
		if( rkMap.end() == find_iter )
		{
			auto kRet = rkMap.insert( std::make_pair(rkKey, _MapType::mapped_type()) );
			if( kRet.second )
			{
				find_iter = kRet.first;
			}
		}

		if( rkMap.end() == find_iter )
		{
			return false;
		}

		((*find_iter).second).push_back( rkNewVal );
		return true;
	}
};



///
//
PgWorldEventBaseMgr::PgWorldEventBaseMgr()
{
}

PgWorldEventBaseMgr::~PgWorldEventBaseMgr()
{
}

bool PgWorldEventBaseMgr::ParseWorldEvent(TiXmlElement const* pkRoot)
{
	if( !pkRoot )
	{
		return false;
	}

	if( PgWorldEventUtil::kWorldEventElement != pkRoot->Value() )
	{
		return false;
	}

	int const iTempInt = PgStringUtil::SafeAtoi(pkRoot->Attribute("ID"));
	if( 0 > iTempInt
	||	USHRT_MAX < iTempInt )
	{
		PgWorldEventUtil::AddError( SErrorInfo(NULL, __FUNCTIONW__, __LINE__, BM::vstring(L"WorldEvnet ID[") << iTempInt << L"] is wrong range, correct range is [1 ~ 65535]", pkRoot) );
		return false;
	}


	WORD const kID = static_cast< WORD >(iTempInt);
	DWORD const dwResetTime = PgStringUtil::SafeAtoi(pkRoot->Attribute("RESET_TIME"));
	bool const bEnable = PgStringUtil::SafeAtob(pkRoot->Attribute( PgWorldEventUtil::kAttributeEnalbe.c_str() ), true);
	if( 0 == kID )
	{
		PgWorldEventUtil::AddError( SErrorInfo(NULL, __FUNCTIONW__, __LINE__, BM::vstring(L"WorldEvnet ID is empty"), pkRoot) );
		return false;
	}

	PgWorldEvent kNewWorldEvent(kID, bEnable, dwResetTime);
	if( !kNewWorldEvent.Parse(pkRoot) )
	{
		return false;
	}

	auto kRet = m_kEvent.insert( std::make_pair(kID, kNewWorldEvent) );
	if( !kRet.second )
	{
		PgWorldEventUtil::AddError( SErrorInfo(NULL, __FUNCTIONW__, __LINE__, BM::vstring(L"WorldEvent ID[") << kID << L"] is duplicate", pkRoot) );
		return false;
	}
	return true;
}

bool PgWorldEventBaseMgr::GetWorldEvent(WORD const& rkWorldEventID, PgWorldEvent const* &pkOut) const
{
	ContWorldEvent::const_iterator find_iter = m_kEvent.find(rkWorldEventID);
	if( m_kEvent.end() != find_iter )
	{
		pkOut = &((*find_iter).second);
		return true;
	}
	return false;
}


///
//
PgWorldEventMgr::PgWorldEventMgr()
	: PgWorldEventBaseMgr()
{
}

PgWorldEventMgr::~PgWorldEventMgr()
{
}

void PgWorldEventMgr::BuildWorldEventLinkedData()
{
	ContWorldEvent::const_iterator iter = m_kEvent.begin();
	while( m_kEvent.end() != iter )
	{
		ContWorldEvent::mapped_type const& rkWorldEvent = (*iter).second;
		BuildWorldEventLinkedData(rkWorldEvent, rkWorldEvent.AndCondition());
		BuildWorldEventLinkedData(rkWorldEvent, rkWorldEvent.OrCondition());

		//rkWorldEvent.ActivateAction();
		//rkWorldEvent.DeactivateAction();
		++iter;
	}
}

void PgWorldEventMgr::BuildWorldEventLinkedData(PgWorldEvent const& rkWorldEvent, ContWorldEventCondition const& rkConditionList)
{
	using namespace PgWorldEventMgrUtil;
	using namespace WorldEventTarget;
	ContWorldEventCondition::const_iterator cond_iter = rkConditionList.begin();
	while( rkConditionList.end() != cond_iter )
	{
		ContWorldEventCondition::value_type const& rkCondition = (*cond_iter);
		switch( rkCondition.TargetType() )
		{
		case WETT_MonRegenPoint:
			{
				BM::GUID const kTargetID(rkCondition.TargetIDAsGuid());
				PushBackListMapElement(kTargetID, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kMonGenToEvent);
			}break;
		case WETT_MonRegenPointGroup:
			{
				PushBackListMapElement(rkCondition.TargetIDAsInt(), SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kMonGenGroupToEvent);
			}break;
		case WETT_ObjectRegenPoint:
			{
				BM::GUID const kTargetID(rkCondition.TargetIDAsGuid());
				PushBackListMapElement(kTargetID, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kObjectGenToEvent);
			}break;
		case WETT_ObjectRegenPointGroup:
			{
				PushBackListMapElement(rkCondition.TargetIDAsInt(), SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kObjectGenGroupToEvent);
			}break;
		case WETT_WorldEvent:
			{
				WORD const kWorldEvnetID = static_cast< WORD >(rkCondition.TargetIDAsDWORD());
				PushBackListMapElement(kWorldEvnetID, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kWorldEventToEvent);
			}break;
		case WETT_WEClientObject:
			{
				WORD const kWEClientObjectID = static_cast< WORD >(rkCondition.TargetIDAsDWORD());
				PushBackListMapElement(kWEClientObjectID, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kWEClientObjectToEvent);
			}break;
		case WETT_MonEffect:
			{
				int const iMonNo = rkCondition.TargetIDAsInt();
				PushBackListMapElement(iMonNo, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kMonEffectToEvent);
			}break;
		case WETT_IndunState:
			{
				int const iState = rkCondition.ConditionValueAsInt();
				PushBackListMapElement(iState, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_kIndunStateToEvent);
			}break;
		case WETT_EventGroundState:
			{
				int const iState = rkCondition.ConditionValueAsInt();
				PushBackListMapElement(iState, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_EventGroundStateToEventMap);
			}break;
		case WETT_UnitGoalTrigger:
			{
				const std::string kTriggerName = MB(rkCondition.ConditionValue());
				PushBackListMapElement(kTriggerName, SWorldEventLinkedCondition(rkWorldEvent, rkCondition), m_UnitGoalTarget);
			}break;
		default:
			{
			}break;
		}
		++cond_iter;
	}
}

void PgWorldEventMgr::CloneWorldEvent(PgWorldEventMgr &rTo) const
{
	//BM::CAutoMutex kLock(m_kMutex);
	//BM::CAutoMutex kLock2(rTo.m_kMutex);
	rTo.m_kEvent = m_kEvent;
	rTo.m_kMonGenToEvent = m_kMonGenToEvent;
	rTo.m_kMonGenGroupToEvent = m_kMonGenGroupToEvent;
	rTo.m_kObjectGenToEvent = m_kObjectGenToEvent;
	rTo.m_kWorldEventToEvent = m_kWorldEventToEvent;
	rTo.m_kMonEffectToEvent = m_kMonEffectToEvent;
	rTo.m_kObjectGenGroupCount = m_kObjectGenGroupCount;
	rTo.m_kObjectGenGroupToEvent = m_kObjectGenGroupToEvent;
	rTo.m_kIndunStateToEvent = m_kIndunStateToEvent;
	rTo.m_EventGroundStateToEventMap = m_EventGroundStateToEventMap;
	rTo.m_UnitGoalTarget = m_UnitGoalTarget;
	//rTo.m_kMonGenCount = m_kMonGenCount;
	//rTo.m_kObjectGenCount = m_kObjectGenCount;
}

void PgWorldEventMgr::TickWorldEvent(int const iGroundNo, DWORD const dwEleapsedTime)
{
	ContWorldEvent::iterator iter = m_kEvent.begin();
	while( m_kEvent.end() != iter )
	{
		ContWorldEvent::key_type const& rkKey = (*iter).first;
		ContWorldEvent::mapped_type& rkWorldEvent = (*iter).second;

		bool bNextStatus = false;
		if( rkWorldEvent.Tick(dwEleapsedTime, bNextStatus) )
		{
			SetWorldEventStatus(iGroundNo, rkKey, bNextStatus, false);
		}
		DoLinkedCondition(m_kWorldEventToEvent, iGroundNo, rkKey);
		++iter;
	}
}

bool PgWorldEventMgr::SetWorldEventStatus(int const iGroundNo, WORD const& rkWorldEventID, bool const bNextStatus, bool bByAction)
{
	bool bChanged = false;
	{
		//BM::CAutoMutex kLock(m_kMutex);
		ContWorldEvent::iterator iter = m_kEvent.find(rkWorldEventID);
		if( m_kEvent.end() == iter )
		{
			return false;
		}
		PgWorldEvent& rkWorldEvent = (*iter).second;

		ContWorldEventAction const* pkOut = NULL;
		bChanged = rkWorldEvent.Status(bNextStatus, pkOut, bByAction);
		if( bChanged
		&&	pkOut )
		{
			NfyWorldEventToGroundUser(rkWorldEvent);
			DoWorldEventAction(iGroundNo, *pkOut, rkWorldEvent);
		}
	}
	return true;
}

bool PgWorldEventMgr::SetWorldEventEnable(WORD const& rkWorldEventID, bool const bEnable)
{
	ContWorldEvent::iterator iter = m_kEvent.find(rkWorldEventID);
	if( m_kEvent.end() == iter )
	{
		return false;
	}
	PgWorldEvent& rkWorldEvent = (*iter).second;
	rkWorldEvent.Enable(bEnable);

	NfyWorldEventToGroundUser(rkWorldEvent);
	return true;
}

bool PgWorldEventMgr::SetWorldEventConditionStatus(int const iGroundNo, WORD const& rkWorldEventID, SWorldEventCondition const& rkWhatCondition, bool const bStatus)
{
	//BM::CAutoMutex kLock(m_kMutex);
	ContWorldEvent::iterator iter = m_kEvent.find(rkWorldEventID);
	if( m_kEvent.end() == iter )
	{
		return false;
	}
	PgWorldEvent& rkWorldEvent = (*iter).second;

	bool const bRet = rkWorldEvent.SetCondition(rkWhatCondition, bStatus);
	if( !bRet )
	{
		return false;
	}
	return SetWorldEventStatus(iGroundNo, rkWorldEventID, rkWorldEvent.CheckCondition(), false);
}

void PgWorldEventMgr::DoWorldEventCondition(int const iGroundNo, WORD const& rkWorldEventID, CUnit const* pkCaster)
{
	if( !iGroundNo )
	{
		return;
	}

	PgWorldEvent const* pkWorldEvent = NULL;
	if( GetWorldEvent(rkWorldEventID, pkWorldEvent) )
	{
		if( pkWorldEvent )
		{
			DoWorldEventCondition(iGroundNo, pkWorldEvent->AndCondition(), *pkWorldEvent, pkCaster);
			DoWorldEventCondition(iGroundNo, pkWorldEvent->OrCondition(), *pkWorldEvent, pkCaster);
		}
	}
}

void PgWorldEventMgr::DoWorldEventCondition(int const iGroundNo, ContWorldEventCondition const& rkConditionCont, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster)
{
	if( rkConditionCont.empty() )
	{
		return;
	}

	ContWorldEventCondition::const_iterator iter = rkConditionCont.begin();
	while( rkConditionCont.end() != iter )
	{
		DoWorldEventCondition(iGroundNo, (*iter), rkWorldEvent, pkCaster);
		++iter;
	}
}


void PgWorldEventMgr::DoWorldEventCondition(int const iGroundNo, SWorldEventCondition const& rkCondition, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster)
{
	using namespace WorldEventTarget;
	using namespace WorldEventCondition;
	using PgWorldEventUtil::SErrorInfo;
	using PgWorldEventUtil::AddError;

	bool bProcessed = true;
	switch( rkCondition.TargetType() )
	{
	case WETT_WorldEvent:
		{
			PgWorldEvent const* pkWorldEvent = NULL;
			bProcessed = GetWorldEvent(static_cast< WORD >(rkCondition.TargetIDAsDWORD()), pkWorldEvent);

			switch( rkCondition.ConditionType() )
			{
			case CT_ActAfterTime:
				{
					if( bProcessed )
					{
						bool const bStatus = pkWorldEvent->Status() && rkCondition.ConditionValueAsDWORD() < pkWorldEvent->AccumActivatedTime(); // && pkWorldEvent->Enable();
						SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
					}
				}break;
			case CT_Status:
				{
					if( bProcessed )
					{
						bool const bStatus = pkWorldEvent->Status() == rkCondition.ConditionValueAsBool();
						SetWorldEventConditionStatus(iGroundNo, rkWorldEvent.ID(), rkCondition, bStatus);
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	default:
		{
			bProcessed = false;
		}break;
	}

	if( !bProcessed )
	{
		BM::vstring const kTempHeader = BM::vstring("GroundNo[") << iGroundNo << L"] WORLD EVENT CONDITION[Target[" << ConvertTargetTypeToStr(rkCondition.TargetType()) << L"] TargetID[" << rkCondition.TargetID() << L"] ConditionType[" << ConvertConditionTypeToStr(rkCondition.ConditionType()) << L"] ConditionValue[" << rkCondition.ConditionValue() << L"]] is wrong WORLD EVENT CONDITION";

		CAUTION_LOG(BM::LOG_LV1, kTempHeader);
	}
}

void PgWorldEventMgr::DoWorldEventAction(int const iGroundNo, ContWorldEventAction const& rkActionList, PgWorldEvent const& rkWorldEvent)
{
	if( rkActionList.empty() )
	{
		return;
	}

	ContWorldEventAction::const_iterator iter = rkActionList.begin();
	while( rkActionList.end() != iter )
	{
		DoWorldEventAction(iGroundNo, (*iter), rkWorldEvent);
		++iter;
	}
}

void PgWorldEventMgr::DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent)
{
	using namespace WorldEventTarget;
	using namespace WorldEventAction;
	using PgWorldEventUtil::SErrorInfo;
	using PgWorldEventUtil::AddError;

	bool bProcessed = true;
	switch( rkAction.TargetType() )
	{
	case WETT_WorldEvent:
		{
			PgWorldEvent const* pkWorldEvent = NULL;
			WORD const kTargetWorldEventID = static_cast< WORD >(rkAction.TargetIDAsDWORD());
			bProcessed = GetWorldEvent(kTargetWorldEventID, pkWorldEvent);

			switch( rkAction.ActionType() )
			{
			case AT_Status:
				{
					if( bProcessed )
					{
						bProcessed = SetWorldEventStatus(iGroundNo, kTargetWorldEventID, rkAction.ActionValueAsBool(), true);
					}
				}break;
			case AT_Enable:
				{
					if( bProcessed )
					{
						bProcessed = SetWorldEventEnable(kTargetWorldEventID, rkAction.ActionValueAsBool());
					}
				}break;
			default:
				{
					bProcessed = false;
				}break;
			}
		}break;
	default:
		{
			bProcessed = false;
		}break;
	}

	if( !bProcessed )
	{
		BM::vstring const kTempHeader = BM::vstring("GroundNo[") << iGroundNo << L"] WORLD EVENT ACTION[Target[" << ConvertTargetTypeToStr(rkAction.TargetType()) << L"] TargetID[" << rkAction.TargetID() << L"] ActionType[" << ConvertActionTypeToStr(rkAction.ActionType()) << L"] ActionValue[" << rkAction.ActionValue() << L"]] is wrong WORLD EVENT ACTION";
		CAUTION_LOG(BM::LOG_LV1, kTempHeader);
	}
}

void PgWorldEventMgr::DoMonsterEffect(int const iGroundNo, CUnit const * pkUnit)
{
	if( !pkUnit )
	{
		return;
	}

	typedef ContGenPointGroupToContidionMap _MapType;
	typedef _MapType::mapped_type _ListType;
	_MapType::const_iterator find_iter = m_kMonEffectToEvent.find( pkUnit->GetAbil(AT_CLASS) );
	if( m_kMonEffectToEvent.end() == find_iter )
	{
		return;
	}

	_ListType const& rkList = (*find_iter).second;	
	_ListType::const_iterator loop_iter = rkList.begin();
	while( rkList.end() != loop_iter )
	{
		DoWorldEventCondition(iGroundNo, (*loop_iter).kCondition, (*loop_iter).kWorldEvnet, pkUnit);
		++loop_iter;
	}
}

void PgWorldEventMgr::DoIndunState(int const iGroundNo, int const iState, CUnit const* pkUnit)
{
	DoLinkedCondition(m_kIndunStateToEvent, iGroundNo, iState, pkUnit);
}

void PgWorldEventMgr::DoEventGroundState(int const iGroundNo, int const iState)
{
	typedef ContEventGroundStateToConditionMap _MapType;
	typedef _MapType::mapped_type _ListType;

	_MapType::const_iterator find_iter = m_EventGroundStateToEventMap.find( iState );
	if( m_EventGroundStateToEventMap.end() == find_iter )
	{
		return;
	}

	_ListType const & List = (*find_iter).second;
	_ListType::const_iterator loop_iter = List.begin();
	while( List.end() != loop_iter )
	{
		DoWorldEventCondition(iGroundNo, (*loop_iter).kCondition, (*loop_iter).kWorldEvnet);
		++loop_iter;
	}
}

void PgWorldEventMgr::DoUnitGoalTarget(int const iGroundNo, CUnit const* pkUnit)
{
	typedef ContUnitGoalTarget _MapType;
	typedef _MapType::mapped_type _ListType;
	_MapType::const_iterator find_iter = m_UnitGoalTarget.begin();
	for (find_iter; find_iter != m_UnitGoalTarget.end(); ++find_iter)
	{
		_ListType const& rkList = (*find_iter).second;
		_ListType::const_iterator loop_iter = rkList.begin();
		while( rkList.end() != loop_iter )
		{
			DoWorldEventCondition(iGroundNo, (*loop_iter).kWorldEvnet.AndCondition(), (*loop_iter).kWorldEvnet, pkUnit);
			DoWorldEventCondition(iGroundNo, (*loop_iter).kWorldEvnet.OrCondition(), (*loop_iter).kWorldEvnet, pkUnit);
			++loop_iter;
		}
	}
}
void PgWorldEventMgr::DoMonsterRegenPoint(int const iGroundNo, BM::GUID const& rkMonRegenPointGuid, int const iGenPointGroup, CUnit const *pUnit)
{
	DoLinkedCondition(m_kMonGenToEvent, iGroundNo, rkMonRegenPointGuid, pUnit);
	DoLinkedCondition(m_kMonGenGroupToEvent, iGroundNo, iGenPointGroup, pUnit);
}

void PgWorldEventMgr::DoObjectRegenPoint(int const iGroundNo, BM::GUID const& rkObjectRegenPointGuid, int const iGenPointGroup, CUnit const*pUnit)
{
	DoLinkedCondition(m_kObjectGenToEvent, iGroundNo, rkObjectRegenPointGuid, pUnit);
	DoLinkedCondition(m_kObjectGenGroupToEvent, iGroundNo, iGenPointGroup, pUnit);
}

void PgWorldEventMgr::DoWEClientObject(int const iGroundNo, WORD const kWEClientObjectID)
{
	DoLinkedCondition(m_kWEClientObjectToEvent, iGroundNo, kWEClientObjectID);
}

void PgWorldEventMgr::NfyWorldEventToGroundUser(PgWorldEventState const& rkState)
{
}

void PgWorldEventMgr::WorldEventWriteToPacket(BM::Stream& kPacket) const
{
	kPacket.Push( m_kEvent.size() );
	ContWorldEvent::const_iterator iter = m_kEvent.begin();
	while( m_kEvent.end() != iter )
	{
		((*iter).second).WriteToPacket(kPacket);
		++iter;
	}
}

void PgWorldEventMgr::IncMonsterGenPointCount(BM::GUID const& rkGenPointGuid)	{ IncGenPointCount(m_kMonGenCount, rkGenPointGuid); }
void PgWorldEventMgr::DecMonsterGenPointCount(BM::GUID const& rkGenPointGuid)	{ DecGenPointCount(m_kMonGenCount, rkGenPointGuid); }
size_t PgWorldEventMgr::GetMonsterGenPointCount(BM::GUID const& rkGenPointGuid)	{ return GetGenPointCount(m_kMonGenCount, rkGenPointGuid); }
bool PgWorldEventMgr::IsMonsterGenPointCount(BM::GUID const& rkGenPointGuid)	{ return m_kMonGenCount.end() != m_kMonGenCount.find(rkGenPointGuid); }
void PgWorldEventMgr::IncMonsterGenPointGroupCount(int const& riGenPointGroupNo)	{ IncGenPointCount(m_kMonGenGroupCount, riGenPointGroupNo); }
void PgWorldEventMgr::DecMonsterGenPointGroupCount(int const& riGenPointGroupNo)	{ DecGenPointCount(m_kMonGenGroupCount, riGenPointGroupNo); }
size_t PgWorldEventMgr::GetMonsterGenPointGroupCount(int const& riGenPointGroupNo)	{ return GetGenPointCount(m_kMonGenGroupCount, riGenPointGroupNo); }
bool PgWorldEventMgr::IsMonsterGenPointGroupCount(int const& riGenPointGroupNo)		{ return m_kMonGenGroupCount.end() != m_kMonGenGroupCount.find(riGenPointGroupNo); }
void PgWorldEventMgr::IncObjectGenPointCount(BM::GUID const& rkGenPointGuid)	{ IncGenPointCount(m_kObjectGenCount, rkGenPointGuid); }
void PgWorldEventMgr::DecObjectGenPointCount(BM::GUID const& rkGenPointGuid)	{ DecGenPointCount(m_kObjectGenCount, rkGenPointGuid); }
size_t PgWorldEventMgr::GetObjectGenPointCount(BM::GUID const& rkGenPointGuid)	{ return GetGenPointCount(m_kObjectGenCount, rkGenPointGuid); }
bool PgWorldEventMgr::IsObjectGenPointCount(BM::GUID const& rkGenPointGuid)		{ return m_kObjectGenCount.end() != m_kObjectGenCount.find(rkGenPointGuid); }

void PgWorldEventMgr::IncObjectGenPointGroupCount(int const& riGenPointGroupNo)
{
	IncGenPointCount(m_kObjectGenGroupCount, riGenPointGroupNo);
}
void PgWorldEventMgr::DecObjectGenPointGroupCount(int const& riGenPointGroupNo)
{
	DecGenPointCount(m_kObjectGenGroupCount, riGenPointGroupNo);
}
size_t PgWorldEventMgr::GetObjectGenPointGroupCount(int const& riGenPointGroupNo)
{
	return GetGenPointCount(m_kObjectGenGroupCount, riGenPointGroupNo);
}
bool PgWorldEventMgr::IsObjectGenPointGroupCount(int const& riGenPointGroupNo)
{
	return m_kObjectGenGroupCount.end() != m_kObjectGenGroupCount.find(riGenPointGroupNo);
}

void PgWorldEventMgr::ClearGenPointCount()
{
	m_kMonGenCount.clear();
	m_kMonGenGroupCount.clear();
	m_kObjectGenCount.clear();
	m_kObjectGenGroupCount.clear();
}

int PgWorldEventMgr::GetMonRegenPointGroupTotalCount()
{
	int iCount = 0;

	ContWorldEvent::iterator iter = m_kEvent.begin();
	while( m_kEvent.end() != iter )
	{
		PgWorldEvent& rkWorldEvent = (*iter).second;

		if( false == rkWorldEvent.CheckCondition() )
		{
			
			if( IsActivateAction(rkWorldEvent.ActivateAction(), WorldEventTarget::WETT_MonRegenPointGroup, WorldEventAction::AT_Active) )
			{
				++iCount;
			}
		}
		++iter;
	}

	return iCount;
}

bool PgWorldEventMgr::IsActivateAction(ContWorldEventAction const& rkActionCont, WorldEventTarget::ETargetType eTargetType, WorldEventAction::EActionType eActionType)
{
	if( rkActionCont.empty() )
	{
		return 0;
	}

	ContWorldEventAction::const_iterator iter = rkActionCont.begin();
	while( rkActionCont.end() != iter )
	{
		if( (eTargetType == iter->TargetType()) && (eActionType == iter->ActionType()) )
		{
			return true;
		}

		++iter;
	}

	return false;
}