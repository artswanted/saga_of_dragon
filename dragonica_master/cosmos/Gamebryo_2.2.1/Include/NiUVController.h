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

#ifndef NIUVCONTROLLER_H
#define NIUVCONTROLLER_H

#include <NiGeometry.h>
#include <NiTimeController.h>
#include "NiAnimationLibType.h"
#include "NiUVData.h"

NiSmartPointer(NiUVController);

// This class is deprecated. Its functionality has been replaced by
// NiTextureTransformController.

class NIANIMATION_ENTRY NiUVController : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareClone(NiUVController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiUVController(NiUVData* pkData = 0);
    virtual ~NiUVController();

    // updates
    virtual void Update(float fTime);

    // shared data access
    void SetUVData(NiUVData* pkData);
    NiUVData* GetUVData();

    // Offset access
    unsigned int GetNumUOffsetKeys() const;
    NiFloatKey* GetUOffsetData(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;
    unsigned int GetNumVOffsetKeys() const;
    NiFloatKey* GetVOffsetData(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping Offset animation sequences at runtime. The 
    //   NiUVData object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceUOffsetData(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType, bool bUpdate = true);
    void ReplaceVOffsetData(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType, bool bVpdate = true);

    // Tiling access
    unsigned int GetNumUTilingKeys() const;
    NiFloatKey* GetUTilingData(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;
    unsigned int GetNumVTilingKeys() const;
    NiFloatKey* GetVTilingData(unsigned int& uiNumKeys,
        NiAnimationKey::KeyType& eType, unsigned char& ucSize) const;

    // For swapping Tiling animation sequences at runtime. The 
    //   NiUVData object assumes ownership of the data array being 
    //   passed in, and has the responsibility for deleting it when finished 
    //   with it.
    void ReplaceUTilingData(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType, bool bUpdate = true);
    void ReplaceVTilingData(NiFloatKey* pkKeys, unsigned int uiNumKeys,
        NiAnimationKey::KeyType eType, bool bVpdate = true);

    unsigned short GetTextureSet() const;
    void SetTextureSet(unsigned short usTextureSet);

    // Override default
    virtual void SetTarget(NiObjectNET* pkTarget);

    // *** begin Emergent internal use only ***
    virtual void OnPreDisplay();
    virtual bool IsVertexController() const;
    // *** end Emergent internal use only ***

protected:
    bool IsVisible(float fTime);

    // Reset the UV extrema after a call to any of the Replace methods.
    void ResetKeyTimeExtrema();

    unsigned int m_uiLastUOffsetIdx;
    unsigned int m_uiLastUTilingIdx;
    unsigned int m_uiLastVOffsetIdx;
    unsigned int m_uiLastVTilingIdx;

    unsigned short m_usTextureSet;

    NiUVDataPtr m_spUVData;
    bool m_bNeedPreDisplay;

    // debugging
    virtual bool TargetIsRequiredType() const;
};

#include "NiUVController.inl"

#endif // NIUVCONTROLLER_H

