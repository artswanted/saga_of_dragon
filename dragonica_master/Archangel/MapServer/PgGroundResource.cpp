#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgStringUtil.h"
#include "Variant/AlramMissionMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuest.h"

namespace PgGroundResourceUtil
{
	void ClearContGTrigger(CONT_GTRIGGER& rkCont)
	{
		CONT_GTRIGGER::iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			SAFE_DELETE((*iter).second);
			++iter;
		}
		rkCont.clear();
	}
};


size_t const DEFAULT_NODE_TOTAL = 11;
PgGroundResource::CONT_NiStream PgGroundResource::m_skShareStream;

PgGroundResource::PgGroundResource()
:	m_spSceneRoot(NULL)
,	m_spPhysXScene(NULL)
,	m_pkPhysxObject(NULL)
,	m_pkPhysxNode(NULL)
,	m_spPathRoot(NULL)
,	m_spSpawnRoot(NULL)
,	m_spPermissionRoot(NULL)
,	m_spTriggerRoot(NULL)
,	m_spLadderRoot(NULL)
,	m_kMin( FLT_MAX, FLT_MAX, FLT_MAX )
,	m_kMax( -FLT_MAX, -FLT_MAX, -FLT_MAX )
,	m_bFoundMinMax( false )
,	m_bLoadedByNif( false )
,	m_kAttribute(GATTR_DEFAULT)
,	m_bOpeningMovie(false)
,	m_kContTrigger()
,	m_eUnlockBidirection(UT_PET)	//���� �⺻������ ���� ����
,	m_pkEventAbil(dynamic_cast<PgEventAbil*>(&g_kEventView))
,	m_pkPT3ResultSpawnLoc(NULL)
,	m_pkAlramMissionMgr(NULL)
,	m_kMaxMonsterCount(0)
,	m_kMaxSpawnLocationCount(0)
,	m_kSpawnCountPerLocation(0)
,	m_bPartyBreakIn(0)
,	m_kContCheckPointOrder()
{
	::memset(m_iSpawnCount,0,sizeof(m_iSpawnCount));
}

PgGroundResource::~PgGroundResource()
{
	Clear();
}

PgGroundResource::PgGroundResource( PgGroundResource const &rhs )
:	m_pkEventAbil(dynamic_cast<PgEventAbil*>(&g_kEventView))
{
	CloneResource(&rhs);
}

PgGroundResource& PgGroundResource::operator=( PgGroundResource const &rhs )
{
	CloneResource(&rhs);
	return *this;
}

void PgGroundResource::CloneResource( PgGroundResource const * prhs )
{
	SetAttr( prhs->GetAttr() );
	m_bPartyBreakIn = prhs->m_bPartyBreakIn;
	m_kGroundKey.GroundNo(prhs->GroundKey().GroundNo());

	m_spSceneRoot	= prhs->m_spSceneRoot;
	m_spPhysXScene	= prhs->m_spPhysXScene;
	m_pkPhysxObject	= prhs->m_pkPhysxObject;
	m_pkPhysxNode	= prhs->m_pkPhysxNode;
	m_spPathRoot	= prhs->m_spPathRoot;
	m_spSpawnRoot	= prhs->m_spSpawnRoot;
	m_spPermissionRoot = prhs->m_spPermissionRoot;
	m_spTriggerRoot = prhs->m_spTriggerRoot;
	m_spLadderRoot	= prhs->m_spLadderRoot;

	m_kMin			= prhs->m_kMin;
	m_kMax			= prhs->m_kMax;

	::memcpy(m_iSpawnCount,prhs->m_iSpawnCount,sizeof(m_iSpawnCount));

	m_kSmallAreaInfo = prhs->m_kSmallAreaInfo;
	m_bLoadedByNif = prhs->m_bLoadedByNif;
	m_bFoundMinMax = prhs->m_bFoundMinMax;

	m_kNpcCont = prhs->m_kNpcCont;
	m_kContPvPHill = prhs->m_kContPvPHill;
	m_kStoneCtrl = prhs->m_kStoneCtrl;

	OpeningMovie( prhs->OpeningMovie() );

	m_eUnlockBidirection = prhs->m_eUnlockBidirection;
	prhs->CloneWorldEvent(*this);
	prhs->ClonseWEClientOjbect(*this);

	CopyContGTrigger( prhs->m_kContTrigger );
	m_kContCheckPointOrder = prhs->m_kContCheckPointOrder;

	PgEventAbil * const pkEventAbil = dynamic_cast< PgEventAbil * >(&g_kEventView);
	if ( prhs->m_pkEventAbil != pkEventAbil )
	{
		m_pkEventAbil = new_tr PgEventAbil;
		if ( m_pkEventAbil )
		{
			*m_pkEventAbil = *(prhs->m_pkEventAbil);
		}
		else
		{
			m_pkEventAbil = pkEventAbil;
		}
	}

	if ( prhs->m_pkPT3ResultSpawnLoc )
	{
		SetResultSpawnLoc( *(prhs->m_pkPT3ResultSpawnLoc) );
	}
	else
	{
		SAFE_DELETE(m_pkPT3ResultSpawnLoc);
	}

	m_pkAlramMissionMgr = prhs->m_pkAlramMissionMgr;
	m_kContEventScriptStopAI = prhs->m_kContEventScriptStopAI;
	m_kContEventScriptNoLock = prhs->m_kContEventScriptNoLock;
	m_kMaxMonsterCount = prhs->m_kMaxMonsterCount;
	m_kMaxSpawnLocationCount = prhs->m_kMaxSpawnLocationCount;
	m_kSpawnCountPerLocation = prhs->m_kSpawnCountPerLocation;
}

void PgGroundResource::SetMutatorAbil(std::set<int> const rhs)
{
	CONT_DEF_MISSION_MUTATOR const * pkMutator = NULL;
	g_kTblDataMgr.GetContDef(pkMutator);
	if(!pkMutator)
	{
		return;
	}

	int iResultExpRate = 0;
	int iResultMoneyRate = 0;
	for(std::set<int>::const_iterator it = rhs.begin(); it != rhs.end(); it++)
	{
		CONT_DEF_MISSION_MUTATOR::const_iterator kMutator = pkMutator->find(*it);
		if(kMutator == pkMutator->end())
		{
			continue;
		}
		iResultExpRate += kMutator->second.iAddedExpRate;
		iResultMoneyRate += kMutator->second.iAddedMoneyRate;
	}

	m_pkEventAbil->SetAbil(AT_ADD_EXP_PER, m_pkEventAbil->GetAbil(AT_ADD_EXP_PER) + iResultMoneyRate);
	m_pkEventAbil->SetAbil(AT_ADD_MONEY_PER, m_pkEventAbil->GetAbil(AT_ADD_MONEY_PER) + iResultMoneyRate);
}

void PgGroundResource::Clear()
{
	m_spLadderRoot = NULL;
	m_spTriggerRoot = NULL;
	m_spSpawnRoot = NULL;
	m_spPermissionRoot = NULL;
	m_spPathRoot = NULL;
	m_pkPhysxNode = NULL;
	m_pkPhysxObject = NULL;
	m_spPhysXScene = NULL;
	m_spSceneRoot = NULL;

	m_kSmallAreaInfo = PgSmallAreaInfo();

	m_kContPvPHill.clear();
	m_kGroundKey.Clear();

	OpeningMovie(false);

	// MultiSpawn
	::memset(m_iSpawnCount,0,sizeof(m_iSpawnCount));

	// GroundTrigger
	PgGroundResourceUtil::ClearContGTrigger(m_kContTrigger);
	m_kContCheckPointOrder.clear();

	// Emporia Portal
	m_kContEmporiaPortal.clear();

	PgEventAbil * const pkEventAbil = dynamic_cast< PgEventAbil * >(&g_kEventView);
	if ( m_pkEventAbil != pkEventAbil )
	{
		SAFE_DELETE( m_pkEventAbil );
		m_pkEventAbil = pkEventAbil;
	}

	SAFE_DELETE(m_pkPT3ResultSpawnLoc);
	m_pkAlramMissionMgr = NULL;
}

