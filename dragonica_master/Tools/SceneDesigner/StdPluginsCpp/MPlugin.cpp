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

#include "MPlugin.h"

// Services.
#include "MCommandService.h"
#include "MComponentService.h"
#include "MInteractionModeService.h"
#include "MMessageService.h"
#include "MPropertyTypeService.h"
#include "MRenderingModeService.h"
#include "MSelectionSetService.h"
#include "MSelectionService.h"

// Primitive type converters and editors.
#include "MPoint2Converter.h"
#include "MPoint3Converter.h"
#include "MEulerQuaternionConverter.h"
#include "MEulerMatrix3Converter.h"

// Semantic type converters and editors.
#include "MFilenameEditor.h"
#include "MXMLFilenameEditor.h"
#include "MNifFilenameEditor.h"
#include "MKfmFilenameEditor.h"
#include "MLightTypeConverter.h"
#include "MSequenceIDConverter.h"

// Interaction modes.
#include "MTranslateInteractionMode.h"
#include "MRotationInteractionMode.h"
#include "MScaleInteractionMode.h"
#include "MSelectionInteractionMode.h"
#include "MCreateInteractionMode.h"
#include "MPanInteractionMode.h"
#include "MZoomInteractionMode.h"
#include "MLookInteractionMode.h"
#include "MOrbitInteractionMode.h"
#include "MViewInteractionMode.h"

// Rendering modes.
#include "MGhostRenderingMode.h"
#include "MStandardRenderingMode.h"
#include "MWireframeRenderingMode.h"
#include "MFlatWireframeRenderingMode.h"

// Components
#include "NiGridComponent.h"
#include "NiBoxComponent.h"
#include "NiRoomComponent.h"
#include "NiPortalComponent.h"
#include "NiPlaneComponent.h"



//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472
#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif


using namespace System::Reflection;
using namespace System::Drawing;
using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

