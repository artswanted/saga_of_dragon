#include "stdafx.h"
#include "Effect/ChannelEffect.h"
#include <PgGround.h>
#include <drem/sys/log.h>

void ApplyChannelEffect(PgPlayer* pkPlayer, PgGround* pkGround)
{
	if (!pkPlayer || !pkGround)
		return;

	const CONT_DEF_CHANNEL_EFFECT* pkContChannelEffect = NULL;
	g_kTblDataMgr.GetContDef( pkContChannelEffect );
	if (!pkContChannelEffect)
	{
		drem::logerr("Channel Effect table is null?!");
		return;
	}

	const int iRealmNo = g_kProcessCfg.RealmNo();
	const int iChannelNo = g_kProcessCfg.ChannelNo();
	const int iLevel = pkPlayer->GetAbil( AT_LEVEL );
	CONT_DEF_CHANNEL_EFFECT::const_iterator c_iter = pkContChannelEffect->begin();
    for (; c_iter != pkContChannelEffect->end(); ++c_iter )
    {
        const short sMinLv = c_iter->sMinLv;
        const short sMaxLv = c_iter->sMaxLv;
		const bool bSetEffect =
			iRealmNo == c_iter->sRealm &&
			iChannelNo == c_iter->sChannel &&
			(sMinLv <= iLevel && sMaxLv >= iLevel);

		if (bSetEffect)
		{
			SActArg kArg;
			PgGroundUtil::SetActArgGround(kArg, pkGround);
			pkPlayer->AddEffect( c_iter->iEffectNo, 0, &kArg, pkPlayer );
		}
		else
		{
			pkPlayer->DeleteEffect( c_iter->iEffectNo );
		}
	}
}