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

#ifndef NID3DPASS_H
#define NID3DPASS_H

#include <NiAlphaProperty.h>
#include <NiCriticalSection.h>

#include "NiD3DTextureStage.h"
#include "NiD3DShaderConstantMap.h"
#include "NiD3DPixelShader.h"
#include "NiD3DVertexShader.h"
#include "NiD3DRenderStateGroup.h"

class NiGeometryBufferData;
NiSmartPointer(NiD3DPass);

// NiD3DPass
// A pass consists of a number of TextureStages, representing a single pass 
// in the rendering pipeline. These will vary according to the hardware that 
// Gamebryo is currently running on.
// IT IS THE USERS RESPONSIBILITY TO VERIFY THEIR PASSES!
// Gamebryo will perform some validation in Debug builds, but for speed
// reasons, will not do any during a Release build. It is assumed that if
// the user is advanced enough to work at this level, then they will have
// a proprietary scheme for tracking and validation texture passes.
class NID3D_ENTRY NiD3DPass : public NiMemObject
{
protected:
    // Do not set these to anything. Use them only for accessing required
    // functionality. You may assume they will ALWAYS be valid.
    static D3DDevicePtr ms_pkD3DDevice;
    static NiD3DRenderer* ms_pkD3DRenderer;
    static NiD3DRenderState* ms_pkD3DRenderState;

    // *** begin Emergent internal use only ***
    static void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    static void SetD3DRenderState(NiD3DRenderState* pkRS);

public:
    static void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);

    D3DDevicePtr GetD3DDevice();
    NiD3DRenderer* GetD3DRenderer();
    NiD3DRenderState* GetD3DRenderState();
    // *** end Emergent internal use only ***

public:
    NiD3DPass();
    virtual ~NiD3DPass();

    static NiD3DPassPtr CreateNewPass();

    //*** Access functions
    
    // Name
    const char* GetName() const;
    void SetName(char* pcName);

    // Render State Group
    NiD3DRenderStateGroup* GetRenderStateGroup() const;
    void SetRenderStateGroup(NiD3DRenderStateGroup* pkRenderStateGroup);
    void SetRenderState(unsigned int uiRenderState, unsigned int uiValue,
        bool bSave = false);
    bool RemoveRenderState(unsigned int uiRenderState);
    bool GetRenderState(unsigned int uiRenderState, unsigned int& uiValue,
        bool& bSave) const;

    // Pixel shader
    NiD3DShaderConstantMap* GetPixelConstantMap() const;
    void SetPixelConstantMap(NiD3DShaderConstantMap* pkSCMPixel);
    const char* GetPixelShaderProgramFileName() const;
    void SetPixelShaderProgramFileName(const char* pcProgramFile);
    const char* GetPixelShaderProgramEntryPoint() const;
    void SetPixelShaderProgramEntryPoint(const char* pcEntryPoint);
    const char* GetPixelShaderProgramShaderTarget() const;
    void SetPixelShaderProgramShaderTarget(const char* pcShaderTarget);
    NiD3DPixelShader* GetPixelShader() const;
    void SetPixelShader(NiD3DPixelShader* pkPixelShader);

    // Vertex shader
    NiD3DShaderConstantMap* GetVertexConstantMap() const;
    void SetVertexConstantMap(NiD3DShaderConstantMap* pkSCMVertex);
    const char* GetVertexShaderProgramFileName() const;
    void SetVertexShaderProgramFileName(const char* pcProgramFile);
    const char* GetVertexShaderProgramEntryPoint() const;
    void SetVertexShaderProgramEntryPoint(const char* pcEntryPoint);
    const char* GetVertexShaderProgramShaderTarget() const;
    void SetVertexShaderProgramShaderTarget(const char* pcShaderTarget);
    NiD3DVertexShader* GetVertexShader() const;
    void SetVertexShader(NiD3DVertexShader* pkVertexShader);

    // Software vertex processing
    bool GetSoftwareVertexProcessing() const;
    void SetSoftwareVertexProcessing(bool bSoftwareVP);

    // Stage(s)
    unsigned int GetCurrentStage() const;
    unsigned int GetStageCount() const;
    unsigned int GetTextureCount() const;
    NiD3DTextureStage* GetStage(unsigned int uiStageNum) const;
    NiD3DError SetStage(unsigned int uiStageNum, NiD3DTextureStage* pkStage);
    NiD3DError AppendStage(NiD3DTextureStage* pkStage);

    //*** Pipeline functions
    // Set the shader constants
    virtual unsigned int SetupShaderPrograms(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass = 0);

    // Setup the pass for rendering the given object
    virtual unsigned int SetupRenderingPass(NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
        const NiTransform& kWorld, const NiBound& kWorldBound, 
        unsigned int uiPass = 0);

    // End the pass
    virtual unsigned int PostProcessRenderingPass(unsigned int uiPass = 0);

    //*** Helper functions
    // Return true is there are no stages set in the pass
    bool IsPassEmpty() const;
    // Validate the current pass on the given device
    unsigned int ValidatePass();
    // Return whether the current stage can fit at the end of the pass
    bool CheckFreeStages(unsigned int uiStages = 1, 
        unsigned int uiTextures = 1) const;
    // Return the total available slots/texture slots
    unsigned int GetAvailableStages() const;
    unsigned int GetAvailableTextures() const;
    unsigned int GetTotalAvailableStages() const;
    bool IsPassContiguous() const;
    void ReleaseTextureStages();

    // *** begin Emergent internal use only ***

    void SetRendererOwned();
    bool IsRendererOwned() const;

    static void SetMaxTextureBlendStages(
        unsigned int uiMaxTextureBlendStages);
    static void SetMaxSimultaneousTextures(
        unsigned int uiMaxSimultaneousTextures);
    static void SetMaxSamplers(unsigned int uiMaxSamplers);

    static void InitializePools();
    static void ShutdownPools();

    // For use with NiSmartPointers
    void IncRefCount();
    void DecRefCount();

    // *** end Emergent internal use only ***

