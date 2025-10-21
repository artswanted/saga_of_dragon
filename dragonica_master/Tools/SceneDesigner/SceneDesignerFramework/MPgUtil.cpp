//
// HandOver, 강정욱 2008.01.29
//
// 커스터마이징 해서 전체적으로 쓰는 Util 클래스.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgPropertyDef.h"
#include "IEntityPathService.h"
#include "MPgUtil.h"
#include <errno.h>

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgUtil::MPgUtil(void)
{
}

MPgUtil::~MPgUtil(void)
{
}

bool MPgUtil::IsEqualF(float const fLhs, float const fRhs, float const fTol)
{
	if( (fLhs > fRhs-fTol) && (fLhs < fRhs+fTol) )
	{
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
IComponentService* MPgUtil::get_ComponentService()
{
    if (ms_pmComponentService == NULL)
    {
        ms_pmComponentService = MGetService(IComponentService);
        MAssert(ms_pmComponentService != NULL, "Component service not "
            "found!");
    }
    return ms_pmComponentService;
}

bool MPgUtil::Initialize()
{
	if (!MFramework::Instance || !MFramework::Instance->Scene)
	{
		return false;
	}
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity* pkEntity = pkEntities[i];
		{
			String* pkPGProperty = pkEntity->GetNiEntityInterface()->GetPGProperty();
			String* pkPGAlphaGroup = pkEntity->GetNiEntityInterface()->GetPGAlphaGroup();
			String* pkPGOptimization = pkEntity->GetNiEntityInterface()->GetPGOptimization();

			// PG Property
			MEntity::ePGProperty eType = MEntity::ePGProperty::Object;
			for (int j=0 ; j<gs_iPGPropertyCount ; j++)
			{
				if (pkPGProperty->Equals(gs_pcPGProperty[j]))
				{
					eType = (MEntity::ePGProperty)j;
					break;
				}
			}
			pkEntity->SetPGProperty(eType);

			// UsePhysX
			bool bUsePhysX = pkEntity->GetNiEntityInterface()->GetPGUsePhysX();
			pkEntity->SetPGUsePhysX(bUsePhysX, false);
			// PostfixTexture
			String* pkPostfixTexture = pkEntity->GetNiEntityInterface()->GetPGPostfixTexture();
			if (pkPostfixTexture->Length > 0)
			{
				pkEntity->SetPGPostfixTexture(pkPostfixTexture, false);
			}

			// PG AlphaGroup
			MEntity::ePGAlphaGroup eAlphaType = MEntity::ePGAlphaGroup::Group0;
			for (int j=0 ; j<gs_iPGAlphaGroupCount ; j++)
			{
				if (pkPGAlphaGroup->Equals(gs_pcPGAlphaGroup[j]))
				{
					eAlphaType = (MEntity::ePGAlphaGroup)j;
					break;
				}
			}
			pkEntity->SetPGAlphaGroup(eAlphaType, false);

			// PG Optimization
			MEntity::ePGOptimization eOptiType = MEntity::ePGOptimization::High_Mid_Low;
			for (int j=0 ; j<gs_iPGOptimizationCount ; j++)
			{
				if (pkPGOptimization->Equals(gs_pcPGOptimization[j]))
				{
					eOptiType = (MEntity::ePGOptimization)j;
					break;
				}
			}
			pkEntity->SetPGOptimization(eOptiType, false);
	
			// RandomAni
			bool bRandomAni = pkEntity->GetNiEntityInterface()->GetPGRandomAni();
			pkEntity->SetPGRandomAni(bRandomAni, false);
		}

		// Other Component.
		{
			String* astrPropertyNames[] = pkEntity->GetPropertyNames();
			for (int i = 0; i < astrPropertyNames->Length; i++)
			{
				String* strPropertyName = astrPropertyNames[i];
				Object* pmData = pkEntity->GetPropertyData(strPropertyName);
				MPgUtil::ApplyPropertyData(pkEntity, strPropertyName, pmData);
			}
		}
	}

	MFramework::Instance->ExistFile->FindNotExistFile();

	return true;
}

//---------------------------------------------------------------------------
// Start에서 End 경로까지 상대경로 만들어서 리턴.
// ConvertAbsolutepathToRelativepath
String* MPgUtil::ConvertAbsPathToRelPath(String* pStartPath, String* pEndPath)
{
	ArrayList *arStartPath = new ArrayList();
	ArrayList *arEndPath = new ArrayList();

	int iFirstIdx = 0;
	int iSecondIdx = 0;
	while ( (iSecondIdx = pStartPath->IndexOf("\\", iFirstIdx)) != -1 )
	{
		arStartPath->Add(pStartPath->Substring(iFirstIdx, iSecondIdx-iFirstIdx));
		iFirstIdx = iSecondIdx+1;
	} 
	if (iFirstIdx < pStartPath->Length)
		arStartPath->Add(pStartPath->Substring(iFirstIdx, pStartPath->Length-iFirstIdx));

	iFirstIdx = 0;
	iSecondIdx = 0;
	while ( (iSecondIdx = pEndPath->IndexOf("\\", iFirstIdx)) != -1 )
	{
		arEndPath->Add(pEndPath->Substring(iFirstIdx, iSecondIdx-iFirstIdx));
		iFirstIdx = iSecondIdx+1;
	}
	if (iFirstIdx < pEndPath->Length)
		arEndPath->Add(pEndPath->Substring(iFirstIdx, pEndPath->Length-iFirstIdx));

	int iSameCount = 0;
	int iMoveupCount = 0;

	while( iSameCount < arStartPath->Count && iSameCount < arEndPath->Count &&
		arStartPath->get_Item(iSameCount)->Equals(arEndPath->get_Item(iSameCount)) )
		iSameCount += 1;
	iMoveupCount = arStartPath->Count - iSameCount;

	String *strResult;
	// 두번째 경로가 첫번째 경로의 처음부터 끝까지를 가지고 있을때.
	if (iMoveupCount == 0)
	{
		strResult = ".\\";
		String *strTemp2 = "\\";
		for (int i=iSameCount ; i<arEndPath->Count ; i++)
		{
			strResult = String::Concat(strResult, arEndPath->get_Item(i), strTemp2);
		}
	}
	// 완전 새경로 (C: 와 D: 정도로 서로 중복되는 디렉토리가 없을) 일때.
	else if (iMoveupCount == arStartPath->Count)
	{
		strResult = pEndPath;
	}
	// 조금 중복이 있을때
	else
	{
		strResult = ".\\";
		String *strTemp = "..\\";
		String *strTemp2 = "\\";
		for (int i=0 ; i<iMoveupCount ; i++)
			strResult = String::Concat(strResult, strTemp);
		for (int i=iSameCount ; i<arEndPath->Count ; i++)
			strResult = String::Concat(strResult, arEndPath->get_Item(i), strTemp2);
	}

	// 만약 .\ 있다면(.\\ 이거 말고) 지워주자.
	int iCount = arStartPath->Count + arEndPath->Count;
	for (int i = 0 ; i < iCount ; i++)
	{
		strResult = strResult->Replace("\\.\\", "\\");
	}

	return strResult;
}

void MPgUtil::SetTimeToAniObj(NiObjectNET* pkObj, float &rfTime)
{
	if (!pkObj)
	{
		return;
	}
    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
		if (rfTime == -1000 && (int)pkControl->GetEndKeyTime() != 0.0f)
		{
			int iDat = ((int)pkControl->GetEndKeyTime() * 100);
			if (iDat)
			{
				rfTime = (float)(rand() % iDat);
				rfTime *= 0.01f;
			}
		}
		pkControl->SetPhase(rfTime);
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;
        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pProperty && pProperty->GetControllers())
			{
                SetTimeToAniObj(pProperty, rfTime);
			}
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode;
        unsigned int i;
            
        pkNode = (NiNode*) pkObj;

        // recurse on children
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;
            pkChild = pkNode->GetAt(i);
            if (pkChild)
			{
                SetTimeToAniObj(pkChild, rfTime);
			}
        }
    }
}

