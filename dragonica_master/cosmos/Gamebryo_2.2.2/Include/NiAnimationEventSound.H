#pragma once

#include "NiAnimationEvent.H"

class	NIANIMATION_ENTRY	NiAnimationEventSound	:	public	NiAnimationEvent
{
	NiDeclareRTTI;

public:

	NiAnimationEventSound()
		:NiAnimationEvent(NULL),
		m_fVolume(0),
		m_fMinDist(0),
		m_fMaxDist(0)
	{
	};
	NiAnimationEventSound(NiTextKey *pkTextKey, const NiFixedString &kSoundName,float fVolume,float fMinDist,float fMaxDist)
		:NiAnimationEvent(pkTextKey),
		m_kSoundName(kSoundName),
		m_fVolume(fVolume),
		m_fMinDist(fMinDist),
		m_fMaxDist(fMaxDist)
	{
	}

	virtual	DWORD GetEventType()	const;

	virtual	void	SaveFile(NiFile &kFile);
	virtual	void	LoadFile(NiFile &kFile,unsigned int uiVersion);

	NiFixedString	const&	GetName()	const	{	return	m_kSoundName;	}
	float	GetVolume()	const	{	return	m_fVolume;	}
	float	GetMinDist()	const	{	return	m_fMinDist;	}
	float	GetMaxDist()	const	{	return	m_fMaxDist;	}

private:

	NiFixedString m_kSoundName;
	float m_fVolume;
	float m_fMinDist;
	float m_fMaxDist;
};