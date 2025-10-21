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

#ifndef NID3DSHADERCONSTANTMAP_H
#define NID3DSHADERCONSTANTMAP_H

#include <NiGeometry.h>
#include <NiShaderConstantMap.h>
#include <NiTArray.h>
#include "NiD3DDefines.h"
#include "NiD3DError.h"
#include "NiD3DRenderState.h"
#include "NiShaderConstantMapEntry.h"

class NiD3DShaderProgram;
class NiGeometryBufferData;

class NID3D_ENTRY NiD3DShaderConstantMap : public NiShaderConstantMap
{
public:
    NiD3DShaderConstantMap(NiD3DRenderer* pkRenderer, 
        NiGPUProgram::ProgramType eType);
    virtual ~NiD3DShaderConstantMap();

    //*** Access functions
    bool GetModified();
    void SetModified(bool bModified);
    NiD3DError GetLastError();

    //*** Entry access/manipulation functions
    unsigned int GetEntryCount();

    virtual NiShaderError AddEntry(const char* pszKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiShaderRegister, 
        unsigned int uiRegisterCount, const char* pszVariableName = NULL, 
        unsigned int uiDataSize = 0, unsigned int uiDataStride = 0, 
        const void* pvDataSource = NULL, bool bCopyData = false);
    virtual NiShaderError AddPredefinedEntry(const char* pszKey, 
        unsigned int uiExtra, unsigned int uiShaderRegister, 
        const char* pszVariableName);
    virtual NiShaderError AddAttributeEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = true);
    virtual NiShaderError AddConstantEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = true);
    virtual NiShaderError AddGlobalEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName, unsigned int uiDataSize, 
        unsigned int uiDataStride, const void* pvDataSource, 
        bool bCopyData = false);
    virtual NiShaderError AddOperatorEntry(const char* pszKey, 
        unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiShaderRegister, unsigned int uiRegisterCount, 
        const char* pszVariableName);
    virtual NiShaderError AddObjectEntry(const char* pszKey,
        unsigned int uiShaderRegister, const char* pszVariableName,
        unsigned int uiObjectIndex,
        NiShaderAttributeDesc::ObjectType eObjectType);

    virtual NiD3DError RemoveEntry(const char* pszKey);
    virtual NiShaderConstantMapEntry* GetEntry(const char* pszKey);
    virtual NiShaderConstantMapEntry* GetEntryAtIndex(
        unsigned int uiIndex);

    //*** Pipeline functions
    virtual NiD3DError SetShaderConstants(NiD3DShaderProgram* pkShaderProgram, 
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass = 0, 
        bool bGlobal = false);

    // *** begin Emergent internal use only ***
    void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    virtual unsigned int GetEntryIndex(const char* pszKey);

    static void SetupTextureTransformMatrix(D3DXMATRIX& kResult, 
        const NiMatrix3* pkTexMatrix = NULL, bool bInv = false, 

        bool bTrans = false, unsigned int uiTexCoordSize = 4);

    static bool GetDynamicEffectData(void* pvData, unsigned int uiDataSize,
        ObjectMappings eMapping, NiDynamicEffect* pkDynEffect,
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin,
        const NiSkinPartition::Partition* pkPartition,
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState,
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld,
        const NiBound& kWorldBound, unsigned int uiPass);
    // *** end Emergent internal use only ***

