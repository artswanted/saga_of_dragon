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

#ifndef NID3DRENDERERHEADERS_H
#define NID3DRENDERERHEADERS_H

#if defined(_XENON)
    #include "NiXenonRendererHeaders.h"
#elif defined(_DX9)
    #include "NiDX9RendererHeaders.h"
#else
    #error _XENON or _DX9 must be specified
#endif

#endif  //#ifndef NID3DRENDERERHEADERS_H
