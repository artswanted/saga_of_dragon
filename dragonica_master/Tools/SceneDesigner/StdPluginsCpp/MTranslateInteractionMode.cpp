// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MTranslateInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

#define PI	(3.141592653589793238462643f)

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MTranslateInteractionMode::MTranslateInteractionMode() : m_pkXAxisName(NULL),
    m_pkYAxisName(NULL), m_pkZAxisName(NULL), m_pkXYPlaneName(NULL), 
    m_pkXZPlaneName(NULL), m_pkYZPlaneName(NULL), m_pkXLineName(NULL), 
    m_pkYLineName(NULL), m_pkZLineName(NULL), m_pkXYLineName(NULL), 
    m_pkXZLineName(NULL), m_pkYXLineName(NULL), m_pkYZLineName(NULL), 
    m_pkZXLineName(NULL), m_pkZYLineName(NULL), 
    m_pkHighLightColor(NULL), m_pkStartPoint(NULL), 
    m_pkInitialTranslation(NULL), m_pkInitialRotation(NULL), 
    m_pmPreviousSelection(NULL), m_pmCloneArray(NULL), m_pkCloneCenter(NULL),
	m_pkFinalSelectRecord(NULL), m_bFoundSnapSrcPoint(false), m_pkSnapSrcPoint(NULL),
	m_pkSrcNearestPoint(NULL), m_pkSrcNearPoint(NULL),
	m_pkSnapSrcLineVerts(NULL),	m_pkSnapSrcLineMarker(NULL),m_pkSrcRotation(NULL),
	m_pkSrcTranslate(NULL), m_pkDestNearestPoint(NULL), m_pkDestNearPoint(NULL),
	m_pkSrcMultiRotation(NULL)
{
    m_pkXAxisName = NiNew NiFixedString("XAxis");
    m_pkYAxisName = NiNew NiFixedString("YAxis");
    m_pkZAxisName = NiNew NiFixedString("ZAxis");
    m_pkXYPlaneName = NiNew NiFixedString("XYPlane");
    m_pkXZPlaneName = NiNew NiFixedString("XZPlane");
    m_pkYZPlaneName = NiNew NiFixedString("YZPlane");
    m_pkXLineName = NiNew NiFixedString("XLine");
    m_pkYLineName = NiNew NiFixedString("YLine");
    m_pkZLineName = NiNew NiFixedString("ZLine");
    m_pkXYLineName = NiNew NiFixedString("XYLine");
    m_pkXZLineName = NiNew NiFixedString("XZLine");
    m_pkYXLineName = NiNew NiFixedString("YXLine");
    m_pkYZLineName = NiNew NiFixedString("YZLine");
    m_pkZXLineName = NiNew NiFixedString("ZXLine");
    m_pkZYLineName = NiNew NiFixedString("ZYLine");
    m_pkInitialTranslation = NiNew NiTObjectSet<NiPoint3>(32);
    m_pkInitialRotation = NiNew NiTObjectSet<NiMatrix3>(32);
    m_pmPreviousSelection = new ArrayList(1);
    m_pmCloneArray = new ArrayList(1);
    m_pkCloneCenter = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);

    NiDelete m_pkXAxisName;
    NiDelete m_pkYAxisName;
    NiDelete m_pkZAxisName;
    NiDelete m_pkXYPlaneName;
    NiDelete m_pkXZPlaneName;
    NiDelete m_pkYZPlaneName;
    NiDelete m_pkXLineName;
    NiDelete m_pkYLineName;
    NiDelete m_pkZLineName;
    NiDelete m_pkXYLineName;
    NiDelete m_pkXZLineName;
    NiDelete m_pkYXLineName;
    NiDelete m_pkYZLineName;
    NiDelete m_pkZXLineName;
    NiDelete m_pkZYLineName;
    m_pkInitialTranslation->RemoveAll();
    NiDelete m_pkInitialTranslation;
    m_pkInitialRotation->RemoveAll();
    NiDelete m_pkInitialRotation;

    NiDelete m_pkHighLightColor;
    NiDelete m_pkPick;
    NiDelete m_pkStartPoint;
    NiDelete m_pkStartPick;

    NiDelete m_pkCloneCenter;
	
	
	NiDelete m_pkSnapSrcPoint;
	MDisposeRefObject(m_pkSnapSrcMarker);
	NiDelete m_pkSnapDestPoint;
	MDisposeRefObject(m_pkSnapSrcLineMarker);

	NiDelete m_pkSrcNearestPoint;
	NiDelete m_pkSrcNearPoint;
	NiDelete m_pkDestNearestPoint;
	NiDelete m_pkDestNearPoint;
	NiDelete m_pkSrcRotation;
	//NiDelete m_pkSrcMultiRotation;
	NiDelete m_pkSrcTranslate;
	if (m_pkSrcMultiRotation)
		NiDelete[] m_pkSrcMultiRotation;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MTranslateInteractionMode::OnSettingChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::HIGHLIGHT_COLOR,
        *m_pkHighLightColor, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_SNAP,
        m_fSnapSpacing, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED, m_bSnapEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::TRANSLATION_PRECISION,
        m_fPrecision, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, m_bPrecisionEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED, m_bSnapToPickEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_POINT_ENABLED, m_bSnapToPointPickEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED, m_bRotateToPickEnabled, 
        pmHandler);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::HIGHLIGHT_COLOR))
    {
        Object* pmObj;
        __box Color* pmValue;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        pmValue = dynamic_cast<__box Color*>(pmObj);
        if (pmValue != NULL)
        {
            m_pkHighLightColor->r = MUtility::RGBToFloat((*pmValue).R);
            m_pkHighLightColor->g = MUtility::RGBToFloat((*pmValue).G);
            m_pkHighLightColor->b = MUtility::RGBToFloat((*pmValue).B);
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fSnapSpacing = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fPrecision = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bPrecisionEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToPickEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SNAP_TO_POINT_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToPointPickEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bRotateToPickEnabled = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
String* MTranslateInteractionMode::get_Name()
{
    return "GamebryoTranslation";
}
//---------------------------------------------------------------------------
bool MTranslateInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_eAxis = TranslateAxis::PLANE_XY;
    m_eCurrentAxis = TranslateAxis::PLANE_XY;
    m_bOnGizmo = false;
    m_bAlreadyTranslating = false;
    m_fDefaultDistance = STANDARD_DISTANCE;
    m_fSnapSpacing = 1.0f;
    m_bSnapEnabled = true;
    m_fPrecision = 0.1f;
    m_bPrecisionEnabled = true;
    m_bSnapToPickEnabled = false;
    m_bRotateToPickEnabled = false;
    m_usAlignFacingAxis = 2;
    m_usAlignUpAxis = 1;
    m_fStartScale = 1.0f;

    m_pkHighLightColor = NiNew NiColor(1.0f, 1.0f, 0.0f);
    m_pkStartPoint = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
    m_pkStartPick = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
    m_pkPick = NiNew NiPick();
    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "Data\\translate.nif");
    NiStream kStream;
    const char* pcPath = MStringToCharPointer(pmPath);
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcPath);
    MFreeCharPointer(pcPath);

	////
	// Snap Point 관련
	m_bFoundSnapSrcPoint = false;
	m_pkSnapSrcPoint = NiNew NiPoint3(0.0f, 0.0f, 0.0f);
	m_pkSnapDestPoint = NiNew NiPoint3(0.0f, 0.0f, 0.0f);

	
	NiBool* pbMinorConnections;
	pbMinorConnections = NiAlloc(NiBool, 4);
	pbMinorConnections[0] = true;
	pbMinorConnections[1] = true;
	pbMinorConnections[2] = true;
	pbMinorConnections[3] = true;
	m_pkSnapSrcMarkerVerts = NiNew NiPoint3[4];
	m_pkSnapSrcMarker = NiNew NiLines(4, m_pkSnapSrcMarkerVerts, 0, 0, 0, NiGeometryData::NBT_METHOD_NONE, pbMinorConnections);
    MInitRefObject(m_pkSnapSrcMarker);
	m_pkSnapSrcMarker->UpdateProperties();
	m_pkSnapSrcMarker->UpdateEffects();
	m_pkSnapSrcMarker->Update(0.0f);

	NiBool* pbMinorConnections2;
	pbMinorConnections2 = NiAlloc(NiBool, 2);
	pbMinorConnections2[0] = true;
	pbMinorConnections2[1] = true;
	m_pkSnapSrcLineVerts = NiNew NiPoint3[2];
	m_pkSnapSrcMarkerVerts[0] = NiPoint3(0, 0, -5.0f);
	m_pkSnapSrcMarkerVerts[1] = NiPoint3(0, 0, 5.0f);
	m_pkSnapSrcMarkerVerts[2] = NiPoint3(-5.0f, 0, 0);
	m_pkSnapSrcMarkerVerts[3] = NiPoint3(5.0f, 0, 0);	
	m_pkSnapSrcLineMarker = NiNew NiLines(2, m_pkSnapSrcLineVerts, 0, 0, 0, NiGeometryData::NBT_METHOD_NONE, pbMinorConnections2);
    MInitRefObject(m_pkSnapSrcLineMarker);
	m_pkSnapSrcLineMarker->Update(0.0f);
	m_pkSnapSrcLineMarker->UpdateProperties();
	m_pkSnapSrcLineMarker->UpdateEffects();
	m_pkSnapSrcLineMarker->UpdateNodeBound();

	NiMaterialProperty* pkGridMaterial;
	pkGridMaterial = NiNew NiMaterialProperty();
	pkGridMaterial->SetAmbientColor(NiColor::WHITE);
	pkGridMaterial->SetDiffuseColor(NiColor::WHITE);
	pkGridMaterial->SetSpecularColor(NiColor::WHITE);
	pkGridMaterial->SetEmittance(NiColor::WHITE);
	pkGridMaterial->SetShineness(0.0f);
	pkGridMaterial->SetAlpha(1.0f);
	m_pkSnapSrcMarker->AttachProperty(pkGridMaterial);
	m_pkSnapSrcMarker->Update(0.0f);
	m_pkSnapSrcMarker->UpdateProperties();
	m_pkSnapSrcMarker->UpdateEffects();
	m_pkSnapSrcMarker->UpdateNodeBound();
	
	//NiDelete pkGridMaterial;

	pkGridMaterial = NiNew NiMaterialProperty();
	pkGridMaterial->SetAmbientColor(NiColor::NiColor(1, 0, 0));
	pkGridMaterial->SetDiffuseColor(NiColor::NiColor(1, 0, 0));
	pkGridMaterial->SetSpecularColor(NiColor::NiColor(1, 0, 0));
	pkGridMaterial->SetEmittance(NiColor::NiColor(1, 0, 0));
	pkGridMaterial->SetShineness(0.0f);
	pkGridMaterial->SetAlpha(1.0f);
	m_pkSnapSrcLineMarker->AttachProperty(pkGridMaterial);
	m_pkSnapSrcLineMarker->UpdateProperties();
	m_pkSnapSrcLineMarker->UpdateEffects();
	m_pkSnapSrcLineMarker->Update(0.0f);
	//NiDelete pkGridMaterial;

	m_pkSrcNearestPoint = NiNew NiPoint3();
	m_pkSrcNearPoint = NiNew NiPoint3();
	m_pkDestNearestPoint = NiNew NiPoint3();
	m_pkDestNearPoint = NiNew NiPoint3();
	m_pkSrcRotation = NiNew NiMatrix3();
	m_pkSrcTranslate = NiNew NiPoint3();
	m_pkSrcRotation->IDENTITY;



    if (iSuccess != 0)
    {
        m_pkGizmo = (NiNode*)kStream.GetObjectAt(0);
        MInitRefObject(m_pkGizmo);
        m_pkGizmo->UpdateProperties();
        m_pkGizmo->UpdateEffects();
        m_pkGizmo->Update(0.0f);
    }
    else
    {
        MessageService->AddMessage(MessageChannelType::Errors,
            "Failed to load translation gizmo file 'translation.nif'");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::SetInteractionMode(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::ValidateInteractionMode(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
bool MTranslateInteractionMode::CanTransform()
{
    MEntity* amEntities[] = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amEntities->Count; i++)
    {
        if (amEntities[i] != NULL &&
            amEntities[i]->HasProperty(*m_pkTranslationName) &&
            amEntities[i]->HasProperty(*m_pkRotationName))
        {
            if (!amEntities[i]->IsPropertyReadOnly(*m_pkTranslationName) &&
                !amEntities[i]->IsPropertyReadOnly(*m_pkRotationName))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiPoint3 GetNearestVertexPoint(NiPick::Record* pRecord, unsigned short *pIndex = NULL, NiPoint3 *pkException=NULL);
NiPoint3 GetNearestVertexPoint(NiPick::Record* pRecord, unsigned short *pIndex, NiPoint3 *pkException)
{
	// 물체(Record : 폴리곤)와 마우스와 가장 근접한 버텍스를 찾는다.
	NiPoint3 kSnapPoint;
	// 가장 근접한 triangle과 그 것을 이루는 vectex의 index를 받는다.
	unsigned short vi[3];
	//unsigned short ti = pRecord->GetTriangleIndex();
	pRecord->GetVertexIndices(vi[0], vi[1], vi[2]);
	NiGeometry *pkGeo = NiDynamicCast(NiGeometry, pRecord->GetAVObject());
	// 그 vertex중 충돌점과 가장 가까운 vertex를 snap-point로 선정한다.
	if(pkGeo)
	{
		float fMinDist = 99999.9f;
		for(int i = 0; i < 3; i++)
		{
			NiPoint3 pkPoint = pkGeo->GetModelData()->GetVertices()[vi[i]];
			NiPoint3 kPoint = pkGeo->GetWorldTransform() * pkPoint;
			if (pkException && *pkException == kPoint)
				continue;

			//NiPoint3 pkPick = pRecord->GetIntersection();
			float fDist = (kPoint - pRecord->GetIntersection()).Length();

			if(fDist < fMinDist)
			{
				fMinDist = fDist;
				kSnapPoint = pkGeo->GetWorldTransform() * pkPoint;
				if (pIndex)
					*pIndex = vi[i];
			}
		}
	}

	return kSnapPoint;
}

//---------------------------------------------------------------------------
void MTranslateInteractionMode::TranslateHelper(
    const NiPoint3* pkOrigin, const NiPoint3* pkDir)
{
    MVerifyValidInstance;

    NiPoint3 kTranslation;
    NiMatrix3 kRotation;
	NiMatrix3 kSnapLineRot;
	bool bSnapLine = false;
    NiPoint3 kAxis;
    bool bPicked;

    if ((m_eAxis == AXIS_X) || (m_eAxis == PLANE_YZ))
        kAxis = NiPoint3::UNIT_X;
    else if ((m_eAxis == AXIS_Y) || (m_eAxis == PLANE_XZ))
        kAxis = NiPoint3::UNIT_Y;
    else if ((m_eAxis == AXIS_Z) || (m_eAxis == PLANE_XY))
        kAxis = NiPoint3::UNIT_Z;

    if ((m_eAxis == AXIS_X) || (m_eAxis == AXIS_Y) || (m_eAxis == AXIS_Z))
    {
        kTranslation = NiViewMath::TranslateOnAxis(*m_pkStartPick, kAxis, 
            *pkOrigin, *pkDir);
    }
    else if ((m_eAxis == PLANE_XY) || (m_eAxis == PLANE_XZ) ||
        (m_eAxis == PLANE_YZ))
    {
        kTranslation = NiViewMath::TranslateOnPlane(*m_pkStartPick, kAxis, 
            *pkOrigin, *pkDir);
    }

    kRotation = NiMatrix3::IDENTITY;
    bPicked = false;

    if (m_bSnapEnabled)
    {
        // if snap is enabled, alter kNewTranslation such that we round 
        // to the nearest m_fSnapSpacing
        kTranslation /= m_fSnapSpacing;
        kTranslation.x = NiFloor(kTranslation.x + 0.5f);
        kTranslation.y = NiFloor(kTranslation.y + 0.5f);
        kTranslation.z = NiFloor(kTranslation.z + 0.5f);
        kTranslation *= m_fSnapSpacing;
    }

    if (m_bSnapToPickEnabled && !m_bSnapToPointPickEnabled)
    {
        // perform a pick, and use the delta between that and the starting pt
        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->Scene, *pkOrigin, *pkDir, true))
        {
            const NiPick* pkPick;
            pkPick = MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
            NiPick::Record* pkFinalRecord = NULL;
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            for (unsigned int ui = 0; ui < kPickResults.GetSize(); ui++)
            {
                NiPick::Record* pkPickRecord = kPickResults.GetAt(ui);
                if (pkPickRecord != NULL)
                {
                    NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                    MEntity* pmPickedEntity = MFramework::Instance->
                        PickUtility->GetEntityFromPickedObject(pkPickedObject);
                    bool bIsSelected = false;
                    for (int j = 0; j < amEntities->Count; j++)
                    {
                        if (pmPickedEntity == amEntities[j])
                        {
                            // the picked entity is selected; ignore it
                            bIsSelected = true;
                            break;
                        }
                    }
                    if (!bIsSelected)
                    {
                        pkFinalRecord = pkPickRecord;
                        bPicked = true;
                        break;
                    }
                }
            }
            if (pkFinalRecord)
            {
				// 면에 붙이기.
				kTranslation = pkFinalRecord->GetIntersection() - 
					*m_pkStartPoint;

                if (m_bRotateToPickEnabled)
                {
                    NiPoint3 kNormal;
                    NiPoint3 kUpAxis;	
                    
					kNormal = pkFinalRecord->GetNormal();
                    kUpAxis = *m_pkUpAxis;
                    // first check if model's up and facing axis are parallel
                    if ((m_usAlignUpAxis - m_usAlignFacingAxis) % 3 == 0)
                    {
                        m_usAlignUpAxis += 1;
                    }
                    if ((kNormal.Dot(kUpAxis) > NiViewMath::PARALLEL_THRESHOLD)
                        || (kNormal.Dot(kUpAxis) < 
                        -NiViewMath::PARALLEL_THRESHOLD))
                    {
                        if ((kNormal.Dot(NiPoint3::UNIT_Z) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Z) > 
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Z;
                        }
                        else if ((kNormal.Dot(NiPoint3::UNIT_Y) < 
                            NiViewMath::PARALLEL_THRESHOLD) &&
                            (kNormal.Dot(NiPoint3::UNIT_Y) >
                            -NiViewMath::PARALLEL_THRESHOLD))
                        {
                            kUpAxis = NiPoint3::UNIT_Y;
                        }
                        else
                        {
                            kUpAxis = NiPoint3::UNIT_X;
                        }
                    }
                    while (m_usAlignFacingAxis > 5)
                        m_usAlignFacingAxis -= 6;
                    while (m_usAlignUpAxis > 5)
                        m_usAlignUpAxis -= 6;
                    if (m_usAlignFacingAxis > 2)
                    {
                        m_usAlignFacingAxis -= 3;
                        kNormal = -kNormal;
                    }
                    if (m_usAlignUpAxis > 2)
                    {
                        m_usAlignUpAxis -= 3;
                        kUpAxis = -kUpAxis;
                    }

                    // calculate rotation matrix
                    NiPoint3 kFrameX;
                    NiPoint3 kFrameY;
                    NiPoint3 kFrameZ;
                    if (m_usAlignFacingAxis == 0)
                    {
                        if (m_usAlignUpAxis == 1)
                        {
                            // we want X axis to face N and Y to face up
                            kFrameX = kNormal;
                            kFrameZ = kFrameX.Cross(kUpAxis);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                        else
                        {
                            // we want X axis to face N and Z to face up
                            kFrameX = kNormal;
                            kFrameY = kUpAxis.Cross(kFrameX);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else if (m_usAlignFacingAxis == 1)
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Y axis to face N and X to face up
                            kFrameY = kNormal;
                            kFrameZ = kUpAxis.Cross(kFrameY);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Y axis to face N and Z to face up
                            kFrameY = kNormal;
                            kFrameX = kFrameY.Cross(kUpAxis);
                            kFrameZ = kFrameX.Cross(kFrameY);
                        }
                    }
                    else
                    {
                        if (m_usAlignUpAxis == 0)
                        {
                            // we want Z axis to face N and X to face up
                            kFrameZ = kNormal;
                            kFrameY = kFrameZ.Cross(kUpAxis);
                            kFrameX = kFrameY.Cross(kFrameZ);
                        }
                        else
                        {
                            // we want Z axis to face N and Y to face up
                            kFrameZ = kNormal;
                            kFrameX = kUpAxis.Cross(kFrameZ);
                            kFrameY = kFrameZ.Cross(kFrameX);
                        }
                    }
                    kRotation.SetCol(0, kFrameX);
                    kRotation.SetCol(1, kFrameY);
                    kRotation.SetCol(2, kFrameZ);
                    kRotation.Reorthogonalize();
                }
            }
        }
    }

	
	// HandOver, 스냅, 강정욱 2008.01.29
	// 스냅, 활성화시 점과 점끼리 붙이는 기능.
    if (m_bSnapToPointPickEnabled)
    {
        // perform a pick, and use the delta between that and the starting pt
        if (MFramework::Instance->PickUtility->PerformPick(
            MFramework::Instance->Scene, *pkOrigin, *pkDir, true))
        {
			// 마우스를 피킹 하여 가장 가까운 폴리곤을 찾는다.
            const NiPick* pkPick;
            pkPick = MFramework::Instance->PickUtility->GetNiPick();
            const NiPick::Results& kPickResults = pkPick->GetResults();
			NiPick::Record* pkFinalRecord = NULL;
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
            for (unsigned int ui = 0; ui < kPickResults.GetSize(); ui++)
            {
                NiPick::Record* pkPickRecord = kPickResults.GetAt(ui);
                if (pkPickRecord != NULL)
                {
                    NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                    MEntity* pmPickedEntity = MFramework::Instance->
                        PickUtility->GetEntityFromPickedObject(pkPickedObject);
                    bool bIsSelected = false;
                    for (int j = 0; j < amEntities->Count; j++)
                    {
                        if (pmPickedEntity == amEntities[j])
                        {
                            // the picked entity is selected; ignore it
                            bIsSelected = true;
                            break;
                        }
                    }
                    if (!bIsSelected)
                    {
                        pkFinalRecord = pkPickRecord;
						m_pkPickDestRecord = pkFinalRecord;
						if (!m_bRotateToPickEnabled)
							bPicked = true;
                        break;
                    }
                }
            }

			if (pkFinalRecord && m_bFoundSnapSrcPoint)
            {
				NiPoint3 kSnapPoint = GetNearestVertexPoint(pkFinalRecord);
				*m_pkSnapDestPoint = kSnapPoint;
				kTranslation = kSnapPoint - *m_pkSnapSrcPoint;
			}
			else
			{
				m_pkPickDestRecord = NULL;
			}

			// HandOver, 스냅(라인), 강정욱 2008.01.29
			// 라인 스냅을 한다.
			if (m_bSnapToPickEnabled && m_pkPickDestRecord && m_bRotateToPickEnabled)
			{
				NiPoint3 kVectorSrc = *m_pkSrcNearPoint - *m_pkSrcNearestPoint;
				// Dest
				*m_pkDestNearestPoint = GetNearestVertexPoint(m_pkPickDestRecord);
				*m_pkDestNearPoint = GetNearestVertexPoint(m_pkPickDestRecord, NULL, m_pkDestNearestPoint);

				NiPoint3 kVectorDest = *m_pkDestNearPoint - *m_pkDestNearestPoint;

				NiPoint3 kNormVectorSrc = kVectorSrc;
				kNormVectorSrc.Unitize();
				NiPoint3 kNormVectorDest = kVectorDest;
				kNormVectorDest.Unitize();

				float fDotValue = kNormVectorSrc.Dot(kNormVectorDest);

				if (fDotValue <= 1.0f && fDotValue >= -1.0f)
				{
					// 내적으로 각 구하기. 
					float fRadian = acos(fDotValue);
					float fAngle = fRadian * 180.0f / PI;

					NiPoint3 ptAxisVec = kVectorSrc.UnitCross(kVectorDest);

					NiQuaternion kQuat(fRadian, ptAxisVec);
					kQuat.Normalize();
					
					kQuat.ToRotation(kSnapLineRot);
					kRotation = kSnapLineRot;

					bSnapLine = true;
					bPicked = true;
				}
			}
		}
	}


    if (m_bCloning)
    {
        // if cloning, transform clones
        NiPoint3 kNewPosition;
        bool bMultipleSelected;
        bMultipleSelected = (m_pmCloneArray->Count > 1);
        unsigned int uiIndex = 0;
        for (int i = 0; i < m_pmCloneArray->Count; i++)
        {
            MEntity* pmEntity = dynamic_cast<MEntity*>(
                m_pmCloneArray->Item[i]);
            if (pmEntity == NULL ||
                !pmEntity->HasProperty(*m_pkTranslationName) ||
                !pmEntity->HasProperty(*m_pkRotationName))
            {
                continue;
            }

			// 개개인 물체마다 각, 이동을 적용시켜줘야 한다.
			NiPoint3 kTransl;
			kTransl = kTranslation;
			if (m_bSnapToPickEnabled &&
				m_pkPickDestRecord &&
				m_bRotateToPickEnabled &&
				bSnapLine)
			{
				NiPoint3 kBeforePoint = *m_pkSrcNearestPoint - m_pkInitialTranslation->GetAt(uiIndex);
				NiPoint3 kAfterPoint = kSnapLineRot * kBeforePoint;
				kTransl -= kAfterPoint - kBeforePoint;
			}

            NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();
            kNewPosition = m_pkInitialTranslation->GetAt(uiIndex) + 
                kTransl;
            if ((m_bSnapToPickEnabled) && (m_bRotateToPickEnabled))
            {
                if (bPicked)
                {
                    // do additional transformation
                    if (bMultipleSelected &&
						!m_bSnapToPointPickEnabled)
                    {
                        NiPoint3 kDelta;
                        kDelta = m_pkInitialTranslation->GetAt(uiIndex) - 
                            *m_pkStartPoint;
                        kDelta = kRotation * kDelta;
                        kNewPosition = *m_pkStartPoint + kTransl +
                            kDelta;

                        NiMatrix3 kNewRotation;
                        kNewRotation = kRotation * 
                            m_pkInitialRotation->GetAt(uiIndex);
                        pkEntity->SetPropertyData(*m_pkRotationName, 
                            kNewRotation);
                    }
                    else
                    {
						NiMatrix3 kNewRotation;
						NiMatrix3 kRot;

						if (m_pkSrcMultiRotation)
							kNewRotation = kRotation * m_pkSrcMultiRotation[i];
						else
							kNewRotation = kRotation;
						pkEntity->SetPropertyData(*m_pkRotationName, 
                            kNewRotation);
                    }
                }
                else
                {
                    pkEntity->SetPropertyData(*m_pkRotationName, 
                        m_pkInitialRotation->GetAt(uiIndex));
                }
            }
            if (m_bPrecisionEnabled)
            {
                kNewPosition /= m_fPrecision;
                kNewPosition.x = NiFloor(kNewPosition.x + 0.5f);
                kNewPosition.y = NiFloor(kNewPosition.y + 0.5f);
                kNewPosition.z = NiFloor(kNewPosition.z + 0.5f);
                kNewPosition *= m_fPrecision;
            }
            pkEntity->SetPropertyData(*m_pkTranslationName, kNewPosition);
            uiIndex++;
        }
        *m_pkCloneCenter = *m_pkStartPoint + kTranslation;
    }
    else
    {
        // if not cloning, transform selection
        MEntity* amEntities[] = SelectionService->GetSelectedEntities();
        bool bMultipleSelected;
        bMultipleSelected = (amEntities->Count > 1);
        unsigned int uiIndex = 0;
        for (int i = 0; i < amEntities->Count; i++)
        {
			NiMatrix3 kRotationBackup;

            if (amEntities[i] != NULL &&
                amEntities[i]->HasProperty(*m_pkTranslationName) &&
                amEntities[i]->HasProperty(*m_pkRotationName))
            {
                NiEntityInterface* pkEntity = amEntities[i]->
                    GetNiEntityInterface();

				// 스냅 라인 이라면 개개인 물체마다 각, 이동을 적용시켜줘야 한다.
				NiPoint3 kTransl;
				kTransl = kTranslation;
				if (m_bSnapToPickEnabled &&
					m_pkPickDestRecord &&
					m_bRotateToPickEnabled &&
					bSnapLine)
				{
					NiPoint3 kTest = m_pkInitialTranslation->GetAt(uiIndex);
					kTest = m_pkInitialRotation->GetAt(uiIndex) * kTest;
					NiPoint3 kBeforePoint = *m_pkSrcNearestPoint - m_pkInitialTranslation->GetAt(uiIndex);
					NiPoint3 kAfterPoint = kSnapLineRot * kBeforePoint;
					kTransl -= kAfterPoint - kBeforePoint;
				}
                
				NiPoint3 kNewPosition;
                kNewPosition = m_pkInitialTranslation->GetAt(uiIndex) + 
                    kTransl;
                if ((m_bSnapToPickEnabled) && (m_bRotateToPickEnabled))
                {
                    if (bPicked)
                    {
                        // do additional transformation
                        if (bMultipleSelected &&
							!m_bSnapToPointPickEnabled)
                        {
                            NiPoint3 kDelta;
                            kDelta = m_pkInitialTranslation->GetAt(uiIndex) - 
                                *m_pkStartPoint;
                            kDelta = kRotation * kDelta;
                            kNewPosition = *m_pkStartPoint + kTransl 
                                + kDelta;

                            NiMatrix3 kNewRotation;
                            kNewRotation = kRotation * 
                                m_pkInitialRotation->GetAt(uiIndex);
                            pkEntity->SetPropertyData(*m_pkRotationName, 
                                kNewRotation);
                        }
		                else
                        {
							NiMatrix3 kNewRotation;
							NiMatrix3 kRot;
							if (m_pkSrcMultiRotation)
								kNewRotation = kRotation * m_pkSrcMultiRotation[i];
							else
								kNewRotation = kRotation;
							pkEntity->SetPropertyData(*m_pkRotationName, 
                                kNewRotation);
						}
                    }
                    else
                    {
                        pkEntity->SetPropertyData(*m_pkRotationName,
                            m_pkInitialRotation->GetAt(uiIndex));
                    }
                }
                if (m_bPrecisionEnabled)
                {
                    kNewPosition /= m_fPrecision;
                    kNewPosition.x = NiFloor(kNewPosition.x + 0.5f);
                    kNewPosition.y = NiFloor(kNewPosition.y + 0.5f);
                    kNewPosition.z = NiFloor(kNewPosition.z + 0.5f);
                    kNewPosition *= m_fPrecision;
                }
                pkEntity->SetPropertyData(*m_pkTranslationName, kNewPosition);
                uiIndex++;
            }
        }

        NiPoint3 kNewPosition = *m_pkStartPoint + kTranslation;
        SelectionService->SelectionCenter->X = kNewPosition.x;
        SelectionService->SelectionCenter->Y = kNewPosition.y;
        SelectionService->SelectionCenter->Z = kNewPosition.z;
    }

	if (m_bFoundSnapSrcPoint)
	{
		NiPoint3 kCenterPoint;
		if (bSnapLine)
		{
			kCenterPoint = *m_pkDestNearestPoint;
		}
		else
		{
			kCenterPoint = *m_pkSnapSrcPoint + kTranslation;
		}
		
		m_pkSnapSrcMarker->SetTranslate(kCenterPoint);
		m_pkSnapSrcMarker->Update(0.0f);
	}

	if (bSnapLine)
	{
		NiPoint3 kCenter = (*m_pkDestNearestPoint + *m_pkDestNearPoint) * 0.5f;
		
		m_pkSnapSrcLineMarker->SetTranslate(kCenter);
		m_pkSnapSrcLineVerts[0] = *m_pkDestNearestPoint - kCenter;
		m_pkSnapSrcLineVerts[1] = *m_pkDestNearPoint - kCenter;
		m_pkSnapSrcLineMarker->Update(0.0f);
	}
	else if(m_bFoundSnapLineSrcPoint)
	{
		NiPoint3 kStart = *m_pkSrcNearestPoint + kTranslation;
		NiPoint3 kEnd = *m_pkSrcNearPoint + kTranslation;
		NiPoint3 kCenter = (kStart + kEnd) * 0.5f;
		
		m_pkSnapSrcLineMarker->SetTranslate(kCenter);
		m_pkSnapSrcLineVerts[0] = kStart - kCenter;
		m_pkSnapSrcLineVerts[1] = kEnd - kCenter;
		m_pkSnapSrcLineMarker->Update(0.0f);
	}
}
//---------------------------------------------------------------------------
MTranslateInteractionMode::TranslateAxis 
    MTranslateInteractionMode::GetBestAxis(const TranslateAxis eAxis)
{
    TranslateAxis eReturnValue;
    NiPoint3 kAxisDirection;
    bool bAxis;

    eReturnValue = eAxis;
    if (eAxis == AXIS_X)
    {
        kAxisDirection = NiPoint3::UNIT_X;
        bAxis = true;
    }
    else if (eAxis == AXIS_Y)
    {
        kAxisDirection = NiPoint3::UNIT_Y;
        bAxis = true;
    }
    else if (eAxis == AXIS_Z)
    {
        kAxisDirection = NiPoint3::UNIT_Z;
        bAxis = true;
    }
    else if (eAxis == PLANE_XY)
    {
        kAxisDirection = NiPoint3::UNIT_Z;
        bAxis = false;
    }
    else if (eAxis == PLANE_XZ)
    {
        kAxisDirection = NiPoint3::UNIT_Y;
        bAxis = false;
    }
    else if (eAxis == PLANE_YZ)
    {
        kAxisDirection = NiPoint3::UNIT_X;
        bAxis = false;
    }

    NiCamera* pkCam;
    NiPoint3 kLook;
    float kCosine;

    pkCam = MFramework::Instance->ViewportManager->ActiveViewport->
        GetNiCamera();
    pkCam->GetRotate().GetCol(0, (float*)&kLook);
    kCosine = kLook.Dot(kAxisDirection);

    // for axis, check if the axis is parallel to the view
    // for plane, check if the normal is perpendicular to view
    if ((bAxis) && ((kCosine >= NiViewMath::PARALLEL_THRESHOLD) || 
        (kCosine <= -NiViewMath::PARALLEL_THRESHOLD)))
    {
        // if axis is parallel to view, make us translate along
        // the perpendicular plane
        if (eReturnValue == AXIS_X)
        {
            eReturnValue = PLANE_YZ;
        }
        else if (eReturnValue == AXIS_Y)
        {
            eReturnValue = PLANE_XZ;
        }
        else if (eReturnValue == AXIS_Z)
        {
            eReturnValue = PLANE_XY;
        }
    }
    else if ((kCosine <= NiViewMath::INV_PARALLEL_THRESHOLD) && 
        (kCosine >= -NiViewMath::INV_PARALLEL_THRESHOLD))
    {
        // if plane's normal is perpendicular to view, we are looking down
        // the plane - use the plane's normal as a translation axis instead
        if (eReturnValue == PLANE_YZ)
        {
            eReturnValue = AXIS_X;
        }
        else if (eReturnValue == PLANE_XZ)
        {
            eReturnValue = AXIS_Y;
        }
        else if (eReturnValue == PLANE_XY)
        {
            eReturnValue = AXIS_Z;
        }
    }

    return eReturnValue;
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::HighLightAxis(const TranslateAxis eAxis)
{
    // first, reset everything to it's default
    NiAVObject* pkLineX;
    NiAVObject* pkLineY;
    NiAVObject* pkLineZ;
    NiAVObject* pkLineXY;
    NiAVObject* pkLineXZ;
    NiAVObject* pkLineYX;
    NiAVObject* pkLineYZ;
    NiAVObject* pkLineZX;
    NiAVObject* pkLineZY;

    pkLineX = m_pkGizmo->GetObjectByName(*m_pkXLineName);
    pkLineY = m_pkGizmo->GetObjectByName(*m_pkYLineName);
    pkLineZ = m_pkGizmo->GetObjectByName(*m_pkZLineName);
    pkLineXY = m_pkGizmo->GetObjectByName(*m_pkXYLineName);
    pkLineXZ = m_pkGizmo->GetObjectByName(*m_pkXZLineName);
    pkLineYX = m_pkGizmo->GetObjectByName(*m_pkYXLineName);
    pkLineYZ = m_pkGizmo->GetObjectByName(*m_pkYZLineName);
    pkLineZX = m_pkGizmo->GetObjectByName(*m_pkZXLineName);
    pkLineZY = m_pkGizmo->GetObjectByName(*m_pkZYLineName);
    assert(pkLineX);
    assert(pkLineY);
    assert(pkLineZ);
    assert(pkLineXY);
    assert(pkLineXZ);
    assert(pkLineYX);
    assert(pkLineYZ);
    assert(pkLineZX);
    assert(pkLineZY);

    NiMaterialProperty* pkMaterial;
    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineX->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineY->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZ->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineXY->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineXZ->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineYX->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineYZ->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZX->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    pkMaterial = NiDynamicCast(NiMaterialProperty, 
        pkLineZY->GetProperty(NiProperty::MATERIAL));
    assert(pkMaterial);
    pkMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    if (eAxis == AXIS_X)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == AXIS_Y)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == AXIS_Z)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_XY)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineXY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineYX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_XZ)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineXZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZX->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }
    else if (eAxis == PLANE_YZ)
    {
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineZY->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
        pkMaterial = NiDynamicCast(NiMaterialProperty,
            pkLineYZ->GetProperty(NiProperty::MATERIAL));
        pkMaterial->SetEmittance(*m_pkHighLightColor);
    }

    m_pkGizmo->UpdateProperties();
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::SetGizmoScale(NiCamera* pkCamera)
{
    // figure out the gizmo's location
    NiPoint3 kCenter;
    if (m_bCloning)
    {
        kCenter = *m_pkCloneCenter;
    }
    else
    {
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    }
    m_pkGizmo->SetTranslate(kCenter);
    // figure out how large it needs to be
    if (m_bAlreadyTranslating)
    {
        m_pkGizmo->SetScale(m_fStartScale);
    }
    else
    {
        if (pkCamera->GetViewFrustum().m_bOrtho)
        {
            NiFrustum pkFrustum = pkCamera->GetViewFrustum();
            m_pkGizmo->SetScale((pkFrustum.m_fRight * 2.0f) / 
                m_fDefaultDistance);
        }
        else
        {
            float fCamDistance = (kCenter - 
                pkCamera->GetWorldTranslate()).Length();
            if ((fCamDistance / m_fDefaultDistance) > 0.0f)
            {
                m_pkGizmo->SetScale((fCamDistance / m_fDefaultDistance) * 
                    pkCamera->GetViewFrustum().m_fRight * 2.0f);
            }
            else
            {
                m_pkGizmo->SetScale(1.0f);
            }
        }
    }
    m_pkGizmo->Update(0.0f);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;

    __super::Update(fTime);

    if (m_bCloning)
    {
        for (int i = 0; i < m_pmCloneArray->Count; i++)
        {
            MEntity* pmEntity = dynamic_cast<MEntity*>(
                m_pmCloneArray->Item[i]);
            pmEntity->Update(fTime, 
                MFramework::Instance->ExternalAssetManager);
        }
    }
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;
	NiEntityRenderingContext* pkContext = 
		pmRenderingContext->GetRenderingContext();

    if ((m_pkGizmo) && ((CanTransform()) || (m_bCloning)))
    {
        NiCamera* pkCam = pkContext->m_pkCamera;

        if (m_bCloning)
        {
            // if we are cloning, draw a ghost of the new objects
            GhostRenderingMode->Begin(pmRenderingContext);
            for (int i = 0; i < m_pmCloneArray->Count; i++)
            {
                MEntity* pmEntity = dynamic_cast<MEntity*>(
                    m_pmCloneArray->Item[i]);
                GhostRenderingMode->Render(pmEntity, pmRenderingContext);
            }
            GhostRenderingMode->End(pmRenderingContext);
        }

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkCam);
        NiDrawScene(pkCam, m_pkGizmo, *pkContext->m_pkCullingProcess);

		if(m_bFoundSnapSrcPoint)
		{
			NiDrawScene(pkCam, m_pkSnapSrcMarker, *pkContext->m_pkCullingProcess);
		}
		if (m_bFoundSnapLineSrcPoint && m_bRotateToPickEnabled)
		{
			NiDrawScene(pkCam, m_pkSnapSrcLineMarker, *pkContext->m_pkCullingProcess);
		}
    }
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::MouseDown(MouseButtonType eType, int iX,
    int iY)
{
    MVerifyValidInstance;

    if (eType == MouseButtonType::LeftButton)
    {
        // we use these to see if the mouse has moved far enough to count
        m_iMouseX = iX;
        m_iMouseY = iY;

        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);


		SetGizmoScale(MFramework::Instance->ViewportManager->ActiveViewport->
			GetNiCamera());
		m_pkPick->SetTarget(m_pkGizmo);
		if ((!CanTransform()) || 
			(!m_pkPick->PickObjects(kOrigin, kDir, false)))
		{
			// pass the call off to selection mode, it handles if we 
			// are or aren't clicking on an object
			__super::MouseDown(eType, iX, iY);

			m_eAxis = GetBestAxis(m_eAxis);
		}
		else
		{
			// set the working axis to the one we are hovering over
			const NiPick::Results& kPickResults = m_pkPick->GetResults();
			NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
			if (pkPickRecord)
			{
				NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
				// set current axis
				NiFixedString kName = pkPickedObject->GetName();
				if (kName == *m_pkXAxisName)
					m_eCurrentAxis = AXIS_X;
				else if (kName == *m_pkYAxisName)
					m_eCurrentAxis = AXIS_Y;
				else if (kName == *m_pkZAxisName)
					m_eCurrentAxis = AXIS_Z;
				else if (kName == *m_pkXYPlaneName)
					m_eCurrentAxis = PLANE_XY;
				else if (kName == *m_pkXZPlaneName)
					m_eCurrentAxis = PLANE_XZ;
				else if (kName == *m_pkYZPlaneName)
					m_eCurrentAxis = PLANE_YZ;
				else
					m_eCurrentAxis = PLANE_XY;
				m_eCurrentAxis = GetBestAxis(m_eCurrentAxis);

				// set axis colors appropriately
				HighLightAxis(m_eCurrentAxis);

				m_bOnGizmo = true;
			}
			m_eAxis = m_eCurrentAxis;
			m_bLeftDown = true;
		}

		if (CanTransform())
		{
			// record the starting position of the gizmo
			SelectionService->SelectionCenter->ToNiPoint3(*m_pkStartPoint);
			// store where the pick landed in world space
			if (m_bOnGizmo)
			{
				// if our mouse is on the gizmo, we must use different 
				// pick results
				const NiPick::Results& kPickResults = 
					m_pkPick->GetResults();
				NiPick::Record* pkPickRecord = NULL;
				pkPickRecord = kPickResults.GetAt(0);
				if (pkPickRecord)
				{
					*m_pkStartPick = pkPickRecord->GetIntersection();
				}
				else
				{
					// we have no choice but to pick some arbitrary point
					*m_pkStartPick = *m_pkStartPoint;
				}
			}
			else
			{
				// if our mouse is on the entity, use the most recent pick 
				// utility results
				*m_pkStartPick = m_pkPickRecord->GetIntersection();
			}
		}

		// 점 스냅을 활성화하면, 이동의 시작점을 점으로 찾는다.
		if (m_bSnapToPointPickEnabled
		&& MFramework::Instance->PickUtility->PerformPick(MFramework::Instance->Scene, kOrigin, kDir, true))
		{
			const NiPick* pkPick;
            pkPick = MFramework::Instance->PickUtility->GetNiPick();
			const NiPick::Results& kPickResults = pkPick->GetResults();
			MEntity* amEntities[] = SelectionService->GetSelectedEntities();

			if (m_pkSrcMultiRotation)
			{
				NiDelete[] m_pkSrcMultiRotation;
				m_pkSrcMultiRotation = 0;
			}
			m_pkSrcMultiRotation = NiNew NiMatrix3[amEntities->Count];
			for (int i = 0 ; i < amEntities->Count ; i++)
			{
				if (amEntities[i] != NULL &&
					amEntities[i]->HasProperty(*m_pkTranslationName) &&
					amEntities[i]->HasProperty(*m_pkRotationName))
				{
					NiEntityInterface* pkEntity = amEntities[i]->
						GetNiEntityInterface();

	                pkEntity->GetPropertyData(*m_pkRotationName, m_pkSrcMultiRotation[i]);
				}
			}

			for (unsigned int ui = 0; ui < kPickResults.GetSize(); ui++)
			{
				NiPick::Record* pkPickRecord = kPickResults.GetAt(ui);
				if (pkPickRecord != NULL)
				{
					NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
					MEntity* pmPickedEntity = MFramework::Instance->
						PickUtility->GetEntityFromPickedObject(pkPickedObject);
					NiEntityInterface* pkEntity = pmPickedEntity->GetNiEntityInterface();

					bool bIsSelected = false;
					
					for (int j = 0; j < amEntities->Count; j++)
					{
						if (pmPickedEntity == amEntities[j])
						{
							// the picked entity is selected; ignore it
							bIsSelected = true;
							break;
						}
					}

					if (bIsSelected)
					{
						// 선택한 물체의 충돌면을 이루는 가장 인접한 버텍스 정보를 스냅 점으로 이용한다.
						m_bFoundSnapSrcPoint = true;
						m_pkPickSrcRecord = pkPickRecord;
						m_pkSrcGeometry = NiDynamicCast(NiGeometry, m_pkPickSrcRecord->GetAVObject());
						*m_pkSnapSrcPoint = GetNearestVertexPoint(pkPickRecord);

						*m_pkSrcNearestPoint = GetNearestVertexPoint(m_pkPickSrcRecord);
						*m_pkSrcNearPoint = GetNearestVertexPoint(m_pkPickSrcRecord, NULL, m_pkSrcNearestPoint);

						m_pkSnapSrcMarker->SetTranslate(*m_pkSnapSrcPoint);
						m_pkSnapSrcMarker->Update(0.0f);

						m_bFoundSnapLineSrcPoint = true;
						pkEntity->GetPropertyData(*m_pkRotationName, *m_pkSrcRotation);
						pkEntity->GetPropertyData(*m_pkTranslationName, *m_pkSrcTranslate);
						NiPoint3 kCenter = (*m_pkSrcNearestPoint + *m_pkSrcNearPoint) * 0.5f;
						m_pkSnapSrcLineMarker->SetTranslate(kCenter);
						m_pkSnapSrcLineVerts[0] = *m_pkSrcNearestPoint - kCenter;
						m_pkSnapSrcLineVerts[1] = *m_pkSrcNearPoint - kCenter;
						m_pkSnapSrcLineMarker->Update(0.0f);
					}
				}
			}
		}


    }
    else
    {
        if ((m_bAlreadyTranslating) && (eType == MouseButtonType::RightButton))
        {
            // if the user right-click canceled, translate objects back
            // if the operation was a clone, restore the original selection
            if (m_bCloning)
            {
                SelectionService->AddEntitiesToSelection(static_cast<
                    MEntity*[]>(m_pmPreviousSelection->ToArray(__typeof(
                    MEntity))));
                // detach the clones from scene lights
                MEntity* amEntities[] = static_cast<MEntity*[]>(m_pmCloneArray
                    ->ToArray(__typeof(MEntity)));
                MFramework::Instance->LightManager->
                    RemoveEntitiesFromDefaultLights(amEntities);
                for (int i = 0; i < amEntities->Length; i++)
                {
                    MFramework::Instance->EntityFactory->Remove(
                        amEntities[i]->GetNiEntityInterface());
                }
                m_pmPreviousSelection->Clear();
                m_pmCloneArray->Clear();
                MFramework::Instance->Scene->UpdateEffects();
                m_bCloning = false;
            }
            else
            {
                MEntity* amEntities[] = 
                    SelectionService->GetSelectedEntities();
                unsigned int uiIndex = 0;
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkRotationName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();
                        if (!amEntities[i]->IsPropertyReadOnly(
                            *m_pkTranslationName))
                        {
                            pkEntity->SetPropertyData(*m_pkTranslationName, 
                                m_pkInitialTranslation->GetAt(uiIndex));
                        }
                        if (!amEntities[i]->IsPropertyReadOnly(
                            *m_pkRotationName))
                        {
                            pkEntity->SetPropertyData(*m_pkRotationName,
                                m_pkInitialRotation->GetAt(uiIndex));
                        }
                        uiIndex++;
                    }
                }
                SelectionService->SelectionCenter->X = m_pkStartPoint->x;
                SelectionService->SelectionCenter->Y = m_pkStartPoint->y;
                SelectionService->SelectionCenter->Z = m_pkStartPoint->z;
            }

            m_pkInitialTranslation->RemoveAll();
            m_pkInitialRotation->RemoveAll();
            m_bAlreadyTranslating = false;
            m_bLeftDown = false;
        }
        else
        {
            // if a button other than the left was clicked, just let the super
            // class deal with it - probably viewport movement
            __super::MouseDown(eType, iX, iY);
        }
    }
}

