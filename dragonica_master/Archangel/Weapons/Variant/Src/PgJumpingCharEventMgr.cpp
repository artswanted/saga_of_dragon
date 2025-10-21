#include "StdAfx.h"
#include "PgJumpingCharEventMgr.h"
#include "TableDataManager.h"
#include "constant.h"
#include "PgControlDefMgr.h"

inline __int64 GetCommonClassLimit(int const iBaseClass)
{
	switch(iBaseClass)
	{
	case UCLASS_FIGHTER:	return UCLIMIT_MARKET_FIGHTER;
	case UCLASS_MAGICIAN:	return UCLIMIT_MARKET_MAGICIAN;
	case UCLASS_ARCHER:		return UCLIMIT_MARKET_ARCHER;
	case UCLASS_THIEF:		return UCLIMIT_MARKET_THIEF;
	case UCLASS_SHAMAN:		return UCLIMIT_MARKET_SHAMAN;
	case UCLASS_DOUBLE_FIGHTER:		return UCLIMIT_MARKET_DOUBLE_FIGHTER;
	}
	return 0i64;
}

PgJumpingCharEvent::PgJumpingCharEvent()
{
}

PgJumpingCharEvent::~PgJumpingCharEvent()
{
}

void PgJumpingCharEvent::Build(TBL_DEF_JUMPINGCHAREVENT const& kDef)
{
	iEventNo = kDef.iEventNo;
	StartTime = kDef.StartTime;		//kStartTime로 수정
	EndTime = kDef.EndTime;			//kEndTime로 수정
	iLevelLimit = kDef.iLevelLimit;
	iMaxRewardCount = kDef.iMaxRewardCount;
	iRewardGroupNo = kDef.iRewardGroupNo;

	CONT_DEF_JUMPINGCHAREVENT_REWARD const *pkContReward = NULL;
	g_kTblDataMgr.GetContDef(pkContReward);
	if(pkContReward)
	{
		CONT_DEF_JUMPINGCHAREVENT_REWARD::const_iterator reward_group_it = pkContReward->find(iRewardGroupNo);
		if(reward_group_it != pkContReward->end())
		{
			CONT_DEF_JUMPINGCHAREVENT_REWARD::mapped_type::const_iterator reward_it = (*reward_group_it).second.begin();
			while(reward_it != (*reward_group_it).second.end())
			{
				kContReward[(*reward_it).iClass] = &(*reward_it);
				//CONT_DEF_JUMPINGCHAREVENT_ITEM const *pkContItem = NULL;
				//g_kTblDataMgr.GetContDef(pkContItem);
				++reward_it;
			}
		}
	}
}

PgJumpingCharEvent::CONT_REWARD::mapped_type PgJumpingCharEvent::GetReward(int const iClass)const
{
	CONT_REWARD::const_iterator c_it = kContReward.find(iClass);
	if(c_it != kContReward.end())
	{
		return (*c_it).second;
	}
	return NULL;
}

PgJumpingCharEvent::CONT_REWARD const& PgJumpingCharEvent::GetContReward()const
{
	return kContReward;
}

bool PgJumpingCharEvent::IsInDateTime(BM::PgPackedTime const& kCur)const
{
	if( BM::PgPackedTime(StartTime) < kCur
	&&	kCur < BM::PgPackedTime(EndTime) )
	{
		return true;
	}
	return false;
}


PgJumpingCharEventMgr::PgJumpingCharEventMgr()
	: m_iNowEventNo(0)
{
}

PgJumpingCharEventMgr::~PgJumpingCharEventMgr()
{
}

void PgJumpingCharEventMgr::Build()
{
	BM::CAutoMutex kLock(m_kMutex);
	kContEvent.clear();

	CONT_DEF_JUMPINGCHAREVENT const *pkContEvent = NULL;
	g_kTblDataMgr.GetContDef(pkContEvent);

	if(!pkContEvent)
	{
		return;
	}

	CONT_DEF_JUMPINGCHAREVENT::const_iterator event_it = pkContEvent->begin();
	while(event_it != pkContEvent->end())
	{
		kContEvent[(*event_it).second.iEventNo].Build((*event_it).second);
		++event_it;
	}
	ProcessNowEventNo();
}

bool PgJumpingCharEventMgr::GetClass(int const iEventNo, int const iBaseClass, VEC_INT & rkOutClass)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it == kContEvent.end())
	{
		return false;
	}

	PgJumpingCharEvent::CONT_REWARD::const_iterator c_it = (*event_it).second.GetContReward().begin();
	while(c_it != (*event_it).second.GetContReward().end())
	{
		if( true == IS_CLASS_LIMIT(GetCommonClassLimit(iBaseClass), (*c_it).second->iClass) )
		{
			rkOutClass.push_back((*c_it).second->iClass);
		}
		++c_it;
	}
	return true;
}

