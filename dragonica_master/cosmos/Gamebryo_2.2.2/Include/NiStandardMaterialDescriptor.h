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

#ifndef NISTANDARDMATERIALDESCRIPTOR_H
#define NISTANDARDMATERIALDESCRIPTOR_H

#include "NiMaterialDescriptor.h"
#include "NiBitfield.h"
#include "NiStandardMaterial.h"

class NIMAIN_ENTRY NiStandardMaterialDescriptor : public NiMaterialDescriptor
{

public:
    NiStandardMaterialDescriptor();    
    
    bool AddLight(NiLight* pkLight);

    bool GetLightType(unsigned int uiWhichLight, 
        NiStandardMaterial::LightType& eLightType);
    bool GetLightInfo(unsigned int uiWhichLight, bool& bShadowed);
    void SetLightInfo(unsigned int uiWhichLight, bool bShadowed);
    
    void SetTextureUsage(unsigned int uiWhichTexture, unsigned int uiInputUV,
        NiStandardMaterial::TexGenOutput eTextureOutput);

    void GetTextureUsage(unsigned int uiWhichTexture, unsigned int& uiInputUV,
        NiStandardMaterial::TexGenOutput& eTextureOutput);

    void SetProjectedLight(unsigned int uiWhichLight, bool bClipped,
        NiStandardMaterial::TexEffectType eType);
    void GetProjectedLight(unsigned int uiWhichLight, bool& bClipped,
        NiStandardMaterial::TexEffectType& eType);

    void SetProjectedShadow(unsigned int uiWhichShadow, bool bClipped,
        NiStandardMaterial::TexEffectType eType);
    void GetProjectedShadow(unsigned int uiWhichShadow, bool& bClipped,
        NiStandardMaterial::TexEffectType& eType);

    
    unsigned int GetStandardTextureCount();

    NiBeginBitfieldEnum()
        
        // First Byte, Index 0
        NiDeclareIndexedBitfieldEntry(TRANSFORM,       2, BITFIELDSTART,    0)
        NiDeclareIndexedBitfieldEntry(NORMAL,          2, TRANSFORM,        0)
        NiDeclareIndexedBitfieldEntry(SPECULAR,        1, NORMAL,           0)
        NiDeclareIndexedBitfieldEntry(VERTEXCOLORS,    1, SPECULAR,         0)
        NiDeclareIndexedBitfieldEntry(AMBDIFFEMISSIVE, 2, VERTEXCOLORS,     0)

        // Second Byte, Index 0
        NiDeclareIndexedBitfieldEntry(LIGHTINGMODE,    1, AMBDIFFEMISSIVE,  0)
        NiDeclareIndexedBitfieldEntry(APPLYMODE,       1, LIGHTINGMODE,     0)
        NiDeclareIndexedBitfieldEntry(FOGTYPE,         2, APPLYMODE,        0)
        NiDeclareIndexedBitfieldEntry(INPUTUVCOUNT,    3, FOGTYPE,          0)
        NiDeclareIndexedBitfieldEntry(PARALLAXMAPCOUNT,1, INPUTUVCOUNT,     0)

        //Third Byte, Index 0
        NiDeclareIndexedBitfieldEntry(BASEMAPCOUNT,    1, PARALLAXMAPCOUNT, 0)
        NiDeclareIndexedBitfieldEntry(NORMALMAPCOUNT,  1, BASEMAPCOUNT,     0)
        NiDeclareIndexedBitfieldEntry(NORMALMAPTYPE,   2, NORMALMAPCOUNT,   0)
        NiDeclareIndexedBitfieldEntry(DARKMAPCOUNT,    1, NORMALMAPTYPE,    0)
        NiDeclareIndexedBitfieldEntry(DETAILMAPCOUNT,  1, DARKMAPCOUNT,     0)
        NiDeclareIndexedBitfieldEntry(BUMPMAPCOUNT,    1, DETAILMAPCOUNT,   0)
        NiDeclareIndexedBitfieldEntry(GLOSSMAPCOUNT,   1, BUMPMAPCOUNT,     0)
                                                          
