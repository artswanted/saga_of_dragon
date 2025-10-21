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

#ifndef NIDX9RENDERSTATE_H
#define NIDX9RENDERSTATE_H

#include "NiD3DDefines.h"

#include "NiDX9MaterialManager.h"
#include "NiDX9RendererMetrics.h"
#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"
#include "NiDX9ShaderConstantManager.h"

#include <NiPropertyState.h>
#include <NiAlphaProperty.h>
#include <NiStencilProperty.h>
#include <NiSmartPointer.h>
#include <NiRefObject.h>
#include <NiZBufferProperty.h>
#include <NiColor.h>

class NiMatrix3;
class NiPoint3;
class NiFogProperty;
class NiDitherProperty;
class NiMaterialProperty;
class NiShadeProperty;
class NiSpecularProperty;
class NiTransform;
class NiWireframeProperty;

class NID3D_ENTRY NiDX9RenderState : public NiRefObject
{
    // *** begin Emergent internal use only ***
public:
    void SetD3DDevice(D3DDevicePtr pkD3DDevice);
    void SetD3DRenderer(NiD3DRenderer* pkD3DRenderer);
    // *** end Emergent internal use only ***

public:
    enum NiD3DRenderStateCaps
    {
        CAPS_TABLE_FOG      = 0x00000001,
        CAPS_ZBUFFER        = 0x00000002,
        CAPS_IND_UV_WRAP    = 0x00000004
    };

    enum NiD3DSamplerState
    {
        NISAMP_INVALID          = -1,
        NISAMP_ADDRESSU         = 0, 
        NISAMP_ADDRESSV, 
        NISAMP_ADDRESSW, 
        NISAMP_BORDERCOLOR, 
        NISAMP_MAGFILTER, 
        NISAMP_MINFILTER, 
        NISAMP_MIPFILTER, 
        NISAMP_MIPMAPLODBIAS, 
        NISAMP_MAXMIPLEVEL, 
        NISAMP_MAXANISOTROPY, 
        //*** DX9-only start
        NISAMP_SRGBTEXTURE, 
        NISAMP_ELEMENTINDEX, 
        NISAMP_DMAPOFFSET, 
        //*** DX9-only end
        // New ones go ABOVE here
        NISAMP_TOTAL_COUNT,
    };

protected:
    NiDX9RenderState(NiD3DRenderer* pkRenderer, const D3DCAPS9& kD3DCaps9);

public:
    virtual ~NiDX9RenderState();

    static NiDX9RenderState* Create(NiD3DRenderer* pkRenderer, 
        const D3DCAPS9& kD3DCaps9, bool bZBuffer);

    // Update render states according to the given property
    void UpdateRenderState(const NiPropertyState* pkNew,NiGeometry *pkGeometry = NULL);

    // public to allow TexturePipeline to override for multipass
    void ApplyAlpha(const NiAlphaProperty* pkNew,NiGeometry *pkGeometry = NULL);
    void ApplyDitherMode(const NiDitherProperty* pkNew);
    void ApplyFog(const NiFogProperty* pkNew);
    void ApplyMaterial(NiMaterialProperty* pkNew);
    void ApplyShading(const NiShadeProperty* pkNew);
    void ApplySpecular(const NiSpecularProperty* pkNew);
    void ApplyStencil(const NiStencilProperty* pkNew);
    void ApplyWireframe(const NiWireframeProperty* pkNew);
    void ApplyZBuffer(const NiZBufferProperty* pkNew);

    //*** Helper functions
    // Alphas
    void DisableAlpha();
    unsigned int GetSrcAlphaCaps();
    unsigned int GetDestAlphaCaps();
    unsigned int GetD3DBlendMode(NiAlphaProperty::AlphaFunction eBlend);
    unsigned int GetD3DTestMode(NiAlphaProperty::TestFunction eTest);

    // Transformation relative
    void SetBoneCount(unsigned short usNumBones);
    void SetModelTransform(const NiTransform& kXform);

    // 
    void SetAntiAlias(bool bOn);

    // Fog
    float GetMaxFogValue() const;
    float GetMaxFogFactor() const;
    void SetMaxFogValue(float fFogVal);

    // Camera
    void GetCameraNearAndFar(float& fNear, float& fFar);
    void SetCameraNearAndFar(float fNear, float fFar);
    bool GetLeftHanded();
    void SetLeftHanded(bool bLeft);

    // Purge a material from the system
    void PurgeMaterial(NiMaterialProperty* pkMaterial);