//---------------------------------------------------------------------------
void MTranslateInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

	m_bFoundSnapSrcPoint = false;
	m_bFoundSnapLineSrcPoint = false;
	m_pkSnapSrcLineVerts[0] = NiPoint3(0,0,0);
	m_pkSnapSrcLineVerts[1] = NiPoint3(0,0,0);


    if ((eType == MouseButtonType::LeftButton) && (m_bAlreadyTranslating))
    {
        // stop transforming object
        m_bAlreadyTranslating = false;

        // commit transform to properties
        NiPoint3 kCenter;
        NiPoint3 kDeltaPosition;
        NiPoint3 kCurrentPosition;
        NiMatrix3 kCurrentRotation;

        if (m_bCloning)
        {
            kCenter = *m_pkCloneCenter;
        }
        else
        {
            SelectionService->SelectionCenter->ToNiPoint3(kCenter);
        }
        kDeltaPosition = *m_pkStartPoint - kCenter;

        // if we were cloning, add them to the scene
        if (m_bCloning)
        {
			CommandService->BeginUndoFrame("Placement: Clone and translate "
                "selection");
            MScene* pmScene = MFramework::Instance->Scene;
            MEntity* amEntities[] = static_cast<MEntity*[]>(m_pmCloneArray
                ->ToArray(__typeof(MEntity)));
            // detach the default lights and let the scene handle adding 
            // appropriate ones
            MFramework::Instance->LightManager->
                RemoveEntitiesFromDefaultLights(amEntities);
            for (int i = 0; i < amEntities->Length; i++)
            {
                MEntity* pmClone = amEntities[i];

                // If the entity is a light, we must prepare its affected
                // entities before adding.
                if (MLightManager::EntityIsLight(pmClone))
                {
                    PrepareClonedLight(pmClone);
                }

                // we must assign each clone a unique name before adding
                pmClone->Name = pmScene->GetUniqueEntityName(pmClone->Name);

                // Add entity.
                pmScene->AddEntity(pmClone, true);
				if(!MLightManager::EntityIsLight(pmClone))
				{
					MFramework::Instance->LightManager->ApplySameLight(m_pCloneSource, pmClone);
				}
			}
            SelectionService->AddEntitiesToSelection(amEntities);
            CommandService->EndUndoFrame(true);
            m_pmPreviousSelection->Clear();
            m_pmCloneArray->Clear();
            m_bCloning = false;

			MFramework::Instance->PgUtil->UpdateTriangleCount();
        }
        else if ((kDeltaPosition.x > 0.01f) || (kDeltaPosition.x < -0.01f) ||
            (kDeltaPosition.y > 0.01f) || (kDeltaPosition.y < -0.01f) ||
            (kDeltaPosition.z > 0.01f) || (kDeltaPosition.z < -0.01f))
        {
            MEntity* amEntities[] = SelectionService->GetSelectedEntities();
			bool bCheck = false;

			if (!bCheck)
			{
				CommandService->BeginUndoFrame("Placement: Translate selected "
					"entities");
			}
			unsigned int uiIndex = 0;
			for (int i = 0; i < amEntities->Count; i++)
			{
				if (amEntities[i] != NULL &&
					amEntities[i]->HasProperty(*m_pkTranslationName) &&
					amEntities[i]->HasProperty(*m_pkRotationName))
				{
					// in order for the undo operation to store the right info,
					// we need to change from the original strait to the final
					// in a single MEntity->SetPropertyData call.  Therefore,
					// we set the original translation back on the interface
					// right before setting the final.
					NiEntityInterface* pkEntity = amEntities[i]->
						GetNiEntityInterface();
					pkEntity->GetPropertyData(*m_pkTranslationName, 
						kCurrentPosition);
					pkEntity->GetPropertyData(*m_pkRotationName,
						kCurrentRotation);
					pkEntity->SetPropertyData(*m_pkTranslationName,
						m_pkInitialTranslation->GetAt(uiIndex));
					pkEntity->SetPropertyData(*m_pkRotationName,
						m_pkInitialRotation->GetAt(uiIndex));
					if (!amEntities[i]->IsPropertyReadOnly(
						*m_pkTranslationName))
					{
						amEntities[i]->SetPropertyData(*m_pkTranslationName,
							new MPoint3(kCurrentPosition), true);
					}
					if (!amEntities[i]->IsPropertyReadOnly(
						*m_pkRotationName))
					{
						amEntities[i]->SetPropertyData(*m_pkRotationName, new
							MMatrix3(kCurrentRotation), true);
					}
					uiIndex++;
				}
			}

			if (!bCheck)
			{
				CommandService->EndUndoFrame(true);
			}
        }
        else
        {
            // if we did not move far enough, reset transformation
            MEntity* amEntities[] = 
                SelectionService->GetSelectedEntities();
            unsigned int uiIndex = 0;
            for (int i = 0; i < amEntities->Count; i++)
            {
                if (amEntities[i] != NULL &&
                    amEntities[i]->HasProperty(*m_pkTranslationName) &&
                    amEntities[i]->HasProperty(*m_pkRotationName))
                {
                    NiEntityInterface* pkEntity = amEntities[i]->
                        GetNiEntityInterface();
                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkTranslationName))
                    {
                        pkEntity->SetPropertyData(*m_pkTranslationName, 
                            m_pkInitialTranslation->GetAt(uiIndex));
                    }
                    if (!amEntities[i]->IsPropertyReadOnly(
                        *m_pkRotationName))
                    {
                        pkEntity->SetPropertyData(*m_pkRotationName,
                            m_pkInitialRotation->GetAt(uiIndex));
                    }
                    uiIndex++;
                }
            }
            SelectionService->SelectionCenter->X = m_pkStartPoint->x;
            SelectionService->SelectionCenter->Y = m_pkStartPoint->y;
            SelectionService->SelectionCenter->Z = m_pkStartPoint->z;
        }

        m_pkInitialTranslation->RemoveAll();
        m_pkInitialRotation->RemoveAll();
    }

    __super::MouseUp(eType, iX, iY);
        // make sure that our gizmo is updated
    MouseMove(iX, iY);
}
//---------------------------------------------------------------------------
void MTranslateInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

    NiPoint3 kOrigin, kDir;

    if (m_bAlreadyTranslating)
    {
        //transform
        NiViewMath::MouseToRay((float)iX, (float)iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);
        TranslateHelper(&kOrigin, &kDir);
    }
    else if (CanTransform())
    {
        // allow camera movement
        __super::MouseMove(iX, iY);

        NiViewMath::MouseToRay((float) iX, (float) iY, 
            MFramework::Instance->ViewportManager->ActiveViewport->Width,
            MFramework::Instance->ViewportManager->ActiveViewport->Height,
            MFramework::Instance->ViewportManager->ActiveViewport->
            GetNiCamera(), kOrigin, kDir);
        // highlight the axis we are over
        // set gizmo scale
        SetGizmoScale(MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera());
        m_pkPick->SetTarget(m_pkGizmo);
        if (m_pkPick->PickObjects(kOrigin, kDir, false))
        {
            const NiPick::Results& kPickResults = m_pkPick->GetResults();
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                // set current axis
                NiFixedString kName = pkPickedObject->GetName();
                if (kName == *m_pkXAxisName)
                    m_eCurrentAxis = AXIS_X;
                else if (kName == *m_pkYAxisName)
                    m_eCurrentAxis = AXIS_Y;
                else if (kName == *m_pkZAxisName)
                    m_eCurrentAxis = AXIS_Z;
                else if (kName == *m_pkXYPlaneName)
                    m_eCurrentAxis = PLANE_XY;
                else if (kName == *m_pkXZPlaneName)
                    m_eCurrentAxis = PLANE_XZ;
                else if (kName == *m_pkYZPlaneName)
                    m_eCurrentAxis = PLANE_YZ;
                else
                    m_eCurrentAxis = PLANE_XY;
                m_eCurrentAxis = GetBestAxis(m_eCurrentAxis);

                // set axis colors appropriately
                HighLightAxis(m_eCurrentAxis);

                m_bOnGizmo = true;
            }
        }
        else
        {
            //  reset selected axis to previous value
            m_bOnGizmo = false;
            m_eCurrentAxis = m_eAxis;
            HighLightAxis(m_eAxis);
        }

        if (m_bLeftDown)
        {
            // if the left is down and we are not already translating, the
            // mouse hasn't moved yet
            if ((m_iMouseX != iX) || (m_iMouseY != iY))
            {
                // if the user is just click + dragging, move the selection
                MEntity* amEntities[] = SelectionService->
                    GetSelectedEntities();
                for (int i = 0; i < amEntities->Count; i++)
                {
                    if (amEntities[i] != NULL &&
                        amEntities[i]->HasProperty(*m_pkTranslationName) &&
                        amEntities[i]->HasProperty(*m_pkRotationName))
                    {
                        NiEntityInterface* pkEntity = amEntities[i]->
                            GetNiEntityInterface();
                        NiPoint3 kCurrentPosition;
                        pkEntity->GetPropertyData(*m_pkTranslationName, 
                            kCurrentPosition);
                        m_pkInitialTranslation->Add(kCurrentPosition);
                        NiMatrix3 kCurrentRotation;
                        pkEntity->GetPropertyData(*m_pkRotationName,
                            kCurrentRotation);
                        m_pkInitialRotation->Add(kCurrentRotation);
                    }
                }

                if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
                {
                    // if the user is holding shift, we need to clone the 
                    // selection
                    MEntity* amEntities[] = SelectionService->
                        GetSelectedEntities();
                    for (int i = 0; i < amEntities->Count; i++)
                    {
                        if (amEntities[i] != NULL)
                        {
							if  (MFramework::Instance->ExtEntity->IsExtEntityType(amEntities[i])
								!= MPgExtEntity::ExtEntityType_Normal)
								continue;

                            m_pmPreviousSelection->Add(amEntities[i]);
							m_pCloneSource = amEntities[i];
                            MEntity* pmClone = amEntities[i]->Clone(
                                amEntities[i]->Name, false);
                            m_pmCloneArray->Add(pmClone);
                            pmClone->Update(MFramework::Instance->
                                TimeManager->CurrentTime, 
                                MFramework::Instance->ExternalAssetManager);
                        }
                    }
                    SelectionService->ClearSelectedEntities();
                    // add the default lights to the clones so we can see them
                    MEntity* amClones[] = static_cast<MEntity*[]>(
                        m_pmCloneArray->ToArray(__typeof(MEntity)));
                    MFramework::Instance->LightManager->
                        AddEntitiesToDefaultLights(amClones);
                    m_bCloning = true;
                }
                else
                {
                    m_pmPreviousSelection->Clear();
                    m_pmCloneArray->Clear();
                    m_bCloning = false;
                }

                m_bAlreadyTranslating = true;
                if (m_pkGizmo)
                    m_fStartScale = m_pkGizmo->GetScale();
            }
        }
    }
    else
    {
        // allow camera movement
        __super::MouseMove(iX, iY);
    }
}
//---------------------------------------------------------------------------
