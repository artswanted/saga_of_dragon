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

#ifndef NSBUTILITY_H
#define NSBUTILITY_H

#include "NiBinaryShaderLibLibType.h"
#include "NSBRenderStates.h"
#include <NiBinaryStream.h>

class NSBStateGroup;
class NSBConstantMap;

#if defined(_DEBUG)
#define STATE_CASE_STRING(x)      case x: return #x;
#endif  //#if defined(_DEBUG)

class NIBINARYSHADERLIB_ENTRY NSBUtility : public NiMemObject
{
public:
    static void SetRenderer(NiD3DRenderer* pkRenderer);
    static NiD3DRenderer* GetRenderer();

    static bool SaveBinaryStateGroup(NiBinaryStream& kStream,
        NSBStateGroup* pkStateGroup);
    static bool LoadBinaryStateGroup(NiBinaryStream& kStream,
        NSBStateGroup*& pkStateGroup);

    static bool SaveBinaryConstantMap(NiBinaryStream& kStream,
        NSBConstantMap* pkMap);
    static bool LoadBinaryConstantMap(NiBinaryStream& kStream,
        NSBConstantMap*& pkMap);

#if defined(_DEBUG)
    static unsigned int ms_uiIndent;

    static void IndentInsert();
    static void IndentRemove();
    static void Dump(FILE* pf, bool bIndent, char* pcFmt, ...);
#endif  //#if defined(_DEBUG)

protected:
    static NiD3DRenderer* ms_pkD3DRenderer;
};

#endif  //NSBUTILITY_H