    // Render State management
    void InitRenderState();
    void SaveRenderState();
    void RestoreRenderState();

    void SetRenderState(D3DRENDERSTATETYPE eState, 
        unsigned int uiValue, bool bSave);
    void SetRenderState(D3DRENDERSTATETYPE eState, unsigned int uiValue);
    unsigned int GetRenderState(D3DRENDERSTATETYPE eState) const;
    void RestoreRenderState(D3DRENDERSTATETYPE eState);

    // Shader program management
    void InitShaderState();
    void SaveShaderState();
    void RestoreShaderState();

    void SetPixelShader(NiD3DPixelShaderHandle hShader, 
        bool bSave = false);
    NiD3DPixelShaderHandle GetPixelShader() const;
    void RestorePixelShader();
    void ClearPixelShader(NiD3DPixelShaderHandle hShader);

    void SetVertexShader(NiD3DVertexShaderHandle hShader, 
        bool bSave = false);
    NiD3DVertexShaderHandle GetVertexShader() const;
    void RestoreVertexShader();
    void ClearVertexShader(NiD3DVertexShaderHandle hShader);

    void SetFVF(unsigned int uiFVF, bool bSave = false);
    unsigned int GetFVF() const;
    void RestoreFVF();
    void ClearFVF(unsigned int uiFVF);

    void SetDeclaration(NiD3DVertexDeclaration hDeclaration, 
        bool bSave = false);
    NiD3DVertexDeclaration GetDeclaration() const;
    void RestoreDeclaration();
    void ClearDeclaration(NiD3DVertexDeclaration hDeclaration);

    // Texture Stage and Sampler State management
    void InitTextureStageState();
    void SaveTextureStageState();
    void RestoreTextureStageState();

	NiDX9ShaderConstantManager*	GetShaderConstantManager()	{	return	m_spShaderConstantManager;	}

    void SetTextureStageState(unsigned int uiStage, 
        D3DTEXTURESTAGESTATETYPE eState, unsigned int uiValue, 
        bool bSave);
    void SetTextureStageState(unsigned int uiStage, 
        D3DTEXTURESTAGESTATETYPE eState, unsigned int uiValue);
    unsigned int GetTextureStageState(unsigned int uiStage, 
        D3DTEXTURESTAGESTATETYPE eState) const;
    void RestoreTextureStageState(unsigned int uiStage, 
        D3DTEXTURESTAGESTATETYPE eState);

