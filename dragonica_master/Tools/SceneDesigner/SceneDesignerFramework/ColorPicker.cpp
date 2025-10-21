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

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "ColorPicker.h"
#include <NiD3DShaderFactory.h>
#include <NiDX9RenderedTextureData.h>

//---------------------------------------------------------------------------
// default to a 1x1 pixel frustum
unsigned int ColorPicker::ms_uiDefaultPickW = 1;
unsigned int ColorPicker::ms_uiDefaultPickH = 1;
//---------------------------------------------------------------------------
ColorPicker::ColorPicker() : m_eFormat(D3DFMT_UNKNOWN)
{
    NiD3DRenderer* pkD3DRenderer =
        NiSmartPointerCast(NiD3DRenderer, NiRenderer::GetRenderer());
    m_pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    assert(m_pkD3DDevice);
    D3D_POINTER_REFERENCE(m_pkD3DDevice);

    // Set reset notification function
#if !defined(_XENON)
    pkD3DRenderer->AddResetNotificationFunc(
        ColorPicker::ResetNotificationFunction, this);
#endif  //#if !defined(_XENON)

    CreateRenderTarget();
    CreatePropertyAndEffectStates();
    CreateMaterial();
}
//---------------------------------------------------------------------------
ColorPicker::~ColorPicker()
{
    if (m_pkD3DReadTexture)
        D3D_POINTER_RELEASE(m_pkD3DReadTexture);
    D3D_POINTER_RELEASE(m_pkD3DDevice);

    m_pkD3DTexture = 0;
    m_pkD3DReadTexture = 0;
    m_pkD3DDevice = 0;
}
//---------------------------------------------------------------------------
void ColorPicker::StartPick(NiCamera* pkCamera, unsigned int uiPickX, 
    unsigned int uiPickY, unsigned int uiScreenWidth,
    unsigned int uiScreenHeight)
{
    m_spObjPickShader->ResetGeometryIndex();

    m_spCamera = pkCamera;

    m_kSaveFrustum = pkCamera->GetViewFrustum();
    NiFrustum kNewFrustum;

    m_fScaleHorz = (m_kSaveFrustum.m_fRight - m_kSaveFrustum.m_fLeft) / 
        uiScreenWidth;
    m_fScaleVert = (m_kSaveFrustum.m_fTop - m_kSaveFrustum.m_fBottom) / 
        uiScreenHeight;

    kNewFrustum.m_fNear = m_kSaveFrustum.m_fNear;
    kNewFrustum.m_fFar = m_kSaveFrustum.m_fFar;

    float fCenterH = m_fScaleHorz * uiPickX;
    float fCenterV = m_fScaleVert * uiPickY;
    float fOffsetH = m_fScaleHorz * (((float)ms_uiDefaultPickW) / 2.0f);
    float fOffsetV = m_fScaleVert * (((float)ms_uiDefaultPickH) / 2.0f);

    kNewFrustum.m_fLeft = m_kSaveFrustum.m_fLeft + (fCenterH - fOffsetH);
    kNewFrustum.m_fRight = kNewFrustum.m_fLeft + (fOffsetH * 2);
    kNewFrustum.m_fTop = m_kSaveFrustum.m_fTop - (fCenterV - fOffsetV);
    kNewFrustum.m_fBottom = kNewFrustum.m_fTop - (fOffsetV * 2);

    m_spCamera->SetViewFrustum(kNewFrustum);
    m_spCamera->Update(0.0f);

    // Begin rendering frame.
    NiRenderer::GetRenderer()->BeginOffScreenFrame();

    // Open the render target for geometry rendering (clear in the process)
    NiRenderer::GetRenderer()->GetBackgroundColor(m_kBackgroundColor);
    NiRenderer::GetRenderer()->SetBackgroundColor(NiColorA::WHITE);
    NiRenderer::GetRenderer()->BeginUsingRenderTargetGroup(
        m_spRenderTargetGroup, NiRenderer::CLEAR_ALL);

    //  Set the camera data
    NiRenderer::GetRenderer()->SetCameraData(m_spCamera);

    // Start the batch
    NiRenderer::GetRenderer()->BeginBatch(m_spPropertyState, 0);

    m_bFirstObject = true;
    m_pkFirstGeom = 0;
}
//---------------------------------------------------------------------------
void ColorPicker::PickRender(unsigned int uiColor, NiAVObject* pkObject)
{
    NiVisibleArray kVisibleSet;
    NiCullingProcess kCuller(&kVisibleSet);
    NiCullScene(m_spCamera, pkObject, kCuller, kVisibleSet);

    for (unsigned int ui = 0; ui < kVisibleSet.GetCount(); ui++)
    {
        NiGeometry* pkGeom = &kVisibleSet.GetAt(ui);
        NiGeometryData* pkData = pkGeom->GetModelData();
        NiSkinInstance* pkSkin = pkGeom->GetSkinInstance();

        if (m_bFirstObject)
        {
            // Batch rendering will use the shader of the first object for 
            // all objects rendered in the batch... Store the first object 
            // and it's shader so we can replace it when we are done. 
            m_bFirstObject = false;
            m_pkFirstGeom = pkGeom;
            m_pkFirstMaterial = m_pkFirstGeom->GetActiveMaterial();
            m_pkFirstGeom->ApplyAndSetActiveMaterial(m_spObjPickMaterial);
        }

        if (NiIsKindOf(NiTriShape, pkGeom))
        {
            NiRenderer::GetRenderer()->BatchRenderShape((NiTriShape*) pkGeom);
        }
        else if (NiIsKindOf(NiTriStrips, pkGeom))
        {
            NiRenderer::GetRenderer()->BatchRenderStrips((NiTriStrips*)
                pkGeom);
        }
        else
        {
            // No other NiGeometry types are supported for picking.
            return;
        }

        // We have to do this after the call to render so that if there
        // are partitions, they have been definitely been created
        if (!pkSkin)
        {
            m_spObjPickShader->RegisterColor(uiColor);
        }
        else
        {
            //  For each partition in this skin instance, we need to set
            //  the current geometry object...
            NiSkinPartition* pkPartition = pkSkin->GetSkinPartition();
            for (unsigned int ui = 0;
                ui < pkPartition->GetPartitionCount(); ui++)
            {
                m_spObjPickShader->RegisterColor(uiColor);
            }
        }
    }
}
//---------------------------------------------------------------------------
unsigned int ColorPicker::EndPick()
{
    const NiAVObject* pkPicked = NULL;

    m_spObjPickShader->ResetGeometryIndex();

    //  End the batch...
    NiRenderer::GetRenderer()->EndBatch();

    // Replace the first objects shader
    if (m_pkFirstGeom)
    {
        m_pkFirstGeom->SetActiveMaterial(m_pkFirstMaterial);
    }

    // Close the render target to allow rendering to complete
    NiRenderer::GetRenderer()->EndUsingRenderTargetGroup();

    //  Now, see what color at the point of the pixel
    D3DLOCKED_RECT kLockRect;
    RECT kCheckRect;

    kCheckRect.left = ms_uiDefaultPickW / 2;
    kCheckRect.top = ms_uiDefaultPickH / 2;
    kCheckRect.right = kCheckRect.left + 1;
    kCheckRect.bottom = kCheckRect.top + 1;

    //  We have to copy the rendered texture to the 'read' texture
    D3DSurfacePtr pkSrc, pkDst;

    m_pkD3DTexture->GetSurfaceLevel(0, &pkSrc);
    m_pkD3DReadTexture->GetSurfaceLevel(0, &pkDst);

#if defined(_DX9)
    m_pkD3DDevice->GetRenderTargetData(pkSrc, pkDst);
#endif  //#if defined(_DX9)
    pkSrc->Release();
    pkDst->Release();

    unsigned int uiColor = 0x00ffffff;
    //  Now, check the color
    if (SUCCEEDED(m_pkD3DReadTexture->LockRect(0, &kLockRect, &kCheckRect, 
        D3DLOCK_READONLY)))
    {
        //  Find the pixel...

        // Only support 32-bit rendered texture formats.
        switch (m_eFormat)
        {
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8:
            uiColor = *((unsigned int*)(kLockRect.pBits));
            break;
        default:
            assert(0);
            break;
        }

        m_pkD3DReadTexture->UnlockRect(0);
    }

    //  Restore the camera
    m_spCamera->SetViewFrustum(m_kSaveFrustum);
    m_spCamera->Update(0.0f);
    NiRenderer::GetRenderer()->SetBackgroundColor(m_kBackgroundColor);
    m_spCamera = 0;

    // End rendering frame.
    NiRenderer::GetRenderer()->EndOffScreenFrame();

    return uiColor;
}
//---------------------------------------------------------------------------
void ColorPicker::CreateRenderTarget()
{
    m_spRenderTexture = NiRenderedTexture::Create(ms_uiDefaultPickW,
        ms_uiDefaultPickH, NiRenderer::GetRenderer());
    assert(m_spRenderTexture);

    m_spRenderTargetGroup = NiRenderTargetGroup::Create(
        m_spRenderTexture->GetBuffer(),
        NiRenderer::GetRenderer(), true, true);

    NiDX9RenderedTextureData* pkRenderData = 
        (NiDX9RenderedTextureData*) m_spRenderTexture->GetRendererData();

    m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());

    D3DSURFACE_DESC kSurfDesc;

    m_pkD3DTexture->GetLevelDesc(0, &kSurfDesc);

    //  We also need to create a texture that we can lock and read for 
    //  getting the pixel color out...
    m_eFormat = kSurfDesc.Format;
    m_pkD3DDevice->CreateTexture(kSurfDesc.Width, kSurfDesc.Height,
        1, 0, kSurfDesc.Format, D3DPOOL_SYSTEMMEM, &m_pkD3DReadTexture
#if defined(_DX9) || defined(_XENON)
        , 0);
