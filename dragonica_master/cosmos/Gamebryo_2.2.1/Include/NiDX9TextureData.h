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
#ifndef NIDX9TEXTUREDATA_H
#define NIDX9TEXTUREDATA_H

#include "NiDX9PixelFormat.h"
#include "NiSourceTexture.h"

class NiDX9Renderer;
class NiDX9RenderedTextureData;
class NiDX9SourceTextureData;
class NiDX9DynamicTextureData;

class NIDX9RENDERER_ENTRY NiDX9TextureData : public NiTexture::RendererData
{
public:
    NiDX9TextureData(NiTexture* pkTexture, NiDX9Renderer* pkRenderer);
    virtual ~NiDX9TextureData();

    virtual unsigned int GetLevels() const;

    virtual D3DBaseTexturePtr GetD3DTexture() const;

    virtual NiDX9RenderedTextureData* GetAsRenderedTexture();
    virtual NiDX9SourceTextureData* GetAsSourceTexture();
    virtual NiDX9DynamicTextureData* GetAsDynamicTexture();

    static const NiPixelFormat* FindClosestPixelFormat(
        const NiTexture::FormatPrefs& kPrefs,
        NiPixelFormat* const* ppkDestFmts);

    static void ClearTextureData(NiDX9Renderer* pkRenderer);

protected:
    const NiPixelFormat* FindMatchingPixelFormat(
        const NiPixelFormat& kSrcFmt, const NiTexture::FormatPrefs& kPrefs,
        NiPixelFormat** ppkDestFmts);

    virtual bool InitializeFromD3DTexture(
        D3DBaseTexturePtr pkD3DTexture);

    NiDX9Renderer* m_pkRenderer;

    D3DBaseTexturePtr m_pkD3DTexture;

    unsigned int m_uiLevels;
};

typedef NiPointer<NiDX9TextureData> NiDX9TextureDataPtr;

#include "NiDX9TextureData.inl"

#endif  //#ifndef NiDX9TEXTUREDATA_H
