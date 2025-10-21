#ifndef WEAPON_VARIANT_USERINFO_PGACHIEVEMENTS_H
#define WEAPON_VARIANT_USERINFO_PGACHIEVEMENTS_H

#include "idobject.h"
#include "Lohengrin/LockUtil.h"
#include "item.h"

int const ACHIEVEMENT_BIT_NUM = 2;	// 한개 업적 저장 비트 갯수 1: 달성 여부 1: 아이템화 여부 
int const MAX_ACHIEVEMENTS_NUM = 1000;
int const MAX_ACHIEVEMENTS_BYTES = ((MAX_ACHIEVEMENTS_NUM / 8) * ACHIEVEMENT_BIT_NUM);

typedef std::vector<BYTE> CONT_ACHIEVEMENTS;

typedef std::map<int,BM::PgPackedTime> CONT_ACHIEVEMENT_TIMELIMIT;
typedef std::set<int> CONT_ACHIEVEMENT_TIMEOUTED;
typedef std::map< EAchievementsCategory, int > CONT_AVCHIEVEMENT_POINT;

class PgAchievements
{
public:

	PgAchievements()
	{
		Init();
	}
	~PgAchievements(){}

	PgAchievements& operator=( PgAchievements const &rhs );

	void Init();
	bool const IsComplete(int const iIdx) const;
	bool Complete(int const iIdx);
	bool Reset(int const iIdx);
	bool const CheckHasItem(int const iIdx) const;
	bool ResetItem(int const iIdx,bool const IsPopItem);

	void WriteToPacket(BM::Stream & kPacket) const;
	void ReadFromPacket(BM::Stream & kPacket);

	void GetBuffer(CONT_ACHIEVEMENTS & kContAchievements)
	{
		kContAchievements.resize(MAX_ACHIEVEMENTS_BYTES);
		memcpy(&kContAchievements.at(0),m_byteAchievements,MAX_ACHIEVEMENTS_BYTES);
	}

	void LoadDBAchievement(CEL::DB_DATA_ARRAY::const_iterator & itor);
	void LoadDBAchievementTimeLimit(CEL::DB_DATA_ARRAY::const_iterator & itor, int const iAchievementsTimeLimitCount);
	void LoadDBAchievementPoint(CEL::DB_DATA_ARRAY::const_iterator & itor, int const iPointCount);

	void SetAchievementTimeLimit(int const iSaveIdx,BM::PgPackedTime const & kTimeLimit);
	bool ResetAchievementTimeLimit(int const iSaveIdx);
	void ProcessAchievementTimeOut(BM::GUID const & kOwnerGuid, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, CONT_ACHIEVEMENT_TIMEOUTED & kTimeOuted) const;
	bool GetAchievementTimeLimit(int const iSaveIdx,BM::PgPackedTime & kTimeLimit) const;
	void SetAchievementPoint(EAchievementsCategory const eType, int const iPoint);
	void AddAchievementPoint(EAchievementsCategory const eType, int const iAddPoint);
	int GetAchievementPoint(EAchievementsCategory const eType) const;

protected:
	void CalcTotalAchievementPoint();

	bool CalcIDToOffset(int const iIdx,bool const bCheckHasItem,int & iByteOffset,BYTE & bValue) const;
	BYTE m_byteAchievements[MAX_ACHIEVEMENTS_BYTES];
	CONT_ACHIEVEMENT_TIMELIMIT	m_kContTimeLimit;
	CONT_AVCHIEVEMENT_POINT m_kContPoint;
};

#endif // WEAPON_VARIANT_USERINFO_PGACHIEVEMENTS_H