HRESULT PgGroundResource::LoadGsa( char const *pcGsaPath )
{
	//int iLoadStep = 0;
	INFO_LOG(BM::LOG_LV6, __FL__<<L"[LoadGsa] "<<UNI(pcGsaPath));
	m_bLoadedByNif = false;

	//INFO_LOG(BM::LOG_LV6, _T("[LoadGsa] LoadStep ") << iLoadStep++);
	
	NiEntityStreaming* pkEntityStreaming = NiFactories::GetStreamingFactory()->GetPersistent("GSA");
	if ( !pkEntityStreaming->Load(pcGsaPath) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("[LoadGsa] Failed GSA=") << UNI(pcGsaPath));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	//INFO_LOG(BM::LOG_LV6, _T("[LoadGsa] LoadStep ") << iLoadStep++);
	m_spSceneRoot = NULL;
	m_spPhysXScene = NULL;
	NiScene *pkScene = pkEntityStreaming->GetSceneAt(0);

	// BaseCase
	NiPoint3 kBaseTranslation(0,0,0);
	NiMatrix3 kBaseRotMat;
	float fBaseScale = 1.0f;

	bool bFindBase = false;
	bool bFindSlave = false;
	int const iTot = pkScene->GetEntityCount();

	//INFO_LOG(BM::LOG_LV6, _T("[LoadGsa] LoadStep ") << iLoadStep++);
	for (int i=0 ; i<iTot ; i++)
	{
		NiEntityInterface *pkEntity = pkScene->GetEntityAt(i);
		if (pkEntity->GetPGProperty().Equals("BaseObject"))
		{
			NiFixedString kPath;
			pkEntity->GetPropertyData("NIF File Path", kPath, 0);
			pkEntity->GetPropertyData("Translation", kBaseTranslation);
			pkEntity->GetPropertyData("Rotation", kBaseRotMat);
			pkEntity->GetPropertyData("Scale", fBaseScale);

			if( !kPath.Exists() )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("[LoadGsa] GSA=") << pcGsaPath << _T(", kPath isn't exist =") << kPath.operator char const*());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
			// Load Base Nif
			NiStream kStream;
			if( !kStream.Load(kPath) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[LoadGsa] GSA=") << pcGsaPath << _T(", Stream is null =") << kPath.operator char const*());
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			bFindBase = true;
			m_spSceneRoot = NiDynamicCast(NiNode, kStream.GetObjectAt(0));
			unsigned int cnt = kStream.GetObjectCount();
			for (unsigned int ui = 0; ui < cnt; ui++)
			{
				NiObject *pkObject = kStream.GetObjectAt(ui);

				// BaseNode���� Transform�� �ٲپ� �ش�.
				if(NiIsKindOf(NiNode, pkObject))
				{
					NiNode* pkNode = (NiNode*)pkObject;
					pkNode->Update(0);
					if (pkNode)
					{
						for (unsigned int uj=0 ; uj<pkNode->GetChildCount() ; uj++)
						{
							NiAVObject *pkAVObj = (NiAVObject*)pkNode->GetAt(uj);
							if (pkAVObj)
							{
								float fAppScale = fBaseScale * pkAVObj->GetWorldScale();
								NiMatrix3 fAppRot = kBaseRotMat * pkAVObj->GetWorldRotate();
								NiPoint3 fAppTrans = kBaseTranslation + pkAVObj->GetWorldTranslate();
								pkAVObj->SetScale(fAppScale);
								pkAVObj->SetRotate(fAppRot);
								pkAVObj->SetTranslate(fAppTrans);
							}
						}
					}
				}

				// ���� ��带 �����Ѵ�.
				if(NiIsKindOf(NiPhysXScene, pkObject))
				{
					if (m_spPhysXScene)
					{
						m_spPhysXScene->ReleaseSnapshot();
						m_spPhysXScene->ClearSceneFromSnapshot();
						m_spPhysXScene->ReleaseScene();
						m_spPhysXScene = 0;
					}
					NiTransform kTransform;
					kTransform.m_fScale = fBaseScale;
					kTransform.m_Rotate = kBaseRotMat;
					kTransform.m_Translate = kBaseTranslation;
					m_spPhysXScene = (NiPhysXScene *)pkObject;
					m_spPhysXScene->CreateSceneFromSnapshot(0);
					m_spPhysXScene->SetSceneXform(kTransform);
				}
			}
			break;
		}
	}

	// End of Base

	if (!bFindBase)
	{
		NiAVObject *pkRoot = NiNew NiNode;
		pkRoot->SetName("Scene Root");
		m_spSceneRoot = (NiNode *)pkRoot;
	}

	// Create default node
	// �տ� ���� �������� �������� Type
	// �ڿ� ���� ���̴� ��� �̸�.
	static char const *szDefaultNodeName[DEFAULT_NODE_TOTAL][2] =
	{
		{"Path", "paths"},
		{"PhysX", "physx"},
		{"CharacterSpawn", "char_spawns"},
		{"PermissionArea", "permission_area"},
		{"CameraWalls", "camera_walls"},
		{"SkyBox", "sky_boxes"},
		{"Trigger", "triggers"},
		{"Water", "waters"},
		{"Ladder", "ladders"},
		{"Rope", "ropes"},
		{"Room", "rooms"},
	};
	for (int i=0 ; i<DEFAULT_NODE_TOTAL ; i++)
	{
		// Base���� �߰� ���� �ʾ��� ��쿡�� ��� �߰��� �Ѵ�.
		if (SearchNode(m_spSceneRoot, szDefaultNodeName[i][1], false) <= 0)
		{
			NiAVObject *pkDefaultNode = NiNew NiNode();
			pkDefaultNode->SetName(szDefaultNodeName[i][1]);
			m_spSceneRoot->AttachChild(pkDefaultNode);
		}
	}

	// GSA���� Physx�� �ϳ��� ���� ��찡 �ֱ� ������.
	// Nodecheck�� �����ϱ� ���Ͽ� Physx �ϳ��� �־��ش�.
	if (((NiNode*)m_spSceneRoot->GetObjectByName("physx"))->GetChildCount() <= 0)
	{

		NiNode* pkDummyParent = ((NiNode*)m_spSceneRoot->GetObjectByName("physx"));
		if (pkDummyParent)
		{
			NiAVObject *pkDummyNode = NiNew NiNode();
			pkDummyParent->AttachChild(pkDummyNode);
		}
	}

	// Create PhysX Scene
	if(!m_spPhysXScene)
	{
		m_spPhysXScene = NiNew NiPhysXScene();

		// Create NxScene
		NxSceneDesc kSceneDesc;
		kSceneDesc.gravity = NxVec3(0.0f, 0.0f, 9.8f);
		kSceneDesc.simType = NX_SIMULATION_SW;
		kSceneDesc.maxTimestep = 1/10.0f;

		if( !NiPhysXManager::GetPhysXManager() )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[LoadGsa] PhysXManager is null"));
			return E_FAIL;
		}
		if( !NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[LoadGsa] PhysXSDK is null"));
			return E_FAIL;
		}

		//
		{
			//static int g_iCreateSceneCount = 0;
			//g_iCreateSceneCount += 1;
			if( NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK->getNbScenes() >= 64 )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[LoadGsa] PhysXScene Count over 64 that Ageia PhysX limitation."));
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}


			NxScene* pkNxScene = NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK->createScene(kSceneDesc);
			if( !pkNxScene )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[LoadGsa] pkNxScene is null"));
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
	
			m_spPhysXScene->SetPhysXScene(pkNxScene);
		}
	}

	NiExternalAssetManagerPtr pkAssetManager = NiNew NiExternalAssetManager(NiFactories::GetAssetFactory());
	int iSceneCnt = pkEntityStreaming->GetSceneCount();


	// �ϳ��� Entity�� �ϳ��� Nif�� ����
	int iTotal = pkScene->GetEntityCount();
	for (int i=0 ; i<iTotal ; i++)
	{
		NiEntityInterface *pkEntity = pkScene->GetEntityAt(i);
		if( !pkEntity )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"[LoadGsa] entity["<<i<<L"] is null");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkEntity is NULL"));
			continue;
		}

#ifdef LOADGSA_USE_AVAILABLE_ENTITY
		// ���� �ʴ� ������Ʈ���� �ε带 ���� �ʴ´�.
		if (GetAvailEntity(pkEntity) == E_FAIL)
		{
			continue;
		}
#else
		if (pkEntity->GetPGProperty().Equals("BaseObject"))
		{
			continue;
		}
#endif


#ifdef LOADGSA_USE_SHARE
		NiStream *pkShareStream = 0;
		NiStream kUniqueStream;
#else
		NiStream kStream;
#endif
		NiFixedString kPath;
		pkEntity->GetPropertyData("NIF File Path", kPath);
		if (kPath.Exists())
		{
			NiFixedString kEntityName = pkEntity->GetName();
			std::string strPath = kPath;
			UPR(strPath);
			NiNode* pkNifRoot = 0;

#ifdef LOADGSA_USE_SHARE
			bool bShareStream = false;
			kUniqueStream.Load(kPath);
			// ���� Ʈ���Ű� �ִٸ� Share�� ���� �ʰ� �׳� �ε� �Ѵ�.
			bool bIsTrigger = false;
			unsigned int tcnt = kUniqueStream.GetObjectCount();
			for (unsigned int ui = 0 ; ui < tcnt ; ui++)
			{
				NiObject *pkObject = kUniqueStream.GetObjectAt(ui);
				if(NiIsKindOf(NiPhysXScene, pkObject))
				{
					NiPhysXScene *pkPhysXObject = 0;
					pkPhysXObject = NiDynamicCast(NiPhysXScene, pkObject);
					for (unsigned int iSrcCount=0 ; iSrcCount<pkPhysXObject->GetSourcesCount() ; iSrcCount++)
					{
						NiPhysXSrc *pkPhysXSrc = pkPhysXObject->GetSourceAt(iSrcCount);
						if(NiIsKindOf(NiPhysXRigidBodySrc, pkPhysXSrc))
						{
							bIsTrigger = true;
							iSrcCount = pkPhysXObject->GetSourcesCount();
							ui = tcnt;
							break;
						}
					}
				}
			}
			// Ʈ���� ã�� ��.

			//std::map< std::string, NiStream* >::iterator share_itr;
			CONT_NiStream::iterator share_itr = m_skShareStream.find(strPath);
			// Ʈ������ ��� Share�� ���� �ʴ´�.
			if (bIsTrigger)
			{
				pkShareStream = &kUniqueStream;
				pkNifRoot = NiDynamicCast(NiNode, pkShareStream->GetObjectAt(0));
			}
			else if (share_itr != m_skShareStream.end())
			{
				bShareStream = true;
				pkShareStream = share_itr->second;
				pkNifRoot = NiDynamicCast(NiNode, pkShareStream->GetObjectAt(0));
			}
			else
			{
				pkShareStream = NiNew NiStream();
				if( !pkShareStream->Load(kPath) )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"[LoadGsa] " << UNI(pcGsaPath) << L" NIF Stream Loading Failed ["<<UNI(kPath.operator char const*())<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
					return E_FAIL;
				}
				pkNifRoot = NiDynamicCast(NiNode, pkShareStream->GetObjectAt(0));
				m_skShareStream.insert(std::make_pair(strPath, pkShareStream));
			}
