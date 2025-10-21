#ifndef WEAPON_VARIANT_QUEST_PGBASEQUESTMNG_H
#define WEAPON_VARIANT_QUEST_PGBASEQUESTMNG_H

typedef std::map< int, ContQuestID > ContQuestGroup;
class PgBaseQuestMng
{
public:
	PgBaseQuestMng();
	virtual ~PgBaseQuestMng();

	void Clear();
	bool Build(CONT_DEF_QUEST_REWARD const* pkDefCont, CONT_DEF_QUEST_RESET_SCHEDULE const* pkDefResetSchedule);

	ContQuestID const* GetGroup(int const iGroupNo)const;
	bool IsHaveResetSchedule(int const iQuestID) const;

	CLASS_DECLARATION_S_NO_SET(ContQuestID, DayQuestAll);				// 1일 퀘스트
	CLASS_DECLARATION_S_NO_SET(ContQuestID, WeekQuestAll);				// 주간 퀘스트
	CLASS_DECLARATION_S_NO_SET(ContQuestID, DeletedQuestAll);			// 삭제 예정 퀘스트
	CLASS_DECLARATION_S_NO_SET(ContQuestResetSchedule, ResetSchedule);	// 퀘스트 리셋 스케쥴
	CLASS_DECLARATION_S_NO_SET(ContQuestGroup, QuestGroup);				// 그룹 퀘스트
	CLASS_DECLARATION_S_NO_SET(ContQuestID, CoupleQuestAll);			// 커플 퀘스트 목록
	CLASS_DECLARATION_S_NO_SET(ContQuestID, BattlePassQuestAll);		// Battle Pass Quests
private:
};

#endif // WEAPON_VARIANT_QUEST_PGBASEQUESTMNG_H