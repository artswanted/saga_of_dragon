//
// HandOver, 강정욱 2008.01.29
//
// 전역으로 쓰고 싶은 클래스가 있다면. 여기 헤더에 등록을 하고.
// 여기에서 초기화, 소멸을 처리 해주고. 쓰면 됩니다.
// MFramework.h에 사용하려는 클래스의 헤더 포함, 클래스를 멤버변수로 추가, __property declarator 선언
// PgInitialize() 함수에서 해당 클래스 생성
// PgTerminate() 함수에서 해당 클래스 해제
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
#include "SceneDesignerFrameworkPCH.h"

#include "MFramework.h"
#include "MPgPropertyDef.h"
#include "NiLightProxyComponent.h"
#include "ServiceProvider.h"
#include "MPoint3.h"
#include "MUtility.h"
#include "MFolderLocation.h"
//#include "MPgUtil.h"
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472
#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif

using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MFramework::Init()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MFramework();
    }
}
//---------------------------------------------------------------------------
void MFramework::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
#if _MSC_VER == 1310
	__crt_dll_terminate();
#endif
}
//---------------------------------------------------------------------------
bool MFramework::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MFramework* MFramework::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MFramework::MFramework() : m_strImageSubfolder1(String::Empty), m_strImageSubfolder2(String::Empty), m_strImageSubfolder3(String::Empty), 
    m_bDoNotUpdate(false), m_uiLongOperationCount(0), m_pmScene(NULL)
{
    NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
        NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false)
#else
        NiExternalNew NiStandardAllocator()