#else   //#if defined(_DX9) || defined(_XENON)
        );
#endif  //#if defined(_DX9) || defined(_XENON)
    assert(m_pkD3DReadTexture);
}
//---------------------------------------------------------------------------
void ColorPicker::CreatePropertyAndEffectStates()
{
    m_spPropertyState = NiNew NiPropertyState();
    assert(m_spPropertyState);

    NiZBufferProperty* pkZBuffer = NiNew NiZBufferProperty();
    assert(pkZBuffer);
    pkZBuffer->SetZBufferTest(true);
    pkZBuffer->SetZBufferWrite(true);
    m_spPropertyState->SetProperty(pkZBuffer);

    NiTexturingProperty* pkTexProp = NiNew NiTexturingProperty;
    m_spPropertyState->SetProperty(pkTexProp);
}
//---------------------------------------------------------------------------
void ColorPicker::CreateMaterial()
{
    //  Create the Shader instance
    m_spObjPickShader = NiNew ObjectPickShader();
    assert(m_spObjPickShader);
    m_spObjPickShader->Initialize();

    m_spObjPickMaterial = NiSingleShaderMaterial::Create(m_spObjPickShader);
}
//---------------------------------------------------------------------------
bool ColorPicker::ResetNotificationFunction(bool bBeforeReset, 
    void* pvData)
{
    ColorPicker* pkColorPicker = (ColorPicker*)pvData;
    return pkColorPicker->HandleReset(bBeforeReset);
}
//---------------------------------------------------------------------------
bool ColorPicker::HandleReset(bool bBeforeReset)
{
    // Only need to handle what happens after the device is reset
    if (!bBeforeReset)
    {
        // Re-grab cached D3D texture because it has been released and
        // recreated as a result of the device reset
        NiDX9RenderedTextureData* pkRenderData = 
            (NiDX9RenderedTextureData*) m_spRenderTexture->GetRendererData();
        m_pkD3DTexture = (D3DTexturePtr)(pkRenderData->GetD3DTexture());
    }

    return true;
}
//---------------------------------------------------------------------------