int PgJumpingCharEventMgr::GetClass(int const iEventNo, int const iBaseClass, int const iIdx)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it == kContEvent.end())
	{
		return false;
	}

	int iCount = 1;
	PgJumpingCharEvent::CONT_REWARD::const_iterator c_it = (*event_it).second.GetContReward().begin();
	while(c_it != (*event_it).second.GetContReward().end())
	{
		if( true == IS_CLASS_LIMIT(GetCommonClassLimit(iBaseClass), (*c_it).second->iClass) )
		{
			if(iIdx==iCount)
			{
				return (*c_it).second->iClass;
			}
			++iCount;
		}
		++c_it;
	}
	return 0;
}

int PgJumpingCharEventMgr::GetLevel(int const iEventNo, int const iBaseClass, int const iIdx)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it == kContEvent.end())
	{
		return false;
	}

	int iCount = 1;
	PgJumpingCharEvent::CONT_REWARD::const_iterator c_it = (*event_it).second.GetContReward().begin();
	while(c_it != (*event_it).second.GetContReward().end())
	{
		if( true == IS_CLASS_LIMIT(GetCommonClassLimit(iBaseClass), (*c_it).second->iClass) )
		{
			if(iIdx==iCount)
			{
				return (*c_it).second->iLevel;
			}
			++iCount;
		}
		++c_it;
	}
	return 0;
}

bool PgJumpingCharEventMgr::GetEvent(int const iEventNo, int const iBaseClass, int const iRealClass, SCreateCharacterInfo & rkCreateCharacterInfo)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it == kContEvent.end())
	{
		return false;
	}

	if( false == IS_CLASS_LIMIT(GetCommonClassLimit(iBaseClass), iRealClass) )
	{
		return false;
	}

	PgJumpingCharEvent::CONT_REWARD::mapped_type pkReward = (*event_it).second.GetReward(iRealClass);
	if(!pkReward)
	{
		return false;
	}

	GET_DEF(PgClassDefMgr, kClassDefMgr);
	rkCreateCharacterInfo.iClass = pkReward->iClass;
	rkCreateCharacterInfo.iLv = pkReward->iLevel;
	rkCreateCharacterInfo.iExp = kClassDefMgr.GetAbil64( SClassKey(pkReward->iClass,pkReward->iLevel), AT_EXPERIENCE );
	rkCreateCharacterInfo.iHP = kClassDefMgr.GetAbil( SClassKey(pkReward->iClass,pkReward->iLevel), AT_MAX_HP );
	rkCreateCharacterInfo.iMP = kClassDefMgr.GetAbil( SClassKey(pkReward->iClass,pkReward->iLevel), AT_MAX_MP );
	rkCreateCharacterInfo.pkIngQuest = pkReward->f_IngQuest;
	rkCreateCharacterInfo.pkEndQuest = pkReward->f_EndQuest;
	rkCreateCharacterInfo.pkEndQuest2 = pkReward->f_EndQuest2;
	rkCreateCharacterInfo.pkEndQuestExt = pkReward->f_EndQuestExt;
	rkCreateCharacterInfo.pkEndQuestExt2 = pkReward->f_EndQuestExt2;
	rkCreateCharacterInfo.iMapNo = pkReward->iMapNo;
	rkCreateCharacterInfo.iJumpingCharEvent_ItemGroupNo = pkReward->iItemGroupNo;
	rkCreateCharacterInfo.iJumpingCharEvent_MaxRewardCount = (*event_it).second.iMaxRewardCount;
	kClassDefMgr.AccSkillPoint(SClassKey(rkCreateCharacterInfo.iClass,rkCreateCharacterInfo.iLv), rkCreateCharacterInfo.iExtraSkillPoint);
	return true;
}

bool PgJumpingCharEventMgr::ProcessNowEventNo()
{
	BM::DBTIMESTAMP_EX kSysTime;
	g_kEventView.GetLocalTime(kSysTime);
	BM::PgPackedTime const kCurTime(kSysTime);

	CONT_BLD_EVENT::const_iterator event_it = kContEvent.begin();
	while(event_it != kContEvent.end())
	{
		if((*event_it).second.IsInDateTime(kCurTime))
		{
			m_iNowEventNo = (*event_it).second.iEventNo;
			m_dwNextEventTime = (*event_it).second.EndTime;
			return true;
		}
		++event_it;
	}

	m_iNowEventNo = 0;
	m_dwNextEventTime.Clear();
	return false;
}

int PgJumpingCharEventMgr::GetNowEventNo(bool const bUpdate)
{
	if(bUpdate)
	{
		BM::DBTIMESTAMP_EX kSysTime;
		g_kEventView.GetLocalTime(kSysTime);
		BM::PgPackedTime const kCurTime(kSysTime);

		if(m_dwNextEventTime < kCurTime)
		{
			ProcessNowEventNo();
		}
	}

	return m_iNowEventNo;
}

int PgJumpingCharEventMgr::GetLevelLimit(int const iEventNo)const
{
	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it != kContEvent.end())
	{
		return (*event_it).second.iLevelLimit;
	}
	return 0;
}

int PgJumpingCharEventMgr::GetMaxRewardCount(int const iEventNo)const
{
	CONT_BLD_EVENT::const_iterator event_it = kContEvent.find(iEventNo);
	if(event_it != kContEvent.end())
	{
		return (*event_it).second.iMaxRewardCount;
	}
	return 0;
}