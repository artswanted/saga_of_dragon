#include "SceneDesignerFrameworkPCH.h"
#include "MEntityPostFixTextureUtil.H"
#include "MEntity.H"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

void MEntityPostFixTextureUtil::GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeometries)
{
	for(unsigned int i = 0;
		i < pkNode->GetChildCount();
		i++)
	{	
		NiAVObject *pkChild = pkNode->GetAt(i);

		if(NiIsKindOf(NiNode, pkChild))
		{
			GetAllGeometries(NiDynamicCast(NiNode, pkChild), kGeometries);
			//kQueue.AddHead(pkChild);
		}
		else if(NiIsKindOf(NiGeometry, pkChild))
		{
			kGeometries.AddTail(pkChild);
			NiPoint3 *pkPoint = NiDynamicCast(NiGeometry, pkChild)->GetVertices();
			NiGeometryData *pkData = NiDynamicCast(NiGeometry, pkChild)->GetModelData();
		}
	}
}

//텍스쳐 변경함수
bool	MEntityPostFixTextureUtil::ReplacePostFixTextureOldToNew(NiTexturingProperty::Map *pkMap,String* pkNewTexFileName)
{

	char const* pcFileName = MStringToCharPointer(pkNewTexFileName);
	bool bIsFileExist = NiImageConverter::DefaultFileExistsFunc(pcFileName);

	if(bIsFileExist)
	{
		NiSourceTexture* pkTexture = NiSourceTexture::Create(pcFileName);
		pkMap->SetTexture(pkTexture);
	}
	
	MFreeCharPointer(pcFileName);
	return	bIsFileExist;
}

bool	MEntityPostFixTextureUtil::CheckCanApplyPostFixTextureToTexture(NiTexturingProperty::Map *pkMap,String* pkNewPostFix,String* pkOldPostFix)
{
	if(!pkMap)
	{
		return	false;
	}

	NiSourceTexture	*pkSourceTexture = NiDynamicCast(NiSourceTexture,pkMap->GetTexture());
	if(!pkSourceTexture)
	{
		return	false;
	}

	String* pkNewTexFileName = GetNewTexFileName(pkSourceTexture->GetFilename(),pkNewPostFix,pkOldPostFix);

	char const* pcFileName = MStringToCharPointer(pkNewTexFileName);
	bool bIsFileExist = NiImageConverter::DefaultFileExistsFunc(pcFileName);
	MFreeCharPointer(pcFileName);
	return	bIsFileExist;
}
bool	MEntityPostFixTextureUtil::ApplyPostFixTextureToTexture(NiTexturingProperty::Map *pkMap,String* pkNewPostFix,String* pkOldPostFix)
{
	if(!pkMap)
	{
		return	false;
	}

	NiSourceTexture	*pkSourceTexture = NiDynamicCast(NiSourceTexture,pkMap->GetTexture());
	if(!pkSourceTexture)
	{
		return	false;
	}

	String* pkNewTexFileName = GetNewTexFileName(pkSourceTexture->GetFilename(),pkNewPostFix,pkOldPostFix);

	return	ReplacePostFixTextureOldToNew(pkMap,pkNewTexFileName);
}
bool	MEntityPostFixTextureUtil::CheckCanApplyPostFixTextureToGeometry(NiGeometry* pkGeom,String* pkNewPostFix,String* pkOldPostFix)
{
	if(NULL==pkGeom || NULL==pkGeom->GetPropertyState() || NULL==pkGeom->GetPropertyState()->GetTexturing())
	{
		return	false;
	}

	NiTexturingProperty* pkTexProp = pkGeom->GetPropertyState()->GetTexturing();
	if(!pkTexProp->GetBaseMap())
	{
		return	false;
	}

	if(!CheckCanApplyPostFixTextureToTexture(pkTexProp->GetBaseMap(),pkNewPostFix,pkOldPostFix))
	{
		return	false;
	}

	if(pkTexProp->GetGlowMap())
	{
		if(!CheckCanApplyPostFixTextureToTexture(pkTexProp->GetGlowMap(),pkNewPostFix,pkOldPostFix))
		{
			return	false;
		}
	}

	return	true;
}

