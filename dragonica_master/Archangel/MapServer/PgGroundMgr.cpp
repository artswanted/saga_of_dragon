#include "stdafx.h"
#include "NiCollision.h"
#include "BM/PgTask.h"
#include "Lohengrin/PacketType.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgWorldEvent.h"
#include "Variant/AlramMissionMgr.h"
#include "constant.h"
#include "global.h"
#include "PgTask_MapServer.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgGround.h"
#include "PgIndun.h"
#include "PgGroundMgr.h"
#include "PgMissionMan.h"
#include "PgAction.h"
#include "PgHackDualKeyboard.h"
#include "Variant/PgNpcEvent.h"
#include "PgConstellationMgr.h"
#include "Variant/PgWorldTimeCondition.h"

#define IG_LOG  /##/

namespace MapBuildData
{
	class Builder
	{
		typedef std::list< BM::vstring > CONT_ERROR_MSG;
	public:
		Builder(SERVER_IDENTITY const &kSI, CONT_MAP_CONFIG const &rkStaticData, CONT_MAP_CONFIG const &rkMissionData,
				CONT_DEFMAP const& rkDefMap, CONT_DEF_MISSION_ROOT const& rkMissionRoot, CONT_DEF_MISSION_CANDIDATE const& rkMissionCandi, CONT_MISSION_BONUSMAP const& rkMissionBonusMap, CONT_DEF_SUPER_GROUND_GROUP const& rkSuperGroundGrp)
			: m_rkSI(kSI), m_rkStaticData(rkStaticData), m_rkMissionData(rkMissionData), m_rkDefMap(rkDefMap), m_rkMissionRoot(rkMissionRoot), m_rkMissionCandi(rkMissionCandi), m_rkMissionBonusMap(rkMissionBonusMap)
			, m_rkSuperGroundGrp(rkSuperGroundGrp), m_kErrorMsg()
		{
		}
		~Builder()
		{
		}

		bool Build(CONT_MAP_BUILD_DATA &rOutAddData, bool const bOnlyMyServer, bool const bIsPublicChannel)
		{
			BuildStatic(rOutAddData, bOnlyMyServer, bIsPublicChannel); // �ʵ�
			BuildMission(rOutAddData, bOnlyMyServer); // �̼�
			// ���� ó��
			CONT_ERROR_MSG::const_iterator msg_iter = m_kErrorMsg.begin();
			while( m_kErrorMsg.end() != msg_iter )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, (*msg_iter));
				++msg_iter;
			}
			return m_kErrorMsg.empty();
		}

	private:
		void AddError(BM::vstring const& rkErrorMsg)
		{
			m_kErrorMsg.push_back(rkErrorMsg);
		}
		void AddErrorMapConfig(CONT_MAP_CONFIG::key_type const &kElement)
		{
			AddError( BM::vstring() << __FL__<<L"Not Found Ground["<<kElement.iGroundNo<<L"] : Realm["<<kElement.nRealmNo<<L"] Channel["<<kElement.nChannelNo<<L"] ServerNo["<<kElement.nServerNo<<L"]" );
		}
		void BuildStatic(CONT_MAP_BUILD_DATA &rOutAddData, bool const bOnlyMyServer, bool const bIsPublicChannel)
		{
			// Static Ground
			CONT_MAP_CONFIG::const_iterator itor = m_rkStaticData.begin();
			while(itor != m_rkStaticData.end())
			{
				CONT_MAP_CONFIG::key_type const &kElement = *itor;
				if( kElement.nRealmNo == m_rkSI.nRealm
				&&	kElement.nChannelNo == m_rkSI.nChannel )
				{
					if(	!bOnlyMyServer
					||	(0 == kElement.nServerNo) 
					||	(kElement.nServerNo == m_rkSI.nServerNo) )//���ǿ� �´� ServerNo�� ���� ��鸸
					{
						if ( kElement.iGroundNo )
						{// GroundNo�� ������ PvP Lobby�̴�.
							CONT_DEFMAP::const_iterator map_itr = m_rkDefMap.find(kElement.iGroundNo);
							if ( map_itr != m_rkDefMap.end() )
							{
								bool bCheck = true;
								if ( bIsPublicChannel )
								{// ���� ä���� ��쿡�� ���� ä���� ��� �ϴ� �׶��常
									if ( !(map_itr->second.iAttr & GATTR_FLAG_PUBLIC_CHANNEL) )
									{
										AddError( BM::vstring() << __FL__<<L"This Ground["<<kElement.iGroundNo<<L"]'s Attribute["<<map_itr->second.iAttr<<L"] : No Public Channel" );
										bCheck = false;
									}
								}
								else
								{// ���� ä���� �ƴ϶��
									if ( (map_itr->second.iAttr & GATTR_FLAG_PUBLIC_CHANNEL) )
									{
										AddError( BM::vstring() << __FL__<<L"This Ground["<<kElement.iGroundNo<<L"]'s Attribute["<<map_itr->second.iAttr<<L"] : Channel Only 0" );
										bCheck = false;
									}
								}

								if ( bCheck )
								{
									rOutAddData.insert( CONT_MAP_BUILD_DATA::key_type(kElement, static_cast<T_GNDATTR const>(map_itr->second.iAttr), map_itr->second.iKey) );
								}
							}
							else
							{
								AddErrorMapConfig(kElement);
							}
						}
						else
						{
							if ( bIsPublicChannel )
							{// ����ä�ο��� PvP Lobby�� ���� �� �ִ�.
								rOutAddData.insert( CONT_MAP_BUILD_DATA::key_type(kElement, GATTR_DEFAULT, 0) );
							}
							else
							{
								AddErrorMapConfig(kElement);
							}
						}
					}
				}
				++itor;
			}
		}
		void BuildMission(CONT_MAP_BUILD_DATA &rOutAddData, bool const bOnlyMyServer)
		{
			// Mission Ground
			CONT_MAP_CONFIG::const_iterator itor = m_rkMissionData.begin();
			while(itor != m_rkMissionData.end())
			{
				CONT_MAP_CONFIG::key_type const &kElement = *itor;
				if( kElement.nRealmNo == m_rkSI.nRealm
				&&	kElement.nChannelNo == m_rkSI.nChannel )
				{
					if(	!bOnlyMyServer
					||	(kElement.nServerNo == m_rkSI.nServerNo) )//���ǿ� �´� ServerNo�� ���� ��鸸
					{
						//->�̼��� �̼� Root�� ���� �ؼ� ��� �׶��带 ������ �;� �Ѵ�.
						CONT_DEF_MISSION_ROOT::const_iterator root_itr = m_rkMissionRoot.find(kElement.iGroundNo);
						if(root_itr != m_rkMissionRoot.end() )
						{
							for(int i=0; i!=DEFENCE8_MISSION_LEVEL; ++i) // MAX_MISSION_LEVEL
							{
								int iDefenceLevel = 0;
								if( i >= (MAX_MISSION_LEVEL) ) // DEFENCE_MISSION_LEVEL
								{
									switch( i )
									{
									case DEFENCE7_MISSION_LEVEL:
										{
											iDefenceLevel = root_itr->second.iDefence7;
										}break;
									case DEFENCE8_MISSION_LEVEL:
										{
											iDefenceLevel = root_itr->second.iDefence8;
										}break;
									case DEFENCE_MISSION_LEVEL:									
										{
											iDefenceLevel = root_itr->second.iDefence;
										}break;
									default:
										{
											iDefenceLevel = 0;
										}break;
									}
									if( !AddMissionLevel(kElement, iDefenceLevel, rOutAddData) )
									{
										AddError( BM::vstring() << __FL__ << L"wrong CandidateNo["<<i<<L"]" );
									}
								}
								else
								{
									if( !AddMissionLevel(kElement, root_itr->second.aiLevel[i], rOutAddData) )
									{
										AddError( BM::vstring() << __FL__ << L"wrong CandidateNo["<<i<<L"]" );
									}
								}
							}
						}
						else
						{
							if( !AddSuperGround(kElement, rOutAddData) )
							{
								AddError( BM::vstring() << __FL__ <<L"Error GroundNo["<<kElement.iGroundNo<<L"], can't find in [TB_DefMission_LevelRoot or TB_DefSuperGround] table" );
							}
						}
					}
				}
				++itor;
			}
		}
		bool AddSuperGround(CONT_MAP_CONFIG::key_type const& kElement, CONT_MAP_BUILD_DATA& rOutAddData)
		{
			CONT_DEF_SUPER_GROUND_GROUP::const_iterator super_iter = m_rkSuperGroundGrp.find(kElement.iGroundNo);
			if( m_rkSuperGroundGrp.end() != super_iter )
			{
				CONT_DEF_SUPER_GROUND_GROUP::mapped_type const& rkSuperGroundGrp = (*super_iter).second;
				CONT_SUPER_GROUND::const_iterator gnd_iter = rkSuperGroundGrp.kContSuperGround.begin();
				while( rkSuperGroundGrp.kContSuperGround.end() != gnd_iter )
				{
					CONT_SUPER_GROUND::value_type const& rkSuperGnd = (*gnd_iter);
					CONT_DEFMAP::const_iterator map_iter = m_rkDefMap.find(rkSuperGnd.iGroundNo);
					if( m_rkDefMap.end() != map_iter )
					{
						CONT_DEFMAP::mapped_type const& rkDefMap = (*map_iter).second;
						if( 0 != (rkDefMap.iAttr&GATTR_FLAG_SUPER) )
						{
							CONT_MAP_BUILD_DATA::key_type kInsertElement(kElement, static_cast<T_GNDATTR const>(rkDefMap.iAttr), rkDefMap.iKey);
							kInsertElement.iGroundNo = rkSuperGnd.iGroundNo;
							rOutAddData.insert(kInsertElement);
						}
						else
						{
							AddError( BM::vstring() << __FL__<<L"Error Ground["<<rkSuperGnd.iGroundNo<<L"] Attribute["<<rkDefMap.iAttr<<L"] -> SuperGround Have Attribute is "<<GATTR_SUPER_GROUND );
						}
					}
					else
					{
						AddErrorMapConfig(kElement);
					}
					++gnd_iter;
				}
			}
			else
			{
				return false;
			}
			return true;
		}
		bool AddMissionLevel(CONT_MAP_CONFIG::key_type const &kElement, int const iLevel, CONT_MAP_BUILD_DATA &rOutAddData)
		{
			if( 0 < iLevel )
			{
				CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr = m_rkMissionCandi.find(iLevel);
				if( candi_itr != m_rkMissionCandi.end() )
				{
					for(int j=0; j!=MAX_MISSION_CANDIDATE; ++j)
					{
						AddMissionGround(kElement, candi_itr->second.aiCandidate[j], rOutAddData);
					}
					AddMissionGround(kElement, candi_itr->second.iCandidate_End, rOutAddData);

					CONT_MISSION_BONUSMAP::key_type kKey(iLevel);
					CONT_MISSION_BONUSMAP::const_iterator iter = m_rkMissionBonusMap.find(kKey);
					if( m_rkMissionBonusMap.end() != iter )
					{
						CONT_MISSION_BONUSMAP::mapped_type const& rkBonusMap = (*iter).second;
						AddMissionGround(kElement, rkBonusMap.iBonusMap1, rOutAddData);
						AddMissionGround(kElement, rkBonusMap.iBonusMap2, rOutAddData);
					}					
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		void AddMissionGround(CONT_MAP_CONFIG::key_type const &kElement, int const iGroundNo, CONT_MAP_BUILD_DATA &rOutAddData)
		{
			if( iGroundNo )
			{
				CONT_DEFMAP::const_iterator map_itr = m_rkDefMap.find(iGroundNo);
				if ( map_itr != m_rkDefMap.end() )
				{
					if( 0 == (map_itr->second.iAttr&GATTR_MISSION) )
					{
						AddError( BM::vstring() << __FL__<<L"Error Ground["<<iGroundNo<<L"] Attribute["<<map_itr->second.iAttr<<L"] -> Mission Have Attribute is "<<GATTR_MISSION );
					}

					CONT_MAP_BUILD_DATA::key_type kInsertElement(kElement, static_cast<T_GNDATTR const>(map_itr->second.iAttr), map_itr->second.iKey);
					kInsertElement.iGroundNo = iGroundNo;
					rOutAddData.insert(kInsertElement);
				}
				else
				{
					AddError( BM::vstring() << __FL__<<L"Not Found Ground["<<iGroundNo<<L"] In MissionNo["<<kElement.iGroundNo<<L"] : Realm["<<kElement.nRealmNo<<L"] Channel["<<kElement.nChannelNo<<L"] ServerNo["<<kElement.nServerNo<<L"]");
				}
			}
		}
	private:
		SERVER_IDENTITY const &m_rkSI;
		CONT_MAP_CONFIG const &m_rkStaticData;
		CONT_MAP_CONFIG const &m_rkMissionData;
		CONT_DEFMAP const& m_rkDefMap;
		CONT_DEF_MISSION_ROOT const& m_rkMissionRoot;
		CONT_DEF_MISSION_CANDIDATE const& m_rkMissionCandi;
		CONT_MISSION_BONUSMAP const& m_rkMissionBonusMap;
		CONT_DEF_SUPER_GROUND_GROUP const& m_rkSuperGroundGrp;

		CONT_ERROR_MSG m_kErrorMsg;
	};
};

PgGroundRscMgr::PgGroundRscMgr()
:	m_kGndResourcePool(16,8)
{

}

PgGroundRscMgr::~PgGroundRscMgr()
{
	Clear();
}

bool PgGroundRscMgr::GetMapBuildData(	SERVER_IDENTITY const &kSI,
										CONT_MAP_CONFIG const &rkStaticData,
										CONT_MAP_CONFIG const &rkMissionData,
										CONT_DEFMAP const* pkDefMap,
										CONT_DEF_MISSION_ROOT const* pkMissionRoot,
										CONT_DEF_MISSION_CANDIDATE const* pkMissionCandi,
										CONT_MISSION_BONUSMAP const* pkMissionBonusMap,
										CONT_DEF_SUPER_GROUND_GROUP const* pkSuperGroundGrp,
										CONT_MAP_BUILD_DATA &rOutAddData,
										bool const bOnlyMyServer
									)
{
	rOutAddData.clear();

	if( !pkDefMap
	||	!pkMissionRoot
	||	!pkMissionCandi )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkDefMap is NULL"));
		return false;
	}

	MapBuildData::Builder kBuilder(kSI, rkStaticData, rkMissionData, *pkDefMap, *pkMissionRoot, *pkMissionCandi, *pkMissionBonusMap, *pkSuperGroundGrp);
	return kBuilder.Build(rOutAddData, bOnlyMyServer, g_kProcessCfg.IsPublicChannel());
}

