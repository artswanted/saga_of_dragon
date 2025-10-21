#include "stdafx.h"
#include "PgDailyReward.h"
#include "PgEventView.h"

bool PgDailyReward::CanGetReward() const
{
    SYSTEMTIME kLocalTime;
	g_kEventView.GetLocalTime(&kLocalTime);
    return m_kRewardDate <= BM::DBTIMESTAMP_EX(kLocalTime);
}

void PgDailyReward::IncreaseRewardDate()
{
    SYSTEMTIME kLocalTime;
	g_kEventView.GetLocalTime(&kLocalTime);
    CGameTime::AddTime(kLocalTime, CGameTime::OneDay);
    m_kRewardDate = kLocalTime;
}

void PgDailyReward::ReadFromPacket(BM::Stream & rkPacket)
{
    rkPacket.Pop(m_kDayNo);
    BM::PgPackedTime kPackedTime;
    rkPacket.Pop(kPackedTime);
    m_kRewardDate = kPackedTime;
}

void PgDailyReward::WriteToPacket(BM::Stream & rkPacket) const
{
    rkPacket.Push(m_kDayNo);
    rkPacket.Push(BM::PgPackedTime(m_kRewardDate));
}

bool PgDailyReward::IsExpired() const
{
    SYSTEMTIME kLocalTime;
	g_kEventView.GetLocalTime(&kLocalTime);
    return CGameTime::GetElapsedTime(BM::DBTIMESTAMP_EX(kLocalTime), m_kRewardDate) > CGameTime::OneDay / CGameTime::SECOND;
}