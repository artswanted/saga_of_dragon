#pragma once


#include "PgEffectProcessor.H"


namespace NiManagedToolInterface
{

	class	PgEffectProcessorFadeOut	:	public	PgEffectProcessor
	{
		NiDeclareRTTI;
		NiDeclareClone(PgEffectProcessorFadeOut);

	public:

		PgEffectProcessorFadeOut()
			:m_fFadeOutTime(0),m_fFadeStartTime(-1)
		{
		};
		PgEffectProcessorFadeOut(float fFadeOutTime)
			:m_fFadeOutTime(fFadeOutTime),m_fFadeStartTime(-1)
		{
		}

		virtual	bool	DoProcess(PgEffect *pkEffect,float fAccumTime,float fFrameTime);
		
		virtual	PgEffectProcessor::EFFECT_PROCESSOR_ID	GetProcessorID()	const	{	return	PgEffectProcessor::EPID_FADE_OUT;	}

	private:

		float	GetFadeOutTime()	const	{	return	m_fFadeOutTime;	}
		void	SetFadeOutTime(float fTime)	{	m_fFadeOutTime = fTime;	}

		void	UpdateElapsedTime(float fAccumTime)
		{
			if(m_fFadeStartTime == -1)
			{
				m_fFadeStartTime = fAccumTime;
			}

			m_fElapsedTime = fAccumTime - m_fFadeStartTime;
		}

		float	GetElapsedTime()	const	{	return	m_fElapsedTime;	}

	private:

		float	m_fFadeOutTime;
		float	m_fFadeStartTime;
		float	m_fElapsedTime;

	};

	NiSmartPointer(PgEffectProcessorFadeOut);

}