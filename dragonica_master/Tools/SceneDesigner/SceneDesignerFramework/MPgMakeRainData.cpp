//
// HandOver, 강정욱 2008.01.29
//
// *현재 쓰지 않음.
// 모든 폴리곤에 대해서 일정한 값에 대한 반사각을 구하는 클래스.
// 비가 반사되어야 할 방향을 구하기 위해서 만듬.
//
#include "SceneDesignerFrameworkPCH.h"
#include <vector>
#include "MFramework.h"
#include "MPgMakeRainData.h"

//#define PI	(3.141592653589793238462643f)

using namespace std;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgMakeRainData::MPgMakeRainData(void)
{
}

MPgMakeRainData::~MPgMakeRainData(void)
{
}

String* MPgMakeRainData::get_Filename()
{
	return m_pkFilename;
}

void MPgMakeRainData::set_Filename(String *pkFilename)
{
	m_pkFilename = pkFilename;
}

float MPgMakeRainData::get_DotValue()
{
	return m_fDotValue;
}

void MPgMakeRainData::set_DotValue(float fDotValue)
{
	m_fDotValue = fDotValue;
}

bool MPgMakeRainData::Initialize()
{
	m_fDotValue = 0.9f;
	m_bIsBinaryMode = true;

	return true;
}
ArrayList* MPgMakeRainData::get_IgnoreList()
{
	return m_pkIgnoreList;
}

void MPgMakeRainData::set_IgnoreList(ArrayList *pkList)
{
	m_pkIgnoreList = pkList;
}

bool MPgMakeRainData::get_IsBinaryMode()
{
	return m_bIsBinaryMode;
}

void MPgMakeRainData::set_IsBinaryMode(bool bMode)
{
	m_bIsBinaryMode = bMode;
}

// Geometry를 찾는 Stack 재귀 함수, 깊이 탐색
void MPgMakeRainData::GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeomerties)
{
	for(unsigned int i = 0;
		i < pkNode->GetChildCount();
		i++)
	{	
		NiAVObject *pkChild = pkNode->GetAt(i);

		// Ignore
		bool bIgnore = false;
		for (int j=0 ; j<m_pkIgnoreList->Count ; j++)
		{
		    const char* pcIgnorename = MStringToCharPointer(m_pkIgnoreList->get_Item(j)->ToString());
			if ( pkChild->GetName().Equals(pcIgnorename)
				// 오류 나기 때문에 잠시 주석.
				//|| MFramework::Instance->ExtEntity->IsExtEntityType(pcIgnorename)
				//!= MPgExtEntity::ExtEntityType_Normal
				)
			{
				bIgnore = true;
				break;
			}
		}
		if (bIgnore)
			continue;

		if(NiIsKindOf(NiNode, pkChild))
		{
			GetAllGeometries(NiDynamicCast(NiNode, pkChild), kGeomerties);
			//kQueue.AddHead(pkChild);
		}
		else if(NiIsKindOf(NiGeometry, pkChild))
		{
			kGeomerties.AddTail(pkChild);
			NiPoint3 *pkPoint = NiDynamicCast(NiGeometry, pkChild)->GetVertices();
			NiGeometryData *pkData = NiDynamicCast(NiGeometry, pkChild)->GetModelData();
		}
	}
}