//---------------------------------------------------------------------------
// HandOver, ApplyComponentProps, 강정욱 2008.01.29
// Component를 추가 혹은 수정 했을 때 여기로 오는데...
// Component값이 바뀜에 따라 또 다른 것들이 바뀌어야 할 때의 처리를 해준다.
// MPropertyContainer.cpp에서 Component를 Set할 때 이 곳으로 들어 온다.
//
// ex) 애니메이션 시간을 바꾸었을때.. 맵에 찍혀 있는 그 Object의 애니메이션 시간을 진짜 바꾸어서
// 표시 하기 위해서 이 함수 안에 구현 한다.
void MPgUtil::ApplyPropertyData(MEntity* pmEntity, String* strPropertyName, Object* pmData)
{
	//// Parent Prop Name  (ex, Animation Object)
	//pkPropertyInterface->GetName();
	//// Child Prop Name  (ex, Phase)
	//strPropertyName;

	if (!pmEntity)
	{
		return;
	}
	NiEntityPropertyInterface* pkPropertyInterface = pmEntity->PropertyInterface;
	const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    NiFixedString kPropertyName = pcPropertyName;
    MFreeCharPointer(pcPropertyName);
    NiFixedString kParent;
	bool bSuccess = NIBOOL_IS_TRUE(pkPropertyInterface->GetCategory(kPropertyName, kParent));
    MAssert(bSuccess, "MEntity Error: Property name not found!");

	if(kParent == gs_pcPropAnimationObject)
	{
		// Animation Object 에 대한 처리!
		if (kPropertyName == gs_pcPropPhase)
		{
			// Phase값에 대한 처리.
			if (pmEntity && pmEntity->GetSceneRootPointer(0))
			{
				__box float* pfData = dynamic_cast<__box float*>(pmData);
				if (pfData == NULL)
				{
					if (pmData == NULL)
					{
						pfData = __box((float)0);
					}
				}
				MAssert(pfData != NULL, "Object type does not match primitive type; "
					"cannot set value.");

				// Set Ani Time.
				float fTime = *pfData;
				MPgUtil::SetTimeToAniObj(pmEntity->GetSceneRootPointer(0), fTime);
			}
		}
		else if(0)
		{
			// 혹시나 Animation Object에 대해 다른 값들이 생기면 이쪽에 추가!
		}
	}
	else if(0)
	{
		// 혹시나 Component가 또 다른 것이 있으면 여기서 처리!
	}
}

