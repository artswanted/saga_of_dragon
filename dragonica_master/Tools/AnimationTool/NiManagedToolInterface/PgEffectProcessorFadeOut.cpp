#include "stdafx.h"
#include "PgEffectProcessorFadeOut.H"
#include "PgEffect.H"

namespace NiManagedToolInterface
{
	NiImplementRTTI(PgEffectProcessorFadeOut, PgEffectProcessor);

	bool	PgEffectProcessorFadeOut::DoProcess(PgEffect *pkEffect,float fAccumTime,float fFrameTime)
	{
		if(!pkEffect)
		{
			return	false;
		}

		UpdateElapsedTime(fAccumTime);

		float	fAlpha = 0;
		
		if(GetFadeOutTime()>0)
		{
			fAlpha = GetElapsedTime()/GetFadeOutTime();
			if(fAlpha>1)
			{
				fAlpha = 1;
			}

			fAlpha = 1-fAlpha;
		}

		NiColorA	kColor = pkEffect->GetEffectRoot()->GetColorLocal();
		kColor.a = kColor.a * fAlpha;

		pkEffect->GetEffectRoot()->SetColorLocal(pkEffect->GetEffectRoot()->GetColorLocal() * fAlpha);

		if(fAlpha==0)
		{
			SetFinished(true);
			pkEffect->SetFinished(true);
		}

		return	false;

	}
	NiImplementCreateClone(PgEffectProcessorFadeOut);

	void PgEffectProcessorFadeOut::CopyMembers(PgEffectProcessorFadeOut* pDest, NiCloningProcess& kCloning)
	{
		PgEffectProcessor::CopyMembers(pDest,kCloning);

		pDest->SetFadeOutTime(GetFadeOutTime());
	}

}