#else
			if( !kStream.Load(kPath) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"[LoadGsa] nif stream loading failed ["<<UNI(kPath.operator char const*())<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Load Failed Stream"));
			}
			pkNifRoot = NiDynamicCast(NiNode, kStream.GetObjectAt(0));
#endif
			if( !pkNifRoot )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"[LoadGsa] no root node ["<<UNI(kPath.operator char const*())<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}

			if (pkNifRoot)
			{
				NiNode *pkSceneRoot = m_spSceneRoot;

				// Transform
				NiPoint3 kTranslation;
				NiMatrix3 kRotMat;
				kRotMat.IDENTITY;
				float fScale;
				pkEntity->GetPropertyData("Translation", kTranslation);
				pkEntity->GetPropertyData("Rotation", kRotMat);
				pkEntity->GetPropertyData("Scale", fScale);
				NiPoint3 kObjTransl = kTranslation;
				NiMatrix3 kObjRotMat = kRotMat;
				kObjRotMat.IDENTITY;
				float kObjRootScale = fScale;

				// PG Property
				bool bUsePhysX = pkEntity->GetPGUsePhysX();

				// �� �Ӽ��� �ִ� ���� ������ ���ӿ� �־��ش�.
				NiAVObject *pkGetObject = 0;
				for (int iDefaultNodeCount=0 ;
					iDefaultNodeCount<DEFAULT_NODE_TOTAL;
					iDefaultNodeCount++)
				{
					if (pkEntity->GetPGProperty().Equals(szDefaultNodeName[iDefaultNodeCount][0]))
					{
						pkGetObject = pkSceneRoot->GetObjectByName(szDefaultNodeName[iDefaultNodeCount][1]);
						pkSceneRoot = (NiNode*)pkGetObject;
						kObjTransl = kTranslation - pkSceneRoot->GetTranslate();
						kObjRootScale = fScale * (1 / pkSceneRoot->GetScale());
						kObjRotMat = pkSceneRoot->GetRotate().Inverse() * kRotMat;

						break;
					}
				}

				POINTER_VERIFY_INFO_LOG(pkSceneRoot);
				NiNode *pkNewNode = 0;
				{
					pkNewNode = NiNew NiNode();

					bool bIsCharacter = false;
					bool bIsHaveNode = false;
					NiNode *pkAniNode = 0;
					NiNode *pkChildRootNode = pkNewNode;
					int iTotalChild = pkNifRoot->GetChildCount();
					for (int iChildCount=0 ;
						iChildCount < iTotalChild;
						++iChildCount)
					{
						NiAVObject *pkObject = pkNifRoot->GetAt(iChildCount);
						if (!pkObject)
						{
							continue;
						}
						// ���� �ִ� Character��� NewNode�� �ƴ� �� ���� ���� ��忡 TRS�� �ش�.
						if (NiIsKindOf(NiGeometry, pkObject) &&
							((NiGeometry *)pkObject)->GetSkinInstance())
						{
							bIsCharacter = true;
						}
						if (NiIsKindOf(NiNode, pkObject))
						{
							bIsHaveNode = true;
							pkAniNode = NiDynamicCast(NiNode, pkObject);
						}
						// ������Ʈ ��忡 �߰� ��Ų��.
						if (NiIsKindOf(NiAVObject, pkObject))
						{
#ifdef LOADGSA_USE_SHARE
							if (bIsTrigger)
								pkNewNode->AttachChild(NiDynamicCast(NiAVObject, pkObject));
							else
								pkNewNode->AttachChild(NiDynamicCast(NiAVObject, pkObject->Clone()));
#else
							pkNewNode->AttachChild(NiDynamicCast(NiAVObject, pkObject));
#endif
						}
					}

					if (bIsCharacter && bIsHaveNode && pkAniNode)
					{
						pkChildRootNode = pkAniNode;
					}
					pkChildRootNode->SetScale(kObjRootScale);
					pkChildRootNode->SetRotate(kObjRotMat);
					pkChildRootNode->SetTranslate(kObjTransl);
				}

				pkNewNode->SetName(kEntityName);
				pkSceneRoot->AttachChild(pkNewNode);

				// Physx Data�� ã�Ƽ� �־�����.
				NiPoint3 kPhysXTranslation = kTranslation;
				NiMatrix3 kPhysXRotMat = kRotMat;
				float fPhysXScale = fScale;
#ifdef LOADGSA_USE_SHARE
				unsigned int cnt = kUniqueStream.GetObjectCount();
				for (unsigned int ui = 0 ; ui < cnt ; ui++)
				{
					NiObject *pkObject = kUniqueStream.GetObjectAt(ui);
#else
				unsigned int cnt = kStream.GetObjectCount();
				for (unsigned int ui = 0 ; ui < cnt ; ui++)
				{
					NiObject *pkObject = kStream.GetObjectAt(ui);
#endif
					
					if(NiIsKindOf(NiPhysXScene, pkObject) && bUsePhysX)
					{
						NiPhysXScene *pkPhysXObject = NiDynamicCast(NiPhysXScene, pkObject);

						// ������ �� ��Ʈ�� ���� ��Ų��.
						// �׷����ν� �浹 �� ȿ���� ����.
						if (pkPhysXObject->GetSnapshot())
						{
							NiPhysXSceneDesc *pkDesc = pkPhysXObject->GetSnapshot();
							// �������� 1�� �ƴҰ��. ReCook�� �Ѵ�.
							if ( fPhysXScale != 1.0f )
							{
								SetScaleToPhysXObject(pkDesc, fPhysXScale);
							}
							// �ߺ� ������ ������ �������� �߸� ������ ������
							// ������ �̸��� �ٲپ� �ش�.
							int iActorTotal = pkDesc->GetActorCount();
							for (int iActorCount=0 ; iActorCount<iActorTotal ; ++iActorCount)
							{
								NiPhysXActorDesc *pkActorDesc = pkDesc->GetActorAt(iActorCount);
								int iShapeTotal = pkActorDesc->GetActorShapes().GetSize();
								for (int iShapeCount=0 ; iShapeCount<iShapeTotal ; iShapeCount++)
								{
									NiPhysXShapeDesc *pkShapeDesc =
										pkActorDesc->GetActorShapes().GetAt(iShapeCount);

									// Rename PhysX Object
									if (pkShapeDesc->GetMeshDesc())
									{
										NiString strDescName = pkEntity->GetName();
										strDescName += "_";
#ifdef LOADGSA_USE_SHARE
										char szCount[256];
										_itoa_s(iActorCount, szCount, 10);
										strDescName += szCount;
										strDescName += "_";
										_itoa_s(iShapeCount, szCount, 10);
										strDescName += szCount;
										strDescName += "_";
										_itoa_s((int)(fScale*10000), szCount, 10);
										strDescName += szCount;
#else
										strDescName += pkShapeDesc->GetMeshDesc()->GetName();
#endif
										strDescName += "_";
										strDescName += kPath;
										NiFixedString strDescName_ = strDescName.MakeExternalCopy();
										pkShapeDesc->GetMeshDesc()->SetName(strDescName_);
										bFindSlave = true;

#ifdef LOADGSA_FIND_MINMAX
										// ���ؽ��� �� MinMax ���ϱ�
										if (!pkShapeDesc)
										{
											INFO_LOG( BM::LOG_LV5, __FL__<<L"[LoadGsa] ShapeDesc is null... Actor: "<<UNI(pkActorDesc->GetActorName().operator char const*()));
											LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkShapeDesc is NULL"));
										}
										else if (!pkShapeDesc->GetMeshDesc())
										{
											continue;
										}
										else if (pkShapeDesc->GetType() != NX_SHAPE_MESH)
										{
											//INFO_LOG( BM::LOG_LV5, _T("[LoadGsa] NIF Have Differnt physx shape object! Actor: %s, Object: %s"), UNI(pkActorDesc->GetActorName().operator char const*()), UNI(pkShapeDesc->GetMeshDesc()->GetName().operator char const*()));
										}
										else
										{
											if( false == pkEntity->GetName().ContainsNoCase("path") )//false == pkEntity->GetName().ContainsNoCase("physx") && 
											{
												NxTriangleMeshShapeDesc kTriMeshShapeDesc;
												pkShapeDesc->GetMeshDesc()->ToTriMeshDesc(kTriMeshShapeDesc, true);

												NxTriangleMesh *pkTriMesh = kTriMeshShapeDesc.meshData;
												if(!pkTriMesh)
												{
													continue;
												}

												NxTriangleMeshDesc kTriMeshDesc;
												pkTriMesh->saveToDesc(kTriMeshDesc);

												unsigned short usNumNxVerts = kTriMeshDesc.numVertices;
												NxVec3* pkNxVerts = (NxVec3*)kTriMeshDesc.points;

												for (unsigned short us = 0; us < usNumNxVerts; us++)
												{
													NiPoint3 kWorldVertex;
													kWorldVertex.x = pkNxVerts[us].x + kPhysXTranslation.x;
													kWorldVertex.y = pkNxVerts[us].y + kPhysXTranslation.y;
													kWorldVertex.z = pkNxVerts[us].z + kPhysXTranslation.z;

													m_kMin.x = __min(m_kMin.x, kWorldVertex.x);
													m_kMin.y = __min(m_kMin.y, kWorldVertex.y);
													m_kMin.z = __min(m_kMin.z, kWorldVertex.z);

													m_kMax.x = __max(m_kMax.x, kWorldVertex.x);
													m_kMax.y = __max(m_kMax.y, kWorldVertex.y);
													m_kMax.z = __max(m_kMax.z, kWorldVertex.z);

													m_bFoundMinMax = true;
												}
											}
										}
#endif
									}
								}
							}

							NxMat34 kSlaveMat;
							NiPhysXTypes::NiTransformToNxMat34(kPhysXRotMat, kPhysXTranslation, kSlaveMat);
							pkPhysXObject->SetSlaved(m_spPhysXScene, kSlaveMat);
							pkPhysXObject->CreateSceneFromSnapshot(0);
						}

						// Insert PhysX Kinetic Data(Trigger Data)
						for (unsigned int iSrcCount=0 ; iSrcCount<pkPhysXObject->GetSourcesCount() ; iSrcCount++)
						{
							NiPhysXSrc *pkPhysXSrc = pkPhysXObject->GetSourceAt(iSrcCount);
							if(NiIsKindOf(NiPhysXRigidBodySrc, pkPhysXSrc))
							{
								NiPhysXRigidBodySrc *pkBodySrc = (NiPhysXRigidBodySrc *)pkPhysXSrc;
								NiAVObject *pkSrcObj = pkBodySrc->GetSource();
								pkSrcObj->SetName(pkEntity->GetName());
							}
							m_spPhysXScene->AddSource(pkPhysXSrc);
						}

					}
				}
			}
		}
		else
		{
			pkEntity->GetPropertyData("KFM File Path", kPath);	//Nif������ ���� KFM������ �ִ� Ʈ������ ���
			if(kPath.Exists())
			{
				// Transform
				NiPoint3 kTranslation;
				NiMatrix3 kRotMat;
				kRotMat.IDENTITY;
				float fScale = 1.0f;
				pkEntity->GetPropertyData("Translation", kTranslation);
				pkEntity->GetPropertyData("Rotation", kRotMat);
				pkEntity->GetPropertyData("Scale", fScale);

				NiNode *pkNewNode = 0;
				pkNewNode = NiNew NiNode();
				pkNewNode->SetScale(fScale);
				pkNewNode->SetRotate(kRotMat);
				pkNewNode->SetTranslate(kTranslation);

				pkNewNode->SetName(pkEntity->GetName());
				m_spSceneRoot->AttachChild(pkNewNode);
			}
		}
	}

	pkAssetManager = 0;

	if (m_spPhysXScene)
	{
		m_spPhysXScene->UpdateSources(0);
	}

	if (!m_spPhysXScene->GetPhysXScene())
	{
		m_spPhysXScene->CreateSceneFromSnapshot(0);
	}