HRESULT PgGroundRscMgr::Reserve( CONT_MAP_BUILD_DATA &rHash )
{//�׶��� ���
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );

	HRESULT hRet = S_OK;
	GroundArr kSendGndArr;
	GroundNoArr	kSendBalancingGndArr;	// �ε� �뷱���� ���� ����(Contents)

	// All_Npc.xm
	if( FAILED( LoadAllNpc() ) )
	{
		ASSERT_LOG(false, BM::LOG_LV0, __FL__ << _T("Load All_Npc.xml failed"));
		return hRet;
	}

	if( FAILED( LoadConstellation() ) )
	{
		ASSERT_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Constellation.xml failed"));
		return S_FALSE;
	}

	CONT_MAP_BUILD_DATA::iterator itr = rHash.begin();
	while( itr!=rHash.end() )
	{
		CONT_MAP_BUILD_DATA::key_type const &kElement = *itr;
		if ( kElement.iGroundNo )
		{
			PgAlramMissionMgr_Warpper* pkAlramMissionMgr = NULL;

			ALRAM_MISSION::eAlramType const kAlramMissionType = PgAlramMissionMgr::GetAlramType( kElement.kGndAttr );
			if ( ALRAM_MISSION::AMT_NONE != kAlramMissionType )
			{
				CONT_ALRAM_MISSION_MGR::iterator alram_itr = m_kContAlramMissionMgr.find( static_cast<CONT_ALRAM_MISSION_MGR::key_type>(kAlramMissionType) );
				if ( alram_itr == m_kContAlramMissionMgr.end() )
				{
					pkAlramMissionMgr = new_tr PgAlramMissionMgr_Warpper;
					if ( pkAlramMissionMgr )
					{
						CONT_DEF_ALRAM_MISSION const *pkDefAlramMission = NULL;
						g_kTblDataMgr.GetContDef( pkDefAlramMission );

						if ( SUCCEEDED( pkAlramMissionMgr->Init( kAlramMissionType, *pkDefAlramMission ) ) )
						{
							m_kContAlramMissionMgr.insert( std::make_pair( kAlramMissionType, pkAlramMissionMgr ) );
						}
						else
						{
							SAFE_DELETE( pkAlramMissionMgr );
							CAUTION_LOG( BM::LOG_LV4, L"CONT_DEF_ALRAM_MISSION data is Error!!! AlramType<" << kAlramMissionType << L">" );
						}
					}
				}
				else
				{
					pkAlramMissionMgr = alram_itr->second;
				}
			}

			if ( FAILED(Locked_AddGroundResource(kElement.iGroundNo, kElement.kGndAttr, pkAlramMissionMgr)) )
			{
				INFO_LOG( BM::LOG_LV0, __FL__<<L"AddGroundResource Failed GroundNo["<<kElement.iGroundNo<<L"]" );
				hRet = S_FALSE;
				itr = rHash.erase(itr);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Locked_AddGroundResource Failed!"));
			}
			else
			{
				++itr;
			}
		}
	}
	if(hRet != S_OK)
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Add Ground Resource failed"));
	}
	return hRet;
}

HRESULT PgGroundRscMgr::Locked_AddGroundResource(int const iGroundNo, T_GNDATTR const kAttr, PgAlramMissionMgr_Warpper const * pkAlramMissionMgr )
{
	BM::CAutoMutex kWLock(m_kRscLock, true);

	GND_RESOURCE_CONT::const_iterator rsc_itor = m_kGndResourceCon.find(iGroundNo);
	if(m_kGndResourceCon.end() != rsc_itor)
	{//�̹� ����
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	CONT_DEFMAP const *pContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pContDefMap);

	CONT_DEFMAP::const_iterator map_itor = pContDefMap->find(iGroundNo);// DB�� ��� �Ǿ��ִ� �׶��� �ΰ�?
	if(map_itor == pContDefMap->end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Can't Find DefMap MapNo["<<iGroundNo<<L"]" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	std::wstring wstrXmlFileName = _T("./XML/");
	wstrXmlFileName += (std::wstring)((map_itor->second).strXMLPath);

	PgGroundResource* pkGndRsc = m_kGndResourcePool.New();
	if(	!pkGndRsc )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<iGroundNo<<L" Ground Resoruce Memory Pool Error!" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_OUTOFMEMORY"));
		return E_OUTOFMEMORY;
	}

	pkGndRsc->GroundKey( SGroundKey(iGroundNo) );
	pkGndRsc->SetAttr( kAttr );
	pkGndRsc->SetAlramMissionMgr( pkAlramMissionMgr );
	if (S_OK != pkGndRsc->BuildAbil())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Cannot build ground abil GroundNo=") << iGroundNo);
		goto __FAILED;
	}

	INFO_LOG( BM::LOG_LV6, _T("-[LoadMap]------------------------------------------------------") );
	INFO_LOG( BM::LOG_LV6, wstrXmlFileName );
	if ( S_OK != ParseXml( pkGndRsc, MB(wstrXmlFileName) ) )
	{
		INFO_LOG( BM::LOG_LV0, _T("=======================================================[Failed]=") );
		goto __FAILED;
	}
	else
	{
		// All_Npc.xml�� ���ǵ� NPC �߰� ���
		CONT_WORLD_NPC::const_iterator c_worldNpcIter = m_kContWorldNpc.find( iGroundNo );
		if( c_worldNpcIter != m_kContWorldNpc.end() )
		{
			CONT_WORLD_NPC::mapped_type kContNpc = c_worldNpcIter->second;
			CONT_NPC::const_iterator c_NpcIter = kContNpc.begin();
			while( kContNpc.end() != c_NpcIter )
			{
				CONT_NPC::mapped_type kElement = c_NpcIter->second;

				if( !pkGndRsc->AddNPC( kElement.kName.c_str(), kElement.kActorName.c_str(), kElement.kScriptName.c_str(), kElement.kLocation.c_str(), kElement.kNpcGuid, kElement.kNpcType, kElement.GiveEffectNo ) )
				{
					INFO_LOG( BM::LOG_LV0, _T("=======================================================[Failed]=") );
					goto __FAILED;
				}

				++c_NpcIter;
			}
		}
	}
	INFO_LOG( BM::LOG_LV6, _T("======================================================[Success]=") );

	auto ret = m_kGndResourceCon.insert( std::make_pair( iGroundNo, pkGndRsc ) );
	if(ret.second)
	{
		return S_OK;	
	}

__FAILED:
	{
		if(pkGndRsc)
		{
			m_kGndResourcePool.Delete( pkGndRsc );
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
}

HRESULT PgGroundRscMgr::ParseXml( PgGroundResource *pkGndResource, char const *pcXmlPath )
{
	TiXmlDocument kXmlDoc(pcXmlPath);
	if( !kXmlDoc.LoadFile(pcXmlPath) )
	{
		if( kXmlDoc.Error() )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("GroundNo[") << pkGndResource->GroundKey().GroundNo() << _T(" Loadfile Error: ") << UNI(pcXmlPath) << _T(" Row[") << kXmlDoc.ErrorRow() << _T("] Column[") << kXmlDoc.ErrorCol() << _T("] ErrorDesc[") << UNI(kXmlDoc.ErrorDesc()) << _T("]"));
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("GroundNo[") << pkGndResource->GroundKey().GroundNo() << _T(", LoadFile Error:") << UNI(pcXmlPath));
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	TiXmlElement const *pkParentElement = kXmlDoc.FirstChildElement();
	while ( pkParentElement )
	{
		if ( !::strcmp( pkParentElement->Value(), "WORLD") )
		{
			TiXmlElement const *pkElement = pkParentElement->FirstChildElement();

			if( !ParseWorldXml(pkGndResource, pkElement) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Wrong MapXML File =") << UNI(pcXmlPath) );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			pkGndResource->BuildWorldEventLinkedData();
		}
		pkParentElement = pkParentElement->NextSiblingElement();
	}

	return S_OK;
}

bool PgGroundRscMgr::ParseWorldXml( PgGroundResource *pkGndResource, TiXmlElement const *pkElement )
{
	CONT_GTRIGGER kContTrigger;
	std::string kEventID;

	bool bRet = true;
	TiXmlAttribute const *pkAttr = NULL;
	TiXmlElement const *pkChildElement = NULL;
	while ( pkElement )
	{
		if ( !::strcmp( pkElement->Value(), "LOCAL") )
		{
			TiXmlNode const* pkFindLocalNode = PgXmlLocalUtil::FindInLocal(g_kLocal, pkElement);
			if( pkFindLocalNode )
			{
				TiXmlElement const* pkResultNode = pkFindLocalNode->FirstChildElement();
				if( pkResultNode )
				{
					ParseWorldXml( pkGndResource, pkResultNode );
				}
			}
		}
		else if ( !::strcmp( pkElement->Value(), "TIME_EVENT") )
		{
			PgWorldTimeCondition kTimeCondition;
			kTimeCondition.SetConditionAttributeName("FROM","TO");
			if( kTimeCondition.ReadFromXmlElement(pkElement) )
			{
				SYSTEMTIME kNow;
				::GetLocalTime( &kNow );
				if(true==kTimeCondition.CheckTimeIsInsideDuration(kNow))
				{
					kEventID = kTimeCondition.GetID();
				}
			}
		}
		// Load NIF
		else if ( !::strcmp( pkElement->Value(), "NIFPATH") )
		{
			char const *pkPath = pkElement->GetText();
			if ( pkPath )
			{
				if ( (pkPath[0] == '.') && (pkPath[1] == '.') )
				{
					++pkPath;
				}
				pkGndResource->LoadNif( pkPath );
			}	
		}

		// Load GSA
		else if ( !::strcmp( pkElement->Value(), "GSAPATH") )
		{

			std::string kPath;
			bool	bPathLoaded = false;

			TiXmlNode const	*pkNode = static_cast<TiXmlNode const*>(pkElement);

			if(pkNode)
			{
				const TiXmlNode * pkChildNode = pkNode->FirstChild();
				if(pkChildNode && pkChildNode->Type() == TiXmlNode::ELEMENT)
				{
					bPathLoaded = GetGsaPath(pkChildNode, kEventID, kPath);
				}
			}

			if(!bPathLoaded)
			{
				kPath = pkElement->GetText();
			}

			if ( kPath.empty() == false )
			{
				const	char	*pkPath = kPath.c_str();

				if ( (pkPath[0] == '.') && (pkPath[1] == '.') )
				{
					++pkPath;
				}

				if ( FAILED(pkGndResource->LoadGsa( pkPath )) )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return false;
				}
			}
		}

		// NPC
		else if ( !::strcmp( pkElement->Value(), "NPC") )
		{
			pkGndResource->AddNPC( pkElement->FirstAttribute() );
		}

		// Crystal Stones
		else if ( !::strcmp( pkElement->Value(), "STONES") )
		{
			int iMin = 0;
			int iMax = 0;
			pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !::strcmp( pkAttr->Name(), "MINIMAM") )
				{
					iMin = ::atoi( pkAttr->Value() );
				}
				else if ( !::strcmp( pkAttr->Name(), "MAXIMAM" ) )
				{
					iMax = ::atoi( pkAttr->Value() );
				}
				pkAttr = pkAttr->Next();
			}

			pkChildElement = pkElement->FirstChildElement();
			while ( pkChildElement )
			{
				if ( !::strcmp( pkChildElement->Value(), "STONE" ) )
				{
					pkGndResource->AddStone( pkChildElement->FirstAttribute() );
				}
				pkChildElement = pkChildElement->NextSiblingElement();
			}
		}

		else if ( !::strcmp( pkElement->Value(), "TRIGGER") )
		{
			pkGndResource->AddGTrigger( pkElement );
		}

		//
		else if ( !::strcmp( pkElement->Value(), "OPENING_MOVIE") )
		{
			pkGndResource->OpeningMovie( true );
		}
		else if( !::strcmp( pkElement->Value(), "RESULT") )
		{
			pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !::strcmp( pkAttr->Name(), "SPAWN_LOC") )
				{
					POINT3 pt3Pos;
					::sscanf_s( pkAttr->Value(), "%f,%f,%f", &pt3Pos.x, &pt3Pos.y, &pt3Pos.z );
					pkGndResource->SetResultSpawnLoc( pt3Pos );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if ( !::strcmp( pkElement->Value(), "FREE_DIRECTION") )
		{
			TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
			char const *pcAttrName = NULL;
			char const *pcAttrValue = NULL;
			while ( pkAttribute )
			{
				pcAttrName = pkAttribute->Name();
				pcAttrValue = pkAttribute->Value();

				bool bTrue = (strcmp(pcAttrValue, "TRUE") == 0);

				if ( !::strcmp( pcAttrName, "ALL") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_ALLUNIT);
				}
				else if ( !::strcmp( pcAttrName, "PLAYER") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_PLAYER);
				}
				else if ( !::strcmp( pcAttrName, "MONSTER") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_MONSTER);
				}
				else if ( !::strcmp( pcAttrName, "NPC") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_NPC);
				}
				else if ( !::strcmp( pcAttrName, "BOSS") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_BOSSMONSTER);
				}
				else if ( !::strcmp( pcAttrName, "PET") && bTrue )
				{
					pkGndResource->AddUnlockBidirection(UT_PET);
				}

				pkAttribute = pkAttribute->Next();
			}
		}
		else if( !::strcmp( pkElement->Value(), "EVENT_SCRIPT_STOP_AI") )
		{
			pkGndResource->AddEventScriptStopAI( pkElement );
		}
		else if( !::strcmp( pkElement->Value(), "EVENT_SCRIPT_NO_LOCK") )
		{
			pkGndResource->AddEventScriptNoLock( pkElement );
		}
		else if( !::strcmp( pkElement->Value(), "MONSTER") )
		{
			pkAttr = pkElement->FirstAttribute();
			while ( pkAttr )
			{
				if ( !::strcmp( pkAttr->Name(), "MAX_REGEN_COUNT") )
				{
					int const iCount = ::atoi( pkAttr->Value() );
					pkGndResource->MaxMonsterCount( iCount );
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if( !::strcmp( pkElement->Value(), "MAX_SPAWN_LOCATION") )
		{
			pkAttr = pkElement->FirstAttribute();
			while( pkAttr )
			{
				if( !::strcmp( pkAttr->Name(), "COUNT") )
				{
					int const Count = ::atoi( pkAttr->Value() );
					pkGndResource->MaxSpawnLocationCount(Count);
				}
				pkAttr = pkAttr->Next();
			}
		}
		else if( !::strcmp( pkElement->Value(), "SPAWN_COUNT_PER_LOCATION") )
		{
			pkAttr = pkElement->FirstAttribute();
			while( pkAttr )
			{
				if( !::strcmp( pkAttr->Name(), "COUNT") )
				{
					int const Count = ::atoi( pkAttr->Value() );
					pkGndResource->SpawnCountPerLocation(Count);
				}
				pkAttr = pkAttr->Next();
			}
		}
		// World Event
		else if( PgWorldEventUtil::kWorldEventElement == pkElement->Value() )
		{
			pkGndResource->ParseWorldEvent( pkElement );
		}
		else if( !::strcmp( pkElement->Value(), "WORLDEVENT_OBJECT_LIST") )
		{
			pkGndResource->ParseWEClientObjectList( pkElement );
		}
		// Dungeon Minimap
		else if( ::strcmp( pkElement->Value(), "PROGRESS_MAP") == 0)
		{
		}
		else if( ::strcmp( pkElement->Value(), "PROGRESS") == 0)
		{
		}
		else if( ::strcmp( pkElement->Value(), "PROGRESS_WAY") == 0)
		{
		}
		// Client Only
		else if( !::strcmp( pkElement->Value(), "MINIMAP") )					{}
		else if( !::strcmp( pkElement->Value(), "SCRIPT") )						{}
		else if( !::strcmp( pkElement->Value(), "BLOOM") )						{}
		else if( !::strcmp( pkElement->Value(), "MAPVALUE")	)					{}
		else if( !::strcmp( pkElement->Value(), "FREE_DIRECTION") )				{}
		else if( !::strcmp( pkElement->Value(), "CAMERA_ADJUST") )				{}
		else if( !::strcmp( pkElement->Value(), "FOG") )						{}
		else if( !::strcmp( pkElement->Value(), "WORLD_QUALITY") )				{}
		else if( !::strcmp( pkElement->Value(), "MINIMAP") )					{}
		else if( !::strcmp( pkElement->Value(), "CAMERA_KFMPATH") )				{}
		else if( !::strcmp( pkElement->Value(), "WORLD_ACTION_SETTING") )		{}
		else if( !::strcmp( pkElement->Value(), "COMPLETE_PATH") )				{}
		else if( !::strcmp( pkElement->Value(), "VIEW_DISTANCE") )				{}
		else if( !::strcmp( pkElement->Value(), "BGSOUND") )					{}
		else if( !::strcmp( pkElement->Value(), "BGSOUND_SUB") )				{}
		else if( !::strcmp( pkElement->Value(), "EVENT_SCRIPT_ON_ENTER") )		{}
		else if( !::strcmp( pkElement->Value(), "WAVE_EFFECT") )				{}
		else if( !::strcmp( pkElement->Value(), "SPOTLIGHT") )					{}
		else if( !::strcmp( pkElement->Value(), "OBJECT") )						{}
		else if( !::strcmp( pkElement->Value(), "BUILDING") )					{}
		else if( !::strcmp( pkElement->Value(), "ROPE") )						{}
		else if( !::strcmp( pkElement->Value(), "DISABLE_RANDOM_ANI") )			{}
		else if( !::strcmp( pkElement->Value(), "MAP_MOVE_COMPLETE_FADE") )		{}
		else if( !::strcmp( pkElement->Value(), "DUNGEON_EXIT_UI") )		{}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Wrong Element[") << pkElement->Value() << _T("] name"));
			bRet = false;
		}

		pkElement = pkElement->NextSiblingElement();
	}

	return bRet;
}

