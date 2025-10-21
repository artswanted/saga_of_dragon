/*****************************************************************

    MODULE    : FxAssetManager.h

    PURPOSE   : Reference implementation for an asset manager.
				
    CREATED   : 10/14/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/
#ifndef FXASSETMANAGER_H
#define FXASSETMANAGER_H

#include "FxStudioReferenceLibType.h"

#include <NiRefObject.h>
#include <NiSmartPointer.h>

NiSmartPointer(NiSourceTexture);
class NiFont;
class NiStream;
class NiObject;

class FXSTUDIOREFERENCE_ENTRY FxAssetManager : public NiRefObject
{
public :

	virtual NiSourceTexturePtr GetSourceTexture(const char* szFileName) = 0;
	virtual NiFont*			 GetFont(const char* szFileName) = 0;

	// Be sure to always clone the objects from this stream.
	virtual NiStream*		 GetStream(const char* szFileName) = 0;

	virtual	NiNodePtr	GetNif(const char* szFileName) = 0;

	// Release the object on the next frame, used by the components to delay release a resource which must last for as long
	// as the next call to the renderer.
	virtual void ReleaseNextFrame(NiObject* pkObject) = 0;
};

NiSmartPointer(FxAssetManager);


#endif // FXASSETMANAGER_H