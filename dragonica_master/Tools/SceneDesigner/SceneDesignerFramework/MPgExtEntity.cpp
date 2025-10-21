//
// HandOver, 강정욱 2008.01.29
//
// 맵툴상에서 특수한 Object에 대한 처리(패스, 몬스터, NPC 등)
// 실제론 맵에 찍혀서 게임에 보여지는 물체가 아닌. 그외 특수 목적으로 만든 Object.
//
// 맵상에 Object를 새로 찍거나, 제거 했을때, 움직이거나 돌리거나 스케일을 바꾸었을 때.
// *ExtEntity() 로 들어와서 특수한 처리를 해준다.
//
// 특수한 처리를 하게 되면 그 물체가 m_pmWait*Entities에 추가 되는데...
// 위의 Wait관련 Entities를 가지고 MainForm.cs의 Process_*ExtEntity() 에 보면..
// UI에서 바뀌어야 할 경우를 처리 해주고 있다.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "IEntityPathService.h"
#include "MpgExtEntity.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgExtEntity::MPgExtEntity(void)
{
	m_bHaveWaitAddEntity = false;
	m_bHaveWaitDeleteEntity = false;
	m_bHaveWaitTranslateEntity = false;
	m_bHaveWaitSelectEntity = false;
	
	m_pmWaitAddEntities = new ArrayList();
	m_pmWaitDeleteEntities = new ArrayList();
	m_pmWaitTranslateEntities = new ArrayList();
	m_pmWaitSelectEntities = new ArrayList();
}

MPgExtEntity::~MPgExtEntity(void)
{
}

bool MPgExtEntity::DeleteExtEntity(MEntity* pkEntity)
{
	eExtEntityType eType = IsExtEntityType(pkEntity);
	int iBagNo = 0;

	switch(eType)
	{
	case ExtEntityType_Monster:
		MFramework::Instance->Monster->DeleteSettedMonsterData(pkEntity->TemplateID);
		MFramework::Instance->Monster->DeletedMonsterData = true;
		return true;
	case ExtEntityType_Npc:
		MFramework::Instance->Npc->InsertDeletedNpcData(pkEntity->Name);
		MFramework::Instance->Npc->IsDeletedNpcData = true;
		return true;
	case ExtEntityType_WayPoint:
		MFramework::Instance->WayPoint->DeleteWayPoint(pkEntity);
		return true;
	case ExtEntityType_Path:
		MFramework::Instance->Path->DeletePath(pkEntity);
		return true;
	case ExtEntityType_ShineStone:
		m_bHaveWaitDeleteEntity = true;
		m_pmWaitDeleteEntities->Add(pkEntity);
		m_eWaitType = eType;
		return true;
	case ExtEntityType_DecalTarget:
		return true;
	case ExtEntityType_DecalData:
		return true;
	case ExtEntityType_BreakObject:
		//MFramework::Instance->BreakObject->RemoveGroupData(pkEntity);
		iBagNo = MFramework::Instance->BreakObject->DeleteSettedBreakObjectData(pkEntity->TemplateID);
		MFramework::Instance->BreakObject->RemoveObjectBagData(iBagNo);
		MFramework::Instance->BreakObject->set_DeleteGroup(true);
		return true;

	case ExtEntityType_ElevatorTarget:
		MFramework::Instance->Elevator->DeletePointEntity(pkEntity);
		MFramework::Instance->Elevator->SetSyncData(true);
		return true;
	}
	return false;
}

