//
// HandOver, 강정욱 2008.01.29
//
// NPC 관련을 관리.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MUtility.h"
#include "MpgNpc.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgNpc::MPgNpc(void)
{
	m_bDeletedNpcData = false;
	m_pmDeletedList = new ArrayList();
	m_bModifiedData = false;
	m_pmModifiedList = new ArrayList();
}

MPgNpc::~MPgNpc(void)
{
}

bool MPgNpc::Initialize()
{
	return true;
}

void MPgNpc::Clear()
{
	m_pmDeletedList->Clear();
	m_pmModifiedList->Clear();
	m_bDeletedNpcData = false;
	m_bModifiedData = false;
}

bool MPgNpc::IsNpcData(String *pkTemplateName)
{
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkTemp = MFramework::Instance->Scene->GetEntities()[i];
		if (pkTemp->Name->Equals(pkTemplateName))
		{
			if (pkTemp->MasterEntity->get_Name()->Equals("[General]Target.npc_target"))
				return true;
			else
				return false;
		}
	}
	return false;
}

bool MPgNpc::IsNpcData(MEntity *pkEntity)
{
	if (!pkEntity->MasterEntity)
		return false;

	if (pkEntity->MasterEntity->get_Name()->Equals("[General]Target.npc_target"))
		return true;
	else
		return false;
}

bool MPgNpc::get_IsDeletedNpcData()
{
	return m_bDeletedNpcData;
}

void MPgNpc::set_IsDeletedNpcData(bool bDelete)
{
	m_bDeletedNpcData = bDelete;
}
bool MPgNpc::get_IsModifiedNpcData()
{
	return m_bModifiedData;
}
void MPgNpc::set_IsModifiedNpcData(bool bModified)
{
	m_bModifiedData = bModified;
}
bool MPgNpc::get_IsSelectedData()
{
	return m_bSelectedData;
}
void MPgNpc::set_IsSelectedData(bool bSelect)
{
	m_bSelectedData = bSelect;
}

void MPgNpc::InsertDeletedNpcData(String *pkName)
{
	m_pmDeletedList->Add(pkName);
}
int MPgNpc::CountDeletedNpcData()
{
	return m_pmDeletedList->Count;
}
String* MPgNpc::PopDeletedNpcData()
{
	if (m_pmDeletedList->Count > 0)
	{
		String *strName = m_pmDeletedList->get_Item(0)->ToString();
		m_pmDeletedList->RemoveAt(0);
		return strName;
	}
	return 0;
}

void MPgNpc::InsertNpc(MPoint3* pkTranslation, MPoint3* pkDirection, MEntity* pkNpcEntity)
{
	NiPoint3 kDirection;
	pkDirection->ToNiPoint3(kDirection);
	kDirection.Unitize();
	
	if (kDirection != NiPoint3::ZERO)
	{
		NiPoint3 kUnit = NiPoint3(1,0,0);

		// Dot
		float fDotValue = kDirection.Dot(kUnit);
		if (fDotValue < -1.0f) fDotValue = -1.0f;
		else if (fDotValue > 1.0f) fDotValue = 1.0f;
		float fRadian = acos(fDotValue);
		float fAngle = fRadian * 180.0f / (float)Math::PI;

		// 외적
		NiPoint3 ptAxisVec;
		ptAxisVec = (kDirection).UnitCross(kUnit);

		//  사원수
		NiQuaternion kQuat;
		NiMatrix3 kRotMat;
		kQuat = NiQuaternion(fRadian, ptAxisVec);
		kQuat.Normalize();
		kQuat.ToRotation(kRotMat);

		pkNpcEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRotMat);
	}

	NiPoint3 kPosition;
	pkTranslation->ToNiPoint3(kPosition);
	pkNpcEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPosition);
	MFramework::Instance->Scene->AddEntity(pkNpcEntity, false);
}

void MPgNpc::InsertModifiedNpcData(String *pkSrcName, String *pkDestName)
{
	ModifiedNpcData *pkData = new ModifiedNpcData;
	pkData->Initialize(pkSrcName, pkDestName);
	m_pmModifiedList->Add(pkData);

	m_bModifiedData = true;
}

void MPgNpc::InsertModifiedNpcData(String *pkSrcName, float fPosX, float fPosY, float fPosZ)
{
	ModifiedNpcData *pkData = new ModifiedNpcData;
	pkData->Initialize(pkSrcName, fPosX, fPosY, fPosZ);
	m_pmModifiedList->Add(pkData);

	m_bModifiedData = true;
}

void MPgNpc::InsertModifiedNpcData_Rotation(String *pkSrcName, const NiMatrix3& rkMatrix)
{
	ModifiedNpcData *pkData = new ModifiedNpcData;

	NiPoint3 kPoint(0,-1,0);
	kPoint = rkMatrix * kPoint;
	pkData->SetRotate(pkSrcName, kPoint.x, kPoint.y, kPoint.z);
	m_pmModifiedList->Add(pkData);

	m_bModifiedData = true;
}

bool MPgNpc::GetModifiedNpcData_IsTranslate(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName) &&
			pkData->m_bIsTranslte)
		{
			return true;
		}
	}

	return false;
}

bool MPgNpc::GetModifiedNpcData_IsRotate(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName) &&
			pkData->m_bIsRotate)
		{
			return true;
		}
	}

	return false;
}

bool MPgNpc::GetModifiedNpcData_IsNamed(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName) &&
			pkData->m_bIsNamed)
		{
			return true;
		}
	}

	return false;
}

String* MPgNpc::GetModifiedNpcData_DestName(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_pkDestName;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_PosX(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fPosX;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_PosY(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fPosY;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_PosZ(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fPosZ;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_RotateX(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fRotateX;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_RotateY(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fRotateY;
		}
	}

	return 0;
}

float MPgNpc::GetModifiedNpcData_RotateZ(String *pkSrcName)
{
	for (int i=0 ; i<m_pmModifiedList->Count ; i++)
	{
		ModifiedNpcData *pkData =
			dynamic_cast<ModifiedNpcData *>(m_pmModifiedList->get_Item(i));
		if (pkData->m_pkSrcName->Equals(pkSrcName))
		{
			return pkData->m_fRotateZ;
		}
	}

	return 0;
}

void MPgNpc::ClearModifiedNpcData()
{
	m_pmModifiedList->Clear();
}

void MPgNpc::SetSelectEntity(MEntity *pkEntity)
{
	m_pkSelectedEntity = pkEntity;
}
