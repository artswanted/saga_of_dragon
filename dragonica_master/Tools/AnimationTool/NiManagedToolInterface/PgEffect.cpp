#include "stdafx.h"
#include "PgEffect.H"

namespace NiManagedToolInterface
{
	std::string PgEffect::EXTRA_DATA_EFFECT_TEXT_KEY = "EXTRA_DATA_EFFECT_TEXT_KEY";

	PgEffect::PgEffect()
		:m_bFinished(false)
	{
	}

	PgEffect::PgEffect(NiAVObjectPtr spEffect) : 
		m_spEffectRoot(spEffect), 
		m_fBeginTime(0.0f), m_fEndTime(0.0f),
		m_bLoop(false),
		m_bFinished(false)
	{
	}

	PgEffect::~PgEffect()
	{
		m_spEffectRoot = 0;
	}

	NiAVObject *PgEffect::GetEffectRoot()
	{
		return m_spEffectRoot;
	}

	void PgEffect::SetEffectRoot(NiAVObject *pkEffect)
	{
		m_spEffectRoot = pkEffect;
	}

	float PgEffect::GetEndTime()
	{
		return m_fEndTime;
	}

	void PgEffect::SetEndTime(float fTime)
	{
		m_fEndTime = fTime;
	}

	void PgEffect::SetBeginTime(float fTime)
	{
		m_fBeginTime = fTime;
	}

	float PgEffect::GetBeginTime()
	{
		return m_fBeginTime;
	}

	bool PgEffect::GetLoop()
	{
		return m_bLoop;
	}

	void PgEffect::SetLoop(bool bLoop)
	{
		m_bLoop = bLoop;
	}

	bool	PgEffect::Update(float fTime)
	{
		CheckFinished(fTime);

		UpdateProcessor(fTime);

		return	GetFinished()==false;
	}
	void	PgEffect::UpdateProcessor(float fTime)
	{
		GetEffectRoot()->SetColorLocal(NiColorA::WHITE);

		for(int i=0;i<PgEffectProcessor::EPID_MAX;++i)
		{
			PgEffectProcessor	*pkProcessor = m_spProcessor[i];
			if(!pkProcessor)
			{
				continue;
			}

			pkProcessor->DoProcess(this,fTime,0);

			if(pkProcessor->GetFinished())
			{	
				m_spProcessor[i] = NULL;
			}
		}
	}

	void	PgEffect::CheckFinished(float fTime)
	{
		if(GetFinished())
		{
			return;
		}

		SetFinished( (!GetLoop()) && Expired(fTime));
	}
	bool PgEffect::Expired(float fAccumTime)
	{
		return (m_fBeginTime + m_fEndTime) < fAccumTime;
	}

	void PgEffect::Analyze(NiObjectNET *pkRoot)
	{
		if(pkRoot == NULL)
		{
			return;
		}

		// Gather all time controllers from this object
		NiTimeController* pkController = pkRoot->GetControllers();
		while(pkController != NULL)
		{
			float fEndTime = pkController->GetEndKeyTime();
			if(m_fEndTime < fEndTime)
			{
				m_fEndTime = fEndTime;
			}

			pkController = pkController->GetNext();
		}

		if(NiIsKindOf(NiAVObject, pkRoot))
		{
			// NiProperties can have time controllers, so search them too
			NiAVObject* pkObj = (NiAVObject*) pkRoot;
			NiPropertyList* pkPropList = &(pkObj->GetPropertyList());
			NiTListIterator kIter = pkPropList->GetHeadPos();
			while(pkPropList != NULL && !pkPropList->IsEmpty() && kIter)
			{
				NiProperty* pkProperty = pkPropList->GetNext(kIter);
				if(pkProperty)
				{
					Analyze(pkProperty);
				}
			}
		}

		if(NiIsKindOf(NiNode, pkRoot))
		{
			NiNode* pkNode = (NiNode*) pkRoot;
			// Search all of the children
			for(unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
			{
				NiAVObject* pkObj = pkNode->GetAt(ui);
				Analyze(pkObj);
			}
			// NiDynamicEffects can have time controllers, so search them too
			const NiDynamicEffectList* pkEffectList = &(pkNode->GetEffectList());

			NiTListIterator kIter = pkEffectList->GetHeadPos();
			while(pkEffectList != NULL && !pkEffectList->IsEmpty() && kIter)
			{
				NiDynamicEffect* pkEffect = pkEffectList->GetNext(kIter);
				if(pkEffect)
				{
					Analyze(pkEffect);
				}
			}
		}
	}

	PgEffect *PgEffect::Clone()
	{
		NiAVObject *pkNewEffectRoot = (NiAVObject *)m_spEffectRoot->Clone();
		if(!pkNewEffectRoot)
		{
			return 0;
		}

		PgEffect *pkCloned = NiExternalNew PgEffect(pkNewEffectRoot);
		if(!pkCloned)
		{
			return 0;
		}

		pkCloned->SetBeginTime(m_fBeginTime);
		pkCloned->SetEndTime(m_fEndTime);
		pkCloned->SetLoop(m_bLoop);

		return pkCloned;
	}
}