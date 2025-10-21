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

#ifndef NIEXTRADATA_H
#define NIEXTRADATA_H

#include "NiObject.h"
#include "NiStream.h"
#include "NiFixedString.h"
NiSmartPointer(NiExtraData);


class NIMAIN_ENTRY NiExtraData : public NiObject
{
    NiDeclareRTTI;
    //NiDeclareClone(NiExtraData);
    NiDeclareAbstractClone(NiExtraData);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // construction and destruction
    NiExtraData();  // support for streaming
    NiExtraData(const NiFixedString& kName);
    virtual ~NiExtraData ();

    // streaming (the char* pointer is copied, not the data)
    ////void SetSizeAndChunk (unsigned int uiSize, char* pChunk);
    ////unsigned int GetSize () const;
    ////char* GetChunk () const;

    const NiFixedString& GetName() const;
    void SetName(const NiFixedString& kName);   // Used for cloning.

    // The following members dictate whether or not the extra data should be
    // streamed or cloned which can be useful in the case of extra data that
    // holds temporary values during execution.
    virtual bool IsStreamable() const;
    virtual bool IsCloneable() const;

    // *** begin Emergent internal use only ***

    // The following INTERNAL function is used specifically in streaming Nif
    //    files prior to Nif version 5.0.0.11.  In that case, the m_pcName
    //    member variable initially holds a "next" pointer for the old style
    //    ExtraData linked list.  Thus, it's important NOT to delete the name,
    //    which is really a pointer to the "next" ExtraData instance.  Note
    //    that SetName() and SetNameNoDelete() differ by only the single line:
    //    "delete m_pcName;".
    void ClearName();

    // *** end Emergent internal use only ***

protected:
    // data is an array of bytes
    ////unsigned int m_uiSize;
    ////char* m_pChunk;

    // Every extra data instance has a unique name; 
    // if one is not provided, a unique name will be generated.
    // For legacy Nif files prior to Nif version 5.0.0.11,
    //    this pointer is cast as (NiExtraData*) during
    //    streaming and used for the legacy NiExtraData linked
    //    list, since there is no name.  NiObjectNET's
    //    post-link step converts the legacy linked list to an
    //    array of NiExtraData pointers, and then generates
    //    unique names.
    NiFixedString m_kName; 
    
};

NiSmartPointer(NiExtraData);

#endif

