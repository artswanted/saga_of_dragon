/*****************************************************************

    MODULE    : FxStudioReferenceLibType.h

    PURPOSE   : Implements macros to allow for creating and using a 
				DLL version of the integration.
				
    CREATED   : 10/14/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXSTUDIOREFERENCELIBTYPE_H
#define FXSTUDIOREFERENCELIBTYPE_H

#ifdef FXSTUDIOREFERENCE_EXPORT
    // DLL library project uses this
    #define FXSTUDIOREFERENCE_ENTRY __declspec(dllexport)
#else
#ifdef FXSTUDIOREFERENCE_IMPORT
    // client of DLL uses this
    #define FXSTUDIOREFERENCE_ENTRY __declspec(dllimport)
#else
    // static library project uses this
    #define FXSTUDIOREFERENCE_ENTRY
#endif
#endif


#endif // FXSTUDIOREFERENCELIBTYPE_H