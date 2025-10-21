//
// HandOver, 강정욱 2008.01.29
//
// 몇몇 숨겨야 할 Type의 Object들을 숨긴다.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MpgExtEntity.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgHideSomeObject::MPgHideSomeObject(void)
{
}

MPgHideSomeObject::~MPgHideSomeObject(void)
{
}

bool MPgHideSomeObject::Initialize()
{
	bIsHide = false;

	return true;
}

bool MPgHideSomeObject::HideNeedlessObject()
{
	bIsHide = !bIsHide;

	const int iNeedless = 8;
	const char* pcNeedless[] =
	{
        "char_spawns",
		"paths",
		"physx",
        "triggers",
		"camera_walls",
		"ladders",
		"ropes",
		"telejump",
	};

	MScene* pmScene = MFramework::Instance->Scene;
	
	MEntity* pkEntities[] = pmScene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity* pkEntity = pkEntities[i];

		for(unsigned int j = 0;
			j < pkEntity->GetSceneRootPointerCount();
			j++)
		{
			NiNode *pkRoot = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(j));
			
			if(pkRoot)
			{
				for (int k=0 ; k<iNeedless ; k++)
				{
					NiAVObject *pkObject = pkRoot->GetObjectByName(pcNeedless[k]);
					//if(NiIsKindOf(NiNode, pkObject))
					//{
					//	pkObject = NiDynamicCast(NiNode, pkObject)->GetAt(0);
					//}

					if (pkObject)
					{
						pkObject->SetAppCulled(bIsHide);
					}
				}
			}
		}
	}

	return true;
}
