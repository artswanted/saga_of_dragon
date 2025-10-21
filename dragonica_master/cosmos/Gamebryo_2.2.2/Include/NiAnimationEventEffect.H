#pragma once

#include "NiAnimationEvent.H"

class	NIANIMATION_ENTRY	NiAnimationEventEffect	:	public	NiAnimationEvent
{
	NiDeclareRTTI;

public:

	NiAnimationEventEffect()
		:NiAnimationEvent(NULL),
		m_fScale(1),
		m_fFadeInTime(0),
		m_bUseFadeIn(false),
		m_fFadeOutTimeWhenSequenceChanged(0),
		m_bUseFadeOutWhenSequenceChanged(false),
		m_bAttachToPos(false)
	{
	};

	NiAnimationEventEffect(NiTextKey *pkTextKey, const NiFixedString &kEffectName,
		const NiFixedString &kAttachPoint, float fScale,float fFadeInTime,bool bUseFadeIn,
		float fFadeOutTimeWhenSequenceChanged,bool bUseFadeOutWhenSequenceChanged)
		:NiAnimationEvent(pkTextKey),
		m_kEffectName(kEffectName),
		m_kAttachPoint(kAttachPoint),
		m_fScale(fScale),
		m_fFadeInTime(fFadeInTime),
		m_bUseFadeIn(bUseFadeIn),
		m_fFadeOutTimeWhenSequenceChanged(fFadeOutTimeWhenSequenceChanged),
		m_bUseFadeOutWhenSequenceChanged(bUseFadeOutWhenSequenceChanged),
		m_bAttachToPos(false)
	{
	}

	virtual	DWORD GetEventType()	const;

	virtual	void	SaveFile(NiFile &kFile);
	virtual	void	LoadFile(NiFile &kFile,unsigned int uiVersion);

	NiFixedString	const&	GetEffectName()	const	{	return	m_kEffectName;	}
	NiFixedString	const&	GetAttachPointName()	const	{	return	m_kAttachPoint;	}
	float	GetScale()	const	{	return	m_fScale;	}
	float	GetFadeInTime()	const	{	return	m_fFadeInTime;	}
	bool	GetUseFadeIn()	const	{	return	m_bUseFadeIn;	}

	float	GetFadeOutTimeWhenSequenceChanged()	const	{	return	m_fFadeOutTimeWhenSequenceChanged;	}
	bool	GetUseFadeOutWhenSequenceChanged()	const	{	return	m_bUseFadeOutWhenSequenceChanged;	}
	bool	GetAttachToPos() const;
private:


	NiFixedString	m_kEffectName;
	NiFixedString	m_kAttachPoint;
	float	m_fScale;
	float	m_fFadeInTime;
	bool	m_bUseFadeIn;
	float	m_fFadeOutTimeWhenSequenceChanged;
	bool	m_bUseFadeOutWhenSequenceChanged;
	//Attach용도가 아니다! Attach가 노드가 아니라 노드의 해당 월드좌표에 이펙트를 붙이기 위해서 사용한다!
	bool	m_bAttachToPos;
	void	SetAttachToPos(bool bValue);
};