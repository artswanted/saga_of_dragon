#include "stdafx.h"
#include "PgPlayer.h"
#include "PgQuestInfo.h"
#include "PgPortalAccess.h"
#include "PortalAccessInfo.h"
#include "PgStringUtil.h"

bool SPortalAccessInfo::IsAccess()const
{
	if(bMoveLock)
	{
		return false;
	}

	if(!kLevel.bOk)
	{
		return false;
	}

	if(!kTime.bOk)
	{
		return false;
	}

	bool bOk = false;
	for(CONT_AND_QUEST::const_iterator and_it=kContQuest.begin(); and_it!=kContQuest.end(); ++and_it)
	{
		for(CONT_OR_QUEST::const_iterator or_it=(*and_it).second.begin(); or_it!=(*and_it).second.end(); ++or_it)
		{
			if((*or_it).bOk)
			{
				bOk = true;
				break;
			}
		}
		if(!bOk)
		{
			return false;
		}
	}

	for(CONT_AND_ITEM::const_iterator and_it=kContItem.begin(); and_it!=kContItem.end(); ++and_it)
	{
		for(CONT_OR_ITEM::const_iterator or_it=(*and_it).second.begin(); or_it!=(*and_it).second.end(); ++or_it)
		{
			if((*or_it).bOk)
			{
				bOk = true;
				break;
			}
		}
		if(!bOk)
		{
			return false;
		}
	}

	bOk = true;
	for(CONT_EFFECT::const_iterator cont_it = kContEffect.begin(); cont_it != kContEffect.end(); ++cont_it)
	{
		if( false == (*cont_it).second )
		{
			bOk = false;
			break;
		}
	}
	if(false == bOk)
	{
		return false;
	}

	return true;
}

//============================================================================
// PgMapMoveChecker
//----------------------------------------------------------------------------
PgMapMoveChecker::PgMapMoveChecker( CONT_DEFMAP const &kContDetMap )
:	m_kContDefMap(kContDetMap)
{
}

EPartyMoveType PgMapMoveChecker::GetMoveType( int const iGroundNo )const
{
	CONT_DEFMAP::const_iterator defmap_itr = m_kContDefMap.find( iGroundNo );
	if ( defmap_itr != m_kContDefMap.end() )
	{
		if ( GATTR_INSTANCE & (defmap_itr->second.iAttr)  )
		{
			return E_MOVE_PARTYMASTER;
		}
		else if( GATTR_STATIC_DUNGEON & (defmap_itr->second.iAttr)  )
		{
			return E_MOVE_ANY_PARTYMEMMBER;
		}
		return E_MOVE_PERSONAL;
	}

	return E_MOVE_LOCK;
}

//============================================================================
// PgPortalAccess
//----------------------------------------------------------------------------
PgPortalAccess::PgPortalAccess(void)
:	m_nTargetPortal(0)
,	m_byMoveType(E_MOVE_LOCK)
,	m_iName(0)
,	m_iLevelLimit_Min(0)
,	m_iLevelLimit_Max(INT_MAX)
,	m_bLevelParty(false)
,	m_bUseModeLevel(false)
,	m_ModeNo(0)
,	m_iChannelNo(0)
,	m_bIsBossPortal(false)
,	m_bChildOfChild(false)
,	m_eLastAccessErr(ERR_NONE)
{
	for(int i = 0; i < MAX_DUNGEON_MODE; ++i )
	{
		m_iModeLevelLimit_Min[i] = 0;
		m_iModeLevelLimit_Max[i] = 0;
		m_iModeGroundNo[i] = 0;
	}
	memset(m_iErrorMessage, 0, sizeof(m_iErrorMessage));
}

PgPortalAccess::~PgPortalAccess(void)
{
}