// Return이 true 이면 SceneRoot에 entity 를 추가하지 않는다.
bool MPgExtEntity::AddExtEntity(MEntity* pkOrgEntity)
{
    MEntity* pmNewProxy;
    if (pmNewProxy != NULL)
    {
        MFramework::Instance->EntityFactory->Remove(
            pmNewProxy->GetNiEntityInterface());
        pmNewProxy = NULL;
    }
	MScene* pmScene = MFramework::Instance->Scene;

	eExtEntityType eType = IsExtEntityType(pkOrgEntity);

	if (eType == MPgExtEntity::ExtEntityType_Monster)
	{
		SettedMonsterData *pkData = MFramework::Instance->Monster->m_pkWaitData;
		if (!pkData)
		{
			return false;
		}

		IEntityPathService* pmPathService = MGetService(IEntityPathService);
		String* strMonName = "Mon_";
		strMonName = strMonName->Concat(strMonName, pkData->m_strMemo);
		String* strCloneName = pmScene->GetUniqueEntityName(String::Concat(pmPathService->GetSimpleName(strMonName), " 01"));

		Guid kGuid = Guid::NewGuid();
		pkOrgEntity = pkOrgEntity->Clone(strCloneName, false);
		pkOrgEntity->TemplateID = kGuid;
		pkOrgEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
			MFramework::Instance->ExternalAssetManager);

		pmNewProxy = MFramework::Instance->ProxyManager->CreateProxy(pkOrgEntity);

		NiMatrix3 kRotMat;
		kRotMat.MakeIdentity();
		kRotMat.MakeXRotation(0);
		kRotMat.MakeYRotation(0);
		kRotMat.MakeZRotation(0);
		pkOrgEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRotMat);

		pmScene->AddEntity(pkOrgEntity, false);

		NiPoint3 kPoint;
		pkOrgEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
		MFramework::Instance->Monster->CompleateStackedMonsterData(kGuid, kPoint.x, kPoint.y, kPoint.z, pkOrgEntity);

		return true;
	}
	else if (eType == MPgExtEntity::ExtEntityType_Path)
	{
		MFramework::Instance->Path->InsertPath(pkOrgEntity);
		pmScene->AddEntity(pkOrgEntity, false);
		return true;
	}
	else if (eType == MPgExtEntity::ExtEntityType_Npc)
	{
		return true;
	}
	else if (eType == MPgExtEntity::ExtEntityType_WayPoint)
	{
		Guid kGuid = Guid::NewGuid();
		pkOrgEntity = pkOrgEntity->Clone(kGuid.ToString(), false);
		pkOrgEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
			MFramework::Instance->ExternalAssetManager);
		MFramework::Instance->LightManager->AddEntityToDefaultLights(
			pkOrgEntity);
		pmNewProxy = MFramework::Instance->ProxyManager->CreateProxy(
			pkOrgEntity);

		NiMatrix3 kRotMat;
		kRotMat.MakeIdentity();
		kRotMat.MakeXRotation(0);
		kRotMat.MakeYRotation(0);
		kRotMat.MakeZRotation(0);
		pkOrgEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRotMat);
		pmScene->AddEntity(pkOrgEntity, false);

		// add
		NiPoint3 kPoint;
		pkOrgEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
		MFramework::Instance->WayPoint->AddWayPoint(pkOrgEntity, pkOrgEntity->Name, -1, 0, kPoint.x, kPoint.y, kPoint.z);

		return true;
	}
	//else if (eType == MPgExtEntity::ExtEntityType_MonAreaTarget)
	//{
	//	if (MFramework::Instance->MonArea->AddMonAreaTarget(pkOrgEntity))
	//	{
	//		pmScene->AddEntity(pkOrgEntity, false);
	//	}

	//	return true;
	//}
	else if (eType == ExtEntityType_ShineStone)
	{
		m_bHaveWaitAddEntity = true;
		m_pmWaitAddEntities->Add(pkOrgEntity);
		m_eWaitType = eType;

		return true;
	}
	else if (eType == ExtEntityType_DecalTarget)
	{
		Guid kGuid = Guid::NewGuid();
		String* strName = "decal_target_";
		strName = strName->Concat(strName, kGuid.ToString());
		pkOrgEntity = pkOrgEntity->Clone(strName, false);
		pkOrgEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
			MFramework::Instance->ExternalAssetManager);
		MFramework::Instance->LightManager->AddEntityToDefaultLights(
			pkOrgEntity);
		pmNewProxy = MFramework::Instance->ProxyManager->CreateProxy(
			pkOrgEntity);

		NiMatrix3 kRotMat;
		kRotMat.MakeIdentity();
		kRotMat.MakeXRotation(0);
		kRotMat.MakeYRotation(0);
		kRotMat.MakeZRotation(0);
		pkOrgEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRotMat);
		pmScene->AddEntity(pkOrgEntity, false);

		return true;
	}
	else if(eType == ExtEntityType_DecalData)
	{
		return true;
	}
	else if(eType == ExtEntityType_BreakObject)
	{
		MEntity* pEntity = MFramework::Instance->BreakObject->AddBagListEntity(pkOrgEntity);
		if(pEntity)
		{
			MFramework::Instance->BreakObject->AddBagListFromMakingData();
			MFramework::Instance->BreakObject->set_AddGroup(true);
		}

		return true;
		//if(MFramework::Instance->BreakObject->AddGroupDataEntity(pkOrgEntity))
		//{
		//	if(MFramework::Instance->BreakObject->AddGroupData())
		//	{
		//		MFramework::Instance->BreakObject->set_AddGroup(true);
		//	}
		//}
		//return true;
	}
	else if(eType == ExtEntityType_ElevatorTarget)
	{
		MFramework::Instance->Elevator->AddPointEntity(pkOrgEntity);
		MFramework::Instance->Elevator->SetSyncData(true);
	}

	return false;
}

