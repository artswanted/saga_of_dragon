#ifndef CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTRANDOM_H
#define CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTRANDOM_H

#include "Variant/PgQuestInfo.h"

class PgQuestRandomElement
{
public:
	PgQuestRandomElement();
	explicit PgQuestRandomElement(int const iGroupNo);
	explicit PgQuestRandomElement(PgQuestRandomElement const& rhs);
	~PgQuestRandomElement();

	bool Add(int iStartID, int iEndID, int const iDisplayCount);
	void Shuffle(ContQuestID &rkOut, EQuestType eQuestType) const;

	size_t Size() const	{ return m_kQuestID.size(); }
	bool Empty() const	{ return 0 == Size(); }
	CLASS_DECLARATION_NO_SET(int, m_iGroupNo, GroupNo);
	CLASS_DECLARATION_NO_SET(size_t, m_iDisplayCount, DisplayCount);
	CLASS_DECLARATION_S_NO_SET(ContQuestID, QuestID);
private:
};
typedef std::list< PgQuestRandomElement > ContQuestRndElem;


//
class PgQuestRandom
{
public:
	explicit PgQuestRandom(int const iLevelMin, int const iLevelMax);
	explicit PgQuestRandom(PgQuestRandom const& rhs);
	~PgQuestRandom();

	bool Check(int const iLevel) const	{ return (m_iLevelMin <= iLevel) && (m_iLevelMax >= iLevel); }
	void ShuffleQuest(int const iLevel, ContQuestID& rkOut, const EQuestType eQuestType) const;
	bool Add(TBL_DEF_QUEST_RANDOM const& rkElement);

	bool Verify(ContQuestID& rkAllQuestID) const;
protected:
	BM::vstring Key() const;

private:
	CLASS_DECLARATION_NO_SET(int, m_iLevelMin, LevelMin);
	CLASS_DECLARATION_NO_SET(int, m_iLevelMax, LevelMax);
	ContQuestRndElem m_kQuestRnd;
};
typedef std::list< PgQuestRandom > ContQuestRandom;

#endif // CONTENTS_CONTENTSSERVER_CONTENTS_PGQUESTRANDOM_H