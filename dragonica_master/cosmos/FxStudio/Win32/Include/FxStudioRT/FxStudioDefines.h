/*****************************************************************

    MODULE    : FxStudioDefines.h

    PURPOSE   : Defines used by FxStudio.

    CREATED   : 8/14/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXSTUDIODEFINES_H_
#define FXSTUDIODEFINES_H_

// Client code should add FXSTUDIO_DLL to their project
// if they wish to use the FxStudioRT.dll.  Otherwise
// they should link against FxStudioRT_Static.lib.
#ifdef FXSTUDIO_DLL

	// Client code should never define FXSTUDIO_DLL_EXPORTS
	#ifdef FXSTUDIO_DLL_EXPORTS
		#define FXSTUDIO_API __declspec(dllexport)
	#else
		#define FXSTUDIO_API __declspec(dllimport)
	#endif // FXSTUDIO_DLL_EXPORTS

#else

	#define FXSTUDIO_API

#endif // FXSTUDIO_DLL


// FxStudio has some unions which expose the byte-ordering differences of Intel machines.
#if !defined(FXSTUDIO_LITTLE_ENDIAN) && !defined(FXSTUDIO_BIG_ENDIAN)

	#if defined(WIN32)
		#define FXSTUDIO_LITTLE_ENDIAN
	#else
		#define FXSTUDIO_BIG_ENDIAN
	#endif 

#endif // FXSTUDIO_LITTLE_ENDIAN

#endif // FXSTUDIODEFINES_H_