bool MPgUtil::HasTimeController(NiAVObject *pkAVObject)
{
	if(NiIsKindOf(NiNode,pkAVObject))
	{
		NiNode *pkNode = NiDynamicCast(NiNode,pkAVObject);
		if(pkNode)
		{
			int iChildCount = pkNode->GetArrayCount();
			for(int i=0;i<iChildCount;i++)
			{
				NiAVObject	*pkChild = pkNode->GetAt(i);
				if(pkChild)
				{
					if(HasTimeController(pkChild)) return true;
				}
			}
		}
	}
	
	if(pkAVObject->GetControllers()) return true;
	return false;
}

//---------------------------------------------------------------------------
// Entity에 TimeController가 있으면, Animation Object Component를 넣어준다.
bool MPgUtil::AddAniComponentToEntity(MEntity* pmEntity, MComponent* pmComponent, bool bIsPalettesEntity)
{
	if (!pmComponent)
	{
		pmComponent = ComponentService->GetComponentByName(gs_pcPropAnimationObject);
		if (!pmComponent)
		{
			String* pmString = gs_pcPropAnimationObject;
			pmString = String::Concat(pmString, "Component가 없습니다..");
			const char* pcString = MStringToCharPointer(pmString);
			::MessageBox(0, pcString, 0, 0);
			MFreeCharPointer(pcString);
			return false;
		}
	}
	MEntity* pmNewEntity = pmEntity;
	if (bIsPalettesEntity)
	{
		Guid kGuid = Guid::NewGuid();
		pmNewEntity = pmEntity->Clone(kGuid.ToString(), true);
	}
	NiAVObject* pkObject = pmNewEntity->GetSceneRootPointer(0);

	bool bForceReturn = false;
	if (!pkObject)
	{
		bForceReturn = true;
	}
	else if(!HasTimeController(pkObject))
	{
		bForceReturn = true;
	}
	if (bIsPalettesEntity)
	{
        MFramework::Instance->EntityFactory->Remove(pmNewEntity->GetNiEntityInterface());
        pmNewEntity = NULL;
	}
	if (bForceReturn)
	{
		return false;
	}

	// Add component
	NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
	if (!pkEntity)
	{
		return false;
	}
	for (unsigned int ui = 0 ; ui < pkEntity->GetComponentCount() ; ++ui)
	{
		if (pkEntity->GetComponentAt(ui)->GetName() == gs_pcPropAnimationObject)
		{
			return false;
		}
	}
	pkEntity->AddComponent(pmComponent->GetNiEntityComponentInterface());

	return true;
}
bool MPgUtil::AddHideObjectComponentToEntity(MEntity* pmEntity, MComponent* pmComponent, bool bIsPalettesEntity)
{
	if (!pmComponent)
	{
		pmComponent = ComponentService->GetComponentByName(gs_pcHideObject);
		if (!pmComponent)
		{
			String* pmString = gs_pcHideObject;
			pmString = String::Concat(pmString, "Component가 없습니다..");
			const char* pcString = MStringToCharPointer(pmString);
			::MessageBox(0, pcString, 0, 0);
			MFreeCharPointer(pcString);
			return false;
		}
	}
	// Add component
	NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
	if (!pkEntity)
	{
		return false;
	}
	for (unsigned int ui = 0 ; ui < pkEntity->GetComponentCount() ; ++ui)
	{
		if (pkEntity->GetComponentAt(ui)->GetName() == gs_pcHideObject)
		{
			return false;
		}
	}
	pkEntity->AddComponent(pmComponent->GetNiEntityComponentInterface());

	return true;
}