bool PgPortalAccess::Build( TiXmlElement const *pkElement, CONT_DEFMAP const &kContDerMap, CONT_DEF_QUEST_REWARD const& kContDefQuest )
{
	if ( !pkElement )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	bool bMoveTypeParsed = false;

	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( !::strcmp(pcAttrName, "PARAM") )
		{
			m_kTargetGroundKey.GroundNo( ::atoi( pcAttrValue ) );
		}
		else if ( !::strcmp(pcAttrName, "PARAM2" ) )
		{
			m_nTargetPortal = static_cast<short>( ::atoi( pcAttrValue ) );
		}
		// PARAM3~PARAM7 퀘스트 포탈
		else if(	!::strcmp(pcAttrName, "PARAM3" )
		||			!::strcmp(pcAttrName, "PARAM4" )
		||			!::strcmp(pcAttrName, "PARAM5" )
		||			!::strcmp(pcAttrName, "PARAM6" )
		||			!::strcmp(pcAttrName, "PARAM7" ) )
		{
			typedef std::list< std::string > CONT_STR;
			CONT_STR kContList;
			BM::vstring::CutTextByKey( std::string( pcAttrValue ), std::string("/"), kContList );
			CONT_STR::const_iterator c_iter = kContList.begin();
			if( kContList.end() != c_iter )
			{
				SQuestPortal kQuestPortal;
				kQuestPortal.iQuestID = PgStringUtil::SafeAtoi(*c_iter);					++c_iter;
				kQuestPortal.kTargetGroundKey.GroundNo( PgStringUtil::SafeAtoi(*c_iter) );	++c_iter;
				kQuestPortal.sTargetPortal = PgStringUtil::SafeAtoi(*c_iter);				++c_iter;

				m_kContQuestPortal.insert( kQuestPortal );
			}
		}
		else if ( !::strcmp(pcAttrName, "MOVETYPE") )
		{
			bMoveTypeParsed = true;
			m_byMoveType = static_cast<BYTE>( ::atoi( pcAttrValue ) );
		}
		else if ( !::strcmp(pcAttrName, "NAME") )
		{
			m_iName = ::atoi( pcAttrValue );
		}
		else if( !::strcmp(pcAttrName, "CHANNEL") )
		{
			m_iChannelNo = static_cast<int>( ::atoi(pcAttrValue) );
		}
		else if( !::strcmp(pcAttrName, "IS_BOSS_PORTAL")
			|| !::strcmp(pcAttrName, "IS_BOSS_TRIGGER") )
		{
			m_bIsBossPortal = static_cast<bool>(::atoi(pcAttrValue));
		}

		pkAttribute = pkAttribute->Next();
	}

	TiXmlElement const * pkChildElement = pkElement->FirstChildElement();
	while ( pkChildElement )
	{
		if ( !::strcmp( pkChildElement->Value(), "ACCESS") )
		{
			pkAttribute = pkChildElement->FirstAttribute();
			while ( pkAttribute )
			{
				pcAttrName = pkAttribute->Name();
				pcAttrValue = pkAttribute->Value();

				if( !::strcmp(pcAttrName, "ERROR_MSG") )
				{
					m_iErrorMessage[EMT_DEFAULT] = ::atoi(pcAttrValue);
				}
				else if( !::strcmp(pcAttrName, "HAVE_EFFECT_ERROR_MSG") )
				{
					m_iErrorMessage[EMT_HAVE_EFFECT] = ::atoi(pcAttrValue);
				}
				else if( !::strcmp(pcAttrName, "NOT_HAVE_EFFECT_ERROR_MSG") )
				{
					m_iErrorMessage[EMT_NOT_HAVE_EFFECT] = ::atoi(pcAttrValue);
				}

				pkAttribute = pkAttribute->Next();
			}

			m_bChildOfChild = true;
			pkChildElement = pkChildElement->FirstChildElement();
			continue;
		}
		else if ( m_bChildOfChild )
		{
			if ( !::strcmp( pkChildElement->Value(), "LEVEL") )
			{
				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "MIN") )
					{
						m_iLevelLimit_Min = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "MAX") )
					{
						m_iLevelLimit_Max = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "PARTY") )
					{
						m_bLevelParty = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "USE_MODE_LEVEL") )
					{
						m_bUseModeLevel = ( 0 != ::atoi(pcAttrValue) );
					}

					pkAttribute = pkAttribute->Next();
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "MODE1_LEVEL") )
			{
				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "MIN") )
					{
						m_iModeLevelLimit_Min[0] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "MAX") )
					{
						m_iModeLevelLimit_Max[0] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "GROUND_NO") )
					{
						m_iModeGroundNo[0] = ::atoi(pcAttrValue);
					}
					
					pkAttribute = pkAttribute->Next();
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "MODE2_LEVEL") )
			{
				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "MIN") )
					{
						m_iModeLevelLimit_Min[1] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "MAX") )
					{
						m_iModeLevelLimit_Max[1] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "GROUND_NO") )
					{
						m_iModeGroundNo[1] = ::atoi(pcAttrValue);
					}
					
					pkAttribute = pkAttribute->Next();
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "MODE3_LEVEL") )
			{
				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "MIN") )
					{
						m_iModeLevelLimit_Min[2] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "MAX") )
					{
						m_iModeLevelLimit_Max[2] = ::atoi(pcAttrValue);
					}
					else if ( !::strcmp(pcAttrName, "GROUND_NO") )
					{
						m_iModeGroundNo[2] = ::atoi(pcAttrValue);
					}
					
					pkAttribute = pkAttribute->Next();
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "USEITEM") )
			{
				SKey kKey;

				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "INV") )
					{
						kKey.kInvType = static_cast<EInvType>( ::atoi(pcAttrValue) );
					}
					else if( !::strcmp(pcAttrName, "NO") )
					{
						kKey.iItemNo = ::atoi(pcAttrValue);
					}
					else if( !::strcmp(pcAttrName, "COUNT") )
					{
						kKey.iItemCount = static_cast<size_t>( ::atoi(pcAttrValue) );
					}
					else if( !::strcmp(pcAttrName, "CONSUME") )
					{
						kKey.bConsume = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "PARTY") )
					{
						kKey.bParty = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "GROUP") )
					{
						kKey.iGroup = ::atoi(pcAttrValue);
					}

					pkAttribute = pkAttribute->Next();
				}

				if ( !kKey.IsEmpty() )
				{
					m_kConKey.push_back( kKey );
					m_kConKeyGroup.insert( kKey.iGroup );
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "QUEST") )
			{
				SQKey kQKey;

				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "NO") )
					{
						kQKey.sQuestNo = static_cast<short int>( ::atoi(pcAttrValue) );
					}
					else if( !::strcmp(pcAttrName, "CHECK") )
					{
						kQKey.byState = static_cast<BYTE>( ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "PARTY") )
					{
						kQKey.bParty = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "GROUP") )
					{
						kQKey.iGroup = ::atoi(pcAttrValue);
					}

					pkAttribute = pkAttribute->Next();
				}

				if ( !kQKey.IsEmpty() )
				{
					CONT_DEF_QUEST_REWARD::const_iterator c_it = kContDefQuest.find(kQKey.sQuestNo);
					if(c_it != kContDefQuest.end())
					{
						kQKey.i64ClassFlag = (*c_it).second.i64ClassFlag | ((*c_it).second.i64DraClassLimit << DRAGONIAN_LSHIFT_VAL);
					}
					m_kConQuestKey.push_back( kQKey );
					m_kConQuestKeyGroup.insert( kQKey.iGroup );
				}
			}
			else if ( !::strcmp( pkChildElement->Value(), "TIME") )
			{
				STimeKey kTimeKey;
				bool bEveryDay = false;
				WORD wMinDay = 0;
				WORD wValidityMin = 0;

				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::strcmp(pcAttrName, "DAYOFWEEK") )
					{
						wMinDay = static_cast<WORD>( ::atoi(pcAttrValue) );
						if ( CGameTime::EVERY_WEEK_CHK > wMinDay )
						{
							wMinDay *= 1440;//60*24
						}
						else
						{
							bEveryDay = true;
						}
					}
					else if ( !::strcmp(pcAttrName, "HOUR") )
					{
						WORD wHour = static_cast<WORD>( ::atoi(pcAttrValue) );
						if ( wHour > 23 )
						{
							wHour = 23;
						}

						wHour *= 60;
						kTimeKey.wMin_Open += wHour;
					}
					else if ( !::strcmp(pcAttrName, "MIN") )
					{
						WORD wMin = static_cast<WORD>( ::atoi(pcAttrValue) );
						if ( wMin > 59 )
						{
							kTimeKey.wMin_Open += 60;
							wMin = 0;
						}

						kTimeKey.wMin_Open += wMin;
					}
					else if ( !::strcmp(pcAttrName, "VALIDITY_MIN") )
					{
						wValidityMin = static_cast<WORD>( ::atoi(pcAttrValue) );
					}

					pkAttribute = pkAttribute->Next();
				}

				if ( wValidityMin )
				{
					kTimeKey.wMin_Close = kTimeKey.wMin_Open + wValidityMin;

					if ( true == bEveryDay )
					{
						WORD wValue = CGameTime::EVERY_SUNDAY;
						for ( ; wValue < CGameTime::EVERY_WEEK_CHK ; ++wValue )
						{
							STimeKey kEveryTimeKey( kTimeKey.wMin_Open + ( 1440 * wValue ), kTimeKey.wMin_Close + ( 1440 * wValue ) );
							m_kContTimekey.push_back( kEveryTimeKey );
						}
					}
					else
					{
						m_kContTimekey.push_back( kTimeKey );
					}
				}	
			}
			else if ( !::strcmp( pkChildElement->Value(), "USEMONEY") )
			{
				SKey kKey;

				pkAttribute = pkChildElement->FirstAttribute();
				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					kKey.kInvType = IT_NONE;
					kKey.iItemNo = -1;

					if( !::strcmp(pcAttrName, "MONEY") )
					{
						kKey.iItemCount = static_cast<size_t>( ::atoi(pcAttrValue) );
					}
					else if( !::strcmp(pcAttrName, "CONSUME") )
					{
						kKey.bConsume = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "PARTY") )
					{
						kKey.bParty = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::strcmp(pcAttrName, "GROUP") )
					{
						kKey.iGroup = ::atoi(pcAttrValue);
					}

					pkAttribute = pkAttribute->Next();
				}

				if ( !kKey.IsEmpty() )
				{
					m_kConKey.push_back( kKey );
					m_kConKeyGroup.insert( kKey.iGroup );
				}
			}
			else if ( !::stricmp( pkChildElement->Value(), "EFFECT") )
			{
				pkAttribute = pkChildElement->FirstAttribute();

				SEffectInfo kHave;
				SEffectInfo kNotHave;
				kHave.second = true;
				kNotHave.second = false;

				while ( pkAttribute )
				{
					pcAttrName = pkAttribute->Name();
					pcAttrValue = pkAttribute->Value();

					if( !::stricmp(pcAttrName, "HAVE_NO")
					 || !::stricmp(pcAttrName, "NOT_HAVE_NO") )
					{
						SET_INT * pkCont = !::stricmp(pcAttrName, "HAVE_NO") ? &kHave.first.kCont : &kNotHave.first.kCont;

						std::wstring const kTempStr = UNI(pcAttrValue);
						VEC_WSTRING kVec;
						PgStringUtil::BreakSep(kTempStr, kVec, L"/");
						VEC_WSTRING::const_iterator loop_iter = kVec.begin();
						while( kVec.end() != loop_iter )
						{
							int const iEffectNo = PgStringUtil::SafeAtoi((*loop_iter));
							pkCont->insert(iEffectNo);
							++loop_iter;
						}
					}
					else if ( !::stricmp(pcAttrName, "HAVE_PARTY") )
					{
						kHave.first.bParty = ( 0 != ::atoi(pcAttrValue) );
					}
					else if ( !::stricmp(pcAttrName, "NOT_HAVE_PARTY") )
					{
						kNotHave.first.bParty = ( 0 != ::atoi(pcAttrValue) );
					}

					pkAttribute = pkAttribute->Next();
				}

				CONT_EFFECT_OR kContOr;
				if(!kHave.first.kCont.empty())
				{
					kContOr.push_back(kHave);
				}
				if(!kNotHave.first.kCont.empty())
				{
					kContOr.push_back(kNotHave);
				}
				if(!kContOr.empty())
				{
					m_kContEffect.push_back(kContOr);
				}
			}
		}
		
		pkChildElement = pkChildElement->NextSiblingElement();
	}
	
	std::sort( m_kContTimekey.begin(), m_kContTimekey.end() );


	if ( !m_kTargetGroundKey.IsEmpty() )
	{
		CONT_DEFMAP::const_iterator map_itr = kContDerMap.find( m_kTargetGroundKey.GroundNo() );
		if ( map_itr != kContDerMap.end() )
		{
			T_GNDATTR kAttr = (T_GNDATTR)(map_itr->second.iAttr);
			if (	(kAttr & GATTR_FLAG_PUBLIC_CHANNEL)
				||	(kAttr & GATTR_FLAG_MISSION) 
				)
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" TargetGoundNo=") << m_kTargetGroundKey.GroundNo() << _T(" GroundAttribute=") << kAttr );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			if ( !bMoveTypeParsed )
			{
				if( kAttr & GKIND_EXPEDITION )
				{
					m_byMoveType = E_MOVE_EXPEDITIONMASTER;
				}
				else if ( kAttr & GATTR_INSTANCE )
				{
					m_byMoveType = E_MOVE_PERSONAL|E_MOVE_PARTYMASTER;
				}
				else if( kAttr & GATTR_STATIC_DUNGEON )
				{
					m_byMoveType = E_MOVE_PERSONAL|E_MOVE_ANY_PARTYMEMMBER;
				}
				else
				{
					m_byMoveType = E_MOVE_PERSONAL;
				}		
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Bad TargetGoundNo=") <<  m_kTargetGroundKey.GroundNo() );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else
	{
		//m_byMoveType = E_MOVE_LOCK;
		return false;
	}

	return true;
}

