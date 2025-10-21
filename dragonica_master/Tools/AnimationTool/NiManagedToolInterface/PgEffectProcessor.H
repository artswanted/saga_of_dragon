#pragma once
#include <NiMain.H>

namespace NiManagedToolInterface
{
	class	PgEffect;
	__nogc class	PgEffectProcessor	:	public	NiObject
	{

		NiDeclareRTTI;

	public:

		enum	EFFECT_PROCESSOR_ID
		{
			EPID_NONE=-1,
			EPID_FADE_IN,
			EPID_FADE_OUT,
			EPID_MAX,
		};

	public:

		PgEffectProcessor()
			:m_bFinished(false)
		{
		}
		virtual	~PgEffectProcessor()	{};

		virtual	bool	DoProcess(PgEffect *pkEffect,float fAccumTime,float fFrameTime) = 0;
		virtual	EFFECT_PROCESSOR_ID	GetProcessorID()	const	{	return	EPID_NONE;	}

		void	SetFinished(bool bFinished)	{	m_bFinished = bFinished;	}
		bool	GetFinished()	const	{	return	m_bFinished;	}

	protected:

		void CopyMembers(PgEffectProcessor* pDest, NiCloningProcess& kCloning);

	private:

		bool	m_bFinished;

	};
	NiSmartPointer(PgEffectProcessor);
}