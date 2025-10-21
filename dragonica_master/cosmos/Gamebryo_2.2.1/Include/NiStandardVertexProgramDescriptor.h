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

#ifndef NISTANDARDVERTEXPROGRAMDESCRIPTOR_H
#define NISTANDARDVERTEXPROGRAMDESCRIPTOR_H

#include "NiGPUProgramDescriptor.h"
#include "NiBitfield.h"
#include "NiStandardMaterial.h"

class NIMAIN_ENTRY NiStandardVertexProgramDescriptor : 
    public NiGPUProgramDescriptor
{
public:
    NiStandardVertexProgramDescriptor();

    void SetOutputUVUsage(unsigned int uiWhichOutputUV, unsigned int uiInputUV,
        NiStandardMaterial::TexGenOutput eOutputUV);

    void GetOutputUVUsage(unsigned int uiWhichOutputUV, 
        unsigned int& uiInputUV, NiStandardMaterial::TexGenOutput& eOutputUV);

    NiBeginBitfieldEnum()
        
        // First Byte, Index 0
        NiDeclareIndexedBitfieldEntry(TRANSFORM,    2, BITFIELDSTART, 0)
        NiDeclareIndexedBitfieldEntry(OUTPUTWORLDPOS,   1, TRANSFORM, 0)
        NiDeclareIndexedBitfieldEntry(OUTPUTWORLDNBT,   1, OUTPUTWORLDPOS, 0)
        NiDeclareIndexedBitfieldEntry(OUTPUTWORLDVIEW,  1, OUTPUTWORLDNBT, 0)
        NiDeclareIndexedBitfieldEntry(OUTPUTTANGENTVIEW,1, OUTPUTWORLDVIEW, 0)
        NiDeclareIndexedBitfieldEntry(NORMAL,   2, OUTPUTTANGENTVIEW, 0)

        // Second Byte, Index 0
        NiDeclareIndexedBitfieldEntry(SPECULAR, 1, NORMAL, 0)
        NiDeclareIndexedBitfieldEntry(FOGTYPE,  2, SPECULAR, 0)
        NiDeclareIndexedBitfieldEntry(ENVMAPTYPE,   3, FOGTYPE, 0)
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPCOUNT,    2, ENVMAPTYPE, 0)

        // Third Byte, Index 0
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPTYPES,3,PROJLIGHTMAPCOUNT,0)
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,2,PROJLIGHTMAPTYPES,0)
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPTYPES,3,
            PROJSHADOWMAPCOUNT,0)

        // Fourth Byte, Index 0
        NiDeclareIndexedBitfieldEntry(OUTPUTUVCOUNT,4,PROJSHADOWMAPTYPES, 0)
        NiDeclareIndexedBitfieldEntry(UVSET00,3, OUTPUTUVCOUNT, 0)
        NiDeclareLastIndexedBitfieldEntry(UVSET00TEXOUTPUT,1, UVSET00, 0)

        // First Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSET01,  3, BITFIELDSTART, 1)
        NiDeclareIndexedBitfieldEntry(UVSET01TEXOUTPUT, 1, UVSET01, 1)
        NiDeclareIndexedBitfieldEntry(UVSET02,  3, UVSET01TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET02TEXOUTPUT, 1, UVSET02, 1)

        // Second Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSET03,  3, UVSET02TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET03TEXOUTPUT, 1, UVSET03, 1)
        NiDeclareIndexedBitfieldEntry(UVSET04,  3, UVSET03TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET04TEXOUTPUT, 1, UVSET04, 1)

        // Third Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSET05,  3, UVSET04TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET05TEXOUTPUT, 1, UVSET05, 1)
        NiDeclareIndexedBitfieldEntry(UVSET06,  3, UVSET05TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET06TEXOUTPUT, 1, UVSET06, 1)

        // Fourth Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSET07,  3, UVSET06TEXOUTPUT, 1)
        NiDeclareIndexedBitfieldEntry(UVSET07TEXOUTPUT, 1, UVSET07, 1)
        NiDeclareIndexedBitfieldEntry(UVSET08,  3, UVSET07TEXOUTPUT, 1)
        NiDeclareLastIndexedBitfieldEntry(UVSET08TEXOUTPUT,1, UVSET08, 1)

        // First Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSET09,  3, BITFIELDSTART, 2)
        NiDeclareIndexedBitfieldEntry(UVSET09TEXOUTPUT, 1, UVSET09, 2)
        NiDeclareIndexedBitfieldEntry(UVSET10,  3, UVSET09TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(UVSET10TEXOUTPUT, 1, UVSET10, 2)

        // Second Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSET11,  3, UVSET10TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(UVSET11TEXOUTPUT, 1, UVSET11, 2)
        NiDeclareIndexedBitfieldEntry(POINTLIGHTCOUNT,  4, UVSET11TEXOUTPUT, 2)

        // Third Byte, Index 2
        NiDeclareIndexedBitfieldEntry(SPOTLIGHTCOUNT,   4, POINTLIGHTCOUNT, 2)
        NiDeclareIndexedBitfieldEntry(DIRLIGHTCOUNT,    4, SPOTLIGHTCOUNT, 2)

        // Fourth Byte, Index 2
        NiDeclareIndexedBitfieldEntry(VERTEXCOLORS,  1, DIRLIGHTCOUNT, 2)
        NiDeclareIndexedBitfieldEntry(VERTEXLIGHTSONLY, 1, VERTEXCOLORS, 2)
        NiDeclareIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2, VERTEXLIGHTSONLY, 2)
        NiDeclareIndexedBitfieldEntry(LIGHTINGMODE, 2, AMBDIFFEMISSIVE, 2)
        NiDeclareIndexedBitfieldEntry(APPLYMODE, 1, LIGHTINGMODE, 2)

    NiEndBitfieldEnum();       
};
#endif  //#ifndef NISTANDARDVERTEXPROGRAMDESCRIPTOR_H
