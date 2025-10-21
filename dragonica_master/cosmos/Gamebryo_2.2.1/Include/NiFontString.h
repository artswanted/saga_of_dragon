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

#ifndef NIFONTSTRING_H
#define NIFONTSTRING_H

#include <NiRTTI.h>
#include <NiRefObject.h>
#include <NiColor.h>

#include "NiFontLibType.h"

NiSmartPointer(NiFont);

class NIFONT_ENTRY NiFontString : public NiRefObject
{
    NiDeclareRootRTTI(NiFontString);

public:
    enum Flags
    {
        COLORED     = 0x00000001,
        CENTERED    = 0x00000002
    };

protected:
    NiFontString(NiFont* pkFont, unsigned int uiFlags,
        unsigned int uiMaxStringLen, const char* pcString,
        const NiColorA& kColor);

public:
    virtual ~NiFontString();

    // Text
    void SetText(const char* pcText, unsigned int uiFlags = 0xffffffff);
    const char* GetText();

    // Color
    void SetColor(const NiColorA& kColor);
    const NiColorA& GetColor() const;

    //*** begin Emergent internal use only
    void SplitString();
    void ShutdownString();
    //*** end Emergent internal use only

protected:
    NiFontPtr m_spFont;
    unsigned int m_uiFlags;
    unsigned int m_uiMaxStringLen;
    unsigned int m_uiStringLen;
    char* m_pcString;
    NiColorA m_kColor;
    
    // *** begin Emergent internal use only *** //
    unsigned int m_uiStringCount;   // Used to handle '\n'
    float* m_pfStringWidth;         // contains width of each line
    float m_fStringWidth;           // contains max width
    float m_fStringHeight;

    bool m_bModified;

    // Be careful with threading when using the working string!
    static char ms_acWorkingString[NI_MAX_PATH];

    // *** end Emergent internal use only *** //
};

typedef NiPointer<NiFontString> NiFontStringPtr;

#include "NiFontString.inl"

#endif //#ifndef NIFONTSTRING_H
