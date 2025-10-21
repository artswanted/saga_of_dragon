#include "stdafx.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgJobSkillTool.h"
#include "PgLocalPartyMgr.h"
#include "PgGroundTrigger.h"
#include "PgGround.h"
#include "PgMissionGround.h"
#include "PgPartyItemRule.h"
#include "PgRequest.h"
#include "PgReqMapMove.h"
#include "PgMissionMan.h"
#include "PgAction.h"
#include "Lohengrin/VariableContainer.h"
#include "PgSuperGround.h"
#include "Variant/PgJobSkill.h"
#include "Variant/unit.h"
#include "Variant/PgPet.h"
#include "PublicMap/PgWarGround.h"
#include "Variant/PgStringUtil.h"
#include "PgStaticRaceGround.h"

PgGroundTrigger::PgGroundTrigger(void)
{
}

PgGroundTrigger::~PgGroundTrigger(void)
{
}

PgGroundTrigger::PgGroundTrigger( PgGroundTrigger const &rhs )
:	m_kID(rhs.m_kID)
,	m_ptMin(rhs.m_ptMin)
,	m_ptMax(rhs.m_ptMax)
,	m_kEnable(rhs.m_kEnable)
,	m_kCheckEffectNo(rhs.m_kCheckEffectNo)
,	m_kErrorEffectMsgNo(rhs.m_kErrorEffectMsgNo)
,	m_kContTimeEvent(rhs.m_kContTimeEvent)
{
}

PgGroundTrigger& PgGroundTrigger::operator=( PgGroundTrigger const &rhs )
{
	m_kID = rhs.m_kID;
	m_ptMin = rhs.m_ptMin;
	m_ptMax = rhs.m_ptMax;
	m_kEnable = rhs.m_kEnable;
	m_kCheckEffectNo = rhs.m_kCheckEffectNo;
	m_kErrorEffectMsgNo = rhs.m_kErrorEffectMsgNo;
	m_kContTimeEvent = rhs.m_kContTimeEvent;
	return *this;
}

bool PgGroundTrigger::InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer)
{
	if( !pkObj )
	{
		return false;
	}

	NiBound const &kBound = pkObj->GetWorldBound();
	m_ptMin = m_ptMax = POINT3( kBound.GetCenter().x, kBound.GetCenter().y ,kBound.GetCenter().z );

	// 낮은 트리거, 높은 트리거 모두 OK
	float const fRadius = kBound.GetRadius() + fBuffer;	// 서버에서는 전체범위를 약간 크게 잡아야 한다.
	m_ptMin -= fRadius;
	m_ptMax += fRadius;
	return true;
}

bool PgGroundTrigger::Init( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, float const fBuffer )
{
	if ( !pkTriggerRoot )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( kID.empty() )
	{
		INFO_LOG( BM::LOG_LV0, __FL__<<L"ID is None");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false") );
		return false;
	}

	m_kID = kID;
	NiAVObject *pkObj = pkTriggerRoot->GetObjectByName( m_kID.c_str() );
	if( !InitTriggerPhysX(pkObj, fBuffer) )
	{
		//INFO_LOG( BM::LOG_LV0, __FL__<<L"Not Found Object["<<UNI(m_kID)<<L"]" );	//상위에서 로그 남김
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgGroundTrigger::IsInPos( POINT3 const &pt3Pos )const
{
	return (pt3Pos <= m_ptMax) && (pt3Pos >= m_ptMin);
}

void PgGroundTrigger::WriteToPacket(BM::Stream& rkPacket) const
{
	rkPacket.Push( m_kID );
	rkPacket.Push( m_kEnable );
}

void PgGroundTrigger::AddTimeEvent(TiXmlElement const *pkElement)
{ //xml로부터 트리거의 활성시간을 파싱
	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	SSIMPLETIMELIMIT kTimeLimit;

	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "FROM") )
		{
			::sscanf(pcAttrValue,"%d,%d",&kTimeLimit.kBegin.byHour,&kTimeLimit.kBegin.byMin);
		}
		else if( !::strcmp(pcAttrName, "TO") )
		{
			::sscanf(pcAttrValue,"%d,%d",&kTimeLimit.kEnd.byHour,&kTimeLimit.kEnd.byMin);
		}
		else if( !::strcmp(pcAttrName, "DAYOFWEEK") )
		{
			kTimeLimit.kDayOfWeek.SetDay(EDAYOFWEEK::EDOW_NONE);
			VEC_STRING kVec;
			PgStringUtil::BreakSep(pcAttrValue, kVec, "/");
			for(VEC_STRING::const_iterator it = kVec.begin(); it != kVec.end(); ++it)
			{
				if((*it) == "MON") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_MON); }
				else if((*it) == "TUE") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_TUE); }
				else if((*it) == "WED") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_WED); }
				else if((*it) == "THU") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_THU); }
				else if((*it) == "FRI") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_FRI); }
				else if((*it) == "SAT") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_SAT); }
				else if((*it) == "SUN") { kTimeLimit.kDayOfWeek.AddDay(EDAYOFWEEK::EDOW_SUN); }
			}
		}

		pkAttribute = pkAttribute->Next();
	}

	m_kContTimeEvent.push_back(kTimeLimit);
}

bool PgGroundTrigger::CheckEffectFromPlayer( PgPlayer *pPlayer, PgGround *pGround, BYTE const MoveType ){
/*/////////////////////////////////////////////////////////////////////////////////
	이펙트가 플레이어에게 걸려있는지 검사하는 함수.
	반드시 이 함수 밖에서 m_kCheckEffectNo 가 있는지 먼저 검사해야된다.
	if( m_kCheckEffectNo )
	{
		if( CheckEffectFromPlayer( pPlayer, pGround, MoveType ) ) { ... };
	}
/////////////////////////////////////////////////////////////////////////////////*/
	if( !pPlayer || !pGround )
	{
		return false;
	}
	if( MoveType & E_MOVE_EXPEDITIONMASTER )
	{ // 원정 대장만 이동을 요청 할 수 있다.
		if( pPlayer->HaveExpedition() )
		{// 원정대에 속해있을 경우 원정대장이 검사하면 된다.
			BM::GUID MasterGuid;
			if( pGround->GetExpeditionMasterGuid( pPlayer->GetExpeditionGuid(), MasterGuid ) )
			{
				if( pPlayer->GetID() == MasterGuid )
				{// 내가 원정대장
					if( pGround->CheckEffectFromExpeditionMember( pPlayer->ExpeditionGuid(), m_kCheckEffectNo ) )
					{
						return true;
					}
				}
			}
		}
	}
	else if( MoveType & E_MOVE_PARTYMASTER )
	{ // 파티 마스터만 이동을 요청 할 수 있다.
		if( pPlayer->HaveParty() )
		{// 파티에 속해있을 경우 파티장이 검사하면 된다.
			BM::GUID MasterGuid;
			if( pGround->GetPartyMasterGuid( pPlayer->GetPartyGuid(), MasterGuid ) )
			{
				if( pPlayer->GetID() == MasterGuid )
				{// 내가 파티장
					if( pGround->CheckEffectFromPartyMember( pPlayer->PartyGuid(), m_kCheckEffectNo ) )
					{
						return true;
					}
				}
			}
		}
	}
	else if( MoveType & E_MOVE_ANY_PARTYMEMMBER)
	{
		if( pPlayer->HaveParty() )
		{// 파티원 누구나 이동 요청 가능
			if( pGround->CheckEffectFromPartyMember( pPlayer->PartyGuid(), m_kCheckEffectNo ) )
			{
				return true;
			}
		}
	}
	else if( MoveType & E_MOVE_PERSONAL )
	{
		CEffect const *pEffect = pPlayer->FindEffect( m_kCheckEffectNo );
		if( pEffect )
		{
			return true;
		}
	}
	return false;
}

//============================================================================
// PgGTrigger_Portal
//----------------------------------------------------------------------------
PgGTrigger_Normal::PgGTrigger_Normal()
	: PgGroundTrigger()
{
}

PgGTrigger_Normal::PgGTrigger_Normal(PgGTrigger_Normal const& rhs)
	: PgGroundTrigger(rhs)
{
}

PgGTrigger_Normal::~PgGTrigger_Normal()
{
}

