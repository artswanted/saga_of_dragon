#pragma once
#include "PgEffectProcessor.H"

namespace NiManagedToolInterface
{
	__nogc class PgEffect
	{
	public:

		static	std::string EXTRA_DATA_EFFECT_TEXT_KEY;

	public:
		PgEffect();
		PgEffect(NiAVObjectPtr spEffect);
		virtual	~PgEffect();

		NiAVObject *GetEffectRoot();
		void SetEffectRoot(NiAVObject *pkParitlce);

		float GetEndTime();
		void SetEndTime(float fTime);

		float GetBeginTime();
		void SetBeginTime(float fTime);

		bool GetLoop();
		void SetLoop(bool bLoop);

		//! 이펙트를 떼어내야 하는지 체크
		bool Expired(float fAccumTime);

		//! 이펙트를 끝내야 하는 시간을 알아낸다.
		void Analyze(NiObjectNET *pkRoot);

		//! Clone Function
		PgEffect *Clone();

		bool	Update(float fTime);

		void	SetProcessor(PgEffectProcessor *pkProcessor)
		{
			if(!pkProcessor)
			{
				return;
			}
			m_spProcessor[pkProcessor->GetProcessorID()] = pkProcessor;
		}
		void	RemoveProcessor(PgEffectProcessor::EFFECT_PROCESSOR_ID const &kID)
		{
			m_spProcessor[kID] = NULL;
		}

		void	SetFinished(bool bFinished)	{	m_bFinished = bFinished;	}
		bool	GetFinished()	const	{	return	m_bFinished;	}

	private:

		void	UpdateProcessor(float fTime);
		void	CheckFinished(float fTime);

	protected:
		NiAVObjectPtr m_spEffectRoot;
		bool m_bLoop;
		float m_fEndTime;
		float m_fBeginTime;

		PgEffectProcessorPtr	m_spProcessor[PgEffectProcessor::EPID_MAX];

		bool	m_bFinished;
	};
};