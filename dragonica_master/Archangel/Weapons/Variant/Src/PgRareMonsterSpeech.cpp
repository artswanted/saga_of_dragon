#include "stdafx.h"
#include "tabledatamanager.h"
#include "PgRareMonsterSpeech.h"

bool PgRareMonsterSpeech::GetSpeech(int const iTalkNo,int & iSpeechNo)
{
	CONT_DEF_RARE_MONSTER_SPEECH const *pkDef = NULL;
	g_kTblDataMgr.GetContDef(pkDef);
	if( !pkDef )
	{
		return false;
	}

	CONT_DEF_RARE_MONSTER_SPEECH::const_iterator iter = pkDef->find(iTalkNo);
	if(iter == pkDef->end() || (*iter).second.empty())
	{
		return false;
	}

	int kSpeechIdx = BM::Rand_Index(static_cast<int>((*iter).second.size()));

	iSpeechNo = (*iter).second.at(kSpeechIdx);

	return true;
}