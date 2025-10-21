#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MpgZoneControl.h"

using namespace std; 
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgZoneControl::MPgZoneControl()
{
}



void MPgZoneControl::Render(MRenderingContext* pmRenderingContext)
{
	if(false != m_bZoneDraw)
	{
		NiEntityRenderingContext* pkContext = 
				pmRenderingContext->GetRenderingContext();
			NiCamera* pkCam = pkContext->m_pkCamera;
		AdjustMesh();
		if(false == m_bPlayerBase)
		{
			for(int ix=0;static_cast<int>(m_pkZoneCount->x)>=ix;++ix)
			{
				for(int iy=0;static_cast<int>(m_pkZoneCount->y)>=iy;++iy)
				{
					for(int iz=0;static_cast<int>(m_pkZoneCount->z)>=iz;++iz)
					{
						m_pkZoneLine->SetTranslate(
							m_pkMinPos->x + m_pkZoneSize->x*ix,
							m_pkMinPos->y + m_pkZoneSize->y*iy,
							m_pkMinPos->z + m_pkZoneSize->z*iz	);
						m_pkZoneLine->Update(0.0f);
						NiDrawScene(pkCam, m_pkZoneLine, *pkContext->m_pkCullingProcess);
					}
				}
			}			
		}
		else
		{
			int const iCountTargetZone = 3;
			NiPoint3 kPlayerPos;
			m_pkPlayer->GetNiEntityInterface()->GetPropertyData("Translation", kPlayerPos);
			int const iPlayerXindex = (kPlayerPos.x - m_pkMinPos->x) / m_pkZoneSize->x;
			int const iPlayerYindex = (kPlayerPos.y - m_pkMinPos->y) / m_pkZoneSize->y;
			int const iPlayerZindex = (kPlayerPos.z - m_pkMinPos->z) / m_pkZoneSize->z;
			for(int ix=iPlayerXindex-1;iPlayerXindex+1>=ix;++ix)
			{
				for(int iy=iPlayerYindex-1;iPlayerYindex+1>=iy;++iy)
				{
					for(int iz=iPlayerZindex-1;iPlayerZindex+1>=iz;++iz)
					{
						m_pkZoneLine->SetTranslate(
							m_pkMinPos->x + m_pkZoneSize->x*ix,
							m_pkMinPos->y + m_pkZoneSize->y*iy,
							m_pkMinPos->z + m_pkZoneSize->z*iz	);
						m_pkZoneLine->Update(0.0f);
						NiDrawScene(pkCam, m_pkZoneLine, *pkContext->m_pkCullingProcess);
					}
				}
			}
		}
	}
}

void MPgZoneControl::AdjustMesh()
{
	//if(NULL != m_pkAnchor)
	{
		NiPoint3 kAnchorPos;
		//MEntity* pkSelEntity = SelectionService->GetSelectedEntities()[0];
		if(NULL != m_pkAnchor)
		{
			m_pkAnchor->GetNiEntityInterface()->GetPropertyData("Translation", kAnchorPos);
			//kAnchorPos = *m_pkMinPos + *m_pkZoneSize;
			//pkSelEntity->GetNiEntityInterface()->SetPropertyData("Translation", kAnchorPos);

			NiPoint3 kPoint0(0.0f, 0.0f,0.0f), 
				kPoint1(kAnchorPos.x, 0.0f, 0.0f), 
				kPoint2(kAnchorPos.x, kAnchorPos.y, 0.0f), 
				kPoint3(0.0f, kAnchorPos.y, 0.0f);
			//NiPoint3 kPoint0(0.0f, 0.0f, 0.0f), kPoint1(20.0f, 0.0f, 0.0f), kPoint2(20.0f, 20.0f, 0.0f), kPoint3(0.0f, 20.0f, 0.0f);

			::memset(m_pkConnect, false, sizeof(NiBool)*16);
			::memset(m_pkConnect, true, sizeof(NiBool)*7);
			m_pkZoneVertexArray[0] = kPoint0; 
			m_pkZoneVertexArray[1] = kPoint1; 
			m_pkZoneVertexArray[2] = kPoint2; 
			m_pkZoneVertexArray[3] = kPoint3; 
			m_pkZoneVertexArray[4] = kPoint0; 
			m_pkZoneVertexArray[5] = kPoint0; m_pkZoneVertexArray[5].z = kAnchorPos.z;
			m_pkZoneVertexArray[6] = kPoint1; m_pkZoneVertexArray[6].z = kAnchorPos.z;
			m_pkZoneVertexArray[7] = kPoint1; 

			::memset(m_pkConnect+8, true, sizeof(NiBool)*7);
			m_pkZoneVertexArray[8]  = kPoint3; m_pkZoneVertexArray[8].z = kAnchorPos.z;
			m_pkZoneVertexArray[9]  = kPoint0; m_pkZoneVertexArray[9].z = kAnchorPos.z;
			m_pkZoneVertexArray[10] = kPoint1; m_pkZoneVertexArray[10].z = kAnchorPos.z;
			m_pkZoneVertexArray[11] = kPoint2; m_pkZoneVertexArray[11].z = kAnchorPos.z;
			m_pkZoneVertexArray[12] = kPoint3; m_pkZoneVertexArray[12].z = kAnchorPos.z;
			m_pkZoneVertexArray[13] = kPoint3;
			m_pkZoneVertexArray[14] = kPoint2;
			m_pkZoneVertexArray[15] = kPoint2; m_pkZoneVertexArray[15].z = kAnchorPos.z;

			*m_pkZoneSize = kAnchorPos;// - *m_pkMinPos;
			m_pkZoneCount->x = (m_pkMaxPos->x - m_pkMinPos->x) / m_pkZoneSize->x;
			m_pkZoneCount->y = (m_pkMaxPos->y - m_pkMinPos->y) / m_pkZoneSize->y;
			m_pkZoneCount->z = (m_pkMaxPos->z - m_pkMinPos->z) / m_pkZoneSize->z;
		}
	}
}

