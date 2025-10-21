#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTMNG_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTMNG_H

#include "Variant/PgQuestInfo.h"
#include "Variant/PgBaseQuestMng.h"


class PgQuestMng : public PgBaseQuestMng
{
public:
	PgQuestMng();
	virtual ~PgQuestMng();

	bool GetQuestDef(int const iQuestID, TBL_DEF_QUEST_REWARD const* &pkOut);

	bool BuildRandom(CONT_DEF_QUEST_RANDOM const *pkDefRandom, CONT_DEF_QUEST_RANDOM_EXP const* pkDefRandomExp, CONT_DEF_QUEST_RANDOM_TACTICS_EXP const* pkDefTacticsRandomExp);
	void GetQuestRandom(int const iLevel, ContQuestID& rkOut) const;
	void GetQuestTacticsRandom(int const iLevel, ContQuestID& rkOut) const;
	void GetQuestWanted(int const iLevel, ContQuestID& rkOut) const;
protected:

	CLASS_DECLARATION_S_NO_SET(ContQuestID, QuestRandomAll);
	CLASS_DECLARATION_S_NO_SET(ContQuestID, QuestTacticsRandomAll);
	CLASS_DECLARATION_S_NO_SET(ContQuestID, QuestWantedAll);
private:
	ContQuestRandom m_kQuestRandom;
	ContQuestRandom m_kQuestTacticsRandom;
	ContQuestRandom m_kQuestWanted;
};
#define g_kQuestMng		SINGLETON_STATIC(PgQuestMng)

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTMNG_H