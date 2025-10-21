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

#ifndef NISTANDARDPIXELPROGRAMDESCRIPTOR_H
#define NISTANDARDPIXELPROGRAMDESCRIPTOR_H

#include "NiLight.h"
#include "NiGPUProgramDescriptor.h"
#include "NiBitfield.h"

class NIMAIN_ENTRY NiStandardPixelProgramDescriptor : 
    public NiGPUProgramDescriptor
{
public:
    NiStandardPixelProgramDescriptor();    

    unsigned int GetUVSetForMap(unsigned int uiWhichMap);
    void SetUVSetForMap(unsigned int uiWhichMap, unsigned int uiUVSet);

    bool AddLight(NiLight* pkLight);
    unsigned int GetStandardTextureCount();
    unsigned int GetInputUVCount();

    NiBeginBitfieldEnum()
        
        // First Byte, Index 0
        NiDeclareIndexedBitfieldEntry(APPLYMODE,    1, BITFIELDSTART, 0)
        NiDeclareIndexedBitfieldEntry(WORLDPOSITION,   1, APPLYMODE, 0)
        NiDeclareIndexedBitfieldEntry(WORLDNORMAL,   1, WORLDPOSITION, 0)
        NiDeclareIndexedBitfieldEntry(WORLDNBT,  1, WORLDNORMAL, 0)
        NiDeclareIndexedBitfieldEntry(WORLDVIEW, 1, WORLDNBT, 0)
        NiDeclareIndexedBitfieldEntry(NORMALMAPTYPE,   2, WORLDVIEW, 0)
        NiDeclareIndexedBitfieldEntry(PARALLAXMAPCOUNT,1, NORMALMAPTYPE, 0)

        // Second Byte, Index 0
        NiDeclareIndexedBitfieldEntry(BASEMAPCOUNT,     1, PARALLAXMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(NORMALMAPCOUNT,   1, BASEMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(DARKMAPCOUNT,     1, NORMALMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(DETAILMAPCOUNT,   1, DARKMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(BUMPMAPCOUNT,     1, DETAILMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(GLOSSMAPCOUNT,    1, BUMPMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(GLOWMAPCOUNT,     1, GLOSSMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP00COUNT, 1, GLOWMAPCOUNT, 0)

        // Third Byte, Index 0
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP01COUNT, 1, CUSTOMMAP00COUNT, 0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP02COUNT, 1, CUSTOMMAP01COUNT, 0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP03COUNT, 1, CUSTOMMAP02COUNT, 0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP04COUNT, 1, CUSTOMMAP03COUNT, 0)
        NiDeclareIndexedBitfieldEntry(DECALMAPCOUNT,    3, CUSTOMMAP04COUNT, 0)
        NiDeclareIndexedBitfieldEntry(FOGENABLED,       1, DECALMAPCOUNT, 0)
                                                               
        // Fourth Byte, Index 0
        NiDeclareIndexedBitfieldEntry(ENVMAPTYPE,   3,  FOGENABLED, 0)
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPCOUNT, 2, ENVMAPTYPE, 0)
        NiDeclareLastIndexedBitfieldEntry(PROJLIGHTMAPTYPES, 3, 
            PROJLIGHTMAPCOUNT, 0)

        // First Byte, Index 1
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPCLIPPED, 3, BITFIELDSTART, 1)
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,  2, 
            PROJLIGHTMAPCLIPPED, 1)
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPTYPES,  3, 
            PROJSHADOWMAPCOUNT, 1)

        // Second Byte, Index 1
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPCLIPPED, 3, 
            PROJSHADOWMAPTYPES, 1)
        NiDeclareIndexedBitfieldEntry(PERVERTEXLIGHTING, 1, 
            PROJSHADOWMAPCLIPPED, 1)
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP00,  4, 
            PERVERTEXLIGHTING, 1)

        // Third Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP01, 4, UVSETFORMAP00, 1)
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP02, 4, UVSETFORMAP01, 1)

        // Fourth Byte, Index 1
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP03, 4, UVSETFORMAP02, 1)
        NiDeclareLastIndexedBitfieldEntry(UVSETFORMAP04, 4, UVSETFORMAP03, 1)

        // First Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP05, 4, BITFIELDSTART, 2)
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP06, 4, UVSETFORMAP05, 2)

        // Second Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP07, 4, UVSETFORMAP06, 2)
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP08, 4, UVSETFORMAP07, 2)

        // Third Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP09, 4, UVSETFORMAP08, 2)
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP10, 4, UVSETFORMAP09, 2)

        // Fourth Byte, Index 2
        NiDeclareIndexedBitfieldEntry(UVSETFORMAP11, 4, UVSETFORMAP10, 2)
        NiDeclareLastIndexedBitfieldEntry(POINTLIGHTCOUNT, 4, UVSETFORMAP11, 2)

        // First Byte, Index 3
        NiDeclareIndexedBitfieldEntry(SPOTLIGHTCOUNT, 4, BITFIELDSTART, 3)
        NiDeclareIndexedBitfieldEntry(DIRLIGHTCOUNT, 4, SPOTLIGHTCOUNT, 3)

        // Second Byte, Index 3
        NiDeclareIndexedBitfieldEntry(SHADOWMAPFORLIGHT, 8, DIRLIGHTCOUNT, 3)

        // Third Byte, Index 3
        NiDeclareIndexedBitfieldEntry(SPECULAR, 1, SHADOWMAPFORLIGHT, 3)
        NiDeclareIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2, SPECULAR, 3)
        NiDeclareIndexedBitfieldEntry(LIGHTINGMODE, 2, AMBDIFFEMISSIVE, 3)
        NiDeclareIndexedBitfieldEntry(APPLYAMBIENT, 1, LIGHTINGMODE, 3)
        NiDeclareIndexedBitfieldEntry(BASEMAPALPHAONLY, 1, APPLYAMBIENT, 3)
        NiDeclareIndexedBitfieldEntry(APPLYEMISSIVE, 1, BASEMAPALPHAONLY, 3)

    NiEndBitfieldEnum(); 
};

#endif  //#ifndef NISTANDARDPIXELPROGRAMDESCRIPTOR_H
