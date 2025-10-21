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

#ifndef NID3DXEFFECTPARAMETER_H
#define NID3DXEFFECTPARAMETER_H

#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectParameterDesc.h"

#include <NiD3DShaderConstantMap.h>
#include <NiD3DTextureStage.h>
#include <NiFixedString.h>

class NiD3DXEffectAttributeTable;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameter : public NiMemObject
{
public:
    NiD3DXEffectParameter();
    ~NiD3DXEffectParameter();

    bool Initialize(NiD3DXEffectParameterDesc* pkParamDesc, 
        NiD3DXEffectAttributeTable* pkAttribTable);

    NiD3DError SetParameter(LPD3DXEFFECT pkEffect, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass, 
        bool bVertexShaderPresent);

    // *** begin Emergent internal use only ***

    const NiFixedString& GetName() const;

    void SetParameterPtr(D3DXParameterPtr pkParam);
    D3DXParameterPtr GetParameterPtr() const;

    void LinkHandle(LPD3DXEFFECT pkEffect);

    bool RequiresTime() const;

    void DestroyRendererData();

    void SetUsed(bool bUsed);
    bool GetUsed() const;

    NiD3DShaderConstantMap::DefinedMappings GetPredefinedMapping() const;
    unsigned int GetArrayCount() const;

    static void ReleaseBoneArray();

    // *** end Emergent internal use only ***

protected:
    void Construct2DTexture();
    void Construct3DTexture();
    void ConstructCubeTexture();

    bool FillPredefinedConstantValue(LPD3DXEFFECT pkEffect, 
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass, 
        bool bVertexShaderPresent);
    NiD3DError FillObjectConstantValue(LPD3DXEFFECT pkD3DXEffect,
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass);
    D3DBaseTexturePtr GetNiTexture(NiTexturingProperty* pkTexProp, 
        unsigned int uiTextureFlags);

    D3DXParameterPtr m_pkParam;
    NiFixedString m_kName;
    bool m_bGlobal;

    NiD3DShaderConstantMap::DefinedMappings m_ePredefinedMapping;
    NiD3DShaderConstantMap::ObjectMappings m_eObjectMapping;
    NiD3DXEffectParameterDesc::ParameterType m_eParameterType;
    NiD3DXEffectParameterTextureDesc::TextureType m_eTextureType;
    unsigned int m_uiParameterCount;
    unsigned int m_uiArrayCount;

    unsigned int m_uiExtraData;

    // Texture-specific variables
    char* m_pcTextureSource;
    char* m_pcTextureTarget;
    unsigned int m_uiTextureFlags;
    D3DBaseTexturePtr m_pkTexture;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiDepth;

    // Object-specific variables.
    NiShaderAttributeDesc::ObjectType m_eObjectType;

    bool m_bUsed;

    static D3DXVECTOR4 
        ms_akVector4Array[NiD3DXEffectParameterArrayDesc::MAX_ROWS];

    static D3DXMATRIX* ms_pkMatrixArray;
    static unsigned int ms_uiMatrixArraySize;
    static float ms_afObjectData[16];
};

#include "NiD3DXEffectParameter.inl"

#endif //NID3DXEFFECTPARAMETER_H