#ifdef LOADGSA_FIND_MINMAX
	// FindMinMax
	if (bFindBase)
	{
		FindMinMaxInLoadGsa(m_spPhysXScene, NiPoint3(0,0,0), pcGsaPath);
	}
#endif

	// Transform�� ���� �Ǿ��� ������ Update
	m_spSceneRoot->UpdateNodeBound();
	m_spSceneRoot->UpdateProperties();
	m_spSceneRoot->UpdateEffects();
	m_spSceneRoot->Update(0.0f);

	// ���� �˻��� ���� ������ �غ��Ѵ�.
	m_spPathRoot = (NiNode *)m_spSceneRoot->GetObjectByName("paths");
	m_spSpawnRoot = (NiNode *)m_spSceneRoot->GetObjectByName("char_spawns");
	m_spPermissionRoot = (NiNode *)m_spSceneRoot->GetObjectByName("permission_area");
	m_spTriggerRoot = (NiNode *)m_spSceneRoot->GetObjectByName("triggers");
	m_spLadderRoot = (NiNode *)m_spSceneRoot->GetObjectByName("ladders");
	m_pkPhysxObject = (NiNode *)m_spSceneRoot->GetObjectByName("physx");

	if ( m_pkPhysxObject == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_pkPhysxNode = NiDynamicCast(NiNode, m_pkPhysxObject);
	
	if ( m_pkPhysxNode == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	// char_spawn_1 �� �ִ��� Ȯ��
	NiAVObject *pkSpawn = NULL;
	pkSpawn = m_spSpawnRoot->GetObjectByName("char_spawn_1");
	
	if ( !pkSpawn )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, "[LoadGsa] Can't Find SpawnLoc [char_spawn_1] "<<pcGsaPath);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	char acSpawnName[64] = {0,};
	::memset( m_iSpawnCount, 0, sizeof(m_iSpawnCount) );

	// Red Spawn�� � �ִ��� Ȯ��
	int iPortalNo = 0;
	do
	{
		if ( ++iPortalNo > 100 )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"[LoadGsa] OverFlow RED SpawnCount["<<iPortalNo<<L"]" );
			break;
		}
		::sprintf_s(acSpawnName,sizeof(acSpawnName),"red_spawn_%d",iPortalNo);
		pkSpawn = m_spSpawnRoot->GetObjectByName(acSpawnName);
	}while(pkSpawn);
	

	m_iSpawnCount[0] = iPortalNo-1;

	if ( m_iSpawnCount[0] > 0 )
	{
		INFO_LOG(BM::LOG_LV7, __FL__<<L"[LoadGsa] Find RED SpawnCount["<<m_iSpawnCount[0]<<L"]");
	}
	
	// Blue Spawn�� � �ִ��� Ȯ��
	m_iSpawnCount[1] = 0;
	iPortalNo = 0;
	pkSpawn = NULL;
	do
	{
		if ( ++iPortalNo > 100 )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"[LoadGsa] OverFlow RED SpawnCount["<<iPortalNo<<L"]");
			break;
		}
		::sprintf_s(acSpawnName,sizeof(acSpawnName),"blue_spawn_%d",iPortalNo);
		pkSpawn = m_spSpawnRoot->GetObjectByName(acSpawnName);
	}while(pkSpawn);

	m_iSpawnCount[1] = iPortalNo-1;
	if ( m_iSpawnCount[1] > 0 )
	{
		INFO_LOG(BM::LOG_LV7, __FL__<<L"[LoadGsa] Find BLUE SpawnCount["<<m_iSpawnCount[1]<<L"]");
	}
	
	INFO_LOG(BM::LOG_LV1, _T("[LoadGsa] OK:") << UNI(pcGsaPath) << _T(" Loaded Count:")<<NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK->getNbScenes());

	return S_OK;
}