TiXmlNode const* PgGroundRscMgr::ParseAllNpcXml( TiXmlNode const *pkNode, int& rikOutGroundNo)
{
	int const iType = pkNode->Type();

	switch(iType)
	{
	case TiXmlNode::ELEMENT:
		{
			TiXmlElement *pkElement = (TiXmlElement *)pkNode;

			std::string kTagName(pkElement->Value());

			if( kTagName == "LOCAL")
			{
				TiXmlNode const* pkFindLocalNode = PgXmlLocalUtil::FindInLocal(g_kLocal, pkElement);
				if( pkFindLocalNode )
				{
					TiXmlNode const* pkResultNode = pkFindLocalNode->FirstChildElement();
					while( pkResultNode )
					{
						pkResultNode = ParseAllNpcXml( pkResultNode, rikOutGroundNo );
					}
				}
			}
			else if( kTagName == "WORLD" )
			{
				TiXmlAttribute* pkAttr = pkElement->FirstAttribute();
				while( pkAttr )
				{
					char const* pcAttrName = pkAttr->Name();
					char const* pcAttrValue = pkAttr->Value();

					if( strcmp( pcAttrName, "MAPNO" ) == 0 )
					{
						rikOutGroundNo = atoi(pcAttrValue);
					}

					pkAttr = pkAttr->Next();
				}

				const TiXmlNode* pkChildNode = pkNode->FirstChild();
				if( pkChildNode != 0 )
				{
					while( ( pkChildNode = ParseAllNpcXml( pkChildNode, rikOutGroundNo ) ) );
				}
			}
			else if( kTagName == "NPC" )
			{
				SWorldNpc kWorldNpc;

				TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
				while ( pkAttribute )
				{
					if ( !::strcmp( pkAttribute->Name(), "NAME") )
					{
						kWorldNpc.kName = pkAttribute->Value();
					}
					else if ( !::strcmp( pkAttribute->Name(), "ACTOR") )
					{
						kWorldNpc.kActorName = pkAttribute->Value();
					}
					else if ( !::strcmp( pkAttribute->Name(), "SCRIPT") )
					{
						kWorldNpc.kScriptName = pkAttribute->Value();
					}
					else if ( !::strcmp( pkAttribute->Name(), "LOCATION") )
					{
						kWorldNpc.kLocation = pkAttribute->Value();
					}
					else if ( !::strcmp( pkAttribute->Name(), "GUID") )
					{
						kWorldNpc.kNpcGuid.Set( pkAttribute->Value() );
					}
					else if ( !::strcmp( pkAttribute->Name(), "KID") )
					{
						kWorldNpc.iNpcKID = ::atoi( pkAttribute->Value() );
					}
					else if ( !::strcmp( pkAttribute->Name(), "TYPE") )
					{
						if (_stricmp( pkAttribute->Value(), "FIXED"))
						{
							kWorldNpc.kNpcType = E_NPC_TYPE_FIXED;
						}
						else if (_stricmp( pkAttribute->Value(), "DYNAMIC"))
						{
							kWorldNpc.kNpcType = E_NPC_TYPE_DYNAMIC;
						}
						else if (_stricmp( pkAttribute->Value(), "CREATURE"))
						{
							kWorldNpc.kNpcType = E_NPC_TYPE_CREATURE;
						}
					}
					else if ( !::strcmp( pkAttribute->Name(), "PARAM") )
					{
						kWorldNpc.iParam = ::atoi( pkAttribute->Value() );
					}
					else if ( !::strcmp( pkAttribute->Name(), "GIVE_EFFECTNO") )
					{
						kWorldNpc.GiveEffectNo = ::atoi( pkAttribute->Value() );
					}

					pkAttribute = pkAttribute->Next();
				}

				AddNpc( kWorldNpc );
			}
			else if( kTagName == "NPC_EVENT" )
			{
				int const iMapNo = rikOutGroundNo;
				g_kNpcEventMgr.Parse(pkElement, iMapNo);
			}
			else
			{

			}
		}break;
	default:
		{
		}break;
	}

	// ���� ���� ���� ��带 ��������� �Ľ��Ѵ�.
	const TiXmlNode* pkNextNode = pkNode->NextSibling();
	if(pkNextNode)
	{
		return pkNextNode;
	}

	return NULL;
}

bool PgGroundRscMgr::AddNpc( const SWorldNpc& rkWorldNpc )
{
	auto ret = m_kContNpc.insert( std::make_pair( rkWorldNpc.kNpcGuid, rkWorldNpc ) );
	if( !ret.second )
	{
		return false;
	}

	return true;
}

bool PgGroundRscMgr::AddWorldNpc( const int iGroundNo, const CONT_NPC& kContNpc )
{
	auto ret = m_kContWorldNpc.insert( std::make_pair( iGroundNo, kContNpc ) );
	if( !ret.second )
	{
		return false;
	}

	m_kContNpc.clear();
	return true;
}
			

HRESULT PgGroundRscMgr::LoadAllNpc()
{
	char const *pcXmlPath = "./XML/All_Npc.xml";

	TiXmlDocument kXmlDoc;
	if( !kXmlDoc.LoadFile(pcXmlPath) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("LoadFile Error:") << UNI(pcXmlPath));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if( m_kContWorldNpc.size() )	{ m_kContWorldNpc.clear(); }
	if( m_kContNpc.size() )			{ m_kContNpc.clear(); }

	TiXmlNode* pkRootNode = kXmlDoc.FirstChild("TOTALNPC");
	if( pkRootNode )
	{
		TiXmlNode* pkNextNode = pkRootNode->FirstChild();
		while( pkNextNode )
		{
			int iOutGroundNo = 0;
			ParseAllNpcXml( pkNextNode, iOutGroundNo );
			AddWorldNpc( iOutGroundNo, m_kContNpc );

			pkNextNode = pkNextNode->NextSibling();
		}
	}
	
	return S_OK;
}

HRESULT PgGroundRscMgr::LoadConstellation()
{
	char const *pcXmlPath = "./XML/ConstellationSetting.xml";

	TiXmlDocument XmlDoc;
	if( !XmlDoc.LoadFile(pcXmlPath) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("LoadFile Error:") << UNI(pcXmlPath));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_ConstellationMgr.Clear();

	TiXmlNode* pRootNode = XmlDoc.FirstChild("CONSTELLATION");
	if( pRootNode )
	{
		CONT_DEFITEM const* pDefItem = NULL;
		g_kTblDataMgr.GetContDef(pDefItem);
		CONT_DEFMONSTER const* pDefMonster = NULL;
		g_kTblDataMgr.GetContDef(pDefMonster);
		CONT_DEF_QUEST_REWARD const* pDefQuestReward = NULL;
		g_kTblDataMgr.GetContDef(pDefQuestReward);

		if( pDefItem && pDefMonster && pDefQuestReward )
		{
			TiXmlNode* pNextNode = pRootNode->FirstChild();
			while( pNextNode )
			{
				int OutGroundNo = 0;
				m_ConstellationMgr.ParseXml( pNextNode, pDefItem, pDefMonster, pDefQuestReward, OutGroundNo );
				pNextNode = pNextNode->NextSibling();
			}
		}
	}

	return S_OK;
}

