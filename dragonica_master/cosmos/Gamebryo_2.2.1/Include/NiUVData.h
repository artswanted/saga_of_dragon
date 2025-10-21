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

#ifndef NIUVDATA_H
#define NIUVDATA_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiFloatKey.h"
#include "NiStream.h"

NiSmartPointer(NiUVData);

// This class is deprecated. Its functionality has been replaced by
// NiTextureTransformController.

class NIANIMATION_ENTRY NiUVData : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiUVData();
    virtual ~NiUVData();

    // Offset key access
    unsigned int GetNumUOffsetKeys() const;
    NiFloatKey* GetUOffsetAnim(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;
    unsigned int GetNumVOffsetKeys() const;
    NiFloatKey* GetVOffsetAnim(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;
    // For swapping Offset animation sequences at runtime. The 

    //   object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceUOffsetAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);
    void ReplaceVOffsetAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);

    // For swapping Offset animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetUOffsetAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);
    void SetVOffsetAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);

    // Tiling key access
    unsigned int GetNumUTilingKeys() const;
    NiFloatKey* GetUTilingAnim(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;
    unsigned int GetNumVTilingKeys() const;
    NiFloatKey* GetVTilingAnim(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping Tiling animation sequences at runtime. The 
    //   object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceUTilingAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);
    void ReplaceVTilingAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);

    // For swapping Tiling animation sequences at runtime.  The method
    // neither deletes the old data nor makes internal copies of the new
    // data. The application is responsible for the deletion.
    void SetUTilingAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);
    void SetVTilingAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType);

    // *** begin Emergent internal use only ***

    float GetLastUOffset() const;
    float GetLastVOffset() const;
    float GetLastUTiling() const;
    float GetLastVTiling() const;
    void SetLastUVOffsetTiling(float fUOffset, float fVOffset, float fUTiling,
        float fVTiling);

    void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);

    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiNumUOffsetKeys;
    NiFloatKey* m_pkUOffsetKeys;
    NiAnimationKey::KeyType m_eUOffsetType;
    unsigned int m_uiNumVOffsetKeys;
    NiFloatKey* m_pkVOffsetKeys;
    NiAnimationKey::KeyType m_eVOffsetType;

    unsigned int m_uiNumUTilingKeys;
    NiFloatKey* m_pkUTilingKeys;
    NiAnimationKey::KeyType m_eUTilingType;
    unsigned int m_uiNumVTilingKeys;
    NiFloatKey* m_pkVTilingKeys;
    NiAnimationKey::KeyType m_eVTilingType;

    float m_fLastUOffset;
    float m_fLastVOffset;
    float m_fLastUTiling;
    float m_fLastVTiling;

    unsigned char m_ucUOffsetSize;
    unsigned char m_ucVOffsetSize;
    unsigned char m_ucUTilingSize;
    unsigned char m_ucVTilingSize;

};

#include "NiUVData.inl"

#endif // NIUVDATA_H
