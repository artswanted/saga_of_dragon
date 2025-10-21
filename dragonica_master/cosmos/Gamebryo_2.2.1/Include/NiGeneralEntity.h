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

#ifndef NIGENERALENTITY_H
#define NIGENERALENTITY_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityInterface.h"
#include <NiTPtrSet.h>

class NiEntityComponentInterface;

class NIENTITY_ENTRY NiGeneralEntity : public NiRefObject,
    public NiEntityInterface
{
public:
    NiGeneralEntity(
        const NiFixedString& kName, 
        const NiUniqueID& kTemplateID,
        unsigned int uiComponentArraySize = 0);

    // *** begin Emergent internal use only ***
    NiGeneralEntity();
    // *** end Emergent internal use only ***

protected:
    NiTObjectPtrSet<NiEntityComponentInterfaceIPtr> m_kComponents;
    NiFixedString m_kName;
    NiEntityInterfaceIPtr m_spMasterEntity;
    bool m_bHidden;
    NiUniqueID m_kTemplateID;

	// PG
	// ProjectG Entity Property Type
    NiFixedString m_kPGProperty;
	// ProjectG Use PhysX
	bool m_bPGUsePhysX;
	// ProjectG Select Texture Type
	NiFixedString m_kPGPostfixTexture;
	// ProjectG Optimization
	NiFixedString m_kPGOptimization;
	// ProjectG AlphaGroup
	NiFixedString m_kPGAlphaGroup;
	// ProjectG Random Ani
	bool m_bPGRandomAni;

    // Class name.
    static NiFixedString ms_kClassName;

public:
    // NiEntityInterface overrides.
    virtual NiEntityInterface* Clone(const NiFixedString& kNewName,
        bool bInheritProperties);
    virtual NiEntityInterface* GetMasterEntity();
    virtual void ReplaceMasterEntity(NiEntityInterface* pkMasterEntity);
    virtual NiBool GetHidden() const;
    virtual void SetHidden(bool bHidden);
	
	// ProjecrG(PG) ����
    virtual NiFixedString GetPGProperty() const;
	virtual void SetPGProperty(const NiFixedString& kTypeName);
    virtual bool GetPGUsePhysX() const;
	virtual void SetPGUsePhysX(const bool bUsePhysX);
    virtual NiFixedString GetPGPostfixTexture() const;
	virtual void SetPGPostfixTexture(const NiFixedString& kPostfixTexture);
	virtual NiFixedString GetPGOptimization() const;
	virtual void SetPGOptimization(const NiFixedString& kOptimization);
	virtual NiFixedString GetPGAlphaGroup() const;
	virtual void SetPGAlphaGroup(const NiFixedString& kAlphaGroup);
    virtual bool GetPGRandomAni() const;
	virtual void SetPGRandomAni(const bool bRandomAni);

    virtual NiBool SupportsComponents();
    virtual NiBool AddComponent(NiEntityComponentInterface* pkComponent,
        bool bPerformErrorChecking);
    virtual unsigned int GetComponentCount() const;
    virtual NiEntityComponentInterface* GetComponentAt(unsigned int uiIndex)
        const;
    virtual NiEntityComponentInterface* GetComponentByTemplateID(
        const NiUniqueID& kTemplateID) const;
    virtual NiBool RemoveComponent(NiEntityComponentInterface* pkComponent,
        bool bPerformErrorChecking);
    virtual NiBool RemoveComponentAt(unsigned int uiIndex,
        bool bPerformErrorChecking);
    virtual void RemoveAllComponents();
    virtual NiBool IsComponentProperty(const NiFixedString& kPropertyName, 
        bool& bIsComponentProperty);

    // NiEntityPropertyInterface overrides.
    virtual NiBool SetTemplateID(const NiUniqueID& kTemplateID);
    virtual NiUniqueID GetTemplateID();
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetClassName() const;
    virtual NiFixedString GetName() const;
    virtual NiBool SetName(const NiFixedString& kName);
    virtual NiBool IsAnimated() const;
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);
    virtual void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const;
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const;
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName);
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique);
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName);
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType);
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType);
    virtual NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription);
    virtual NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const;
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly);
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique);
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable);
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable);
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        unsigned int uiIndex, bool& bIsExternalAssetPath) const;
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName,
        unsigned int& uiCount) const;
    virtual NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        float& fData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        int& iData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned int& uiData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        short& sData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned short& usData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint2& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint3& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiQuaternion& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiMatrix3& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColor& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColorA& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        void*& pvData, size_t& stDataSizeInBytes, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float fData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        int iData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned int uiData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        short sData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned short usData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint2& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint3& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiQuaternion& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiMatrix3& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColor& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColorA& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const void* pvData, size_t stDataSizeInBytes, unsigned int uiIndex);

    // *** begin Emergent internal use only ***
    virtual void SetMasterEntity(NiEntityInterface* pkMasterEntity);
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***
};

NiSmartPointer(NiGeneralEntity);

#endif // NIGENERALENTITY_H