        //Fourth Byte, Index 0
        NiDeclareIndexedBitfieldEntry(GLOWMAPCOUNT,    1, GLOSSMAPCOUNT,    0)
        NiDeclareIndexedBitfieldEntry(ENVMAPTYPE,      3, GLOWMAPCOUNT,     0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP00COUNT,1, ENVMAPTYPE,       0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP01COUNT,1, CUSTOMMAP00COUNT, 0)
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP02COUNT,1, CUSTOMMAP01COUNT, 0)
        NiDeclareLastIndexedBitfieldEntry(CUSTOMMAP03COUNT,1, CUSTOMMAP02COUNT,
            0)

        // First Byte, Index 1
        NiDeclareIndexedBitfieldEntry(CUSTOMMAP04COUNT,  1, BITFIELDSTART, 1)
        NiDeclareIndexedBitfieldEntry(DECALMAPCOUNT,     2, CUSTOMMAP04COUNT,1)
        NiDeclareIndexedBitfieldEntry(PERVERTEXFORLIGHTS,1, DECALMAPCOUNT,  1)
        NiDeclareIndexedBitfieldEntry(POINTLIGHTCOUNT,   4, PERVERTEXFORLIGHTS,
            1)
                                                               
        // Second Byte, Index 1
        NiDeclareIndexedBitfieldEntry(SPOTLIGHTCOUNT,  4, POINTLIGHTCOUNT, 1)
        NiDeclareIndexedBitfieldEntry(DIRLIGHTCOUNT,   4, SPOTLIGHTCOUNT,  1)

        // Third Byte, Index 1
        NiDeclareIndexedBitfieldEntry(SHADOWMAPFORLIGHT,8, DIRLIGHTCOUNT,  1)

        // Fourth Byte, Index 1
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPCOUNT,   2, 
            SHADOWMAPFORLIGHT,  1)
        NiDeclareIndexedBitfieldEntry(PROJLIGHTMAPTYPES,   3, 
            PROJLIGHTMAPCOUNT,  1)
        NiDeclareLastIndexedBitfieldEntry(PROJLIGHTMAPCLIPPED, 3, 
            PROJLIGHTMAPTYPES,  1)

        // First Byte, Index 2
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPCOUNT,   2, 
            BITFIELDSTART,  2)
        NiDeclareIndexedBitfieldEntry(PROJSHADOWMAPTYPES,   3, 
            PROJSHADOWMAPCOUNT,  2)
        NiDeclareLastIndexedBitfieldEntry(PROJSHADOWMAPCLIPPED, 3, 
            PROJSHADOWMAPTYPES,  2)

        // Second Byte, Index 2
        NiDeclareIndexedBitfieldEntry(MAP00,    3,  PROJSHADOWMAPCLIPPED, 2)
        NiDeclareIndexedBitfieldEntry(MAP00TEXOUTPUT, 1,    MAP00, 2)
        NiDeclareIndexedBitfieldEntry(MAP01,    3, MAP00TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(MAP01TEXOUTPUT, 1,    MAP01, 2)
        
        // Third Byte, Index 2
        NiDeclareIndexedBitfieldEntry(MAP02,    3,  MAP01TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(MAP02TEXOUTPUT, 1,    MAP02, 2)
        NiDeclareIndexedBitfieldEntry(MAP03,    3, MAP02TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(MAP03TEXOUTPUT, 1,    MAP03, 2)

        // Fourth Byte, Index 2
        NiDeclareIndexedBitfieldEntry(MAP04,    3,  MAP03TEXOUTPUT, 2)
        NiDeclareIndexedBitfieldEntry(MAP04TEXOUTPUT, 1,    MAP04, 2)
        NiDeclareIndexedBitfieldEntry(MAP05,    3, MAP04TEXOUTPUT, 2)
        NiDeclareLastIndexedBitfieldEntry(MAP05TEXOUTPUT, 1,    MAP05, 2)   
            
        // First Byte, Index 3
        NiDeclareIndexedBitfieldEntry(MAP06,    3,  BITFIELDSTART, 3)
        NiDeclareIndexedBitfieldEntry(MAP06TEXOUTPUT, 1,    MAP06, 3)
        NiDeclareIndexedBitfieldEntry(MAP07,    3, MAP06TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(MAP07TEXOUTPUT, 1,    MAP07, 3)   

        // Second Byte, Index 3
        NiDeclareIndexedBitfieldEntry(MAP08,    3,  MAP07TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(MAP08TEXOUTPUT, 1,    MAP08, 3)
        NiDeclareIndexedBitfieldEntry(MAP09,    3, MAP08TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(MAP09TEXOUTPUT, 1,    MAP09, 3) 
        
        // Third Byte, Index 3
        NiDeclareIndexedBitfieldEntry(MAP10,    3,  MAP09TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(MAP10TEXOUTPUT, 1,    MAP10, 3)
        NiDeclareIndexedBitfieldEntry(MAP11,    3, MAP10TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(MAP11TEXOUTPUT, 1,    MAP11, 3) 

        // Fourth Byte, Index 3
        NiDeclareIndexedBitfieldEntry(USERDEFINED00,    1, MAP11TEXOUTPUT, 3)
        NiDeclareIndexedBitfieldEntry(USERDEFINED01,    1, USERDEFINED00, 3)
        NiDeclareIndexedBitfieldEntry(USERDEFINED02,    1, USERDEFINED01, 3)
        NiDeclareIndexedBitfieldEntry(USERDEFINED03,    1, USERDEFINED02, 3)
        // Leave 4 bits for future.

    NiEndBitfieldEnum();   
};

#endif  //#ifndef NISTANDARDMATERIALDESCRIPTOR_H
