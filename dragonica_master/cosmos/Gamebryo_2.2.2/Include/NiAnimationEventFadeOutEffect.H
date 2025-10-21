#pragma once

#include "NiAnimationEvent.H"

class	NIANIMATION_ENTRY	NiAnimationEventFadeOutEffect	:	public	NiAnimationEvent
{
	NiDeclareRTTI;

public:

	NiAnimationEventFadeOutEffect()
		:NiAnimationEvent(NULL),
		m_fFadeOutTime(0)
	{
	};

	NiAnimationEventFadeOutEffect(NiTextKey *pkTextKey, const NiFixedString &kEffectName,float fFadeOutTime)
		:NiAnimationEvent(pkTextKey),
		m_kEffectName(kEffectName),
		m_fFadeOutTime(fFadeOutTime)
	{
	}

	virtual	DWORD GetEventType()	const;

	virtual	void	SaveFile(NiFile &kFile);
	virtual	void	LoadFile(NiFile &kFile,unsigned int uiVersion);

	NiFixedString	const&	GetEffectName()	const	{	return	m_kEffectName;	}
	float	GetFadeOutTime()	const	{	return	m_fFadeOutTime;	}

private:

	NiFixedString	m_kEffectName;
	float	m_fFadeOutTime;
};