// 수행중인 퀘스트에 따라 이동할 맵이 달라진다.
void PgPortalAccess::SetQuestPortal( PgPlayer* pkPlayer )
{
	if( m_kContQuestPortal.empty() )
	{
		return ;
	}

	if( pkPlayer )
	{
		PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
		if( pkMyQuest )
		{
			CONT_QUEST_PORTAL::const_iterator c_iter = m_kContQuestPortal.begin();
			while( c_iter != m_kContQuestPortal.end() )
			{
				CONT_QUEST_PORTAL::value_type kQuestPortal = (*c_iter);

				if( pkMyQuest->IsIngQuest( kQuestPortal.iQuestID ) )
				{
					m_kTargetGroundKey = kQuestPortal.kTargetGroundKey;
					m_nTargetPortal = kQuestPortal.sTargetPortal;
					m_byMoveType = E_MOVE_PARTYMASTER;

					return ;
				}

				++c_iter;
			}
		}
	}
}

void PgPortalAccess::IsAccessInfo( PgPlayer * pkPlayer, bool const bIsPartyMaster, SPortalAccessInfo & rkOut )const
{
	if( !pkPlayer || !pkPlayer->GetInven() )
	{
		return;
	}

	if ( E_MOVE_LOCK == GetMoveType() )
	{
		rkOut.bMoveLock = true;
	}

	if( m_bUseModeLevel )
	{// 하나의 트리거에서 여러개의 던전을 입장하는 타입일 경우에는 각 던전별로 레벨제한이 다르기 때문에 따로 처리한다.
		// 레벨 체크
		if ( bIsPartyMaster || !m_bLevelParty )
		{
			if( m_ModeNo > -1 && m_ModeNo < MAX_DUNGEON_MODE )
			{
				// 레벨 체크
				rkOut.kLevel.bOk = true;
				rkOut.kLevel.iMin = m_iModeLevelLimit_Min[m_ModeNo];
				rkOut.kLevel.iMax = m_iModeLevelLimit_Max[m_ModeNo];

				int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
				if ( m_iLevelLimit_Min > iLevel )
				{
					rkOut.kLevel.bOk = false;
				}

				if ( m_iLevelLimit_Max )
				{
					if( m_iLevelLimit_Max < iLevel )
					{
						rkOut.kLevel.bOk = false;
					}
				}
			}
			else
			{
				rkOut.kLevel.bOk = false;
			}
		}
	}
	else
	{
		// 레벨 체크
		rkOut.kLevel.bOk = true;
		rkOut.kLevel.iMin = m_iLevelLimit_Min;
		rkOut.kLevel.iMax = m_iLevelLimit_Max;
		if ( bIsPartyMaster || !m_bLevelParty )
		{
			int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
			if ( m_iLevelLimit_Min > iLevel )
			{
				rkOut.kLevel.bOk = false;
			}

			if ( m_iLevelLimit_Max < iLevel )
			{
				rkOut.kLevel.bOk = false;
			}
		}
	}

	// 시간 체크
	rkOut.kTime.bOk = true;
	if ( (true == bIsPartyMaster) && m_kContTimekey.size() )
	{
		SYSTEMTIME kNowTime;
		g_kEventView.GetLocalTime( &kNowTime );

		WORD const wNowMinTime = (kNowTime.wHour * 60) + kNowTime.wMinute + ( 1440 * kNowTime.wDayOfWeek );
	
		int iRet = STimeKey::TIME_YET;
		ContTimeKey::const_iterator time_itr = m_kContTimekey.begin();
		while ( (time_itr != m_kContTimekey.end()) && (STimeKey::TIME_YET == iRet)  )
		{
			iRet = time_itr->Check(wNowMinTime);
			++time_itr;
		}

		if ( STimeKey::TIME_IN != iRet )
		{
			rkOut.kTime.bOk = false;
		}
	}

	// Quest Check
	ConKey_Base	kTempBase = m_kConQuestKeyGroup;
	ConKey_Base::iterator chk_itr = kTempBase.end();

	int iCanPassConditionCount = 0;
	for(ConQKey::const_iterator Qkey_itr=m_kConQuestKey.begin(); Qkey_itr!=m_kConQuestKey.end() ; ++Qkey_itr )
	{
		chk_itr = kTempBase.find( Qkey_itr->iGroup );
		if ( chk_itr != kTempBase.end() )
		{
			if ( bIsPartyMaster || !(Qkey_itr->bParty) )
			{
				if(!Qkey_itr->CheckClass(pkPlayer->GetAbil(AT_CLASS)))
				{
					continue;
				}

				iCanPassConditionCount = 0;

				BYTE const& rkState = Qkey_itr->byState;
				if( 0 != (rkState & (GWQT_Ing| GWQT_IngAny)) )
				{
					SUserQuestState const* pkQState = pkPlayer->GetQuestState(Qkey_itr->sQuestNo);
					if( pkQState )
					{
						if( 0 != (rkState & GWQT_Ing)
							&&	pkQState->byQuestState != QS_Failed )
						{
							++iCanPassConditionCount;
						}
						if( 0 != (rkState & GWQT_IngAny) )
						{
							++iCanPassConditionCount;
						}
					}
				}

				if( 0 != (rkState & GWQT_Ended) )
				{
					PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
					if( pkMyQuest
						&&	pkMyQuest->IsEndedQuest(Qkey_itr->sQuestNo) )
					{
						++iCanPassConditionCount;
					}
				}

				bool const bFail = (0 == iCanPassConditionCount);
				SPortalAccessInfo::SQuest const kData((*Qkey_itr), !bFail);
				SPortalAccessInfo::CONT_OR_QUEST & kCont = rkOut.kContQuest[(*chk_itr)];
				if(!bFail)
				{
					kCont.clear();
				}
				kCont.push_back(kData);

				if( bFail )
				{
					continue;
				}
			}

			kTempBase.erase( chk_itr );
			if ( kTempBase.empty() )
			{
				// Check할 Group이 비었으니까 break
				break;
			}
		}
	}
	// -------> Quest Check


	// Item & Money Check
	PgInventory * pkInv = pkPlayer->GetInven();

	kTempBase = m_kConKeyGroup;
	chk_itr = kTempBase.end();

	SPortalAccessInfo::CONT_OR_ITEM kContOrItem;
	ConKey::const_iterator key_itr = m_kConKey.begin();
	for( ; key_itr!=m_kConKey.end() ; ++key_itr )
	{
		chk_itr = kTempBase.find( key_itr->iGroup );
		if ( chk_itr != kTempBase.end() )
		{
			if ( bIsPartyMaster || !(key_itr->bParty) )
			{
				HRESULT hRet = E_FAIL;
				switch(key_itr->kInvType)
				{
				case IT_NONE:	// 돈을 처리 할땐 인벤타입을 0 으로 해줘야 한다.
					{
						__int64 const i64Money = -static_cast<__int64>((*key_itr).iItemCount);
						hRet = pkInv->AddMoney(i64Money,true); // 실제로 빼지 않고 검사만 한다.
					}break;
				default:
					{
						hRet = pkInv->GetItemModifyOrder( key_itr->kInvType, key_itr->iItemNo, key_itr->iItemCount, NULL );
					}break;
				}

				bool const bFile = FAILED(hRet);
				SPortalAccessInfo::SItem const kData((*key_itr), !bFile);
				SPortalAccessInfo::CONT_OR_ITEM & kCont = rkOut.kContItem[(*chk_itr)];
				if ( !bFile )
				{
					kCont.clear();
				}
				kCont.push_back( kData );
				if ( bFile )
				{
					continue;
				}
			}

			kTempBase.erase( chk_itr );
			if ( kTempBase.empty() )
			{
				// Check할 Group이 비었으니까 break
				break;
			}
		}
	}
	// ----> Item Check


	// Effect Check
	for(CONT_EFFECT::const_iterator cont_it = m_kContEffect.begin(); cont_it != m_kContEffect.end(); ++cont_it)
	{
		bool bOk = false;
		SPortalAccessInfo::CONT_EFFECT_OR kContOr;
		for(CONT_EFFECT::value_type::const_iterator or_it = (*cont_it).begin(); or_it != (*cont_it).end(); ++or_it)
		{
			if ( bIsPartyMaster || !(*or_it).first.bParty )
			{
				SPortalAccessInfo::SEffectInfo kInfo;
				kInfo.bHave = (*or_it).second;
				for(SET_INT::const_iterator eff_it = (*or_it).first.kCont.begin(); eff_it != (*or_it).first.kCont.end(); ++eff_it)
				{
					if ( bIsPartyMaster || !(*or_it).first.bParty )
					{
						SPortalAccessInfo::SEffect kData;
						kData.iNo = (*eff_it);
						if( (*or_it).second )	//bHave
						{
							kData.bOk = (NULL != pkPlayer->FindEffect(kData.iNo));
						}
						else
						{
							kData.bOk = (NULL == pkPlayer->FindEffect(kData.iNo));
						}
						kInfo.kCont.push_back(kData);

						if(kData.bOk)
						{
							bOk = true;
						}
					}
				}
				kContOr.push_back(kInfo);
			}
		}
		if(false == kContOr.empty())
		{
			rkOut.kContEffect.push_back(std::make_pair(kContOr,bOk));
		}
	}
	// ----> Effect Check
	return;
}

