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
//---------------------------------------------------------------------------
//  NiDX9TextureData Inline Functions
//---------------------------------------------------------------------------
inline NiDX9RenderedTextureData* NiDX9TextureData::GetAsRenderedTexture()
{
    return NULL;
}
//---------------------------------------------------------------------------
inline NiDX9SourceTextureData* NiDX9TextureData::GetAsSourceTexture()
{
    return NULL;
}
//---------------------------------------------------------------------------
inline NiDX9DynamicTextureData* NiDX9TextureData::GetAsDynamicTexture()
{
    return NULL;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9TextureData::GetLevels() const
{
    return m_uiLevels;
}
//---------------------------------------------------------------------------
inline D3DBaseTexturePtr NiDX9TextureData::GetD3DTexture() const
{
    return m_pkD3DTexture;
}
//---------------------------------------------------------------------------