void MPgZoneControl::CreateMesh()
{
	m_pkZoneVertexArray = NiNew NiPoint3[16];
	m_pkConnect = NiAlloc(NiBool, 16);
	m_pkZoneLine = NiNew NiLines(16, m_pkZoneVertexArray, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, m_pkConnect);
	//m_pkZoneLine->IncRefCount();

	m_pkMaterial = NiNew NiMaterialProperty();
	m_pkMaterial->SetAmbientColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaterial->SetDiffuseColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaterial->SetSpecularColor(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaterial->SetEmittance(NiColor::NiColor(1.0f, 1.0f, 1.0f));
	m_pkMaterial->SetShineness(0.0f);

	m_pkZoneLine->AttachProperty(m_pkMaterial);
	m_pkZoneLine->Update(0.0f);
	m_pkZoneLine->UpdateEffects();
	m_pkZoneLine->UpdateProperties();
	m_pkZoneLine->UpdateNodeBound();

	m_pkZoneSize = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
	m_pkZoneCount = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
	m_pkMinPos = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
	m_pkMaxPos = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
}


void MPgZoneControl::Do_Dispose(bool bDisposing)
{
}

void MPgZoneControl::Init()
{
	ms_pmThis = new MPgZoneControl();
}

void MPgZoneControl::Shutdown()
{
	if(NULL != m_pkConnect)
	{
		NiFree(m_pkConnect);
	}
	if(NULL != m_pkMinPos)
	{
		NiDelete m_pkMinPos;
	}
	if(NULL != m_pkMaxPos)
	{
		NiDelete m_pkMaxPos;
	}
	if(NULL != m_pkZoneVertexArray)
	{
		NiDelete[] m_pkZoneVertexArray;
	}
	if(NULL != m_pkZoneLine)
	{
		//NiDelete m_pkZoneLine;
	}
	

	if(NULL != ms_pmThis)
	{
		delete ms_pmThis;
	}
}

bool MPgZoneControl::InstanceIsValid()
{
	return (NULL != ms_pmThis);
}

MPgZoneControl* MPgZoneControl::get_Instance()
{
	return ms_pmThis;
}

void MPgZoneControl::SetAnchor(MEntity *pkTarget)
{
	if(NULL == m_pkAnchor && NULL != pkTarget)
	{
		m_pkAnchor = pkTarget;
	}
}

MEntity* MPgZoneControl::GetAnchor()
{
	return m_pkAnchor;
}


void MPgZoneControl::SetPlayer(MEntity* pkTarget)
{
	if(NULL == m_pkPlayer && NULL != pkTarget )
	{
		m_pkPlayer = pkTarget;
	}
}

MEntity* MPgZoneControl::GetPlayer()
{
	return m_pkPlayer;
}

void MPgZoneControl::set_ZoneDraw(bool kZoneDraw)
{
	m_bZoneDraw = kZoneDraw;
}

void MPgZoneControl::set_PlayerBase(bool kPlayerBase)
{
	m_bPlayerBase= kPlayerBase;
}

void MPgZoneControl::SetWolrdBound(NiPoint3* pkMinPos, NiPoint3* pkMaxPos)
{
	m_pkMinPos = pkMinPos;
	m_pkMaxPos = pkMaxPos;
}

void MPgZoneControl::ComputeBound()
{
	MFramework::Instance->Scene->GetPGWolrdBound(m_pkMinPos, m_pkMaxPos);
}

ISelectionService* MPgZoneControl::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}