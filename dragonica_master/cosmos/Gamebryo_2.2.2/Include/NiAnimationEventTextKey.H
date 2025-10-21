#pragma once

#include "NiAnimationEvent.H"

class	NIANIMATION_ENTRY	NiAnimationEventTextKey	:	public	NiAnimationEvent
{

	NiDeclareRTTI;

public:

	NiAnimationEventTextKey()
		:NiAnimationEvent(NULL)
	{
	};
	NiAnimationEventTextKey(NiTextKey *pkTextKey)
		:NiAnimationEvent(pkTextKey)
	{
	};

	virtual	DWORD GetEventType()	const;

	void	SaveFile(NiFile &kFile);
	void	LoadFile(NiFile &kFile,unsigned int uiVersion);

};