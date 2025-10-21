#include "StdAfx.h"
#include "PgPremium.h"
#include "PgPlayer.h"
#include "PgQuestInfo.h"

int S_PST_ShareRentalSafeExpand::FromDB(BM::Stream & rkPacket, size_t const iSize)
{
	m_kContRental.clear();
	rkPacket.Pop(m_kContRental);
	return 0;
}

int S_PST_ShareRentalSafeExpand::ToDB(BM::Stream & rkPacket)const
{
	rkPacket.Push(m_kContRental);
	return 0;
}

int S_PST_ShareRentalSafeExpand::Setup(PgPlayer * pkPlayer)
{
	if(!pkPlayer)
	{
		return 1;
	}
	PgInventory * pkInven = pkPlayer->GetInven();
	if(!pkInven)
	{
		return 1;
	}

	int iCount = 0;
	
	//기존에 열렸던 금고를 우선적으로 오픈함
	SET_INT::iterator it = m_kContRental.begin();
	while(it != m_kContRental.end())
	{
		EInvType const kInvType = static_cast<EInvType>(*it);
		if(E_ENABLE_TIMEOUT == pkInven->CheckEnableUseRentalSafe(kInvType))
		{
			++iCount;
			++it;
		}
		else
		{
			it = m_kContRental.erase(it);
		}
	}

	for(int i=IT_SHARE_RENTAL_SAFE1; (i<=IT_SHARE_RENTAL_SAFE4) && (iCount<byExpand); ++i)
	{
		EInvType const kInvType = static_cast<EInvType>(i);
		if(false==IsRental(kInvType) && E_ENABLE_TIMEOUT==pkInven->CheckEnableUseRentalSafe(kInvType))
		{
			m_kContRental.insert(kInvType);
			++iCount;
		}
	}
	return 0;
}

bool S_PST_ShareRentalSafeExpand::IsRental(int const iInvType)const
{
	SET_INT::const_iterator c_it = m_kContRental.find(iInvType);
	return c_it != m_kContRental.end();
}


bool S_PST_QuestOnceMore::IsCheck(int const iQuestType)const
{
	if(QT_RandomTactics==iQuestType || QT_Random==iQuestType)
	{
		CONT_QUEST_TYPE::const_iterator c_it = m_kContQuest.find(iQuestType);
		if(c_it != m_kContQuest.end())
		{
			BM::DBTIMESTAMP_EX kLocalTime;
			g_kEventView.GetLocalTime(kLocalTime);

			return ((*c_it).second < static_cast<BM::PgPackedTime>(kLocalTime));
		}
		return true;
	}
	return false;
}

void S_PST_QuestOnceMore::AddQuestType(int const iQuestType)
{
	BM::DBTIMESTAMP_EX kLocalTime;
	g_kEventView.GetLocalTime(kLocalTime);

	BM::PgPackedTime kNextTime(kLocalTime);
	CGameTime::AddTime(kNextTime, CGameTime::OneDay);
	kNextTime.Hour(0), kNextTime.Min(0), kNextTime.Sec(0);

	AddQuestType(iQuestType, kNextTime);
}

void S_PST_QuestOnceMore::AddQuestType(int const iQuestType, BM::PgPackedTime const& rkNextTime)
{
	auto kRet = m_kContQuest.insert(std::make_pair(iQuestType, rkNextTime));
	if(false==kRet.second)
	{
		kRet.first->second = rkNextTime;
	}
}

bool S_PST_QuestOnceMore::GetNextTime(int const iQuestType, BM::PgPackedTime & rkNextTime)const
{
	CONT_QUEST_TYPE::const_iterator c_it = m_kContQuest.find(iQuestType);
	if(c_it != m_kContQuest.end() && false==(*c_it).second.IsNull())
	{
		rkNextTime = (*c_it).second;
		return true;
	}
	return false;
}

int S_PST_QuestOnceMore::FromDB(BM::Stream & rkPacket, size_t const iSize)
{
	if(0 < iSize)
	{
		m_kContQuest.clear();
		rkPacket.Pop(m_kContQuest);
	}
	return 0;
}

int S_PST_QuestOnceMore::ToDB(BM::Stream & rkPacket)const
{
	if(false==m_kContQuest.empty())
	{
		rkPacket.Push(m_kContQuest);
	}
	return 0;
}
