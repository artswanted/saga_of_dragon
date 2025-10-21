#pragma once

#include "Lohengrin/DBTables.h"

class PgJumpingCharEvent : public TBL_DEF_JUMPINGCHAREVENT
{
public:
	typedef std::map<int, TBL_DEF_JUMPINGCHAREVENT_REWARD const*> CONT_REWARD;

public:
	PgJumpingCharEvent();
	~PgJumpingCharEvent();

	void Build(TBL_DEF_JUMPINGCHAREVENT const& kDef);
	CONT_REWARD::mapped_type GetReward(int const iClass)const;
	CONT_REWARD const& GetContReward()const;
	bool IsInDateTime(BM::PgPackedTime const& kCur)const;

private:
	CONT_REWARD kContReward;
};

class PgJumpingCharEventMgr
{
public:
	typedef std::map<int, PgJumpingCharEvent> CONT_BLD_EVENT;

public:
	PgJumpingCharEventMgr();
	~PgJumpingCharEventMgr();
	
	void Build();
	bool GetClass(int const iEventNo, int const iBaseClass, VEC_INT & rkOutClass)const;
	int GetClass(int const iEventNo, int const iBaseClass, int const iIdx)const;
	int GetLevel(int const iEventNo, int const iBaseClass, int const iIdx)const;
	bool GetEvent(int const iEventNo, int const iBaseClass, int const iRealClass, SCreateCharacterInfo & rkCreateCharacterInfo)const;

	bool ProcessNowEventNo();
	int GetNowEventNo(bool const bUpdate = true);
	int GetLevelLimit(int const iEventNo)const;
	int GetMaxRewardCount(int const iEventNo)const;

private:
	mutable ACE_RW_Thread_Mutex m_kMutex;
	CONT_BLD_EVENT kContEvent;
	int m_iNowEventNo;
	BM::PgPackedTime m_dwNextEventTime;
};

#define g_kJumpingCharEventMgr SINGLETON_STATIC(PgJumpingCharEventMgr)