public:
    enum
    {
        NID3DPASS_NAME_LEN  = 16
    };

    //  Max number of blending stages supported.
    //  Should correspond to the number of pixel shader instructions allowed.
    static unsigned int ms_uiMaxTextureBlendStages;
    //  Max number of textures that can be bound to the blending stages.
    //  Should correspond to the number of texture registers supported by the
    //  pixel shaders.
    static unsigned int ms_uiMaxSimultaneousTextures;
    //  Max number of samplers supported.
    //  Can only be greater than ms_uiMaxTextureBlendStages under DX9 when
    //  PS2.0 (or greater) hardware is supported.
    static unsigned int ms_uiMaxSamplers;

protected:
    void ReturnPassToPool();

    char m_szName[NID3DPASS_NAME_LEN];      // User tag Name

    unsigned int m_uiCurrentStage;          // Current stage (internal)
    unsigned int m_uiStageCount;            // Number of active stages
    unsigned int m_uiTexturesPerPass;       // Textures used in this pass

    // Array of active stages
    NiTObjectArray<NiD3DTextureStagePtr> m_kStages;

    // 'Local' render state settings for the pass
    NiD3DRenderStateGroup* m_pkRenderStateGroup;

    //  Pixel shader related
    //  NOTE: Using the pixel shader overrides the setting of many texture
    //  stages - please see the documentation.
    NiD3DShaderConstantMapPtr m_spPixelConstantMap;    // constant map
    char* m_pcPixelShaderProgramFile;
    char* m_pcPixelShaderEntryPoint;
    char* m_pcPixelShaderTarget;
    NiD3DPixelShaderPtr m_spPixelShader;       // shader program

    //  Vertex shader related
    NiD3DShaderConstantMapPtr m_spVertexConstantMap;  // constant map
    char* m_pcVertexShaderProgramFile;
    char* m_pcVertexShaderEntryPoint;
    char* m_pcVertexShaderTarget;
    NiD3DVertexShaderPtr m_spVertexShader;     // shader program

    bool m_bSoftwareVP;

    // Internal flags
    bool m_bRendererOwned;
    unsigned int m_uiRefCount;

    static NiCriticalSection ms_kPassPoolCriticalSection;
    static NiTObjectPool<NiD3DPass>* ms_pkPassPool;
};

#include "NiD3DPass.inl"

#endif  //#ifndef NID3DPASS_H