bool	PgGroundRscMgr::GetDefaultGsaPath(TiXmlNode const *pkNode,std::string &kPath)const
{
	int const iType = pkNode->Type();

	switch(iType)
	{
	case TiXmlNode::ELEMENT:
		{
			TiXmlElement *pkElement = (TiXmlElement *)pkNode;

			std::string kTagName(pkElement->Value());
			if(kTagName == "CONDITION")
			{
				std::string	kType;

				const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
				while(pkAttr)
				{
					std::string kAttrName(pkAttr->Name());
					std::string kAttrValue(pkAttr->Value());

					if(kAttrName == "TYPE")
					{
						kType = kAttrValue;
					}

					pkAttr = pkAttr->Next();
				}

				if(kType == "DEFAULT")
				{
					kPath = pkElement->GetText();
					return	true;
				}
			}
		}
		break;
	}

	// ���� ���� ���� ��带 ��������� �Ľ��Ѵ�.
	while(const TiXmlNode* pkNextNode = pkNode->NextSibling())
	{
		if(GetDefaultGsaPath(pkNextNode,kPath))
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return	false;
}

bool	PgGroundRscMgr::GetGsaPathByID(TiXmlNode const *pkNode, std::string const& kEventID, std::string &kPath)const
{
	if(kEventID.empty())
	{
		return false;
	}

	int const iType = pkNode->Type();

	switch(iType)
	{
	case TiXmlNode::ELEMENT:
		{
			TiXmlElement *pkElement = (TiXmlElement *)pkNode;

			std::string kTagName(pkElement->Value());
			if(kTagName == "CONDITION")
			{
				std::string	kType;

				const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
				while(pkAttr)
				{
					std::string kAttrName(pkAttr->Name());
					std::string kAttrValue(pkAttr->Value());

					if(kAttrName == "ID")
					{
						kType = kAttrValue;
					}

					pkAttr = pkAttr->Next();
				}

				if(kEventID == kType)
				{
					kPath = pkElement->GetText();
					return true;
				}
			}
		}
		break;
	}

	// ���� ���� ���� ��带 ��������� �Ľ��Ѵ�.
	while(const TiXmlNode* pkNextNode = pkNode->NextSibling())
	{
		if(GetGsaPathByID(pkNextNode, kEventID, kPath))
		{
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return	false;
}

bool	PgGroundRscMgr::GetGsaPath(TiXmlNode const *pkNode, std::string const& kEventID, std::string &kPath)const
{
	std::string kDefaultPath;
	GetDefaultGsaPath(pkNode, kDefaultPath);

	if(GetGsaPathByID(pkNode, kEventID, kPath))
	{
		return true;
	}

	kPath = kDefaultPath;
	return false==kPath.empty();
}

HRESULT PgGroundRscMgr::Locked_GetGroundResource(int const iGroundNo, PgGroundResource const *&rkOut)const	// �׶��� ���ҽ��� ��ȯ�Ѵ�.
{
	BM::CAutoMutex kWLock(m_kRscLock, false);
	
	GND_RESOURCE_CONT::const_iterator rsc_itor = m_kGndResourceCon.find(iGroundNo);

	if(rsc_itor != m_kGndResourceCon.end())
	{
		rkOut = (*rsc_itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgGroundRscMgr::Clear()
{
	BM::CAutoMutex kWLock(m_kRscLock, true);

	GND_RESOURCE_CONT::iterator rsc_itor = m_kGndResourceCon.begin();
	while (rsc_itor != m_kGndResourceCon.end())
	{
		rsc_itor->second->Clear();
		m_kGndResourcePool.Delete(rsc_itor->second);

		++rsc_itor;
	}
	m_kGndResourceCon.clear();


#ifdef LOADGSA_USE_SHARE
	PgGroundResource::CONT_NiStream::const_iterator itor_share = PgGroundResource::m_skShareStream.begin();
	while (PgGroundResource::m_skShareStream.end() != itor_share)
	{
		if (itor_share->second != NULL)
		{
			NiDelete(itor_share->second);
		}
		++itor_share;
	}
	PgGroundResource::m_skShareStream.clear();
#endif

	CONT_ALRAM_MISSION_MGR::iterator alram_itr = m_kContAlramMissionMgr.begin();
	for ( ; alram_itr != m_kContAlramMissionMgr.end() ; ++alram_itr )
	{
		SAFE_DELETE( alram_itr->second );
	}
}

PgGroundRscMgr::CONT_ALRAM_MISSION_MGR::mapped_type PgGroundRscMgr::GetAlramMissionMgr( int const iType )const
{
	//���� ���� �ʰ� ���ο����� ���. (���� ������ ����� �߻� ���)
	CONT_ALRAM_MISSION_MGR::const_iterator alram_itr = m_kContAlramMissionMgr.find( iType );
	if ( alram_itr != m_kContAlramMissionMgr.end() )
	{
		return alram_itr->second;
	}
	return NULL;
}

PgGroundManager::PgGroundManager()
:	m_kGndPool(0,5)
,	m_kGndPoolIndun(0,5)
,	m_kGndPoolWar(0,5)
,	m_kGndPoolMission(0,5)
,	m_kGndPoolEmporia(0,0)
,	m_kGndPoolHome(0,0)
,	m_kGndPoolHardCore(0,0)
,	m_GndPoolExpeditionGround(0,0)
,	m_GndPoolConstellationGround(0,0)
,	m_GndPoolCartMission(0,0)
{
	STickInfo sTickInfo;
	sTickInfo.Init();
	m_kTickInfo.resize(ETICK_INTERVAL_MAX, sTickInfo);

}

PgGroundManager::~PgGroundManager()
{
	Clear();
}

void PgGroundManager::Init(bool bIsPublicMap)
{
	if ( bIsPublicMap )
	{
		m_kGndPool.Init(0,0);
		m_kGndPoolIndun.Init(0,5);
		m_kGndPoolWar.Init(10,10);
		m_kGndPoolMission.Init(0,0);
		m_kGndPoolEmporia.Init(0,3);
		m_kGndPoolHome.Init(0,5);
		m_kGndPoolBS.Init(0,5);
		m_kGndPoolHardCore.Init(0,2);
		m_kGndPoolSuperGround.Init(0,0);
		m_GndPoolExpeditionGround.Init(0, 0);
		m_GndPoolConstellationGround.Init(0,0);
		m_GndPoolCartMission.Init(0,0);
	}
	else
	{
		m_kGndPool.Init(0,5);
		m_kGndPoolIndun.Init(0,5);
		m_kGndPoolWar.Init(0,0);
		m_kGndPoolMission.Init(0,10);
		m_kGndPoolEmporia.Init(0,0);
		m_kGndPoolHome.Init(0,0);
		m_kGndPoolBS.Init(0,0);
		m_kGndPoolHardCore.Init(0,0);
		m_kGndPoolSuperGround.Init(0,5);
		m_GndPoolExpeditionGround.Init(0, 5);
		m_GndPoolConstellationGround.Init(0,5);
		m_GndPoolCartMission.Init(0,10);
	}
}

void PgGroundManager::DisplayState()
{
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );

	BM::CAutoMutex kLock(m_kGndLock);

	INFO_LOG(BM::LOG_LV0, _T("======================================="));
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Running MapState GroundCount["<<m_kStaticMapCon.size()<<L"]");

	GND_CONT::iterator gnd_itor = m_kStaticMapCon.begin();
	while(gnd_itor != m_kStaticMapCon.end())
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"MapNo["<<(*gnd_itor).second->GroundKey().GroundNo()<<L"]["<<(*gnd_itor).second->GroundKey().Guid()<<L"] User["<<(*gnd_itor).second->GetUnitCount(UT_PLAYER)<<L"] Mon["<<(*gnd_itor).second->GetUnitCount(UT_MONSTER)<<L"]");

		gnd_itor->second->DisplayAreaState();
		++gnd_itor;
	}
	INFO_LOG(BM::LOG_LV0, _T("======================================="));
}

void PgGroundManager::SendAllGround()const
{
	BM::CAutoMutex kLock(m_kGndLock);

	GroundArr kGndArr;
	GND_CONT::const_iterator gnd_itr = m_kStaticMapCon.begin();
	if( gnd_itr != m_kStaticMapCon.end())
	{
		kGndArr.push_back( GroundArr::value_type( g_kProcessCfg.ServerIdentity(), gnd_itr->first, gnd_itr->second->GetAttr() ) );
		++gnd_itr;
	}
	SendNfyCreateGround(kGndArr);
}

void PgGroundManager::SendNfyCreateGround( GroundArr const &kSendGndArr )const
{
	if ( kSendGndArr.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("kSendGndArr is Empty!"));
		return;
	}

	BM::Stream kPacket( PT_M_T_NFY_CREATE_GROUND );//��� ����.
	kPacket.Push(kSendGndArr);
	SendToCenter( kPacket );

	{
		GroundArr::const_iterator iter = kSendGndArr.begin();
		while( kSendGndArr.end() != iter )
		{
			SEventMessage kMsg(PMET_HACK_CHECK_DUALKEYBOARD, HackDualKeyboard::ACMM_REG_GROUND);
			(*iter).kKey.WriteToPacket( kMsg );
			g_kTask.PutMsg(kMsg);
			++iter;
		}
	}

//	GroundArr::const_iterator gnd_itr;
//	for ( gnd_itr=kSendGndArr.begin(); gnd_itr!=kSendGndArr.end(); ++gnd_itr )
//	{
//		PgLogWrapper kLog( LOG_CONTENTS_GROUND );
//		kLog.MemberKey( gnd_itr->kKey.Guid() );
//		kLog.ChannelNo(g_kProcessCfg.ChannelNo());
//		kLog.Push( (int)1 );	// 1�̸� ����
//		kLog.Push( (int)gnd_itr->kSI.nServerNo );
//		kLog.Push( (int)gnd_itr->kKey.GroundNo() );
//		kLog.Push( (int)gnd_itr->kAttr );
//		kLog.Send();
//	}	
}

/*
void PgGroundManager::SendAllBalancingGnd()
{
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	BM::CAutoMutex kLock(m_kGndLock);

	GroundNoArr	kSendBalancingGndArr;	// �ε� �뷱���� ���� ����(Contents)

	GND_RESOURCE_CONT::const_iterator gndrsc_itr;
	for(gndrsc_itr=m_kGndResourceCon.begin();gndrsc_itr!=m_kGndResourceCon.end();++gndrsc_itr)
	{
		if ( gndrsc_itr->second->GetAttr() & GATTR_INSTANCE )
		{
			kSendBalancingGndArr.push_back(gndrsc_itr->first);
		}
	}

	if ( !kSendBalancingGndArr.empty() )
	{
		BM::Stream kPacket( PT_M_N_NFY_BALANCINGINFO_GROUND );
		kPacket.Push( kSendBalancingGndArr );
		SendToContents(kPacket);
		INFO_LOG(BM::LOG_LV6,_T("[%s] Ground Count[%u]"),__FUNCTIONW__,kSendBalancingGndArr.size());
	}
}
*/
bool PgGroundManager::Reserve( CONT_MAP_BUILD_DATA &rHash )
{//�׶��� ���
// ��� ó���� ���½ÿ��� ȣ�� ����?
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );

	bool const bReturn = (S_OK==m_kGndRscMgr.Reserve(rHash));

	GroundArr kSendGndArr;

	if ( true == bReturn )
	{
		BM::CAutoMutex kWLock(m_kGndLock, true);
		CONT_MAP_BUILD_DATA::const_iterator itr = rHash.begin();
		for( ; itr!=rHash.end(); ++itr )
		{
			CONT_MAP_BUILD_DATA::key_type const &kElement = *itr;
			
			if ( kElement.iGroundNo )
			{
				if( 0 == (GATTR_INSTANCE & kElement.kGndAttr) )
				{
					SGroundAddDesc kGndAddDesc;
					kGndAddDesc.kKey.Set(kElement.iGroundNo);
					kGndAddDesc.kAttr = kElement.kGndAttr;

					AddGround( kGndAddDesc, kSendGndArr, NULL );
				}
			}
		}

		// ������ ����
		SendNfyCreateGround( kSendGndArr );

		// Update Trigger�� �ؾ� Contents�� ���� ������ ���� �� �ִ�.
		GND_CONT::iterator map_itr = m_kStaticMapCon.begin();
		for ( ; map_itr != m_kStaticMapCon.end() ; ++map_itr )
		{
			map_itr->second->UpdateGTrigger();
		}
	}

	return bReturn;
}

HRESULT PgGroundManager::OrderCreate( SGroundMakeOrder const &kOrder, BM::Stream * const pkPacket )
{
	SGroundAddDesc kAddDesc(kOrder);
	{
		CONT_DEFMAP const *pkContDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkContDefMap);
		if ( !pkContDefMap )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_HANDLE"));
			return E_HANDLE;
		}

		CONT_DEFMAP::const_iterator map_itr = pkContDefMap->find(kOrder.kKey.GroundNo());
		if ( map_itr == pkContDefMap->end() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		kAddDesc.kAttr = (T_GNDATTR)(map_itr->second.iAttr);
	}

	{ 
		BM::CAutoMutex kWLock(m_kGndLock, true);
		GroundArr kSendGndArr;
		if ( AddGround( kAddDesc, kSendGndArr, pkPacket ) )
		{
			//->�̰� Reserve �� ���� ��Ŷ�� ������. �ؾ�����.
			SendNfyCreateGround(kSendGndArr);
			return S_OK;	
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgGroundManager::OrderCreateMission( PgMission *pkMission )
{
	if ( !pkMission )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	//BM::CAutoMutex kLock(m_kGndLock); // AutoLock ����

	GroundArr kSendGndArr;
	SGroundAddDesc kGndAddDesc;
	pkMission->GetGroundKey(kGndAddDesc.kKey);
	kGndAddDesc.kAttr = GATTR_MISSION;
	kGndAddDesc.iBalance = pkMission->GetLevel()+1;
//	kGndAddDesc.iWeight = (int)(pkMission->GetUserCount() - 1 );
	kGndAddDesc.iControlID = pkMission->GetAbilRateBagNo();
	kGndAddDesc.iOwnerLv = pkMission->GetOwnerLevel();
	kGndAddDesc.pkConstellationMission = &pkMission->GetConstellationMission();
	kGndAddDesc.rkMutatorData = pkMission->GetMutatorSet();

	{
		BM::CAutoMutex kWLock(m_kGndLock, true);
		// AddGround() �Լ��� SendNfyCreateGround() �Լ��� ���� Lock�� ���� �־�� Ground ���� ������ ��Ȯ�� ����.

		BM::Stream kAddPacket;
		kAddPacket.Push( pkMission->GetStage() );
		PgGround* pkNewGround = AddGround( kGndAddDesc, kSendGndArr, &kAddPacket );
		if (pkNewGround == NULL)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		PgMissionGround *pkMissionGnd = dynamic_cast<PgMissionGround*>( pkNewGround );
		if ( !pkMissionGnd )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"GroundType casting failed PgGround->PgMissionGround GroundKey["<<kGndAddDesc.kKey.GroundNo()<<L":"<<kGndAddDesc.kKey.Guid()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		//////////////////////////////////////////////////
		PgMissionInfo const * pkMissionInfo = NULL;
		bool bRet = g_kMissionMan.GetMission( pkMission->GetMissionNo(), pkMissionInfo );
		if( bRet )
		{
			pkMission->SetTimeMin(pkMissionInfo->GetTimeMin());
			pkMission->SetTimeSec(pkMissionInfo->GetTimeSec());
		}
		//////////////////////////////////////////////////

		pkMissionGnd->SetMission(pkMission);
		SendNfyCreateGround( kSendGndArr );
	}
	return true;
}

bool PgGroundManager::RestartMission(BM::Stream * const pkPacket)
{
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	BM::CAutoMutex kLock(m_kGndLock);

	PgMission kMission;
	kMission.ReadFromPacket(*pkPacket);

	SGroundKey kGndKey;
	kMission.GetGroundKey(kGndKey);

	PgMissionGround *pkMissionGnd = dynamic_cast<PgMissionGround*>(GetGround(kGndKey));
	if ( !pkMissionGnd )
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"Not Found Mission Ground["<<kGndKey.GroundNo()<<L"-"<<kGndKey.Guid()<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMissionGround::ConStageResource kConStageResource;

	
	size_t iTemp = 0;
	int iGndNo = kMission.GetStageGroundNo(iTemp++);
	while ( iGndNo )
	{
		PgGroundResource const *pkGndRsc = NULL;
		if(S_OK == m_kGndRscMgr.Locked_GetGroundResource(iGndNo, pkGndRsc))
		{
			kConStageResource.push_back(pkGndRsc);
		}
		iGndNo = kMission.GetStageGroundNo(iTemp++);
	}
	pkMissionGnd->ReStartMission( &kMission, kConStageResource );
	return true;
}

PgGround* PgGroundManager::Create( SGroundAddDesc const &rkAddDesc, BM::Stream * const pkPacket )
{
	//IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	//BM::CAutoMutex kWLock(m_kGndLock, true);

	GND_CONT::iterator itor = m_kStaticMapCon.find(rkAddDesc.kKey);
	if(itor != m_kStaticMapCon.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	PgGround *pkGnd = CreatePool( rkAddDesc.kAttr );

	if( !pkGnd )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	//pkGnd->Clear(); pool ����� ��Ȱ�� PgGround::Clear()�� �ִ� ������Ƽ.Clear()�� ȣ�� �ǵ��� �Ǿ� �Ѵ�(Init ���� ȣ��Ǹ� �ȵ�/�̼Ǹʿ��� SwapStage �ϸ鼭 ��Ƽ������ �ʱ�ȭ ��)
	pkGnd->GroundKey( rkAddDesc.kKey );
	pkGnd->SetMapLevel( rkAddDesc.iBalance );
	pkGnd->GroundWeight( rkAddDesc.iWeight );
	pkGnd->TunningLevel( rkAddDesc.iOwnerLv );

	if(GATTR_INSTANCE & rkAddDesc.kAttr)
	{
		pkGnd->IndunPartyDie( rkAddDesc.bIndunPartyDie );

		if(rkAddDesc.pkConstellationMission && (BM::GUID::NullData() != rkAddDesc.pkConstellationMission->ConstellationKey.PartyGuid) )
		{
			pkGnd->SetConstellationKey(rkAddDesc.pkConstellationMission->ConstellationKey); 		
			if( false == g_ConstellationPartyMgr.AddParty(*rkAddDesc.pkConstellationMission) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"ConstellationPartyMgr AddParty Failed : "<<pkGnd->GroundKey().GroundNo() );
			}
		}
	}

	switch( pkGnd->GetKind() )
	{
	case GKIND_DEFAULT:
	case GKIND_EXPEDITION_LOBBY:
		{
			GND_CONT::const_iterator map_itr = m_kStaticMapCon.find(pkGnd->GroundKey());
			if ( map_itr != m_kStaticMapCon.end() )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"This Ground Key is Bad : "<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid() );
				DeletePool( pkGnd );
				break;
			}
		}//break ����.
	case GKIND_INDUN:
	case GKIND_WAR:
	case GKIND_HOME:
	case GKIND_EXPEDITION:
	case GKIND_CONSTELLATION:
	case GKIND_CART_MISSION:
		{	
			PgGroundResource const *pkGndRsc = NULL;
			if( S_OK == m_kGndRscMgr.Locked_GetGroundResource(pkGnd->GroundKey().GroundNo(), pkGndRsc) )
			{
				pkGnd->CloneResource( pkGndRsc );
				pkGnd->Init( rkAddDesc.iControlID, true );

				m_kStaticMapCon.insert( std::make_pair( pkGnd->GroundKey(), pkGnd ) );
			}
			else
			{
				INFO_LOG( BM::LOG_LV0, __FL__<<L"Ground["<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid()<<L"] Add Resource Failed" );
				DeletePool( pkGnd );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Locked_GetGroundResource is Failed!"));
			}
		}break;
	case GKIND_MISSION:
		{
			PgMissionGround *pkMission = dynamic_cast<PgMissionGround*>(pkGnd);
			pkGnd->GroundMutator().clear();
			pkGnd->GroundMutator() = rkAddDesc.rkMutatorData; // mutator used only for mission map
			pkGnd->MakeMutatorAbil();

			ConStage kConStage;
			if ( pkPacket && pkPacket->Pop( kConStage ) )
			{
				if ( kConStage.size() )
				{
					PgGroundResource const *pkGndRsc = NULL;
					if( S_OK == m_kGndRscMgr.Locked_GetGroundResource( kConStage.begin()->iGroundNo, pkGndRsc ) )
					{
						pkGnd->CloneResource( pkGndRsc );
						pkGnd->Init( rkAddDesc.iControlID, false );// �̼ǿ����� ó���� ���͸� ����Ű�� ���� �׷��� ȿ������ ����.

						ConStage::const_iterator stage_itr = kConStage.begin();
						for( ; stage_itr!=kConStage.end(); ++stage_itr )
						{
							if(S_OK == m_kGndRscMgr.Locked_GetGroundResource(stage_itr->iGroundNo, pkGndRsc))
							{
								pkMission->AddStage( pkGndRsc );
							}
						}
						m_kStaticMapCon.insert( std::make_pair( pkGnd->GroundKey(), pkGnd ) );
						break;
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"This Ground Key is Bad : "<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid() );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Locked_GetGroundResource is Failed!"));
					}
				}
			}

			DeletePool( pkGnd );
		}break;
	case GKIND_SUPER:
		{
			PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(pkGnd);
			int iSuperGroundNo = 0, iSuperGroundMode = 0, iFloorNo = 0;
			if( pkSuperGround
			&&	pkPacket
			&&	pkPacket->Pop(iSuperGroundNo)
			&&	pkPacket->Pop(iSuperGroundMode)
			&&	pkPacket->Pop(iFloorNo) )
			{
				CONT_DEF_SUPER_GROUND_GROUP const* pkDefSuperGround = NULL;
				g_kTblDataMgr.GetContDef(pkDefSuperGround);

				CONT_DEF_SUPER_GROUND_GROUP::const_iterator find_iter = pkDefSuperGround->find(iSuperGroundNo);
				if( pkDefSuperGround->end() != find_iter )
				{
					CONT_DEF_SUPER_GROUND_GROUP::mapped_type const& rkDefSuperGnd = (*find_iter).second;
					size_t iTargetFloor = static_cast< size_t >((0 < iFloorNo)? iFloorNo-1: 0);
					iTargetFloor = ((rkDefSuperGnd.kContSuperGround.size() > iTargetFloor)? iTargetFloor: 0);

					PgSuperGround::ContFloorResource kContResource;
					CONT_SUPER_GROUND::const_iterator gnd_iter = rkDefSuperGnd.kContSuperGround.begin();
					while( rkDefSuperGnd.kContSuperGround.end() != gnd_iter )
					{
						CONT_SUPER_GROUND::value_type const& rkSuperGnd = (*gnd_iter);
						PgGroundResource const* pkGndRsc = NULL;
						if( S_OK == m_kGndRscMgr.Locked_GetGroundResource(rkSuperGnd.iGroundNo, pkGndRsc) )
						{
							kContResource.push_back( pkGndRsc );
						}
						++gnd_iter;
					}

					if( rkDefSuperGnd.kContSuperGround.size() == kContResource.size() )
					{
						pkSuperGround->CloneResource(kContResource.at(iTargetFloor));
						pkSuperGround->Init(rkAddDesc.iControlID, false);

						pkSuperGround->AddFloor(kContResource);
						pkSuperGround->SetSuperGroundMode(iSuperGroundNo, static_cast< ESuperGroundMode >(iSuperGroundMode), iTargetFloor);
						pkSuperGround->GroundKey( rkAddDesc.kKey ); // ���� �׶��� Ű�� ����

						m_kStaticMapCon.insert( std::make_pair(pkGnd->GroundKey(), pkGnd) );
						break;
					}
				}
			}
			DeletePool(pkGnd);
		}break;
	case GKIND_EMPORIA:
		{
			if ( pkPacket )
			{
				T_GNDATTR kGndAttr = GATTR_DEFAULT;
				pkPacket->Pop( kGndAttr );
				if ( GATTR_EMPORIA == kGndAttr )
				{
					PgGroundResource const *pkGndRsc = NULL;
					if( S_OK == m_kGndRscMgr.Locked_GetGroundResource( pkGnd->GroundKey().GroundNo(), pkGndRsc) )
					{
						pkGnd->CloneResource( pkGndRsc );
						pkGnd->Init( rkAddDesc.iControlID, true );

						m_kStaticMapCon.insert( std::make_pair( pkGnd->GroundKey(), pkGnd ) );

						BM::GUID kID;
						size_t iIndex = 0;
						pkPacket->Pop( kID );
						pkPacket->Pop( iIndex );

						PgEmporiaGround *pkEmporiaGnd = dynamic_cast<PgEmporiaGround*>(pkGnd);
						pkEmporiaGnd->ReadFromPacket_BaseInfo( *pkPacket );
						break;
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Bad Packet GndAttr["<<kGndAttr<<L"] GroundNo["<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid()<<L"]" );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Wrong kGndAttr!"));
				}
			}

			INFO_LOG( BM::LOG_LV0, __FL__<<L"Ground["<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid()<<L"] Add Resource Failed" );
			DeletePool(pkGnd);
		}break;
	case GKIND_HARDCORE:
		{
			PgHardCoreDungeon *pkHDC = dynamic_cast<PgHardCoreDungeon*>(pkGnd);
			if ( pkHDC && pkPacket )
			{
				int iMode = 0;
				__int64 i64EndTime = 0i64;
				int iBossGroundNo = 0;
				pkPacket->Pop( iMode );
				pkPacket->Pop( i64EndTime );
				pkPacket->Pop( iBossGroundNo );

				PgGroundResource const *pkGndRsc = NULL;
				if( S_OK == m_kGndRscMgr.Locked_GetGroundResource( pkGnd->GroundKey().GroundNo(), pkGndRsc ) )
				{
					pkGnd->CloneResource( pkGndRsc );
					pkHDC->Init( iMode, i64EndTime, iBossGroundNo, rkAddDesc.iControlID, true );
					m_kStaticMapCon.insert( std::make_pair( pkGnd->GroundKey(), pkGnd ) );
					break;
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"This Ground Key is Bad : "<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid() );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Locked_GetGroundResource is Failed!"));
				}
			}

			INFO_LOG( BM::LOG_LV0, __FL__<<L"Ground["<<pkGnd->GroundKey().GroundNo()<<L"-"<<pkGnd->GroundKey().Guid()<<L"] Add Resource Failed" );
			DeletePool(pkGnd);
		}break;
	}
	return pkGnd;
}


