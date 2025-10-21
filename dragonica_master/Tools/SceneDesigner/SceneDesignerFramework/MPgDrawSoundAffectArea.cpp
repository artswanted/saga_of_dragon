#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgDrawSoundAffectArea.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

void SetAllConnectivity(NiBool* pkConnect, bool const bSetVal)
{//pkTarget의 내용을 일괄적으로 bSetVal로 채운다.
	assert(NULL!=pkConnect);
	memset(pkConnect, bSetVal, sizeof(NiBool)*512);
}

void SetRangeConnectivity(NiBool* pkConnect, size_t const iStartPos, size_t const iDestCount, bool const bSetVal)
{//pkTarget에서 iStartPos ~ iStartPos+iDestCount 범위의 내용을 일괄적으로 bSetVal로 채운다.
	assert(NULL!=pkConnect);
	assert(512>iStartPos);//iStartPos가 최대범위보다는 작아야 함
	assert(512>(iStartPos+iDestCount)); //iStartPos+iDestCount가 최대범위보다는 작아야 함
	memset(pkConnect+iStartPos, bSetVal, sizeof(NiBool)*iDestCount);
}

void BuildSphere(NiPoint3* pkSphereVertexArray, NiBool* pkSphereConnect, int iLat, int iLongs) 
{
	SetAllConnectivity(pkSphereConnect, false);

	NiPoint3 kPrevPoint(0.0f, 0.0f, 0.0f);
	NiPoint3 kFirstPoint(0.0f, 0.0f, 0.0f);
	int iVCPL = 3; //Vertex Count Per Loop

	for(int i = 0; i <= iLat; ++i) 
	{
		float dLat0 = NI_PI * (-0.5 + static_cast<float> (i - 1) / iLat);
		float dZ0  = sin(dLat0);
		float dZr0 =  cos(dLat0);

		float lat1 = NI_PI * (-0.5 + static_cast<float> (i)/ iLat);
		float dZ1 = sin(lat1);
		float dZr1 = cos(lat1);

		for(int j = 0; j <= iLongs; ++j) 
		{
			SetRangeConnectivity(pkSphereConnect, (i*iLongs*iVCPL)+(j*iVCPL), 2, true);

			float dLng = 2 * NI_PI * static_cast<float> (j - 1) / iLongs;
			float dX = cos(dLng);
			float dY = sin(dLng);

			pkSphereVertexArray[(i*iLongs*iVCPL)+( j*iVCPL)   ] = NiPoint3(dX*dZr0, dY*dZr0, dZ0);
			pkSphereVertexArray[(i*iLongs*iVCPL)+( j*iVCPL)+1] = NiPoint3(dX*dZr1, dY*dZr1, dZ1);
			if((iLongs-1)==j)
			{
				pkSphereVertexArray[(i*iLongs*iVCPL)+2] = pkSphereVertexArray[(i*iLongs*iVCPL)+(j*iVCPL)+1];
			}
			pkSphereVertexArray[(i*iLongs*iVCPL)+(j*iVCPL)+2] = kPrevPoint;
			kPrevPoint = pkSphereVertexArray[(i*iLongs*iVCPL)+(j*iVCPL)+1];
		}
	}
}

ISelectionService* MPgDrawSoundAffectArea::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}

void MPgDrawSoundAffectArea::Render(MRenderingContext* pmRenderingContext)
{
	NiEntityRenderingContext* pkContext = 
			pmRenderingContext->GetRenderingContext();
	NiCamera* pkCam = pkContext->m_pkCamera;

	MEntity* pkSelectedEntity[] = SelectionService->GetSelectedEntities();
	if(0<pkSelectedEntity->Count)
	{//엔티티가 1개 이상 선택되어 있으면
		for(int iIndex=0;pkSelectedEntity->Count > iIndex;++iIndex)
		{//모든 엔티티에 대해
			//엔티티의 컴퍼넌트 갯수를 구함
			unsigned int uiCompCount = pkSelectedEntity[iIndex]->GetNiEntityInterface()->GetComponentCount();
			for(unsigned int uiCount = 0 ; uiCompCount>uiCount;++uiCount)
			{//모든 컴퍼넌트에 대해
				//루프 현재 엔티티에 대해 루프 현재의 컴퍼넌트 이름을 얻어옴
				String* pkName = pkSelectedEntity[iIndex]->GetNiEntityInterface()->GetComponentAt(uiCount)->GetName();
				if(true==pkName->Equals("Sound Object"))
				{//컴퍼넌트가 사운드오브젝트이면 데이터 설정 및 렌더링
					float fScaleMax = 0.0f;
					float fScaleAtten = 0.0f;
					NiPoint3 kPos;
					pkSelectedEntity[iIndex]->GetNiEntityInterface()->GetPropertyData("Translation", kPos);
					NiEntityComponentInterface* pkComponent = pkSelectedEntity[iIndex]->GetNiEntityInterface()->GetComponentAt(uiCount);
					pkComponent->GetPropertyData("Affect Attenuation", fScaleAtten);
					pkComponent->GetPropertyData("Affect Range", fScaleMax);
					m_pkMaxDist->SetScale(fScaleMax);
					m_pkMaxDist->SetTranslate(kPos);
					m_pkAttenDist->SetScale(fScaleAtten);
					m_pkAttenDist->SetTranslate(kPos);

					m_pkMaxDist->UpdateProperties();
					m_pkMaxDist->Update(0.0f);				
					m_pkAttenDist->UpdateProperties();
					m_pkAttenDist->Update(0.0f);

					NiDrawScene(pkCam, m_pkMaxDist, *pkContext->m_pkCullingProcess);
					NiDrawScene(pkCam, m_pkAttenDist, *pkContext->m_pkCullingProcess);
					break;
				}
			}
		}
	}	
}

