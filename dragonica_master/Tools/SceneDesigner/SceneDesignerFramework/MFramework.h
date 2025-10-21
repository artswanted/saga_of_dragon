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

#pragma once
#pragma warning(disable : 4793)
#pragma warning(disable : 4275)
// HandOver, 강정욱 2008.01.29
// GSA저장시.. GSA 파일에 ExtraObject(Path, Npc, Monster, etc...)들을 지울지 저장 할 지 설정한다.
// ExtraObject를 제외 하고 GSA 저장하게 되면.. 저장시 매우 느림...
// 사용 할 경우 순서 : ExtraObj삭제 -> GSA저장 -> ExtraObj를 다시 맵에 추가.
//#define EXTRAOBJ_NOT_SAVE

#define CRTDBG_MAP_ALLOC

#include "MDisposable.h"

#include "MRenderer.h"
#include "MEventManager.h"
#include "MTimeManager.h"
#include "MCameraManager.h"
#include "MViewportManager.h"
#include "MPaletteManager.h"
#include "MProxyManager.h"
#include "MLightManager.h"
#include "MBoundManager.h"

#include "MSceneFactory.h"
#include "MEntityFactory.h"
#include "MSelectionSetFactory.h"
#include "MComponentFactory.h"

#include "MScene.h"
#include "MPickUtility.h"
#include "IInteractionModeService.h"
#include "IRenderingModeService.h"
#include "IComponentService.h"
#include "ISelectionService.h"
#include "IUICommandService.h"
#include "ISettingsService.h"
#include "IOptionsService.h"

