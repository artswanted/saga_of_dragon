#include "stdafx.h"
#include "PgEffectProcessor.H"


namespace NiManagedToolInterface
{
	NiImplementRTTI(PgEffectProcessor, NiObject);

	void PgEffectProcessor::CopyMembers(PgEffectProcessor* pDest, NiCloningProcess& kCloning)
	{
		NiObject::CopyMembers(pDest,kCloning);
	}
}