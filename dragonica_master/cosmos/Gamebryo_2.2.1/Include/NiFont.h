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

#ifndef NIFONT_H
#define NIFONT_H

#include <NiRTTI.h>
#include <NiRefObject.h>
#include <NiStream.h>
#include <NiColor.h>
#include <NiRenderer.h>
#include <NiPixelData.h>
#include <NiSourceTexture.h>

#include "NiFontLibType.h"

class NIFONT_ENTRY NiFont : public NiRefObject
{
    NiDeclareRootRTTI(NiFont);
    NiDeclareViewerStrings;

protected:
    NiFont(NiRenderer* pkRenderer);

public:
#if defined(WIN32) && !defined(_XENON)
    // Used for creation tool only
    NiFont(NiRenderer* pkRenderer, unsigned int uiHeight, 
        bool bAntiAlias, const char* pcFileName,
        const char* pcFontName, int iGlyphCount, char* pcGlyphMap,
        LOGFONT* pkLogFont);

    bool Save(const char* pcFilePath);
    bool SaveDirect(const char* pcFilename);
#endif //#if defined(WIN32) && !defined(_XENON)

    ~NiFont();

    static NiFont* Create(NiRenderer* pkRenderer, const char* pcFontFile);

    enum Error
    {
        ERR_OK,
        ERR_BADFILE = 0x80000001,
        ERR_INVALIDVERSION,
        ERR_INVALIDFONT,
        ERR_CREATEFAILED,
        ERR_PIXELDATAFAILED
    };

    bool IsBold() const;
    bool IsStrikeOut() const;
    bool IsUnderline() const;
    bool IsAntiAliased() const;

    //  Get Size, etc.
    void GetTextExtent(const char* pcText, float& fWidth, float& fHeight);
    void GetCharExtent(char cChar, float& fWidth, float& fHeight);
    // Get the size up to the first newline
    void GetTextLineExtent(const char* pcText, float& fWidth, float& fHeight);
    // Get the size of the first N chars
    void GetTextRangeExtent(const char* pcText, unsigned int uiNumChars,
        float& fWidth, float& fHeight);
    
    //
    unsigned int GetHeight() const;
    unsigned int GetCharHeight() const;
    NiSourceTexture* GetTexture() const;
    unsigned int GetTextureWidth() const;
    unsigned int GetTextureHeight() const;

    const NiPixelData* GetPixelData() const;
    void SetPixelData(NiPixelData* pkPixelData);

    //  Error
    Error GetLastError();

    //*** begin Emergent internal use only
    float GetTextureScale() const;
    int GetGlyphCount() const;
    const char* GetGlyphMap() const;
    const float* GetCharUVPair(const char chChar);

    //*** end Emergent internal use only

protected:

    enum
    {
        MAGIC_NUMBER    = 0x0046464E
    };

    // Font flags
    enum
    {
        BOLD        = 0x00000001,
        // ITALIC      = 0x00000002, italic fonts not supported
        STRIKEOUT   = 0x00000004,
        UNDERLINE   = 0x00000008,
        ANTIALIASED = 0x00001000
    };

    // streaming support
    NiFont();

    //  Load functions
    bool Load(const char* pacFileName);
    bool LoadFromStream(NiBinaryStream& kStream);
    bool LoadFontHeader(NiBinaryStream& kStream, unsigned int& uiVersion);
    void LoadPixelData(NiBinaryStream& kStream);

#if defined(WIN32)
    //  Creation
    bool CreateFont(LOGFONT* pkLogFont, const char* pcFontName,
        bool bAntiAlias);

    //  Save functions
    void WriteToStream(NiBinaryStream& kStream);
    void WriteFontHeader(NiBinaryStream& kStream);
    void WritePixelData(NiBinaryStream& kStream);
#endif

    void CreateTexture();
    float TexCoord(unsigned int i, unsigned int j) const;

    NiRendererPtr m_spRenderer;

    unsigned int m_uiHeight;
    unsigned int m_uiCharHeight;
    unsigned int m_uiFlags;

    int m_iGlyphCount;
    char* m_pcGlyphMap;

    NiSourceTexturePtr m_spTexture;
    unsigned int m_uiTextureWidth;
    unsigned int m_uiTextureHeight;
    float m_fTextureScale;
    float* m_pfTextureCoords;

    Error m_eLastError;
    
    //  For creation only...
    NiPixelDataPtr m_spPixelData;

    static unsigned int ms_uiCurrentVersion;
};

typedef NiPointer<NiFont> NiFontPtr;

#include "NiFont.inl"

#endif // NIFONT_H