//---------------------------------------------------------------------------
// HandOver, ApplyComponentToPalettes, 강정욱 2008.01.29
// 팔레트의 모든 오브젝트에 대해서. AniComponent를 추가 시켜 준다!!!
// 평소 때는 안쓰고 제일 처음 한번 때 쓰려고 만들었음.
bool MPgUtil::AddAniComponentToPalettes()
{
	if (!MFramework::Instance || !MFramework::Instance->Scene)
	{
		return false;
	}

	MComponent* pmComponent = ComponentService->GetComponentByName(gs_pcPropAnimationObject);
	if (!pmComponent)
	{
		String* pmString = gs_pcPropAnimationObject;
		pmString = String::Concat(pmString, "Component가 없습니다..");
		const char* pcString = MStringToCharPointer(pmString);
		::MessageBox(0, pcString, 0, 0);
	    MFreeCharPointer(pcString);
		return false;
	}

	for (unsigned int i=0 ; i<MFramework::Instance->PaletteManager->PaletteCount ; i++)
	{
		MPalette* palette = MFramework::Instance->PaletteManager->GetPalettes()[i];
		for (unsigned int j=0 ; j<palette->EntityCount ; j++)
		{
			MEntity *pmEntity = palette->GetEntities()[j];
			if (!pmEntity)
			{
				continue;
			}

			AddAniComponentToEntity(pmEntity, pmComponent, true);
		}
	}

	return true;
}