String*	MEntityPostFixTextureUtil::GetNewTexFileName(String *pkTexFileName,String *pkNewPostFix,String *pkOldPostFix)
{
	String* pkNewTexFileName = pkTexFileName;
	String* kTexFileExtName = pkNewTexFileName->Substring(pkNewTexFileName->LastIndexOf("."));
	pkNewTexFileName = pkNewTexFileName->Remove(pkNewTexFileName->LastIndexOf("."));					
	pkNewTexFileName = pkNewTexFileName->ToUpperInvariant();

	if(pkOldPostFix->Length>0 && pkNewTexFileName->Contains(pkOldPostFix))
	{
		String* pkExt = pkNewPostFix->Concat(pkNewPostFix, kTexFileExtName);
		pkExt = pkExt->ToUpperInvariant();
		pkNewTexFileName = pkNewTexFileName->Replace(pkOldPostFix, pkExt);
	}
	else
	{
		pkNewTexFileName = pkNewTexFileName->Concat(pkNewTexFileName, pkNewPostFix);
		pkNewTexFileName = pkNewTexFileName->Concat(pkNewTexFileName, kTexFileExtName);
	}

	if(false == pkNewTexFileName->Contains(":\\"))
	{
		String* kTexFileByImageSub1=pkNewTexFileName;
		String* kTexFileByImageSub2=pkNewTexFileName;
		String* kTexFileByImageSub3=pkNewTexFileName;
		
		kTexFileByImageSub1 = String::Concat(MFramework::Instance->ImageSubfolder1, pkNewTexFileName);
		kTexFileByImageSub2 = String::Concat(MFramework::Instance->ImageSubfolder2, pkNewTexFileName);
		kTexFileByImageSub3 = String::Concat(MFramework::Instance->ImageSubfolder3, pkNewTexFileName);
		
		char const* pcFileName1 = MStringToCharPointer(kTexFileByImageSub1);
		char const* pcFileName2 = MStringToCharPointer(kTexFileByImageSub2);
		char const* pcFileName3 = MStringToCharPointer(kTexFileByImageSub3);
		if(NiImageConverter::DefaultFileExistsFunc(pcFileName1))
		{
			pkNewTexFileName = pkNewTexFileName->Concat(MFramework::Instance->ImageSubfolder1, pkNewTexFileName);
		}
		else if(NiImageConverter::DefaultFileExistsFunc(pcFileName2))
		{
			pkNewTexFileName = pkNewTexFileName->Concat(MFramework::Instance->ImageSubfolder2, pkNewTexFileName);
		}
		else if(NiImageConverter::DefaultFileExistsFunc(pcFileName3))
		{
			pkNewTexFileName = pkNewTexFileName->Concat(MFramework::Instance->ImageSubfolder3, pkNewTexFileName);
		}
		MFreeCharPointer(pcFileName1);
		MFreeCharPointer(pcFileName2);
		MFreeCharPointer(pcFileName3);
	}

	return	pkNewTexFileName;
}
bool	MEntityPostFixTextureUtil::ApplyPostFixTextureToGeometry(NiGeometry* pkGeom,String* pkNewPostFix,String* pkOldPostFix)
{
	if(!CheckCanApplyPostFixTextureToGeometry(pkGeom,pkNewPostFix,pkOldPostFix))
	{
		return	false;
	}

	NiTexturingProperty* pkTexProp = pkGeom->GetPropertyState()->GetTexturing();

	ApplyPostFixTextureToTexture(pkTexProp->GetBaseMap(),pkNewPostFix,pkOldPostFix);

	if(pkTexProp->GetGlowMap())
	{
		ApplyPostFixTextureToTexture(pkTexProp->GetGlowMap(),pkNewPostFix,pkOldPostFix);
	}

	return	true;
}

bool	MEntityPostFixTextureUtil::ApplyPostFixTextureToSceneRoot(MEntity* pmEntity,NiNode* pkRoot,String* pkNewPostFix,String* pkOldPostFix)
{
	if(!pkRoot)
	{
		return	false;
	}

	pkRoot->UpdateProperties();

	NiObjectList kGeomList;
	GetAllGeometries(pkRoot, kGeomList);

	bool	bResult = false;
	NiTListIterator kIter = kGeomList.GetHeadPos();
	while(NULL != kIter)
	{
		NiGeometry* pkGeom = NiDynamicCast(NiGeometry, kGeomList.GetNext(kIter));
		bResult |= ApplyPostFixTextureToGeometry(pkGeom,pkNewPostFix,pkOldPostFix);
	}

	return	bResult;
}
String* MEntityPostFixTextureUtil::ApplyPostFixTextureToEntity(MEntity* pmEntity, String* pkNewPostFix)
{
	String* pkOldPostFix = pmEntity->m_kPostfixTexture;

	pkNewPostFix = pkNewPostFix->ToUpperInvariant();
	pkOldPostFix = pkOldPostFix->ToUpperInvariant();

	bool	bChangeToNewPostFix = false;
	unsigned int const iSceneRoot = pmEntity->GetSceneRootPointerCount();
	for(unsigned int i=0; iSceneRoot>i; ++i)
	{
		NiNode* pkRoot = NiDynamicCast(NiNode, pmEntity->GetSceneRootPointer(i));

		bChangeToNewPostFix |= ApplyPostFixTextureToSceneRoot(pmEntity,pkRoot,pkNewPostFix,pkOldPostFix);
	}	
	return bChangeToNewPostFix ? pkNewPostFix : pkOldPostFix;
}

