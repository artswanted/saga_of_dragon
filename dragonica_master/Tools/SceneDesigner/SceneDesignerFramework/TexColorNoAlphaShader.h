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

#ifndef TEXCOLORNOALPHASHADER_H
#define TEXCOLORNOALPHASHADER_H

class TexColorNoAlphaShader : public NiD3DShader
{
public:
    TexColorNoAlphaShader();
    virtual ~TexColorNoAlphaShader();

    virtual unsigned int UpdatePipeline(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        NiGeometryData::RendererData* pkRendererData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        const NiTransform& kWorld, const NiBound& kWorldBound);

protected:
    bool CreateStagesAndPasses();
    bool PresetStages(NiD3DTextureStage* pkStage);

    NiD3DTextureStage* m_pkStage;
};

typedef NiPointer<TexColorNoAlphaShader> TexColorNoAlphaShaderPtr;

#endif  // #ifndef TEXCOLORNOALPHASHADER_H