bool PgPortalAccess::IsAccess( PgPlayer *pkPlayer, bool bIsPartyMaster, CONT_PLAYER_MODIFY_ORDER *pContOrder )const
{
	if ( E_MOVE_LOCK == GetMoveType() )
	{
		pkPlayer->SendWarnMessage( 99 );//입장권한이 없습니다.
		m_eLastAccessErr = ERR_MOVE_LOCK;
		return false;
	}

	if( m_bUseModeLevel )
	{// 하나의 트리거에서 여러개의 던전을 입장하는 타입일 경우에는 각 던전별로 레벨제한이 다르기 때문에 따로 처리한다.
		// 레벨 체크
		if ( bIsPartyMaster || !m_bLevelParty )
		{
			if( m_ModeNo > -1 && m_ModeNo < MAX_DUNGEON_MODE )
			{
				int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
				if( m_iModeLevelLimit_Min[m_ModeNo] > iLevel )
				{
					pkPlayer->SendWarnMessage2( 800, m_iModeLevelLimit_Min[m_ModeNo] );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					m_eLastAccessErr = ERR_LV_MIN;
					return false;
				}
				
				if( m_iModeLevelLimit_Max[m_ModeNo] )
				{
					if ( m_iModeLevelLimit_Max[m_ModeNo] < iLevel )
					{
						pkPlayer->SendWarnMessage2( 801, m_iModeLevelLimit_Max[m_ModeNo] );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						m_eLastAccessErr = ERR_LV_MAX;
						return false;
					}
				}
			}
			else
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				m_eLastAccessErr = ERR_FAILED;
				return false;
			}
		}
	}
	else
	{
		// 레벨 체크
		if ( bIsPartyMaster || !m_bLevelParty )
		{
			int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
			if ( m_iLevelLimit_Min > iLevel )
			{
				pkPlayer->SendWarnMessage2( 800, m_iLevelLimit_Min );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				m_eLastAccessErr = ERR_LV_MIN;
				return false;
			}

			if ( m_iLevelLimit_Max < iLevel )
			{
				pkPlayer->SendWarnMessage2( 801, m_iLevelLimit_Max );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				m_eLastAccessErr = ERR_LV_MAX;
				return false;
			}
		}
	}

	// 시간 체크
	if ( (true == bIsPartyMaster) && m_kContTimekey.size() )
	{
		SYSTEMTIME kNowTime;
		g_kEventView.GetLocalTime( &kNowTime );

		WORD const wNowMinTime = (kNowTime.wHour * 60) + kNowTime.wMinute + ( 1440 * kNowTime.wDayOfWeek );
	
		int iRet = STimeKey::TIME_YET;
		ContTimeKey::const_iterator time_itr = m_kContTimekey.begin();
		while ( (time_itr != m_kContTimekey.end()) && (STimeKey::TIME_YET == iRet)  )
		{
			iRet = time_itr->Check(wNowMinTime);
			++time_itr;
		}

		if ( STimeKey::TIME_IN != iRet )
		{
			pkPlayer->SendWarnMessage( 803 );
			m_eLastAccessErr = ERR_TIME;
			return false;
		}
	}
	// Quest Check
	ConKey_Base	kTempBase = m_kConQuestKeyGroup;
	ConKey_Base::iterator chk_itr = kTempBase.end();
	
	int iCanPassConditionCount = 0;
	for(ConQKey::const_iterator Qkey_itr=m_kConQuestKey.begin(); Qkey_itr!=m_kConQuestKey.end() ; ++Qkey_itr )
	{
		chk_itr = kTempBase.find( Qkey_itr->iGroup );
		if ( chk_itr != kTempBase.end() )
		{
			if ( bIsPartyMaster || !(Qkey_itr->bParty) )
			{
				if(!Qkey_itr->CheckClass(pkPlayer->GetAbil(AT_CLASS)))
				{
					continue;
				}

				iCanPassConditionCount = 0;

				BYTE const& rkState = Qkey_itr->byState;
				if( 0 != (rkState & (GWQT_Ing| GWQT_IngAny)) )
				{
					SUserQuestState const* pkQState = pkPlayer->GetQuestState(Qkey_itr->sQuestNo);
					if( pkQState )
					{
						if( 0 != (rkState & GWQT_Ing)
							&&	pkQState->byQuestState != QS_Failed )
						{
							++iCanPassConditionCount;
						}
						if( 0 != (rkState & GWQT_IngAny) )
						{
							++iCanPassConditionCount;
						}
					}
				}

				if( 0 != (rkState & GWQT_Ended) )
				{
					PgMyQuest const* pkMyQuest = pkPlayer->GetMyQuest();
					if( pkMyQuest
						&&	pkMyQuest->IsEndedQuest(Qkey_itr->sQuestNo) )
					{
						++iCanPassConditionCount;
					}
				}

				if( 0 == iCanPassConditionCount )
				{
					continue;
				}
			}

			kTempBase.erase( chk_itr );
			if ( kTempBase.empty() )
			{
				// Check할 Group이 비었으니까 break
				break;
			}
		}
	}

	if ( kTempBase.size() )
	{
		if ( m_iErrorMessage[EMT_DEFAULT] )
		{
			pkPlayer->SendWarnMessage( m_iErrorMessage[EMT_DEFAULT] );
		}
		else
		{
			pkPlayer->SendWarnMessage( 11 );
		}

		m_eLastAccessErr = ERR_QUEST;
		return false;
	}
	// -------> Quest Check


	// Item & Money Check
	PgInventory *pkInv = pkPlayer->GetInven();
	if ( !pkInv )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Inv is NULL : CharacterGUID = ") << pkPlayer->GetID() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		m_eLastAccessErr = ERR_ITEM;
		return false;
	}

	kTempBase = m_kConKeyGroup;
	chk_itr = kTempBase.end();

	ConKey::const_iterator key_itr = m_kConKey.begin();
	for( ; key_itr!=m_kConKey.end() ; ++key_itr )
	{
		chk_itr = kTempBase.find( key_itr->iGroup );
		if ( chk_itr != kTempBase.end() )
		{
			if ( bIsPartyMaster || !(key_itr->bParty) )
			{
				HRESULT hRet = E_FAIL;
				if ( key_itr->bConsume )
				{
					switch(key_itr->kInvType)
					{
					case IT_NONE:	// 돈을 처리 할땐 인벤타입을 0 으로 해줘야 한다.
						{
							__int64 const i64Money = -static_cast<__int64>((*key_itr).iItemCount);
							hRet = pkInv->AddMoney(i64Money,true); // 실제로 빼지 않고 검사만 한다.
							if(pContOrder && S_OK == hRet)
							{
								pContOrder->push_back(SPMO(IMET_ADD_MONEY, pkPlayer->GetID(),SPMOD_Add_Money(i64Money)));
							}
						}break;
					default:
						{
							hRet = pkInv->GetItemModifyOrder( key_itr->kInvType, key_itr->iItemNo, key_itr->iItemCount, pContOrder );
						}break;
					}
				}
				else
				{
					switch(key_itr->kInvType)
					{
					case IT_NONE:	// 돈을 처리 할땐 인벤타입을 0 으로 해줘야 한다.
						{
							__int64 const i64Money = -static_cast<__int64>((*key_itr).iItemCount);
							hRet = pkInv->AddMoney(i64Money,true); // 실제로 빼지 않고 검사만 한다.
						}break;
					default:
						{
							hRet = pkInv->GetItemModifyOrder( key_itr->kInvType, key_itr->iItemNo, key_itr->iItemCount, NULL );
						}break;
					}
				}

				if ( FAILED(hRet) )
				{
					continue;
				}
			}

			kTempBase.erase( chk_itr );
			if ( kTempBase.empty() )
			{
				// Check할 Group이 비었으니까 break
				break;
			}
		}
	}

	if ( kTempBase.size() )
	{
		if ( m_iErrorMessage[EMT_DEFAULT] )
		{
			pkPlayer->SendWarnMessage( m_iErrorMessage[EMT_DEFAULT] );
		}
		else
		{
			BM::Stream kCPacket( PT_M_C_NFY_NEEDITEM_MESSAGE, static_cast<int>(0) );
			pkPlayer->Send( kCPacket );
		}

		if ( pContOrder )
		{
			pContOrder->clear();
		}

		m_eLastAccessErr = ERR_ITEM;
		return false;
	}
	// ----> Item Check


	// Effect Check
	for(CONT_EFFECT::const_iterator cont_it = m_kContEffect.begin(); cont_it != m_kContEffect.end(); ++cont_it)
	{
		VEC_INT kContHaveCheckError;
		VEC_INT kContNotHaveCheckError;
		bool bOk = false;
		for(CONT_EFFECT_OR::const_iterator or_it = (*cont_it).begin(); or_it != (*cont_it).end(); ++or_it)
		{
			if ( bIsPartyMaster || !(*or_it).first.bParty )
			{
				for(SET_INT::const_iterator eff_it = (*or_it).first.kCont.begin(); eff_it != (*or_it).first.kCont.end(); ++eff_it)
				{
					if( (*or_it).second )
					{
						if( NULL == pkPlayer->FindEffect(*eff_it) )
						{
							kContHaveCheckError.push_back( *eff_it );
						}
						else
						{
							bOk = true;
							break;
						}
					}
					else
					{
						if( NULL != pkPlayer->FindEffect(*eff_it) )
						{
							kContNotHaveCheckError.push_back( *eff_it );
						}
						else
						{
							bOk = true;
							break;
						}
					}
				}
			}
			else
			{
				bOk = true;
				break;
			}
		}

		if(false == bOk)
		{
			if(false==kContHaveCheckError.empty())
			{
				BM::Stream kData;
				kData.Push(kContHaveCheckError);
				kData.Push(679998);
				pkPlayer->SendWarnMessage3( m_iErrorMessage[EMT_HAVE_EFFECT], kData);
			}
			if(false==kContNotHaveCheckError.empty())
			{
				BM::Stream kData;
				kData.Push(kContNotHaveCheckError);
				kData.Push(679999);
				pkPlayer->SendWarnMessage3( m_iErrorMessage[EMT_NOT_HAVE_EFFECT], kData);
			}
			m_eLastAccessErr = ERR_EFFECT;
			return false;
		}
	}
	// ----> Effect Check

	m_eLastAccessErr = ERR_NONE;
	return true;
}

void PgPortalAccess::Get( SReqMapMove_MT &rkRMM )const
{
	rkRMM.kTargetKey = m_kTargetGroundKey;
	rkRMM.nTargetPortal = m_nTargetPortal;
	rkRMM.bIsBossRoom = m_bIsBossPortal;
}

void PgPortalAccess::SetModeNo(int const ModeNo)
{
	if( ModeNo > -1 && ModeNo < MAX_DUNGEON_MODE )
	{
		m_ModeNo = ModeNo;
	}
}

void PgPortalAccess::SetTargetGroundKey(int const ModeNo)
{
	if( ModeNo > -1 && ModeNo < MAX_DUNGEON_MODE )
	{
		m_kTargetGroundKey.GroundNo(m_iModeGroundNo[ModeNo]);
	}
}
