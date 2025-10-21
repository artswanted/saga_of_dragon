#ifndef WEAPON_VARIANT_QUEST_PGQUESTRESETSCHEDULE_H
#define WEAPON_VARIANT_QUEST_PGQUESTRESETSCHEDULE_H

class PgQuestResetSchedule
{
public:
	explicit PgQuestResetSchedule(SQuestResetSchedule const& rkDBResetSchedule);
	explicit PgQuestResetSchedule(PgQuestResetSchedule const& rhs);	
	~PgQuestResetSchedule();

	bool operator <(PgQuestResetSchedule const& rhs)const;

	int QuestID() const						{ return m_kDBResetSchedule.iQuestID; }
	BM::PgPackedTime ResetBaseDate() const	{ return m_kDBResetSchedule.kResetBaseDate; }
	int ResetLoopDay() const				{ return m_kDBResetSchedule.iResetLoopDay; }
	__int64 DateToDay() const				{ return m_iDayTime; }
	
private:
	SQuestResetSchedule const& m_kDBResetSchedule;
	__int64 m_iDayTime;
};
typedef std::list< PgQuestResetSchedule > ContQuestResetSchedule;

namespace PgQuestResetScheduleUtil
{
	bool CheckResetSchedule(__int64 const iDffUserToNow, __int64 const iUserDayTime, __int64 const iNowDayTime, PgQuestResetSchedule const& rkResetSchedule);
	bool CheckResetSchedule(__int64 const iUserDayTime, __int64 const iNowDayTime, PgQuestResetSchedule const& rkResetSchedule);
	void CheckResetSchedule(BM::PgPackedTime const& rkUserDate, BM::PgPackedTime const& rkNowDate, ContQuestResetSchedule const& rkScheduleList, ContQuestID& rkOut);
};

#endif // WEAPON_VARIANT_QUEST_PGQUESTRESETSCHEDULE_H