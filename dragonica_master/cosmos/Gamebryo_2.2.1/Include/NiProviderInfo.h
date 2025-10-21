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

#ifndef NIPROVIDERINFO_H
#define NIPROVIDERINFO_H

#include "NiAudioSystem.h"

class NIAUDIO_ENTRY NiProviderInfo : public NiMemObject
{
public:
    // *** begin Emergent internal use only ***
   
    NiProviderInfo(const char* cName, const unsigned int uiNumber);
    ~NiProviderInfo();
    
    const char* GetProviderName();
    const unsigned int GetProviderNumber();
    const bool GetIsOpen();
    const unsigned int GetNumberActiveSamples();
    bool OpenProvider();
    bool CloseProvider();

    // *** end Emergent internal use only ***

protected:
    char* m_cProviderString;
    unsigned int m_uiProviderNumber;
    bool m_bIsOpen;
};

//---------------------------------------------------------------------------
inline const char* NiProviderInfo::GetProviderName() 
{ 
    return m_cProviderString; 
}
//---------------------------------------------------------------------------
inline const unsigned int NiProviderInfo::GetProviderNumber() 
{ 
    return m_uiProviderNumber; 
}
//---------------------------------------------------------------------------
inline const bool NiProviderInfo::GetIsOpen() 
{ 
    return m_bIsOpen; 
}
//---------------------------------------------------------------------------

#endif