// Project G
#include "mpgbrightbloom.h"
#include "mpgextentity.h"
#include "mpgmonster.h"
#include "mpgnpc.h"
#include "mpgpath.h"
#include "mpgmakeraindata.h"
#include "mpgwaypoint.h"
#include "mpgstreammanager.h"
#include "mpghidesomeobject.h"
#include "mpgexistfile.h"
#include "mpgmonarea.h"
#include "mpgutil.h"
#include "MPgDecal.h"
#include "MPgBreakObject.h"
#include "MPgElevator.h"
#include "MPgFog.h"
#include "MPgMerge.h"
#include "MPgDrawPhysX.h"
#include "MPgDrawSoundAffectArea.h"
#include "MPgZoneControl.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
	{
		public __gc class MFramework : public MDisposable
		{
		public:
			void Startup();

			// Managers.
			__property MRenderer* get_Renderer();
        __property MEventManager* get_EventManager();
        __property MTimeManager* get_TimeManager();
        __property MCameraManager* get_CameraManager();
        __property MViewportManager* get_ViewportManager();
        __property MPaletteManager* get_PaletteManager();
        __property MProxyManager* get_ProxyManager();
        __property MLightManager* get_LightManager();
        __property MBoundManager* get_BoundManager();

        // Factories.
        __property MSceneFactory* get_SceneFactory();
        __property MEntityFactory* get_EntityFactory();
        __property MSelectionSetFactory* get_SelectionSetFactory();
        __property MComponentFactory* get_ComponentFactory();

        __property String* get_AppStartupPath();
        __property MScene* get_Scene();
        __property MPickUtility* get_PickUtility();
        __property NiExternalAssetManager* get_ExternalAssetManager();
        __property String* get_ApplicationName();
        __property String* get_ImageSubfolder1();
		__property void set_ImageSubfolder1(String* strPath);
		__property String* get_ImageSubfolder2();
		__property void set_ImageSubfolder2(String* strPath);
		__property String* get_ImageSubfolder3();
		__property void set_ImageSubfolder3(String* strPath);
        __property bool get_PerformingLongOperation();
		
		// Factories For PG
		__property MPgBrightBloom* get_BrightBloom();
		__property MPgExtEntity* get_ExtEntity();
		__property MPgMonster* get_Monster();
		__property MPgNpc* get_Npc();
		__property MPgPath* get_Path();
		__property MPgMakeRainData* get_MakeRainData();
		__property MPgWayPoint* get_WayPoint();
		__property MPgStreamManager* get_StreamManager();
		__property MPgHideSomeObject* get_HideSomeObject();
		__property MPgExistFile* get_ExistFile();
		//__property MPgMonArea* get_MonArea();
		__property MPgUtil* get_PgUtil();
		__property MPgDecalManager* get_DecalManager();
		__property MPgBreakObject* get_BreakObject();
		__property MPgElevator* get_Elevator();
		__property MPgFog* get_Fog();
		__property MPgMerge* get_Merge();
		__property MPgDrawPhysX* get_DrawPhysX();
		__property MPgDrawSoundAffectArea* get_DrawSoundAffectArea();
		__property MPgZoneControl* get_ZoneControl();

        MScene* NewScene(unsigned int uiEntityArraySize);
        MScene* LoadScene(String* strFilename, String* strFormat);
        void InitNewScene(MScene* pmNewScene);
        bool SaveScene(String* strFilename, String* strFormat, bool bAutosave);
		ArrayList* LoadPalettesDirectory(ArrayList* arrPalNameList, String* strFilename, String* strFormat);
		ArrayList* LoadPaletteName(String* strFilename, String* strFormat);
        bool SavePalettesDir(String* strFormat, String* strDefaultDirectoryPath, bool bAutosave);
		void RemoveAllPaletteInDll(String* strFormat);

		String* GetScenePathName();


        void Update();

        // *** begin Emergent internal use only ***
        void BeginLongOperation();
        void EndLongOperation();

        void PauseAnimation();
        void UnPauseAnimation();
        bool IsTimePaused();
        void RestartAnimation();
        // *** end Emergent internal use only ***

    private:
        // Helper functions.
        void Update(float fTime);
        void InitNewScene(MScene* pmNewScene, bool bRaiseEvent);
        void RegisterSettingsAndOptions();
        void OnImageSubfolder1SettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
		void OnImageSubfolder2SettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);
		void OnImageSubfolder3SettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        // Service accessors.
        __property static IInteractionModeService*
            get_InteractionModeService();
        static IInteractionModeService* ms_pmInteractionModeService;
        __property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService;
        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;
        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        // Member variables.
        MScene* m_pmScene;
        MScene* m_pmToolScene;
        MPickUtility* m_pmPickUtility;
        NiExternalAssetManager* m_pkAssetManager;
        String* m_strImageSubfolder1;
		String* m_strImageSubfolder2;
		String* m_strImageSubfolder3;

        bool m_bDoNotUpdate;
        unsigned int m_uiLongOperationCount;
        bool m_bPausedAnimation;
        float m_fTimeOfLastPause;

		//
		bool PgInitialize();
		bool PgTerminate();
		// Member variables. - ProjectG
		static bool m_bPgInit = false;
		MPgBrightBloom* m_pkBrightBloom;
		MPgMonster* m_pkMonster;
		MPgExtEntity* m_pkExtEntity;
		MPgNpc* m_pkNpc;
		MPgPath* m_pkPath;
		MPgMakeRainData* m_pkMakeRainData;
		MPgWayPoint* m_pkWayPoint;
		MPgStreamManager* m_pkStreamManager;
		MPgHideSomeObject* m_pkHideSomeObject;
		MPgExistFile* m_pkExistFile;
		//MPgMonArea* m_pkMonArea;
		MPgUtil* m_pkPgUtil;
		MPgDecalManager* m_pkDecalManager;
		MPgBreakObject* m_pkBreakObject;
		MPgElevator* m_pkElevator;
		MPgFog* m_pkFog;
		MPgMerge* m_pkMerge;
		MPgDrawPhysX* m_pkDrawPhysX;
		MPgDrawSoundAffectArea* m_pkDrawSoundAffectArea;
		MPgZoneControl*	m_pkZoneControl;

		String* m_strScenePathName;


        static String* ms_strImageSubfolder1SettingName = "Image Subfolder1";
        static String* ms_strImageSubfolder1OptionName = String::Concat(
            "External File Settings.", ms_strImageSubfolder1SettingName);

		static String* ms_strImageSubfolder2SettingName = "Image Subfolder2";
        static String* ms_strImageSubfolder2OptionName = String::Concat(
            "External File Settings.", ms_strImageSubfolder2SettingName);

		static String* ms_strImageSubfolder3SettingName = "Image Subfolder3";
        static String* ms_strImageSubfolder3OptionName = String::Concat(
            "External File Settings.", ms_strImageSubfolder3SettingName);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MFramework* get_Instance();
    private:
        static MFramework* ms_pmThis = NULL;

        MFramework();
    };
}}}}
