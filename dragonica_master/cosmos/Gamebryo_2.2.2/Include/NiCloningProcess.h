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

#ifndef NICLONINGPROCESS_H
#define NICLONINGPROCESS_H

#include "NiMainLibType.h"
#include "NiObjectNET.h"
#include "NiTPointerMap.h"

class NiObject;

class NIMAIN_ENTRY NiCloningProcess : public NiMemObject
{
public:
    NiCloningProcess(unsigned int uiHashSize = 37)
    {
        m_pkCloneMap = NiNew NiTPointerMap<NiObject*, NiObject*>(uiHashSize);
        assert(m_pkCloneMap != NULL);
        m_pkProcessMap = NiNew NiTPointerMap<NiObject*, bool>(uiHashSize);
        assert(m_pkProcessMap != NULL);
        m_eCopyType = NiObjectNET::GetDefaultCopyType();
        m_cAppendChar = NiObjectNET::GetDefaultAppendCharacter();
    }

    ~NiCloningProcess()
    {
        NiDelete m_pkCloneMap;
        NiDelete m_pkProcessMap;
    }

    NiTPointerMap<NiObject*, NiObject*>* m_pkCloneMap;
    NiTPointerMap<NiObject*, bool>* m_pkProcessMap;
    NiObjectNET::CopyType m_eCopyType;
    char m_cAppendChar;
};

#endif // NICLONINGPROCESS_H