bool MPgExtEntity::TranslateExtEntity(MEntity* pkEntity, NiPoint3 kDeltaPosition)
{
	MPgExtEntity::eExtEntityType eType =
		MFramework::Instance->ExtEntity->IsExtEntityType(pkEntity);

	if (eType == MPgExtEntity::ExtEntityType_Monster ||
		eType == MPgExtEntity::ExtEntityType_WayPoint )
	{
	}
	else if (eType == MPgExtEntity::ExtEntityType_Npc)
	{
		MFramework::Instance->Npc->InsertModifiedNpcData(
			pkEntity->Name,
			kDeltaPosition.x,
			kDeltaPosition.y,
			kDeltaPosition.z);
	}
	else if (eType == ExtEntityType_ShineStone)
	{
		m_bHaveWaitTranslateEntity = true;
		m_pmWaitTranslateEntities->Add(pkEntity);
		m_eWaitType = eType;
	}
	else if (eType == ExtEntityType_DecalTarget)
	{
	}
	else if (eType == ExtEntityType_DecalData)
	{

	}
	else if(eType == ExtEntityType_BreakObject)
	{
		MFramework::Instance->BreakObject->ModifySettedBreakObjectPos(pkEntity, kDeltaPosition);
	}
	else if(eType == ExtEntityType_ElevatorTarget)
	{
		MFramework::Instance->Elevator->TranslatePointEntity(pkEntity, kDeltaPosition);
		MFramework::Instance->Elevator->SetSyncData(true);
	}

	return false;
}

bool MPgExtEntity::RotateExtEntity(MEntity* pkEntity, const NiMatrix3 &rkMatrix)
{
	if (!pkEntity)
	{
		return false;
	}

	MPgExtEntity::eExtEntityType eType =
		MFramework::Instance->ExtEntity->IsExtEntityType(pkEntity);

	if (eType == MPgExtEntity::ExtEntityType_Monster ||
		eType == MPgExtEntity::ExtEntityType_WayPoint )
	{
	}
	else if (eType == MPgExtEntity::ExtEntityType_Npc)
	{
		MFramework::Instance->Npc->InsertModifiedNpcData_Rotation(pkEntity->Name, rkMatrix);
	}
	else if (eType == ExtEntityType_ShineStone)
	{
	}
	else if (eType == ExtEntityType_DecalTarget)
	{
	}
	else if(eType == ExtEntityType_DecalData)
	{

	}
	else if(eType == ExtEntityType_BreakObject)
	{

	}
	else if(eType == ExtEntityType_ElevatorTarget)
	{

	}

	return false;
}

bool MPgExtEntity::SelectExtEntity(MEntity* pkEntity)
{
	// NPC일 경우를 처리.
	MPgExtEntity::eExtEntityType eType =
		MFramework::Instance->ExtEntity->IsExtEntityType(pkEntity);
	if (eType == MPgExtEntity::ExtEntityType_Npc)
	{
		// NPC 리스트에서 선택 되어지게.
		MFramework::Instance->Npc->SetSelectEntity(pkEntity);
		MFramework::Instance->Npc->set_IsSelectedData(true);
	}
	else if (eType == ExtEntityType_ShineStone)
	{
		m_bHaveWaitSelectEntity = true;
		//m_pkWaitSelectEntity = pkEntity;
		m_pmWaitSelectEntities->Add(pkEntity);
		m_eWaitType = eType;
	}
	else if(eType == ExtEntityType_ElevatorTarget)
	{
		return true;
	}

	return false;
}