//---------------------------------------------------------------------------
// HandOver, Sync Object To Palettes, 강정욱 2008.01.30
// 맵에 찍혀있는 오브젝트들중... 팔레트 부모를 잃어버린 애들을 찾아준다.
//!/
bool MPgUtil::SyncObjectToPalettes()
{
	if (IDOK != ::MessageBox(0, "팔레트의 내용과 맞춥니다.", "Warning", MB_OKCANCEL))
	{
		return false;
	}

	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int iMapObjCount=0 ; iMapObjCount<pkEntities->Count ; iMapObjCount++)
	{
		MEntity* pkMapEntity = pkEntities[iMapObjCount];

		if (!pkMapEntity)
		{
			continue;
		}
		if (!pkMapEntity->MasterEntity)
		{
			continue;
		}

		IEntityPathService *pathService = MGetService(IEntityPathService);
		MPalette *pkPalette = pathService->FindPaletteContainingEntity(pkMapEntity->MasterEntity);

		// 있을 경우는 continue, 없을 경우에만 찾아주자...
		if (pkPalette)
		{
			continue;
		}
		if (!pkMapEntity->HasProperty(gs_pcNIFFilePath))
		{
			continue;
		}
		Object* pmData = pkMapEntity->GetPropertyData(gs_pcNIFFilePath);
		if (!pmData)
		{
			continue;
		}
		String* pkMapFilePath = dynamic_cast<String*>(pmData);
		if (!pkMapFilePath)
		{
			continue;
		}

		MEntity *pmPaletteEntity = NULL;
		String* pkPaletteFilePath;

		for (unsigned int i=0 ; i<MFramework::Instance->PaletteManager->PaletteCount ; i++)
		{
			MPalette* palette = MFramework::Instance->PaletteManager->GetPalettes()[i];
			for (unsigned int j=0 ; j<palette->EntityCount ; j++)
			{
				pmPaletteEntity = palette->GetEntities()[j];
				if (!pmPaletteEntity)
				{
					continue;
				}
				if (!pmPaletteEntity->HasProperty(gs_pcNIFFilePath))
				{
					continue;
				}
				Object* pmData = pmPaletteEntity->GetPropertyData(gs_pcNIFFilePath);
				if (!pmData)
				{
					continue;
				}
				pkPaletteFilePath = dynamic_cast<String*>(pmData);
				if (!pkPaletteFilePath)
				{
					continue;
				}

				if (pkMapFilePath->Equals(pkPaletteFilePath))
				{
					pkMapEntity->MasterEntity = pmPaletteEntity;

					// 쌍for문 나가기.
					i = MFramework::Instance->PaletteManager->PaletteCount;
					j = palette->EntityCount;
					Sleep(1);
					break;
				}

			}
		}

		if( pmPaletteEntity != NULL)// 찾아놓은 팔레트 속성으로 맵의 다른 것도 찾아서 바꾼다.
		{
			MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
			for (int iMapObjCount2=iMapObjCount ; iMapObjCount2<pkEntities->Count ; iMapObjCount2++)
			{
				MEntity* pkMapEntity = pkEntities[iMapObjCount2];

				if (!pkMapEntity)
				{
					continue;
				}
				if (!pkMapEntity->MasterEntity)
				{
					continue;
				}

				IEntityPathService *pathService = MGetService(IEntityPathService);
				MPalette *pkPalette = pathService->FindPaletteContainingEntity(pkMapEntity->MasterEntity);

				// 있을 경우는 continue, 없을 경우에만 찾아주자...
				if (pkPalette)
				{
					continue;
				}
				if (!pkMapEntity->HasProperty(gs_pcNIFFilePath))
				{
					continue;
				}
				Object* pmData = pkMapEntity->GetPropertyData(gs_pcNIFFilePath);
				if (!pmData)
				{
					continue;
				}
				String* pkMapFilePath = dynamic_cast<String*>(pmData);
				if (!pkMapFilePath)
				{
					continue;
				}

				if (pkMapFilePath->Equals(pkPaletteFilePath))
				{
					pkMapEntity->MasterEntity = pmPaletteEntity;
				}
			}
		}
	}

	::MessageBox(0, "Complete! Sync Object To Palettes", "Complete", 0);
	return true;
}
//---------------------------------------------------------------------------
// 맵상의 오브젝트, 트라이앵글 카운트를 센다.
void MPgUtil::GetAllTriangleCount(int& _nTriangleCount, int& _nObjectCount)
{//!/
	int nTriCount = 0;
	int nObjCount = 0;
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for(int nn=0; nn<pkEntities->Count; ++nn)
	{
		MEntity *pkEntity = pkEntities[nn];
		if(pkEntity)
		{
			NiObjectList kGeometries;
			for(unsigned int gg=0; gg<pkEntity->GetSceneRootPointerCount(); ++gg)
			{
				NiNode* pkNode = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(gg));
				if(pkNode)
				{
					pkEntity->GetAllGeometries(pkNode, kGeometries);
					nObjCount += 1;
				}
			}
			while(!kGeometries.IsEmpty())
			{
				NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, kGeometries.GetTail());
				if(pkGeometry)
				{
					if(NiIsKindOf(NiTriStrips, pkGeometry))
					{
						NiTriStrips* pkTri = NiDynamicCast(NiTriStrips, pkGeometry);
						nTriCount += pkTri->GetTriangleCount();
					}
					else if(NiIsKindOf(NiTriShape, pkGeometry))
					{
						NiTriShape* pkTri = NiDynamicCast(NiTriShape, pkGeometry);
						nTriCount += pkTri->GetTriangleCount();
					}
				}

				kGeometries.RemoveTail();
			}
		}
	}

	_nTriangleCount = nTriCount;
	_nObjectCount = nObjCount;
}
void MPgUtil::UpdateTriangleCount()
{
	// recount triangle
	//if(MFramework::Instance->ViewportManager &&
	//	MFramework::Instance->ViewportManager->get_ActiveViewport())
	//{
	//	MFramework::Instance->ViewportManager->get_ActiveViewport()->UpdateScreenConsole();
	//}

	if(MFramework::Instance->ViewportManager)
	{
		unsigned int nCount = MFramework::Instance->ViewportManager->ViewportCount;
		for(unsigned int nn=0; nn<nCount; ++nn)
		{
			MViewport* pViewport = MFramework::Instance->ViewportManager->GetViewport(nn);
			pViewport->UpdateScreenConsole();
		}
	}
}
// 로컬 함수
void MPgUtil::RecursiveAnimationType(NiAVObject* pkObject)
{
	NiTimeController* pkControl = pkObject->GetControllers();
	if(pkControl)
	{
		for (/**/; pkControl; pkControl = pkControl->GetNext())
		{
			pkControl->SetCycleType(NiTimeController::LOOP);
			pkControl->SetAnimType(NiTimeController::APP_TIME);
		}
	}


	NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
	if (pkNode)
	{        
		for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
		{
			NiAVObject* pkChild = pkNode->GetAt(i);
			if (pkChild)
				RecursiveAnimationType(pkChild);
		}
	}
}
void MPgUtil::ChangeAnimationType()
{
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for(int nn=0; nn<pkEntities->Count; ++nn)
	{
		MEntity *pkEntity = pkEntities[nn];
		if(pkEntity)
		{
			for(unsigned int gg=0; gg<pkEntity->GetSceneRootPointerCount(); ++gg)
			{
				NiNode* pkNode = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(gg));
				if(pkNode)
				{
					RecursiveAnimationType(pkNode);
				}
			}
		}
	}
}

