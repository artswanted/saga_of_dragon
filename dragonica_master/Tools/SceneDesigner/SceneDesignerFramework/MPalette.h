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
 
#include "MDisposable.h"
#include "MScene.h"
#include "ICommandService.h"

using namespace System::Collections;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPalette : public MDisposable
    {
    public:
        MPalette(MScene* pmScene);
        MPalette(String* strName, unsigned int uiInitialNumEntities);

        __property String* get_Name();
        __property void set_Name(String* strName);

        __property MScene* get_Scene();

        __property unsigned int get_EntityCount();

        __property bool get_DontSave();
        __property void set_DontSave(bool bDontSave);

		// ProjectG - 팔레트 경로를 위한 Path
		__property String* get_Path();
		__property void set_Path(String* pPath);

        MEntity* GetEntities()[];
        MEntity* GetEntityByName(String* strName);
        bool AddEntity(MEntity* pmEntity, String* strCategory,
            bool bUndoable);
        void RemoveEntity(MEntity* pmEntity, bool bUndoable);
        void RemoveAllEntities(bool bUndoable);
        void AddComponentPropertyToEntities(MComponent* pmComponent,
            String* strPropertyName, MScene* pmAffectedScenes[]);
        void RemoveComponentPropertyFromEntities(MComponent* pmComponent,
            String* strPropertyName, MScene* pmAffectedScenes[]);

        String* GetEntityCategory(MEntity* pmEntity);
        void ResetEntityNames();

        __property MEntity* get_ActiveEntity();
        __property void set_ActiveEntity(MEntity* pmActiveEntity);

        static MEntity* CreateTemplateFromFile(String* strFileName);

        //some static naming utilities
        static String* StripPaletteName(String* strName);

    private:
        static void CopyComponentProperty(MComponent* pmSource,
            String* strPropertyName, MComponent* pmDestination);
        static ArrayList* GetAffectedEntities(MEntity* pmTemplateEntity,
            MScene* pmScenes[]);

        MScene* m_pmScene;
        MEntity* m_pmActiveEntity;
        bool m_bDontSave;
		String* m_pPath;

        static ICommandService* ms_pmCommandService;
        __property static ICommandService* get_CommandService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    };
}}}}