void MPgExtEntity::Clear()
{
	MFramework::Instance->Monster->Clear();
	MFramework::Instance->Npc->Clear();
	MFramework::Instance->WayPoint->Clear();
	//MFramework::Instance->MonArea->Clear();
	MFramework::Instance->Path->Clear();
}

MPgExtEntity::eExtEntityType MPgExtEntity::IsExtEntityType(MEntity* pkEntity)
{
	if (pkEntity->MasterEntity)
	{
		if (pkEntity->MasterEntity->get_Name()->Contains("[Monster]"))
			return ExtEntityType_Monster;
		else if (pkEntity->MasterEntity->get_Name()->Contains("[Default]monster_target"))
			return ExtEntityType_Monster;
		else if (pkEntity->MasterEntity->get_Name()->Contains("npc_target"))
			return ExtEntityType_Npc;
		else if (pkEntity->MasterEntity->get_Name()->Contains("waypoint_target"))
			return ExtEntityType_WayPoint;
		else if (pkEntity->MasterEntity->get_Name()->Contains("waypoint_circle"))
			return ExtEntityType_WayPoint_Circle;
		else if (pkEntity->MasterEntity->get_Name()->Contains("path_target"))
			return ExtEntityType_Path;
		//else if (pkEntity->MasterEntity->get_Name()->Contains("monarea_target"))
		//	return ExtEntityType_MonAreaTarget;
		else if (pkEntity->MasterEntity->get_Name()->Contains("ShineStone"))
			return ExtEntityType_ShineStone;
		else if (pkEntity->MasterEntity->get_Name()->Contains("decal_target"))
			return ExtEntityType_DecalTarget;
		else if (pkEntity->MasterEntity->get_Name()->Contains("decal_data"))
			return ExtEntityType_DecalData;
		else if(pkEntity->MasterEntity->get_Name()->Contains("break_object"))
			return ExtEntityType_BreakObject;
		else if(pkEntity->MasterEntity->get_Name()->Contains("elevator_target"))
			return ExtEntityType_ElevatorTarget;
	}

	// not exist master entity case
	if(pkEntity->get_Name()->Contains("[Monster]"))
		return ExtEntityType_Monster;
	else if (pkEntity->get_Name()->Contains("[Default]monster_target"))
			return ExtEntityType_Monster;
	else if(pkEntity->get_Name()->Contains("npc_target") )
		return ExtEntityType_Npc;
	else if(pkEntity->get_Name()->Contains("waypoint_target") )
		return ExtEntityType_WayPoint;
	else if(pkEntity->get_Name()->Contains("waypoint_circle") )
		return ExtEntityType_WayPoint_Circle;
	else if(pkEntity->get_Name()->Contains("path_target") )
		return ExtEntityType_Path;
	//else if(pkEntity->get_Name()->Contains("monarea_target") )
	//	return ExtEntityType_MonAreaTarget;
	else if (pkEntity->get_Name()->Contains("ShineStone"))
		return ExtEntityType_ShineStone;
	else if (pkEntity->get_Name()->Contains("decal_target"))
		return ExtEntityType_DecalTarget;
	else if (pkEntity->get_Name()->Contains("decal_data"))
		return ExtEntityType_DecalData;
	else if(pkEntity->get_Name()->Contains("break_object"))
		return ExtEntityType_BreakObject;
	else if(pkEntity->get_Name()->Contains("elevator_target"))
		return ExtEntityType_ElevatorTarget;
	else
	{
		//// ExtEntityType_MonArea 일 경우.
		//{
		//	NiEntityInterface* pkInter = (NiEntityInterface*)pkEntity->PropertyInterface;
		//	for (int j=0 ; j<pkInter->GetComponentCount() ; j++)
		//	{
		//		if (pkInter->GetComponentAt(j)->GetClassName().Equals("PgMonAreaComponent"))
		//		{
		//			return ExtEntityType_MonArea;
		//		}
		//	}
		//}
	}

	return ExtEntityType_Normal;
}
