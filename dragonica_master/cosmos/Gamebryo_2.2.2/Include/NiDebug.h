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

#ifndef NIDEBUG_H
#define NIDEBUG_H

#if defined(_PS3)
    #define NIDEBUGBREAKPROC() __asm__ volatile("tw 31,1,1")
#elif defined(WIN32) || defined(_XENON)
    #define NIDEBUGBREAKPROC() __debugbreak()
#else
    #error Platform undefined.
#endif // #ifdef _PS3

#if defined(_DEBUG)
    #define NIASSERT(value) {if (0 == (value)) NIDEBUGBREAK()}
    #define NIVERIFY(condition) NIASSERT(condition)
    #define NIDEBUGBREAK() {NIDEBUGBREAKPROC();}
#else
    #define NIASSERT(value)  
    #define NIVERIFY(condition) (condition)
    #define NIDEBUGBREAK() {}
#endif

#endif // NIDEBUG_H