HRESULT PgGroundResource::LoadNif(char const *pcNifPath)
{
	m_bLoadedByNif = true;

//	INFO_LOG(BM::LOG_LV7, _T("[%s] Ground Init Start .....File[%s]"), __FUNCTIONW__, UNI(pcNifPath));

	NiStream kStream;

	if(!kStream.Load(pcNifPath))
	{
		assert(NULL);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_spSceneRoot = (NiNode *)kStream.GetObjectAt(0);
	m_spSceneRoot->UpdateNodeBound();
	m_spSceneRoot->UpdateProperties();
	m_spSceneRoot->UpdateEffects();
	m_spSceneRoot->Update(0.0f);

	unsigned int cnt = kStream.GetObjectCount();
	for (unsigned int ui = 1; ui < cnt; ui++)
	{
		NiObject *pkObject = kStream.GetObjectAt(ui);

		// ���� ��带 �����Ѵ�.
		if(NiIsKindOf(NiPhysXScene, pkObject))
		{
			m_spPhysXScene = (NiPhysXScene *)pkObject;
			m_spPhysXScene->CreateSceneFromSnapshot(0);
		}
	}

	// ���� �˻��� ���� ������ �غ��Ѵ�.
	m_spPathRoot = (NiNode *)m_spSceneRoot->GetObjectByName("paths");
	m_spSpawnRoot = (NiNode *)m_spSceneRoot->GetObjectByName("char_spawns");
	m_spPermissionRoot = (NiNode *)m_spSceneRoot->GetObjectByName("permission_area");
	m_spTriggerRoot = (NiNode *)m_spSceneRoot->GetObjectByName("triggers");
	m_spLadderRoot = (NiNode *)m_spSceneRoot->GetObjectByName("ladders");

	m_pkPhysxObject = (NiNode *)m_spSceneRoot->GetObjectByName("physx");
	if ( m_pkPhysxObject == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	m_pkPhysxNode = NiDynamicCast(NiNode, m_pkPhysxObject);
	if ( m_pkPhysxNode == NULL )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	//m_pkPhysxPick = NiNew NiPick;
	//m_pkPhysxPick->SetPickType(NiPick::FIND_FIRST);
	//m_pkPhysxPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	//m_pkPhysxPick->SetCoordinateType(NiPick::WORLD_COORDINATES);
	//m_pkPhysxPick->SetReturnNormal(true);

	kStream.RemoveAllObjects();

	INFO_LOG(BM::LOG_LV7, __FL__<<L"LoadNIF["<<UNI(pcNifPath)<<L"]");
	return true;
}

bool PgGroundResource::FindMinMaxInLoadGsa(NiPhysXScene *pkScene, NiPoint3 kTranslation, char const *pcGsaPath)
{
	NiPhysXSceneDesc *pkDesc = pkScene->GetSnapshot();
	if (!pkDesc)
	{
		INFO_LOG( BM::LOG_LV5, __FL__<<L"SceneDesc is null... Gsa: "<<UNI(pcGsaPath));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iActorTotal = pkDesc->GetActorCount();
	for (int iActorCount=0 ; iActorCount<iActorTotal ; ++iActorCount)
	{
		NiPhysXActorDesc *pkActorDesc = pkDesc->GetActorAt(iActorCount);
		int iShapeTotal = pkActorDesc->GetActorShapes().GetSize();
		for (int iShapeCount=0 ; iShapeCount<iShapeTotal ; iShapeCount++)
		{
			NiPhysXShapeDesc *pkShapeDesc =
				pkActorDesc->GetActorShapes().GetAt(iShapeCount);
			//
			// ���ؽ��� �� MinMax ���ϱ�
			if (!pkShapeDesc)
			{
				INFO_LOG( BM::LOG_LV5, __FL__<<L"ShapeDesc is null... Gsa: "<<UNI(pcGsaPath)<<L", Actor: "<<UNI(pkActorDesc->GetActorName().operator char const*()) );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkShapeDesc is NULL"));
			}
			else if (!pkShapeDesc->GetMeshDesc())
			{
				continue;
			}
			else if (pkShapeDesc->GetType() != NX_SHAPE_MESH)
			{
				//INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] Nif have differnt physx shape object... Gsa: %s, Actor: %s, Object: %s"),
				//	__FUNCTIONW__, __LINE__, UNI(pcGsaPath), UNI(pkActorDesc->GetActorName().operator char const*()), UNI(pkShapeDesc->GetMeshDesc()->GetName().operator char const*()));
			}
			else
			{
				NxTriangleMeshShapeDesc kTriMeshShapeDesc;
				pkShapeDesc->GetMeshDesc()->ToTriMeshDesc(kTriMeshShapeDesc, true);

				NxTriangleMesh *pkTriMesh = kTriMeshShapeDesc.meshData;
				if(!pkTriMesh)
				{
					continue;
				}

				NxTriangleMeshDesc kTriMeshDesc;
				pkTriMesh->saveToDesc(kTriMeshDesc);

				unsigned short usNumNxVerts = kTriMeshDesc.numVertices;
				NxVec3* pkNxVerts = (NxVec3*)kTriMeshDesc.points;

				for (unsigned short us = 0; us < usNumNxVerts; us++)
				{
					NiPoint3 kWorldVertex;
					kWorldVertex.x = pkNxVerts[us].x + kTranslation.x;
					kWorldVertex.y = pkNxVerts[us].y + kTranslation.y;
					kWorldVertex.z = pkNxVerts[us].z + kTranslation.z;

					m_kMin.x = __min(m_kMin.x, kWorldVertex.x);
					m_kMin.y = __min(m_kMin.y, kWorldVertex.y);
					m_kMin.z = __min(m_kMin.z, kWorldVertex.z);

					m_kMax.x = __max(m_kMax.x, kWorldVertex.x);
					m_kMax.y = __max(m_kMax.y, kWorldVertex.y);
					m_kMax.z = __max(m_kMax.z, kWorldVertex.z);
				}
			}
		}
	}

	m_bFoundMinMax = true;
	return true;
}

void PgGroundResource::FindMinMax(NiNode *pkRootNode, NxVec3 &kMin, NxVec3 &kMax)
{
	if(!pkRootNode)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkRootNode is NULL"));
		return;
	}
	// �Ʒ� �ڵ尡 ����� �۵��� �� ���� �Ʒ� �ڵ�� ������.
	/*
	NiBound kBound = pkRootNode->GetWorldBound();
	kMin.x = kBound.GetCenter().x - kBound.GetRadius();
	kMin.y = kBound.GetCenter().y - kBound.GetRadius();
	kMin.z = kBound.GetCenter().z - kBound.GetRadius();
	kMax.x = kBound.GetCenter().x + kBound.GetRadius();
	kMax.y = kBound.GetCenter().y + kBound.GetRadius();
	kMax.z = kBound.GetCenter().z + kBound.GetRadius();
	*/

	unsigned int uiArrayCount = pkRootNode->GetArrayCount();
	for(unsigned int uiIndex = 0; uiIndex < uiArrayCount; ++uiIndex)
	{
		NiAVObject *pkChild = pkRootNode->GetAt(uiIndex);
		if(!pkChild)
		{
			continue;
		}

		if(NiIsKindOf(NiGeometry, pkChild))
		{
			NiGeometry *pkGeometry = (NiGeometry *)pkChild;
			NiPoint3 *pkVertices = pkGeometry->GetVertices();
			unsigned short usVertexCnt = pkGeometry->GetVertexCount();
			for(unsigned short usIndex = 0; usIndex < usVertexCnt; ++usIndex)
			{
				NiPoint3 *pkVertex = &pkVertices[usIndex];
				NiPoint3 kWorldVertex = *pkVertex + pkChild->GetWorldTranslate();

				kMin.x = __min(kMin.x, kWorldVertex.x);
				kMin.y = __min(kMin.y, kWorldVertex.y);
				kMin.z = __min(kMin.z, kWorldVertex.z);

				kMax.x = __max(kMax.x, kWorldVertex.x);
				kMax.y = __max(kMax.y, kWorldVertex.y);
				kMax.z = __max(kMax.z, kWorldVertex.z);
				//				INFO_LOG( BM::LOG_LV1, _T("Min %d [%.2f, %.2f, %.2f]"), GroundKey().iGroundNo, kMin.x, kMin.y, kMin.z);
				//				INFO_LOG( BM::LOG_LV1, _T("Max %d [%.2f, %.2f, %.2f]"), GroundKey().iGroundNo, kMax.x, kMax.y, kMax.z);
			}
		}
		else if(NiIsKindOf(NiNode, pkChild))
		{
			FindMinMax((NiNode *)pkChild, kMin, kMax);
		}
	}
}

void PgGroundResource::SetScaleToPhysXObject(NiPhysXSceneDesc *pkDesc, float fScale)
{
	int iActorTotal = pkDesc->GetActorCount();
	for (int iActorCount =0 ; iActorCount <iActorTotal ; iActorCount++)
	{
		NiPhysXActorDesc *pkActorDesc = pkDesc->GetActorAt(iActorCount);
		
		if (!pkActorDesc)
		{
			continue;
		}

		// ��Ȯ�� ��ġ ����� ���� Matrix.
		NxMat34 kMultMat, kMultMatInv;
		kMultMat = pkActorDesc->GetPose(0);
		kMultMat.getInverse(kMultMatInv);

		int iShapeDescTotal = pkActorDesc->GetActorShapes().GetSize();
		for (int iShapeDescCount = 0 ;
			iShapeDescCount < iShapeDescTotal ;
			iShapeDescCount++)
		{
			NiPhysXShapeDesc *pkShapeDesc =
				pkActorDesc->GetActorShapes().GetAt(iShapeDescCount);

			if (pkShapeDesc && pkShapeDesc->GetMeshDesc() && pkShapeDesc->GetType() == NX_SHAPE_MESH)
			{
				NxTriangleMeshShapeDesc kTriMeshShapeDesc;
				pkShapeDesc->GetMeshDesc()->ToTriMeshDesc(kTriMeshShapeDesc, true);

				NxTriangleMesh *pkTriMesh = kTriMeshShapeDesc.meshData;
				if(!pkTriMesh)
				{
					continue;
				}

				NxTriangleMeshDesc kTriMeshDesc;
				pkTriMesh->saveToDesc(kTriMeshDesc);

				unsigned short usNumNxVerts = kTriMeshDesc.numVertices;
				unsigned short usMaxNumTris = kTriMeshDesc.numTriangles;
				unsigned short pointStrideBytes = kTriMeshDesc.pointStrideBytes;
				unsigned short triangleStrideBytes = kTriMeshDesc.triangleStrideBytes;
				NxVec3* pkNxVerts = (NxVec3*)kTriMeshDesc.points;
				NxU32* pkNxTriData = (NxU32*)kTriMeshDesc.triangles;
				NxVec3* pkNxCloneVerts = NiAlloc(NxVec3, usNumNxVerts);

				for (unsigned short us = 0; us < usNumNxVerts; us++)
				{
					NxVec3 kVec = kMultMat * pkNxVerts[us];
					pkNxCloneVerts[us] = kMultMatInv * (kVec * fScale);
				}
				kTriMeshDesc.points = pkNxCloneVerts;

				// ReCook Triangle
				NxInitCooking();
				NiPhysXMemStream kMemStream;
				NIVERIFY(NxCookTriangleMesh(kTriMeshDesc, kMemStream));
				NxCloseCooking();
				kMemStream.Reset();
				pkShapeDesc->GetMeshDesc()->SetData(kMemStream.GetSize(), (unsigned char *)kMemStream.GetBuffer());
				NiFree(pkNxCloneVerts);
			}
		}
	}

}

HRESULT PgGroundResource::GetAvailEntity(const NiEntityInterface *pkEntity) const
{
	if(pkEntity->GetPGProperty().Equals("BaseObject"))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	if(pkEntity->GetPGProperty().Equals("PhysX"))
	{
		return S_OK;
	}
	if(pkEntity->GetPGProperty().Equals("CharacterSpawn"))
	{
		return S_OK;
	}
	if(pkEntity->GetPGProperty().Equals("Trigger"))
	{
		return S_OK;
	}
	if(pkEntity->GetPGProperty().Equals("Path"))
	{
		return S_OK;
	}
	if (pkEntity->GetPGUsePhysX())
	{
		return S_OK;
	}
	if (pkEntity->GetHidden())
	{
		if(pkEntity->GetName().Contains("telejump_zone_"))
		{
			return S_OK;
		}
		else if(pkEntity->GetName().Contains("teleport_zone_"))
		{
			return S_OK;
		}
	}
	if(pkEntity->GetPGProperty().Equals("Telejump") || pkEntity->GetPGProperty().Equals("telejump"))
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

int PgGroundResource::SearchNode( NiNode* pkTargetNode, const NiFixedString &kName, bool bRecursive )
{
	int iCount = 0;
	int iChildTotal = pkTargetNode->GetChildCount();

	for (int i=0 ; i<iChildTotal ; i++)
	{
		NiAVObject* pkObject = pkTargetNode->GetAt(i);
		if (!pkObject)
			continue;

		if (bRecursive && NiIsKindOf(NiNode, pkObject))
		{
			NiNode* pkRecursiveNode = NiDynamicCast(NiNode, pkObject);
			iCount += SearchNode(pkRecursiveNode, kName, bRecursive);
		}

		if (pkObject->GetName().Equals(kName))
		{
			iCount += 1;
		}
	}

	return iCount;
}

bool PgGroundResource::IsCorrectPos(POINT3 const &rkPos)const
{
	NxRay kRay(NxVec3(rkPos.x, rkPos.y, rkPos.z + 25.0f), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	const NxScene *pkScene = PhysXScene()->GetPhysXScene();
	if(pkScene)
	{
		NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit);
		if(pkHitShape)
		{
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGroundResource::FindCorrectPos( POINT3 &rkPos, float const fZBuffer )const
{
	NxRay kRay(NxVec3(rkPos.x, rkPos.y, rkPos.z + 25.0f), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	const NxScene *pkScene = PhysXScene()->GetPhysXScene();
	if(pkScene)
	{
		NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit);
		if(pkHitShape)
		{
			rkPos.z = kHit.worldImpact.z + fZBuffer;
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

POINT3 PgGroundResource::GetNodePosition(char const *pcNodeName) const
{
	NiAVObject *pkFindNode = m_spSceneRoot->GetObjectByName(pcNodeName);
	if(!pkFindNode)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFindNode is NULL"));
		return POINT3(0, 0, 0);
	}

	NiPoint3 kPos = pkFindNode->GetTranslate();
	return POINT3(kPos.x, kPos.y, kPos.z);
}

bool PgGroundResource::AddNPC( TiXmlAttribute const *pkAttribute )
{
	char const *pcName = NULL;
	char const *pcActor = NULL;
	char const *pcScript = NULL;
	char const *pcLocation = NULL;
	ENpcType kNpcType = E_NPC_TYPE_FIXED;
	int iID = 0;
	BM::GUID kNpcGuid;
	int iParam = 0;
	int GiveEffectNo = 0;

	while ( pkAttribute )
	{
		if ( !::strcmp( pkAttribute->Name(), "NAME") )
		{
			pcName = pkAttribute->Value();
		}
		else if ( !::strcmp( pkAttribute->Name(), "ACTOR") )
		{
			pcActor = pkAttribute->Value();
		}
		else if ( !::strcmp( pkAttribute->Name(), "SCRIPT") )
		{
			pcScript = pkAttribute->Value();
		}
		else if ( !::strcmp( pkAttribute->Name(), "LOCATION") )
		{
			pcLocation = pkAttribute->Value();
		}
		else if ( !::strcmp( pkAttribute->Name(), "GUID") )
		{
			kNpcGuid.Set( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "KID") )
		{
			iID = ::atoi( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "TYPE") )
		{
			if (_stricmp( pkAttribute->Value(), "FIXED"))
			{
				kNpcType = E_NPC_TYPE_FIXED;
			}
			else if (_stricmp( pkAttribute->Value(), "DYNAMIC"))
			{
				kNpcType = E_NPC_TYPE_DYNAMIC;
			}
			else if (_stricmp( pkAttribute->Value(), "CREATURE"))
			{
				kNpcType = E_NPC_TYPE_CREATURE;
			}
		}
		else if ( !::strcmp( pkAttribute->Name(), "PARAM") )
		{
			iParam = ::atoi( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "GIVE_EFFECTNO") )
		{
			GiveEffectNo = ::atoi( pkAttribute->Value() );
		}

		pkAttribute = pkAttribute->Next();
	}

	PgNpc * pkNpc = AddNPC( pcName, pcActor, pcScript, pcLocation, kNpcGuid, iID, GiveEffectNo );
	if ( pkNpc )
	{
		if ( iParam )
		{
			pkNpc->SetAbil( AT_CUSTOMDATA1, iParam );
		}

		return true;
	}

	return false;
}

PgNpc* PgGroundResource::AddNPC( char const *pcName, char const *pcActor, char const *pcScript, char const *pcLocation, BM::GUID const &rkGuid, int iID, int GiveEffectNo, ENpcType const kNpcType )
{
	if( BM::GUID::IsNull(rkGuid) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"NPC IS NULL GUID MapNo["<<GroundKey().GroundNo()<<L"] Name = "<<UNI(pcName)<<L", GUID= "<<rkGuid);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::vstring vStrNameNo(UNI(pcName));
	int const iNameNo = (int)vStrNameNo;

	wchar_t const *pText = NULL;
	if(!GetDefString(iNameNo, pText))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Can't Get NpcName NameNo["<<iNameNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	POINT3 ptLoc;
	::sscanf_s(pcLocation, "%f,%f,%f", &ptLoc.x, &ptLoc.y, &ptLoc.z);
	INFO_LOG(BM::LOG_LV7, __FL__<<L"MapNo = ["<<GroundKey().GroundNo()<<L"] Name = ["<<UNI(pcName)<<L"] GUID["<<rkGuid<<L"] xyz("<<ptLoc<<L")");
	ptLoc.z += 50;	// �ٴڿ� ������ �ʵ��� ���� �÷�����

	CUnit *pkNpcUnit = g_kTotalObjMgr.CreateUnit(UT_NPC, rkGuid);
	PgNpc *pkNpc = dynamic_cast<PgNpc*>(pkNpcUnit);

	if( pkNpc)
	{
		pkNpc->LastAreaIndex(PgSmallArea::NONE_AREA_INDEX);

		pkNpc->Create( rkGuid, pText, UNI(pcActor), UNI(pcScript), ptLoc, iID, GiveEffectNo, kNpcType);
		g_kQuestMan.BuildNpc(pkNpc->GetID());

		auto kPair = m_kNpcCont.insert(std::make_pair(rkGuid, pkNpc));
		if ( true == kPair.second )
		{
			return pkNpc;
		}
		else
		{
			g_kTotalObjMgr.ReleaseUnit( pkNpcUnit );
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Critical Error: NPC<" << rkGuid << L"> is Overlapping GUID(can't insert npc)");
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Critical Error: NPC<" << rkGuid << L"> is NULL(can't create npc)");
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

CUnit* PgGroundResource::GetNPC(BM::GUID const &rkGuid)const
{
	CONT_OBJECT_MGR_UNIT::const_iterator npc_itr = m_kNpcCont.find(rkGuid);
	if( m_kNpcCont.end() != npc_itr )
	{
		if ( npc_itr->second->GetAbil( AT_HP ) <= 0 )
		{
			// HP�� ������ ���� NPC�̴�.
			return NULL;
		}
		return npc_itr->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgGroundResource::AddStone( TiXmlAttribute const *pkAttribute )
{
	int iRate = 0;
	int iItemBagNo = 0;
	SStoneRegenPoint kStoneRegenPoint;
	while ( pkAttribute )
	{
		if ( !::strcmp( pkAttribute->Name(), "RATE") )
		{
			iRate = ::atoi( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "ITEMBAGNO") )
		{
			iItemBagNo = ::atoi( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "ID") )
		{
			kStoneRegenPoint.iID = ::atoi( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "POSX") )
		{
			kStoneRegenPoint.kPos.x = (float)::atof( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "POSY") )
		{
			kStoneRegenPoint.kPos.y = (float)::atof( pkAttribute->Value() );
		}
		else if ( !::strcmp( pkAttribute->Name(), "POSZ") )
		{
			kStoneRegenPoint.kPos.z = (float)::atof( pkAttribute->Value() );
		}
		pkAttribute = pkAttribute->Next();
	}
	AddStoneRegenPoint( kStoneRegenPoint );
}

void PgGroundResource::AddStoneRegenPoint(SStoneRegenPoint const &kStoneRegenPoint)
{
	m_kStoneCtrl.AddData(kStoneRegenPoint);
}


void PgGroundResource::RebuildQuest()
{
	CONT_OBJECT_MGR_UNIT::iterator npc_iter = m_kNpcCont.begin();
	while(m_kNpcCont.end() != npc_iter)
	{
		CONT_OBJECT_MGR_UNIT::mapped_type pkElement = (*npc_iter).second;
		if( pkElement )
		{
			g_kQuestMan.BuildNpc(pkElement->GetID());
		}
		++npc_iter;
	}
}

bool PgGroundResource::LoadHill( size_t const iHillNo, char const *szValue )
{
	if ( m_spTriggerRoot )
	{
		NiAVObject const *pkObj = m_spTriggerRoot->GetObjectByName( szValue );
		if ( pkObj )
		{
			m_kContPvPHill.push_back( CONT_PVPHILL::value_type(iHillNo,pkObj) );
			INFO_LOG(BM::LOG_LV7, __FL__<<L"[GroundNo:"<<GroundKey().GroundNo()<<L"] PvP Hill ["<<UNI(szValue)<<L"]");
			return true;
		}

		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"[GroundNo:"<<GroundKey().GroundNo()<<L"] Not Found <HILL NAME=\""<<UNI(szValue)<<L"\"" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkObj is NULL"));
		return false;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"[GroundNo:"<<GroundKey().GroundNo()<<L"] TriggerRoot is NULL " );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgGroundResource::CopyContGTrigger( CONT_GTRIGGER const& rkFrom )
{
	PgGroundResourceUtil::ClearContGTrigger(m_kContTrigger);

	CONT_GTRIGGER::const_iterator iter = rkFrom.begin();
	while( rkFrom.end() != iter )
	{
		CONT_GTRIGGER::mapped_type const& pkElement = (*iter).second;
		if( pkElement )
		{
			CONT_GTRIGGER::mapped_type pkNewTrigger = pkElement->Clone();
			if( pkNewTrigger )
			{
				auto kRet = m_kContTrigger.insert( std::make_pair((*iter).first, pkNewTrigger) );
				if( !kRet.second )
				{
					SAFE_DELETE(pkNewTrigger);
					CAUTION_LOG( BM::LOG_LV1, __FL__<<C2L(GroundKey())<<L"Can't copy trigger, Duplicate ID["<<(*iter).first );
				}
			}
		}
		++iter;
	}
}


void PgGroundResource::UpdateGTrigger()
{
	if ( m_kContTrigger.empty() )
	{
		return;
	}

	SET_GUID kContEmporiaPortalIDList;

	CONT_GTRIGGER::const_iterator tri_itr = m_kContTrigger.begin();
	for ( ; tri_itr != m_kContTrigger.end() ; ++tri_itr )
	{
		switch ( tri_itr->second->GetType() )
		{
		case GTRIGGER_TYPE_PORTAL_EMPORIA:
			{
				PgGTrigger_PortalEmporia const * pkGTrigger_PortalEmporia = dynamic_cast<PgGTrigger_PortalEmporia const*>(tri_itr->second);
				if ( pkGTrigger_PortalEmporia )
				{
					auto kPair = m_kContEmporiaPortal.insert( std::make_pair( pkGTrigger_PortalEmporia->GetEmporiaKey(),PgPortal_Emporia( pkGTrigger_PortalEmporia ) ) );
					if ( true == kPair.second )
					{
						kContEmporiaPortalIDList.insert( pkGTrigger_PortalEmporia->GetEmporiaKey().kID );
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Casting Error GTRIGGER_TYPE_PORTAL_EMPORIA");
				}

			}break;
		default:
			{
			}break;
		}
	}

	if ( kContEmporiaPortalIDList.size() > 0 )
	{
		BM::Stream kPacket( PT_M_N_REQ_EMPORIA_PORTAL_INFO, g_kProcessCfg.ChannelNo() );
		GroundKey().WriteToPacket( kPacket );
		PU::TWriteArray_A( kPacket, kContEmporiaPortalIDList );
		::SendToRealmContents( PMET_EMPORIA, kPacket );
	}
}

bool PgGroundResource::AddGTrigger( TiXmlElement const *pkElement )
{
	if( !pkElement )
	{
		return false;
	}

	NiNode *pkTriggerRoot = GetTriggerRoot();
	if ( !pkTriggerRoot )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_GTRIGGER::key_type kID;
	bool bInitEnable = true;
	PgGroundTrigger *pkGTrigger = NULL;
	int checkEffectNo = 0;
	int errorEffectMsgNo = 0;

	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;

	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if( 0 == ::strcmp(pcAttrName, "ID") )
		{
			kID = pcAttrValue;
		}
		else if( 0 == ::strcmp(pcAttrName, "ENABLE") )
		{
			bInitEnable = PgStringUtil::SafeAtob(pcAttrValue, true);
		}
		else if( 0 == ::strcmp(pcAttrName, "TYPE") )
		{
			if ( !pkGTrigger )
			{
				if ( !::strcmp( pcAttrValue, "PORTAL") )
				{
					PgGTrigger_Portal *pkTemp = new_tr PgGTrigger_Portal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "PARTY_PORTAL") )
				{
					PgGTrigger_PartyPortal *pkTemp = new_tr PgGTrigger_PartyPortal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "PARTY_MEMBER_PORTAL") )
				{
					PgGTrigger_PartyMemberPortal *pkTemp = new_tr PgGTrigger_PartyMemberPortal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "RAG_PARTY_MEMBER_PORTAL") )
				{
					PgGTrigger_RagPartyMemberPortal *pkTemp = new_tr PgGTrigger_RagPartyMemberPortal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "MISSION") )
				{
					if ( GetAttr() & GATTR_FLAG_MISSION )
					{
						PgGTrigger_InMission *pkTemp = new_tr PgGTrigger_InMission;
						pkGTrigger = dynamic_cast<PgGTrigger_InMission*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
					else
					{
						PgGTrigger_Mission *pkTemp = new_tr PgGTrigger_Mission;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}	
				}
				else if ( !::strcmp( pcAttrValue, "MISSION_NPC") )
				{
					if ( GetAttr() & GATTR_FLAG_MISSION )
					{
						PgGTrigger_InMission *pkTemp = new_tr PgGTrigger_InMission;
						pkGTrigger = dynamic_cast<PgGTrigger_InMission*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
					else
					{
						PgGTrigger_Mission_Npc *pkTemp = new_tr PgGTrigger_Mission_Npc;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}	
				}
				else if ( !::strcmp( pcAttrValue, "MISSION_EVENT_NPC") )
				{
					if ( GetAttr() & GATTR_FLAG_MISSION )
					{
						PgGTrigger_InMission *pkTemp = new_tr PgGTrigger_InMission;
						pkGTrigger = dynamic_cast<PgGTrigger_InMission*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
					else
					{
						PgGTrigger_Mission_Event_Npc *pkTemp = new_tr PgGTrigger_Mission_Event_Npc;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}	
				}
				else if ( !::strcmp( pcAttrValue, "MISSION_EASY") )
				{
					if ( GetAttr() & GATTR_FLAG_MISSION )
					{
						PgGTrigger_InMission *pkTemp = new_tr PgGTrigger_InMission;
						pkGTrigger = dynamic_cast<PgGTrigger_InMission*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
					else
					{
						PgGTrigger_Mission_Easy *pkTemp = new_tr PgGTrigger_Mission_Easy;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}	
				}
				else if ( !::strcmp( pcAttrValue, "SUPER_GROUND") )
				{
					if( GetAttr() & GATTR_FLAG_SUPER )
					{
						PgGTrigger_InSuperGround *pkTemp = new_tr PgGTrigger_InSuperGround;
						pkGTrigger = dynamic_cast< PgGroundTrigger* >(pkTemp);
						if( !pkGTrigger )
						{
							SAFE_DELETE(pkTemp);
						}
					}
					else
					{
						PgGTrigger_SuperGround *pkTemp = new_tr PgGTrigger_SuperGround;
						pkGTrigger = dynamic_cast< PgGroundTrigger* >(pkTemp);
						if( !pkGTrigger )
						{
							SAFE_DELETE(pkTemp);
						}
					}
				}
				else if ( !::strcmp( pcAttrValue, "HIDDEN_PORTAL") )
				{
					PgGTrigger_Hidden_Portal *pkTemp = new_tr PgGTrigger_Hidden_Portal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if( !::strcmp( pcAttrValue, "TELEJUMP") || !::strcmp( pcAttrValue, "TELEPORT") )
				{
					PgGTrigger_TeleMove *pkTemp = new_tr PgGTrigger_TeleMove;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "BATTLEAREA") )
				{
					PgGTrigger_BattleArea *pkTemp = new_tr PgGTrigger_BattleArea;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "EMPORIA") )
				{
					PgGTrigger_PortalEmporia *pkTemp = new_tr PgGTrigger_PortalEmporia;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "TRANSTOWER") )
				{
					PgGTrigger_TransTower *pkTemp = new_tr PgGTrigger_TransTower;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "DOUBLE_UP") )
				{
					PgGTrigger_Double_Up *pkTemp = new_tr PgGTrigger_Double_Up;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "JOB_SKILL") )
				{
					PgGTrigger_JobSkill *pkTemp = new_tr PgGTrigger_JobSkill;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
				else if( !::strcmp( pcAttrValue, "CHANNEL_PORTAL") )
				{
					PgGTrigger_ChannelPortal *pTemp = new_tr PgGTrigger_ChannelPortal;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pTemp);
					if( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pTemp );
					}
				}
				else if( !::strcmp( pcAttrValue, "KING_OF_HILL") )
				{
					PgGTrigger_KingOfHill *pTemp = new_tr PgGTrigger_KingOfHill;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pTemp);
					if( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pTemp );
					}
				}
				else if( !::strcmp(pcAttrValue, "SCORE") )
				{
					PgGTrigger_Score * pTemp = new_tr PgGTrigger_Score;
					pkGTrigger = dynamic_cast<PgGroundTrigger *>(pTemp);
					if( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pTemp );
					}
				}
				else if( !::strcmp(pcAttrValue, "LOVE_FENCE") )
				{
					PgGTrigger_LoveFence * pTemp = new_tr PgGTrigger_LoveFence;
					pkGTrigger = dynamic_cast<PgGroundTrigger *>(pTemp);
					if( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pTemp );
					}
				}
				else if ( !::strcmp( pcAttrValue, "MISSION_EVENT_NOT_HAVE_ARCADE") )
				{
					if ( GetAttr() & GATTR_FLAG_MISSION )
					{
						PgGTrigger_InMission *pkTemp = new_tr PgGTrigger_InMission;
						pkGTrigger = dynamic_cast<PgGTrigger_InMission*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
					else
					{
						PgGTrigger_Mission_Event_Not_Have_Arcade *pkTemp = new_tr PgGTrigger_Mission_Event_Not_Have_Arcade;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}	
				}
				else if(0 == ::strcmp(pcAttrValue, "RACE_CHECK_POINT"))
				{
					//if( GetAttr() & GATTR_FLAG_�޸����̺�Ʈ )
					{
						PgGTrigger_CheckPoint *pkTemp = new_tr PgGTrigger_CheckPoint;
						pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
						if ( !pkGTrigger )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
							SAFE_DELETE( pkTemp );
						}
					}
				}
				if ( !::strcmp( pcAttrValue, "MISSION_UNUSED_GADACOIN") )
				{
					PgGTrigger_Mission_Unused_GadaCoin *pkTemp = new_tr PgGTrigger_Mission_Unused_GadaCoin;
					pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
					if ( !pkGTrigger )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
						SAFE_DELETE( pkTemp );
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"TYPE= only one plz" );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
			}
		}
		else if( 0 == ::strcmp(pcAttrName, "VALUE") )
		{
			PgGTrigger_CheckPoint* pkCheckPoint = dynamic_cast<PgGTrigger_CheckPoint*>(pkGTrigger);
			if(NULL != pkCheckPoint)
			{
				pkCheckPoint->SetProgressNo( PgStringUtil::SafeAtoi(pcAttrValue) );
			}
		}
		else if( 0 == ::strcmp(pcAttrName, "CHECK_EFFECT") )
		{
			checkEffectNo = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		else if( 0 == ::strcmp(pcAttrName, "ERROR_EFFECT_MSG") )
		{
			errorEffectMsgNo = PgStringUtil::SafeAtoi(pcAttrValue);
		}
		pkAttribute = pkAttribute->Next();
	}

	if( !kID.empty()
	&&	!pkGTrigger )
	{
		PgGroundTrigger *pkTemp = new_tr PgGTrigger_Normal;
		pkGTrigger = dynamic_cast<PgGroundTrigger*>(pkTemp);
		if ( !pkGTrigger )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGTrigger is NULL"));
			SAFE_DELETE( pkTemp );
			return false;
		}
	}

	bool bInsertComplete = false;
	if ( pkGTrigger )
	{
		pkGTrigger->Enable( bInitEnable );
		pkGTrigger->CheckEffectNo( checkEffectNo );
		pkGTrigger->ErrorEffectMsgNo( errorEffectMsgNo );

		pkElement = pkElement->FirstChildElement();
		while ( pkElement )
		{
			if ( !bInsertComplete && !::strcmp( pkElement->Value(), "ACTION") )
			{
				if ( pkGTrigger->Build( kID, pkTriggerRoot, pkElement ) )
				{
					if(pkGTrigger->GetType() == GTRIGGER_TYPE_CHECK_POINT) //�޸��� �̺�Ʈ�� Ʈ���Ŷ��
					{
						m_kContCheckPointOrder.push_back(kID); //Ʈ���� ������� Ű ���� ���� �����̳ʿ� ����
					}

					auto kPair = m_kContTrigger.insert( std::make_pair( kID, pkGTrigger ) );
					if ( kPair.second )
					{
						bInsertComplete = true;
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Overap GTriggerID["<<kID<<L"] GroundNo["<<GroundKey().GroundNo()<<L"]" );
					}
				}
				else
				{
					if(!GetSceneRoot()->GetObjectByName( kID.c_str() ))	//Ʈ������ ��� ProjectG Property�� Trap�̶� Ʈ���Ÿ�Ͽ����� ã�� �� �� ����.
					{
						INFO_LOG( BM::LOG_LV0, __FUNCTIONW__<<" Not Found Object["<<kID<<"] MapNo : "<<GroundKey().GroundNo() );
					}
				}
			}
			else if ( !::strcmp( pkElement->Value(), "SIMPLE_TIME_EVENT") )
			{
				CONT_GTRIGGER::iterator iterTrigger = m_kContTrigger.find(kID);
				if(m_kContTrigger.end() != iterTrigger)
				{
					iterTrigger->second->AddTimeEvent(pkElement);
				}
				else
				{
					//���������ּ��� 
				}
			}
			pkElement = pkElement->NextSiblingElement();
		}
	}

	if(!bInsertComplete)
	{
		SAFE_DELETE( pkGTrigger );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	}
	return bInsertComplete;
}

HRESULT PgGroundResource::BuildAbil()
{
	CONT_DEFMAP const * pkDefMap = NULL;
	CONT_DEFMAP_ABIL const * pkDefMapAbil = NULL;
	g_kTblDataMgr.GetContDef(pkDefMap);
	g_kTblDataMgr.GetContDef(pkDefMapAbil);


	CONT_DEFMAP::const_iterator itor_map = pkDefMap->find(GroundKey().GroundNo());
	if (itor_map == pkDefMap->end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot find Map GroundNo=") << GroundKey().GroundNo());
		return E_FAIL;
	}
	for (int i=0; i<MAX_MAP_ABIL_COUNT; i++)
	{
		int const iAbil = (*itor_map).second.iAbil[i];
		if (iAbil != 0)
		{
			CONT_DEFMAP_ABIL::const_iterator itor_abil = pkDefMapAbil->find(iAbil);
			if (itor_abil == pkDefMapAbil->end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot find MapAbil GroundNo=") << GroundKey().GroundNo() << _T(", Abil=") << iAbil);
				return E_FAIL;
			}
			for (int j=0; j<MAX_MAP_ABIL; j++)
			{
				if ((*itor_abil).second.iType[j] != 0)
				{
					m_kResourceAbil.insert(std::make_pair((*itor_abil).second.iType[j], (*itor_abil).second.iValue[j]));
				}
			}
		}
	}

	m_bPartyBreakIn = (0 < (*itor_map).second.byPartyBreakIn);

	return S_OK;
}

bool PgGroundResource::AddEventScriptStopAI(TiXmlElement const *pkElement)
{
	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if ( !::strcmp( pcAttrName, "NO") && pcAttrValue )
		{
			VEC_STRING kNoVec;
			PgStringUtil::BreakSep(pcAttrValue, kNoVec, ",");

			VEC_STRING::const_iterator iter = kNoVec.begin();
			while( iter != kNoVec.end() )
			{
				int const iNo = PgStringUtil::SafeAtoi(*iter);
				m_kContEventScriptStopAI.insert(iNo);

				++iter;
			}
		}

		pkAttribute = pkAttribute->Next();
	}
	return true;
}

bool PgGroundResource::IsCheckEventScriptStopAI(int const iEventScriptNo)const
{
	return m_kContEventScriptStopAI.end() != m_kContEventScriptStopAI.find(iEventScriptNo);
}

bool PgGroundResource::AddEventScriptNoLock(TiXmlElement const *pkElement)
{
	TiXmlAttribute const *pkAttribute = pkElement->FirstAttribute();
	char const *pcAttrName = NULL;
	char const *pcAttrValue = NULL;
	while ( pkAttribute )
	{
		pcAttrName = pkAttribute->Name();
		pcAttrValue = pkAttribute->Value();

		if ( !::strcmp( pcAttrName, "NO") && pcAttrValue )
		{
			VEC_STRING kNoVec;
			PgStringUtil::BreakSep(pcAttrValue, kNoVec, ",");

			VEC_STRING::const_iterator iter = kNoVec.begin();
			while( iter != kNoVec.end() )
			{
				int const iNo = PgStringUtil::SafeAtoi(*iter);
				m_kContEventScriptNoLock.insert(iNo);

				++iter;
			}
		}

		pkAttribute = pkAttribute->Next();
	}
	return true;
}

bool PgGroundResource::IsCheckEventScriptNoLock(int const iEventScriptNo)const
{
	return m_kContEventScriptNoLock.end() != m_kContEventScriptNoLock.find(iEventScriptNo);
}

int PgGroundResource::GetRscAbil(WORD const wType)
{
	DYN_ABIL::const_iterator itor_abil = m_kResourceAbil.find(wType);
	if (itor_abil != m_kResourceAbil.end())
	{
		return (*itor_abil).second;
	}
	return 0;
}

void PgGroundResource::SetResultSpawnLoc( POINT3 const &pt3Pos )
{
	if ( !m_pkPT3ResultSpawnLoc )
	{
		m_pkPT3ResultSpawnLoc = new_tr POINT3;
	}

	if ( m_pkPT3ResultSpawnLoc )
	{
		*m_pkPT3ResultSpawnLoc = pt3Pos;
	}
}
