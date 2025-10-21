#include "stdafx.h"
#include "PgEffectProcessorFadeIn.H"
#include "PgEffect.H"

namespace NiManagedToolInterface
{
	NiImplementRTTI(PgEffectProcessorFadeIn, PgEffectProcessor);

	bool	PgEffectProcessorFadeIn::DoProcess(PgEffect *pkEffect,float fAccumTime,float fFrameTime)
	{
		if(!pkEffect)
		{
			return	false;
		}

		float	fAlpha = 1;
		
		if(GetFadeInTime()>0)
		{
			float	fElapsedTime = fAccumTime - pkEffect->GetBeginTime();

			fAlpha = fElapsedTime/GetFadeInTime();
			if(fAlpha>1)
			{
				fAlpha = 1;
			}
		}

		NiColorA	kColor = pkEffect->GetEffectRoot()->GetColorLocal();
		kColor.a = kColor.a * fAlpha;

		pkEffect->GetEffectRoot()->SetColorLocal(kColor);

		if(fAlpha==1)
		{
			SetFinished(true);
		}

		return	false;

	}
	NiImplementCreateClone(PgEffectProcessorFadeIn);

	void PgEffectProcessorFadeIn::CopyMembers(PgEffectProcessorFadeIn* pDest, NiCloningProcess& kCloning)
	{
		PgEffectProcessor::CopyMembers(pDest,kCloning);

		pDest->SetFadeInTime(GetFadeInTime());
	}

	void	PgEffectProcessorFadeIn::AttachColorDecorator(PgEffect *pkEffect)
	{
		if(GetColorDecoratorAttached())
		{
			return;
		}

		SetColorDecoratorAttached(true);

		NiAVObject	*pkAVObject = pkEffect->GetEffectRoot();
		if(!pkAVObject)
		{
			return;
		}



	}

}