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
inline unsigned int NiFont::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetCharHeight() const
{
    return m_uiCharHeight;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsBold() const
{
    return (m_uiFlags & BOLD) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsStrikeOut() const
{
    return (m_uiFlags & STRIKEOUT) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsUnderline() const
{
    return (m_uiFlags & UNDERLINE) != 0;
}
//---------------------------------------------------------------------------
inline bool NiFont::IsAntiAliased() const
{
    return (m_uiFlags & ANTIALIASED) != 0;
}
//---------------------------------------------------------------------------
inline int NiFont::GetGlyphCount() const
{
    return m_iGlyphCount;
}
//---------------------------------------------------------------------------
inline const char* NiFont::GetGlyphMap() const
{
    return m_pcGlyphMap;
}
//---------------------------------------------------------------------------
inline NiSourceTexture* NiFont::GetTexture() const
{
    return m_spTexture;
}
//---------------------------------------------------------------------------
inline float NiFont::GetTextureScale() const
{
    return m_fTextureScale;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetTextureWidth() const
{
    return m_uiTextureWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiFont::GetTextureHeight() const
{
    return m_uiTextureHeight;
}
//---------------------------------------------------------------------------
inline NiFont::Error NiFont::GetLastError()
{
    return m_eLastError;
}
//---------------------------------------------------------------------------
inline const NiPixelData* NiFont::GetPixelData() const
{
    if (!m_spTexture)
        return m_spPixelData;

    return m_spTexture->GetSourcePixelData();
}
//---------------------------------------------------------------------------
inline void NiFont::SetPixelData(NiPixelData* pkPixelData)
{
    m_spPixelData = pkPixelData;
}
//---------------------------------------------------------------------------
inline float NiFont::TexCoord(unsigned int i, unsigned int j) const
{
    assert(i < (unsigned int) m_iGlyphCount);
    assert(j < 4);
    return m_pfTextureCoords[4 * i + j];
}