#endif
        );

    NiInit(pkInitOptions);

    NiProxyComponent::_SDMInit();
    NiLightProxyComponent::_SDMInit();
    MViewport::_SDMInit();
    MEntity::_SDMInit();

    MSceneFactory::Init();
    MEntityFactory::Init();
    MSelectionSetFactory::Init();
    MComponentFactory::Init();

    MEventManager::Init();
    MRenderer::Init();
    MTimeManager::Init();
    MCameraManager::Init();
    MViewportManager::Init();
    MPaletteManager::Init();
    MProxyManager::Init();
    MLightManager::Init();
    MBoundManager::Init();
	//ProjectG
	MPgDrawPhysX::Init();
	MPgDrawSoundAffectArea::Init();
	MPgZoneControl::Init();

    // Ensure that all registered assets clone from a pristine copy
    NiTFactory<NiExternalAssetHandler*>* pkAssetFactory = 
        NiFactories::GetAssetFactory();
    NiTMapIterator kIter = pkAssetFactory->GetFirstPos();
    while (kIter)
    {
        const char* pcKey = NULL;
        pkAssetFactory->GetNext(kIter, pcKey);

        NiExternalAssetHandler* pkHandler = 
            pkAssetFactory->GetPersistent(pcKey);

        NiBool bSuccess = NIBOOL_IS_TRUE(
            pkHandler->SetCloneFromPristine(true));
        assert(bSuccess);
    }

    m_pmPickUtility = new MPickUtility();

    m_pkAssetManager = NiNew NiExternalAssetManager(
        NiFactories::GetAssetFactory());

    // To make clear that error handling, if used, will be set explicitly.
    m_pkAssetManager->SetErrorHandler(NULL);

    MInitRefObject(m_pkAssetManager);

    MScene* pmScene = NewScene(10);
    InitNewScene(pmScene, false);

    this->TimeManager->Enabled = true;
    m_bPausedAnimation = false;

	//PgInitialize();

	// PhysX가 안되네;;...
	// 잘 되도록 해줘야 할 텐데...
	//NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
	//if (!pkManager->Initialize())
	//{
	//	MAssert(0, "PhysX Init Faild");
	//}
	//pkManager->m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	//pkManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
	//pkManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 15.0f);
	//pkManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_BODY_MASS_AXES, 1.0f);
}
//---------------------------------------------------------------------------
void MFramework::Do_Dispose(bool bDisposing)
{
    MDisposeRefObject(m_pkAssetManager);

    if (bDisposing)
    {
		PgTerminate();

        MProxyManager::Shutdown();

        if (m_pmPickUtility != NULL)
        {
            m_pmPickUtility->Dispose();
            m_pmPickUtility = NULL;
        }

        MComponentFactory::Shutdown();
        MSelectionSetFactory::Shutdown();
        MEntityFactory::Shutdown();
        MSceneFactory::Shutdown();

        MLightManager::Shutdown();
        MPaletteManager::Shutdown();
        MViewportManager::Shutdown();
        MCameraManager::Shutdown();
        MTimeManager::Shutdown();
        MBoundManager::Shutdown();
        MRenderer::Shutdown();
        MEventManager::Shutdown();
		MPgDrawPhysX::Shutdown();
		MPgDrawSoundAffectArea::Shutdown();
		MPgZoneControl::Shutdown();
    }

    NiLightProxyComponent::_SDMShutdown();
    NiProxyComponent::_SDMShutdown();
    MEntity::_SDMShutdown();
    MViewport::_SDMShutdown();

    const NiInitOptions* pkInitOptions = NiStaticDataManager
        ::GetInitOptions();
    NiShutdown();
    NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
    NiExternalDelete pkInitOptions;
    NiExternalDelete pkAllocator;
}
//---------------------------------------------------------------------------
void MFramework::Startup()
{
    MVerifyValidInstance;

    IUICommandService* pmCommand = MGetService(IUICommandService);
    pmCommand->BindCommands(MViewportManager::Instance);
    pmCommand->BindCommands(MCameraManager::Instance);

    this->ProxyManager->Startup();
    this->LightManager->Startup();
    this->CameraManager->Startup();
    this->ViewportManager->Startup();
    this->BoundManager->Startup();

    RegisterSettingsAndOptions();
}
//---------------------------------------------------------------------------
void MFramework::RegisterSettingsAndOptions()
{
    MVerifyValidInstance;

	String* pkDescStr = "The directory to use when loading external images referenced by NIF "
        "files. A blank path indicates that the same directory as the NIF "
        "file should be used.\n\nNote: this option is stored per scene and "
        "does not take effect until the scene is saved and reloaded.";

    SettingsService->RegisterSettingsObject(ms_strImageSubfolder1SettingName,
        new MFolderLocation(m_strImageSubfolder1), SettingsCategory::PerScene);
    SettingsService->SetChangedSettingHandler(ms_strImageSubfolder1SettingName,
        SettingsCategory::PerScene, new SettingChangedHandler(this,
        &MFramework::OnImageSubfolder1SettingChanged));
    OnImageSubfolder1SettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strImageSubfolder1OptionName,
        SettingsCategory::PerScene, ms_strImageSubfolder1SettingName);
    OptionsService->SetHelpDescription(ms_strImageSubfolder1OptionName,
       pkDescStr );

	SettingsService->RegisterSettingsObject(ms_strImageSubfolder2SettingName,
        new MFolderLocation(m_strImageSubfolder2), SettingsCategory::PerScene);
    SettingsService->SetChangedSettingHandler(ms_strImageSubfolder2SettingName,
        SettingsCategory::PerScene, new SettingChangedHandler(this,
        &MFramework::OnImageSubfolder2SettingChanged));
    OnImageSubfolder2SettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strImageSubfolder2OptionName,
        SettingsCategory::PerScene, ms_strImageSubfolder2SettingName);
    OptionsService->SetHelpDescription(ms_strImageSubfolder2OptionName,
       pkDescStr);

	SettingsService->RegisterSettingsObject(ms_strImageSubfolder3SettingName,
        new MFolderLocation(m_strImageSubfolder3), SettingsCategory::PerScene);
    SettingsService->SetChangedSettingHandler(ms_strImageSubfolder3SettingName,
        SettingsCategory::PerScene, new SettingChangedHandler(this,
        &MFramework::OnImageSubfolder3SettingChanged));
    OnImageSubfolder3SettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strImageSubfolder3OptionName,
        SettingsCategory::PerScene, ms_strImageSubfolder3SettingName);
    OptionsService->SetHelpDescription(ms_strImageSubfolder3OptionName,
       pkDescStr);
}
//---------------------------------------------------------------------------
void MFramework::OnImageSubfolder1SettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    MFolderLocation* pmImageSubfolder = dynamic_cast<MFolderLocation*>(
        SettingsService->GetSettingsObject(ms_strImageSubfolder1SettingName,
        SettingsCategory::PerScene));
    if (pmImageSubfolder != NULL)
    {
        // Store new image subfolder.
        m_strImageSubfolder1 = pmImageSubfolder->Path;

		StreamManager->set_ImageSubfolder1(m_strImageSubfolder1);
		StreamManager->SaveConfig("config.txt");

        // Set platform-specific subdirectory with new image subfolder.
        const char* pcImageSubfolder = MStringToCharPointer(
            m_strImageSubfolder1);
		NiDevImageConverter::SetPlatformSpecificSubdirectory(
			pcImageSubfolder, 0);
        MFreeCharPointer(pcImageSubfolder);

		m_pkExistFile->RefreshPostfixTexture();
    }
}
//---------------------------------------------------------------------------
void MFramework::OnImageSubfolder2SettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    MFolderLocation* pmImageSubfolder = dynamic_cast<MFolderLocation*>(
        SettingsService->GetSettingsObject(ms_strImageSubfolder2SettingName,
        SettingsCategory::PerScene));
    if (pmImageSubfolder != NULL)
    {
        // Store new image subfolder.
        m_strImageSubfolder2 = pmImageSubfolder->Path;

		StreamManager->set_ImageSubfolder2(m_strImageSubfolder2);
		StreamManager->SaveConfig("config.txt");

        // Set platform-specific subdirectory with new image subfolder.
        const char* pcImageSubfolder = MStringToCharPointer(
            m_strImageSubfolder2);
		NiDevImageConverter::SetPlatformSpecificSubdirectory(
			pcImageSubfolder, 1);
        MFreeCharPointer(pcImageSubfolder);

		m_pkExistFile->RefreshPostfixTexture();
    }
}
//---------------------------------------------------------------------------
void MFramework::OnImageSubfolder3SettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    MFolderLocation* pmImageSubfolder = dynamic_cast<MFolderLocation*>(
        SettingsService->GetSettingsObject(ms_strImageSubfolder3SettingName,
        SettingsCategory::PerScene));
    if (pmImageSubfolder != NULL)
    {
        // Store new image subfolder.
        m_strImageSubfolder3 = pmImageSubfolder->Path;

		StreamManager->set_ImageSubfolder3(m_strImageSubfolder3);
		StreamManager->SaveConfig("config.txt");

        // Set platform-specific subdirectory with new image subfolder.
        const char* pcImageSubfolder = MStringToCharPointer(
            m_strImageSubfolder3);
		NiDevImageConverter::SetPlatformSpecificSubdirectory(
			pcImageSubfolder, 2);
        MFreeCharPointer(pcImageSubfolder);

		m_pkExistFile->RefreshPostfixTexture();
    }
}
//---------------------------------------------------------------------------
MSceneFactory* MFramework::get_SceneFactory()
{
    MVerifyValidInstance;

    return MSceneFactory::Instance;
}
//---------------------------------------------------------------------------
MEntityFactory* MFramework::get_EntityFactory()
{
    MVerifyValidInstance;

    return MEntityFactory::Instance;
}
//---------------------------------------------------------------------------
MSelectionSetFactory* MFramework::get_SelectionSetFactory()
{
    MVerifyValidInstance;

    return MSelectionSetFactory::Instance;
}
//---------------------------------------------------------------------------
MComponentFactory* MFramework::get_ComponentFactory()
{
    MVerifyValidInstance;

    return MComponentFactory::Instance;
}
//---------------------------------------------------------------------------
MRenderer* MFramework::get_Renderer()
{
    MVerifyValidInstance;

    return MRenderer::Instance;
}
//---------------------------------------------------------------------------
MEventManager* MFramework::get_EventManager()
{
    MVerifyValidInstance;

    return MEventManager::Instance;
}
//---------------------------------------------------------------------------
MTimeManager* MFramework::get_TimeManager()
{
    MVerifyValidInstance;

    return MTimeManager::Instance;
}
//---------------------------------------------------------------------------
MCameraManager* MFramework::get_CameraManager()
{
    MVerifyValidInstance;

    return MCameraManager::Instance;
}
//---------------------------------------------------------------------------
MViewportManager* MFramework::get_ViewportManager()
{
    MVerifyValidInstance;

    return MViewportManager::Instance;
}
//---------------------------------------------------------------------------
MPaletteManager* MFramework::get_PaletteManager()
{
    MVerifyValidInstance;

    return MPaletteManager::Instance;
}
//---------------------------------------------------------------------------
MProxyManager* MFramework::get_ProxyManager()
{
    MVerifyValidInstance;

    return MProxyManager::Instance;
}
//---------------------------------------------------------------------------
MLightManager* MFramework::get_LightManager()
{
    MVerifyValidInstance;

    return MLightManager::Instance;
}
//---------------------------------------------------------------------------
MBoundManager* MFramework::get_BoundManager()
{
    MVerifyValidInstance;

    return MBoundManager::Instance;
}
//---------------------------------------------------------------------------
String* MFramework::get_AppStartupPath()
{
    MVerifyValidInstance;

    return String::Format(S"{0}{1}",Application::StartupPath, S"\\");
}
//---------------------------------------------------------------------------
MScene* MFramework::get_Scene()
{
    MVerifyValidInstance;

    return m_pmScene;
}
//---------------------------------------------------------------------------
MPickUtility* MFramework::get_PickUtility()
{
    MVerifyValidInstance;

    return m_pmPickUtility;
}
//---------------------------------------------------------------------------
NiExternalAssetManager* MFramework::get_ExternalAssetManager()
{
    MVerifyValidInstance;

    return m_pkAssetManager;
}
//---------------------------------------------------------------------------
String* MFramework::get_ApplicationName()
{
    MVerifyValidInstance;

    return Application::ProductName;
}
//---------------------------------------------------------------------------
String* MFramework::get_ImageSubfolder1()
{
    MVerifyValidInstance;

    return m_strImageSubfolder1;
}
void MFramework::set_ImageSubfolder1(String* strPath)
{
	MVerifyValidInstance;
	m_strImageSubfolder1 = strPath;
}
//---------------------------------------------------------------------------
String* MFramework::get_ImageSubfolder2()
{
    MVerifyValidInstance;

    return m_strImageSubfolder2;
}
void MFramework::set_ImageSubfolder2(String* strPath)
{
	MVerifyValidInstance;
	m_strImageSubfolder2 = strPath;
}
//---------------------------------------------------------------------------
String* MFramework::get_ImageSubfolder3()
{
	MVerifyValidInstance;

	return m_strImageSubfolder3;
}
void MFramework::set_ImageSubfolder3(String* strPath)
{
	MVerifyValidInstance;
	m_strImageSubfolder3 = strPath;
}
//---------------------------------------------------------------------------
bool MFramework::PgInitialize()
{
	if(false==m_bPgInit)
	{
		m_bPgInit = true;
		m_pkBrightBloom = new MPgBrightBloom;	
		m_pkExtEntity = new MPgExtEntity;
		m_pkMonster = new MPgMonster;
		m_pkMonster->Initialize();
		m_pkNpc = new MPgNpc;
		m_pkNpc->Initialize();
		m_pkPath = new MPgPath;
		m_pkPath->Initialize();
		m_pkMakeRainData = new MPgMakeRainData;
		m_pkMakeRainData->Initialize();
		m_pkWayPoint = new MPgWayPoint;
		m_pkWayPoint->Initialize();
		m_pkStreamManager = new MPgStreamManager;
		m_pkStreamManager->Initialize();
		m_pkHideSomeObject = new MPgHideSomeObject;
		m_pkHideSomeObject->Initialize();
		m_pkExistFile = new MPgExistFile;
		m_pkExistFile->Initialize();
		//m_pkMonArea = new MPgMonArea;
		//m_pkMonArea->Initialize();
		m_pkPgUtil = new MPgUtil;
		m_pkDecalManager = new MPgDecalManager;
		m_pkBreakObject = new MPgBreakObject;
		m_pkBreakObject->Initalize();
		m_pkElevator = new MPgElevator;
		m_pkFog = new MPgFog;
		m_pkMerge = new MPgMerge;
		this->DrawSoundAffectArea->CreateAreaMesh();
		this->ZoneControl->CreateMesh();
	}
	return true;
}
//---------------------------------------------------------------------------
bool MFramework::PgTerminate()
{
	if(true==m_bPgInit)
	{
		m_bPgInit=false;
		if (m_pkPgUtil)
		{
			delete m_pkPgUtil;
			m_pkPgUtil = 0;
		}
		//if (m_pkMonArea)
		//{
		//	delete m_pkMonArea;
		//	m_pkMonArea = 0;
		//}
		if (m_pkExistFile)
		{
			delete m_pkExistFile;
			m_pkExistFile = 0;
		}
		if (m_pkHideSomeObject)
		{
			delete m_pkHideSomeObject;
			m_pkHideSomeObject = 0;
		}
		if (m_pkStreamManager)
		{
			delete m_pkStreamManager;
			m_pkStreamManager = 0;
		}
		if (m_pkWayPoint)
		{
			delete m_pkWayPoint;
			m_pkWayPoint = 0;
		}
		if (m_pkMakeRainData)
		{
			delete m_pkMakeRainData;
			m_pkMakeRainData = 0;
		}
		if (m_pkNpc)
		{
			delete m_pkNpc;
			m_pkNpc = 0;
		}
		if (m_pkPath)
		{
			delete m_pkPath;
			m_pkPath = 0;
		}
		if (m_pkMonster)
		{
			delete m_pkMonster;
			m_pkMonster = 0;
		}
		if (m_pkBrightBloom)
		{
			delete m_pkBrightBloom;
			m_pkBrightBloom = 0;
		}
		if (m_pkDecalManager)
		{
			delete m_pkDecalManager;
			m_pkDecalManager = 0;
		}
		if(m_pkBreakObject)
		{
			delete m_pkBreakObject;
			m_pkBreakObject = 0;
		}
		if(m_pkElevator)
		{
			delete m_pkElevator;
			m_pkElevator = 0;
		}
		if(m_pkFog)
		{
			delete m_pkFog;
			m_pkFog = 0;
		}
		if(m_pkMerge)
		{
			delete m_pkMerge;
			m_pkMerge = 0;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
MScene* MFramework::NewScene(unsigned int uiEntityArraySize)
{
    MVerifyValidInstance;

	PgTerminate();
	PgInitialize();

	if(NULL != m_pkStreamManager)
	{
		m_strImageSubfolder1 = m_pkStreamManager->ImageSubfolder1;
		m_strImageSubfolder2 = m_pkStreamManager->ImageSubfolder2;
		m_strImageSubfolder3 = m_pkStreamManager->ImageSubfolder3;
	}

    return this->SceneFactory->Get(NiNew NiScene("Main Scene",
        uiEntityArraySize));
}
//---------------------------------------------------------------------------
MScene* MFramework::LoadScene(String* strFilename, String* strFormat)
{
    MVerifyValidInstance;

	// Delete PaletteList in dll before Load
	RemoveAllPaletteInDll(strFormat);

	// Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Load the scene
    const char* pcFilename = MStringToCharPointer(strFilename);
    NiBool bSuccess = pkEntityStreaming->Load(pcFilename);
    MFreeCharPointer(pcFilename);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    if (!bSuccess)
    {
        return NULL;
    }
    
    assert(pkEntityStreaming->GetSceneCount() == 1);
    MScene* pmScene = this->SceneFactory->Get(pkEntityStreaming->GetSceneAt(0));
	
	m_strScenePathName = strFilename;
	// PGProperty Load
    pcFilename = MStringToCharPointer(strFilename);
    pkEntityStreaming->Load(pcFilename);
    MFreeCharPointer(pcFilename);
	// End PGProperty Load

	pkEntityStreaming->RemoveAllScenes(); // Because static instance

    return pmScene;
}
//---------------------------------------------------------------------------
void MFramework::InitNewScene(MScene* pmNewScene)
{
    MVerifyValidInstance;

    InitNewScene(pmNewScene, true);
	//SetCameraInEntityAll();
}
//---------------------------------------------------------------------------
void MFramework::InitNewScene(MScene* pmNewScene, bool bRaiseEvent)
{
    MVerifyValidInstance;

    MAssert(pmNewScene != NULL, "Null scene provided to function!");

    m_bDoNotUpdate = true;
    BeginLongOperation();

    if (m_pmScene != NULL)
    {
        MSceneFactory::Instance->Remove(m_pmScene);
        m_pmScene->Dispose();
        m_pmScene = NULL;
    }
    m_pkAssetManager->RemoveAll();
    this->TimeManager->ResetTime(0.0f);

    m_pmScene = pmNewScene;
    m_pmScene->Update(TimeManager->CurrentTime, m_pkAssetManager);

    if (bRaiseEvent)
    {
        this->EventManager->RaiseNewSceneLoaded(m_pmScene);
    }

    EndLongOperation();
    m_bDoNotUpdate = false;
}
//---------------------------------------------------------------------------
bool MFramework::SaveScene(String* strFilename, String* strFormat, bool bAutosave)
{
    MVerifyValidInstance;

    if (!m_pmScene)
        return false;

    if (!m_pmScene->GetNiScene())
        return false;

    m_bDoNotUpdate = true;

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

	// Delete Setted Monster, Npc and etc..
	// Check Necessary object. (char spawn, Main Camera and path)
	bool bExistCharSpawn = false;
	bool bExistMainCamera = false;
	bool bExistPath = false;
	bool bExistBaseObject = false;

//#ifndef EXTRAOBJ_NOT_SAVE
	ArrayList *pkDeletedList = new ArrayList();
//#endif
	MEntity* pkEntities[] = m_pmScene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity* pkEntity = pkEntities[i];
//#ifndef EXTRAOBJ_NOT_SAVE
		if(false == bAutosave)
		{
			if (MFramework::Instance->ExtEntity->IsExtEntityType(pkEntity)
				!= MPgExtEntity::ExtEntityType_Normal)
			{
				pkDeletedList->Add(pkEntity);
				m_pmScene->RemoveEntity(pkEntity, false);
			}
		}
//#endif

		if (!bAutosave)
		{
			if (pkEntity->get_PGProperty() == MEntity::ePGProperty::CharacterSpawn)
			{
				bExistCharSpawn = true;
			}
			else if (pkEntity->get_PGProperty() == MEntity::ePGProperty::MainCamera)
			{
				bExistMainCamera = true;
			}
			else if (pkEntity->get_PGProperty() == MEntity::ePGProperty::Path)
			{
				bExistPath = true;
			}
			else if (pkEntity->get_PGProperty() == MEntity::ePGProperty::BaseObject)
			{
				bExistBaseObject = true;
			}
		}
	}

    // Insert desired scenes
    pkEntityStreaming->InsertScene(m_pmScene->GetNiScene());

    // Save the scene
    const char* pcFilename = MStringToCharPointer(strFilename);
    bool bSuccess = NIBOOL_IS_TRUE(pkEntityStreaming->Save(pcFilename));
    MFreeCharPointer(pcFilename);
    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    m_bDoNotUpdate = false;

	//#ifndef EXTRAOBJ_NOT_SAVE
	if(false == bAutosave)
	{
		for (int i=0 ; i<pkDeletedList->Count ; i++)
		{
			MEntity *pkEntity = dynamic_cast<MEntity *>(pkDeletedList->get_Item(i));
			m_pmScene->AddEntity(pkEntity, false);
		}
	}
//#endif

	// Necessary object error message.
	if (!bAutosave &&
		!bExistBaseObject )
	{
		if (!bExistCharSpawn)
		{
			::MessageBox(0, "No Character Spawn.", "OK", 0);
		}
		if (!bExistMainCamera)
		{
			::MessageBox(0, "No Main Camera", "OK", 0);
		}
		if (!bExistPath)
		{
			::MessageBox(0, "No Path", "OK", 0);
		}
	}

    return bSuccess;
}

//---------------------------------------------------------------------------
ArrayList* MFramework::LoadPalettesDirectory(ArrayList* arrPalNameList, String* strFilename, String* strFormat)
{
    MVerifyValidInstance;

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

	// Load Palettes Directory.
	unsigned int uiPalCount = arrPalNameList->Count;	ArrayList *arPalList = new ArrayList;
	for (unsigned int ui = 0 ; ui < uiPalCount ; ui++)
	{
		String *strIdx = pkEntityStreaming->GetPaletteIdx(ui);
		int iIdx = Convert::ToInt32(strIdx);
		String *pDir = new String(pkEntityStreaming->GetPaletteDirectory(iIdx));
		arPalList->Add(pDir);
	}

	return arPalList;
}

void MFramework::RemoveAllPaletteInDll(String* strFormat)
{
    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

	pkEntityStreaming->RemoveAllPaletteDir();
	pkEntityStreaming->RemoveAllPaletteName();
	pkEntityStreaming->RemoveAllPaletteIdx();
}

//---------------------------------------------------------------------------
bool MFramework::SavePalettesDir(String* strFormat, String* strDefaultDirectoryPath, bool bAutosave)
{
    MVerifyValidInstance;

	RemoveAllPaletteInDll(strFormat);

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

	// 팔레트 폴더 리스트를 넣는다.
	MPalette *pPalettes[] = MPaletteManager::Instance->GetPalettes();
	for (unsigned int i = 0 ; i < MPaletteManager::Instance->get_PaletteCount() ; i++)
	{
		const char* pcName = MStringToCharPointer(pPalettes[i]->Name);
		pkEntityStreaming->InsertPaletteName(pcName);

		String *pPath = pPalettes[i]->get_Path();
		// \의 중복을 걸러준다. cull
		if (pPath)
		{
			bool bCheck = false;
			while (!bCheck)
			{
				if (pPath->Contains("\\\\"))
				{
					pPath = pPath->Replace("\\\\", "\\");
				}
				else
				{
					bCheck = true;
				}
			}
		} // end cull

		String *pTemp = MPaletteManager::Instance->get_PaletteFolder();

		// AutoSave 시
		if (bAutosave || strDefaultDirectoryPath == 0)
		{
			const char* pcRelativePath = MStringToCharPointer(pPath);
			pkEntityStreaming->InsertPaletteDirectory(pcRelativePath);
		}
		// 경로가 없을때. 기본 경로를 넣어줌.
		else if (!pPath)
		{
			String *strRelativePath = ".\\";

			const char* pcRelativePath = MStringToCharPointer(strRelativePath);
			pkEntityStreaming->InsertPaletteDirectory(pcRelativePath);
		}
		// 경로가 있으면 그 경로로
		else
		{
			String *strRelativePath = MPgUtil::ConvertAbsPathToRelPath(strDefaultDirectoryPath,pPath);

			const char* pcRelativePath = MStringToCharPointer(strRelativePath);
			pkEntityStreaming->InsertPaletteDirectory(pcRelativePath);
		}
	}

    return true;
}

//---------------------------------------------------------------------------
ArrayList* MFramework::LoadPaletteName(String* strFilename, String* strFormat)
{
    MVerifyValidInstance;

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

	// Load Palettes Name.
	unsigned int uiPalCount = pkEntityStreaming->GetPaletteNameCount();
	ArrayList *arPalList = new ArrayList;
	for (unsigned int ui = 0 ; ui < uiPalCount ; ui++)
	{
		String *pName = new String(pkEntityStreaming->GetPaletteName(ui));		
		arPalList->Add(pName);
	}

	return arPalList;
}


//---------------------------------------------------------------------------
void MFramework::Update()
{
    MVerifyValidInstance;

    if (m_bDoNotUpdate)
    {
        return;
    }

    m_bDoNotUpdate = true;

    this->TimeManager->UpdateTime();
    Update(this->TimeManager->CurrentTime);
    this->Renderer->Render();

    m_bDoNotUpdate = false;
}
//---------------------------------------------------------------------------
void MFramework::Update(float fTime)
{
    MVerifyValidInstance;

    InteractionModeService->Update(fTime);
    LightManager->Update(fTime);
    CameraManager->Update(fTime);

    if (m_pmScene != NULL)
    {
        if (!m_bPausedAnimation)
        {
            m_pmScene->Update(fTime, m_pkAssetManager);
        }
        else
        {
            m_pmScene->Update(m_fTimeOfLastPause, m_pkAssetManager);
        }
    }
    if (this->ProxyManager->ProxyScene != NULL)
    {
        this->ProxyManager->ProxyScene->Update(fTime, m_pkAssetManager);
    }
    for (unsigned int ui = 0; ui < this->ViewportManager->ViewportCount; ui++)
    {
        this->ViewportManager->GetViewport(ui)->ToolScene->Update(fTime,
            m_pkAssetManager);
    }

    BoundManager->Update(fTime);
}
//---------------------------------------------------------------------------
bool MFramework::get_PerformingLongOperation()
{
    MVerifyValidInstance;

    return (m_uiLongOperationCount > 0);
}
//---------------------------------------------------------------------------
MPgBrightBloom* MFramework::get_BrightBloom()
{
    MVerifyValidInstance;

	return m_pkBrightBloom;
}
//---------------------------------------------------------------------------
MPgExtEntity* MFramework::get_ExtEntity()
{
    MVerifyValidInstance;

	return m_pkExtEntity;
}
//---------------------------------------------------------------------------
MPgMonster* MFramework::get_Monster()
{
    MVerifyValidInstance;

	return m_pkMonster;
}
//---------------------------------------------------------------------------
MPgNpc* MFramework::get_Npc()
{
    MVerifyValidInstance;

	return m_pkNpc;
}
//---------------------------------------------------------------------------
MPgPath* MFramework::get_Path()
{
    MVerifyValidInstance;

	return m_pkPath;
}
//---------------------------------------------------------------------------
MPgMakeRainData* MFramework::get_MakeRainData()
{
    MVerifyValidInstance;

	return m_pkMakeRainData;
}
//---------------------------------------------------------------------------
MPgWayPoint* MFramework::get_WayPoint()
{
	return m_pkWayPoint;
}
//---------------------------------------------------------------------------
MPgStreamManager* MFramework::get_StreamManager()
{
	return m_pkStreamManager;
}
//---------------------------------------------------------------------------
MPgHideSomeObject* MFramework::get_HideSomeObject()
{
	return m_pkHideSomeObject;
}
//---------------------------------------------------------------------------
MPgExistFile* MFramework::get_ExistFile()
{
	return m_pkExistFile;
}
//---------------------------------------------------------------------------
//MPgMonArea* MFramework::get_MonArea()
//{
//	return m_pkMonArea;
//}
//---------------------------------------------------------------------------
MPgUtil* MFramework::get_PgUtil()
{
	return m_pkPgUtil;
}
//---------------------------------------------------------------------------
MPgDecalManager* MFramework::get_DecalManager()
{
	return m_pkDecalManager;
}
//---------------------------------------------------------------------------
MPgBreakObject* MFramework::get_BreakObject()
{
	return m_pkBreakObject;
}
//---------------------------------------------------------------------------
MPgElevator* MFramework::get_Elevator()
{
	return m_pkElevator;
}
//---------------------------------------------------------------------------
MPgFog* MFramework::get_Fog()
{
	return m_pkFog;
}
//---------------------------------------------------------------------------
MPgMerge* MFramework::get_Merge()
{
	return m_pkMerge;
}
//---------------------------------------------------------------------------
MPgDrawPhysX* MFramework::get_DrawPhysX()
{
	return m_pkDrawPhysX;
}
//---------------------------------------------------------------------------
MPgDrawSoundAffectArea* MFramework::get_DrawSoundAffectArea()
{
	return MPgDrawSoundAffectArea::Instance;
}
//---------------------------------------------------------------------------
MPgZoneControl* MFramework::get_ZoneControl()
{
	return MPgZoneControl::Instance;
}
//---------------------------------------------------------------------------
void MFramework::BeginLongOperation()
{
    MVerifyValidInstance;

    if (m_uiLongOperationCount == 0)
    {
        this->EventManager->RaiseLongOperationStarted();
    }
    m_uiLongOperationCount++;
}
//---------------------------------------------------------------------------
void MFramework::EndLongOperation()
{
    MVerifyValidInstance;

    if (m_uiLongOperationCount > 0)
    {
        m_uiLongOperationCount--;

        if (m_uiLongOperationCount == 0)
        {
            this->EventManager->RaiseLongOperationCompleted();
        }
    }
}
//---------------------------------------------------------------------------
void MFramework::PauseAnimation()
{
    m_fTimeOfLastPause = this->TimeManager->CurrentTime;
    m_bPausedAnimation = true;
}
//---------------------------------------------------------------------------
void MFramework::UnPauseAnimation()
{
    m_bPausedAnimation = false;
}
//---------------------------------------------------------------------------
bool MFramework::IsTimePaused()
{
    return m_bPausedAnimation;
}
//---------------------------------------------------------------------------
void MFramework::RestartAnimation()
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    int iEntityCount = pmEntities->Count;
    NiFixedString kActorClassName = "NiActorComponent";
    for (int iEntityIndex = 0; iEntityIndex < iEntityCount; iEntityIndex++)
    {
        MComponent* pmComponents[] = pmEntities[iEntityIndex]->GetComponents();
        int iComponentCount = pmComponents->Count;
        for (int iComponentIndex = 0; iComponentIndex < iComponentCount; 
            iComponentIndex++)
        {
            MComponent* pmComponent = pmComponents[iComponentIndex];
            NiEntityComponentInterface* pkComponent = 
                pmComponent->GetNiEntityComponentInterface();
            NiFixedString kClassName = pkComponent->GetClassName();
            if (kClassName == kActorClassName)
            {
                NiActorComponent* pkActor = static_cast<NiActorComponent*>
                    (pkComponent);
                pkActor->ResetAnimation();
            }            
        }
    }
}
//---------------------------------------------------------------------------
IInteractionModeService* MFramework::get_InteractionModeService()
{
    if (ms_pmInteractionModeService == NULL)
    {
        ms_pmInteractionModeService = MGetService(IInteractionModeService);
        MAssert(ms_pmInteractionModeService != NULL, "Interaction mode "
            "service not found!");
    }
    return ms_pmInteractionModeService;
}
//---------------------------------------------------------------------------
ISelectionService* MFramework::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ISettingsService* MFramework::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not "
            "found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MFramework::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not "
            "found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
String* MFramework::GetScenePathName()
{
	return m_strScenePathName;
}
//---------------------------------------------------------------------------