//---------------------------------------------------------------------------
MPlugin::MPlugin()
{
}
//---------------------------------------------------------------------------
void MPlugin::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
		NiBoxComponent::_SDMShutdown();
		NiRoomComponent::_SDMShutdown();
		NiPortalComponent::_SDMShutdown();
		NiPlaneComponent::_SDMShutdown();
        NiGridComponent::_SDMShutdown();
        if (m_pmGridManager != NULL)
            m_pmGridManager->Dispose();
        if (m_pmToolbarListener != NULL)
            m_pmToolbarListener->Dispose();
        MSettingsHelper::Shutdown();
    }
}
//---------------------------------------------------------------------------
String* MPlugin::get_Name()
{
    return this->GetType()->ToString();
}
//---------------------------------------------------------------------------
Version* MPlugin::get_Version()
{
    return Assembly::GetExecutingAssembly()->GetName()->Version;
}
//---------------------------------------------------------------------------
Version* MPlugin::get_ExpectedVersion()
{
    return new System::Version(1, 0);
}
//---------------------------------------------------------------------------
void MPlugin::Load(int iToolMajorVerion, int iToolMinorVersion)
{
    NiGridComponent::_SDMInit();
	NiBoxComponent::_SDMInit();
	NiRoomComponent::_SDMInit();
	NiPortalComponent::_SDMInit();
	NiPlaneComponent::_SDMInit();
    MSettingsHelper::Init();
}
//---------------------------------------------------------------------------
IService* MPlugin::GetProvidedServices()[]
{
    ArrayList* pmServices = new ArrayList();

    pmServices->Add(new MCommandService());
    pmServices->Add(new MComponentService());
    pmServices->Add(new MInteractionModeService());
    pmServices->Add(new MMessageService());
    pmServices->Add(new MPropertyTypeService());
    pmServices->Add(new MRenderingModeService());
    m_pmSelectionService = new MSelectionService();
    pmServices->Add(m_pmSelectionService);
    pmServices->Add(new MSelectionSetService());

    return dynamic_cast<IService*[]>(pmServices->ToArray(__typeof(IService)));
}
//---------------------------------------------------------------------------
void MPlugin::Start()
{
    AddPropertyTypes();
    RegisterComponents();
    RegisterSettings();
    AddInteractionModes();
    AddRenderingModes();
    AddGrid();
    AddToolbarListener();
}
//---------------------------------------------------------------------------
void MPlugin::AddPropertyTypes()
{
    // Get property type service.
    IPropertyTypeService* pmPropertyTypeService = MGetService(
        IPropertyTypeService);
    MAssert(pmPropertyTypeService != NULL, "Service not found!");

    // Register primitive types.
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_FLOAT,
        NiEntityPropertyInterface::PT_FLOAT, __typeof(float)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_BOOL,
        NiEntityPropertyInterface::PT_BOOL, __typeof(bool)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_INT,
        NiEntityPropertyInterface::PT_INT, __typeof(int)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_UINT,
        NiEntityPropertyInterface::PT_UINT, __typeof(unsigned int)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_SHORT,
        NiEntityPropertyInterface::PT_SHORT, __typeof(short)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_USHORT,
        NiEntityPropertyInterface::PT_USHORT, __typeof(unsigned short)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_STRING,
        NiEntityPropertyInterface::PT_STRING, __typeof(String)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_POINT2,
        NiEntityPropertyInterface::PT_POINT2, __typeof(MPoint2), NULL,
        __typeof(MPoint2Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_POINT3,
        NiEntityPropertyInterface::PT_POINT3, __typeof(MPoint3), NULL,
        __typeof(MPoint3Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_QUATERNION,
        NiEntityPropertyInterface::PT_QUATERNION, __typeof(MQuaternion), NULL,
        __typeof(MEulerQuaternionConverter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_MATRIX3,
        NiEntityPropertyInterface::PT_MATRIX3, __typeof(MMatrix3), NULL,
        __typeof(MEulerMatrix3Converter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_COLOR,
        NiEntityPropertyInterface::PT_COLOR, __typeof(Color)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        NiEntityPropertyInterface::PT_COLORA,
        NiEntityPropertyInterface::PT_COLORA, __typeof(Color)));

    // Register standard semantic types.
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "NIF Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MNifFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "KFM Filename", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MKfmFilenameEditor), NULL));
	pmPropertyTypeService->RegisterType(new PropertyType(
		"Xml Filename", NiEntityPropertyInterface::PT_STRING,
		__typeof(String), __typeof(MXmlFilenameEditor), NULL));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Light Type", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), NULL, __typeof(MLightTypeConverter)));
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Sequence ID", NiEntityPropertyInterface::PT_UINT,
        __typeof(unsigned int), NULL, __typeof(MSequenceIDConverter)));
}
//---------------------------------------------------------------------------
void MPlugin::RegisterComponents()
{
    // Get component service.
    IComponentService* pmComponentService = MGetService(IComponentService);
    MAssert(pmComponentService != NULL, "Component service not found!");

    // Get component factory.
    MComponentFactory* pmFactory = MFramework::Instance->ComponentFactory;
    MAssert(pmFactory != NULL, "Component factory not found!");

    // Register standard component types.
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiTransformationComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiActorComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiCameraComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiLightComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiSceneGraphComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiInheritedTransformationComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiBoxComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiPlaneComponent()));

    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiRoomComponent()));
    pmComponentService->RegisterComponent(pmFactory->Get(NiNew
        NiPortalComponent()));

	{//PG용 추가 프로퍼티 : Actor Xml
		MComponent* kIgnorePortalComponent = MComponent::CreateGeneralComponent("IgnorePortalSystem");
		pmComponentService->RegisterComponent(kIgnorePortalComponent);
		String* kArgStr1 = "IgnorePortalSystem";
		String* kArgStr2 = "Boolean";
		String* kArgStr3 = "Boolean";
		String* kArgStr4 = "포털 렌더링의 영향을 받지 않게 하고 싶을 경우 true 로 설정함.";
		kIgnorePortalComponent->AddProperty(kArgStr1,
			kArgStr1,
			kArgStr2,
			kArgStr3,
			false,
			kArgStr4, false);
	}

	{//PG용 추가 프로퍼티 : Actor Xml
		MComponent* kActorXmlComponent = MComponent::CreateGeneralComponent("Actor Xml");
		pmComponentService->RegisterComponent(kActorXmlComponent);
		String* kArgStr1 = "Actor Xml Path";
		String* kArgStr2 = "String";
		String* kArgStr3 = "Filename";
		String* kArgStr4 = "Actor Xml의 경로를 설정합니다.";
		kActorXmlComponent->AddProperty(kArgStr1,
			kArgStr1,
			kArgStr2,
			kArgStr3,
			false,
			kArgStr4, false);
	}

	{//PG용 추가 프로퍼티 : Use Shared Stream
		MComponent* kUseSharedStreamComponent = MComponent::CreateGeneralComponent("Shared Stream");
		pmComponentService->RegisterComponent(kUseSharedStreamComponent);
		String* kArgStr1 = "Use Shared Stream";
		String* kArgStr2 = "Boolean";
		String* kArgStr3 = "Boolean";
		String* kArgStr4 = "공유 스트림을 사용여부를 설정합니다.";
		kUseSharedStreamComponent->AddProperty(kArgStr1,
			kArgStr1,
			kArgStr2,
			kArgStr3,
			false,
			kArgStr4, false);
	}
	
	{//PG용 추가 프로퍼티 : SoundsObject
		MComponent* kSOPComponent = MComponent::CreateGeneralComponent("Sound Object");
		pmComponentService->RegisterComponent(kSOPComponent);
		//미디어 경로 프로퍼티 추가
		String* kArgStr1 = "Media Path";
		String* kArgStr2 = "String";
		String* kArgStr3 = "Filename";
		String* kArgStr4 = "Media file의 경로를 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//사운드 볼륨 
		kArgStr1 = "Volume";
		kArgStr2 = "Float";
		kArgStr3 = "Float";
		kArgStr4 = "환경음의 음량을 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//사운드 최대 영향거리 프로퍼티 추가
		kArgStr1 = "Affect Range";
		kArgStr2 = "Float";
		kArgStr3 = "Float";
		kArgStr4 = "환경음의 영향범위를 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//사운드 감쇄 시작거리 프로퍼티 추가
		kArgStr1 = "Affect Attenuation";
		kArgStr2 = "Float";
		kArgStr3 = "Float";
		kArgStr4 = "환경음의 감쇄가 시작되는 지점을 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//사운드 무작위 재생 옵션
		kArgStr1 = "Random Play";
		kArgStr2 = "Boolean";
		kArgStr3 = "Boolean";
		kArgStr4 = "환경음의 무작위 재생여부를 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//사운드 무작위 재생 확률
		kArgStr1 = "Random Probability";
		kArgStr2 = "Float";
		kArgStr3 = "Float";
		kArgStr4 = "환경음의 재생이 끝났을 때 다시 재생될 확률을 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
		//배경 사운드와 Mix 여부
		kArgStr1 = "Mix BGSound";
		kArgStr2 = "Boolean";
		kArgStr3 = "Boolean";
		kArgStr4 = "배경음과 섞을지 여부를 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
	}

	{//PG용 추가 프로퍼티 : Shader Information
		MComponent* kSOPComponent = MComponent::CreateGeneralComponent("Shader Information");
		pmComponentService->RegisterComponent(kSOPComponent);
		//Shader name
		String* kArgStr1 = "Shader Name";
		String* kArgStr2 = "String";
		String* kArgStr3 = "String";
		String* kArgStr4 = "Shader의 이름을 설정합니다.";
		kSOPComponent->AddProperty(kArgStr1,
			kArgStr1,		kArgStr2,		kArgStr3,		false,
			kArgStr4, false);
	}
}
//---------------------------------------------------------------------------
void MPlugin::RegisterSettings()
{
	m_pmSelectionService->RegisterSettings();
    MSettingsHelper::RegisterStandardSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddInteractionModes()
{
    // Add interaction modes.
    IInteractionModeService* pmInteractionModeService = MGetService(
        IInteractionModeService);
    MAssert(pmInteractionModeService != NULL, "Service not found!");
    MTranslateInteractionMode* pmTranslate = new MTranslateInteractionMode();
    MRotationInteractionMode* pmRotate = new MRotationInteractionMode();
    MScaleInteractionMode* pmScale = new MScaleInteractionMode();
    MSelectionInteractionMode* pmSelect = new MSelectionInteractionMode();
    MCreateInteractionMode* pmCreate = new MCreateInteractionMode();
    MPanInteractionMode* pmPan = new MPanInteractionMode();
    MZoomInteractionMode* pmZoom = new MZoomInteractionMode();
    MLookInteractionMode* pmLook = new MLookInteractionMode();
    MOrbitInteractionMode* pmOrbit = new MOrbitInteractionMode();
    MViewInteractionMode* pmView = new MViewInteractionMode();
    pmInteractionModeService->AddInteractionMode(pmSelect);
    pmInteractionModeService->AddInteractionMode(pmTranslate);
    pmInteractionModeService->AddInteractionMode(pmRotate);
    pmInteractionModeService->AddInteractionMode(pmScale);
    pmInteractionModeService->AddInteractionMode(pmCreate);
    pmInteractionModeService->AddInteractionMode(pmPan);
    pmInteractionModeService->AddInteractionMode(pmZoom);
    pmInteractionModeService->AddInteractionMode(pmLook);
    pmInteractionModeService->AddInteractionMode(pmOrbit);
    pmInteractionModeService->AddInteractionMode(pmView);	

    // Register each interaction mode's settings
    pmTranslate->RegisterSettings();
    pmRotate->RegisterSettings();
    pmScale->RegisterSettings();
    pmSelect->RegisterSettings();
    pmCreate->RegisterSettings();
    pmPan->RegisterSettings();
    pmZoom->RegisterSettings();
    pmLook->RegisterSettings();
    pmOrbit->RegisterSettings();
	pmView->RegisterSettings();

    // Bind interaction callbacks to commands
    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "Service not found!");
    pmUICommandService->BindCommands(pmTranslate);
    pmUICommandService->BindCommands(pmRotate);
    pmUICommandService->BindCommands(pmScale);
    pmUICommandService->BindCommands(pmSelect);
    pmUICommandService->BindCommands(pmCreate);
    pmUICommandService->BindCommands(pmPan);
    pmUICommandService->BindCommands(pmZoom);
    pmUICommandService->BindCommands(pmLook);
    pmUICommandService->BindCommands(pmOrbit);
	pmUICommandService->BindCommands(pmView);
}
//---------------------------------------------------------------------------
void MPlugin::AddRenderingModes()
{
    // Add rendering modes.
    IRenderingModeService* pmRenderingModeService = MGetService(
        IRenderingModeService);
    MAssert(pmRenderingModeService != NULL, "Service not found!");

    MFlatWireframeRenderingMode* pkFlatWireMode = 
        new MFlatWireframeRenderingMode();
    pmRenderingModeService->AddRenderingMode(new MStandardRenderingMode());
    pmRenderingModeService->AddRenderingMode(new MWireframeRenderingMode());
    pmRenderingModeService->AddRenderingMode(pkFlatWireMode);
    pmRenderingModeService->AddRenderingMode(new MGhostRenderingMode());

    pkFlatWireMode->RegisterSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddGrid()
{
    m_pmGridManager = new MGridManager();
    m_pmGridManager->CreateGridEntity();
    m_pmGridManager->RegisterSettings();
}
//---------------------------------------------------------------------------
void MPlugin::AddToolbarListener()
{
    m_pmToolbarListener = new MToolbarListener();
    m_pmToolbarListener->RegisterSettings();
    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "Service not found!");
    pmUICommandService->BindCommands(m_pmToolbarListener);
}
//---------------------------------------------------------------------------
void MPlugin::InitStatics()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
}
//---------------------------------------------------------------------------
void MPlugin::ShutdownStatics()
{
#if _MSC_VER == 1310
    __crt_dll_terminate();
#endif
}