void MPgDrawSoundAffectArea::CreateAreaMesh()
{
	//build  max-dist-mesh
	m_pkMaxDistVerts = NiNew NiPoint3[ms_iMaxVerts];
	m_pkMaxDistConnect = NiAlloc(NiBool, ms_iMaxVerts);
	m_pkMaxDist = NiNew NiLines(ms_iMaxVerts, m_pkMaxDistVerts, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, m_pkMaxDistConnect);

	m_pkMaxDistMaterial = NiNew NiMaterialProperty();
	m_pkMaxDistMaterial->SetAmbientColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaxDistMaterial->SetDiffuseColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaxDistMaterial->SetSpecularColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaxDistMaterial->SetEmittance(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaxDistMaterial->SetShineness(0.0f);

	m_pkMaxDist->AttachProperty(m_pkMaxDistMaterial);
	m_pkMaxDist->Update(0.0f);
	m_pkMaxDist->UpdateEffects();
	m_pkMaxDist->UpdateProperties();
	m_pkMaxDist->UpdateNodeBound();
	m_pkMaxDist->IncRefCount();

	BuildSphere(m_pkMaxDistVerts, m_pkMaxDistConnect, 10, 10);

	//build atten-dist-mesh
	m_pkAttenDistVerts = NiNew NiPoint3[ms_iMaxVerts];
	m_pkAttenDistConnect = NiAlloc(NiBool, ms_iMaxVerts);
	m_pkAttenDist = NiNew NiLines(ms_iMaxVerts, m_pkAttenDistVerts, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, m_pkAttenDistConnect);

	m_pkAttenDistMaterial = NiNew NiMaterialProperty();
	m_pkAttenDistMaterial->SetAmbientColor(NiColor::NiColor(1.0f, 0.0f, 0.0f));
	m_pkAttenDistMaterial->SetDiffuseColor(NiColor::NiColor(1.0f, 0.0f, 0.0f));
	m_pkAttenDistMaterial->SetSpecularColor(NiColor::NiColor(1.0f, 0.0f, 0.0f));
	m_pkAttenDistMaterial->SetEmittance(NiColor::NiColor(1.0f, 0.0f, 0.0f));
	m_pkAttenDistMaterial->SetShineness(0.0f);

	m_pkAttenDist->AttachProperty(m_pkAttenDistMaterial);
	m_pkAttenDist->Update(0.0f);
	m_pkAttenDist->UpdateEffects();
	m_pkAttenDist->UpdateProperties();
	m_pkAttenDist->UpdateNodeBound();
	m_pkAttenDist->IncRefCount();

	BuildSphere(m_pkAttenDistVerts, m_pkAttenDistConnect, 10, 10);
}

void MPgDrawSoundAffectArea::Do_Dispose(bool bDisposing)
{
}

void MPgDrawSoundAffectArea::Init()
{
	ms_pmThis = new MPgDrawSoundAffectArea();
}

void MPgDrawSoundAffectArea::Shutdown()
{
	if(NULL != ms_pmThis)
	{
		delete ms_pmThis;
	}
}

bool MPgDrawSoundAffectArea::InstanceIsValid()
{
	return (NULL != ms_pmThis);
}

MPgDrawSoundAffectArea* MPgDrawSoundAffectArea::get_Instance()
{
	return ms_pmThis;
}