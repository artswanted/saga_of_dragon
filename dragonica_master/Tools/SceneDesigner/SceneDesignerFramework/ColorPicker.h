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

#ifndef COLORPICKER_H
#define COLORPICKER_H

#include "ObjectPickShader.h"

class ColorPicker : public NiRefObject
{
public:
    ColorPicker();
    ~ColorPicker();

    void StartPick(NiCamera* pkCamera, unsigned int uiPickX, 
        unsigned int uiPickY, unsigned int uiScreenWidth,
        unsigned int uiScreenHeight);
    void PickRender(unsigned int uiColor, NiAVObject* pkObject);
    unsigned int EndPick();

    void ToggleZBuffer();

    NiRenderedTexture* GetRenderTexture()
    {
        return m_spRenderTexture;
    };

protected:
    void CreateRenderTarget();
    void CreatePropertyAndEffectStates();
    void CreateMaterial();

    // Functions for handling device reset
    static bool ResetNotificationFunction(bool bBeforeReset, void* pvData);
    bool HandleReset(bool bBeforeReset);

    D3DDevicePtr m_pkD3DDevice;

    //  Width/Height parameters
    static unsigned int ms_uiDefaultPickW;
    static unsigned int ms_uiDefaultPickH;

    NiCameraPtr m_spCamera;

    //  Viewport rect
    float m_fScaleHorz;
    float m_fScaleVert;
    NiFrustum m_kSaveFrustum;

    //  The target rendering surface...
    D3DFORMAT m_eFormat;
    NiRenderedTexturePtr m_spRenderTexture;
    NiRenderTargetGroupPtr m_spRenderTargetGroup;
    D3DTexturePtr m_pkD3DTexture;
    D3DTexturePtr m_pkD3DReadTexture;

    //  PropertyState for Z buffering
    NiPropertyStatePtr m_spPropertyState;

    //  Shader for setting up the pick rendering
    NiMaterialPtr m_spObjPickMaterial;
    ObjectPickShaderPtr m_spObjPickShader;

    bool m_bFirstObject;
    NiGeometry* m_pkFirstGeom;
    const NiMaterial* m_pkFirstMaterial;

    NiColorA m_kBackgroundColor;
};

typedef NiPointer<ColorPicker> ColorPickerPtr;

#endif  //#ifndef COLORPICKER_H
