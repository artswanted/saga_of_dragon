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
#include "MPropertyContainer.h"
#include "PropertyType.h"
#include "MComponent.h"
#include "MRenderingContext.h"
#include "IMessageService.h"
#include "IPropertyTypeService.h"
#include "ICommandService.h"
#include <unordered_map>

using namespace System::ComponentModel;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

typedef std::unordered_map< std::wstring, std::wstring >		CONT_POSTFIX_TEX;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEntity : public MPropertyContainer
    {
    public:
        MEntity(NiEntityInterface* pkEntity);

        virtual String* ToString();

        __property String* get_Name();
        __property void set_Name(String* strName);
        __property Guid get_TemplateID();
        __property void set_TemplateID(Guid mGuid);
        
        __property virtual NiEntityPropertyInterface* get_PropertyInterface();
        static MEntity* CreateGeneralEntity(String* strName);
        MEntity* Clone(String* strNewName, bool bInheritProperties);
        __property MEntity* get_MasterEntity();
        __property void set_MasterEntity(MEntity* pmEntity);
        void MakeEntityUnique();

        __property bool get_SupportsComponents();
        __property unsigned int get_ComponentCount();
        MComponent* GetComponents()[];
        MComponent* GetComponentByTemplateID(Guid mTemplateID);
        bool CanAddComponent(MComponent* pmComponent);
        bool CanRemoveComponent(MComponent*pmComponent);
        void AddComponent(MComponent* pmComponent, bool bPerformErrorChecking,
            bool bUndoable);
        void RemoveComponent(MComponent* pmComponent,
            bool bPerformErrorChecking, bool bUndoable);

        String* GetPropertyNames()[];

        NiAVObject* GetSceneRootPointer(unsigned int uiIndex);
        unsigned int GetSceneRootPointerCount();

        __property bool get_Hidden();
        __property void set_Hidden(bool bHidden);
        void SetHidden(bool bHidden, bool bUndoable);

        __property bool get_Frozen();
        __property void set_Frozen(bool bFrozen);
        void SetFrozen(bool bFrozen, bool bUndoable);

		void SetPropertyDataInMEntity(String* strPropertyName, Object* pmData, bool bUndoable);
        virtual void SetPropertyData(String* strPropertyName, Object* pmData,
            bool bUndoable);
        virtual void SetPropertyData(String* strPropertyName, Object* pmData,
            unsigned int uiIndex, bool bUndoable);

        __value enum ePGProperty
        {
			Object,
			MainCamera,
            CharacterSpawn,
			Path,
			PhysX,
            Trigger,
			MinimapCamera,
			CameraWalls,
			Ladder,
			Rope,
			SkyBox,
			BaseObject,
			Water,
            CollisionObject,
            Dummy,
            Puppet,
			PermissionArea,
			Trap,
			Telejump,
			PS_Room,
			PS_Portal
        };
		__value enum ePGAlphaGroup
        {
			Group_5,
			Group_4,
			Group_3,
			Group_2,
			Group_1,
			Group0,
			Group1,
			Group2,
			Group3,
			Group4,
			Group5,
        };
		__value enum ePGOptimization
        {
            High_Mid_Low,
			High_Mid,
			High,
        };
		__property ePGProperty get_PGProperty();
		__property void set_PGProperty(ePGProperty eType);
		void SetPGProperty(ePGProperty eType);
		__property bool get_PGUsePhysX();
		__property void set_PGUsePhysX(bool bUsePhysX);
		void SetPGUsePhysX(bool bUsePhysX, bool bUndoable);
		__property String* get_PGPostfixTexture();
		__property void set_PGPostfixTexture(String* kPostfixTexture);
		void SetPGPostfixTexture(String* kPostfixTexture, bool bUndoable);
		__property ePGAlphaGroup get_PGAlphaGroup();
		__property void set_PGAlphaGroup(ePGAlphaGroup eAlphaGroup);
		void SetPGAlphaGroup(ePGAlphaGroup eAlphaGroup, bool bUndoable);
		__property ePGOptimization get_PGOptimization();
		__property void set_PGOptimization(ePGOptimization eOptimization);
		void SetPGOptimization(ePGOptimization eOptimization, bool bUndoable);
		__property bool get_PGRandomAni();
		__property void set_PGRandomAni(bool bRandomAni);
		void SetPGRandomAni(bool bRandomAni, bool bUndoable);

		void GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeometries);
		void GetAllGeometries(NiObjectList &kGeometries);

		__property virtual bool get_Dirty();
        __property virtual void set_Dirty(bool bDirty);

        NiEntityInterface* GetNiEntityInterface();

        void Update(float fTime, NiExternalAssetManager* pkAssetManager);

        // ICustomTypeDescriptor overrides.

        PropertyDescriptorCollection* GetProperties();
        PropertyDescriptorCollection* GetProperties(
            Attribute* amAttributes[]);


    private:
        NiEntityInterface* m_pkEntity;
        static NiTObjectSet<NiFixedString>* m_pkPropertyNames;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        __gc class EntityDescriptor : public PropertyDescriptor
        {
        public:
            __value enum ValueType
            {
                Name,
				ProjectGProperty,
				ProjectGUsePhysX,
				ProjectGPostfixTexture,
				ProjectGAlphaGroup,
				ProjectGOptimization,
				ProjectGRandomAni,
                MasterEntity,
                Hidden,
                Frozen
            };

            EntityDescriptor(MEntity* pmEntity, ValueType eValueType,
                String* strName, Attribute* amAttributes[]);

            // PropertyDescriptor overrides.
            __property Type* get_ComponentType();
            __property bool get_IsReadOnly();
            __property Type* get_PropertyType();
            bool CanResetValue(Object* pmComponent);
            Object* GetValue(Object* pmComponent);
            void ResetValue(Object* pmComponent);
            void SetValue(Object* pmComponent, Object* pmValue);
            bool ShouldSerializeValue(Object* pmComponent);

        private:
            MEntity* m_pmEntity;
            ValueType m_eValueType;
        };

    private public:
        static void _SDMInit();
        static void _SDMShutdown();
        bool m_bFrozen;
		ePGProperty m_ePGProperty;
		bool m_bUsePhysX;
		String* m_kPostfixTexture;
		ePGAlphaGroup m_eAlphaGroup;
		ePGOptimization m_eOptimization;
		bool m_bRandomAni;

    // MDisposable members.
    protected:

        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