PgGround* PgGroundManager::AddGround( SGroundAddDesc const &rkAddDesc, GroundArr& rkOutGndArr, BM::Stream * const pkPacket )
{
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	// Create �Լ����� WriteLock �����
	//BM::CAutoMutex kLock(m_kGndLock);	// AutoLock ����

	PgGround *pkCreateGnd = Create( rkAddDesc, pkPacket );
	if ( !pkCreateGnd )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	GroundArr::value_type kGround( g_kProcessCfg.ServerIdentity(), rkAddDesc.kKey, pkCreateGnd->GetAttr() );
	rkOutGndArr.push_back(kGround);
	return pkCreateGnd;
}

bool PgGroundManager::Delete(GND_KEYLIST& rkGndKeyList)
{
	GND_KEYLIST::iterator itor = rkGndKeyList.begin();
	__int64 i64Now = BM::GetTime64();
	SMapPlayTime kPlayTime;
	while (itor != rkGndKeyList.end())
	{
		GND_CONT::iterator map_itor = m_kStaticMapCon.find(*itor);
		if (map_itor != m_kStaticMapCon.end())
		{
			PgGround* pkGnd = map_itor->second;
			m_kStaticMapCon.erase( map_itor );
			pkGnd->GetStatisticsPlayTime(i64Now, kPlayTime);
			AddPlayTime(pkGnd->GroundKey(), kPlayTime);
			{
				SEventMessage kMsg(PMET_HACK_CHECK_DUALKEYBOARD, HackDualKeyboard::ACMM_UNREG_GROUND);
				pkGnd->GroundKey().WriteToPacket( kMsg );
				g_kTask.PutMsg(kMsg);
			}
			g_ConstellationPartyMgr.DelParty(pkGnd->GetConstellationKey().PartyGuid);
			DeletePool( pkGnd );
		}
		else
		{
			itor = rkGndKeyList.erase(itor);
			continue;
		}
		++itor;
	}

	return !rkGndKeyList.empty();
}

PgGround* PgGroundManager::CreatePool( T_GNDATTR const kGndAttr )
{
	PgGround *pkGnd = NULL;

	switch( kGndAttr )
	{
	case GATTR_PVP:
	case GATTR_EMPORIABATTLE:
		{
			PgWarGround *pkWarGnd = m_kGndPoolWar.New();
			if ( pkWarGnd )
			{
				pkGnd = dynamic_cast<PgGround*>(pkWarGnd);
			}
		}break;
	case GATTR_MISSION:
		{
			PgMissionGround *pkMission = m_kGndPoolMission.New();
			if ( pkMission )
			{
				pkGnd = dynamic_cast<PgGround*>(pkMission);
			}
		}break;
	case GATTR_EMPORIA:
		{
			PgEmporiaGround *pkEmporia = m_kGndPoolEmporia.New();
			if ( pkEmporia )
			{
				pkGnd = dynamic_cast<PgGround*>(pkEmporia);
			}
		}break;
	case GATTR_MYHOME:
		{
			PgIndunHome *pkHome = m_kGndPoolHome.New();
			if ( pkHome )
			{
				pkGnd = dynamic_cast<PgGround*>(pkHome);
			}
		}break;
	case GATTR_BATTLESQUARE:
		{
			PgBSGround *pkBSGround = m_kGndPoolBS.New();
			if( pkBSGround )
			{
				pkGnd = dynamic_cast<PgGround*>(pkBSGround);
			}
		}break;
	case GATTR_HARDCORE_DUNGEON:
		{
			PgHardCoreDungeon * pkHDC = m_kGndPoolHardCore.New();
			if ( pkHDC )
			{
				pkGnd = dynamic_cast<PgGround*>(pkHDC);
			}
		}break;
	case GATTR_SUPER_GROUND:
	case GATTR_ELEMENT_GROUND:
		{
			PgSuperGround* pkSuperGround = m_kGndPoolSuperGround.New();
			if( pkSuperGround )
			{
				pkGnd = dynamic_cast< PgGround* >(pkSuperGround);
			}
		}break;
	case GATTR_EXPEDITION_GROUND:
		{
			PgExpeditionGround * pExpeditionGround = m_GndPoolExpeditionGround.New();
			if( pExpeditionGround )
			{
				pkGnd = dynamic_cast< PgGround* >(pExpeditionGround);
			}
		}break;
	case GATTR_EXPEDITION_LOBBY:
		{
			PgExpeditionLobby * pExpeditionLobby = m_GndPoolExpeditionLobby.New();
			if( pExpeditionLobby )
			{
				pkGnd = dynamic_cast< PgGround* >(pExpeditionLobby);
			}
		}break;
	case GATTR_EVENT_GROUND:
		{
			PgStaticEventGround * pStaticEventGround = m_GndPoolStaticEventGround.New();
			if( pStaticEventGround )
			{
				pkGnd = dynamic_cast< PgGround* >(pStaticEventGround);
			}
		}break;
	case GATTR_RACE_GROUND:
		{
			PgStaticRaceGround* pkStaticRaceGround = m_GndPoolStaticRaceGround.New();
			if( pkStaticRaceGround )
			{
				pkGnd = dynamic_cast< PgGround* >(pkStaticRaceGround);
			}
		}break;
	case GATTR_CONSTELLATION_GROUND:
	case GATTR_CONSTELLATION_BOSS:
		{
			PgConstellationGround * pConstellationGround = m_GndPoolConstellationGround.New();
			if( pConstellationGround )
			{
				pkGnd = dynamic_cast< PgGround* >(pConstellationGround);
			}
		}break;
	case GATTR_CART_MISSION_GROUND:
		{
			PgCartMissionGround * PgCartMissionGround = m_GndPoolCartMission.New();
			if( PgCartMissionGround )
			{
				pkGnd = dynamic_cast< PgGround* >(PgCartMissionGround);
			}
		}break;
	default:
		{
			if ( kGndAttr & GATTR_INSTANCE )
			{
				PgIndun *pkIndun = m_kGndPoolIndun.New();
				if ( pkIndun ) 			
				{
					pkGnd = dynamic_cast<PgGround*>(pkIndun);
				}
			}
			else
			{
				pkGnd = m_kGndPool.New();
			}
		}break;
	}

	return pkGnd;
}