    void SetSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState, unsigned int uiValue, 
        bool bSave);
    void SetSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState, unsigned int uiValue);
    unsigned int GetSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState);
    void RestoreSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState);

    void SetSamplerState(unsigned int uiStage, D3DSAMPLERSTATETYPE eState, 
        unsigned int uiValue, bool bSave);
    void SetSamplerState(unsigned int uiStage, D3DSAMPLERSTATETYPE eState, 
        unsigned int uiValue);
    unsigned int GetSamplerState(unsigned int uiStage, 
        D3DSAMPLERSTATETYPE eState);
    void RestoreSamplerState(unsigned int uiStage, D3DSAMPLERSTATETYPE eState);

    // Texture management
    void InitTextures();
    void SetTexture(unsigned int uiStage, 
        D3DBaseTexturePtr pkTexture);
    D3DBaseTexturePtr GetTexture(unsigned int uiStage);
    void ClearTexture(D3DBaseTexturePtr pkTexture);

    // Shader Constant management
    bool SetVertexShaderConstantB(unsigned int uiStartRegister,
        const BOOL* pbConstantData, unsigned int uiBoolCount, 
        bool bSave = false);
    bool GetVertexShaderConstantB(unsigned int uiStartRegister,
        BOOL* pbConstantData, unsigned int uiBoolCount);
    bool RestoreVertexShaderConstantB(unsigned int uiStartRegister, 
        unsigned int uiBoolCount);
    bool SetVertexShaderConstantF(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetVertexShaderConstantF(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestoreVertexShaderConstantF(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);
    bool SetVertexShaderConstantI(unsigned int uiStartRegister,
        const int* piConstantData, unsigned int uiInt4Count, 
        bool bSave = false);
    bool GetVertexShaderConstantI(unsigned int uiStartRegister,
        int* piConstantData, unsigned int uiInt4Count);
    bool RestoreVertexShaderConstantI(unsigned int uiStartRegister, 
        unsigned int uiInt4Count);
    bool SetVertexShaderConstant(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetVertexShaderConstant(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestoreVertexShaderConstant(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);

    bool SetPixelShaderConstantB(unsigned int uiStartRegister,
        const BOOL* pbConstantData, unsigned int uiBoolCount, 
        bool bSave = false);
    bool GetPixelShaderConstantB(unsigned int uiStartRegister,
        BOOL* pbConstantData, unsigned int uiBoolCount);
    bool RestorePixelShaderConstantB(unsigned int uiStartRegister, 
        unsigned int uiBoolCount);
    bool SetPixelShaderConstantF(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetPixelShaderConstantF(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestorePixelShaderConstantF(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);
    bool SetPixelShaderConstantI(unsigned int uiStartRegister,
        const int* piConstantData, unsigned int uiInt4Count, 
        bool bSave = false);
    bool GetPixelShaderConstantI(unsigned int uiStartRegister,
        int* piConstantData, unsigned int uiInt4Count);
    bool RestorePixelShaderConstantI(unsigned int uiStartRegister, 
        unsigned int uiInt4Count);
    bool SetPixelShaderConstant(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetPixelShaderConstant(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestorePixelShaderConstant(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);

	void FastOverwriteVertexShaderConstantB(unsigned int uiStartRegister,
		const BOOL* pbConstantData, unsigned int uiBoolCount);
	void MarkVertexShaderConstantsDirtyB(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);
	void FastOverwriteVertexShaderConstantF(unsigned int uiStartRegister,
		const float* pfConstantData, unsigned uiFloat4Count);
	void MarkVertexShaderConstantsDirtyF(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);
	void FastOverwriteVertexShaderConstantI(unsigned int uiStartRegister,
		const int* piConstantData, unsigned int uiInt4Count);
	void MarkVertexShaderConstantsDirtyI(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);

    void FastOverwritePixelShaderConstantB(unsigned int uiStartRegister,
		const BOOL* pbConstantData, unsigned int uiBoolCount);
	void MarkPixelShaderConstantsDirtyB(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);
	void FastOverwritePixelShaderConstantF(unsigned int uiStartRegister,
		const float* pfConstantData, unsigned uiFloat4Count);
	void MarkPixelShaderConstantsDirtyF(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);
	void FastOverwritePixelShaderConstantI(unsigned int uiStartRegister,
		const int* piConstantData, unsigned int uiInt4Count);
	void MarkPixelShaderConstantsDirtyI(unsigned int uiFirstDirtyRegister,
		unsigned int uiFirstCleanRegister);

    void SetSoftwareVertexProcessing(bool bSoftware);
    bool GetSoftwareVertexProcessing() const;

    void SetForceNormalizeNormals(bool bSet);
    bool GetForceNormalizeNormals() const;

    // *** begin Emergent internal use only ***
    void SetInternalNormalizeNormals(bool bSet);
    bool GetInternalNormalizeNormals() const;

    void InitRenderModes();
    const unsigned int GetFlags();

    static unsigned int GetSamplerStateMapping(unsigned int uiSamplerState);

    void CommitShaderConstants();

    // *** end Emergent internal use only ***

protected:
    // Flag management
    void ClearFlags();
    void ClearFlag(unsigned int uiFlag);
    void SetFlag(unsigned int uiFlag);
    void ToggleFlag(unsigned int uiFlag);
    void CopyFlag(unsigned int uiFlag, bool bSet);
    bool IsFlag(unsigned int uiFlag);
    bool IsNotFlag(unsigned int uiFlag);

    void ReadRenderStateCaps();

    unsigned int m_uiFlags;

    enum VertexBlendModes
    {
        NIVBMODES_COUNT = 5
    };

    D3DVERTEXBLENDFLAGS m_aD3DVertexBlendMappings[NIVBMODES_COUNT];

    // Alpha Blending Members
    unsigned int m_auiAlphaMapping[NiAlphaProperty::ALPHA_MAX_MODES];
    unsigned int m_auiAlphaTestMapping[NiAlphaProperty::TEST_MAX_MODES];
    unsigned int m_uiSrcAlphaCaps;
    unsigned int m_uiDestAlphaCaps;

    NiAlphaPropertyPtr m_spDisabledAlphaProperty;

    // Fog Members
    float m_fCameraNear;
    float m_fCameraFar;
    float m_fCameraDepthRange;
    float m_fMaxFogFactor;
    float m_fMaxFogValue;
    NiColor m_kCurrentFogColor;
    unsigned int m_uiCurrentFogColor;

    // Stencil Members
    unsigned int m_auiStencilTestMapping[NiStencilProperty::TEST_MAX];
    unsigned int m_auiStencilActionMapping[NiStencilProperty::ACTION_MAX];
    unsigned int m_auiCullModeMapping[NiStencilProperty::DRAW_MAX][2];
    unsigned int m_uiLeftHanded;

    // Material Members
    NiDX9MaterialManager m_kMaterials;

    // Z Buffer Members
    unsigned int m_auiZBufferTestMapping[NiZBufferProperty::TEST_MAX_MODES];

    //*** Render state tracking
    class NiRenderStateSetting : public NiMemObject
    {
    public:
        NiRenderStateSetting();

        // SetValue will attempt to change the render state
        void SetValue(unsigned int uiValue, bool bSave);

        // InitValue only sets m_dwCurrValue - should only be used to
        // initialize value to the default state.
        void InitValue(unsigned int uiValue);

        void SaveValue();
        void RestoreValue();

        unsigned int GetValue() const;
        unsigned int GetPrevValue() const;

    private:
        unsigned int m_uiCurrValue;
        unsigned int m_uiPrevValue;
    };

    // Ensure these value is always greater than the max DX values!
    enum 
    { 
        NI_MAX_RENDER_STATES        = 256,
        NI_MAX_TEXTURE_STAGE_STATES = 33,
        NI_MAX_TEXTURE_STAGES       = 16,
		NI_MAX_SAMPLER_STATES		= 14
    };

    // Initial Values
    // Render States
    class RenderStateInitialValue 
    {
    public:
        D3DRENDERSTATETYPE m_eType;
        unsigned int m_uiValue;
    };

    static RenderStateInitialValue 
        ms_akInitialRenderStateSettings[NI_MAX_RENDER_STATES + 1];

    // Texture Stage States
    class TextureStageStateInitialValue 
    {
    public:
        D3DTEXTURESTAGESTATETYPE m_eType;
        unsigned int m_uiValue;
    };

    static TextureStageStateInitialValue 
        ms_akInitialTextureStageStateSettings[NI_MAX_TEXTURE_STAGE_STATES + 1];

    // Sampler States
    class SamplerStateInitialValue 
    {
    public:
        D3DSAMPLERSTATETYPE m_eType;
        unsigned int m_uiValue;
    };

    static SamplerStateInitialValue 
        ms_akInitialSamplerStateSettings[NI_MAX_SAMPLER_STATES + 1];

    // Tracking render states
    NiRenderStateSetting m_akRenderStateSettings[NI_MAX_RENDER_STATES];

    // Texture stage and sampler states
    NiRenderStateSetting m_akTextureStageStateSettings[
        NI_MAX_TEXTURE_STAGES][NI_MAX_TEXTURE_STAGE_STATES];
    NiRenderStateSetting m_akSamplerStateSettings[
        NI_MAX_TEXTURE_STAGES][NI_MAX_SAMPLER_STATES];

    // These need to be defined on a platform basis
    static unsigned int ms_auiSamplesStateMappings[NISAMP_TOTAL_COUNT];

    // Textures
    D3DBaseTexturePtr m_apkTextureStageTextures[NI_MAX_TEXTURE_STAGES];

    // Shader programs
    NiD3DVertexShaderHandle m_hCurrentVertexShader;
    NiD3DVertexShaderHandle m_hPreviousVertexShader;
    NiD3DPixelShaderHandle m_hCurrentPixelShader;
    NiD3DPixelShaderHandle m_hPreviousPixelShader;

    // Shader Constant Manager
    NiDX9ShaderConstantManagerPtr m_spShaderConstantManager;

    // Flag to indicate NORMALIZENORMALS should be forced
    bool m_bForceNormalizeNormals;
    // This is the INTERNAL-ONLY version!
    bool m_bInternalNormalizeNormals;

    // Software vertex processing
    bool m_bUsingSoftwareVP;

    // Vertex formats
    bool m_bDeclaration;
    unsigned int m_uiCurrentFVF;
    unsigned int m_uiPreviousFVF;
    NiD3DVertexDeclaration m_hCurrentVertexDeclaration;
    NiD3DVertexDeclaration m_hPreviousVertexDeclaration;

    D3DCAPS9 m_kD3DCaps9;

    D3DDevicePtr m_pkD3DDevice;
    NiD3DRenderer* m_pkD3DRenderer;
};

typedef NiPointer<NiDX9RenderState> NiDX9RenderStatePtr;

#include "NiDX9RenderState.inl"

#endif  //#ifndef NIDX9RENDERSTATE_H
