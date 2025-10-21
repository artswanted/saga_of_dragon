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

#ifndef OBJECTPICKSHADER_H
#define OBJECTPICKSHADER_H

class ObjectPickShader : public NiD3DShader
{
public:
    ObjectPickShader();
    virtual ~ObjectPickShader();

    // Override these functions to implement a custom pipeline...
    virtual unsigned int PreProcessPipeline(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        const NiTransform& kWorld, const NiBound& kWorldBound);
    virtual unsigned int SetupShaderPrograms(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound);
    virtual unsigned int PostProcessPipeline(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        const NiTransform& kWorld, const NiBound& kWorldBound);

    // Usage functions
    void ResetGeometryIndex();
    void RegisterColor(unsigned int uiColor);

protected:
    bool CreateStagesAndPasses();
    bool PresetStages(NiD3DTextureStage* pkStage);

    //  Functions for accessing the color/object mapping
    void ResetColorArray();

    unsigned int m_uiGeometryIndex;
    NiTPrimitiveArray<unsigned int> m_kColorArray;
};

typedef NiPointer<ObjectPickShader> ObjectPickShaderPtr;

#endif  //#ifndef OBJECTPICKSHADER_H
