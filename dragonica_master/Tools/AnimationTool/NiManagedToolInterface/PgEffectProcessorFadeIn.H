#pragma once

#include "PgEffectProcessor.H"
#include "PgPSysColorModifierDecorator.H"

namespace NiManagedToolInterface
{
	class	PgEffectProcessorFadeIn	:	public	PgEffectProcessor
	{
		NiDeclareRTTI;
		NiDeclareClone(PgEffectProcessorFadeIn);

	public:

		PgEffectProcessorFadeIn()
			:m_fFadeInTime(0)
			,m_bColorDecoratorAttached(false)
		{
		};
		PgEffectProcessorFadeIn(float fFadeInTime)
			:m_fFadeInTime(fFadeInTime),
			m_bColorDecoratorAttached(false)
		{
		}

		virtual	bool	DoProcess(PgEffect *pkEffect,float fAccumTime,float fFrameTime);
		
		virtual	PgEffectProcessor::EFFECT_PROCESSOR_ID	GetProcessorID()	const	{	return	PgEffectProcessor::EPID_FADE_IN;	}

	private:

		float	GetFadeInTime()	const	{	return	m_fFadeInTime;	}
		void	SetFadeInTime(float fTime)	{	m_fFadeInTime = fTime;	}

		void	AttachColorDecorator(PgEffect *pkEffect);
		
		bool	GetColorDecoratorAttached()	const	{	return	m_bColorDecoratorAttached;	}
		void	SetColorDecoratorAttached(bool bAttached)	{	m_bColorDecoratorAttached = bAttached;	}

		void	SetColorDecorator(PgPSysColorModifierDecorator *pkDecorator)	
		{
			m_spColorDecorator = pkDecorator;
		}
		PgPSysColorModifierDecorator*	GetColorDecorator()	const
		{
			return	m_spColorDecorator;
		}


	private:

		float	m_fFadeInTime;

		PgPSysColorModifierDecoratorPtr	m_spColorDecorator;
		bool	m_bColorDecoratorAttached;

	};

	NiSmartPointer(PgEffectProcessorFadeIn);
}