bool PgGTrigger_Normal::Build(GTRIGGER_ID const& kID, NiNode* pkTriggerRoot, TiXmlElement const* pkElement)
{
	if( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgGTrigger_Normal::Event(CUnit* pkUnit, PgGround* const pkGround, BM::Stream* const pkPacket)
{
	if( !pkUnit
	||	!pkGround
	||	!pkPacket )
	{
		return false;
	}

	if( !Enable() )
	{
		return false;
	}

	if( !IsInPos(pkUnit->GetPos()) )
	{
		return false;
	}
	return true;
}

//============================================================================
// PgGTrigger_Portal
//----------------------------------------------------------------------------
PgGTrigger_Portal::PgGTrigger_Portal(void)
	: PgGroundTrigger(), m_kContPortal()
{
}

PgGTrigger_Portal::PgGTrigger_Portal(PgGTrigger_Portal const& rhs)
	: PgGroundTrigger(rhs), m_kContPortal(rhs.m_kContPortal)
{
}

PgGTrigger_Portal::~PgGTrigger_Portal(void)
{
}

bool PgGTrigger_Portal::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEFMAP const * pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	if ( !pkContDefMap || !pkQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot, 100.0f ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		CONT_PORTAL_ACCESS::value_type kPortal;
		if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
		{
			m_kContPortal.push_back( kPortal );
		}
	}

	pkElement = pkElement->FirstChildElement();
	while ( pkElement )
	{
		if ( !::strcmp(pkElement->Value(), "ITEM") )
		{
			if ( m_kContPortal.empty() )
			{
				m_kContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type kPortal;
			if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
			{
				m_kContPortal.push_back( kPortal );
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}

	if( m_kContPortal.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	
	return !m_kContPortal.empty();
}

bool PgGTrigger_Portal::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	size_t iTargetIndex = 0;
	if ( pkPacket->Pop( iTargetIndex ) )
	{
		if ( m_kContPortal.size() > iTargetIndex )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				bool bSuccess = false;
				BYTE const kMoveType = m_kContPortal.at(iTargetIndex).GetMoveType();

				PgPortalAccess kAccess = m_kContPortal.at(iTargetIndex);
				kAccess.SetQuestPortal(pkPlayer); // 수행중인 퀘스트에 따라 이동할 맵 변경

				SReqMapMove_MT kRMM(MMET_None);
				kRMM.bIndunPartyDie = pkGround->IndunPartyDie();
				PgReqMapMove kMapMove( pkGround, kRMM, &kAccess );
				if ( kMapMove.Add( pkPlayer ) )
				{
					if( kMoveType & E_MOVE_EXPEDITIONMASTER )
					{ // 원정 대장만 이동을 요청 할 수 있다.
						if( pkPlayer->HaveExpedition() )
						{
							bSuccess = pkGround->AddExpeditionMember(pkPlayer, kMapMove);
							if( false == bSuccess )
							{
								// 원정대원 중 한명이라도 안되면 모두 이동 불가.
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}
					}
					else if( kMoveType & E_MOVE_PARTYMASTER )
					{ // 파티 마스터만 이동을 요청 할 수 있다.
						if( pkPlayer->HaveParty() )
						{
							bSuccess = pkGround->AddPartyMember(pkPlayer, kMapMove);
							if( false == bSuccess )
							{
								// 파티원 중 한명이라도 안되면 모두 이동 불가.
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}						
					}
					else if( kMoveType & E_MOVE_ANY_PARTYMEMMBER )
					{ // 파티원 누구나 이동 요청 가능.
						if( pkPlayer->HaveParty() )
						{
							bSuccess = pkGround->AddAnyPartyMember(pkPlayer, kMapMove);
							if( false == bSuccess )
							{
								// 파티원 중 한명이라도 안되면 모두 이동 불가.
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}
						}
					}

					if ( pkGround && CheckEffectNo() )
					{// 트리거에 체크할 이펙트번호가 있을 때만 검사한다.
						if( false == CheckEffectFromPlayer( pkPlayer, pkGround, kMoveType ) )
						{
							pkPlayer->SendWarnMessage( m_kErrorEffectMsgNo );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}

					if ( !bSuccess )
					{
						bSuccess = kMoveType & E_MOVE_PERSONAL;
					}

					if ( bSuccess )
					{
						return kMapMove.DoAction();
					}
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	// 잘못된 포탈입니다.
	pkUnit->SendWarnMessage( 18994 );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_PartyPortal
//----------------------------------------------------------------------------
PgGTrigger_PartyPortal::PgGTrigger_PartyPortal(void)
	: PgGroundTrigger(), m_kContPortal()
{
}

PgGTrigger_PartyPortal::PgGTrigger_PartyPortal(PgGTrigger_PartyPortal const& rhs)
	: PgGroundTrigger(rhs), m_kContPortal(rhs.m_kContPortal)
{
}

PgGTrigger_PartyPortal::~PgGTrigger_PartyPortal(void)
{
}

bool PgGTrigger_PartyPortal::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEFMAP const * pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	if ( !pkContDefMap || !pkQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot, 100.0f) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		CONT_PORTAL_ACCESS::value_type kPortal;
		if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
		{
			m_kContPortal.push_back( kPortal );
		}
	}

	pkElement = pkElement->FirstChildElement();
	while ( pkElement )
	{
		if ( !::strcmp(pkElement->Value(), "ITEM") )
		{
			if ( m_kContPortal.empty() )
			{
				m_kContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type kPortal;
			if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
			{
				m_kContPortal.push_back( kPortal );
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}

	if( m_kContPortal.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	
	return !m_kContPortal.empty();
}

bool PgGTrigger_PartyPortal::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	size_t iTargetIndex = 0;
	if ( pkPacket->Pop( iTargetIndex ) )
	{
		if ( m_kContPortal.size() > iTargetIndex )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				bool bSuccess = false;
				BYTE const kMoveType = m_kContPortal.at(iTargetIndex).GetMoveType();

				PgPortalAccess kAccess = m_kContPortal.at(iTargetIndex);
				kAccess.SetQuestPortal(pkPlayer); // 수행중인 퀘스트에 따라 이동할 맵 변경

				SReqMapMove_MT kRMM(MMET_None);
				kRMM.bIndunPartyDie = pkGround->IndunPartyDie();
				PgReqMapMove kMapMove( pkGround, kRMM, &kAccess );
				if ( kMapMove.Add( pkPlayer ) )
				{					
					if ( pkPlayer->HaveParty() )
					{
						// 파티원 누구나 이동 요청 가능
						bSuccess = pkGround->AddAnyPartyMember( pkPlayer, kMapMove );
						if( false == bSuccess )
						{
							// 파티원중 한명이라도 안되면 모두 이동 불가
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
					else
					{
						if ( !bSuccess )
						{
							bSuccess = kMoveType & E_MOVE_PERSONAL;
						}
					}

					if ( pkGround && CheckEffectNo() )
					{// 트리거에 체크할 이펙트번호가 있을 때만 검사한다.
						if( false == CheckEffectFromPlayer( pkPlayer, pkGround, E_MOVE_ANY_PARTYMEMMBER ) )
						{
							pkPlayer->SendWarnMessage( m_kErrorEffectMsgNo );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}

					if ( bSuccess )
					{
						return kMapMove.DoAction();
					}
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	// 잘못된 포탈입니다.
	pkUnit->SendWarnMessage( 18994 );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_PartyMemberPortal
//----------------------------------------------------------------------------
PgGTrigger_PartyMemberPortal::PgGTrigger_PartyMemberPortal(void)
	: PgGroundTrigger(), m_kContPortal()
{
}

PgGTrigger_PartyMemberPortal::PgGTrigger_PartyMemberPortal(PgGTrigger_PartyMemberPortal const& rhs)
	: PgGroundTrigger(rhs), m_kContPortal(rhs.m_kContPortal)
{
}

PgGTrigger_PartyMemberPortal::~PgGTrigger_PartyMemberPortal(void)
{
}

bool PgGTrigger_PartyMemberPortal::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEFMAP const * pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	if ( !pkContDefMap || !pkQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		CONT_PORTAL_ACCESS::value_type kPortal;
		if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
		{
			m_kContPortal.push_back( kPortal );
		}
	}

	pkElement = pkElement->FirstChildElement();
	while ( pkElement )
	{
		if ( !::strcmp(pkElement->Value(), "ITEM") )
		{
			if ( m_kContPortal.empty() )
			{
				m_kContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type kPortal;
			if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward  ) )
			{
				m_kContPortal.push_back( kPortal );
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}

	if( m_kContPortal.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	
	return !m_kContPortal.empty();
}

bool PgGTrigger_PartyMemberPortal::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	size_t iTargetIndex = 0;
	if ( pkPacket->Pop( iTargetIndex ) )
	{
		if ( m_kContPortal.size() > iTargetIndex )
		{
			PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkPlayer )
			{
				bool bSuccess = false;
				BYTE const kMoveType = m_kContPortal.at(iTargetIndex).GetMoveType();

				PgPortalAccess kAccess = m_kContPortal.at(iTargetIndex);
				kAccess.SetQuestPortal(pkPlayer); // 수행중인 퀘스트에 따라 이동할 맵 변경

				SReqMapMove_MT kRMM(MMET_None);
				kRMM.bIndunPartyDie = pkGround->IndunPartyDie();
				PgReqMapMove kMapMove( pkGround, kRMM, &kAccess );
				if ( kMapMove.Add( pkPlayer ) )
				{
					if ( pkPlayer->HaveParty() )
					{
						if( pkGround )
						{
							// 파티장만 이동가능
							BM::GUID kMasterGuid;
							if ( pkGround->GetPartyMasterGuid( pkPlayer->PartyGuid(), kMasterGuid ) )
							{
								if ( kMasterGuid != pkPlayer->GetID() )
								{
									pkPlayer->SendWarnMessage(80027);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}

						bSuccess = pkGround->AddPartyAllMember( pkPlayer, kMapMove );
						if( false == bSuccess )
						{
							int const iErr = kMapMove.GetLastAccessError()>0 ? kMapMove.GetLastAccessError() : 400906;
							pkPlayer->SendWarnMessage(iErr);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
					else
					{
						if ( !bSuccess )
						{
							bSuccess = kMoveType & E_MOVE_PERSONAL;
						}
					}

					if ( pkGround && CheckEffectNo() )
					{// 트리거에 체크할 이펙트번호가 있을 때만 검사한다.
						if( false == CheckEffectFromPlayer( pkPlayer, pkGround, E_MOVE_PARTYMASTER ) )
						{
							pkPlayer->SendWarnMessage( m_kErrorEffectMsgNo );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}

					if ( bSuccess )
					{
						return kMapMove.DoAction();
					}
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	// 잘못된 포탈입니다.
	pkUnit->SendWarnMessage( 18994 );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_RagPartyMemberPortal
//----------------------------------------------------------------------------
PgGTrigger_RagPartyMemberPortal::PgGTrigger_RagPartyMemberPortal(void)
	: PgGroundTrigger(), m_kContPortal()
{
}

PgGTrigger_RagPartyMemberPortal::PgGTrigger_RagPartyMemberPortal(PgGTrigger_RagPartyMemberPortal const& rhs)
	: PgGroundTrigger(rhs), m_kContPortal(rhs.m_kContPortal)
{
}

PgGTrigger_RagPartyMemberPortal::~PgGTrigger_RagPartyMemberPortal(void)
{
}

bool PgGTrigger_RagPartyMemberPortal::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEFMAP const * pkContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkContDefMap);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	if ( !pkContDefMap || !pkQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		CONT_PORTAL_ACCESS::value_type kPortal;
		if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward ) )
		{
			m_kContPortal.push_back( kPortal );
		}
	}

	pkElement = pkElement->FirstChildElement();
	while ( pkElement )
	{
		if ( !::strcmp(pkElement->Value(), "ITEM") )
		{
			if ( m_kContPortal.empty() )
			{
				m_kContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type kPortal;
			if ( kPortal.Build( pkElement, *pkContDefMap, *pkQuestReward  ) )
			{
				m_kContPortal.push_back( kPortal );
			}
		}
		pkElement = pkElement->NextSiblingElement();
	}

	if( m_kContPortal.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	
	return !m_kContPortal.empty();
}

bool PgGTrigger_RagPartyMemberPortal::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	size_t ModeNo = 0;
	if ( pkPacket->Pop( ModeNo ) )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( pkPlayer )
		{
			if( m_kContPortal.size() )
			{
				bool bSuccess = false;
				BYTE const kMoveType = m_kContPortal.at(0).GetMoveType();	// 인덱스를 0 으로 고정해도 되는지?
				PgPortalAccess kAccess = m_kContPortal.at(0);				// 위와 동일
				
				kAccess.SetModeNo(ModeNo);			// 레벨체크를 위해 입장하려는 모드의 번호를 저장해둔다.
				kAccess.SetTargetGroundKey(ModeNo);	// 입장하려는 모드의 그라운드 번호로 바꿔준다.

				SReqMapMove_MT kRMM(MMET_None);
				kRMM.bIndunPartyDie = pkGround->IndunPartyDie();
				PgReqMapMove kMapMove( pkGround, kRMM, &kAccess );
				if ( kMapMove.Add( pkPlayer ) )
				{
					if ( pkPlayer->HaveParty() )
					{
						if( pkGround )
						{
							// 파티장만 이동가능
							BM::GUID kMasterGuid;
							if ( pkGround->GetPartyMasterGuid( pkPlayer->PartyGuid(), kMasterGuid ) )
							{
								if ( kMasterGuid != pkPlayer->GetID() )
								{
									pkPlayer->SendWarnMessage(80027);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}

						bSuccess = pkGround->AddPartyAllMember( pkPlayer, kMapMove );
						if( false == bSuccess )
						{
							pkPlayer->SendWarnMessage(400906);							
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
					else
					{
						if ( !bSuccess )
						{
							bSuccess = kMoveType & E_MOVE_PERSONAL;
						}
					}

					if ( pkGround && CheckEffectNo() )
					{// 트리거에 체크할 이펙트번호가 있을 때만 검사한다.
						if( false == CheckEffectFromPlayer( pkPlayer, pkGround, E_MOVE_PARTYMASTER ) )
						{
							pkPlayer->SendWarnMessage( m_kErrorEffectMsgNo );
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}

					if ( bSuccess )
					{
						return kMapMove.DoAction();
					}
				}
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	// 잘못된 포탈입니다.
	pkUnit->SendWarnMessage( 18994 );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_Mission
//----------------------------------------------------------------------------
PgGTrigger_Mission::PgGTrigger_Mission(void)
:	PgGroundTrigger(), m_iMissionKey(0), m_iMissionNo(0)
{}

PgGTrigger_Mission::PgGTrigger_Mission(PgGTrigger_Mission const& rhs)
	: PgGroundTrigger(rhs), m_iMissionKey(rhs.m_iMissionKey), m_iMissionNo(rhs.m_iMissionNo)
{
}

PgGTrigger_Mission::~PgGTrigger_Mission(void)
{}

bool PgGTrigger_Mission::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEF_MISSION_ROOT const *pkMissionRoot = NULL;
	g_kTblDataMgr.GetContDef( pkMissionRoot );
	if ( !pkMissionRoot )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"CONT_DEF_MISSION_ROOT is NULL" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_iMissionNo = ::atoi( pcAttrValue );
			CONT_DEF_MISSION_ROOT::const_iterator itr = pkMissionRoot->find( m_iMissionNo );
			if ( itr != pkMissionRoot->end() )
			{
				m_iMissionKey = itr->second.iKey;
				return true;
			}

			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Not Found MissionNo["<<m_iMissionNo<<L"]" );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		pkAttribute = pkAttribute->Next();
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGTrigger_Mission::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	//-----------xml에서 설정된 시간 이벤트와 현재 시간을 비교하여 유효하지 않다면 리턴------
	if(!m_kContTimeEvent.empty())
	{
		bool bActive = false;
		SYSTEMTIME kNow;
		g_kEventView.GetLocalTime(&kNow);

		for(CONT_TIME_EVENT::const_iterator iterTimeEvent = m_kContTimeEvent.begin();
			iterTimeEvent != m_kContTimeEvent.end(); ++iterTimeEvent)
		{
			if((*iterTimeEvent).CheckTimeIsInDuration(kNow))
			{
				bActive = true;
			}
		}

		if(!bActive)
		{
			pkUnit->SendWarnMessage(803);
			return false;
		}
	}
/*
	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
*/
	return MissionEvent(pkUnit, pkGround, pkPacket);
}

bool PgGTrigger_Mission::MissionEvent( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);//맵서버가 받아서 셋팅한다
	if(pkPlayer)
	{
// 		if ( NULL != pkPlayer->GetEffect(PgThrowUpPenalty::ms_iEffectNo) )
// 		{
// 			// 패널티 상태에서는 미션에 입장 할 수 없습니다.
// 			pkPlayer->SendWarnMessage(80016);
// 			return false;
// 		}

		if( !pkGround )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"pkGround is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		BM::GUID kMasterGuid;
		//if ( g_kLocalPartyMgr.GetPartyMasterGuid( pkPlayer->PartyGuid(), kMasterGuid ) )
		if ( pkGround->GetPartyMasterGuid( pkPlayer->PartyGuid(), kMasterGuid ) )
		{
			if ( kMasterGuid != pkPlayer->GetID() )
			{
				if ( pkGround->CheckUnit(kMasterGuid) )
				{// 파티 마스터랑 같은 그라운드에 있으면 미션에 들어 갈 수 없다.
					pkPlayer->SendWarnMessage(80027);
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}

		if ( INT_MAX == pkPlayer->GetAbil( AT_MISSION_THROWUP_PENALTY ) )
		{
			// 패널티가 있으면 체크를 해야 한다.
			PgRequest_CheckPenalty kCheckPenalty( pkGround->GroundKey(), static_cast<WORD>(AT_MISSION_THROWUP_PENALTY), pkPacket );
			kCheckPenalty.DoAction( pkPlayer );
			return false;
		}

		int iLevel = 0;
		if ( !pkPacket->Pop( iLevel ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int iType = 0;
		int iEventMissionKey = m_iMissionKey;

		switch( GetType() )
		{
		case GTRIGGER_TYPE_MISSION_NPC:
			{
				iType = MT_EVENT_MISSION;
			}break;
		case GTRIGGER_TYPE_MISSION_EVENT_NPC:
			{
				iType = MT_EVENT_MISSION_NPC;
			}break;
		case GTRIGGER_TYPE_MISSION_EASY:
			{
				iType = MT_MISSION_EASY;
			}break;
		case GTRIGGER_TYPE_MISSION_EVENT_NOT_HAVE_ARCADE:
			{
				iType = MT_EVENT_MISSION_NOT_HAVE_ARCADE;
			}break;
		case GTRIGGER_TYPE_MISSION_UNUSED_GADACOIN:
			{
				iType = MT_MISSION_UNUSED_GADACOIN;
			}break;
		default:
			{
				iType = MT_MISSION;
			}break;
		}
		//int iType = static_cast<int>( (GTRIGGER_TYPE_MISSION_NPC == GetType()) ? MT_EVENT_MISSION : MT_MISSION );
		// 히든 맵은 아직 열리지 않아야 된다.
		//if( MT_MISSION != static_cast<EMissionTypeKind>(iType) )
		{
			if( MT_EVENT_MISSION_NPC == static_cast<EMissionTypeKind>(iType) )
			{
				/*int iEventMissionNo = 0;
				if ( !pkPacket->Pop( iEventMissionNo ) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
				if( 0 > (m_iMissionKey + iEventMissionNo) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
				iEventMissionKey = m_iMissionKey + iEventMissionNo;*/
			}
			else if( MT_EVENT_MISSION == static_cast<EMissionTypeKind>(iType) )
			{
				int const iMissionEventAllClear = 0x000F;
				if( iMissionEventAllClear == (iMissionEventAllClear & pkPlayer->GetAbil(AT_MISSION_EVENT)) )
				{
					// 미션 이벤트 맵을 모두 "SSS" 등급으로 클리어 했다. 히든 맵이 등장한다.
					iType = static_cast<int>(MT_EVENT_HIDDEN);
				}
			}
		}
		/////////////////////////////////////

		if ( 0 == iLevel )
		{
			PgRequest_MissionInfo kReq( iEventMissionKey, pkGround->GroundKey(), iType );
			if ( kReq.DoAction( pkPlayer ) )
			{
				return true;
			}
			pkPlayer->SendWarnMessage(400225);
		}
		else
		{
			PgPlayer_MissionData const *pkMissionData = pkPlayer->GetMissionData( static_cast<unsigned int>(iEventMissionKey) );
			if ( pkMissionData )
			{
				PgMissionInfo const *pkMissionInfo = NULL;
				if( g_kMissionMan.GetMissionKey( iEventMissionKey, pkMissionInfo) )
				{
					// Client에서는 레벨이 1부터 오고 서버에서는 레벨은 0부터이다.
					SMissionKey kMissionKey( iEventMissionKey, --iLevel );

					SMissionOptionMissionOpen const *pkMissionOpen = NULL;
					if ( pkMissionInfo->GetMissionOpen( kMissionKey.iLevel, pkMissionOpen) )
					{
						if ( !pkMissionData->IsPlayingLevel(pkMissionOpen->m_kLimit_PreLevelValue) )
						{
							pkPlayer->SendWarnMessage(401198); //카오스 모드는 아케이드 모드를 완료해야 입장할 수 있습니다.
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						if( (false == pkMissionData->IsClearLevel(iLevel)) && (0 == (GATTR_FLAG_CONSTELLATION & pkGround->GetAttr())) )
						{
							// Clear가 되지 않은 경우만 Quest 체크한다.
							PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
							if(!pkMyQuest)
							{
								pkPlayer->SendWarnMessage(400225);
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}

							if( DEFENCE7_MISSION_LEVEL == (iLevel+1) )
							{
								VEC_INT	VecClearQuestValue;
								VecClearQuestValue.clear();
								pkMissionOpen->GetClearQuestIDVec(VecClearQuestValue);
								bool const bClearRet = pkMyQuest->IsEndedQuestVec(VecClearQuestValue);
								if( (false == bClearRet) && ( VecClearQuestValue.size() != 0) )
								{
									pkPlayer->SendWarnMessage(8016);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}

							VEC_INT	VecIngQuestValue;
							VecIngQuestValue.clear();
							pkMissionOpen->GetIngQuestIDVec(VecIngQuestValue);
							bool const bIngRet = pkMyQuest->IsIngQuestVec(VecIngQuestValue);
							bool const bEndRet = pkMyQuest->IsEndedQuestVec(VecIngQuestValue);

							if( false == (bIngRet || bEndRet) && ( VecIngQuestValue.size() != 0))
							{
								int const iErrorTTW = pkMissionInfo->GetErrorText();
								pkPlayer->SendWarnMessage(iErrorTTW);
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}

							if( pkUnit )
							{
								VEC_INT	VecEffectValue;
								VecEffectValue.clear();
								pkMissionOpen->GetEffectNo(VecEffectValue);

								PgAction_MissionEffectCheck kCheckAction( VecEffectValue, pkGround, pkGround->GroundKey(), true );
								if( false == kCheckAction.DoAction( pkUnit, NULL ) )
								{
									pkPlayer->SendWarnMessage(400645);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}
						else
						{
							VEC_GUID kGuidVec;
							pkGround->GetPartyMemberGround( pkPlayer->PartyGuid(), pkGround->GroundKey(), kGuidVec );

							PgPlayer *pkPartyMember = NULL;
							VEC_GUID::iterator user_itr = kGuidVec.begin();
							for ( ; user_itr!=kGuidVec.end(); ++user_itr )
							{
								pkPartyMember = dynamic_cast<PgPlayer*>(pkGround->GetUnit( *user_itr ));
								if( pkPartyMember )
								{
									if ( pkPartyMember->GetID() != pkPlayer->GetID() )
									{
										if ( INT_MAX == pkPartyMember->GetAbil( AT_MISSION_THROWUP_PENALTY ) )
										{
											PgRequest_CheckPenalty kCheckPenalty( pkGround->GroundKey(), static_cast<WORD>(AT_MISSION_THROWUP_PENALTY), NULL );
											kCheckPenalty.DoAction( pkPartyMember );
										}
									}
								}
							}
						}

						if( GTRIGGER_TYPE_MISSION_NPC == GetType() )
						{
							// Event Map Quest Check(Party All Member)
							// Quest All Check 모두 설정된 퀘스트가 진행/완료가 되어야 된다.
							VEC_INT	VecIngQuestValue;
							VecIngQuestValue.clear();
							pkMissionOpen->GetIngQuestIDVec(VecIngQuestValue);
							PgAction_MissionEventQuestCheck kCheckQuestAction( VecIngQuestValue, pkGround, pkGround->GroundKey() );
							if( false == kCheckQuestAction.DoAction( pkUnit, NULL ) )
							{
								pkPlayer->SendWarnMessage(790551);
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
								return false;
							}

							if( MT_EVENT_HIDDEN == static_cast<EMissionTypeKind>(iType) )
							{
								PgAction_MissionEventHiddenRankCheck kCheckHiddenRankAction(pkGround, pkGround->GroundKey());
								if( false == kCheckHiddenRankAction.DoAction( pkUnit, NULL ) )
								{
									pkPlayer->SendWarnMessage(790552);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}

								
								// 이벤트 맵에서는 레벨 6을 파티장이 들어 갈 경우, 파티원 모두 5레벨을 클리어 해야만 된다.
								// 그렇지 않은 경우는 5레벨만 입장이 가능하다.
								PgAction_MissionEventHiddenLevelClearCheck kCheckHiddenLevelClearCheck(iEventMissionKey, iLevel, pkGround, pkGround->GroundKey());
								if( false == kCheckHiddenLevelClearCheck.DoAction( pkUnit, NULL ))
								{
									pkPlayer->SendWarnMessage(790553);
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}

						// 파티가 있는 경우 모든 파티원이 같은 맵에 존재해야 들어 갈수가 있다.
						PgAction_MissionPartyMemberCheck kPartyMemverCheck( pkGround );
						if( false == kPartyMemverCheck.DoAction( pkPlayer, NULL ) )
						{
							pkPlayer->SendWarnMessage(400906);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						PgRequest_MissionJoin kReq( kMissionKey, pkGround, iType, pkPacket );
						if ( kReq.DoAction( pkPlayer ) )
						{
							return true;
						}
					}
					else
					{
						if( MISSION_LEVEL_MAX == (iLevel+1) )
						{
							pkPlayer->SendWarnMessage(401070);

							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"pkMissionOpen is NULL Key["<<kMissionKey.iKey<<L"] Level["<<kMissionKey.iLevel<<L"]" );
					}
				}
				pkPlayer->SendWarnMessage(400225);
			}
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_Mission_Npc
//----------------------------------------------------------------------------
PgGTrigger_Mission_Npc::PgGTrigger_Mission_Npc(void)
	: PgGTrigger_Mission()
{
}

PgGTrigger_Mission_Npc::PgGTrigger_Mission_Npc(PgGTrigger_Mission_Npc const& rhs)
	: PgGTrigger_Mission(rhs)
{
}

PgGTrigger_Mission_Npc::~PgGTrigger_Mission_Npc(void)
{
}

//============================================================================
// PgGTrigger_Mission_Event_Npc
//----------------------------------------------------------------------------
PgGTrigger_Mission_Event_Npc::PgGTrigger_Mission_Event_Npc(void)
	: PgGTrigger_Mission()
{
}

PgGTrigger_Mission_Event_Npc::PgGTrigger_Mission_Event_Npc(PgGTrigger_Mission_Event_Npc const& rhs)
	: PgGTrigger_Mission(rhs)
{
}

PgGTrigger_Mission_Event_Npc::~PgGTrigger_Mission_Event_Npc(void)
{
}

//============================================================================
// PgGTrigger_Mission_Event_Not_Have_Arcade
//----------------------------------------------------------------------------
PgGTrigger_Mission_Event_Not_Have_Arcade::PgGTrigger_Mission_Event_Not_Have_Arcade(void)
	: PgGTrigger_Mission()
{
}

PgGTrigger_Mission_Event_Not_Have_Arcade::PgGTrigger_Mission_Event_Not_Have_Arcade(PgGTrigger_Mission_Event_Not_Have_Arcade const& rhs)
	: PgGTrigger_Mission(rhs)
{
}

PgGTrigger_Mission_Event_Not_Have_Arcade::~PgGTrigger_Mission_Event_Not_Have_Arcade(void)
{
}

//============================================================================
// PgGTrigger_Mission_Easy
//----------------------------------------------------------------------------
PgGTrigger_Mission_Easy::PgGTrigger_Mission_Easy(void)
	: PgGTrigger_Mission()
{
}

PgGTrigger_Mission_Easy::PgGTrigger_Mission_Easy(PgGTrigger_Mission_Easy const& rhs)
	: PgGTrigger_Mission(rhs)
{
}

PgGTrigger_Mission_Easy::~PgGTrigger_Mission_Easy(void)
{
}

//============================================================================
// PgGTrigger_Mission_Unused_GadaCoin
//----------------------------------------------------------------------------
PgGTrigger_Mission_Unused_GadaCoin::PgGTrigger_Mission_Unused_GadaCoin(void)
	: PgGTrigger_Mission()
{
}

PgGTrigger_Mission_Unused_GadaCoin::PgGTrigger_Mission_Unused_GadaCoin(PgGTrigger_Mission_Unused_GadaCoin const& rhs)
	: PgGTrigger_Mission(rhs)
{
}

PgGTrigger_Mission_Unused_GadaCoin::~PgGTrigger_Mission_Unused_GadaCoin(void)
{
}

//============================================================================
// PgGTrigger_Hidden_Portal
//----------------------------------------------------------------------------
PgGTrigger_Hidden_Portal::PgGTrigger_Hidden_Portal(void)
	: PgGroundTrigger(), m_iParam(0)
{
}

PgGTrigger_Hidden_Portal::PgGTrigger_Hidden_Portal(PgGTrigger_Hidden_Portal const& rhs)
	: PgGroundTrigger(rhs), m_iParam(rhs.m_iParam)
{
}

PgGTrigger_Hidden_Portal::~PgGTrigger_Hidden_Portal(void)
{
}

bool PgGTrigger_Hidden_Portal::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_iParam = ::atoi( pcAttrValue );
		}
		pkAttribute = pkAttribute->Next();
	}

	return m_iParam > 0;
}

bool PgGTrigger_Hidden_Portal::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// 제한 입장 시작 체크
	// Error Msg : 400987
	SYSTEMTIME kNow;
	::GetLocalTime( &kNow );

	int iOpenMinValue = 0;
	if( S_OK != g_kVariableContainer.Get(EVar_Kind_Hidden, EVar_Hidden_Open_Min, iOpenMinValue) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find EVar_Kind_Hidden 'EVar_Hidden_Open_Min' Value") );
	}

	if( (0 > iOpenMinValue) || (59 < iOpenMinValue) )
	{
		iOpenMinValue = 10;
	}

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pkPlayer )
	{
		if( (0 == (kNow.wHour%2)) && (iOpenMinValue >= kNow.wMinute) )
		{
			// 짝수 시간 Open

			const CONT_DEFMAP* pkContDefMap = NULL;
			g_kTblDataMgr.GetContDef(pkContDefMap);

			if(!pkContDefMap)
			{
				INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find ContDefMap");
				return false;
			}

			GET_DEF(CItemDefMgr, kItemDefMgr);
			CItemDef const* pItemDef = kItemDefMgr.GetDef(m_iParam);
			if(!pItemDef)
			{
				INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find HiddenPortal ItemNo");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			int const iMapNo = pItemDef->GetAbil(AT_MAP_NUM);

			CONT_DEFMAP::const_iterator itor = pkContDefMap->find(iMapNo);
			if (itor == pkContDefMap->end())
			{
				INFO_LOG(BM::LOG_LV0, __FL__ << L"Cannot find DefMap MapNo[" << iMapNo << "]" );
				return false;
			}

			const TBL_DEF_MAP& rkDefMap = itor->second;

			if( false == pkPlayer->GetHiddenOpen()->IsComplete(rkDefMap.sHiddenIndex) )
			{
				BM::Stream kPacket( PT_M_C_REQ_HIDDEN_MOVE_CHECK );
				kPacket.Push( pkPlayer->GetID() );
				kPacket.Push( m_iParam );
				pkPlayer->Send(kPacket);

				return true;
			}
			else
			{
				// 하루에 한번만 입장 가능
				pkPlayer->SendWarnMessage( 400990 );
			}

			return false;
		}
		else
		{
			pkPlayer->SendWarnMessage( 400987 );
		}
	}
	//



	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
//============================================================================
// PgGTrigger_InMission
//----------------------------------------------------------------------------
PgGTrigger_InMission::PgGTrigger_InMission(void)
	: PgGroundTrigger(), m_iType(0)
{
}

PgGTrigger_InMission::PgGTrigger_InMission(PgGTrigger_InMission const& rhs)
	: PgGroundTrigger(rhs), m_iType(rhs.m_iType)
{
}

PgGTrigger_InMission::~PgGTrigger_InMission(void)
{}

bool PgGTrigger_InMission::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot, 100.0f ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_iType = ::atoi( pcAttrValue );
		}
		pkAttribute = pkAttribute->Next();
	}

	return m_iType > 0;
}

bool PgGTrigger_InMission::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const /*pkPacket*/ )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if( NULL == pPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	PgAction_MissionPartyMemberCheck kPartyMemverCheck( pkGround );
	if( false == kPartyMemverCheck.DoAction( pPlayer, NULL ) )
	{
		pPlayer->SendWarnMessage(400906);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMissionGround *pkMissionGnd = dynamic_cast<PgMissionGround*>(pkGround);
	if ( pkMissionGnd )
	{
		size_t const iNowStage = pkMissionGnd->GetStage();
		return pkMissionGnd->SwapStage_Before( iNowStage + 1, pkUnit );
	}	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_PortalEmporia
//----------------------------------------------------------------------------
PgGTrigger_PortalEmporia::PgGTrigger_PortalEmporia(void)
	: PgGroundTrigger(), m_kEmporiaKey()
{
}

PgGTrigger_PortalEmporia::PgGTrigger_PortalEmporia( PgGTrigger_PortalEmporia const &rhs )
:	PgGroundTrigger(rhs)
,	m_kEmporiaKey(rhs.m_kEmporiaKey)
{
}

PgGTrigger_PortalEmporia& PgGTrigger_PortalEmporia::operator=( PgGTrigger_PortalEmporia const &rhs )
{
	PgGroundTrigger::operator = ( rhs );
	m_kEmporiaKey = rhs.m_kEmporiaKey;
	return *this;
}

PgGTrigger_PortalEmporia::~PgGTrigger_PortalEmporia(void)
{
}

bool PgGTrigger_PortalEmporia::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	CONT_DEF_EMPORIA const * pkContDefEmporia = NULL;
	g_kTblDataMgr.GetContDef(pkContDefEmporia);

	if ( !pkContDefEmporia )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEF_EMPORIA is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "GUID") )
		{
			m_kEmporiaKey.kID.Set( std::string(pcAttrValue) );
		}
		else if ( !::strcmp(pcAttrName, "PARAM") )
		{
			m_kEmporiaKey.byGrade = static_cast<BYTE>(::atoi(pcAttrValue));
		}
		pkAttribute = pkAttribute->Next();
	}

	if ( pkContDefEmporia->find( m_kEmporiaKey.kID ) == pkContDefEmporia->end() )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Not Found EmporiaID<") << m_kEmporiaKey.kID << _T("> TriggerID <") << kID << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgGTrigger_PortalEmporia::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pkPlayer )
	{
		if ( pkPlayer->HaveParty() )
		{
			// 파티가 있으면 진입할 수 없습니다.
			INFO_LOG( BM::LOG_LV5, _T("[ReqJoinEmporia] FAILED ") << pkPlayer->Name() << _T("<") << pkPlayer->GetID() << _T("> Had Party") );
			pkPlayer->SendWarnMessage( 19013 );
		}
		else
		{
			SReqMapMove_MT kRMM(MMET_None);
			kRMM.nTargetPortal = 1;
			kRMM.kCasterKey = pkGround->GroundKey();
			kRMM.kCasterSI = g_kProcessCfg.ServerIdentity();

			BM::Stream kCheckPacket( PT_M_N_REQ_MAP_MOVE_CHECK, pkPlayer->GetID() );
			kCheckPacket.Push(kRMM);
			kCheckPacket.Push(pkPlayer->GuildGuid());
			kCheckPacket.Push(false);
			kCheckPacket.Push(m_kEmporiaKey);
			
			::SendToRealmContents( PMET_EMPORIA, kCheckPacket );
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_TeleMove
//----------------------------------------------------------------------------
PgGTrigger_TeleMove::PgGTrigger_TeleMove(void)
	: PgGroundTrigger(), m_kTargetPos(), m_kTargetID(), m_bIsBossTrigger(false)
{
}

PgGTrigger_TeleMove::PgGTrigger_TeleMove(PgGTrigger_TeleMove const& rhs)
	: PgGroundTrigger(rhs), m_kTargetPos(rhs.m_kTargetPos), m_kTargetID(rhs.m_kTargetID), m_bIsBossTrigger(rhs.m_bIsBossTrigger)
{
}

PgGTrigger_TeleMove::~PgGTrigger_TeleMove(void)
{}

bool PgGTrigger_TeleMove::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM_STRING") )
		{
			m_kTargetID = pcAttrValue;
		}
		else if( !::strcmp(pcAttrName, "IS_BOSS_TRIGGER") )
		{
			m_bIsBossTrigger = static_cast<bool>(atoi( pcAttrValue ));
		}
		pkAttribute = pkAttribute->Next();
	}

	return !m_kTargetID.empty();
}

bool PgGTrigger_TeleMove::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if(!pkPacket)
	{
		return false;
	}

	if( !Enable() )
	{
		return false;
	}

	int iType = 0;
	pkPacket->Pop(iType);
	std::string kTriggerID;
	pkPacket->Pop(kTriggerID);
	POINT3 kNowPos;
	pkPacket->Pop(kNowPos);

	switch( iType )
	{
	case TELE_PARTY_MOVE_OTHER:
	case TELE_PARTY_MOVE_ROCKET_OTHER:
		{
			// 위치 검사 없음
		}break;
	case TELE_PARTY_MOVE:
	case TELE_PARTY_MOVE_ROCKET:
		{
			if( (TELE_PARTY_MOVE == iType || TELE_PARTY_MOVE_ROCKET == iType)
			&&	BM::GUID::IsNotNull(pkUnit->GetPartyGuid()) )
			{
				VEC_GUID kContGuid;
				if( (pkGround->GetAttr() & GKIND_EXPEDITION) ) // 그라운드가 원정대 던전.
				{
					PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if( pPlayer )
					{
						if( pPlayer->HaveExpedition() )			// 원정대에 속해있다.
						{
							pkGround->GetExpeditionMember(pPlayer->ExpeditionGuid(), kContGuid);
						}
					}
				}
				else
				{
					pkGround->GetPartyMember(pkUnit->GetPartyGuid(), kContGuid);
				}
				VEC_GUID::const_iterator iter = kContGuid.begin();
				while( kContGuid.end() != iter )
				{
					if( pkUnit->GetID() != (*iter) )
					{
						PgPlayer* pkOtherPlayer = dynamic_cast< PgPlayer* >(pkGround->GetUnit((*iter)));
						if( pkOtherPlayer
						&&	!pkOtherPlayer->IsAlive() )
						{
							int const iMessageNo = 400649;
							BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, iMessageNo );
							kPacket.Push( static_cast< BYTE >(EL_Warning) );
							pkGround->Broadcast(kPacket);

							BM::Stream kFailedPacket(PT_M_C_ANS_FAILED_HYPER_MOVE);
							kFailedPacket.Push( iType );
							pkUnit->Send(kFailedPacket);
							return false;
						}
					}
					++iter;
				}
			}
		} // no break;
	case TELE_MOVE:
	case TELE_JUMP:
	case TELE_MOVE_DIRECT_OTHER_REQ:
	default:
		{
			if ( !IsInPos( pkUnit->GetPos() ) )
			{
				if ( !IsInPos( kNowPos ) )	//안전장치. 클라에서 보내준 데이터.
				{
					// 포탈을 벗어났습니다.
					//pkUnit->SendWarnMessage( 18995 );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
		}break;
	}
	if( POINT3::NullData() == m_kTargetPos )
	{
		m_kTargetPos = pkGround->GetNodePosition(m_kTargetID.c_str());
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(pkPlayer)	//점프 시작
	{
		pkPlayer->SetHyperMoveInfo(static_cast<HYPERMOVE_TYPE>(iType), BM::GetTime32(), m_kTargetPos);

		if( (TELE_PARTY_MOVE == iType 
			|| TELE_PARTY_MOVE_ROCKET == iType 
			|| TELE_MOVE_DIRECT_OTHER_REQ == iType)
		&&	BM::GUID::IsNotNull(pkPlayer->GetPartyGuid()) )
		{
			VEC_GUID kContGuid;
			if( (pkGround->GetAttr() & GKIND_EXPEDITION) ) // 그라운드가 원정대 던전.
			{
				PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if( pPlayer )
				{
					if( pPlayer->HaveExpedition() )			// 원정대에 속해있다.
					{
						pkGround->GetExpeditionMember(pPlayer->ExpeditionGuid(), kContGuid);
					}
				}
			}
			else
			{
				pkGround->GetPartyMember(pkUnit->GetPartyGuid(), kContGuid);
			}
			VEC_GUID::const_iterator iter = kContGuid.begin();
			while( kContGuid.end() != iter )
			{
				if( pkPlayer->GetID() != (*iter) )
				{
					PgPlayer* pkOtherPlayer = dynamic_cast< PgPlayer* >(pkGround->GetUnit((*iter)));
					if( pkOtherPlayer )
					{
						BM::Stream kPacket(PT_M_C_NFY_PARTY_TELE_PORT);
						kPacket.Push( kTriggerID );
						switch(iType)
						{
						case TELE_PARTY_MOVE:
							{
								kPacket.Push( static_cast< int >(TELE_PARTY_MOVE_OTHER) );
							}break;
						case TELE_PARTY_MOVE_ROCKET:
							{
								kPacket.Push( static_cast< int >(TELE_PARTY_MOVE_ROCKET_OTHER) );
							}break;
						case TELE_MOVE_DIRECT_OTHER_REQ:
							{
								kPacket.Push( static_cast< int >(TELE_MOVE_DIRECT_OTHER_ACT) );
							}break;
						}
						pkOtherPlayer->Send(kPacket);
					}
				}
				++iter;
			}
		}

		if( pkUnit->GetPartyGuid().IsNotNull() )
		{
			PgGroundUtil::SendPartyMgr_Refuse(pkUnit->GetID(), EPR_BOSSROOM, m_bIsBossTrigger);
		}

		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_BattleArea
//----------------------------------------------------------------------------
PgGTrigger_BattleArea::PgGTrigger_BattleArea(void)
	:	PgGTrigger_Normal(), m_iParam(0)
{}

PgGTrigger_BattleArea::~PgGTrigger_BattleArea(void)
{
}

bool PgGTrigger_BattleArea::InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer)
{
	if( !pkObj )
	{
		return false;
	}

	//NiPoint3 ptPos = pkObj->GetWorldTranslate();
	NiBound const& kBound = pkObj->GetWorldBound();
	m_ptMin = m_ptMax = POINT3( kBound.GetCenter().x, kBound.GetCenter().y ,kBound.GetCenter().z );

	// 엠포리아 방어전 용도
	float const fRadius = kBound.GetRadius();
	float const fAddValue = ::sqrt( ( fRadius * fRadius ) / 3.0f + 1.0f ) + fBuffer; // 1.0은 나누기로 손실된 값의 보정용
	m_ptMin -= fAddValue;
	m_ptMax += fAddValue;
	return true;
}

PgGTrigger_BattleArea::PgGTrigger_BattleArea( PgGTrigger_BattleArea const &rhs )
:	PgGTrigger_Normal(rhs)
,	m_iParam(rhs.m_iParam)
{
}

PgGTrigger_BattleArea& PgGTrigger_BattleArea::operator=( PgGTrigger_BattleArea const &rhs )
{
	PgGTrigger_Normal::operator = ( rhs );
	m_iParam = rhs.m_iParam;
	return *this;
}

bool PgGTrigger_BattleArea::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot, 0.0f ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_iParam = ::atoi( pcAttrValue );
		}
		pkAttribute = pkAttribute->Next();
	}

	return 0 != m_iParam;
}

//============================================================================
// PgGTrigger_TransTower
//----------------------------------------------------------------------------
PgGTrigger_TransTower::PgGTrigger_TransTower(void)
:	m_i64SaveMoney(0i64)
{
}

PgGTrigger_TransTower::PgGTrigger_TransTower(PgGTrigger_TransTower const& rhs)
:	PgGTrigger_Normal(rhs)
,	m_kTowerID(rhs.m_kTowerID)
,	m_i64SaveMoney(rhs.m_i64SaveMoney)
{
}

PgGTrigger_TransTower::~PgGTrigger_TransTower(void)
{
}

bool PgGTrigger_TransTower::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_i64SaveMoney = 0i64;

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "GUID") )
		{
			m_kTowerID.Set( std::string(pcAttrValue) );
		}
		else if ( !::strcmp(pcAttrName, "PARAM") )
		{
			m_i64SaveMoney = ::_atoi64( pcAttrValue );
		}

		pkAttribute = pkAttribute->Next();
	}

	CONT_DEF_TRANSTOWER const *pkDefTransTower = NULL;
	g_kTblDataMgr.GetContDef( pkDefTransTower );

	CONT_DEF_TRANSTOWER::const_iterator itr = pkDefTransTower->find( m_kTowerID );
	if ( itr != pkDefTransTower->end() )
	{
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found TransTower ID<" << m_kTowerID << L"> in DEF_TRANSTOWER");
	LIVE_CHECK_LOG( BM::LOG_LV4, L"Return false");
	return false;
}

bool PgGTrigger_TransTower::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
//		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return Event( pkUnit, pkGround, pkPacket, m_kTowerID, m_i64SaveMoney );
}

bool PgGTrigger_TransTower::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket, BM::GUID const &kTowerID, __int64 const i64SaveMoney, bool const bByPetSkill )
{
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( !pkPlayer )
	{
		LIVE_CHECK_LOG( BM::LOG_LV0, __FL__ << L"Dynamic case Error" );
		return false;
	}

	int iType = 0;
	pkPacket->Pop( iType );

	try
	{
		__int64 const i64HaveMoney = pkUnit->GetAbil64( AT_MONEY );

		switch ( iType )
		{
		case E_TRANSTOWER_SAVEPOS:
			{
				// 돈이 충분히 있는지 검사
				if ( i64HaveMoney < i64SaveMoney )
				{
					throw 700036;// 소지금이 부족합니다.
				}

				CONT_PLAYER_MODIFY_ORDER kOrder;

				if ( i64SaveMoney )
				{
					SPMOD_Add_Money kDelMoneyData(-i64SaveMoney);//필요머니 빼기.
					SPMO kIMO(IMET_ADD_MONEY, pkUnit->GetID(), kDelMoneyData);
					kOrder.push_back(kIMO);
				}
				
				{
					SRecentInfo kRecentInfo( pkGround->GetGroundNo(), pkUnit->GetPos() );
					SPMO kIMO( IMET_TRANSTOWER_SAVE_RECENT, pkUnit->GetID(), kRecentInfo );
					kOrder.push_back(kIMO);
				}

				PgAction_ReqModifyItem kItemModifyAction( CIE_TransTower_Save_Recent, pkGround->GroundKey(), kOrder );
				kItemModifyAction.DoAction( pkUnit, NULL );
			}break;
		case E_TRANSTOWER_MAPMOVE:
			{
				TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;
				bool bDisCountItem = false;
				if (	!pkPacket->Pop( kTargetKey )
					||	!pkPacket->Pop( bDisCountItem )
					)
				{
					throw 0;
				} 

				CONT_DEF_TRANSTOWER const *pkDefTransTower = NULL;
				g_kTblDataMgr.GetContDef( pkDefTransTower );

				CONT_DEF_TRANSTOWER::const_iterator def_itr = pkDefTransTower->find( kTowerID );
				if ( def_itr == pkDefTransTower->end() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found TransTower ID<" << kTowerID << L"> in DEF_TRANSTOWER");
					throw 0;
				}

				TBL_DEF_TRANSTOWER_TARGET kTarget( kTargetKey );
				CONT_DEF_TRANSTOWER_TARGET::const_iterator target_itr = def_itr->second.find( kTarget );
				if ( target_itr == def_itr->second.end() )
				{
					throw 0;
				}

				if ( FAILED(pkPlayer->IsOpenWorldMap( kTarget.iGroundNo )) )
				{
					// 이펙트가 존재하는지 찾아본다.
					if ( !pkPlayer->IsItemEffect( EFFECTNO_TRANSTOWER_FREE ) )
					{
						CONT_DEFMAP const *pkDefMap = NULL;
						g_kTblDataMgr.GetContDef( pkDefMap );

						int iContinent = 0;
						CONT_DEFMAP::const_iterator map_itr = pkDefMap->find( kTarget.iGroundNo );
						if ( map_itr != pkDefMap->end() )
						{
							iContinent = static_cast<int>(map_itr->second.sContinent);
						}

						if (	!iContinent 
							||	!pkPlayer->IsItemEffect( EFFECTNO_TRANSTOWER_FREE + iContinent ) )
						{
							// 이동한 맵이 아님
							throw 16;
						}
					}
				}

				kTarget = *target_itr;

				//순간이동 비용할인(펫스킬)
				PgPet* pkPet = pkGround->GetPet(pkPlayer);
				if(pkPet)
				{
					kTarget.i64Price -= std::max( SRateControl::GetValueRate( kTarget.i64Price , static_cast<__int64>(pkPet->GetAbil(AT_TRANS_DC_PET_SKILL)) ), 0i64 ); 
				}

				//프리미엄 할인
				if( S_PST_TranstowerDiscount const* pkPremiumDiscount = pkPlayer->GetPremium().GetType<S_PST_TranstowerDiscount>() )
				{
					kTarget.i64Price -= std::max( SRateControl::GetValueRate(kTarget.i64Price, static_cast<__int64>(pkPremiumDiscount->iDiscount)), 0i64);
				}

				if ( true == bDisCountItem )
				{
					SItemPos kDisCountItemPos;
					pkPacket->Pop( kDisCountItemPos );

					PgBase_Item kDisCountItem;
					if ( SUCCEEDED(pkPlayer->GetInven()->GetItem( kDisCountItemPos, kDisCountItem )) )
					{
						if ( !kDisCountItem.IsUseTimeOut() )
						{
							GET_DEF(CItemDefMgr, kItemDefMgr);
							CItemDef const *pkItemDef = kItemDefMgr.GetDef(kDisCountItem.ItemNo());
							if ( pkItemDef )
							{
								kTarget.i64Price -= std::max( SRateControl::GetValueRate( kTarget.i64Price , static_cast<__int64>(pkItemDef->GetAbil( AT_USE_ITEM_CUSTOM_VALUE_1 )) ), 0i64 ); 
							}
						}
					}
				}

				if ( i64HaveMoney < kTarget.i64Price )
				{
					// 돈이 부족
					throw 700036;
				}

				// 맵이동을 해야 한다.
				SReqMapMove_MT kRMM(bByPetSkill ? MMET_RidingPet_Transport : MMET_None);
				kRMM.kTargetKey.GroundNo(kTarget.iGroundNo);
				kRMM.nTargetPortal = kTarget.nTargetSpawn;

				PgReqMapMove kMapMove( pkGround, kRMM, NULL );
				if( !kMapMove.Add( pkPlayer ) )
				{
					throw 0;
				}

				if ( 0 < kTarget.i64Price )
				{
					SPMOD_Add_Money kDelMoneyData( -kTarget.i64Price );//필요머니 빼기.
					SPMO kIMO(IMET_ADD_MONEY, pkPlayer->GetID(), kDelMoneyData);
					kMapMove.AddModifyOrder( kIMO );
				}

				if ( !kMapMove.DoAction() )
				{
					throw 0;
				}
			}break;
/*
		case E_TRANSTOWER_OPENMAP:
			{
				TBL_DEF_TRANSTOWER_TARGET_KEY kTargetKey;
				if ( !pkPacket->Pop( kTargetKey ) )
				{
					throw 0;
				}

				SItemPos kItemPos;
				if ( !pkPacket->Pop( kItemPos ) )
				{
					throw 0;
				}

				CONT_DEF_TRANSTOWER const *pkDefTransTower = NULL;
				g_kTblDataMgr.GetContDef( pkDefTransTower );

				CONT_DEF_TRANSTOWER::const_iterator def_itr = pkDefTransTower->find( kTowerID );
				if ( def_itr == pkDefTransTower->end() )
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found TransTower ID<" << kTowerID << L"> in DEF_TRANSTOWER");
					throw 0;
				}

				TBL_DEF_TRANSTOWER_TARGET kTarget( kTargetKey );
				CONT_DEF_TRANSTOWER_TARGET::const_iterator target_itr = def_itr->second.find( kTarget );
				if ( target_itr == def_itr->second.end() )
				{
					throw 0;
				}

				if ( SUCCEEDED(pkPlayer->IsOpenWorldMap( kTarget.iGroundNo )) )
				{
					// 이동한 맵임
					throw 0;
				}

				PgBase_Item kItem;
				if ( FAILED(pkPlayer->GetInven()->GetItem( kItemPos, kItem )) )
				{
					throw 0;
				}

				GET_DEF(CItemDefMgr, kItemDefMgr);
				CItemDef const* pItemDef = kItemDefMgr.GetDef(kItem.ItemNo());
				if(!pItemDef)
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found ItemDef ItemNo<" << kItem.ItemNo() << L">" );
					throw 0;
				}

				int const iErrorMsg = PgAction_ReqUseItem::CheckUseTime( *pItemDef );
				if ( iErrorMsg )
				{
					throw iErrorMsg;
				}

				int const iCustomType = pItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if ( UICT_TRANSTOWER_OPENMAP != iCustomType )
				{
					throw 0;
				}

				CONT_PLAYER_MODIFY_ORDER kOrder;

				{
					SPMOD_Modify_Count kDelData( kItem, kItemPos, -1 );//1씩 감소.
					SPMO kIMO(IMET_MODIFY_COUNT|IMC_DEC_DUR_BY_USE, pkPlayer->GetID(), kDelData);
					kOrder.push_back(kIMO);
				}

				{
					SRecentInfo kRecentInfo( kTarget.iGroundNo, pkPlayer->GetPos() );
					SPMO kIMO( IMET_TRANSTOWER_SAVE_RECENT, pkPlayer->GetID(), kRecentInfo );
					kOrder.push_back(kIMO);
				}

				PgAction_ReqModifyItem kItemModifyAction( CIE_TransTower_Open_Map, pkGround->GroundKey(), kOrder );
				kItemModifyAction.DoAction( pkUnit, NULL );
			}break;
*/
		}

	}
	catch( int iErrorMsg )
	{
		if ( iErrorMsg )
		{
			pkUnit->SendWarnMessage( iErrorMsg );
		}
		return false;
	}

	return true;
}




///////////////////////////////////
PgGTrigger_SuperGround::PgGTrigger_SuperGround(void)
	: PgGroundTrigger(), m_iSuperGroundNo(0), m_iSpawnNo(0)
{
}
PgGTrigger_SuperGround::PgGTrigger_SuperGround(PgGTrigger_SuperGround const& rhs)
	: PgGroundTrigger(rhs), m_iSuperGroundNo(rhs.m_iSuperGroundNo), m_iSpawnNo(rhs.m_iSpawnNo)
{
}
PgGTrigger_SuperGround::~PgGTrigger_SuperGround(void)
{
}
bool PgGTrigger_SuperGround::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_iSuperGroundNo = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		else if( !::strcmp(pcAttrName, "PARAM2") )
		{
			m_iSpawnNo = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		pkAttribute = pkAttribute->Next();
	}

	if( 0 == m_iSpawnNo )
	{
		m_iSpawnNo = 1;
	}
	if( 0 == m_iSuperGroundNo )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"SuperGroundNo is 0, Trigger["<<kID<<L"]" );
		return false;
	}
	return true;
}
bool PgGTrigger_SuperGround::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		return false;
	}

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);//맵서버가 받아서 셋팅한다
	if(pkPlayer)
	{
		if( !pkGround )
		{
			return false;
		}
		
		// 파티가 있는 경우 모든 파티원이 같은 맵에 존재해야 들어 갈수가 있다.
		PgAction_MissionPartyMemberCheck kPartyMemberCheck( pkGround );
		if( false == kPartyMemberCheck.DoAction( pkPlayer, NULL ) )
		{
			pkPlayer->SendWarnMessage(400906);
			return false;
		}

		int iSuperGroundMode = 0;
		if( !pkPacket->Pop( iSuperGroundMode ) )
		{
			return false;
		}

		if( SuperGroundUtil::ReqEnterSuperGround(pkUnit, pkGround->GroundKey(), m_iSuperGroundNo, iSuperGroundMode) )
		{
			return true;
		}
		else
		{
			pkPlayer->SendWarnMessage( 6, EL_Warning );
		}
	}
	return false;
}

//////////////////////////////////////////
PgGTrigger_InSuperGround::PgGTrigger_InSuperGround(void)
	: PgGroundTrigger(), m_iSpawnNo(0), m_bMoveAbsolute(false), m_iNextFloor(1)
{
}

PgGTrigger_InSuperGround::PgGTrigger_InSuperGround(PgGTrigger_InSuperGround const& rhs)
	: PgGroundTrigger(rhs), m_iSpawnNo(rhs.m_iSpawnNo), m_bMoveAbsolute(rhs.m_bMoveAbsolute), m_iNextFloor(rhs.m_iNextFloor), m_ContPortal(rhs.m_ContPortal), m_spAccess(rhs.m_spAccess)
{
}

PgGTrigger_InSuperGround::~PgGTrigger_InSuperGround(void)
{}

bool PgGTrigger_InSuperGround::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if( NULL == pkTriggerRoot )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pkTriggerRoot is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( NULL == pkElement )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pkElement is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !Init( kID, pkTriggerRoot, 100.0f ) )
	{
		return false;
	}

	CONT_DEFMAP const * pContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pContDefMap);

	CONT_DEF_QUEST_REWARD const* pQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pQuestReward);

	if ( !pContDefMap || !pQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM2") )
		{
			m_iSpawnNo = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		else if( !::strcmp(pcAttrName, "ABSOLUTE_MOVE") )
		{
			m_bMoveAbsolute = ((strcmp(pcAttrValue, "TRUE") == 0) || (strcmp(pcAttrValue, "true") == 0));
		}
		else if( !::strcmp(pcAttrName, "NEXTFLOOR") )
		{
			m_iNextFloor = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		pkAttribute = pkAttribute->Next();
	}
	if( 0 == m_iSpawnNo )
	{
		m_iSpawnNo = 1;
	}

	CONT_PORTAL_ACCESS::value_type Portal;
	if ( Portal.Build( pkElement, *pContDefMap, *pQuestReward ) )
	{
		m_ContPortal.push_back( Portal );
	}
	else
	{
		if(Portal.IsChildOfChild())
		{
			if( PgPortalAccess * pkAccess  = new PgPortalAccess )
			{
				Portal.SetMoveType(E_MOVE_PERSONAL);
				*pkAccess = Portal;
				m_spAccess.reset( pkAccess );
			}
		}
	}

	TiXmlElement const *pElement = pkElement->FirstChildElement();
	while ( pElement )
	{
		if ( !::strcmp(pElement->Value(), "ITEM") )
		{
			if ( m_ContPortal.empty() )
			{
				m_ContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type Portal;
			if ( Portal.Build( pElement, *pContDefMap, *pQuestReward ) )
			{
				m_ContPortal.push_back( Portal );
			}
		}
		pElement = pElement->NextSiblingElement();
	}

	return true;
}

bool PgGTrigger_InSuperGround::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if ( pPlayer )
	{
		if( m_ContPortal.size() )
		{
			BYTE const MoveType = m_ContPortal.at(0).GetMoveType();
			PgPortalAccess Access = m_ContPortal.at(0);
			Access.SetQuestPortal(pPlayer); // 수행중인 퀘스트에 따라 이동할 맵 변경

			bool bSuccess = false;
			SReqMapMove_MT RMM(MMET_None);
			PgReqMapMove MapMove( pkGround, RMM, &Access );
			if( MapMove.Add( pPlayer ) )
			{
				if( pPlayer->HaveParty() )
				{
					if( MoveType & E_MOVE_PARTYMASTER )
					{ // 파티 마스터만 이동을 요청 할 수 있다.
						bSuccess = pkGround->AddPartyMember(pPlayer, MapMove);
					}
					else if( MoveType & E_MOVE_ANY_PARTYMEMMBER )
					{ // 파티원 누구나 이동 요청 가능.
						bSuccess = pkGround->AddAnyPartyMember(pPlayer, MapMove);
					}
				}
				else
				{
					bSuccess = true;
				}
			}
			if( false == bSuccess )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	VEC_GUID kContGuid;
	BM::GUID kMasterGuid;
	if( BM::GUID::IsNotNull(pkUnit->GetPartyGuid()) )
	{
		pkGround->GetPartyMember(pkUnit->GetPartyGuid(), kContGuid);
		pkGround->GetPartyMasterGuid(pkUnit->GetPartyGuid(), kMasterGuid);
	}
	else if( pPlayer )
	{
		kContGuid.push_back(pPlayer->GetID());
	}

	VEC_GUID::const_iterator iter = kContGuid.begin();
	while( kContGuid.end() != iter )
	{
//		if( pkUnit->GetID() != (*iter) )
		{
			PgPlayer* pkOtherPlayer = dynamic_cast< PgPlayer* >(pkGround->GetUnit((*iter)));
			if( pkOtherPlayer )
			{
				if( !pkOtherPlayer->IsAlive() )
				{
					int const iMessageNo = 400649;
					BM::Stream kPacket( PT_M_C_NFY_WARN_MESSAGE, iMessageNo );
					kPacket.Push( static_cast< BYTE >(EL_Warning) );
					pkGround->Broadcast(kPacket);
					return false;
				}

				bool const bPartyMaster = kMasterGuid==pkOtherPlayer->GetID();
				if(m_spAccess && false == m_spAccess->IsAccess(pkOtherPlayer, bPartyMaster, NULL))
				{
					if(int const iErr = m_spAccess->GetLastAccessError())
					{
						pkOtherPlayer->SendWarnMessage(iErr);
						if(pPlayer && pPlayer!=pkOtherPlayer)
						{
							pPlayer->SendWarnMessage(iErr);
						}
					}
					return false;
				}
			}
		}
		++iter;
	}

	int iPortal = 0;
	if( pkPacket->RemainSize() )
	{
		pkPacket->Pop(iPortal);
	}
	PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(pkGround);
	if( pkSuperGround )
	{
		int NextFloor = 0;
		if( m_bMoveAbsolute )
		{// 절대 이동
			if( 1 <= m_iNextFloor )
			{// 최소 1층 이상만 이동 가능
				NextFloor = m_iNextFloor - 1; // 1층이 0부터 시작하므로 1을 빼준다.
			}
		}
		else
		{// 상대 이동
			NextFloor = pkSuperGround->NowFloor() + m_iNextFloor;
		}

		if( m_bMoveAbsolute )
		{
			return pkSuperGround->NextFloor(NextFloor, m_iSpawnNo);
		}
		else
		{
			if( iPortal )
			{
				return pkSuperGround->NextFloor(NextFloor, iPortal);
			}
			return pkSuperGround->NextFloor(NextFloor, m_iSpawnNo);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_JobSkill
//----------------------------------------------------------------------------
PgGTrigger_JobSkill::PgGTrigger_JobSkill(void)
	: PgGroundTrigger(), iJobGrade(0)
{
}

PgGTrigger_JobSkill::PgGTrigger_JobSkill(PgGTrigger_JobSkill const& rhs)
: PgGroundTrigger(rhs), iJobGrade(rhs.iJobGrade)
{
}

PgGTrigger_JobSkill::~PgGTrigger_JobSkill(void)
{
}

bool PgGTrigger_JobSkill::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SetID(kID);

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			iJobGrade = ::atoi( pcAttrValue );
		}
		pkAttribute = pkAttribute->Next();
	}

	return true;
}

bool PgGTrigger_JobSkill::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	BM::Stream kPacket;	

	GTRIGGER_ID kName = GetID();

	if( !Enable() )
	{
		goto __FAILED;
	}

	if( !pkUnit )
	{
		goto __FAILED;
	}

	if ( !IsInPos( pkUnit->GetPos() ) )
	{
		// 포탈을 벗어났습니다.
		/*pkUnit->SendWarnMessage( 18995 );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		goto __FAILED;*/
	}

	if( !pkGround )
	{
		goto __FAILED;
	}

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if( !pkPlayer )
	{
		goto __FAILED;
	}
	int iSkillNo = 0;
	pkPacket->Pop( iSkillNo );

	int iErrMgs = 0;
	// 유저 등록	
	CONT_DEF_JOBSKILL_SKILL const* pkDefJobSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill);
	if( !pkDefJobSkill )
	{
		goto __FAILED;
	}

	EGatherType eGatherType = GT_None;
	if( !(pkGround->JobSkillLocationMgr().GetGatherType(m_kID, eGatherType)) )
	{//3. 기술 지역 오류
		pkUnit->SendWarnMessage(25014);
		goto __FAILED;
	}

	CONT_DEF_JOBSKILL_SKILL const* pkJSSkill = NULL;
	CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkJSExpertness = NULL;
	g_kTblDataMgr.GetContDef(pkJSSkill);
	g_kTblDataMgr.GetContDef(pkJSExpertness);

	int const iMainSkillNo = pkGround->JobSkillLocationMgr().GetSkillNo(iJobGrade);
	int const iSubSkillNo = JobSkillUtil::GetJobSkillNo(eGatherType, JST_1ST_SUB, *pkDefJobSkill);
	//1. 도구장착 오류
	int iUseSkillNo = JobSkillToolUtil::GetUseSkill(pkPlayer, eGatherType, iMainSkillNo, iSubSkillNo);
	if( 0 == iUseSkillNo)
	{//오류메세지는 JobSkillToolUtil::GetUseSkill()에서 처리한다.
		goto __FAILED;
	}	
	if( !JobSkillUtil::IsJobSkill( pkPlayer->GetActionID() ) )
	{//이전 액션이 직업기술이 아닐 경우엔 무조건 메인 기술 사용.
		iUseSkillNo = iMainSkillNo;
	}
	int const iUseSkillExpertness = pkPlayer->JobSkillExpertness().Get(iUseSkillNo);
	int iNeedExhaustion = JobSkillExpertnessUtil::GetUseExhaustion(iUseSkillNo, iUseSkillExpertness, *pkJSSkill, *pkJSExpertness);
	int const iMaxExhaustion = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), pkPlayer->JobSkillExpertness().GetAllSkillExpertness(), *pkJSSkill, *pkJSExpertness);
	int const iCurExhaustion = pkPlayer->JobSkillExpertness().CurExhaustion();
	if( iMaxExhaustion == iCurExhaustion)
	{//2. 피로도 오류
		pkUnit->SendWarnMessage(25013);
		goto __FAILED;
	}
	if( iMaxExhaustion <= (iCurExhaustion + iNeedExhaustion) )
	{
		iNeedExhaustion = iMaxExhaustion - iCurExhaustion;
	}

	int iSkillGatherType = JobSkillUtil::GetGatherType(iSkillNo, *pkDefJobSkill);
	if( iSkillGatherType != static_cast<int>(eGatherType) )
	{//4. 적정 지역 오류
		pkGround->JobSkillLocationMgr().DelUser(pkUnit, GetID(), pkUnit->GetID());	//현재 적용중인 채집 스킬을 스킬을 종료하고
		int iCorrectSkillNo = pkGround->JobSkillLocationMgr().GetSkillNo(iJobGrade);
		BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_WRONG_SKILL_LOCATION);
		kPacket.Push( iSkillNo );
		kPacket.Push( iCorrectSkillNo );
		pkUnit->Send(kPacket);
		goto __FAILED;
	}	

	if( !pkGround->JobSkillLocationMgr().IsEnable(m_kID) )
	{//5. 비활성화 오류
		pkUnit->SendWarnMessage(25011);
		goto __FAILED;
	}

	if( !pkGround->JobSkillLocationMgr().CheckSkillExpertness(pkPlayer, iJobGrade, iErrMgs) )
	{//에러 메세지 처리 세분화 필요, 패킷 날려서 확인 해야 함.
	 //6. 도감 학습 오류
		if( iErrMgs )
		{
			pkUnit->SendWarnMessage(iErrMgs);
		}
		goto __FAILED;
	}
	
	if( (0 < iUseSkillNo) && (!kName.empty()) )
	{
		DWORD dwOutTurnTime = 0;
		if( pkGround->JobSkillLocationMgr().SetUser(kName, pkPlayer, iUseSkillNo, dwOutTurnTime, iNeedExhaustion) )
		{
			pkGround->JobSkillLocationMgr().SendLocationAction(kPacket, true);
			kPacket.Push( iUseSkillNo );	// 사용할 스킬 번호
			kPacket.Push( GetID() );		// 사용되는 트리거의 아이디(바라 봐야할 위치 세팅을 하기위해)
			kPacket.Push( dwOutTurnTime );

			pkUnit->Send(kPacket);

			pkUnit->SetAbil(AT_CANNOT_EQUIP, 1);
			return true;
		}
		else
		{//트리거에 유저 등록 실패
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("JobSkill TriggerID User Add Fail[") << pkPlayer->GetID().str().c_str() << _T("]") );
		}
	}
	else
	{
		// 스킬 설정이 되어 있지 않음
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("JobSkill TriggerID Location SkillNo is not Data[") << iJobGrade << _T("]") );
	}

__FAILED:
	{
		if( !pkGround )
		{
			return false;
		}

		if( !pkUnit )
		{
			return false;
		}
		
		pkGround->JobSkillLocationMgr().SendLocationAction(kPacket, false);
		pkUnit->Send(kPacket);

		return false;
	}
}

//============================================================================
// PgGTrigger_Double_Up
//----------------------------------------------------------------------------
PgGTrigger_Double_Up::PgGTrigger_Double_Up(void)
	: PgGroundTrigger()
{
}

PgGTrigger_Double_Up::PgGTrigger_Double_Up(PgGTrigger_Double_Up const& rhs)
	: PgGroundTrigger(rhs)
{
}

PgGTrigger_Double_Up::~PgGTrigger_Double_Up(void)
{
}

bool PgGTrigger_Double_Up::Build( GTRIGGER_ID const &kID, NiNode *pkTriggerRoot, TiXmlElement const *pkElement )
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgGTrigger_Double_Up::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	if( !Enable() )
	{
		return false;
	}	
	return true;
}

//============================================================================
// PgGTrigger_ChannelPortal
//----------------------------------------------------------------------------

PgGTrigger_ChannelPortal::PgGTrigger_ChannelPortal(void)
{
}

PgGTrigger_ChannelPortal::~PgGTrigger_ChannelPortal(void)
{
}

PgGTrigger_ChannelPortal::PgGTrigger_ChannelPortal(PgGTrigger_ChannelPortal const & rhs)
	: PgGroundTrigger(rhs), m_ContPortal(rhs.m_ContPortal)
{
}

bool PgGTrigger_ChannelPortal::Build( GTRIGGER_ID const & kID, NiNode * pTriggerRoot, TiXmlElement const * pElement )
{
	if( NULL == pTriggerRoot )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pTriggerRoot is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( NULL == pElement )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("pElement is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFMAP const * pContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pContDefMap);

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);

	if( !pContDefMap || !pkQuestReward )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("CONT_DEFMAP is NULL") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !Init( kID, pTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	{
		CONT_PORTAL_ACCESS::value_type Portal;
		if( Portal.Build( pElement, *pContDefMap, *pkQuestReward ) )
		{
			m_ContPortal.push_back(Portal);
		}
	}

	pElement = pElement->FirstChildElement();
	while( pElement )
	{
		if( !::strcmp(pElement->Value(), "ITEM") )
		{
			if( m_ContPortal.empty() )
			{
				m_ContPortal.resize(1);
			}

			CONT_PORTAL_ACCESS::value_type Portal;
			if( Portal.Build(pElement, *pContDefMap, *pkQuestReward) )
			{
				m_ContPortal.push_back(Portal);
			}
		}
		pElement = pElement->NextSiblingElement();
	}

	if( m_ContPortal.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}

	return !m_ContPortal.empty();
}

bool PgGTrigger_ChannelPortal::Event( CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket)
{
	if( !Enable() )
	{
		return false;
	}

	if( !IsInPos(pUnit->GetPos()) )
	{
		// 포탈을 벗어 났습니다.
		pUnit->SendWarnMessage(18995);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	size_t iTargetIndex = 0;
	if( pPacket->Pop(iTargetIndex) )
	{
		if( m_ContPortal.size() > iTargetIndex )
		{
			PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
			if( pPlayer )
			{
				BYTE const MoveType = m_ContPortal.at(iTargetIndex).GetMoveType();

				PgPortalAccess Access = m_ContPortal.at(iTargetIndex);

				if( 0 == (MoveType & E_MOVE_PERSONAL) )
				{	
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}

				if ( pGround && CheckEffectNo() )
				{// 트리거에 체크할 이펙트번호가 있을 때만 검사한다.
					if( false == CheckEffectFromPlayer( pPlayer, pGround, MoveType ) )
					{
						pPlayer->SendWarnMessage( m_kErrorEffectMsgNo );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}

				SChannelMapMove MoveInfo;
				if( (pGround->GetAttr() & GATTR_EXPEDITION_GROUND)
					|| (pGround->GetAttr() & GATTR_EXPEDITION_LOBBY) )
				{
					SRecentPlayerNormalMapData MapData;
					pPlayer->GetRecentNormalMap(MapData);

					MoveInfo.iChannelNo = MapData.ChannelNo;
					MoveInfo.iGroundNo = MapData.GroundNo;

					pPlayer->SetRecentNormalMap(MapData); // 접속이 비정상적으로 종료 되었을 때 맵 정보를 다시 되돌릴 때 사용함.
				}
				else
				{
					// 마지막 채널 번호, 맵, 위치 저장.
					MoveInfo.iChannelNo = Access.GetChannelNo();
					MoveInfo.iGroundNo = Access.GetGroundKey().GroundNo();
					SRecentPlayerNormalMapData MapData(pGround->GetGroundNo(), g_kProcessCfg.ChannelNo(), pPlayer->GetPos());
					pPlayer->SetRecentNormalMap(MapData);
				}

				// 스위치로 보냄.
				BM::Stream Packet(PT_M_L_TRY_LOGIN_CHANNELMAPMOVE);
				Packet.Push(pPlayer->GetID());
				Packet.Push(MoveInfo);
				SendToServer( pPlayer->GetSwitchServer(), Packet );

				// 클라로 보냄.
				BM::Stream UserPacket(PT_M_C_TRY_LOGIN_CHANNELMAPMOVE);
				UserPacket.Push(MoveInfo);
				pPlayer->Send(UserPacket);

				return true;
			}
		}
	}

	// 잘못된 포탈입니다.
	pUnit->SendWarnMessage(18994);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

//============================================================================
// PgGTrigger_NpcMove
//----------------------------------------------------------------------------

PgGTrigger_NpcMove::PgGTrigger_NpcMove(void)
{
}

PgGTrigger_NpcMove::PgGTrigger_NpcMove(PgGTrigger_NpcMove const & rhs)
:	PgGTrigger_Normal(rhs)
,	m_NpcGuid(rhs.m_NpcGuid)
{
}

PgGTrigger_NpcMove::~PgGTrigger_NpcMove(void)
{
}

bool PgGTrigger_NpcMove::Build(GTRIGGER_ID const & ID, NiNode *pTriggerRoot, TiXmlElement const * pElement)
{
	if( !Init(ID, pTriggerRoot) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgGTrigger_NpcMove::Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket)
{
	if( !Enable() )
	{
		return false;
	}

	if( !IsInPos(pUnit->GetPos() ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return Event(pUnit, pGround, pPacket, m_NpcGuid);
}

bool PgGTrigger_NpcMove::Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket, BM::GUID const & NpcGuid)
{
	PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
	if( !pPlayer )
	{
		LIVE_CHECK_LOG( BM::LOG_LV0, __FL__ << L"dynamic_cast Error" );
		return false;
	}

	CONT_DEF_EXPEDITION_NPC const * pContExpeditionNpc = NULL;
	g_kTblDataMgr.GetContDef(pContExpeditionNpc);

	if( NULL == pContExpeditionNpc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_EXPEDITION_NPC::const_iterator iter = pContExpeditionNpc->find(NpcGuid);
	if( iter == pContExpeditionNpc->end() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SReqMapMove_MT RMM(MMET_None);
	RMM.kTargetKey.GroundNo(iter->second.iMapNo);
	RMM.nTargetPortal = 1;
	RMM.bIndunPartyDie = pGround->IndunPartyDie();

	bool bSuccess = false;
	PgReqMapMove MapMove( pGround, RMM, NULL );
	if( MapMove.Add(pPlayer) )
	{
		if( pPlayer->HaveExpedition() )
		{
			bSuccess = pGround->AddExpeditionMember(pPlayer, MapMove);
			if( false == bSuccess )
			{
				// 원정대원 중 한명이라도 안되면 모두 이동 불가.
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}

		if ( bSuccess )
		{
			return MapMove.DoAction();
		}

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}


//============================================================================
// PgGTrigger_KingOfHill
//----------------------------------------------------------------------------

PgGTrigger_KingOfHill::PgGTrigger_KingOfHill(void)
:	m_fRadius(0.0f)
,	m_eInitEntityTeam(TEAM_NONE)
,	m_iIngRedEffect(0)
,	m_iIngBlueEffect(0)
,	m_pkTriggerUnit(NULL)
{
}

PgGTrigger_KingOfHill::PgGTrigger_KingOfHill(PgGTrigger_KingOfHill const & rhs)
:	PgGroundTrigger(rhs)
,	m_fRadius(rhs.m_fRadius)
,	m_kContLink(rhs.m_kContLink)
,	m_eInitEntityTeam(rhs.m_eInitEntityTeam)
,	m_iIngRedEffect(rhs.m_iIngRedEffect)
,	m_iIngBlueEffect(rhs.m_iIngBlueEffect)
,	m_kContDefEntityNo(rhs.m_kContDefEntityNo)
,	m_pkTriggerUnit(rhs.m_pkTriggerUnit)
,	m_kTriggerUnitGuid(rhs.m_kTriggerUnitGuid)
{
}

PgGTrigger_KingOfHill::~PgGTrigger_KingOfHill(void)
{
}

bool PgGTrigger_KingOfHill::InitTriggerPhysX(NiAVObject const* pkObj, float const fBuffer)
{
	if( !pkObj )
	{
		return false;
	}

	NiBound const& kBound = pkObj->GetWorldBound();
	m_fRadius = kBound.GetRadius();
	return PgGroundTrigger::InitTriggerPhysX(pkObj, 0.f);
}

bool PgGTrigger_KingOfHill::Build(GTRIGGER_ID const & kID, NiNode *pkTriggerRoot, TiXmlElement const * pkElement)
{
	if ( !Init( kID, pkTriggerRoot ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	do {
		TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();

		char const *pcAttrName = NULL;
		char const *pcAttrValue = NULL;
		while ( pkAttribute )
		{
			pcAttrName = pkAttribute->Name();
			pcAttrValue = pkAttribute->Value();

			if( !::strcmp(pcAttrName, "LINK") )
			{
				m_kContLink.insert(pcAttrValue);
			}
			else if( !::strcmp(pcAttrName, "INIT_ENTITY_TEAM") )
			{
				m_eInitEntityTeam = static_cast<ETeam>(::atoi( pcAttrValue ));
			}
			else if( !::strcmp(pcAttrName, "RED_ING_EFFECT_NO") )
			{
				m_iIngRedEffect = ::atoi( pcAttrValue );
			}
			else if( !::strcmp(pcAttrName, "BLUE_ING_EFFECT_NO") )
			{
				m_iIngBlueEffect = ::atoi( pcAttrValue );
			}
			else if( !::strcmp(pcAttrName, "ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(0, std::make_pair(::atoi( pcAttrValue ), 0)));
			}
			else if( !::strcmp(pcAttrName, "ENTITY_LV") )
			{
				m_kContDefEntityNo[0].second = ::atoi( pcAttrValue );
			}
			else if( !::strcmp(pcAttrName, "RED_LV1_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(1, std::make_pair(::atoi( pcAttrValue ), 1)));
			}
			else if( !::strcmp(pcAttrName, "RED_LV2_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(2, std::make_pair(::atoi( pcAttrValue ), 2)));
			}
			else if( !::strcmp(pcAttrName, "RED_LV3_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(3, std::make_pair(::atoi( pcAttrValue ), 3)));
			}
			else if( !::strcmp(pcAttrName, "BLUE_LV1_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(-1, std::make_pair(::atoi( pcAttrValue ), 1)));
			}
			else if( !::strcmp(pcAttrName, "BLUE_LV2_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(-2, std::make_pair(::atoi( pcAttrValue ), 2)));
			}
			else if( !::strcmp(pcAttrName, "BLUE_LV3_ENTITY_NO") )
			{
				m_kContDefEntityNo.insert(std::make_pair(-3, std::make_pair(::atoi( pcAttrValue ), 3)));
			}
			pkAttribute = pkAttribute->Next();
		}

		if ( 0 != ::strcmp( pkElement->Value(), "ACTION") )
		{
			break;
		}

		pkElement = pkElement->NextSiblingElement();
	} while ( pkElement );

	return true;
}

bool PgGTrigger_KingOfHill::Event(CUnit * pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket)
{
	if( !pkUnit
	||	!pkGround
	||	!pkPacket )
	{
		return false;
	}

	if( !Enable() )
	{
		return false;
	}

	if( !IsInPos(pkUnit->GetPos() ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

KOH_ENTITY_KEY PgGTrigger_KingOfHill::GetEntityNo(ETeam const eTeam, int const iLevel)const
{
	int iKey = 0;
	if(TEAM_RED==eTeam)
	{
		iKey = iLevel;
	}
	else if(TEAM_BLUE==eTeam)
	{
		iKey = -iLevel;
	}

	CONT_KOH_DEF_ENTITY_NO::const_iterator c_it = m_kContDefEntityNo.find(iKey);
	if(c_it!=m_kContDefEntityNo.end())
	{
		return (*c_it).second;
	}
	static const CONT_KOH_DEF_ENTITY_NO::mapped_type kNullData(0,0);
	return kNullData;
}

int PgGTrigger_KingOfHill::GetIngEffectNo(ETeam const eTeam)const
{
	if(TEAM_RED==eTeam)
	{
		return m_iIngRedEffect;
	}
	else if(TEAM_BLUE==eTeam)
	{
		return m_iIngBlueEffect;
	}
	return 0;
}

void PgGTrigger_KingOfHill::SetUnit(CUnit * const pkUnit)
{
	m_pkTriggerUnit = pkUnit;
	if(pkUnit)
	{
		m_kTriggerUnitGuid = pkUnit->GetID();
	}
}


//============================================================================
// PgGTrigger_Score
//----------------------------------------------------------------------------

PgGTrigger_Score::PgGTrigger_Score()
{
	TriggerTeamNo = 0;
}

PgGTrigger_Score::~PgGTrigger_Score()
{
}

PgGTrigger_Score::PgGTrigger_Score(PgGTrigger_Score const & rhs)
:	PgGroundTrigger(rhs)
	,TriggerTeamNo(rhs.TriggerTeamNo)
{
}

bool PgGTrigger_Score::Build(GTRIGGER_ID const & ID, NiNode * pTriggerRoot, TiXmlElement const * pElement)
{
	if( !Init(ID, pTriggerRoot) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const * pAttribute = pElement->FirstAttribute();
	char const * pcAttrName = NULL;
	char const * pcAttrValue = NULL;
	while ( pAttribute )
	{
		pcAttrName = pAttribute->Name();
		pcAttrValue = pAttribute->Value();

		if( !::strcmp(pcAttrName, "TEAM") )
		{
			if( !::strcmp(pcAttrValue, "RED") )
			{
				TriggerTeamNo = TEAM_RED;
			}
			else if( !::strcmp(pcAttrValue, "BLUE") )
			{
				TriggerTeamNo = TEAM_BLUE;
			}
		}		
		pAttribute = pAttribute->Next();
	}

	return true;
}

bool PgGTrigger_Score::Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket)
{
	if( !pUnit || !pGround )
	{
		return false;
	}

	if( !Enable() )
	{
		return false;
	}

	PgUnitEffectMgr const & EffectMgr = pUnit->GetEffectMgr();
	if( !EffectMgr.GetAbil(AT_BEAR_EFFECT_RED) && !EffectMgr.GetAbil(AT_BEAR_EFFECT_BLUE) )
	{
		return false;
	}

	int UnitTeam = pUnit->GetAbil(AT_TEAM);
	if( UnitTeam != TriggerTeamNo )
	{
		return false;
	}

	PgWarGround * pWarGnd = NULL;
	pWarGnd = dynamic_cast<PgWarGround *>(pGround);
	if( pWarGnd )
	{
		pWarGnd->BearTouchDown(pUnit);
	}

	return true;
}

void PgGTrigger_Score::CheckBearOnTrigger(PgGround * pGnd, VEC_UNIT const & UnitArray)
{	// 우리편 곰을 회수되었을 때, 우리편 스코어 트리거에 곰을 업고 있는 플레이어가 있는지 확인.
	
	VEC_UNIT::const_iterator unit_iter = UnitArray.begin();
	for( ; unit_iter != UnitArray.end() ; ++unit_iter )
	{
		if( IsInPos( (*unit_iter)->GetPos() ) )
		{
			Event((*unit_iter), pGnd, NULL);
		}
	}
}

//============================================================================
// PgGTrigger_LoveFence
//----------------------------------------------------------------------------

PgGTrigger_LoveFence::PgGTrigger_LoveFence()
{
	FenceObjectNo =0;
	FenceObjectTeamNo = 0;
	FenceObjectAlive = false;
}

PgGTrigger_LoveFence::~PgGTrigger_LoveFence()
{
}

PgGTrigger_LoveFence::PgGTrigger_LoveFence(PgGTrigger_LoveFence const & rhs)
:	PgGroundTrigger(rhs)
	,FenceObjectNo(rhs.FenceObjectNo)
	,FenceObjectTeamNo(rhs.FenceObjectTeamNo)
	,FenceObjectAlive(rhs.FenceObjectAlive)
{
}

bool PgGTrigger_LoveFence::Build(GTRIGGER_ID const & ID, NiNode * pTriggerRoot, TiXmlElement const * pElement)
{
	if( !Init(ID, pTriggerRoot) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlAttribute const * pAttribute = pElement->FirstAttribute();
	char const * pcAttrName = NULL;
	char const * pcAttrValue = NULL;
	while ( pAttribute )
	{
		pcAttrName = pAttribute->Name();
		pcAttrValue = pAttribute->Value();

		if( !::strcmp(pcAttrName, "INIT_OBJECT_NO") )
		{
			FenceObjectNo = atoi(pcAttrValue);
		}
		else if( !::strcmp(pcAttrName, "INIT_OBJECT_TEAM") )
		{
			FenceObjectTeamNo = atoi(pcAttrValue);
		}
		pAttribute = pAttribute->Next();
	}

	return true;
}

bool PgGTrigger_LoveFence::Event(CUnit * pUnit, PgGround * const pGround, BM::Stream * const pPacket)
{
	if( !pGround || !pPacket )
	{
		return false;
	}

	if( !Enable() )
	{
		return false;
	}

	int BreakObjectNo = 0;
	bool bAlive = false;

	pPacket->Pop(BreakObjectNo);
	pPacket->Pop(bAlive);

	FenceObjectAlive = bAlive;

	BM::Stream Packet(PT_M_C_NFY_UPDATE_LOVE_FENCE);
	Packet.Push(BreakObjectNo);
	Packet.Push(bAlive);

	pGround->Broadcast(Packet);

	return true;
}

int PgGTrigger_LoveFence::GetFenceObjectNo(void)
{
	return FenceObjectNo;
}


PgGTrigger_CheckPoint::PgGTrigger_CheckPoint() : m_iProgressNo(0)
{
}

PgGTrigger_CheckPoint::~PgGTrigger_CheckPoint()
{
}

bool PgGTrigger_CheckPoint::Build(const GTRIGGER_ID &kID, NiNode *pkTriggerRoot, const TiXmlElement *pkElement)
{
	if( !Init(kID, pkTriggerRoot) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("PgGTrigger_CheckPoint::Build() Return false"));
		return false;
	}
	return true;
}

bool PgGTrigger_CheckPoint::Event( CUnit *pkUnit, PgGround * const pkGround, BM::Stream * const pkPacket )
{
	PgStaticRaceGround* pkRaceGround = dynamic_cast<PgStaticRaceGround*>(pkGround);
	if(NULL == pkRaceGround)
	{
		return false;
	}

	return true;
}