void PgGroundManager::DeletePool( PgGround*& pkGnd )
{
	pkGnd->Clear();
	switch( pkGnd->GetKind() )
	{
	case GKIND_DEFAULT:
	case GKIND_EXPEDITION_LOBBY:
		{	
			m_kGndPool.Delete( pkGnd );	
		}break;
 	case GKIND_INDUN:
		{
			PgIndun *pkIndun = dynamic_cast<PgIndun*>(pkGnd);
			m_kGndPoolIndun.Delete( pkIndun );
		}break;
 	case GKIND_WAR:	
		{	
			PgWarGround *pkWarGnd = dynamic_cast<PgWarGround*>(pkGnd);
			m_kGndPoolWar.Delete(	pkWarGnd );	
		}break;
 	case GKIND_MISSION:
		{
			PgMissionGround *pkMission = dynamic_cast<PgMissionGround*>(pkGnd);
			m_kGndPoolMission.Delete( pkMission );
		}break;
	case GKIND_EMPORIA:
		{
			PgEmporiaGround *pkEmporia = dynamic_cast<PgEmporiaGround*>(pkGnd);
			m_kGndPoolEmporia.Delete( pkEmporia );
		}break;
	case GKIND_HOME:
		{
			PgIndunHome *pkHome = dynamic_cast<PgIndunHome*>(pkGnd);
			m_kGndPoolHome.Delete( pkHome );
		}break;
	case GKIND_HARDCORE:
		{
			PgHardCoreDungeon *pkHDC = dynamic_cast<PgHardCoreDungeon*>(pkGnd);
			m_kGndPoolHardCore.Delete( pkHDC );
		}break;
	case GKIND_BS:
		{
			PgBSGround *pkBSGround = dynamic_cast<PgBSGround*>(pkGnd);
			m_kGndPoolBS.Delete( pkBSGround );
		}break;
	case GKIND_SUPER:
		{
			PgSuperGround* pkSuperGround = dynamic_cast< PgSuperGround* >(pkGnd);
			m_kGndPoolSuperGround.Delete( pkSuperGround );
		}break;
	case GKIND_EXPEDITION:
		{
			PgExpeditionGround * pExpeditionGround = dynamic_cast<PgExpeditionGround*>(pkGnd);
			m_GndPoolExpeditionGround.Delete(pExpeditionGround);
		}break;
	case GKIND_EVENT:
		{
			PgStaticEventGround * pStaticEventGround = dynamic_cast<PgStaticEventGround*>(pkGnd);
			m_GndPoolStaticEventGround.Delete(pStaticEventGround);
		}break;
	case GKIND_RACE:
		{
			PgStaticRaceGround * pStaticRaceGround = dynamic_cast<PgStaticRaceGround*>(pkGnd);
			m_GndPoolStaticRaceGround.Delete(pStaticRaceGround);
		}break;
	case GKIND_CONSTELLATION:
		{
			PgConstellationGround * pConstellationGround = dynamic_cast<PgConstellationGround*>(pkGnd);
			m_GndPoolConstellationGround.Delete(pConstellationGround);
		}break;
	case GKIND_CART_MISSION:
		{
			PgCartMissionGround * pCartMissionGround = dynamic_cast<PgCartMissionGround*>(pkGnd);
			m_GndPoolCartMission.Delete(pCartMissionGround);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
		}break;
	}
	pkGnd = NULL;
}
//! ���� ��ȯ�Ѵ�.
PgGround* PgGroundManager::GetGround(SGroundKey const &rkKey)
{
	BM::CAutoMutex kLock(m_kGndLock);
	GND_CONT::iterator itor = m_kStaticMapCon.find(rkKey);
	if(itor != m_kStaticMapCon.end())
	{
		return (*itor).second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

bool PgGroundManager::IsHaveGround(SGroundKey const &rkKey) const
{
	//BM::CAutoMutex kLock(m_kGndLock);
	//���� ���� �ʰ� �˻縸 �Ѵ�. (���� ������ ����� �߻� ���)
 	return (m_kStaticMapCon.end() != m_kStaticMapCon.find(rkKey));
}

void PgGroundManager::ProcessRemoveUser( SERVER_IDENTITY const &kSI )
{
	BM::CAutoMutex kLock(m_kGndLock);
	
	GND_CONT::iterator gnd_itr = m_kStaticMapCon.begin();
	for ( ; gnd_itr!=m_kStaticMapCon.end(); ++gnd_itr )
	{
		gnd_itr->second->LogOut( kSI );
	}
}

void PgGroundManager::ProcessRemoveUser(BM::GUID const &kCharGuid)
{
	BM::CAutoMutex kLock(m_kGndLock);
	
	GND_CONT::iterator gnd_itor = m_kStaticMapCon.begin();
	while(gnd_itor != m_kStaticMapCon.end())
	{
		(*gnd_itor).second->LogOut(kCharGuid);
		++gnd_itor;
	}
}

//volatile LONG g_kCount = 0;
void PgGroundManager::OnTimer(ETickInterval eInterval)
{
	switch( eInterval )
	{
	case ETICK_INTERVAL_5S:
		{
			OnTimer5s();
		}break;
	case ETICK_INTERVAL_30S:
		{
			OnTimer30s();
		}break;
	default:
		{
		}break;
	}
	{
		BM::CAutoMutex kLock(m_kGndLock);
		{
			BM::CAutoMutex kTickLock(m_akTickInfoMutex[eInterval]);
			TICK_INFO_VEC::value_type& rkTickInfo = m_kTickInfo.at(eInterval);
			if (rkTickInfo.sLoopCount == TickInfo_LoopCountMax)
			{
				rkTickInfo.sLoopCount = 0;
			}
		}
		PgGround* pkGround = NULL;
		while ((pkGround = PickupTickGround(eInterval)) != NULL)
		{
			switch(eInterval)
			{
			case ETICK_INTERVAL_100MS :
				{
					//::InterlockedIncrement(&g_kCount);
					pkGround->OnTick100ms();
					//::InterlockedDecrement(&g_kCount);
				}break;
			case ETICK_INTERVAL_30S :
				{
					pkGround->OnTick30s();
				}break;
			case ETICK_INTERVAL_1S :
				{
					pkGround->OnTick1s();
				}break;
			case ETICK_INTERVAL_5S :
				{
					pkGround->OnTick5s();
				}break;
			case ETICK_INTERVAL_1M:
				{
					pkGround->OnTick1m();
				}break;
			}
		}
	}
}

void PgGroundManager::OnTimer5s()
{//5�ʿ� �ѹ�

	// BM::CAutoMutex kLock(m_kGndLock) // AutoLock ����
	GND_KEYLIST kList;
	{
		//IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
		BM::CAutoMutex kLock(m_kGndLock);

		PgGround *pGnd = NULL;
		GND_CONT::iterator gnd_itr = m_kStaticMapCon.begin();
		while( gnd_itr!=m_kStaticMapCon.end() )
		{
 			pGnd = gnd_itr->second;
			if ( pGnd->IsDeleteTime() )
			{
				kList.push_back(gnd_itr->first);
			}
			++gnd_itr;
		}
	}

	{
		BM::CAutoMutex kLock(m_kGndLock, true);
		if ( !kList.empty() )
		{
			if ( true == Delete(kList) )
			{
				BM::Stream kNfyPacket(PT_M_T_NFY_DELETE_GROUND, kList);
				SendToCenter( kNfyPacket );
			}
		}

		// WriteLock �� �ɰ� ReleaseFlush ȣ���Ͽ���,
		//	Release�� Unit�� ���� ������ ��� ������ ���¿��� ReleaseFlush ȣ��ȴ�.
		g_kTotalObjMgr.ReleaseFlush();
	}
}

void PgGroundManager::RecvGndWrapped( BM::Stream* const pkPacket )
{
	IG_LOG LockCheck( g_kLogWorker, LT_INFO, fm );
	BM::CAutoMutex kLock(m_kGndLock);

	SGroundKey kKey;
	pkPacket->Pop(kKey);

	GND_CONT::iterator gItr = m_kStaticMapCon.find(kKey);
	if( m_kStaticMapCon.end() == gItr)
	{
		// ���нÿ� ������ �ش�.
		PACKET_ID_TYPE usType = 0;
		pkPacket->Pop( usType );

		INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't find Ground["<<kKey.GroundNo()<<L"]["<<kKey.Guid()<<L"] from m_kStaticMapCon, Type[" << usType << L"]");
		
		switch ( usType )
		{
		case PT_T_M_REQ_READY_GROUND:
			{
				// ���⿡ ���� ��Ȳ
				// 1. Center(public)���� �׶��尡 �����ϴ��� ã�ƺôµ� �־���.
				// 2. ��Ŷ�� ������, ���ÿ� �ʼ��������� �׶��尡 ��������.
				// -> ������ �������� �̷� ��Ȳ�� ���� �� �ִ�.
				// 

				EPVPTYPE kType = PVP_TYPE_NONE;
				BYTE kOption  = E_PVP_OPT_NONE;
				int iLobbyID = 0;
				int iRoomIndex = 0;
				pkPacket->Pop(kType);
				pkPacket->Pop(kOption);
				pkPacket->Pop(iLobbyID);
				pkPacket->Pop(iRoomIndex);

				// PvPRoom�� �뺸�� ���־ �ٽ� ����
				BM::Stream kFailedPacket( PT_M_T_ANS_READY_GROUND, kKey );
				SendToPvPRoom( iLobbyID, iRoomIndex, kFailedPacket );
			}break;
		case PT_N_M_REQ_READY_EMPORIA_BATTLE:
			{
				EPVPTYPE kType = PVP_TYPE_NONE;
				pkPacket->Pop(kType);
			}break;
		case PT_T_M_REQ_KICK_ROOM_USER:
			{
				BM::GUID kCharGuid;
				pkPacket->Pop( kCharGuid );

				BM::Stream kFailedPacket( PT_M_T_ANS_KICK_ROOM_USER_NOTFOUND, kCharGuid );
				kFailedPacket.Push( kKey );
			}break;
		case PT_M_M_REQ_READY_HARDCORE_BOSS:
			{
				SGroundKey kGndKey;
				kGndKey.ReadFromPacket( *pkPacket );

				BM::Stream kPacket( PT_M_M_ANS_READY_HARDCORE_BOSS );
				kKey.WriteToPacket( kPacket );
				kPacket.Push( false );
				::SendToGround( kGndKey, kPacket );
			}break;
		}
	}
	else
	{
		GND_CONT::mapped_type pkGround = (*gItr).second;

		if(pkGround)
		{
			pkGround->RecvGndWrapped(pkPacket);
		}
	}
}

bool PgGroundManager::ProcessMsg(SEventMessage *pkMsg)
{
	switch(pkMsg->SecType())
	{
	case PT_S_M_WRAPPED_USER_PACKET:
		{
			// No Lock
			SGroundKey kGroundKey;
			BM::GUID kCharacterGuid;
			kGroundKey.ReadFromPacket(*pkMsg);
			pkMsg->Pop( kCharacterGuid );
			GroundEventNfy( kGroundKey, kCharacterGuid, pkMsg );
		}break;
	case PT_M_T_USEITEM:
		{
			BM::CAutoMutex kLock(m_kGndLock);
			SGroundKey kGndKey;
			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( NULL == pkPacket )
			{
				return false;
			}

			BM::GUID kCasterGuid;
			pkPacket->Pop(kCasterGuid);
			pkPacket->Pop(kGndKey);

			BM::Stream kOPacket(PT_U_G_NFY_PT_M_T_USEITEM);
			kOPacket.Push(*pkPacket);
			GroundEventNfy(kGndKey, kCasterGuid, &kOPacket);
		}break;
	case PT_M_C_NFY_GAMBLEMACHINE_MIXUP_RESULT:
	case PT_M_C_NOTI_ACHIEVEMENT_COMPLETE_FIRST:
	case PT_M_C_NFY_GENERIC_ACHIEVEMENT_NOTICE:
	case PT_M_C_NFY_GAMBLEMACHINE_RESULT:
	case PT_M_C_NFY_ITEM_MAKING_SUCCESS:
	case PT_M_C_NOTI_OPEN_GAMBLE:
	case PT_M_C_NFY_USEITEM:
	case PT_M_C_NOTI_OXQUIZ_OPEN:
	case PT_M_C_NOTI_OXQUIZ_LOCK:
	case PT_M_C_LUCKYSTAR_RESULT:
	case PT_M_C_LUCKYSTAR_CLOSE:
	case PT_M_C_NFY_EVENT_QUEST_NOTICE:
	case PT_N_C_NFY_BS_NOTICE:
	case PT_SYNC_DEF_PLAYERPLAYTIME:
	case PT_N_C_NFY_REALM_QUEST_REWARD:
	case PT_M_C_NOTI_EVENT_ITEM_REWARD:
	case PT_M_C_NOTI_TREASURE_CHEST:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				BM::Stream kPacket( static_cast<BM::Stream::DEF_STREAM_TYPE>(pkMsg->SecType()) );
				kPacket.Push(*pkPacket);
				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if ( pkGround )
					{
						pkGround->Broadcast( kPacket, BM::GUID::NullData() );
					}
				}
			}
		}break;
	case PT_N_M_NFY_ALL_USER_ADDEFFECT:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				int iBuffNo = 0;
				pkPacket->Pop( iBuffNo );
				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if( pkGround )
					{
						pkGround->AddEffectToAllPlayer( iBuffNo );
					}
				}
			}
		}break;
	case PT_N_C_NFY_REALM_QUEST_INFO:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				SRealmQuestInfo kRealmQuestInfo;
				bool bAllowSendToUser = false;

				kRealmQuestInfo.ReadFromPacket(*pkPacket);
				pkPacket->Pop( bAllowSendToUser );

				BM::Stream kBroadPacket(PT_N_C_NFY_REALM_QUEST_INFO);
				kRealmQuestInfo.WriteToPacket(kBroadPacket);

				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if( pkGround )
					{
						pkGround->SetRealmQuestStatus(kRealmQuestInfo);
						if( bAllowSendToUser )
						{
							pkGround->Broadcast(kBroadPacket);
						}
					}
				}
			}
		}break;
	case PT_N_M_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast< BM::Stream* >(pkMsg);
			if( pkPacket )
			{
				SWorldEnvironmentStatus kNewStatus;
				kNewStatus.ReadFromPacket( *pkPacket );
				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if( pkGround )
					{
						pkGround->UpdateWorldEnvironmentStatus( kNewStatus );
					}
				}
			}
		}break;
	case PT_M_M_UPDATE_PLAYERPLAYTIME:
		{
			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				SGroundKey kGndKey;
				BM::GUID kCharGuid;
				int iAccConSec = 0;
				int iAccDisSec = 0;
				__int64 i64SelectSec = 0i64;

				pkMsg->Pop(kGndKey);
				pkPacket->Pop(kCharGuid);
				pkPacket->Pop(iAccConSec);
				pkPacket->Pop(iAccDisSec);
				pkPacket->Pop(i64SelectSec);

				BM::CAutoMutex kLock(m_kGndLock);
				PgGround* pkGround = GetGround(kGndKey);
				if( pkGround )
				{
					pkGround->SyncPlayerPlayTime(kCharGuid, iAccConSec, iAccDisSec, i64SelectSec);
				}
			}
		}break;
	case PT_I_M_NOTI_OXQUIZ_NOTI:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				BM::Stream kPacket(PT_M_C_NFY_CHAT);
				kPacket.Push(static_cast<BYTE>(CT_OXQUIZEVENT));
				kPacket.Push(*pkPacket);

				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if ( pkGround )
					{
						pkGround->Broadcast( kPacket, BM::GUID::NullData() );
					}
				}
			}
		}break;
	case PT_M_C_NFY_EVENT_QUEST_INFO:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if( pkPacket )
			{
				bool bClearInfo = false;
				PgEventQuest kTempEvent;

				pkPacket->Pop( bClearInfo );
				if( !bClearInfo )
				{
					kTempEvent.ReadFromMapPacket( *pkPacket ); // �о
				}

				//
				BM::Stream kNfyPacket( PT_M_C_NFY_EVENT_QUEST_INFO );
				kNfyPacket.Push( bClearInfo );
				if( !bClearInfo )
				{
					bool const bIncludeCompleteInfo = false;
					kNfyPacket.Push( bIncludeCompleteInfo );
					kTempEvent.WriteToClientPacket( kNfyPacket ); // Ŭ���̾�Ʈ ������ ����
				}

				//
				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if( pkGround )
					{
						pkGround->SetEventQuest( kTempEvent );
						pkGround->Broadcast( kNfyPacket, BM::GUID::NullData() );
					}
				}
			}
		}break;
	case PT_N_T_NFY_TRADE:
		{
			BM::Stream::STREAM_DATA kData;
			BM::Stream kPacket;

			pkMsg->Pop(kData);

			kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));

			BM::CAutoMutex kLock(m_kGndLock);

			GND_CONT::iterator it = m_kStaticMapCon.begin();
			for( ; it != m_kStaticMapCon.end() ; ++it)
			{
				PgGround *pkGround = it->second;
				if( pkGround )
				{
					pkGround->Broadcast( BM::Stream( PT_M_C_NFY_CHAT, kPacket ), BM::GUID::NullData() );
				}
			}
		}break;
	case PT_M_C_ANS_SUMMONPARTYMEMBER:
		{
			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if(!pkPacket)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			VEC_GUID kMembGuids;
			SReqMapMove_CM kRMMC;

			kRMMC.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kMembGuids);

			{
				BM::CAutoMutex kLock(m_kGndLock);

				PgGround *pkGround = GetGround(kRMMC.kGndKey);
				if( pkGround )
				{
					pkGround->ReqMapMoveCome(kRMMC,kMembGuids);
				}
				else
				{
					INFO_LOG(BM::LOG_LV5,__FL__ << _T(" Can't find map MapNo:") << kRMMC.kGndKey.GroundNo() << _T(" Guid:") << kRMMC.kGndKey.Guid());
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
				}
			}
		}break;
	//case PT_A_G_SEND_TOGROUND_PARTY:
	//	{
	//		SGroundKey kKey;
	//		BM::GUID kPartyGuid;
	//		VEC_GUID kVec;
	//		BM::Stream::STREAM_DATA kData;
	//		DWORD dwSendType = E_SENDTYPE_SELF;

	//		pkMsg->Pop(kKey);
	//		pkMsg->Pop(kPartyGuid);
	//		pkMsg->Pop(kVec);
	//		pkMsg->Pop(kData);
	//		pkMsg->Pop(dwSendType);

	//		BM::Stream kPacket;
	//		kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));

	//		{
	//			BM::CAutoMutex kLock(m_kGndLock);

	//			PgGround *pkGround = GetGround(kKey);
	//			if( pkGround )
	//			{
	//				pkGround->SendToUser_ByGuidVec(kVec, kPacket, dwSendType);
	//			}
	//			else
	//			{
	//				INFO_LOG(BM::LOG_LV0, _T("[%s]-[%d] Can't find map [%d][%s] on party [%s]"),
	//					__FUNCTIONW__, __LINE__, kKey.GroundNo(), kKey.Guid().str().c_str(), kPartyGuid.str().c_str());
	//			}
	//		}
	//	}break;
	case PT_A_G_SEND_TOGROUND_USER:
		{
			SGroundKey kGndKey;
			VEC_GUID kVec;
			BM::Stream::STREAM_DATA kData;
			DWORD dwSendType = E_SENDTYPE_SELF;

			pkMsg->Pop(kGndKey);
			pkMsg->Pop(kVec);
			pkMsg->Pop(kData);
			pkMsg->Pop(dwSendType);

			BM::Stream kPacket;
			kPacket.Push(&kData.at(0), kData.size()*sizeof(BM::Stream::STREAM_DATA::value_type));

			{
				BM::CAutoMutex kLock(m_kGndLock);

				PgGround* pkGround = GetGround(kGndKey);
				if( pkGround )
				{
					pkGround->SendToUser_ByGuidVec(kVec, kPacket, dwSendType);
				}
				else
				{
					INFO_LOG(BM::LOG_LV0, __FL__<<L"Can't find map ["<<kGndKey.GroundNo()<<L"]["<<kGndKey.Guid()<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
				}
			}
		}break;
	//case PT_A_G_SET_USERS_PARTYGUID:
	//	{
	//		SGroundKey kGndKey;
	//		std::vector<BM::GUID> kVec;
	//		BM::GUID kPartyGuid;

	//		pkMsg->Pop(kGndKey);
	//		pkMsg->Pop(kVec);
	//		pkMsg->Pop(kPartyGuid);

	//		//
	//		{
	//			BM::CAutoMutex kLock(m_kGndLock);

	//			PgGround *pkGround = GetGround(kGndKey);
	//			if(NULL != pkGround)
	//			{
	//				bRet = pkGround->SetUser_PartyGuid(kVec, kPartyGuid);
	//			}
	//		}
	//	}break;
	case PT_T_C_NFY_MARRY:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream * const pkMsgPacket = dynamic_cast<BM::Stream*>(pkMsg);
			BYTE cType = 0;
			SGroundKey kGndKey;
			kGndKey.ReadFromPacket(*pkMsgPacket);
			pkMsgPacket->Pop( cType );
			
			switch( cType )
			{
			case EM_NOTICE:
				{
					BYTE cCmdType = 0;
					SGroundKey kGndKey;

					pkMsgPacket->Pop( cCmdType );
					pkMsgPacket->Pop( kGndKey );

					BM::Stream kClientPacket;
					kClientPacket.Push( *pkMsgPacket );

					GND_CONT::iterator itr = m_kStaticMapCon.begin();
					for (; itr != m_kStaticMapCon.end(); ++itr)
					{
						PgGround *pkGround = itr->second;
						if ( pkGround )
						{
							bool bAll = true;
							if( static_cast<BYTE>(MC_End) == cCmdType )
							{
								if( kGndKey == pkGround->GroundKey() )
								{
									bAll = false;
								}
							}
								
							if( true == bAll )
							{
								pkGround->Broadcast( kClientPacket, BM::GUID::NullData() );
							}
						}
					}
				}break;
			default:
				{
					PgGround* pkGround = GetGround(kGndKey);
					if( pkGround )
					{
						pkGround->OnPT_T_C_NFY_MARRY(static_cast<EMarryState>(cType), pkMsgPacket);
					}
				}break;
			}
		}break;
	case PT_T_C_NFY_BULLHORN:
	case PT_T_C_NFY_NOTICE:
	case PT_N_C_NFY_NOTICE_PACKET:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				GND_CONT::iterator itr = m_kStaticMapCon.begin();
				for (; itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if ( pkGround )
					{
						pkGround->Broadcast( BM::Stream( (BM::Stream::DEF_STREAM_TYPE)pkMsg->SecType(), *pkPacket ), BM::GUID::NullData() );
					}
				}
			}
		}break;
	case PT_M_C_SYNC_GAMETIME:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				pkPacket->PosAdjust();
				GND_CONT::iterator itr;
				for (itr = m_kStaticMapCon.begin(); itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if ( pkGround )
					{
						pkGround->OrderSyncGameTime(pkPacket);
					}
				}
			}
		}break;
	case PT_T_M_NFY_MONSTERGEN:
		{
			BM::CAutoMutex kLock(m_kGndLock);

			BM::Stream *pkPacket = dynamic_cast<BM::Stream*>(pkMsg);
			if ( pkPacket )
			{
				int iGenGroupNo = 0;
				E_MONSTER_GEN_MODE kMode = MGM_NONE;

				pkPacket->Pop(iGenGroupNo);
				pkPacket->Pop(kMode);

				GND_CONT::iterator itr;
				for (itr = m_kStaticMapCon.begin(); itr != m_kStaticMapCon.end(); ++itr)
				{
					PgGround *pkGround = itr->second;
					if ( pkGround )
					{
						pkGround->OnRecvMonsterGenNfy(iGenGroupNo,kMode);
					}
				}
			}
		}break;