void MPgMakeRainData::MakeRainDataToFile()
{
	// 전체
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	vector<NiPoint3> vtCenterVt;
	vector<NiPoint3> vtNormVt;

	//float fAngle = 30;
	//float fRadian = fAngle * PI / 180.0f;
	//float fRad = cos(fRadian);
	float fDotValue = m_fDotValue;

	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];

		NiObjectList kGeometries;
		for (unsigned int j=0 ; j<pkEntity->GetSceneRootPointerCount() ; j++)
		{
			NiNode *pkRoot = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(j));
	
			if(pkRoot)
			{
				GetAllGeometries(pkRoot, kGeometries);
			}
		}

		while(!kGeometries.IsEmpty())
		{
			NiGeometry *pkGeo = NiDynamicCast(NiGeometry, kGeometries.GetTail());
			if(NiIsKindOf(NiTriStrips, pkGeo))
			{
				NiTriStrips *pkTri = NiDynamicCast(NiTriStrips, pkGeo);
				int iCount = pkTri->GetTriangleCount();
				for (int k=0 ; k<pkTri->GetTriangleCount() ; k++)
				{
					unsigned short vi[3];
					pkTri->GetTriangleIndices(k, vi[0], vi[1], vi[2]);
					
					NiPoint3 kUpVt = NiPoint3(0,0,1);
					NiPoint3 kPt1 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[0]];
					NiPoint3 kPt2 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[1]];
					NiPoint3 kPt3 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[2]];
					NiPoint3 kNormVt = (kPt1 - kPt2).UnitCross((kPt1 - kPt3));
					NiPoint3 kCenterVt = NiPoint3(0,0,0);
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[0]];
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[1]];
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[2]];

					kCenterVt = kCenterVt / 3;

					if (kUpVt.Dot(kNormVt) > fDotValue)
					{
						vtCenterVt.push_back(kCenterVt);
						vtNormVt.push_back(kNormVt);
					}

				}
			}
			else if(NiIsKindOf(NiTriShape, pkGeo))
			{
				NiTriShape *pkTri = NiDynamicCast(NiTriShape, pkGeo);
				for (int k=0 ; k<pkTri->GetTriangleCount() ; k++)
				{
					unsigned short vi[3];
					pkTri->GetTriangleIndices(k, vi[0], vi[1], vi[2]);
					
					NiPoint3 kUpVt = NiPoint3(0,0,1);
					NiPoint3 kPt1 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[0]];
					NiPoint3 kPt2 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[1]];
					NiPoint3 kPt3 = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[2]];
					NiPoint3 kNormVt = (kPt1 - kPt2).UnitCross((kPt1 - kPt3));
					NiPoint3 kCenterVt = NiPoint3(0,0,0);
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[0]];
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[1]];
					kCenterVt += pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[2]];

					kCenterVt = kCenterVt / 3;

					if (kUpVt.Dot(kNormVt) > fDotValue)
					{
						vtCenterVt.push_back(kCenterVt);
						vtNormVt.push_back(kNormVt);
					}

				}
			}

			kGeometries.RemoveTail();
		}
	}

	// 출력
    //const char* pcFilename = MStringToCharPointer(m_pkFilename);
	
	std::wstring wstrFilePath;
	wstrFilePath.resize(m_pkFilename->Length);
	for (int i=0 ; i<m_pkFilename->Length ; i++)
	{
		wstrFilePath.at(i) = m_pkFilename->get_Chars(i);
	}


	int iTotal = vtCenterVt.size();

	FILE *fp;
	if (m_bIsBinaryMode)
	{
		_wfopen_s(&fp, wstrFilePath.c_str(), L"wb");

		fwrite(&iTotal,sizeof(int),1,fp);

		std::vector<NiPoint3>::iterator itr_CenterVt;
		std::vector<NiPoint3>::iterator itr_NormVt;
		for (itr_CenterVt = vtCenterVt.begin(), itr_NormVt = vtNormVt.begin() ;
			itr_CenterVt != vtCenterVt.end() ;
			++itr_CenterVt, ++itr_NormVt)
		{
			NiPoint3 kCenterVt = *itr_CenterVt;
			NiPoint3 kNormVt = *itr_NormVt;

			fwrite(&kCenterVt.x,sizeof(float),1,fp);
			fwrite(&kCenterVt.y,sizeof(float),1,fp);
			fwrite(&kCenterVt.z,sizeof(float),1,fp);
			fwrite(&kNormVt.x,sizeof(float),1,fp);
			fwrite(&kNormVt.y,sizeof(float),1,fp);
			fwrite(&kNormVt.z,sizeof(float),1,fp);
		}
	}
	else
	{
		_wfopen_s(&fp, wstrFilePath.c_str(), L"w");

		fwprintf(fp, L"%d \n", vtCenterVt.size());

		std::vector<NiPoint3>::iterator itr_CenterVt;
		std::vector<NiPoint3>::iterator itr_NormVt;
		for (itr_CenterVt = vtCenterVt.begin(), itr_NormVt = vtNormVt.begin() ;
			itr_CenterVt != vtCenterVt.end() ;
			++itr_CenterVt, ++itr_NormVt)
		{
			NiPoint3 kCenterVt = *itr_CenterVt;
			NiPoint3 kNormVt = *itr_NormVt;

			fwprintf(fp, L"%f %f %f %f %f %f\n",
				kCenterVt.x, kCenterVt.y, kCenterVt.z, 
				kNormVt.x, kNormVt.y, kNormVt.z );
		}
	}

	fclose(fp);
}