NiCamera* MPgUtil::FindCamera(NiAVObject* pObject)
{
	NiCamera* pkCamera = NULL;

	if(NiIsKindOf(NiCamera, pObject))
	{
		pkCamera = NiDynamicCast(NiCamera, pObject);
	}

	NiNode* pkNode = NiDynamicCast(NiNode, pObject);
	if(pkNode)
	{
		for(unsigned int i=0; i<pkNode->GetArrayCount(); ++i)
		{
			NiAVObject* pkChild = pkNode->GetAt(i);
			if(pkChild)
			{
				NiCamera* pkCameraChild;
				pkCameraChild = FindCamera(pkChild);
				if(!pkCamera)
					pkCamera = pkCameraChild;
			}
		}
	}

	return pkCamera;
}

std::wstring MPgUtil::ConvToUnicode(std::string const& strSrc)
{
	std::wstring strOut;
	MPgUtil::ConvToUnicode(strSrc, strOut);
	return strOut;
}

errno_t MPgUtil::ConvToUnicode(std::string const &strSrc, std::wstring& wstrTgt)
{
	size_t const src_size = strSrc.size();
	if(0 < src_size)
	{
		std::wstring wstrTemp;
		wstrTemp.resize( src_size*4 );

		size_t size = 0;
		const errno_t err = ::mbstowcs_s( &size, &wstrTemp.at(0), wstrTemp.size(), strSrc.c_str(), src_size );
		if( !err && size )
		{
			wstrTgt = wstrTemp.c_str();
		}

		return err;
	}
	return EINVAL;
}

std::string MPgUtil::ConvToMultiByte(std::wstring const &wstrSrc)
{
	std::string strOut;
	MPgUtil::ConvToMultiByte(wstrSrc, strOut);
	return strOut;
}

errno_t MPgUtil::ConvToMultiByte(std::wstring const& wstrSrc, std::string &strTgt)
{
	size_t const src_size = wstrSrc.size();
	if(0 < src_size)
	{
		std::string strTemp;
		strTemp.resize( src_size*4 );

		size_t size = 0;
		const errno_t err = ::wcstombs_s( &size,  &strTemp.at(0), strTemp.size(), wstrSrc.c_str(),   strTemp.size() );
		if( !err && size )
		{
			strTgt = strTemp.c_str();
		}
		return err;
	}

	return EINVAL;
}