#ifndef _MDo_
	case PT_A_G_NFY_RELOAD_NPCQUEST:
		{
			INFO_LOG(BM::LOG_LV0, __FL__<<L"5. Rebuild Quest NPC Information");
			BM::CAutoMutex kLock(m_kGndLock);
			GND_CONT::iterator gnd_iter = m_kStaticMapCon.begin();
			while(m_kStaticMapCon.end() != gnd_iter)
			{
				GND_CONT::mapped_type pkGround = (*gnd_iter).second;
				if( pkGround )
				{
					pkGround->RebuildQuest();
				}
				++gnd_iter;
			}
			INFO_LOG(BM::LOG_LV0, __FL__<<L"6. End Reload Quest");
		}break;
#endif

	case PT_M_G_NFY_EVENT_ABIL:
		{
			SGroundKey kTargetGndKey;
			bool bCheckOnlyGroundNo = false;
			TBL_EVENT kTblEvent;
			kTargetGndKey.ReadFromPacket( *pkMsg );
			pkMsg->Pop( bCheckOnlyGroundNo );
			kTblEvent.ReadFromPacket( *pkMsg );

			BM::CAutoMutex kLock(m_kGndLock);

			GND_CONT::iterator gnd_itr = m_kStaticMapCon.begin();
			for ( ; gnd_itr != m_kStaticMapCon.end() ; ++gnd_itr )
			{
				if (	kTargetGndKey.GroundNo() == gnd_itr->second->GroundKey().GroundNo() 
					&&	(bCheckOnlyGroundNo || kTargetGndKey.Guid() == gnd_itr->second->GroundKey().Guid()) )
				{
					gnd_itr->second->SetEvent( kTblEvent );
				}
			}
		}break;
	case PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY:
		{
			int RecoveryValue = 0;
			pkMsg->Pop(RecoveryValue);

			GND_CONT::iterator gnd_iter = m_kStaticMapCon.begin();
			for( ; gnd_iter != m_kStaticMapCon.end() ; ++gnd_iter )
			{
				gnd_iter->second->Recv_PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY(RecoveryValue);
			}
		}break;
	case PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE:
		{
			int EventNo = 0;
			bool EventMapState = false;
			pkMsg->Pop(EventNo);
			pkMsg->Pop(EventMapState);

			GND_CONT::iterator gnd_iter = m_kStaticMapCon.begin();
			for( ; gnd_iter != m_kStaticMapCon.end() ; ++gnd_iter )
			{
				gnd_iter->second->Recv_PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE(EventNo, EventMapState);
			}
		}break;
	case PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY:
		{
			int EventNo = 0;
			int UserCount = 0;
			pkMsg->Pop(EventNo);
			pkMsg->Pop(UserCount);

			GND_CONT::iterator gnd_iter = m_kStaticMapCon.begin();
			for( ; gnd_iter != m_kStaticMapCon.end() ; ++gnd_iter )
			{
				gnd_iter->second->Recv_PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY(EventNo, UserCount);
			}
		}break;
	case PT_M_T_REQ_INDUN_PARTY_LIST:
		{
			BM::GUID kCharGuid;
			BM::GUID kKeyGuid;
			int Type = 0;
			int iMapNo = 0;

			pkMsg->Pop(kCharGuid);
			pkMsg->Pop(kKeyGuid);
			pkMsg->Pop(Type);
			pkMsg->Pop(iMapNo);

			BM::CAutoMutex kLock(m_kGndLock);
			bool bSend = false;
			CONT_INDUN_PARTY kCont;
			for(GND_CONT::iterator gnd_iter=m_kStaticMapCon.begin(); gnd_iter!=m_kStaticMapCon.end() ; ++gnd_iter)
			{
				if(iMapNo == (*gnd_iter).first.GroundNo())
				{
					bSend = true;
					(*gnd_iter).second->GetIndunPartyGuid(kCont);
				}
			}

			if(bSend)
			{
				BM::Stream kPacket(PT_M_T_ANS_INDUN_PARTY_LIST);
				kPacket.Push(kCharGuid);
				kPacket.Push(kKeyGuid);
				kPacket.Push(Type);
				kPacket.Push(g_kProcessCfg.ChannelNo());
				kPacket.Push(iMapNo);
				PU::TWriteArray_M(kPacket, kCont);
				SendToContents(kPacket);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"invalid packet type ["<<pkMsg->SecType()<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}

void PgGroundManager::Release(const T_GNDATTR kReleaseGndAttr)
{
	// BM::CAutoMutex kLock(m_kGndLock) // AutoLock ����
	GND_KEYLIST kList;
	{
		BM::CAutoMutex kLock(m_kGndLock);

		GND_CONT::iterator gnd_itr = m_kStaticMapCon.begin();
		while( gnd_itr!=m_kStaticMapCon.end() )
		{
			PgGround *pkGnd = gnd_itr->second;

			switch( kReleaseGndAttr )
			{
			case GATTR_DEFAULT:
			case GATTR_ALL:
				{
					kList.push_back(gnd_itr->first);
				}break;
			default:
				{
					if ( pkGnd->GetAttr() & kReleaseGndAttr )
					{
						kList.push_back(gnd_itr->first);
					}
				}break;
			}
			++gnd_itr;
		}

	}

	if ( !kList.empty() )
	{
		BM::CAutoMutex kLock(m_kGndLock, true);
		if ( true == Delete(kList) )
		{
			BM::Stream kNfyPacket(PT_M_T_NFY_DELETE_GROUND, kList);
			SendToCenter( kNfyPacket );
		}
	}

}

//void PgGroundManager::SendToGroundUser(SGroundKey const& rkTrgGndKey, const BM::GUID rkGuid, BM::Stream const& rkPacket, DWORD const dwSendType) const
//{
//	VEC_GUID kVec;
//	kVec.push_back(rkGuid);
//	SendToGroundUser(rkTrgGndKey, kVec, rkPacket, dwSendType);
//}
//
//void PgGroundManager::SendToGroundUser(SGroundKey const& rkTrgGndKey, VEC_GUID const& rkGuidVec, BM::Stream const& rkPacket, DWORD const dwSendType) const
//{
//	SEventMessage kEventMsg;
//	kEventMsg.PriType(PMET_GROUND_MGR);
//	kEventMsg.SecType(PT_A_G_SEND_TOGROUND_USER);
//	kEventMsg.Push(rkTrgGndKey);
//	kEventMsg.Push(rkGuidVec);
//	kEventMsg.Push(rkPacket.Data());
//	kEventMsg.Push(dwSendType);
//	g_kTask.PutMsg(kEventMsg);
//}

void PgGroundManager::BroadcastAll(BM::Stream const& pkPacket)
{
	GND_CONT::iterator itr = m_kStaticMapCon.begin();
	for (; itr != m_kStaticMapCon.end(); ++itr)
	{
		PgGround *pkGround = itr->second;
		if ( pkGround )
			pkGround->Broadcast( pkPacket, BM::GUID::NullData() );
	}
}

bool PgGroundManager::GroundEventNfy(SGroundKey const& rkGroundKey, BM::GUID const &rkCharacterGuid, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kGndLock);

	GND_CONT::iterator sta_map_itor = m_kStaticMapCon.find(rkGroundKey);
	if (sta_map_itor != m_kStaticMapCon.end())
	{
		if((*sta_map_itor).second->UnitEventNfy(rkCharacterGuid, pkPacket))
		{
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GND_CONT::iterator sta_map_itor2 = m_kStaticMapCon.begin();
	while(sta_map_itor2 != m_kStaticMapCon.end())
	{
		if((*sta_map_itor2).second->UnitEventNfy(rkCharacterGuid, pkPacket))
		{
			return true;
		}
		++sta_map_itor2;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;	
}

PgGround* PgGroundManager::PickupTickGround(ETickInterval eInterval)
{
	//BM::CAutoMutex kLock(m_kGndLock);
	if (m_kStaticMapCon.empty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

{
	BM::CAutoMutex kTickLock(m_akTickInfoMutex[eInterval]);
	TICK_INFO_VEC::value_type& rkTickInfo = m_kTickInfo.at(eInterval);
	if(rkTickInfo.sLoopCount == TickInfo_LoopCountMax)
	{// �̹� ��Turn �������Ƿ�, ���� �������� �ٸ� Timer�� ����ǵ��� �� �ش�.
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	DWORD const dwNow = BM::GetTime32();
	DWORD dwElapsedTime = 0;
	// Tick �� Ground ã�ƺ���
	GND_CONT::iterator itor = m_kStaticMapCon.find(rkTickInfo.kNextGroundKey);
	if(itor == m_kStaticMapCon.end())
	{
		itor = m_kStaticMapCon.begin();
	}

	size_t const iCountSize = m_kStaticMapCon.size();
	// ���ѷ��� ���� �ʵ��� �����ؾ� �Ѵ�.
	// ��, Lag�� �ɰ��� ���  CheckTickAvailable ���� ������ return TRUE �ϰ� �ǹǷ�
	// while �� ���ǿ��� �ѹ��� �̻� ���� �ʵ��� �����ؾ� �Ѵ�.
	PgGround* pkGround = NULL;
	while(pkGround == NULL 
	&& iCountSize > (size_t)rkTickInfo.sLoopCount++)
	{
		if (itor->second->CheckTickAvailable(eInterval, dwNow, dwElapsedTime))
		{// FOUND
			pkGround = itor->second;
		}
		++itor;
		if (itor == m_kStaticMapCon.end())
		{
			itor = m_kStaticMapCon.begin();
		}
	}
	rkTickInfo.kNextGroundKey = itor->second->GroundKey();
	if (pkGround != NULL)
	{
		return pkGround;
	}
	rkTickInfo.sLoopCount = TickInfo_LoopCountMax;
}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgGroundManager::_TestReqCreateMissionGround(int const iCount) const
{
	BM::CAutoMutex kLock(m_kGndLock);
	SMissionKey kMissionKey;
	kMissionKey.iKey = 2;	//
	kMissionKey.iLevel = 2;

	//INFO_LOG(BM::LOG_LV0, _T("[%s] Count[%d]"), __FUNCTIONW__, iCount);
	GND_CONT::const_iterator begin_itor = m_kStaticMapCon.begin();
	if (begin_itor != m_kStaticMapCon.end())
	{
		SGroundKey const& rkGndKey = begin_itor->second->GroundKey();
		for (int i=0; i<iCount; i++)
		{
			BM::Stream kPacket(PT_M_N_REQ_ENTER_MISSION);
			BM::GUID kPlayerGuid = BM::GUID::NullData();
			kPacket.Push(kMissionKey);
			kPacket.Push(rkGndKey);
			kPacket.Push(kPlayerGuid);
			kPacket.Push((int)99);	// LEVEL
			SendToMissionMgr(kPacket);
			//INFO_LOG(BM::LOG_LV0, _T("[%s] Sent Request MissionKey[%d]"), __FUNCTIONW__, kMissionKey.iKey);
			Sleep(100);
		}
	}
	//INFO_LOG(BM::LOG_LV0, _T("[%s] Finished"), __FUNCTIONW__);
}

void PgGroundManager::Clear()
{
	BM::CAutoMutex kLock(m_kGndLock, true);

	m_kGndRscMgr.Clear();
	m_kPlayTimeInfo.clear();
}

int PgGroundManager::Locked_GetConnectionMapUserCount() const
{
	BM::CAutoMutex kLock(m_kGndLock);

	int iUserCount = 0;

	GND_CONT::const_iterator Itr = m_kStaticMapCon.begin();
	while(Itr != m_kStaticMapCon.end())
	{
		GND_CONT::mapped_type kElement = Itr->second;
		iUserCount += kElement->GetUnitCount(UT_PLAYER);
		++Itr;
	}
	return iUserCount;

}

void PgGroundManager::AddPlayTime(SGroundKey const& rkGndKey, SMapPlayTime const& rkPlayTime)
{
	GND_PLAYTIME::iterator itor_time = m_kPlayTimeInfo.find(rkGndKey.GroundNo());
	if (itor_time != m_kPlayTimeInfo.end())
	{
		(*itor_time).second += rkPlayTime;
	}
	else
	{
		m_kPlayTimeInfo.insert(std::make_pair(rkGndKey.GroundNo(), rkPlayTime));
	}
}

void PgGroundManager::OnTimer30s()
{
	BM::CAutoMutex kLock(m_kGndLock, true);

	static __int64 i64LastCheckPlayTime = 0;
	static __int64 i64PlayTimeLogInterval = 0;
	if (i64PlayTimeLogInterval == 0)
	{
		int iIntervalMin = 0;
		if (S_OK == g_kVariableContainer.Get(EVar_Kind_Log, EVar_EachMap_PlayTimeLog_Interval_Min, iIntervalMin))
		{
			i64PlayTimeLogInterval = iIntervalMin * 60 * 1000;
		}
	}

	__int64 const i64Now = BM::GetTime64();
	if (i64LastCheckPlayTime == 0)
	{
		i64LastCheckPlayTime = i64Now;
	}

	__int64 const i64DiffTime = i64Now - i64LastCheckPlayTime;
	//INFO_LOG(BM::LOG_LV9, __FL__ << _T("TimeDiff=") << i64DiffTime << _T(", Now=") << i64Now << _T(", LastTime=") << i64LastCheckPlayTime);
	if (i64PlayTimeLogInterval > 0 && i64DiffTime >= i64PlayTimeLogInterval)
	{
		i64LastCheckPlayTime = i64Now;

		GND_CONT::const_iterator itor = m_kStaticMapCon.begin();
		while(itor != m_kStaticMapCon.end())
		{
			SMapPlayTime kPlayTime;
			(*itor).second->GetStatisticsPlayTime(i64Now, kPlayTime);
			AddPlayTime((*itor).second->GroundKey(), kPlayTime);
			++itor;
		}

		WritePlayTimeLog(i64PlayTimeLogInterval);
		m_kPlayTimeInfo.clear();
	}

}

void PgGroundManager::WritePlayTimeLog(int const iElapsedTimeSec) const
{
	PgLogCont kLogCont(ELogMain_Statistics, ELogSub_Map);

	GND_PLAYTIME::const_iterator itor_time = m_kPlayTimeInfo.begin();
	while (itor_time != m_kPlayTimeInfo.end())
	{
		SMapPlayTime const& rkPlayTime = (*itor_time).second;

		if (false == rkPlayTime.IsEmpty())
		{
			PgLog kLog(ELOrderMain_Time, ELOrderSub_Calculate);
			kLog.Set(0, iElapsedTimeSec/1000);	// ElaspedTime (sec)
			kLog.Set(1, rkPlayTime.iInCount);	// Ground-In player count
			kLog.Set(2, rkPlayTime.iOutCount);	// Ground-out player count
			kLog.Set(3, (*itor_time).first);	// GroundNo
			kLog.Set(0, rkPlayTime.iPlayTime/1000);	// PlayTime
			kLogCont.Add(kLog);
		}

		++itor_time;
	}
	if (kLogCont.LogCount() > 0)
	{
		kLogCont.Commit();
	}
}

// PgConstellationMgr

PgConstellationMgr::PgConstellationMgr()
{
	Clear();
}

PgConstellationMgr::~PgConstellationMgr()
{
}

void PgConstellationMgr::Clear()
{
	Constellation::CONT_CONSTELLATION kContEnterTemp;
	m_ContEnter.swap(kContEnterTemp);
	Constellation::CONT_CONSTELLATION_BONUS_ITEM kContBonusItemTemp;
	m_ContBonusItem.swap(kContBonusItemTemp);
	m_ContEntranceCashItem.clear();
}

TiXmlNode const* PgConstellationMgr::ParseXml(TiXmlNode const* pNode
											, CONT_DEFITEM const* pDefItem
											, CONT_DEFMONSTER const* pDefMonster
											, CONT_DEF_QUEST_REWARD const* pDefQuestReward
											, int & rOutGroundNo)
{
	if( NULL == pNode || NULL == pDefItem || NULL == pDefMonster || NULL == pDefQuestReward )
	{
		return NULL;
	}

	int const NodeType = pNode->Type();
	switch(NodeType)
	{
	case TiXmlNode::ELEMENT:
		{
			TiXmlElement *pElement = (TiXmlElement *)pNode;

			std::string TagName(pElement->Value());

			if( TagName == "CHECK_ENTRANCE_CASH_ITEM" )
			{
				Constellation::CONT_CONSTELLATION_ENTRANCE_CASH_ITEM::mapped_type element;
				element.ParseXml( pElement );
				
				m_ContEntranceCashItem.insert( std::make_pair(element.WorldMapNo, element) );
			}
			else if( TagName == "WORLD" )
			{
				TiXmlAttribute* pAttr = pElement->FirstAttribute();
				while( pAttr )
				{
					char const* pcAttrName = pAttr->Name();
					char const* pcAttrValue = pAttr->Value();

					if( strcmp( pcAttrName, "MAPNO" ) == 0 )
					{
						rOutGroundNo = atoi(pcAttrValue);
					}
					pAttr = pAttr->Next();
				}
				
				const TiXmlNode* pChildNode = pNode->FirstChild();
				if( NULL != pChildNode )
				{
					while( ( pChildNode = ParseXml( pChildNode, pDefItem, pDefMonster, pDefQuestReward, rOutGroundNo) ) );
				}
			}
			else if( TagName == "CHECK_BONUS_CASH_ITEM" )
			{
				Constellation::SCheck_Bonus_Cash_item kTemp;
				kTemp.ParseXml(pElement, pDefItem);
				m_ContBonusItem.insert(std::make_pair(kTemp.WorldMapNo, kTemp));

			}
			else if( TagName == "LOCAL" )
			{
				TiXmlNode const* pFindLocalNode = PgXmlLocalUtil::FindInLocal(g_kLocal, pElement);
				if( pFindLocalNode )
				{
					TiXmlElement const* pResultNode = pFindLocalNode->FirstChildElement();
					if( pResultNode )
					{
						ParseXml( pResultNode, pDefItem, pDefMonster, pDefQuestReward, rOutGroundNo );
					}
				}
			}
			else if( TagName == "DUNGEON" )
			{
				Constellation::CONT_CONSTELLATION::mapped_type NewElement;
				Constellation::CONT_CONSTELLATION_ENTER::mapped_type SubElement;

				if( SubElement.ParseXml(pElement, pDefItem, pDefMonster, pDefQuestReward) )
				{
					Constellation::CONT_CONSTELLATION_ENTER::key_type Key(SubElement.Position, SubElement.Difficulty);
					Constellation::CONT_CONSTELLATION::iterator iter = m_ContEnter.find(rOutGroundNo);
					if( iter == m_ContEnter.end() )
					{
						NewElement.insert( std::make_pair(Key, SubElement) );
						m_ContEnter.insert( std::make_pair(rOutGroundNo, NewElement) );
					}
					else
					{
						iter->second.insert( std::make_pair(Key, SubElement) );
					}
				}
			}
		}break;
	default:
		{
		}break;
	}

	// ���� ���� ���� ��带 ��������� �Ľ��Ѵ�.
	const TiXmlNode* pNextNode = pNode->NextSibling();
	if(pNextNode)
	{
		return pNextNode;
	}

	return NULL;
}