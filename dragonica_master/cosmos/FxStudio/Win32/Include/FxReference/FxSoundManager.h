/*****************************************************************

    MODULE    : FxSoundManager.h

    PURPOSE   : Reference implementation for the sound manager.
				
    CREATED   : 10/14/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXSOUNDMANAGER_H
#define FXSOUNDMANAGER_H

#include "FxStudioReferenceLibType.h"

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiPoint3.h>
#include <NiPoint2.h>


class FXSTUDIOREFERENCE_ENTRY FxSoundManager : public NiRefObject
{
public :

	virtual ~FxSoundManager() { }

	// Plays a sound
	// szSound = sound friendly name
	// pos = position to start sound
	// vel = intitial velocity of the sound (optional)
	// falloff = array of 3 vectors, x is distance and y is volume (0 - 1)
	// Returns the pointer to SoundData containing the sound (may be NULL if sound failed to play)
	virtual void* PlayGameSound(const char* szSound, const NiPoint3& pos, bool bLoop, NiPoint2* pFallOff, const NiPoint3& vel = NiPoint3(0,0,0)) = 0;

	// Stops a sound.
	virtual void StopGameSound(void* hSound) = 0;

	// Pauses a sound.
	virtual void PauseGameSound(void* hSound, bool bPaused) = 0;
};

NiSmartPointer(FxSoundManager);


#endif // FXSOUNDMANAGER_H
