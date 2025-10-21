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
#include "MEntity.h"
#include "MSelectionSet.h"
#include "MRenderingContext.h"
#include "MPoint3.h"
#include "IMessageService.h"
#include "ICommandService.h"

using namespace System::Collections;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MScene : public MDisposable
    {
    public:
        MScene(NiScene* pkScene);

        NiScene* GetNiScene();

        __property String* get_Name();
        __property void set_Name(String* strName);

        __property unsigned int get_EntityCount();
        __property bool get_Dirty();
        __property void set_Dirty(bool bDirty);
        MEntity* GetEntities()[];
        MEntity* GetEntityByName(String* strName);
        bool AddEntity(MEntity* pmEntity, bool bUndoable);
        void RemoveEntity(MEntity* pmEntity, bool bUndoable);
        void RemoveAllEntities(bool bUndoable);
        bool IsEntityInScene(MEntity* pmEntity);
        MEntity* GetDependentEntities(MEntity* pmEntity)[]; 

        __property unsigned int get_SelectionSetCount();
        MSelectionSet* GetSelectionSets()[];
        MSelectionSet* GetSelectionSetByName(String* strName);
        bool AddSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveSelectionSet(MSelectionSet* pmSelectionSet);
        void RemoveAllSelectionSets();
        bool IsSelectionSetInScene(MSelectionSet* pmSelectionSet);

        void Update(float fTime, NiExternalAssetManager* pkAssetManager);
        void UpdateEffects();

        String* GetUniqueEntityName(String* strProposedName);
        String* GetUniqueSelectionSetName(String* strProposedName);

        void GetBound(NiBound* pkBound);
		bool GetPGAvailEntity(MEntity* pkEntity);
		void GetPGWolrdBound(NiPoint3* pkMinPos, NiPoint3* pkMaxPos);

        static MScene* FindSceneContainingEntity(MEntity* pmEntity);

		//__property String* get_Path();
		//__property void set_Path(String* strPath);

    protected:
        virtual void Do_Dispose(bool bDisposing);

    private:
        void UpdateEffects(MEntity* pmEntity);

        void OnEntityComponentAdded(MEntity* pmEntity, 
            MComponent* pmComponent);
        void OnEntityComponentRemoved(MEntity* pmEntity, 
            MComponent* pmComponent);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);

        void ResolveAddedComponentDependencies(MEntity* pmEntity,
            MEntity* pmMasterEntity, MComponent* pmAddedComponent);
        void ResolveRemovedComponentDependencies(MEntity* pmEntity,
            MEntity* pmMasterEntity, MComponent* pmRemovedComponent);
        void ResolveRemovedEntityDependencies(MEntity* pmEntity,
            MEntity* pmRemovedEntity);
        void ResolveRemovedEntityDependencies(MSelectionSet* pmSelectionSet,
            MEntity* pmRemovedEntity);

        NiScene* m_pkScene;
		NiDefaultErrorHandler* m_pkErrors;
        bool m_bDirtyBit;
		//String* m_strPath;

        static ArrayList* ms_pmScenes = new ArrayList();

        static String* ms_strTranslationPropertyName = "Translation";
        static String* ms_strApplyTransformsPropertyName = "Apply Transforms";

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;
    };
}}}}