protected:
    void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    void SetD3DRenderState(NiD3DRenderState* pkRS);

    virtual NiD3DError InsertEntry(NiShaderConstantMapEntry* pkEntry);
    virtual NiD3DError SetupPredefinedEntry(
        NiShaderConstantMapEntry* pkEntry);
    virtual NiD3DError SetDefinedConstant(
        NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass);
    virtual NiD3DError SetConstantConstant(
        NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, unsigned int uiPass);
    virtual NiD3DError SetAttributeConstant(
        NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass, 
        bool bGlobal = false, NiExtraData* pkExtraData = 0);
    virtual NiD3DError SetGlobalConstant(
        NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, unsigned int uiPass);
    virtual NiD3DError SetOperatorConstant(
        NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld,
        const NiBound& kWorldBound, unsigned int uiPass);
    virtual NiD3DError SetupObjectEntry(NiShaderConstantMapEntry* pkEntry);
    virtual NiD3DError SetObjectConstant(NiD3DShaderProgram* pkShaderProgram,
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry,
        const NiSkinInstance* pkSkin,
        const NiSkinPartition::Partition* pkPartition,
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState,
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld,
        const NiBound& kWorldBound, unsigned int uiPass);

    // Attribute Mapping Helper functions
    const void* MapAttributeValue(NiShaderConstantMapEntry* pkEntry,
        NiExtraData* pkExtra);
    const void* MapAttributeArrayValue(NiShaderConstantMapEntry* pkEntry,
        NiExtraData* pkExtra);

    // Constant Mapping Helper functions
    NiShaderError SetShaderConstantArray(NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry, const float * pfData);

    bool ComputeRestridingValues(NiShaderConstantMapEntry* pkEntry, 
        unsigned int & uiRegistersPerElement, 
        unsigned int & uiItemsPerRegister);

    const void* MapConstantValue(NiShaderConstantMapEntry* pkEntry);

    const void* MapConstantArray(NiShaderConstantMapEntry* pkEntry, 
        const float * pfSource);

    static float ms_vTaylorSin[4];
    static float ms_vTaylorCos[4];

    NiTPrimitiveArray<float> m_afArrayMapping;
    static float ms_vMappingValue[4];
    static D3DXMATRIX ms_mMappingValue;

    static BOOL ms_bMappingValue;
    static int ms_aiMappingValue[4];

    NiTObjectArray<NiShaderConstantMapEntryPtr> m_aspEntries;
    bool m_bModified;

    NiD3DShaderProgram* m_pkLastShaderProgram;
    NiD3DError m_eLastError;

    D3DDevicePtr m_pkD3DDevice;
    NiD3DRenderer* m_pkD3DRenderer;
    NiD3DRenderState* m_pkD3DRenderState;

    static D3DXALIGNEDMATRIX ms_kMatrices[2];
    static float ms_afVector4[2][4];
    static D3DXALIGNEDMATRIX ms_kMatrixResult;
    static float ms_fVector4Result[4];

    unsigned int SetupDefinedConstantValue(unsigned int uiOperatorNum, 
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld,
        const NiBound& kWorldBound);
    unsigned int SetupConstantConstantValue(unsigned int uiOperatorNum, 
        NiShaderConstantMapEntry* pkEntry, unsigned int uiPass);
    unsigned int SetupAttributeConstantValue(unsigned int uiOperatorNum, 
        NiShaderConstantMapEntry* pkEntry, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
        const NiTransform& kWorld, const NiBound& kWorldBound, 
        unsigned int uiPass);
    unsigned int SetupGlobalConstantValue(unsigned int uiOperatorNum, 
        NiShaderConstantMapEntry* pkEntry, unsigned int uiPass);

    NiD3DError PerformOperatorMultiply(NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry,
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2, bool bInverse, 
        bool bTranspose);
    NiD3DError PerformOperatorDivide(NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry,
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2, bool bInverse, 
        bool bTranspose);
    NiD3DError PerformOperatorAdd(NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry,
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2, bool bInverse, 
        bool bTranspose);
    NiD3DError PerformOperatorSubtract(NiD3DShaderProgram* pkShaderProgram, 
        NiShaderConstantMapEntry* pkEntry,
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2, bool bInverse, 
        bool bTranspose);

    static const float ms_fDirLightDistance;
    static float ms_afObjectData[16];
};

typedef NiPointer<NiD3DShaderConstantMap> NiD3DShaderConstantMapPtr;

#include "NiD3DShaderConstantMap.inl"

#endif  //#ifndef NID3DSHADERCONSTANTMAP_H
